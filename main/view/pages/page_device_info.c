#include <stdlib.h>
#include "src/lv_core/lv_disp.h"
#include "src/lv_core/lv_obj.h"
#include "src/lv_core/lv_obj_style_dec.h"
#include "src/lv_misc/lv_area.h"
#include "src/lv_widgets/lv_btn.h"
#include "src/lv_widgets/lv_cont.h"
#include "src/lv_widgets/lv_label.h"
#include "controller/configuration.h"
#include "view/intl/AUTOGEN_FILE_strings.h"
#include "view/style.h"
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
LV_IMG_DECLARE(img_trash);


enum {
    BACK_BTN_ID,
    DELETE_BTN_ID,
    REFRESH_BTN_ID,
};


struct page_data {
    device_t  device;
    lv_obj_t *img_icon;
    lv_obj_t *lbl_info;
    lv_obj_t *lbl_extra;
};


static void update_info(model_t *pmodel, struct page_data *data) {
    data->device = model_get_device(pmodel, data->device.address);

    view_common_get_class_icon(data->device.class, data->img_icon);

    char device_type[64] = "";
    view_common_get_class_string(pmodel, data->device.class, device_type, sizeof(device_type));

    uint8_t alarm = 0;

    if (data->device.status == DEVICE_STATUS_COMMUNICATION_ERROR) {
        alarm = 1;
    } else {
        if (CLASS_GET_MODE(data->device.class) == DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY) {
            // lv_label_set_text_fmt(data->lbl_state, "Status: 0x%X\nPress: %i Pa", data->device.sensor_data.state,
            // model_get_device_pressure(pmodel, data->device));
        }

        for (size_t i = 0; i < MAX_NUM_MESSAGES; i++) {
            if ((data->device.alarms & (1 << i)) > 0) {
                alarm = 1;
                break;
            }
        }
    }

    size_t group = CLASS_GET_GROUP(data->device.class);
    lv_label_set_text_fmt(data->lbl_info, "%s: %s\n%s: %i.%i.%i\nSN: %i\nIP: %i  Gru: %i\n%s: %s",
                          view_intl_get_string(pmodel, STRINGS_TIPO), device_type,
                          view_intl_get_string(pmodel, STRINGS_REV_FW), (data->device.firmware_version >> 10) & 0x3F,
                          (data->device.firmware_version >> 6) & 0xF, (data->device.firmware_version >> 0) & 0x3F,
                          data->device.serial_number, data->device.address, group + 1,
                          view_intl_get_string(pmodel, STRINGS_ALLARME), alarm ? "KO" : "OK");

    switch (CLASS_GET_MODE(data->device.class)) {
        case DEVICE_MODE_PRESSURE:
        case DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY:
            view_common_set_hidden(data->lbl_extra, 0);

            int16_t calibrated_pressure =
                data->device.sensor_data.pressure + pmodel->configuration.pressure_offsets[group];

            lv_label_set_text_fmt(data->lbl_extra, "%s: %i Pa\nAVG %s: %i Pa\n%s: %i Pa\n%s: %i Pa",
                                  view_intl_get_string(pmodel, STRINGS_PRESSIONE_LETTA),
                                  data->device.sensor_data.pressure, view_intl_get_string(pmodel, STRINGS_PRESSIONE),
                                  pmodel->pressure_average, view_intl_get_string(pmodel, STRINGS_OFFSET_PRESSIONE),
                                  pmodel->configuration.pressure_offsets[group],
                                  view_intl_get_string(pmodel, STRINGS_PRESSIONE_CALIBRATA), calibrated_pressure);
            // lv_label_set_text_fmt(data->lbl_state, "Status: 0x%X\nPress: %i Pa", data->device.sensor_data.state,
            // model_get_device_pressure(pmodel, data->device));
            break;

        default:
            view_common_set_hidden(data->lbl_extra, 1);
            break;
    }
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    data->device           = model_get_device(model, (uint8_t)(uintptr_t)extra);
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *data  = arg;
    lv_obj_t         *title = view_common_title(BACK_BTN_ID, "Info dispositivo", NULL);
    (void)title;

    lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_add_style(cont, LV_CONT_PART_MAIN, &style_transparent_cont);
    lv_obj_set_size(cont, LV_HOR_RES - 64, LV_VER_RES - 80);
    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, -10);

    lv_obj_t *img = lv_img_create(cont, NULL);
    lv_obj_set_size(img, 32, 32);
    lv_obj_set_auto_realign(img, 1);
    lv_obj_align(img, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    data->img_icon = img;

    lv_obj_t *lbl = lv_label_create(cont, NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_set_width(lbl, LV_HOR_RES - 64);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 40);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    data->lbl_info = lbl;

    data->lbl_extra = lv_label_create(cont, data->lbl_info);
    lv_obj_align(data->lbl_extra, data->lbl_info, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(btn, 180, 48);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, STYLE_RED);

    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, STYLE_WHITE);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(lbl, view_intl_get_string(pmodel, STRINGS_RIMUOVI_DISPOSITIVO));
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(lbl, 100);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);

    img = lv_img_create(btn, NULL);
    lv_img_set_src(img, &img_trash);
    lv_obj_align(img, NULL, LV_ALIGN_IN_LEFT_MID, 8, 0);

    img = lv_img_create(btn, NULL);
    lv_img_set_src(img, &img_trash);
    lv_obj_align(img, NULL, LV_ALIGN_IN_RIGHT_MID, -8, 0);

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
            /*data->num_messages = event.num_messages;
            for (size_t i = 0; i < MAX_NUM_MESSAGES; i++) {
                data->messages[i] = event.messages[i];
            }*/
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

                        case DELETE_BTN_ID: {
                            model_delete_device(pmodel, data->device.address);
                            configuration_save_device_data(model_get_device(pmodel, data->device.address));
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SAVE;
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
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
    struct page_data *pdata = args;

    // for (size_t i = 0; i < pdata->num_messages; i++) {
    // free(pdata->messages[i]);
    //}
    free(pdata);
}


const pman_page_t page_device_info = {
    .create        = create_page,
    .destroy       = destroy_page,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};
