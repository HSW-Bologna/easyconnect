#ifndef STYLE_H_INCLUDED
#define STYLE_H_INCLUDED

#include "lvgl.h"


#define STYLE_FAINT_YELLOW lv_color_make(0xff, 0xcb, 0x4f)
#define STYLE_FAINT_CYAN   lv_color_make(0x81, 0xc4, 0xfa)
#define STYLE_WHITE        lv_color_make(0x00, 0x00, 0x00)
#define STYLE_RED          lv_color_make(0xF0, 0x00, 0x00)


void style_init(void);


extern lv_style_t style_transparent_cont;


#endif
