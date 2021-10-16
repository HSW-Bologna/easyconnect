#ifndef MODBUS_H_INCLUDED
#define MODBUS_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

typedef enum {
    MODBUS_RESPONSE_DEVICE_MANUAL_CONFIGURATION,
    MODBUS_RESPONSE_CODE_INFO,
    MODBUS_RESPONSE_CODE_CLASS,
    MODBUS_RESPONSE_CODE_SCAN_DONE,
    MODBUS_RESPONSE_CODE_DEVICE_OK,
    MODBUS_RESPONSE_ERROR,
} modbus_response_code_t;

typedef struct {
    modbus_response_code_t code;
    uint8_t                address;
    int                    error;
    union {
        struct {
            uint16_t class;
            uint16_t serial_number;
        };
    };
} modbus_response_t;

void modbus_init(void);
void modbus_configure_device_address(uint8_t address);
void modbus_read_device_info(uint8_t address);
void modbus_read_device_inputs(uint8_t address);
void modbus_set_device_output(uint8_t address, int value);
void modbus_set_device_class(uint8_t address, uint8_t class);
void modbus_automatic_commissioning(void);
int  modbus_automatic_commissioning_done(unsigned long millis);
int  modbus_get_response(modbus_response_t *response);
void modbus_set_class_output(uint16_t class, int value);
void modbus_scan(void);

#endif