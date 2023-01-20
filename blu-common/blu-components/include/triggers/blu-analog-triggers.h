#ifndef BLU_ANALOG_TRIGER_H
#define BLU_ANALOG_TRIGER_H

#include "blu-hardware.h"
#include "blu-analogs.h"
#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

// Hardware
#if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
    #ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG
        #define LEFT_TRIGGER_GPIO   CONFIG_BLUCONTROL_LEFT_TRIGGER_GPIO

        #if CONFIG_BLUCONTROL_LEFT_TRIGGER_IS_INVERTED
            #define LEFT_TRIGGER_MULTIPLIER -1
        #else
            #define LEFT_TRIGGER_MULTIPLIER 1
        #endif
    #else
        #define LEFT_TRIGGER_GPIO   -1
    #endif

    #ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG
        #define RIGHT_TRIGGER_GPIO  CONFIG_BLUCONTROL_RIGHT_TRIGGER_GPIO

        #if CONFIG_BLUCONTROL_RIGHT_TRIGGER_IS_INVERTED
            #define RIGHT_TRIGGER_MULTIPLIER -1
        #else
            #define RIGHT_TRIGGER_MULTIPLIER 1
        #endif
    #else
        #define RIGHT_TRIGGER_GPIO  -1
    #endif
#endif

//Flags
#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG
    #define BLU_ANALOG_TRIGGER_LEFT 0
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG
    #define BLU_ANALOG_TRIGGER_RIGHT 1
#endif

void blu_analog_triggers_init(void);
int blu_analog_trigger_get_value(char trigger);

#endif