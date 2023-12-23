/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include "esp_lcd_panel_io.h"
#include "ESP_PanelPrivate"
#include "ESP_PanelBus.h"

static const char *TAG = "ESP_PanelBus";

ESP_PanelBus::ESP_PanelBus(ESP_PanelBusType_t host_type, bool host_need_init):
    host_need_init(host_need_init),
    host_type((ESP_PanelBusType_t)host_type),
    handle(NULL)
{
}

void ESP_PanelBus::readCommandParameter(int lcd_cmd, void *param, int param_size)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_io_rx_param(handle, lcd_cmd, param, param_size));
}

void ESP_PanelBus::writeCommandParameter(int lcd_cmd, const void *param, int param_size)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_io_tx_param(handle, lcd_cmd, param, param_size));
}

void ESP_PanelBus::writeCommandColor(int lcd_cmd, const void *color, int color_size)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_io_tx_param(handle, lcd_cmd, color, color_size));
}

void ESP_PanelBus::del(void)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_io_del(handle));
}

esp_lcd_panel_io_handle_t ESP_PanelBus::handle(void)
{
    return handle;
}

int ESP_PanelBus::type(void)
{
    return host_type;
}
