/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <functional>
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_rgb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "lcd/base/esp_lcd_custom_types.h"
#include "ESP_PanelBus.h"

#define ESP_LCD_DEVICE_CONFIG_DEFAULT(rst_io, color_bits, vendor_cfg) \
    {                                                                 \
        .reset_gpio_num = rst_io,                                     \
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,                   \
        .data_endian = LCD_RGB_DATA_ENDIAN_BIG,                       \
        .bits_per_pixel = color_bits,                                 \
        .flags = {                                                    \
            .reset_active_high = 0,                                   \
        },                                                            \
        .vendor_config = vendor_cfg,                                  \
    }

#define ESP_LCD_COMMON_VENDOR_CONFIG_DEFAULT(cmds, cmds_size)   \
    {                                                           \
        .init_cmds = cmds,                                      \
        .init_cmds_size = cmds_size,                            \
    }

class ESP_PanelLcd {
public:
    /**
     * @brief Construct a new LCD object in a simple way, the `init()` function should be called after this function
     *
     * @note  This function uses some default values to config the LCD object, please use `config*()` functions to
     *        change them
     *
     * @param bus           Pointer of LCD bus
     * @param color_bits    Bits per pixel
     * @param rst_io        Reset pin
     * @param init_cmd      Pointer of the init commands array, should be declared as `static const` and positioned
     *                      outside the function. Set to NULL if no use
     * @param init_cmd_size Byte size of init commands array, set to 0 if no use
     */
    ESP_PanelLcd(ESP_PanelBus *bus, uint8_t color_bits, int rst_io, const esp_lcd_panel_vendor_init_cmd_t init_cmd[],
                 uint16_t init_cmd_size);

    /**
     * @brief Construct a new LCD object in a complex way, the `init()` function should be called after this function
     *
     * @param bus           Pointer of LCD bus
     * @param panel_config  LCD device configuration
     */
    ESP_PanelLcd(ESP_PanelBus *bus, const esp_lcd_panel_dev_config_t &panel_config);

    /**
     * @brief Destroy the LCD object
     *
     */
    virtual ~ESP_PanelLcd() = default;

    /**
     * @brief Configure the color order of LCD to BGR, default is RGB
     *
     * @note  This function should be called before `init()`
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool configColorOrderBGR(void);

    /**
     * @brief Configure RGB interface to mirror by command, default is false (by software)
     *
     * @note  This function should be called before `init()` and is only valid for "3-wire SPI + RGB" interface
     * @note  The `mirror()` function will be implemented by LCD command. Otherwise, the function will be implemented
     *        by software.
     * @note  This function is conflict with `configRgbAutoReleaseBus()`, please don't use them at the same time
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool configRgbMirrorByCmd(void);

    /**
     * @brief Configure RGB interface to release bus automatically, default is false
     *
     * @note  This function should be called before `init()` and is only valid for "3-wire SPI + RGB" interface
     * @note  If the 3-wire SPI interface are sharing pins of the RGB interface to save GPIOs, please call
     *        this function to release the bus and its pins (except CS signal)
     * @note  This function is conflict with `configRgbMirrorByCmd()`, please don't use them at the same time
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool configRgbAutoReleaseBus(void);

    /**
     * @brief Initialize the LCD object, the `begin()` function should be called after this function
     *
     * @note  This function typically calls `esp_lcd_new_panel_*()` to create the LCD panel handle
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    virtual bool init(void) = 0;

    /**
     * @brief Startup the LCD device
     *
     * @note  This function should be called after `init()`
     * @note  This function typically calls `esp_lcd_panel_init()` to initialize the LCD device
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool begin(void);

    /**
     * @brief Reset the LCD device
     *
     * @note  This function should be called after `begin()`
     * @note  This function typically calls `esp_lcd_panel_reset()` to reset the LCD device
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool reset(void);

    /**
     * @brief Delete the LCD object, release the resources
     *
     * @note  This function should be called after `begin()`
     * @note  This function typically calls `esp_lcd_panel_del()` to delete the LCD device
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool del(void);

    /**
     * @brief Draw the bitmap to the LCD device without waiting for the drawing to finish
     *
     * @note  This function should be called after `begin()`
     * @note  This function typically calls `esp_lcd_panel_draw_bitmap()` to draw the bitmap
     * @note  This function is non-blocking, the drawing will be finished in the background. So the bitmap data should
     *        not be modified until the drawing is finished
     * @note  For RGB interface, this function is same as `drawBitmapWaitUntilFinish()`
     *
     * @param x_start    X coordinate of the start point, the range is [0, width - 1]
     * @param y_start    Y coordinate of the start point, the range is [0, height - 1]
     * @param x_end      X coordinate of the end point, the range is [1, width], should be greater than `x_start`
     * @param y_end      Y coordinate of the end point, the range is [1, height], should be greater than `y_start`
     * @param color_data Pointer of the color data array
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool drawBitmap(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, const uint8_t *color_data);

    /**
     * @brief Draw the bitmap to the LCD device with a timeout to wait for the drawing to finish
     *
     * @note  This function should be called after `begin()`
     * @note  This function not only calls `esp_lcd_panel_draw_bitmap()` to draw the bitmap, but also waits for the
     *        drawing to finish until the timeout is reached.
     * @note  This function is blocking and will quit after the drawing is finished. So the bitmap data can be
     *        immediately modified
     * @note  For RGB interface, this function is same as `drawBitmap()`
     *
     * @param x_start    X coordinate of the start point, the range is [0, width - 1]
     * @param y_start    Y coordinate of the start point, the range is [0, height - 1]
     * @param x_end      X coordinate of the end point, the range is [1, width], should be greater than `x_start`
     * @param y_end      Y coordinate of the end point, the range is [1, height], should be greater than `y_start`
     * @param color_data Pointer of the color data array
     *
     * @return
     *      - true:  if success
     *      - false: if fail or timeout
     */
    bool drawBitmapWaitUntilFinish(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end,
                                   const uint8_t *color_data, int timeout_ms = -1);

