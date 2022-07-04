#include <stdlib.h>
#include "src/lv_core/lv_disp.h"
#include "src/lv_core/lv_obj.h"
#include "src/lv_misc/lv_area.h"
#include "src/lv_widgets/lv_btn.h"
#include "src/lv_widgets/lv_label.h"
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"



enum {
    BACK_BTN_ID,
    DELETE_BTN_ID,
    CLASS_BTN_ID,
    REFRESH_BTN_ID,
};


struct page_data {
    device_t  device;
    lv_obj_t *classdd;
    lv_obj_t *lbl_info;
};



static const device_class_t classes[] = {
    DEVICE_CLASS_LIGHT_1,
    DEVICE_CLASS_LIGHT_2,
    DEVICE_CLASS_LIGHT_3,
    DEVICE_CLASS_ELECTROSTATIC_FILTER,
    DEVICE_CLASS_ULTRAVIOLET_FILTER,
    DEVICE_CLASS_SIPHONING_FAN,
    DEVICE_CLASS_IMMISSION_FAN,
};


static void update_info(model_t *pmodel, struct page_data *data) {
    model_get_device(pmodel, &data->device, data->device.address);
    lv_label_set_text_fmt(data->lbl_info, "Dispositivo %i, classe 0x%X, stato %i, SN 0x%X, allarmi 0x%X",
                          data->device.address, data->device.class, data->device.status, data->device.serial_number,
                          data->device.alarms);
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    model_get_device(model, &data->device, (uint8_t)(uintptr_t)extra);
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *data  = arg;
    lv_obj_t         *title = view_common_title(BACK_BTN_ID, "Info dispositivo", NULL);
    (void)title;

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_set_width(lbl, 400);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_MID, 0, 80);
    data->lbl_info = lbl;

    lv_obj_t *dd = lv_dropdown_create(lv_scr_act(), NULL);
    lv_dropdown_clear_options(dd);
    char string[32]       = {0};
    char device_class[32] = {0};
    view_common_get_class_string(data->device.class, device_class, sizeof(device_class));

    size_t selected = 0;
    for (size_t i = 0; i < sizeof(classes) / sizeof(device_class_t); i++) {
        memset(string, 0, sizeof(string));
        view_common_get_class_string(classes[i], string, sizeof(string));
        lv_dropdown_add_option(dd, string, i);

        if (strcmp(string, device_class) == 0) {
            selected = i;
        }
    }
    lv_dropdown_set_selected(dd, selected);
    lv_obj_set_width(dd, 160);
    lv_obj_align(dd, NULL, LV_ALIGN_IN_TOP_MID, -lv_obj_get_width(dd) / 2, 130);
    data->classdd = dd;

    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_width(btn, 180);
    lbl = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, "imposta classe");
    lv_obj_align(btn, dd, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
    view_register_default_callback(btn, CLASS_BTN_ID);

    btn = lv_btn_create(lv_scr_act(), NULL);
    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_text(lbl, "Rimuovi");
    lv_obj_set_size(btn, 180, 48);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -4);
    view_register_default_callback(btn, DELETE_BTN_ID);

    btn = lv_btn_create(lv_scr_act(), NULL);
    lbl = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, LV_SYMBOL_REFRESH);
    lv_obj_set_size(btn, 48, 48);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    view_register_default_callback(btn, REFRESH_BTN_ID);

    update_info(pmodel, data);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *data = arg;
    (void)data;
    view_message_t msg = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_DEVICE_UPDATE:
        case VIEW_EVENT_CODE_DEVICE_ALARM:
            if (event.address == data->device.address) {
                update_info(pmodel, data);
            }
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BACK_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;

                        case DELETE_BTN_ID:
                            model_delete_device(pmodel, data->device.address);
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;

                        case CLASS_BTN_ID: {
                            device_class_t class = classes[lv_dropdown_get_selected(data->classdd)];
                            msg.cmsg.code        = VIEW_CONTROLLER_MESSAGE_CODE_SET_CLASS;
                            msg.cmsg.class       = class;
                            msg.cmsg.address     = data->device.address;
                            break;
                        }

                        case REFRESH_BTN_ID: {
                            msg.cmsg.code    = VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_INFO;
                            msg.cmsg.address = data->device.address;
                        }

                        default:
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


void destroy_page(void *args, void *extra) {
    free(args);
}


const pman_page_t page_device_info = {
    .create        = create_page,
    .destroy       = destroy_page,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};