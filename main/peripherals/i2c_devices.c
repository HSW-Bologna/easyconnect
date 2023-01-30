#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_ports/esp-idf/esp_idf_i2c_port.h"
#include "i2c_devices/temperature/SHT21/sht21.h"
#include "i2c_devices/temperature/SHTC3/shtc3.h"
#include "i2c_devices/rtc/DS1307/ds1307.h"
#include "i2c_devices.h"
#include "esp_log.h"



static void delay_ms(unsigned long ms);


static const char *TAG = "I2C devices";


i2c_driver_t sht21_driver = {
    .device_address = SHT21_DEFAULT_ADDRESS,
    .i2c_transfer   = esp_idf_i2c_port_transfer,
    .delay_ms       = delay_ms,
};


i2c_driver_t shtc3_driver = {
    .device_address = SHTC3_DEFAULT_ADDRESS,
    .i2c_transfer   = esp_idf_i2c_port_transfer,
    .delay_ms       = delay_ms,
};


i2c_driver_t rtc_driver = {
    .device_address = DS1307_DEFAULT_ADDRESS,
    .i2c_transfer   = esp_idf_i2c_port_transfer,
};


static void delay_ms(unsigned long ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}
