#ifndef DEVICE_H_INCLUDED
#define DEVICE_H_INCLUDED


#include "address_map.h"
#include <stdint.h>


#define DEVICE_CLASS_DEFAULT     DEVICE_CLASS_LIGHT_1
#define NUMBER_OF_DEVICE_CLASSES 3

#define MODBUS_MAX_DEVICES 255

#define DEVICE_CLASS(class)    (((class) >> 8) & 0xFF)
#define DEVICE_SUBCLASS(class) ((class) & 0xFF)

#define DEVICE_LIST(name) device_t name[MODBUS_MAX_DEVICES]


typedef enum {
    DEVICE_STATUS_NOT_CONFIGURED = 0,
    DEVICE_STATUS_OK,
    DEVICE_STATUS_CONFIGURED,
    DEVICE_STATUS_COMMUNICATION_ERROR,
} device_status_t;


typedef enum {
    DEVICE_CLASS_LIGHT_1              = 0x0101,
    DEVICE_CLASS_LIGHT_2              = 0x0102,
    DEVICE_CLASS_LIGHT_3              = 0x0103,
    DEVICE_CLASS_ULTRAVIOLET_FILTER   = 0x0201,
    DEVICE_CLASS_ELECTROSTATIC_FILTER = 0x0300,
    DEVICE_CLASS_SIPHONING_FAN        = 0x0501,
    DEVICE_CLASS_IMMISSION_FAN        = 0x0502,
} device_class_t;


typedef struct {
    uint8_t  status;
    uint8_t  address;
    uint16_t hardware_model;
    uint16_t firmware_version;
    uint16_t class;
    uint16_t serial_number;
    uint16_t alarms;
} device_t;


void          device_list_init(device_t *devices);
uint8_t       device_list_get_prev_device_address(device_t *devices, uint8_t next);
uint8_t       device_list_get_next_device_address(device_t *devices, uint8_t previous);
int           device_list_is_address_configured(device_t *devices, uint8_t address);
uint8_t       device_list_get_available_address(device_t *devices, uint8_t previous);
uint8_t       device_list_get_next_device_address_by_class(device_t *devices, uint8_t previous, device_class_t class);
int           device_list_new_device(device_t *devices, uint8_t address);
address_map_t device_list_get_address_map(device_t *devices);
address_map_t device_list_get_error_map(device_t *devices);
void          device_list_delete_device(device_t *devices, uint8_t address);
void          device_list_get_device(device_t *devices, device_t *device, uint8_t address);
void          device_list_set_device_error(device_t *devices, uint8_t address, int error);
void          device_list_set_device_sn(device_t *devices, uint8_t address, uint16_t serial_number);
void          device_list_set_device_class(device_t *devices, uint8_t address, uint16_t class);
uint8_t       device_list_set_device_alarms(device_t *devices, uint8_t address, uint16_t alarms);
size_t        device_list_get_configured_devices(device_t *devices);


#endif