#include <stdlib.h>
#include "src/lv_core/lv_disp.h"
#include "src/lv_core/lv_obj.h"
#include "src/lv_misc/lv_area.h"
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"


#define DEVICES_PER_PAGE 15


enum {
    BUTTON_BACK_ID,
    ADDRESS_BTN_ID,
    PREV_PAGE_BTN_ID,
    NEXT_PAGE_BTN_ID,
    ADDRESS_DD_ID,
    SEARCH_BTN_ID,
};


struct page_data {
    uint8_t   starting_address;
    lv_obj_t *cont;
    lv_obj_t *next;
    lv_obj_t *prev;
    lv_obj_t *scan;
    lv_obj_t *spinner;
    lv_obj_t *lbl_scan;

    device_t devices[MODBUS_MAX_DEVICES];
};


static lv_obj_t *address_button(lv_obj_t *root, uint8_t address) {
    lv_obj_t *btn = lv_btn_create(root, NULL);
    lv_obj_set_size(btn, 80, 48);
    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_label_set_text_fmt(lbl, "%i", address);
    view_register_default_callback_number(btn, ADDRESS_BTN_ID, address);
    lv_page_glue_obj(btn, 1);

    return btn;
}


static uint8_t last_address_page(struct page_data *data) {
    uint8_t result = 0;
    int     done   = 0;

    do {
        uint8_t next = result;
        result       = device_list_get_next_device_address(data->devices, result);

        for (size_t i = 0; i < DEVICES_PER_PAGE; i++) {
            uint8_t old = next;
            next        = device_list_get_next_device_address(data->devices, old);

            if (old == next) {
                done = 1;
                break;
            }
        }

        if (!done) {
            result = next;
        }
    } while (!done);

    return result;
}


static void update_device_list(struct page_data *data) {
    lv_obj_clean(data->cont);
    size_t devices = device_list_get_configured_devices(data->devices);

    if (devices <= DEVICES_PER_PAGE) {
        lv_btn_set_state(data->prev, LV_BTN_STATE_DISABLED);
        lv_btn_set_state(data->next, LV_BTN_STATE_DISABLED);
    } else {
        lv_btn_set_state(data->prev, LV_BTN_STATE_RELEASED);
        lv_btn_set_state(data->next, LV_BTN_STATE_RELEASED);
    }

    if (devices == 0) {
        // There are no devices
        return;
    }

    // First time
    if (data->starting_address == 0) {
        data->starting_address = device_list_get_next_device_address(data->devices, data->starting_address);
    }
    // The current starting address has been deleted
    if (!device_list_is_address_configured(data->devices, data->starting_address)) {
        data->starting_address = device_list_get_next_device_address(data->devices, data->starting_address);
    }
    // There were no more devices
    if (!device_list_is_address_configured(data->devices, data->starting_address)) {
        data->starting_address = last_address_page(data);
    }

    size_t  i            = 0;
    uint8_t address      = data->starting_address;
    uint8_t prev_address = address;
    do {
        address_button(data->cont, address);
        prev_address = address;
        address      = device_list_get_next_device_address(data->devices, prev_address);
    } while (address != prev_address && ++i < DEVICES_PER_PAGE);
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    device_list_init(data->devices);
    data->starting_address = 0;
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *data = arg;

    lv_obj_t *title = view_common_title(BUTTON_BACK_ID, "Ricerca dispositivi", NULL);

    lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_layout(cont, LV_LAYOUT_PRETTY_TOP);
    lv_obj_set_size(cont, LV_HOR_RES_MAX, LV_VER_RES_MAX - lv_obj_get_height(title) - 8 - 48);
    lv_obj_align(cont, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -52);
    data->cont = cont;

    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_text(lbl, LV_SYMBOL_REFRESH);
    lv_obj_set_size(btn, 64, 48);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -4);
    view_register_default_callback(btn, SEARCH_BTN_ID);
    data->scan = btn;

    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), NULL);
    lv_obj_set_size(spinner, 48, 48);
    lv_obj_align(spinner, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -4);
    data->spinner = spinner;

    btn = lv_btn_create(lv_scr_act(), NULL);
    lbl = lv_label_create(btn, lbl);
    lv_label_set_text(lbl, LV_SYMBOL_LEFT);
    lv_obj_set_size(btn, 100, 48);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 4, -4);
    view_register_default_callback(btn, PREV_PAGE_BTN_ID);
    data->prev = btn;

    btn = lv_btn_create(lv_scr_act(), btn);
    lbl = lv_label_create(btn, lbl);
    lv_label_set_text(lbl, LV_SYMBOL_RIGHT);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -4, -4);
    view_register_default_callback(btn, NEXT_PAGE_BTN_ID);
    data->next = btn;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_size(lbl, 64, 48);
    lv_obj_set_style_local_text_font(lbl, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_align(lbl, data->scan, LV_ALIGN_OUT_RIGHT_MID, 4, 0);
    data->lbl_scan = lbl;

    update_device_list(data);
    lv_obj_set_hidden(data->spinner, 1);
    lv_obj_set_hidden(data->scan, 0);
    lv_obj_set_hidden(data->lbl_scan, 1);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *data = arg;
    view_message_t    msg  = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_DEVICE_UPDATE:
            if (!event.error) {
                device_list_new_device(data->devices, event.address);
                device_list_set_device_error(data->devices, event.address, 0);
                update_device_list(data);
            } 
            if (lv_obj_get_hidden(data->lbl_scan)) {
                lv_obj_set_hidden(data->lbl_scan, 0);
            }
            lv_label_set_text_fmt(data->lbl_scan, "%i...", event.address);
            break;

        case VIEW_EVENT_CODE_DEVICE_SEARCH_DONE:
            lv_obj_set_hidden(data->spinner, 1);
            lv_obj_set_hidden(data->scan, 0);
            lv_obj_set_hidden(data->lbl_scan, 1);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case SEARCH_BTN_ID:
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_SCAN;
                            lv_obj_set_hidden(data->spinner, 0);
                            lv_obj_set_hidden(data->scan, 1);
                            break;

                        case BUTTON_BACK_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_STOP_CURRENT_OPERATION;
                            break;

                        case ADDRESS_BTN_ID:
                            //msg.vmsg.code  = VIEW_COMMAND_CODE_CHANGE_PAGE_EXTRA;
                            //msg.vmsg.page  = &page_device_info;
                            //msg.vmsg.extra = (void *)(uintptr_t)event.data.number;
                            break;

                        case PREV_PAGE_BTN_ID: {
                            size_t  i            = 0;
                            uint8_t address      = data->starting_address;
                            uint8_t next_address = address;
                            do {
                                next_address = address;
                                address      = model_get_prev_device_address(pmodel, next_address);
                            } while (address != next_address && ++i < DEVICES_PER_PAGE);

                            if (address != next_address) {
                                data->starting_address = address;
                            } else {
                                data->starting_address = last_address_page(data);
                            }

                            update_device_list(data);
                            break;
                        }

                        case NEXT_PAGE_BTN_ID: {
                            size_t  i            = 0;
                            uint8_t address      = data->starting_address;
                            uint8_t prev_address = address;
                            do {
                                prev_address = address;
                                address      = model_get_next_device_address(pmodel, prev_address);
                            } while (address != prev_address && ++i < DEVICES_PER_PAGE);

                            if (address != prev_address) {
                                data->starting_address = address;
                            } else {
                                data->starting_address = model_get_next_device_address(pmodel, 0);
                            }
                            update_device_list(data);
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


const pman_page_t page_device_search = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};