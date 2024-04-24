#include <sys/time.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "controller.h"
#include "model/model.h"
#include "view/view.h"
#include "modbus.h"
#include "log.h"
#include "utils/utils.h"
#include "gel/timer/timecheck.h"
#include "configuration.h"
#include "state.h"
#include "peripherals/storage.h"
#include "esp_log.h"
#include "peripherals/i2c_devices.h"
#include "i2c_devices/rtc/DS1307/ds1307.h"
#include "temperature.h"
#include "peripherals/tft.h"
#include "peripherals/buzzer.h"
#include "services/network.h"
#include "services/server.h"
#include "view/view_types.h"
#include "services/device_commands.h"
#include "sensors_calibration.h"
#ifndef PC_SIMULATOR
#include "esp32_commandline.h"
#endif


static void     update_work_hours_cycle(model_t *pmodel);
static void     error_condition_on_device(model_t *pmodel, uint8_t address, uint8_t alarms, uint8_t communication);
static void     print_heap_status(void);
static void     check_sensors_levels(model_t *pmodel);
static void     console_task(void *args);
static uint32_t get_serial_number(void *arg);
static int      initialize_time(void);
static void     system_shutdown(model_t *pmodel);


static const char   *TAG                  = "Controller";
static uint8_t       refresh_devices      = 1;
static uint8_t       update_time          = 1;
static uint8_t       first_update_loop    = 1;
static uint8_t       scanning             = 0;
static uint8_t       refreshing           = 0;
static uint8_t       last_refresh_address = 0;
static QueueHandle_t queue                = NULL;


static void delay_ms(unsigned long ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}


static easyconnect_interface_t context = {
    .save_serial_number = configuration_save_serial_number,
    .get_serial_number  = get_serial_number,
    .delay_ms           = delay_ms,
};


void controller_init(model_t *pmodel) {
    queue = xQueueCreate(4, sizeof(view_controller_message_t));

    modbus_init();
    temperature_init();
    log_init();
    configuration_load(pmodel);
    view_start(pmodel);

    server_init();
    network_init();

    tft_backlight_set(model_get_active_backlight(pmodel));

    pmodel->internal_rtc_error = initialize_time();

    LOG_POWER_ON();

    if (model_get_wifi_enabled(pmodel) && model_get_wifi_configured(pmodel)) {
        network_start_sta();
    }

    context.arg = pmodel;
    xTaskCreate(console_task, "Console", 512 * 6, &context, 1, NULL);
}


