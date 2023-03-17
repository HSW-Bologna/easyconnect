#include <stdlib.h>
#include "src/lv_core/lv_disp.h"
#include "src/lv_core/lv_obj.h"
#include "src/lv_misc/lv_area.h"
#include "src/lv_widgets/lv_btn.h"
#include "src/lv_widgets/lv_label.h"
#include "view/view.h"
#include "view/common.h"
#include "view/intl/intl.h"
#include "model/model.h"


LV_IMG_DECLARE(img_icona_luce_1);
LV_IMG_DECLARE(img_icona_luce_2);
LV_IMG_DECLARE(img_icona_luce_3);
LV_IMG_DECLARE(img_icona_elettrostatico);
LV_IMG_DECLARE(img_icona_uvc);
LV_IMG_DECLARE(img_icona_aspirazione);
LV_IMG_DECLARE(img_icona_immissione);



enum {
    BACK_BTN_ID,
    DELETE_BTN_ID,
    REFRESH_BTN_ID,
};


struct page_data {
    device_t  device;
    lv_obj_t *img_icon;
    lv_obj_t *lbl_info;
    lv_obj_t *lbl_serial_address;
    lv_obj_t *lbl_alarms[MAX_NUM_MESSAGES];
    size_t    num_messages;
    char     *messages[MAX_NUM_MESSAGES];
};


static void update_info(model_t *pmodel, struct page_data *data) {
    data->device = model_get_device(pmodel, data->device.address);

    view_common_get_class_icon(data->device.class, data->img_icon);

    char device_type[64] = "";
    view_common_get_class_string(pmodel, data->device.class, device_type, sizeof(device_type));

    lv_label_set_text_fmt(data->lbl_info, "%s\n%s %i\n%s %i.%i.%i", device_type,
                          view_intl_get_string(pmodel, STRINGS_GRUPPO), CLASS_GET_GROUP(data->device.class),
                          view_intl_get_string(pmodel, STRINGS_VERSIONE), (data->device.firmware_version >> 10) & 0x3F,
                          (data->device.firmware_version >> 6) & 0xF, (data->device.firmware_version >> 0) & 0x3F);

    lv_label_set_text_fmt(data->lbl_serial_address, "%s: %i\n%s: %i", view_intl_get_string(pmodel, STRINGS_SERIALE),
                          data->device.serial_number, view_intl_get_string(pmodel, STRINGS_INDIRIZZO),
                          data->device.address);

    if (data->device.status == DEVICE_STATUS_COMMUNICATION_ERROR) {
        lv_label_set_text(data->lbl_alarms[0], view_intl_get_string(pmodel, STRINGS_ERRORE_DI_COMUNICAZIONE));
        lv_obj_set_hidden(data->lbl_alarms[0], 0);
        lv_obj_set_hidden(data->lbl_alarms[1], 1);
    } else {
        for (size_t i = 0; i < MAX_NUM_MESSAGES; i++) {
            if ((data->device.alarms & (1 << i)) > 0) {
                lv_obj_set_hidden(data->lbl_alarms[i], 0);
                if (data->messages[i] == NULL || strlen(data->messages[i]) == 0) {
                    lv_label_set_text(data->lbl_alarms[i], "----");
                } else {
                    lv_label_set_text(data->lbl_alarms[i], data->messages[i]);
                }
            } else {
                lv_obj_set_hidden(data->lbl_alarms[i], 1);
            }
        }
    }
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    data->device           = model_get_device(model, (uint8_t)(uintptr_t)extra);
    data->num_messages     = 0;
    for (size_t i = 0; i < MAX_NUM_MESSAGES; i++) {
        data->messages[i] = NULL;
    }
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *data  = arg;
    lv_obj_t         *title = view_common_title(BACK_BTN_ID, "Info dispositivo", NULL);
    (void)title;

    lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_border_width(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 2);
    lv_obj_set_style_local_radius(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 8);
    lv_obj_set_size(cont, 48, 48);

    lv_obj_t *img = lv_img_create(cont, NULL);
    lv_obj_set_size(img, 32, 32);
    lv_obj_set_auto_realign(img, 1);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);
    data->img_icon = img;

    lv_obj_align(cont, NULL, LV_ALIGN_IN_TOP_LEFT, 16, 80);

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_set_width(lbl, 160);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 70, 72);
    data->lbl_info = lbl;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_set_width(lbl, 220);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 72);
    data->lbl_serial_address = lbl;

    for (size_t i = 0; i < MAX_NUM_MESSAGES; i++) {
        lbl = lv_label_create(lv_scr_act(), NULL);
        lv_obj_set_auto_realign(lbl, 1);
        lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_MID, 0, 160 + 24 * i);
        data->lbl_alarms[i] = lbl;
    }

    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lbl           = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_text(lbl, "Rimuovi");
    lv_obj_set_size(btn, 180, 48);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -8);
    view_register_default_callback(btn, DELETE_BTN_ID);

    btn = lv_btn_create(lv_scr_act(), NULL);
    lbl = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, LV_SYMBOL_REFRESH);
    lv_obj_set_size(btn, 48, 48);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -8);
    view_register_default_callback(btn, REFRESH_BTN_ID);

    update_info(pmodel, data);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *data = arg;
    (void)data;
    view_message_t msg = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_OPEN:
            msg.cmsg.code    = VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_MESSAGES;
            msg.cmsg.address = data->device.address;
            break;

        case VIEW_EVENT_CODE_DEVICE_MESSAGES:
            data->num_messages = event.num_messages;
            for (size_t i = 0; i < MAX_NUM_MESSAGES; i++) {
                data->messages[i] = event.messages[i];
            }
            update_info(pmodel, data);
            break;

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
    struct page_data *pdata = args;

    for (size_t i = 0; i < pdata->num_messages; i++) {
        free(pdata->messages[i]);
    }
    free(pdata);
}


const pman_page_t page_device_info = {
    .create        = create_page,
    .destroy       = destroy_page,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};