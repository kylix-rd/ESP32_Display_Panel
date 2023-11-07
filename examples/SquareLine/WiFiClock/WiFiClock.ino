/**
 * This example implements a simple Wi-Fi clock demo, which UI is created by Squareline Studio.
 *
 * This example currently supports the boards including **ESP-S3-BOX**, **ESP-S3-BOX(beta)**, **ESP-S3-BOX-3**, **ESP32_S3_KORVO**, **ESP32_S3_LCD_EV_BOARD** and **ESP32_S3_LCD_EV_BOARD-2**.
 *
 * ## How to Use
 *
 * To use this example, please firstly install `ESP32_Display_Panel` (including its dependent libraries) and the following libraries:
 *
 *    * lvgl (v8.3.x)
 *    * NTPClient (v3.2.1)
 *    * ArduinoJson (v6.21.3)
 *
 * Then follow the steps below to configure the example.
 *
 * 1. [Configure ESP32_Display_Panel](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configure-esp32_display_panel)
 * 2. [Port the Squareline Project](https://github.com/esp-arduino-libs/ESP32_Display_Panel#port-the-squareline-project) (see its step `4`)
 * 3. [Configure Board](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configure-board)
 *
 * ## Configure Functions
 *
 * To obtain weather information after connecting to Wi-Fi, please follow these steps:
 *
 * 1. Register an account on [OpenWeather](https://openweathermap.org/) and obtain an **API KEY**.
 * 2. Fill the obtained API KEY in the macro definition `WEATHER_API_KEY`.
 * 3. Fill the name of the city for which need to obtain weather information (e.g. `Shanghai`) in the macro definition `WEATHER_CITY`.
 *
 * To obtain time information after connecting to Wi-Fi, please follow these steps:
 *
 * 1. Fill in the NTP server URL in the macro definition `SERVER_IP`, e.g. `ntp6.aliyun.com`.
 * 2. Adjust the macro `TIME_OFFSET_S` and `TIME_UPDATE_INTERVAL_US` for time calibration.
 *
 * ## Example Output
 *
 * ```bash
 * ...
 * Squareline Clock init begin!
 * Setup done
 * ...
 * ```
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <Arduino.h>
#include "esp_flash.h"
#include "nvs.h"
#include "nvs_flash.h"

#include <lvgl.h>
#include <ESP_Panel_Library.h>
#include <ESP_IOExpander_Library.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <ui.h>

/* Here are some macros need to be filled */
#define WEATHER_API_KEY         "91c57a80e449b8837b06b38d2460ee69"    // Fill in the OpenWeather API KEY
#define WEATHER_CITY            "Shanghai"                            // Fill in the city name, e.g. "Shanghai"
#define SERVER_IP               "ntp6.aliyun.com"                     // Fill in the NTP server URL, e.g. "ntp6.aliyun.com"

/* Here are some macros can be changed */
#define GEO_API_BASE_URL        "http://api.openweathermap.org/geo/1.0/direct?q="
#define API_KEY_SUFFIX          "&limit=1&appid="
#define OPENWEATHER_BASE_URL    "https://api.openweathermap.org/data/2.5/weather?lat="
#define LAT_PARAM               "&lon="
#define API_KEY_PARAM           "&appid="
#define NVS_PART_NAME           "nvs"
#define NVS_PART_NAMESPACE      "result"
#define WIFI_NAME_LEN_MAX       20         // Maximum length for Wi-Fi name
#define WIFI_PASSWORD_LEN_MAX   20         // Maximum length for Wi-Fi password
#define ALARM_ROL_LEN           10         // Maximum length for alarm time rollers
#define ALARM_TIME_LEN          30         // Maximum length for alarm time
#define DATE_INFO_LEN           20         // Maximum length for date information
#define TIME_OFFSET_S           28800      // Time offset (in seconds)
#define TIME_UPDATE_INTERVAL_US 60000      // Time update interval (in milliseconds)
#define TIME_INFO_LEN           10         // Maximum length for time information
#define WEEK_INFO_LEN           20         // Maximum length for week information
#define TEMPERATURE_LEN         20         // Maximum length for temperature information
#define LV_BUF_SIZE             (ESP_PANEL_LCD_H_RES * 20)

