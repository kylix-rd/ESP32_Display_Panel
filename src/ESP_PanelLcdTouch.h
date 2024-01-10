/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <functional>
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
    ESP_PanelLcdTouch(ESP_PanelBus *bus, uint16_t width, uint16_t height, int int_io = -1);
    ESP_PanelLcdTouch(ESP_PanelBus *bus, const esp_lcd_touch_config_t &config);
    virtual ~ESP_PanelLcdTouch() = default;

    bool init(void);
    virtual bool begin(void) = 0;
    bool del(void);
    bool swapXY(bool en);
    bool mirrorX(bool en);
    bool mirrorY(bool en);
    bool readRawData(int timeout_ms = 0);
    int getPoints(ESP_PanelTouchPoint points[], uint8_t num = 1);
    int getIndexButtonState(uint8_t index = 0);
    int readPoints(ESP_PanelTouchPoint points[], uint8_t num = 1, int timeout_ms = 0);
    int readIndexButtonState(uint8_t index = 0, int timeout_ms = 0);
    void attachInterruptCallback(std::function<bool (void *)> callback, void *user_data = NULL);

    esp_lcd_touch_handle_t getHandle(void);
    ESP_PanelBus *getBus(void);

protected:
    ESP_PanelBus *bus;
    esp_lcd_touch_config_t config;
    esp_lcd_touch_handle_t handle;

private:
    static void onInterrupt(esp_lcd_touch_handle_t tp);

    uint8_t _tp_points_num;
    uint8_t _tp_buttons_state[CONFIG_ESP_LCD_TOUCH_MAX_BUTTONS];
    ESP_PanelTouchPoint _tp_points[CONFIG_ESP_LCD_TOUCH_MAX_POINTS];

    std::function<bool (void *)> onInterruptPendCallback;
    SemaphoreHandle_t _isr_sem;
    typedef struct {
        void *tp_ptr;
        void *user_data;
    } ESP_PanelLcdTouchCallbackData_t;
    ESP_PanelLcdTouchCallbackData_t callback_data;
};
