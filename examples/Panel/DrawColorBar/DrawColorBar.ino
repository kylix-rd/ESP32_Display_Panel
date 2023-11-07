/**
 * The example demonstrates how to draw simple color bar.
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
 * Draw color bar from top to bottom, the order is B - G - R
 * Setup done
 * Loop
 * Loop
 * Loop
 * Loop
 * ...
 * ```
 */

#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <ESP_IOExpander_Library.h>

ESP_Panel *panel;

#if ESP_PANEL_LCD_BUS_TYPE != ESP_PANEL_BUS_TYPE_RGB
bool lcd_trans_done_callback(void *user_ctx)
{
    BaseType_t need_yield = pdFALSE;
    SemaphoreHandle_t sem_lcd_trans_done = (SemaphoreHandle_t)user_ctx;
    xSemaphoreGiveFromISR(sem_lcd_trans_done, &need_yield);

    return (need_yield == pdTRUE);
}
#endif /* ESP_PANEL_LCD_BUS_TYPE */

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */
    Serial.println("Hello Arduino!");
    Serial.println("I am ESP32_Display_Panel.");

    panel = new ESP_Panel();

    /* There are some extral initialization for ESP32-S3-LCD-EV-Board */
#ifdef ESP_PANEL_BOARD_ESP32_S3_LCD_EV_BOARD
    /* Initialize IO expander */
    ESP_IOExpander *expander = new ESP_IOExpander_TCA95xx_8bit(ESP_PANEL_LCD_TOUCH_BUS_HOST_ID, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, ESP_PANEL_LCD_TOUCH_I2C_IO_SCL, ESP_PANEL_LCD_TOUCH_I2C_IO_SDA);
    expander->init();
    expander->begin();
    /* Add into panel for 3-wire SPI */
    panel->addIOExpander(expander);
    /* For the newest version sub board, need to set `ESP_PANEL_LCD_RGB_IO_VSYNC` to high before initialize LCD */
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
#if ESP_PANEL_LCD_BUS_TYPE != ESP_PANEL_BUS_TYPE_RGB
    SemaphoreHandle_t sem_lcd_trans_done = xSemaphoreCreateBinary();
    assert(sem_lcd_trans_done);
    /* Register a function to notify when the panel is ready to refresh */
    /* This is useful for refreshing the screen using DMA transfers */
    panel->getLcd()->setCallback(lcd_trans_done_callback, sem_lcd_trans_done);
#endif
    /* Start panel */
    panel->begin();

    Serial.println("Draw color bar from top to bottom, the order is B - G - R");
    uint16_t line_per_bar = ESP_PANEL_LCD_V_RES / ESP_PANEL_LCD_COLOR_BITS;
    uint16_t *color = (uint16_t *)calloc(1, line_per_bar * ESP_PANEL_LCD_H_RES * ESP_PANEL_LCD_COLOR_BITS / 8);
    for (int j = 0; j < ESP_PANEL_LCD_COLOR_BITS; j++) {
        for (int i = 0; i < line_per_bar * ESP_PANEL_LCD_H_RES; i++) {
#if ESP_PANEL_LCD_BUS_TYPE == ESP_PANEL_BUS_TYPE_SPI
            color[i] = SPI_SWAP_DATA_TX(1ULL << j, ESP_PANEL_LCD_COLOR_BITS);
#else
            color[i] = 1ULL << j;
#endif
        }
        panel->getLcd()->drawBitmap(0, j * line_per_bar, ESP_PANEL_LCD_H_RES, (j + 1) * line_per_bar, color);
#if ESP_PANEL_LCD_BUS_TYPE != ESP_PANEL_BUS_TYPE_RGB
        xSemaphoreTake(sem_lcd_trans_done, portMAX_DELAY);
#endif
    }
    free(color);

    Serial.println("Setup done");
}

void loop()
{
    Serial.println("Loop");
    sleep(1);
}
