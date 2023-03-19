#ifndef BLU_BTNS_PAD_H
#define BLU_BTNS_PAD_H

#include "blu-hardware.h"
#include "driver/gpio.h"

// Bit Masks
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
    #ifdef CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS
        #define LEFT_STICK_X_MAX    CONFIG_BLUCONTROL_LEFT_STICK_X_MAX
        #define LEFT_STICK_X_MIN    CONFIG_BLUCONTROL_LEFT_STICK_X_MIN
        #define LEFT_STICK_Y_MAX    CONFIG_BLUCONTROL_LEFT_STICK_Y_MAX
        #define LEFT_STICK_Y_MIN    CONFIG_BLUCONTROL_LEFT_STICK_Y_MIN
    #else
        #define LEFT_STICK_X_MAX    -1
        #define LEFT_STICK_X_MIN    -1
        #define LEFT_STICK_Y_MAX    -1
        #define LEFT_STICK_Y_MIN    -1
    #endif
    #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS
        #define RIGHT_STICK_X_MAX   CONFIG_BLUCONTROL_RIGHT_STICK_X_MAX
        #define RIGHT_STICK_X_MIN   CONFIG_BLUCONTROL_RIGHT_STICK_X_MIN
        #define RIGHT_STICK_Y_MAX   CONFIG_BLUCONTROL_RIGHT_STICK_Y_MAX
        #define RIGHT_STICK_Y_MIN   CONFIG_BLUCONTROL_RIGHT_STICK_Y_MIN
    #else
        #define RIGHT_STICK_X_MAX   -1
        #define RIGHT_STICK_X_MIN   -1
        #define RIGHT_STICK_Y_MAX   -1
        #define RIGHT_STICK_Y_MIN   -1
    #endif

    #if CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS && CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS
        #define BOTH_STICK_BTNS_MASK ((1ULL<<LEFT_STICK_X_MAX)  | (1ULL<<LEFT_STICK_X_MIN)  | (1ULL<<LEFT_STICK_Y_MAX)  | (1ULL<<LEFT_STICK_Y_MIN) | \
                                      (1ULL<<RIGHT_STICK_X_MAX) | (1ULL<<RIGHT_STICK_X_MIN) | (1ULL<<RIGHT_STICK_Y_MAX) | (1ULL<<RIGHT_STICK_Y_MIN))
    #elif CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS
        #define BOTH_STICK_BTNS_MASK  (1ULL<<LEFT_STICK_X_MAX)  | (1ULL<<LEFT_STICK_X_MIN)  | (1ULL<<LEFT_STICK_Y_MAX)  | (1ULL<<LEFT_STICK_Y_MIN)
    #elif CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS
        #define BOTH_STICK_BTNS_MASK  (1ULL<<RIGHT_STICK_X_MAX) | (1ULL<<RIGHT_STICK_X_MIN) | (1ULL<<RIGHT_STICK_Y_MAX) | (1ULL<<RIGHT_STICK_Y_MIN)
    #else
        #define BOTH_STICK_BTNS_MASK 0
    #endif
#else
    #define BOTH_STICK_BTNS_MASK   0
#endif

typedef struct 
{
    int x_axis;
    int y_axis;
} blu_btn_stick_data_t;

//Flags
#ifdef CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS
    #define BLU_BUTTONS_PAD_LEFT 0
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS
    #define BLU_BUTTONS_PAD_RIGHT 1
#endif

void blu_buttons_stick_init(void);
blu_btn_stick_data_t *blu_buttons_stick_get_data(char stick);

#endif