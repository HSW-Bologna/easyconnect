#include <stdlib.h>
#include "lvgl.h"
#include "model/model.h"
#include "src/lv_core/lv_disp.h"
#include "src/lv_core/lv_obj.h"
#include "src/lv_font/lv_symbol_def.h"
#include "src/lv_misc/lv_area.h"
#include "src/lv_themes/lv_theme.h"
#include "src/lv_widgets/lv_label.h"
#include "src/lv_widgets/lv_page.h"
#include "view/view.h"
#include "view/view_types.h"
#include "view/common.h"
#include "view/intl/intl.h"
#include "view/style.h"


#define MAX_MENU_ITEMS MAX_AP_SCAN_LIST_SIZE


enum {
    BACK_BTN_ID,
    CONNECT_BTN_ID,
    SCAN_BTN_ID,
};


struct page_data {
    lv_obj_t *spinner;
    lv_obj_t *lbl_no_networks;
    lv_obj_t *refresh;
    lv_obj_t *list;
};


static void      update_wifi_list(model_t *pmodel, struct page_data *pdata);
static lv_obj_t *create_list(void);


static void *create_page(model_t *pmodel, void *extra) {
    struct page_data *pdata = malloc(sizeof(struct page_data));
    assert(pdata != NULL);

    return pdata;
}


static void open_page(model_t *pmodel, void *args) {
    struct page_data *pdata = args;

    view_common_title(BACK_BTN_ID, view_intl_get_string(pmodel, STRINGS_RETI), NULL);

    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), NULL);
    lv_obj_align(spinner, NULL, LV_ALIGN_CENTER, 0, 0);
    pdata->spinner = spinner;

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_small());
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(lbl, 400);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(lbl, view_intl_get_string(pmodel, STRINGS_NESSUNA_RETE_TROVATA));
    pdata->lbl_no_networks = lbl;


    lv_obj_t *page = create_list();
    pdata->list    = page;

    lv_obj_t *refresh = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(refresh, 48, 48);
    lbl = lv_label_create(refresh, NULL);
    lv_label_set_text(lbl, LV_SYMBOL_REFRESH);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback(refresh, SCAN_BTN_ID);

    lv_obj_align(refresh, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    pdata->refresh = refresh;

    update_wifi_list(pmodel, pdata);
}


static view_message_t page_event(model_t *pmodel, void *args, view_event_t event) {
    view_message_t    msg   = {0};
    struct page_data *pdata = args;

    switch (event.code) {
        case VIEW_EVENT_CODE_WIFI_UPDATE:
            update_wifi_list(pmodel, pdata);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            if (event.lv_event == LV_EVENT_CLICKED) {
                switch (event.data.id) {
                    case BACK_BTN_ID:
                        msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                        break;

                    case SCAN_BTN_ID:
                        msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SCAN_WIFI;
                        break;

                    case CONNECT_BTN_ID: {
                        char *ssid = malloc(33);
                        assert(ssid != NULL);
                        memset(ssid, 0, 33);
                        strcpy(ssid, pmodel->ap_list[event.data.number]);
                        msg.vmsg.code  = VIEW_COMMAND_CODE_CHANGE_PAGE_EXTRA;
                        msg.vmsg.page  = (void *)&page_wifi_psk;
                        msg.vmsg.extra = ssid;
                        break;
                    }
                }
            }
            break;
        }

        default:
            break;
    }

    return msg;
}


static void update_wifi_list(model_t *pmodel, struct page_data *pdata) {
    if (model_get_scanning(pmodel)) {
        view_common_set_hidden(pdata->list, 1);
        view_common_set_hidden(pdata->refresh, 1);
        view_common_set_hidden(pdata->lbl_no_networks, 1);
        view_common_set_hidden(pdata->spinner, 0);
    } else if (model_get_available_networks_count(pmodel) == 0) {
        view_common_set_hidden(pdata->list, 1);
        view_common_set_hidden(pdata->refresh, 0);
        view_common_set_hidden(pdata->lbl_no_networks, 0);
        view_common_set_hidden(pdata->spinner, 1);
    } else {
        view_common_set_hidden(pdata->list, 0);
        view_common_set_hidden(pdata->lbl_no_networks, 1);
        view_common_set_hidden(pdata->spinner, 1);
        view_common_set_hidden(pdata->refresh, 0);

        lv_obj_del(pdata->list);
        pdata->list = create_list();

        for (size_t j = 0; j < model_get_available_networks_count(pmodel); j++) {
            lv_obj_t *btn = lv_btn_create(pdata->list, NULL);
            lv_obj_set_size(btn, 320, 60);
            lv_page_glue_obj(btn, 1);

            lv_obj_t *lbl = lv_label_create(btn, NULL);
            lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
            lv_label_set_long_mode(lbl, LV_LABEL_LONG_SROLL);
            lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
            lv_obj_set_width(lbl, 310);
            lv_label_set_text(lbl, pmodel->ap_list[j]);
            view_register_default_callback_number(btn, CONNECT_BTN_ID, j);
        }
    }
}


static lv_obj_t *create_list(void) {
    lv_obj_t *page = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(page, LV_HOR_RES - 32, 260);
    lv_obj_align(page, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    lv_page_set_scrl_layout(page, LV_LAYOUT_COLUMN_LEFT);
    lv_obj_set_style_local_pad_inner(page, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 4);
    lv_obj_set_style_local_pad_top(page, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 4);

    return page;
}


const pman_page_t page_wifi_ap_list = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = page_event,
};
