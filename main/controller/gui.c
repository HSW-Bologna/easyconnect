#include "model/model.h"
#include "view/view.h"
#include "utils/utils.h"
#include "gel/timer/timecheck.h"


void controller_gui_manage(model_t *pmodel) {
    unsigned long  timestamp = 0;
    view_message_t umsg;
    view_event_t   event;

    unsigned long now = get_millis();
    lv_task_handler();
    lv_tick_inc(time_interval(timestamp, now));
    timestamp = now;

    while (view_get_next_message(pmodel, &umsg, &event)) {
        // controllerProcessMessage(umsg.cmsg, pmodel);
        view_process_message(umsg.vmsg, pmodel);
    }
}
