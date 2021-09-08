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


extern pman_page_t page_main;


#endif