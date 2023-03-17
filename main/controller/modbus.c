#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "config/app_config.h"
#include "lightmodbus/lightmodbus.h"
#include "lightmodbus/master_func.h"
#include "gel/timer/timecheck.h"
#include "utils/utils.h"
#include "model/device.h"
#include "easyconnect_interface.h"
#include "modbus.h"
#include "peripherals/rs485.h"
#include "gel/serializer/serializer.h"


#define MODBUS_RESPONSE_03_LEN(data_len) (5 + data_len * 2)
#define MODBUS_RESPONSE_05_LEN           8

#define MODBUS_MESSAGE_QUEUE_SIZE     512
#define MODBUS_TIMEOUT                30
#define MODBUS_MAX_PACKET_SIZE        256
#define MODBUS_BROADCAST_ADDRESS      0
#define MODBUS_COMMUNICATION_ATTEMPTS 3

#define HOLDING_REGISTER_MOTOR_SPEED 256
#define HOLDING_REGISTER_PRESSURE    256
#define HOLDING_REGISTER_WORK_HOURS  256

#define MODBUS_AUTO_COMMISSIONING_DONE_BIT 0x01

typedef enum {
    TASK_MESSAGE_CODE_READ_DEVICE_INFO,
    TASK_MESSAGE_CODE_READ_DEVICE_MESSAGES,
    TASK_MESSAGE_CODE_READ_DEVICE_STATE,
    TASK_MESSAGE_CODE_READ_DEVICE_PRESSURE,
    TASK_MESSAGE_CODE_READ_DEVICE_INPUTS,
    TASK_MESSAGE_CODE_READ_DEVICE_WORK_HOURS,
    TASK_MESSAGE_CODE_RESET_DEVICE_WORK_HOURS,
    TASK_MESSAGE_CODE_SET_DEVICE_OUTPUT,
    TASK_MESSAGE_CODE_BEGIN_AUTOMATIC_COMMISSIONING,
    TASK_MESSAGE_CODE_SET_CLASS_OUTPUT,
    TASK_MESSAGE_CODE_SET_FAN_PERCENTAGE,
    TASK_MESSAGE_CODE_UPDATE_TIME,
    TASK_MESSAGE_CODE_UPDATE_EVENTS,
    TASK_MESSAGE_CODE_SCAN,
} task_message_code_t;


struct __attribute__((packed)) task_message {
    task_message_code_t code;
    uint8_t             address;
    union {
        struct {
            int value;
            uint16_t class;
        };
        uint16_t expected_devices;
        uint16_t num_messages;
        uint16_t event_count;
    };
};

typedef struct {
    uint32_t device_map[MODBUS_MAX_DEVICES];
} device_map_context_t;

typedef struct {
    uint16_t start;
    void    *pointer;
} master_context_t;


static LIGHTMODBUS_RET_ERROR build_custom_request(ModbusMaster *status, uint8_t function, uint8_t *data, size_t len);
static LIGHTMODBUS_RET_ERROR random_serial_number_response(ModbusMaster *status, uint8_t address, uint8_t function,
                                                           const uint8_t *requestPDU, uint8_t requestLength,
                                                           const uint8_t *responsePDU, uint8_t responseLength);

static void modbus_task(void *args);
static int  write_holding_register(ModbusMaster *master, uint8_t address, uint16_t index, uint16_t data);
static int  write_holding_registers(ModbusMaster *master, uint8_t address, uint16_t starting_address, uint16_t *data,
                                    size_t num);
static int  write_coil(ModbusMaster *master, uint8_t address, uint16_t index, int value);
static int  read_holding_registers(ModbusMaster *master, uint16_t *registers, uint8_t address, uint16_t start,
                                   uint16_t count);
static void send_custom_function(ModbusMaster *master, uint8_t address, uint8_t function, uint8_t *data, size_t len);
static void configure_device_with_serial_number(ModbusMaster *master, uint8_t address, uint32_t serial_number);


static const char   *TAG       = "Modbus";
static QueueHandle_t messageq  = NULL;
static QueueHandle_t responseq = NULL;
static TaskHandle_t  task      = NULL;


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

    {EASYCONNECT_FUNCTION_CODE_RANDOM_SERIAL_NUMBER, random_serial_number_response},
    // Guard - prevents 0 size array
    {0, NULL},
};


