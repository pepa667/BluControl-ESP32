#ifndef BLU_HW_H
#define BLU_HW_H

#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

#define GET_JOYSTICK_X_AXIS(val) (val + BLU_JOYSTICK_MAX_X) * BLU_JOYSTICK_ABS_MAX / BLU_JOYSTICK_MAX_X
#define GET_JOYSTICK_Y_AXIS(val) (val + BLU_JOYSTICK_MAX_Y) * BLU_JOYSTICK_ABS_MAX / BLU_JOYSTICK_MAX_Y

#define GET_ANALOG_JOYSTICK_X_AXIS(val) (val + BLU_ANALOG_JOYSTICK_MAX_X) * BLU_JOYSTICK_ABS_MAX / BLU_ANALOG_JOYSTICK_MAX_X
#define GET_ANALOG_JOYSTICK_Y_AXIS(val) (val + BLU_ANALOG_JOYSTICK_MAX_Y) * BLU_JOYSTICK_ABS_MAX / BLU_ANALOG_JOYSTICK_MAX_Y

#define GET_ANALOG_TRIGGER_AXIS(val) val * BLU_JOYSTICK_ABS_MAX / BLU_ANALOG_TRIGGER_MAX * 2
#define CHECK_IF_TRIGGER_IS_PRESSED(val, perc) perc > 0 && (val - BLU_JOYSTICK_ABS_MAX) * 100.0 / BLU_JOYSTICK_ABS_MAX >= perc

//DON'T RE-DEFINE THE BUTTONS GPIO HERE!!!
//DO IT USING THE `idf.py menuconfig` COMMAND
//READ THE README.MD FOR MORE INFO

// Buttons
#ifdef CONFIG_BLUCONTROL_BUTTON_A_GPIO
    #define BUTTON_A_PIN CONFIG_BLUCONTROL_BUTTON_A_GPIO
#else
    #define BUTTON_A_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_B_GPIO
    #define BUTTON_B_PIN CONFIG_BLUCONTROL_BUTTON_B_GPIO
#else
    #define BUTTON_B_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_Y_GPIO
    #define BUTTON_Y_PIN CONFIG_BLUCONTROL_BUTTON_Y_GPIO
#else
    #define BUTTON_Y_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_X_GPIO
    #define BUTTON_X_PIN CONFIG_BLUCONTROL_BUTTON_X_GPIO
#else
    #define BUTTON_X_PIN ""
#endif

// DPAD
#ifdef CONFIG_BLUCONTROL_DPAD_RIGHT_GPIO
    #define DPAD_RIGHT_PIN CONFIG_BLUCONTROL_DPAD_RIGHT_GPIO
#else
    #define DPAD_RIGHT_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_DPAD_DOWN_GPIO
    #define DPAD_DOWN_PIN CONFIG_BLUCONTROL_DPAD_DOWN_GPIO
#else
    #define DPAD_DOWN_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_DPAD_LEFT_GPIO
    #define DPAD_LEFT_PIN CONFIG_BLUCONTROL_DPAD_LEFT_GPIO
#else
    #define DPAD_LEFT_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_DPAD_UP_GPIO
    #define DPAD_UP_PIN CONFIG_BLUCONTROL_DPAD_UP_GPIO
#else
    #define DPAD_UP_PIN ""
#endif

// Triggers
#ifdef CONFIG_BLUCONTROL_BUTTON_L_GPIO
    #define TRIGGER_L_PIN CONFIG_BLUCONTROL_BUTTON_L_GPIO
#else
    #define TRIGGER_L_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_ZL_GPIO
    #define TRIGGER_ZL_PIN CONFIG_BLUCONTROL_BUTTON_ZL_GPIO
#else
    #define TRIGGER_ZL_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_R_GPIO
    #define TRIGGER_R_PIN CONFIG_BLUCONTROL_BUTTON_R_GPIO
#else
    #define TRIGGER_R_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_ZR_GPIO
    #define TRIGGER_ZR_PIN CONFIG_BLUCONTROL_BUTTON_ZR_GPIO
#else
    #define TRIGGER_ZR_PIN ""
#endif

// Special
#ifdef CONFIG_BLUCONTROL_BUTTON_START_GPIO
    #define BUTTON_START_PIN CONFIG_BLUCONTROL_BUTTON_START_GPIO
#else
    #define BUTTON_START_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_SELECT_GPIO
    #define BUTTON_SELECT_PIN CONFIG_BLUCONTROL_BUTTON_SELECT_GPIO
#else
    #define BUTTON_SELECT_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_HOME_GPIO
    #define BUTTON_HOME_PIN CONFIG_BLUCONTROL_BUTTON_HOME_GPIO
#else
    #define BUTTON_HOME_PIN ""
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_CAPTURE_GPIO
    #define BUTTON_CAPTURE_PIN CONFIG_BLUCONTROL_BUTTON_CAPTURE_GPIO
#else
    #define BUTTON_CAPTURE_PIN ""
#endif

// Button Sticks
#ifdef CONFIG_BLUCONTROL_BUTTON_STICK_L_GPIO
    #define BUTTON_STICK_L CONFIG_BLUCONTROL_BUTTON_STICK_L_GPIO
#else
    #define BUTTON_STICK_L ""
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_STICK_R_GPIO
    #define BUTTON_STICK_R CONFIG_BLUCONTROL_BUTTON_STICK_R_GPIO
#else
    #define BUTTON_STICK_R ""
#endif

// Misc
#ifdef CONFIG_BLUCONTROL_BUTTONS_PRESS_STATE_HIGH
    #define BUTTONS_PRESS_STATE 1
#else
    #define BUTTONS_PRESS_STATE 0
#endif

#define BLU_JOYSTICK_MAX_X      40
#define BLU_JOYSTICK_MAX_Y      40

#define BLU_ANALOG_JOYSTICK_MAX_X   1200
#define BLU_ANALOG_JOYSTICK_MAX_Y   1200

#define BLU_ANALOG_TRIGGER_MAX  2040

#define BLU_JOYSTICK_ABS_MAX    2040

typedef struct
{
    uint8_t value   : 1;
    int8_t *gpios;
    uint8_t gpio_length;
} blu_button_t;

typedef struct
{
    union
    {
        struct
        {
            // Buttons
            blu_button_t button_A;
            blu_button_t button_B;
            blu_button_t button_Y;
            blu_button_t button_X;
        };
        uint16_t buttons;
    };
    union
    {
        struct
        {
            // DPAD
            blu_button_t dpad_right;
            blu_button_t dpad_down;
            blu_button_t dpad_left;
            blu_button_t dpad_up;
        };
        uint16_t dpad;
    };
    union
    {
        struct
        {
            // Triggers
            blu_button_t trigger_l;
            blu_button_t trigger_zl;
            blu_button_t trigger_r;
            blu_button_t trigger_zr;
        };
        uint16_t triggers;
    };
    union
    {
        struct
        {
            // Special Functions
            blu_button_t special_start;
            blu_button_t special_select;
            blu_button_t special_home;
            blu_button_t special_capture;
        };
        uint16_t special_buttons;
    };
    union
    {
        struct
        {
            // Stick clicks
            blu_button_t button_stick_left;
            blu_button_t button_stick_right;
        };
        uint16_t button_sticks;
    };
} blu_buttons_t;

extern blu_buttons_t blu_buttons;

void blu_init_hardware(void);
void blu_refresh_buttons(void);

#endif