/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_lcd_types.h"

#define ESP_PANEL_BUS_TYPE_I2C      0
#define ESP_PANEL_BUS_TYPE_SPI      1
#define ESP_PANEL_BUS_TYPE_RGB      2
#define ESP_PANEL_BUS_TYPE_QSPI     3
#define ESP_PANEL_BUS_TYPE_I80      4

class ESP_PanelBus {
public:
    ESP_PanelBus(uint8_t host_type, bool host_need_init);
    virtual ~ESP_PanelBus() = default;

    virtual void begin(void) = 0;
    void del(void);
    void readCommandParameter(int lcd_cmd, void *param, int param_size);
    void writeCommandParameter(int lcd_cmd, const void *param, int param_size);
    void writeCommandColor(int lcd_cmd, const void *color, int color_size);

    uint8_t getType(void);
    esp_lcd_panel_io_handle_t getHandle(void);

protected:
    bool host_need_init;
    uint8_t host_type;
    esp_lcd_panel_io_handle_t handle;
};
