#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "view/view.h"
#include "view/intl/intl.h"
#include "view/common.h"
#include "model/model.h"
#include "view/style.h"
#include "esp_log.h"
#include "config/app_config.h"


LV_IMG_DECLARE(img_icon_reset_md);
LV_IMG_DECLARE(img_icon_reset_warning_md);
LV_IMG_DECLARE(img_icon_reset_stop_md);

LV_IMG_DECLARE(img_error_sm);
LV_IMG_DECLARE(img_error_md);

LV_IMG_DECLARE(img_stop);

LV_IMG_DECLARE(img_icona_luce_1);
LV_IMG_DECLARE(img_icona_luce_2);
LV_IMG_DECLARE(img_icona_luce_3);
LV_IMG_DECLARE(img_icona_elettrostatico);
LV_IMG_DECLARE(img_icona_uvc);
LV_IMG_DECLARE(img_icona_aspirazione);
LV_IMG_DECLARE(img_icona_immissione);

LV_IMG_DECLARE(img_icon_gas_sm);

LV_IMG_DECLARE(img_icon_pressure_temperature_sm);

LV_IMG_DECLARE(img_icon_temperature_humidity_sm);

LV_IMG_DECLARE(img_icon_pressure_temperature_humidity_sm);

LV_IMG_DECLARE(img_icon_immission_sm);
LV_IMG_DECLARE(img_icon_immission_md);

LV_IMG_DECLARE(img_icon_siphoning_md);

LV_IMG_DECLARE(img_icon_siphoning_immission_md);

LV_IMG_DECLARE(img_icon_uvc_md);
LV_IMG_DECLARE(img_icon_esf_md);
LV_IMG_DECLARE(img_icon_helmet_md);
LV_IMG_DECLARE(img_icon_passive_filter_md);
LV_IMG_DECLARE(img_start_cleaning);
LV_IMG_DECLARE(img_finish_cleaning);
LV_IMG_DECLARE(img_speed_1);
LV_IMG_DECLARE(img_speed_2);
LV_IMG_DECLARE(img_bar);
LV_IMG_DECLARE(img_pipe);
LV_IMG_DECLARE(img_stop);
LV_IMG_DECLARE(img_warn);
LV_IMG_DECLARE(img_num);
LV_IMG_DECLARE(img_calibration);

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
    BTN_BACK_ID,
    SLIDER_ID,
    SETTINGS_BTN_ID,
    ERRORS_BTN_ID,
    TECH_SETTINGS_BTN_ID,
    TASK_BLINK_ID,
    BTN_REC_SYSTEM_ID,
    BTN_VENTILATION_ID,
    BTN_FILTERING_ID,
    BTN_SENSORS_ID,
    BTN_REC_DEVICE_ID,
    BTN_AUTOCONF_ID,
    BTN_SEARCH_ID,
    BTN_MAINTENANCE_ID,
    BTN_ACTIVE_FILTERING_ID,
    BTN_SETUP_MAINTENANCE_FILTERING_ID,
    BTN_PRESSURE_ID,
    BTN_HUMIDITY_ID,
    BTN_TEMPERATURE_ID,

    // Ventilation popup
    BTN_SIPHONING_PERCENTAGE_0_MOD,
    BTN_SIPHONING_PERCENTAGE_1_MOD,
    BTN_SIPHONING_PERCENTAGE_2_MOD,
    BTN_SIPHONING_PERCENTAGE_3_MOD,
    BTN_SIPHONING_PERCENTAGE_4_MOD,
    BTN_IMMISSION_PERCENTAGE_0_MOD,
    BTN_IMMISSION_PERCENTAGE_1_MOD,
    BTN_IMMISSION_PERCENTAGE_2_MOD,
    BTN_IMMISSION_PERCENTAGE_3_MOD,
    BTN_IMMISSION_PERCENTAGE_4_MOD,
    BTN_CLEANING_START_MOD,
    BTN_CLEANING_FINISH_MOD,

    // Active filtering popup
    BTN_UVC_FILTER_0_MOD,
    BTN_UVC_FILTER_1_MOD,
    BTN_UVC_FILTER_2_MOD,
    BTN_UVC_FILTER_3_MOD,
    BTN_UVC_FILTER_4_MOD,
    BTN_ESF_FILTER_0_MOD,
    BTN_ESF_FILTER_1_MOD,
    BTN_ESF_FILTER_2_MOD,
    BTN_ESF_FILTER_3_MOD,
    BTN_ESF_FILTER_4_MOD,
    BTN_PRESSURE_THRESHOLD_MOD,

    // Maintenance popup
    BTN_RESET_PASSIVE_FILTERS_HOURS,
    BTN_RESET_FILTER_HOURS,

    // Filters popup
    BTN_UVC_FILTERS_HOURS_THRESHOLD_WARN_MOD,
    BTN_UVC_FILTERS_HOURS_THRESHOLD_STOP_MOD,
    BTN_ESF_FILTERS_HOURS_THRESHOLD_WARN_MOD,
    BTN_ESF_FILTERS_HOURS_THRESHOLD_STOP_MOD,
    BTN_PASSIVE_FILTERS_HOURS_THRESHOLD_WARN_MOD,
    BTN_PASSIVE_FILTERS_HOURS_THRESHOLD_STOP_MOD,
    BTN_PASSIVE_FILTERS_NUM_MOD,

    // Rec system group
    BTN_DEVICE_ID,

    // Pressure group
    BTN_PRESSURE_DIFFERENCE_0_MOD,
    BTN_PRESSURE_DIFFERENCE_1_MOD,
    BTN_PRESSURE_DIFFERENCE_2_MOD,
    BTN_PRESSURE_DIFFERENCE_3_MOD,
    BTN_PRESSURE_DIFFERENCE_4_MOD,
    BTN_PRESSURE_WARN_MOD,
    BTN_PRESSURE_STOP_MOD,
    BTN_START_CALIBRATION,

    // Humidity/temperature group
    BTN_HUMTEMP_FIRST_DELTA_MOD,
    BTN_HUMTEMP_SECOND_DELTA_MOD,
    BTN_HUMTEMP_FIRST_CORRECTION_MOD,
    BTN_HUMTEMP_SECOND_CORRECTION_MOD,
    BTN_HUMTEMP_WARN_MOD,
    BTN_HUMTEMP_STOP_MOD,

    // Temperature group
    BTN_TEMPERATURE_FIRST_DELTA_MOD,
    BTN_TEMPERATURE_SECOND_DELTA_MOD,
    BTN_TEMPERATURE_FIRST_CORRECTION_MOD,
    BTN_TEMPERATURE_SECOND_CORRECTION_MOD,
    BTN_TEMPERATURE_WARN_MOD,
    BTN_TEMPERATURE_STOP_MOD,
};


typedef enum {
    PAGE_ROUTE_MAIN_PAGE = 0,
    PAGE_ROUTE_SYSTEM_SETUP,
    PAGE_ROUTE_REC_SYSTEM,
    PAGE_ROUTE_REC_DEVICE,
    PAGE_ROUTE_VENTILATION,
    PAGE_ROUTE_FILTERING,
    PAGE_ROUTE_MAINTENANCE,
    PAGE_ROUTE_ACTIVE_FILTERING,
    PAGE_ROUTE_SETUP_MAINTENANCE,
    PAGE_ROUTE_SENSORS,
    PAGE_ROUTE_PRESSURE,
    PAGE_ROUTE_HUMIDITY,
    PAGE_ROUTE_TEMPERATURE,
} page_route_t;


typedef struct {
    uint8_t   address;
    lv_obj_t *lbl_state;
    lv_obj_t *lbl_alarms;
    lv_obj_t *lbl_address;
    lv_obj_t *lbl_serial_number;
    lv_obj_t *img_icon;
} device_info_widget_t;

struct page_data {
    lv_obj_t *btn_light;
    lv_obj_t *btn_fan;
    lv_obj_t *btn_filter;
    lv_obj_t *lbl_temperature;
    lv_obj_t *lbl_time;
    lv_obj_t *lbl_date;
    lv_obj_t *slider;

    lv_obj_t *img_light;
    lv_obj_t *img_filter;
    lv_obj_t *img_motor;

    lv_obj_t *img_error_light_1;
    lv_obj_t *img_error_light_2[2];
    lv_obj_t *img_error_light_3[3];
    lv_obj_t *img_error_filters_1;
    lv_obj_t *img_error_filters_2[2];
    lv_obj_t *img_error_motors_1;
    lv_obj_t *img_error_motors_2[2];

    lv_obj_t *lbl_pressure;

    lv_task_t *blink_task;

    lv_obj_t *cont_subpage;

    page_route_t page_route;
    union {
        struct {
            device_info_widget_t device_info_widgets[MODBUS_MAX_DEVICES];
            size_t               count;
        } rec_devices;
        struct {
            lv_obj_t *lbl_siphoning_percentages[MAX_FAN_SPEED];
            lv_obj_t *lbl_immission_percentages[MAX_FAN_SPEED];
            lv_obj_t *lbl_start;
            lv_obj_t *lbl_finish;
        } ventilation;
        struct {
            lv_obj_t *lbl_uvc_filters[MAX_FAN_SPEED];
            lv_obj_t *lbl_esf_filters[MAX_FAN_SPEED];
            lv_obj_t *lbl_pressure_threshold;
        } active_filtering;
        struct {
            lv_obj_t *lbl_uvc_filters_hours_threshold_warn;
            lv_obj_t *lbl_uvc_filters_hours_threshold_stop;
            lv_obj_t *lbl_esf_filters_hours_threshold_warn;
            lv_obj_t *lbl_esf_filters_hours_threshold_stop;
            lv_obj_t *lbl_passive_filters_hours_threshold_warn;
            lv_obj_t *lbl_passive_filters_hours_threshold_stop;
            lv_obj_t *lbl_num_passive_filters;
        } filters;
        struct {
            lv_obj_t *lbl_pressure_differences[MAX_FAN_SPEED];
            lv_obj_t *lbl_pressure_warn;
            lv_obj_t *lbl_pressure_stop;
        } pressure;
        struct {
            lv_obj_t *lbl_first_delta;
            lv_obj_t *lbl_second_delta;
            lv_obj_t *lbl_first_speed_raise;
            lv_obj_t *lbl_second_speed_raise;
            lv_obj_t *lbl_warn;
            lv_obj_t *lbl_stop;
        } humidity_temperature;
    };

    int     blink;
    uint8_t uvc_alarm;
    uint8_t esf_alarm;
    uint8_t siph_alarm;
    uint8_t imm_alarm;
};


