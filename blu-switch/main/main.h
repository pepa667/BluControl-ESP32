#ifndef MAIN_H
#define MAIN_H

#include "blucontrol_mode.h"
#include "blu-hardware.h"
#include "esp_log.h"
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

#if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
    #include "blu-analog-triggers.h"
#endif

#if defined(CONFIG_BLUCONTROL_CONTROL_TYPE_N64)
    #define HOJA_CONTROL_TYPE NS_TYPE_N64
#elif defined(CONFIG_BLUCONTROL_CONTROL_TYPE_JOY_L)
    #define HOJA_CONTROL_TYPE NS_TYPE_JOYCON_L
#elif defined(CONFIG_BLUCONTROL_CONTROL_TYPE_JOY_R)
    #define HOJA_CONTROL_TYPE NS_TYPE_JOYCON_R
#elif defined(CONFIG_BLUCONTROL_CONTROL_TYPE_SNES)
    #define HOJA_CONTROL_TYPE NS_TYPE_SNES
#elif defined(CONFIG_BLUCONTROL_CONTROL_TYPE_NES)
    #define HOJA_CONTROL_TYPE NS_TYPE_NES
#elif defined(CONFIG_BLUCONTROL_CONTROL_TYPE_FC)
    #define HOJA_CONTROL_TYPE NS_TYPE_FC
#elif defined(CONFIG_BLUCONTROL_CONTROL_TYPE_GENESIS)
    #define HOJA_CONTROL_TYPE NS_TYPE_GENESIS
#else
    #define HOJA_CONTROL_TYPE NS_TYPE_PROCON
#endif

// Triggers
#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ACT
    #define LEFT_TRIGGER_ACT CONFIG_BLUCONTROL_LEFT_TRIGGER_ACT
#else
    #define LEFT_TRIGGER_ACT -1
#endif

#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ACT
    #define RIGHT_TRIGGER_ACT CONFIG_BLUCONTROL_RIGHT_TRIGGER_ACT
#else
    #define RIGHT_TRIGGER_ACT -1
#endif

#endif