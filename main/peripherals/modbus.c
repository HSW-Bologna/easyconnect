#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "hardwareprofile.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "config/app_config.h"
#include "driver/gpio.h"
#include "config/app_config.h"
#include <sys/types.h>
#include "lightmodbus/lightmodbus.h"
#include "lightmodbus/master_func.h"
#include "gel/timer/timecheck.h"
#include "utils/utils.h"
#include "modbus.h"


#define MB_PORTNUM 1
// Timeout threshold for UART = number of symbols (~10 tics) with unchanged state on receive pin
#define ECHO_READ_TOUT (3)     // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks

#define MODBUS_MESSAGE_QUEUE_SIZE     512
#define MODBUS_QUERY_INTERVAL         500
#define MODBUS_TIMEOUT                20
#define MODBUS_MAX_PACKET_SIZE        256
#define MODBUS_BROADCAST_ADDRESS      0
#define MODBUS_COMMUNICATION_ATTEMPTS 4

#define MODBUS_CONFIG_ADDRESS_FUNCTION 64
#define MODBUS_RANDOM_SERIAL_NUMBER    66

#define MODBUS_ADDRESS_HOLDING_REGISTER_ADDRESS 0
#define MODBUS_CLASS_HOLDING_REGISTER_ADDRESS   1
#define MODBUS_SN_HOLDING_REGISTER_ADDRESS      2

#define MODBUS_AUTO_COMMISSIONING_DONE_BIT 0x01

typedef enum {
    TASK_MESSAGE_CODE_CONFIGURE_DEVICE_MANUALLY,
    TASK_MESSAGE_CODE_READ_DEVICE_INFO,
    TASK_MESSAGE_CODE_READ_DEVICE_INPUTS,
    TASK_MESSAGE_CODE_SET_DEVICE_OUTPUT,
    TASK_MESSAGE_CODE_SET_DEVICE_CLASS,
    TASK_MESSAGE_CODE_BEGIN_AUTOMATIC_COMMISSIONING,
} task_message_code_t;


struct task_message {
    task_message_code_t code;
    uint8_t             address;
    union {
        int value;
        uint16_t class;
    };
};


typedef struct {
    uint16_t device_map[255];
} master_context_t;


static LIGHTMODBUS_RET_ERROR build_custom_request(ModbusMaster *status, uint8_t function, uint8_t *data, size_t len);
static LIGHTMODBUS_RET_ERROR random_serial_number_response(ModbusMaster *status, uint8_t address, uint8_t function,
                                                           const uint8_t *requestPDU, uint8_t requestLength,
                                                           const uint8_t *responsePDU, uint8_t responseLength);

static void modbus_task(void *args);
static int  configure_device_manually(ModbusMaster *master, uint8_t address);
static int  write_holding_register(ModbusMaster *master, uint8_t address, uint16_t index, uint16_t data);
static int  write_holding_registers(ModbusMaster *master, uint8_t address, uint16_t starting_address, uint16_t *data,
                                    size_t num);
static int  write_coil(ModbusMaster *master, uint8_t address, uint16_t index, int value);


static const char *       TAG       = "Modbus";
static QueueHandle_t      messageq  = NULL;
static QueueHandle_t      responseq = NULL;
static EventGroupHandle_t events    = NULL;


static ModbusMasterFunctionHandler custom_functions[] = {
#if defined(LIGHTMODBUS_F01M) || defined(LIGHTMODBUS_MASTER_FULL)
    {1, modbusParseResponse01020304},
#endif

#if defined(LIGHTMODBUS_F02M) || defined(LIGHTMODBUS_MASTER_FULL)
    {2, modbusParseResponse01020304},
#endif

#if defined(LIGHTMODBUS_F03M) || defined(LIGHTMODBUS_MASTER_FULL)
    {3, modbusParseResponse01020304},
#endif

#if defined(LIGHTMODBUS_F04M) || defined(LIGHTMODBUS_MASTER_FULL)
    {4, modbusParseResponse01020304},
#endif

#if defined(LIGHTMODBUS_F05M) || defined(LIGHTMODBUS_MASTER_FULL)
    {5, modbusParseResponse0506},
#endif

#if defined(LIGHTMODBUS_F06M) || defined(LIGHTMODBUS_MASTER_FULL)
    {6, modbusParseResponse0506},
#endif

#if defined(LIGHTMODBUS_F15M) || defined(LIGHTMODBUS_MASTER_FULL)
    {15, modbusParseResponse1516},
#endif

#if defined(LIGHTMODBUS_F16M) || defined(LIGHTMODBUS_MASTER_FULL)
    {16, modbusParseResponse1516},
#endif

#if defined(LIGHTMODBUS_F22M) || defined(LIGHTMODBUS_MASTER_FULL)
    {22, modbusParseResponse22},
#endif

    {MODBUS_RANDOM_SERIAL_NUMBER, random_serial_number_response},
    // Guard - prevents 0 size array
    {0, NULL},
};


