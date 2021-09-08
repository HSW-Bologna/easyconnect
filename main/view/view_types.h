#ifndef VIEW_TYPES_H_INCLUDED
#define VIEW_TYPES_H_INCLUDED

typedef enum {
    VIEW_EVENT_CODE_LVGL,
    VIEW_EVENT_CODE_OPEN,
    VIEW_EVENT_CODE_TIMER,
} view_event_code_t;

typedef struct {
    view_event_code_t code;
    union {
        int timer_id;
        struct {
            int lv_event;
            struct {
                int id, number;
            } data;
        };
        int sensor;
    };
} view_event_t;

typedef enum {
    VIEW_COMMAND_CODE_NOTHING = 0,
    VIEW_COMMAND_CODE_REBASE,
    VIEW_COMMAND_CODE_CHANGE_PAGE,
    VIEW_COMMAND_CODE_CHANGE_PAGE_EXTRA,
    VIEW_COMMAND_CODE_BACK,
    VIEW_COMMAND_CODE_UPDATE,
} view_page_command_code_t;

typedef struct {
    view_page_command_code_t code;

    union {
        struct {
            const void *page;
            void *      extra;
        };
    };
} view_page_command_t;

typedef struct {

} view_controller_command_t;

typedef struct {
    view_page_command_t       vmsg;
    view_controller_command_t cmsg;
} view_message_t;


#endif