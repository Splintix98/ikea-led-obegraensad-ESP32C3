#pragma once

#include <Arduino.h>

// Set to 1 to run a barebones panel hardware test without WiFi, webserver, plugins, or button handling.
#define PANEL_TEST_MODE 0

// disable if you do not want to have online functionality
#define ENABLE_SERVER

// Network pixel input plugins. Disabled by default to keep the plugin list focused.
#define ENABLE_DDP_PLUGIN 0
#define ENABLE_ARTNET_PLUGIN 0

#if defined(CONFIG_IDF_TARGET_ESP32C3)

#define PIN_ENABLE 0
#define PIN_DATA   6
#define PIN_CLOCK  4
#define PIN_LATCH  7
#define PIN_BUTTON 5
#define PIN_PRESENCE -1

#elif defined(ESP32)

#define PIN_ENABLE 26
#define PIN_DATA   27
#define PIN_CLOCK  14
#define PIN_LATCH  12
#define PIN_BUTTON 16

#elif defined(ESP8266)

#define PIN_ENABLE 16
#define PIN_DATA   13
#define PIN_CLOCK  14
#define PIN_LATCH  0
#define PIN_BUTTON 2

#endif

#ifndef PIN_PRESENCE
#define PIN_PRESENCE -1
#endif

// disable if you do not want to use the internal storage
// https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
// timer1 on esp8266 is not compatible with flash file system reads
#ifndef ESP8266
#define ENABLE_STORAGE
#endif

#ifdef ENABLE_SERVER
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.json
#define NTP_SERVER "de.pool.ntp.org"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"
#endif

#define COLS 16
#define ROWS 16

// Display constants
constexpr uint8_t MAX_BRIGHTNESS = 255;
constexpr uint16_t TOTAL_PIXELS = ROWS * COLS;

// set your city, postal code or coords (https://github.com/chubin/wttr.in)
#define WEATHER_LOCATION "31311 Uetze Niedersachsen Deutschland"

// name of WiFi created by the device if no known WiFi is available
#define WIFI_MANAGER_SSID "IKEA"

// use ALL of the following to use static IP config
/*
#define IP_ADDRESS "192.168.0.250"
#define SUBNET "255.255.255.0"
#define DNS1 "1.1.1.1"
#define DNS2 "8.8.8.8"
#define GWY "192.168.0.1"
*/

// ---------------

enum SYSTEM_STATUS
{
  NONE,
  WSBINARY,
  UPDATE,
  LOADING,
};

extern volatile SYSTEM_STATUS currentStatus;
