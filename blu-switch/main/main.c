#include "main.h"

#define LOG_TAG "BLU_SWITCH"

#if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG
bool is_left_trigger_pressed = false;
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG
bool is_right_trigger_pressed = false;
#endif
#endif

#ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
static bool is_shift_pressed = false;
static gpio_num_t shift_gpio_num = GPIO_NUM_NC;
#endif

void button_task()
{
    blu_refresh_buttons();

#ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
    // Read the SHIFT modifier state
    is_shift_pressed = blu_get_button_state(shift_gpio_num);

    if (!is_shift_pressed)
    {
        // Normal: A, B, L, R, START, SELECT
        hoja_button_data.button_right |= blu_buttons.button_A.value;
        hoja_button_data.button_down |= blu_buttons.button_B.value;
        hoja_button_data.trigger_l |= blu_buttons.trigger_l.value;
        hoja_button_data.trigger_r |= blu_buttons.trigger_r.value;
        hoja_button_data.button_start |= blu_buttons.special_start.value;
        hoja_button_data.button_select |= blu_buttons.special_select.value;
    }
    else
    {
        // SHIFT: A→X, B→Y, L→ZL, R→ZR, START→HOME, SELECT→CAPTURE
        hoja_button_data.button_up |= blu_buttons.button_A.value;
        hoja_button_data.button_left |= blu_buttons.button_B.value;
        hoja_button_data.trigger_zl |= blu_buttons.trigger_l.value;
        hoja_button_data.trigger_zr |= blu_buttons.trigger_r.value;
        hoja_button_data.button_home |= blu_buttons.special_start.value;
        hoja_button_data.button_capture |= blu_buttons.special_select.value;
    }
#else
    hoja_button_data.button_right |= blu_buttons.button_A.value;
    hoja_button_data.button_down |= blu_buttons.button_B.value;
    hoja_button_data.button_left |= blu_buttons.button_Y.value;
    hoja_button_data.button_up |= blu_buttons.button_X.value;

    hoja_button_data.dpad_right |= blu_buttons.dpad_right.value;
    hoja_button_data.dpad_down |= blu_buttons.dpad_down.value;
    hoja_button_data.dpad_left |= blu_buttons.dpad_left.value;
    hoja_button_data.dpad_up |= blu_buttons.dpad_up.value;

#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_BTN_L
    hoja_button_data.trigger_l |= blu_buttons.trigger_l.value | is_left_trigger_pressed;
#else
    hoja_button_data.trigger_l |= blu_buttons.trigger_l.value;
#endif
#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_BTN_ZL
    hoja_button_data.trigger_zl |= blu_buttons.trigger_zl.value | is_left_trigger_pressed;
#else
    hoja_button_data.trigger_zl |= blu_buttons.trigger_zl.value;
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_BTN_R
    hoja_button_data.trigger_r |= blu_buttons.trigger_r.value | is_right_trigger_pressed;
#else
    hoja_button_data.trigger_r |= blu_buttons.trigger_r.value;
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_BTN_ZR
    hoja_button_data.trigger_zr |= blu_buttons.trigger_zr.value | is_right_trigger_pressed;
#else
    hoja_button_data.trigger_zr |= blu_buttons.trigger_zr.value;
#endif

    hoja_button_data.button_start |= blu_buttons.special_start.value;
    hoja_button_data.button_select |= blu_buttons.special_select.value;
    hoja_button_data.button_home |= blu_buttons.special_home.value;
    hoja_button_data.button_capture |= blu_buttons.special_capture.value;

    hoja_button_data.button_stick_left |= blu_buttons.button_stick_left.value;
    hoja_button_data.button_stick_right |= blu_buttons.button_stick_right.value;
#endif
}

void event_task(hoja_event_type_t type, uint8_t evt, uint8_t param)
{
    ESP_LOGD(LOG_TAG, "Event: \n\ttype: %d\n\tevent: %d\n\tparam: %d", type, evt, param);
    if (type == HOJA_EVT_BT && evt == HEVT_BT_DISCONNECTED)
    {
        // This shouldn't be needed, but HOJA glitches after disconnecting the Switch... so... ¯\_(ツ)_/¯
        esp_restart();
    }

    if (type == HOJA_EVT_BT)
    {
#ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
        if (evt != HEVT_BT_CONNECTED)
        {
            if (!blu_energy_is_clock_running())
            {
                blu_energy_start_clock();
            }
        }
        else
        {
            if (blu_energy_is_clock_running())
            {
                blu_energy_stop_clock();
            }
        }
#endif
    }
}

#if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
blu_analog_stick_data_t *analog_stick_data;
#endif
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
blu_btn_stick_data_t *button_stick_data;
#endif

// Separate task to read sticks.
// This is essential to have as a separate component as ADC scans typically take more time and this is only
// scanned once between each polling interval. This varies from core to core.
void stick_task()
{
// Joystick
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG)
    analog_stick_data = blu_analog_stick_get_data(BLU_ANALOG_PAD_LEFT);
    hoja_analog_data.ls_x = GET_ANALOG_JOYSTICK_X_AXIS(analog_stick_data->x_axis);
    hoja_analog_data.ls_y = GET_ANALOG_JOYSTICK_Y_AXIS(analog_stick_data->y_axis);
