#ifndef VIEW_COMMON_H_INCLUDED
#define VIEW_COMMON_H_INCLUDED

#include "lvgl.h"

lv_obj_t *view_common_back_button(int id);
lv_obj_t *view_common_title(int id, char *string, lv_obj_t **label);

#endif