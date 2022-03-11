#include <sys/time.h>
#include <time.h>
#include "controller.h"
#include "model/model.h"
#include "view/view.h"
#include "peripherals/modbus.h"
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


static const char *TAG             = "Controller";
static int         refresh_devices = 1;


void controller_init(model_t *pmodel) {
    temperature_init();
    log_init();
    configuration_load(pmodel);
    view_start(pmodel);

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
            size_t esf_count = model_get_class_count(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER);
            size_t ulf_count = model_get_class_count(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER);

            if (ulf_count > 0) {
                if (model_get_fan_state(pmodel) == MODEL_FAN_STATE_FAN_RUNNING || model_get_uvc_filter_state(pmodel)) {
                    model_uvc_filter_toggle(pmodel);
                    controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER,
                                                   model_get_uvc_filter_state(pmodel));
                    if (model_get_uvc_filter_state(pmodel)) {
                        controller_state_event(pmodel, STATE_EVENT_FAN_START);
                    }
                    view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
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
                view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
            }

            break;
        }

        case VIEW_CONTROLLER_MESSAGE_CODE_REFRESH_DEVICES:
            refresh_devices = 1;
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_AUTOMATIC_COMMISSIONING:
            modbus_automatic_commissioning();
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

        case VIEW_CONTROLLER_MESSAGE_CODE_CONFIGURE_DEVICE_ADDRESS:
            modbus_configure_device_address(msg->address);
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_SET_CLASS:
            ESP_LOGI(TAG, "Class %X", msg->class);
            modbus_set_device_class(msg->address, msg->class);
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_SAVE:
            configuration_save(pmodel);
            break;
    }
}


void controller_manage(model_t *pmodel) {
    static unsigned long refreshts = 0;
    static unsigned long tempts    = 0;

    if (is_expired(tempts, get_millis(), 500UL)) {
        pmodel->temperature = (int)temperature_get();
        view_event((view_event_t){.code = VIEW_EVENT_CODE_ANCILLARY_DATA_UPDATE});
        tempts = get_millis();
    }

    if (refresh_devices || is_expired(refreshts, get_millis(), 5UL * 60UL * 1000UL)) {
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

    modbus_response_t response = {0};
    if (modbus_get_response(&response)) {
        switch (response.code) {
            case MODBUS_RESPONSE_CODE_DEVICE_OK:
                model_set_device_error(pmodel, response.address, 0);
                break;

            case MODBUS_RESPONSE_ERROR:
                ESP_LOGW(TAG, "Device %i did not respond!", response.address);
                model_set_device_error(pmodel, response.address, 1);
                view_event(
                    (view_event_t){.code = VIEW_EVENT_CODE_DEVICE_UPDATE, .address = response.address, .error = 1});
                break;

            case MODBUS_RESPONSE_CODE_SCAN_DONE:
                view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_SEARCH_DONE});
                break;

            case MODBUS_RESPONSE_DEVICE_AUTOMATIC_CONFIGURATION_LISTENING_DONE:
                view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_LISTENING_DONE});

                break;
            case MODBUS_RESPONSE_DEVICE_MANUAL_CONFIGURATION:
                view_event((view_event_t){
                    .code = VIEW_EVENT_CODE_DEVICE_CONFIGURED, .error = response.error, .address = response.address});
                break;

            case MODBUS_RESPONSE_DEVICE_AUTOMATIC_CONFIGURATION:
                view_event((view_event_t){.code        = VIEW_EVENT_CODE_DEVICE_CONFIGURED,
                                          .error       = response.error,
                                          .address     = response.address,
                                          .data.number = response.devices_number});
                break;

            case MODBUS_RESPONSE_CODE_CLASS:
                model_set_device_error(pmodel, response.address, 0);
                model_set_device_class(pmodel, response.address, response.class);
                view_event((view_event_t){.code = VIEW_EVENT_CODE_DEVICE_UPDATE, .address = response.address});
                break;

            case MODBUS_RESPONSE_CODE_INFO:
                if (!response.scanning && model_is_address_configured(pmodel, response.address)) {
                    model_set_device_error(pmodel, response.address, 0);
                    model_set_device_sn(pmodel, response.address, response.serial_number);
                    model_set_device_class(pmodel, response.address, response.class);
                }
                view_event((view_event_t){
                    .code = VIEW_EVENT_CODE_DEVICE_UPDATE, .address = response.address, .error = response.error});
                break;
        }
    }

    controller_state_manage(pmodel);
}


void controller_update_class_output(model_t *pmodel, device_class_t class, int value) {
    modbus_set_class_output(class, value);

    uint8_t starting_address = 0;
    uint8_t address          = model_get_next_device_address_by_class(pmodel, starting_address, class);
    while (address != starting_address) {
        modbus_set_device_output(address, value > 0);
        starting_address = address;
        address          = model_get_next_device_address_by_class(pmodel, starting_address, class);
    }
}