void modbus_init(void) {
    static StaticQueue_t static_queue1;
    static uint8_t       queue_buffer1[MODBUS_MESSAGE_QUEUE_SIZE * sizeof(struct task_message)] = {0};
    messageq =
        xQueueCreateStatic(MODBUS_MESSAGE_QUEUE_SIZE, sizeof(struct task_message), queue_buffer1, &static_queue1);

    static StaticQueue_t static_queue2;
    static uint8_t       queue_buffer2[MODBUS_MESSAGE_QUEUE_SIZE * sizeof(modbus_response_t)] = {0};
    responseq = xQueueCreateStatic(MODBUS_MESSAGE_QUEUE_SIZE, sizeof(modbus_response_t), queue_buffer2, &static_queue2);

#ifdef PC_SIMULATOR
    xTaskCreate(modbus_task, TAG, BASE_TASK_SIZE * 6, NULL, 5, &task);
#else
    static uint8_t      task_stack[BASE_TASK_SIZE * 6] = {0};
    static StaticTask_t static_task;
    task = xTaskCreateStatic(modbus_task, TAG, sizeof(task_stack), NULL, 5, task_stack, &static_task);
#endif
}


int modbus_get_response(modbus_response_t *response) {
    return xQueueReceive(responseq, response, 0) == pdTRUE;
}


void modbus_automatic_commissioning(uint16_t expected_devices) {
    struct task_message message = {.code             = TASK_MESSAGE_CODE_BEGIN_AUTOMATIC_COMMISSIONING,
                                   .expected_devices = expected_devices};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_update_events(uint8_t address, uint16_t previous_event_count) {
    struct task_message message = {
        .code = TASK_MESSAGE_CODE_UPDATE_EVENTS, .address = address, .event_count = previous_event_count};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_update_time(void) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_UPDATE_TIME};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_set_class_output(uint16_t class, int value) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_SET_CLASS_OUTPUT, .class = class, .value = value};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_set_fan_percentage(uint8_t address, uint8_t percentage) {
    struct task_message message = {
        .code = TASK_MESSAGE_CODE_SET_FAN_PERCENTAGE, .address = address, .value = percentage};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_scan(void) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_SCAN};
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


void modbus_read_device_messages(uint8_t address, uint8_t device_model) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_READ_DEVICE_MESSAGES, .address = address};

    switch (device_model) {
        case EASYCONNECT_DEVICE_RELE_PERIPHERAL:
            message.num_messages = 2;
            break;

        default:
            message.num_messages = 1;
            break;
    }

    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_read_device_state(uint8_t address) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_READ_DEVICE_STATE, .address = address};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_read_device_pressure(uint8_t address) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_READ_DEVICE_PRESSURE, .address = address};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_read_device_work_hours(uint8_t address) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_READ_DEVICE_WORK_HOURS, .address = address};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_reset_device_work_hours(uint8_t address) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_RESET_DEVICE_WORK_HOURS, .address = address};
    xQueueSend(messageq, &message, portMAX_DELAY);
    modbus_read_device_work_hours(address);
}


void modbus_read_device_inputs(uint8_t address) {
    struct task_message message = {.code = TASK_MESSAGE_CODE_READ_DEVICE_INPUTS, .address = address};
    xQueueSend(messageq, &message, portMAX_DELAY);
}


void modbus_stop_current_operation(void) {
    xTaskNotifyGive(task);
}


static ModbusError data_callback(const ModbusMaster *master, const ModbusDataCallbackArgs *args) {
    master_context_t *ctx = modbusMasterGetUserPointer(master);

    if (ctx != NULL) {
        switch (args->type) {
            case MODBUS_HOLDING_REGISTER: {
                uint16_t *buffer                 = ctx->pointer;
                buffer[args->index - ctx->start] = args->value;
                break;
            }

            case MODBUS_DISCRETE_INPUT: {
                uint8_t *buffer                  = ctx->pointer;
                buffer[args->index - ctx->start] = args->value;
                break;
            }

            case MODBUS_INPUT_REGISTER: {
                uint16_t *buffer                 = ctx->pointer;
                buffer[args->index - ctx->start] = args->value;
                break;
            }

            case MODBUS_COIL: {
                uint8_t *buffer                  = ctx->pointer;
                buffer[args->index - ctx->start] = args->value;
                break;
            }
        }
    }

    return MODBUS_OK;
}


