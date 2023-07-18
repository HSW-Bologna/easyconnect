#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED


#include "model/model.h"


void configuration_save(model_t *pmodel);
void configuration_load(model_t *pmodel);
void configuration_manage(void);
void configuration_save_serial_number(void *args, uint32_t value);


#endif
