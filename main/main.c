#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "lvgl.h"
#include "model/model.h"
#include "view/view.h"
#include "controller/controller.h"
#include "controller/gui.h"
#include "lvgl_helpers.h"
#include "peripherals/system.h"
#include "peripherals/modbus.h"
#include "peripherals/storage.h"
#include "gel/timer/timecheck.h"
#include "utils/utils.h"


static const char *TAG = "Main";
static model_t     model;


void app_main(void) {

    /* Initialize SPI or I2C bus used by the drivers */
    system_i2c_init();
    lvgl_driver_init();
    modbus_init();
    storage_init();

    model_init(&model);
    view_init(disp_driver_flush, ft6x36_read);
    controller_init(&model);

    ESP_LOGI(TAG, "Begin main loop");
    for (;;) {
        controller_gui_manage(&model);
        controller_manage(&model);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
