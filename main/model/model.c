#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "model.h"
#include "easyconnect_interface.h"


static uint8_t alarm_for_device(model_t *pmodel, size_t i);


void model_init(model_t *pmodel) {
    assert(pmodel != NULL);

    memset(pmodel, 0, sizeof(model_t));
    pmodel->temperature = 32;
    device_list_init(pmodel->devices);
    pmodel->configuration.language                           = 0;
    pmodel->fan_speed                                        = 0;
    pmodel->light_state                                      = LIGHT_STATE_OFF;
    pmodel->configuration.active_backlight                   = 100;
    pmodel->configuration.buzzer_volume                      = MAX_BUZZER_VOLUME;
    pmodel->configuration.use_fahrenheit                     = 0;
    pmodel->configuration.environment_cleaning_start_period  = 4;
    pmodel->configuration.environment_cleaning_finish_period = 4;

    pmodel->configuration.pressure_differences_for_speed[0] = 0;
    pmodel->configuration.pressure_differences_for_speed[1] = 1;
    pmodel->configuration.pressure_differences_for_speed[2] = 2;
    pmodel->configuration.pressure_differences_for_speed[3] = 3;
    pmodel->configuration.pressure_differences_for_speed[4] = 4;

    pmodel->configuration.pressure_difference_deviation_warn = 30;
    pmodel->configuration.pressure_difference_deviation_stop = 50;

    pmodel->configuration.siphoning_percentages[0] = 0;
    pmodel->configuration.siphoning_percentages[1] = 20;
    pmodel->configuration.siphoning_percentages[2] = 30;
    pmodel->configuration.siphoning_percentages[3] = 80;
    pmodel->configuration.siphoning_percentages[4] = 100;

    pmodel->configuration.immission_percentages[0] = 0;
    pmodel->configuration.immission_percentages[1] = 5;
    pmodel->configuration.immission_percentages[2] = 20;
    pmodel->configuration.immission_percentages[3] = 60;
    pmodel->configuration.immission_percentages[4] = 80;

    for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
        pmodel->configuration.uvc_filters_for_speed[i] = 1;
        pmodel->configuration.esf_filters_for_speed[i] = 1;
    }

    pmodel->configuration.pressure_threshold_mb = 0;

    pmodel->configuration.passive_filters_hours_warning_threshold = 1000;
    pmodel->configuration.passive_filters_hours_stop_threshold    = 2000;
    pmodel->configuration.uvc_filters_hours_warning_threshold     = 8000;
    pmodel->configuration.uvc_filters_hours_stop_threshold        = 10000;
    pmodel->configuration.esf_filters_hours_warning_threshold     = 1000;
    pmodel->configuration.esf_filters_hours_stop_threshold        = 2000;

    pmodel->configuration.first_temperature_delta        = 25;
    pmodel->configuration.second_temperature_delta       = 35;
    pmodel->configuration.first_temperature_speed_raise  = 10;
    pmodel->configuration.second_temperature_speed_raise = 20;
    pmodel->configuration.temperature_warn               = 70;
    pmodel->configuration.temperature_stop               = 80;

    pmodel->configuration.first_humidity_delta        = 10;
    pmodel->configuration.second_humidity_delta       = 20;
    pmodel->configuration.first_humidity_speed_raise  = 10;
    pmodel->configuration.second_humidity_speed_raise = 20;
    pmodel->configuration.humidity_warn               = 95;
    pmodel->configuration.humidity_stop               = 100;

    pmodel->stats.passive_filters_work_seconds = 0;

    pmodel->show_work_hours_state = 0;
}


void model_reset_passive_filters_work_hours(model_t *pmodel) {
    assert(pmodel != NULL);
    pmodel->stats.passive_filters_work_seconds = 0;
}


uint8_t model_add_passive_filters_work_seconds(model_t *pmodel, uint32_t seconds) {
    assert(pmodel != NULL);
    uint8_t previous_warn = model_get_passive_filter_warning(pmodel);
    uint8_t previous_stop = model_get_passive_filter_stop(pmodel);
    pmodel->stats.passive_filters_work_seconds += seconds;
    uint8_t next_warn = model_get_passive_filter_warning(pmodel);
    uint8_t next_stop = model_get_passive_filter_stop(pmodel);

    return (next_warn || next_stop) && (previous_warn != next_warn || previous_stop != next_stop);
}


