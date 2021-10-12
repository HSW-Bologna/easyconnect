#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "model.h"


#define ADDR2INDEX(addr)     (addr - 1)
#define INDEX2ADDR(addr)     (addr + 1)
#define ASSERT_ADDRESS(addr) assert(addr != 0 && ADDR2INDEX(addr) < MODEL_MAX_DEVICES);


void model_init(model_t *pmodel) {
    assert(pmodel != NULL);

    memset(pmodel, 0, sizeof(model_t));
    pmodel->temperature = 32;

    for (size_t i = 0; i < MODEL_MAX_DEVICES; i++) {
        pmodel->devices[i].address = INDEX2ADDR(i);
    }
}


int model_get_temperature(model_t *pmodel) {
    assert(pmodel != NULL);
    return pmodel->temperature;
}


int model_is_address_configured(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    ASSERT_ADDRESS(address);

    return pmodel->devices[ADDR2INDEX(address)].status != DEVICE_STATUS_NOT_CONFIGURED;
}


uint8_t model_get_available_address(model_t *pmodel, uint8_t previous) {
    assert(pmodel != NULL);

    for (size_t i = ADDR2INDEX(previous + 1); i < MODEL_MAX_DEVICES; i++) {
        if (pmodel->devices[i].status == DEVICE_STATUS_NOT_CONFIGURED) {
            return (uint8_t)INDEX2ADDR(i);
        }
    }

    return previous;
}


uint8_t model_get_next_device_address_by_class(model_t *pmodel, uint8_t previous, device_class_t class) {
    assert(pmodel != NULL);

    for (size_t i = ADDR2INDEX(previous + 1); i < MODEL_MAX_DEVICES; i++) {
        if (pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED && pmodel->devices[i].class == class) {
            return (uint8_t)INDEX2ADDR(i);
        }
    }

    return previous;
}


uint8_t model_get_next_device_address(model_t *pmodel, uint8_t previous) {
    assert(pmodel != NULL);

    for (size_t i = ADDR2INDEX(previous + 1); i < MODEL_MAX_DEVICES; i++) {
        if (pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
            return (uint8_t)INDEX2ADDR(i);
        }
    }

    return previous;
}


uint8_t model_get_prev_device_address(model_t *pmodel, uint8_t next) {
    assert(pmodel != NULL);

    if (ADDR2INDEX((int)next - 1) >= 0) {
        for (int i = ADDR2INDEX(next - 1); i >= 0; i--) {
            if (pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
                return (uint8_t)INDEX2ADDR(i);
            }
        }
    }

    return next;
}

int model_new_device(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    ASSERT_ADDRESS(address);
    size_t index = ADDR2INDEX(address);

    if (pmodel->devices[index].status != DEVICE_STATUS_NOT_CONFIGURED) {
        return -1;
    }

    pmodel->devices[index].status = DEVICE_STATUS_CONFIGURED;
    return 0;
}


address_map_t model_get_address_map(model_t *pmodel) {
    assert(pmodel != NULL);
    address_map_t map = {0};

    for (size_t i = 0; i < MODEL_MAX_DEVICES; i++) {
        if (pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
            address_map_set_bit(&map, INDEX2ADDR(i));
        }
    }

    return map;
}


size_t model_get_configured_devices(model_t *pmodel) {
    assert(pmodel != NULL);
    size_t count = 0;

    for (size_t i = 0; i < MODEL_MAX_DEVICES; i++) {
        if (pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
            count++;
        }
    }

    return count;
}


void model_delete_device(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    ASSERT_ADDRESS(address);
    size_t index                  = ADDR2INDEX(address);
    pmodel->devices[index].status = DEVICE_STATUS_NOT_CONFIGURED;
}


void model_get_device(model_t *pmodel, device_t *device, uint8_t address) {
    assert(pmodel != NULL);
    ASSERT_ADDRESS(address);
    size_t index = ADDR2INDEX(address);
    *device      = pmodel->devices[index];
}


void model_set_device_error(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    ASSERT_ADDRESS(address);
    size_t index                  = ADDR2INDEX(address);
    pmodel->devices[index].status = DEVICE_STATUS_COMMUNICATION_ERROR;
}


void model_set_device_sn(model_t *pmodel, uint8_t address, uint16_t serial_number) {
    assert(pmodel != NULL);
    ASSERT_ADDRESS(address);
    size_t index                         = ADDR2INDEX(address);
    pmodel->devices[index].status        = DEVICE_STATUS_OK;
    pmodel->devices[index].serial_number = serial_number;
}


void model_set_device_class(model_t *pmodel, uint8_t address, uint8_t class) {
    assert(pmodel != NULL);
    ASSERT_ADDRESS(address);
    size_t index                  = ADDR2INDEX(address);
    pmodel->devices[index].status = DEVICE_STATUS_OK;
    pmodel->devices[index].class  = class;
}


int model_get_light_class(model_t *pmodel, device_class_t *class) {
    assert(pmodel != NULL);
    int found = 0;

    for (size_t i = 0; i < MODEL_MAX_DEVICES; i++) {
        if (pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
            if (pmodel->devices[i].class == DEVICE_CLASS_LIGHT_1 || pmodel->devices[i].class == DEVICE_CLASS_LIGHT_2 ||
                pmodel->devices[i].class == DEVICE_CLASS_LIGHT_3) {
                if (!found) {
                    found  = 1;
                    *class = pmodel->devices[i].class;
                } else if (pmodel->devices[i].class > *class) {
                    *class = pmodel->devices[i].class;
                }
            }
        }

        if (*class == DEVICE_CLASS_LIGHT_3) {
            break;
        }
    }

    return found;
}


size_t model_get_class_count(model_t *pmodel, device_class_t class) {
    assert(pmodel != NULL);
    size_t count = 0;

    for (size_t i = 0; i < MODEL_MAX_DEVICES; i++) {
        if (pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
            if (pmodel->devices[i].class == class) {
                count++;
            }
        }
    }

    return count;
}



int model_all_devices_queried(model_t *pmodel) {
    assert(pmodel != NULL);
    for (size_t i = 0; i < MODEL_MAX_DEVICES; i++) {
        if (pmodel->devices[i].status == DEVICE_STATUS_CONFIGURED) {
            return 0;
        }
    }

    return 1;
}


model_fan_state_t model_get_fan_state(model_t *pmodel) {
    assert(pmodel != NULL);
    return pmodel->state;
}


void model_set_filter_state(model_t *pmodel, model_fan_state_t state) {
    assert(pmodel != NULL);
    pmodel->state = state;
}


int model_get_electrostatic_filter_state(model_t *pmodel) {
    assert(pmodel != NULL);
    return pmodel->electrostatic_filter_on;
}


void model_electrostatic_filter_toggle(model_t *pmodel) {
    assert(pmodel != NULL);
    pmodel->electrostatic_filter_on = !pmodel->electrostatic_filter_on;
}


int model_get_uvc_filter_state(model_t *pmodel) {
    assert(pmodel != NULL);
    return pmodel->uvc_filter_on;
}


void model_uvc_filter_toggle(model_t *pmodel) {
    assert(pmodel != NULL);
    pmodel->uvc_filter_on = !pmodel->uvc_filter_on;
}


void model_uvc_filter_on(model_t *pmodel) {
    assert(pmodel != NULL);
    pmodel->uvc_filter_on = 1;
}