#ifndef VIEW_TYPES_H_INCLUDED
#define VIEW_TYPES_H_INCLUDED

#include <stdint.h>
#include "model/device.h"

typedef enum {
    VIEW_EVENT_CODE_LVGL,
    VIEW_EVENT_CODE_OPEN,
    VIEW_EVENT_CODE_TIMER,
    VIEW_EVENT_CODE_DEVICE_CONFIGURED,
    VIEW_EVENT_CODE_DEVICE_UPDATE,
    VIEW_EVENT_CODE_DEVICE_MESSAGES,
    VIEW_EVENT_CODE_DEVICE_ALARM,
    VIEW_EVENT_CODE_DEVICE_SEARCH_DONE,
    VIEW_EVENT_CODE_DEVICE_LISTENING_DONE,
    VIEW_EVENT_CODE_STATE_UPDATE,
    VIEW_EVENT_CODE_ANCILLARY_DATA_UPDATE,
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
        struct {
            uint8_t address;
            int     error;
        };
        struct {
            size_t num_messages;
            char  *messages[MAX_NUM_MESSAGES];
        };
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
            void const *page;
            void       *extra;
        };
    };
} view_page_command_t;


typedef enum {
    VIEW_CONTROLLER_MESSAGE_NOTHING = 0,
    VIEW_CONTROLLER_MESSAGE_CODE_SET_RTC,
    VIEW_CONTROLLER_MESSAGE_CODE_AUTOMATIC_COMMISSIONING,
    VIEW_CONTROLLER_MESSAGE_CODE_SAVE,
    VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_LIGHTS,
    VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FAN,
    VIEW_CONTROLLER_MESSAGE_CODE_CONTROL_FILTER,
    VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_SCAN,
    VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_INFO,
    VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_MESSAGES,
    VIEW_CONTROLLER_MESSAGE_CODE_DEVICE_INFO_AND_SAVE,
    VIEW_CONTROLLER_MESSAGE_CODE_STOP_CURRENT_OPERATION,
    VIEW_CONTROLLER_MESSAGE_CODE_REFRESH_DEVICES,
    VIEW_CONTROLLER_MESSAGE_CODE_SET_FAN_SPEED,
} view_controller_message_code_t;


typedef struct {
    view_controller_message_code_t code;

    union {
        struct {
            uint8_t address;
            uint16_t class;
        };
        uint8_t  light_value;
        uint8_t  speed;
        uint16_t expected_devices;
    };
} view_controller_message_t;

typedef struct {
    view_page_command_t       vmsg;
    view_controller_message_t cmsg;
} view_message_t;


#endif