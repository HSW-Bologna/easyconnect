#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "i2c_ports/esp-idf/esp_idf_i2c_port.h"
#include "i2c_devices/temperature/SHT21/sht21.h"
#include "peripherals/i2c_devices.h"
#include "esp_log.h"


#define DELAY       11
#define AVERAGE_NUM 5


static void temperature_task(void *args);


static const char       *TAG                       = "Temperature";
static SemaphoreHandle_t sem                       = NULL;
static double            temperatures[AVERAGE_NUM] = {0};
static size_t            average_index             = 0;
static int               full_circle               = 0;


void temperature_init(void) {
    static StaticSemaphore_t static_sem;
    sem = xSemaphoreCreateMutexStatic(&static_sem);

    static StaticTask_t static_task;
    static StackType_t  task_stack[1024 + 512];
    xTaskCreateStatic(temperature_task, TAG, sizeof(task_stack) / sizeof(StackType_t), NULL, 1, task_stack,
                      &static_task);
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


static void temperature_task(void *args) {
    (void)args;
    sht21_set_resolution(sht21_driver, SHT21_RESOLUTION_11_11);

    for (;;) {
        double temperature = 0;

        if (sht21_read(sht21_driver, &temperature, NULL, DELAY) == 0) {
            xSemaphoreTake(sem, portMAX_DELAY);
            temperatures[average_index] = temperature;
            if (average_index == AVERAGE_NUM - 1) {
                full_circle = 1;
            }
            average_index = (average_index + 1) % AVERAGE_NUM;
            xSemaphoreGive(sem);
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }

    vTaskDelete(NULL);
}