static void     change_page_route(model_t *pmodel, struct page_data *pdata, page_route_t route);
static uint16_t modify_parameter(uint16_t value, int16_t mod, uint16_t min, uint16_t max);
static uint16_t modify_percentage(uint16_t value, int16_t mod);
static void     update_menus(model_t *pmodel, struct page_data *pdata);
static void     update_device_list(model_t *pmodel, struct page_data *pdata);


static const char *TAG = "MainPage";


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



static void update_filter_buttons(model_t *pmodel, struct page_data *data) {
    size_t esf_count = model_get_class_count(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER);
    size_t ulf_count = model_get_mode_count(pmodel, DEVICE_MODE_UVC);

    if (esf_count > 0 && ulf_count > 0) {
        lv_obj_set_hidden(data->img_error_filters_1, 1);
        lv_obj_set_hidden(data->img_error_filters_2[0], !data->uvc_alarm);
        lv_obj_set_hidden(data->img_error_filters_2[1], !data->esf_alarm);

        if (model_get_electrostatic_filter_state(pmodel) && model_get_uvc_filter_state(pmodel)) {
            lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL), &img_filtro_lampada_accesi);
        } else if (model_get_electrostatic_filter_state(pmodel)) {
            lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL), &img_filtro_acceso_uvc_spenta);
        } else {
            lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL), &img_filtro_lampada_spenti);
        }
    } else if (esf_count > 0) {
        lv_obj_set_hidden(data->img_error_filters_1, !data->esf_alarm);
        lv_obj_set_hidden(data->img_error_filters_2[0], 1);
        lv_obj_set_hidden(data->img_error_filters_2[1], 1);

        lv_obj_set_hidden(data->btn_filter, 0);
        lv_img_set_src(lv_obj_get_child(data->btn_filter, NULL), model_get_electrostatic_filter_state(pmodel)
                                                                     ? &img_filtro_elettrostatico_acceso
                                                                     : &img_filtro_elettrostatico_spento);
    } else if (ulf_count > 0) {
        lv_obj_set_hidden(data->img_error_filters_1, !data->uvc_alarm);
        lv_obj_set_hidden(data->img_error_filters_2[0], 1);
        lv_obj_set_hidden(data->img_error_filters_2[1], 1);

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
        lv_obj_set_hidden(data->img_error_motors_1, 1);
        lv_obj_set_hidden(data->img_error_motors_2[0], !data->siph_alarm);
        lv_obj_set_hidden(data->img_error_motors_2[1], !data->imm_alarm);

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
        lv_obj_set_hidden(data->img_error_motors_1, !data->siph_alarm);
        lv_obj_set_hidden(data->img_error_motors_2[0], 1);
        lv_obj_set_hidden(data->img_error_motors_2[1], 1);

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


static void update_light_buttons(model_t *pmodel, struct page_data *pdata) {
    if (model_get_light_state(pmodel) > LIGHT_STATE_OFF) {
        lv_btn_set_state(pdata->btn_light, LV_BTN_STATE_CHECKED_RELEASED);
    } else {
        lv_btn_set_state(pdata->btn_light, LV_BTN_STATE_RELEASED);
    }

    uint16_t class;
    if (model_get_light_class(pmodel, &class)) {
        lv_obj_set_hidden(pdata->btn_light, 0);

        uint8_t  alarm     = 0;
        uint8_t  alarms[]  = {0, 0, 0};
        uint16_t classes[] = {DEVICE_CLASS_LIGHT_1, DEVICE_CLASS_LIGHT_2, DEVICE_CLASS_LIGHT_3};
        for (size_t i = 0; i < 3; i++) {
            alarms[i] = model_is_there_any_alarm_for_class(pmodel, classes[i]);
            alarm |= alarms[i];
        }

        light_state_t light_state = model_get_light_state(pmodel);
        switch (class) {
            case DEVICE_CLASS_LIGHT_1:
                lv_img_set_src(pdata->img_light, light_state == LIGHT_STATE_SINGLE_ON ? &img_luce_singola_accesa
                                                                                      : &img_luce_singola_spenta);
                lv_obj_set_hidden(pdata->img_error_light_1, !alarm);
                lv_obj_set_hidden(pdata->img_error_light_2[0], 1);
                lv_obj_set_hidden(pdata->img_error_light_2[1], 1);
                lv_obj_set_hidden(pdata->img_error_light_3[0], 1);
                lv_obj_set_hidden(pdata->img_error_light_3[1], 1);
                lv_obj_set_hidden(pdata->img_error_light_3[2], 1);
                break;

            case DEVICE_CLASS_LIGHT_2: {
                const lv_img_dsc_t *images[] = {
                    &img_due_luci_spente,
                    &img_due_luci_gruppo_1_acceso,
                    &img_due_luci_gruppo_2_acceso,
                    &img_due_luci_accese,
                };
                lv_img_set_src(pdata->img_light, images[light_state]);

                lv_obj_set_hidden(pdata->img_error_light_1, 1);
                lv_obj_set_hidden(pdata->img_error_light_2[0], !alarms[0]);
                lv_obj_set_hidden(pdata->img_error_light_2[1], !alarms[1]);
                lv_obj_set_hidden(pdata->img_error_light_3[0], 1);
                lv_obj_set_hidden(pdata->img_error_light_3[1], 1);
                lv_obj_set_hidden(pdata->img_error_light_3[2], 1);
                break;
            }

            case DEVICE_CLASS_LIGHT_3: {
                const lv_img_dsc_t *images[] = {
                    &img_tre_luci_spente,
                    &img_tre_luci_gruppo_1_acceso,
                    &img_tre_luci_gruppo_1_2_accesi,
                    &img_tre_luci_accese,
                };
                lv_img_set_src(pdata->img_light, images[light_state]);

                lv_obj_set_hidden(pdata->img_error_light_1, 1);
                lv_obj_set_hidden(pdata->img_error_light_2[0], 1);
                lv_obj_set_hidden(pdata->img_error_light_2[1], 1);
                lv_obj_set_hidden(pdata->img_error_light_3[0], !alarms[0]);
                lv_obj_set_hidden(pdata->img_error_light_3[1], !alarms[1]);
                lv_obj_set_hidden(pdata->img_error_light_3[2], !alarms[2]);
                break;
            }

            default:
                assert(0);
        }
    } else {
        ESP_LOGI(TAG, "no light class");
        lv_obj_set_hidden(pdata->btn_light, 1);
    }
}


static void update_all_buttons(model_t *pmodel, struct page_data *data) {
    data->uvc_alarm  = 0;
    data->esf_alarm  = 0;
    data->siph_alarm = 0;
    data->imm_alarm  = 0;
    model_get_alarms_for_classes(pmodel, &data->uvc_alarm, &data->esf_alarm, &data->siph_alarm, &data->imm_alarm);


    update_light_buttons(pmodel, data);
    update_filter_buttons(pmodel, data);
    update_fan_buttons(pmodel, data);
}


static void update_device_sensors(model_t *pmodel, struct page_data *pdata) {
    float pressures[3] = {0};

    uint8_t starting_address = 0;
    uint8_t address          = model_get_next_pressure_sensor_device(pmodel, starting_address);
    size_t  i                = 0;

    while (address != starting_address && i < 3) {
        pressures[i] = ((float)model_get_device(pmodel, address).sensor_data.pressure) / 10.;

        starting_address = address;
        address          = model_get_next_pressure_sensor_device(pmodel, starting_address);
        i++;
    }

    char string[64] = {0};
    snprintf(string, sizeof(string), "%.1f %.1f %.1f", pressures[0], pressures[1], pressures[2]);
    lv_label_set_text(pdata->lbl_pressure, string);
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
    data->blink            = 0;
    data->cont_subpage     = NULL;

    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;
    lv_task_set_prio(data->blink_task, LV_TASK_PRIO_MID);

    lv_obj_t *img, *lbl;

    lv_obj_t *logo = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(logo, &img_logo);
    lv_obj_align(logo, NULL, LV_ALIGN_IN_TOP_MID, CENTER_X_DELTA, 8);

    lbl = lv_label_create(lv_scr_act(), NULL);
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

    data->img_light = lv_img_create(butn1, NULL);
    lv_img_set_src(data->img_light, &img_tre_luci_accese);
    lv_obj_align(data->img_light, NULL, LV_ALIGN_CENTER, 0, 0);

    data->img_motor = lv_img_create(butn2, NULL);
    lv_img_set_src(data->img_motor, &img_aspirazione_accesa);
    lv_obj_align(data->img_motor, NULL, LV_ALIGN_CENTER, 0, 0);

    data->img_filter = lv_img_create(butn3, NULL);
    lv_img_set_src(data->img_filter, &img_filtro_lampada_accesi);
    lv_obj_align(data->img_filter, NULL, LV_ALIGN_CENTER, 0, 0);

    view_register_default_callback(butn1, BTN_LIGHT_ID);
    view_register_default_callback(butn2, BTN_FAN_ID);
    view_register_default_callback(butn3, BTN_FILTER_ID);

    img = lv_img_create(data->img_light, NULL);
    lv_img_set_src(img, &img_error_md);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 15, 10);
    data->img_error_light_1 = img;

    img = lv_img_create(data->img_light, NULL);
    lv_img_set_src(img, &img_error_sm);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, -25, -5);
    data->img_error_light_2[0] = img;

    img = lv_img_create(data->img_light, NULL);
    lv_img_set_src(img, &img_error_sm);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 5, 20);
    data->img_error_light_2[1] = img;

    img = lv_img_create(data->img_light, NULL);
    lv_img_set_src(img, &img_error_sm);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, -32, -8);
    data->img_error_light_3[0] = img;

    img = lv_img_create(data->img_light, NULL);
    lv_img_set_src(img, &img_error_sm);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, -5, 10);
    data->img_error_light_3[1] = img;

    img = lv_img_create(data->img_light, NULL);
    lv_img_set_src(img, &img_error_sm);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 20, 25);
    data->img_error_light_3[2] = img;

    data->btn_light = butn1;

    img = lv_img_create(data->img_filter, NULL);
    lv_img_set_src(img, &img_error_md);
    lv_obj_align(img, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -16);
    data->img_error_filters_1 = img;

    img = lv_img_create(data->img_filter, NULL);
    lv_img_set_src(img, &img_error_sm);
    lv_obj_align(img, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -16);
    data->img_error_filters_2[0] = img;

    img = lv_img_create(data->img_filter, NULL);
    lv_img_set_src(img, &img_error_sm);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, -20);
    data->img_error_filters_2[1] = img;

    data->btn_filter = butn3;

    img = lv_img_create(data->img_motor, NULL);
    lv_img_set_src(img, &img_error_md);
    lv_obj_align(img, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -16);
    data->img_error_motors_1 = img;

    img = lv_img_create(data->img_motor, NULL);
    lv_img_set_src(img, &img_error_sm);
    lv_obj_align(img, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -16);
    data->img_error_motors_2[0] = img;

    img = lv_img_create(data->img_motor, NULL);
    lv_img_set_src(img, &img_error_sm);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, -20);
    data->img_error_motors_2[1] = img;

    data->btn_fan = butn2;

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

    lv_obj_t *tech =
        view_common_menu_button(drawer, view_intl_get_string(model, STRINGS_SYSTEM_SETUP), 300, TECH_SETTINGS_BTN_ID);
    lv_obj_set_drag_parent(tech, true);

    update_info(model, data);
    update_all_buttons(model, data);
    lv_slider_set_value(data->slider, model_get_fan_speed(model), LV_ANIM_OFF);

    lbl = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -4);
    data->lbl_pressure = lbl;

    update_device_sensors(model, data);

    update_all_buttons(model, data);

    change_page_route(model, data, data->page_route);
}


