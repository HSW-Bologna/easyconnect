#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"
#include "view/intl/intl.h"
#include "utils/utils.h"


#define STARTING_YEAR 2022


enum {
    BUTTON_BACK_ID,
    ROLLER_DAY_ID,
    ROLLER_MONTH_ID,
    ROLLER_YEAR_ID,
    BTN_OK_ID,
};

struct page_data {
    lv_obj_t *day;
    lv_obj_t *month;
    lv_obj_t *year;
    uint16_t  oldyear;
    uint16_t  oldmonth;
};


static int is_leap(int year) {
    return (year % 4 == 0) && (!(year % 100 == 0) || (year % 400 == 0));
}


static void update_days_roller(model_t *pmodel, struct page_data *data) {
    int       year              = lv_roller_get_selected(data->year) + STARTING_YEAR;
    int       month             = lv_roller_get_selected(data->month);
    const int days_by_month[12] = {31, is_leap(year) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    const int weekdays[] = {
        STRINGS_MON, STRINGS_TUE, STRINGS_WED, STRINGS_THU, STRINGS_FRI, STRINGS_SAT, STRINGS_SUN,
    };

    char day_options[10 * 31] = {0};
    for (int i = 0; i < days_by_month[month]; i++) {
        char      tmp[10] = {0};
        struct tm tm_info = {.tm_year = year - 1900, .tm_mon = month, .tm_mday = i};
        time_t    t       = mktime(&tm_info);
        int       wday    = localtime(&t)->tm_wday;

        if (i == days_by_month[month] - 1) {
            snprintf(tmp, 10, "%s %i", view_intl_get_string(pmodel, weekdays[wday]), i + 1);
        } else {
            snprintf(tmp, 10, "%s %i\n", view_intl_get_string(pmodel, weekdays[wday]), i + 1);
        }
        strcat(day_options, tmp);
    }

    lv_roller_set_options(data->day, day_options, LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_fix_width(data->day, 150);
    lv_roller_set_selected(data->day, 0, LV_ANIM_OFF);
}




static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;
    (void)data;

    time_t    timer;
    struct tm tm_info;

    timer   = time(NULL);
    tm_info = *localtime(&timer);

    char            month_options[8 * 12] = {0};
    const strings_t months[]              = {
        STRINGS_JEN, STRINGS_FEB, STRINGS_MAR, STRINGS_APR, STRINGS_MAY, STRINGS_JUN,
        STRINGS_JUL, STRINGS_AUG, STRINGS_SEP, STRINGS_OCT, STRINGS_NOV, STRINGS_DEC,
    };
    for (int i = 0; i < 12; i++) {
        char tmp[16] = {0};
        if (i == 11) {
            snprintf(tmp, 16, "%s", view_intl_get_string(model, months[i]));
        } else {
            snprintf(tmp, 16, "%s\n", view_intl_get_string(model, months[i]));
        }
        strcat(month_options, tmp);
    }

    lv_obj_t *om = lv_objmask_create(lv_scr_act(), NULL);
    lv_obj_set_size(om, LV_HOR_RES_MAX, 220);
    lv_obj_align(om, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *month = lv_roller_create(om, NULL);
    lv_obj_set_style_local_text_font(month, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_style_local_text_font(month, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_roller_set_options(month, month_options, LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_visible_row_count(month, 5);
    lv_obj_align(month, NULL, LV_ALIGN_CENTER, -30, 0);
    lv_roller_set_selected(month, tm_info.tm_mon, LV_ANIM_OFF);
    data->month = month;
    lv_signal_send(month, LV_SIGNAL_FOCUS, NULL);
    view_register_default_callback(month, ROLLER_MONTH_ID);

    char year_options[128 * 8] = {0};
    // Con piu' di 80 anni il roller si rompe
    for (int i = 0; i < 80; i++) {
        char tmp[16] = {0};
        if (i == 79) {
            snprintf(tmp, 16, "%i", STARTING_YEAR + i);
        } else {
            snprintf(tmp, 16, "%i\n", STARTING_YEAR + i);
        }
        strcat(year_options, tmp);
    }

    lv_obj_t *year = lv_roller_create(om, NULL);
    lv_obj_set_style_local_text_font(year, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_style_local_text_font(year, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_roller_set_options(year, year_options, LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_visible_row_count(year, 5);
    lv_obj_align(year, month, LV_ALIGN_OUT_LEFT_MID, 0, 0);
    view_register_default_callback(year, ROLLER_YEAR_ID);
    lv_signal_send(year, LV_SIGNAL_FOCUS, NULL);

    if (tm_info.tm_year < STARTING_YEAR - 1900) {
        lv_roller_set_selected(year, 0, LV_ANIM_OFF);
    } else {
        lv_roller_set_selected(year, (tm_info.tm_year + 1900) - STARTING_YEAR, LV_ANIM_OFF);
    }
    data->year = year;

    data->oldmonth = lv_roller_get_selected(month);
    data->oldyear  = lv_roller_get_selected(year);

    lv_obj_t *day = lv_roller_create(om, NULL);
    lv_obj_set_style_local_text_font(day, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_style_local_text_font(day, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_roller_set_visible_row_count(day, 5);
    data->day = day;
    update_days_roller(model, data);
    lv_roller_set_selected(data->day, tm_info.tm_mday - 1, LV_ANIM_OFF);
    lv_obj_align(day, month, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    view_register_default_callback(day, ROLLER_DAY_ID);
    lv_signal_send(day, LV_SIGNAL_FOCUS, NULL);

    lv_area_t                 a;
    lv_draw_mask_fade_param_t f1;
    a.x1 = 0;
    a.y1 = 0;
    a.x2 = LV_HOR_RES_MAX;
    a.y2 = 40;
    lv_draw_mask_fade_init(&f1, &a, LV_OPA_TRANSP, 10, LV_OPA_COVER, 40);
    lv_objmask_add_mask(om, &f1);

    a.x1 = 0;
    a.y1 = 180;
    a.x2 = LV_HOR_RES_MAX;
    a.y2 = 220;
    lv_draw_mask_fade_init(&f1, &a, LV_OPA_COVER, 180, LV_OPA_TRANSP, 210);
    lv_objmask_add_mask(om, &f1);

    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, LV_SYMBOL_OK);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback(btn, BTN_OK_ID);
    lv_obj_set_size(btn, 48, 48);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -5, -5);

    view_common_title(BUTTON_BACK_ID, "Data", NULL);
}


static view_message_t process_page_event(model_t *model, void *arg, view_event_t event) {
    struct page_data *data = arg;
    (void)data;
    view_message_t msg = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BUTTON_BACK_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;

                        case BTN_OK_ID: {
                            time_t    timer   = time(NULL);
                            struct tm tm_info = *localtime(&timer);
                            tm_info.tm_year   = lv_roller_get_selected(data->year) + STARTING_YEAR - 1900;
                            tm_info.tm_mon    = lv_roller_get_selected(data->month);
                            tm_info.tm_mday   = lv_roller_get_selected(data->day) + 1;
                            utils_set_system_time(tm_info);

                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SET_RTC;
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;
                        }
                    }
                    break;
                }

                case LV_EVENT_VALUE_CHANGED:
                    switch (event.data.id) {
                        case ROLLER_DAY_ID:
                            break;

                        case ROLLER_YEAR_ID:
                        case ROLLER_MONTH_ID: {
                            int month = lv_roller_get_selected(data->month);
                            int year  = lv_roller_get_selected(data->year);
                            if (month != data->oldmonth || year != data->oldyear) {
                                update_days_roller(model, data);
                                lv_roller_set_selected(data->day, 0, LV_ANIM_OFF);
                                data->oldmonth = lv_roller_get_selected(data->month);
                                data->oldyear  = lv_roller_get_selected(data->year);
                            }
                            break;
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

    return msg;
}


const pman_page_t page_date = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};