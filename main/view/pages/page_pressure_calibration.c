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


enum {
    BACK_BTN_ID,
    BTN_NEXT_ID,
};


struct page_data {
    lv_obj_t *lbl_speed;
    lv_obj_t *lbl_pressure;
};


static void update_page(model_t *pmodel, struct page_data *pdata);


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    lv_obj_t *btn, *lbl;

    struct page_data *pdata = arg;
    lv_obj_t         *title =
        view_common_title(BACK_BTN_ID, view_intl_get_string(pmodel, STRINGS_CALIBRAZIONE_PRESSIONE), NULL);
    (void)title;

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_size(btn, 56, 56);
    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_MID, 92, 64);
    lv_label_set_text(lbl, LV_SYMBOL_NEXT);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback(btn, BTN_NEXT_ID);

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_align(lbl, btn, LV_ALIGN_OUT_LEFT_MID, -32, 0);
    pdata->lbl_speed = lbl;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -56);
    lv_obj_set_auto_realign(lbl, 1);
    pdata->lbl_pressure = lbl;

    update_page(pmodel, pdata);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *pdata = arg;
    view_message_t    msg   = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_OPEN:
            break;

        case VIEW_EVENT_CODE_STATE_UPDATE:
        case VIEW_EVENT_CODE_DEVICE_UPDATE:
            update_page(pmodel, pdata);
            break;

        case VIEW_EVENT_CODE_DEVICE_MESSAGES:
            break;

        case VIEW_EVENT_CODE_DEVICE_ALARM:
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BACK_BTN_ID:
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_FAN_OFF;
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;

                        case BTN_NEXT_ID:
                            if (model_get_fan_state(pmodel) == MODEL_FAN_STATE_OFF) {
                                msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FAN;

                                int16_t pressure_1 = 0;
                                int16_t pressure_2 = 0;

                                // Align 1 and 2
                                model_get_raw_pressures(pmodel, &pressure_1, &pressure_2, NULL);
                                model_set_pressure_offset(pmodel, DEVICE_GROUP_2, -(pressure_2 - pressure_1));

                            } else {
                                uint8_t speed = model_get_fan_speed(pmodel);

                                int16_t pressure_1 = 0;
                                int16_t pressure_2 = 0;
                                model_get_pressures(pmodel, &pressure_1, &pressure_2, NULL);

                                if (pressure_1 < pressure_2) {
                                    model_set_pressure_difference(pmodel, speed, pressure_2 - pressure_1);
                                } else {
                                    model_set_pressure_difference(pmodel, speed, 0);
                                }

                                if (speed < MAX_FAN_SPEED - 1) {
                                    speed++;
                                    model_set_fan_speed(pmodel, speed);

                                    update_page(pmodel, pdata);
                                    msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SET_FAN_SPEED;
                                } else {
                                    msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_FAN_OFF;
                                    msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                                }
                            }
                            break;

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


static void update_page(model_t *pmodel, struct page_data *pdata) {
    if (model_get_fan_state(pmodel) == MODEL_FAN_STATE_OFF) {
        lv_label_set_text(pdata->lbl_speed, "0");
    } else {
        lv_label_set_text_fmt(pdata->lbl_speed, "%i", model_get_fan_speed(pmodel) + 1);
    }

    int16_t pressure_1 = 0;
    int16_t pressure_2 = 0;

    model_get_pressures(pmodel, &pressure_1, &pressure_2, NULL);
    lv_label_set_text_fmt(pdata->lbl_pressure, "Gruppo 1: %4i Pa      Gruppo 2: %4i Pa", pressure_1, pressure_2);
}


const pman_page_t page_pressure_calibration = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};