#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "easyconnect_interface.h"
#include "model.h"
#include <esp_log.h>


#define ABS(x) ((x) > 0 ? (x) : -(x))



static uint8_t alarm_for_device(model_t *pmodel, size_t i);


static const char *TAG = "Model";


void model_init(model_t *pmodel) {
    assert(pmodel != NULL);

    pmodel->configuration.wifi_enabled = 0;

    memset(pmodel, 0, sizeof(model_t));
    pmodel->temperature           = 0;
    pmodel->internal_sensor_error = 0;
    pmodel->internal_rtc_error    = 0;
    device_list_init(pmodel->devices);
    pmodel->configuration.language                           = 0;
    pmodel->fan_speed                                        = 0;
    pmodel->light_state                                      = LIGHT_STATE_OFF;
    pmodel->configuration.active_backlight                   = 100;
    pmodel->configuration.buzzer_volume                      = MAX_BUZZER_VOLUME;
    pmodel->configuration.use_fahrenheit                     = 0;
    pmodel->configuration.environment_cleaning_start_period  = 15;
    pmodel->configuration.environment_cleaning_finish_period = 15;

    pmodel->configuration.pressure_differences_for_speed[0] = 0;
    pmodel->configuration.pressure_differences_for_speed[1] = 1;
    pmodel->configuration.pressure_differences_for_speed[2] = 2;
    pmodel->configuration.pressure_differences_for_speed[3] = 3;
    pmodel->configuration.pressure_differences_for_speed[4] = 4;

    pmodel->configuration.pressure_difference_deviation_warn = 30;
    pmodel->configuration.pressure_difference_deviation_stop = 50;

    pmodel->configuration.siphoning_percentages[0] = 10;
    pmodel->configuration.siphoning_percentages[1] = 25;
    pmodel->configuration.siphoning_percentages[2] = 35;
    pmodel->configuration.siphoning_percentages[3] = 60;
    pmodel->configuration.siphoning_percentages[4] = 95;

    pmodel->configuration.immission_percentages[0] = 5;
    pmodel->configuration.immission_percentages[1] = 15;
    pmodel->configuration.immission_percentages[2] = 20;
    pmodel->configuration.immission_percentages[3] = 35;
    pmodel->configuration.immission_percentages[4] = 55;

    for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
        pmodel->configuration.uvc_filters_for_speed[i] = 1;
        pmodel->configuration.esf_filters_for_speed[i] = 1;
    }

    pmodel->configuration.pressure_threshold_mb = 0;

    pmodel->configuration.uvc_filters_hours_warning_threshold = 8000;
    pmodel->configuration.uvc_filters_hours_stop_threshold    = 10000;

    pmodel->configuration.first_temperature_delta        = 25;
    pmodel->configuration.second_temperature_delta       = 35;
    pmodel->configuration.first_temperature_speed_raise  = 10;
    pmodel->configuration.second_temperature_speed_raise = 20;
    pmodel->configuration.temperature_warn               = 70;
    pmodel->configuration.temperature_stop               = 80;

    pmodel->configuration.first_humidity_delta        = 25;
    pmodel->configuration.second_humidity_delta       = 35;
    pmodel->configuration.first_humidity_speed_raise  = 10;
    pmodel->configuration.second_humidity_speed_raise = 20;
    pmodel->configuration.humidity_warn               = 95;
    pmodel->configuration.humidity_stop               = 100;
    pmodel->configuration.pressure_offsets[0]         = 0;
    pmodel->configuration.pressure_offsets[1]         = 0;
    pmodel->configuration.pressure_offsets[2]         = 0;

    pmodel->configuration.serial_number = 0;

    pmodel->show_work_hours_state = 0;

    pmodel->system_alarm = SYSTEM_ALARM_NONE;
    pmodel->sensors_read = 0;
    pmodel->ap_list_size = 0;
    pmodel->scanning     = 0;
    pmodel->wifi_state   = WIFI_STATE_DISCONNECTED;

    pmodel->logs_num  = 0;
    pmodel->logs_from = 0;
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
        default:
            return 0;
    }
}


