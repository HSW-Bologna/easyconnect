#include "freertos/projdefs.h"
#include "hardwareprofile.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "config/app_config.h"
#include <sys/types.h>
#include "lightmodbus/lightmodbus.h"
#include "lightmodbus/master_func.h"
#include <assert.h>

#define MB_PORTNUM 1
// Timeout threshold for UART = number of symbols (~10 tics) with unchanged state on receive pin
#define ECHO_READ_TOUT (3)     // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks
static const char* TAG = "Modbus";

ModbusError dataCallback(const ModbusMaster *master, const ModbusDataCallbackArgs *args);
ModbusError masterExceptionCallback(const ModbusMaster *master,uint8_t address, uint8_t function, ModbusExceptionCode code);

void modbus_init(void) {

    ModbusMaster master;
    ModbusErrorInfo err;
    err = modbusMasterInit(
        &master,          
        dataCallback,                  // Callback for handling incoming data
        masterExceptionCallback,             // Exception callback (optional)
        modbusDefaultAllocator,          // Memory allocator used to allocate request
        modbusMasterDefaultFunctions,    // Set of supported functions
        modbusMasterDefaultFunctionCount // Number of supported functions
    );
    
    // Check for errors
    assert(modbusIsOk(err) && "modbusMasterInit() failed"); 

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

    return; 
    
    int val = 0;
    for (;;) {
        ModbusErrorInfo err;
        err = modbusBuildRequest05RTU(&master, 2, 0,val );
        assert(modbusIsOk(err) && "modbusBuildRequest01RTU() failed");
        uart_write_bytes(MB_PORTNUM, modbusMasterGetRequest(&master), modbusMasterGetRequestLength(&master));
        uint8_t buffer[256] = {0};
        int len = uart_read_bytes(MB_PORTNUM, buffer, 256, pdMS_TO_TICKS(40));
        err = modbusParseResponseRTU(&master,modbusMasterGetRequest(&master),modbusMasterGetRequestLength(&master),buffer,len);
        ESP_LOGI(TAG, "%i %i %i", len, err.source, err.error);
        vTaskDelay(1000);
        val = !val;
    }

}

ModbusError dataCallback(const ModbusMaster *master, const ModbusDataCallbackArgs *args)
{
    printf(
        "Received data:\n"
        "\t from: %d\n"
        "\tvalue: %d\n",
        args->address,
        args->value
    );
 
    // Always return MODBUS_OK
    return MODBUS_OK;
}

ModbusError masterExceptionCallback(const ModbusMaster *master,uint8_t address, uint8_t function, ModbusExceptionCode code) {
    printf("Received exception (function %d) from slave %d\n",function,address);
 
    // Always return MODBUS_OK
    return MODBUS_OK;
}