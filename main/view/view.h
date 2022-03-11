#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED

#include "lvgl.h"
#include "view_types.h"
#include "model/model.h"
#include "gel/pagemanager/page_manager.h"


lv_indev_t *view_init(void (*flush_cb)(struct _disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p),
                      bool (*read_cb)(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data));
void        view_event(view_event_t event);
int         view_get_next_message(model_t *model, view_message_t *msg, view_event_t *eventcopy);
void        view_process_message(view_page_command_t vmsg, model_t *model);
void        view_destroy_all(void *data, void *extra);
void        view_close_all(void *data);
void        view_start(model_t *model);
void        view_register_default_callback(lv_obj_t *obj, int id);
void        view_register_default_callback_number(lv_obj_t *obj, int id, int number);
lv_task_t  *view_register_periodic_task(size_t period, lv_task_prio_t prio, int id);
void        view_blood_pact(lv_obj_t *obj1, lv_obj_t *obj2);
void        view_new_signal_handler(lv_obj_t *obj, lv_signal_cb_t cb);


extern const pman_page_t page_main, page_settings, page_tech_settings, page_devices, page_commissioning,
    page_manual_device_config, page_device_info, page_splash, page_device_search, page_automatic_device_config,
    page_datetime, page_date, page_time;


#endif