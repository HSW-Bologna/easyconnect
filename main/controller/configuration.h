#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED


#include "model/model.h"


void configuration_save(model_t *pmodel);
void configuration_load(model_t *pmodel);
void configuration_manage(void);
void configuration_save_serial_number(void *args, uint32_t value);
void configuration_save_device_data(device_t device);
void configuration_load_device_data(device_t *device);


#endif