void modbus_init(void) {
    uart_config_t uart_config = {
        .baud_rate           = 115200,
        .data_bits           = UART_DATA_8_BITS,
        .parity              = UART_PARITY_DISABLE,
        .stop_bits           = UART_STOP_BITS_1,
        .flow_ctrl           = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(MB_PORTNUM, &uart_config));

    uart_set_pin(MB_PORTNUM, MB_UART_TXD, MB_UART_RXD, MB_DERE, -1);
    ESP_ERROR_CHECK(uart_driver_install(MB_PORTNUM, 512, 512, 10, NULL, 0));
    ESP_ERROR_CHECK(uart_set_mode(MB_PORTNUM, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(uart_set_rx_timeout(MB_PORTNUM, ECHO_READ_TOUT));

    static StaticQueue_t static_queue1;
    static uint8_t       queue_buffer1[MODBUS_MESSAGE_QUEUE_SIZE * sizeof(struct task_message)] = {0};
    messageq =
        xQueueCreateStatic(MODBUS_MESSAGE_QUEUE_SIZE, sizeof(struct task_message), queue_buffer1, &static_queue1);

    static StaticQueue_t static_queue2;
    static uint8_t       queue_buffer2[MODBUS_MESSAGE_QUEUE_SIZE * sizeof(modbus_response_t)] = {0};
    responseq = xQueueCreateStatic(MODBUS_MESSAGE_QUEUE_SIZE, sizeof(modbus_response_t), queue_buffer2, &static_queue2);

    events = xEventGroupCreate();
    xEventGroupSetBits(events, MODBUS_AUTO_COMMISSIONING_DONE_BIT);

    xTaskCreate(modbus_task, TAG, BASE_TASK_SIZE * 3, NULL, 5, NULL);
}


int modbus_get_response(modbus_response_t *response) {
    return xQueueReceive(responseq, response, 0) == pdTRUE;
}


int modbus_automatic_commissioning_done(unsigned long millis) {
    return xEventGroupWaitBits(events, MODBUS_AUTO_COMMISSIONING_DONE_BIT, pdFALSE, pdTRUE, pdMS_TO_TICKS(millis)) ==
           pdTRUE;
}


void modbus_configure_device_address(uint8_t address) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_CONFIGURE_DEVICE_MANUALLY, .address = address};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_automatic_commissioning(void) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_BEGIN_AUTOMATIC_COMMISSIONING};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_set_device_class(uint8_t address, uint8_t class) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_SET_DEVICE_CLASS, .address = address, .class = class};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_set_device_output(uint8_t address, int value) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_SET_DEVICE_OUTPUT, .address = address, .value = value};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_read_device_info(uint8_t address) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_READ_DEVICE_INFO, .address = address};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_read_device_inputs(uint8_t address) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_READ_DEVICE_INPUTS, .address = address};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


static ModbusError dataCallback(const ModbusMaster *master, const ModbusDataCallbackArgs *args) {
    modbus_response_t *response = modbusMasterGetUserPointer(master);
    printf("Received data from %d, reg: %d, value: %d\n", args->address, args->index, args->value);

    if (response != NULL) {
        switch (response->code) {
            case MODBUS_RESPONSE_CODE_INFO: {
                switch (args->index) {
                    case MODBUS_SN_HOLDING_REGISTER_ADDRESS:
                        response->serial_number = args->value;
                        break;
                    case MODBUS_CLASS_HOLDING_REGISTER_ADDRESS:
                        response->class = args->value;
                        break;

                    default:
                        break;
                }
                break;
            }

            default:
                break;
        }
    }

    return MODBUS_OK;
}


