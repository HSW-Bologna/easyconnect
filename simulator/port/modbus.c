#include <stdint.h>
#include "peripherals/modbus.h"


void modbus_automatic_commissioning(void) {}

int modbus_automatic_commissioning_done(unsigned long millis) {
    return 0;
}


int modbus_get_response(modbus_response_t *response) {
    response->error = 0;
    return 0;
}

void modbus_set_fan_speed(uint8_t address, size_t speed) {}

void modbus_read_device_alarms(uint8_t address) {}

void modbus_read_device_info(uint8_t address) {}

void modbus_set_device_output(uint8_t address, int value) {}

void modbus_scan(void) {}

void modbus_stop_current_operation() {}

void modbus_set_class_output(uint16_t class, int value) {}