static view_message_t process_page_event(model_t *model, void *arg, view_event_t event) {
    struct page_data *data = arg;
    view_message_t    msg  = {0};

    switch (event.code) {
        case VIEW_EVENT_CODE_STATE_UPDATE:
            update_all_buttons(model, data);
            break;

        case VIEW_EVENT_CODE_UPDATE_WORK_HOURS:
            if (data->page_route == PAGE_ROUTE_MAINTENANCE) {
                // Refresh the page
                change_page_route(model, data, data->page_route);
            }
            break;

        case VIEW_EVENT_CODE_ANCILLARY_DATA_UPDATE:
            update_info(model, data);
            break;

        case VIEW_EVENT_CODE_DEVICE_UPDATE:
        case VIEW_EVENT_CODE_DEVICE_ALARM:
            update_all_buttons(model, data);
            update_device_sensors(model, data);
            update_device_list(model, data);
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
                case LV_EVENT_LONG_PRESSED_REPEAT:
                case LV_EVENT_CLICKED: {
                    switch (event.data.id) {
                        case BTN_SIPHONING_PERCENTAGE_0_MOD:
                        case BTN_SIPHONING_PERCENTAGE_1_MOD:
                        case BTN_SIPHONING_PERCENTAGE_2_MOD:
                        case BTN_SIPHONING_PERCENTAGE_3_MOD:
                        case BTN_SIPHONING_PERCENTAGE_4_MOD: {
                            uint8_t i = event.data.id - BTN_SIPHONING_PERCENTAGE_0_MOD;
                            model_set_siphoning_percentage(
                                model, i,
                                modify_percentage(model_get_siphoning_percentage(model, i), event.data.number));
                            update_menus(model, data);
                            break;
                        }

                        case BTN_IMMISSION_PERCENTAGE_0_MOD:
                        case BTN_IMMISSION_PERCENTAGE_1_MOD:
                        case BTN_IMMISSION_PERCENTAGE_2_MOD:
                        case BTN_IMMISSION_PERCENTAGE_3_MOD:
                        case BTN_IMMISSION_PERCENTAGE_4_MOD: {
                            uint8_t i = event.data.id - BTN_IMMISSION_PERCENTAGE_0_MOD;
                            model_set_immission_percentage(
                                model, i,
                                modify_percentage(model_get_immission_percentage(model, i), event.data.number));
                            update_menus(model, data);
                            break;
                        }

                        case BTN_CLEANING_START_MOD:
                            model_set_environment_cleaning_start_period(
                                model, modify_parameter(model_get_environment_cleaning_start_period(model),
                                                        event.data.number, 0, 30));
                            update_menus(model, data);
                            break;

                        case BTN_CLEANING_FINISH_MOD:
                            model_set_environment_cleaning_finish_period(
                                model, modify_parameter(model_get_environment_cleaning_finish_period(model),
                                                        event.data.number, 0, 30));
                            update_menus(model, data);
                            break;

                        case BTN_UVC_FILTER_0_MOD:
                        case BTN_UVC_FILTER_1_MOD:
                        case BTN_UVC_FILTER_2_MOD:
                        case BTN_UVC_FILTER_3_MOD:
                        case BTN_UVC_FILTER_4_MOD: {
                            uint8_t i = event.data.id - BTN_UVC_FILTER_0_MOD;
                            model_set_uvc_filters_for_speed(
                                model, i,
                                modify_parameter(model_get_uvc_filters_for_speed(model, i), event.data.number, 0, 3));
                            update_menus(model, data);
                            break;
                        }

                        case BTN_ESF_FILTER_0_MOD:
                        case BTN_ESF_FILTER_1_MOD:
                        case BTN_ESF_FILTER_2_MOD:
                        case BTN_ESF_FILTER_3_MOD:
                        case BTN_ESF_FILTER_4_MOD: {
                            uint8_t i = event.data.id - BTN_ESF_FILTER_0_MOD;
                            model_set_esf_filters_for_speed(
                                model, i,
                                modify_parameter(model_get_esf_filters_for_speed(model, i), event.data.number, 0, 3));
                            update_menus(model, data);
                            break;
                        }

                        case BTN_PRESSURE_DIFFERENCE_0_MOD:
                        case BTN_PRESSURE_DIFFERENCE_1_MOD:
                        case BTN_PRESSURE_DIFFERENCE_2_MOD:
                        case BTN_PRESSURE_DIFFERENCE_3_MOD:
                        case BTN_PRESSURE_DIFFERENCE_4_MOD: {
                            uint8_t i = event.data.id - BTN_PRESSURE_DIFFERENCE_0_MOD;
                            model_set_pressure_difference(
                                model, i,
                                modify_parameter(model_get_pressure_difference(model, i), event.data.number, 0, 100));
                            update_menus(model, data);
                            break;
                        }

                        case BTN_PRESSURE_WARN_MOD:
                            model_set_pressure_difference_deviation_warn(
                                model, modify_parameter(model_get_pressure_difference_deviation_warn(model),
                                                        event.data.number * 5, 0, 100));
                            update_menus(model, data);
                            break;

                        case BTN_PRESSURE_STOP_MOD:
                            model_set_pressure_difference_deviation_stop(
                                model, modify_parameter(model_get_pressure_difference_deviation_stop(model),
                                                        event.data.number * 5, 0, 100));
                            update_menus(model, data);
                            break;

                        case BTN_PRESSURE_THRESHOLD_MOD:
                            model_set_pressure_threshold_mb(
                                model,
                                modify_parameter(model_get_pressure_threshold_mb(model), event.data.number, 0, 50));
                            update_menus(model, data);
                            break;

                        case BTN_PASSIVE_FILTERS_HOURS_THRESHOLD_WARN_MOD:
                            model_set_passive_filters_hours_warning_threshold(
                                model, modify_parameter(model_get_passive_filters_hours_warning_threshold(model),
                                                        event.data.number * 100, 100, 10000));
                            update_menus(model, data);
                            break;

                        case BTN_PASSIVE_FILTERS_HOURS_THRESHOLD_STOP_MOD:
                            model_set_passive_filters_hours_stop_threshold(
                                model, modify_parameter(model_get_passive_filters_hours_stop_threshold(model),
                                                        event.data.number * 100, 100, 10000));
                            update_menus(model, data);
                            break;

                        case BTN_UVC_FILTERS_HOURS_THRESHOLD_WARN_MOD:
                            model_set_uvc_filters_hours_warning_threshold(
                                model, modify_parameter(model_get_uvc_filters_hours_warning_threshold(model),
                                                        event.data.number * 100, 100, 10000));
                            update_menus(model, data);
                            break;

                        case BTN_UVC_FILTERS_HOURS_THRESHOLD_STOP_MOD:
                            model_set_uvc_filters_hours_stop_threshold(
                                model, modify_parameter(model_get_uvc_filters_hours_stop_threshold(model),
                                                        event.data.number * 100, 100, 10000));
                            update_menus(model, data);
                            break;

                        case BTN_ESF_FILTERS_HOURS_THRESHOLD_WARN_MOD:
                            model_set_esf_filters_hours_warning_threshold(
                                model, modify_parameter(model_get_esf_filters_hours_warning_threshold(model),
                                                        event.data.number * 100, 100, 10000));
                            update_menus(model, data);
                            break;

                        case BTN_ESF_FILTERS_HOURS_THRESHOLD_STOP_MOD:
                            model_set_esf_filters_hours_stop_threshold(
                                model, modify_parameter(model_get_esf_filters_hours_stop_threshold(model),
                                                        event.data.number * 100, 100, 10000));
                            update_menus(model, data);
                            break;

                        case BTN_HUMTEMP_FIRST_DELTA_MOD:
                            if (data->page_route == PAGE_ROUTE_HUMIDITY) {
                                model_set_first_humidity_delta(
                                    model,
                                    modify_parameter(model_get_first_humidity_delta(model), event.data.number, 0, 100));
                            } else {
                                model_set_first_temperature_delta(
                                    model, modify_parameter(model_get_first_temperature_delta(model), event.data.number,
                                                            0, 100));
                            }
                            update_menus(model, data);
                            break;

                        case BTN_HUMTEMP_SECOND_DELTA_MOD:
                            if (data->page_route == PAGE_ROUTE_HUMIDITY) {
                                model_set_second_humidity_delta(model,
                                                                modify_parameter(model_get_second_humidity_delta(model),
                                                                                 event.data.number, 0, 100));
                            } else {
                                model_set_second_temperature_delta(
                                    model, modify_parameter(model_get_second_temperature_delta(model),
                                                            event.data.number, 0, 100));
                            }
                            update_menus(model, data);
                            break;

                        case BTN_HUMTEMP_FIRST_CORRECTION_MOD:
                            if (data->page_route == PAGE_ROUTE_HUMIDITY) {
                                model_set_first_humidity_speed_raise(
                                    model, modify_parameter(model_get_first_humidity_speed_raise(model),
                                                            event.data.number, 0, 100));
                            } else {
                                model_set_first_temperature_speed_raise(
                                    model, modify_parameter(model_get_first_temperature_speed_raise(model),
                                                            event.data.number, 0, 100));
                            }
                            update_menus(model, data);
                            break;

                        case BTN_HUMTEMP_SECOND_CORRECTION_MOD:
                            if (data->page_route == PAGE_ROUTE_HUMIDITY) {
                                model_set_second_humidity_speed_raise(
                                    model, modify_parameter(model_get_second_humidity_speed_raise(model),
                                                            event.data.number, 0, 100));
                            } else {
                                model_set_second_temperature_speed_raise(
                                    model, modify_parameter(model_get_second_temperature_speed_raise(model),
                                                            event.data.number, 0, 100));
                            }
                            update_menus(model, data);
                            break;

                        case BTN_HUMTEMP_WARN_MOD:
                            if (data->page_route == PAGE_ROUTE_HUMIDITY) {
                                model_set_humidity_warn(
                                    model, modify_parameter(model_get_humidity_warn(model), event.data.number, 0, 100));
                            } else {
                                model_set_temperature_warn(model, modify_parameter(model_get_temperature_warn(model),
                                                                                   event.data.number, 0, 100));
                            }
                            update_menus(model, data);
                            break;

                        case BTN_HUMTEMP_STOP_MOD:
                            if (data->page_route == PAGE_ROUTE_HUMIDITY) {
                                model_set_humidity_stop(
                                    model, modify_parameter(model_get_humidity_stop(model), event.data.number, 0, 100));
                            } else {
                                model_set_temperature_stop(model, modify_parameter(model_get_temperature_stop(model),
                                                                                   event.data.number, 0, 100));
                            }
                            update_menus(model, data);
                            break;

                        case BTN_PASSIVE_FILTERS_NUM_MOD:
                            model_set_num_passive_filters(model, modify_parameter(model_get_num_passive_filters(model),
                                                                                  event.data.number, 0, 10));
                            update_menus(model, data);
                            break;

                        case BUTTON_PLUS_ID: {
                            uint8_t speed = model_get_fan_speed(model);

                            if (speed < MAX_FAN_SPEED - 1) {
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

                        case BTN_RESET_PASSIVE_FILTERS_HOURS:
                            model_reset_passive_filters_work_hours(model);
                            change_page_route(model, data, data->page_route);
                            break;

                        case BTN_RESET_FILTER_HOURS:
                            msg.cmsg.code    = VIEW_CONTROLLER_MESSAGE_CODE_RESET_FILTER_HOURS;
                            msg.cmsg.address = event.data.number;
                            break;

                        case BTN_BACK_ID: {
                            switch (data->page_route) {
                                case PAGE_ROUTE_MAIN_PAGE:
                                case PAGE_ROUTE_SYSTEM_SETUP:
                                    change_page_route(model, data, PAGE_ROUTE_MAIN_PAGE);
                                    break;

                                case PAGE_ROUTE_REC_SYSTEM:
                                case PAGE_ROUTE_VENTILATION:
                                case PAGE_ROUTE_FILTERING:
                                case PAGE_ROUTE_SENSORS:
                                    change_page_route(model, data, PAGE_ROUTE_SYSTEM_SETUP);
                                    break;

                                case PAGE_ROUTE_MAINTENANCE:
                                case PAGE_ROUTE_SETUP_MAINTENANCE:
                                case PAGE_ROUTE_ACTIVE_FILTERING:
                                    change_page_route(model, data, PAGE_ROUTE_FILTERING);
                                    break;

                                case PAGE_ROUTE_REC_DEVICE:
                                    change_page_route(model, data, PAGE_ROUTE_REC_SYSTEM);
                                    break;

                                case PAGE_ROUTE_PRESSURE:
                                case PAGE_ROUTE_HUMIDITY:
                                case PAGE_ROUTE_TEMPERATURE:
                                    change_page_route(model, data, PAGE_ROUTE_SENSORS);
                                    break;
                            }
                            break;
                        }

                        case BTN_REC_SYSTEM_ID:
                            change_page_route(model, data, PAGE_ROUTE_REC_SYSTEM);
                            break;

                        case BTN_VENTILATION_ID:
                            change_page_route(model, data, PAGE_ROUTE_VENTILATION);
                            break;

                        case BTN_FILTERING_ID:
                            change_page_route(model, data, PAGE_ROUTE_FILTERING);
                            break;

                        case BTN_MAINTENANCE_ID:
                            change_page_route(model, data, PAGE_ROUTE_MAINTENANCE);
                            break;

                        case BTN_ACTIVE_FILTERING_ID:
                            change_page_route(model, data, PAGE_ROUTE_ACTIVE_FILTERING);
                            break;

                        case BTN_REC_DEVICE_ID:
                            change_page_route(model, data, PAGE_ROUTE_REC_DEVICE);
                            break;

                        case BTN_SETUP_MAINTENANCE_FILTERING_ID:
                            change_page_route(model, data, PAGE_ROUTE_SETUP_MAINTENANCE);
                            break;

                        case BTN_SENSORS_ID:
                            change_page_route(model, data, PAGE_ROUTE_SENSORS);
                            break;

                        case BTN_PRESSURE_ID:
                            change_page_route(model, data, PAGE_ROUTE_PRESSURE);
                            break;

                        case BTN_HUMIDITY_ID:
                            change_page_route(model, data, PAGE_ROUTE_HUMIDITY);
                            break;

                        case BTN_TEMPERATURE_ID:
                            change_page_route(model, data, PAGE_ROUTE_TEMPERATURE);
                            break;

                        case BTN_DEVICE_ID:
                            msg.vmsg.code  = VIEW_COMMAND_CODE_CHANGE_PAGE_EXTRA;
                            msg.vmsg.page  = &page_device_info;
                            msg.vmsg.extra = (void *)(uintptr_t)event.data.number;
                            break;

                        case BTN_SEARCH_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_devices_manage;
                            break;

                        case BTN_AUTOCONF_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_automatic_device_config;
                            break;

                        case ERRORS_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_errors;
                            break;

                        case SETTINGS_BTN_ID:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_settings;
                            break;

                        case TECH_SETTINGS_BTN_ID:
                            change_page_route(model, data, PAGE_ROUTE_SYSTEM_SETUP);
                            break;

                        case BTN_START_CALIBRATION:
                            msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                            msg.vmsg.page = &page_pressure_calibration;
                            break;

                        case BTN_LIGHT_ID: {
                            uint16_t class;
                            if (!model_get_light_class(model, &class) || event.lv_event != LV_EVENT_CLICKED) {
                                break;
                            }

                            model_light_switch(model);

                            switch (class) {
                                case DEVICE_CLASS_LIGHT_1:
                                    msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_LIGHTS;
                                    if (model_get_light_state(model) == LIGHT_STATE_SINGLE_ON) {
                                        msg.cmsg.light_value = 1;
                                    } else {
                                        msg.cmsg.light_value = 0;
                                    }
                                    break;

                                case DEVICE_CLASS_LIGHT_2: {
                                    msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_LIGHTS;

                                    switch (model_get_light_state(model)) {
                                        case LIGHT_STATE_OFF:
                                            msg.cmsg.light_value = 0;
                                            break;

                                        case LIGHT_STATE_GROUP_1_ON:
                                            msg.cmsg.light_value = 0x01;
                                            break;


                                        case LIGHT_STATE_GROUP_2_ON:
                                            msg.cmsg.light_value = 0x02;
                                            break;

                                        case LIGHT_STATE_DOUBLE_ON:
                                            msg.cmsg.light_value = 0x03;
                                            break;
                                    }
                                    break;
                                }

                                case DEVICE_CLASS_LIGHT_3: {
                                    msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_LIGHTS;

                                    switch (model_get_light_state(model)) {
                                        case LIGHT_STATE_OFF:
                                            msg.cmsg.light_value = 0;
                                            break;

                                        case LIGHT_STATE_GROUP_1_ON:
                                            msg.cmsg.light_value = 0x01;
                                            break;

                                        case LIGHT_STATE_GROUP_2_ON:
                                            msg.cmsg.light_value = 0x03;
                                            break;

                                        case LIGHT_STATE_TRIPLE_ON:
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
                            if (event.lv_event != LV_EVENT_CLICKED) {
                                break;
                            }
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FAN;
                            break;

                        case BTN_FILTER_ID:
                            if (event.lv_event != LV_EVENT_CLICKED) {
                                break;
                            }
                            msg.cmsg.code = VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FILTER;
                            break;

                        default:
                            break;
                    }
                    break;

                    case LV_EVENT_LONG_PRESSED: {
                        switch (event.data.id) {
                            case BTN_REC_DEVICE_ID:
                                msg.vmsg.code = VIEW_COMMAND_CODE_CHANGE_PAGE;
                                msg.vmsg.page = &page_devices;
                                break;
                        }
                        break;
                    }

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
    data->cont_subpage = NULL;
}


static lv_obj_t *cont_subpage_create(const char *title) {
    lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont, LV_HOR_RES - 32, LV_VER_RES - 2);
    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *lbl = lv_label_create(cont, NULL);
    lv_label_set_text(lbl, title);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 16, 12);

    view_common_back_button(cont, BTN_BACK_ID);

    return cont;
}


static lv_obj_t *bordered_cont(lv_obj_t *root) {
    lv_obj_t *cont = lv_cont_create(root, NULL);
    lv_obj_set_style_local_border_width(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 2);
    lv_obj_set_style_local_radius(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 8);
    lv_obj_set_size(cont, 70, 70);
    return cont;
}


static lv_obj_t *par_control_cont(lv_obj_t *root, lv_obj_t **lbl_return, int id) {
    lv_obj_t *cont = bordered_cont(root);

    lv_obj_t *lbl = lv_label_create(cont, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_small());
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(lbl, NULL, LV_ALIGN_IN_TOP_MID, 0, 2);
    lv_obj_set_auto_realign(lbl, 1);

    if (lbl_return != NULL) {
        *lbl_return = lbl;
    }

    lv_obj_t *btn = lv_btn_create(cont, NULL);
    lv_obj_set_style_local_border_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, STYLE_FAINT_YELLOW);
    lv_obj_set_style_local_radius(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_obj_set_size(btn, lv_obj_get_width(cont) - 8, lv_obj_get_height(cont) / 3 - 1);

    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_small());
    lv_label_set_text(lbl, LV_SYMBOL_PLUS);

    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -lv_obj_get_height(btn) - 4);
    view_register_default_callback_number(btn, id, +1);

    btn = lv_btn_create(cont, NULL);
    lv_obj_set_style_local_border_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, STYLE_FAINT_CYAN);
    lv_obj_set_style_local_radius(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_obj_set_size(btn, lv_obj_get_width(cont) - 8, lv_obj_get_height(cont) / 3 - 1);

    lbl = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_font(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_small());
    lv_label_set_text(lbl, LV_SYMBOL_MINUS);

    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -3);
    view_register_default_callback_number(btn, id, -1);

    return cont;
}


static void update_device_list(model_t *pmodel, struct page_data *pdata) {
    if (pdata->cont_subpage == NULL || pdata->page_route != PAGE_ROUTE_REC_DEVICE) {
        return;
    }

    for (size_t i = 0; i < pdata->rec_devices.count; i++) {
        lv_obj_t *img               = pdata->rec_devices.device_info_widgets[i].img_icon;
        lv_obj_t *lbl_state         = pdata->rec_devices.device_info_widgets[i].lbl_state;
        lv_obj_t *lbl_alarms        = pdata->rec_devices.device_info_widgets[i].lbl_alarms;
        lv_obj_t *lbl_address       = pdata->rec_devices.device_info_widgets[i].lbl_address;
        lv_obj_t *lbl_serial_number = pdata->rec_devices.device_info_widgets[i].lbl_serial_number;
        device_t  device            = model_get_device(pmodel, pdata->rec_devices.device_info_widgets[i].address);

        lv_label_set_text_fmt(lbl_serial_number, "SN: %8i", device.serial_number);

        // Icon
        view_common_get_class_icon(device.class, img);

        // State
        switch (device.class) {
            case DEVICE_CLASS_LIGHT_1:
            case DEVICE_CLASS_LIGHT_2:
            case DEVICE_CLASS_LIGHT_3:
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1):
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2):
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3):
            case DEVICE_CLASS_ELECTROSTATIC_FILTER:
            case DEVICE_CLASS_GAS:
                lv_label_set_text_fmt(lbl_state, "Output: %s", device.actuator_data.ourput_state ? "ON" : "OFF");
                break;
            case DEVICE_CLASS_IMMISSION_FAN:
            case DEVICE_CLASS_SIPHONING_FAN:
                lv_label_set_text_fmt(lbl_state, "Output: %s - %i%%",
                                      (device.actuator_data.ourput_state & 0xFF) > 0 ? "ON" : "OFF",
                                      (device.actuator_data.ourput_state >> 8) & 0xFF);
                break;
            case DEVICE_CLASS_PRESSURE_SAFETY:
                lv_label_set_text_fmt(lbl_state, "%i%s %imB", device.sensor_data.temperature,
                                      model_get_degrees_symbol(pmodel), device.sensor_data.pressure);
                break;
            case DEVICE_CLASS_TEMPERATURE_HUMIDITY_SAFETY:
                lv_label_set_text_fmt(lbl_state, "%i%% %i%s", device.sensor_data.humidity,
                                      device.sensor_data.temperature, model_get_degrees_symbol(pmodel));
                break;
            case DEVICE_CLASS_PRESSURE_TEMPERATURE_HUMIDITY_SAFETY:
                lv_label_set_text_fmt(lbl_state, "%i%% %i%s %imB", device.sensor_data.humidity,
                                      device.sensor_data.temperature, model_get_degrees_symbol(pmodel),
                                      device.sensor_data.pressure);
                break;
            default:
                lv_label_set_text(lbl_state, "");
                break;
        }

        // Alarms
        switch (device.class) {
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1):
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2):
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3):
            case DEVICE_CLASS_ELECTROSTATIC_FILTER:
            case DEVICE_CLASS_GAS:
                lv_label_set_text_fmt(lbl_alarms, "%s: %s  FB: %s", view_intl_get_string(pmodel, STRINGS_ALLARME),
                                      (device.alarms & EASYCONNECT_SAFETY_ALARM) > 0 ? "OK" : "KO",
                                      (device.alarms & EASYCONNECT_FEEDBACK_ALARM) > 0 ? "OK" : "KO");
                break;
            default:
                lv_label_set_text_fmt(lbl_alarms, "%s: %s", view_intl_get_string(pmodel, STRINGS_ALLARME),
                                      (device.alarms & EASYCONNECT_SAFETY_ALARM) > 0 ? "OK" : "KO");
                break;
        }


        // Group
        switch (device.class) {
            case DEVICE_CLASS_LIGHT_1:
            case DEVICE_CLASS_LIGHT_2:
            case DEVICE_CLASS_LIGHT_3:
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_1):
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_2):
            case DEVICE_CLASS_ULTRAVIOLET_FILTER(DEVICE_GROUP_3):
            case DEVICE_CLASS_SIPHONING_FAN:
            case DEVICE_CLASS_IMMISSION_FAN:
                lv_label_set_text_fmt(lbl_address, "IP: %03i Gr.: %i", device.address, CLASS_GET_GROUP(device.class));
                break;

            default:
                lv_label_set_text_fmt(lbl_address, "IP: %03i Gr.: -", device.address);
                break;
        }

        lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);
    }
}


