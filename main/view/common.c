#include <stdio.h>
#include "lvgl.h"
#include "view.h"
#include "common.h"
#include "model/model.h"


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



lv_obj_t *view_common_limit_address_picker(model_t *pmodel, lv_obj_t *dd) {
    size_t    count = 0;
    uint8_t prev_address = 0;
    uint8_t address      = model_get_available_address(pmodel, prev_address);

    lv_dropdown_clear_options(dd);

    while (address != prev_address) {
        char string[32] = {0};
        snprintf(string, sizeof(string), "%i", address);
        lv_dropdown_add_option(dd, string, count);
        count++;
        prev_address = address;
        address      = model_get_available_address(pmodel, prev_address);
    }

    return dd;
}



lv_obj_t *view_common_address_picker(lv_obj_t *root, int id) {
    lv_obj_t *dd = lv_dropdown_create(root, NULL);
    lv_obj_set_style_local_text_font(dd, LV_DROPDOWN_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_style_local_text_font(dd, LV_DROPDOWN_PART_LIST, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_style_local_text_font(dd, LV_DROPDOWN_PART_SELECTED, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());

    for (size_t i = 1; i < 256; i++) {
        char string[32] = {0};
        snprintf(string, sizeof(string), "%zu", i);
        lv_dropdown_add_option(dd, string, i - 1);
    }

    return dd;
}


lv_obj_t *view_common_menu_button(lv_obj_t *root, char *text, int id) {
    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(btn, 220, 60);
    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_text(lbl, text);
    view_register_default_callback(btn, id);

    return btn;
}