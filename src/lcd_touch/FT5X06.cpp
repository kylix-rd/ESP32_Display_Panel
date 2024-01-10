/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ESP_PanelPrivate.h"
#include "FT5X06.h"

static const char *TAG = "FT5X06_CPP";

ESP_PanelLcdTouch_FT5X06::ESP_PanelLcdTouch_FT5X06(ESP_PanelBus *bus, uint16_t width, uint16_t height):
    ESP_PanelLcdTouch(bus, width, height)
{
}

ESP_PanelLcdTouch_FT5X06::ESP_PanelLcdTouch_FT5X06(ESP_PanelBus *bus, const esp_lcd_touch_config_t &config):
    ESP_PanelLcdTouch(bus, config)
{
}

ESP_PanelLcdTouch_FT5X06::~ESP_PanelLcdTouch_FT5X06()
{
    if (handle) {
        del();
    }
    ESP_LOGD(TAG, "Destory");
}

bool ESP_PanelLcdTouch_FT5X06::begin(void)
{
    ENABLE_TAG_PRINT_DEBUG_LOG();

    CHECK_ERR_RET(esp_lcd_touch_new_i2c_ft5x06(bus->getHandle(), &config, &handle), false, "New driver failed");
    ESP_LOGD(TAG, "Touch panel @%p begin", handle);

    return true;
}
