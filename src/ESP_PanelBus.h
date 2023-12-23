/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_lcd_types.h"

typedef enum {
    ESP_PANEL_BUS_TYPE_I2C = 0,
    ESP_PANEL_BUS_TYPE_SPI,
    ESP_PANEL_BUS_TYPE_RGB,
    ESP_PANEL_BUS_TYPE_QSPI,
    // ESP_PANEL_BUS_TYPE_I80,
} ESP_PanelBusType_t;

class ESP_PanelBus {
public:
    ESP_PanelBus(ESP_PanelBusType_t host_type, bool host_need_init);
    virtual ~ESP_PanelBus() = default;

    virtual void begin(void) = 0;
    void del(void);
    void readCommandParameter(int lcd_cmd, void *param, int param_size);
    void writeCommandParameter(int lcd_cmd, const void *param, int param_size);
    void writeCommandColor(int lcd_cmd, const void *color, int color_size);

    ESP_PanelBusType_t type(void);
    esp_lcd_panel_io_handle_t handle(void);

protected:
    bool host_need_init;
    ESP_PanelBusType_t host_type;
    esp_lcd_panel_io_handle_t handle;
};
