// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.2
// LVGL version: 8.3.6
// Project name: ArduinoDemo

#include "../ui.h"

void ui_ScreenPassord_screen_init(void)
{
ui_ScreenPassord = lv_obj_create(NULL);
lv_obj_clear_flag( ui_ScreenPassord, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_ScreenPassord, lv_color_hex(0x112D4E), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_ScreenPassord, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_TextPassword = lv_textarea_create(ui_ScreenPassord);
lv_obj_set_width( ui_TextPassword, 283);
lv_obj_set_height( ui_TextPassword, 42);
lv_obj_set_x( ui_TextPassword, -1 );
lv_obj_set_y( ui_TextPassword, -58 );
lv_obj_set_align( ui_TextPassword, LV_ALIGN_CENTER );
lv_textarea_set_placeholder_text(ui_TextPassword,"Please enter Wifi password");
lv_obj_set_style_bg_color(ui_TextPassword, lv_color_hex(0xDBE2EF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_TextPassword, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_KeyboardPassword = lv_keyboard_create(ui_ScreenPassord);
lv_obj_set_width( ui_KeyboardPassword, 300);
lv_obj_set_height( ui_KeyboardPassword, 120);
lv_obj_set_x( ui_KeyboardPassword, 0 );
lv_obj_set_y( ui_KeyboardPassword, 48 );
lv_obj_set_align( ui_KeyboardPassword, LV_ALIGN_CENTER );
lv_obj_set_style_bg_color(ui_KeyboardPassword, lv_color_hex(0xDBE2EF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_KeyboardPassword, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_ButtonRetWifi = lv_btn_create(ui_ScreenPassord);
lv_obj_set_width( ui_ButtonRetWifi, 30);
lv_obj_set_height( ui_ButtonRetWifi, 26);
lv_obj_set_x( ui_ButtonRetWifi, -138 );
lv_obj_set_y( ui_ButtonRetWifi, -103 );
lv_obj_set_align( ui_ButtonRetWifi, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_ButtonRetWifi, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_ButtonRetWifi, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_ButtonRetWifi, lv_color_hex(0x3F72AF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_ButtonRetWifi, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_ImageRetWifi = lv_img_create(ui_ButtonRetWifi);
lv_img_set_src(ui_ImageRetWifi, &ui_img_return_png);
lv_obj_set_width( ui_ImageRetWifi, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_ImageRetWifi, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_ImageRetWifi, -2 );
lv_obj_set_y( ui_ImageRetWifi, 1 );
lv_obj_set_align( ui_ImageRetWifi, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_ImageRetWifi, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_ImageRetWifi, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_img_set_zoom(ui_ImageRetWifi,30);
lv_obj_set_style_img_recolor(ui_ImageRetWifi, lv_color_hex(0xDBE2EF), LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_img_recolor_opa(ui_ImageRetWifi, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SpinnerLoadPassword = lv_spinner_create(ui_ScreenPassord,1000,90);
lv_obj_set_width( ui_SpinnerLoadPassword, 80);
lv_obj_set_height( ui_SpinnerLoadPassword, 80);
lv_obj_set_align( ui_SpinnerLoadPassword, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_SpinnerLoadPassword, LV_OBJ_FLAG_HIDDEN );   /// Flags
lv_obj_clear_flag( ui_SpinnerLoadPassword, LV_OBJ_FLAG_CLICKABLE );    /// Flags
lv_obj_set_style_arc_color(ui_SpinnerLoadPassword, lv_color_hex(0x3F72AF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_arc_opa(ui_SpinnerLoadPassword, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_arc_width(ui_SpinnerLoadPassword, 5, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_set_style_arc_width(ui_SpinnerLoadPassword, 5, LV_PART_INDICATOR| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_KeyboardPassword, ui_event_KeyboardPassword, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_ButtonRetWifi, ui_event_ButtonRetWifi, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_KeyboardPassword, keyboard_event_cb, LV_EVENT_ALL, NULL);

}
