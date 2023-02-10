#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"


LV_IMG_DECLARE(img_error_sm);
LV_IMG_DECLARE(img_icona_luce_1);
LV_IMG_DECLARE(img_icona_luce_2);
LV_IMG_DECLARE(img_icona_luce_3);
LV_IMG_DECLARE(img_icona_elettrostatico);
LV_IMG_DECLARE(img_icona_uvc);
LV_IMG_DECLARE(img_icona_aspirazione);
LV_IMG_DECLARE(img_icona_immissione);
LV_IMG_DECLARE(img_icona_pressione);


#define DEVICES_PER_PAGE 15


enum {
    BUTTON_BACK_ID,
    ADDRESS_BTN_ID,
    NEW_DEVICE_BTN_ID,
    PREV_PAGE_BTN_ID,
    NEXT_PAGE_BTN_ID,
    ADDRESS_DD_ID,
    CONFIRM_ADDRESS_BTN_ID,
    CANCEL_ADDRESS_BTN_ID,
    REFRESH_BTN_ID,
};


struct page_data {
    uint8_t   starting_address;
    lv_obj_t *cont;
    lv_obj_t *address_dd;
    lv_obj_t *popup;
    lv_obj_t *next;
    lv_obj_t *prev;
};


static lv_obj_t *address_button(lv_obj_t *root, device_t device) {
    lv_obj_t *btn = lv_btn_create(root, NULL);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_size(btn, 80, 48);
    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_label_set_text_fmt(lbl, "%i\n%i", device.address, device.serial_number);
    lv_obj_t *img = lv_img_create(btn, NULL);

    lv_obj_align(lbl, NULL, LV_ALIGN_IN_LEFT_MID, 8, 0);

    switch (device.class) {
        case DEVICE_CLASS_LIGHT_1:
            lv_img_set_src(img, &img_icona_luce_1);
            break;
        case DEVICE_CLASS_LIGHT_2:
            lv_img_set_src(img, &img_icona_luce_2);
            break;
        case DEVICE_CLASS_LIGHT_3:
            lv_img_set_src(img, &img_icona_luce_3);
            break;
        case DEVICE_CLASS_ELECTROSTATIC_FILTER:
            lv_img_set_src(img, &img_icona_elettrostatico);
            break;
        case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1):
        case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2):
        case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3):
            lv_img_set_src(img, &img_icona_uvc);
            break;
        case DEVICE_CLASS_IMMISSION_FAN:
            lv_img_set_src(img, &img_icona_immissione);
            break;
        case DEVICE_CLASS_SIPHONING_FAN:
            lv_img_set_src(img, &img_icona_aspirazione);
            break;
        case DEVICE_CLASS_PRESSURE_SAFETY:
            lv_img_set_src(img, &img_icona_pressione);
            break;

        default:
            lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_obj_set_hidden(img, 1);
            break;
    }

    if (device.status == DEVICE_STATUS_COMMUNICATION_ERROR || device.alarms > 0) {
        lv_obj_t *img_error = lv_img_create(img, NULL);
        lv_img_set_src(img_error, &img_error_sm);
        lv_obj_align(img_error, img, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    }
    lv_obj_align(img, NULL, LV_ALIGN_IN_RIGHT_MID, -8, 0);

    view_register_default_callback_number(btn, ADDRESS_BTN_ID, device.address);
    lv_page_glue_obj(btn, 1);


    return btn;
}


