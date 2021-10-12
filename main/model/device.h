#ifndef DEVICE_H_INCLUDED
#define DEVICE_H_INCLUDED


#include <stdint.h>


#define DEVICE_CLASS_DEFAULT     DEVICE_CLASS_LIGHT_1
#define NUMBER_OF_DEVICE_CLASSES 3

#define DEVICE_CLASS(class)    (((class) >> 8) & 0xFF)
#define DEVICE_SUBCLASS(class) ((class) & 0xFF)


typedef enum {
    DEVICE_STATUS_NOT_CONFIGURED = 0,
    DEVICE_STATUS_OK,
    DEVICE_STATUS_CONFIGURED,
    DEVICE_STATUS_COMMUNICATION_ERROR,
} device_status_t;


typedef enum {
    DEVICE_CLASS_LIGHT_1              = 0x01,
    DEVICE_CLASS_LIGHT_2              = 0x02,
    DEVICE_CLASS_LIGHT_3              = 0x03,
    DEVICE_CLASS_ELECTROSTATIC_FILTER = 0x40,
    DEVICE_CLASS_ULTRAVIOLET_FILTER   = 0x41,
    DEVICE_CLASS_SIPHONING_FAN        = 0x80,
    DEVICE_CLASS_IMMISSION_FAN        = 0x81,
} device_class_t;


typedef struct {
    uint8_t status;
    uint8_t address;
    uint16_t class;
    uint16_t serial_number;
} device_t;


#endif