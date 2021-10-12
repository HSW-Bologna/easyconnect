#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include "model/model.h"


typedef enum {
    STATE_EVENT_FAN_START,
    STATE_EVENT_FAN_UVC_START,
    STATE_EVENT_ENVIRONMENTAL_CLEANING_DONE,
    STATE_EVENT_FAN_STOP,
} state_event_code_t;


void controller_state_event(model_t *pmodel, state_event_code_t event);
void controller_state_manage(model_t *pmodel);


#endif