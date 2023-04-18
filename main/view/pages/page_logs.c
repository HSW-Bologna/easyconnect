#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "src/lv_core/lv_obj.h"
#include "src/lv_widgets/lv_label.h"
#include "view/intl/AUTOGEN_FILE_strings.h"
#include "view/view.h"
#include "view/common.h"
#include "view/intl/intl.h"
#include "model/model.h"
#include "view/view_types.h"


enum {
    BUTTON_BACK_ID,
    LEFT_BTN_ID,
    RIGHT_BTN_ID,
};


struct page_data {
    lv_obj_t *cont;
    lv_obj_t *title;

    uint32_t logs_from;
};


static void      update_page(model_t *pmodel, struct page_data *pdata);
static lv_obj_t *create_error_list(void);


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    data->logs_from        = 0;
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *pdata = arg;

    view_common_title(BUTTON_BACK_ID, view_intl_get_string(pmodel, STRINGS_LOG), &pdata->title);
    lv_obj_set_width(pdata->title, 280);
    lv_obj_align(pdata->title, NULL, LV_ALIGN_IN_TOP_MID, -32, 0);

    lv_obj_t *left = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(left, 40, 48);
    lv_obj_t *lbl = lv_label_create(left, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    lv_label_set_text(lbl, LV_SYMBOL_LEFT);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback(left, LEFT_BTN_ID);
    lv_obj_align(left, pdata->title, LV_ALIGN_OUT_LEFT_MID, -8, 0);

    lv_obj_t *right = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(right, 40, 48);
    lbl = lv_label_create(right, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    lv_label_set_text(lbl, LV_SYMBOL_RIGHT);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback(right, RIGHT_BTN_ID);
    lv_obj_align(right, pdata->title, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    lv_obj_t *cont = create_error_list();
    pdata->cont    = cont;

    update_page(pmodel, pdata);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *pdata = arg;
    view_message_t    msg   = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_OPEN:
            msg.cmsg.code      = VIEW_CONTROLLER_MESSAGE_CODE_READ_LOGS;
            msg.cmsg.logs_from = pdata->logs_from;
            break;

        case VIEW_EVENT_CODE_LOG_UPDATE:
            pdata->logs_from = pmodel->logs_from;
            update_page(pmodel, pdata);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BUTTON_BACK_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;

                        case LEFT_BTN_ID:
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_READ_LOGS;
                            if (pdata->logs_from > LOG_BUFFER_SIZE) {
                                msg.cmsg.logs_from = pdata->logs_from - LOG_BUFFER_SIZE;
                            } else {
                                msg.cmsg.logs_from = 0;
                            }
                            break;

                        case RIGHT_BTN_ID:
                            msg.cmsg.code      = VIEW_CONTROLLER_MESSAGE_CODE_READ_LOGS;
                            msg.cmsg.logs_from = pdata->logs_from + LOG_BUFFER_SIZE;
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
    lv_obj_del(pdata->cont);
    pdata->cont = create_error_list();

    for (size_t i = 0; i < pmodel->logs_num; i++) {
        log_t log = pmodel->logs[i];

        lv_obj_t *lbl = lv_label_create(pdata->cont, NULL);
        lv_label_set_align(lbl, LV_LABEL_ALIGN_LEFT);
        lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
        lv_obj_set_width(lbl, LV_VER_RES_MAX - 16);

        char      datetime[64] = {0};
        time_t    then         = (time_t)log.timestamp;
        struct tm time_info    = *localtime(&then);
        snprintf(datetime, sizeof(datetime), "%02i:%02i:%02i %02i/%02i/%02i", time_info.tm_hour, time_info.tm_min,
                 time_info.tm_sec, time_info.tm_mday, time_info.tm_mon + 1, time_info.tm_year % 100);

        switch (log.code) {
            case LOG_EVENT_POWER_ON:
                lv_label_set_text_fmt(lbl, "[%s] %s", datetime, view_intl_get_string(pmodel, STRINGS_ACCENSIONE));
                break;

            case LOG_EVENT_COMMUNICATION_ERROR:
                lv_label_set_text_fmt(lbl, "[%s] %s: %s %i", datetime,
                                      view_intl_get_string(pmodel, STRINGS_ERRORE_DI_COMUNICAZIONE),
                                      view_intl_get_string(pmodel, STRINGS_DISPOSITIVO), log.target_address);
                break;

            case LOG_EVENT_ALARM:
                lv_label_set_text_fmt(lbl, "[%s] %s %i: %s %i", datetime, view_intl_get_string(pmodel, STRINGS_ALLARME),
                                      log.code, view_intl_get_string(pmodel, STRINGS_DISPOSITIVO), log.target_address);
                break;
        }
    }

    lv_label_set_text_fmt(pdata->title, "[%i - %i]", pdata->logs_from + 1,
                          pdata->logs_from + pmodel->logs_num);
}


static lv_obj_t *create_error_list(void) {
    lv_obj_t *page = lv_page_create(lv_scr_act(), NULL);

    lv_obj_set_size(page, LV_HOR_RES_MAX, 270);
    lv_obj_align(page, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_page_set_scrl_layout(page, LV_LAYOUT_COLUMN_LEFT);

    return page;
}


const pman_page_t page_logs = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};
