#include "blu-hardware.h"

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

#define buttons_length 18

blu_buttons_t blu_buttons = {};

gpio_num_t buttons_gpio[buttons_length] = 
{
    BUTTON_A_PIN,
    BUTTON_B_PIN,
    BUTTON_Y_PIN,
    BUTTON_X_PIN,
    DPAD_RIGHT_PIN,
    DPAD_DOWN_PIN,
    DPAD_LEFT_PIN,
    DPAD_UP_PIN,
    TRIGGER_L_PIN,
    TRIGGER_ZL_PIN,
    TRIGGER_R_PIN,
    TRIGGER_ZR_PIN,
    BUTTON_START_PIN,
    BUTTON_SELECT_PIN,
    BUTTON_HOME_PIN,
    BUTTON_CAPTURE_PIN,
    BUTTON_STICK_L,
    BUTTON_STICK_R,
};

uint8_t get_button_state(gpio_num_t gpio_num);

void blu_init_hardware(void)
{
    gpio_config_t io_conf = {};
    for (int i = 0; i < buttons_length; i++)
    {
        if (buttons_gpio[i] >= 0)
        {
            io_conf.intr_type = GPIO_INTR_DISABLE;
            io_conf.pin_bit_mask = (1ULL<<buttons_gpio[i]);
            io_conf.mode = GPIO_MODE_INPUT;
            io_conf.pull_up_en = BUTTONS_PRESS_STATE == 0 ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
            io_conf.pull_down_en = BUTTONS_PRESS_STATE == 1 ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
            gpio_config(&io_conf);
        }
    }

    #if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
    blu_analog_stick_init();
    #endif
    #if defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
    blu_buttons_stick_init();
    #endif
    #if defined(CONFIG_BLUCONTROL_LEFT_STICK_N64) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_N64)
    blun64_init();
    #endif

    #if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
    blu_analog_triggers_init();
    #endif
}

void blu_refresh_buttons(void)
{
    blu_buttons.button_A            = get_button_state(BUTTON_A_PIN);
    blu_buttons.button_B            = get_button_state(BUTTON_B_PIN);
    blu_buttons.button_Y            = get_button_state(BUTTON_Y_PIN);
    blu_buttons.button_X            = get_button_state(BUTTON_X_PIN);
    blu_buttons.dpad_right          = get_button_state(DPAD_RIGHT_PIN);
    blu_buttons.dpad_down           = get_button_state(DPAD_DOWN_PIN);
    blu_buttons.dpad_left           = get_button_state(DPAD_LEFT_PIN);
    blu_buttons.dpad_up             = get_button_state(DPAD_UP_PIN);
    blu_buttons.trigger_l           = get_button_state(TRIGGER_L_PIN);
    blu_buttons.trigger_zl          = get_button_state(TRIGGER_ZL_PIN);
    blu_buttons.trigger_r           = get_button_state(TRIGGER_R_PIN);
    blu_buttons.trigger_zr          = get_button_state(TRIGGER_ZR_PIN);
    blu_buttons.special_start       = get_button_state(BUTTON_START_PIN);
    blu_buttons.special_select      = get_button_state(BUTTON_SELECT_PIN);
    blu_buttons.special_home        = get_button_state(BUTTON_HOME_PIN);
    blu_buttons.special_capture     = get_button_state(BUTTON_CAPTURE_PIN);
    blu_buttons.button_stick_left   = get_button_state(BUTTON_STICK_L);
    blu_buttons.button_stick_right  = get_button_state(BUTTON_STICK_R);
}

uint8_t get_button_state(gpio_num_t gpio_num)
{
    if (gpio_num >= 0 && gpio_get_level(gpio_num) == BUTTONS_PRESS_STATE)
    {
        return 1;
    }
    return 0;
}