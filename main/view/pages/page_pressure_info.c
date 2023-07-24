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
#include "view/style.h"


enum {
    BACK_BTN_ID,
    BTN_OFFSET_GROUP,
    BTN_CLEAR_OFFSETS,
};


typedef struct {
    lv_obj_t *lbl_group;
    lv_obj_t *lbl_delta;
} pressure_widget_t;


struct page_data {
    pressure_widget_t widgets[DEVICE_GROUPS];
};


static void update_pressures(model_t *pmodel, struct page_data *pdata);


static pressure_widget_t pressure_widget_create(lv_obj_t *parent, device_group_t group) {
    lv_obj_t *btn, *lbl;

    lv_obj_t *cont = lv_cont_create(parent, NULL);
    lv_obj_add_style(cont, LV_CONT_PART_MAIN, &style_transparent_cont);
    lv_cont_set_layout(cont, LV_LAYOUT_OFF);
    lv_obj_set_size(cont, LV_HOR_RES - 32, 64);
    lv_page_glue_obj(cont, 1);

    btn = lv_btn_create(cont, NULL);
    lv_page_glue_obj(btn, 1);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_size(btn, 56, 56);
    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_align(btn, NULL, LV_ALIGN_IN_LEFT_MID, 16, 0);
    lv_label_set_text(lbl, "0");
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback_number(btn, BTN_OFFSET_GROUP, group);

    lbl = lv_label_create(cont, NULL);
    lv_page_glue_obj(lbl, 1);
    lv_obj_align(lbl, btn, LV_ALIGN_OUT_RIGHT_TOP, 16, 0);
    lv_obj_t *lbl_group = lbl;

    lbl = lv_label_create(cont, NULL);
    lv_page_glue_obj(lbl, 1);
    lv_obj_align(lbl, btn, LV_ALIGN_OUT_RIGHT_BOTTOM, 128, 0);
    lv_obj_t *lbl_delta = lbl;

    return (pressure_widget_t){.lbl_delta = lbl_delta, .lbl_group = lbl_group};
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    lv_obj_t *page;

    struct page_data *pdata = arg;
    lv_obj_t         *title = view_common_title(BACK_BTN_ID, view_intl_get_string(pmodel, STRINGS_PRESSIONE), NULL);
    (void)title;

    page = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(page, LV_HOR_RES, 270);
    lv_obj_align(page, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    lv_page_set_scrl_layout(page, LV_LAYOUT_COLUMN_LEFT);
    lv_obj_set_style_local_pad_inner(page, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_pad_top(page, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 0);

    for (device_group_t i = 0; i < DEVICE_GROUPS; i++) {
        pdata->widgets[i] = pressure_widget_create(page, i);
    }

    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_size(btn, 56, 56);
    lv_obj_t *lbl = lv_label_create(btn, NULL);
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
                            sensor_group_report_t pressures[DEVICE_GROUPS] = {0};
                            model_get_raw_pressures(pmodel, pressures);
                            model_set_pressure_offset(pmodel, event.data.number,
                                                      -pressures[event.data.number].pressure);

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
    sensor_group_report_t pressures[DEVICE_GROUPS] = {0};

    const char *missing_group = "Nessun dispositivo nel gruppo %i";
    const char *pressure_text = "Pressione gruppo %i: %i Pa";
    const char *delta_text    = "Delta %i-%i: %i Pa";

    int group = model_get_pressures(pmodel, pressures);
    for (int i = 0; i < DEVICE_GROUPS; i++) {
        if (i <= group) {
            lv_label_set_text_fmt(pdata->widgets[i].lbl_group, pressure_text, i + 1, pressures[i].pressure);
        } else {
            lv_label_set_text_fmt(pdata->widgets[i].lbl_group, missing_group, i + 1);
        }

        if (i + 1 <= group) {
            lv_label_set_text_fmt(pdata->widgets[i].lbl_delta, delta_text, i, i + 1,
                                  pressures[i].pressure - pressures[i + 1].pressure);
        } else {
            lv_label_set_text(pdata->widgets[i].lbl_delta, "");
        }
    }
}


const pman_page_t page_pressure_info = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};
