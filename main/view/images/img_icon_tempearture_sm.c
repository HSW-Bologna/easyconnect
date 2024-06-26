#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_IMG_ICON_TEMPEARTURE_SM
#define LV_ATTRIBUTE_IMG_IMG_ICON_TEMPEARTURE_SM
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_IMG_ICON_TEMPEARTURE_SM uint8_t img_icon_tempearture_sm_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Alpha 8 bit, Red: 3 bit, Green: 3 bit, Blue: 2 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x14, 0x00, 0x16, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x2a, 0x93, 0x33, 0xe3, 0x57, 0xe6, 0x2e, 0xa0, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x88, 0x57, 0xfd, 0x5b, 0xfe, 0x7f, 0xfe, 0x7f, 0xfd, 0x2e, 0x9e, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x33, 0xda, 0x5b, 0xfe, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xfe, 0x57, 0xe6, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x57, 0xfe, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x5b, 0xf6, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x2e, 0xff, 0x52, 0xff, 0x7b, 0xff, 0x7f, 0xff, 0x5b, 0xf7, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x32, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x5b, 0xf7, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x2e, 0xff, 0x57, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x5b, 0xf7, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x32, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x5b, 0xf7, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x49, 0xff, 0x84, 0xff, 0xa5, 0xff, 0x89, 0xff, 0x5b, 0xf7, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x89, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x5b, 0xf5, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x69, 0xff, 0x84, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x5b, 0xfe, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x89, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x5b, 0xff, 0x21, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x69, 0xff, 0xa4, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x5b, 0xff, 0x20, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x89, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x5b, 0xff, 0x20, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x69, 0xff, 0xa4, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x5b, 0xff, 0x20, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x89, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x5b, 0xff, 0x20, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x69, 0xff, 0x84, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x5b, 0xff, 0x25, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x33, 0xe8, 0x89, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x5b, 0xff, 0x25, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1f, 0x01, 0x00, 0x1d, 0x33, 0xeb, 0x89, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x5b, 0xff, 0x20, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x04, 0x17, 0x2e, 0xbf, 0x57, 0xfe, 0x89, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xa9, 0xff, 0x7f, 0xff, 0x52, 0xcf, 0x4e, 0x15, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0x2e, 0xb6, 0x57, 0xfd, 0x6e, 0xfe, 0xe5, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0x8e, 0xff, 0x7f, 0xff, 0x52, 0xc8, 0x4e, 0x12, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x55, 0x57, 0xfa, 0x6e, 0xff, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0x8e, 0xff, 0x7f, 0xff, 0x49, 0x81, 0xff, 0x01, 
  0x00, 0x01, 0x2e, 0xab, 0x57, 0xfe, 0xc5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xc4, 0xff, 0xc5, 0xff, 0x7b, 0xff, 0x52, 0xbb, 0x00, 0x00, 
  0x00, 0x07, 0x33, 0xc9, 0x53, 0xfe, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xc5, 0xff, 0x77, 0xff, 0x57, 0xd5, 0x4e, 0x0f, 
  0x00, 0x08, 0x33, 0xcb, 0x53, 0xfe, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xc5, 0xff, 0xc5, 0xff, 0x57, 0xfe, 0x57, 0xdd, 0x05, 0x12, 
  0x00, 0x01, 0x2e, 0xb1, 0x57, 0xfe, 0xc5, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xa4, 0xff, 0xc5, 0xff, 0x7b, 0xfe, 0x56, 0xc6, 0x00, 0x08, 
  0x00, 0x00, 0x05, 0x63, 0x57, 0xfc, 0x6e, 0xfe, 0xe5, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xa4, 0xff, 0xc5, 0xff, 0x8e, 0xff, 0x7f, 0xfd, 0x29, 0x76, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0b, 0x2e, 0xc7, 0x57, 0xfe, 0x6e, 0xff, 0xc5, 0xff, 0xe5, 0xff, 0xe5, 0xff, 0xc5, 0xff, 0x8e, 0xff, 0x7f, 0xfe, 0x52, 0xd4, 0x00, 0x13, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x04, 0x21, 0x2e, 0xcf, 0x5b, 0xfd, 0x5b, 0xfe, 0x56, 0xfe, 0x56, 0xfe, 0x5b, 0xfe, 0x7f, 0xfe, 0x56, 0xd8, 0x05, 0x29, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x29, 0x7e, 0x33, 0xd3, 0x57, 0xf0, 0x5b, 0xf2, 0x57, 0xd7, 0x2d, 0x8b, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x1b, 0x00, 0x1c, 0x00, 0x0f, 0x1f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x14, 0x00, 0x00, 0x16, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x6d, 0x1a, 0x93, 0x36, 0x2c, 0xe3, 0x16, 0x45, 0xe6, 0x8e, 0x2b, 0xa0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x12, 0x88, 0x7d, 0x3d, 0xfd, 0x7e, 0x4e, 0xfe, 0x3d, 0x67, 0xfe, 0xfc, 0x5e, 0xfd, 0x6e, 0x2b, 0x9e, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x36, 0x2c, 0xda, 0xdf, 0x3d, 0xfe, 0xfd, 0x5e, 0xff, 0x1d, 0x5f, 0xff, 0x1d, 0x67, 0xfe, 0x96, 0x4d, 0xe6, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x77, 0x2c, 0xe8, 0xf9, 0x34, 0xfe, 0x3d, 0x67, 0xff, 0x1d, 0x67, 0xff, 0x1d, 0x67, 0xff, 0xd8, 0x4d, 0xf6, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x0f, 0x23, 0xff, 0x92, 0x3c, 0xff, 0x9b, 0x5e, 0xff, 0x1d, 0x5f, 0xff, 0xd8, 0x4d, 0xf7, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0xf3, 0x2b, 0xff, 0x5e, 0x67, 0xff, 0x1d, 0x67, 0xff, 0x1d, 0x67, 0xff, 0xd8, 0x4d, 0xf7, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x30, 0x23, 0xff, 0x35, 0x45, 0xff, 0xbb, 0x5e, 0xff, 0x1d, 0x5f, 0xff, 0xd8, 0x4d, 0xf7, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0xd3, 0x2b, 0xff, 0xfc, 0x5e, 0xff, 0xdc, 0x5e, 0xff, 0xdc, 0x5e, 0xff, 0xd8, 0x4d, 0xf7, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x77, 0x2c, 0xe8, 0x2b, 0x4a, 0xff, 0xe4, 0x80, 0xff, 0x04, 0xb1, 0xff, 0x6a, 0x82, 0xff, 0xd7, 0x4d, 0xf7, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x4c, 0x8a, 0xff, 0x45, 0xf9, 0xff, 0x45, 0xf1, 0xff, 0x49, 0xaa, 0xff, 0xd7, 0x4d, 0xf5, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x2b, 0x52, 0xff, 0xa3, 0x88, 0xff, 0x25, 0xe1, 0xff, 0x49, 0xaa, 0xff, 0xb7, 0x4d, 0xfe, 0x00, 0x08, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x4c, 0x8a, 0xff, 0x45, 0xf9, 0xff, 0x45, 0xf1, 0xff, 0x49, 0xaa, 0xff, 0xb7, 0x4d, 0xff, 0x25, 0x10, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x4b, 0x62, 0xff, 0xe4, 0xb0, 0xff, 0x25, 0xe1, 0xff, 0x4a, 0xaa, 0xff, 0xb7, 0x4d, 0xff, 0x03, 0x18, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x4c, 0x8a, 0xff, 0x45, 0xf1, 0xff, 0x45, 0xf1, 0xff, 0x4a, 0xaa, 0xff, 0xb7, 0x4d, 0xff, 0x02, 0x18, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x2b, 0x5a, 0xff, 0xc3, 0x90, 0xff, 0x25, 0xe1, 0xff, 0x4a, 0xaa, 0xff, 0xb7, 0x4d, 0xff, 0x02, 0x18, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x4c, 0x8a, 0xff, 0x45, 0xf9, 0xff, 0x45, 0xf1, 0xff, 0x4a, 0xaa, 0xff, 0xb7, 0x4d, 0xff, 0x02, 0x18, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x2b, 0x52, 0xff, 0xc3, 0x90, 0xff, 0x25, 0xe1, 0xff, 0x4a, 0xaa, 0xff, 0xb7, 0x4d, 0xff, 0xe6, 0x20, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x97, 0x2c, 0xe8, 0x4c, 0x8a, 0xff, 0x45, 0xf9, 0xff, 0x45, 0xf1, 0xff, 0x4a, 0xaa, 0xff, 0xb7, 0x4d, 0xff, 0x27, 0x21, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x07, 0x01, 0x00, 0x00, 0x1d, 0x97, 0x2c, 0xeb, 0x4c, 0x8a, 0xff, 0x45, 0xf1, 0xff, 0x45, 0xf1, 0xff, 0x49, 0xaa, 0xff, 0xb7, 0x4d, 0xff, 0x04, 0x20, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x08, 0x17, 0xf0, 0x1a, 0xbf, 0x7d, 0x3d, 0xfe, 0x2b, 0x8a, 0xff, 0x45, 0xf1, 0xff, 0x45, 0xf1, 0xff, 0x29, 0xaa, 0xff, 0xbb, 0x5e, 0xff, 0x51, 0x3c, 0xcf, 0x0e, 0x4b, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x1a, 0xb6, 0x7c, 0x35, 0xfd, 0x10, 0x73, 0xfe, 0x05, 0xd9, 0xff, 0x25, 0xf1, 0xff, 0x45, 0xf1, 0xff, 0x25, 0xd9, 0xff, 0x4d, 0x83, 0xff, 0xbb, 0x5e, 0xff, 0x51, 0x3c, 0xc8, 0xac, 0x4a, 0x12, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x83, 0x08, 0x55, 0x3c, 0x35, 0xfa, 0x31, 0x6b, 0xff, 0x45, 0xe9, 0xff, 0xa6, 0xf1, 0xff, 0xa6, 0xf1, 0xff, 0x45, 0xf1, 0xff, 0x45, 0xf1, 0xff, 0x25, 0xd9, 0xff, 0x6e, 0x7b, 0xff, 0xbb, 0x5e, 0xff, 0x8c, 0x3a, 0x81, 0xff, 0xff, 0x01, 
  0x00, 0x00, 0x01, 0x11, 0x23, 0xab, 0x3b, 0x35, 0xfe, 0x25, 0xc1, 0xff, 0xa6, 0xf1, 0xff, 0xa6, 0xf1, 0xff, 0xa6, 0xf1, 0xff, 0xa6, 0xf1, 0xff, 0x45, 0xf1, 0xff, 0xe4, 0xb8, 0xff, 0x25, 0xc9, 0xff, 0x18, 0x56, 0xff, 0x72, 0x3c, 0xbb, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xf5, 0x2b, 0xc9, 0x78, 0x3c, 0xfe, 0x25, 0xe1, 0xff, 0xa6, 0xf1, 0xff, 0xa6, 0xf1, 0xff, 0xa6, 0xf1, 0xff, 0xa6, 0xf1, 0xff, 0x45, 0xf1, 0xff, 0x25, 0xd9, 0xff, 0x04, 0xc9, 0xff, 0x15, 0x55, 0xff, 0x76, 0x4d, 0xd5, 0xaf, 0x32, 0x0f, 
  0x00, 0x00, 0x08, 0x16, 0x2c, 0xcb, 0x78, 0x3c, 0xfe, 0x25, 0xe1, 0xff, 0xa6, 0xf1, 0xff, 0xa6, 0xf1, 0xff, 0xa6, 0xf1, 0xff, 0x86, 0xf1, 0xff, 0x25, 0xf1, 0xff, 0x04, 0xd1, 0xff, 0xe4, 0xc0, 0xff, 0x14, 0x55, 0xfe, 0x76, 0x4d, 0xdd, 0xe7, 0x10, 0x12, 
  0x00, 0x00, 0x01, 0x52, 0x23, 0xb1, 0x3b, 0x35, 0xfe, 0x05, 0xc1, 0xff, 0x45, 0xf1, 0xff, 0x86, 0xf1, 0xff, 0x86, 0xf1, 0xff, 0x45, 0xf1, 0xff, 0x45, 0xf1, 0xff, 0xc3, 0xa8, 0xff, 0x04, 0xc1, 0xff, 0x19, 0x56, 0xfe, 0x93, 0x3c, 0xc6, 0x04, 0x00, 0x08, 
  0x00, 0x00, 0x00, 0x05, 0x09, 0x63, 0x5c, 0x3d, 0xfc, 0x52, 0x63, 0xfe, 0x25, 0xe9, 0xff, 0x25, 0xf1, 0xff, 0x25, 0xf1, 0xff, 0x05, 0xd9, 0xff, 0xc3, 0xa0, 0xff, 0x04, 0xc1, 0xff, 0x8e, 0x7b, 0xff, 0xbb, 0x5e, 0xfd, 0x29, 0x1a, 0x76, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x52, 0x23, 0xc7, 0x9e, 0x3d, 0xfe, 0x50, 0x6b, 0xff, 0x25, 0xc9, 0xff, 0x25, 0xe9, 0xff, 0x04, 0xd9, 0xff, 0x04, 0xc1, 0xff, 0x4e, 0x7b, 0xff, 0xdb, 0x5e, 0xfe, 0x92, 0x3c, 0xd4, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x08, 0x21, 0x93, 0x23, 0xcf, 0xbd, 0x3d, 0xfd, 0xd9, 0x4d, 0xfe, 0xf4, 0x4c, 0xfe, 0xd3, 0x4c, 0xfe, 0xf8, 0x55, 0xfe, 0xdc, 0x5e, 0xfe, 0xb3, 0x3c, 0xd8, 0x25, 0x11, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xeb, 0x11, 0x7e, 0x14, 0x2c, 0xd3, 0x76, 0x45, 0xf0, 0xb7, 0x4d, 0xf2, 0xf4, 0x44, 0xd7, 0xec, 0x22, 0x8b, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x0f, 0xff, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit  BUT the 2  color bytes are swapped*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x14, 0x00, 0x00, 0x16, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1a, 0x6d, 0x93, 0x2c, 0x36, 0xe3, 0x45, 0x16, 0xe6, 0x2b, 0x8e, 0xa0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x0b, 0x88, 0x3d, 0x7d, 0xfd, 0x4e, 0x7e, 0xfe, 0x67, 0x3d, 0xfe, 0x5e, 0xfc, 0xfd, 0x2b, 0x6e, 0x9e, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x2c, 0x36, 0xda, 0x3d, 0xdf, 0xfe, 0x5e, 0xfd, 0xff, 0x5f, 0x1d, 0xff, 0x67, 0x1d, 0xfe, 0x4d, 0x96, 0xe6, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x77, 0xe8, 0x34, 0xf9, 0xfe, 0x67, 0x3d, 0xff, 0x67, 0x1d, 0xff, 0x67, 0x1d, 0xff, 0x4d, 0xd8, 0xf6, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x23, 0x0f, 0xff, 0x3c, 0x92, 0xff, 0x5e, 0x9b, 0xff, 0x5f, 0x1d, 0xff, 0x4d, 0xd8, 0xf7, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x2b, 0xf3, 0xff, 0x67, 0x5e, 0xff, 0x67, 0x1d, 0xff, 0x67, 0x1d, 0xff, 0x4d, 0xd8, 0xf7, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x23, 0x30, 0xff, 0x45, 0x35, 0xff, 0x5e, 0xbb, 0xff, 0x5f, 0x1d, 0xff, 0x4d, 0xd8, 0xf7, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x2b, 0xd3, 0xff, 0x5e, 0xfc, 0xff, 0x5e, 0xdc, 0xff, 0x5e, 0xdc, 0xff, 0x4d, 0xd8, 0xf7, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x77, 0xe8, 0x4a, 0x2b, 0xff, 0x80, 0xe4, 0xff, 0xb1, 0x04, 0xff, 0x82, 0x6a, 0xff, 0x4d, 0xd7, 0xf7, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x8a, 0x4c, 0xff, 0xf9, 0x45, 0xff, 0xf1, 0x45, 0xff, 0xaa, 0x49, 0xff, 0x4d, 0xd7, 0xf5, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x52, 0x2b, 0xff, 0x88, 0xa3, 0xff, 0xe1, 0x25, 0xff, 0xaa, 0x49, 0xff, 0x4d, 0xb7, 0xfe, 0x08, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x8a, 0x4c, 0xff, 0xf9, 0x45, 0xff, 0xf1, 0x45, 0xff, 0xaa, 0x49, 0xff, 0x4d, 0xb7, 0xff, 0x10, 0x25, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x62, 0x4b, 0xff, 0xb0, 0xe4, 0xff, 0xe1, 0x25, 0xff, 0xaa, 0x4a, 0xff, 0x4d, 0xb7, 0xff, 0x18, 0x03, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x8a, 0x4c, 0xff, 0xf1, 0x45, 0xff, 0xf1, 0x45, 0xff, 0xaa, 0x4a, 0xff, 0x4d, 0xb7, 0xff, 0x18, 0x02, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x5a, 0x2b, 0xff, 0x90, 0xc3, 0xff, 0xe1, 0x25, 0xff, 0xaa, 0x4a, 0xff, 0x4d, 0xb7, 0xff, 0x18, 0x02, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x8a, 0x4c, 0xff, 0xf9, 0x45, 0xff, 0xf1, 0x45, 0xff, 0xaa, 0x4a, 0xff, 0x4d, 0xb7, 0xff, 0x18, 0x02, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x52, 0x2b, 0xff, 0x90, 0xc3, 0xff, 0xe1, 0x25, 0xff, 0xaa, 0x4a, 0xff, 0x4d, 0xb7, 0xff, 0x20, 0xe6, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x2c, 0x97, 0xe8, 0x8a, 0x4c, 0xff, 0xf9, 0x45, 0xff, 0xf1, 0x45, 0xff, 0xaa, 0x4a, 0xff, 0x4d, 0xb7, 0xff, 0x21, 0x27, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x01, 0x00, 0x00, 0x1d, 0x2c, 0x97, 0xeb, 0x8a, 0x4c, 0xff, 0xf1, 0x45, 0xff, 0xf1, 0x45, 0xff, 0xaa, 0x49, 0xff, 0x4d, 0xb7, 0xff, 0x20, 0x04, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xc3, 0x17, 0x1a, 0xf0, 0xbf, 0x3d, 0x7d, 0xfe, 0x8a, 0x2b, 0xff, 0xf1, 0x45, 0xff, 0xf1, 0x45, 0xff, 0xaa, 0x29, 0xff, 0x5e, 0xbb, 0xff, 0x3c, 0x51, 0xcf, 0x4b, 0x0e, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x1a, 0xf0, 0xb6, 0x35, 0x7c, 0xfd, 0x73, 0x10, 0xfe, 0xd9, 0x05, 0xff, 0xf1, 0x25, 0xff, 0xf1, 0x45, 0xff, 0xd9, 0x25, 0xff, 0x83, 0x4d, 0xff, 0x5e, 0xbb, 0xff, 0x3c, 0x51, 0xc8, 0x4a, 0xac, 0x12, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x08, 0x83, 0x55, 0x35, 0x3c, 0xfa, 0x6b, 0x31, 0xff, 0xe9, 0x45, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0x45, 0xff, 0xf1, 0x45, 0xff, 0xd9, 0x25, 0xff, 0x7b, 0x6e, 0xff, 0x5e, 0xbb, 0xff, 0x3a, 0x8c, 0x81, 0xff, 0xff, 0x01, 
  0x00, 0x00, 0x01, 0x23, 0x11, 0xab, 0x35, 0x3b, 0xfe, 0xc1, 0x25, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0x45, 0xff, 0xb8, 0xe4, 0xff, 0xc9, 0x25, 0xff, 0x56, 0x18, 0xff, 0x3c, 0x72, 0xbb, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x2b, 0xf5, 0xc9, 0x3c, 0x78, 0xfe, 0xe1, 0x25, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0x45, 0xff, 0xd9, 0x25, 0xff, 0xc9, 0x04, 0xff, 0x55, 0x15, 0xff, 0x4d, 0x76, 0xd5, 0x32, 0xaf, 0x0f, 
  0x00, 0x00, 0x08, 0x2c, 0x16, 0xcb, 0x3c, 0x78, 0xfe, 0xe1, 0x25, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0xa6, 0xff, 0xf1, 0x86, 0xff, 0xf1, 0x25, 0xff, 0xd1, 0x04, 0xff, 0xc0, 0xe4, 0xff, 0x55, 0x14, 0xfe, 0x4d, 0x76, 0xdd, 0x10, 0xe7, 0x12, 
  0x00, 0x00, 0x01, 0x23, 0x52, 0xb1, 0x35, 0x3b, 0xfe, 0xc1, 0x05, 0xff, 0xf1, 0x45, 0xff, 0xf1, 0x86, 0xff, 0xf1, 0x86, 0xff, 0xf1, 0x45, 0xff, 0xf1, 0x45, 0xff, 0xa8, 0xc3, 0xff, 0xc1, 0x04, 0xff, 0x56, 0x19, 0xfe, 0x3c, 0x93, 0xc6, 0x00, 0x04, 0x08, 
  0x00, 0x00, 0x00, 0x09, 0x05, 0x63, 0x3d, 0x5c, 0xfc, 0x63, 0x52, 0xfe, 0xe9, 0x25, 0xff, 0xf1, 0x25, 0xff, 0xf1, 0x25, 0xff, 0xd9, 0x05, 0xff, 0xa0, 0xc3, 0xff, 0xc1, 0x04, 0xff, 0x7b, 0x8e, 0xff, 0x5e, 0xbb, 0xfd, 0x1a, 0x29, 0x76, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x23, 0x52, 0xc7, 0x3d, 0x9e, 0xfe, 0x6b, 0x50, 0xff, 0xc9, 0x25, 0xff, 0xe9, 0x25, 0xff, 0xd9, 0x04, 0xff, 0xc1, 0x04, 0xff, 0x7b, 0x4e, 0xff, 0x5e, 0xdb, 0xfe, 0x3c, 0x92, 0xd4, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xc4, 0x21, 0x23, 0x93, 0xcf, 0x3d, 0xbd, 0xfd, 0x4d, 0xd9, 0xfe, 0x4c, 0xf4, 0xfe, 0x4c, 0xd3, 0xfe, 0x55, 0xf8, 0xfe, 0x5e, 0xdc, 0xfe, 0x3c, 0xb3, 0xd8, 0x11, 0x25, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x11, 0xeb, 0x7e, 0x2c, 0x14, 0xd3, 0x45, 0x76, 0xf0, 0x4d, 0xb7, 0xf2, 0x44, 0xf4, 0xd7, 0x22, 0xec, 0x8b, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x0f, 0x07, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x6b, 0x4c, 0x14, 0x93, 0xae, 0x85, 0x28, 0xe3, 0xae, 0x9f, 0x3e, 0xe6, 0x71, 0x6f, 0x2b, 0xa0, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x3f, 0x10, 0x88, 0xe7, 0xac, 0x34, 0xfd, 0xf1, 0xcd, 0x4b, 0xfe, 0xe7, 0xe2, 0x5c, 0xfe, 0xdf, 0xda, 0x59, 0xfd, 0x6c, 0x6a, 0x29, 0x9e, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0xae, 0x83, 0x26, 0xda, 0xfd, 0xb8, 0x3a, 0xfe, 0xe7, 0xdd, 0x59, 0xff, 0xe5, 0xe0, 0x5b, 0xff, 0xe6, 0xe1, 0x5c, 0xfe, 0xb0, 0xae, 0x45, 0xe6, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb8, 0x8d, 0x2a, 0xe8, 0xcb, 0x9c, 0x32, 0xfe, 0xea, 0xe4, 0x5d, 0xff, 0xe6, 0xe1, 0x5c, 0xff, 0xe6, 0xe1, 0x5c, 0xff, 0xbd, 0xb9, 0x4b, 0xf6, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2a, 0xe8, 0x79, 0x5e, 0x1e, 0xff, 0x92, 0x8e, 0x38, 0xff, 0xd4, 0xcf, 0x54, 0xff, 0xe6, 0xe1, 0x5b, 0xff, 0xbe, 0xb9, 0x4b, 0xf7, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2a, 0xe8, 0x9b, 0x7c, 0x29, 0xff, 0xed, 0xe7, 0x5e, 0xff, 0xe6, 0xe1, 0x5c, 0xff, 0xe6, 0xe1, 0x5c, 0xff, 0xbe, 0xb9, 0x4b, 0xf7, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2a, 0xe8, 0x80, 0x65, 0x20, 0xff, 0xa9, 0xa3, 0x41, 0xff, 0xd7, 0xd3, 0x55, 0xff, 0xe5, 0xe0, 0x5b, 0xff, 0xbe, 0xb9, 0x4b, 0xf7, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2a, 0xe8, 0x96, 0x78, 0x28, 0xff, 0xe2, 0xdc, 0x5a, 0xff, 0xde, 0xd8, 0x58, 0xff, 0xde, 0xd9, 0x58, 0xff, 0xbe, 0xb9, 0x4b, 0xf7, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xba, 0x8c, 0x2a, 0xe8, 0x58, 0x42, 0x47, 0xff, 0x1e, 0x1d, 0x7f, 0xff, 0x23, 0x21, 0xad, 0xff, 0x4d, 0x4b, 0x80, 0xff, 0xba, 0xb6, 0x49, 0xf7, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2b, 0xe8, 0x5c, 0x47, 0x86, 0xff, 0x2b, 0x28, 0xf5, 0xff, 0x2a, 0x27, 0xf1, 0xff, 0x4b, 0x49, 0xa6, 0xff, 0xbb, 0xb6, 0x49, 0xf5, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2a, 0xe8, 0x5b, 0x45, 0x53, 0xff, 0x17, 0x15, 0x88, 0xff, 0x26, 0x23, 0xdc, 0xff, 0x4b, 0x49, 0xa5, 0xff, 0xb8, 0xb4, 0x47, 0xfe, 0x00, 0x00, 0x06, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2b, 0xe8, 0x5c, 0x47, 0x85, 0xff, 0x2b, 0x28, 0xf4, 0xff, 0x29, 0x26, 0xf0, 0xff, 0x4b, 0x49, 0xa5, 0xff, 0xb7, 0xb4, 0x47, 0xff, 0x28, 0x05, 0x11, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2a, 0xe8, 0x5b, 0x46, 0x62, 0xff, 0x1d, 0x1a, 0xae, 0xff, 0x26, 0x23, 0xe0, 0xff, 0x4c, 0x49, 0xa5, 0xff, 0xb7, 0xb3, 0x47, 0xff, 0x17, 0x00, 0x17, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2b, 0xe8, 0x5c, 0x47, 0x84, 0xff, 0x2b, 0x27, 0xf1, 0xff, 0x29, 0x26, 0xef, 0xff, 0x4c, 0x49, 0xa5, 0xff, 0xb7, 0xb3, 0x47, 0xff, 0x11, 0x00, 0x17, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2a, 0xe8, 0x5b, 0x45, 0x58, 0xff, 0x19, 0x17, 0x93, 0xff, 0x26, 0x23, 0xde, 0xff, 0x4c, 0x49, 0xa5, 0xff, 0xb7, 0xb3, 0x47, 0xff, 0x11, 0x00, 0x17, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2b, 0xe8, 0x5c, 0x47, 0x86, 0xff, 0x2b, 0x28, 0xf6, 0xff, 0x29, 0x26, 0xf0, 0xff, 0x4c, 0x49, 0xa5, 0xff, 0xb7, 0xb3, 0x47, 0xff, 0x11, 0x00, 0x17, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2a, 0xe8, 0x5b, 0x45, 0x53, 0xff, 0x18, 0x16, 0x8c, 0xff, 0x25, 0x23, 0xdc, 0xff, 0x4c, 0x49, 0xa5, 0xff, 0xb7, 0xb3, 0x47, 0xff, 0x2e, 0x1c, 0x22, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xb9, 0x8e, 0x2b, 0xe8, 0x5c, 0x47, 0x86, 0xff, 0x2b, 0x28, 0xf4, 0xff, 0x29, 0x26, 0xf0, 0xff, 0x4c, 0x49, 0xa5, 0xff, 0xb7, 0xb3, 0x47, 0xff, 0x34, 0x22, 0x22, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1d, 0xb9, 0x8e, 0x2a, 0xeb, 0x5c, 0x46, 0x84, 0xff, 0x2a, 0x27, 0xf1, 0xff, 0x29, 0x26, 0xef, 0xff, 0x4b, 0x49, 0xa5, 0xff, 0xb7, 0xb3, 0x47, 0xff, 0x1c, 0x00, 0x1c, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x16, 0x0b, 0x17, 0x7d, 0x5c, 0x19, 0xbf, 0xe8, 0xab, 0x36, 0xfe, 0x58, 0x42, 0x85, 0xff, 0x2a, 0x27, 0xf1, 0xff, 0x29, 0x26, 0xef, 0xff, 0x46, 0x43, 0xa6, 0xff, 0xd8, 0xd3, 0x56, 0xff, 0x8b, 0x86, 0x38, 0xcf, 0x6d, 0x61, 0x48, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x7e, 0x5b, 0x19, 0xb6, 0xe3, 0xaa, 0x33, 0xfd, 0x82, 0x60, 0x6e, 0xfe, 0x24, 0x21, 0xd4, 0xff, 0x29, 0x25, 0xee, 0xff, 0x29, 0x26, 0xee, 0xff, 0x25, 0x22, 0xda, 0xff, 0x6b, 0x68, 0x7c, 0xff, 0xd7, 0xd2, 0x55, 0xff, 0x8a, 0x87, 0x36, 0xc8, 0x63, 0x54, 0x46, 0x12, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x15, 0x0f, 0x06, 0x55, 0xdc, 0xa4, 0x33, 0xfa, 0x8b, 0x64, 0x64, 0xff, 0x28, 0x28, 0xe5, 0xff, 0x2e, 0x32, 0xef, 0xff, 0x2e, 0x32, 0xee, 0xff, 0x2a, 0x29, 0xee, 0xff, 0x29, 0x26, 0xed, 0xff, 0x25, 0x22, 0xd7, 0xff, 0x6e, 0x6b, 0x77, 0xff, 0xd7, 0xd2, 0x56, 0xff, 0x5c, 0x4f, 0x35, 0x81, 0xff, 0xff, 0xff, 0x01, 
  0x00, 0x00, 0x00, 0x01, 0x86, 0x5f, 0x1c, 0xab, 0xd7, 0xa3, 0x33, 0xfe, 0x28, 0x23, 0xc0, 0xff, 0x2f, 0x34, 0xee, 0xff, 0x2f, 0x35, 0xee, 0xff, 0x2f, 0x35, 0xee, 0xff, 0x2e, 0x32, 0xee, 0xff, 0x29, 0x26, 0xee, 0xff, 0x1f, 0x1c, 0xba, 0xff, 0x24, 0x22, 0xca, 0xff, 0xc3, 0xbe, 0x50, 0xff, 0x90, 0x8d, 0x39, 0xbb, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xab, 0x7c, 0x26, 0xc9, 0xc3, 0x8c, 0x38, 0xfe, 0x26, 0x25, 0xdd, 0xff, 0x2f, 0x35, 0xee, 0xff, 0x2f, 0x35, 0xef, 0xff, 0x2f, 0x35, 0xef, 0xff, 0x2f, 0x34, 0xee, 0xff, 0x29, 0x26, 0xed, 0xff, 0x24, 0x22, 0xd8, 0xff, 0x21, 0x1e, 0xc9, 0xff, 0xa5, 0xa1, 0x50, 0xff, 0xaf, 0xab, 0x45, 0xd5, 0x77, 0x55, 0x33, 0x0f, 
  0x00, 0x00, 0x00, 0x08, 0xac, 0x7e, 0x26, 0xcb, 0xc3, 0x8c, 0x37, 0xfe, 0x25, 0x23, 0xdc, 0xff, 0x2e, 0x33, 0xee, 0xff, 0x2f, 0x35, 0xee, 0xff, 0x2f, 0x35, 0xef, 0xff, 0x2d, 0x31, 0xee, 0xff, 0x29, 0x25, 0xed, 0xff, 0x22, 0x20, 0xcf, 0xff, 0x20, 0x1d, 0xc3, 0xff, 0xa3, 0xa0, 0x4f, 0xfe, 0xad, 0xab, 0x45, 0xdd, 0x38, 0x1c, 0x0e, 0x12, 
  0x00, 0x00, 0x00, 0x01, 0x8e, 0x66, 0x1e, 0xb1, 0xd9, 0xa3, 0x33, 0xfe, 0x27, 0x21, 0xbe, 0xff, 0x2a, 0x28, 0xee, 0xff, 0x2d, 0x2f, 0xee, 0xff, 0x2c, 0x2e, 0xee, 0xff, 0x29, 0x27, 0xed, 0xff, 0x29, 0x26, 0xee, 0xff, 0x1b, 0x19, 0xa8, 0xff, 0x22, 0x1f, 0xbc, 0xff, 0xc4, 0xbf, 0x50, 0xfe, 0x94, 0x90, 0x39, 0xc6, 0x1f, 0x00, 0x00, 0x08, 
  0x00, 0x00, 0x00, 0x00, 0x2b, 0x1e, 0x07, 0x63, 0xe3, 0xa9, 0x34, 0xfc, 0x8d, 0x66, 0x60, 0xfe, 0x26, 0x24, 0xe4, 0xff, 0x29, 0x25, 0xed, 0xff, 0x29, 0x25, 0xee, 0xff, 0x24, 0x21, 0xd4, 0xff, 0x1a, 0x18, 0xa3, 0xff, 0x21, 0x1e, 0xc2, 0xff, 0x71, 0x6e, 0x76, 0xff, 0xda, 0xd5, 0x57, 0xfd, 0x45, 0x45, 0x19, 0x76, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x8f, 0x69, 0x1e, 0xc7, 0xec, 0xae, 0x34, 0xfe, 0x80, 0x67, 0x68, 0xff, 0x25, 0x23, 0xc9, 0xff, 0x27, 0x24, 0xe8, 0xff, 0x23, 0x21, 0xd4, 0xff, 0x23, 0x21, 0xc1, 0xff, 0x6c, 0x69, 0x77, 0xff, 0xdb, 0xd6, 0x57, 0xfe, 0x93, 0x8f, 0x38, 0xd4, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x17, 0x07, 0x21, 0x97, 0x6e, 0x20, 0xcf, 0xe6, 0xb3, 0x3b, 0xfd, 0xc9, 0xb7, 0x49, 0xfe, 0x9f, 0x9b, 0x4b, 0xfe, 0x9b, 0x97, 0x4b, 0xfe, 0xc0, 0xbc, 0x4e, 0xfe, 0xdc, 0xd7, 0x58, 0xfe, 0x98, 0x95, 0x3b, 0xd8, 0x25, 0x25, 0x0c, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x54, 0x3c, 0x10, 0x7e, 0xa3, 0x7e, 0x29, 0xd3, 0xb3, 0xaa, 0x42, 0xf0, 0xb7, 0xb3, 0x48, 0xf2, 0xa0, 0x9c, 0x3e, 0xd7, 0x5f, 0x5d, 0x22, 0x8b, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
};

const lv_img_dsc_t img_icon_tempearture_sm = {
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 14,
  .header.h = 32,
  .data_size = 448 * LV_IMG_PX_SIZE_ALPHA_BYTE,
  .data = img_icon_tempearture_sm_map,
};
