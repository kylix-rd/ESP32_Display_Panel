/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ESP_PanelPrivate.h"
#include "ESP_PanelHost.h"

using namespace std;

ESP_PanelHost::ESP_PanelHost()
{

}

ESP_PanelHost::~ESP_PanelHost()
{

}

bool ESP_PanelHost::addHostI2C(const i2c_config_t *host_config, i2c_port_t host_id)
{
    auto ret = _i2c_host_config_map.find(host_id);
    if (ret == _i2c_host_config_map.end()) {
        _i2c_host_config_map.insert(pair<i2c_port_t, i2c_config_t>(host_id, *host_config));
        return true;
    }
    CHECK_FALSE_RET((ret->second.scl_io_num != scl_io) || (ret->second.sda_io_num != sda_io), false,
                    "I2C[%d] is already added and attempt to add a different configuartion", host_id);

    return true;
}

bool ESP_PanelHost::addHostI2C(int scl_io, int sda_io, i2c_port_t host_id)
{
    i2c_config_t host_config = I2C_HOST_CONFIG_DEFAULT(scl_io, sda_io);

    auto ret = _i2c_host_config_map.find(host_id);
    if (ret == _i2c_host_config_map.end()) {
        _i2c_host_config_map.insert(pair<i2c_port_t, i2c_config_t>(host_id, host_config));
        return true;
    }
    CHECK_FALSE_RET((ret->second.scl_io_num != scl_io) || (ret->second.sda_io_num != sda_io), false,
                    "I2C[%d] is already added and attempt to add a different configuartion", (int)host_id);

    return true;
}

bool ESP_PanelHost::addHostSPI(const spi_bus_config_t *host_config, spi_host_device_t host_id)
{
    auto ret = _spi_host_config_map.find(host_id);
    if (ret == _spi_host_config_map.end()) {
        _spi_host_config_map.insert(pair<spi_host_device_t, spi_bus_config_t>(host_id, *host_config));
        return true;
    }
    CHECK_FALSE_RET((ret->second.mosi_io_num != host_config->mosi_io_num) ||
                    (ret->second.sclk_io_num != host_config->sclk_io_num), false,
                    "SPI[%d] is already added and attempt to add a different configuartion", (int)host_id);

    return true;
}

bool ESP_PanelHost::addHostSPI(int sck_io, int sda_io, spi_host_device_t host_id)
{
    spi_bus_config_t host_config = SPI_HOST_CONFIG_DEFAULT(sck_io, sda_io);

    auto ret = _spi_host_config_map.find(host_id);
    if (ret == _spi_host_config_map.end()) {
        _spi_host_config_map.insert(pair<spi_host_device_t, spi_bus_config_t>(host_id, host_config));
        return true;
    }
    CHECK_FALSE_RET((ret->second.mosi_io_num != host_config.mosi_io_num) ||
                    (ret->second.sclk_io_num != host_config.sclk_io_num), false,
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
