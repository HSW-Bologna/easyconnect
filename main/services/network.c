#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "network.h"
#include "app_config.h"
#include "server.h"
#include "model/model.h"
#include "mdns.h"


#define ATTEMPTS 5


static void      event_handler(void *arg, esp_event_base_t event_base, int event_id, void *event_data);
static void      network_connected(uint32_t ip);
static void      network_disconnected(void);
static void      network_connecting(wifi_err_reason_t error_reason);
static void      reconnect_wifi_cb(TimerHandle_t handle);
static esp_err_t scan_networks(uint8_t channel);


static const int WIFI_CONNECTED_EVENT = BIT0;
static const int WIFI_STOPPED_EVENT   = BIT1;
static const int AP_SCAN_DONE         = BIT4;

static const char *TAG = "Network";

static EventGroupHandle_t wifi_event_group               = NULL;
static SemaphoreHandle_t  sem                            = NULL;
static TimerHandle_t      timer                          = NULL;
static uint32_t           ip_address                     = 0;
static wifi_state_t       wifi_state                     = WIFI_STATE_DISCONNECTED;
static uint16_t           ap_list_count                  = 0;
static wifi_ap_record_t   ap_list[MAX_AP_SCAN_LIST_SIZE] = {0};
static int                request_scan_while_connecting  = 0;
static uint8_t            wifi_scan_channel              = 0;


void network_init(void) {
    static StaticEventGroup_t event_group_buffer;
    wifi_event_group = xEventGroupCreateStatic(&event_group_buffer);
    static StaticSemaphore_t semaphore_buffer;
    sem = xSemaphoreCreateMutexStatic(&semaphore_buffer);
    static StaticTimer_t timer_buffer;
    timer = xTimerCreateStatic("Wifi", pdMS_TO_TICKS(500), 1, NULL, reconnect_wifi_cb, &timer_buffer);

    /* Initialize networking stack */
    ESP_ERROR_CHECK(esp_netif_init());
    /* Create default event loop needed by the
     * main app and the provisioning service */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // initialize mDNS
    ESP_ERROR_CHECK(mdns_init());
    // set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK(mdns_hostname_set(APP_CONFIG_MDNS_HOSTNAME));
    ESP_LOGI(TAG, "mdns hostname set to: [%s]", APP_CONFIG_MDNS_HOSTNAME);
    // set default mDNS instance name
    ESP_ERROR_CHECK(mdns_instance_name_set(APP_CONFIG_MDNS_HOSTNAME));
    // structure with TXT records
    mdns_txt_item_t serviceTxtData[1] = {
        {"board", "esp32"},
    };

    // initialize service
    ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData, 1));

    /* Initialize Wi-Fi with default config */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Set our event handling */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL));
    esp_netif_create_default_wifi_sta();
}


void network_connect_to(char *ssid, char *psk) {
    ESP_LOGI(TAG, "Trying to connect to %s, %s", ssid, psk);
    wifi_config_t config = {0};
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_get_config(WIFI_IF_STA, &config));
    strcpy((char *)config.sta.ssid, ssid);
    strcpy((char *)config.sta.password, psk);
    esp_wifi_set_config(WIFI_IF_STA, &config);
    esp_wifi_stop();
    xTimerStart(timer, portMAX_DELAY);
}


void network_start_sta(void) {
    /* Start Wi-Fi in station mode with credentials set during provisioning */
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_start());
}


void network_stop(void) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_stop());
}


void network_scan_access_points(uint8_t channel) {
    xEventGroupClearBits(wifi_event_group, AP_SCAN_DONE);
    if (scan_networks(channel) != ESP_OK) {
        ESP_LOGI(TAG, "Temporarily unable to scan");
        xSemaphoreTake(sem, portMAX_DELAY);
        request_scan_while_connecting = 1;
        wifi_scan_channel             = channel;
        xSemaphoreGive(sem);
    }
}


int network_get_scan_result(model_t *pmodel) {
    uint32_t res = xEventGroupWaitBits(wifi_event_group, AP_SCAN_DONE, pdFALSE, pdTRUE, 0);
    if ((res & AP_SCAN_DONE) > 0) {
        xSemaphoreTake(sem, portMAX_DELAY);
        for (size_t i = 0; i < ap_list_count; i++) {
            strcpy(pmodel->ap_list[i], (char *)ap_list[i].ssid);
            pmodel->ap_rssi[i] = ap_list[i].rssi;
        }
        model_set_available_networks_count(pmodel, ap_list_count);
        xSemaphoreGive(sem);
        return 1;
    } else {
        return 0;
    }
}


