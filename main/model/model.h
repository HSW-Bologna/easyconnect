#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <assert.h>
#include <stdint.h>
#include "address_map.h"
#include "device.h"
#include "gel/collections/queue.h"


#define NUM_PARAMETERS    3
#define MAX_FAN_SPEED     5
#define TOP_FAN_SPEED     4
#define MAX_BUZZER_VOLUME 3

#define GETTER(name, field)                                                                                            \
    static inline                                                                                                      \
        __attribute__((always_inline, const)) typeof(((model_t *)0)->field) model_get_##name(model_t *pmodel) {        \
        assert(pmodel != NULL);                                                                                        \
        return pmodel->field;                                                                                          \
    }

#define SETTER(name, field)                                                                                            \
    static inline                                                                                                      \
        __attribute__((always_inline)) void model_set_##name(model_t *pmodel, typeof(((model_t *)0)->field) value) {   \
        assert(pmodel != NULL);                                                                                        \
        pmodel->field = value;                                                                                         \
    }

#define GETTERNSETTER(name, field)                                                                                     \
    GETTER(name, field)                                                                                                \
    SETTER(name, field)


typedef enum {
    MODEL_FAN_STATE_OFF = 0,
    MODEL_FAN_STATE_SF_ENV_CLEANING,
    MODEL_FAN_STATE_SF_IF_ENV_CLEANING,
    MODEL_FAN_STATE_IF_ENV_CLEANING,
    MODEL_FAN_STATE_FAN_RUNNING,
} model_fan_state_t;


typedef enum {
    LIGHT_STATE_OFF        = 0,
    LIGHT_STATE_SINGLE_ON  = 1,
    LIGHT_STATE_GROUP_1_ON = 1,
    LIGHT_STATE_GROUP_2_ON = 2,
    LIGHT_STATE_DOUBLE_ON  = 3,
    LIGHT_STATE_TRIPLE_ON  = 3,
} light_state_t;


typedef struct {
    int temperature;
    int electrostatic_filter_on;
    int uvc_filter_on;

    model_fan_state_t state;
    light_state_t     light_state;

    struct {
        uint16_t language;
        uint16_t active_backlight;
        uint16_t buzzer_volume;
        uint8_t  use_fahrenheit;
        uint16_t pressure_differences_for_speed[MAX_FAN_SPEED];
        uint8_t  pressure_difference_deviation_warn;
        uint8_t  pressure_difference_deviation_stop;
        uint8_t  uvc_filters_for_speed[MAX_FAN_SPEED];
        uint8_t  esf_filters_for_speed[MAX_FAN_SPEED];
        uint16_t environment_cleaning_start_period;      // In seconds
        uint16_t environment_cleaning_finish_period;     // In seconds
        uint8_t  siphoning_percentages[MAX_FAN_SPEED];
        uint8_t  immission_percentages[MAX_FAN_SPEED];
        uint16_t pressure_threshold_mb;
        uint16_t passive_filters_hours_warning_threshold;
        uint16_t passive_filters_hours_stop_threshold;
        uint16_t num_passive_filters;
        uint16_t uvc_filters_hours_warning_threshold;
        uint16_t uvc_filters_hours_stop_threshold;
        uint16_t esf_filters_hours_warning_threshold;
        uint16_t esf_filters_hours_stop_threshold;

        uint8_t first_humidity_delta;
        uint8_t second_humidity_delta;
        uint8_t first_humidity_speed_raise;
        uint8_t second_humidity_speed_raise;
        uint8_t humidity_warn;
        uint8_t humidity_stop;

        uint16_t first_temperature_delta;
        uint16_t second_temperature_delta;
        uint8_t  first_temperature_speed_raise;
        uint8_t  second_temperature_speed_raise;
        uint16_t temperature_warn;
        uint16_t temperature_stop;
    } configuration;

    struct {
        uint32_t passive_filters_work_seconds;
    } stats;

    uint8_t fan_speed;

    device_t devices[MODBUS_MAX_DEVICES];
} model_t;