void controller_manage_message(model_t *pmodel, view_controller_message_t *msg) {
    switch (msg->code) {
        case VIEW_CONTROLLER_MESSAGE_NOTHING:
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_MESSAGES:
            modbus_read_device_messages(msg->address, model_get_device(pmodel, msg->address).class);
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_SET_RTC: {
            struct tm tm;
            utils_get_sys_time(&tm);
            ds1307_set_time(rtc_driver, ds1307_rtc_from_tm(tm), 0);
            break;
        }

        case VIEW_CONTROLLER_MESSAGE_CODE_STOP_CURRENT_OPERATION: {
            modbus_stop_current_operation();
            break;
        }

        case VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_SCAN:
            if (!scanning) {
                modbus_scan();
                refresh_devices = 1;

                controller_state_event(pmodel, STATE_EVENT_FAN_STOP);
                model_uvc_filter_off(pmodel);
                controller_state_event(pmodel, STATE_EVENT_FAN_UVC_OFF);
                model_electrostatic_filter_off(pmodel);
                controller_update_class_output(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER,
                                               model_get_electrostatic_filter_state(pmodel));
            } else {
                ESP_LOGI(TAG, "Already scanning!");
            }
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_LIGHTS: {
            uint16_t class;
            if (!model_get_light_class(pmodel, &class)) {
                break;
            }

            model_light_switch(pmodel);

            switch (class) {
                case DEVICE_CLASS_LIGHT_1:
                    if (model_get_light_state(pmodel) == LIGHT_STATE_SINGLE_ON) {
                        msg->light_value = 1;
                    } else {
                        msg->light_value = 0;
                    }
                    break;

                case DEVICE_CLASS_LIGHT_2: {
                    switch (model_get_light_state(pmodel)) {
                        case LIGHT_STATE_OFF:
                            msg->light_value = 0;
                            break;

                        case LIGHT_STATE_GROUP_1_ON:
                            msg->light_value = 0x01;
                            break;


                        case LIGHT_STATE_GROUP_2_ON:
                            msg->light_value = 0x02;
                            break;

                        case LIGHT_STATE_DOUBLE_ON:
                            msg->light_value = 0x03;
                            break;
                    }
                    break;
                }

                case DEVICE_CLASS_LIGHT_3: {
                    switch (model_get_light_state(pmodel)) {
                        case LIGHT_STATE_OFF:
                            msg->light_value = 0;
                            break;

                        case LIGHT_STATE_GROUP_1_ON:
                            msg->light_value = 0x01;
                            break;

                        case LIGHT_STATE_GROUP_2_ON:
                            msg->light_value = 0x03;
                            break;

                        case LIGHT_STATE_TRIPLE_ON:
                            msg->light_value = 0x07;
                            break;
                    }
                    break;
                }

                default:
                    assert(0);
            }

            controller_update_class_output(pmodel, DEVICE_CLASS_LIGHT_1, (msg->light_value & 0x01) > 0);
            controller_update_class_output(pmodel, DEVICE_CLASS_LIGHT_2, (msg->light_value & 0x02) > 0);
            controller_update_class_output(pmodel, DEVICE_CLASS_LIGHT_3, (msg->light_value & 0x04) > 0);
            view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
            break;
        }

        case VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FAN:
            switch (model_get_fan_state(pmodel)) {
                case MODEL_FAN_STATE_OFF:
                    ESP_LOGI(TAG, "Turning fan on");
                    controller_state_event(pmodel, STATE_EVENT_FAN_START);
                    break;

                default:
                    ESP_LOGI(TAG, "Turning fan off");
                    controller_state_event(pmodel, STATE_EVENT_FAN_STOP);
            }
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FAN_FORCE:
            switch (model_get_fan_state(pmodel)) {
                case MODEL_FAN_STATE_CLEANING:
                    controller_state_event(pmodel, STATE_EVENT_FAN_TOGGLE);
                    break;

                default:
                    break;
            }
            break;


        case VIEW_CONTROLLER_MESSAGE_CODE_PRESSURE_CALIBRATION:
            controller_state_event(pmodel, STATE_EVENT_FAN_START_CALIBRATION);
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_ALL_OFF:
            controller_state_event(pmodel, STATE_EVENT_FAN_STOP);
            model_uvc_filter_off(pmodel);
            controller_state_event(pmodel, STATE_EVENT_FAN_UVC_OFF);
            model_electrostatic_filter_off(pmodel);
            controller_update_class_output(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER,
                                           model_get_electrostatic_filter_state(pmodel));
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FILTER: {
            if (model_is_any_fatal_alarm(pmodel)) {
                ESP_LOGI(TAG, "Attempt failed due to alarm");
                buzzer_beep(2, 100, 50, model_get_buzzer_volume(pmodel));
                pmodel->system_alarm = SYSTEM_ALARM_TRIGGERED;
                view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
                break;
            }

            uint16_t uvc_filters = model_get_uvc_filters_for_speed(pmodel, model_get_fan_speed(pmodel));

            size_t esf_count = model_get_class_count(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER);
            size_t ulf_count =
                uvc_filters > 0 ? model_get_class_count(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(uvc_filters - 1)) : 0;
            uint8_t update = 0;

            if (ulf_count > 0) {
                ESP_LOGI(TAG, "UVC filter toggle");
                model_uvc_filter_toggle(pmodel);
                controller_state_event(pmodel, model_get_uvc_filter_state(pmodel) ? STATE_EVENT_FAN_UVC_ON
                                                                                  : STATE_EVENT_FAN_UVC_OFF);
                update = 1;
            } else if (model_get_uvc_filter_state(pmodel)) {
                model_uvc_filter_off(pmodel);
                controller_state_event(pmodel, STATE_EVENT_FAN_UVC_OFF);
                update = 1;
            }

            if (esf_count > 0) {
                model_electrostatic_filter_toggle(pmodel);
                controller_update_class_output(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER,
                                               model_get_electrostatic_filter_state(pmodel));
                if (model_get_electrostatic_filter_state(pmodel)) {
                    controller_state_event(pmodel, STATE_EVENT_FAN_START);
                }
                update = 1;
            }

            if (update) {
                view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
            }
            break;
        }

        case VIEW_CONTROLLER_MESSAGE_CODE_REFRESH_DEVICES:
            refresh_devices = 1;
            refreshing      = 1;
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_AUTOMATIC_COMMISSIONING:
            model_delete_all_devices(pmodel);
            modbus_automatic_commissioning(msg->expected_devices);
            for (size_t i = 1; i < MODBUS_MAX_DEVICES; i++) {
                // Clear all devices
                configuration_save_device_data(model_get_device(pmodel, i));
            }
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_INFO: {
            modbus_read_device_info(msg->address);
            break;
        }

        case VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_INFO_AND_SAVE: {
            modbus_read_device_info(msg->address);
            configuration_save(pmodel);
            break;
        }

        case VIEW_CONTROLLER_MESSAGE_CODE_SAVE:
            configuration_save(pmodel);
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_SET_FAN_SPEED:
            controller_state_event(pmodel, STATE_EVENT_FAN_CHANGE_SPEED);
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_RESET_FILTER_HOURS:
            modbus_reset_device_work_hours(msg->address);
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_UPDATE_WIFI:
            if (model_get_wifi_enabled(pmodel) && model_get_wifi_configured(pmodel)) {
                network_start_sta();
                if (model_get_wifi_state(pmodel) == WIFI_STATE_CONNECTED) {
                    server_start();
                }
            } else {
                network_stop();
                server_stop();
            }
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_SCAN_WIFI:
            network_scan_access_points(0);
            model_set_scanning(pmodel, 1);
            view_event((view_event_t){.code = VIEW_EVENT_CODE_WIFI_UPDATE});
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_CONNECT_TO_WIFI:
            network_connect_to(msg->ssid, msg->psk);
            view_event((view_event_t){.code = VIEW_EVENT_CODE_WIFI_UPDATE});
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_READ_LOGS: {
            size_t num = log_read(pmodel->logs, msg->logs_from, LOG_BUFFER_SIZE);
            if (num > 0) {
                pmodel->logs_num  = num;
                pmodel->logs_from = msg->logs_from;
                view_event((view_event_t){.code = VIEW_EVENT_CODE_LOG_UPDATE});
            }
            break;
        }

        case VIEW_CONTROLLER_MESSAGE_CODE_CHANGE_SPEED:
            ESP_LOGI(TAG, "Changing speed: %i", msg->speed);
            model_set_fan_speed(pmodel, msg->speed < 5 ? msg->speed : 4);
            controller_state_event(pmodel, STATE_EVENT_FAN_CHANGE_SPEED);
            view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
            break;
    }
}


