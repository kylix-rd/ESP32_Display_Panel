/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ESP_PanelPrivate.h"

#include "driver/gpio.h"
#include "ESP_PanelLcdTouch.h"

static const char *TAG = "TP_CPP";

#define LCD_TOUCH_CONFIG_DEFAULT(width, height) \
    {                                           \
        .x_max = width,                         \
        .y_max = height,                        \
        .rst_gpio_num = GPIO_NUM_NC,            \
        .int_gpio_num = GPIO_NUM_NC,            \
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
        .interrupt_callback = NULL,             \
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
    return ((p.x == x) && (p.y == y) && (p.strength == strength));
}

bool ESP_PanelTouchPoint::operator!=(ESP_PanelTouchPoint p)
{
    return ((p.x != x) || (p.y != y) || (p.strength != strength));
}

ESP_PanelLcdTouch::ESP_PanelLcdTouch(ESP_PanelBus *bus, uint16_t width, uint16_t height):
    bus(bus),
    config((esp_lcd_touch_config_t)LCD_TOUCH_CONFIG_DEFAULT(width, height)),
    handle(NULL),
    touch_state(false)
{
}

ESP_PanelLcdTouch::ESP_PanelLcdTouch(ESP_PanelBus *bus, const esp_lcd_touch_config_t &config):
    bus(bus),
    config(config),
    handle(NULL),
    touch_state(false)
{
}

void ESP_PanelLcdTouch::init(void)
{
    ENABLE_TAG_PRINT_DEBUG_LOG();
}

void ESP_PanelLcdTouch::del(void)
{
    CHECK_ERROR_RETURN(esp_lcd_touch_del(handle));
    handle = NULL;
}

void ESP_PanelLcdTouch::readData(void)
{
    CHECK_ERROR_RETURN(esp_lcd_touch_read_data(handle));
    touch_state = esp_lcd_touch_get_coordinates(handle, x, y, strength, &num_points, LCD_TOUCH_MAX_POINTS);
}

bool ESP_PanelLcdTouch::getLcdTouchState(void)
{
    bool ret = false;
    CHECK_NULL_GOTO(handle, err);

    ret = touch_state;
    touch_state = false;
    return ret;

err:
    return false;
}

ESP_PanelTouchPoint ESP_PanelLcdTouch::getPoint(uint8_t n)
{
    ESP_PanelTouchPoint point(0, 0, 0);
    CHECK_FALSE_GOTO(n < num_points, err);

    point.x = x[n];
    point.y = y[n];
    point.strength = strength[n];

err:
    return point;
}

bool ESP_PanelLcdTouch::getButtonState(uint8_t n)
{
    uint8_t button_state[LCD_TOUCH_MAX_BUTTONS] = {0};
    CHECK_ERROR_GOTO(esp_lcd_touch_get_button_state(handle, n, &button_state[n]), err);
    return button_state[n];

err:
    return false;
}

void ESP_PanelLcdTouch::swapAxes(bool en)
{
    CHECK_ERROR_RETURN(esp_lcd_touch_set_swap_xy(handle, en));
}

void ESP_PanelLcdTouch::mirrorX(bool en)
{
    CHECK_ERROR_RETURN(esp_lcd_touch_set_mirror_x(handle, en));
}

void ESP_PanelLcdTouch::mirrorY(bool en)
{
    CHECK_ERROR_RETURN(esp_lcd_touch_set_mirror_y(handle, en));
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