static uint8_t last_address_page(model_t *pmodel) {
    uint8_t result = 0;
    int     done   = 0;

    do {
        uint8_t next = result;
        result       = model_get_next_device_address(pmodel, result);

        for (size_t i = 0; i < DEVICES_PER_PAGE; i++) {
            uint8_t old = next;
            next        = model_get_next_device_address(pmodel, old);

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


static void update_device_list(model_t *pmodel, struct page_data *data) {
    lv_obj_clean(data->cont);
    size_t devices = model_get_configured_devices(pmodel);

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
        data->starting_address = model_get_next_device_address(pmodel, data->starting_address);
    }
    // The current starting address has been deleted
    if (!model_is_address_configured(pmodel, data->starting_address)) {
        data->starting_address = model_get_next_device_address(pmodel, data->starting_address);
    }
    // There were no more devices
    if (!model_is_address_configured(pmodel, data->starting_address)) {
        data->starting_address = last_address_page(pmodel);
    }

    size_t  i            = 0;
    uint8_t address      = data->starting_address;
    uint8_t prev_address = address;
    do {
        device_t device = model_get_device(pmodel, address);
        address_button(data->cont, device);
        prev_address = address;
        address      = model_get_next_device_address(pmodel, prev_address);
    } while (address != prev_address && ++i < DEVICES_PER_PAGE);

    view_common_limit_address_picker(pmodel, data->address_dd);
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    data->starting_address = 0;
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *data = arg;

    lv_obj_t *title = view_common_title(BUTTON_BACK_ID, "Dispositivi", NULL);

    lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_layout(cont, LV_LAYOUT_PRETTY_TOP);
    lv_obj_set_size(cont, LV_HOR_RES_MAX, LV_VER_RES_MAX - lv_obj_get_height(title) - 8 - 48);
    lv_obj_align(cont, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -52);
    data->cont = cont;

    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_text(lbl, LV_SYMBOL_PLUS);
    lv_obj_set_size(btn, 64, 48);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -4);
    view_register_default_callback(btn, NEW_DEVICE_BTN_ID);

    btn = lv_btn_create(lv_scr_act(), NULL);
    lbl = lv_label_create(btn, lbl);
    lv_label_set_text(lbl, LV_SYMBOL_LEFT);
    lv_obj_set_size(btn, 80, 48);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 4, -4);
    view_register_default_callback(btn, PREV_PAGE_BTN_ID);
    data->prev = btn;

    btn = lv_btn_create(lv_scr_act(), btn);
    lbl = lv_label_create(btn, lbl);
    lv_label_set_text(lbl, LV_SYMBOL_RIGHT);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -4, -4);
    view_register_default_callback(btn, NEXT_PAGE_BTN_ID);
    data->next = btn;

    btn = lv_btn_create(lv_scr_act(), NULL);
    lbl = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, LV_SYMBOL_REFRESH);
    lv_obj_set_size(btn, 64, 48);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 100, -4);
    view_register_default_callback(btn, REFRESH_BTN_ID);

    cont = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_layout(cont, LV_LAYOUT_OFF);
    lv_obj_set_size(cont, 240, 280);
    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);

    lbl = lv_label_create(cont, NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(lbl, 220);
    lv_label_set_text(lbl, "Selezionare un indirizzo disponibile");
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_MID, 0, 8);

    lv_obj_t *dd = view_common_address_picker(cont, ADDRESS_DD_ID);
    lv_obj_align(dd, NULL, LV_ALIGN_IN_TOP_MID, 0, 64);
    data->address_dd = dd;

    btn = lv_btn_create(cont, btn);
    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_text(lbl, LV_SYMBOL_PLUS);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -8);
    view_register_default_callback(btn, CONFIRM_ADDRESS_BTN_ID);

    btn = lv_btn_create(cont, btn);
    lbl = lv_label_create(btn, lbl);
    lv_label_set_text(lbl, LV_SYMBOL_CLOSE);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 8, -8);
    view_register_default_callback(btn, CANCEL_ADDRESS_BTN_ID);
    data->popup = cont;

    update_device_list(pmodel, data);
    lv_obj_set_hidden(data->popup, 1);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *data = arg;
    (void)data;
    view_message_t msg = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_DEVICE_ALARM:
        case VIEW_EVENT_CODE_DEVICE_UPDATE:
            update_device_list(pmodel, data);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BUTTON_BACK_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SAVE;
                            break;

                        case NEW_DEVICE_BTN_ID:
                            lv_obj_set_hidden(data->popup, 0);
                            break;

                        case CANCEL_ADDRESS_BTN_ID:
                            lv_obj_set_hidden(data->popup, 1);
                            break;

                        case REFRESH_BTN_ID:
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_REFRESH_DEVICES;
                            break;

                        case ADDRESS_BTN_ID:
                            msg.vmsg.code  = VIEW_COMMAND_CODE_CHANGE_PAGE_EXTRA;
                            msg.vmsg.page  = &page_device_info;
                            msg.vmsg.extra = (void *)(uintptr_t)event.data.number;
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
                                data->starting_address = last_address_page(pmodel);
                            }

                            lv_obj_set_hidden(data->popup, 1);
                            update_device_list(pmodel, data);
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
                            lv_obj_set_hidden(data->popup, 1);
                            update_device_list(pmodel, data);
                            break;
                        }

                        case CONFIRM_ADDRESS_BTN_ID: {
                            char string[32] = {0};
                            lv_dropdown_get_selected_str(data->address_dd, string, sizeof(string));
                            uint8_t address = atoi(string);
                            model_new_device(pmodel, address);
                            lv_obj_set_hidden(data->popup, 1);
                            update_device_list(pmodel, data);
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


const pman_page_t page_devices = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};