wifi_state_t network_get_wifi_state(void) {
    xSemaphoreTake(sem, portMAX_DELAY);
    wifi_state_t state = wifi_state;
    xSemaphoreGive(sem);
    return state;
}


int network_is_connected(uint32_t *ip, char *ssid) {
    xSemaphoreTake(sem, portMAX_DELAY);
    *ip     = ip_address;
    int res = wifi_state == WIFI_STATE_CONNECTED;
    xSemaphoreGive(sem);
    if ((wifi_state == WIFI_STATE_CONNECTED || wifi_state == WIFI_STATE_CONNECTING) && ssid != NULL) {
        wifi_config_t config = {0};
        esp_wifi_get_config(WIFI_IF_STA, &config);
        strcpy(ssid, (char *)config.sta.ssid);
    }
    return res;
}


void network_get_current_rssi(model_t *pmodel) {
    wifi_ap_record_t ap;
    if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) {
        model_set_current_network_rssi(pmodel, ap.rssi);
    } else {
        model_set_current_network_rssi(pmodel, -1000);
    }
}


uint8_t network_wait_for_wifi(TickType_t delay) {
    uint32_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_EVENT, pdFALSE, pdTRUE, delay);
    return (bits & WIFI_CONNECTED_EVENT) > 0;
}


static void event_handler(void *arg, esp_event_base_t event_base, int event_id, void *event_data) {
    static uint16_t attempts = 0;

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START: {
                wifi_mode_t mode = WIFI_MODE_STA;
                ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_get_mode(&mode));
                if (mode == WIFI_MODE_STA) {
                    ESP_LOGI(TAG, "STA started");
                    attempts = 0;
                    xSemaphoreTake(sem, portMAX_DELAY);
                    wifi_state = WIFI_STATE_CONNECTING;
                    xSemaphoreGive(sem);
                    esp_wifi_connect();
                }
                break;
            }

            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "Access Point started");
                xEventGroupClearBits(wifi_event_group, WIFI_STOPPED_EVENT);
                /* Default WiFi IP 192.168.4.1 of AP */
                network_connected(192UL | (168UL << 8) | (4UL << 16) | (1UL << 24));
                break;

            case WIFI_EVENT_AP_STOP:
                ESP_LOGI(TAG, "AP Stopped");
                network_disconnected();
                xEventGroupSetBits(wifi_event_group, WIFI_STOPPED_EVENT);
                break;

            case WIFI_EVENT_STA_STOP:
                ESP_LOGI(TAG, "STA Stopped");
                attempts = 0;
                network_disconnected();
                xEventGroupSetBits(wifi_event_group, WIFI_STOPPED_EVENT);
                break;

            case WIFI_EVENT_AP_STACONNECTED:
            case WIFI_EVENT_AP_STADISCONNECTED:
                ESP_LOGI(TAG, "Device connected to (or disconnected from) our AP");
                break;

            case WIFI_EVENT_SCAN_DONE: {
                uint16_t number = MAX_AP_SCAN_LIST_SIZE;
                xSemaphoreTake(sem, portMAX_DELAY);
                esp_err_t err = esp_wifi_scan_get_ap_records(&number, ap_list);
                if (err == ESP_OK) {
                    if (number >= MAX_AP_SCAN_LIST_SIZE) {
                        number = MAX_AP_SCAN_LIST_SIZE;
                    }

                    ap_list_count = number;
                } else {
                    number = 0;
                }

                int should_rescan             = request_scan_while_connecting;
                request_scan_while_connecting = 0;
                xSemaphoreGive(sem);

                if (should_rescan) {
                    // Retry indefinitely
                    network_connecting(WIFI_REASON_UNSPECIFIED);
                    esp_wifi_connect();
                }
                ESP_LOGI(TAG, "Wifi scan done, %i networks found", number);
                xEventGroupSetBits(wifi_event_group, AP_SCAN_DONE);
                break;
            }

            case WIFI_EVENT_STA_CONNECTED: {
                ESP_LOGI(TAG, "Connected to a network");
                attempts = 0;
                break;
            }

            case WIFI_EVENT_STA_DISCONNECTED: {
                wifi_event_sta_disconnected_t *disconnected = (wifi_event_sta_disconnected_t *)event_data;

                switch (disconnected->reason) {
                    case WIFI_REASON_AUTH_EXPIRE:
                    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
                    case WIFI_REASON_BEACON_TIMEOUT:
                    case WIFI_REASON_AUTH_FAIL:
                    case WIFI_REASON_ASSOC_FAIL:
                    case WIFI_REASON_HANDSHAKE_TIMEOUT:
                        ESP_LOGW(TAG, "connect to the AP fail : auth Error.");
                        break;

                    case WIFI_REASON_NO_AP_FOUND:
                        ESP_LOGW(TAG, "connect to the AP fail : not found.");
                        break;

                    default:
                        /* None of the expected reasons */
                        break;
                }

                xSemaphoreTake(sem, portMAX_DELAY);
                int     should_scan = request_scan_while_connecting;
                uint8_t channel     = wifi_scan_channel;
                xSemaphoreGive(sem);
                attempts++;

                if (should_scan) {
                    ESP_LOGI(TAG, "Putting connection efforts aside to scan");
                    network_disconnected();
                    scan_networks(channel);
                } else {
                    // Retry indefinitely
                    ESP_LOGI(TAG, "Retrying...");
                    network_connecting(disconnected->reason);
                    esp_wifi_connect();
                }
                break;
            }

            default:
                ESP_LOGI(TAG, "Unknown WiFi event %i", event_id);
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP: {
                ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
                ESP_LOGI(TAG, "Got ip:%s", ip4addr_ntoa((const ip4_addr_t *)&event->ip_info.ip));

                xEventGroupClearBits(wifi_event_group, WIFI_STOPPED_EVENT);
                network_connected(event->ip_info.ip.addr);
                break;
            }

            default:
                ESP_LOGI(TAG, "Unknown IP event %i", event_id);
                break;
        }
    }
}