uint8_t model_get_filter_device_stop(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    return model_get_filter_device_remaining_hours(pmodel, address) == 0;
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


uint16_t model_get_fan_percentage_correction(model_t *pmodel) {
    assert(pmodel != NULL);

    sensor_group_report_t ths[DEVICE_GROUPS] = {0};

    uint8_t any = 0;

    any = model_get_temperatures_humidities(pmodel, &ths[0], &ths[1]) || any;

    if (!any || ths[0].valid == 0 || ths[1].valid == 0) {
        return 0;
    }

    uint16_t correction = 0;

    if (ABS(ths[DEVICE_GROUP_1].temperature - ths[DEVICE_GROUP_2].temperature) >
        model_get_second_temperature_delta(pmodel)) {
        correction += model_get_second_temperature_speed_raise(pmodel);
    } else if (ABS(ths[DEVICE_GROUP_1].temperature - ths[DEVICE_GROUP_2].temperature) >
               model_get_first_temperature_delta(pmodel)) {
        correction += model_get_first_temperature_speed_raise(pmodel);
    }

    if (ths[DEVICE_GROUP_1].humidity > model_get_second_humidity_delta(pmodel)) {
        correction += model_get_second_humidity_speed_raise(pmodel);
    } else if (ths[DEVICE_GROUP_1].humidity > model_get_first_humidity_delta(pmodel)) {
        correction += model_get_first_humidity_speed_raise(pmodel);
    }

    return correction;
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
    return device_list_get_next_device_address_by_classes(pmodel->devices, previous, &class, 1);
}


uint8_t model_get_next_device_address_by_classes(model_t *pmodel, uint8_t previous, uint16_t *classes, size_t num) {
    assert(pmodel != NULL);
    return device_list_get_next_device_address_by_classes(pmodel->devices, previous, classes, num);
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
    uint16_t modes[] = {
        DEVICE_MODE_PRESSURE,
        DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY,
    };
    return model_get_next_device_address_by_modes(pmodel, previous, modes, sizeof(modes) / sizeof(modes[0]));
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


void model_delete_all_devices(model_t *pmodel) {
    assert(pmodel != NULL);

    for (size_t i = 1; i < MODBUS_MAX_DEVICES; i++) {
        device_list_delete_device(pmodel->devices, i);
    }
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


int model_get_max_group_per_mode(model_t *pmodel, uint16_t mode) {
    uint8_t previous = 0;
    uint8_t address  = previous;
    int     max      = -1;

    do {
        previous = address;
        address  = model_get_next_device_address_by_modes(pmodel, previous, (uint16_t *)&mode, 1);

        if (address != previous) {
            if (CLASS_GET_GROUP(model_get_device(pmodel, address).class) > max) {
                max = CLASS_GET_GROUP(model_get_device(pmodel, address).class);
            }
        }
    } while (previous != address);

    return max;
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

    if (device->status == DEVICE_STATUS_OK) {
        if (model_is_device_sensor(pmodel, address)) {
            device->sensor_data.state = state;
        } else {
            device->actuator_data.output_state = state;
        }
    }
}


uint8_t model_is_device_sensor(model_t *pmodel, uint8_t address) {
    assert(pmodel != NULL);
    uint16_t mode = CLASS_GET_MODE(model_get_device(pmodel, address).class);
    return mode == DEVICE_MODE_PRESSURE || mode == DEVICE_MODE_TEMPERATURE_HUMIDITY ||
           mode == DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY;
}


void model_set_sensors_values(model_t *pmodel, uint8_t address, int16_t pressure, int16_t temperature,
                              int16_t humidity) {
    assert(pmodel != NULL);

    device_t *device = model_get_device_mut(pmodel, address);
    uint16_t  mode   = CLASS_GET_MODE(device->class);
    if (device->status == DEVICE_STATUS_OK &&
        (mode == DEVICE_MODE_PRESSURE || mode == DEVICE_MODE_TEMPERATURE_HUMIDITY ||
         mode == DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY)) {
        device->sensor_data.pressure    = pressure;
        device->sensor_data.temperature = temperature;
        device->sensor_data.humidity    = humidity;
    }
}


void model_set_pressure_offset(model_t *pmodel, int group, int16_t offset) {
    assert(pmodel != NULL);

    pmodel->configuration.pressure_offsets[group] = offset;
}


int16_t model_get_pressure_offset(model_t *pmodel, int group) {
    assert(pmodel != NULL);

    return pmodel->configuration.pressure_offsets[group];
}


int model_get_pressures(model_t *pmodel, sensor_group_report_t *pressures) {
    int res = model_get_raw_pressures(pmodel, pressures);
    for (size_t i = 0; i < DEVICE_GROUPS; i++) {
        pressures[i].pressure += pmodel->configuration.pressure_offsets[i];
    }
    return res;
}


int16_t model_get_device_pressure(model_t *pmodel, device_t device) {
    return device.sensor_data.pressure + pmodel->configuration.pressure_offsets[CLASS_GET_GROUP(device.class)];
}


int model_calibrate_pressures(model_t *pmodel) {
    sensor_group_report_t pressures[DEVICE_GROUPS] = {0};
    int                   res                      = model_get_raw_pressures(pmodel, pressures);

    size_t  count          = 0;
    int64_t pressure_total = 0;

    for (size_t i = 0; i < DEVICE_GROUPS; i++) {
        if (pressures[i].valid) {
            ESP_LOGI(TAG, "Found pressure for group %zu: %i", i, pressures[i].pressure);
            pressure_total += pressures[i].pressure;
            count++;
        }
    }

    if (count == 0) {
        for (size_t i = 0; i < DEVICE_GROUPS; i++) {
            pmodel->configuration.pressure_offsets[i] = 0;
        }
    } else {
        int64_t pressure_average = pressure_total / count;

        ESP_LOGI(TAG, "Average: %lli", pressure_average);
        // Make sure every device reads the average pressure
        for (size_t i = 0; i < DEVICE_GROUPS; i++) {
            if (pressures[i].valid) {
                pmodel->configuration.pressure_offsets[i] = pressure_average - pressures[i].pressure;
                ESP_LOGI(TAG, "Offset for group %zu: %i", i, pmodel->configuration.pressure_offsets[i]);
            } else {
                pmodel->configuration.pressure_offsets[i] = 0;
            }
        }
    }

    return res;
}


int model_get_raw_pressures(model_t *pmodel, sensor_group_report_t *pressures) {
    assert(pmodel != NULL);

    uint16_t const modes[] = {DEVICE_MODE_PRESSURE, DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY};

    uint8_t previous      = 0;
    uint8_t address       = previous;
    int     highest_group = -1;

    int64_t  pressure_sums[DEVICE_GROUPS] = {0};
    uint16_t group_counts[DEVICE_GROUPS]  = {0};
    uint16_t group_errors[DEVICE_GROUPS]  = {0};

    do {
        previous = address;
        address  = model_get_next_device_address_by_modes(pmodel, previous, (uint16_t *)modes,
                                                          sizeof(modes) / sizeof(modes[0]));

        if (previous == address) {
            break;
        }

        device_t device = model_get_device(pmodel, address);
        uint16_t group  = CLASS_GET_GROUP(device.class);
        if (group > highest_group) {
            highest_group = CLASS_GET_GROUP(device.class);
        }

        if (device.status == DEVICE_STATUS_OK && (device.sensor_data.state & 0x02) == 0) {
            group_counts[group]++;
            pressure_sums[group] += device.sensor_data.pressure;
        } else {
            group_errors[group]++;
        }
    } while (previous != address);

    for (size_t i = 0; i < DEVICE_GROUPS; i++) {
        pressures[i].errors = group_errors[i];

        if (group_counts[i] != 0) {
            pressures[i].pressure = (int16_t)(pressure_sums[i] / group_counts[i]);
            pressures[i].valid    = 1;
        } else {
            pressures[i].pressure = 0;
            pressures[i].valid    = 0;
        }
    }

    return highest_group;
}


int model_get_temperatures_humidities(model_t *pmodel, sensor_group_report_t *group_1, sensor_group_report_t *group_2) {
    assert(pmodel != NULL);

    uint16_t const modes[] = {DEVICE_MODE_TEMPERATURE_HUMIDITY, DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY};

    uint8_t previous = 0;
    uint8_t address  = previous;

    int64_t  temperatures[3] = {0};
    int64_t  humidities[3]   = {0};
    uint16_t group_counts[3] = {0};
    uint16_t group_errors[3] = {0};

    do {
        previous = address;
        address  = model_get_next_device_address_by_modes(pmodel, previous, (uint16_t *)modes,
                                                          sizeof(modes) / sizeof(modes[0]));

        if (previous == address) {
            break;
        }

        device_t device = model_get_device(pmodel, address);
        uint16_t group  = CLASS_GET_GROUP(device.class);

        if (device.status == DEVICE_STATUS_OK && (device.sensor_data.state & 0x01) == 0) {
            group_counts[group]++;
            temperatures[group] += device.sensor_data.temperature;
            humidities[group] += device.sensor_data.humidity;
        } else {
            group_errors[group]++;
        }
    } while (previous != address);

    // There are at least two external sensor groups, pick from them
    if (group_counts[0] + group_errors[0] > 0 && group_counts[1] + group_errors[1] > 0) {
        group_1->errors = group_errors[0];
        group_2->errors = group_errors[1];

        if (group_counts[0] > 0) {
            group_1->valid       = 1;
            group_1->temperature = (int16_t)(temperatures[0] / group_counts[0]);
            group_1->humidity    = (int16_t)(humidities[0] / group_counts[0]);
        } else {
            group_1->valid       = 0;
            group_1->temperature = 0;
            group_1->humidity    = 0;
        }

        if (group_counts[1] > 0) {
            group_2->valid       = 1;
            group_2->temperature = (int16_t)(temperatures[1] / group_counts[1]);
            group_2->humidity    = (int16_t)(humidities[1] / group_counts[1]);
        } else {
            group_2->valid       = 0;
            group_2->temperature = 0;
            group_2->humidity    = 0;
        }
    } else {     // There is at most one external sensor group, use the internal sensor as first temperature
        group_1->temperature = model_get_temperature(pmodel);
        group_1->humidity    = model_get_humidity(pmodel);

        if (pmodel->internal_sensor_error) {
            group_1->valid  = 0;
            group_1->errors = 1;
        } else {
            group_1->valid  = 1;
            group_1->errors = 0;
        }

        if (group_counts[0] > 0) {
            group_2->valid       = 1;
            group_2->temperature = (int16_t)(temperatures[0] / group_counts[0]);
            group_2->humidity    = (int16_t)(humidities[0] / group_counts[0]);
            group_2->errors      = group_errors[0];
        } else if (group_counts[1] > 0) {
            group_2->valid       = 1;
            group_2->temperature = (int16_t)(temperatures[1] / group_counts[1]);
            group_2->humidity    = (int16_t)(humidities[1] / group_counts[1]);
            group_2->errors      = group_errors[1];
        } else {
            group_2->valid       = 0;
            group_2->temperature = 0;
            group_2->humidity    = 0;
        }
    }

    return group_counts[0] > 0 || group_counts[1] > 0;
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


uint8_t model_is_system_locked(model_t *pmodel) {
    return pmodel->system_alarm == SYSTEM_ALARM_TRIGGERED;
}


uint8_t model_is_any_fatal_alarm(model_t *pmodel) {
    return pmodel->system_alarm != SYSTEM_ALARM_OVERRULED && device_list_is_system_alarm(pmodel->devices);
}


const char *model_get_ssid(model_t *pmodel) {
    assert(pmodel != NULL);
    return pmodel->ssid;
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


uint8_t model_is_pressure_difference_ok(model_t *pmodel, int16_t p1, int16_t p2) {
    int16_t maximum_allowed_difference = (ABS(p1) * model_get_pressure_difference_deviation_warn(pmodel)) / 100;
    return ABS(p1 - p2) < maximum_allowed_difference;
}


size_t model_get_temperature_difference_level(model_t *pmodel, int16_t t1, int16_t t2) {
    if (ABS(t1 - t2) < model_get_first_temperature_delta(pmodel)) {
        return 0;
    } else if (ABS(t1 - t2) < model_get_second_temperature_delta(pmodel)) {
        return 1;
    } else {
        return 2;
    }
}


size_t model_get_humidity_difference_level(model_t *pmodel, int16_t h1, int16_t h2) {
    if (ABS(h1 - h2) < model_get_first_humidity_delta(pmodel)) {
        return 0;
    } else if (ABS(h1 - h2) < model_get_second_humidity_delta(pmodel)) {
        return 1;
    } else {
        return 2;
    }
}


size_t model_get_local_temperature_humidity_error_level(model_t *pmodel) {
    if (model_get_temperature(pmodel) > model_get_temperature_stop(pmodel) ||
        model_get_humidity(pmodel) > model_get_humidity_stop(pmodel)) {
        return 2;
    } else if (model_get_humidity(pmodel) > model_get_humidity_warn(pmodel) ||
               model_get_temperature(pmodel) > model_get_temperature_warn(pmodel)) {
        return 1;
    } else {
        return 0;
    }
}


static uint8_t alarm_for_device(model_t *pmodel, size_t i) {
    return ((pmodel->devices[i].status != DEVICE_STATUS_NOT_CONFIGURED && (pmodel->devices[i].alarms & 0xFF) > 0) ||
            pmodel->devices[i].status == DEVICE_STATUS_COMMUNICATION_ERROR);
}
