/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// *INDENT-OFF*

/* Set to 1 if assert on error. Otherwise print error message and jump */
#define ESP_PANEL_CHECK_RESULT_ASSERT       (0)         // 0/1

/* Set to 1 if print log message */
#define ESP_PANEL_ENABLE_LOG                (1)         // 0/1

/* Set to 0 if use a custom board */
#define ESP_PANEL_USE_SUPPORTED_BOARD       (1)         // 0/1

#if ESP_PANEL_USE_SUPPORTED_BOARD   // For Supported Board
/*
 * Supported Boards:
 *  - ESP32-C3-LCDkit: https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c3/esp32-c3-lcdkit/index.html
 *  - ESP32-S3-Box: https://github.com/espressif/esp-box/tree/master
 *  - ESP32-S3-Box-3: https://github.com/espressif/esp-box/tree/master
 *  - ESP32-S3-Box-3(beta): https://github.com/espressif/esp-box/tree/c4c954888e11250423f083df0067d99e22d59fbe
 *  - ESP32-S3-Box-Lite: https://github.com/espressif/esp-box/tree/master
 *  - ESP32-S3-EYE: https://github.com/espressif/esp-who/blob/master/docs/en/get-started/ESP32-S3-EYE_Getting_Started_Guide.md
 *  - ESP32-S3-Korvo-2: https://docs.espressif.com/projects/esp-adf/en/latest/design-guide/dev-boards/user-guide-esp32-s3-korvo-2.html
 *  - ESP32-S3-LCD-EV-Board: https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-lcd-ev-board/index.html
 *  - ESP32-S3-LCD-EV-Board-2: https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-lcd-ev-board/index.html
 *  - ESP32-S3-USB-OTG: https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-usb-otg/index.html
 *
 * Uncomment one of the following macros to select an supported development board.
 *
 */
// #define ESP_PANEL_BOARD_ESP32_C3_LCDKIT
#define ESP_PANEL_BOARD_ESP32_S3_BOX
// #define ESP_PANEL_BOARD_ESP32_S3_BOX_3
// #define ESP_PANEL_BOARD_ESP32_S3_BOX_3_BETA
// #define ESP_PANEL_BOARD_ESP32_S3_BOX_LITE
// #define ESP_PANEL_BOARD_ESP32_S3_EYE
// #define ESP_PANEL_BOARD_ESP32_S3_KORVO_2
// #define ESP_PANEL_BOARD_ESP32_S3_LCD_EV_BOARD
// #define ESP_PANEL_BOARD_ESP32_S3_LCD_EV_BOARD_2
// #define ESP_PANEL_BOARD_ESP32_S3_USB_OTG

#else   // For Custom Board

/*===================================== LCD Related =====================================*/
/* Set to 0 if not using LCD */
#define ESP_PANEL_USE_LCD           (1)     // 0/1

#if ESP_PANEL_USE_LCD
/**
 * LCD IC name. Choose one of the following:
 *      - ILI9341
 *      - GC9503, GC9A01
 *      - ST7262, ST7789, ST7796, ST7701
 */
#define ESP_PANEL_LCD_NAME          ST7701

/* LCD resolution in pixels */
#define ESP_PANEL_LCD_H_RES         (480)
#define ESP_PANEL_LCD_V_RES         (480)

/* LCD Bus Settings */
/**
 * If set to 1, the bus will skip to initialize the corresponding host. Users need to initialize the host in advance.
 * It is useful if other devices use the same host. Please ensure that the host is initialized only once.
 */
#define ESP_PANEL_LCD_BUS_SKIP_INIT_HOST        (0)     // 0/1
/**
 * LCD bus type. Choose one of the following:
 *      - 0: I2C (not supported yet)
 *      - 1: SPI
 *      - 2: RGB (only supported for ESP32-S3)
 *      - 3: I80 (not supported yet)
 *      - 4: QSPI (not supported yet)
 */
#define ESP_PANEL_LCD_BUS_TYPE      (2)
/**
 * LCD bus parameters.
 *
 * Please refer to https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/lcd.html
 * for details.
 *
 */
#if ESP_PANEL_LCD_BUS_TYPE == 0         // I2C related parameters

#error "This function is not implemented and will be implemented in the future."