void          model_init(model_t *model);
int           model_get_temperature(model_t *pmodel);
uint8_t       model_get_available_address(model_t *pmodel, uint8_t previous);
uint8_t       model_get_next_device_address(model_t *pmodel, uint8_t previous);
uint8_t       model_get_prev_device_address(model_t *pmodel, uint8_t next);
address_map_t model_get_address_map(model_t *pmodel);
address_map_t model_get_error_map(model_t *pmodel);
int           model_new_device(model_t *pmodel, uint8_t address);
size_t        model_get_configured_devices(model_t *pmodel);
int           model_is_address_configured(model_t *pmodel, uint8_t address);
void          model_delete_device(model_t *pmodel, uint8_t address);
device_t      model_get_device(model_t *pmodel, uint8_t address);
uint8_t       model_set_device_error(model_t *pmodel, uint8_t address, int error);
void          model_set_device_sn(model_t *pmodel, uint8_t address, uint32_t serial_number);
void          model_set_device_class(model_t *pmodel, uint8_t address, uint16_t class);
uint8_t       model_set_device_alarms(model_t *pmodel, uint8_t address, uint16_t alarms);
void          model_set_device_state(model_t *pmodel, uint8_t address, uint16_t state);
void          model_set_device_work_hours(model_t *pmodel, uint8_t address, uint16_t work_hours);
int           model_get_light_class(model_t *pmodel, uint16_t *class);
uint8_t       model_get_next_device_address_by_class(model_t *pmodel, uint8_t previous, uint16_t class);
uint8_t       model_get_next_device_address_by_mode(model_t *pmodel, uint8_t previous, uint16_t mode);
uint8_t       model_get_next_device_address_by_modes(model_t *pmodel, uint8_t previous, uint16_t *modes, size_t num);
uint8_t       model_get_next_pressure_sensor_device(model_t *pmodel, uint8_t previous);
int           model_all_devices_queried(model_t *pmodel);
size_t        model_get_class_count(model_t *pmodel, uint16_t class);
size_t        model_get_mode_count(model_t *pmodel, uint16_t mode);
model_fan_state_t model_get_fan_state(model_t *pmodel);
void              model_set_fan_state(model_t *pmodel, model_fan_state_t state);
int               model_get_uvc_filter_state(model_t *pmodel);
int               model_get_electrostatic_filter_state(model_t *pmodel);
void              model_electrostatic_filter_toggle(model_t *pmodel);
void              model_uvc_filter_toggle(model_t *pmodel);
void              model_uvc_filter_on(model_t *pmodel);
void              model_uvc_filter_off(model_t *pmodel);
void              model_electrostatic_filter_off(model_t *pmodel);
void              model_set_device_firmware(model_t *pmodel, uint8_t address, uint16_t firmware_version);
const char       *model_get_degrees_symbol(model_t *pmodel);
uint8_t           model_is_there_an_alarm(model_t *pmodel);
uint8_t           model_is_there_any_alarm_for_class(model_t *pmodel, uint16_t class);
uint8_t           model_is_filter_alarm_on(model_t *pmodel, uint8_t alarms);
uint8_t           model_is_there_a_fan_alarm(model_t *pmodel);
void              model_set_device_pressure(model_t *pmodel, uint8_t address, uint16_t pressure);
device_t         *model_get_device_mut(model_t *pmodel, uint8_t address);
void              model_light_switch(model_t *model);
uint8_t           model_get_siphoning_percentage(model_t *pmodel, uint8_t fan_speed);
void              model_set_siphoning_percentage(model_t *pmodel, uint8_t fan_speed, uint8_t percentage);
uint8_t           model_get_immission_percentage(model_t *pmodel, uint8_t fan_speed);
void              model_set_immission_percentage(model_t *pmodel, uint8_t fan_speed, uint8_t percentage);
uint8_t           model_get_uvc_filters_for_speed(model_t *pmodel, uint8_t fan_speed);
void              model_set_uvc_filters_for_speed(model_t *pmodel, uint8_t fan_speed, uint8_t filters);
uint8_t           model_get_esf_filters_for_speed(model_t *pmodel, uint8_t fan_speed);
void              model_set_esf_filters_for_speed(model_t *pmodel, uint8_t fan_speed, uint8_t filters);
void     model_get_alarms_for_classes(model_t *pmodel, uint8_t *uvc, uint8_t *esf, uint8_t *siph, uint8_t *imm);
void     model_reset_passive_filters_work_hours(model_t *pmodel);
void     model_add_passive_filters_work_seconds(model_t *pmodel, uint32_t seconds);
uint16_t model_get_passive_filters_remaining_hours(model_t *pmodel);
uint8_t  model_get_passive_filter_warning(model_t *pmodel);
uint8_t  model_get_passive_filter_stop(model_t *pmodel);
uint16_t model_get_passive_filters_work_hours(model_t *pmodel);
uint16_t model_get_filter_device_work_hours(model_t *pmodel, uint8_t address);
uint16_t model_get_filter_device_remaining_hours(model_t *pmodel, uint8_t address);
uint8_t  model_get_filter_device_warning(model_t *pmodel, uint8_t address);
uint16_t model_get_pressure_difference(model_t *pmodel, uint8_t fan_speed);
void     model_set_pressure_difference(model_t *pmodel, uint8_t fan_speed, uint16_t difference);

