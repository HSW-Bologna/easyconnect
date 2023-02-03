#ifndef VIEW_COMMON_H_INCLUDED
#define VIEW_COMMON_H_INCLUDED

#include "lvgl.h"
#include "model/model.h"


typedef struct {
    void (*setter)(model_t *, uint16_t);
    uint16_t    initial;
    uint16_t    max;
    const char *title;
    const char *um;
} slider_parameter_t;


lv_obj_t           *view_common_back_button(int id);
lv_obj_t           *view_common_title(int id, const char *string, lv_obj_t **label);
lv_obj_t           *view_common_address_picker(lv_obj_t *root, int id);
lv_obj_t           *view_common_default_menu_button(lv_obj_t *root, const char *text, int id);
lv_obj_t           *view_common_menu_button(lv_obj_t *root, const char *text, size_t width, int id);
lv_obj_t           *view_common_limit_address_picker(model_t *pmodel, lv_obj_t *dd);
lv_obj_t           *view_common_min_sec_rollers(lv_obj_t *parent, int visible, lv_obj_t **rethour, lv_obj_t **retmin);
void                view_common_roller_set_option_from_to(lv_obj_t *roller, int from, int to, int mode);
void                view_common_set_hidden(lv_obj_t *obj, int hidden);
void                view_common_get_class_string(uint16_t class, char *string, size_t len);
slider_parameter_t *view_common_slider_parameter_create(const char *title, const char *um, uint16_t value, uint16_t max,
                                                        void (*setter)(model_t *pmodel, uint16_t));


#endif