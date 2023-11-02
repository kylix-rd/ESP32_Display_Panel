// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.2
// LVGL version: 8.3.6
// Project name: Porting

#ifndef _PORTING_UI_H
#define _PORTING_UI_H

#ifdef __cplusplus
extern "C" {
#endif

    #include "lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"
// SCREEN: ui_ScreenMain
void ui_ScreenMain_screen_init(void);
extern lv_obj_t *ui_ScreenMain;
void ui_event_ImgButtonScreenMain1( lv_event_t * e);
extern lv_obj_t *ui_ImgButtonScreenMain1;
void ui_event_ImgButtonScreenMain2( lv_event_t * e);
extern lv_obj_t *ui_ImgButtonScreenMain2;
extern lv_obj_t *ui____initial_actions0;

LV_IMG_DECLARE( ui_img_lexin_1_png);   // assets/img_lexin_1.png
LV_IMG_DECLARE( ui_img_lexin_2_png);   // assets/img_lexin_2.png



void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif