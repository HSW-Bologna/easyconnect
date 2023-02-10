#include <sys/time.h>
#include <time.h>
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


static void error_condition_on_device(model_t *pmodel, uint8_t address, uint8_t alarms, uint8_t communication);
static void print_heap_status(void);


static const char *TAG             = "Controller";
static uint8_t     refresh_devices = 1;
static uint8_t     update_time     = 1;


void controller_init(model_t *pmodel) {
    modbus_init();
    temperature_init();
    log_init();
    configuration_load(pmodel);
    view_start(pmodel);

    tft_backlight_set(model_get_active_backlight(pmodel));

    if (ds1307_is_clock_halted(rtc_driver)) {
        rtc_time_t rtc_time = {.day = 6, .wday = 1, .month = 3, .year = 22};
        ds1307_set_time(rtc_driver, rtc_time, 0);
        ESP_LOGI(TAG, "RTC Clock started");
    } else {
        rtc_time_t rtc_time = {0};
        ds1307_get_time(rtc_driver, &rtc_time);
        struct tm tm = ds1307_tm_from_rtc(rtc_time);
        utils_set_system_time(tm);
    }
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
            modbus_scan();
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_LIGHTS: {
            controller_update_class_output(pmodel, DEVICE_CLASS_LIGHT_1, (msg->light_value & 0x01) > 0);
            controller_update_class_output(pmodel, DEVICE_CLASS_LIGHT_2, (msg->light_value & 0x02) > 0);
            controller_update_class_output(pmodel, DEVICE_CLASS_LIGHT_3, (msg->light_value & 0x04) > 0);
            break;
        }

        case VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FAN:
            switch (model_get_fan_state(pmodel)) {
                case MODEL_FAN_STATE_OFF:
                    controller_state_event(pmodel, STATE_EVENT_FAN_START);
                    break;

                default:
                    controller_state_event(pmodel, STATE_EVENT_FAN_STOP);
            }
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FILTER: {
            if (model_is_filter_alarm_on(pmodel, EASYCONNECT_SAFETY_ALARM)) {
                ESP_LOGI(TAG, "Attempt failed due to alarm");
                buzzer_beep(2, 100, 50, model_get_buzzer_volume(pmodel));
                break;
            }

            uint16_t ulf_class =
                DEVICE_CLASS_ULTRAVIOLET_FILTER(model_get_uvc_filters_for_speed(pmodel, model_get_fan_speed(pmodel)));
            size_t  esf_count = model_get_class_count(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER);
            size_t  ulf_count = model_get_class_count(pmodel, ulf_class);
            uint8_t update    = 0;

            if (ulf_count > 0) {
                if (model_get_fan_state(pmodel) == MODEL_FAN_STATE_FAN_RUNNING || model_get_uvc_filter_state(pmodel)) {
                    model_uvc_filter_toggle(pmodel);
                    controller_update_class_output(pmodel, ulf_class, model_get_uvc_filter_state(pmodel));
                    if (model_get_uvc_filter_state(pmodel)) {
                        controller_state_event(pmodel, STATE_EVENT_FAN_START);
                    }
                    update = 1;
                } else {
                    controller_state_event(pmodel, STATE_EVENT_FAN_UVC_START);
                }
            }

            if (esf_count > 0) {
                model_electrostatic_filter_toggle(pmodel);
                controller_update_class_output(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER,
                                               model_get_electrostatic_filter_state(pmodel));
                if (model_get_electrostatic_filter_state(pmodel)) {
                    controller_state_event(pmodel, STATE_EVENT_FAN_START);
                }
                // update = 1;
            }

            if (update) {
                view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
            }
            break;
        }

        case VIEW_CONTROLLER_MESSAGE_CODE_REFRESH_DEVICES:
            refresh_devices = 1;
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_AUTOMATIC_COMMISSIONING:
            modbus_automatic_commissioning(msg->expected_devices);
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
            switch (model_get_fan_state(pmodel)) {
                case MODEL_FAN_STATE_SF_ENV_CLEANING:
                case MODEL_FAN_STATE_IF_ENV_CLEANING:
                    // Max speed, ignore
                    break;

                default:
                    controller_update_fan_speed(pmodel, model_get_fan_speed(pmodel));
                    break;
            }
            break;
    }
}


