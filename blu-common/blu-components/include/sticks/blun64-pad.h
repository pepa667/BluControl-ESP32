#ifndef BLUN64_PAD_H
#define BLUN64_PAD_H

#include "blu-hardware.h"
#include "driver/gpio.h"

// Bit Masks
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_N64) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_N64)
    #ifdef CONFIG_BLUCONTROL_LEFT_STICK_N64
        #define LEFT_STICK_X_INT    CONFIG_BLUCONTROL_LEFT_STICK_X1
        #define LEFT_STICK_X_Q      CONFIG_BLUCONTROL_LEFT_STICK_X2
        #define LEFT_STICK_Y_INT    CONFIG_BLUCONTROL_LEFT_STICK_Y1
        #define LEFT_STICK_Y_Q      CONFIG_BLUCONTROL_LEFT_STICK_Y2

        #define LEFT_STICK_INT_BIT_MASK ((1ULL<<LEFT_STICK_X_INT) | (1ULL<<LEFT_STICK_Y_INT))
        #define LEFT_STICK_Q_BIT_MASK   ((1ULL<<LEFT_STICK_X_Q) | (1ULL<<LEFT_STICK_Y_Q))
    #else
        #define LEFT_STICK_X_INT    -1
        #define LEFT_STICK_X_Q      -1
        #define LEFT_STICK_Y_INT    -1
        #define LEFT_STICK_Y_Q      -1

        #define LEFT_STICK_INT_BIT_MASK 0
        #define LEFT_STICK_Q_BIT_MASK   0
    #endif

    #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_N64
        #define RIGHT_STICK_X_INT   CONFIG_BLUCONTROL_RIGHT_STICK_X1
        #define RIGHT_STICK_X_Q     CONFIG_BLUCONTROL_RIGHT_STICK_X2
        #define RIGHT_STICK_Y_INT   CONFIG_BLUCONTROL_RIGHT_STICK_Y1
        #define RIGHT_STICK_Y_Q     CONFIG_BLUCONTROL_RIGHT_STICK_Y2

        #define RIGHT_STICK_INT_BIT_MASK ((1ULL<<RIGHT_STICK_X_INT) | (1ULL<<RIGHT_STICK_Y_INT))
        #define RIGHT_STICK_Q_BIT_MASK   ((1ULL<<RIGHT_STICK_X_Q) | (1ULL<<RIGHT_STICK_Y_Q))
    #else
        #define RIGHT_STICK_X_INT   -1
        #define RIGHT_STICK_X_Q     -1
        #define RIGHT_STICK_Y_INT   -1
        #define RIGHT_STICK_Y_Q     -1

        #define RIGHT_STICK_INT_BIT_MASK 0
        #define RIGHT_STICK_Q_BIT_MASK   0
    #endif
#else
    #define LEFT_STICK_INT_BIT_MASK 0
    #define LEFT_STICK_Q_BIT_MASK   0
    #define RIGHT_STICK_INT_BIT_MASK 0
    #define RIGHT_STICK_Q_BIT_MASK   0
#endif

typedef struct
{
    int value;
    gpio_num_t interrupt_pin;
    gpio_num_t counter_pin;
} n64_joystick_axis_t;

typedef struct 
{
    n64_joystick_axis_t x_axis;
    n64_joystick_axis_t y_axis;
} n64_joystick_data_t;

#ifdef CONFIG_BLUCONTROL_LEFT_STICK_N64
extern n64_joystick_data_t n64_left_joystick_data;
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_STICK_N64
extern n64_joystick_data_t n64_right_joystick_data;
#endif

// Flags
#define N64_JOYSTICK_X_AXIS 0
#define N64_JOYSTICK_Y_AXIS 1

void blun64_init(void);

#endif