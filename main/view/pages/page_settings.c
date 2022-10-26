#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"
#include "view/intl/intl.h"


enum {
    BUTTON_BACK_ID,
    DATETIME_BTN_ID,
    LANGUAGE_BTN_ID,
    DEGREES_BTN_ID,
    VOLUME_BTN_ID,
    BRIGHTNESS_BTN_ID,
    FILTRI_BTN_ID,
};

struct page_data {};


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *data = arg;
    lv_obj_t         *btn;
    (void)data;

    view_common_title(BUTTON_BACK_ID, view_intl_get_string(pmodel, STRINGS_IMPOSTAZIONI), NULL);

    btn = view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_LINGUA), LANGUAGE_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_LEFT, 8, 100);
    lv_obj_t *btn_previous = btn;

    btn =
        view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_DATA_ORA), DATETIME_BTN_ID);
    lv_obj_align(btn, btn_previous, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    btn_previous = btn;

    btn = view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_VOLUME), VOLUME_BTN_ID);
    lv_obj_align(btn, btn_previous, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    btn_previous = btn;

    btn = view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_LUMINOSITA),
                                          BRIGHTNESS_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -8, 100);
    btn_previous = btn;

    btn = view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_GRADI), DEGREES_BTN_ID);
    lv_obj_align(btn, btn_previous, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    btn_previous = btn;

    btn = view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_FILTRI), FILTRI_BTN_ID);
    lv_obj_align(btn, btn_previous, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    btn_previous = btn;
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
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

                        case DATETIME_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_datetime;
                            break;

                        case LANGUAGE_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_language;
                            break;

                        case DEGREES_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_degrees;
                            break;

                        case FILTRI_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_filters;
                            break;

                        case BRIGHTNESS_BTN_ID: {
                            slider_parameter_t *args = view_common_slider_parameter_create(
                                view_intl_get_string(pmodel, STRINGS_LUMINOSITA), model_get_active_backlight(pmodel),
                                100, model_set_active_backlight);

                            msg.vmsg.code  = VIEW_COMMAND_CODE_CHANGE_PAGE_EXTRA;
                            msg.vmsg.page  = &page_parameter_slider;
                            msg.vmsg.extra = args;
                            break;
                        }

                        case VOLUME_BTN_ID: {
                            slider_parameter_t *args = view_common_slider_parameter_create(
                                view_intl_get_string(pmodel, STRINGS_VOLUME), model_get_buzzer_volume(pmodel),
                                MAX_BUZZER_VOLUME, model_set_buzzer_volume);

                            msg.vmsg.code  = VIEW_COMMAND_CODE_CHANGE_PAGE_EXTRA;
                            msg.vmsg.page  = &page_parameter_slider;
                            msg.vmsg.extra = args;
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


const pman_page_t page_settings = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};