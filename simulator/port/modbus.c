#include <stdint.h>
#include "peripherals/modbus.h"


void modbus_automatic_commissioning(void) {
}

void modbus_configure_device_address(uint8_t address) {
}

int modbus_automatic_commissioning_done(unsigned long millis) {
    return 0;
}


int modbus_get_response(modbus_response_t *response) {
    response->error = 0;
    return 0;
}