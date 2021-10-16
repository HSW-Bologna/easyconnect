#include <stdlib.h>
#include "src/lv_widgets/lv_btn.h"
#include "src/lv_widgets/lv_label.h"
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"

enum {
    BUTTON_BACK_ID,
    CONFIG_BTN_ID,
};

typedef enum {
    CONFIGURATION_STEP_SENDING,
    CONFIGURATION_STEP_WAITING,
    CONFIGURATION_STEP_DONE,
    CONFIGURATION_STEP_ERROR,
} configuration_step_t;

struct page_data {
    lv_obj_t *btn;
    lv_obj_t *status;
    lv_obj_t *spinner;
    lv_obj_t *btn_back;

    int dev_number;
    configuration_step_t step;
};


static void update(struct page_data *data) {
    switch (data->step) {
        case CONFIGURATION_STEP_WAITING:
            lv_obj_set_hidden(data->btn, 1);
            lv_btn_set_state(data->btn_back, LV_BTN_STATE_DISABLED);
            lv_obj_set_hidden(data->status, 0);
            lv_label_set_text(data->status, "Sto ascoltando");
            lv_obj_set_hidden(data->spinner, 0);
            break;

        case CONFIGURATION_STEP_SENDING:
            lv_obj_set_hidden(data->btn, 1);
            lv_btn_set_state(data->btn_back, LV_BTN_STATE_DISABLED);
            lv_obj_set_hidden(data->spinner, 0);
            lv_obj_set_hidden(data->status, 0);
            lv_label_set_text(data->status, "Configuro");
            break;

        case CONFIGURATION_STEP_DONE:
            lv_obj_set_hidden(data->btn, 0);
            lv_obj_set_hidden(data->spinner, 1);
            lv_btn_set_state(data->btn_back, LV_BTN_STATE_RELEASED);
            lv_obj_set_hidden(data->status, 0);
            lv_label_set_text_fmt(data->status, "Configurati %i  dispositivi!", data->dev_number);
            break;

        case CONFIGURATION_STEP_ERROR:
            lv_obj_set_hidden(data->btn, 0);
            lv_obj_set_hidden(data->spinner, 1);
            lv_obj_set_hidden(data->status, 0);
            lv_label_set_text_fmt(data->status, "Errore");
            break;
    }
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;

    data->btn_back = view_common_title(BUTTON_BACK_ID, "Configurazione automatica", NULL);

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(lbl, 400);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    data->status = lbl;

    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), NULL);
    lv_obj_align(spinner, NULL, LV_ALIGN_CENTER, 0, 0);
    data->spinner = spinner;

    lv_obj_t *btn = view_common_default_menu_button(lv_scr_act(), "Configura", CONFIG_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);
    data->btn = btn;

    lv_obj_set_hidden(data->status, 1);
    lv_obj_set_hidden(data->spinner,1);
    update(data);
}


static view_message_t process_page_event(model_t *model, void *arg, view_event_t event) {
    struct page_data *data = arg;
    (void)data;
    view_message_t msg = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_DEVICE_CONFIGURED:
            if (event.error) {
                data->step = CONFIGURATION_STEP_ERROR;
            } else {
                data->step = CONFIGURATION_STEP_DONE;
                data->dev_number = event.data.number;
            }
            update(data);
            break;

            case VIEW_EVENT_CODE_DEVICE_LISTENING_DONE:
            data->step = CONFIGURATION_STEP_SENDING;
            update(data);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BUTTON_BACK_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;

                        case CONFIG_BTN_ID: {
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_AUTOMATIC_COMMISSIONING;
                            data->step             = CONFIGURATION_STEP_WAITING;
                            update(data);
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


const pman_page_t page_automatic_device_config = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};