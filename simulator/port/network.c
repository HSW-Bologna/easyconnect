#include "services/network.h"
#include "model/model.h"
#include "services/server.h"


void network_init(void) {}


int network_get_scan_result(model_t *pmodel) {
    return 1;
}


int network_is_connected(uint32_t *ip, char *ssid) {
    return 0;
}


wifi_state_t network_get_wifi_state(void) {
    return WIFI_STATE_DISCONNECTED;
}


void network_stop(void) {}


void network_start_sta(void) {}


void network_scan_access_points(uint8_t channel) {}


void network_connect_to(char *ssid, char *psk) {}

void server_init(void) {}
void server_start(void) {}
void server_stop(void) {}

uint8_t server_is_there_a_new_connection(void) {
    return 0;
}


void server_notify_state_change(model_t *pmodel) {
    (void)pmodel;
}

void network_get_current_rssi(model_t *pmodel) {}
