#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <assert.h>
#include <stdint.h>
#include "address_map.h"
#include "device.h"
#include "gel/collections/queue.h"


#define NUM_PARAMETERS    3
#define MAX_FAN_SPEED     4
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


QUEUE_DECLARATION(alarms_queue, uint8_t, 256);

typedef enum {
    MODEL_FAN_STATE_OFF = 0,
    MODEL_FAN_STATE_SF_ENV_CLEANING,
    MODEL_FAN_STATE_SF_IF_ENV_CLEANING,
    MODEL_FAN_STATE_IF_ENV_CLEANING,
    MODEL_FAN_STATE_FAN_RUNNING,
} model_fan_state_t;


typedef struct {
    int temperature;
    int electrostatic_filter_on;
    int uvc_filter_on;

    model_fan_state_t state;

    struct {
        uint16_t language;
        uint16_t active_backlight;
        uint16_t buzzer_volume;
        uint8_t  use_fahrenheit;
    } configuration;
    uint8_t fan_speed;

    device_t devices[MODBUS_MAX_DEVICES];

    struct alarms_queue alarms;
} model_t;


void              model_init(model_t *model);
int               model_get_temperature(model_t *pmodel);
uint8_t           model_get_available_address(model_t *pmodel, uint8_t previous);
uint8_t           model_get_next_device_address(model_t *pmodel, uint8_t previous);
uint8_t           model_get_prev_device_address(model_t *pmodel, uint8_t next);
address_map_t     model_get_address_map(model_t *pmodel);
address_map_t     model_get_error_map(model_t *pmodel);
int               model_new_device(model_t *pmodel, uint8_t address);
size_t            model_get_configured_devices(model_t *pmodel);
int               model_is_address_configured(model_t *pmodel, uint8_t address);
void              model_delete_device(model_t *pmodel, uint8_t address);
void              model_get_device(model_t *pmodel, device_t *device, uint8_t address);
uint8_t           model_set_device_error(model_t *pmodel, uint8_t address, int error);
void              model_set_device_sn(model_t *pmodel, uint8_t address, uint16_t serial_number);
void              model_set_device_class(model_t *pmodel, uint8_t address, uint16_t class);
uint8_t           model_set_device_alarms(model_t *pmodel, uint8_t address, uint16_t alarms);
int               model_get_light_class(model_t *pmodel, uint16_t *class);
uint8_t           model_get_next_device_address_by_class(model_t *pmodel, uint8_t previous, uint16_t class);
int               model_all_devices_queried(model_t *pmodel);
size_t            model_get_class_count(model_t *pmodel, uint16_t class);
model_fan_state_t model_get_fan_state(model_t *pmodel);
void              model_set_filter_state(model_t *pmodel, model_fan_state_t state);
int               model_get_uvc_filter_state(model_t *pmodel);
int               model_get_electrostatic_filter_state(model_t *pmodel);
void              model_electrostatic_filter_toggle(model_t *pmodel);
void              model_uvc_filter_toggle(model_t *pmodel);
void              model_uvc_filter_on(model_t *pmodel);
void              model_add_new_alarm(model_t *pmodel, uint8_t address);
void              model_set_device_firmware(model_t *pmodel, uint8_t address, uint16_t firmware_version);
const char       *model_get_degrees_symbol(model_t *pmodel);

GETTERNSETTER(fan_speed, fan_speed);
GETTERNSETTER(active_backlight, configuration.active_backlight);
GETTERNSETTER(language, configuration.language);
GETTERNSETTER(buzzer_volume, configuration.buzzer_volume);
GETTERNSETTER(use_fahrenheit, configuration.use_fahrenheit);

#endif