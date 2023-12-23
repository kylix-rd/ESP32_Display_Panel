/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_lcd_panel_rgb.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Refer to `hal/spi_ll.h` in SDK */
#ifdef CONFIG_IDF_TARGET_ESP32
#define SPI_MAX_TRANSFER_SIZE   ((1 << 24) >> 3)
#elif CONFIG_IDF_TARGET_ESP32S2
#define SPI_MAX_TRANSFER_SIZE   ((1 << 23) >> 3)
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
#define SPI_MAX_TRANSFER_SIZE   ((1 << 18) >> 3)
#endif

/**
 * @brief LCD panel initialization commands.
 *
 */
typedef struct {
    int cmd;                /*<! The specific LCD command */
    const void *data;       /*<! Buffer that holds the command specific data */
    size_t data_bytes;      /*<! Size of `data` in memory, in bytes */
    unsigned int delay_ms;  /*<! Delay in milliseconds after this command */
} esp_lcd_panel_vendor_init_cmd_t;

/**
 * @brief LCD panel vendor configuration.
 *
 * @note  This structure needs to be passed to the `vendor_config` field in `esp_lcd_panel_dev_config_t`.
 *
 */
typedef struct {
    const esp_lcd_panel_vendor_init_cmd_t *init_cmds;   /*!< Pointer to initialization commands array. Set to NULL if using default commands.
                                                         *   The array should be declared as `static const` and positioned outside the function.
                                                         *   Please refer to `vendor_specific_init_default` in source file.
                                                         */
    unsigned int init_cmds_size;                        /*<! Number of commands in above array */

    /* Below configurations are only available for RGB interface */
#if SOC_LCD_RGB_SUPPORTED
    const esp_lcd_rgb_panel_config_t *rgb_config;   /*!< RGB panel configuration. */
    struct {
        unsigned int mirror_by_cmd: 1;              /*<! The `mirror()` function will be implemented by LCD command if set to 1.
                                                     *   Otherwise, the function will be implemented by software.
                                                     */
        unsigned int auto_del_panel_io: 1;          /*<! Delete the panel IO instance automatically if set to 1. All `*_by_cmd` flags will be invalid.
                                                     *   If the panel IO pins are sharing other pins of the RGB interface to save GPIOs,
                                                     *   Please set it to 1 to release the panel IO and its pins (except CS signal).
                                                     */
    } flags;
#endif
} esp_lcd_panel_vendor_config_t;

#ifdef __cplusplus
}
#endif
