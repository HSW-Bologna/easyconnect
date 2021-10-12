#include "model/model.h"
#include "view/view.h"
#include "utils/utils.h"
#include "gel/timer/timecheck.h"
#include "controller.h"
#include "esp_log.h"


void controller_gui_manage(model_t *pmodel) {
    static unsigned long  timestamp = 0;
    view_message_t umsg;
    view_event_t   event;

    unsigned long now = get_millis();
    unsigned long interval = time_interval(timestamp, now);
    lv_task_handler();
    lv_tick_inc(interval);
    timestamp = now;

    while (view_get_next_message(pmodel, &umsg, &event)) {
        controller_manage_message(pmodel, &umsg.cmsg);
        view_process_message(umsg.vmsg, pmodel);
    }
}
