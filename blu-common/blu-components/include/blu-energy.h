#ifndef BLU_ENERGY_H
#define BLU_ENERGY_H

#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

#ifdef CONFIG_BLUCONTROL_ENERGY_POWER_OFF_SECONDS
    #define POWER_OFF_SECONDS CONFIG_BLUCONTROL_ENERGY_POWER_OFF_SECONDS
#else
    #define POWER_OFF_SECONDS 60
#endif
#ifdef CONFIG_BLUCONTROL_ENERGY_POWER_LED_GPIO
    #define POWER_LED_GPIO CONFIG_BLUCONTROL_ENERGY_POWER_LED_GPIO
#else
    #define POWER_LED_GPIO -1
#endif
#ifdef CONFIG_BLUCONTROL_ENERGY_BUTTONS_LENGTH
    #define POWER_BUTTONS_LENGTH CONFIG_BLUCONTROL_ENERGY_BUTTONS_LENGTH
#else
    #define POWER_BUTTONS_LENGTH 1
#endif
#ifdef CONFIG_BLUCONTROL_ENERGY_BUTTON1_GPIO
    #define POWER_BUTTON_1 CONFIG_BLUCONTROL_ENERGY_BUTTON1_GPIO
#else
    #define POWER_BUTTON_1 -1
#endif
#ifdef CONFIG_BLUCONTROL_ENERGY_BUTTON2_GPIO
    #define POWER_BUTTON_2 CONFIG_BLUCONTROL_ENERGY_BUTTON2_GPIO
#else
    #define POWER_BUTTON_2 -1
#endif
#ifdef CONFIG_BLUCONTROL_ENERGY_BUTTON3_GPIO
    #define POWER_BUTTON_3 CONFIG_BLUCONTROL_ENERGY_BUTTON3_GPIO
#else
    #define POWER_BUTTON_3 -1
#endif

#ifdef CONFIG_BLUCONTROL_ENERGY_WAKING_MODE_LOW
    #define POWER_WAKE_MODE ESP_EXT1_WAKEUP_ALL_LOW
#else
    #define POWER_WAKE_MODE ESP_EXT1_WAKEUP_ANY_HIGH
#endif

#ifdef CONFIG_BLUCONTROL_LEDS_MODE_HIGH
    #define LED_POWER_ON 1
#else
    #define LED_POWER_ON 0
#endif

void blu_energy_init();
bool blu_energy_is_clock_running();
void blu_energy_start_clock();
void blu_energy_stop_clock();

#endif