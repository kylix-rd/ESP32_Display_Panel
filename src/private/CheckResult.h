/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "esp_check.h"
#include "esp_log.h"
#include "ESP_Panel_Conf_Internal.h"

#if ESP_PANEL_CHECK_RESULT_ASSERT
#define CHECK_NULL_RETURN(x)                assert(x)
#define CHECK_NULL_GOTO(x, goto_tag)        assert(x)
#define CHECK_FALSE_RETURN(x)               assert(x)
#define CHECK_FALSE_GOTO(x, goto_tag)       assert(x)
#else
#define ERROR_LOG_FORMAT(format)            "[%s:%u] %s(): ", format, esp_panel::path_to_file_name(__FILE__), \
                                                                                                __LINE__, __FUNCTION__
#define ERROR_CHECK_LOGE(tag, format, ...)  printf(ERROR_LOG_FORMAT(format), ##__VA_ARGS__)

#define CHECK_NULL_RET(x, ret, fmt, ...) do {           \
        if ((x) == NULL) {                  \
            ERROR_CHECK_LOGE(fmt, ##__VA_ARGS__); \
            return ret;                         \
        }                                   \
    } while(0)

#define CHECK_FALSE_RET(x, ret, fmt, ...) do {           \
        if ((x) == false) {                  \
            ERROR_CHECK_LOGE(fmt, ##__VA_ARGS__); \
            return ret;                         \
        }                                   \
    } while(0)

#define CHECK_NULL_RETURN(x, fmt, ...) do {           \
        if ((x) == NULL) {                  \
            ERROR_CHECK_LOGE(fmt, ##__VA_ARGS__); \
            return;                         \
        }                                   \
    } while(0)

#define CHECK_NULL_GOTO(x, goto_tag, fmt, ...) do {   \
        if ((x) == NULL) {                  \
            ERROR_CHECK_LOGE(fmt, ##__VA_ARGS__); \
            goto goto_tag;                  \
        }                                   \
    } while(0)

#define CHECK_FALSE_RETURN(x, fmt, ...)  do {         \
        if (unlikely((x) == false)) {       \
            ERROR_CHECK_LOGE(fmt, ##__VA_ARGS__); \
            return;                         \
        }                                   \
    } while(0)

#define CHECK_FALSE_GOTO(x, goto_tag, fmt, ...) do {  \
        if (unlikely((x) == false)) {       \
            ERROR_CHECK_LOGE(fmt, ##__VA_ARGS__); \
            goto goto_tag;                  \
        }                                   \
    } while(0)
#endif /* ESP_PANEL_CHECK_RESULT_ASSERT */

namespace esp_panel {
const char *path_to_file_name(const char *path);
}
