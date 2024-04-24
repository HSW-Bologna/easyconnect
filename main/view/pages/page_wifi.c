#include <stdlib.h>
#include "lvgl.h"
#include "model/model.h"
#include "src/lv_misc/lv_area.h"
#include "src/lv_misc/lv_task.h"
#include "src/lv_themes/lv_theme.h"
#include "src/lv_widgets/lv_btn.h"
#include "view/view.h"
#include "view/view_types.h"
#include "view/common.h"
#include "view/intl/intl.h"
#include "view/style.h"
#include "gel/pagemanager/page_manager.h"


enum {
    DOT_TIMER_ID,
    BACK_BTN_ID,
    WIFI_BTN_ID,
    AP_LIST_BTN_ID,
};


struct page_data {
    uint8_t    dots;
    lv_task_t *timer;

    lv_obj_t *lbl_connection;

    lv_obj_t *btn_enable;
    lv_obj_t *btn_list;
};


static void update_page(model_t *pmodel, struct page_data *pdata);
const char *dots_string(uint8_t dots);


static void *create_page(model_t *pmodel, void *extra) {
    struct page_data *pdata = malloc(sizeof(struct page_data));
    assert(pdata != NULL);
    pdata->timer = view_register_periodic_task(1000UL, LV_TASK_PRIO_OFF, DOT_TIMER_ID);
    return pdata;
}


static void open_page(model_t *pmodel, void *args) {
    lv_obj_t         *btn, *lbl;
    struct page_data *pdata = args;
    pdata->dots             = 0;

    lv_task_set_prio(pdata->timer, LV_TASK_PRIO_MID);

    view_common_title(BACK_BTN_ID, view_intl_get_string(pmodel, STRINGS_WIFI), NULL);

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(lbl, view_intl_get_string(pmodel, STRINGS_WIFI));
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_LEFT_MID, 32, -48);

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_checkable(btn, 1);
    lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_CHECKED, LV_COLOR_GREEN);
    lv_obj_set_size(btn, 80, 64);
    lbl = lv_label_create(btn, NULL);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_RIGHT_MID, -32, -48);
    view_register_default_callback(btn, WIFI_BTN_ID);
    pdata->btn_enable = btn;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(lbl, 400);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 32);
    pdata->lbl_connection = lbl;

    btn = view_common_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_RETI), 160, AP_LIST_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -8);
    pdata->btn_list = btn;

    update_page(pmodel, pdata);
}


static view_message_t page_event(model_t *pmodel, void *args, view_event_t event) {
    view_message_t    msg   = {0};
    struct page_data *pdata = args;

    switch (event.code) {
        case VIEW_EVENT_CODE_WIFI_UPDATE:
            update_page(pmodel, pdata);
            break;

        case VIEW_EVENT_CODE_TIMER:
            pdata->dots = (pdata->dots + 1) % 4;
            update_page(pmodel, pdata);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            if (event.lv_event == LV_EVENT_CLICKED) {
                switch (event.data.id) {
                    case BACK_BTN_ID:
                        msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                        break;

                    case AP_LIST_BTN_ID:
                        msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                        msg.vmsg.page = (void *)&page_wifi_ap_list;
                        msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SCAN_WIFI;
                        break;
                }
            } else if (event.lv_event == LV_EVENT_VALUE_CHANGED) {
                switch (event.data.id) {
                    case WIFI_BTN_ID:
                        model_toggle_wifi_enabled(pmodel);
                        update_page(pmodel, pdata);
                        msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_UPDATE_WIFI;
                        break;
                }
            }
            break;
        }

        default:
            break;
    }

    return msg;
}


static void update_page(model_t *pmodel, struct page_data *pdata) {
    if (model_get_wifi_enabled(pmodel)) {
        lv_obj_add_state(pdata->btn_enable, LV_STATE_CHECKED);
        lv_label_set_text(lv_obj_get_child(pdata->btn_enable, 0), view_intl_get_string(pmodel, STRINGS_ON));

        switch (model_get_wifi_state(pmodel)) {
            case WIFI_STATE_CONNECTED: {
                uint32_t ip = model_get_ip_addr(pmodel);
                lv_label_set_text_fmt(pdata->lbl_connection, "%s: %s (%i.%i.%i.%i)",
                                      view_intl_get_string(pmodel, STRINGS_CONNESSO), model_get_ssid(pmodel),
                                      IP_PART(ip, 0), IP_PART(ip, 1), IP_PART(ip, 2), IP_PART(ip, 3));
                break;
            }

            case WIFI_STATE_AUTH_ERROR: {
                if (strlen(model_get_ssid(pmodel)) == 0) {
                    lv_label_set_text(pdata->lbl_connection, view_intl_get_string(pmodel, STRINGS_NON_CONNESSO));
                } else {
                    lv_label_set_text_fmt(pdata->lbl_connection, "%s (%s) %s",
                                          view_intl_get_string(pmodel, STRINGS_AUTENTICAZIONE_FALLITA),
                                          model_get_ssid(pmodel), dots_string(pdata->dots));
                }
                break;
            }

            case WIFI_STATE_SSID_NOT_FOUND_ERROR: {
                if (strlen(model_get_ssid(pmodel)) == 0) {
                    lv_label_set_text(pdata->lbl_connection, view_intl_get_string(pmodel, STRINGS_NON_CONNESSO));
                } else {
                    lv_label_set_text_fmt(pdata->lbl_connection, "%s (%s) %s",
                                          view_intl_get_string(pmodel, STRINGS_RETE_NON_TROVATA),
                                          model_get_ssid(pmodel), dots_string(pdata->dots));
                }
                break;
            }

            case WIFI_STATE_CONNECTING: {
                if (strlen(model_get_ssid(pmodel)) == 0) {
                    lv_label_set_text(pdata->lbl_connection, view_intl_get_string(pmodel, STRINGS_NON_CONNESSO));
                } else {
                    lv_label_set_text_fmt(pdata->lbl_connection, "%s (%s) %s",
                                          view_intl_get_string(pmodel, STRINGS_CONNESSIONE_IN_CORSO),
                                          model_get_ssid(pmodel), dots_string(pdata->dots));
                }
                break;
            }

            case WIFI_STATE_DISCONNECTED:
                lv_label_set_text(pdata->lbl_connection, view_intl_get_string(pmodel, STRINGS_NON_CONNESSO));
                break;
        }

        view_common_set_hidden(pdata->lbl_connection, 0);
        view_common_set_hidden(pdata->btn_list, 0);
    } else {
        lv_obj_clear_state(pdata->btn_enable, LV_STATE_CHECKED);
        lv_label_set_text(lv_obj_get_child(pdata->btn_enable, 0), view_intl_get_string(pmodel, STRINGS_OFF));

        view_common_set_hidden(pdata->lbl_connection, 1);
        view_common_set_hidden(pdata->btn_list, 1);
    }
}


const char *dots_string(uint8_t dots) {
    switch (dots) {
        case 1:
            return ".";
        case 2:
            return "..";
        case 3:
            return "...";
        default:
            return "";
    }
}


static void close_page(void *args) {
    struct page_data *pdata = args;
    lv_task_set_prio(pdata->timer, LV_TASK_PRIO_OFF);
    lv_obj_clean(lv_scr_act());
}


static void destroy_page(void *args, void *extra) {
    (void)extra;
    struct page_data *pdata = args;
    lv_task_del(pdata->timer);
    free(pdata);
}


const pman_page_t page_wifi = {
    .id            = PAGE_ID_WIFI,
    .create        = create_page,
    .destroy       = destroy_page,
    .open          = open_page,
    .close         = close_page,
    .process_event = page_event,
};
