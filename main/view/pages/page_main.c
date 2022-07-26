#include <time.h>
#include <stdlib.h>
#include "view/view.h"
#include "view/intl/intl.h"
#include "view/common.h"
#include "model/model.h"
#include "view/style.h"


LV_IMG_DECLARE(img_aspirazione_accesa);
LV_IMG_DECLARE(img_aspirazione_spenta);
LV_IMG_DECLARE(img_aspirazione_immissione_accese);
LV_IMG_DECLARE(img_aspirazione_immissione_spente);
LV_IMG_DECLARE(img_aspirazione_accesa_immissione_spenta);
LV_IMG_DECLARE(img_aspirazione_spenta_immissione_accesa);

LV_IMG_DECLARE(img_lampada_uvc_spenta);
LV_IMG_DECLARE(img_lampada_uvc_accesa);
LV_IMG_DECLARE(img_filtro_elettrostatico_spento);
LV_IMG_DECLARE(img_filtro_elettrostatico_acceso);
LV_IMG_DECLARE(img_filtro_lampada_accesi);
LV_IMG_DECLARE(img_filtro_lampada_spenti);
LV_IMG_DECLARE(img_filtro_spento_uvc_accesa);
LV_IMG_DECLARE(img_filtro_acceso_uvc_spenta);

LV_IMG_DECLARE(img_luce_singola_accesa);
LV_IMG_DECLARE(img_luce_singola_spenta);
LV_IMG_DECLARE(img_due_luci_accese);
LV_IMG_DECLARE(img_due_luci_gruppo_1_acceso);
LV_IMG_DECLARE(img_due_luci_gruppo_2_acceso);
LV_IMG_DECLARE(img_due_luci_spente);
LV_IMG_DECLARE(img_tre_luci_accese);
LV_IMG_DECLARE(img_tre_luci_gruppo_1_acceso);
LV_IMG_DECLARE(img_tre_luci_gruppo_1_2_accesi);
LV_IMG_DECLARE(img_tre_luci_spente);

LV_IMG_DECLARE(img_logo);


#define CENTER_X_DELTA 20
#define DRAWER_HEIGHT  280
#define PULLER_HEIGHT  64
#define DRAWER_RADIUS  32


enum {
    SINGLE_LIGHT_OFF = 0,
    SINGLE_LIGHT_ON,
};


enum {
    TWO_LIGHTS_OFF = 0,
    TWO_LIGHTS_GROUP_1_ON,
    TWO_LIGHTS_GROUP_2_ON,
    TWO_LIGHTS_ALL_GROUPS_ON,
};


enum {
    THREE_LIGHTS_OFF = 0,
    THREE_LIGHTS_GROUP_1_ON,
    THREE_LIGHTS_GROUPS_1_2_ON,
    THREE_LIGHTS_ALL_GROUPS_ON,
};


enum {
    FILTER_OFF = 0,
    FILTER_ON,
};

enum {
    TWO_FILTERS_OFF = 0,
    TWO_FILTERS_UVC_WATING,
    TWO_FILTERS_ON,
};

enum { FAN_OFF = 0, FAN_ON };

enum {
    FAN_BOTH_OFF = 0,
    FAN_BOTH_SIPHON_ON_IMMISSION_OFF,
    FAN_BOTH_SIPHON_OFF_IMMISSION_ON,
    FAN_BOTH_ON,
};


enum {
    BUTTON_PLUS_ID,
    BUTTON_MINUS_ID,
    BTN_LIGHT_ID,
    BTN_FAN_ID,
    BTN_FILTER_ID,
    SLIDER_ID,
    SETTINGS_BTN_ID,
    ERRORS_BTN_ID,
    TECH_SETTINGS_BTN_ID,
    TASK_BLINK_ID,
    BTN_ALARM_OK_ID,
};


