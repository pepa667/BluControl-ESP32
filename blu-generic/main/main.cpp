#include "main.h"

#define LOG_TAG "BLU_GENERIC"

BleGamepad bleGamepad("bebopCore", "Pépa");
BleGamepadConfiguration bleGamepadConfig;

TaskHandle_t loopTaskHandle = NULL;

#if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
blu_analog_stick_data_t *analog_stick_data;
#endif
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
blu_btn_stick_data_t *button_stick_data;
#endif
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG) || defined(CONFIG_BLUCONTROL_LEFT_STICK_N64) || defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS)
int old_l_thumb_x_value;
int old_l_thumb_y_value;
#endif
#if defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_N64) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
int old_r_thumb_x_value;
int old_r_thumb_y_value;
#endif

#if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
int trigger_value;
#endif
#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG
int old_l_trigger_value;
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG
int old_r_trigger_value;
#endif

int btn_x_axis = 0;
int btn_y_axis = 0;
int8_t dpad_axis = 0;
bool need_report = false;
int8_t old_dpad_axis = 0;

#ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
static bool is_shift_pressed = false;
static gpio_num_t shift_gpio_num = GPIO_NUM_NC;
#endif


void app_loop(void *params)
{
    while (true)
    {
        if (bleGamepad.isConnected())
        {
            need_report = false;
            #ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
            if (blu_energy_is_clock_running())
            {
                blu_energy_stop_clock();
            }
            #endif
            
            btn_x_axis = btn_y_axis = 0;

            blu_refresh_buttons();

#ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
            {
                static bool prev_shift_pressed = false;
                is_shift_pressed = blu_get_button_state(shift_gpio_num);
                if (prev_shift_pressed != is_shift_pressed)
                {
                    need_report = true;
                    prev_shift_pressed = is_shift_pressed;
                }
            }

            if (!is_shift_pressed)
            {
                // Normal: A→A, B→B, L→L, R→R, START→START, SELECT→SELECT
                if (BUTTON_A_NUMBER > 0)  { bleGamepad.setState(BUTTON_A_NUMBER, blu_buttons.button_A.value);     need_report |= (blu_buttons.button_A.old_value != blu_buttons.button_A.value); }
                if (BUTTON_B_NUMBER > 0)  { bleGamepad.setState(BUTTON_B_NUMBER, blu_buttons.button_B.value);     need_report |= (blu_buttons.button_B.old_value != blu_buttons.button_B.value); }
                if (BUTTON_X_NUMBER > 0)  { bleGamepad.setState(BUTTON_X_NUMBER, 0); }
                if (BUTTON_Y_NUMBER > 0)  { bleGamepad.setState(BUTTON_Y_NUMBER, 0); }
                if (BUTTON_L_NUMBER > 0)  { bleGamepad.setState(BUTTON_L_NUMBER, blu_buttons.trigger_l.value);    need_report |= (blu_buttons.trigger_l.old_value != blu_buttons.trigger_l.value); }
                if (BUTTON_ZL_NUMBER > 0) { bleGamepad.setState(BUTTON_ZL_NUMBER, 0); }
                if (BUTTON_R_NUMBER > 0)  { bleGamepad.setState(BUTTON_R_NUMBER, blu_buttons.trigger_r.value);    need_report |= (blu_buttons.trigger_r.old_value != blu_buttons.trigger_r.value); }
                if (BUTTON_ZR_NUMBER > 0) { bleGamepad.setState(BUTTON_ZR_NUMBER, 0); }

                if (BUTTON_START_NUMBER > 0)       { bleGamepad.setState(BUTTON_START_NUMBER, blu_buttons.special_start.value);          need_report |= (blu_buttons.special_start.old_value != blu_buttons.special_start.value); }
                else if (BUTTON_START_NUMBER == 0) { bleGamepad.setStateSpecialButton(START_BUTTON, blu_buttons.special_start.value);    need_report |= (blu_buttons.special_start.old_value != blu_buttons.special_start.value); }
                if (BUTTON_HOME_NUMBER > 0)        { bleGamepad.setState(BUTTON_HOME_NUMBER, 0); }
                else if (BUTTON_HOME_NUMBER == 0)  { bleGamepad.setStateSpecialButton(HOME_BUTTON, 0); }

                if (BUTTON_SELECT_NUMBER > 0)       { bleGamepad.setState(BUTTON_SELECT_NUMBER, blu_buttons.special_select.value);       need_report |= (blu_buttons.special_select.old_value != blu_buttons.special_select.value); }
                else if (BUTTON_SELECT_NUMBER == 0) { bleGamepad.setStateSpecialButton(SELECT_BUTTON, blu_buttons.special_select.value); need_report |= (blu_buttons.special_select.old_value != blu_buttons.special_select.value); }
                if (BUTTON_CAPTURE_NUMBER > 0)      { bleGamepad.setState(BUTTON_CAPTURE_NUMBER, 0); }
            }
            else
            {
                // SHIFT: A→X, B→Y, L→ZL, R→ZR, START→HOME, SELECT→CAPTURE
                if (BUTTON_A_NUMBER > 0)  { bleGamepad.setState(BUTTON_A_NUMBER, 0); }
                if (BUTTON_B_NUMBER > 0)  { bleGamepad.setState(BUTTON_B_NUMBER, 0); }
                if (BUTTON_X_NUMBER > 0)  { bleGamepad.setState(BUTTON_X_NUMBER, blu_buttons.button_A.value);     need_report |= (blu_buttons.button_A.old_value != blu_buttons.button_A.value); }
                if (BUTTON_Y_NUMBER > 0)  { bleGamepad.setState(BUTTON_Y_NUMBER, blu_buttons.button_B.value);     need_report |= (blu_buttons.button_B.old_value != blu_buttons.button_B.value); }
                if (BUTTON_L_NUMBER > 0)  { bleGamepad.setState(BUTTON_L_NUMBER, 0); }
                if (BUTTON_ZL_NUMBER > 0) { bleGamepad.setState(BUTTON_ZL_NUMBER, blu_buttons.trigger_l.value);   need_report |= (blu_buttons.trigger_l.old_value != blu_buttons.trigger_l.value); }
                if (BUTTON_R_NUMBER > 0)  { bleGamepad.setState(BUTTON_R_NUMBER, 0); }
                if (BUTTON_ZR_NUMBER > 0) { bleGamepad.setState(BUTTON_ZR_NUMBER, blu_buttons.trigger_r.value);   need_report |= (blu_buttons.trigger_r.old_value != blu_buttons.trigger_r.value); }

                if (BUTTON_START_NUMBER > 0)       { bleGamepad.setState(BUTTON_START_NUMBER, 0); }
                else if (BUTTON_START_NUMBER == 0) { bleGamepad.setStateSpecialButton(START_BUTTON, 0); }
                if (BUTTON_HOME_NUMBER > 0)        { bleGamepad.setState(BUTTON_HOME_NUMBER, blu_buttons.special_start.value);           need_report |= (blu_buttons.special_start.old_value != blu_buttons.special_start.value); }
                else if (BUTTON_HOME_NUMBER == 0)  { bleGamepad.setStateSpecialButton(HOME_BUTTON, blu_buttons.special_start.value);     need_report |= (blu_buttons.special_start.old_value != blu_buttons.special_start.value); }

                if (BUTTON_SELECT_NUMBER > 0)       { bleGamepad.setState(BUTTON_SELECT_NUMBER, 0); }
                else if (BUTTON_SELECT_NUMBER == 0) { bleGamepad.setStateSpecialButton(SELECT_BUTTON, 0); }
                if (BUTTON_CAPTURE_NUMBER > 0)      { bleGamepad.setState(BUTTON_CAPTURE_NUMBER, blu_buttons.special_select.value);      need_report |= (blu_buttons.special_select.old_value != blu_buttons.special_select.value); }
            }
#else
            if (BUTTON_A_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_A_NUMBER, blu_buttons.button_A.value);
                need_report |= (blu_buttons.button_A.old_value != blu_buttons.button_A.value);
            }
            if (BUTTON_B_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_B_NUMBER, blu_buttons.button_B.value);
                need_report |= (blu_buttons.button_B.old_value != blu_buttons.button_B.value);
            }
            if (BUTTON_Y_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_Y_NUMBER, blu_buttons.button_Y.value);
                need_report |= (blu_buttons.button_Y.old_value != blu_buttons.button_Y.value);
            }
            if (BUTTON_X_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_X_NUMBER, blu_buttons.button_X.value);
                need_report |= (blu_buttons.button_X.old_value != blu_buttons.button_X.value);
            }

            if (BUTTON_L_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_L_NUMBER, blu_buttons.trigger_l.value);
                need_report |= (blu_buttons.trigger_l.old_value != blu_buttons.trigger_l.value);
            }
            if (BUTTON_ZL_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_ZL_NUMBER, blu_buttons.trigger_zl.value);
                need_report |= (blu_buttons.trigger_zl.old_value != blu_buttons.trigger_zl.value);
            }
            if (BUTTON_R_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_R_NUMBER, blu_buttons.trigger_r.value);
                need_report |= (blu_buttons.trigger_r.old_value != blu_buttons.trigger_r.value);
            }
            if (BUTTON_ZR_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_ZR_NUMBER, blu_buttons.trigger_zr.value);
                need_report |= (blu_buttons.trigger_zr.old_value != blu_buttons.trigger_zr.value);
            }

            if (BUTTON_START_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_START_NUMBER, blu_buttons.special_start.value);
                need_report |= (blu_buttons.special_start.old_value != blu_buttons.special_start.value);
            }
            else if (BUTTON_START_NUMBER == 0)
            {
                bleGamepad.setStateSpecialButton(START_BUTTON, blu_buttons.special_start.value);
                need_report |= (blu_buttons.special_start.old_value != blu_buttons.special_start.value);
            }

            if (BUTTON_SELECT_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_SELECT_NUMBER, blu_buttons.special_select.value);
                need_report |= (blu_buttons.special_select.old_value != blu_buttons.special_select.value);
            }
            else if (BUTTON_SELECT_NUMBER == 0)
            {
                bleGamepad.setStateSpecialButton(SELECT_BUTTON, blu_buttons.special_select.value);
                need_report |= (blu_buttons.special_select.old_value != blu_buttons.special_select.value);
            }

            if (BUTTON_HOME_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_HOME_NUMBER, blu_buttons.special_home.value);
                need_report |= (blu_buttons.special_home.old_value != blu_buttons.special_home.value);
            }
            else if (BUTTON_HOME_NUMBER == 0)
            {
                bleGamepad.setStateSpecialButton(HOME_BUTTON, blu_buttons.special_home.value);
                need_report |= (blu_buttons.special_home.old_value != blu_buttons.special_home.value);
            }

            if (BUTTON_CAPTURE_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_CAPTURE_NUMBER, blu_buttons.special_capture.value);
                need_report |= (blu_buttons.special_capture.old_value != blu_buttons.special_capture.value);
            }
