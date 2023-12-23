/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_lcd_panel_ops.h"
#include "ESP_PanelPrivate.h"
#include "ESP_PanelBus.h"
#include "ESP_PanelLcd.h"

#define CALLBACK_DATA_DEFAULT()             \
    {                                       \
        .lcd_ptr = this,                    \
        .user_data = NULL,                  \
    }

static const char *TAG = "ESP_PanelLcd";

ESP_PanelLcd::ESP_PanelLcd(ESP_PanelBus *bus, uint8_t color_bits, int rst_io,
                           const esp_lcd_panel_vendor_init_cmd_t init_cmd[], uint16_t init_cmd_size);
bus(bus),
    panel_config(ESP_LCD_DEVICE_CONFIG_DEFAULT(rst_io, color_bits, &vendor_config)),
    vendor_config(ESP_LCD_COMMON_VENDOR_CONFIG_DEFAULT(init_cmd, init_cmd_size)),
    handle(NULL),
    onDrawBitmapFinishCallback(NULL),
    sem_draw_bitmap_finish(NULL),
    callback_data(CALLBACK_DATA_DEFAULT())
{
#if SOC_LCD_RGB_SUPPORTED
    if (bus->type() == ESP_PANEL_BUS_TYPE_RGB) {
        const esp_lcd_rgb_panel_config_t *rgb_config = static_cast<ESP_PanelBus_RGB *>(bus)->rgbConfig();
        vendor_config.rgb_config = rgb_config;
    }
#endif
}

ESP_PanelLcd::ESP_PanelLcd(ESP_PanelBus *bus, const esp_lcd_panel_dev_config_t &panel_config):
    bus(bus),
    panel_config(panel_config),
    vendor_config(ESP_LCD_COMMON_VENDOR_CONFIG_DEFAULT(NULL, 0)),
    handle(NULL)
{
}

void ESP_PanelLcd::attachFrameEndCallback(ESP_PanelBusCallback_t onFrameEndCallback, void *user_data)
{
    CHECK_NULL_RETURN(bus);
    bus->attachTransmitFinishCallback(onFrameEndCallback, user_data);
}

void ESP_PanelLcd::setColorBits(int bits_per_pixel)
{
    // Only allow to set init commands before `begin()`.
    CHECK_FALSE_RETURN(handle == NULL);
    CHECK_FALSE_RETURN(bits_per_pixel > 0);
    panel_config.bits_per_pixel = bits_per_pixel;
}

void ESP_PanelLcd::setResetPin(int rst_io)
{
    CHECK_FALSE_RETURN(handle == NULL);
    panel_config.reset_gpio_num = rst_io;
}

void ESP_PanelLcd::setInitCommands(const esp_lcd_panel_vendor_init_cmd_t init_cmd[], int init_cmd_size)
{
    // Only allow to set init commands before `begin()`.
    CHECK_FALSE_RETURN(handle == NULL);
    CHECK_FALSE_RETURN((init_cmd_size == 0) || ((init_cmd != NULL) && (init_cmd_size > 0)));
    vendor_config.init_cmds = init_cmd;
    vendor_config.init_cmds_size = init_cmd_size;
}

#if SOC_LCD_RGB_SUPPORTED
void ESP_PanelLcd::enableAutoReleaseBus(void)
{
    // Only allow to set init commands before `begin()`.
    CHECK_FALSE_RETURN(handle == NULL);
    vendor_config.flags.auto_del_panel_io = 1;
}
#endif

void ESP_PanelLcd::begin(void)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_init(handle));

#if !SOC_LCD_RGB_SUPPORTED
    sem_draw_bitmap_finish = xSemaphoreCreateBinary();
    CHECK_NULL_RETURN(sem_draw_bitmap_finish);
#endif

    if (bus->type() != ESP_PANEL_BUS_TYPE_RGB) {
        esp_lcd_panel_io_callbacks_t io_cb = {
            .on_color_trans_done = (esp_lcd_panel_io_color_trans_done_cb_t)onDrawBitmapFinish,
        };
    }
#if SOC_LCD_RGB_SUPPORTED
    else {
        const esp_lcd_rgb_panel_config_t *rgb_config = static_cast<ESP_PanelBus_RGB *>(bus)->rgbConfig();
        esp_lcd_rgb_panel_event_callbacks_t rgb_event_cb = { NULL };
        if (rgb_config->bounce_buffer_size_px == 0) {
            event_cb.on_vsync = (esp_lcd_rgb_panel_vsync_cb_t)bus->on_transmit_finish_callback;
        } else {
            event_cb.on_bounce_frame_finish = (esp_lcd_rgb_panel_bounce_buf_finish_cb_t)bus->on_transmit_finish_callback;
        }
        CHECK_ERROR_RETURN(esp_lcd_rgb_panel_register_event_callbacks(handle, &event_cb, &bus->callback_data));
    }
