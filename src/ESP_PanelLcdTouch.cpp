/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ESP_PanelPrivate.h"

#include "driver/gpio.h"
#include "ESP_PanelLcdTouch.h"

static const char *TAG = "ESP_PanelLcdTouch";

#define LCD_TOUCH_CONFIG_DEFAULT(width, height, int_io, callback, callback_data) \
    {                                           \
        .x_max = width,                         \
        .y_max = height,                        \
        .rst_gpio_num = GPIO_NUM_NC,            \
        .int_gpio_num = (gpio_num_t)int_io,     \
        .levels = {                             \
            .reset = 0,                         \
            .interrupt = 0,                     \
        },                                      \
        .flags = {                              \
            .swap_xy = 0,                       \
            .mirror_x = 0,                      \
            .mirror_y = 0,                      \
        },                                      \
        .process_coordinates = NULL,            \
        .interrupt_callback = callback,         \
        .user_data = callback_data,             \
    }

#define CALLBACK_DATA_DEFAULT()             \
    {                                       \
        .tp_ptr = this,                     \
        .user_data = NULL,                  \
    }

ESP_PanelTouchPoint::ESP_PanelTouchPoint(void):
    x(0),
    y(0),
    strength(0)
{
}

ESP_PanelTouchPoint::ESP_PanelTouchPoint(uint16_t x, uint16_t y, uint16_t strength):
    x(x),
    y(y),
    strength(strength)
{
}

bool ESP_PanelTouchPoint::operator==(ESP_PanelTouchPoint p)
{
    return ((p.x == x) && (p.y == y));
}

bool ESP_PanelTouchPoint::operator!=(ESP_PanelTouchPoint p)
{
    return ((p.x != x) || (p.y != y));
}

ESP_PanelLcdTouch::ESP_PanelLcdTouch(ESP_PanelBus *bus, uint16_t width, uint16_t height, int int_io):
    bus(bus),
    config((esp_lcd_touch_config_t)LCD_TOUCH_CONFIG_DEFAULT(width, height, int_io, onInterrupt, &callback_data)),
    handle(NULL),
    _tp_points_num(0),
    _tp_buttons_state{0},
    onInterruptPendCallback(NULL),
    _isr_sem(NULL),
    callback_data(CALLBACK_DATA_DEFAULT())
{
}

ESP_PanelLcdTouch::ESP_PanelLcdTouch(ESP_PanelBus *bus, const esp_lcd_touch_config_t &config):
    bus(bus),
    config(config),
    handle(NULL),
    _tp_points_num(0),
    _tp_buttons_state{0},
    onInterruptPendCallback(NULL),
    _isr_sem(NULL),
    callback_data(CALLBACK_DATA_DEFAULT())
{
    if ((config.int_gpio_num != GPIO_NUM_NC) && (config.interrupt_callback == NULL) && (config.user_data == NULL)) {
        this->config.interrupt_callback = onInterrupt;
        this->config.user_data = &callback_data;
    }
}

bool ESP_PanelLcdTouch::init(void)
{
    ENABLE_TAG_PRINT_DEBUG_LOG();

    if (config.interrupt_callback == onInterrupt) {
        _isr_sem = xSemaphoreCreateBinary();
        CHECK_NULL_RET(_isr_sem, false, "Create semaphore failed");

        ESP_LOGD(TAG, "Touch panel @%p enable interruption", handle);
    }

    return true;
}

bool ESP_PanelLcdTouch::del(void)
{
    CHECK_NULL_RET(handle, false, "Invalid handle");
    CHECK_ERR_RET(esp_lcd_touch_del(handle), false, "Delete touch panel failed");

    ESP_LOGD(TAG, "Touch panel @%p deleted", handle);
    handle = NULL;

    return true;
}

bool ESP_PanelLcdTouch::swapAxes(bool en)
{
    CHECK_NULL_RET(handle, false, "Invalid handle");
    CHECK_ERR_RET(esp_lcd_touch_set_swap_xy(handle, en), false, "Swap axes failed");

    return true;
}

bool ESP_PanelLcdTouch::mirrorX(bool en)
{
    CHECK_NULL_RET(handle, false, "Invalid handle");
    CHECK_ERR_RET(esp_lcd_touch_set_mirror_x(handle, en), false, "Mirror X failed");

    return true;
}

