#include "controller.h"
#include "model/model.h"
#include "view/view.h"
#include "peripherals/modbus.h"
#include "log.h"
#include "utils/utils.h"
#include "gel/timer/timecheck.h"


void controller_init(model_t *model) {
    (void)model;
    log_init();
    view_start(model);
}


void controller_manage_message(model_t *pmodel, view_controller_message_t *msg) {
    switch (msg->code) {
        case VIEW_CONTROLLER_MESSAGE_NOTHING:
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_AUTOMATIC_COMMISSIONING:
            modbus_automatic_commissioning();
            break;

        case VIEW_CONTROLLER_MESSAGE_CODE_CONFIGURE_DEVICE_ADDRESS:
            modbus_configure_device_address(msg->address);
            break;
    }
}


void controller_manage(model_t *pmodel) {
    static unsigned long timestamp = 0;
    static int           flag      = 0;
    static int           first     = 1;

    if (first) {
        //modbus_configure_device_address(8);
        first     = 0;
        timestamp = get_millis();
    }

    if (modbus_automatic_commissioning_done(0) && is_expired(timestamp, get_millis(), 1000UL)) {
        //modbus_read_device_inputs(8);
        //modbus_set_device_output(8, flag);
        flag = !flag;

        timestamp = get_millis();
    }
}