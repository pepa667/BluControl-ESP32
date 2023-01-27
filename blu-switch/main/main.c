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

void button_task(hoja_button_data_s *button_data)
{
    blu_refresh_buttons();

    button_data->button_right = blu_buttons.button_A;
    button_data->button_down = blu_buttons.button_B;
    button_data->button_left = blu_buttons.button_Y;
    button_data->button_up = blu_buttons.button_X;

    button_data->dpad_right = blu_buttons.dpad_right;
    button_data->dpad_down = blu_buttons.dpad_down;
    button_data->dpad_left = blu_buttons.dpad_left;
    button_data->dpad_up = blu_buttons.dpad_up;

    #ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_BTN_L
    button_data->trigger_l = blu_buttons.trigger_l | is_left_trigger_pressed;
    #else
    button_data->trigger_l = blu_buttons.trigger_l;
    #endif
    #ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_BTN_ZL
    button_data->trigger_zl = blu_buttons.trigger_zl | is_left_trigger_pressed;
    #else
    button_data->trigger_zl = blu_buttons.trigger_zl;
    #endif
    #ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_BTN_R
    button_data->trigger_r = blu_buttons.trigger_r | is_right_trigger_pressed;
    #else
    button_data->trigger_r = blu_buttons.trigger_r;
    #endif
    #ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_BTN_ZR
    button_data->trigger_zr = blu_buttons.trigger_zr | is_right_trigger_pressed;
    #else
    button_data->trigger_zr = blu_buttons.trigger_zr;
    #endif

    button_data->button_start = blu_buttons.special_start;
    button_data->button_select = blu_buttons.special_select;
    button_data->button_home = blu_buttons.special_home;
    button_data->button_capture = blu_buttons.special_capture;

    button_data->button_stick_left = blu_buttons.button_stick_left;
    button_data->button_stick_right = blu_buttons.button_stick_right;
}

void event_task(hoja_event_type_t type, uint8_t evt, uint8_t param)
{
    ESP_LOGD(LOG_TAG, "Event: \n\ttype: %d\n\tevent: %d\n\tparam: %d", type, evt, param);
    if (type == HOJA_EVT_BT && evt == HEVT_BT_DISCONNECT)
    {
        //This shouldn't be needed, but HOJA glitches after disconnecting the Switch... so... ¯\_(ツ)_/¯
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
void stick_task(hoja_analog_data_s* analog_data)
{
    // Joystick
    #if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG)
    analog_stick_data = blu_analog_stick_get_data(BLU_ANALOG_PAD_LEFT);
    analog_data->ls_x = GET_ANALOG_JOYSTICK_X_AXIS(analog_stick_data->x_axis);
    analog_data->ls_y = GET_ANALOG_JOYSTICK_Y_AXIS(analog_stick_data->y_axis);
    #elif defined(CONFIG_BLUCONTROL_LEFT_STICK_N64)
    analog_data->ls_x = GET_JOYSTICK_X_AXIS(n64_left_joystick_data.x_axis.value);
    analog_data->ls_y = GET_JOYSTICK_Y_AXIS(n64_left_joystick_data.y_axis.value);
    #elif defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS)
    button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_LEFT);
    analog_data->ls_x = GET_JOYSTICK_X_AXIS(button_stick_data->x_axis);
    analog_data->ls_y = GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis);
    #else
    analog_data->ls_x = GET_JOYSTICK_X_AXIS(0);
    analog_data->ls_y = GET_JOYSTICK_Y_AXIS(0);
    #endif

    #if defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
    analog_stick_data = blu_analog_stick_get_data(BLU_ANALOG_PAD_RIGHT);
    analog_data->rs_x = GET_ANALOG_JOYSTICK_X_AXIS(analog_stick_data->x_axis);
    analog_data->rs_y = GET_ANALOG_JOYSTICK_Y_AXIS(analog_stick_data->y_axis);
    #elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_N64)
    analog_data->rs_x = GET_JOYSTICK_X_AXIS(n64_right_joystick_data.x_axis.value);
    analog_data->rs_y = GET_JOYSTICK_Y_AXIS(n64_right_joystick_data.y_axis.value);
    #elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
    button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_RIGHT);
    analog_data->rs_x = GET_JOYSTICK_X_AXIS(button_stick_data->x_axis);
    analog_data->rs_y = GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis);
    #else
    analog_data->rs_x = GET_JOYSTICK_X_AXIS(0);
    analog_data->rs_y = GET_JOYSTICK_Y_AXIS(0);
    #endif

    #if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG)
    is_left_trigger_pressed = CHECK_IF_TRIGGER_IS_PRESSED(GET_ANALOG_TRIGGER_AXIS(blu_analog_trigger_get_value(BLU_ANALOG_TRIGGER_LEFT)), LEFT_TRIGGER_ACT);
    #endif
    #if defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
    is_right_trigger_pressed = CHECK_IF_TRIGGER_IS_PRESSED(GET_ANALOG_TRIGGER_AXIS(blu_analog_trigger_get_value(BLU_ANALOG_TRIGGER_RIGHT)), RIGHT_TRIGGER_ACT);
    #endif
}

void app_main(void)
{
    ESP_LOGD(LOG_TAG, "HEAP=%#010lx", esp_get_free_heap_size());

    hoja_register_button_callback(button_task);
    hoja_register_analog_callback(stick_task);
    hoja_register_event_callback(event_task);

    blu_energy_init();
    blu_energy_start_clock();
    blu_init_hardware();
    blucontrol_mode_init(true);

    hoja_init();
    hoja_set_core(HOJA_CORE_NS);
    core_ns_set_subcore(HOJA_CONTROL_TYPE);

    while(hoja_start_core() != HOJA_OK)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        ESP_LOGW(LOG_TAG, "Retrying start...");
    }
    ESP_LOGI(LOG_TAG, "Started!");
}