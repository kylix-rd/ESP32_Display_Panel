/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <map>
#include "driver/i2c.h"

#define ESP_PANEL_HOST_I2C      (0)
#define ESP_PANEL_HOST_SPI      (1)
#define ESP_PANEL_HOST_I80      (2)
#define ESP_PANEL_HOST_RGB      (3)

#define ESP_PANEL_I2C_HOST_ID_DEFAULT    (I2C_NUM_0)
#define ESP_PANEL_I2C_HOST_CONFIG_DEFAULT(scl_io, sda_io)       \
    {                                                           \
        .mode = I2C_MODE_MASTER,                                \
        .sda_io_num = sda_io,                                   \
        .scl_io_num = scl_io,                                   \
        .sda_pullup_en = GPIO_PULLUP_ENABLE,                    \
        .scl_pullup_en = GPIO_PULLUP_ENABLE,                    \
        .master = {                                             \
            .clk_speed = 400000,                                \
        },                                                      \
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL,               \
    }

class ESP_PanelHost {
public:
    ESP_PanelHost();
    ~ESP_PanelHost();

    bool addHostI2C(const i2c_config_t *host_config, int host_id = ESP_PANEL_I2C_HOST_ID_DEFAULT);
    bool addHostI2C(int host_id, int scl_io, int sda_io);

    void begin(void);

private:
    std::map<int, i2c_config_t> _i2c_host_config_map;
};