ESP_Panel *panel = NULL;
SemaphoreHandle_t lvgl_mux = NULL;

nvs_handle_t nvs_flash_handle;

bool nvs_init_flag = false;         //Control and ensure that NVS initialization is performed only once
int8_t nvs_int = 0;
bool wifi_connected_flag = false;   // Flag indicating whether Wi-Fi is connected
bool wifi_list_switch = false;      // Flag indicating whether Wi-Fi list is opened
bool password_valid_flag = false;   // Flag indicating whether Wi-Fi password is being authenticated
bool alarm_flag = false;            // Flag indicating whether the alarm has been triggered

/* Define variables for storing WiFi credentials */
char st_wifi_name[WIFI_NAME_LEN_MAX];
char st_wifi_password[WIFI_PASSWORD_LEN_MAX];
const char *selected_wifi_name = NULL;
const char *wifi_password = NULL;

/* Define variables for alarm time in hours, minutes, and seconds */
char hours_rol[ALARM_ROL_LEN];
char min_rol[ALARM_ROL_LEN];
char sec_rol[ALARM_ROL_LEN];
char init_hours_rol[ALARM_ROL_LEN];
char init_min_rol[ALARM_ROL_LEN];
char init_sec_rol[ALARM_ROL_LEN];
uint16_t init_hour_roll_set = 0;
uint16_t init_min_roll_set = 0;
uint16_t init_sec_roll_set = 0;
char alarm_time[ALARM_TIME_LEN];

/* Variables for storing WiFi network information */
lv_obj_t * wifi_list = NULL;
int num_wifi = 0;
char date_string[DATE_INFO_LEN];
int connected_count = 0;
int nvs_wifi_connect_count = 0;

/* NTP Client for obtaining time information */
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, SERVER_IP, TIME_OFFSET_S, TIME_UPDATE_INTERVAL_US);

/* HTTP Client for making HTTP requests */
HTTPClient http;

/* OpenWeatherMap API configuration */
String openweather_api_key = WEATHER_API_KEY;
String openweather_city = WEATHER_CITY;
String url_openweather = "";
String url_lat_lon = "";
String weather_url = "";
String lat_lon_url = "";
String Weather = "";
String prev_weather = "";
String lat = "";
String lon = "";
int temperature = 999;
char temperature_buffer[TEMPERATURE_LEN];

/* Variables for time and date information */
time_t now;
struct tm timeinfo;
char time_str[TIME_INFO_LEN] = "12:59:59";
char weekday_str[WEEK_INFO_LEN];

/**
 * LVGL Porting Functions
 *
 * These code provide functions for handling display flushing, touch input, and LVGL task
 *
 */
#if ESP_PANEL_LCD_BUS_TYPE == ESP_PANEL_BUS_TYPE_RGB
/* Display flushing */
void lvgl_port_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    panel->getLcd()->drawBitmap(area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
    lv_disp_flush_ready(disp);
}
#else
/* Display flushing */
void lvgl_port_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    panel->getLcd()->drawBitmap(area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
}

bool notify_lvgl_flush_ready(void *user_ctx)
{
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}
#endif /* ESP_PANEL_LCD_BUS_TYPE */

#if ESP_PANEL_USE_LCD_TOUCH
/* Read the touchpad */
void lvgl_port_touchpad_read(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    panel->getLcdTouch()->readData();

    bool touched = panel->getLcdTouch()->getTouchState();
    if(!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        TouchPoint point = panel->getLcdTouch()->getPoint();

        data->state = LV_INDEV_STATE_PR;
        /*Set the coordinates*/
        data->point.x = point.x;
        data->point.y = point.y;

        // Serial.printf("Touch point: x %d, y %d\n", point.x, point.y);
    }
}
#endif