#endif // CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES

            if (BUTTON_STICK_L_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_STICK_L_NUMBER, blu_buttons.button_stick_left.value);
                need_report |= (blu_buttons.button_stick_left.old_value != blu_buttons.button_stick_left.value);
            }
            if (BUTTON_STICK_R_NUMBER > 0)
            {
                bleGamepad.setState(BUTTON_STICK_R_NUMBER, blu_buttons.button_stick_right.value);
                need_report |= (blu_buttons.button_stick_right.old_value != blu_buttons.button_stick_right.value);
            }

            if (blu_buttons.dpad_up.value)
                btn_y_axis++;
            if (blu_buttons.dpad_down.value)
                btn_y_axis--;
            if (blu_buttons.dpad_left.value)
                btn_x_axis--;
            if (blu_buttons.dpad_right.value)
                btn_x_axis++;

#if defined(CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES) && defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS)
            if (is_shift_pressed)
            {
                button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_LEFT);
                if (button_stick_data->x_axis > 0)
                    btn_x_axis++;
                else if (button_stick_data->x_axis < 0)
                    btn_x_axis--;
                if (button_stick_data->y_axis > 0)
                    btn_y_axis++;
                else if (button_stick_data->y_axis < 0)
                    btn_y_axis--;
            }
#endif

            if (btn_x_axis == 0 && btn_y_axis == 0)
                dpad_axis = 0;
            else if (btn_x_axis == 0 && btn_y_axis > 0)
                dpad_axis = DPAD_UP;
            else if (btn_x_axis > 0 && btn_y_axis > 0)
                dpad_axis = DPAD_UP_RIGHT;
            else if (btn_x_axis > 0 && btn_y_axis == 0)
                dpad_axis = DPAD_RIGHT;
            else if (btn_x_axis > 0 && btn_y_axis < 0)
                dpad_axis = DPAD_DOWN_RIGHT;
            else if (btn_x_axis == 0 && btn_y_axis < 0)
                dpad_axis = DPAD_DOWN;
            else if (btn_x_axis < 0 && btn_y_axis < 0)
                dpad_axis = DPAD_DOWN_LEFT;
            else if (btn_x_axis < 0 && btn_y_axis == 0)
                dpad_axis = DPAD_LEFT;
            else
                dpad_axis = DPAD_UP_LEFT;
            bleGamepad.setHat1(dpad_axis);
            need_report |= (old_dpad_axis != dpad_axis);
            old_dpad_axis = dpad_axis;

            // Joystick
            #if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG)
            analog_stick_data = blu_analog_stick_get_data(BLU_ANALOG_PAD_LEFT);
            bleGamepad.setLeftThumb(GET_ANALOG_JOYSTICK_X_AXIS(analog_stick_data->x_axis),
                                    GET_ANALOG_JOYSTICK_Y_AXIS(analog_stick_data->y_axis * -1));
            
            need_report |= (old_l_thumb_x_value != analog_stick_data->x_axis) || (old_l_thumb_y_value != analog_stick_data->y_axis);
            old_l_thumb_x_value = analog_stick_data->x_axis;
            old_l_thumb_y_value = analog_stick_data->y_axis;
            #elif defined(CONFIG_BLUCONTROL_LEFT_STICK_N64)
            bleGamepad.setLeftThumb(GET_JOYSTICK_X_AXIS(n64_left_joystick_data.x_axis.value),
                                    GET_JOYSTICK_Y_AXIS(n64_left_joystick_data.y_axis.value * -1));

            need_report |= (old_l_thumb_x_value != n64_left_joystick_data.x_axis.value) || (old_l_thumb_y_value != n64_left_joystick_data.y_axis.value);
            old_l_thumb_x_value = n64_left_joystick_data.x_axis.value;
            old_l_thumb_y_value = n64_left_joystick_data.y_axis.value;
            #elif defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS)
            button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_LEFT);
            #ifdef CONFIG_BLUCONTROL_BUTTONS_MINIMAL_YES
            if (!is_shift_pressed)
            {
                bleGamepad.setLeftThumb(GET_JOYSTICK_X_AXIS(button_stick_data->x_axis),
                                        GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis * -1));
                need_report |= (old_l_thumb_x_value != button_stick_data->x_axis) || (old_l_thumb_y_value != button_stick_data->y_axis);
                old_l_thumb_x_value = button_stick_data->x_axis;
                old_l_thumb_y_value = button_stick_data->y_axis;
            }
            else
            {
                // SHIFT: left stick buttons → DPAD (already remapped above)
                bleGamepad.setLeftThumb(GET_JOYSTICK_X_AXIS(0), GET_JOYSTICK_Y_AXIS(0));
                need_report |= (old_l_thumb_x_value != 0) || (old_l_thumb_y_value != 0);
                old_l_thumb_x_value = 0;
                old_l_thumb_y_value = 0;
            }
            #else
            bleGamepad.setLeftThumb(GET_JOYSTICK_X_AXIS(button_stick_data->x_axis),
                                    GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis * -1));
            need_report |= (old_l_thumb_x_value != button_stick_data->x_axis) || (old_l_thumb_y_value != button_stick_data->y_axis);
            old_l_thumb_x_value = button_stick_data->x_axis;
            old_l_thumb_y_value = button_stick_data->y_axis;
            #endif
            #else
            bleGamepad.setLeftThumb(GET_JOYSTICK_X_AXIS(0),
                                    GET_JOYSTICK_Y_AXIS(0));
            #endif

            #if defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
            analog_stick_data = blu_analog_stick_get_data(BLU_ANALOG_PAD_RIGHT);
            bleGamepad.setRightThumb(GET_ANALOG_JOYSTICK_X_AXIS(analog_stick_data->x_axis),
                                     GET_ANALOG_JOYSTICK_Y_AXIS(analog_stick_data->y_axis * -1));

            need_report |= (old_r_thumb_x_value != analog_stick_data->x_axis) || (old_r_thumb_y_value != analog_stick_data->y_axis);
            old_r_thumb_x_value = analog_stick_data->x_axis;
            old_r_thumb_y_value = analog_stick_data->y_axis;
            #elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_N64)
            bleGamepad.setRightThumb(GET_JOYSTICK_X_AXIS(n64_right_joystick_data.x_axis.value),
                                     GET_JOYSTICK_X_AXIS(n64_right_joystick_data.y_axis.value * -1));

            need_report |= (old_r_thumb_x_value != n64_right_joystick_data.x_axis.value) || (old_r_thumb_y_value != n64_right_joystick_data.y_axis.value);
            old_r_thumb_x_value = n64_right_joystick_data.x_axis.value;
            old_r_thumb_y_value = n64_right_joystick_data.y_axis.value;
            #elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
            button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_RIGHT);
            bleGamepad.setRightThumb(GET_JOYSTICK_X_AXIS(button_stick_data->x_axis),
                                     GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis * -1));

            need_report |= (old_r_thumb_x_value != button_stick_data->x_axis) || (old_r_thumb_y_value != button_stick_data->y_axis);
            old_r_thumb_x_value = button_stick_data->x_axis;
            old_r_thumb_y_value = button_stick_data->y_axis;
            #endif

            #ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG
            trigger_value = GET_ANALOG_TRIGGER_AXIS(blu_analog_trigger_get_value(BLU_ANALOG_TRIGGER_LEFT));
            need_report |= (old_l_trigger_value != trigger_value);
            old_l_trigger_value = trigger_value;

            bleGamepad.setLeftTrigger(trigger_value);
            if (LEFT_TRIGGER_BTN > 0)
            {
                bleGamepad.setStateSpecialButton(LEFT_TRIGGER_BTN, CHECK_IF_TRIGGER_IS_PRESSED(trigger_value, LEFT_TRIGGER_ACT));
            }
            #else
            bleGamepad.setLeftTrigger(GET_ANALOG_TRIGGER_AXIS(0));
            #endif

            #ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG
            trigger_value = GET_ANALOG_TRIGGER_AXIS(blu_analog_trigger_get_value(BLU_ANALOG_TRIGGER_RIGHT));
            bleGamepad.setRightTrigger(trigger_value);
            need_report |= (old_r_trigger_value != trigger_value);
            old_r_trigger_value = trigger_value;

            if (RIGHT_TRIGGER_BTN > 0)
            {
                bleGamepad.setStateSpecialButton(RIGHT_TRIGGER_BTN, CHECK_IF_TRIGGER_IS_PRESSED(trigger_value, RIGHT_TRIGGER_ACT));
            }
            #else
            bleGamepad.setRightTrigger(GET_ANALOG_TRIGGER_AXIS(0));
            #endif

            if (need_report)
            {
                bleGamepad.sendReport();
            }
        }
        else
        {
            #ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
            if (!blu_energy_is_clock_running())
            {
                blu_energy_start_clock();
            }
            #endif
        }
    }
}

