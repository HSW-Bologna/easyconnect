#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED


#include "model/model.h"


void    server_init(void);
void    server_start(void);
void    server_stop(void);
void    server_notify_state_change(model_t *pmodel);
uint8_t server_is_there_a_new_connection(void);


#endif
