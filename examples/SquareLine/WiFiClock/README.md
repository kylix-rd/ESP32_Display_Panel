# Squareline - Simple Wi-Fi Clock Example

This example implements a simple Wi-Fi clock demo, which UI is created by Squareline Studio. And this example currently supports the following boards:

* ESP-S3-BOX
* ESP-S3-BOX(beta)
* ESP-S3-BOX-3
* ESP32_S3_KORVO
* ESP32_S3_LCD_EV_BOARD
* ESP32_S3_LCD_EV_BOARD-2

## How to Use

To use this example, please firstly install `ESP32_Display_Panel` (including its dependent libraries) and the following libraries:

   * lvgl (v8.3.x)
   * NTPClient (v3.2.1)
   * ArduinoJson (v6.21.3)

Then follow the steps below to configure the example.

1. [Configure ESP32_Display_Panel](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configure-esp32_display_panel)
2. [Port the Squareline Project](https://github.com/esp-arduino-libs/ESP32_Display_Panel#port-the-squareline-project) (see its step `4`)
3. [Configure LVGL](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configure-lvgl)
4. [Configure Board](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configure-board)

## Configure Functions

To obtain weather information after connecting to Wi-Fi, please follow these steps:

1. Register an account on [OpenWeather](https://openweathermap.org/) and obtain an **API KEY**.
2. Fill the obtained API KEY in the macro definition `WEATHER_API_KEY`.
3. Fill the name of the city for which need to obtain weather information (such as `Shanghai`) in the macro definition `WEATHER_CITY`.

To obtain time information after connecting to Wi-Fi, please follow these steps:

1. Fill in the NTP server URL in the macro definition `SERVER_IP`, for example, `ntp6.aliyun.com`.
2. Adjust the macro `TIME_OFFSET` and `TIME_UPDATE_INTERVAL` for time calibration.

## Example Output

```bash
...
Squareline Clock init begin!
Setup done
...
```
