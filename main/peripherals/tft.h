#ifndef TFT_H_INCLUDED
#define TFT_H_INCLUDED


#include <stdint.h>


void tft_init(void);
void tft_backlight_set(uint8_t brightness_percent);


#endif
