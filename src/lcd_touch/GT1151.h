/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "base/esp_lcd_touch_gt1151.h"
#include "ESP_PanelLcdTouch.h"

class ESP_PanelLcdTouch_GT1151 : public ESP_PanelLcdTouch {
public:
    ESP_PanelLcdTouch_GT1151(ESP_PanelBus *bus, uint16_t width, uint16_t height);
    ESP_PanelLcdTouch_GT1151(ESP_PanelBus *bus, const esp_lcd_touch_config_t &config);
    ~ESP_PanelLcdTouch_GT1151() override;

    bool begin(void) override;
};
