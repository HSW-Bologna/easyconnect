#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED


#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "model/model.h"


void         network_init(void);
void         network_stop(void);
void         network_scan_access_points(uint8_t channel);
void         network_stop(void);
void         network_start_sta(void);
int          network_get_scan_result(model_t *pmodel);
int          network_is_connected(uint32_t *ip, char *ssid);
uint8_t      network_wait_for_wifi(TickType_t delay);
void         network_connect_to(char *ssid, char *psk);
wifi_state_t network_get_wifi_state(void);


#endif