static ModbusError masterExceptionCallback(const ModbusMaster *master, uint8_t address, uint8_t function,
                                           ModbusExceptionCode code) {
    printf("Received exception (function %d) from slave %d code %d\n", function, address, code);
    return MODBUS_OK;
}


static void modbus_task(void *args) {
    (void)args;
    ModbusMaster    master;
    ModbusErrorInfo err = modbusMasterInit(&master,
                                           dataCallback,                // Callback for handling incoming data
                                           masterExceptionCallback,     // Exception callback (optional)
                                           modbusDefaultAllocator,      // Memory allocator used to allocate request
                                           custom_functions,            // Set of supported functions
                                           modbusMasterDefaultFunctionCount + 1     // Number of supported functions
    );

    // Check for errors
    assert(modbusIsOk(err) && "modbusMasterInit() failed");
    struct task_message message                        = {0};
    uint8_t             buffer[MODBUS_MAX_PACKET_SIZE] = {0};
    modbus_response_t   error_resp                     = {.code = MODBUS_RESPONSE_ERROR};

    ESP_LOGI(TAG, "Task starting");
    for (;;) {
        size_t counter = 0;

        if (xQueueReceive(messageq, &message, pdMS_TO_TICKS(100))) {
            modbus_response_t response = {.address = message.address};
            error_resp.address         = message.address;
            modbusMasterSetUserPointer(&master, NULL);

            switch (message.code) {
                case TASK_MESSAGE_CODE_CONFIGURE_DEVICE_MANUALLY: {
                    int res = 0;
                    do {
                        res = configure_device_manually(&master, message.address);
                        if (res) {
                            vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
                        }
                    } while (res && counter++ < MODBUS_COMMUNICATION_ATTEMPTS);

                    response.error = res;
                    response.code  = MODBUS_RESPONSE_DEVICE_MANUAL_CONFIGURATION;
                    xQueueSend(responseq, &response, portMAX_DELAY);
                    break;
                }

                case TASK_MESSAGE_CODE_READ_DEVICE_INFO: {
                    response.code = MODBUS_RESPONSE_CODE_INFO;
                    modbusMasterSetUserPointer(&master, &response);
                    ESP_LOGI(TAG, "Reading info from %i", message.address);
                    int res = 0;

                    do {
                        res = 0;
                        err =
                            modbusBuildRequest03RTU(&master, message.address, MODBUS_CLASS_HOLDING_REGISTER_ADDRESS, 2);
                        assert(modbusIsOk(err));
                        uart_write_bytes(MB_PORTNUM, modbusMasterGetRequest(&master),
                                         modbusMasterGetRequestLength(&master));

                        int len = uart_read_bytes(MB_PORTNUM, buffer, sizeof(buffer), pdMS_TO_TICKS(MODBUS_TIMEOUT));
                        err     = modbusParseResponseRTU(&master, modbusMasterGetRequest(&master),
                                                     modbusMasterGetRequestLength(&master), buffer, len);

                        if (!modbusIsOk(err)) {
                            res = 1;
                            vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
                        }
                    } while (res && counter++ < MODBUS_COMMUNICATION_ATTEMPTS);

                    if (res) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    } else {
                        xQueueSend(responseq, &response, portMAX_DELAY);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_READ_DEVICE_INPUTS: {
                    ESP_LOGI(TAG, "Reading inputs from %i", message.address);
                    err = modbusBuildRequest02RTU(&master, message.address, 0, 2);
                    assert(modbusIsOk(err));
                    uart_write_bytes(MB_PORTNUM, modbusMasterGetRequest(&master),
                                     modbusMasterGetRequestLength(&master));

                    int len = uart_read_bytes(MB_PORTNUM, buffer, sizeof(buffer), pdMS_TO_TICKS(MODBUS_TIMEOUT));
                    err     = modbusParseResponseRTU(&master, modbusMasterGetRequest(&master),
                                                 modbusMasterGetRequestLength(&master), buffer, len);

                    if (!modbusIsOk(err)) {
                        ESP_LOG_BUFFER_HEX(TAG, buffer, len);
                        ESP_LOGW(TAG, "Cold not query device %i: %i %i", message.address, err.source, err.error);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_SET_DEVICE_OUTPUT: {
                    if (write_coil(&master, message.address, 0, message.value)) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_BEGIN_AUTOMATIC_COMMISSIONING: {
                    uint16_t period = 10;
                    uint8_t  data[] = {(period >> 8) & 0xFF, period & 0xFF};

                    xEventGroupClearBits(events, MODBUS_AUTO_COMMISSIONING_DONE_BIT);

                    err = modbusBeginRequestRTU(&master);
                    assert(modbusIsOk(err));
                    err = build_custom_request(&master, MODBUS_RANDOM_SERIAL_NUMBER, data, sizeof(data));
                    assert(modbusIsOk(err));
                    err = modbusEndRequestRTU(&master, MODBUS_BROADCAST_ADDRESS);
                    assert(modbusIsOk(err));
                    /* Broadcast message, we expect no answer */
                    uart_write_bytes(MB_PORTNUM, modbusMasterGetRequest(&master),
                                     modbusMasterGetRequestLength(&master));
                    vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));

                    unsigned long timestamp = get_millis();
                    while (!is_expired(timestamp, get_millis(), period * 1000UL)) {
                        // While waiting here messages are thrown away
                        struct task_message zero    = {0};
                        size_t              counter = 0;
                        while (xQueueReceive(messageq, &zero, 0) == pdTRUE && counter++ < MODBUS_MESSAGE_QUEUE_SIZE)
                            ;

                        int len = uart_read_bytes(MB_PORTNUM, buffer, sizeof(buffer), pdMS_TO_TICKS(MODBUS_TIMEOUT));
                        if (len > 0) {
                            err = modbusParseResponseRTU(&master, modbusMasterGetRequest(&master),
                                                         modbusMasterGetRequestLength(&master), buffer, len);

                            if (modbusIsOk(err)) {
                                ESP_LOGI(TAG, "correct");
                            } else {
                                ESP_LOG_BUFFER_HEX(TAG, buffer, len);
                                ESP_LOGI(TAG, "wrong %i %i", err.source, err.error);
                            }
                            uart_flush(MB_PORTNUM);
                        }
                    }

                    xEventGroupSetBits(events, MODBUS_AUTO_COMMISSIONING_DONE_BIT);
                    break;
                }

                case TASK_MESSAGE_CODE_SET_DEVICE_CLASS: {
                    if (write_holding_register(&master, message.address, MODBUS_CLASS_HOLDING_REGISTER_ADDRESS,
                                               message.class)) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    } else {
                        response.code  = MODBUS_RESPONSE_CODE_CLASS;
                        response.class = message.class;
                        xQueueSend(responseq, &response, portMAX_DELAY);
                    }
                    break;
                }
            }
            vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
        }
    }

    vTaskDelete(NULL);
}


static LIGHTMODBUS_RET_ERROR build_custom_request(ModbusMaster *status, uint8_t function, uint8_t *data, size_t len) {
    if (modbusMasterAllocateRequest(status, len + 1)) {
        return MODBUS_GENERAL_ERROR(ALLOC);
    }

    status->request.pdu[0] = function;
    for (size_t i = 0; i < len; i++) {
        status->request.pdu[1 + i] = data[i];
    }

    return MODBUS_NO_ERROR();
}


static LIGHTMODBUS_RET_ERROR random_serial_number_response(ModbusMaster *master, uint8_t address, uint8_t function,
                                                           const uint8_t *requestPDU, uint8_t requestLength,
                                                           const uint8_t *responsePDU, uint8_t responseLength) {
    // Check lengths
    if (requestLength != 3) {
        return MODBUS_REQUEST_ERROR(LENGTH);
    }
    if (responseLength != 3) {
        return MODBUS_RESPONSE_ERROR(LENGTH);
        ;
    }

    master_context_t *ctx = modbusMasterGetUserPointer(master);

    uint16_t serial_number   = (responsePDU[1] << 8) | responsePDU[2];
    ctx->device_map[address] = serial_number;

    ESP_LOGI(TAG, "Received serial number %i from %i", serial_number, address);

    return MODBUS_NO_ERROR();
}


static int configure_device_manually(ModbusMaster *master, uint8_t address) {
    uint8_t buffer[MODBUS_MAX_PACKET_SIZE] = {0};
    uint8_t data[]                         = {address};

    ModbusErrorInfo err = modbusBeginRequestRTU(master);
    assert(modbusIsOk(err));
    err = build_custom_request(master, MODBUS_CONFIG_ADDRESS_FUNCTION, data, sizeof(data));
    assert(modbusIsOk(err));
    err = modbusEndRequestRTU(master, MODBUS_BROADCAST_ADDRESS);
    assert(modbusIsOk(err));
    /* Broadcast message, we expect no answer */
    uart_write_bytes(MB_PORTNUM, modbusMasterGetRequest(master), modbusMasterGetRequestLength(master));
    vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));

    err = modbusBuildRequest03RTU(master, address, MODBUS_SN_HOLDING_REGISTER_ADDRESS, 1);
    assert(modbusIsOk(err));
    uart_write_bytes(MB_PORTNUM, modbusMasterGetRequest(master), modbusMasterGetRequestLength(master));

    int len = uart_read_bytes(MB_PORTNUM, buffer, sizeof(buffer), pdMS_TO_TICKS(MODBUS_TIMEOUT));
    err = modbusParseResponseRTU(master, modbusMasterGetRequest(master), modbusMasterGetRequestLength(master), buffer,
                                 len);
    if (!modbusIsOk(err)) {
        ESP_LOG_BUFFER_HEX(TAG, buffer, len);
        ESP_LOGW(TAG, "Error %i %i", err.source, err.error);
        return 1;
    } else {
        ESP_LOGI(TAG, "Configured device %i", address);
        return 0;
    }
}


