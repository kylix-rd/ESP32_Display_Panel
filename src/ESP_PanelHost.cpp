/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstring>
#include "ESP_PanelPrivate.h"
#include "ESP_PanelHost.h"

using namespace std;

static const char *TAG = "ESP_PanelHost";

ESP_PanelHost::ESP_PanelHost()
{
}

ESP_PanelHost::~ESP_PanelHost()
{
}

bool ESP_PanelHost::addHostI2C(const i2c_config_t &host_config, i2c_port_t host_id)
{
    auto ret = _i2c_host_config_map.find(host_id);
    if (ret == _i2c_host_config_map.end()) {
        _i2c_host_config_map.insert(pair<i2c_port_t, i2c_config_t>(host_id, host_config));
        return true;
    }
    CHECK_FALSE_RET(!memcmp(&ret->second, &host_config, sizeof(i2c_config_t)), false,
                    "Host I2C[%d] is already added and attempt to add a different configuartion", (int)host_id);

    return true;
}

bool ESP_PanelHost::addHostI2C(int scl_io, int sda_io, i2c_port_t host_id)
{
    i2c_config_t host_config = ESP_PANEL_HOST_I2C_CONFIG_DEFAULT(scl_io, sda_io);

    auto ret = _i2c_host_config_map.find(host_id);
    if (ret == _i2c_host_config_map.end()) {
        _i2c_host_config_map.insert(pair<i2c_port_t, i2c_config_t>(host_id, host_config));
        return true;
    }
    CHECK_FALSE_RET(!memcmp(&ret->second, &host_config, sizeof(i2c_config_t)), false,
                    "Host I2C[%d] is already added and attempt to add a different configuartion", (int)host_id);

    return true;
}

bool ESP_PanelHost::addHostSPI(const spi_bus_config_t &host_config, spi_host_device_t host_id)
{
    auto ret = _spi_host_config_map.find(host_id);
    if (ret == _spi_host_config_map.end()) {
        _spi_host_config_map.insert(pair<spi_host_device_t, spi_bus_config_t>(host_id, host_config));
        return true;
    }
    CHECK_FALSE_RET(!memcmp(&ret->second, &host_config, sizeof(spi_bus_config_t)), false,
                    "Host SPI[%d] is already added and attempt to add a different configuartion", (int)host_id);

    return true;
}

bool ESP_PanelHost::addHostSPI(int sck_io, int sda_io, spi_host_device_t host_id)
{
    spi_bus_config_t host_config = ESP_PANEL_HOST_SPI_CONFIG_DEFAULT(sck_io, sda_io);

    auto ret = _spi_host_config_map.find(host_id);
    if (ret == _spi_host_config_map.end()) {
        _spi_host_config_map.insert(pair<spi_host_device_t, spi_bus_config_t>(host_id, host_config));
        return true;
    }
    CHECK_FALSE_RET(!memcmp(&ret->second, &host_config, sizeof(spi_bus_config_t)), false,
                    "SPI[%d] is already added and attempt to add a different configuartion", (int)host_id);

    return true;
}

bool ESP_PanelHost::begin(void)
{
    // Initialize all I2C hosts
    if (_i2c_host_config_map.size() > 0) {
        for (auto &it : _i2c_host_config_map) {
            CHECK_ERR_RET(i2c_param_config(it.first, &it.second), false, "I2C[%d] config param failed", it.first);
            CHECK_ERR_RET(i2c_driver_install(it.first, it.second.mode, 0, 0, 0), false, "I2C[%d] install driver failed",
                          it.first);
        }
    }

    // Initialize all SPI hosts
    if (_spi_host_config_map.size() > 0) {
        for (auto &it : _spi_host_config_map) {
            CHECK_ERR_RET(spi_bus_initialize(it.first, &it.second, SPI_DMA_CH_AUTO), false, "SPI[%d] initialize failed",
                          it.first);
        }
    }

    return true;
}
