/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"


#if SOC_LCD_RGB_SUPPORTED
#include "ESP_PanelLcd.h"
#include "base/esp_lcd_gc9503.h"

class ESP_PanelLcd_GC9503: public ESP_PanelLcd {
public:
    ESP_PanelLcd_GC9503(ESP_PanelBus *bus, const esp_lcd_panel_dev_config_t *panel_config):
        ESP_PanelLcd(bus, panel_config) { };
    ESP_PanelLcd_GC9503(ESP_PanelBus *bus, const esp_lcd_panel_dev_config_t &panel_config):
        ESP_PanelLcd(bus, panel_config) { };
    ESP_PanelLcd_GC9503(ESP_PanelBus *bus, int bits_per_pixel, int rst_io = -1,
                        const esp_lcd_panel_init_cmd_t init_cmd[] = NULL, int init_cmd_size = 0):
        ESP_PanelLcd(bus, bits_per_pixel, rst_io, init_cmd, init_cmd_size) { };
    ~ESP_PanelLcd_GC9503() override;

    void init(void) override;
};

#endif