#elif ESP_PANEL_LCD_BUS_TYPE == 1       // SPI panel IO related parameters

    #define ESP_PANEL_LCD_BUS_HOST_ID           (1)
    #define ESP_PANEL_LCD_SPI_IO_CS             (5)
    #define ESP_PANEL_LCD_SPI_IO_DC             (4)
    #define ESP_PANEL_LCD_SPI_MODE              (0)     // 0/1/2/3
    #define ESP_PANEL_LCD_SPI_CLK_HZ            (40 * 1000 * 1000)
    #define ESP_PANEL_LCD_SPI_TRANS_QUEUE_SZ    (10)
    #define ESP_PANEL_LCD_SPI_CMD_BITS          (8)     // 8/16/32
    #define ESP_PANEL_LCD_SPI_PARAM_BITS        (8)     // 8/16/32
#if !ESP_PANEL_LCD_BUS_SKIP_INIT_HOST   // SPI host related parameters
    #define ESP_PANEL_LCD_SPI_IO_SCK            (7)
    #define ESP_PANEL_LCD_SPI_IO_MOSI           (6)
    #define ESP_PANEL_LCD_SPI_IO_MISO           (-1)
#endif /* ESP_PANEL_LCD_BUS_SKIP_INIT_HOST */

#elif ESP_PANEL_LCD_BUS_TYPE == 2       // RGB related parameters

    #define ESP_PANEL_LCD_RGB_CLK_HZ            (11 * 1000 * 1000)
    #define ESP_PANEL_LCD_RGB_HPW               (10)
    #define ESP_PANEL_LCD_RGB_HBP               (10)
    #define ESP_PANEL_LCD_RGB_HFP               (20)
    #define ESP_PANEL_LCD_RGB_VPW               (10)
    #define ESP_PANEL_LCD_RGB_VBP               (10)
    #define ESP_PANEL_LCD_RGB_VFP               (10)
    #define ESP_PANEL_LCD_RGB_PCLK_ACTIVE_NEG   (0)     // 0: rising edge, 1: falling edge
    #define ESP_PANEL_LCD_RGB_DATA_WIDTH        (16)    //  8 | 16
    #define ESP_PANEL_LCD_RGB_PIXEL_BITS        (16)    // 24 | 16
    #define ESP_PANEL_LCD_RGB_FRAME_BUF_NUM     (1)     // 1/2/3
    #define ESP_PANEL_LCD_RGB_BOUNCE_BUF_SIZE   (ESP_PANEL_LCD_H_RES * 10)  // Bounce buffer size in bytes. Set to 0 if disable bounce buffer.
    #define ESP_PANEL_LCD_RGB_IO_HSYNC          (16)
    #define ESP_PANEL_LCD_RGB_IO_VSYNC          (17)
    #define ESP_PANEL_LCD_RGB_IO_DE             (18)
    #define ESP_PANEL_LCD_RGB_IO_PCLK           (21)
    #define ESP_PANEL_LCD_RGB_IO_DISP           (-1)
    #define ESP_PANEL_LCD_RGB_IO_DATA0          (4)
    #define ESP_PANEL_LCD_RGB_IO_DATA1          (5)
    #define ESP_PANEL_LCD_RGB_IO_DATA2          (6)
    #define ESP_PANEL_LCD_RGB_IO_DATA3          (7)
    #define ESP_PANEL_LCD_RGB_IO_DATA4          (15)
    #define ESP_PANEL_LCD_RGB_IO_DATA5          (8)
    #define ESP_PANEL_LCD_RGB_IO_DATA6          (20)
    #define ESP_PANEL_LCD_RGB_IO_DATA7          (3)
#if ESP_PANEL_LCD_RGB_DATA_WIDTH > 8
    #define ESP_PANEL_LCD_RGB_IO_DATA8          (46)
    #define ESP_PANEL_LCD_RGB_IO_DATA9          (9)
    #define ESP_PANEL_LCD_RGB_IO_DATA10         (10)
    #define ESP_PANEL_LCD_RGB_IO_DATA11         (11)
    #define ESP_PANEL_LCD_RGB_IO_DATA12         (12)
    #define ESP_PANEL_LCD_RGB_IO_DATA13         (13)
    #define ESP_PANEL_LCD_RGB_IO_DATA14         (14)
    #define ESP_PANEL_LCD_RGB_IO_DATA15         (0)
#endif /* ESP_PANEL_LCD_RGB_DATA_WIDTH */

