/**
 * The example demonstrates how to port LVGL.
 *
 * ## How to Use
 *
 * To use this example, please firstly install `ESP32_Display_Panel` (including its dependent libraries) and
 * `lvgl` (v8.3.x) libraries, then follow the steps to configure them:
 *
 * 1. [Configure ESP32_Display_Panel](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configure-esp32_display_panel)
 * 2. [Configure LVGL](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configure-lvgl)
 * 3. [Configure Board](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configure-board)
 *
 * ## Example Output
 *
 * ```bash
 * ...
 * Hello LVGL! V8.3.8
 * I am ESP32_Display_Panel
 * Starting LVGL task
 * Setup done
 * Loop
 * Loop
 * Loop
 * Loop
 * ...
 * ```
 */

#include <Arduino.h>
#include <lvgl.h>
#include <ESP_Panel_Library.h>
#include "bsp_lvgl_port.h"

/**
/* To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 * You also need to copy `lvgl/examples` to `lvgl/src/examples`. Similarly for the demos `lvgl/demos` to `lvgl/src/demos`.
 */
#include <demos/lv_demos.h>
// #include <examples/lv_examples.h>

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */

    delay(2000);

    String LVGL_Arduino = "Hello LVGL! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println(LVGL_Arduino);
    Serial.println("I am ESP32_Display_Panel");

    bsp_lvgl_port_init(NULL, NULL);

    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    bsp_lvgl_port_lock(-1);

    // /* Create simple label */
    // lv_obj_t *label = lv_label_create(lv_scr_act());
    // lv_label_set_text(label, LVGL_Arduino.c_str());
    // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    /**
     * Try an example. Don't forget to uncomment header.
     * See all the examples online: https://docs.lvgl.io/master/examples.html
     * source codes: https://github.com/lvgl/lvgl/tree/e7f88efa5853128bf871dde335c0ca8da9eb7731/examples
     */
    //  lv_example_btn_1();

    /**
     * Or try out a demo.
     * Don't forget to uncomment header and enable the demos in `lv_conf.h`. E.g. `LV_USE_DEMOS_WIDGETS`
     */
    // lv_demo_widgets();
    // lv_demo_benchmark();
    lv_demo_music();
    // lv_demo_stress();

    /* Release the mutex */
    bsp_lvgl_port_unlock();

    Serial.println("Setup done");
}

void loop()
{
    Serial.println("Loop");
    delay(1000);
}
