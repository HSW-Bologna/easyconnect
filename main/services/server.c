#include <unistd.h>
#include <time.h>
#include <esp_log.h>
#include <errno.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "easyconnect_interface.h"
#include "esp_wifi_types.h"
#include "model/model.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <assert.h>
#include <esp_http_server.h>
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "utils/utils.h"
#include "config/app_config.h"
#include "server.h"
#include "controller/controller.h"
#include "controller/configuration.h"
#include "controller/log.h"
#include "view/view_types.h"
#include "ws_list.h"
#include <esp_ota_ops.h>


#define JSON_UPDATE      "update"
#define JSON_DEVICES     "devices"
#define JSON_ADDRESS     "ip"
#define JSON_SERIAL      "sn"
#define JSON_CLASS       "class"
#define JSON_ALARMS      "alarm"
#define JSON_STATE       "out"
#define JSON_FEEDBACK    "fb"
#define JSON_PRESSURE    "press"
#define JSON_TEMPERATURE "temp"
#define JSON_HUMIDITY    "hum"
#define JSON_SPEED       "speed"
#define JSON_FAN         "fan"
#define JSON_FILTER      "filt"
#define JSON_LOGS        "logs"


#define OK_RESPONSE "{\"status\":\"ok\"}\n"

#define WS_BUFFER_SIZE 2048

static void notify_connected_clients(void *arg);


static const char *TAG = "Http Server";

static uint32_t NEW_CONNECTION_EVENT = 0x0001;

static httpd_handle_t     server = NULL;
static EventGroupHandle_t event_group;


void server_init(void) {
    event_group = xEventGroupCreate();
}


uint8_t server_is_there_a_new_connection(void) {
    uint8_t res = (xEventGroupGetBits(event_group) & NEW_CONNECTION_EVENT) > 0;
    xEventGroupClearBits(event_group, NEW_CONNECTION_EVENT);
    return res;
}


static esp_err_t state_websocket_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Websocket method %i", req->method);
    if (req->method == HTTP_GET) {
        int fd = httpd_req_to_sockfd(req);
        ESP_LOGI(TAG, "New client subscribed: %i", fd);
        ws_list_activate(fd);
        xEventGroupSetBits(event_group, NEW_CONNECTION_EVENT);
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt = {0};
    uint8_t         *buffer = NULL;
    ws_pkt.type             = HTTPD_WS_TYPE_TEXT;

    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }

    if (ws_pkt.len) {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buffer = calloc(1, ws_pkt.len + 1);
        if (buffer == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buffer;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buffer);
            return ret;
        }

        cJSON *json = cJSON_Parse((const char *)ws_pkt.payload);

        if (json != NULL) {
            cJSON *json_logs = cJSON_GetObjectItem(json, JSON_LOGS);
            if (cJSON_IsNumber(json_logs)) {
                log_t  logs[50] = {0};
                size_t from     = cJSON_GetNumberValue(json_logs);
                size_t num      = log_read(logs, from, 50);
                ESP_LOGI(TAG, "Read %zu logs from %zu", num, from);

                cJSON *json_response = cJSON_CreateObject();
                assert(json_response != NULL);

                cJSON *json_logs = cJSON_CreateArray();
                assert(json_logs != NULL);

                for (size_t i = 0; i < num; i++) {
                    cJSON *json_item = cJSON_CreateObject();
                    assert(json_item != NULL);

                    cJSON_AddNumberToObject(json_item, "ts", logs[i].timestamp);
                    cJSON_AddNumberToObject(json_item, "code", logs[i].code);
                    cJSON_AddNumberToObject(json_item, "desc", logs[i].description);
                    cJSON_AddNumberToObject(json_item, "addr", logs[i].target_address);

                    cJSON_AddItemToArray(json_logs, json_item);
                }

                cJSON_AddItemToObject(json_response, "logs", json_logs);

                char *message = cJSON_PrintUnformatted(json_response);
                ESP_LOGI(TAG, "Sending %s", message);

                httpd_ws_frame_t ws_pkt = {0};
                ws_pkt.payload          = (uint8_t *)message;
                ws_pkt.len              = strlen(message);
                ws_pkt.type             = HTTPD_WS_TYPE_TEXT;

                httpd_ws_send_frame_async(server, httpd_req_to_sockfd(req), &ws_pkt);

                cJSON_Delete(json_response);
                free(message);
            }

            cJSON *json_fan = cJSON_GetObjectItem(json, JSON_FAN);
            if (cJSON_IsBool(json_fan)) {
                controller_send_message((view_controller_message_t){.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FAN});
            }

            cJSON *json_speed = cJSON_GetObjectItem(json, JSON_SPEED);
            if (cJSON_IsNumber(json_speed)) {
                controller_send_message((view_controller_message_t){
                    .code  = VIEW_CONTROLLER_MESSAGE_CODE_CHANGE_SPEED,
                    .speed = cJSON_GetNumberValue(json_speed),
                });
            }

            cJSON *json_filter = cJSON_GetObjectItem(json, JSON_FILTER);
            if (cJSON_IsBool(json_filter)) {
                controller_send_message(
                    (view_controller_message_t){.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FILTER});
            }

            cJSON_Delete(json);
        } else {
            ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
        }
        free(buffer);
    }
    ESP_LOGD(TAG, "Packet type: %d", ws_pkt.type);
    ESP_LOGD(TAG, "Packet content: %s", ws_pkt.payload);

    return ret;
}