#if !ESP_PANEL_LCD_BUS_SKIP_INIT_HOST       // 3-wire SPI panel IO related parameters
    #define ESP_PANEL_LCD_3WIRE_SPI_SCL_ACTIVE_EDGE     (0)     // 0: rising edge, 1: falling edge
    #define ESP_PANEL_LCD_3WIRE_SPI_AUTO_DEL_PANEL_IO   (0)     // Delete the panel IO instance automatically if set to 1.
                                                                // If the 3-wire SPI pins are sharing other pins of the RGB interface to save GPIOs,
                                                                // Please set it to 1 to release the panel IO and its pins (except CS signal).
    #define ESP_PANEL_LCD_3WIRE_SPI_CS_USE_EXPNADER     (0)     // 0/1
    #define ESP_PANEL_LCD_3WIRE_SPI_SCL_USE_EXPNADER    (0)     // 0/1
    #define ESP_PANEL_LCD_3WIRE_SPI_SDA_USE_EXPNADER    (0)     // 0/1
    #define ESP_PANEL_LCD_3WIRE_SPI_IO_CS               (39)
    #define ESP_PANEL_LCD_3WIRE_SPI_IO_SCL              (48)
    #define ESP_PANEL_LCD_3WIRE_SPI_IO_SDA              (47)
#endif /* ESP_PANEL_LCD_BUS_SKIP_INIT_HOST */

#endif /* ESP_PANEL_LCD_BUS_TYPE */

/**
 * LCD initialization commands.
 *
 * Vendor specific initialization can be different between manufacturers, should consult the LCD supplier for
 * initialization sequence code. Please uncomment the following macro definitions and change them in the same format
 * if needed. Otherwise, the LCD driver will use the default initialization sequence code.
 *
 * Format: {cmd, { data }, data_size, delay_ms}
 *
 */
#define ESP_PANEL_LCD_INIT_CMD                                      \
    {                                                               \
        {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x10}, 5, 0}, \
        {0xC0, (uint8_t []){0x3B, 0x00}, 2, 0}, \
        {0xC1, (uint8_t []){0x0D, 0x02}, 2, 0}, \
        {0xC2, (uint8_t []){0x31, 0x05}, 2, 0}, \
        {0xCD, (uint8_t []){0x00}, 1, 0}, \
        {0xB0, (uint8_t []){0x00, 0x11, 0x18, 0x0E, 0x11, 0x06, 0x07, 0x08, 0x07, 0x22, 0x04, 0x12, 0x0F, 0xAA, 0x31, 0x18}, 16, 0}, \
        {0xB1, (uint8_t []){0x00, 0x11, 0x19, 0x0E, 0x12, 0x07, 0x08, 0x08, 0x08, 0x22, 0x04, 0x11, 0x11, 0xA9, 0x32, 0x18}, 16, 0}, \
        {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x11}, 5, 0}, \
        {0xB0, (uint8_t []){0x60}, 1, 0}, \
        {0xB1, (uint8_t []){0x32}, 1, 0}, \
        {0xB2, (uint8_t []){0x07}, 1, 0}, \
        {0xB3, (uint8_t []){0x80}, 1, 0}, \
        {0xB5, (uint8_t []){0x49}, 1, 0}, \
        {0xB7, (uint8_t []){0x85}, 1, 0}, \
        {0xB8, (uint8_t []){0x21}, 1, 0}, \
        {0xC1, (uint8_t []){0x78}, 1, 0}, \
        {0xC2, (uint8_t []){0x78}, 1, 0}, \
        {0xE0, (uint8_t []){0x00, 0x1B, 0x02}, 3, 0}, \
        {0xE1, (uint8_t []){0x08, 0xA0, 0x00, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x44, 0x44}, 11, 0}, \
        {0xE2, (uint8_t []){0x11, 0x11, 0x44, 0x44, 0xED, 0xA0, 0x00, 0x00, 0xEC, 0xA0, 0x00, 0x00}, 12, 0}, \
        {0xE3, (uint8_t []){0x00, 0x00, 0x11, 0x11}, 4, 0}, \
        {0xE4, (uint8_t []){0x44, 0x44}, 2, 0}, \
        {0xE5, (uint8_t []){0x0A, 0xE9, 0xD8, 0xA0, 0x0C, 0xEB, 0xD8, 0xA0, 0x0E, 0xED, 0xD8, 0xA0, 0x10, 0xEF, 0xD8, 0xA0}, 16, 0}, \
        {0xE6, (uint8_t []){0x00, 0x00, 0x11, 0x11}, 4, 0}, \
        {0xE7, (uint8_t []){0x44, 0x44}, 2, 0}, \
        {0xE8, (uint8_t []){0x09, 0xE8, 0xD8, 0xA0, 0x0B, 0xEA, 0xD8, 0xA0, 0x0D, 0xEC, 0xD8, 0xA0, 0x0F, 0xEE, 0xD8, 0xA0}, 16, 0}, \
        {0xEB, (uint8_t []){0x02, 0x00, 0xE4, 0xE4, 0x88, 0x00, 0x40}, 7, 0}, \
        {0xEC, (uint8_t []){0x3C, 0x00}, 2, 0}, \
        {0xED, (uint8_t []){0xAB, 0x89, 0x76, 0x54, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x45, 0x67, 0x98, 0xBA}, 16, 0}, \
        {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x13}, 5, 0}, \
        {0xE5, (uint8_t []){0xE4}, 1, 0}, \
        {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x00}, 5, 0}, \
        {0x11, (uint8_t []){0x00}, 0, 120}, \
        {0x29, (uint8_t []){0x00}, 0, 0}, \
    }

