#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "ota_updater.h"

#define OTA_WIFI_SSID       "BluControl OTA"
#define OTA_WIFI_PASS       "BluControl"
#define OTA_WIFI_CHANNEL    0
#define OTA_MAX_STA_CONN    1

#ifdef CONFIG_BLUCONTROL_OTA_LED_GPIO
    #define OTA_LED_GPIO CONFIG_BLUCONTROL_OTA_LED_GPIO
#else
    #define OTA_LED_GPIO -1
#endif

#endif