static device_info_widget_t device_info_widget_create(lv_obj_t *parent, device_t device) {
    lv_obj_t *cont = lv_cont_create(parent, NULL);
    lv_obj_add_style(cont, LV_CONT_PART_MAIN, &style_transparent_cont);
    lv_obj_set_size(cont, 410, 56);
    lv_page_glue_obj(cont, 1);

    lv_obj_t *btn = lv_btn_create(cont, NULL);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_size(btn, 48, 48);
    lv_obj_t *img = lv_img_create(btn, NULL);

    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);
    view_register_default_callback_number(btn, BTN_DEVICE_ID, device.address);

    lv_obj_t *lbl_state = lv_label_create(cont, NULL);
    lv_obj_set_auto_realign(lbl_state, 1);
    lv_obj_align(lbl_state, btn, LV_ALIGN_OUT_LEFT_TOP, -4, 0);

    lv_obj_t *lbl_address = lv_label_create(cont, NULL);
    lv_obj_set_auto_realign(lbl_address, 1);
    lv_obj_align(lbl_address, btn, LV_ALIGN_OUT_RIGHT_TOP, 4, 0);

    lv_obj_t *lbl_alarms = lv_label_create(cont, NULL);
    lv_obj_set_auto_realign(lbl_alarms, 1);
    lv_obj_align(lbl_alarms, btn, LV_ALIGN_OUT_LEFT_BOTTOM, -4, 0);

    lv_obj_t *lbl_serial_number = lv_label_create(cont, NULL);
    lv_obj_set_auto_realign(lbl_serial_number, 1);
    lv_obj_align(lbl_serial_number, btn, LV_ALIGN_OUT_RIGHT_BOTTOM, 4, 0);

    return (device_info_widget_t){
        .address           = device.address,
        .lbl_address       = lbl_address,
        .lbl_alarms        = lbl_alarms,
        .lbl_serial_number = lbl_serial_number,
        .lbl_state         = lbl_state,
        .img_icon          = img,
    };
}


