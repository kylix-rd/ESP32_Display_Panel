/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"


#if SOC_LCD_RGB_SUPPORTED
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_lcd_panel_commands.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"

#include "../ESP_PanelPrivate"
#include "../bus/RGB.h"
#include "ST7701.h"

static const char *TAG = "st7701";

ESP_PanelLcd_ST7701::~ESP_PanelLcd_ST7701()
{
    if (handle) {
        del();
    }
}

void ESP_PanelLcd_ST7701::init()
{
    CHECK_NULL_RETURN(bus);
    CHECK_ERROR_RETURN(esp_lcd_new_panel_st7701(bus->getHandle(), &panel_config, &handle));
}

#endif
