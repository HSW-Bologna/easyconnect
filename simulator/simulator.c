#include "FreeRTOS.h"
#include "task.h"
#include "esp_log.h"
#include <SDL2/SDL.h>
#include "display/monitor.h"
#include "indev/mouse.h"
#include "view/view.h"
#include "model/model.h"
#include "controller/controller.h"


static const char *TAG = "Main";


void app_main(void *arg) {
    model_t model;
    (void)arg;

    monitor_init();
    mouse_init();

    model_init(&model);
    view_init(monitor_flush, mouse_read);
    controller_init(&model);

    ESP_LOGI(TAG, "Begin main loop");
    for (;;) {
        controller_gui_manage(&model);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}