#include <stdlib.h>
#include "src/lv_core/lv_obj.h"
#include "src/lv_core/lv_obj_style_dec.h"
#include "src/lv_misc/lv_anim.h"
#include "src/lv_misc/lv_area.h"
#include "src/lv_misc/lv_color.h"
#include "src/lv_misc/lv_types.h"
#include "src/lv_widgets/lv_btn.h"
#include "src/lv_widgets/lv_cont.h"
#include "src/lv_widgets/lv_slider.h"
#include "view/view.h"
#include "model/model.h"

static lv_signal_cb_t ancestor_signal;

lv_res_t limit_cb(lv_obj_t *obj, lv_signal_t signal, void *arg) {

    if (signal == LV_SIGNAL_COORD_CHG) {
        if (lv_obj_get_y(obj) > 0) {
            lv_obj_set_y(obj, 0);
        }
        if (lv_obj_get_y(obj) < - 100) {
            lv_obj_set_y(obj, -100);
        }
    }
    return ancestor_signal(obj,signal,arg);
}


enum {
    BUTTON_PLUS_ID,
    BUTTON_MINUS_ID,
    SLIDER_ID
};

struct page_data {
    lv_obj_t *btn1;
    lv_obj_t *btn2;
    lv_obj_t *btn3;
    lv_obj_t *slider;
};


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;

    lv_obj_t *cont_drag = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont_drag, 250, 140);
    lv_obj_align(cont_drag,NULL, LV_ALIGN_OUT_TOP_MID, 0, 40);
    lv_obj_set_style_local_border_width(cont_drag, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_bg_opa(cont_drag, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
    lv_obj_t *cont = lv_cont_create(cont_drag, NULL);
    lv_obj_set_size(cont, 250, 100);
    lv_obj_align(cont,NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_drag(cont_drag, true);
    lv_obj_set_drag_parent(cont, true);
    lv_obj_set_drag_dir(cont_drag, LV_DRAG_DIR_VER);
    ancestor_signal = lv_obj_get_signal_cb(cont_drag);
    lv_obj_set_signal_cb(cont_drag, limit_cb);

    lv_obj_t *butn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_radius(butn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 8);
    lv_obj_t *butn2 = lv_btn_create(lv_scr_act(), butn1);
    lv_obj_t *butn3 = lv_btn_create(lv_scr_act(), butn1);
    lv_obj_set_size(butn1, 100, 100);
    lv_obj_set_size(butn2, 100, 100);
    lv_obj_set_size(butn3, 100, 100);
    lv_obj_align(butn1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_align(butn2, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
    lv_obj_align(butn3, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_btn_set_checkable(butn1,true);
    lv_btn_set_checkable(butn2,true);
    lv_btn_set_checkable(butn3,true);
    data->btn1 = butn1;
    data->btn2 = butn2;
    data->btn3 = butn3;

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_MID, -lv_obj_get_width(lbl)/2, 0);
    lv_label_set_text(lbl, "Easy Connect");

    lv_obj_t *butn_plus = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *butn_minus = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(butn_plus, 60, 60);
    lv_obj_set_size(butn_minus, 60, 60);
    lv_obj_align(butn_plus, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_obj_align(butn_minus, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    lv_obj_t *lbl_plus = lv_label_create(butn_plus, NULL);
    lv_obj_t *lbl_minus = lv_label_create(butn_minus, NULL);
    lv_label_set_text(lbl_plus, LV_SYMBOL_PLUS);
    lv_label_set_text(lbl_minus, LV_SYMBOL_MINUS);
    view_register_default_callback(butn_minus, BUTTON_MINUS_ID);
    view_register_default_callback(butn_plus, BUTTON_PLUS_ID);

    lv_obj_t *sl = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_size(sl, 30, LV_VER_RES_MAX-lv_obj_get_height(butn_plus)*2-50);
    lv_obj_align(sl, NULL, LV_ALIGN_IN_RIGHT_MID, -(lv_obj_get_width(butn_plus)/2)+lv_obj_get_width(sl)/2, 0);
    lv_slider_set_range(sl, 0, 4);
    data->slider = sl;
    view_register_default_callback(data->slider, SLIDER_ID);

}


static view_message_t process_page_event(model_t *model, void *arg, view_event_t event) {
    struct page_data *data = arg;
    view_message_t    msg  = {0};

    switch (event.code) {
    case VIEW_EVENT_CODE_LVGL: {
        switch (event.lv_event) {
            case LV_EVENT_CLICKED: {
                switch (event.data.id) {
                    case BUTTON_PLUS_ID: {
                        if (model->slider<4) {
                            model->slider++;
                            lv_slider_set_value(data->slider, model->slider, LV_ANIM_ON);
                        }
                        break;
                    }
                    case BUTTON_MINUS_ID: {
                        if (model->slider>0) {
                            model->slider--;
                            lv_slider_set_value(data->slider, model->slider, LV_ANIM_ON);
                        }
                        break;
                    }
                    default:
                    break;
                }
                break; 

            case LV_EVENT_VALUE_CHANGED: {
                switch (event.data.id) {
                    case SLIDER_ID: {
                        model->slider = lv_slider_get_value(data->slider);
                        break;
                    }
                    default:
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


pman_page_t page_main = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};