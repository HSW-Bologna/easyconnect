#ifndef MODBUS_H_INCLUDED
#define MODBUS_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>




typedef enum {
    MODBUS_RESPONSE_CODE_INFO,
} modbus_response_code_t;


typedef struct {
    modbus_response_code_t code;
    union {
        struct {
            uint8_t address;
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
void modbus_automatic_commissioning(void);
int  modbus_automatic_commissioning_done(unsigned long millis);


#endif