bool lvgl_port_lock(uint32_t timeout_ms)
{
    const TickType_t timeout_ticks = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTakeRecursive(lvgl_mux, timeout_ticks) == pdTRUE;
}

void lvgl_port_unlock(void)
{
    xSemaphoreGiveRecursive(lvgl_mux);
}

void lvgl_task(void *pvParameter)
{
    while (1) {
        lvgl_port_lock(-1);
        lv_task_handler();
        lvgl_port_unlock();
        delay(5);
    }
}

/** NVS Utility Functions **/
/** This set of functions provides a simple interface for reading and writing data to NVS memory on ESP32 **/
/*  Write a string to the NVS */
void nvs_write_str(const char* key, const char* value)
{
    nvs_open_from_partition(NVS_PART_NAME, NVS_PART_NAMESPACE, NVS_READWRITE, &nvs_flash_handle);

    nvs_set_str(nvs_flash_handle, key, value);
    nvs_commit(nvs_flash_handle);
    nvs_close(nvs_flash_handle);
}

/* Read a string from NVS */
void nvs_read_str(const char* key, char* nvs_str, size_t max_length)
{
    nvs_open_from_partition(NVS_PART_NAME, NVS_PART_NAMESPACE, NVS_READWRITE, &nvs_flash_handle);

    nvs_get_str(nvs_flash_handle, key, nvs_str, &max_length);
    nvs_close(nvs_flash_handle);
}

/* Write an 8-bit integer to NVS */
void nvs_write_int(const char* key, int8_t value)
{
    nvs_open_from_partition(NVS_PART_NAME, NVS_PART_NAMESPACE, NVS_READWRITE, &nvs_flash_handle);

    nvs_set_i8(nvs_flash_handle, key, value);
    nvs_commit(nvs_flash_handle);
    nvs_close(nvs_flash_handle);
}

/* Read an 8-bit integer from NVS */
int8_t nvs_read_int(const char* key)
{
    nvs_open_from_partition(NVS_PART_NAME, NVS_PART_NAMESPACE, NVS_READWRITE, &nvs_flash_handle);

    nvs_get_i8(nvs_flash_handle, key, &nvs_int);
    nvs_close(nvs_flash_handle);
    return nvs_int;
}

/** Weather Data and Utility Functions **/
/** This set of functions is designed for retrieving weather data from the OpenWeatherMap API and parsing it **/
String Getlatlon(String api,String city)
{
    url_lat_lon =  GEO_API_BASE_URL;
    url_lat_lon += city;
    url_lat_lon += API_KEY_SUFFIX;
    url_lat_lon += api;
    return url_lat_lon;
}

String GetURL(String api,String lat, String lon)
{
    url_openweather =  OPENWEATHER_BASE_URL;
    url_openweather += lat;
    url_openweather += LAT_PARAM;
    url_openweather += lon;
    url_openweather += API_KEY_PARAM;
    url_openweather += api;
    return url_openweather;
}

void ParseWeather(String url)
{
    DynamicJsonDocument doc(1024); //Allocate memory dynamically

    http.begin(url);

    int httpGet = http.GET();
    if(httpGet > 0) {
        if(httpGet == HTTP_CODE_OK) {
            String json = http.getString();
            deserializeJson(doc, json);

            Weather = doc["weather"][0]["main"].as<String>();
            temperature = doc["main"]["temp"].as<int>() - 273.15;

            Serial.printf("Weather: %s\n", Weather);
            Serial.printf("temperature: %d\n", temperature);
        } else {
            Serial.printf("ERROR: HTTP_CODE\n");
        }
    } else {
        Serial.printf("ERROR: httpGet\n");
    }

    http.end();
}

