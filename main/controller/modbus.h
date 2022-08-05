#ifndef MODBUS_H_INCLUDED
#define MODBUS_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

typedef enum {
    MODBUS_RESPONSE_DEVICE_AUTOMATIC_CONFIGURATION,
    MODBUS_RESPONSE_DEVICE_AUTOMATIC_CONFIGURATION_LISTENING_DONE,
    MODBUS_RESPONSE_CODE_INFO,
    MODBUS_RESPONSE_CODE_ALARMS_REG,
    MODBUS_RESPONSE_CODE_SCAN_DONE,
    MODBUS_RESPONSE_CODE_DEVICE_OK,
    MODBUS_RESPONSE_CODE_ERROR,
    MODBUS_RESPONSE_CODE_ALARM,
} modbus_response_code_t;

typedef struct {
    modbus_response_code_t code;
    uint8_t                address;
    int                    error;
    int                    scanning;
    int                    devices_number;
    union {
        struct {
            uint16_t class;
            uint16_t serial_number;
            uint16_t firmware_version;
        };
        uint16_t alarms;
    };
} modbus_response_t;

void modbus_init(void);
void modbus_read_device_info(uint8_t address);
void modbus_read_device_inputs(uint8_t address);
void modbus_set_device_output(uint8_t address, int value);
void modbus_automatic_commissioning(uint16_t expected_devices);
int  modbus_automatic_commissioning_done(unsigned long millis);
int  modbus_get_response(modbus_response_t *response);
void modbus_set_class_output(uint16_t class, int value);
void modbus_scan(void);
void modbus_stop_current_operation(void);
void modbus_set_fan_speed(uint8_t address, size_t speed);
void modbus_read_device_alarms(uint8_t address);

#endif