static void network_connected(uint32_t ip) {
    xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_EVENT);
    xSemaphoreTake(sem, portMAX_DELAY);
    ip_address = ip;
    wifi_state = WIFI_STATE_CONNECTED;
    xSemaphoreGive(sem);
}


static void network_disconnected(void) {
    xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_EVENT);
    xSemaphoreTake(sem, portMAX_DELAY);
    wifi_state = WIFI_STATE_DISCONNECTED;
    xSemaphoreGive(sem);
}


static void network_connecting(wifi_err_reason_t error_reason) {
    xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_EVENT);
    xSemaphoreTake(sem, portMAX_DELAY);
    switch (error_reason) {
        case WIFI_REASON_AUTH_EXPIRE:
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        case WIFI_REASON_BEACON_TIMEOUT:
        case WIFI_REASON_AUTH_FAIL:
        case WIFI_REASON_ASSOC_FAIL:
        case WIFI_REASON_HANDSHAKE_TIMEOUT:
            wifi_state = WIFI_STATE_AUTH_ERROR;
            break;
        case WIFI_REASON_NO_AP_FOUND:
            wifi_state = WIFI_STATE_SSID_NOT_FOUND_ERROR;
            break;
        default:
            wifi_state = WIFI_STATE_CONNECTING;
            break;
    }
    xSemaphoreGive(sem);
}


static void reconnect_wifi_cb(TimerHandle_t handle) {
    uint32_t res = xEventGroupWaitBits(wifi_event_group, WIFI_STOPPED_EVENT, pdFALSE, pdTRUE, 0);
    if ((res & WIFI_STOPPED_EVENT) > 0) {
        esp_wifi_start();
        xTimerStop(handle, portMAX_DELAY);
    }
}


static esp_err_t scan_networks(uint8_t channel) {
    wifi_scan_config_t config = {
        .ssid        = NULL,
        .bssid       = NULL,
        .channel     = channel,
        .show_hidden = 0,
        .scan_type   = WIFI_SCAN_TYPE_ACTIVE,
    };
    return esp_wifi_scan_start(&config, 0);
}