void controller_send_message(view_controller_message_t msg) {
    xQueueSend(queue, &msg, pdMS_TO_TICKS(10));
}


void controller_manage(model_t *pmodel) {
    static unsigned long refreshts     = 0;
    static unsigned long work_hours_ts = 0;
    static unsigned long tempts        = 0;
    static unsigned long poll_ts       = 0;
    static unsigned long heapts        = 0;
    static unsigned long time_ts       = 0;
    static unsigned long server_ts     = 0;
    static uint8_t       poll_address  = 0;
    static uint8_t       old_fan_speed = 0;

    if (old_fan_speed < model_get_fan_speed(pmodel)) {
        pmodel->extra_delta_status = EXTRA_DELTA_INCREASE;
        pmodel->speed_change_ts    = get_millis();
    } else if (old_fan_speed > model_get_fan_speed(pmodel)) {
        pmodel->extra_delta_status = EXTRA_DELTA_DECREASE;
        pmodel->speed_change_ts    = get_millis();
    }
    old_fan_speed = model_get_fan_speed(pmodel);

    configuration_manage();

    view_controller_message_t server_msg = {0};
    // Handle messages from the server
    if (xQueueReceive(queue, &server_msg, 0)) {
        controller_manage_message(pmodel, &server_msg);
    }

    if (model_get_scanning(pmodel) && network_get_scan_result(pmodel)) {
        model_set_scanning(pmodel, 0);
        view_event((view_event_t){.code = VIEW_EVENT_CODE_WIFI_UPDATE});
    }

    uint32_t     ip         = 0;
    wifi_state_t wifi_state = network_get_wifi_state();
    if (model_get_wifi_state(pmodel) != wifi_state) {
        network_is_connected(&ip, pmodel->ssid);
        model_set_wifi_state(pmodel, wifi_state);
        model_set_ip_addr(pmodel, ip);

        if (wifi_state == WIFI_STATE_CONNECTED) {
            server_start();
        }

        view_event((view_event_t){.code = VIEW_EVENT_CODE_WIFI_UPDATE});
    }

    if (is_expired(poll_ts, get_millis(), 500UL)) {
        if (poll_address == 0) {
            poll_address = model_get_next_device_address(pmodel, poll_address);
        } else {
            modbus_read_device_state(poll_address);

            switch (CLASS_GET_MODE(model_get_device(pmodel, poll_address).class)) {
                case DEVICE_MODE_PRESSURE:
                case DEVICE_MODE_TEMPERATURE_HUMIDITY:
                case DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY:
                    modbus_read_device_pressure(poll_address);
                    break;
            }

            poll_ts = get_millis();
        }
    }

    if (is_expired(tempts, get_millis(), 1000UL)) {
        if (model_get_uvc_filter_state(pmodel) || model_get_electrostatic_filter_state(pmodel)) {
            sensor_group_report_t pressures[DEVICE_GROUPS] = {0};
            int                   highest_group            = model_get_pressures(pmodel, pressures);
            if (highest_group >= DEVICE_GROUP_2) {
                int difference = pressures[0].pressure - pressures[1].pressure;
                difference     = difference < 0 ? -difference : difference;

                if (difference > pmodel->configuration.pressure_threshold_mb) {
                    pmodel->min_pressure_ts = get_millis();
                }
            } else {
                ESP_LOGI(TAG, "Not enough pressures");
                pmodel->min_pressure_ts = get_millis();
            }

            if (is_expired(pmodel->min_pressure_ts, get_millis(), 60000UL)) {
                ESP_LOGW(TAG, "Minimum pressure difference not reached");
                pmodel->system_alarm = SYSTEM_ALARM_TRIGGERED;
                system_shutdown(pmodel);
            }
        } else {
            pmodel->min_pressure_ts = get_millis();
        }

        if (temperature_error()) {
            pmodel->internal_sensor_error = 1;
        } else {
            pmodel->internal_sensor_error = 0;
            pmodel->temperature           = (int)temperature_get();
            model_set_humidity(pmodel, temperature_get_humidity());

            check_sensors_levels(pmodel);
        }

        // Periodically retry to initialize time if an error was found with the RTC
        if (pmodel->internal_rtc_error) {
            pmodel->internal_rtc_error = initialize_time();
        }

        view_event((view_event_t){.code = VIEW_EVENT_CODE_ANCILLARY_DATA_UPDATE});
        tempts = get_millis();
    }

    if (is_expired(server_ts, get_millis(), 1000UL) || server_is_there_a_new_connection()) {
        server_notify_state_change(pmodel);
        server_ts = get_millis();
    }

    if (refresh_devices || is_expired(refreshts, get_millis(), 1UL * 30UL * 1000UL)) {
        uint8_t starting_address = 0;
        uint8_t address          = model_get_next_device_address(pmodel, starting_address);
        while (address != starting_address) {
            modbus_read_device_info(address);
            last_refresh_address = address;
            starting_address     = address;
            address              = model_get_next_device_address(pmodel, starting_address);
        }
        refresh_devices = 0;
        refreshts       = get_millis();
    }

    if (model_all_devices_queried(pmodel) &&
        (first_update_loop || is_expired(work_hours_ts, get_millis(), 1UL * 60UL * 1000UL))) {
        update_work_hours_cycle(pmodel);
        work_hours_ts = get_millis();
    }

    if (update_time || is_expired(time_ts, get_millis(), 10UL * 60UL * 1000UL)) {
        modbus_update_time();
        update_time = 0;
        time_ts     = get_millis();
    }

    modbus_response_t response = {0};
    if (modbus_get_response(&response)) {
        switch (response.code) {
            case MODBUS_RESPONSE_CODE_DEVICE_OK:
                model_set_device_error(pmodel, response.address, 0);
                break;

            case MODBUS_RESPONSE_CODE_ERROR:
                if (response.success_code == MODBUS_RESPONSE_CODE_INFO && response.address == last_refresh_address &&
                    refreshing) {
                    view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_REFRESH_DONE});
                    refreshing = 0;
                }

                if (model_set_device_error(pmodel, response.address, 1)) {
                    ESP_LOGW(TAG, "Errore di comunicazione con %i", response.address);
                    LOG_DEVICE_COMMUNICATION_ERROR(response.address);
                    ESP_LOGW(TAG, "Device %i did not respond!", response.address);
                    error_condition_on_device(pmodel, response.address, 0, 1);
                    view_event(
                        (view_event_t){.code = VIEW_EVENT_CODE_DEVICE_UPDATE, .address = response.address, .error = 1});
                }

                if (response.address == poll_address) {
                    uint8_t new_poll_address = model_get_next_device_address(pmodel, poll_address);
                    if (new_poll_address == poll_address) {
                        // Restart
                        poll_address = 0;
                    } else {
                        poll_address = new_poll_address;
                    }
                }
                break;

            case MODBUS_RESPONSE_CODE_EVENTS:
                // TODO:
                break;

            case MODBUS_RESPONSE_CODE_ALARM:
                modbus_read_device_state(response.address);
                poll_ts = get_millis();
                break;

            case MODBUS_RESPONSE_CODE_SCAN_DONE:
                ESP_LOGI(TAG, "Scan done");
                view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_SEARCH_DONE});
                scanning = 0;
                configuration_save(pmodel);
                break;

            case MODBUS_RESPONSE_DEVICE_AUTOMATIC_CONFIGURATION_LISTENING_DONE:
                view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_LISTENING_DONE});
                scanning = 0;
                break;

            case MODBUS_RESPONSE_DEVICE_AUTOMATIC_CONFIGURATION:
                view_event((view_event_t){.code        = VIEW_EVENT_CODE_DEVICE_CONFIGURED,
                                          .error       = response.error,
                                          .address     = response.address,
                                          .data.number = response.devices_number});
                configuration_save(pmodel);
                refresh_devices = 1;
                break;

            case MODBUS_RESPONSE_CODE_INFO:
                if (response.scanning && response.address != 0 && !response.error) {
                    ESP_LOGI(TAG, "Scan info from %i", response.address);
                    if (!model_is_address_configured(pmodel, response.address)) {
                        model_new_device(pmodel, response.address);
                        view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_NEW});
                    }
                    scanning = 1;
                }

                if (!response.scanning && response.address == last_refresh_address && refreshing) {
                    view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_REFRESH_DONE});
                    refreshing = 0;
                }

                device_t device = model_get_device(pmodel, response.address);

                uint8_t save_data = 0;
                if (device.status == DEVICE_STATUS_CONFIGURED || device.status == DEVICE_STATUS_COMMUNICATION_ERROR) {
                    ESP_LOGI(TAG, "Found device %i on the network, saving data...", response.address);
                    save_data = 1;
                }
                if (model_is_address_configured(pmodel, response.address)) {
                    model_set_device_error(pmodel, response.address, 0);
                    model_set_device_sn(pmodel, response.address, response.serial_number);
                    model_set_device_class(pmodel, response.address, response.class);
                    model_set_device_firmware(pmodel, response.address, response.firmware_version);
                }

                if (save_data) {
                    configuration_save_device_data(model_get_device(pmodel, response.address));
                }

                view_event((view_event_t){
                    .code = VIEW_EVENT_CODE_DEVICE_UPDATE, .address = response.address, .error = response.error});
                break;

            case MODBUS_RESPONSE_CODE_WORK_HOURS:
                ESP_LOGI(TAG, "Device %i work hours: %i", response.address, response.work_hours);
                if (model_set_filter_work_hours(pmodel, response.address, response.work_hours)) {
                    model_set_show_work_hours_state(pmodel, 1);
                }
                view_event((view_event_t){.code = VIEW_EVENT_CODE_UPDATE_WORK_HOURS});
                break;

            case MODBUS_RESPONSE_CODE_MESSAGES: {
                view_event_t event = {.code = VIEW_EVENT_CODE_DEVICE_MESSAGES};
                event.num_messages = response.num_messages;
                for (size_t i = 0; i < response.num_messages; i++) {
                    event.messages[i] = response.messages[i];
                }
                view_event(event);
                break;
            }

            case MODBUS_RESPONSE_CODE_STATE:
                model_set_device_error(pmodel, response.address, 0);
                if (model_set_device_alarms(pmodel, response.address, response.alarms)) {
                    LOG_DEVICE_ALARM(response.address, response.alarms);
                    error_condition_on_device(pmodel, response.address, response.alarms, 0);
                    view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_ALARM, .address = response.address});
                }
                model_set_device_state(pmodel, response.address, response.state);

                uint8_t new_poll_address = model_get_next_device_address(pmodel, poll_address);
                if (new_poll_address == poll_address) {
                    // Restart
                    poll_address = 0;
                } else {
                    poll_address = new_poll_address;
                }
                break;

            case MODBUS_RESPONSE_CODE_PRESSURE:
                model_set_sensors_values(pmodel, response.address, response.pressure, response.temperature,
                                         response.humidity);
                if (model_is_device_pressure_sensor(pmodel, response.address)) {
                    sensors_calibration_new_reading(pmodel, response.address, response.pressure);
                }
                view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_UPDATE});
                break;
        }
    }


    if (is_expired(heapts, get_millis(), 4000UL)) {
        // print_heap_status();
        network_get_current_rssi(pmodel);
        controller_state_event(pmodel, STATE_EVENT_SENSORS_CHECK);
        heapts = get_millis();
    }

    controller_state_manage(pmodel);
    sensors_calibration_manage(pmodel);
}


