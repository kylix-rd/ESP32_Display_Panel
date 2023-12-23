/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include "ESP_PanelPrivate.h"
#include "I2C.h"

static const char *TAG = "ESP_PanelBus_I2C";

ESP_PanelBus_I2C::ESP_PanelBus_I2C(uint8_t address, int scl_io, int sda_io, i2c_port_t host_id):
    ESP_PanelBus(ESP_PANEL_BUS_TYPE_I2C, true),
    host_id(host_id),
    host_config((i2c_config_t)ESP_PANEL_HOST_I2C_CONFIG_DEFAULT(scl_io, sda_io)),
    io_config((esp_lcd_panel_io_i2c_config_t)ESP_PANEL_BUS_I2C_CONFIG_DEFAULT(address))
{
}

ESP_PanelBus_I2C::ESP_PanelBus_I2C(uint8_t address, i2c_port_t host_id):
    ESP_PanelBus(ESP_PANEL_BUS_TYPE_I2C, false),
    host_id(host_id),
    io_config((esp_lcd_panel_io_i2c_config_t)ESP_PANEL_BUS_I2C_CONFIG_DEFAULT(address))
{
}

ESP_PanelBus_I2C::ESP_PanelBus_I2C(const i2c_config_t *host_config, const esp_lcd_panel_io_i2c_config_t *io_config,
                                   i2c_port_t host_id):
    ESP_PanelBus(ESP_PANEL_BUS_TYPE_I2C, true),
    host_id(host_id),
    host_config(*host_config),
    io_config(*io_config)
{
}

ESP_PanelBus_I2C::ESP_PanelBus_I2C(const esp_lcd_panel_io_i2c_config_t *io_config, i2c_port_t host_id):
    ESP_PanelBus(ESP_PANEL_BUS_TYPE_I2C, false),
    host_id(host_id),
    io_config(*io_config)
{
}

ESP_PanelBus_I2C::~ESP_PanelBus_I2C()
{
    if (handle != NULL) {
        del();
        if (host_need_init) {
            i2c_driver_delete(host_id);
        }
    }
}

void ESP_PanelBus_I2C::begin(void)
{
    if (host_need_init) {
        CHECK_ERROR_RETURN(i2c_param_config(host_id, &host_config));
        CHECK_ERROR_RETURN(i2c_driver_install(host_id, host_config.mode, 0, 0, 0));
    }
    CHECK_ERROR_RETURN(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)host_id, &io_config, &handle));
}
