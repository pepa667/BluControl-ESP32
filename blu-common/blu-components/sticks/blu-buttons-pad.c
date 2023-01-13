#include "blu-buttons-pad.h"

void blu_buttons_stick_init(void)
{
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = BOTH_STICK_BTNS_MASK;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
}

blu_btn_stick_data_t btns_pad_tmp_data = {};
blu_btn_stick_data_t *blu_buttons_stick_get_data(char stick)
{
    btns_pad_tmp_data.x_axis = 0;
    btns_pad_tmp_data.y_axis = 0;

    #ifdef CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS
    if (stick == BLU_BUTTONS_PAD_LEFT)
    {
        if (!gpio_get_level(LEFT_STICK_X_MAX))
            btns_pad_tmp_data.x_axis += BLU_JOYSTICK_MAX_X;
        if (!gpio_get_level(LEFT_STICK_X_MIN))
            btns_pad_tmp_data.x_axis -= BLU_JOYSTICK_MAX_X;
        if (!gpio_get_level(LEFT_STICK_Y_MAX))
            btns_pad_tmp_data.y_axis += BLU_JOYSTICK_MAX_Y;
        if (!gpio_get_level(LEFT_STICK_Y_MIN))
            btns_pad_tmp_data.y_axis -= BLU_JOYSTICK_MAX_Y;
    }
    #endif

    #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS
    if (stick == BLU_BUTTONS_PAD_RIGHT)
    {
        if (!gpio_get_level(RIGHT_STICK_X_MAX))
            btns_pad_tmp_data.x_axis += BLU_JOYSTICK_MAX_X;
        if (!gpio_get_level(RIGHT_STICK_X_MIN))
            btns_pad_tmp_data.x_axis -= BLU_JOYSTICK_MAX_X;
        if (!gpio_get_level(RIGHT_STICK_Y_MAX))
            btns_pad_tmp_data.y_axis += BLU_JOYSTICK_MAX_Y;
        if (!gpio_get_level(RIGHT_STICK_Y_MIN))
            btns_pad_tmp_data.y_axis -= BLU_JOYSTICK_MAX_Y;
    }
    #endif

    return &btns_pad_tmp_data;
}