void controller_update_class_output(model_t *pmodel, uint16_t class, int value) {
    ESP_LOGI(TAG, "Setting output for class (generic) %X: %i", class, value);
    modbus_set_class_output(class, value, pmodel->system_alarm == SYSTEM_ALARM_OVERRULED);

    uint8_t starting_address = 0;
    uint8_t address          = model_get_next_device_address_by_class(pmodel, starting_address, class);
    while (address != starting_address) {
        modbus_set_device_output(address, value > 0, pmodel->system_alarm == SYSTEM_ALARM_OVERRULED);
        starting_address = address;
        address          = model_get_next_device_address_by_class(pmodel, starting_address, class);
    }
}


void controller_update_fan_percentage(model_t *pmodel, uint8_t fan_speed) {
    uint16_t correction    = model_get_fan_percentage_correction(pmodel);
    uint16_t classes[]     = {DEVICE_CLASS_SIPHONING_FAN, DEVICE_CLASS_IMMISSION_FAN};
    uint16_t percentages[] = {
        model_get_siphoning_percentage(pmodel, fan_speed) + correction,
        model_get_immission_percentage(pmodel, fan_speed) + correction,
    };

    for (size_t i = 0; i < sizeof(classes) / sizeof(classes[0]); i++) {
        percentages[i] = percentages[i] > 100 ? 100 : percentages[i];     // Cap to 100

        uint8_t starting_address = 0;
        uint8_t address          = model_get_next_device_address_by_class(pmodel, starting_address, classes[i]);

        while (address != starting_address) {
            modbus_set_fan_percentage(address, percentages[i]);
            starting_address = address;
            address          = model_get_next_device_address_by_class(pmodel, starting_address, classes[i]);
        }
    }
}


