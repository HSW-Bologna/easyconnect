#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "model.h"
#include "easyconnect_interface.h"


void model_init(model_t *pmodel) {
    assert(pmodel != NULL);

    memset(pmodel, 0, sizeof(model_t));
    pmodel->temperature = 32;
    device_list_init(pmodel->devices);
    pmodel->configuration.language                    = 0;
    pmodel->fan_speed                                 = 0;
    pmodel->configuration.active_backlight            = 100;
    pmodel->configuration.buzzer_volume               = MAX_BUZZER_VOLUME;
    pmodel->configuration.use_fahrenheit              = 0;
    pmodel->configuration.environment_cleaning_period = 4;
    pmodel->configuration.immission_percentage        = 100;

    for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
        pmodel->configuration.filters_for_speed[i] = 0;
    }
}


uint8_t model_get_filters_for_speed(model_t *pmodel, uint8_t speed) {
    assert(pmodel != NULL);
    assert(speed < MAX_FAN_SPEED);
    return pmodel->configuration.filters_for_speed[speed];
}


void model_modify_filters_for_speed(model_t *pmodel, uint8_t speed, int op) {
    assert(pmodel != NULL);
    if (op > 0 && pmodel->configuration.filters_for_speed[speed] < 2) {
        pmodel->configuration.filters_for_speed[speed]++;
    } else if (op < 0 && pmodel->configuration.filters_for_speed[speed] > 0) {
        pmodel->configuration.filters_for_speed[speed]--;
    }
}


uint8_t model_set_filters_for_speed(model_t *pmodel, uint8_t speed, uint8_t filters) {
    assert(pmodel != NULL);
    assert(speed < MAX_FAN_SPEED);
    return pmodel->configuration.filters_for_speed[speed];
}


const char *model_get_degrees_symbol(model_t *pmodel) {
    assert(pmodel != NULL);
    if (model_get_use_fahrenheit(pmodel)) {
        return "°F";
    } else {
        return "°C";
    }
}


int model_get_temperature(model_t *pmodel) {
    assert(pmodel != NULL);
    if (model_get_use_fahrenheit(pmodel)) {
        return pmodel->temperature * 9 / 5 + 32;
    } else {
        return pmodel->temperature;
    }
}


int model_is_address_configured(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    return device_list_is_address_configured(pmodel->devices, address);
}


uint8_t model_get_available_address(model_t *pmodel, uint8_t previous) {
    assert(pmodel != NULL);
    return device_list_get_available_address(pmodel->devices, previous);
}


uint8_t model_get_next_device_address_by_class(model_t *pmodel, uint8_t previous, uint16_t class) {
    assert(pmodel != NULL);
    return device_list_get_next_device_address_by_class(pmodel->devices, previous, class);
}


uint8_t model_get_next_device_address(model_t *pmodel, uint8_t previous) {
    assert(pmodel != NULL);
    return device_list_get_next_configured_device_address(pmodel->devices, previous);
}


uint8_t model_get_prev_device_address(model_t *pmodel, uint8_t next) {
    assert(pmodel != NULL);
    return device_list_get_prev_device_address(pmodel->devices, next);
}


int model_new_device(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    return device_list_configure_device(pmodel->devices, address);
}


address_map_t model_get_address_map(model_t *pmodel) {
    assert(pmodel != NULL);
    return device_list_get_address_map(pmodel->devices);
}


address_map_t model_get_error_map(model_t *pmodel) {
    assert(pmodel != NULL);
    return device_list_get_error_map(pmodel->devices);
}



size_t model_get_configured_devices(model_t *pmodel) {
    assert(pmodel != NULL);
    return device_list_get_configured_devices(pmodel->devices);
}


void model_delete_device(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    device_list_delete_device(pmodel->devices, address);
}


device_t model_get_device(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    return device_list_get_device(pmodel->devices, address);
}


device_t *model_get_device_mut(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    return device_list_get_device_mut(pmodel->devices, address);
}


uint8_t model_set_device_error(model_t *pmodel, uint8_t address, int error) {
    assert(pmodel != NULL);
    return device_list_set_device_error(pmodel->devices, address, error);
}