static lv_obj_t *filter_info_create(model_t *pmodel, lv_obj_t *parent, const char *description, device_group_t group,
                                    uint16_t worked_hours, uint16_t hours_left, const lv_img_dsc_t *img_icon,
                                    uint8_t warning, int id, int num) {
    const lv_img_dsc_t *img_btn = &img_icon_reset_md;
    if (hours_left == 0) {
        img_btn = &img_icon_reset_stop_md;
    } else if (warning) {
        img_btn = &img_icon_reset_warning_md;
    }

    lv_obj_t *cont = lv_cont_create(parent, NULL);
    lv_obj_add_style(cont, LV_CONT_PART_MAIN, &style_transparent_cont);
    lv_obj_set_size(cont, 410, 70);
    lv_page_glue_obj(cont, 1);

    lv_obj_t *btn_reset = lv_btn_create(cont, NULL);
    lv_obj_set_size(btn_reset, 70, 70);
    lv_obj_t *img = lv_img_create(btn_reset, NULL);
    lv_img_set_src(img, img_btn);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(btn_reset, NULL, LV_ALIGN_IN_LEFT_MID, 8, 0);
    view_register_default_callback_number(btn_reset, id, num);

    lv_obj_t *cont_icon = bordered_cont(cont);
    img                 = lv_img_create(cont_icon, NULL);
    lv_img_set_src(img, img_icon);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    if (hours_left == 0) {
        img = lv_img_create(cont_icon, NULL);
        lv_img_set_src(img, &img_stop);
        lv_obj_align(img, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -4, -4);
    } else if (warning) {
        img = lv_img_create(cont_icon, NULL);
        lv_img_set_src(img, &img_warn);
        lv_obj_align(img, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -4, -4);
    }

    lv_obj_align(cont_icon, btn_reset, LV_ALIGN_OUT_RIGHT_MID, 4, 0);

    lv_obj_t *lbl = lv_label_create(cont, NULL);
    lv_label_set_text_fmt(lbl, "%s: %s - %i", view_intl_get_string(pmodel, STRINGS_GRUPPO),
                          view_intl_get_string(pmodel, STRINGS_FILTRO), group + 1);
    lv_obj_align(lbl, cont_icon, LV_ALIGN_OUT_RIGHT_TOP, 4, 0);

    lbl = lv_label_create(cont, NULL);
    lv_label_set_text_fmt(lbl, "%s: %i", view_intl_get_string(pmodel, STRINGS_ORE_DI_LAVORO), worked_hours);
    lv_obj_align(lbl, cont_icon, LV_ALIGN_OUT_RIGHT_MID, 4, 0);

    lbl = lv_label_create(cont, NULL);
    lv_label_set_text_fmt(lbl, "%s: %i", view_intl_get_string(pmodel, STRINGS_ORE_RIMASTE), hours_left);
    lv_obj_align(lbl, cont_icon, LV_ALIGN_OUT_RIGHT_BOTTOM, 4, 0);

    return cont;
}


