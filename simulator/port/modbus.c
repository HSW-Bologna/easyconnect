#include <stdint.h>
#include "peripherals/modbus.h"


void modbus_automatic_commissioning(void) {}

void modbus_configure_device_address(uint8_t address) {}

int modbus_automatic_commissioning_done(unsigned long millis) {
    return 0;
}


int modbus_get_response(modbus_response_t *response) {
    response->error = 0;
    return 0;
}


void modbus_set_device_class(uint8_t address, uint8_t class) {}

void modbus_read_device_info(uint8_t address) {}

void modbus_set_device_output(uint8_t address, int value) {}

void modbus_scan(void) {}