uint16_t model_get_passive_filters_work_hours(model_t *pmodel) {
    assert(pmodel != NULL);
    return pmodel->stats.passive_filters_work_seconds / (60UL * 60UL);
}


uint16_t model_get_passive_filters_remaining_hours(model_t *pmodel) {
    assert(pmodel != NULL);
    if (model_get_passive_filters_work_hours(pmodel) < pmodel->configuration.passive_filters_hours_stop_threshold) {
        return pmodel->configuration.passive_filters_hours_stop_threshold -
               model_get_passive_filters_work_hours(pmodel);
    } else {
        return 0;
    }
}


uint16_t model_get_filter_device_work_hours(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    device_t device = device_list_get_device(pmodel->devices, address);
    switch (CLASS_GET_MODE(device.class)) {
        case DEVICE_MODE_UVC:
        case DEVICE_MODE_ESF:
            return device.actuator_data.work_hours;
        default:
            return 0;
    }
}


uint16_t model_get_filter_device_remaining_hours(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    device_t device = device_list_get_device(pmodel->devices, address);
    switch (CLASS_GET_MODE(device.class)) {
        case DEVICE_MODE_UVC:
            if (device.actuator_data.work_hours > pmodel->configuration.uvc_filters_hours_stop_threshold) {
                return 0;
            } else {
                return pmodel->configuration.uvc_filters_hours_stop_threshold - device.actuator_data.work_hours;
            }
        case DEVICE_MODE_ESF:
            if (device.actuator_data.work_hours > pmodel->configuration.esf_filters_hours_stop_threshold) {
                return 0;
            } else {
                return pmodel->configuration.esf_filters_hours_stop_threshold - device.actuator_data.work_hours;
            }
        default:
            return 0;
    }
}


uint8_t model_get_filter_device_warning(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    device_t device = device_list_get_device(pmodel->devices, address);
    switch (CLASS_GET_MODE(device.class)) {
        case DEVICE_MODE_UVC:
            return device.actuator_data.work_hours > pmodel->configuration.uvc_filters_hours_warning_threshold;
        case DEVICE_MODE_ESF:
            return device.actuator_data.work_hours > pmodel->configuration.esf_filters_hours_warning_threshold;
        default:
            return 0;
    }
}


uint8_t model_get_filter_device_stop(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    return model_get_filter_device_remaining_hours(pmodel, address) == 0;
}


uint8_t model_get_passive_filter_warning(model_t *pmodel) {
    assert(pmodel != NULL);
    return model_get_passive_filters_work_hours(pmodel) > pmodel->configuration.passive_filters_hours_warning_threshold;
}


uint8_t model_get_passive_filter_stop(model_t *pmodel) {
    assert(pmodel != NULL);
    return model_get_passive_filters_remaining_hours(pmodel) == 0;
}


uint16_t model_get_pressure_difference(model_t *pmodel, uint8_t fan_speed) {
    assert(pmodel != NULL);
    assert(fan_speed < MAX_FAN_SPEED);
    return pmodel->configuration.pressure_differences_for_speed[fan_speed];
}


void model_set_pressure_difference(model_t *pmodel, uint8_t fan_speed, uint16_t difference) {
    assert(pmodel != NULL);
    assert(fan_speed < MAX_FAN_SPEED);
    pmodel->configuration.pressure_differences_for_speed[fan_speed] = difference;
}


uint8_t model_get_siphoning_percentage(model_t *pmodel, uint8_t fan_speed) {
    assert(pmodel != NULL);
    assert(fan_speed < MAX_FAN_SPEED);
    return pmodel->configuration.siphoning_percentages[fan_speed];
}


void model_set_siphoning_percentage(model_t *pmodel, uint8_t fan_speed, uint8_t percentage) {
    assert(pmodel != NULL);
    assert(fan_speed < MAX_FAN_SPEED);
    pmodel->configuration.siphoning_percentages[fan_speed] = percentage;
}


uint8_t model_get_immission_percentage(model_t *pmodel, uint8_t fan_speed) {
    assert(pmodel != NULL);
    assert(fan_speed < MAX_FAN_SPEED);
    return pmodel->configuration.immission_percentages[fan_speed];
}