static esp_err_t home_get_handler(httpd_req_t *req) {
    extern const unsigned char index_html_start[] asm("_binary_index_html_gzip_start");
    extern const unsigned char index_html_end[] asm("_binary_index_html_gzip_end");
    const size_t               index_html_size = (index_html_end - index_html_start);

    httpd_resp_set_hdr(req, "Content-encoding", "gzip");
    httpd_resp_send(req, (const char *)index_html_start, index_html_size);

    return ESP_OK;
}


void close_cb(httpd_handle_t hd, int sockfd) {
    ESP_LOGI(TAG, "Deactivating websocket %i", sockfd);
    ws_list_deactivate(sockfd);
    close(sockfd);
}


esp_err_t open_cb(httpd_handle_t hd, int sockfd) {
    ESP_LOGI(TAG, "Opening websocket %i", sockfd);
    return ESP_OK;
}


/* Function for starting the webserver */
void server_start(void) {
    if (server != NULL) {
        ESP_LOGW(TAG, "Server was already started!");
        return;
    }

    httpd_config_t config   = HTTPD_DEFAULT_CONFIG();
    config.task_priority    = 5;
    config.max_open_sockets = 3;
    config.close_fn         = close_cb;
    config.open_fn          = open_cb;
    config.stack_size       = BASE_TASK_SIZE * 4;
    config.lru_purge_enable = true;
    config.uri_match_fn     = httpd_uri_match_wildcard;
    config.max_uri_handlers = 20;

    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_uri_t home = {
            .uri     = "/",
            .method  = HTTP_GET,
            .handler = home_get_handler,
        };
        httpd_register_uri_handler(server, &home);

        static const httpd_uri_t ws = {
            .uri          = "/ws",
            .method       = HTTP_GET,
            .handler      = state_websocket_handler,
            .user_ctx     = NULL,
            .is_websocket = true,
        };
        httpd_register_uri_handler(server, &ws);
    }
    /* If server failed to start, handle will be NULL */

    ESP_LOGI(TAG, "Started");
}


/* Function for stopping the webserver */
void server_stop(void) {
    if (server) {
        ESP_LOGI(TAG, "Stopping the server");
        /* Stop the httpd server */
        httpd_stop(server);
        server = NULL;
    }
}



int server_is_anyone_connected(void) {
    if (server != NULL) {
        for (size_t i = 0; i < CONFIG_LWIP_MAX_SOCKETS; i++) {
            if (ws_list_is_active(i)) {
                ESP_LOGD(TAG, "Found active connection");
                return 1;
            }
        }
        ESP_LOGD(TAG, "No active connections");
        return 0;
    } else {
        return 0;
    }
}