static void error_condition_on_device(model_t *pmodel, uint8_t address, uint8_t alarms, uint8_t communication) {
    device_t device = model_get_device(pmodel, address);
    ESP_LOGW(TAG, "error condition on device %i, comm %i, class 0x%X, alarms 0x%X", address, communication,
             device.class, alarms);

    switch (CLASS_GET_MODE(device.class)) {
        case DEVICE_MODE_FAN:
        case DEVICE_MODE_ESF:
        case DEVICE_MODE_UVC:
            if (communication || (alarms & EASYCONNECT_SAFETY_ALARM)) {
                system_shutdown(pmodel);
            }
            break;

        case DEVICE_MODE_PRESSURE:
        case DEVICE_MODE_TEMPERATURE_HUMIDITY:
        case DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY:
            break;
    }
}


static void system_shutdown(model_t *pmodel) {
    if (pmodel->system_alarm == SYSTEM_ALARM_OVERRULED) {
        return;
    }

    uint8_t something_was_on = 0;

    if (model_get_fan_state(pmodel)) {
        controller_state_event(pmodel, STATE_EVENT_FAN_EMERGENCY_STOP);
        pmodel->system_alarm = SYSTEM_ALARM_TRIGGERED;
        something_was_on     = 1;
    }
    if (model_get_electrostatic_filter_state(pmodel)) {
        controller_update_class_output(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER, 0);
        model_electrostatic_filter_off(pmodel);
        pmodel->system_alarm = SYSTEM_ALARM_TRIGGERED;
        something_was_on     = 1;
    }
    if (model_get_uvc_filter_state(pmodel)) {
        controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1), 0);
        controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2), 0);
        controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3), 0);
        model_uvc_filter_off(pmodel);
        pmodel->system_alarm = SYSTEM_ALARM_TRIGGERED;
        something_was_on     = 1;
    }

    if (something_was_on) {
        ESP_LOGI(TAG, "System shutdown!");
        view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_SHUTDOWN});
    }
}


