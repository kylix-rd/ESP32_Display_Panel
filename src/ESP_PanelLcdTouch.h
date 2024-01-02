/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "lcd_touch/base/esp_lcd_touch.h"
#include "ESP_PanelBus.h"

class ESP_PanelTouchPoint {
public:
    ESP_PanelTouchPoint(void);
    ESP_PanelTouchPoint(uint16_t x, uint16_t y, uint16_t strength);

    bool operator==(ESP_PanelTouchPoint p);
    bool operator!=(ESP_PanelTouchPoint p);

    uint16_t x;
    uint16_t y;
    uint16_t strength;
};

class ESP_PanelLcdTouch {
public:
    ESP_PanelLcdTouch(ESP_PanelBus *bus, uint16_t width, uint16_t height);
    ESP_PanelLcdTouch(ESP_PanelBus *bus, const esp_lcd_touch_config_t &config);
    virtual ~ESP_PanelLcdTouch() = default;

    void init(void);
    virtual bool begin(void) = 0;
    bool del(void);
    bool swapAxes(bool en);
    bool mirrorX(bool en);
    bool mirrorY(bool en);
    bool readRawData(void);
    int getPoints(ESP_PanelTouchPoint points[], uint8_t num = 1);
    int getIndexButtonState(uint8_t index = 0);
    int readPoints(ESP_PanelTouchPoint points[], uint8_t num = 1);
    int readIndexButtonState(uint8_t index = 0);

    esp_lcd_touch_handle_t getHandle(void);
    ESP_PanelBus *getBus(void);

protected:
    ESP_PanelBus *bus;
    esp_lcd_touch_config_t config;
    esp_lcd_touch_handle_t handle;

private:
    uint8_t _tp_points_num;
    uint8_t _tp_buttons_state[CONFIG_ESP_LCD_TOUCH_MAX_BUTTONS];
    ESP_PanelTouchPoint _tp_points[CONFIG_ESP_LCD_TOUCH_MAX_POINTS];
};
