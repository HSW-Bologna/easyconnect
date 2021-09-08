#include <assert.h>
#include "lvgl.h"
#include "view.h"
#include "gel/pagemanager/page_manager.h"
#include "gel/collections/queue.h"
#include "view_types.h"
#include "model/model.h"


QUEUE_DECLARATION(event_queue, view_event_t, 32);
QUEUE_DEFINITION(event_queue, view_event_t);

#define DISP_BUF_SIZE (LV_HOR_RES_MAX * 40)

static void change_page_extra(model_t *model, const pman_page_t *page, void *extra);
static void change_page(model_t *model, const pman_page_t *page);

static struct event_queue q;
static page_manager_t     pman;
static lv_indev_t *       touch_dev = NULL;


lv_indev_t *view_init(void (*flush_cb)(struct _disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p),
                      bool (*read_cb)(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data)) {
    lv_init();

    static lv_color_t    buf1[DISP_BUF_SIZE];
    static lv_color_t    buf2[DISP_BUF_SIZE];
    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = flush_cb;
    disp_drv.buffer   = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = read_cb;
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);

    pman_init(&pman);
    event_queue_init(&q);

    touch_dev = indev;
    return indev;
}

int view_get_next_message(model_t *model, view_message_t *msg, view_event_t *eventcopy) {
    view_event_t event;
    int          found = 0;

    if (!event_queue_is_empty(&q)) {
        event_queue_dequeue(&q, &event);
        found = 1;
    }

    if (found) {
        *msg = pman.current_page.process_event(model, pman.current_page.data, event);
        if (eventcopy)
            *eventcopy = event;
    }

    return found;
}

void view_process_message(view_page_command_t vmsg, model_t *model) {
    switch (vmsg.code) {
        case VIEW_COMMAND_CODE_CHANGE_PAGE:
            change_page(model, vmsg.page);
            break;

        case VIEW_COMMAND_CODE_CHANGE_PAGE_EXTRA:
            change_page_extra(model, vmsg.page, vmsg.extra);
            break;

        case VIEW_COMMAND_CODE_BACK:
            lv_indev_wait_release(touch_dev);
            pman_back(&pman, model);
            event_queue_init(&q);
            view_event((view_event_t){.code = VIEW_EVENT_CODE_OPEN});
            break;

        case VIEW_COMMAND_CODE_REBASE:
            assert(vmsg.page != NULL);
            lv_indev_wait_release(touch_dev);
            pman_rebase_page(&pman, model, *(pman_page_t *)vmsg.page);
            event_queue_init(&q);
            view_event((view_event_t){.code = VIEW_EVENT_CODE_OPEN});
            break;

        case VIEW_COMMAND_CODE_UPDATE:
            pman_page_update(&pman, model);
            break;

        default:
            /* do nothing */
            break;
    }
}

void view_event(view_event_t event) {
    event_queue_enqueue(&q, &event);
}

void view_destroy_all(void *data, void *extra) {
    free(data);
    free(extra);
}

void view_close_all(void *data) {
    lv_obj_clean(lv_scr_act());
}

static void page_event_cb(lv_obj_t *obj, lv_event_t event) {
    view_event_t myevent;
    myevent.code            = VIEW_EVENT_CODE_LVGL;
    lv_obj_user_data_t data = lv_obj_get_user_data(obj);
    myevent.data.id         = data.id;
    myevent.data.number     = data.number;
    myevent.lv_event        = event;
    view_event(myevent);
}

static void periodic_task(lv_task_t *task) {
    view_event((view_event_t){.code = VIEW_EVENT_CODE_TIMER, .timer_id = (int)(uintptr_t)task->user_data});
}

lv_task_t *view_register_periodic_task(size_t period, lv_task_prio_t prio, int id) {
    return lv_task_create(periodic_task, period, prio, (void *)(uintptr_t)id);
}


void view_start(model_t *model) {
    change_page(model, &page_main);
}


void view_register_default_callback(lv_obj_t *obj, int id) {
    lv_obj_user_data_t data = {.id = id, .number = 0};
    lv_obj_set_user_data(obj, data);
    lv_obj_set_event_cb(obj, page_event_cb);
}

void view_register_default_callback_number(lv_obj_t *obj, int id, int number) {
    lv_obj_user_data_t data = {.id = id, .number = number};
    lv_obj_set_user_data(obj, data);
    lv_obj_set_event_cb(obj, page_event_cb);
}

static void change_page_extra(model_t *model, const pman_page_t *page, void *extra) {
    assert(page != NULL);
    lv_indev_wait_release(touch_dev);
    pman_change_page_extra(&pman, model, *page, extra);
    event_queue_init(&q); /* discard all previous events when page changes */
    view_event((view_event_t){.code = VIEW_EVENT_CODE_OPEN});
}

static void change_page(model_t *model, const pman_page_t *page) {
    change_page_extra(model, page, NULL);
}