bool ESP_PanelLcdTouch::mirrorY(bool en)
{
    CHECK_NULL_RET(handle, false, "Invalid handle");
    CHECK_ERR_RET(esp_lcd_touch_set_mirror_y(handle, en), false, "Mirror Y failed");

    return true;
}

bool ESP_PanelLcdTouch::readRawData(int timeout_ms)
{
    CHECK_NULL_RET(handle, false, "Invalid handle");

    uint16_t x[CONFIG_ESP_LCD_TOUCH_MAX_BUTTONS] = {0};
    uint16_t y[CONFIG_ESP_LCD_TOUCH_MAX_BUTTONS] = {0};
    uint16_t strength[CONFIG_ESP_LCD_TOUCH_MAX_BUTTONS] = {0};
    _tp_points_num = 0;

    if (_isr_sem != NULL) {
        BaseType_t timeout_ticks = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
        if (xSemaphoreTake(_isr_sem, timeout_ticks) != pdTRUE) {
            ESP_LOGD(TAG, "Touch panel @%p wait for isr timeout", handle);
            return true;
        }
    }

    CHECK_ERR_RET(esp_lcd_touch_read_data(handle), false, "Read data failed");
    esp_lcd_touch_get_coordinates(handle, x, y, strength, &_tp_points_num, CONFIG_ESP_LCD_TOUCH_MAX_BUTTONS);

    for (int i = 0; i < _tp_points_num; i++) {
        _tp_points[i].x = x[i];
        _tp_points[i].y = y[i];
        _tp_points[i].strength = strength[i];
        ESP_LOGD(TAG, "Touch panel @%p touched (%d, %d, %d)", handle, _tp_points[i].x, _tp_points[i].y,
                                                                      _tp_points[i].strength);
    }

    return true;
}

int ESP_PanelLcdTouch::getPoints(ESP_PanelTouchPoint points[], uint8_t num)
{
    CHECK_NULL_RET(handle, -1, "Invalid handle");
    CHECK_FALSE_RET((num == 0) || (points != NULL), -1, "Invalid points or num");

    int i = 0;
    for (; (i < num) && (i < _tp_points_num); i++) {
        points[i] = _tp_points[i];
    }

    return i;
}

int ESP_PanelLcdTouch::getIndexButtonState(uint8_t n)
{
    uint8_t button_state = 0;
    CHECK_ERR_RET(esp_lcd_touch_get_button_state(handle, n, &button_state), -1, "Get button state failed");

    return button_state;
}

int ESP_PanelLcdTouch::readPoints(ESP_PanelTouchPoint points[], uint8_t num, int timeout_ms)
{
    CHECK_FALSE_RET(readRawData(timeout_ms), -1, "Read raw data failed");

    return getPoints(points, num);
}

int ESP_PanelLcdTouch::readIndexButtonState(uint8_t n, int timeout_ms)
{
    CHECK_FALSE_RET(readRawData(timeout_ms), -1, "Read raw data failed");

    return getIndexButtonState(n);
}

void ESP_PanelLcdTouch::attachInterruptCallback(std::function<bool (void *)> callback, void *user_data)
{
    onInterruptPendCallback = callback;
    callback_data.user_data = user_data;
}

esp_lcd_touch_handle_t ESP_PanelLcdTouch::getHandle(void)
{
    CHECK_NULL_GOTO(handle, err);
    return handle;

err:
    return NULL;
}

ESP_PanelBus *ESP_PanelLcdTouch::getBus(void)
{
    return bus;
}

void ESP_PanelLcdTouch::onInterrupt(esp_lcd_touch_handle_t tp)
{
    if ((tp == NULL) || (tp->config.user_data == NULL)) {
        return;
    }

    ESP_PanelLcdTouch *panel = (ESP_PanelLcdTouch *)((ESP_PanelLcdTouchCallbackData_t *)tp->config.user_data)->tp_ptr;
    if (panel == NULL) {
        return;
    }

    BaseType_t need_yield = pdFALSE;
    if (panel->onInterruptPendCallback != NULL) {
        need_yield = panel->onInterruptPendCallback(panel->callback_data.user_data) ? pdTRUE : need_yield;
    }
    if (panel->_isr_sem != NULL) {
        xSemaphoreGiveFromISR(panel->_isr_sem, &need_yield);
    }
    if (need_yield == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}