static lv_res_t drawer_drag_cb(lv_obj_t *obj, lv_signal_t signal, void *arg) {
    const int start  = -DRAWER_HEIGHT;
    const int end    = -DRAWER_RADIUS;
    const int midway = (end - start) / 2;

    lv_obj_user_data_t data = lv_obj_get_user_data(obj);

    if (signal == LV_SIGNAL_COORD_CHG) {
        if (lv_obj_get_y(obj) < start) {
            lv_obj_set_y(obj, start);
        } else if (lv_obj_get_y(obj) > end) {
            lv_obj_set_y(obj, end);
        }
        lv_obj_align(data.sibling, obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    } else if (signal == LV_SIGNAL_DRAG_END) {
        if (lv_obj_get_y(obj) < midway) {
            lv_obj_set_y(obj, start);
        } else {
            lv_obj_set_y(obj, end);
        }
    }

    lv_signal_cb_t ancestor_signal = data.ancestor_signal;
    return ancestor_signal(obj, signal, arg);
}


static lv_res_t tab_drag_cb(lv_obj_t *obj, lv_signal_t signal, void *arg) {
    const int start  = 0;
    const int end    = DRAWER_HEIGHT - DRAWER_RADIUS;
    const int midway = (end - start) / 2;

    lv_obj_user_data_t data = lv_obj_get_user_data(obj);

    if (signal == LV_SIGNAL_COORD_CHG) {
        if (lv_obj_get_y(obj) < start) {
            lv_obj_set_y(obj, start);
        } else if (lv_obj_get_y(obj) > end) {
            lv_obj_set_y(obj, end);
        }
        lv_obj_align(data.sibling, obj, LV_ALIGN_OUT_TOP_MID, 0, 0);
    } else if (signal == LV_SIGNAL_DRAG_END) {
        if (lv_obj_get_y(obj) < midway) {
            lv_obj_set_y(obj, start);
        } else {
            lv_obj_set_y(obj, end);
        }
    }

    lv_signal_cb_t ancestor_signal = data.ancestor_signal;
    return ancestor_signal(obj, signal, arg);
}


struct page_data {
    lv_obj_t *btn_light;
    lv_obj_t *btn_fan;
    lv_obj_t *btn_filter;
    lv_obj_t *lbl_temperature;
    lv_obj_t *lbl_time;
    lv_obj_t *lbl_date;
    lv_obj_t *slider;
    lv_obj_t *alarm_popup;
    lv_obj_t *lbl_alarm_description;
    lv_obj_t *lbl_num_alarms;

    lv_task_t *blink_task;

    size_t demo_index;

    int light_state;
    int blink;

    int shown_alarm;
};


static void new_alarm(model_t *pmodel, struct page_data *pdata);
static void update_alarm_popup(model_t *pmodel, struct page_data *pdata);


static void update_filter_buttons(model_t *pmodel, struct page_data *data) {
    size_t esf_count = model_get_class_count(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER);
    size_t ulf_count = model_get_class_count(pmodel, DEVICE_CLASS_ULTRAVIOLET_FILTER);

    if (esf_count > 0 && ulf_count > 0) {
        if (model_get_electrostatic_filter_state(pmodel) && model_get_uvc_filter_state(pmodel)) {
            lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL), &img_filtro_lampada_accesi);
        } else if (model_get_electrostatic_filter_state(pmodel)) {
            lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL), &img_filtro_acceso_uvc_spenta);
        } else {
            lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL), &img_filtro_lampada_spenti);
        }
    } else if (esf_count > 0) {
        lv_obj_set_hidden(data->btn_filter, 0);
        lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL), model_get_electrostatic_filter_state(pmodel)
                                                                     ? &img_filtro_elettrostatico_acceso
                                                                     : &img_filtro_elettrostatico_spento);
    } else if (ulf_count > 0) {
        lv_obj_set_hidden(data->btn_filter, 0);
        lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL),
                       model_get_uvc_filter_state(pmodel) ? &img_lampada_uvc_accesa : &img_lampada_uvc_spenta);
    } else {
        lv_obj_set_hidden(data->btn_filter, 1);
    }
}


