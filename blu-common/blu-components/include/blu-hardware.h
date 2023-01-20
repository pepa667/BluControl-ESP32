#ifndef BLU_HW_H
#define BLU_HW_H

#include "driver/gpio.h"

#define GET_JOYSTICK_X_AXIS(val) (val + BLU_JOYSTICK_MAX_X) * BLU_JOYSTICK_ABS_MAX / BLU_JOYSTICK_MAX_X
#define GET_JOYSTICK_Y_AXIS(val) (val + BLU_JOYSTICK_MAX_Y) * BLU_JOYSTICK_ABS_MAX / BLU_JOYSTICK_MAX_Y

#define GET_ANALOG_JOYSTICK_X_AXIS(val) (val + BLU_ANALOG_JOYSTICK_MAX_X) * BLU_JOYSTICK_ABS_MAX / BLU_ANALOG_JOYSTICK_MAX_X
#define GET_ANALOG_JOYSTICK_Y_AXIS(val) (val + BLU_ANALOG_JOYSTICK_MAX_Y) * BLU_JOYSTICK_ABS_MAX / BLU_ANALOG_JOYSTICK_MAX_Y

#define GET_ANALOG_TRIGGER_AXIS(val) (val + BLU_ANALOG_TRIGGER_MAX) * BLU_JOYSTICK_ABS_MAX / BLU_ANALOG_TRIGGER_MAX
#define CHECK_IF_TRIGGER_IS_PRESSED(val, perc) perc > 0 && (val - BLU_JOYSTICK_ABS_MAX) * 100.0 / BLU_JOYSTICK_ABS_MAX >= perc

//DON'T RE-DEFINE THE BUTTONS GPIO HERE!!!
//DO IT USING THE `idf.py menuconfig` COMMAND
//READ THE README.MD FOR MORE INFO

// Buttons
#ifdef CONFIG_BLUCONTROL_BUTTON_A_GPIO
    #define BUTTON_A_PIN CONFIG_BLUCONTROL_BUTTON_A_GPIO
#else
    #define BUTTON_A_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_B_GPIO
    #define BUTTON_B_PIN CONFIG_BLUCONTROL_BUTTON_B_GPIO
#else
    #define BUTTON_B_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_Y_GPIO
    #define BUTTON_Y_PIN CONFIG_BLUCONTROL_BUTTON_Y_GPIO
#else
    #define BUTTON_Y_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_X_GPIO
    #define BUTTON_X_PIN CONFIG_BLUCONTROL_BUTTON_X_GPIO
#else
    #define BUTTON_X_PIN -1
#endif

// DPAD
#ifdef CONFIG_BLUCONTROL_DPAD_RIGHT_GPIO
    #define DPAD_RIGHT_PIN CONFIG_BLUCONTROL_DPAD_RIGHT_GPIO
#else
    #define DPAD_RIGHT_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_DPAD_DOWN_GPIO
    #define DPAD_DOWN_PIN CONFIG_BLUCONTROL_DPAD_DOWN_GPIO
#else
    #define DPAD_DOWN_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_DPAD_LEFT_GPIO
    #define DPAD_LEFT_PIN CONFIG_BLUCONTROL_DPAD_LEFT_GPIO
#else
    #define DPAD_LEFT_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_DPAD_UP_GPIO
    #define DPAD_UP_PIN CONFIG_BLUCONTROL_DPAD_UP_GPIO
#else
    #define DPAD_UP_PIN -1
#endif

// Triggers
#ifdef CONFIG_BLUCONTROL_BUTTON_L_GPIO
    #define TRIGGER_L_PIN CONFIG_BLUCONTROL_BUTTON_L_GPIO
#else
    #define TRIGGER_L_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_ZL_GPIO
    #define TRIGGER_ZL_PIN CONFIG_BLUCONTROL_BUTTON_ZL_GPIO
#else
    #define TRIGGER_ZL_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_R_GPIO
    #define TRIGGER_R_PIN CONFIG_BLUCONTROL_BUTTON_R_GPIO
#else
    #define TRIGGER_R_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_ZR_GPIO
    #define TRIGGER_ZR_PIN CONFIG_BLUCONTROL_BUTTON_ZR_GPIO
#else
    #define TRIGGER_ZR_PIN -1
#endif

// Special
#ifdef CONFIG_BLUCONTROL_BUTTON_START_GPIO
    #define BUTTON_START_PIN CONFIG_BLUCONTROL_BUTTON_START_GPIO
#else
    #define BUTTON_START_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_HOME_GPIO
    #define BUTTON_HOME_PIN CONFIG_BLUCONTROL_BUTTON_HOME_GPIO
#else
    #define BUTTON_HOME_PIN -1
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_CAPTURE_GPIO
    #define BUTTON_CAPTURE_PIN CONFIG_BLUCONTROL_BUTTON_CAPTURE_GPIO
#else
    #define BUTTON_CAPTURE_PIN -1
#endif

// Button Sticks
#ifdef CONFIG_BLUCONTROL_BUTTON_STICK_L_GPIO
    #define BUTTON_STICK_L CONFIG_BLUCONTROL_BUTTON_STICK_L_GPIO
#else
    #define BUTTON_STICK_L -1
#endif
#ifdef CONFIG_BLUCONTROL_BUTTON_STICK_R_GPIO
    #define BUTTON_STICK_R CONFIG_BLUCONTROL_BUTTON_STICK_R_GPIO
#else
    #define BUTTON_STICK_R -1
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

#define BLU_ANALOG_TRIGGER_MAX  3200

#define BLU_JOYSTICK_ABS_MAX    2040

typedef struct
{
    union
    {
        struct
        {
            // Buttons
            uint8_t button_A    : 1;
            uint8_t button_B    : 1;
            uint8_t button_Y    : 1;
            uint8_t button_X    : 1;
        };
        uint16_t buttons;
    };
    union
    {
        struct
        {
            // DPAD
            uint8_t dpad_right  : 1;
            uint8_t dpad_down   : 1;
            uint8_t dpad_left   : 1;
            uint8_t dpad_up     : 1;
        };
        uint16_t dpad;
    };
    union
    {
        struct
        {
            // Triggers
            uint8_t trigger_l   : 1;
            uint8_t trigger_zl  : 1;
            uint8_t trigger_r   : 1;
            uint8_t trigger_zr  : 1;
        };
        uint16_t triggers;
    };
    union
    {
        struct
        {
            // Special Functions
            uint8_t special_start   : 1;
            uint8_t special_home    : 1;
            uint8_t special_capture : 1;
        };
        uint16_t special_buttons;
    };
    union
    {
        struct
        {
            // Stick clicks
            uint8_t button_stick_left   : 1;
            uint8_t button_stick_right  : 1;
        };
        uint16_t button_sticks;
    };
} blu_buttons_t;

extern blu_buttons_t blu_buttons;

void blu_init_hardware(void);
void blu_refresh_buttons(void);

#endif