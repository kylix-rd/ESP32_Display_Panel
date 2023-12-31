/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
    bool readData(void);
    bool swapAxes(bool en);
    bool mirrorX(bool en);
    bool mirrorY(bool en);
    bool getLcdTouchState(void);
    bool getPoint(ESP_PanelTouchPoint &point, uint8_t n = 0);
    bool getButtonState(uint8_t n = 0);

    esp_lcd_touch_handle_t getHandle(void);
    ESP_PanelBus *getBus(void);

protected:
    ESP_PanelBus *bus;
    esp_lcd_touch_config_t config;
    esp_lcd_touch_handle_t handle;

private:
    uint8_t touch_state;
    uint8_t num_points;
    uint16_t x[LCD_TOUCH_MAX_POINTS];
    uint16_t y[LCD_TOUCH_MAX_POINTS];
    uint16_t strength[LCD_TOUCH_MAX_POINTS];
};
