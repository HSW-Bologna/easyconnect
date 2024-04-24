#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"
#include "view/intl/intl.h"


enum {
    BUTTON_BACK_ID,
    ON_BTN_ID,
    OFF_BTN_ID,
};

struct page_data {
    lv_obj_t *btns[2];
};


static void update_page(model_t *pmodel, struct page_data *pdata);


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *pdata = arg;
    lv_obj_t         *btn;

    view_common_title(BUTTON_BACK_ID, view_intl_get_string(pmodel, STRINGS_VOLUME), NULL);

    btn = view_common_default_menu_button(lv_scr_act(), "Beep ON", ON_BTN_ID);
    lv_btn_set_checkable(btn, 1);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_MID, 0, 100);
    lv_obj_t *btn_previous = btn;
    pdata->btns[0]         = btn;

    btn = view_common_default_menu_button(lv_scr_act(), "Beep OFF", OFF_BTN_ID);
    lv_obj_align(btn, btn_previous, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    pdata->btns[1] = btn;

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

                        case ON_BTN_ID:
                            model_set_buzzer_volume(pmodel, 1);
                            update_page(pmodel, pdata);
                            break;

                        case OFF_BTN_ID:
                            model_set_buzzer_volume(pmodel, 0);
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
    lv_obj_set_state(pdata->btns[0], pmodel->configuration.buzzer_volume ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
    lv_obj_set_state(pdata->btns[1], pmodel->configuration.buzzer_volume ? LV_STATE_DEFAULT : LV_STATE_CHECKED);
}


const pman_page_t page_buzzer = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};
