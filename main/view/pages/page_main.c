#include <stdlib.h>
#include "src/lv_core/lv_obj.h"
#include "src/lv_core/lv_obj_style_dec.h"
#include "src/lv_misc/lv_anim.h"
#include "src/lv_misc/lv_area.h"
#include "src/lv_misc/lv_color.h"
#include "src/lv_misc/lv_types.h"
#include "src/lv_widgets/lv_btn.h"
#include "src/lv_widgets/lv_cont.h"
#include "src/lv_widgets/lv_slider.h"
#include "view/view.h"
#include "model/model.h"


LV_IMG_DECLARE(img_aspirazione_accesa);
LV_IMG_DECLARE(img_aspirazione_spenta);
LV_IMG_DECLARE(img_aspirazione_immissione_accese);
LV_IMG_DECLARE(img_aspirazione_immissione_spente);

LV_IMG_DECLARE(img_lampada_uvc_spenta);
LV_IMG_DECLARE(img_lampada_uvc_accesa);
LV_IMG_DECLARE(img_filtro_elettrostatico_spento);
LV_IMG_DECLARE(img_filtro_elettrostatico_acceso);
LV_IMG_DECLARE(img_filtro_lampada_accesi);
LV_IMG_DECLARE(img_filtro_lampada_spenti);

LV_IMG_DECLARE(img_luce_singola_accesa);
LV_IMG_DECLARE(img_luce_singola_spenta);
LV_IMG_DECLARE(img_due_luci_accese);
LV_IMG_DECLARE(img_due_luci_spente);
LV_IMG_DECLARE(img_tre_luci_accese);
LV_IMG_DECLARE(img_tre_luci_spente);


#define CENTER_X_DELTA 20
#define DRAWER_HEIGHT  340
#define PULLER_HEIGHT  40
#define DRAWER_RADIUS  32


enum {
    BUTTON_PLUS_ID,
    BUTTON_MINUS_ID,
    BTN_LIGHT_ID,
    BTN_FAN_ID,
    BTN_FILTER_ID,
    SLIDER_ID,
    SETTINGS_BTN_ID,
    TECH_SETTINGS_BTN_ID,
};


static lv_signal_cb_t ancestor_signal;

static const lv_img_dsc_t *images[][2][3] = {
    {
        {&img_luce_singola_spenta, &img_aspirazione_spenta, &img_lampada_uvc_spenta},
        {&img_luce_singola_accesa, &img_aspirazione_accesa, &img_lampada_uvc_accesa},
    },
    {
        {&img_due_luci_spente, &img_aspirazione_immissione_spente, &img_filtro_elettrostatico_spento},
        {&img_due_luci_accese, &img_aspirazione_immissione_accese, &img_filtro_elettrostatico_acceso},
    },
    {
        {&img_tre_luci_spente, &img_aspirazione_immissione_spente, &img_filtro_lampada_spenti},
        {&img_tre_luci_accese, &img_aspirazione_immissione_accese, &img_filtro_lampada_accesi},
    },
};



static lv_res_t limit_cb(lv_obj_t *obj, lv_signal_t signal, void *arg) {
    const int start  = (-DRAWER_RADIUS);
    const int end    = -(DRAWER_HEIGHT - PULLER_HEIGHT);
    const int midway = (end - start) / 2;

    if (signal == LV_SIGNAL_COORD_CHG) {
        if (lv_obj_get_y(obj) > start) {
            lv_obj_set_y(obj, start);
        } else if (lv_obj_get_y(obj) < end) {
            lv_obj_set_y(obj, end);
        }
    } else if (signal == LV_SIGNAL_DRAG_END) {
        if (lv_obj_get_y(obj) >= midway) {
            lv_obj_set_y(obj, start);
        } else {
            lv_obj_set_y(obj, end);
        }
    }
    return ancestor_signal(obj, signal, arg);
}


struct page_data {
    lv_obj_t *btn_light;
    lv_obj_t *btn_fan;
    lv_obj_t *btn_filter;
    lv_obj_t *lbl_temperature;
    lv_obj_t *slider;

    size_t demo_index;
};


static void update_info(model_t *pmodel, struct page_data *data) {
    lv_label_set_text_fmt(data->lbl_temperature, "%i C", model_get_temperature(pmodel));
}