/* LCD Color Settings */
/* LCD color depth in bits */
#define ESP_PANEL_LCD_COLOR_BITS    (18)        // 8/16/18/24
/*
 * LCD RGB Element Order. Choose one of the following:
 *      - 0: RGB
 *      - 1: BGR
 */
#define ESP_PANEL_LCD_RGB_ORDER     (0)
#define ESP_PANEL_LCD_INEVRT_COLOR  (0)         // 0/1

/* LCD Transformation Flags */
#define ESP_PANEL_LCD_SWAP_XY       (0)         // 0/1
#define ESP_PANEL_LCD_MIRROR_X      (0)         // 0/1
#define ESP_PANEL_LCD_MIRROR_Y      (0)         // 0/1

/* LCD Other Settings */
/* IO num of RESET pin, set to -1 if not use */
#define ESP_PANEL_LCD_IO_RST        (-1)
#define ESP_PANEL_LCD_RST_LEVEL     (0)         // 0/1

#elif ESP_PANEL_LCD_BUS_TYPE == 3       // I80 related parameters

#error "This function is not implemented and will be implemented in the future."

#endif /* ESP_PANEL_USE_LCD */

/*===================================== LCD Touch Related =====================================*/
/* Set to 0 if not using LCD touch */
#define ESP_PANEL_USE_LCD_TOUCH     (1)         // 0/1
#if ESP_PANEL_USE_LCD_TOUCH
/**
 * LCD Touch IC name. Choose one of the following:
 *      - CST816S
 *      - FT5x06
 *      - GT1151, GT911
 *      - TT21100
 *      - STMPE610
 */
#define ESP_PANEL_LCD_TOUCH_NAME    GT911

/* LCD Touch resolution in pixels */
#define ESP_PANEL_LCD_TOUCH_H_RES   (ESP_PANEL_LCD_H_RES)
#define ESP_PANEL_LCD_TOUCH_V_RES   (ESP_PANEL_LCD_V_RES)

/* LCD Touch Bus Settings */
/**
 * If set to 1, the bus will skip to initialize the corresponding host. Users need to initialize the host in advance.
 * It is useful if other devices use the same host. Please ensure that the host is initialized only once.
 */
#define ESP_PANEL_LCD_TOUCH_BUS_SKIP_INIT_HOST  (0)     // 0/1
/**
 * LCD touch bus type. Choose one of the following:
 *      - 0: I2C
 *      - 1: SPI
 */
#define ESP_PANEL_LCD_TOUCH_BUS_TYPE            (0)
/**
 * LCD touch bus parameters.
 *
 * Please refer to https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/lcd.html for details.
 */
#if ESP_PANEL_LCD_TOUCH_BUS_TYPE == 0           // I2C related parameters
    #define ESP_PANEL_LCD_TOUCH_BUS_HOST_ID     (0)
#if !ESP_PANEL_LCD_TOUCH_BUS_SKIP_INIT_HOST     // I2C host
    #define ESP_PANEL_LCD_TOUCH_I2C_CLK_HZ      (400 * 1000)
    #define ESP_PANEL_LCD_TOUCH_I2C_SCL_PULLUP  (1)     // 0/1
    #define ESP_PANEL_LCD_TOUCH_I2C_SDA_PULLUP  (1)     // 0/1
    #define ESP_PANEL_LCD_TOUCH_I2C_IO_SCL      (45)
    #define ESP_PANEL_LCD_TOUCH_I2C_IO_SDA      (19)
