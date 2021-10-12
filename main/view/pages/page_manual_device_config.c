#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"


enum {
    BUTTON_BACK_ID,
    ADDRESS_DD_ID,
    CONFIG_BTN_ID,
};


typedef enum {
    CONFIGURATION_STEP_CHOOSE_ADDRESS,
    CONFIGURATION_STEP_WAIT,
    CONFIGURATION_STEP_SUCCESS,
    CONFIGURATION_STEP_ERROR,
} configuration_step_t;


struct page_data {
    lv_obj_t *dd;
    lv_obj_t *btn;
    lv_obj_t *lbl_dd;
    lv_obj_t *status;
    lv_obj_t *spinner;

    uint8_t              address;
    configuration_step_t step;
};


static void update(struct page_data *data) {
    switch (data->step) {
        case CONFIGURATION_STEP_CHOOSE_ADDRESS:
            lv_obj_set_hidden(data->btn, 0);
            lv_obj_set_hidden(data->status, 0);
            lv_label_set_text(
                data->status,
                "Collegare un unico dispositivo alla rete REC e selezionare un indirizzo da assegnare. Ogni "
                "dispositivo deve avere un indirizzo univoco sulla rete.");
            lv_obj_set_hidden(data->spinner, 1);
            lv_obj_set_hidden(data->lbl_dd, 0);
            lv_obj_set_hidden(data->dd, 0);
            break;

        case CONFIGURATION_STEP_WAIT:
            lv_obj_set_hidden(data->btn, 1);
            lv_obj_set_hidden(data->spinner, 0);
            lv_obj_set_hidden(data->status, 0);
            lv_obj_set_hidden(data->lbl_dd, 1);
            lv_obj_set_hidden(data->dd, 1);
            lv_label_set_text(data->status, "Attendere...");
            break;

        case CONFIGURATION_STEP_SUCCESS:
            lv_obj_set_hidden(data->btn, 0);
            lv_obj_set_hidden(data->spinner, 1);
            lv_obj_set_hidden(data->status, 0);
            lv_obj_set_hidden(data->lbl_dd, 0);
            lv_obj_set_hidden(data->dd, 0);
            lv_label_set_text_fmt(data->status, "Dispositivo %i configurato con successo!", data->address);
            break;

        case CONFIGURATION_STEP_ERROR:
            lv_obj_set_hidden(data->btn, 0);
            lv_obj_set_hidden(data->spinner, 1);
            lv_obj_set_hidden(data->status, 0);
            lv_obj_set_hidden(data->lbl_dd, 0);
            lv_obj_set_hidden(data->dd, 0);
            lv_label_set_text_fmt(data->status, "Dispositivo %i non trovato!", data->address);
            break;
    }
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    data->step             = CONFIGURATION_STEP_CHOOSE_ADDRESS;
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;

    view_common_title(BUTTON_BACK_ID, "Configurazione manuale", NULL);

    lv_obj_t *dd = view_common_address_picker(lv_scr_act(), ADDRESS_DD_ID);
    data->dd     = dd;

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_text(lbl, "Nuovo indirizzo:");
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 16, 80);
    lv_obj_align(dd, lbl, LV_ALIGN_OUT_RIGHT_MID, 16, 0);
    data->lbl_dd = lbl;


    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(lbl, 400);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -8);
    data->status = lbl;

    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), NULL);
    lv_obj_align(spinner, NULL, LV_ALIGN_CENTER, 0, 0);
    data->spinner = spinner;

    lv_obj_t *btn = view_common_menu_button(lv_scr_act(), "Configura", CONFIG_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);
    data->btn = btn;

    lv_obj_set_hidden(data->status, 1);
    update(data);
}


static view_message_t process_page_event(model_t *model, void *arg, view_event_t event) {
    struct page_data *data = arg;
    (void)data;
    view_message_t msg = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_DEVICE_CONFIGURED:
            data->address = event.address;
            if (event.error) {
                data->step = CONFIGURATION_STEP_ERROR;
            } else {
                data->step = CONFIGURATION_STEP_SUCCESS;
            }
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
                            uint8_t address  = lv_dropdown_get_selected(data->dd) + 1;
                            msg.cmsg.code    = VIEW_CONTROLLER_MESSAGE_CODE_CONFIGURE_DEVICE_ADDRESS;
                            msg.cmsg.address = address;

                            data->step = CONFIGURATION_STEP_WAIT;
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


const pman_page_t page_manual_device_config = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};