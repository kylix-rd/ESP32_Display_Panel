/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// *INDENT-OFF*
/* Handle special Kconfig options */
#ifndef ESP_PANEL_KCONFIG_IGNORE
    #include "sdkconfig.h"
    #ifdef CONFIG_ESP_PANEL_DRIVER_CONF_SKIP
        #define ESP_PANEL_DRIVER_CONF_SKIP
    #endif
#endif

/* If "ESP_Panel_Driver_Conf.h" is available from here, try to use it later */
#ifdef __has_include
    #if __has_include("ESP_Panel_Driver_Conf.h")
        #ifndef ESP_PANEL_DRIVER_CONF_INCLUDE_SIMPLE
            #define ESP_PANEL_DRIVER_CONF_INCLUDE_SIMPLE
        #endif
    #elif __has_include("../../ESP_Panel_Driver_Conf.h")
        #ifndef ESP_PANEL_DRIVER_CONF_INCLUDE_OUTSIDE
            #define ESP_PANEL_DRIVER_CONF_INCLUDE_OUTSIDE
        #endif
    #endif
#endif

/* If "ESP_Panel_Driver_Conf.h" is not skipped, include it */
#ifndef ESP_PANEL_DRIVER_CONF_SKIP
    #ifdef ESP_PANEL_DRIVER_CONF_PATH                           /* If there is a path defined for "ESP_Panel_Driver_Conf.h" use it */
        #define __TO_STR_AUX(x) #x
        #define __TO_STR(x) __TO_STR_AUX(x)
        #include __TO_STR(ESP_PANEL_DRIVER_CONF_PATH)
        #undef __TO_STR_AUX
        #undef __TO_STR
    #elif defined(ESP_PANEL_DRIVER_CONF_INCLUDE_SIMPLE)         /* Or simply include if "ESP_Panel_Driver_Conf.h" is available */
        #include "ESP_Panel_Driver_Conf.h"
    #elif defined(ESP_PANEL_DRIVER_CONF_INCLUDE_OUTSIDE)
        #include "../../ESP_Panel_Driver_Conf.h"         /* Or include if "../../ESP_Panel_Driver_Conf.h" is available */
    #else
        #include "../ESP_Panel_Driver_Conf.h"
    #endif
#endif

#ifndef ESP_PANEL_CHECK_RESULT_ASSERT
    #ifdef CONFIG_ESP_PANEL_CHECK_RESULT_ASSERT
        #define ESP_PANEL_CHECK_RESULT_ASSERT    CONFIG_ESP_PANEL_CHECK_RESULT_ASSERT
    #else
        #define ESP_PANEL_CHECK_RESULT_ASSERT    (0)
    #endif
#endif

#ifndef ESP_PANEL_ENABLE_LOG
    #ifdef CONFIG_ESP_PANEL_ENABLE_LOG
        #define ESP_PANEL_ENABLE_LOG    CONFIG_ESP_PANEL_ENABLE_LOG
    #else
        #define ESP_PANEL_ENABLE_LOG    (0)
    #endif
#endif
