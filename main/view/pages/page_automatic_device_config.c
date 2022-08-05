#include <stdlib.h>
#include "src/lv_widgets/lv_btn.h"
#include "src/lv_widgets/lv_label.h"
#include "view/view.h"
#include "view/common.h"
#include "model/model.h"
#include "view/intl/intl.h"


enum {
    BUTTON_BACK_ID,
    CONFIG_BTN_ID,
    BUTTON_PLUS_ID,
    BUTTON_MINUS_ID,
    RESTART_BTN_ID,
};

typedef enum {
    CONFIGURATION_STEP_SETUP,
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
    lv_obj_t *lbl_description;
    lv_obj_t *lbl_expected_devices;
    lv_obj_t *btn_minus;
    lv_obj_t *btn_plus;
    lv_obj_t *btn_repeat;

    uint16_t             dev_number;
    uint16_t             expected_devices;
    configuration_step_t step;
    uint8_t              first_time;
};


static void update_page(model_t *pmodel, struct page_data *pdata) {
    switch (pdata->step) {
        case CONFIGURATION_STEP_SETUP:
            lv_obj_set_hidden(pdata->btn, 0);
            lv_btn_set_state(pdata->btn_back, LV_BTN_STATE_RELEASED);
            lv_obj_set_hidden(pdata->status, 1);
            lv_obj_set_hidden(pdata->spinner, 1);
            lv_obj_set_hidden(pdata->btn_repeat, 1);
            lv_obj_set_hidden(pdata->btn_minus, 0);
            lv_obj_set_hidden(pdata->btn_plus, 0);
            lv_obj_set_hidden(pdata->lbl_description, 0);
            lv_label_set_text(pdata->lbl_description,
                              view_intl_get_string(pmodel, STRINGS_QUANTI_DISPOSITIVI_DEVONO_ESSERE_CONFIGURATI));
            lv_obj_set_hidden(pdata->lbl_expected_devices, 0);
            lv_label_set_text_fmt(pdata->lbl_expected_devices, "%i", pdata->expected_devices);
            break;

        case CONFIGURATION_STEP_WAITING:
            lv_obj_set_hidden(pdata->btn, 1);
            lv_btn_set_state(pdata->btn_back, LV_BTN_STATE_DISABLED);
            lv_obj_set_hidden(pdata->status, 0);
            lv_label_set_text(pdata->status, view_intl_get_string(pmodel, STRINGS_STO_ASCOLTANDO));
            lv_obj_set_hidden(pdata->spinner, 0);
            lv_obj_set_hidden(pdata->lbl_description, 1);
            lv_obj_set_hidden(pdata->lbl_expected_devices, 1);
            lv_obj_set_hidden(pdata->btn_repeat, 1);
            lv_obj_set_hidden(pdata->btn_minus, 1);
            lv_obj_set_hidden(pdata->btn_plus, 1);
            break;

        case CONFIGURATION_STEP_SENDING:
            lv_obj_set_hidden(pdata->btn, 1);
            lv_btn_set_state(pdata->btn_back, LV_BTN_STATE_DISABLED);
            lv_obj_set_hidden(pdata->spinner, 0);
            lv_obj_set_hidden(pdata->status, 0);
            lv_label_set_text(pdata->status, view_intl_get_string(pmodel, STRINGS_CONFIGURO));
            lv_obj_set_hidden(pdata->lbl_description, 1);
            lv_obj_set_hidden(pdata->lbl_expected_devices, 1);
            lv_obj_set_hidden(pdata->btn_repeat, 1);
            lv_obj_set_hidden(pdata->btn_minus, 1);
            lv_obj_set_hidden(pdata->btn_plus, 1);
            break;

        case CONFIGURATION_STEP_DONE:
            lv_obj_set_hidden(pdata->btn, 1);
            lv_obj_set_hidden(pdata->spinner, 1);
            lv_btn_set_state(pdata->btn_back, LV_BTN_STATE_RELEASED);
            lv_obj_set_hidden(pdata->status, pdata->first_time);
            lv_label_set_text_fmt(pdata->status, "%s: %i!",
                                  view_intl_get_string(pmodel, STRINGS_DISPOSITIVI_CONFIGURATI), pdata->dev_number);
            lv_obj_set_hidden(pdata->lbl_description, 0);
            lv_label_set_text(
                pdata->lbl_description,
                view_intl_get_string(
                    pmodel,
                    STRINGS_CONFIGURAZIONE_COMPLETATA_SE_NON_TUTTI_I_DISPOSITIVI_SONO_STATI_RILEVATI_RIPETERE_LA_PROCEDURA));
            lv_obj_set_hidden(pdata->lbl_expected_devices, 1);
            lv_obj_set_hidden(pdata->btn_repeat, 0);
            lv_obj_set_hidden(pdata->btn_minus, 1);
            lv_obj_set_hidden(pdata->btn_plus, 1);
            break;

        case CONFIGURATION_STEP_ERROR:
            lv_obj_set_hidden(pdata->btn, 1);
            lv_obj_set_hidden(pdata->spinner, 1);
            lv_obj_set_hidden(pdata->status, 0);
            lv_label_set_text_fmt(pdata->status, view_intl_get_string(pmodel, STRINGS_ERRORE));
            lv_obj_set_hidden(pdata->btn_repeat, 0);
            lv_obj_set_hidden(pdata->btn_minus, 1);
            lv_obj_set_hidden(pdata->btn_plus, 1);
            lv_obj_set_hidden(pdata->lbl_description, 1);
            lv_obj_set_hidden(pdata->lbl_expected_devices, 1);
            break;
    }
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *pdata = malloc(sizeof(struct page_data));
    return pdata;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *pdata = arg;
    lv_obj_t         *lbl;

    pdata->first_time       = 1;
    pdata->step             = CONFIGURATION_STEP_SETUP;
    pdata->dev_number       = 0;
    pdata->expected_devices = 5;

    pdata->btn_back =
        view_common_title(BUTTON_BACK_ID, view_intl_get_string(pmodel, STRINGS_CONFIGURAZIONE_AUTOMATICA), NULL);

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(lbl, LV_HOR_RES_MAX - 32);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_MID, 0, 80);
    pdata->lbl_description = lbl;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 32);
    lv_obj_set_style_local_text_font(lbl, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    pdata->lbl_expected_devices = lbl;

    lv_obj_t *butn_plus  = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *butn_minus = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(butn_plus, 60, 60);
    lv_obj_set_size(butn_minus, 60, 60);
    lv_obj_t *lbl_plus  = lv_label_create(butn_plus, NULL);
    lv_obj_t *lbl_minus = lv_label_create(butn_minus, NULL);
    lv_label_set_text(lbl_plus, LV_SYMBOL_PLUS);
    lv_label_set_text(lbl_minus, LV_SYMBOL_MINUS);
    view_register_default_callback(butn_minus, BUTTON_MINUS_ID);
    view_register_default_callback(butn_plus, BUTTON_PLUS_ID);
    lv_obj_align(butn_minus, lbl, LV_ALIGN_OUT_LEFT_MID, -32, 0);
    lv_obj_align(butn_plus, lbl, LV_ALIGN_OUT_RIGHT_MID, 32, 0);

    pdata->btn_minus = butn_minus;
    pdata->btn_plus  = butn_plus;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(lbl, 400);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -16);
    pdata->status = lbl;

    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), NULL);
    lv_obj_align(spinner, NULL, LV_ALIGN_CENTER, 0, 0);
    pdata->spinner = spinner;

    lv_obj_t *btn =
        view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_RIPROVA), RESTART_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 32);
    pdata->btn_repeat = btn;

    btn = view_common_default_menu_button(lv_scr_act(), view_intl_get_string(pmodel, STRINGS_CONFIGURA), CONFIG_BTN_ID);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 120);
    pdata->btn = btn;

    lv_obj_set_hidden(pdata->status, 1);
    lv_obj_set_hidden(pdata->spinner, 1);
    update_page(pmodel, pdata);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *pdata = arg;
    (void)pdata;
    view_message_t msg = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_DEVICE_CONFIGURED:
            if (event.error) {
                pdata->step = CONFIGURATION_STEP_ERROR;
            } else {
                pdata->step       = CONFIGURATION_STEP_DONE;
                pdata->dev_number = event.data.number;
            }
            update_page(pmodel, pdata);
            break;

        case VIEW_EVENT_CODE_DEVICE_LISTENING_DONE:
            pdata->step = CONFIGURATION_STEP_SENDING;
            update_page(pmodel, pdata);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_LONG_PRESSED_REPEAT:
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BUTTON_BACK_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            break;

                        case BUTTON_PLUS_ID:
                            pdata->expected_devices = (pdata->expected_devices + 1) % (MODBUS_MAX_DEVICES - 1);
                            if (pdata->expected_devices == 0) {
                                pdata->expected_devices++;
                            }
                            update_page(pmodel, pdata);
                            break;

                        case BUTTON_MINUS_ID:
                            if (pdata->expected_devices > 1) {
                                pdata->expected_devices--;
                            } else {
                                pdata->expected_devices = MODBUS_MAX_DEVICES - 1;
                            }
                            update_page(pmodel, pdata);
                            break;

                        case RESTART_BTN_ID:
                            pdata->step = CONFIGURATION_STEP_SETUP;
                            update_page(pmodel, pdata);
                            break;

                        case CONFIG_BTN_ID: {
                            msg.cmsg.code             = VIEW_CONTROLLER_MESSAGE_CODE_AUTOMATIC_COMMISSIONING;
                            msg.cmsg.expected_devices = pdata->expected_devices;
                            pdata->step               = CONFIGURATION_STEP_WAITING;
                            pdata->first_time         = 0;
                            update_page(pmodel, pdata);
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