static void update_fan_buttons(model_t *pmodel, struct page_data *data) {
    size_t sfm_count = model_get_class_count(pmodel, DEVICE_CLASS_SIPHONING_FAN);
    size_t ifm_count = model_get_class_count(pmodel, DEVICE_CLASS_IMMISSION_FAN);

    if (sfm_count > 0 && ifm_count > 0) {
        lv_obj_set_hidden(data->btn_fan, 0);
        switch (model_get_fan_state(pmodel)) {
            case MODEL_FAN_STATE_SF_ENV_CLEANING:
                lv_img_set_src(lv_obj_get_child(data->btn_fan, NULL), data->blink
                                                                          ? &img_aspirazione_accesa_immissione_spenta
                                                                          : &img_aspirazione_immissione_spente);
                break;

            case MODEL_FAN_STATE_SF_IF_ENV_CLEANING:
                lv_img_set_src(lv_obj_get_child(data->btn_fan, NULL), data->blink
                                                                          ? &img_aspirazione_accesa_immissione_spenta
                                                                          : &img_aspirazione_immissione_accese);
                break;

            case MODEL_FAN_STATE_IF_ENV_CLEANING:
                lv_img_set_src(lv_obj_get_child(data->btn_fan, NULL), data->blink
                                                                          ? &img_aspirazione_spenta_immissione_accesa
                                                                          : &img_aspirazione_immissione_spente);
                break;

            case MODEL_FAN_STATE_FAN_RUNNING:
                lv_img_set_src(lv_obj_get_child(data->btn_fan, NULL), &img_aspirazione_immissione_accese);
                break;

            case MODEL_FAN_STATE_OFF:
                lv_img_set_src(lv_obj_get_child(data->btn_fan, NULL), &img_aspirazione_immissione_spente);
                break;
        }
    } else if (sfm_count > 0) {
        lv_obj_set_hidden(data->btn_fan, 0);
        switch (model_get_fan_state(pmodel)) {
            case MODEL_FAN_STATE_SF_ENV_CLEANING:
                lv_img_set_src(lv_obj_get_child(data->btn_fan, NULL),
                               data->blink ? &img_aspirazione_accesa : &img_aspirazione_spenta);
                break;

            case MODEL_FAN_STATE_FAN_RUNNING:
                lv_img_set_src(lv_obj_get_child(data->btn_fan, NULL), &img_aspirazione_accesa);
                break;

            case MODEL_FAN_STATE_OFF:
                lv_img_set_src(lv_obj_get_child(data->btn_fan, NULL), &img_aspirazione_spenta);
                break;

            case MODEL_FAN_STATE_SF_IF_ENV_CLEANING:
            case MODEL_FAN_STATE_IF_ENV_CLEANING:
                assert(0);
                break;
        }
    } else {
        lv_obj_set_hidden(data->btn_fan, 1);
    }
}


static void update_all_buttons(model_t *pmodel, struct page_data *data) {
    if (data->light_state > 0) {
        lv_btn_set_state(data->btn_light, LV_BTN_STATE_CHECKED_RELEASED);
    } else {
        lv_btn_set_state(data->btn_light, LV_BTN_STATE_RELEASED);
    }

    uint16_t class;
    if (model_get_light_class(pmodel, &class)) {
        lv_obj_set_hidden(data->btn_light, 0);
        switch (class) {
            case DEVICE_CLASS_LIGHT_1:
                lv_img_set_src(lv_obj_get_child(data->btn_light, NULL), data->light_state == SINGLE_LIGHT_ON
                                                                            ? &img_luce_singola_accesa
                                                                            : &img_luce_singola_spenta);
                break;

            case DEVICE_CLASS_LIGHT_2: {
                assert(data->light_state <= TWO_LIGHTS_ALL_GROUPS_ON);
                const lv_img_dsc_t *images[] = {
                    &img_due_luci_spente,
                    &img_due_luci_gruppo_1_acceso,
                    &img_due_luci_gruppo_2_acceso,
                    &img_due_luci_accese,
                };
                lv_img_set_src(lv_obj_get_child(data->btn_light, NULL), images[data->light_state]);
                break;
            }

            case DEVICE_CLASS_LIGHT_3:
                assert(data->light_state <= THREE_LIGHTS_ALL_GROUPS_ON);
                const lv_img_dsc_t *images[] = {
                    &img_tre_luci_spente,
                    &img_tre_luci_gruppo_1_acceso,
                    &img_tre_luci_gruppo_1_2_accesi,
                    &img_tre_luci_accese,
                };
                lv_img_set_src(lv_obj_get_child(data->btn_light, NULL), images[data->light_state]);
                break;

            default:
                assert(0);
        }
    } else {
        lv_obj_set_hidden(data->btn_light, 1);
    }

    update_filter_buttons(pmodel, data);

    update_fan_buttons(pmodel, data);
}


