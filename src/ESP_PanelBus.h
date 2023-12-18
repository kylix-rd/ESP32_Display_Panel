/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <functional>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_lcd_types.h"

typedef std::function<bool(void *user_data)> ESP_PanelBusCallback_t;

class ESP_PanelBus {
public:
    ESP_PanelBus(int bus_type, bool host_need_init);
    virtual ~ESP_PanelBus() = default;

    void readCommandParameter(int lcd_cmd, void *param, int param_size);
    void writeCommandParameter(int lcd_cmd, const void *param, int param_size);
    void writeCommandColor(int lcd_cmd, const void *color, int color_size);
    void attachTransmitFinishCallback(ESP_PanelBusCallback_t callback, void *user_data = NULL);

    void del(void);
    virtual void init(void) = 0;

    esp_lcd_panel_io_handle_t getHandle(void);
    int getType(void);

protected:
    typedef struct {
        void *bus;
        void *user_data;
    } ESP_PanelBusCallbackData_t;

    esp_lcd_panel_io_handle_t handle;
    ESP_PanelBusCallback_t onTransmitFinishCallback;
    ESP_PanelBusCallbackData_t callback_data;
    struct {
        uint32_t bus_type: 3;
        uint32_t host_need_init: 1;
        uint32_t use_outside_callback: 1;
    } flags;
    SemaphoreHandle_t sem_transmit_finish;

    void createTransmitFinishSemaphore(void);

    static bool on_transmit_finish_callback(void *panel_io, void *edata, void *user_ctx);

private:
    friend class ESP_PanelLcd;
};