static void update_images(struct page_data *data) {
    if (lv_btn_get_state(data->btn_light) == LV_BTN_STATE_CHECKED_RELEASED) {
        lv_img_set_src(lv_obj_get_child(data->btn_light, NULL), images[data->demo_index][1][0]);
    } else {
        lv_img_set_src(lv_obj_get_child(data->btn_light, NULL), images[data->demo_index][0][0]);
    }
    if (lv_btn_get_state(data->btn_fan) == LV_BTN_STATE_CHECKED_RELEASED) {
        lv_img_set_src(lv_obj_get_child(data->btn_fan, NULL), images[data->demo_index][1][1]);
    } else {
        lv_img_set_src(lv_obj_get_child(data->btn_fan, NULL), images[data->demo_index][0][1]);
    }
    if (lv_btn_get_state(data->btn_filter) == LV_BTN_STATE_CHECKED_RELEASED) {
        lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL), images[data->demo_index][1][2]);
    } else {
        lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL), images[data->demo_index][0][2]);
    }
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    data->demo_index       = 0;
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;

    lv_obj_t *title = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_digital_64);
    lv_label_set_long_mode(title, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(title, 240);
    lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title, "Easy Connect");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, CENTER_X_DELTA, 8);

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, CENTER_X_DELTA, 0);
    data->lbl_temperature = lbl;

    lv_obj_t *cont_drag = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont_drag, 250, DRAWER_HEIGHT);
    lv_obj_align(cont_drag, NULL, LV_ALIGN_OUT_TOP_MID, 0, PULLER_HEIGHT);
    lv_obj_set_style_local_border_width(cont_drag, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_bg_opa(cont_drag, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);

    lv_obj_t *cont = lv_cont_create(cont_drag, NULL);
    lv_obj_set_size(cont, 250, DRAWER_HEIGHT - PULLER_HEIGHT);
    lv_obj_align(cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_MID);
    lv_obj_set_style_local_pad_top(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 64);
    lv_obj_set_drag(cont_drag, true);
    lv_obj_set_drag_parent(cont, true);
    lv_obj_set_drag_dir(cont_drag, LV_DRAG_DIR_VER);
    ancestor_signal = lv_obj_get_signal_cb(cont_drag);
    lv_obj_set_signal_cb(cont_drag, limit_cb);
    lv_obj_set_style_local_radius(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, DRAWER_RADIUS);

    lv_obj_t *warnings = lv_btn_create(cont, NULL);
    lv_obj_set_size(warnings, 200, 60);
    lbl = lv_label_create(warnings, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_label_set_text(lbl, "Record errori");

    lv_obj_t *settings = lv_btn_create(cont, NULL);
    lv_obj_set_size(settings, 200, 60);
    lbl = lv_label_create(settings, lbl);
    lv_label_set_text(lbl, "Menu' utente");
    view_register_default_callback(settings, SETTINGS_BTN_ID);

    lv_obj_t *tech = lv_btn_create(cont, NULL);
    lv_obj_set_size(tech, 200, 60);
    lbl = lv_label_create(tech, lbl);
    lv_label_set_text(lbl, "Menu' assistenza");
    view_register_default_callback(tech, TECH_SETTINGS_BTN_ID);

    lv_obj_t *butn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *butn2 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *butn3 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(butn1, 100, 100);
    lv_obj_set_size(butn2, 100, 100);
    lv_obj_set_size(butn3, 100, 100);
    lv_obj_align(butn1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_align(butn2, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
    lv_obj_align(butn3, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_btn_set_checkable(butn1, true);
    lv_btn_set_checkable(butn2, true);
    lv_btn_set_checkable(butn3, true);

    lv_img_create(butn1, NULL);
    lv_img_create(butn2, NULL);
    lv_img_create(butn3, NULL);

    view_register_default_callback(butn1, BTN_LIGHT_ID);
    view_register_default_callback(butn2, BTN_FAN_ID);
    view_register_default_callback(butn3, BTN_FILTER_ID);

    data->btn_light  = butn1;
    data->btn_fan    = butn2;
    data->btn_filter = butn3;

    lv_obj_t *butn_plus  = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *butn_minus = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(butn_plus, 60, 60);
    lv_obj_set_size(butn_minus, 60, 60);
    lv_obj_align(butn_plus, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 0);
    lv_obj_align(butn_minus, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -20, 0);
    lv_obj_t *lbl_plus  = lv_label_create(butn_plus, NULL);
    lv_obj_t *lbl_minus = lv_label_create(butn_minus, NULL);
    lv_label_set_text(lbl_plus, LV_SYMBOL_PLUS);
    lv_label_set_text(lbl_minus, LV_SYMBOL_MINUS);
    view_register_default_callback(butn_minus, BUTTON_MINUS_ID);
    view_register_default_callback(butn_plus, BUTTON_PLUS_ID);

    lv_obj_t *sl = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_size(sl, 30, LV_VER_RES_MAX - lv_obj_get_height(butn_plus) * 2 - 50);
    lv_obj_align(sl, NULL, LV_ALIGN_IN_RIGHT_MID, -20 - (lv_obj_get_width(butn_plus) / 2) + lv_obj_get_width(sl) / 2,
                 0);
    lv_slider_set_range(sl, 0, 4);
    data->slider = sl;
    view_register_default_callback(data->slider, SLIDER_ID);

    update_info(model, data);
}


static view_message_t process_page_event(model_t *model, void *arg, view_event_t event) {
    struct page_data *data = arg;
    view_message_t    msg  = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_OPEN:
            update_images(data);
            break;

        case VIEW_EVENT_CODE_LVGL: {
            if (event.lv_event == LV_EVENT_VALUE_CHANGED) {
                update_images(data);
            }

            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BUTTON_PLUS_ID: {
                            if (model->slider < 4) {
                                model->slider++;
                                lv_slider_set_value(data->slider, model->slider, LV_ANIM_OFF);
                            }
                            break;
                        }
                        case BUTTON_MINUS_ID: {
                            if (model->slider > 0) {
                                model->slider--;
                                lv_slider_set_value(data->slider, model->slider, LV_ANIM_OFF);
                            }
                            break;
                        }

                        case SETTINGS_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_settings;
                            break;

                        case TECH_SETTINGS_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_tech_settings;
                            break;

                        default:
                            break;
                    }
                    break;

                    case LV_EVENT_VALUE_CHANGED: {
                        switch (event.data.id) {
                            case SLIDER_ID: {
                                model->slider = lv_slider_get_value(data->slider);
                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }
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


pman_page_t page_main = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};