#elif defined(CONFIG_BLUCONTROL_LEFT_STICK_N64)
    hoja_analog_data.ls_x = GET_JOYSTICK_X_AXIS(n64_left_joystick_data.x_axis.value);
    hoja_analog_data.ls_y = GET_JOYSTICK_Y_AXIS(n64_left_joystick_data.y_axis.value);
#elif defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS)
    button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_LEFT);
#ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
    if (!is_shift_pressed)
    {
        hoja_analog_data.ls_x = GET_JOYSTICK_X_AXIS(button_stick_data->x_axis);
        hoja_analog_data.ls_y = GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis);
    }
    else
    {
        // SHIFT: left stick buttons → DPAD
        hoja_button_data.dpad_right |= (button_stick_data->x_axis > 0);
        hoja_button_data.dpad_left |= (button_stick_data->x_axis < 0);
        hoja_button_data.dpad_up |= (button_stick_data->y_axis > 0);
        hoja_button_data.dpad_down |= (button_stick_data->y_axis < 0);
        hoja_analog_data.ls_x = GET_JOYSTICK_X_AXIS(0);
        hoja_analog_data.ls_y = GET_JOYSTICK_Y_AXIS(0);
    }
#else
    hoja_analog_data.ls_x = GET_JOYSTICK_X_AXIS(button_stick_data->x_axis);
    hoja_analog_data.ls_y = GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis);
#endif
#else
    hoja_analog_data.ls_x = GET_JOYSTICK_X_AXIS(0);
    hoja_analog_data.ls_y = GET_JOYSTICK_Y_AXIS(0);
#endif

#if defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
    analog_stick_data = blu_analog_stick_get_data(BLU_ANALOG_PAD_RIGHT);
    hoja_analog_data.rs_x = GET_ANALOG_JOYSTICK_X_AXIS(analog_stick_data->x_axis);
    hoja_analog_data.rs_y = GET_ANALOG_JOYSTICK_Y_AXIS(analog_stick_data->y_axis);
#elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_N64)
    hoja_analog_data.rs_x = GET_JOYSTICK_X_AXIS(n64_right_joystick_data.x_axis.value);
    hoja_analog_data.rs_y = GET_JOYSTICK_Y_AXIS(n64_right_joystick_data.y_axis.value);
#elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
    button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_RIGHT);
    hoja_analog_data.rs_x = GET_JOYSTICK_X_AXIS(button_stick_data->x_axis);
    hoja_analog_data.rs_y = GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis);
#else
    hoja_analog_data.rs_x = GET_JOYSTICK_X_AXIS(0);
    hoja_analog_data.rs_y = GET_JOYSTICK_Y_AXIS(0);
#endif

#if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG)
    is_left_trigger_pressed = CHECK_IF_TRIGGER_IS_PRESSED(GET_ANALOG_TRIGGER_AXIS(blu_analog_trigger_get_value(BLU_ANALOG_TRIGGER_LEFT)), LEFT_TRIGGER_ACT);
#endif
#if defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
    is_right_trigger_pressed = CHECK_IF_TRIGGER_IS_PRESSED(GET_ANALOG_TRIGGER_AXIS(blu_analog_trigger_get_value(BLU_ANALOG_TRIGGER_RIGHT)), RIGHT_TRIGGER_ACT);
#endif
}

void rumble_task(hoja_rumble_data_s *rumble_data)
{
#if LEFT_RUMBLE_GPIO >= 0
    gpio_set_level(LEFT_RUMBLE_GPIO, rumble_data->left_rumble);
#endif
#if RIGHT_RUMBLE_GPIO >= 0
    gpio_set_level(RIGHT_RUMBLE_GPIO, rumble_data->right_rumble);
#endif
}

void app_main(void)
{
    ESP_LOGD(LOG_TAG, "HEAP=%#010lx", esp_get_free_heap_size());

    hoja_register_button_callback(button_task);
    hoja_register_analog_callback(stick_task);
    hoja_register_event_callback(event_task);
    hoja_register_rumble_callback(rumble_task);

    blu_energy_init();
    blu_energy_start_clock();
    blu_init_hardware();

#ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
    {
        const char *shift_pin_str = SHIFT_BUTTON_PIN;
        if (strlen(shift_pin_str) > 0)
        {
            shift_gpio_num = (gpio_num_t)atoi(shift_pin_str);
            gpio_config_t io_conf = {};
            io_conf.intr_type = GPIO_INTR_DISABLE;
            io_conf.mode = GPIO_MODE_INPUT;
            io_conf.pin_bit_mask = (1ULL << shift_gpio_num);
#ifdef CONFIG_BLUCONTROL_BUTTONS_PRESS_STATE_HIGH
            io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
            io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
#else
            io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
            io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
#endif
            gpio_config(&io_conf);
        }
    }
#endif

    blucontrol_mode_init(true);

    hoja_init();
    hoja_set_core(HOJA_CORE_NS);
    core_ns_set_subcore(HOJA_CONTROL_TYPE);

    while (hoja_start_core() != HOJA_OK)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        ESP_LOGW(LOG_TAG, "Retrying start...");
    }
    ESP_LOGI(LOG_TAG, "Started!");
}