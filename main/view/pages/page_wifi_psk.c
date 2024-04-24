#include <stdlib.h>
#include "lvgl.h"
#include "model/model.h"
#include "src/lv_core/lv_obj.h"
#include "src/lv_misc/lv_area.h"
#include "src/lv_themes/lv_theme.h"
#include "src/lv_widgets/lv_btnmatrix.h"
#include "src/lv_widgets/lv_keyboard.h"
#include "src/lv_widgets/lv_label.h"
#include "view/view.h"
#include "view/view_types.h"
#include "view/common.h"
#include "view/intl/intl.h"
#include "view/style.h"
#include "gel/pagemanager/page_manager.h"


#define KB_BTN(width) width


enum {
    BACK_BTN_ID,
    PASSWORD_KB_ID,
};


struct page_data {
    char     *ssid;
    lv_obj_t *textarea;
    lv_obj_t *keyboard;
};


static const char *const kb_map_spec[] = {
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    LV_SYMBOL_BACKSPACE,
    "\n",
    "abc",
    "+",
    "-",
    "/",
    "*",
    "=",
    "%",
    "!",
    "?",
    "#",
    "<",
    ">",
    "\n",
    "\\",
    "@",
    "$",
    "(",
    ")",
    "{",
    "}",
    "[",
    "]",
    ";",
    "\"",
    "'",
    "\n",
    LV_SYMBOL_KEYBOARD,
    "&",
    " ",
    "\u20AC",     // Euro symbol
    LV_SYMBOL_OK,
    "",
};

static const lv_btnmatrix_ctrl_t kb_ctrl_spec_map[] = {
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    LV_BTNMATRIX_CTRL_CHECKABLE | 2,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    KB_BTN(1),
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    LV_BTNMATRIX_CTRL_CHECKABLE | 2,
    6,
    LV_BTNMATRIX_CTRL_CHECKABLE | 2,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
};



static void update_page(model_t *pmodel, struct page_data *pdata);


static void *create_page(model_t *pmodel, void *extra) {
    struct page_data *pdata = malloc(sizeof(struct page_data));
    assert(pdata != NULL);
    pdata->ssid = extra;
    return pdata;
}


static void open_page(model_t *pmodel, void *args) {
    struct page_data *pdata = args;

    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(btn, 56, 56);
    lv_obj_t *symbol = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(symbol, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_text(symbol, LV_SYMBOL_LEFT);
    lv_obj_align(symbol, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    view_register_default_callback(btn, BACK_BTN_ID);

    lv_obj_t *ta = lv_textarea_create(lv_scr_act(), NULL);
    lv_textarea_set_one_line(ta, 1);
    lv_textarea_set_text(ta, "");
    lv_textarea_set_max_length(ta, 33);
    lv_obj_set_width(ta, LV_HOR_RES - 60);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_obj_add_state(ta, LV_STATE_FOCUSED);
    pdata->textarea = ta;

    lv_obj_t *kb = lv_keyboard_create(lv_scr_act(), NULL);
#if 0
    lv_obj_set_style_text_font(kb, STYLE_FONT_MEDIUM, LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_style_border_color(kb, STYLE_GRAY, LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_style_border_opa(kb, LV_OPA_MAX, LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_style_border_width(kb, 2, LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_style_bg_color(kb, lv_color_make(0, 0, 0), LV_STATE_DEFAULT | LV_PART_ITEMS | LV_PART_MAIN);
#endif
    lv_keyboard_set_textarea(kb, ta);
    lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES - 64);
    lv_obj_align(kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_SPECIAL, (const char **)kb_map_spec);
    lv_keyboard_set_ctrl_map(kb, LV_KEYBOARD_MODE_SPECIAL, kb_ctrl_spec_map);
    view_register_keyboard_default_callback_number(kb, PASSWORD_KB_ID, 0);
    pdata->keyboard = kb;

    update_page(pmodel, pdata);
}


static view_message_t page_event(model_t *pmodel, void *args, view_event_t event) {
    view_message_t    msg   = {0};
    struct page_data *pdata = args;

    switch (event.code) {
        case VIEW_EVENT_CODE_WIFI_UPDATE:
            update_page(pmodel, pdata);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            if (event.lv_event == LV_EVENT_CLICKED) {
                switch (event.data.id) {
                    case BACK_BTN_ID:
                        msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                        break;
                }
            } else if (event.lv_event == LV_EVENT_VALUE_CHANGED) {
                switch (event.data.id) {
                    case PASSWORD_KB_ID: {
                        /*size_t len = strlen(lv_textarea_get_text(pdata->textarea));
                        if (len >= 8 || len == 0) {
                            lv_obj_set_style_border_color(pdata->textarea, STYLE_GRAY, LV_STATE_DEFAULT);
                        }*/
                        break;
                    }

                    default:
                        break;
                }
            } else if (event.lv_event == LV_EVENT_APPLY) {
                switch (event.data.id) {
                    case PASSWORD_KB_ID: {
                        const char *string_value = lv_textarea_get_text(pdata->textarea);

                        size_t len = strlen(string_value);
                        if (len > 0 && len < 8) {
                            // lv_obj_set_style_border_color(pdata->textarea, STYLE_RED, LV_STATE_DEFAULT);
                        } else {
                            strcpy(msg.cmsg.ssid, pdata->ssid);
                            strcpy(msg.cmsg.psk, string_value);
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_CONNECT_TO_WIFI;
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK_TWICE;
                        }
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

    return msg;
}


static void update_page(model_t *pmodel, struct page_data *pdata) {}


const pman_page_t page_wifi_psk = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = page_event,
};