void Parselatlon(String url)
{
    DynamicJsonDocument doc(4096); //Allocate memory dynamically

    http.begin(url);

    int httpGet = http.GET();
    if(httpGet > 0) {
        if(httpGet == HTTP_CODE_OK) {
            String json = http.getString();
            deserializeJson(doc, json);

            lat = doc[0]["lat"].as<String>();
            lon = doc[0]["lon"].as<String>();

            Serial.printf("lat: %s\n", lat);
            Serial.printf("lon: %s\n", lon);
        } else {
            Serial.printf("ERROR: HTTP_CODE\n");
        }
    } else {
        Serial.printf("ERROR: httpGet\n");
    }

    http.end();
}

void splitTime(char *timeStr, char *init_hour_rol, char *init_min_rol, char *init_sec_rol) {
    char *token = strtok(timeStr, ":");
    if (token != NULL) {
        strcpy(init_hour_rol, token);
        token = strtok(NULL, ":");
        if (token != NULL) {
            strcpy(init_min_rol, token);
            token = strtok(NULL, ":");
            if (token != NULL) {
                strcpy(init_sec_rol, token);
            } else {
                strcpy(init_sec_rol, "00");
            }
        } else {
            strcpy(init_min_rol, "00");
            strcpy(init_sec_rol, "00");
        }
    } else {
        strcpy(init_hour_rol, "00");
        strcpy(init_min_rol, "00");
        strcpy(init_sec_rol, "00");
    }
}

/** WiFi Clock User Interface Functions **/
/** This set of functions is used to manage the user interface (UI) and interaction in a WiFi-enabled clock application **/
void onTimerClockUpdate(lv_timer_t *timer)
{
    lv_obj_t *ui_LabelTime = (lv_obj_t *) timer->user_data;

    if(wifi_connected_flag) {
        String formattedTime = timeClient.getFormattedTime();
        snprintf(time_str, sizeof(time_str), "%s", formattedTime.c_str());
    }

    lv_label_set_text_fmt(ui_LabelTime, "%s", time_str);
    if(!strcmp(alarm_time, time_str)) {
        alarm_flag = true;
        Serial.println("alarm_flag: true");
    }

}

void onCalendarUpdate(lv_timer_t *timer)
{
    lv_obj_t *ui_Labelcalendar = (lv_obj_t *) timer->user_data;
    lv_label_set_text_fmt(ui_Labelcalendar, "%s", date_string);
}

void onWeekUpdate(lv_timer_t *timer)
{
    lv_obj_t *ui_LabelWeek = (lv_obj_t *) timer->user_data;
    String week_temp_str;
    if(wifi_connected_flag) {
        if (timeClient.getDay() == 0) {
            week_temp_str = "Sunday";
        } else if (timeClient.getDay() == 1) {
            week_temp_str = "Monday";
        } else if (timeClient.getDay() == 2) {
            week_temp_str = "Tuesday";
        } else if (timeClient.getDay() == 3) {
            week_temp_str = "Wednesday";
        } else if (timeClient.getDay() == 4) {
            week_temp_str = "Thursday";
        } else if (timeClient.getDay() == 5) {
            week_temp_str = "Friday";
        } else if (timeClient.getDay() == 6) {
            week_temp_str = "Saturday";
        }
        lv_label_set_text_fmt(ui_LabelWeek, "%s", week_temp_str);
    } else {
        lv_label_set_text_fmt(ui_LabelWeek, "%s", weekday_str);
    }

    if(week_temp_str == "Wednesday" || strcmp(weekday_str, "Wednesday") == 0) {
        lv_obj_set_x( ui_Labelweek, 80 );
    }
}

