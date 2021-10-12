#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include "model/model.h"
#include "view/view.h"


void controller_init(model_t *model);
void controller_manage_message(model_t *pmodel, view_controller_message_t *msg);
void controller_manage(model_t *pmodel);
void controller_update_class_output(model_t *pmodel, device_class_t class, int value);


#endif