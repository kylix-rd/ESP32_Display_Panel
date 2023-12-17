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

bool ESP_PanelHost::addHostI2C(const i2c_config_t *host_config, int host_id = ESP_PANEL_I2C_HOST_ID_DEFAULT)
{
    auto ret = _i2c_host_config_map.find(host_id);
    if (ret == _i2c_host_config_map.end()) {
        _i2c_host_config_map.insert(pair<int, i2c_config_t>(host_id, *host_config));
        return true;
    }
    CHECK_FALSE_RET((ret->second.scl_io_num != scl_io) || (ret->second.sda_io_num != sda_io), false,
                    "I2C[%d] already exists and add a different configuartion", host_id);

    return true;
}

bool ESP_PanelHost::addHostI2C(int host_id, int scl_io, int sda_io)
{
    i2c_config_t host_config = I2C_HOST_CONFIG_DEFAULT(scl_io, sda_io);

    auto ret = _i2c_host_config_map.find(host_id);
    if (ret == _i2c_host_config_map.end()) {
        _i2c_host_config_map.insert(pair<int, i2c_config_t>(host_id, host_config));
        return true;
    }
    CHECK_FALSE_RET((ret->second.scl_io_num != scl_io) || (ret->second.sda_io_num != sda_io), false,
                    "I2C[%d] already exists and add a different configuartion", host_id);

    return true;
}

bool ESP_PanelHost::begin(void)
{
    if (_i2c_host_config_map.size() > 0) {
        for (auto &it : _i2c_host_config_map) {
            CHECK_ERR_RET(i2c_param_config((i2c_port_t)it.first, &it.second), false,
                          "I2C[%d] config param failed", it.first);
            CHECK_ERR_RET(i2c_driver_install((i2c_port_t)it.first, it.second.mode, 0, 0, 0), false,
                          "I2C[%d] install driver failed", it.first);
        }
    }

    return true;
}
