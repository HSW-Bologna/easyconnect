#include <stdio.h>
#include "lvgl.h"
#include "view.h"
#include "common.h"


lv_obj_t *view_common_back_button(int id) {
    lv_obj_t *back = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *lbl  = lv_label_create(back, NULL);
    lv_label_set_text(lbl, LV_SYMBOL_LEFT);
    lv_obj_set_size(back, 50, 64);
    lv_obj_align(back, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 4);
    view_register_default_callback(back, id);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());

    return back;
}


lv_obj_t *view_common_title(int id, char *string, lv_obj_t **label) {
    lv_obj_t *back = view_common_back_button(id);

    lv_obj_t * _internal;
    lv_obj_t **title;
    if (label) {
        title = label;
    } else {
        title = &_internal;
    }
    *title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(*title, LV_LABEL_LONG_SROLL_CIRC);
    lv_obj_set_size(*title, LV_HOR_RES - lv_obj_get_width(back) - 12, 52);
    lv_label_set_align(*title, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_style_local_text_font(*title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    lv_label_set_text(*title, string);
    lv_obj_align(*title, back, LV_ALIGN_OUT_RIGHT_MID, 4, 0);
    return back;
}