static void print_heap_status(void) {
#ifndef PC_SIMULATOR
    printf("[%s] - Internal RAM: LWM = %u, free = %u, biggest = %u\n", TAG,
           heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL), heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
           heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
    printf("[%s] - PSRAM       : LWM = %u, free = %u\n", TAG, heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM),
           heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    lv_mem_monitor_t monitor = {0};
    lv_mem_monitor(&monitor);
    printf("[%s] - LVGL        : free = %u, frag = %u%%\n", TAG, monitor.free_size, monitor.frag_pct);
#endif
}


static void update_work_hours_cycle(model_t *pmodel) {
    static uint8_t starting_address = 0;
    static uint8_t address          = 0;

    uint16_t const modes[] = {DEVICE_MODE_ESF, DEVICE_MODE_UVC};

    address = model_get_next_device_address_by_modes(pmodel, starting_address, (uint16_t *)modes,
                                                     sizeof(modes) / sizeof(modes[0]));
    if (address != starting_address) {
        ESP_LOGI(TAG, "Reading work hours for %i", address);
        modbus_read_device_work_hours(address);
        starting_address = address;
    } else {
        ESP_LOGI(TAG, "work hours update cycle complete");
        // restart
        starting_address  = 0;
        address           = 0;
        first_update_loop = 0;
    }
}


