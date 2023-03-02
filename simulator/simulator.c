#include "FreeRTOS.h"
#include "task.h"
#include "esp_log.h"
#include <SDL2/SDL.h>
#include "display/monitor.h"
#include "indev/mouse.h"
#include "view/view.h"
#include "model/model.h"
#include "controller/controller.h"
#include "controller/gui.h"


static void add_simulated_device(model_t *pmodel, uint8_t address, uint16_t serial_number, uint16_t class);


static const char *TAG = "Main";


void app_main(void *arg) {
    model_t model;
    (void)arg;

    lv_init();
    monitor_init();
    mouse_init();

    model_init(&model);
    add_simulated_device(&model, 1, 1, DEVICE_CLASS_LIGHT_1);
    add_simulated_device(&model, 2, 2, DEVICE_CLASS_LIGHT_2);
    add_simulated_device(&model, 3, 3, DEVICE_CLASS_LIGHT_3);
    add_simulated_device(&model, 4, 4, DEVICE_CLASS_IMMISSION_FAN);
    add_simulated_device(&model, 5, 5, DEVICE_CLASS_SIPHONING_FAN);
    add_simulated_device(&model, 6, 6, DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1));
    add_simulated_device(&model, 7, 7, DEVICE_CLASS_ELECTROSTATIC_FILTER);
    add_simulated_device(&model, 8, 8, DEVICE_CLASS_GAS);
    add_simulated_device(&model, 9, 9, DEVICE_CLASS_PRESSURE_SAFETY);
    add_simulated_device(&model, 10, 10, DEVICE_CLASS_TEMPERATURE_HUMIDITY_SAFETY);
    add_simulated_device(&model, 11, 11, DEVICE_CLASS_PRESSURE_TEMPERATURE_HUMIDITY_SAFETY);

    view_init(monitor_flush, mouse_read);
    controller_init(&model);

    ESP_LOGI(TAG, "Begin main loop");
    for (;;) {
        controller_gui_manage(&model);
        controller_manage(&model);

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
}


static void add_simulated_device(model_t *pmodel, uint8_t address, uint16_t serial_number, uint16_t class) {
    model_new_device(pmodel, address);
    model_set_device_error(pmodel, address, 0);
    model_set_device_sn(pmodel, address, serial_number);
    model_set_device_class(pmodel, address, class);
    model_set_device_firmware(pmodel, address, 0);
}