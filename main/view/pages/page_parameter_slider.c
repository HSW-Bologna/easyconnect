#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"
#include "view/intl/intl.h"


enum {
    BUTTON_BACK_ID,
    VALUE_SLIDER_ID,
    BUTTON_PLUS_ID,
    BUTTON_MINUS_ID,
};

struct page_data {
    slider_parameter_t *parameter;

    lv_obj_t *lbl;

    lv_obj_t *slider;
};


static void update_page(model_t *pmodel, struct page_data *pdata);


static void *create_page(model_t *model, void *extra) {
    struct page_data *pdata = malloc(sizeof(struct page_data));
    pdata->parameter        = extra;
    return pdata;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *pdata = arg;
    lv_obj_t         *slider, *lbl;

    view_common_title(BUTTON_BACK_ID, pdata->parameter->title, NULL);

    slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_size(slider, LV_HOR_RES - 80, 48);
    lv_slider_set_type(slider, LV_SLIDER_TYPE_NORMAL);
    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_slider_set_range(slider, 0, pdata->parameter->max);
    lv_slider_set_value(slider, pdata->parameter->initial, LV_ANIM_OFF);
    view_register_default_callback(slider, VALUE_SLIDER_ID);
    pdata->slider = slider;

    lv_obj_t *butn_plus  = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *butn_minus = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(butn_plus, 60, 60);
    lv_obj_set_size(butn_minus, 60, 60);
    lv_obj_t *lbl_plus  = lv_label_create(butn_plus, NULL);
    lv_obj_t *lbl_minus = lv_label_create(butn_minus, NULL);
    lv_label_set_text(lbl_plus, LV_SYMBOL_PLUS);
    lv_label_set_text(lbl_minus, LV_SYMBOL_MINUS);
    view_register_default_callback(butn_minus, BUTTON_MINUS_ID);
    view_register_default_callback(butn_plus, BUTTON_PLUS_ID);

    lv_obj_align(butn_plus, slider, LV_ALIGN_OUT_BOTTOM_RIGHT, -16, 32);
    lv_obj_align(butn_minus, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 16, 32);

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(lbl, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 32);
    pdata->lbl = lbl;

    update_page(pmodel, pdata);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *pdata = arg;
    view_message_t    msg   = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BUTTON_BACK_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;

                        case BUTTON_MINUS_ID:
                            if (pdata->parameter->initial > 0) {
                                pdata->parameter->initial--;
                                pdata->parameter->setter(pmodel, pdata->parameter->initial);
                                update_page(pmodel, pdata);
                            }
                            break;

                        case BUTTON_PLUS_ID:
                            if (pdata->parameter->initial < pdata->parameter->max) {
                                pdata->parameter->initial++;
                                pdata->parameter->setter(pmodel, pdata->parameter->initial);
                                update_page(pmodel, pdata);
                            }
                            break;
                    }
                    break;
                }

                case LV_EVENT_VALUE_CHANGED: {
                    switch (event.data.id) {
                        case VALUE_SLIDER_ID:
                            pdata->parameter->setter(pmodel, lv_slider_get_value(pdata->slider));
                            pdata->parameter->initial = lv_slider_get_value(pdata->slider);
                            update_page(pmodel, pdata);
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
    lv_slider_set_value(pdata->slider, pdata->parameter->initial, LV_ANIM_OFF);
    lv_label_set_text_fmt(pdata->lbl, "%i", pdata->parameter->initial);
}


const pman_page_t page_parameter_slider = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};