#endif
}

void ESP_PanelLcd::reset(void)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_reset(handle));
}

void ESP_PanelLcd::del(void)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_del(handle));
    handle = NULL;
}

void ESP_PanelLcd::drawBitmap(int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_draw_bitmap(handle, x_start, y_start, x_end, y_end, color_data));
}

void ESP_PanelLcd::drawBitmapWaitUntilFinish(int x_start, int y_start, int x_end, int y_end, const void *color_data,
        int timeout_ms)
{
    if (bus->flags.bus_type == ESP_PANEL_BUS_TYPE_RGB) {
        drawBitmap(x_start, y_start, x_end, y_end, color_data);
        return;
    }

    CHECK_NULL_RETURN(bus->sem_transmit_finish);
    drawBitmap(x_start, y_start, x_end, y_end, color_data);
    BaseType_t timeout_tick = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    xSemaphoreTake(bus->sem_transmit_finish, timeout_tick);
}

void ESP_PanelLcd::mirror(bool mirror_x, bool mirror_y)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_mirror(handle, mirror_x, mirror_y));
}

void ESP_PanelLcd::swapAxes(bool en)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_swap_xy(handle, en));
}

void ESP_PanelLcd::setGap(int x_gap, int y_gap)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_set_gap(handle, x_gap, y_gap));
}

void ESP_PanelLcd::invertColor(bool en)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_invert_color(handle, en));
}

void ESP_PanelLcd::displayOn(void)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_disp_off(handle, false));
}

void ESP_PanelLcd::displayOff(void)
{
    CHECK_ERROR_RETURN(esp_lcd_panel_disp_off(handle, true));
}

void ESP_PanelLcd::drawColorBar(int width, int height)
{
    int bits_per_piexl = getColorBits();
    int bytes_per_piexl = getColorBytes();
    int line_per_bar = height / bits_per_piexl;
    uint8_t *color = (uint8_t *)calloc(1, line_per_bar * width * bytes_per_piexl);
    CHECK_NULL_RETURN(color);
    for (int j = 0; j < bits_per_piexl; j++) {
        for (int i = 0; i < line_per_bar * width; i++) {
            for (int k = 0; k < bytes_per_piexl; k++) {
                if (bus->host_type != ESP_PANEL_BUS_TYPE_RGB) {
                    color[i * bytes_per_piexl + k] = SPI_SWAP_DATA_TX(BIT(j), bits_per_piexl) >> (k * 8);
                } else {
                    color[i * bytes_per_piexl + k] = BIT(j) >> (k * 8);
                }
            }
        }
        drawBitmapWaitUntilFinish(0, j * line_per_bar, width, (j + 1) * line_per_bar, color);
    }
    free(color);
}

void ESP_PanelLcd::attachDrawBitmapFinishCallback(std::function<bool (void *)> callback, void *user_data)
{
    onDrawBitmapFinishCallback = callback;
}

int ESP_PanelLcd::getColorBits(void)
{
    CHECK_NULL_GOTO(bus, err);

    if (bus->type() == ESP_PANEL_BUS_TYPE_RGB) {
        return static_cast<ESP_PanelBus_RGB *>(bus)->rgbConfig()->bits_per_pixel;
    } else  {
        return panel_config.bits_per_pixel;
    }

err:
    return -1;
}

int ESP_PanelLcd::getColorBytes(void)
{
    int color_bits = getColorBits();
    CHECK_FALSE_GOTO(color_bits > 0, err);

    return color_bits / 8;

err:
    return -1;
}

esp_lcd_panel_handle_t ESP_PanelLcd::getHandle(void)
{
    CHECK_NULL_GOTO(handle, err);
    return handle;

err:
    return NULL;
}

bool ESP_PanelLcd::onDrawBitmapFinish(void *panel_io, void *edata, void *user_ctx)
{
    ESP_PanelLcdCallbackData_t *callback_data = (ESP_PanelLcdCallbackData_t *)user_ctx;
    ESP_PanelLcd *lcd_ptr = (ESP_PanelLcd *)callback_data->lcd_ptr
                            BaseType_t need_yield = pdFALSE;

    if (lcd_ptr->sem_draw_bitmap_finish) {
        xSemaphoreGiveFromISR(bus->sem_draw_bitmap_finish, &need_yield);
    }

    if (lcd_ptr->onDrawBitmapFinish) {
        return bus->onDrawBitmapFinish(callback_data->user_data);
    }
    return (need_yield == pdTRUE);
}