void model_set_device_sn(model_t *pmodel, uint8_t address, uint16_t serial_number) {
    assert(pmodel != NULL);
    device_t *device = device_list_get_device_mut(pmodel->devices, address);
    if (device->status != DEVICE_STATUS_NOT_CONFIGURED) {
        device->serial_number = serial_number;
    }
}


void model_set_device_class(model_t *pmodel, uint8_t address, uint16_t class) {
    assert(pmodel != NULL);
    device_t *device = device_list_get_device_mut(pmodel->devices, address);
    if (device->status != DEVICE_STATUS_NOT_CONFIGURED) {
        device->class = class;
    }
}


void model_set_device_firmware(model_t *pmodel, uint8_t address, uint16_t firmware_version) {
    assert(pmodel != NULL);
    device_t *device = device_list_get_device_mut(pmodel->devices, address);
    if (device->status != DEVICE_STATUS_NOT_CONFIGURED) {
        device->firmware_version = firmware_version;
    }
}


uint8_t model_set_device_alarms(model_t *pmodel, uint8_t address, uint16_t alarms) {
    assert(pmodel != NULL);
    return device_list_set_device_alarms(pmodel->devices, address, alarms);
}


void model_set_device_pressure(model_t *pmodel, uint8_t address, uint16_t pressure) {
    assert(pmodel != NULL);

    device_t *device = model_get_device_mut(pmodel, address);
    if (device->status == DEVICE_STATUS_OK && device->class == DEVICE_CLASS_PRESSURE_SAFETY) {
        device->pressure = pressure;
    }
}


int model_get_light_class(model_t *pmodel, uint16_t *class) {
    assert(pmodel != NULL);
    int found = 0;

    for (size_t i = 0; i < MODBUS_MAX_DEVICES; i++) {
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


size_t model_get_class_count(model_t *pmodel, uint16_t class) {
    assert(pmodel != NULL);
    size_t count = 0;

    for (size_t i = 0; i < MODBUS_MAX_DEVICES; i++) {
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
    for (size_t i = 0; i < MODBUS_MAX_DEVICES; i++) {
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


void model_electrostatic_filter_off(model_t *pmodel) {
    assert(pmodel != NULL);
    pmodel->electrostatic_filter_on = 0;
}


int model_get_uvc_filter_state(model_t *pmodel) {
    assert(pmodel != NULL);
    return pmodel->uvc_filter_on;
}


void model_uvc_filter_toggle(model_t *pmodel) {
    assert(pmodel != NULL);
    pmodel->uvc_filter_on = !pmodel->uvc_filter_on;
}


void model_uvc_filter_off(model_t *pmodel) {
    assert(pmodel != NULL);
    pmodel->uvc_filter_on = 0;
}


void model_uvc_filter_on(model_t *pmodel) {
    assert(pmodel != NULL);
    pmodel->uvc_filter_on = 1;
}


uint8_t model_is_there_an_alarm(model_t *pmodel) {
    assert(pmodel != NULL);
    return device_list_is_there_an_alarm(pmodel->devices);
}


uint8_t model_is_there_any_alarm_for_class(model_t *pmodel, uint16_t class) {
    assert(pmodel != NULL);
    return device_list_is_there_any_alarm_for_class(pmodel->devices, class);
}


uint8_t model_is_class_alarms_on(model_t *pmodel, uint16_t class, uint8_t alarms) {
    assert(pmodel != NULL);
    return device_list_is_class_alarms_on(pmodel->devices, class, alarms);
}


uint8_t model_is_any_filter_alarm_on(model_t *pmodel) {
    return model_is_filter_alarm_on(pmodel, 0xFF);
}


uint8_t model_is_filter_alarm_on(model_t *pmodel, uint8_t alarms) {
    return model_is_class_alarms_on(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER, alarms) ||
           model_is_class_alarms_on(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1), alarms) ||
           model_is_class_alarms_on(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2), alarms) ||
           model_is_class_alarms_on(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3), alarms);
}


uint8_t model_is_there_a_fan_alarm(model_t *pmodel) {
    return model_is_there_any_alarm_for_class(pmodel, DEVICE_CLASS_SIPHONING_FAN) ||
           model_is_there_any_alarm_for_class(pmodel, DEVICE_CLASS_IMMISSION_FAN);
}