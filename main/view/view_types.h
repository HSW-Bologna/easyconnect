#ifndef VIEW_TYPES_H_INCLUDED
#define VIEW_TYPES_H_INCLUDED

#include <stdint.h>

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


typedef enum {
    VIEW_CONTROLLER_MESSAGE_NOTHING = 0,
    VIEW_CONTROLLER_MESSAGE_CODE_CONFIGURE_DEVICE_ADDRESS,
    VIEW_CONTROLLER_MESSAGE_CODE_AUTOMATIC_COMMISSIONING,
} view_controller_message_code_t;

typedef struct {
    view_controller_message_code_t code;

    union {
        uint8_t address;
    };
} view_controller_message_t;

typedef struct {
    view_page_command_t       vmsg;
    view_controller_message_t cmsg;
} view_message_t;


#endif