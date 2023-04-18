#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "log.h"
#include "config/app_config.h"


#define LOG_FILE_PATH           APP_CONFIG_SD_MOUNTPOINT "/log.csv"
#define LOG_POSITION_CACHE_SIZE 10


static uint8_t  find_in_cache(uint32_t *position, uint32_t number);
static uint32_t read_logs_from(log_t *logs, uint32_t position, uint32_t num, FILE *fp);


static const char *TAG = "SdLog";

static SemaphoreHandle_t sem = NULL;
static struct {
    uint32_t number;
    uint32_t position;
} log_position_cache[LOG_POSITION_CACHE_SIZE] = {0};
static size_t   log_position_cache_index      = 0;


void log_init(void) {
    sem = xSemaphoreCreateMutex();
    ESP_LOGI(TAG, "Log initialized");
}


void log_event(log_event_code_t code, uint8_t target, uint16_t description) {
    return;
    xSemaphoreTake(sem, portMAX_DELAY);
    FILE *fp = fopen(LOG_FILE_PATH, "a+");
    if (fp == NULL) {
        xSemaphoreGive(sem);
        ESP_LOGW(TAG, "Unable to open %s: %s", LOG_FILE_PATH, strerror(errno));
        return;
    }

    fprintf(fp, "%llu,%i,%i,%i\n", (unsigned long long)time(NULL), code, target, description);
    fclose(fp);
    xSemaphoreGive(sem);
}


uint32_t log_read(log_t *logs, uint32_t from, uint32_t num) {
    uint32_t position = 0;

    xSemaphoreTake(sem, portMAX_DELAY);
    FILE *fp = fopen(LOG_FILE_PATH, "r");
    if (fp == NULL) {
        xSemaphoreGive(sem);
        ESP_LOGW(TAG, "Unable to open %s: %s", LOG_FILE_PATH, strerror(errno));
        return 0;
    }

    // If the position is found, use it. Otherwise it needs to be searched linearly in the file
    if (!find_in_cache(&position, from)) {
        for (position = 0; position < from; position++) {
            char string[64] = {0};
            if (fgets(string, sizeof(string), fp) == 0) {
                // There requested logs aren't present
                fclose(fp);
                xSemaphoreGive(sem);
                return 0;
            }
        }

        // Update the cache
        log_position_cache[log_position_cache_index].number   = from;
        log_position_cache[log_position_cache_index].position = position;
        log_position_cache_index = (log_position_cache_index + 1) % LOG_POSITION_CACHE_SIZE;
    }

    uint32_t res = read_logs_from(logs, position, num, fp);
    fclose(fp);

    xSemaphoreGive(sem);
    return res;
}


static uint8_t find_in_cache(uint32_t *position, uint32_t number) {
    for (size_t i = 0; i < LOG_POSITION_CACHE_SIZE; i++) {
        if (log_position_cache[i].number == number) {
            *position = log_position_cache[i].position;
            return 1;
        }
    }

    return 0;
}


static uint32_t read_logs_from(log_t *logs, uint32_t position, uint32_t num, FILE *fp) {
#define PARSE_COLUMN(string, start, after, c, count, field)                                                            \
    {                                                                                                                  \
        char  csv_column[32] = {0};                                                                                    \
        char *before         = start;                                                                                  \
        after                = strchr(before, c);                                                                      \
        if (after == NULL) {                                                                                           \
            ESP_LOGW(TAG, "Invalid csv line: %s", string);                                                             \
            continue;                                                                                                  \
        }                                                                                                              \
        memcpy(csv_column, before, after - before);                                                                    \
        logs[count].field = atoll(csv_column);                                                                         \
    }

    fseek(fp, position, SEEK_SET);

    uint32_t count = 0;

    while (count < num) {
        char string[64] = {0};
        if (fgets(string, sizeof(string), fp) == 0) {
            ESP_LOGW(TAG, "Unable to read %s: %s", LOG_FILE_PATH, strerror(errno));
            return count;
        }

        char *after_column = string;

        PARSE_COLUMN(string, string, after_column, ',', count, timestamp);
        PARSE_COLUMN(string, after_column + 1, after_column, ',', count, code);
        PARSE_COLUMN(string, after_column + 1, after_column, ',', count, target_address);
        PARSE_COLUMN(string, after_column + 1, after_column, '\n', count, description);

        count++;
    }

    return count;
#undef PARSE_COLUMN
}
