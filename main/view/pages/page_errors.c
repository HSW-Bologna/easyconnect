#include <stdlib.h>
#include "view/view.h"
#include "view/common.h"
#include "view/intl/intl.h"
#include "model/model.h"


enum {
    BUTTON_BACK_ID,
};


struct page_data {
    lv_obj_t *cont;
};


static void update_page(model_t *pmodel, struct page_data *pdata);


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *pmodel, void *arg) {
    struct page_data *pdata = arg;

    lv_obj_t *title = view_common_title(BUTTON_BACK_ID, view_intl_get_string(pmodel, STRINGS_ERRORI), NULL);

    lv_obj_t *cont = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont, LV_HOR_RES_MAX, LV_VER_RES_MAX - lv_obj_get_height(title) - 8);
    lv_obj_align(cont, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    pdata->cont = cont;

    update_page(pmodel, pdata);
}


static view_message_t process_page_event(model_t *pmodel, void *arg, view_event_t event) {
    struct page_data *pdata = arg;
    view_message_t    msg   = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_DEVICE_ALARM:
        case VIEW_EVENT_CODE_DEVICE_UPDATE:
            update_page(pmodel, pdata);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BUTTON_BACK_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_BACK;
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SAVE;
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


    lv_obj_clean(pdata->cont);

    size_t  i            = 0;
    uint8_t address      = 1;
    uint8_t prev_address = address;
    do {
        device_t device = model_get_device(pmodel, address);

        switch (device.status) {
            case DEVICE_STATUS_OK: {
                if (device.alarms > 0) {
                    lv_obj_t *lbl = lv_label_create(pdata->cont, NULL);
                    lv_label_set_align(lbl, LV_LABEL_ALIGN_LEFT);
                    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
                    lv_obj_set_width(lbl, LV_VER_RES_MAX - 16);

                    lv_label_set_text_fmt(lbl, "%s %i: 0x%X", view_intl_get_string(pmodel, STRINGS_DISPOSITIVO),
                                          device.address, device.alarms);
                }
                break;
            }
            case DEVICE_STATUS_COMMUNICATION_ERROR: {
                lv_obj_t *lbl = lv_label_create(pdata->cont, NULL);
                lv_label_set_align(lbl, LV_LABEL_ALIGN_LEFT);
                lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
                lv_obj_set_width(lbl, LV_VER_RES_MAX - 16);
                lv_label_set_text_fmt(lbl, "%s %i: %s", view_intl_get_string(pmodel, STRINGS_DISPOSITIVO),
                                      device.address, view_intl_get_string(pmodel, STRINGS_ERRORE_DI_COMUNICAZIONE));
                break;
            }
            default:
                break;
        }

        prev_address = address;
        address      = model_get_next_device_address(pmodel, prev_address);
    } while (address != prev_address);
}


const pman_page_t page_errors = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};