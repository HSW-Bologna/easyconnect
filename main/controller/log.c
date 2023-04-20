#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "gel/serializer/serializer.h"

#include "log.h"
#include "config/app_config.h"


#define LOG_FILE_PATH           APP_CONFIG_SD_MOUNTPOINT "/log.bin"
#define LOG_POSITION_CACHE_SIZE 10
#define LOG_SERIALIZED_SIZE     13


static uint8_t  find_in_cache(uint32_t *position, uint32_t number);
static uint32_t read_logs_from(log_t *logs, uint32_t position, uint32_t num, FILE *fp);
static size_t   serialize_log(uint8_t *buffer, log_t log);
static size_t   deserialize_log(log_t *log, uint8_t *buffer);
static size_t   log_file_size(void);


static const char *TAG = "SdLog";

static SemaphoreHandle_t sem = NULL;


void log_init(void) {
    sem = xSemaphoreCreateMutex();
    ESP_LOGI(TAG, "Log initialized");
}


void log_event(log_event_code_t code, uint8_t target, uint16_t description) {
    xSemaphoreTake(sem, portMAX_DELAY);
    FILE *fp = fopen(LOG_FILE_PATH, "a");
    if (fp == NULL) {
        xSemaphoreGive(sem);
        ESP_LOGW(TAG, "Unable to open for writing %s: %s", LOG_FILE_PATH, strerror(errno));
        return;
    }

    log_t log = {
        .timestamp      = time(NULL),
        .code           = code,
        .description    = description,
        .target_address = target,
    };
    uint8_t buffer[LOG_SERIALIZED_SIZE];
    serialize_log(buffer, log);

    fwrite(buffer, sizeof(buffer), 1, fp);
    fclose(fp);

    xSemaphoreGive(sem);
}


uint32_t log_read(log_t *logs, uint32_t from, uint32_t num) {
    xSemaphoreTake(sem, portMAX_DELAY);
    FILE *fp = fopen(LOG_FILE_PATH, "r");
    if (fp == NULL) {
        xSemaphoreGive(sem);
        ESP_LOGW(TAG, "Unable to open %s: %s", LOG_FILE_PATH, strerror(errno));
        return 0;
    }

    size_t size     = log_file_size();
    size_t num_logs = size / LOG_SERIALIZED_SIZE;

    if (num_logs < from) {
        fclose(fp);
        xSemaphoreGive(sem);
        return 0;
    }

    if (num_logs - from < num) {
        // Cap to available logs
        num = num_logs - from;
    }

    uint32_t position = size - (from + num) * LOG_SERIALIZED_SIZE;

    // If the position is found, use it. Otherwise it needs to be searched linearly in the file
    uint32_t res = read_logs_from(logs, position, num, fp);
    fclose(fp);

    xSemaphoreGive(sem);
    return res;
}

static uint32_t read_logs_from(log_t *logs, uint32_t position, uint32_t num, FILE *fp) {
    fseek(fp, position, SEEK_SET);

    uint32_t count = 0;

    while (count < num) {
        uint8_t buffer[LOG_SERIALIZED_SIZE] = {0};
        if (fread(buffer, sizeof(buffer), 1, fp) < 1) {
            ESP_LOGW(TAG, "Unable to read %s: %s", LOG_FILE_PATH, strerror(errno));
            return count;
        }

        // logs should be saved in reverse order, from latest to oldest
        deserialize_log(&logs[num - 1 - count], buffer);
        count++;
    }

    return count;
}


static size_t serialize_log(uint8_t *buffer, log_t log) {
    size_t i = 0;
    i += serialize_uint64_be(&buffer[i], log.timestamp);
    i += serialize_uint16_be(&buffer[i], log.code);
    i += serialize_uint16_be(&buffer[i], log.description);
    i += serialize_uint8(&buffer[i], log.target_address);

    assert(i == LOG_SERIALIZED_SIZE);
    return i;
}


static size_t deserialize_log(log_t *log, uint8_t *buffer) {
    size_t i = 0;
    i += deserialize_uint64_be(&log->timestamp, &buffer[i]);
    i += deserialize_uint16_be(&log->code, &buffer[i]);
    i += deserialize_uint16_be(&log->description, &buffer[i]);
    i += deserialize_uint8(&log->target_address, &buffer[i]);

    assert(i == LOG_SERIALIZED_SIZE);
    return i;
}


static size_t log_file_size(void) {
    FILE *fp = fopen(LOG_FILE_PATH, "r");
    if (fp == NULL) {
        ESP_LOGW(TAG, "Unable to open %s: %s", LOG_FILE_PATH, strerror(errno));
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fclose(fp);

    return size;
}
