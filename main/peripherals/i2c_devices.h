#ifndef I2C_DEVICES_H_INCLUDED
#define I2C_DEVICES_H_INCLUDED


#include "i2c_devices/temperature/SHT21/sht21.h"


extern i2c_driver_t sht21_driver;
extern i2c_driver_t shtc3_driver;
extern i2c_driver_t rtc_driver;


#endif