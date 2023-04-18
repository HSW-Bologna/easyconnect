#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"
#include "view/intl/intl.h"


enum {
    BUTTON_BACK_ID,
    CURRENT_BTN_ID,
    LOG_BTN_ID,
};

struct page_data {};


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *data = arg;
    lv_obj_t         *btn;
    (void)data;

    view_common_title(BUTTON_BACK_ID, view_intl_get_string(pmodel, STRINGS_ERRORI), NULL);

    btn = view_common_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_CORRENTI), 200, CURRENT_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_MID, 0, 100);
    lv_obj_t *btn_previous = btn;

    btn = view_common_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_LOG), 200, LOG_BTN_ID);
    lv_obj_align(btn, btn_previous, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    btn_previous = btn;
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *data = arg;
    (void)data;
    view_message_t msg = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BUTTON_BACK_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;

                        case CURRENT_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_current_errors;
                            break;

                        case LOG_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_logs;
                            break;
                    }
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


const pman_page_t page_errors = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};
