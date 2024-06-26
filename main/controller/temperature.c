#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "i2c_devices/temperature/SHTC3/shtc3.h"
#include "peripherals/i2c_devices.h"
#include "esp_log.h"


#define DELAY       11
#define AVERAGE_NUM 10


static void temperature_task(void *args);


static const char       *TAG                       = "Temperature";
static SemaphoreHandle_t sem                       = NULL;
static double            temperatures[AVERAGE_NUM] = {0};
static double            humidities[AVERAGE_NUM]   = {0};
static size_t            average_index             = 0;
static int               full_circle               = 0;
static uint8_t           error                     = 0;


void temperature_init(void) {
    static StaticSemaphore_t static_sem;
    sem = xSemaphoreCreateMutexStatic(&static_sem);

    static StaticTask_t static_task;
    static StackType_t  task_stack[3000];
    xTaskCreateStatic(temperature_task, TAG, sizeof(task_stack) / sizeof(StackType_t), NULL, 1, task_stack,
                      &static_task);
}


uint8_t temperature_error(void) {
    xSemaphoreTake(sem, portMAX_DELAY);
    uint8_t res = error;
    xSemaphoreGive(sem);
    return res;
}


double temperature_get(void) {
    double average = 0;
    xSemaphoreTake(sem, portMAX_DELAY);
    size_t end = full_circle ? AVERAGE_NUM : average_index;

    if (end == 0) {
        xSemaphoreGive(sem);
        return 0;
    }

    for (size_t i = 0; i < end; i++) {
        average += temperatures[i];
    }
    xSemaphoreGive(sem);
    return average / end;
}


double temperature_get_humidity(void) {
    double average = 0;
    xSemaphoreTake(sem, portMAX_DELAY);
    size_t end = full_circle ? AVERAGE_NUM : average_index;

    if (end == 0) {
        xSemaphoreGive(sem);
        return 0;
    }

    for (size_t i = 0; i < end; i++) {
        average += humidities[i];
    }
    xSemaphoreGive(sem);
    return average / end;
}


static void temperature_task(void *args) {
    (void)args;
    // sht21_set_resolution(sht21_driver, SHT21_RESOLUTION_11_11);
    shtc3_wakeup(shtc3_driver);

    for (;;) {
        int16_t temperature = 0;
        int16_t humidity    = 0;

        if (shtc3_start_temperature_humidity_measurement(shtc3_driver) == 0) {
            vTaskDelay(pdMS_TO_TICKS(SHTC3_NORMAL_MEASUREMENT_PERIOD_MS));

            if (shtc3_read_temperature_humidity_measurement(shtc3_driver, &temperature, &humidity) == 0) {
                xSemaphoreTake(sem, portMAX_DELAY);
                temperatures[average_index] = (double)temperature;
                humidities[average_index]   = (double)humidity;
                if (average_index == AVERAGE_NUM - 1) {
                    full_circle = 1;
                }
                average_index = (average_index + 1) % AVERAGE_NUM;
                error         = 0;
                xSemaphoreGive(sem);
            } else {
                xSemaphoreTake(sem, portMAX_DELAY);
                error = 1;
                xSemaphoreGive(sem);

                ESP_LOGW(TAG, "Error in reading temperature measurement");
            }
        } else {
            xSemaphoreTake(sem, portMAX_DELAY);
            error = 1;
            xSemaphoreGive(sem);

            ESP_LOGW(TAG, "Error in starting temperature measurement");
        }

        vTaskDelay(pdMS_TO_TICKS(15000));
    }

    vTaskDelete(NULL);
}