static void notify_connected_clients(void *arg) {
    char *message = cJSON_PrintUnformatted(arg);

    if (server == NULL || arg == NULL) {
        free(message);
        return;
    }

    for (size_t i = 0; i < CONFIG_LWIP_MAX_SOCKETS; i++) {
        int ws_fd = ws_list_get_fd(i);

        if (ws_fd > 0) {
            ESP_LOGD(TAG, "notifica connessione %i: %s", ws_fd, message);
            httpd_ws_frame_t ws_pkt = {0};
            ws_pkt.payload          = (uint8_t *)message;
            ws_pkt.len              = strlen(message);
            ws_pkt.type             = HTTPD_WS_TYPE_TEXT;

            httpd_ws_send_frame_async(server, ws_fd, &ws_pkt);
        }
    }

    free(message);
    cJSON_Delete(arg);
}


void server_notify_state_change(model_t *pmodel) {
#define CHECK_ALLOC(x)                                                                                                 \
    if (x == NULL) {                                                                                                   \
        cJSON_Delete(json);                                                                                            \
        httpd_queue_work(server, notify_connected_clients, NULL);                                                      \
        return;                                                                                                        \
    }

    if (server == NULL || !server_is_anyone_connected()) {
        return;
    }

    cJSON *json = cJSON_CreateObject();
    CHECK_ALLOC(json);

    cJSON *json_update = cJSON_AddObjectToObject(json, JSON_DEVICES);
    CHECK_ALLOC(json_update);

    cJSON_AddNumberToObject(json_update, JSON_SPEED, model_get_fan_speed(pmodel));

    cJSON *json_devices = cJSON_AddArrayToObject(json, JSON_DEVICES);
    CHECK_ALLOC(json_devices);

    uint8_t starting_address = 0;
    uint8_t address          = model_get_next_device_address(pmodel, starting_address);
    while (address != starting_address) {
        device_t device = model_get_device(pmodel, address);

        starting_address = address;
        address          = model_get_next_device_address(pmodel, starting_address);

        cJSON *json_device = cJSON_CreateObject();
        if (json_device == NULL) {
            break;
        }

        CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_ADDRESS, device.address));
        CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_SERIAL, device.serial_number));
        CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_CLASS, device.class));
        CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_ALARMS, device.alarms));

        switch (CLASS_GET_MODE(device.class)) {
            case DEVICE_MODE_LIGHT:
            case DEVICE_MODE_GAS:
                CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_STATE, device.actuator_data.ourput_state));
                break;

            case DEVICE_MODE_UVC:
            case DEVICE_MODE_ESF:
                CHECK_ALLOC(
                    cJSON_AddNumberToObject(json_device, JSON_STATE, (device.alarms & EASYCONNECT_SAFETY_ALARM) > 0));
                CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_FEEDBACK,
                                                    (device.alarms & EASYCONNECT_FEEDBACK_ALARM) > 0));
                break;

            case DEVICE_MODE_FAN:
                CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_STATE, device.actuator_data.ourput_state & 0xFF));
                CHECK_ALLOC(
                    cJSON_AddNumberToObject(json_device, JSON_SPEED, (device.actuator_data.ourput_state >> 8) & 0xFF));
                break;

            case DEVICE_MODE_PRESSURE:
                CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_PRESSURE, device.sensor_data.pressure));
                CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_TEMPERATURE, device.sensor_data.temperature));
                break;

            case DEVICE_MODE_TEMPERATURE_HUMIDITY:
                CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_TEMPERATURE, device.sensor_data.temperature));
                CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_HUMIDITY, device.sensor_data.humidity));
                break;

            case DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY:
                CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_PRESSURE, device.sensor_data.pressure));
                CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_TEMPERATURE, device.sensor_data.temperature));
                CHECK_ALLOC(cJSON_AddNumberToObject(json_device, JSON_HUMIDITY, device.sensor_data.humidity));
                break;
        }

        cJSON_AddItemToArray(json_devices, json_device);
    }

    // TODO: report data

    httpd_queue_work(server, notify_connected_clients, json);
#undef CHECK_ALLOC
}
