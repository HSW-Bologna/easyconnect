#ifndef SENSOR_CALIBRATION_H_INCLUDED
#define SENSOR_CALIBRATION_H_INCLUDED


#include "model/model.h"


void sensors_calibration_manage(model_t *pmodel);
void sensors_calibration_new_reading(model_t *pmodel, uint8_t address, int16_t pressure);


#endif
