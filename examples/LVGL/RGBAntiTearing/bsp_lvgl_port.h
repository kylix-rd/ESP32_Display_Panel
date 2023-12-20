/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "lvgl.h"

esp_err_t bsp_lvgl_port_init(lv_disp_t **disp, lv_indev_t **indev);

bool bsp_lvgl_port_lock(int timeout_ms);

void bsp_lvgl_port_unlock(void);