#endif /* ESP_PANEL_LCD_TOUCH_BUS_SKIP_INIT_HOST */
#elif ESP_PANEL_LCD_TOUCH_BUS_TYPE == 1         // SPI related parameters
    #define ESP_PANEL_LCD_TOUCH_BUS_HOST_ID     (1)
    #define ESP_PANEL_LCD_TOUCH_SPI_IO_CS       (5)
#if !ESP_PANEL_LCD_TOUCH_BUS_SKIP_INIT_HOST     // SPI host
    #define ESP_PANEL_LCD_TOUCH_SPI_IO_SCK      (7)
    #define ESP_PANEL_LCD_TOUCH_SPI_IO_MOSI     (6)
    #define ESP_PANEL_LCD_TOUCH_SPI_IO_MISO     (4)
#endif /* ESP_PANEL_LCD_TOUCH_BUS_SKIP_INIT_HOST */
#endif /* ESP_PANEL_LCD_TOUCH_BUS_TYPE */

/* LCD Touch Transformation Flags */
#define ESP_PANEL_LCD_TOUCH_SWAP_XY         (0)         // 0/1
#define ESP_PANEL_LCD_TOUCH_MIRROR_X        (0)         // 0/1
#define ESP_PANEL_LCD_TOUCH_MIRROR_Y        (0)         // 0/1

/* LCD Touch Other Settings */
#define ESP_PANEL_LCD_TOUCH_IO_RST          (-1)
#define ESP_PANEL_LCD_TOUCH_IO_INT          (-1)
#define ESP_PANEL_LCD_TOUCH_RST_LEVEL       (0)         // 0/1
#define ESP_PANEL_LCD_TOUCH_INT_LEVEL       (0)         // 0/1

#endif /* ESP_PANEL_USE_LCD_TOUCH */

/*===================================== Backlight Related =====================================*/
#define ESP_PANEL_USE_BL                    (1)         // 0/1
#if ESP_PANEL_USE_BL

/* IO num of backlight pin */
#define ESP_PANEL_LCD_IO_BL                 (38)

/* If the backlight is on when high level, set to 1; otherwise to 0 */
#define ESP_PANEL_LCD_BL_ON_LEVEL           (1)         // 0/1

/* Set to 1 if use PWM for backlight brightness control. */
#define ESP_PANEL_LCD_BL_USE_PWM            (1)         // 0/1
#if ESP_PANEL_LCD_BL_USE_PWM
/**
 *  Backlight LEDC Parameters.
 *
 *  Please refer to https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/ledc.html for details.
 */
    #define ESP_PANEL_LCD_BL_PWM_TIMER      (0)
    #define ESP_PANEL_LCD_BL_PWM_CHANNEL    (0)
    #define ESP_PANEL_LCD_BL_PWM_RESOLUTION (10)
    #define ESP_PANEL_LCD_BL_PWM_FREQ_HZ    (5000)
#endif /* ESP_PANEL_LCD_BL_USE_PWM */
#endif /* ESP_PANEL_USE_BL */

/*===================================== IO Expander Related =====================================*/
/* Set to 0 if not using IO Expander */
#define ESP_PANEL_USE_EXPANDER              (0)         // 0/1
#if ESP_PANEL_USE_EXPANDER
/**
 * IO expander IC name. Choose one of the following:
 *      - HT8574
 *      - TCA95xx_8bit
 *      - TCA95xx_16bit
 */
#define ESP_PANEL_EXPANDER_NAME                 TCA95xx_8bit

/* IO expander & host Settings */
// Device parameters
#define ESP_PANEL_EXPANDER_ADDRESS              (0x20)

//  If set to 1, the driver will skip to initialize the corresponding host. Users need to initialize the host in advance.
#define ESP_PANEL_EXPANDER_SKIP_INIT_HOST       (0)     // 0/1
// Host parameters
#if !ESP_PANEL_EXPANDER_SKIP_INIT_HOST
    #define ESP_PANEL_EXPANDER_HOST_ID          (0)
    #define ESP_PANEL_EXPANDER_I2C_CLK_HZ       (400 * 1000)
    #define ESP_PANEL_EXPANDER_I2C_SCL_PULLUP   (0)     // 0/1
    #define ESP_PANEL_EXPANDER_I2C_SDA_PULLUP   (0)     // 0/1
    #define ESP_PANEL_EXPANDER_I2C_IO_SCL       (18)
    #define ESP_PANEL_EXPANDER_I2C_IO_SDA       (8)
#endif
#endif /* ESP_PANEL_USE_EXPANDER */

#endif /* ESP_PANEL_USE_SUPPORTED_BOARD */

// *INDENT-OFF*
