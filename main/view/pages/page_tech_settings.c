#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "view/intl/intl.h"
#include "model/model.h"


enum {
    BUTTON_BACK_ID,
    NETWORK_STATE_BTN_ID,
    DEVICES_BTN_ID,
    COMMISSIONING_BTN_ID,
    FILTERS_BTN_ID,
    IMMISSION_BTN_ID,
};

struct page_data {};


static void *create_page(model_t *pmodel, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *data = arg;
    (void)data;

    view_common_title(BUTTON_BACK_ID, view_intl_get_string(pmodel, STRINGS_SYSTEM_SETUP), NULL);

    lv_obj_t *btn = view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_DISPOSITIVI),
                                                    DEVICES_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_LEFT, 8, 100);

    lv_obj_t *btn1 = view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_CONFIGURAZIONE),
                                                     COMMISSIONING_BTN_ID);
    lv_obj_align(btn1, btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

    lv_obj_t *btn2 =
        view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_FILTRI), FILTERS_BTN_ID);
    lv_obj_align(btn2, btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

    lv_obj_t *btn3 = view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_IMMISSIONE),
                                                     IMMISSION_BTN_ID);
    lv_obj_align(btn3, NULL, LV_ALIGN_IN_TOP_RIGHT, -8, 100);
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

                        case DEVICES_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_devices;
                            break;

                        case COMMISSIONING_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_commissioning;
                            break;

                        case FILTERS_BTN_ID:
                            break;

                        case IMMISSION_BTN_ID: {
                            break;
                        }
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


const pman_page_t page_tech_settings = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};