#ifndef VIEW_COMMON_H_INCLUDED
#define VIEW_COMMON_H_INCLUDED

#include "lvgl.h"
#include "model/model.h"


lv_obj_t *view_common_back_button(int id);
lv_obj_t *view_common_title(int id, char *string, lv_obj_t **label);
lv_obj_t *view_common_address_picker(lv_obj_t *root, int id);
lv_obj_t *view_common_default_menu_button(lv_obj_t *root, char *text, int id);
lv_obj_t *view_common_menu_button(lv_obj_t *root, char *text, size_t width, int id);
lv_obj_t *view_common_limit_address_picker(model_t *pmodel, lv_obj_t *dd);
lv_obj_t *view_common_min_sec_rollers(lv_obj_t *parent, int visible, lv_obj_t **rethour, lv_obj_t **retmin);
void      view_common_roller_set_option_from_to(lv_obj_t *roller, int from, int to, int mode);
void      view_common_set_hidden(lv_obj_t *obj, int hidden);
void      view_common_get_class_string(device_class_t class, char *string, size_t len);


#endif