static ModbusError exception_callback(const ModbusMaster *master, uint8_t address, uint8_t function,
                                      ModbusExceptionCode code) {
    modbus_response_t *response = modbusMasterGetUserPointer(master);
    // printf("Received exception (function %d) from slave %d code %d\n", function, address, code);

    if (response != NULL) {
        switch (code) {
            case MODBUS_EXCEP_SLAVE_FAILURE:
                response->address = address;
                response->code    = MODBUS_RESPONSE_CODE_ALARM;
                break;

            default:
                response->address = address;
                response->code    = MODBUS_RESPONSE_CODE_ERROR;
                break;
        }
    }

    return MODBUS_OK;
}


static void modbus_task(void *args) {
    (void)args;
    ModbusMaster    master;
    ModbusErrorInfo err = modbusMasterInit(&master,
                                           data_callback,              // Callback for handling incoming data
                                           exception_callback,         // Exception callback (optional)
                                           modbusDefaultAllocator,     // Memory allocator used to allocate request
                                           custom_functions,           // Set of supported functions
                                           modbusMasterDefaultFunctionCount + 1     // Number of supported functions
    );

    // Check for errors
    assert(modbusIsOk(err) && "modbusMasterInit() failed");
    struct task_message message                        = {0};
    uint8_t             buffer[MODBUS_MAX_PACKET_SIZE] = {0};
    modbus_response_t   error_resp                     = {.code = MODBUS_RESPONSE_CODE_ERROR};
    unsigned long       timestamp                      = 0;

    ESP_LOGI(TAG, "Task starting");
    send_custom_function(&master, MODBUS_BROADCAST_ADDRESS, EASYCONNECT_FUNCTION_CODE_NETWORK_INITIALIZATION, NULL, 0);

    for (;;) {
        xTaskNotifyStateClear(task);

        if (xQueueReceive(messageq, &message, pdMS_TO_TICKS(100))) {
            modbus_response_t response = {.address = message.address};
            error_resp.address         = message.address;

            switch (message.code) {
                case TASK_MESSAGE_CODE_READ_DEVICE_INFO: {
                    response.code    = MODBUS_RESPONSE_CODE_INFO;
                    response.address = message.address;
                    ESP_LOGI(TAG, "Reading info from %i", message.address);

                    uint16_t registers[5];
                    if (read_holding_registers(&master, registers, message.address,
                                               EASYCONNECT_HOLDING_REGISTER_FIRMWARE_VERSION,
                                               sizeof(registers) / sizeof(registers[0]))) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    } else {
                        response.firmware_version = registers[0];
                        response.class            = registers[1];
                        response.serial_number    = (registers[2] << 16) | registers[3];
                        xQueueSend(responseq, &response, portMAX_DELAY);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_READ_DEVICE_MESSAGES: {
                    response.code         = MODBUS_RESPONSE_CODE_MESSAGES;
                    response.address      = message.address;
                    response.num_messages = message.num_messages;

                    uint16_t  num_registers = message.num_messages * EASYCONNECT_MESSAGE_NUM_REGISTERS;
                    uint16_t *registers     = (uint16_t *)malloc(sizeof(uint16_t) * num_registers);
                    assert(registers != NULL);

                    for (size_t i = 0; i < response.num_messages; i++) {
                        response.messages[i] = calloc(EASYCONNECT_MESSAGE_SIZE + 1, 1);
                        assert(response.messages[i]);
                    }

                    if (read_holding_registers(&master, registers, message.address,
                                               EASYCONNECT_HOLDING_REGISTER_MESSAGE_1, num_registers)) {
                        ESP_LOGI(TAG, "Error while reading messages");
                        for (size_t i = 0; i < response.num_messages; i++) {
                            free(response.messages[i]);
                        }
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    } else {
                        for (size_t i = 0; i < num_registers; i++) {
                            uint16_t num_message                       = i / EASYCONNECT_MESSAGE_NUM_REGISTERS;
                            uint16_t char_index                        = (i % EASYCONNECT_MESSAGE_NUM_REGISTERS) * 2;
                            response.messages[num_message][char_index] = (registers[i] >> 8) & 0xFF;
                            response.messages[num_message][char_index + 1] = registers[i] & 0xFF;
                        }

                        xQueueSend(responseq, &response, portMAX_DELAY);
                    }
                    free(registers);
                    break;
                }

                case TASK_MESSAGE_CODE_READ_DEVICE_STATE: {
                    response.code    = MODBUS_RESPONSE_CODE_STATE;
                    response.address = message.address;

                    uint16_t registers[2];
                    if (read_holding_registers(&master, registers, message.address, EASYCONNECT_HOLDING_REGISTER_ALARMS,
                                               sizeof(registers) / sizeof(registers[0]))) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    } else {
                        response.alarms = registers[0];
                        response.state  = registers[1];
                        xQueueSend(responseq, &response, portMAX_DELAY);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_READ_DEVICE_INPUTS: {
                    ESP_LOGI(TAG, "Reading inputs from %i", message.address);
                    err = modbusBuildRequest02RTU(&master, message.address, 0, 2);
                    assert(modbusIsOk(err));
                    rs485_write(modbusMasterGetRequest(&master), modbusMasterGetRequestLength(&master));

                    int len = rs485_read(buffer, sizeof(buffer), pdMS_TO_TICKS(MODBUS_TIMEOUT));
                    err     = modbusParseResponseRTU(&master, modbusMasterGetRequest(&master),
                                                     modbusMasterGetRequestLength(&master), buffer, len);

                    if (!modbusIsOk(err)) {
                        ESP_LOG_BUFFER_HEX(TAG, buffer, len);
                        ESP_LOGW(TAG, "Cold not query device %i: %i %i", message.address, err.source, err.error);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_SET_DEVICE_OUTPUT: {
                    response.code    = MODBUS_RESPONSE_CODE_DEVICE_OK;
                    response.address = message.address;
                    if (write_coil(&master, message.address, 0, message.value)) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    } else {
                        xQueueSend(responseq, &response, portMAX_DELAY);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_UPDATE_EVENTS: {
                    uint16_t event_count = 0;

                    if (read_holding_registers(&master, &event_count, message.address,
                                               EASYCONNECT_HOLDING_REGISTER_LOGS_COUNTER, 1)) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                        break;
                    }

                    if (event_count > message.event_count) {
                        uint16_t count      = event_count - message.event_count;
                        count               = count > 8 ? 8 : count;
                        uint16_t *registers = malloc(count * sizeof(uint16_t));
                        assert(registers != NULL);

                        if (read_holding_registers(&master, registers, message.address,
                                                   EASYCONNECT_HOLDING_REGISTER_LOGS, count)) {
                            xQueueSend(responseq, &error_resp, portMAX_DELAY);
                            break;
                        }

                        ESP_LOGI(TAG, "New events from %i: %i", message.address, count);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_SCAN: {
                    ESP_LOGI(TAG, "Scan start");
                    response.code     = MODBUS_RESPONSE_CODE_INFO;
                    response.scanning = 1;

                    for (size_t i = 1; i <= MODBUS_MAX_DEVICES; i++) {
                        if (ulTaskNotifyTake(pdTRUE, 0)) {
                            break;
                        }

                        response.address = i;
                        if (read_holding_registers(&master, NULL, i, EASYCONNECT_HOLDING_REGISTER_ADDRESS, 1) == 0) {
                            response.error = 0;
                        } else {
                            response.error = 1;
                        }
                        xQueueSend(responseq, &response, portMAX_DELAY);
                        vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
                    }

                    ESP_LOGI(TAG, "Scan done!");
                    response.code = MODBUS_RESPONSE_CODE_SCAN_DONE;
                    xQueueSend(responseq, &response, portMAX_DELAY);
                    break;
                }

                case TASK_MESSAGE_CODE_UPDATE_TIME: {
                    struct timeval timeval = {0};
                    gettimeofday(&timeval, NULL);
                    uint64_t timestamp = (uint64_t)timeval.tv_sec;

                    uint8_t buffer[8] = {0};
                    serialize_uint64_be(buffer, timestamp);

                    send_custom_function(&master, MODBUS_BROADCAST_ADDRESS, 67, buffer,
                                         sizeof(buffer));     // TODO: use macro
                    break;
                }

                case TASK_MESSAGE_CODE_BEGIN_AUTOMATIC_COMMISSIONING: {
                    uint16_t period = 0;

                    if (message.expected_devices < 5) {
                        period = 4;
                    } else if (message.expected_devices < 10) {
                        period = 10;
                    } else if (message.expected_devices < 20) {
                        period = 20;
                    } else if (message.expected_devices < 30) {
                        period = 30;
                    } else if (message.expected_devices < 50) {
                        period = 60;
                    } else {
                        period = 120;
                    }

                    uint8_t data[] = {(period >> 8) & 0xFF, period & 0xFF};

                    send_custom_function(&master, MODBUS_BROADCAST_ADDRESS,
                                         EASYCONNECT_FUNCTION_CODE_RANDOM_SERIAL_NUMBER, data, sizeof(data));
                    DEVICE_LIST(found_devices);
                    device_list_init(found_devices);
                    uint8_t found_address = 1;

                    ESP_LOGI(TAG, "Starting listening period");
                    unsigned long timestamp = get_millis();
                    while (!is_expired(timestamp, get_millis(), period * 1000UL)) {
                        // While waiting here messages are thrown away
                        struct task_message zero    = {0};
                        size_t              counter = 0;
                        while (xQueueReceive(messageq, &zero, 0) == pdTRUE && counter++ < MODBUS_MESSAGE_QUEUE_SIZE)
                            ;

                        uint8_t response[6] = {0};
                        int     len         = rs485_read(response, sizeof(response), pdMS_TO_TICKS(MODBUS_TIMEOUT));
                        if (len == sizeof(response)) {
                            uint16_t calc_crc = modbusCRC(response, 2);
                            uint16_t read_crc = (response[4] << 8) | response[5];
                            if (calc_crc == read_crc) {
                                uint32_t serial_number =
                                    (response[0] << 24) | (response[1] << 16) | (response[2] << 8) | response[3];
                                ESP_LOGI(TAG, "Found new device with sn %i, given address %i", serial_number,
                                         found_address);
                                device_list_configure_device(found_devices, found_address);
                                device_list_set_device_sn(found_devices, found_address, serial_number);

                                if (found_address == 254) {
                                    ESP_LOGW(TAG, "Network reached max capacity!");
                                } else {
                                    found_address++;
                                }
                            }
                        }
                        rs485_flush();
                    }

                    ESP_LOGI(TAG, "Listening done, configuring...");
                    response.code = MODBUS_RESPONSE_DEVICE_AUTOMATIC_CONFIGURATION_LISTENING_DONE;
                    xQueueSend(responseq, &response, portMAX_DELAY);

                    vTaskDelay(pdMS_TO_TICKS(500));
                    rs485_flush();

                    uint8_t starting_address = 0;
                    uint8_t address = device_list_get_next_configured_device_address(found_devices, starting_address);
                    while (address != starting_address) {
                        device_t device = device_list_get_device(found_devices, address);
                        ESP_LOGI(TAG, "Trying to configure device %i with address %i", device.serial_number, address);
                        configure_device_with_serial_number(&master, address, device.serial_number);
                        starting_address = address;
                        address = device_list_get_next_configured_device_address(found_devices, starting_address);
                    }

                    size_t confirmed_devices = 0;
                    starting_address         = 0;
                    address = device_list_get_next_configured_device_address(found_devices, starting_address);
                    while (address != starting_address) {
                        device_t device = device_list_get_device(found_devices, address);
                        ESP_LOGI(TAG, "Checking configuration for device %i with address %i", device.serial_number,
                                 address);
                        if (read_holding_registers(&master, NULL, device.address, EASYCONNECT_HOLDING_REGISTER_ADDRESS,
                                                   1)) {
                            // TODO: Leggi effettivamente il numero di serie
                            ESP_LOGW(TAG, "Device %i did not have address %i", device.serial_number, address);
                        } else {
                            confirmed_devices++;
                        }

                        starting_address = address;
                        address = device_list_get_next_configured_device_address(found_devices, starting_address);
                    }

                    response.code           = MODBUS_RESPONSE_DEVICE_AUTOMATIC_CONFIGURATION;
                    response.devices_number = confirmed_devices;
                    xQueueSend(responseq, &response, portMAX_DELAY);
                    break;
                }

                case TASK_MESSAGE_CODE_SET_CLASS_OUTPUT: {
                    uint8_t data[] = {(message.class >> 8) & 0xFF, message.class & 0xFF, message.value};
                    send_custom_function(&master, MODBUS_BROADCAST_ADDRESS, EASYCONNECT_FUNCTION_CODE_SET_CLASS_OUTPUT,
                                         data, sizeof(data));
                    break;
                }

                case TASK_MESSAGE_CODE_SET_FAN_PERCENTAGE: {
                    ESP_LOGI(TAG, "Setting fan speed for device %i %i%%", message.address, message.value);
                    if (write_holding_register(&master, message.address, HOLDING_REGISTER_MOTOR_SPEED,
                                               (uint16_t)message.value)) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_READ_DEVICE_PRESSURE: {
                    response.code    = MODBUS_RESPONSE_CODE_PRESSURE;
                    response.address = message.address;

                    if (read_holding_registers(&master, &response.pressure, message.address, HOLDING_REGISTER_PRESSURE,
                                               1)) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    } else {
                        xQueueSend(responseq, &response, portMAX_DELAY);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_READ_DEVICE_WORK_HOURS: {
                    response.code    = MODBUS_RESPONSE_CODE_WORK_HOURS;
                    response.address = message.address;

                    if (read_holding_registers(&master, &response.work_hours, message.address,
                                               HOLDING_REGISTER_WORK_HOURS, 1)) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    } else {
                        xQueueSend(responseq, &response, portMAX_DELAY);
                    }
                    break;
                }

                case TASK_MESSAGE_CODE_RESET_DEVICE_WORK_HOURS: {
                    if (write_holding_register(&master, message.address, HOLDING_REGISTER_WORK_HOURS, 0)) {
                        xQueueSend(responseq, &error_resp, portMAX_DELAY);
                    }
                    break;
                }
            }
            vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT / 2));
        }

        if (is_expired(timestamp, get_millis(), 100)) {
            send_custom_function(&master, MODBUS_BROADCAST_ADDRESS, EASYCONNECT_FUNCTION_CODE_HEARTBEAT, NULL, 0);
            vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT / 2));
            timestamp = get_millis();
        }
    }

    vTaskDelete(NULL);
}


static LIGHTMODBUS_RET_ERROR build_custom_request(ModbusMaster *status, uint8_t function, uint8_t *data, size_t len) {
    if (modbusMasterAllocateRequest(status, len + 1)) {
        return MODBUS_GENERAL_ERROR(ALLOC);
    }

    status->request.pdu[0] = function;
    if (data != NULL) {
        for (size_t i = 0; i < len; i++) {
            status->request.pdu[1 + i] = data[i];
        }
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
    }

    device_map_context_t *ctx = modbusMasterGetUserPointer(master);

    uint32_t serial_number   = (responsePDU[1] << 24) | (responsePDU[2] << 16) | (responsePDU[3] << 8) | responsePDU[4];
    ctx->device_map[address] = serial_number;

    ESP_LOGI(TAG, "Received serial number %i from %i", serial_number, address);

    return MODBUS_NO_ERROR();
}


static void send_custom_function(ModbusMaster *master, uint8_t address, uint8_t function, uint8_t *data, size_t len) {
    ModbusErrorInfo err = modbusBeginRequestRTU(master);
    assert(modbusIsOk(err));
    err = build_custom_request(master, function, data, len);
    assert(modbusIsOk(err));
    err = modbusEndRequestRTU(master, MODBUS_BROADCAST_ADDRESS);
    assert(modbusIsOk(err));
    /* Broadcast message, we expect no answer */
    rs485_write(modbusMasterGetRequest(master), modbusMasterGetRequestLength(master));
    vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
}


static void configure_device_with_serial_number(ModbusMaster *master, uint8_t address, uint32_t serial_number) {
    uint8_t data[] = {
        address,
        (serial_number >> 24) & 0xFF,
        (serial_number >> 16) & 0xFF,
        (serial_number >> 8) & 0xFF,
        serial_number & 0xFF,
    };
    send_custom_function(master, MODBUS_BROADCAST_ADDRESS, EASYCONNECT_FUNCTION_CODE_CONFIG_ADDRESS, data,
                         sizeof(data));
    vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
    send_custom_function(master, MODBUS_BROADCAST_ADDRESS, EASYCONNECT_FUNCTION_CODE_CONFIG_ADDRESS, data,
                         sizeof(data));
    vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
}


static int write_holding_registers(ModbusMaster *master, uint8_t address, uint16_t starting_address, uint16_t *data,
                                   size_t num) {
    uint8_t buffer[MODBUS_MAX_PACKET_SIZE] = {0};
    int     res                            = 0;
    size_t  counter                        = 0;

    rs485_flush();

    do {
        res                 = 0;
        ModbusErrorInfo err = modbusBuildRequest16RTU(master, address, starting_address, num, data);
        assert(modbusIsOk(err));
        rs485_write(modbusMasterGetRequest(master), modbusMasterGetRequestLength(master));

        int len = rs485_read(buffer, sizeof(buffer), pdMS_TO_TICKS(MODBUS_TIMEOUT));
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
    uint8_t buffer[MODBUS_RESPONSE_05_LEN] = {0};
    int     res                            = 0;
    size_t  counter                        = 0;

    rs485_flush();

    do {
        ModbusErrorInfo err = modbusBuildRequest05RTU(master, address, index, value);
        assert(modbusIsOk(err));
        rs485_write(modbusMasterGetRequest(master), modbusMasterGetRequestLength(master));

        int len = rs485_read(buffer, sizeof(buffer), pdMS_TO_TICKS(MODBUS_TIMEOUT));
        err     = modbusParseResponseRTU(master, modbusMasterGetRequest(master), modbusMasterGetRequestLength(master),
                                         buffer, len);

        if (!modbusIsOk(err)) {
            ESP_LOGD(TAG, "Write coil for %i error: %i %i", address, err.source, err.error);
            res = 1;
            vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
        }
    } while (res && counter++ < MODBUS_COMMUNICATION_ATTEMPTS);

    return res;
}


static int read_holding_registers(ModbusMaster *master, uint16_t *registers, uint8_t address, uint16_t start,
                                  uint16_t count) {
    ModbusErrorInfo err;
    uint8_t         buffer[MODBUS_RESPONSE_03_LEN(count)];
    int             res     = 0;
    size_t          counter = 0;

    rs485_flush();

    master_context_t ctx = {.pointer = registers, .start = start};
    if (registers == NULL) {
        modbusMasterSetUserPointer(master, NULL);
    } else {
        modbusMasterSetUserPointer(master, &ctx);
    }

    do {
        res = 0;
        err = modbusBuildRequest03RTU(master, address, start, count);
        assert(modbusIsOk(err));
        rs485_write(modbusMasterGetRequest(master), modbusMasterGetRequestLength(master));

        int len = rs485_read(buffer, sizeof(buffer), pdMS_TO_TICKS(MODBUS_TIMEOUT));
        err     = modbusParseResponseRTU(master, modbusMasterGetRequest(master), modbusMasterGetRequestLength(master),
                                         buffer, len);

        if (!modbusIsOk(err)) {
            ESP_LOGD(TAG, "Read holding registers for %i error %zu: %i %i", address, counter, err.source, err.error);
            if (len == 0) {
                ESP_LOGD(TAG, "Empty packet!");
            } else {
                ESP_LOG_BUFFER_HEX(TAG, buffer, len);
            }
            res = 1;
            vTaskDelay(pdMS_TO_TICKS(MODBUS_TIMEOUT));
        }
    } while (res && counter++ < MODBUS_COMMUNICATION_ATTEMPTS);

    return res;
}