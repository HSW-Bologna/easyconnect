#include <assert.h>
#include <stdio.h>
#include "lvgl.h"
#include "view.h"
#include "style.h"
#include "common.h"
#include "model/model.h"
#include "view/intl/intl.h"


LV_IMG_DECLARE(img_icona_luce_1);
LV_IMG_DECLARE(img_icona_luce_2);
LV_IMG_DECLARE(img_icona_luce_3);
LV_IMG_DECLARE(img_icona_elettrostatico);
LV_IMG_DECLARE(img_icona_uvc);
LV_IMG_DECLARE(img_icona_aspirazione);
LV_IMG_DECLARE(img_icona_immissione);

LV_IMG_DECLARE(img_icon_gas_sm);

LV_IMG_DECLARE(img_icon_pressure_temperature_sm);

LV_IMG_DECLARE(img_icon_temperature_humidity_sm);

LV_IMG_DECLARE(img_icon_pressure_temperature_humidity_sm);

LV_IMG_DECLARE(img_icon_immission_sm);

LV_IMG_DECLARE(img_back_blue);


lv_obj_t *view_common_back_button(lv_obj_t *parent, int id) {
    lv_obj_t *img = lv_img_create(parent, NULL);
    lv_img_set_src(img, &img_back_blue);
    lv_obj_align(img, NULL, LV_ALIGN_IN_TOP_RIGHT, -8, 8);
    lv_obj_set_click(img, 1);
    view_register_default_callback(img, id);

    return img;
}


slider_parameter_t *view_common_slider_parameter_create(const char *title, const char *um, uint16_t value, uint16_t max,
                                                        void (*setter)(model_t *pmodel, uint16_t)) {
    slider_parameter_t *res = malloc(sizeof(slider_parameter_t));
    assert(res != NULL);
    res->title   = title;
    res->initial = value;
    res->max     = max;
    res->setter  = setter;
    res->um      = um;
    return res;
}


lv_obj_t *view_common_title(int id, const char *string, lv_obj_t **label) {
    lv_obj_t *back = view_common_back_button(lv_scr_act(), id);

    lv_obj_t  *_internal;
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
    lv_obj_align(*title, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 0);
    return back;
}



lv_obj_t *view_common_limit_address_picker(model_t *pmodel, lv_obj_t *dd) {
    size_t  count        = 0;
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


lv_obj_t *view_common_menu_button(lv_obj_t *root, const char *text, size_t width, int id) {
    lv_obj_t *btn = lv_btn_create(root, NULL);
    lv_obj_set_size(btn, width, 60);
    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_SROLL);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(lbl, width - 8);
    lv_label_set_text(lbl, text);
    view_register_default_callback(btn, id);

    return btn;
}


lv_obj_t *view_common_default_menu_button(lv_obj_t *root, const char *text, int id) {
    return view_common_menu_button(root, text, 340, id);
}


void view_common_roller_set_option_from_to(lv_obj_t *roller, int from, int to, int mode) {
    if (from >= to) {
        lv_roller_set_options(roller, "", mode);
    } else {
        char *options = malloc((to - from) * 8);
        memset(options, 0, (to - from) * 8);
        for (int i = from; i < to; i++) {
            char tmp[16] = {0};
            if (i == to - 1)
                snprintf(tmp, 16, "%02i", i);
            else
                snprintf(tmp, 16, "%02i\n", i);

            strcat(options, tmp);
        }

        lv_roller_set_options(roller, options, mode);
        free(options);
    }
}


