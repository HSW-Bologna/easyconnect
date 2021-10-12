#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"


enum {
    BUTTON_BACK_ID,
};

struct page_data {};


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;
    (void)data;
    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), NULL);
    lv_obj_align(spinner, NULL, LV_ALIGN_CENTER, 0, 0);
}


static view_message_t process_page_event(model_t *model, void *arg, view_event_t event) {
    struct page_data *data = arg;
    (void)data;
    view_message_t msg = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_DEVICE_UPDATE:
            if (model_all_devices_queried(model)) {
                msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                msg.vmsg.page = &page_main;
            }
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    break;
                }

                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    return msg;
}


const pman_page_t page_splash = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};