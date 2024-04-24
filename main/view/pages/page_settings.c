#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"
#include "view/intl/intl.h"
#include <esp_log.h>


enum {
    BUTTON_BACK_ID,
    DATETIME_BTN_ID,
    LANGUAGE_BTN_ID,
    DEGREES_BTN_ID,
    VOLUME_BTN_ID,
    BRIGHTNESS_BTN_ID,
    WIFI_BTN_ID,
};

struct page_data {
    lv_obj_t *btn_wifi;
};


static const char *TAG = "PageSettings";


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *data = arg;
    lv_obj_t         *btn;
    (void)data;

    view_common_title(BUTTON_BACK_ID, view_intl_get_string(pmodel, STRINGS_IMPOSTAZIONI), NULL);

    btn = view_common_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_LINGUA), 230, LANGUAGE_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_LEFT, 8, 100);
    lv_obj_t *btn_previous = btn;

    btn = view_common_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_DATA_ORA), 230, DATETIME_BTN_ID);
    lv_obj_align(btn, btn_previous, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    btn_previous = btn;

    btn = view_common_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_VOLUME), 230, VOLUME_BTN_ID);
    lv_obj_align(btn, btn_previous, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    btn_previous = btn;

    btn =
        view_common_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_LUMINOSITA), 230, BRIGHTNESS_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -8, 100);
    btn_previous = btn;

    btn = view_common_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_GRADI), 230, DEGREES_BTN_ID);
    lv_obj_align(btn, btn_previous, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    btn_previous = btn;

    btn = view_common_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_WIFI), 230, WIFI_BTN_ID);
    lv_obj_align(btn, btn_previous, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    ESP_LOGI(TAG, "Wifi Configured %i", model_get_wifi_configured(pmodel));
    lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DISABLED, LV_COLOR_GRAY);
    if (!model_get_wifi_configured(pmodel)) {
        lv_btn_set_state(btn, LV_STATE_DISABLED);
    }
    data->btn_wifi = btn;
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *data = arg;
    (void)data;
    view_message_t msg = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_OPEN:
        case VIEW_EVENT_CODE_ANCILLARY_DATA_UPDATE:
            if (model_get_wifi_configured(pmodel) && lv_btn_get_state(data->btn_wifi) == LV_BTN_STATE_DISABLED) {
                lv_btn_set_state(data->btn_wifi, LV_BTN_STATE_RELEASED);
            } else if (!model_get_wifi_configured(pmodel) &&
                       lv_btn_get_state(data->btn_wifi) == LV_BTN_STATE_RELEASED) {
                lv_btn_set_state(data->btn_wifi, LV_BTN_STATE_DISABLED);
            }
            break;

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

                        case BRIGHTNESS_BTN_ID: {
                            slider_parameter_t *args = view_common_slider_parameter_create(
                                view_intl_get_string(pmodel, STRINGS_LUMINOSITA), "%",
                                model_get_active_backlight(pmodel), 100, model_set_active_backlight);

                            msg.vmsg.code  = VIEW_COMMAND_CODE_CHANGE_PAGE_EXTRA;
                            msg.vmsg.page  = &page_parameter_slider;
                            msg.vmsg.extra = args;
                            break;
                        }

                        case VOLUME_BTN_ID: {
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_buzzer;
                            break;
                        }

                        case WIFI_BTN_ID: {
                            if (model_get_wifi_configured(pmodel)) {
                                msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                                msg.vmsg.page = &page_wifi;
                            }
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
