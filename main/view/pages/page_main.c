#include <stdlib.h>
#include "view/view.h"
#include "model/model.h"


LV_IMG_DECLARE(img_lampadina_1_on);
LV_IMG_DECLARE(img_lampadina_1_off);
LV_IMG_DECLARE(img_lampadina_2_on);
LV_IMG_DECLARE(img_lampadina_2_off);
LV_IMG_DECLARE(img_lampadina_3_on);
LV_IMG_DECLARE(img_lampadina_3_off);


struct page_data {};


static void *create_page(model_t *model, void *extra) {
    struct page_data *data = malloc(sizeof(struct page_data));
    return data;
}


static void open_page(model_t *model, void *arg) {
    struct page_data *data = arg;

    lv_obj_t *btn1 = lv_imgbtn_create(lv_scr_act(), NULL);
    lv_imgbtn_set_src(btn1, LV_BTN_STATE_CHECKED_RELEASED, &img_lampadina_1_on);
    lv_imgbtn_set_src(btn1, LV_BTN_STATE_PRESSED, &img_lampadina_1_on);
    lv_imgbtn_set_src(btn1, LV_BTN_STATE_RELEASED, &img_lampadina_1_off);
    lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_imgbtn_set_checkable(btn1, 1);

    lv_obj_t *btn2 = lv_imgbtn_create(lv_scr_act(), NULL);
    lv_imgbtn_set_src(btn2, LV_BTN_STATE_CHECKED_RELEASED, &img_lampadina_2_on);
    lv_imgbtn_set_src(btn2, LV_BTN_STATE_PRESSED, &img_lampadina_2_on);
    lv_imgbtn_set_src(btn2, LV_BTN_STATE_RELEASED, &img_lampadina_2_off);
    lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_imgbtn_set_checkable(btn2, 1);

    lv_obj_t *btn3 = lv_imgbtn_create(lv_scr_act(), NULL);
    lv_imgbtn_set_src(btn3, LV_BTN_STATE_CHECKED_RELEASED, &img_lampadina_3_on);
    lv_imgbtn_set_src(btn3, LV_BTN_STATE_PRESSED, &img_lampadina_3_on);
    lv_imgbtn_set_src(btn3, LV_BTN_STATE_RELEASED, &img_lampadina_3_off);
    lv_obj_align(btn3, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_imgbtn_set_checkable(btn3, 1);

}


static view_message_t process_page_event(model_t *model, void *arg, view_event_t event) {
    struct page_data *data = arg;
    view_message_t    msg  = {0};

    return msg;
}


pman_page_t page_main = {
    .create        = create_page,
    .destroy       = view_destroy_all,
    .open          = open_page,
    .close         = view_close_all,
    .process_event = process_page_event,
};