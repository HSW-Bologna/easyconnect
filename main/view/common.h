#ifndef VIEW_COMMON_H_INCLUDED
#define VIEW_COMMON_H_INCLUDED

#include "lvgl.h"
#include "model/model.h"


lv_obj_t *view_common_back_button(int id);
lv_obj_t *view_common_title(int id, char *string, lv_obj_t **label);
lv_obj_t *view_common_address_picker(lv_obj_t *root, int id);
lv_obj_t *view_common_menu_button(lv_obj_t *root, char *text, int id);
lv_obj_t *view_common_limit_address_picker(model_t *pmodel, lv_obj_t *dd);

#endif