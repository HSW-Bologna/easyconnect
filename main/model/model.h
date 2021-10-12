#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <stdint.h>
#include "address_map.h"
#include "device.h"


#define MODEL_MAX_DEVICES 255


typedef struct {
    int slider;

    int temperature;

    device_t devices[MODEL_MAX_DEVICES];
} model_t;


void          model_init(model_t *model);
int           model_get_temperature(model_t *pmodel);
uint8_t       model_get_available_address(model_t *pmodel, uint8_t previous);
uint8_t       model_get_next_device_address(model_t *pmodel, uint8_t previous);
uint8_t       model_get_prev_device_address(model_t *pmodel, uint8_t next);
address_map_t model_get_address_map(model_t *pmodel);
int           model_new_device(model_t *pmodel, uint8_t address);
size_t        model_get_configured_devices(model_t *pmodel);
int           model_is_address_configured(model_t *pmodel, uint8_t address);
void          model_delete_device(model_t *pmodel, uint8_t address);
void          model_get_device(model_t *pmodel, device_t *device, uint8_t address);
void          model_set_device_error(model_t *pmodel, uint8_t address);
void          model_set_device_class(model_t *pmodel, uint8_t address, uint8_t class);
void          model_set_device_sn(model_t *pmodel, uint8_t address, uint16_t serial_number);
int           model_get_light_class(model_t *pmodel, device_class_t *class);
uint8_t       model_get_next_device_address_by_class(model_t *pmodel, uint8_t previous, device_class_t class);
int           model_all_devices_queried(model_t *pmodel);
size_t        model_get_class_count(model_t *pmodel, device_class_t class);

#endif