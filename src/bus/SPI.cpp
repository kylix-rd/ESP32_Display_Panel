/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "esp_intr_alloc.h"
#include "ESP_PanelPrivate.h"
#include "SPI.h"

static const char *TAG = "ESP_PanelBus_SPI";

ESP_PanelBus_SPI::ESP_PanelBus_SPI(int sck_io, int sda_io, int cs_io, int dc_io, spi_host_device_t host_id):
    ESP_PanelBus(ESP_PANEL_BUS_TYPE_SPI, true),
    host_id(host_id),
    host_config((spi_bus_config_t)ESP_PANEL_HOST_SPI_CONFIG_DEFAULT(clk_io, mosi_io, miso_io)),
    io_config((esp_lcd_panel_io_spi_config_t)SPI_PANEL_IO_CONFIG_DEFAULT(cs_io, dc_io, on_transmit_finish_callback,
              &callback_data))
{
}

ESP_PanelBus_SPI::ESP_PanelBus_SPI(int cs_io, int dc_io, spi_host_device_t host_id):
    ESP_PanelBus(ESP_PANEL_BUS_TYPE_SPI, false),
    host_id(host_id),
    io_config((esp_lcd_panel_io_spi_config_t)SPI_PANEL_IO_CONFIG_DEFAULT(cs_io, dc_io, on_transmit_finish_callback,
              &callback_data))
{
}

ESP_PanelBus_SPI::ESP_PanelBus_SPI(const spi_bus_config_t *host_config, const esp_lcd_panel_io_spi_config_t *io_config,
                                   spi_host_device_t host_id):
    ESP_PanelBus(ESP_PANEL_BUS_TYPE_SPI, true),
    host_config(*host_config),
    io_config(*io_config),
    host_id(host_id)
{
}

ESP_PanelBus_SPI::ESP_PanelBus_SPI(const esp_lcd_panel_io_spi_config_t *io_config, spi_host_device_t host_id):
    ESP_PanelBus(ESP_PANEL_BUS_TYPE_SPI, false),
    io_config(*io_config),
    host_id(host_id)
{
}

ESP_PanelBus_SPI::~ESP_PanelBus_SPI()
{
    if (handle != NULL) {
        del();
        if (flags.host_need_init) {
            spi_bus_free(host_id);
        }
    }
}

void ESP_PanelBus_SPI::begin(void)
{
    if (flags.host_need_init) {
        CHECK_ERROR_RETURN(spi_bus_initialize(host_id, &host_config, SPI_DMA_CH_AUTO));
    }
    CHECK_ERROR_RETURN(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)host_id, &io_config, &handle));
    createTransmitFinishSemaphore();
}
