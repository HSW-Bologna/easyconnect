#include "model/model.h"
#include "view/view.h"
#include "utils/utils.h"
#include "gel/timer/timecheck.h"
#include "controller.h"
#include "esp_log.h"
#include "peripherals/buzzer.h"


void controller_gui_manage(model_t *pmodel) {
    static unsigned long timestamp = 0;
    view_message_t       umsg;
    view_event_t         event;

    unsigned long now      = get_millis();
    unsigned long interval = time_interval(timestamp, now);
    lv_task_handler();
    lv_tick_inc(interval);
    timestamp = now;

    while (view_get_next_message(pmodel, &umsg, &event)) {
        if (event.code == VIEW_EVENT_CODE_LVGL && event.lv_event == LV_EVENT_CLICKED) {
            buzzer_beep(1, 50, 0, model_get_buzzer_volume(pmodel));
        }

        controller_manage_message(pmodel, &umsg.cmsg);
        view_process_message(umsg.vmsg, pmodel);
    }
}
