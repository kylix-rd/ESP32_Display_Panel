/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <map>
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "sdkconfig.h"

#define ESP_PANEL_HOST_TYPE_I2C      (0)
#define ESP_PANEL_HOST_TYPE_SPI      (1)
#define ESP_PANEL_HOST_TYPE_I80      (2)
#define ESP_PANEL_HOST_TYPE_RGB      (3)
#define ESP_PANEL_HOST_TYPE_QSPI     (4)

/**
 * I2C Host Default Configuration
 *
 */
#define ESP_PANEL_HOST_I2C_CONFIG_DEFAULT(scl_io, sda_io)       \
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

/**
 * SPI Host Default Configuration
 *
 */
/* Refer to `hal/spi_ll.h` in ESP-IDF */
#ifdef CONFIG_IDF_TARGET_ESP32
#define SPI_MAX_TRANSFER_SIZE            ((1ULL << 24) >> 3)
#elif CONFIG_IDF_TARGET_ESP32S2
#define SPI_MAX_TRANSFER_SIZE            ((1ULL << 23) >> 3)
#elif CONFIG_IDF_TARGET_ESP32S3
#define SPI_MAX_TRANSFER_SIZE            ((1ULL << 18) >> 3)
#elif CONFIG_IDF_TARGET_ESP32C3
#define SPI_MAX_TRANSFER_SIZE            ((1ULL << 18) >> 3)
#endif
#define ESP_PANEL_HOST_SPI_CONFIG_DEFAULT(sck_io, sda_io)       \
    {                                                           \
        .mosi_io_num = sda_io,                                  \
        .miso_io_num = -1,                                      \
        .sclk_io_num = sck_io,                                  \
        .quadwp_io_num = -1,                                    \
        .quadhd_io_num = -1,                                    \
        .data4_io_num = -1,                                     \
        .data5_io_num = -1,                                     \
        .data6_io_num = -1,                                     \
        .data7_io_num = -1,                                     \
        .max_transfer_sz = SPI_MAX_TRANSFER_SIZE,               \
        .flags = SPICOMMON_BUSFLAG_MASTER,                      \
        .intr_flags = 0,                                        \
    }

class ESP_PanelHost {
public:
    ESP_PanelHost();
    ~ESP_PanelHost();

    bool addHostI2C(const i2c_config_t *host_config, i2c_port_t host_id);
    bool addHostI2C(int scl_io, int sda_io, i2c_port_t host_id);

    bool addHostSPI(const spi_bus_config_t *host_config, spi_host_device_t host_id);
    bool addHostSPI(int sck_io, int sda_io, spi_host_device_t host_id);

    void begin(void);

private:
    std::map<i2c_port_t, i2c_config_t> _i2c_host_config_map;
    std::map<spi_host_device_t, spi_bus_config_t> _spi_host_config_map;
};