GETTERNSETTER(light_state, light_state);
GETTERNSETTER(fan_speed, fan_speed);
GETTERNSETTER(active_backlight, configuration.active_backlight);
GETTERNSETTER(language, configuration.language);
GETTERNSETTER(buzzer_volume, configuration.buzzer_volume);
GETTERNSETTER(use_fahrenheit, configuration.use_fahrenheit);
GETTERNSETTER(environment_cleaning_start_period, configuration.environment_cleaning_start_period);
GETTERNSETTER(environment_cleaning_finish_period, configuration.environment_cleaning_finish_period);
GETTERNSETTER(pressure_threshold_mb, configuration.pressure_threshold_mb);
GETTERNSETTER(passive_filters_hours_warning_threshold, configuration.passive_filters_hours_warning_threshold);
GETTERNSETTER(passive_filters_hours_stop_threshold, configuration.passive_filters_hours_stop_threshold);
GETTERNSETTER(num_passive_filters, configuration.num_passive_filters);
GETTERNSETTER(uvc_filters_hours_warning_threshold, configuration.uvc_filters_hours_warning_threshold);
GETTERNSETTER(uvc_filters_hours_stop_threshold, configuration.uvc_filters_hours_stop_threshold);
GETTERNSETTER(esf_filters_hours_warning_threshold, configuration.esf_filters_hours_warning_threshold);
GETTERNSETTER(esf_filters_hours_stop_threshold, configuration.esf_filters_hours_stop_threshold);
GETTERNSETTER(pressure_difference_deviation_warn, configuration.pressure_difference_deviation_warn);
GETTERNSETTER(pressure_difference_deviation_stop, configuration.pressure_difference_deviation_stop);

GETTERNSETTER(first_humidity_delta, configuration.first_humidity_delta);
GETTERNSETTER(second_humidity_delta, configuration.second_humidity_delta);
GETTERNSETTER(first_humidity_speed_raise, configuration.first_humidity_speed_raise);
GETTERNSETTER(second_humidity_speed_raise, configuration.second_humidity_speed_raise);
GETTERNSETTER(humidity_warn, configuration.humidity_warn);
GETTERNSETTER(humidity_stop, configuration.humidity_stop);

GETTERNSETTER(first_temperature_delta, configuration.first_temperature_delta);
GETTERNSETTER(second_temperature_delta, configuration.second_temperature_delta);
GETTERNSETTER(first_temperature_speed_raise, configuration.first_temperature_speed_raise);
GETTERNSETTER(second_temperature_speed_raise, configuration.second_temperature_speed_raise);
GETTERNSETTER(temperature_warn, configuration.temperature_warn);
GETTERNSETTER(temperature_stop, configuration.temperature_stop);

#endif