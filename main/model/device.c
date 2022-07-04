#include <assert.h>
#include "model.h"


#define ADDR2INDEX(addr)     (addr - 1)
#define INDEX2ADDR(addr)     (addr + 1)
#define ASSERT_ADDRESS(addr) assert(addr != 0 && ADDR2INDEX(addr) < MODBUS_MAX_DEVICES);



void device_list_init(device_t *devices) {
    for (size_t i = 0; i < MODBUS_MAX_DEVICES; i++) {
        devices[i].address = INDEX2ADDR(i);
        devices[i].status  = DEVICE_STATUS_NOT_CONFIGURED;
    }
}


uint8_t device_list_get_next_device_address(device_t *devices, uint8_t previous) {
    assert(devices != NULL);

    for (size_t i = ADDR2INDEX(previous + 1); i < MODBUS_MAX_DEVICES; i++) {
        if (devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
            return (uint8_t)INDEX2ADDR(i);
        }
    }

    return previous;
}


uint8_t device_list_get_prev_device_address(device_t *devices, uint8_t next) {
    assert(devices != NULL);

    if (ADDR2INDEX((int)next - 1) >= 0) {
        for (int i = ADDR2INDEX(next - 1); i >= 0; i--) {
            if (devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
                return (uint8_t)INDEX2ADDR(i);
            }
        }
    }

    return next;
}


int device_list_is_address_configured(device_t *devices, uint8_t address) {
    assert(devices != NULL);
    ASSERT_ADDRESS(address);

    return devices[ADDR2INDEX(address)].status != DEVICE_STATUS_NOT_CONFIGURED;
}


uint8_t device_list_get_available_address(device_t *devices, uint8_t previous) {
    assert(devices != NULL);

    for (size_t i = ADDR2INDEX(previous + 1); i < MODBUS_MAX_DEVICES; i++) {
        if (devices[i].status == DEVICE_STATUS_NOT_CONFIGURED) {
            return (uint8_t)INDEX2ADDR(i);
        }
    }

    return previous;
}



uint8_t device_list_get_next_device_address_by_class(device_t *devices, uint8_t previous, device_class_t class) {
    assert(devices != NULL);

    for (size_t i = ADDR2INDEX(previous + 1); i < MODBUS_MAX_DEVICES; i++) {
        if (devices[i].status != DEVICE_STATUS_NOT_CONFIGURED && devices[i].class == class) {
            return (uint8_t)INDEX2ADDR(i);
        }
    }

    return previous;
}


int device_list_new_device(device_t *devices, uint8_t address) {
    assert(devices != NULL);
    ASSERT_ADDRESS(address);
    size_t index = ADDR2INDEX(address);

    if (devices[index].status != DEVICE_STATUS_NOT_CONFIGURED) {
        return -1;
    }

    devices[index].status = DEVICE_STATUS_CONFIGURED;
    return 0;
}


address_map_t device_list_get_address_map(device_t *devices) {
    assert(devices != NULL);
    address_map_t map = {0};

    for (size_t i = 0; i < MODBUS_MAX_DEVICES; i++) {
        if (devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
            address_map_set_bit(&map, INDEX2ADDR(i));
        }
    }

    return map;
}


address_map_t device_list_get_error_map(device_t *devices) {
    assert(devices != NULL);
    address_map_t map = {0};

    for (size_t i = 0; i < MODBUS_MAX_DEVICES; i++) {
        if (devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
            if (devices[i].status == DEVICE_STATUS_COMMUNICATION_ERROR || devices[i].alarms > 0) {
                address_map_set_bit(&map, INDEX2ADDR(i));
            }
        }
    }

    return map;
}


void device_list_delete_device(device_t *devices, uint8_t address) {
    assert(devices != NULL);
    ASSERT_ADDRESS(address);
    size_t index          = ADDR2INDEX(address);
    devices[index].status = DEVICE_STATUS_NOT_CONFIGURED;
}


void device_list_get_device(device_t *devices, device_t *device, uint8_t address) {
    assert(devices != NULL);
    ASSERT_ADDRESS(address);
    size_t index = ADDR2INDEX(address);
    *device      = devices[index];
}


void device_list_set_device_error(device_t *devices, uint8_t address, int error) {
    assert(devices != NULL);
    ASSERT_ADDRESS(address);
    size_t index          = ADDR2INDEX(address);
    devices[index].status = error ? DEVICE_STATUS_COMMUNICATION_ERROR : DEVICE_STATUS_OK;
}


void device_list_set_device_sn(device_t *devices, uint8_t address, uint16_t serial_number) {
    assert(devices != NULL);
    ASSERT_ADDRESS(address);

    size_t index = ADDR2INDEX(address);
    if (devices[index].status != DEVICE_STATUS_NOT_CONFIGURED) {
        devices[index].serial_number = serial_number;
    }
}


void device_list_set_device_class(device_t *devices, uint8_t address, uint16_t class) {
    assert(devices != NULL);
    ASSERT_ADDRESS(address);
    size_t index = ADDR2INDEX(address);
    if (devices[index].status != DEVICE_STATUS_NOT_CONFIGURED) {
        devices[index].class = class;
    }
}


uint8_t device_list_set_device_alarms(device_t *devices, uint8_t address, uint16_t alarms) {
    assert(devices != NULL);
    ASSERT_ADDRESS(address);
    uint8_t res   = 0;
    size_t  index = ADDR2INDEX(address);
    if (devices[index].status != DEVICE_STATUS_NOT_CONFIGURED) {
        res                   = devices[index].alarms != alarms;
        devices[index].alarms = alarms;
    }

    return res;
}


size_t device_list_get_configured_devices(device_t *devices) {
    assert(devices != NULL);
    size_t count = 0;

    for (size_t i = 0; i < MODBUS_MAX_DEVICES; i++) {
        if (devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
            count++;
        }
    }

    return count;
}
