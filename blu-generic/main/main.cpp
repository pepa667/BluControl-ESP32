#include "main.h"

#define LOG_TAG "BLU_GENERIC"

BleGamepad bleGamepad("BluControl Gamepad", "JPZV");
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

            if (btn_y_axis == 0 && btn_x_axis == 0)
            {
                dpad_axis = 0;
            }
            else
            {
                switch ((int)(atan2(btn_x_axis, btn_y_axis) * 180 / PI))
                {
                    default:
                    case 0:
                        dpad_axis = DPAD_UP;
                        break;
                    case 45:
                        dpad_axis = DPAD_UP_RIGHT;
                        break;
                    case 90:
                        dpad_axis = DPAD_RIGHT;
                        break;
                    case 135:
                        dpad_axis = DPAD_DOWN_RIGHT;
                        break;
                    case 180:
                        dpad_axis = DPAD_DOWN;
                        break;
                    case -135:
                        dpad_axis = DPAD_DOWN_LEFT;
                        break;
                    case -90:
                        dpad_axis = DPAD_LEFT;
                        break;
                    case -45:
                        dpad_axis = DPAD_UP_LEFT;
                        break;
                }
            }
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
                                    GET_JOYSTICK_Y_AXIS(n64_left_joystick_data.y_axis.value)); //Should add -1 too?

            need_report |= (old_l_thumb_x_value != n64_left_joystick_data.x_axis.value) || (old_l_thumb_y_value != n64_left_joystick_data.y_axis.value);
            old_l_thumb_x_value = n64_left_joystick_data.x_axis.value;
            old_l_thumb_y_value = n64_left_joystick_data.y_axis.value;
            #elif defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS)
            button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_LEFT);
            bleGamepad.setLeftThumb(GET_JOYSTICK_X_AXIS(button_stick_data->x_axis),
                                    GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis)); //Should add -1 too?
            
            need_report |= (old_l_thumb_x_value != button_stick_data->x_axis) || (old_l_thumb_y_value != button_stick_data->y_axis);
            old_l_thumb_x_value = button_stick_data->x_axis;
            old_l_thumb_y_value = button_stick_data->y_axis;
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
                                     GET_JOYSTICK_X_AXIS(n64_right_joystick_data.y_axis.value)); //Should add -1 too?

            need_report |= (old_r_thumb_x_value != n64_right_joystick_data.x_axis.value) || (old_r_thumb_y_value != n64_right_joystick_data.y_axis.value);
            old_r_thumb_x_value = n64_right_joystick_data.x_axis.value;
            old_r_thumb_y_value = n64_right_joystick_data.y_axis.value;
            #elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
            button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_RIGHT);
            bleGamepad.setRightThumb(GET_JOYSTICK_X_AXIS(button_stick_data->x_axis),
                                     GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis)); //Should add -1 too?

            need_report |= (old_r_thumb_x_value != button_stick_data->x_axis) || (old_r_thumb_y_value != button_stick_data->y_axis);
            old_r_thumb_x_value = button_stick_data->x_axis;
            old_r_thumb_y_value = button_stick_data->y_axis;
            #else
            bleGamepad.setRightThumb(GET_JOYSTICK_X_AXIS(0),
                                     GET_JOYSTICK_Y_AXIS(0));
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