static int write_holding_registers(ModbusMaster *master, uint8_t address, uint16_t starting_address, uint16_t *data,
                                   size_t num) {
    uint8_t buffer[MODBUS_MAX_PACKET_SIZE] = {0};
    int     res                            = 0;
    size_t  counter                        = 0;

    do {
        res                 = 0;
        ModbusErrorInfo err = modbusBuildRequest16RTU(master, address, starting_address, num, data);
        assert(modbusIsOk(err));
        uart_write_bytes(MB_PORTNUM, modbusMasterGetRequest(master), modbusMasterGetRequestLength(master));

        int len = uart_read_bytes(MB_PORTNUM, buffer, sizeof(buffer), pdMS_TO_TICKS(MODBUS_TIMEOUT));
        err     = modbusParseResponseRTU(master, modbusMasterGetRequest(master), modbusMasterGetRequestLength(master),
                                     buffer, len);

        if (!modbusIsOk(err)) {
            ESP_LOGW(TAG, "Write holding registers for %i error: %i %i", address, err.source, err.error);
            res = 1;
            vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
        }
    } while (res && counter++ < MODBUS_COMMUNICATION_ATTEMPTS);

    return res;
}


static int write_holding_register(ModbusMaster *master, uint8_t address, uint16_t index, uint16_t data) {
    return write_holding_registers(master, address, index, &data, 1);
}


static int write_coil(ModbusMaster *master, uint8_t address, uint16_t index, int value) {
    uint8_t buffer[MODBUS_MAX_PACKET_SIZE] = {0};
    int     res                            = 0;
    size_t  counter                        = 0;

    do {
        ModbusErrorInfo err = modbusBuildRequest05RTU(master, address, index, value);
        assert(modbusIsOk(err));
        uart_write_bytes(MB_PORTNUM, modbusMasterGetRequest(master), modbusMasterGetRequestLength(master));

        int len = uart_read_bytes(MB_PORTNUM, buffer, sizeof(buffer), pdMS_TO_TICKS(MODBUS_TIMEOUT));
        err     = modbusParseResponseRTU(master, modbusMasterGetRequest(master), modbusMasterGetRequestLength(master),
                                     buffer, len);

        if (!modbusIsOk(err)) {
            ESP_LOGW(TAG, "Write coil for %i error: %i %i", address, err.source, err.error);
            res = 1;
            vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
        }
    } while (res && counter++ < MODBUS_COMMUNICATION_ATTEMPTS);

    return res;
}