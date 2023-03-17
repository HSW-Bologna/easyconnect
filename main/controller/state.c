#include "state.h"
#include "gel/timer/stopwatch.h"
#include "model/model.h"
#include "view/view.h"
#include "utils/utils.h"
#include "controller.h"
#include "gel/timer/timecheck.h"


typedef int (*state_event_manager_t)(model_t *, state_event_code_t);
typedef int (*state_entry_t)(model_t *);


static int  off_entry(model_t *pmodel);
static int  off_event_manager(model_t *pmodel, state_event_code_t event);
static int  env_clean_sf_entry(model_t *pmodel);
static int  env_clean_sf_event_manager(model_t *pmodel, state_event_code_t event);
static int  env_clean_sf_if_entry(model_t *pmodel);
static int  env_clean_sf_if_event_manager(model_t *pmodel, state_event_code_t event);
static int  env_clean_if_entry(model_t *pmodel);
static int  env_clean_if_event_manager(model_t *pmodel, state_event_code_t event);
static int  fan_running_entry(model_t *pmodel);
static int  fan_running_event_manager(model_t *pmodel, state_event_code_t event);
static void update_uvc_filters(model_t *pmodel, uint8_t fan_speed, uint8_t value);


static const struct {
    state_entry_t         guard;
    state_event_manager_t event;
} event_managers[] = {
    {off_entry, off_event_manager},
    {env_clean_sf_entry, env_clean_sf_event_manager},
    {env_clean_sf_if_entry, env_clean_sf_if_event_manager},
    {env_clean_if_entry, env_clean_if_event_manager},
    {fan_running_entry, fan_running_event_manager},
};
static stopwatch_t   environment_cleaning_sw = STOPWATCH_NULL;
static int           auto_uvc_on             = 0;
static uint16_t      cleaning_period         = 0;
static unsigned long timestamp               = 0;


void controller_state_event(model_t *pmodel, state_event_code_t event) {
    int res = event_managers[model_get_fan_state(pmodel)].event(pmodel, event);

    if (res >= 0) {
        if (event_managers[res].guard(pmodel) == 0) {
            model_set_fan_state(pmodel, res);
        }
    }
}


void controller_state_manage(model_t *pmodel) {
    (void)pmodel;
    if (stopwatch_get_state(&environment_cleaning_sw) == TIMER_COUNTING &&
        stopwatch_is_timer_reached(&environment_cleaning_sw, get_millis())) {
        stopwatch_stop(&environment_cleaning_sw);
        controller_state_event(pmodel, STATE_EVENT_ENVIRONMENTAL_CLEANING_DONE);
    }
}


static int off_entry(model_t *pmodel) {
    view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
    controller_update_class_output(pmodel, DEVICE_CLASS_SIPHONING_FAN, 0);
    controller_update_class_output(pmodel, DEVICE_CLASS_IMMISSION_FAN, 0);
    controller_update_class_output(pmodel, DEVICE_CLASS_GAS, 0);

    model_uvc_filter_off(pmodel);
    model_electrostatic_filter_off(pmodel);
    controller_update_class_output(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER, 0);
    controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1), 0);
    controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2), 0);
    controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3), 0);

    if (timestamp != 0) {
        model_add_passive_filters_work_seconds(pmodel, time_interval(timestamp, get_millis()) / 1000UL);
        timestamp = 0;
    }

    return 0;
}


static int off_event_manager(model_t *pmodel, state_event_code_t event) {
    switch (event) {
        case STATE_EVENT_FAN_START:
            auto_uvc_on     = 0;
            cleaning_period = model_get_environment_cleaning_start_period(pmodel);
            return MODEL_FAN_STATE_SF_ENV_CLEANING;

        case STATE_EVENT_FAN_UVC_START:
            auto_uvc_on     = 1;
            cleaning_period = model_get_environment_cleaning_start_period(pmodel);
            return MODEL_FAN_STATE_SF_ENV_CLEANING;

        default:
            break;
    }

    return -1;
}


static int env_clean_sf_entry(model_t *pmodel) {
    view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
    stopwatch_setngo(&environment_cleaning_sw, cleaning_period * 1000UL, get_millis());
    if (cleaning_period > 0) {
        controller_update_fan_percentage(pmodel, TOP_FAN_SPEED);
        controller_update_class_output(pmodel, DEVICE_CLASS_SIPHONING_FAN, 1);
        controller_update_class_output(pmodel, DEVICE_CLASS_IMMISSION_FAN, 0);
    }

    timestamp = get_millis();

    return 0;
}