void rumble_callback(RumbleData *rumble_data)
{
    #if LEFT_RUMBLE_GPIO >= 0
    gpio_set_level((gpio_num_t)LEFT_RUMBLE_GPIO, rumble_data->left_rumble);
    #endif
    #if RIGHT_RUMBLE_GPIO >= 0
    gpio_set_level((gpio_num_t)RIGHT_RUMBLE_GPIO, rumble_data->right_rumble);
    #endif
}

extern "C" void app_main(void)
{
    ESP_LOGD(LOG_TAG, "HEAP=%#010lx", esp_get_free_heap_size());

    blu_energy_init();
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

    bleGamepadConfig.setAutoReport(false);

    if (BUTTONS_LENGTH > 0)
    {
        bleGamepadConfig.setButtonCount(BUTTONS_LENGTH);
    }
    else
    {
        int allButtons[] =
        {
            BUTTON_A_NUMBER,
            BUTTON_B_NUMBER,
            BUTTON_Y_NUMBER,
            BUTTON_X_NUMBER,
            BUTTON_L_NUMBER,
            BUTTON_ZL_NUMBER,
            BUTTON_R_NUMBER,
            BUTTON_ZR_NUMBER,
            BUTTON_START_NUMBER,
            BUTTON_HOME_NUMBER,
            BUTTON_CAPTURE_NUMBER,
            BUTTON_STICK_L_NUMBER,
            BUTTON_STICK_R_NUMBER,
            LEFT_TRIGGER_BTN,
            RIGHT_TRIGGER_BTN
        };
        int maxNumber = 1;
        for (int i = 0; i < 13; i++)
        {
            if (allButtons[i] > maxNumber)
            {
                maxNumber = allButtons[i];
            }
        }
        bleGamepadConfig.setButtonCount(maxNumber);
    }

    ESP_LOGD(LOG_TAG, "Buttons length: %d", bleGamepadConfig.getButtonCount());

    bleGamepadConfig.setAxesMin(0x0000);
    bleGamepadConfig.setAxesMax(BLU_JOYSTICK_ABS_MAX * 2);
    bleGamepadConfig.setWhichSpecialButtons(BUTTON_START_NUMBER <= 0, BUTTON_CAPTURE_NUMBER <= 0, false, BUTTON_HOME_NUMBER <= 0, false, false, false, false);
    bleGamepadConfig.setWhichAxes(AXIS_HAS_LEFT_STICK, AXIS_HAS_LEFT_STICK, AXIS_HAS_RIGHT_STICK, AXIS_HAS_LEFT_TRIGGER, AXIS_HAS_RIGHT_TRIGGER, AXIS_HAS_RIGHT_STICK, false, false);
    
    #if LEFT_RUMBLE_GPIO >= 0 || RIGHT_RUMBLE_GPIO >= 0
    bleGamepadConfig.setHasRumble(true);
    bleGamepadConfig.setRumbleCallBack(rumble_callback);
    #else
    bleGamepadConfig.setHasRumble(false);
    #endif
    
    bleGamepad.begin(&bleGamepadConfig);

    xTaskCreatePinnedToCore(app_loop, "APP_LOOP", 4096, NULL, tskIDLE_PRIORITY, &loopTaskHandle, 1);
    configASSERT(loopTaskHandle);

    ESP_LOGI(LOG_TAG, "Started!");
}