    /**
     * @brief Mirror the X axis
     *
     * @note  This function should be called after `begin()`
     * @note  This function typically calls `esp_lcd_panel_mirror()` to mirror the axises
     *
     * @param en true: mirror x, false: restore x
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool mirrorX(bool en);

    /**
     * @brief Mirror the Y axis
     *
     * @note  This function should be called after `begin()`
     * @note  This function typically calls `esp_lcd_panel_mirror()` to mirror the axises
     *
     * @param en true: mirror, false: restore
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool mirrorY(bool en);

    /**
     * @brief Swap the X and Y axis
     *
     * @note  This function should be called after `begin()`
     * @note  This function typically calls `esp_lcd_panel_swap_xy()` to mirror the axises
     *
     * @param en true: swap, false: restore
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool swapXY(bool en);

    /**
     * @brief Set the Gap in the X axis
     *
     * @param value The gap in pixel
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool setGapX(int value);

    /**
     * @brief Set the Gap in the Y axis
     *
     * @param value The gap in pixel
     *
     * @return
     *      - true:  if success
     *      - false: if fail
     */
    bool setGapY(int value);

    bool invertColor(bool en);
    bool displayOn(void);
    bool displayOff(void);
    bool attachDrawBitmapFinishCallback(std::function<bool (void *)> callback, void *user_data = NULL);

    /**
     * @brief Draw color bar from top to bottom, the order is BGR. This function is used for testing.
     *
     * @note  Every bar indicate 1 bit. For example, if the `bits_per_pixel` is `16`, there will be `16` bars.
     *        If the `height` can't be divided by `bits_per_pixel`, the reset area will be filled with white color.
     *
     * @param width
     * @param height
     */
    void drawColorBar(int width, int height);

    uint8_t getPixelColorBits(void);
#if SOC_LCD_RGB_SUPPORTED
    void *getRgbBufferByIndex(int index);
#endif

    esp_lcd_panel_handle_t getHandle(void);
    ESP_PanelBus *getBus(void);

protected:
    ESP_PanelBus *bus;
    esp_lcd_panel_dev_config_t panel_config;
    esp_lcd_panel_vendor_config_t vendor_config;
    esp_lcd_panel_handle_t handle;

private:
    static bool onDrawBitmapFinish(void *panel_io, void *edata, void *user_ctx);

    bool _mirro_x;
    bool _mirro_y;
    int _x_gap;
    int _y_gap;
    std::function<bool (void *)> onDrawBitmapFinishCallback;
    SemaphoreHandle_t sem_draw_bitmap_finish;

    typedef struct {
        void *lcd_ptr;
        void *user_data;
    } ESP_PanelLcdCallbackData_t;
    ESP_PanelLcdCallbackData_t callback_data;
};
