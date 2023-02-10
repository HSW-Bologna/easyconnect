#ifndef STYLE_H_INCLUDED
#define STYLE_H_INCLUDED

#include "lvgl.h"


#define STYLE_FAINT_YELLOW lv_color_make(0xff, 0xcb, 0x4f)
#define STYLE_FAINT_CYAN   lv_color_make(0x81, 0xc4, 0xfa)


void style_init(void);


extern lv_style_t style_transparent_cont;


#endif