static void update_info(model_t *pmodel, struct page_data *data) {
    lv_label_set_text_fmt(data->lbl_temperature, "%i %s", model_get_temperature(pmodel),
                          model_get_degrees_symbol(pmodel));

    char   string[32] = {0};
    time_t rawtime;
    time(&rawtime);
    struct tm *now = localtime(&rawtime);

    strftime(string, sizeof(string), "%H:%M", now);
    lv_label_set_text(data->lbl_time, string);

    memset(string, 0, sizeof(string));
    strftime(string, sizeof(string), "%d/%m/%Y", now);
    lv_label_set_text(data->lbl_date, string);
}


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    data->blink_task       = view_register_periodic_task(500UL, LV_TASK_PRIO_OFF, TASK_BLINK_ID);
    data->demo_index       = 0;
    data->light_state      = 0;
    data->blink            = 0;
    alarms_queue_init(&model->alarms);
    data->shown_alarm = -1;
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;
    lv_task_set_prio(data->blink_task, LV_TASK_PRIO_MID);

    /*lv_obj_t *title = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_digital_64);
    lv_label_set_long_mode(title, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(title, 240);
    lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(title, "Easy Connect");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, CENTER_X_DELTA, 8);*/
    lv_obj_t *logo = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(logo, &img_logo);
    lv_obj_align(logo, NULL, LV_ALIGN_IN_TOP_MID, CENTER_X_DELTA, 8);

    lv_obj_t *lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, CENTER_X_DELTA, 0);
    data->lbl_temperature = lbl;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, data->lbl_temperature, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    data->lbl_time = lbl;

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, data->lbl_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    data->lbl_date = lbl;

    lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_add_style(cont, LV_CONT_PART_MAIN, &style_transparent_cont);
    lv_cont_set_layout(cont, LV_LAYOUT_CENTER);
    lv_obj_set_size(cont, 92, 312);
    lv_obj_align(cont, NULL, LV_ALIGN_IN_LEFT_MID, 4, 0);

    lv_obj_t *butn1 = lv_btn_create(cont, NULL);
    lv_obj_t *butn2 = lv_btn_create(cont, NULL);
    lv_obj_t *butn3 = lv_btn_create(cont, NULL);
    lv_obj_set_size(butn1, 92, 92);
    lv_obj_set_size(butn2, 92, 92);
    lv_obj_set_size(butn3, 92, 92);

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
    lv_obj_align(butn_plus, NULL, LV_ALIGN_IN_TOP_RIGHT, -16, 4);
    lv_obj_align(butn_minus, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -16, -4);
    lv_obj_t *lbl_plus  = lv_label_create(butn_plus, NULL);
    lv_obj_t *lbl_minus = lv_label_create(butn_minus, NULL);
    lv_label_set_text(lbl_plus, LV_SYMBOL_PLUS);
    lv_label_set_text(lbl_minus, LV_SYMBOL_MINUS);
    view_register_default_callback(butn_minus, BUTTON_MINUS_ID);
    view_register_default_callback(butn_plus, BUTTON_PLUS_ID);

    lv_obj_t *sl = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_size(sl, 30, LV_VER_RES_MAX - lv_obj_get_height(butn_plus) * 2 - 58);
    lv_obj_align(sl, NULL, LV_ALIGN_IN_RIGHT_MID, -16 - (lv_obj_get_width(butn_plus) / 2) + lv_obj_get_width(sl) / 2,
                 0);
    lv_slider_set_range(sl, 0, 4);
    data->slider = sl;
    view_register_default_callback(data->slider, SLIDER_ID);

    lv_obj_t *alarm_popup = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(alarm_popup, 320, 240);
    lv_obj_align(alarm_popup, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_cont_set_layout(alarm_popup, LV_LAYOUT_OFF);
    lv_obj_set_style_local_radius(alarm_popup, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, DRAWER_RADIUS);

    lbl = lv_label_create(alarm_popup, NULL);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_BREAK);
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(lbl, 300);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, -32);
    data->lbl_alarm_description = lbl;

    lbl = lv_label_create(alarm_popup, NULL);
    lv_obj_set_auto_realign(lbl, 1);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -16, -16);
    data->lbl_num_alarms = lbl;

    lv_obj_t *btn = lv_btn_create(alarm_popup, NULL);
    lbl           = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, LV_SYMBOL_CLOSE);
    lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -16);
    view_register_default_callback(btn, BTN_ALARM_OK_ID);

    data->alarm_popup = alarm_popup;

    lv_obj_t *drawer = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(drawer, 400, DRAWER_HEIGHT);
    lv_obj_align(drawer, NULL, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_cont_set_layout(drawer, LV_LAYOUT_COLUMN_MID);
    lv_obj_set_style_local_pad_top(drawer, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 64);
    lv_obj_set_drag(drawer, true);
    lv_obj_set_drag_dir(drawer, LV_DRAG_DIR_VER);
    lv_obj_set_style_local_radius(drawer, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, DRAWER_RADIUS);

    lv_obj_t *cont_drag = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont_drag, 250, PULLER_HEIGHT);
    lv_obj_set_auto_realign(cont_drag, 1);
    lv_obj_align(cont_drag, drawer, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_add_style(cont_drag, LV_CONT_PART_MAIN, &style_transparent_cont);
    lv_obj_set_drag(cont_drag, true);
    lv_obj_set_drag_dir(cont_drag, LV_DRAG_DIR_VER);

    view_blood_pact(drawer, cont_drag);
    view_new_signal_handler(cont_drag, tab_drag_cb);
    view_new_signal_handler(drawer, drawer_drag_cb);

    lv_obj_t *warnings = view_common_menu_button(drawer, "Record errori", 300, ERRORS_BTN_ID);
    lv_obj_set_drag_parent(warnings, true);

    lv_obj_t *settings = view_common_menu_button(drawer, "Menu' utente", 300, SETTINGS_BTN_ID);
    lv_obj_set_drag_parent(settings, true);

    lv_obj_t *tech = view_common_menu_button(drawer, "Menu' assistenza", 300, TECH_SETTINGS_BTN_ID);
    lv_obj_set_drag_parent(tech, true);

    update_info(model, data);
    update_all_buttons(model, data);
    lv_slider_set_value(data->slider, model_get_fan_speed(model), LV_ANIM_OFF);
    update_alarm_popup(model, data);
}


