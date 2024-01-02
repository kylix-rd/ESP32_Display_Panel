/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ESP_PanelPrivate.h"
#include "TT21100.h"

static const char *TAG = "TT21100_CPP";

ESP_PanelLcdTouch_TT21100::ESP_PanelLcdTouch_TT21100(ESP_PanelBus *bus, uint16_t width, uint16_t height):
    ESP_PanelLcdTouch(bus, width, height)
{
}

ESP_PanelLcdTouch_TT21100::ESP_PanelLcdTouch_TT21100(ESP_PanelBus *bus, const esp_lcd_touch_config_t &config):
    ESP_PanelLcdTouch(bus, config)
{
}

ESP_PanelLcdTouch_TT21100::~ESP_PanelLcdTouch_TT21100()
{
    if (handle) {
        del();
    }
    ESP_LOGD(TAG, "Destory");
}

bool ESP_PanelLcdTouch_TT21100::begin(void)
{
    ENABLE_TAG_PRINT_DEBUG_LOG();

    CHECK_ERR_RET(esp_lcd_touch_new_i2c_tt21100(bus->getHandle(), &config, &handle), false, "New driver failed");
    ESP_LOGD(TAG, "Touch panel @%p begin", handle);

    return true;
}