static void check_sensors_levels(model_t *pmodel) {
    static uint8_t       old_temperature_1_warning = 0;
    static uint8_t       old_temperature_2_warning = 0;
    static uint8_t       old_humidity_1_warning    = 0;
    static uint8_t       old_humidity_2_warning    = 0;
    static uint8_t       stop                      = 0;
    static unsigned long timestamp                 = 0;

    uint16_t temperature_humidity_count = model_get_mode_count(pmodel, DEVICE_MODE_TEMPERATURE_HUMIDITY);
    uint16_t pressure_temperature_humidity_count =
        model_get_mode_count(pmodel, DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY);

    uint8_t current_temperature_1_warning = 0;
    uint8_t current_temperature_2_warning = 0;
    uint8_t current_humidity_1_warning    = 0;
    uint8_t current_humidity_2_warning    = 0;


    if (pressure_temperature_humidity_count + temperature_humidity_count > 0) {
        sensor_group_report_t group_1 = {0};
        sensor_group_report_t group_2 = {0};
        model_get_temperatures_humidities(pmodel, &group_1, &group_2);

        if (group_1.valid && group_2.valid) {
            current_temperature_1_warning = group_1.temperature > model_get_temperature_warn(pmodel);
            current_temperature_2_warning = group_2.temperature > model_get_temperature_warn(pmodel);

            current_humidity_1_warning = group_1.humidity > model_get_humidity_warn(pmodel);
            current_humidity_2_warning = group_2.humidity > model_get_humidity_warn(pmodel);

            stop = group_1.temperature > model_get_temperature_stop(pmodel) ||
                   group_2.temperature > model_get_temperature_stop(pmodel) ||
                   group_1.humidity > model_get_humidity_stop(pmodel) ||
                   group_2.humidity > model_get_humidity_stop(pmodel);
        }
    } else {
        current_temperature_1_warning = pmodel->temperature > model_get_temperature_warn(pmodel);
        current_humidity_1_warning    = model_get_humidity(pmodel) > model_get_humidity_warn(pmodel);

        stop = pmodel->temperature > model_get_temperature_stop(pmodel) ||
               model_get_humidity(pmodel) > model_get_humidity_stop(pmodel);
    }

    if (stop) {
        if (is_expired(timestamp, get_millis(), 2000UL)) {
            buzzer_beep(1, 500, 0, model_get_buzzer_volume(pmodel));
            timestamp = get_millis();
        }
    } else if (current_temperature_1_warning && !old_temperature_1_warning) {
        buzzer_beep(1, 500, 0, model_get_buzzer_volume(pmodel));
    } else if (current_temperature_2_warning && !old_temperature_2_warning) {
        buzzer_beep(1, 500, 0, model_get_buzzer_volume(pmodel));
    } else if (current_humidity_1_warning && !old_humidity_1_warning) {
        buzzer_beep(1, 500, 0, model_get_buzzer_volume(pmodel));
    } else if (current_humidity_2_warning && !old_humidity_2_warning) {
        buzzer_beep(1, 500, 0, model_get_buzzer_volume(pmodel));
    }

    old_temperature_1_warning = current_temperature_1_warning;
    old_temperature_2_warning = current_temperature_2_warning;
    old_humidity_1_warning    = current_humidity_1_warning;
    old_humidity_2_warning    = current_humidity_2_warning;
}


static void console_task(void *args) {
#ifndef PC_SIMULATOR
    const char              *prompt    = "EC-main> ";
    easyconnect_interface_t *interface = args;

    esp32_commandline_init(interface);
    device_commands_register(interface->arg);

    for (;;) {
        esp32_edit_cycle(prompt);
        esp_log_level_set("*", ESP_LOG_NONE);
    }
#else
    (void)args;
#endif

    vTaskDelete(NULL);
}


static uint32_t get_serial_number(void *arg) {
    return model_get_my_sn(arg);
}


static int initialize_time(void) {
    int result = ds1307_is_clock_halted(rtc_driver);
    if (result > 0) {     // Initialize time
        rtc_time_t rtc_time = {.day = 6, .wday = 1, .month = 3, .year = 22};
        ds1307_set_time(rtc_driver, rtc_time, 0);
        ESP_LOGI(TAG, "RTC Clock started");
    } else if (result == 0) {
        rtc_time_t rtc_time = {0};
        result              = ds1307_get_time(rtc_driver, &rtc_time);
        struct tm tm        = ds1307_tm_from_rtc(rtc_time);
        utils_set_system_time(tm);
    }

    return result < 0 ? result : 0;
}
