#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"


enum {
    BUTTON_BACK_ID,
    ADDRESS_BTN_ID,
};

struct page_data {};


static lv_obj_t *address_button(lv_obj_t *root, uint8_t address) {
    lv_obj_t *btn = lv_btn_create(root, NULL);
    lv_obj_set_size(btn, 80, 48);
    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_label_set_text_fmt(lbl, "%i", address);
    view_register_default_callback_number(btn, ADDRESS_BTN_ID, address);
    lv_page_glue_obj(btn, 1);

    return btn;
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;
    (void)data;

    lv_obj_t *title = view_common_title(BUTTON_BACK_ID, "Rete Easy Connect", NULL);

    lv_obj_t *page = lv_page_create(lv_scr_act(), NULL);
    lv_page_set_scrl_layout(page, LV_LAYOUT_PRETTY_TOP);
    lv_obj_set_size(page, LV_HOR_RES_MAX, LV_VER_RES_MAX - lv_obj_get_height(title) - 8);
    lv_obj_align(page, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    for (size_t i = 1; i < 32; i++) {
        address_button(page, i);
    }
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

                        case ADDRESS_BTN_ID:
                            msg.cmsg.code    = VIEW_CONTROLLER_MESSAGE_CODE_CONFIGURE_DEVICE_ADDRESS;
                            msg.cmsg.address = event.data.number;
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


pman_page_t page_devices = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};