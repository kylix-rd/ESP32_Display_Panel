/**
 * The example demonstrates how to read touch point.
 *
 * ## How to Use
 *
 * To use this example, please firstly install `ESP32_Display_Panel` (including its dependent libraries),
 * then follow the steps to configure them:
 *
 * 1. [Configure ESP32_Display_Panel](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configure-esp32_display_panel)
 * 2. [Configure Board](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configure-board)
 *
 * ## Example Output
 *
 * ```bash
 * ...
 * Hello Arduino!
 * I am ESP32_Display_Panel.
 * Initialize panel
 * Setup done
 * Touch point: x 479, y 476
 * ...
 * ```
 */

#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <ESP_IOExpander_Library.h>

ESP_Panel *panel;

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */
    Serial.println("Hello Arduino!");
    Serial.println("I am ESP32_Display_Panel.");

#if ESP_PANEL_USE_LCD_TOUCH
    panel = new ESP_Panel();

    /* There are some extral initialization for ESP32-S3-LCD-EV-Board */
#ifdef ESP_PANEL_BOARD_ESP32_S3_LCD_EV_BOARD
    /* Initialize IO expander */
    ESP_IOExpander *expander = new ESP_IOExpander_TCA95xx_8bit(ESP_PANEL_LCD_TOUCH_BUS_HOST_ID, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, ESP_PANEL_LCD_TOUCH_I2C_IO_SCL, ESP_PANEL_LCD_TOUCH_I2C_IO_SDA);
    expander->init();
    expander->begin();
    /* Add into panel for 3-wire SPI */
    panel->addIOExpander(expander);
    /* For the newest version sub board, need to set `ESP_PANEL_LCD_RGB_IO_VSYNC` to low before initialize LCD */
    pinMode(ESP_PANEL_LCD_RGB_IO_VSYNC, OUTPUT);
    digitalWrite(ESP_PANEL_LCD_RGB_IO_VSYNC, HIGH);
#endif

    /* There are some extral initialization for ESP32-S3-Korvo-2 */
#ifdef ESP_PANEL_BOARD_ESP32_S3_KORVO_2
    /* Initialize IO expander */
    ESP_IOExpander *expander = new ESP_IOExpander_TCA95xx_8bit(ESP_PANEL_LCD_TOUCH_BUS_HOST_ID, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, ESP_PANEL_LCD_TOUCH_I2C_IO_SCL, ESP_PANEL_LCD_TOUCH_I2C_IO_SDA);
    expander->init();
    expander->begin();
    /* Reset LCD */
    expander->pinMode(2, OUTPUT);
    expander->digitalWrite(2, LOW);
    usleep(20000);
    expander->digitalWrite(2, LOW);
    usleep(120000);
    expander->digitalWrite(2, HIGH);
    /* Turn on backlight */
    expander->pinMode(1, OUTPUT);
    expander->digitalWrite(1, HIGH);
    /* Keep CS low */
    expander->pinMode(3, OUTPUT);
    expander->digitalWrite(3, LOW);
#endif

#ifdef ESP_PANEL_BOARD_ESP32_S3_BOX_3
    pinMode(ESP_PANEL_LCD_TOUCH_IO_INT, OUTPUT);
    digitalWrite(ESP_PANEL_LCD_TOUCH_IO_INT, LOW);
    usleep(100);
#endif

    Serial.println("Initialize panel");
    /* Initialize bus and device of panel */
    panel->init();
    /* Start panel */
    panel->begin();
#else
    Serial.println("No touch panel");
#endif
    Serial.println("Setup done");
}

void loop()
{
#if ESP_PANEL_USE_LCD_TOUCH
    panel->getLcdTouch()->readData();

    bool touched = panel->getLcdTouch()->getTouchState();
    if(touched) {
        TouchPoint point = panel->getLcdTouch()->getPoint();
        Serial.printf("Touch point: x %d, y %d\n", point.x, point.y);
    }
#endif

    usleep(30000);
}
