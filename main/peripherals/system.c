#include "driver/i2c.h"
#include "esp_log.h"

static const char *TAG = "System";


int system_i2c_init(void) {
    int       port     = I2C_NUM_0;
    int       sda_pin  = GPIO_NUM_18;
    int       scl_pin  = GPIO_NUM_19;
    int       speed_hz = 400000;

    ESP_LOGI(TAG, "Initializing I2C master port %d...", port);
    ESP_LOGI(TAG, "SDA pin: %d, SCL pin: %d, Speed: %d (Hz)", sda_pin, scl_pin, speed_hz);

    i2c_config_t conf = {
        .mode             = I2C_MODE_MASTER,
        .sda_io_num       = sda_pin,
        .sda_pullup_en    = GPIO_PULLUP_ENABLE,
        .scl_io_num       = scl_pin,
        .scl_pullup_en    = GPIO_PULLUP_ENABLE,
        .master.clk_speed = speed_hz,
    };

    ESP_LOGI(TAG, "Setting I2C master configuration...");
    ESP_ERROR_CHECK(i2c_param_config(port, &conf));

    ESP_LOGI(TAG, "Installing I2C master driver...");
    ESP_ERROR_CHECK(i2c_driver_install(port, I2C_MODE_MASTER, 0, 0 /*I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE */,
                             0 /* intr_alloc_flags */));
    ESP_ERROR_CHECK(i2c_set_timeout(port, 0xFFFFF));

    return ESP_OK;
}