void onWeatherUpdate(lv_timer_t *timer)
{
    if(wifi_connected_flag) {
        lv_obj_t *ui_Labelweather = (lv_obj_t *) timer->user_data;
        lvgl_port_lock(-1);
        lv_label_set_text_fmt(ui_Labelweather, "%s", Weather);
        if(prev_weather != Weather) {
            if(Weather == "Clear") {
                _ui_flag_modify(ui_ImageCloudy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageRain, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSnow, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSunny, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                _ui_flag_modify(ui_ImageThunderstor, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageAtmosphere, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageDrizzle, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            } else if(Weather == "Clouds") {
                _ui_flag_modify(ui_ImageCloudy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                _ui_flag_modify(ui_ImageRain, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSnow, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSunny, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageThunderstor, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageAtmosphere, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageDrizzle, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            } else if(Weather == "Rain") {
                _ui_flag_modify(ui_ImageCloudy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageRain, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                _ui_flag_modify(ui_ImageSnow, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSunny, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageThunderstor, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageAtmosphere, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageDrizzle, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            } else if(Weather == "Snow") {
                _ui_flag_modify(ui_ImageCloudy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageRain, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSnow, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                _ui_flag_modify(ui_ImageSunny, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageThunderstor, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageAtmosphere, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageDrizzle, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            } else if(Weather == "Drizzle") {
                _ui_flag_modify(ui_ImageCloudy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageRain, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSnow, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSunny, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageThunderstor, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageAtmosphere, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageDrizzle, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            } else if(Weather == "Thunderstorm") {
                _ui_flag_modify(ui_ImageCloudy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageRain, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSnow, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSunny, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageThunderstor, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                _ui_flag_modify(ui_ImageAtmosphere, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageDrizzle, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            } else if(Weather == "Mist" || Weather == "Smoke" || Weather == "Haze" || Weather == "Dust" || \
                      Weather == "Fog" || Weather == "Sand" || Weather == "Dust") {
                _ui_flag_modify(ui_ImageCloudy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageRain, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSnow, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSunny, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageThunderstor, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageAtmosphere, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                _ui_flag_modify(ui_ImageDrizzle, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            } else {
                _ui_flag_modify(ui_ImageCloudy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageRain, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSnow, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageSunny, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageThunderstor, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageAtmosphere, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ImageDrizzle, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            }
        }
        lvgl_port_unlock();
        prev_weather = Weather;
    }
}

void onTemperatureUpdate(lv_timer_t *timer)
{
    if(wifi_connected_flag) {
        lv_obj_t *ui_Labeltemperature = (lv_obj_t *) timer->user_data;
        if(temperature != 999) {
            sprintf(temperature_buffer, "%d℃", temperature);
        }
        lv_label_set_text_fmt(ui_Labeltemperature, "%s", temperature_buffer);
    }
}

void ui_timer_init()
{
    lv_timer_t *LabelTime_Wifitimer = lv_timer_create(onTimerClockUpdate, 1000, (void *) ui_LabelTime);
    onTimerClockUpdate(LabelTime_Wifitimer);
    lv_timer_t *Labelcalendar_timer = lv_timer_create(onCalendarUpdate, 1000, (void *) ui_Labelcalendar);
    onCalendarUpdate(Labelcalendar_timer);
    lv_timer_t *Labelweek_timer = lv_timer_create(onWeekUpdate, 1000, (void *) ui_Labelweek);
    onWeekUpdate(Labelweek_timer);
    lv_timer_t *Labelweather_timer = lv_timer_create(onWeatherUpdate, 1000, (void *) ui_Labelweather);
    onWeatherUpdate(Labelweather_timer);
    lv_timer_t *Labeltemperature_timer = lv_timer_create(onTemperatureUpdate, 1000, (void *) ui_Labeltemperature);
    onTemperatureUpdate(Labeltemperature_timer);
}

void WifiListClicked_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if (event_code == LV_EVENT_CLICKED) {
        wifi_list_switch = false;
        Serial.println("wifi_list_switch: false");

        lvgl_port_lock(-1);
        selected_wifi_name = lv_list_get_btn_text(wifi_list, target);
        if (selected_wifi_name != NULL) {
            Serial.printf("%s\n", selected_wifi_name);
        }
        _ui_screen_change(&ui_ScreenPassord, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenPassord_screen_init);
        lvgl_port_unlock();
    }
}

void keyboard_event_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if(event_code == LV_EVENT_CLICKED && lv_keyboard_get_selected_btn(target) == 39) {
        wifi_password = lv_textarea_get_text(ui_TextPassword);
        Serial.printf("%s\n", wifi_password);

        WiFi.begin(selected_wifi_name, wifi_password);

        password_valid_flag = true;
        Serial.println("password_valid_flag: true");

        _ui_flag_modify(ui_SpinnerLoadPassword, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_KeyboardPassword, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    }
}

void Ala_confirm_cb(lv_event_t * e)
{
    lv_roller_get_selected_str(ui_Rollerhour, hours_rol, sizeof(hours_rol));
    lv_roller_get_selected_str(ui_Rollerminute, min_rol, sizeof(hours_rol));
    lv_roller_get_selected_str(ui_Rollersecond, sec_rol, sizeof(min_rol));

    strcpy(alarm_time, hours_rol);
    strcat(alarm_time, ":");
    strcat(alarm_time, min_rol);
    strcat(alarm_time, ":");
    strcat(alarm_time, sec_rol);
    Serial.printf("alarm_time: %s\n", alarm_time);

    _ui_screen_change(&ui_ScreenClock, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenClock_screen_init);
}

void Factory_cb(lv_event_t * e)
{
    lvgl_port_lock(-1);
    _ui_flag_modify(ui_ImageWifi, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    lvgl_port_unlock();

    http.end();
    WiFi.disconnect();

    wifi_connected_flag = false;
    Serial.println("wifi_connected_flag: false");
    nvs_write_int("NVS_WifiFg", wifi_connected_flag);
}

void ButtonAlarm_cb(lv_event_t * e)
{
    splitTime(time_str, init_hours_rol, init_min_rol, init_sec_rol);

    init_hour_roll_set = atoi(&init_hours_rol[0]);
    init_min_roll_set = atoi(&init_min_rol[0]);
    init_sec_roll_set = atoi(&init_sec_rol[0]);

    lv_roller_set_selected(ui_Rollerhour, init_hour_roll_set, LV_ANIM_OFF);
    lv_roller_set_selected(ui_Rollerminute, init_min_roll_set, LV_ANIM_OFF);
    lv_roller_set_selected(ui_Rollersecond, init_sec_roll_set, LV_ANIM_OFF);
}

void ButtonSetWifi_cb(lv_event_t * e)
{
    wifi_list_switch = true;
}

void ButtonRetSet_cb(lv_event_t * e)
{
    wifi_list_switch = false;
}

void ButtonRetWifi_cb(lv_event_t * e)
{
    wifi_list_switch = true;
}

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */
    Serial.println("Squareline Clock init begin!");

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        Serial.println("nvs init err");
    }

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    setenv("TZ", "CST-8", 1);
    tzset();

    prev_weather = "Clear";

    panel = new ESP_Panel();

    /* Initialize LVGL core */
    lv_init();

    /* Initialize LVGL buffers */
    static lv_disp_draw_buf_t draw_buf;
    /* Using double buffers is more faster than single buffer */
    /* Using internal SRAM is more fast than PSRAM (Note: Memory allocated using `malloc` may be located in PSRAM.) */
    uint8_t *buf = (uint8_t *)heap_caps_calloc(1, LV_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_INTERNAL);
    assert(buf);
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LV_BUF_SIZE);

    /* Initialize the display device */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = ESP_PANEL_LCD_H_RES;
    disp_drv.ver_res = ESP_PANEL_LCD_V_RES;
    disp_drv.flush_cb = lvgl_port_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

#if ESP_PANEL_USE_LCD_TOUCH
    /* Initialize the input device */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_port_touchpad_read;
    lv_indev_drv_register(&indev_drv);
#endif

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

    /* Initialize bus and device of panel */
    panel->init();
#if ESP_PANEL_LCD_BUS_TYPE != ESP_PANEL_BUS_TYPE_RGB
    /* Register a function to notify LVGL when the panel is ready to flush */
    /* This is useful for refreshing the screen using DMA transfers */
    panel->getLcd()->setCallback(notify_lvgl_flush_ready, &disp_drv);
#endif
    /* Start panel */
    panel->begin();

    /* Create a task to run the LVGL task periodically */
    lvgl_mux = xSemaphoreCreateRecursiveMutex();
    xTaskCreate(lvgl_task, "lvgl", 8192, NULL, 1, NULL);

    /**
     * To avoid errors caused by multiple tasks simultaneously accessing LVGL,
     * should acquire a lock before operating on LVGL.
     */
    lvgl_port_lock(-1);

    ui_init();
    ui_timer_init();

    lvgl_port_unlock();

    Serial.println("Setup done");
}

void loop()
{
    if(!nvs_init_flag) {
        if(nvs_read_int("NVS_WifiFg")) {
            nvs_read_str("NVS_WifiNa", st_wifi_name, sizeof(st_wifi_name));
            nvs_read_str("NVS_WifiPw", st_wifi_password, sizeof(st_wifi_password));
            selected_wifi_name = st_wifi_name;
            wifi_password = st_wifi_password;
            Serial.printf("NVS: selected_wifi_name: %s\n", selected_wifi_name);
            Serial.printf("NVS: wifi_password: %s\n", wifi_password);

            if(wifi_password[0] != '\0' && selected_wifi_name[0] != '\0') {
                WiFi.begin(selected_wifi_name, wifi_password);
            }

            while(WiFi.status() != WL_CONNECTED) {
                Serial.println("Wifi connecting...");
                Serial.printf("NVS: selected_wifi_name: %s\n", selected_wifi_name);
                Serial.printf("NVS: wifi_password: %s\n", wifi_password);

                nvs_wifi_connect_count++;

                if(nvs_wifi_connect_count >= 5) {
                    Serial.printf("NVS Wifi Connecting Fail\n");
                    WiFi.disconnect();
                    nvs_wifi_connect_count = 0;
                    break;
                }
                delay(1000);
            }

            if(WiFi.status() == WL_CONNECTED) {
                Serial.printf("NVS Wifi Connecting Success\n");
                lat_lon_url = Getlatlon(openweather_api_key, openweather_city);
                while(lat == NULL && lon == NULL) {
                    Parselatlon(lat_lon_url);
                }
                weather_url = GetURL(openweather_api_key, lat, lon);

                wifi_connected_flag = true;
                Serial.printf("wifi_connected_flag: true\n");
            }
        } else {
            wifi_connected_flag = false;
            Serial.printf("wifi_connected_flag: false\n");
        }
        nvs_init_flag = true;
        Serial.printf("nvs_init_flag: true\n");
        nvs_wifi_connect_count = 0;
    }

    if(wifi_list_switch) {
        num_wifi = WiFi.scanNetworks();
        Serial.println("Scan done");

        if(num_wifi == 0) {
            Serial.println("no networks found");
        } else if(wifi_list_switch) {
            wifi_list_switch = false;
            Serial.println("wifi_list_switch: false");

            Serial.println("Wifi list show:");

            lvgl_port_lock(-1);

            _ui_flag_modify(ui_SpinnerLoadWifi, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

            if(wifi_list != NULL) {
                lv_obj_del(wifi_list);
            }
            wifi_list = lv_list_create(lv_scr_act());
            lv_obj_set_size(wifi_list, 300, 180);
            for (int i = 0; i < num_wifi; i++) {
                lv_obj_t *wifi_list_Item = lv_list_add_btn(wifi_list, NULL, WiFi.SSID(i).c_str());
                lv_obj_set_user_data(wifi_list_Item, (void *)WiFi.SSID(i).c_str());
                lv_obj_add_event_cb(wifi_list_Item, WifiListClicked_cb, LV_EVENT_ALL, NULL);
            }
            lv_obj_align(wifi_list, LV_ALIGN_CENTER, 0, 30);

            lvgl_port_unlock();
        }
        WiFi.scanDelete();
    }

    if(password_valid_flag) {
        Serial.println("Wifi connecting...");
        Serial.printf("%s\n",  selected_wifi_name);
        Serial.printf("%s\n", wifi_password);
        connected_count++;

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("password correct: Wifi connected success");

            lat_lon_url = Getlatlon(openweather_api_key, openweather_city);
            while(lat == NULL && lon == NULL) {
                Parselatlon(lat_lon_url);
            }
            weather_url = GetURL(openweather_api_key, lat, lon);

            nvs_write_str("NVS_WifiNa", selected_wifi_name);
            nvs_write_str("NVS_WifiPw", wifi_password);

            wifi_connected_flag = true;
            Serial.println("wifi_connected_flag: true");
            nvs_write_int("NVS_WifiFg", wifi_connected_flag);

            password_valid_flag = false;
            Serial.println("password_valid_flag: false");

            connected_count = 0;

            lvgl_port_lock(-1);
            _ui_flag_modify(ui_SpinnerLoadPassword, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify(ui_KeyboardPassword, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_screen_change(&ui_ScreenClock, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenClock_screen_init);
            lv_textarea_set_text(ui_TextPassword, "");
            lvgl_port_unlock();
        } else if(connected_count >= 6) {
            Serial.println("password wrong: Wifi connected failed");

            wifi_connected_flag = false;
            Serial.println("password_valid_flag: false");
            nvs_write_int("NVS_WifiFg", wifi_connected_flag);

            http.end();
            WiFi.disconnect();

            password_valid_flag = false;
            Serial.println("wifi_connected_flag: false");

            connected_count = 0;

            lvgl_port_lock(-1);

            _ui_flag_modify(ui_SpinnerLoadPassword, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify(ui_KeyboardPassword, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            lv_textarea_set_text(ui_TextPassword, "");

            lvgl_port_unlock();
        }
    }

    if(wifi_connected_flag) {
        timeClient.update();
        unsigned long epochTime = timeClient.getEpochTime();
        struct tm *ptm = gmtime((time_t *)&epochTime);
        snprintf(date_string, sizeof(date_string), "%04d-%02d-%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
        Serial.print("Now time: ");
        Serial.println(timeClient.getFormattedTime());
        Serial.print("Current date: ");
        Serial.println(date_string);
        Serial.print("Current day of the week: ");
        Serial.println(timeClient.getDay());

        ParseWeather(weather_url);

        struct timeval tv = {
            .tv_sec = epochTime - 28800,
            .tv_usec = 0,
        };
        settimeofday(&tv, NULL);

        if(WiFi.status() != WL_CONNECTED) {
            wifi_connected_flag = false;
            Serial.println("wifi_connected_flag: false");
            nvs_write_int("NVS_WifiFg", wifi_connected_flag);
            http.end();
            WiFi.disconnect();
        }
    } else {
        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(time_str, sizeof(time_str), "%H:%M:%S", &timeinfo);
        strftime(date_string, sizeof(date_string), "%Y-%m-%d", &timeinfo);
        strftime(weekday_str, sizeof(weekday_str), "%A", &timeinfo);
    }

    if(alarm_flag) {
        static const char * btns[] ={""};

        lvgl_port_lock(-1);

        lv_obj_t* Alarm_mbox = lv_msgbox_create(NULL, "Alarm activated", "Click the button to turn off the alarm", btns, true);
        lv_obj_center(Alarm_mbox);

        alarm_flag = false;
        Serial.println("alarm_flag: false");

        lvgl_port_unlock();
    }

    if(wifi_connected_flag) {
        lvgl_port_lock(-1);
        _ui_flag_modify(ui_ImageWifi, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        lvgl_port_unlock();
    } else {
        lvgl_port_lock(-1);
        _ui_flag_modify(ui_ImageWifi, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        lvgl_port_unlock();
    }

    delay(500);
}
