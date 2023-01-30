#ifndef DEVICE_H_INCLUDED
#define DEVICE_H_INCLUDED


#include "address_map.h"
#include <stdint.h>


#define DEVICE_CLASS_DEFAULT     DEVICE_CLASS_LIGHT_1
#define NUMBER_OF_DEVICE_CLASSES 3

#define MAX_NUM_MESSAGES 2

#define MODBUS_MAX_DEVICES 100

#define DEVICE_CLASS(class)    (((class) >> 8) & 0xFF)
#define DEVICE_SUBCLASS(class) ((class) & 0xFF)

#define DEVICE_LIST(name) device_t name[MODBUS_MAX_DEVICES]


typedef enum {
    DEVICE_STATUS_NOT_CONFIGURED = 0,      // Device address is unused
    DEVICE_STATUS_CONFIGURED,              // Device is configured
    DEVICE_STATUS_OK,                      // Device has been queried and communicates correctly
    DEVICE_STATUS_FOUND,                   // Device is not configured, but is on the network nonetheless
    DEVICE_STATUS_COMMUNICATION_ERROR,     // There is a communication issue with the device
} device_status_t;


#define DEVICE_CLASS_LIGHT_1              (CLASS(DEVICE_MODE_LIGHT, DEVICE_GROUP_1))
#define DEVICE_CLASS_LIGHT_2              (CLASS(DEVICE_MODE_LIGHT, DEVICE_GROUP_2))
#define DEVICE_CLASS_LIGHT_3              (CLASS(DEVICE_MODE_LIGHT, DEVICE_GROUP_3))
#define DEVICE_CLASS_ULTRAVIOLET_FILTER   (CLASS(DEVICE_MODE_UVC, DEVICE_GROUP_1))
#define DEVICE_CLASS_ELECTROSTATIC_FILTER (CLASS(DEVICE_MODE_ESF, DEVICE_GROUP_1))
#define DEVICE_CLASS_GAS                  (CLASS(DEVICE_MODE_GAS, DEVICE_GROUP_1))
#define DEVICE_CLASS_PRESSURE_SAFETY      (CLASS(DEVICE_MODE_PRESSURE_SAFETY, DEVICE_GROUP_1))
#define DEVICE_CLASS_SIPHONING_FAN        (CLASS(DEVICE_MODE_SIPHONING_FAN, DEVICE_GROUP_1))
#define DEVICE_CLASS_IMMISSION_FAN        (CLASS(DEVICE_MODE_IMMISSION_FAN, DEVICE_GROUP_1))


typedef struct {
    uint8_t  status;
    uint8_t  address;
    uint16_t event_count;
    uint16_t firmware_version;
    uint16_t class;
    uint16_t serial_number;
    uint16_t alarms;

    union {
        int16_t pressure;
    };
} device_t;


void          device_list_init(device_t *devices);
uint8_t       device_list_get_prev_device_address(device_t *devices, uint8_t next);
uint8_t       device_list_get_next_configured_device_address(device_t *devices, uint8_t previous);
uint8_t       device_list_get_next_found_device_address(device_t *devices, uint8_t previous);
int           device_list_is_address_configured(device_t *devices, uint8_t address);
uint8_t       device_list_get_available_address(device_t *devices, uint8_t previous);
uint8_t       device_list_get_next_device_address_by_class(device_t *devices, uint8_t previous, uint16_t class);
int           device_list_configure_device(device_t *devices, uint8_t address);
int           device_list_device_found(device_t *devices, uint8_t address);
address_map_t device_list_get_address_map(device_t *devices);
address_map_t device_list_get_error_map(device_t *devices);
void          device_list_delete_device(device_t *devices, uint8_t address);
uint8_t       device_list_set_device_error(device_t *devices, uint8_t address, int error);
uint8_t       device_list_set_device_alarms(device_t *devices, uint8_t address, uint16_t alarms);
size_t        device_list_get_configured_devices(device_t *devices);
device_t      device_list_get_device(device_t *devices, uint8_t address);
device_t     *device_list_get_device_mut(device_t *devices, uint8_t address);
void          device_list_set_device_sn(device_t *devices, uint8_t address, uint16_t serial_number);
uint8_t       device_list_is_there_an_alarm(device_t *devices);
uint8_t       device_list_is_there_any_alarm_for_class(device_t *devices, uint16_t class);
uint8_t       device_list_is_class_alarms_on(device_t *devices, uint16_t class, uint8_t alarms);


#endif