#ifndef BLU_ANALOG_PAD_H
#define BLU_ANALOG_PAD_H

#include "blu-hardware.h"
#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

// Hardware
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
    #ifdef CONFIG_BLUCONTROL_LEFT_STICK_ANALOG
        #define LEFT_STICK_X    CONFIG_BLUCONTROL_LEFT_STICK_X
        #define LEFT_STICK_Y    CONFIG_BLUCONTROL_LEFT_STICK_Y

        #ifdef CONFIG_BLUCONTROL_LEFT_STICK_X_INVERTED
            #define LEFT_STICK_X_MULTIPLIER -1
        #else
            #define LEFT_STICK_X_MULTIPLIER 1
        #endif
        #ifdef CONFIG_BLUCONTROL_LEFT_STICK_Y_INVERTED
            #define LEFT_STICK_Y_MULTIPLIER -1
        #else
            #define LEFT_STICK_Y_MULTIPLIER 1
        #endif
    #else
        #define LEFT_STICK_X    -1
        #define LEFT_STICK_Y    -1
    #endif
    #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG
        #define RIGHT_STICK_X    CONFIG_BLUCONTROL_RIGHT_STICK_X
        #define RIGHT_STICK_Y    CONFIG_BLUCONTROL_RIGHT_STICK_Y

        #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_X_INVERTED
            #define RIGHT_STICK_X_MULTIPLIER -1
        #else
            #define RIGHT_STICK_X_MULTIPLIER 1
        #endif
        #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_Y_INVERTED
            #define RIGHT_STICK_Y_MULTIPLIER -1
        #else
            #define RIGHT_STICK_Y_MULTIPLIER 1
        #endif
    #else
        #define RIGHT_STICK_X    -1
        #define RIGHT_STICK_Y    -1
    #endif
#endif

typedef struct blu_analog_stick_data_t
{
    int x_axis;
    int y_axis;
} blu_analog_stick_data_t;

//Flags
#ifdef CONFIG_BLUCONTROL_LEFT_STICK_ANALOG
    #define BLU_ANALOG_PAD_LEFT 0
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG
    #define BLU_ANALOG_PAD_RIGHT 1
#endif

void blu_analog_stick_init(void);
blu_analog_stick_data_t *blu_analog_stick_get_data(char stick);

#endif