static view_message_t process_page_event(model_t *model, void *arg, view_event_t event) {
    struct page_data *data = arg;
    view_message_t    msg  = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_OPEN:
        case VIEW_EVENT_CODE_STATE_UPDATE:
            update_all_buttons(model, data);
            break;

        case VIEW_EVENT_CODE_ANCILLARY_DATA_UPDATE:
            update_info(model, data);
            break;

        case VIEW_EVENT_CODE_DEVICE_UPDATE:
        case VIEW_EVENT_CODE_DEVICE_ALARM:
            if (data->shown_alarm < 0) {
                new_alarm(model, data);
            }
            update_alarm_popup(model, data);
            break;

        case VIEW_EVENT_CODE_TIMER:
            switch (event.timer_id) {
                case TASK_BLINK_ID:
                    data->blink = !data->blink;
                    update_fan_buttons(model, data);
                    update_info(model, data);
                    break;
            }
            break;

        case VIEW_EVENT_CODE_LVGL: {
            switch (event.lv_event) {
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BTN_ALARM_OK_ID:
                            data->shown_alarm = -1;
                            new_alarm(model, data);
                            update_alarm_popup(model, data);
                            break;

                        case BUTTON_PLUS_ID: {
                            uint8_t speed = model_get_fan_speed(model);

                            if (speed < MAX_FAN_SPEED) {
                                speed++;
                                model_set_fan_speed(model, speed);
                                lv_slider_set_value(data->slider, model_get_fan_speed(model), LV_ANIM_OFF);
                                msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SET_FAN_SPEED;
                            }
                            break;
                        }

                        case BUTTON_MINUS_ID: {
                            uint8_t speed = model_get_fan_speed(model);

                            if (speed > 0) {
                                speed--;
                                model_set_fan_speed(model, speed);
                                lv_slider_set_value(data->slider, model_get_fan_speed(model), LV_ANIM_OFF);
                                msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SET_FAN_SPEED;
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

                        case BTN_LIGHT_ID: {
                            uint16_t class;
                            if (!model_get_light_class(model, &class)) {
                                break;
                            }

                            switch (class) {
                                case DEVICE_CLASS_LIGHT_1:
                                    msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_LIGHTS;
                                    if (data->light_state == SINGLE_LIGHT_OFF) {
                                        data->light_state    = SINGLE_LIGHT_ON;
                                        msg.cmsg.light_value = 1;
                                    } else {
                                        data->light_state    = SINGLE_LIGHT_OFF;
                                        msg.cmsg.light_value = 0;
                                    }
                                    break;

                                case DEVICE_CLASS_LIGHT_2: {
                                    msg.cmsg.code     = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_LIGHTS;
                                    data->light_state = (data->light_state + 1) % (TWO_LIGHTS_ALL_GROUPS_ON + 1);

                                    switch (data->light_state) {
                                        case TWO_LIGHTS_OFF:
                                            msg.cmsg.light_value = 0;
                                            break;

                                        case TWO_LIGHTS_GROUP_1_ON:
                                            msg.cmsg.light_value = 0x01;
                                            break;


                                        case TWO_LIGHTS_GROUP_2_ON:
                                            msg.cmsg.light_value = 0x02;
                                            break;

                                        case TWO_LIGHTS_ALL_GROUPS_ON:
                                            msg.cmsg.light_value = 0x03;
                                            break;
                                    }
                                    break;
                                }

                                case DEVICE_CLASS_LIGHT_3: {
                                    msg.cmsg.code     = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_LIGHTS;
                                    data->light_state = (data->light_state + 1) % (THREE_LIGHTS_ALL_GROUPS_ON + 1);

                                    switch (data->light_state) {
                                        case THREE_LIGHTS_OFF:
                                            msg.cmsg.light_value = 0;
                                            break;

                                        case THREE_LIGHTS_GROUP_1_ON:
                                            msg.cmsg.light_value = 0x01;
                                            break;

                                        case THREE_LIGHTS_GROUPS_1_2_ON:
                                            msg.cmsg.light_value = 0x03;
                                            break;

                                        case THREE_LIGHTS_ALL_GROUPS_ON:
                                            msg.cmsg.light_value = 0x07;
                                            break;
                                    }
                                    break;
                                }

                                default:
                                    assert(0);
                            }

                            update_all_buttons(model, data);
                            break;
                        }

                        case BTN_FAN_ID:
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FAN;
                            break;

                        case BTN_FILTER_ID:
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FILTER;
                            break;

                        default:
                            break;
                    }
                    break;

                    case LV_EVENT_VALUE_CHANGED: {
                        switch (event.data.id) {
                            case SLIDER_ID: {
                                model_set_fan_speed(model, lv_slider_get_value(data->slider));
                                break;
                            }

                            default:
                                break;
                        }
                        break;
                    }

                    case LV_EVENT_RELEASED: {
                        switch (event.data.id) {
                            case SLIDER_ID: {
                                msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_SET_FAN_SPEED;
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


static void destroy_page(void *args, void *extra) {
    struct page_data *data = args;
    lv_task_del(data->blink_task);
    free(data);
    free(extra);
}

void close_page(void *args) {
    struct page_data *data = args;
    lv_task_set_prio(data->blink_task, LV_TASK_PRIO_OFF);
    lv_obj_clean(lv_scr_act());
}


static void new_alarm(model_t *pmodel, struct page_data *pdata) {
    uint8_t address = 0;
    if (alarms_queue_dequeue(&pmodel->alarms, &address) == DEQUEUE_RESULT_SUCCESS) {
        pdata->shown_alarm = address;
        device_t device    = {0};
        model_get_device(pmodel, &device, address);

        char string[64] = {0};
        view_common_get_class_string(device.class, string, sizeof(string));

        if (device.status == DEVICE_STATUS_COMMUNICATION_ERROR) {
            lv_label_set_text_fmt(pdata->lbl_alarm_description, "%s: %s\n\t%s %i\n\t%s 0x%X\n\t%s %s",
                                  view_intl_get_string(pmodel, STRINGS_PROBLEMA_CON_IL_DISPOSITIVO),
                                  view_intl_get_string(pmodel, STRINGS_ERRORE_DI_COMUNICAZIONE),
                                  view_intl_get_string(pmodel, STRINGS_INDIRIZZO), device.address,
                                  view_intl_get_string(pmodel, STRINGS_NUMERO_DI_SERIE), device.serial_number,
                                  view_intl_get_string(pmodel, STRINGS_CLASSE), string);
        } else if (device.alarms) {
            lv_label_set_text_fmt(pdata->lbl_alarm_description, "%s: %s\n\t%s %i\n\t%s 0x%X\n\t%s %s",
                                  view_intl_get_string(pmodel, STRINGS_PROBLEMA_CON_IL_DISPOSITIVO),
                                  view_intl_get_string(pmodel, STRINGS_ALLARME_DI_SICUREZZA),
                                  view_intl_get_string(pmodel, STRINGS_INDIRIZZO), device.address,
                                  view_intl_get_string(pmodel, STRINGS_NUMERO_DI_SERIE), device.serial_number,
                                  view_intl_get_string(pmodel, STRINGS_CLASSE), string);
        }
    }
}


static void update_alarm_popup(model_t *pmodel, struct page_data *pdata) {
    view_common_set_hidden(pdata->alarm_popup, pdata->shown_alarm < 0);
    lv_label_set_text_fmt(pdata->lbl_num_alarms, "n %2zu", alarms_queue_count(&pmodel->alarms) + 1);
}


const pman_page_t page_main = {
    .create        = create_page,
    .destroy       = destroy_page,
    .open          = open_page,
    .close         = close_page,
    .process_event = process_page_event,
};