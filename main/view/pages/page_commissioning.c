#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"


enum {
    BUTTON_BACK_ID,
    AUTOMATIC_BTN_ID,
    MANUAL_BTN_ID,
};

struct page_data {};


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;
    (void)data;

    view_common_title(BUTTON_BACK_ID, "Configurazione", NULL);
    lv_obj_t *btn1 = view_common_default_menu_button(lv_scr_act(), "Manuale", MANUAL_BTN_ID);
    lv_obj_t *btn2 = view_common_default_menu_button(lv_scr_act(), "Automatico", AUTOMATIC_BTN_ID);

    lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, 0, 100);
    lv_obj_align(btn2, btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
}


static view_message_t process_page_event(model_t *model, void *arg, view_event_t event) {
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

                        case AUTOMATIC_BTN_ID:
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_AUTOMATIC_COMMISSIONING;
                            break;

                        case MANUAL_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_manual_device_config;
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


const pman_page_t page_commissioning = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};