static int env_clean_sf_event_manager(model_t *pmodel, state_event_code_t event) {
    switch (event) {
        case STATE_EVENT_FAN_UVC_START:
            auto_uvc_on = 1;
            break;

        case STATE_EVENT_ENVIRONMENTAL_CLEANING_DONE:
            if (model_get_class_count(pmodel, DEVICE_CLASS_IMMISSION_FAN) > 0) {
                cleaning_period = model_get_environment_cleaning_start_period(pmodel);
                return MODEL_FAN_STATE_SF_IF_ENV_CLEANING;
            } else {
                if (auto_uvc_on) {
                    auto_uvc_on = 0;
                    model_uvc_filter_on(pmodel);
                    update_uvc_filters(pmodel, model_get_fan_speed(pmodel), 1);
                }

                return MODEL_FAN_STATE_FAN_RUNNING;
            }

        case STATE_EVENT_FAN_EMERGENCY_STOP:
            return MODEL_FAN_STATE_OFF;

        default:
            break;
    }

    return -1;
}


static int env_clean_if_entry(model_t *pmodel) {
    view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
    stopwatch_setngo(&environment_cleaning_sw, cleaning_period * 1000UL, get_millis());
    if (cleaning_period > 0) {
        controller_update_fan_percentage(pmodel, TOP_FAN_SPEED);
        controller_update_class_output(pmodel, DEVICE_CLASS_SIPHONING_FAN, 0);
        controller_update_class_output(pmodel, DEVICE_CLASS_IMMISSION_FAN, 1);
    }

    timestamp = get_millis();

    return 0;
}


static int env_clean_if_event_manager(model_t *pmodel, state_event_code_t event) {
    switch (event) {
        case STATE_EVENT_FAN_UVC_START:
            auto_uvc_on = 1;
            break;

        case STATE_EVENT_ENVIRONMENTAL_CLEANING_DONE:
            return MODEL_FAN_STATE_OFF;

        case STATE_EVENT_FAN_EMERGENCY_STOP:
            return MODEL_FAN_STATE_OFF;

        default:
            break;
    }

    return -1;
}


static int env_clean_sf_if_entry(model_t *pmodel) {
    view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});
    stopwatch_setngo(&environment_cleaning_sw, cleaning_period * 1000UL, get_millis());
    if (cleaning_period > 0) {
        controller_update_fan_percentage(pmodel, TOP_FAN_SPEED);
        controller_update_class_output(pmodel, DEVICE_CLASS_SIPHONING_FAN, 1);
        controller_update_class_output(pmodel, DEVICE_CLASS_IMMISSION_FAN, 1);
    }
    return 0;
}


static int env_clean_sf_if_event_manager(model_t *pmodel, state_event_code_t event) {
    switch (event) {
        case STATE_EVENT_FAN_UVC_START:
            auto_uvc_on = 1;
            break;

        case STATE_EVENT_ENVIRONMENTAL_CLEANING_DONE:
            if (auto_uvc_on) {
                auto_uvc_on = 0;
                model_uvc_filter_on(pmodel);
                update_uvc_filters(pmodel, model_get_fan_speed(pmodel), 1);
            }
            return MODEL_FAN_STATE_FAN_RUNNING;

        case STATE_EVENT_FAN_EMERGENCY_STOP:
            return MODEL_FAN_STATE_OFF;

        default:
            break;
    }

    return -1;
}


static int fan_running_entry(model_t *pmodel) {
    view_event((view_event_t){.code = VIEW_EVENT_CODE_STATE_UPDATE});

    controller_update_fan_percentage(pmodel, model_get_fan_speed(pmodel));
    controller_update_class_output(pmodel, DEVICE_CLASS_SIPHONING_FAN, 1);
    controller_update_class_output(pmodel, DEVICE_CLASS_IMMISSION_FAN, 1);
    controller_update_class_output(pmodel, DEVICE_CLASS_GAS, 1);
    return 0;
}


static int fan_running_event_manager(model_t *pmodel, state_event_code_t event) {
    switch (event) {
        case STATE_EVENT_FAN_STOP:
            if (model_get_class_count(pmodel, DEVICE_CLASS_IMMISSION_FAN)) {
                cleaning_period = model_get_environment_cleaning_finish_period(pmodel);
                return MODEL_FAN_STATE_IF_ENV_CLEANING;
            } else {
                return MODEL_FAN_STATE_OFF;
            }

        case STATE_EVENT_FAN_EMERGENCY_STOP:
            return MODEL_FAN_STATE_OFF;

        default:
            break;
    }

    return -1;
}


static void update_uvc_filters(model_t *pmodel, uint8_t fan_speed, uint8_t value) {
    uint16_t filters = model_get_uvc_filters_for_speed(pmodel, fan_speed);

    if (filters > 0) {
        controller_update_class_output(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER(filters - 1), value);
    }

    for (size_t i = DEVICE_GROUP_1; i <= DEVICE_GROUP_2; i++) {
        if (i != filters - 1) {
            // Only send the broadcast message; some devices may fail to receive it but in this case it's not essential
            modbus_set_class_output(DEVICE_CLASS_ULTRAVIOLET_FILTER(i), 0);
        }
    }
}