void controller_manage(model_t *pmodel) {
    static unsigned long refreshts    = 0;
    static unsigned long tempts       = 0;
    static unsigned long poll_ts      = 0;
    static unsigned long heapts       = 0;
    static unsigned long time_ts      = 0;
    static uint8_t       poll_address = 0;

    configuration_manage();

    if (is_expired(poll_ts, get_millis(), 200UL)) {
        if (poll_address == 0) {
            poll_address = model_get_next_device_address(pmodel, poll_address);
        } else {
            modbus_read_device_alarms(poll_address);

            if (model_get_device(pmodel, poll_address).class == DEVICE_CLASS_PRESSURE_SAFETY) {
                modbus_read_device_pressure(poll_address);
            }

            poll_ts = get_millis();
        }
    }

    if (is_expired(tempts, get_millis(), 500UL)) {
        pmodel->temperature = (int)temperature_get();
        view_event((view_event_t){.code = VIEW_EVENT_CODE_ANCILLARY_DATA_UPDATE});
        tempts = get_millis();
    }

    if (refresh_devices || is_expired(refreshts, get_millis(), 1UL * 60UL * 1000UL)) {
        uint8_t starting_address = 0;
        uint8_t address          = model_get_next_device_address(pmodel, starting_address);
        while (address != starting_address) {
            modbus_read_device_info(address);
            starting_address = address;
            address          = model_get_next_device_address(pmodel, starting_address);
        }
        refresh_devices = 0;
        refreshts       = get_millis();
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
                if (model_set_device_error(pmodel, response.address, 1)) {
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
                modbus_read_device_alarms(response.address);
                poll_ts = get_millis();
                break;

            case MODBUS_RESPONSE_CODE_SCAN_DONE:
                view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_SEARCH_DONE});
                break;

            case MODBUS_RESPONSE_DEVICE_AUTOMATIC_CONFIGURATION_LISTENING_DONE:
                view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_LISTENING_DONE});

                break;

            case MODBUS_RESPONSE_DEVICE_AUTOMATIC_CONFIGURATION:
                view_event((view_event_t){.code        = VIEW_EVENT_CODE_DEVICE_CONFIGURED,
                                          .error       = response.error,
                                          .address     = response.address,
                                          .data.number = response.devices_number});
                break;

            case MODBUS_RESPONSE_CODE_INFO:
                if (!response.scanning && model_is_address_configured(pmodel, response.address)) {
                    model_set_device_error(pmodel, response.address, 0);
                    model_set_device_sn(pmodel, response.address, response.serial_number);
                    model_set_device_class(pmodel, response.address, response.class);
                    model_set_device_firmware(pmodel, response.address, response.firmware_version);
                }
                view_event((view_event_t){
                    .code = VIEW_EVENT_CODE_DEVICE_UPDATE, .address = response.address, .error = response.error});
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

            case MODBUS_RESPONSE_CODE_ALARMS_REG:
                ESP_LOGD(TAG, "Device %i alarm 0x%02X", response.address, response.alarms);
                model_set_device_error(pmodel, response.address, 0);
                if (model_set_device_alarms(pmodel, response.address, response.alarms)) {
                    error_condition_on_device(pmodel, response.address, response.alarms, 0);
                    view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_ALARM, .address = response.address});
                }
                uint8_t new_poll_address = model_get_next_device_address(pmodel, poll_address);
                if (new_poll_address == poll_address) {
                    // Restart
                    poll_address = 0;
                } else {
                    poll_address = new_poll_address;
                }
                break;

            case MODBUS_RESPONSE_CODE_PRESSURE:
                model_set_device_pressure(pmodel, response.address, response.pressure);
                view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_UPDATE});
                break;
        }
    }


    if (is_expired(heapts, get_millis(), 5000UL)) {
        print_heap_status();
        heapts = get_millis();
    }

    controller_state_manage(pmodel);
}


void controller_update_class_output(model_t *pmodel, uint16_t class, int value) {
    modbus_set_class_output(class, value);

    uint8_t starting_address = 0;
    uint8_t address          = model_get_next_device_address_by_class(pmodel, starting_address, class);
    while (address != starting_address) {
        modbus_set_device_output(address, value > 0);
        starting_address = address;
        address          = model_get_next_device_address_by_class(pmodel, starting_address, class);
    }
}


void controller_update_fan_speed(model_t *pmodel, size_t speed) {
    uint16_t classes[] = {DEVICE_CLASS_IMMISSION_FAN, DEVICE_CLASS_SIPHONING_FAN};

    for (size_t i = 0; i < sizeof(classes) / sizeof(classes[0]); i++) {
        uint8_t starting_address = 0;
        uint8_t address          = model_get_next_device_address_by_class(pmodel, starting_address, classes[i]);

        while (address != starting_address) {
            modbus_set_fan_speed(address, speed);
            starting_address = address;
            address          = model_get_next_device_address_by_class(pmodel, starting_address, classes[i]);
        }
    }
}


static void error_condition_on_device(model_t *pmodel, uint8_t address, uint8_t alarms, uint8_t communication) {
    device_t device = model_get_device(pmodel, address);
    ESP_LOGW(TAG, "error condition on device %i, comm %i, class 0x%X, alarms 0x%X", address, communication,
             device.class, alarms);

    switch (device.class) {
        case DEVICE_CLASS_ELECTROSTATIC_FILTER:
            if (model_get_electrostatic_filter_state(pmodel) &&
                (communication || (alarms & EASYCONNECT_SAFETY_ALARM))) {
                controller_update_class_output(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER, 0);
                model_electrostatic_filter_off(pmodel);
            }
            break;

        case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1):
        case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2):
        case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3):
            if (model_get_uvc_filter_state(pmodel) && (communication || (alarms & EASYCONNECT_SAFETY_ALARM))) {
                controller_update_class_output(pmodel, device.class, 0);
                model_uvc_filter_off(pmodel);
            }
            break;

        case DEVICE_CLASS_SIPHONING_FAN:
        case DEVICE_CLASS_IMMISSION_FAN:
            if (model_get_electrostatic_filter_state(pmodel)) {
                controller_update_class_output(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER, 0);
                model_electrostatic_filter_off(pmodel);
            }
            if (model_get_uvc_filter_state(pmodel)) {
                controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1), 0);
                controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2), 0);
                controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3), 0);
                model_uvc_filter_off(pmodel);
            }
            break;
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