static void change_page_route(model_t *pmodel, struct page_data *pdata, page_route_t route) {
    if (pdata->cont_subpage != NULL) {
        lv_obj_del(pdata->cont_subpage);
        pdata->cont_subpage = NULL;
    }

    pdata->page_route = route;
    switch (pdata->page_route) {
        case PAGE_ROUTE_MAIN_PAGE:
            break;

        case PAGE_ROUTE_SYSTEM_SETUP: {
            lv_obj_t *btn, *lbl;
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_SYSTEM_SETUP));

            btn = view_common_default_menu_button(pdata->cont_subpage,
                                                  view_intl_get_string(pmodel, STRINGS_SISTEMA_REC), BTN_REC_SYSTEM_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, -68);

            btn = view_common_default_menu_button(
                pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_VENTILAZIONE), BTN_VENTILATION_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, -4);

            lbl = lv_label_create(pdata->cont_subpage, NULL);
            lv_label_set_text_fmt(lbl, "v%s", APP_CONFIG_FIRMWARE_VERSION);
            lv_obj_align(lbl, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -4);

            btn = view_common_default_menu_button(pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_FILTRAGGIO),
                                                  BTN_FILTERING_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 60);

            btn = view_common_default_menu_button(pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_SENSORI),
                                                  BTN_SENSORS_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 124);
            break;
        }

        case PAGE_ROUTE_REC_SYSTEM: {
            lv_obj_t *btn;
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_SYSTEM_SETUP));

            btn = view_common_default_menu_button(
                pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_DISPOSITIVI_REC), BTN_REC_DEVICE_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, -68);

            btn = view_common_default_menu_button(pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_RICERCA),
                                                  BTN_SEARCH_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, -4);

            btn = view_common_default_menu_button(
                pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_CONF_AUTOMATICA), BTN_AUTOCONF_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 60);
            break;
        }

        case PAGE_ROUTE_SENSORS: {
            lv_obj_t *btn;
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_SENSORI));

            btn = view_common_default_menu_button(pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_PRESSIONE),
                                                  BTN_PRESSURE_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, -68);

            btn = view_common_default_menu_button(pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_UMIDITA),
                                                  BTN_HUMIDITY_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, -4);

            btn = view_common_default_menu_button(
                pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_TEMPERATURA), BTN_TEMPERATURE_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 60);
            break;
        }

        case PAGE_ROUTE_REC_DEVICE: {
            lv_obj_t *page;
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_DISPOSITIVI_REC));

            page = lv_page_create(pdata->cont_subpage, NULL);
            lv_obj_set_size(page, LV_HOR_RES - 32, 270);
            lv_obj_align(page, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

            lv_page_set_scrl_layout(page, LV_LAYOUT_COLUMN_LEFT);
            lv_obj_set_style_local_pad_inner(page, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 0);
            lv_obj_set_style_local_pad_top(page, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 0);

            uint8_t starting_address = 0;
            uint8_t address          = model_get_next_device_address(pmodel, starting_address);
            size_t  count            = 0;
            while (address != starting_address) {
                pdata->rec_devices.device_info_widgets[count] =
                    device_info_widget_create(page, model_get_device(pmodel, address));

                starting_address = address;
                address          = model_get_next_device_address(pmodel, starting_address);
                count++;
            }
            pdata->rec_devices.count = count;

            update_device_list(pmodel, pdata);
            break;
        }

        case PAGE_ROUTE_MAINTENANCE: {
            lv_obj_t *page;
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_MANUTENZIONE));

            page = lv_page_create(pdata->cont_subpage, NULL);
            lv_obj_set_size(page, LV_HOR_RES - 32, 260);
            lv_obj_align(page, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

            lv_page_set_scrl_layout(page, LV_LAYOUT_COLUMN_LEFT);
            lv_obj_set_style_local_pad_inner(page, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 0);
            lv_obj_set_style_local_pad_top(page, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 0);

            filter_info_create(pmodel, page, view_intl_get_string(pmodel, STRINGS_FILTRO), DEVICE_GROUP_1,
                               model_get_passive_filters_work_hours(pmodel),
                               model_get_passive_filters_remaining_hours(pmodel), &img_icon_passive_filter_md,
                               model_get_passive_filter_warning(pmodel), BTN_RESET_PASSIVE_FILTERS_HOURS, 0);

            uint8_t starting_address = 0;
            uint8_t address          = model_get_next_device_address_by_mode(pmodel, starting_address, DEVICE_MODE_UVC);

            while (address != starting_address) {
                device_t device = model_get_device(pmodel, address);
                filter_info_create(pmodel, page, "UVC", CLASS_GET_GROUP(device.class),
                                   model_get_filter_device_work_hours(pmodel, address),
                                   model_get_filter_device_remaining_hours(pmodel, address), &img_icon_uvc_md,
                                   model_get_filter_device_warning(pmodel, address), BTN_RESET_FILTER_HOURS, address);

                starting_address = address;
                address          = model_get_next_device_address_by_mode(pmodel, starting_address, DEVICE_MODE_UVC);
            }


            starting_address = 0;
            address          = model_get_next_device_address_by_mode(pmodel, starting_address, DEVICE_MODE_ESF);

            while (address != starting_address) {
                device_t device = model_get_device(pmodel, address);
                filter_info_create(pmodel, page, "ESF", CLASS_GET_GROUP(device.class),
                                   model_get_filter_device_work_hours(pmodel, address),
                                   model_get_filter_device_remaining_hours(pmodel, address), &img_icon_esf_md,
                                   model_get_filter_device_warning(pmodel, address), BTN_RESET_FILTER_HOURS, address);

                starting_address = address;
                address          = model_get_next_device_address_by_mode(pmodel, starting_address, DEVICE_MODE_ESF);
            }

            break;
        }

        case PAGE_ROUTE_VENTILATION: {
            lv_obj_t *cont, *img;
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_VENTILAZIONE));

            const int           row_y[]                       = {-32, 40};
            const int           row_x[]                       = {8, 8};
            const lv_img_dsc_t *imgs[]                        = {&img_icon_siphoning_md, &img_icon_immission_md};
            const int           identifiers[2][MAX_FAN_SPEED] = {
                {BTN_SIPHONING_PERCENTAGE_0_MOD, BTN_SIPHONING_PERCENTAGE_1_MOD, BTN_SIPHONING_PERCENTAGE_2_MOD,
                           BTN_SIPHONING_PERCENTAGE_3_MOD, BTN_SIPHONING_PERCENTAGE_4_MOD},
                {BTN_IMMISSION_PERCENTAGE_0_MOD, BTN_IMMISSION_PERCENTAGE_1_MOD, BTN_IMMISSION_PERCENTAGE_2_MOD,
                           BTN_IMMISSION_PERCENTAGE_3_MOD, BTN_IMMISSION_PERCENTAGE_4_MOD},
            };
            lv_obj_t **labels[] = {
                pdata->ventilation.lbl_siphoning_percentages,
                pdata->ventilation.lbl_immission_percentages,
            };

            for (size_t row = 0; row < 2; row++) {
                cont = bordered_cont(pdata->cont_subpage);
                lv_obj_align(cont, NULL, LV_ALIGN_IN_LEFT_MID, row_x[row], row_y[row]);

                img = lv_img_create(cont, NULL);
                lv_img_set_src(img, imgs[row]);
                lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

                for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
                    cont = par_control_cont(pdata->cont_subpage, &labels[row][i], identifiers[row][i]);
                    lv_obj_align(cont, NULL, LV_ALIGN_IN_LEFT_MID, row_x[row] + (lv_obj_get_width(cont) + 2) * (i + 1),
                                 row_y[row]);
                }
            }

            for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
                for (size_t j = 0; j < i + 1; j++) {
                    img = lv_img_create(pdata->cont_subpage, NULL);
                    lv_img_set_src(img, &img_bar);
                    lv_obj_align(img, NULL, LV_ALIGN_IN_LEFT_MID, 20 + 72 * (i + 1), -72 - 7 * j);
                }
            }

            cont = bordered_cont(pdata->cont_subpage);
            lv_obj_align(cont, NULL, LV_ALIGN_CENTER, -148, 116);

            img = lv_img_create(cont, NULL);
            lv_img_set_src(img, &img_start_cleaning);
            lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

            lv_obj_t *ref_cont = cont;

            cont = par_control_cont(pdata->cont_subpage, &pdata->ventilation.lbl_start, BTN_CLEANING_START_MOD);
            lv_obj_align(cont, ref_cont, LV_ALIGN_OUT_RIGHT_MID, 4, 0);

            cont = bordered_cont(pdata->cont_subpage);
            lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 68, 116);

            img = lv_img_create(cont, NULL);
            lv_img_set_src(img, &img_finish_cleaning);
            lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

            ref_cont = cont;

            cont = par_control_cont(pdata->cont_subpage, &pdata->ventilation.lbl_finish, BTN_CLEANING_FINISH_MOD);
            lv_obj_align(cont, ref_cont, LV_ALIGN_OUT_RIGHT_MID, 4, 0);

            break;
        }

        case PAGE_ROUTE_FILTERING: {
            lv_obj_t *btn;
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_FILTRAGGIO));

            btn = view_common_default_menu_button(
                pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_MANUTENZIONE), BTN_MAINTENANCE_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, -68);

            btn = view_common_default_menu_button(
                pdata->cont_subpage, view_intl_get_string(pmodel, STRINGS_FILTRI_ATTIVI), BTN_ACTIVE_FILTERING_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, -4);

            btn = view_common_default_menu_button(pdata->cont_subpage,
                                                  view_intl_get_string(pmodel, STRINGS_SETUP_MANUTENZIONE),
                                                  BTN_SETUP_MAINTENANCE_FILTERING_ID);
            lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 60);
            break;
        }

        case PAGE_ROUTE_ACTIVE_FILTERING: {
            lv_obj_t *cont, *img;
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_FILTRI_ATTIVI));

            const int           row_y[]                       = {-34, 38};
            const int           row_x[]                       = {8, 8};
            const lv_img_dsc_t *imgs[]                        = {&img_icon_uvc_md, &img_icon_esf_md};
            const int           identifiers[2][MAX_FAN_SPEED] = {
                {BTN_UVC_FILTER_0_MOD, BTN_UVC_FILTER_1_MOD, BTN_UVC_FILTER_2_MOD, BTN_UVC_FILTER_3_MOD,
                           BTN_UVC_FILTER_4_MOD},
                {BTN_ESF_FILTER_0_MOD, BTN_ESF_FILTER_1_MOD, BTN_ESF_FILTER_2_MOD, BTN_ESF_FILTER_3_MOD,
                           BTN_ESF_FILTER_4_MOD},
            };
            lv_obj_t **labels[] = {
                pdata->active_filtering.lbl_uvc_filters,
                pdata->active_filtering.lbl_esf_filters,
            };

            for (size_t row = 0; row < 2; row++) {
                cont = bordered_cont(pdata->cont_subpage);
                lv_obj_align(cont, NULL, LV_ALIGN_IN_LEFT_MID, row_x[row], row_y[row]);

                img = lv_img_create(cont, NULL);
                lv_img_set_src(img, imgs[row]);
                lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

                for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
                    cont = par_control_cont(pdata->cont_subpage, &labels[row][i], identifiers[row][i]);
                    lv_obj_align(cont, NULL, LV_ALIGN_IN_LEFT_MID, row_x[row] + (lv_obj_get_width(cont) + 2) * (i + 1),
                                 row_y[row]);
                }
            }

            for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
                for (size_t j = 0; j < i + 1; j++) {
                    img = lv_img_create(pdata->cont_subpage, NULL);
                    lv_img_set_src(img, &img_bar);
                    lv_obj_align(img, NULL, LV_ALIGN_IN_LEFT_MID, 20 + 72 * (i + 1), -74 - 7 * j);
                }
            }

            cont = bordered_cont(pdata->cont_subpage);
            lv_obj_set_style_local_border_width(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 3);
            lv_obj_set_size(cont, 160, 80);
            lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 114);

            img = lv_img_create(cont, NULL);
            lv_img_set_src(img, &img_icon_helmet_md);
            lv_obj_align(img, NULL, LV_ALIGN_IN_LEFT_MID, 8, 0);

            lv_obj_t *smaller_cont =
                par_control_cont(cont, &pdata->active_filtering.lbl_pressure_threshold, BTN_PRESSURE_THRESHOLD_MOD);
            lv_obj_align(smaller_cont, NULL, LV_ALIGN_IN_RIGHT_MID, -4, 0);

            img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_pipe);
            lv_obj_align(img, cont, LV_ALIGN_OUT_LEFT_MID, 0, 0);

            img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_pipe);
            lv_obj_align(img, cont, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
            break;
        }

        case PAGE_ROUTE_SETUP_MAINTENANCE: {
            lv_obj_t *cont, *img;
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_SETUP_MANUTENZIONE));

            const lv_img_dsc_t *img_dscs[] = {&img_warn, &img_stop, &img_stop};
            for (size_t i = 0; i < 3; i++) {
                img = lv_img_create(pdata->cont_subpage, NULL);
                lv_img_set_src(img, img_dscs[i]);
                lv_obj_align(img, NULL, LV_ALIGN_IN_TOP_MID, -80 + 80 * i, 56);
            }

            int16_t cont_y = 90;

            size_t esf_count = model_get_class_count(pmodel, DEVICE_CLASS_ELECTROSTATIC_FILTER);
            size_t ulf_count = model_get_mode_count(pmodel, DEVICE_MODE_UVC);

            if (ulf_count > 0) {
                cont = bordered_cont(pdata->cont_subpage);
                lv_obj_align(cont, NULL, LV_ALIGN_IN_TOP_LEFT, 32, cont_y);

                img = lv_img_create(cont, NULL);
                lv_img_set_src(img, &img_icon_uvc_md);
                lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

                lv_obj_t **labels[] = {
                    &pdata->filters.lbl_uvc_filters_hours_threshold_warn,
                    &pdata->filters.lbl_uvc_filters_hours_threshold_stop,
                };
                int identifiers[] = {
                    BTN_UVC_FILTERS_HOURS_THRESHOLD_WARN_MOD,
                    BTN_UVC_FILTERS_HOURS_THRESHOLD_STOP_MOD,
                };

                for (size_t i = 0; i < 2; i++) {
                    lv_obj_t *par_cont = par_control_cont(pdata->cont_subpage, labels[i], identifiers[i]);
                    lv_obj_align(par_cont, cont, LV_ALIGN_OUT_RIGHT_MID, 8 + 78 * i, 0);
                }

                lv_obj_t *par_cont = bordered_cont(pdata->cont_subpage);
                lv_obj_t *lbl      = lv_label_create(par_cont, NULL);
                lv_label_set_text_fmt(lbl, "%zu", ulf_count);
                lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_obj_align(par_cont, cont, LV_ALIGN_OUT_RIGHT_MID, 8 + 78 * 2, 0);

                cont_y += 72;
            }

            if (esf_count > 0) {
                cont = bordered_cont(pdata->cont_subpage);
                lv_obj_align(cont, NULL, LV_ALIGN_IN_TOP_LEFT, 32, cont_y);

                img = lv_img_create(cont, NULL);
                lv_img_set_src(img, &img_icon_esf_md);
                lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

                lv_obj_t **labels[] = {
                    &pdata->filters.lbl_esf_filters_hours_threshold_warn,
                    &pdata->filters.lbl_esf_filters_hours_threshold_stop,
                };
                int identifiers[] = {
                    BTN_ESF_FILTERS_HOURS_THRESHOLD_WARN_MOD,
                    BTN_ESF_FILTERS_HOURS_THRESHOLD_STOP_MOD,
                };

                for (size_t i = 0; i < 2; i++) {
                    lv_obj_t *par_cont = par_control_cont(pdata->cont_subpage, labels[i], identifiers[i]);
                    lv_obj_align(par_cont, cont, LV_ALIGN_OUT_RIGHT_MID, 8 + 78 * i, 0);
                }

                lv_obj_t *par_cont = bordered_cont(pdata->cont_subpage);
                lv_obj_t *lbl      = lv_label_create(par_cont, NULL);
                lv_label_set_text_fmt(lbl, "%zu", esf_count);
                lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_obj_align(par_cont, cont, LV_ALIGN_OUT_RIGHT_MID, 8 + 78 * 2, 0);

                cont_y += 72;
            }

            cont = bordered_cont(pdata->cont_subpage);
            lv_obj_align(cont, NULL, LV_ALIGN_IN_TOP_LEFT, 32, cont_y);

            img = lv_img_create(cont, NULL);
            lv_img_set_src(img, &img_icon_passive_filter_md);
            lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

            lv_obj_t **labels[] = {
                &pdata->filters.lbl_passive_filters_hours_threshold_warn,
                &pdata->filters.lbl_passive_filters_hours_threshold_stop,
                &pdata->filters.lbl_num_passive_filters,
            };
            int identifiers[] = {
                BTN_PASSIVE_FILTERS_HOURS_THRESHOLD_WARN_MOD,
                BTN_PASSIVE_FILTERS_HOURS_THRESHOLD_STOP_MOD,
                BTN_PASSIVE_FILTERS_NUM_MOD,
            };

            for (size_t i = 0; i < 3; i++) {
                lv_obj_t *par_cont = par_control_cont(pdata->cont_subpage, labels[i], identifiers[i]);
                lv_obj_align(par_cont, cont, LV_ALIGN_OUT_RIGHT_MID, 8 + 78 * i, 0);
            }

            break;
        }

        case PAGE_ROUTE_PRESSURE: {
            lv_obj_t *cont, *img;
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_PRESSIONE));

            img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_pipe);
            lv_obj_align(img, NULL, LV_ALIGN_IN_LEFT_MID, 0, -34);

            img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_pipe);
            lv_obj_align(img, NULL, LV_ALIGN_IN_RIGHT_MID, 0, -34);

            int ids[MAX_FAN_SPEED] = {
                BTN_PRESSURE_DIFFERENCE_0_MOD, BTN_PRESSURE_DIFFERENCE_1_MOD, BTN_PRESSURE_DIFFERENCE_2_MOD,
                BTN_PRESSURE_DIFFERENCE_3_MOD, BTN_PRESSURE_DIFFERENCE_4_MOD,
            };

            for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
                cont = par_control_cont(pdata->cont_subpage, &pdata->pressure.lbl_pressure_differences[i], ids[i]);
                lv_obj_align(cont, NULL, LV_ALIGN_IN_LEFT_MID, 45 + (lv_obj_get_width(cont) + 2) * (i), -34);
            }

            for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
                for (size_t j = 0; j < i + 1; j++) {
                    img = lv_img_create(pdata->cont_subpage, NULL);
                    lv_img_set_src(img, &img_bar);
                    lv_obj_align(img, NULL, LV_ALIGN_IN_LEFT_MID, 56 + 72 * (i), -74 - 7 * j);
                }
            }

            cont = par_control_cont(pdata->cont_subpage, &pdata->pressure.lbl_pressure_warn, BTN_PRESSURE_WARN_MOD);
            lv_obj_align(cont, NULL, LV_ALIGN_IN_LEFT_MID, 45, 100);

            img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_warn);
            lv_obj_align(img, cont, LV_ALIGN_OUT_TOP_MID, 0, -4);

            cont = par_control_cont(pdata->cont_subpage, &pdata->pressure.lbl_pressure_stop, BTN_PRESSURE_STOP_MOD);
            lv_obj_align(cont, NULL, LV_ALIGN_IN_LEFT_MID, 45 + 78, 100);

            img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_stop);
            lv_obj_align(img, cont, LV_ALIGN_OUT_TOP_MID, 0, -4);

            lv_obj_t *btn = lv_btn_create(pdata->cont_subpage, NULL);
            lv_obj_set_style_local_border_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
            lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, STYLE_FAINT_YELLOW);
            lv_obj_set_style_local_radius(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 24);
            lv_obj_set_size(btn, 180, 60);

            lv_obj_t *lbl = lv_label_create(btn, NULL);
            lv_label_set_text(lbl, view_intl_get_string(pmodel, STRINGS_INIZIA_CAL));
            lv_obj_align(lbl, NULL, LV_ALIGN_CENTER, 0, 0);

            view_register_default_callback(btn, BTN_START_CALIBRATION);

            lv_obj_align(btn, NULL, LV_ALIGN_IN_LEFT_MID, 210, 96);

            img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_calibration);
            lv_obj_align(img, btn, LV_ALIGN_OUT_TOP_MID, 0, -4);

            break;
        }

        case PAGE_ROUTE_HUMIDITY: {
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_UMIDITA));

            const lv_img_dsc_t *top_imgs[2]  = {&img_speed_1, &img_speed_2};
            const lv_img_dsc_t *left_imgs[2] = {&img_icon_temperature_humidity_sm, &img_icon_siphoning_immission_md};
            lv_obj_t          **labels[4]    = {
                &pdata->humidity_temperature.lbl_first_delta,
                &pdata->humidity_temperature.lbl_second_delta,
                &pdata->humidity_temperature.lbl_warn,
                &pdata->humidity_temperature.lbl_stop,
            };
            const int ids[] = {
                BTN_HUMTEMP_FIRST_DELTA_MOD,
                BTN_HUMTEMP_SECOND_DELTA_MOD,
                BTN_HUMTEMP_WARN_MOD,
                BTN_HUMTEMP_STOP_MOD,
            };
            size_t label_index = 0;

            for (size_t i = 0; i < 2; i++) {
                lv_obj_t *icon_cont = bordered_cont(pdata->cont_subpage);
                lv_obj_align(icon_cont, NULL, LV_ALIGN_IN_TOP_LEFT, 32, 100 + i * 72);

                lv_obj_t *img = lv_img_create(icon_cont, NULL);
                lv_img_set_src(img, left_imgs[i]);
                lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

                lv_obj_t *prev_cont = icon_cont;

                for (size_t j = 0; j < 2; j++) {
                    lv_obj_t *cont = par_control_cont(pdata->cont_subpage, labels[label_index], ids[label_index]);
                    lv_obj_align(cont, prev_cont, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
                    label_index++;

                    if (i == 0) {
                        lv_obj_t *img = lv_img_create(pdata->cont_subpage, NULL);
                        lv_img_set_src(img, top_imgs[j]);
                        lv_obj_align(img, cont, LV_ALIGN_OUT_TOP_MID, 0, -4);
                    }

                    prev_cont = cont;
                }
            }

            lv_obj_t *first_cont =
                par_control_cont(pdata->cont_subpage, &pdata->humidity_temperature.lbl_first_speed_raise,
                                 BTN_HUMTEMP_FIRST_CORRECTION_MOD);
            lv_obj_align(first_cont, NULL, LV_ALIGN_IN_TOP_LEFT, 254, 100);

            lv_obj_t *img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_warn);
            lv_obj_align(img, first_cont, LV_ALIGN_OUT_TOP_MID, 0, -16);

            lv_obj_t *second_cont =
                par_control_cont(pdata->cont_subpage, &pdata->humidity_temperature.lbl_second_speed_raise,
                                 BTN_HUMTEMP_SECOND_CORRECTION_MOD);
            lv_obj_align(second_cont, first_cont, LV_ALIGN_OUT_RIGHT_MID, 2, 0);

            img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_stop);
            lv_obj_align(img, second_cont, LV_ALIGN_OUT_TOP_MID, 0, -16);
            break;
        }

        case PAGE_ROUTE_TEMPERATURE: {
            pdata->cont_subpage = cont_subpage_create(view_intl_get_string(pmodel, STRINGS_TEMPERATURA));

            const lv_img_dsc_t *top_imgs[2]  = {&img_speed_1, &img_speed_2};
            const lv_img_dsc_t *left_imgs[2] = {&img_icon_temperature_humidity_sm, &img_icon_siphoning_immission_md};
            lv_obj_t          **labels[4]    = {
                &pdata->humidity_temperature.lbl_first_delta,
                &pdata->humidity_temperature.lbl_second_delta,
                &pdata->humidity_temperature.lbl_warn,
                &pdata->humidity_temperature.lbl_stop,
            };
            const int ids[] = {
                BTN_HUMTEMP_FIRST_DELTA_MOD,
                BTN_HUMTEMP_SECOND_DELTA_MOD,
                BTN_HUMTEMP_WARN_MOD,
                BTN_HUMTEMP_STOP_MOD,
            };
            size_t label_index = 0;

            for (size_t i = 0; i < 2; i++) {
                lv_obj_t *icon_cont = bordered_cont(pdata->cont_subpage);
                lv_obj_align(icon_cont, NULL, LV_ALIGN_IN_TOP_LEFT, 32, 100 + i * 72);

                lv_obj_t *img = lv_img_create(icon_cont, NULL);
                lv_img_set_src(img, left_imgs[i]);
                lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

                lv_obj_t *prev_cont = icon_cont;

                for (size_t j = 0; j < 2; j++) {
                    lv_obj_t *cont = par_control_cont(pdata->cont_subpage, labels[label_index], ids[label_index]);
                    lv_obj_align(cont, prev_cont, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
                    label_index++;

                    if (i == 0) {
                        lv_obj_t *img = lv_img_create(pdata->cont_subpage, NULL);
                        lv_img_set_src(img, top_imgs[j]);
                        lv_obj_align(img, cont, LV_ALIGN_OUT_TOP_MID, 0, -4);
                    }

                    prev_cont = cont;
                }
            }

            lv_obj_t *first_cont =
                par_control_cont(pdata->cont_subpage, &pdata->humidity_temperature.lbl_first_speed_raise,
                                 BTN_HUMTEMP_FIRST_CORRECTION_MOD);
            lv_obj_align(first_cont, NULL, LV_ALIGN_IN_TOP_LEFT, 254, 100);

            lv_obj_t *img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_warn);
            lv_obj_align(img, first_cont, LV_ALIGN_OUT_TOP_MID, 0, -16);

            lv_obj_t *second_cont =
                par_control_cont(pdata->cont_subpage, &pdata->humidity_temperature.lbl_second_speed_raise,
                                 BTN_HUMTEMP_SECOND_CORRECTION_MOD);
            lv_obj_align(second_cont, first_cont, LV_ALIGN_OUT_RIGHT_MID, 2, 0);

            img = lv_img_create(pdata->cont_subpage, NULL);
            lv_img_set_src(img, &img_stop);
            lv_obj_align(img, second_cont, LV_ALIGN_OUT_TOP_MID, 0, -16);
            break;
        }
    }

    update_menus(pmodel, pdata);
}