lv_obj_t *view_common_min_sec_rollers(lv_obj_t *parent, int visible, lv_obj_t **rethour, lv_obj_t **retmin) {
    lv_obj_t *cont = lv_cont_create(parent, NULL);
    lv_cont_set_layout(cont, LV_LAYOUT_OFF);

    lv_obj_t *hour = lv_roller_create(cont, NULL);
    lv_obj_set_style_local_text_font(hour, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_style_local_text_font(hour, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    view_common_roller_set_option_from_to(hour, 0, 24, LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_align(hour, LV_LABEL_ALIGN_LEFT);
    lv_roller_set_visible_row_count(hour, visible);
    lv_obj_align(hour, NULL, LV_ALIGN_CENTER, -lv_obj_get_width(hour) * 2 / 3, 0);
    lv_signal_send(hour, LV_SIGNAL_FOCUS, NULL);

    lv_obj_t *minute = lv_roller_create(cont, NULL);
    lv_obj_set_style_local_text_font(minute, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_style_local_text_font(minute, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    view_common_roller_set_option_from_to(minute, 0, 60, LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_align(minute, LV_LABEL_ALIGN_RIGHT);
    lv_roller_set_visible_row_count(minute, visible);
    lv_obj_align(minute, NULL, LV_ALIGN_CENTER, lv_obj_get_width(minute) * 2 / 3, 0);
    lv_signal_send(minute, LV_SIGNAL_FOCUS, NULL);

    lv_obj_t *lbl = lv_label_create(cont, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_text(lbl, ":");
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_cont_set_fit(cont, LV_FIT_TIGHT);

    lv_obj_add_style(cont, LV_CONT_PART_MAIN, &style_transparent_cont);

    if (rethour)
        *rethour = hour;
    if (retmin)
        *retmin = minute;

    return cont;
}


void view_common_set_hidden(lv_obj_t *obj, int hidden) {
    if (lv_obj_get_hidden(obj) != hidden) {
        lv_obj_set_hidden(obj, hidden);
    }
}


void view_common_get_class_string(model_t *pmodel, uint16_t class, char *string, size_t len) {
    switch (CLASS_GET_MODE(class)) {
        case DEVICE_MODE_LIGHT:
            strncpy(string, view_intl_get_string(pmodel, STRINGS_LUCE), len);
            break;

        case DEVICE_MODE_ESF:
            strncpy(string, "ESF", len);
            break;

        case DEVICE_MODE_UVC:
            strncpy(string, "UVC", len);
            break;

        case DEVICE_MODE_PRESSURE:
            strncpy(string, view_intl_get_string(pmodel, STRINGS_PRESSIONE), len);
            break;

        case DEVICE_MODE_TEMPERATURE_HUMIDITY:
            snprintf(string, len, "%s/%s", view_intl_get_string(pmodel, STRINGS_TEMPERATURA),
                     view_intl_get_string(pmodel, STRINGS_UMIDITA));
            break;

        case DEVICE_MODE_PRESSURE_TEMPERATURE_HUMIDITY:
            snprintf(string, len, "%s/%s", view_intl_get_string(pmodel, STRINGS_PRESSIONE),
                     view_intl_get_string(pmodel, STRINGS_UMIDITA));
            break;

        case DEVICE_MODE_FAN:
            if (class == DEVICE_CLASS_SIPHONING_FAN) {
                strncpy(string, view_intl_get_string(pmodel, STRINGS_ASPIRAZIONE), len);
            } else {
                strncpy(string, view_intl_get_string(pmodel, STRINGS_IMMISSIONE), len);
            }
            break;

        default:
            snprintf(string, len, "0x%X", class);
            break;
    }
}


void view_common_get_class_icon(uint16_t class, lv_obj_t *img) {
    // Icon
    switch (class) {
        case DEVICE_CLASS_LIGHT_1:
            lv_img_set_src(img, &img_icona_luce_1);
            break;
        case DEVICE_CLASS_LIGHT_2:
            lv_img_set_src(img, &img_icona_luce_2);
            break;
        case DEVICE_CLASS_LIGHT_3:
            lv_img_set_src(img, &img_icona_luce_3);
            break;
        case DEVICE_CLASS_ELECTROSTATIC_FILTER:
            lv_img_set_src(img, &img_icona_elettrostatico);
            break;
        case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1):
        case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2):
        case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3):
            lv_img_set_src(img, &img_icona_uvc);
            break;
        case DEVICE_CLASS_GAS:
            lv_img_set_src(img, &img_icon_gas_sm);
            break;
        case DEVICE_CLASS_IMMISSION_FAN:
            lv_img_set_src(img, &img_icon_immission_sm);
            break;
        case DEVICE_CLASS_SIPHONING_FAN:
            lv_img_set_src(img, &img_icona_aspirazione);
            break;

            CASE_ALL_GROUPS_FOR(DEVICE_CLASS_PRESSURE_SAFETY)
            lv_img_set_src(img, &img_icon_pressure_temperature_sm);
            break;

            CASE_ALL_GROUPS_FOR(DEVICE_CLASS_TEMPERATURE_HUMIDITY_SAFETY)
            lv_img_set_src(img, &img_icon_temperature_humidity_sm);
            break;

            CASE_ALL_GROUPS_FOR(DEVICE_CLASS_PRESSURE_TEMPERATURE_HUMIDITY_SAFETY)
            lv_img_set_src(img, &img_icon_pressure_temperature_humidity_sm);
            break;

        default:
            lv_obj_set_hidden(img, 1);
            break;
    }
}
