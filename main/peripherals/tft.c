#include <driver/ledc.h>
#include "hardwareprofile.h"
#include "esp_log.h"


static const char *TAG = "TFT";


void tft_init(void) {
    // Configure LED (Backlight) pin as PWM for Brightness control.
    const ledc_channel_config_t LCD_backlight_channel = {
        .gpio_num   = HAP_BACKLIGHT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0,
    };
    const ledc_timer_config_t LCD_backlight_timer = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num       = LEDC_TIMER_0,
        .freq_hz         = 5000,
        .clk_cfg         = LEDC_AUTO_CLK,
    };

    ESP_ERROR_CHECK(ledc_timer_config(&LCD_backlight_timer));
    ESP_ERROR_CHECK(ledc_channel_config(&LCD_backlight_channel));
    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    ESP_LOGD(TAG, "Initialized");
}


void tft_backlight_set(uint8_t brightness_percent) {
    // Check input paramters
    if (brightness_percent > 100) {
        brightness_percent = 100;
    }
    ESP_LOGI(TAG, "Setting backlight to %i%%", brightness_percent);

    uint32_t duty_cycle = (1023 * brightness_percent) / 100;     // LEDC resolution set to 10bits, thus: 100% = 1023
    // ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty_cycle));
    // ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
    ESP_ERROR_CHECK(
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty_cycle, 1000, LEDC_FADE_NO_WAIT));
}