void model_set_immission_percentage(model_t *pmodel, uint8_t fan_speed, uint8_t percentage) {
    assert(pmodel != NULL);
    assert(fan_speed < MAX_FAN_SPEED);
    pmodel->configuration.immission_percentages[fan_speed] = percentage;
}


uint8_t model_get_uvc_filters_for_speed(model_t *pmodel, uint8_t fan_speed) {
    assert(pmodel != NULL);
    assert(fan_speed < MAX_FAN_SPEED);
    return pmodel->configuration.uvc_filters_for_speed[fan_speed];
}


void model_set_uvc_filters_for_speed(model_t *pmodel, uint8_t fan_speed, uint8_t filters) {
    assert(pmodel != NULL);
    assert(fan_speed < MAX_FAN_SPEED);
    pmodel->configuration.uvc_filters_for_speed[fan_speed] = filters;
}


uint8_t model_get_esf_filters_for_speed(model_t *pmodel, uint8_t fan_speed) {
    assert(pmodel != NULL);
    assert(fan_speed < MAX_FAN_SPEED);
    return 1;
    // return pmodel->configuration.esf_filters_for_speed[fan_speed];
}


void model_set_esf_filters_for_speed(model_t *pmodel, uint8_t fan_speed, uint8_t filters) {
    assert(pmodel != NULL);
    assert(fan_speed < MAX_FAN_SPEED);
    pmodel->configuration.esf_filters_for_speed[fan_speed] = filters;
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


uint8_t model_get_next_device_address_by_mode(model_t *pmodel, uint8_t previous, uint16_t mode) {
    assert(pmodel != NULL);
    return device_list_get_next_device_address_by_modes(pmodel->devices, previous, &mode, 1);
}


uint8_t model_get_next_device_address_by_modes(model_t *pmodel, uint8_t previous, uint16_t *modes, size_t num) {
    assert(pmodel != NULL);
    return device_list_get_next_device_address_by_modes(pmodel->devices, previous, modes, num);
}


uint8_t model_get_next_pressure_sensor_device(model_t *pmodel, uint8_t previous) {
    assert(pmodel != NULL);

    for (size_t i = ADDR2INDEX(previous + 1); i < MODBUS_MAX_DEVICES; i++) {
        if (pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED &&
            (pmodel->devices[i].class == DEVICE_CLASS_PRESSURE_SAFETY ||
             pmodel->devices[i].class == DEVICE_CLASS_PRESSURE_TEMPERATURE_HUMIDITY_SAFETY)) {
            return (uint8_t)INDEX2ADDR(i);
        }
    }

    return previous;
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


void model_set_device_sn(model_t *pmodel, uint8_t address, uint32_t serial_number) {
    assert(pmodel != NULL);
    device_t *device = device_list_get_device_mut(pmodel->devices, address);
    if (device->status != DEVICE_STATUS_NOT_CONFIGURED) {
        device->serial_number = serial_number;
    }
}


uint8_t model_set_filter_work_hours(model_t *pmodel, uint8_t address, uint16_t work_hours) {
    assert(pmodel != NULL);
    device_t *device = device_list_get_device_mut(pmodel->devices, address);

    switch (CLASS_GET_MODE(device->class)) {
        case DEVICE_MODE_UVC:
        case DEVICE_MODE_ESF: {
            uint8_t previous_warn = model_get_filter_device_warning(pmodel, address);
            uint8_t previous_stop = model_get_filter_device_stop(pmodel, address);
            if (device->status != DEVICE_STATUS_NOT_CONFIGURED) {
                device->actuator_data.work_hours = work_hours;
            }
            uint8_t next_warn = model_get_filter_device_warning(pmodel, address);
            uint8_t next_stop = model_get_filter_device_stop(pmodel, address);
            // returns 1 if an alarm was activated
            return (next_warn || next_stop) && (previous_warn != next_warn || previous_stop != next_stop);
        }
        default:
            break;
    }
    return 0;
}


uint8_t model_get_problematic_filter_device(model_t *pmodel, uint8_t previous) {
    uint16_t const modes[] = {DEVICE_MODE_ESF, DEVICE_MODE_UVC};

    uint8_t original = previous;
    uint8_t address  = previous;
    uint8_t warn     = 0;
    uint8_t stop     = 0;

    do {
        previous = address;
        address  = model_get_next_device_address_by_modes(pmodel, previous, (uint16_t *)modes,
                                                          sizeof(modes) / sizeof(modes[0]));
        warn     = model_get_filter_device_warning(pmodel, address);
        stop     = model_get_filter_device_stop(pmodel, address);
    } while (previous != address && warn == 0 && stop == 0);

    return previous == address ? original : address;
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


void model_set_device_state(model_t *pmodel, uint8_t address, uint16_t state) {
    assert(pmodel != NULL);
    device_t *device = model_get_device_mut(pmodel, address);

    if (device->status == DEVICE_STATUS_OK && (CLASS_GET_MODE(device->class) != DEVICE_MODE_SENSOR)) {
        device->actuator_data.ourput_state = state;
    }
}


void model_set_device_pressure(model_t *pmodel, uint8_t address, uint16_t pressure) {
    assert(pmodel != NULL);

    device_t *device = model_get_device_mut(pmodel, address);
    if (device->status == DEVICE_STATUS_OK && (device->class == DEVICE_CLASS_PRESSURE_SAFETY ||
                                               device->class == DEVICE_CLASS_PRESSURE_TEMPERATURE_HUMIDITY_SAFETY)) {
        device->sensor_data.pressure = pressure;
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


size_t model_get_mode_count(model_t *pmodel, uint16_t mode) {
    assert(pmodel != NULL);
    size_t count = 0;

    for (size_t i = 0; i < MODBUS_MAX_DEVICES; i++) {
        if (pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED) {
            if (CLASS_GET_MODE(pmodel->devices[i].class) == mode) {
                count++;
            }
        }
    }

    return count;
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


void model_set_fan_state(model_t *pmodel, model_fan_state_t state) {
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


void model_get_alarms_for_classes(model_t *pmodel, uint8_t *uvc, uint8_t *esf, uint8_t *siph, uint8_t *imm) {
    for (size_t i = 0; i < MODBUS_MAX_DEVICES; i++) {
        switch (pmodel->devices[i].class) {
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1):
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2):
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3):
                if (alarm_for_device(pmodel, i)) {
                    *uvc = 1;
                }
                break;

            case DEVICE_CLASS_ELECTROSTATIC_FILTER:
                if (alarm_for_device(pmodel, i)) {
                    *esf = 1;
                }
                break;

            case DEVICE_CLASS_SIPHONING_FAN:
                if (alarm_for_device(pmodel, i)) {
                    *siph = 1;
                }
                break;

            case DEVICE_CLASS_IMMISSION_FAN:
                if (alarm_for_device(pmodel, i)) {
                    *imm = 1;
                }
                break;
        }
    }
}


uint8_t model_is_there_any_alarm_for_class(model_t *pmodel, uint16_t class) {
    assert(pmodel != NULL);
    return device_list_is_there_any_alarm_for_class(pmodel->devices, class) ||
           device_list_is_class_communication_error(pmodel->devices, class);
}


uint8_t model_is_class_alarms_on(model_t *pmodel, uint16_t class, uint8_t alarms) {
    assert(pmodel != NULL);
    return device_list_is_class_alarms_on(pmodel->devices, class, alarms);
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


void model_light_switch(model_t *model) {
    uint16_t class;
    if (!model_get_light_class(model, &class)) {
        return;
    }

    switch (class) {
        case DEVICE_CLASS_LIGHT_1:
            if (model_get_light_state(model) == LIGHT_STATE_OFF) {
                model_set_light_state(model, LIGHT_STATE_SINGLE_ON);
            } else {
                model_set_light_state(model, LIGHT_STATE_OFF);
            }
            break;

        case DEVICE_CLASS_LIGHT_2: {
            model_set_light_state(model, (model_get_light_state(model) + 1) % (LIGHT_STATE_DOUBLE_ON + 1));
            break;
        }

        case DEVICE_CLASS_LIGHT_3: {
            model_set_light_state(model, (model_get_light_state(model) + 1) % (LIGHT_STATE_TRIPLE_ON + 1));
            break;
        }

        default:
            assert(0);
    }
}


static uint8_t alarm_for_device(model_t *pmodel, size_t i) {
    return ((pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED && (pmodel->devices[i].alarms & 0xFF) > 0) ||
            pmodel->devices[i].status == DEVICE_STATUS_COMMUNICATION_ERROR);
}