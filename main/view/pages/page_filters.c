#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"
#include "view/intl/intl.h"


enum {
    BUTTON_BACK_ID,
    BUTTON_MINUS_ID,
    BUTTON_PLUS_ID,
};

struct page_data {
    lv_obj_t *lbls[MAX_FAN_SPEED];
};


static void update_page(model_t *pmodel, struct page_data *pdata);


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *pdata = arg;
    lv_obj_t         *btn, *lbl;

    view_common_title(BUTTON_BACK_ID, view_intl_get_string(pmodel, STRINGS_FILTRI), NULL);

    for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
        lbl = lv_label_create(lv_scr_act(), NULL);
        lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
        lv_obj_set_auto_realign(lbl, 1);
        lv_label_set_align(lbl, LV_ALIGN_CENTER);

        if (i < 2) {
            lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, -(i + 1) * 80, 30);
        } else {
            lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, (i - 2) * 80, 30);
        }

        pdata->lbls[i] = lbl;

        btn = lv_btn_create(lv_scr_act(), NULL);
        lbl = lv_label_create(btn, NULL);
        lv_label_set_text(lbl, LV_SYMBOL_MINUS);
        lv_obj_set_size(btn, 64, 64);
        lv_obj_align(btn, pdata->lbls[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
        view_register_default_callback_number(btn, BUTTON_MINUS_ID, i);
        lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());

        btn = lv_btn_create(lv_scr_act(), NULL);
        lbl = lv_label_create(btn, NULL);
        lv_label_set_text(lbl, LV_SYMBOL_PLUS);
        lv_obj_set_size(btn, 64, 64);
        lv_obj_align(btn, pdata->lbls[i], LV_ALIGN_OUT_TOP_MID, 0, -16);
        view_register_default_callback_number(btn, BUTTON_PLUS_ID, i);
        lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    }

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
                            model_modify_filters_for_speed(pmodel, event.data.number, -1);
                            update_page(pmodel, pdata);
                            break;

                        case BUTTON_PLUS_ID:
                            model_modify_filters_for_speed(pmodel, event.data.number, +1);
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
    for (uint16_t i = 0; i < MAX_FAN_SPEED; i++) {
        lv_label_set_text_fmt(pdata->lbls[i], "%i", model_get_filters_for_speed(pmodel, i) + 1);
    }
}


const pman_page_t page_filters = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};