static void update_menus(model_t *pmodel, struct page_data *pdata) {
    switch (pdata->page_route) {
        case PAGE_ROUTE_MAIN_PAGE:
            break;

        case PAGE_ROUTE_VENTILATION:
            for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
                lv_label_set_text_fmt(pdata->ventilation.lbl_siphoning_percentages[i], "%03i%%",
                                      model_get_siphoning_percentage(pmodel, i));
                lv_label_set_text_fmt(pdata->ventilation.lbl_immission_percentages[i], "%03i%%",
                                      model_get_immission_percentage(pmodel, i));
            }
            lv_label_set_text_fmt(pdata->ventilation.lbl_start, "%03i sec",
                                  model_get_environment_cleaning_start_period(pmodel));
            lv_label_set_text_fmt(pdata->ventilation.lbl_finish, "%03i sec",
                                  model_get_environment_cleaning_finish_period(pmodel));
            break;

        case PAGE_ROUTE_ACTIVE_FILTERING:
            for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
                lv_label_set_text_fmt(pdata->active_filtering.lbl_uvc_filters[i], "%i",
                                      model_get_uvc_filters_for_speed(pmodel, i));
                lv_label_set_text_fmt(pdata->active_filtering.lbl_esf_filters[i], "%i",
                                      model_get_esf_filters_for_speed(pmodel, i));
            }
            lv_label_set_text_fmt(pdata->active_filtering.lbl_pressure_threshold, "%imB",
                                  model_get_pressure_threshold_mb(pmodel));
            break;

        case PAGE_ROUTE_SETUP_MAINTENANCE:
            lv_label_set_text_fmt(pdata->filters.lbl_uvc_filters_hours_threshold_warn, "%i",
                                  model_get_uvc_filters_hours_warning_threshold(pmodel));
            lv_label_set_text_fmt(pdata->filters.lbl_uvc_filters_hours_threshold_stop, "%i",
                                  model_get_uvc_filters_hours_stop_threshold(pmodel));
            lv_label_set_text_fmt(pdata->filters.lbl_esf_filters_hours_threshold_warn, "%i",
                                  model_get_esf_filters_hours_warning_threshold(pmodel));
            lv_label_set_text_fmt(pdata->filters.lbl_esf_filters_hours_threshold_stop, "%i",
                                  model_get_esf_filters_hours_stop_threshold(pmodel));
            lv_label_set_text_fmt(pdata->filters.lbl_passive_filters_hours_threshold_warn, "%i",
                                  model_get_passive_filters_hours_warning_threshold(pmodel));
            lv_label_set_text_fmt(pdata->filters.lbl_passive_filters_hours_threshold_stop, "%i",
                                  model_get_passive_filters_hours_stop_threshold(pmodel));
            lv_label_set_text_fmt(pdata->filters.lbl_num_passive_filters, "%i", model_get_num_passive_filters(pmodel));
            break;

        case PAGE_ROUTE_PRESSURE:
            for (size_t i = 0; i < MAX_FAN_SPEED; i++) {
                lv_label_set_text_fmt(pdata->pressure.lbl_pressure_differences[i], "%i",
                                      model_get_pressure_difference(pmodel, i));
            }
            lv_label_set_text_fmt(pdata->pressure.lbl_pressure_warn, "%i%%",
                                  model_get_pressure_difference_deviation_warn(pmodel));
            lv_label_set_text_fmt(pdata->pressure.lbl_pressure_stop, "%i%%",
                                  model_get_pressure_difference_deviation_stop(pmodel));
            break;

        case PAGE_ROUTE_HUMIDITY:
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_first_delta, "%i%%",
                                  model_get_first_humidity_delta(pmodel));
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_second_delta, "%i%%",
                                  model_get_second_humidity_delta(pmodel));
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_first_speed_raise, "%i%%",
                                  model_get_first_humidity_speed_raise(pmodel));
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_second_speed_raise, "%i%%",
                                  model_get_second_humidity_speed_raise(pmodel));
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_warn, "%i%%", model_get_humidity_warn(pmodel));
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_stop, "%i%%", model_get_humidity_stop(pmodel));
            break;

        case PAGE_ROUTE_TEMPERATURE:
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_first_delta, "%i %s",
                                  model_get_first_temperature_delta(pmodel), model_get_degrees_symbol(pmodel));
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_second_delta, "%i %s",
                                  model_get_second_temperature_delta(pmodel), model_get_degrees_symbol(pmodel));
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_first_speed_raise, "%i%%",
                                  model_get_first_temperature_speed_raise(pmodel));
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_second_speed_raise, "%i%%",
                                  model_get_second_temperature_speed_raise(pmodel));
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_warn, "%i %s", model_get_temperature_warn(pmodel),
                                  model_get_degrees_symbol(pmodel));
            lv_label_set_text_fmt(pdata->humidity_temperature.lbl_stop, "%i %s", model_get_temperature_stop(pmodel),
                                  model_get_degrees_symbol(pmodel));
            break;

        default:
            break;
    }
}


static uint16_t modify_parameter(uint16_t value, int16_t mod, uint16_t min, uint16_t max) {
    if (mod > 0) {
        if (value + mod > max) {
            return max;
        } else {
            return value + mod;
        }
    } else {
        if ((int16_t)value + mod < min) {
            return min;
        } else {
            return value + mod;
        }
    }
}

static uint16_t modify_percentage(uint16_t value, int16_t mod) {
    return modify_parameter(value, mod * 5, 0, 100);
}


const pman_page_t page_main = {
    .create        = create_page,
    .destroy       = destroy_page,
    .open          = open_page,
    .close         = close_page,
    .process_event = process_page_event,
};