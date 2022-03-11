#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"
#include "utils/utils.h"


enum {
    BUTTON_BACK_ID,
    BTN_OK_ID,
    ROLLER_MIN_ID,
    ROLLER_HOUR_ID,
};


struct page_data {
    lv_obj_t *minute;
    lv_obj_t *hour;
};


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;
    (void)data;

    view_common_title(BUTTON_BACK_ID, "Ora", NULL);

    time_t    timer;
    struct tm tm_info;
    timer   = time(NULL);
    tm_info = *localtime(&timer);

    lv_obj_t *om = lv_objmask_create(lv_scr_act(), NULL);
    lv_obj_set_size(om, LV_HOR_RES_MAX, 220);
    lv_obj_align(om, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *rollers = view_common_min_sec_rollers(om, 5, &data->hour, &data->minute);
    lv_obj_align(rollers, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_roller_set_selected(data->hour, tm_info.tm_hour, LV_ANIM_OFF);
    view_register_default_callback(data->hour, ROLLER_HOUR_ID);
    lv_roller_set_selected(data->minute, tm_info.tm_min, LV_ANIM_OFF);
    view_register_default_callback(data->minute, ROLLER_MIN_ID);

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
                            tm_info.tm_min    = lv_roller_get_selected(data->minute);
                            tm_info.tm_hour   = lv_roller_get_selected(data->hour);
                            utils_set_system_time(tm_info);

                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SET_RTC;
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
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


const pman_page_t page_time = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};