#ifndef MAIN_H
#define MAIN_H

#include "blucontrol_mode.h"
#include "blu-hardware.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hoja_includes.h"
#include "core_switch_backend.h"
#include <string.h>


#if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
    #include "blu-analog-pad.h"
#endif
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
    #include "blu-buttons-pad.h"
#endif
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_N64) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_N64)
    #include "blun64-pad.h"
#endif

#endif