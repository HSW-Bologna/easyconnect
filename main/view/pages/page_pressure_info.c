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
    BTN_OFFSET_GROUP,
    BTN_CLEAR_OFFSETS,
};


struct page_data {
    lv_obj_t *lbl_group_1;
    lv_obj_t *lbl_delta_1;
    lv_obj_t *lbl_group_2;
    lv_obj_t *lbl_delta_2;
    lv_obj_t *lbl_group_3;
};


static void update_pressures(model_t *pmodel, struct page_data *pdata);


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    lv_obj_t *lbl, *btn;

    struct page_data *pdata = arg;
    lv_obj_t         *title = view_common_title(BACK_BTN_ID, view_intl_get_string(pmodel, STRINGS_PRESSIONE), NULL);
    (void)title;

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_size(btn, 56, 56);
    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_LEFT, 16, 64);
    lv_label_set_text(lbl, "0");
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback_number(btn, BTN_OFFSET_GROUP, DEVICE_GROUP_1);

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(lbl, btn, LV_ALIGN_OUT_RIGHT_TOP, 16, 0);
    pdata->lbl_group_1 = lbl;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(lbl, btn, LV_ALIGN_OUT_RIGHT_BOTTOM, 128, 0);
    pdata->lbl_delta_1 = lbl;

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_size(btn, 56, 56);
    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_LEFT, 16, 160);
    lv_label_set_text(lbl, "0");
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback_number(btn, BTN_OFFSET_GROUP, DEVICE_GROUP_2);

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(lbl, btn, LV_ALIGN_OUT_RIGHT_TOP, 16, 0);
    pdata->lbl_group_2 = lbl;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(lbl, btn, LV_ALIGN_OUT_RIGHT_BOTTOM, 128, 0);
    pdata->lbl_delta_2 = lbl;

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_size(btn, 56, 56);
    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_LEFT, 16, 256);
    lv_label_set_text(lbl, "0");
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback_number(btn, BTN_OFFSET_GROUP, DEVICE_GROUP_3);

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(lbl, btn, LV_ALIGN_OUT_RIGHT_TOP, 16, 0);
    pdata->lbl_group_3 = lbl;


    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_size(btn, 56, 56);
    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -8);
    lv_label_set_text(lbl, "C");
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback(btn, BTN_CLEAR_OFFSETS);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *pdata = arg;
    view_message_t    msg   = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_DEVICE_MESSAGES:
            break;

        case VIEW_EVENT_CODE_OPEN:
        case VIEW_EVENT_CODE_DEVICE_UPDATE:
        case VIEW_EVENT_CODE_DEVICE_ALARM:
            update_pressures(pmodel, pdata);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BACK_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;

                        case BTN_OFFSET_GROUP: {
                            int16_t pressures[3] = {0};
                            model_get_raw_pressures(pmodel, &pressures[0], &pressures[1], &pressures[2]);
                            model_set_pressure_offset(pmodel, event.data.number, -pressures[event.data.number]);

                            update_pressures(pmodel, pdata);
                            break;
                        }

                        case BTN_CLEAR_OFFSETS: {
                            model_set_pressure_offset(pmodel, DEVICE_GROUP_1, 0);
                            model_set_pressure_offset(pmodel, DEVICE_GROUP_2, 0);
                            model_set_pressure_offset(pmodel, DEVICE_GROUP_3, 0);

                            update_pressures(pmodel, pdata);
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
            break;
        }

        default:
            break;
    }

    return msg;
}


static void update_pressures(model_t *pmodel, struct page_data *pdata) {
    int16_t pressure_1 = 0;
    int16_t pressure_2 = 0;
    int16_t pressure_3 = 0;

    const char *missing_group = "Nessun dispositivo nel gruppo %i";
    const char *pressure_text = "Pressione gruppo %i: %i Pa";
    const char *delta_text    = "Delta %i-%i: %i Pa";

    int group = model_get_pressures(pmodel, &pressure_1, &pressure_2, &pressure_3);
    switch (group) {
        case DEVICE_GROUP_1:
            lv_label_set_text_fmt(pdata->lbl_group_1, pressure_text, 1, pressure_1);
            lv_label_set_text_fmt(pdata->lbl_group_2, missing_group, 2);
            lv_label_set_text_fmt(pdata->lbl_group_3, missing_group, 3);
            lv_label_set_text(pdata->lbl_delta_1, "");
            lv_label_set_text(pdata->lbl_delta_2, "");
            break;

        case DEVICE_GROUP_2:
            lv_label_set_text_fmt(pdata->lbl_group_1, pressure_text, 1, pressure_1);
            lv_label_set_text_fmt(pdata->lbl_delta_1, delta_text, 1, 2, pressure_1 - pressure_2);
            lv_label_set_text_fmt(pdata->lbl_group_2, pressure_text, 2, pressure_2);
            lv_label_set_text_fmt(pdata->lbl_group_3, missing_group, 3);
            lv_label_set_text(pdata->lbl_delta_2, "");
            break;

        case DEVICE_GROUP_3:
            lv_label_set_text_fmt(pdata->lbl_group_1, pressure_text, 1, pressure_1);
            lv_label_set_text_fmt(pdata->lbl_delta_1, delta_text, 1, 2, pressure_1 - pressure_2);
            lv_label_set_text_fmt(pdata->lbl_group_2, pressure_text, 2, pressure_2);
            lv_label_set_text_fmt(pdata->lbl_delta_2, delta_text, 2, 3, pressure_2 - pressure_3);
            lv_label_set_text_fmt(pdata->lbl_group_3, pressure_text, 3, pressure_3);
            break;

        default:
            lv_label_set_text_fmt(pdata->lbl_group_1, missing_group, 1);
            lv_label_set_text_fmt(pdata->lbl_group_2, missing_group, 2);
            lv_label_set_text_fmt(pdata->lbl_group_3, missing_group, 3);
            lv_label_set_text(pdata->lbl_delta_1, "");
            lv_label_set_text(pdata->lbl_delta_2, "");
            break;
    }
}


const pman_page_t page_pressure_info = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};