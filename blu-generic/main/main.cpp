#include "main.h"

BleGamepad bleGamepad("BluControl Gamepad", "JPZV");
BleGamepadConfiguration bleGamepadConfig;

TaskHandle_t loopTaskHandle = NULL;

#if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
blu_analog_stick_data_t *analog_stick_data;
#endif
#if defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
blu_btn_stick_data_t *button_stick_data;
#endif
#if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
int trigger_value;
#endif

int btn_x_axis = 0;
int btn_y_axis = 0;
int dpad_axis = 0;
void app_loop(void *params)
{
    while (true)
    {
        if (bleGamepad.isConnected())
        {
            btn_x_axis = btn_y_axis = 0;

            blu_refresh_buttons();

            if (BUTTON_A_NUMBER > 0)
            {
                if (blu_buttons.button_A)
                    bleGamepad.press(BUTTON_A_NUMBER);
                else
                    bleGamepad.release(BUTTON_A_NUMBER);
            }
            if (BUTTON_B_NUMBER > 0)
            {
                if (blu_buttons.button_B)
                    bleGamepad.press(BUTTON_B_NUMBER);
                else
                    bleGamepad.release(BUTTON_B_NUMBER);
            }
            if (BUTTON_Y_NUMBER > 0)
            {
                if (blu_buttons.button_Y)
                    bleGamepad.press(BUTTON_Y_NUMBER);
                else
                    bleGamepad.release(BUTTON_Y_NUMBER);
            }
            if (BUTTON_X_NUMBER > 0)
            {
                if (blu_buttons.button_X)
                    bleGamepad.press(BUTTON_X_NUMBER);
                else
                    bleGamepad.release(BUTTON_X_NUMBER);
            }

            if (BUTTON_L_NUMBER > 0)
            {
                if (blu_buttons.trigger_l)
                    bleGamepad.press(BUTTON_L_NUMBER);
                else
                    bleGamepad.release(BUTTON_L_NUMBER);
            }
            if (BUTTON_ZL_NUMBER > 0)
            {
                if (blu_buttons.trigger_zl)
                    bleGamepad.press(BUTTON_ZL_NUMBER);
                else
                    bleGamepad.release(BUTTON_ZL_NUMBER);
            }
            if (BUTTON_R_NUMBER > 0)
            {
                if (blu_buttons.trigger_r)
                    bleGamepad.press(BUTTON_R_NUMBER);
                else
                    bleGamepad.release(BUTTON_R_NUMBER);
            }
            if (BUTTON_ZR_NUMBER > 0)
            {
                if (blu_buttons.trigger_zr)
                    bleGamepad.press(BUTTON_ZR_NUMBER);
                else
                    bleGamepad.release(BUTTON_ZR_NUMBER);
            }

            if (blu_buttons.special_start)
            {
                if (BUTTON_START_NUMBER > 0)
                {
                    bleGamepad.press(BUTTON_START_NUMBER);
                }
                else
                {
                    bleGamepad.pressStart();
                }
            }
            else
            {
                if (BUTTON_START_NUMBER > 0)
                {
                    bleGamepad.release(BUTTON_START_NUMBER);
                }
                else
                {
                    bleGamepad.releaseStart();
                }
            }
            if (blu_buttons.special_home)
            {
                if (BUTTON_HOME_NUMBER > 0)
                {
                    bleGamepad.press(BUTTON_HOME_NUMBER);
                }
                else
                {
                    bleGamepad.pressHome();
                }
            }
            else
            {
                if (BUTTON_HOME_NUMBER > 0)
                {
                    bleGamepad.release(BUTTON_HOME_NUMBER);
                }
                else
                {
                    bleGamepad.releaseHome();
                }
            }
            if (blu_buttons.special_capture)
            {
                if (BUTTON_CAPTURE_NUMBER > 0)
                {
                    bleGamepad.press(BUTTON_CAPTURE_NUMBER);
                }
                else
                {
                    bleGamepad.pressSelect();
                }
            }
            else
            {
                if (BUTTON_CAPTURE_NUMBER > 0)
                {
                    bleGamepad.release(BUTTON_CAPTURE_NUMBER);
                }
                else
                {
                    bleGamepad.releaseSelect();
                }
            }

            if (BUTTON_STICK_L_NUMBER > 0)
            {
                if (blu_buttons.button_stick_left)
                    bleGamepad.press(BUTTON_STICK_L_NUMBER);
                else
                    bleGamepad.release(BUTTON_STICK_L_NUMBER);
            }
            if (BUTTON_STICK_R_NUMBER > 0)
            {
                if (blu_buttons.button_stick_right)
                    bleGamepad.press(BUTTON_STICK_R_NUMBER);
                else
                    bleGamepad.release(BUTTON_STICK_R_NUMBER);
            }

            if (blu_buttons.dpad_up)
                btn_y_axis++;
            if (blu_buttons.dpad_down)
                btn_y_axis--;
            if (blu_buttons.dpad_left)
                btn_x_axis--;
            if (blu_buttons.dpad_right)
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

            // Joystick
            #if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG)
            analog_stick_data = blu_analog_stick_get_data(BLU_ANALOG_PAD_LEFT);
            bleGamepad.setLeftThumb(GET_ANALOG_JOYSTICK_X_AXIS(analog_stick_data->x_axis),
                                    GET_ANALOG_JOYSTICK_Y_AXIS(analog_stick_data->y_axis));
            #elif defined(CONFIG_BLUCONTROL_LEFT_STICK_N64)
            bleGamepad.setLeftThumb(GET_JOYSTICK_X_AXIS(n64_left_joystick_data.x_axis.value),
                                    GET_JOYSTICK_Y_AXIS(n64_left_joystick_data.y_axis.value));
            #elif defined(CONFIG_BLUCONTROL_LEFT_STICK_BUTTONS)
            button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_LEFT);
            bleGamepad.setLeftThumb(GET_JOYSTICK_X_AXIS(button_stick_data->x_axis),
                                    GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis));
            #else
            bleGamepad.setLeftThumb(GET_JOYSTICK_X_AXIS(0),
                                    GET_JOYSTICK_Y_AXIS(0));
            #endif

            #if defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
            analog_stick_data = blu_analog_stick_get_data(BLU_ANALOG_PAD_RIGHT);
            bleGamepad.setRightThumb(GET_ANALOG_JOYSTICK_X_AXIS(analog_stick_data->x_axis),
                                     GET_ANALOG_JOYSTICK_Y_AXIS(analog_stick_data->y_axis));
            #elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_N64)
            bleGamepad.setRightThumb(GET_JOYSTICK_X_AXIS(n64_right_joystick_data.x_axis.value),
                                     GET_JOYSTICK_X_AXIS(n64_right_joystick_data.y_axis.value));
            #elif defined(CONFIG_BLUCONTROL_RIGHT_STICK_BUTTONS)
            button_stick_data = blu_buttons_stick_get_data(BLU_BUTTONS_PAD_RIGHT);
            bleGamepad.setRightThumb(GET_JOYSTICK_X_AXIS(button_stick_data->x_axis),
                                     GET_JOYSTICK_Y_AXIS(button_stick_data->y_axis));
            #else
            bleGamepad.setRightThumb(GET_JOYSTICK_X_AXIS(0),
                                     GET_JOYSTICK_Y_AXIS(0));
            #endif

            #if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG)
            trigger_value = GET_ANALOG_TRIGGER_AXIS(blu_analog_trigger_get_value(BLU_ANALOG_TRIGGER_LEFT));
            bleGamepad.setLeftTrigger(trigger_value);
            if (LEFT_TRIGGER_BTN > 0)
            {
                if (CHECK_IF_TRIGGER_IS_PRESSED(trigger_value, LEFT_TRIGGER_ACT))
                    bleGamepad.press(LEFT_TRIGGER_BTN);
                else
                    bleGamepad.release(LEFT_TRIGGER_BTN);
            }
            #else
            bleGamepad.setLeftTrigger(GET_ANALOG_TRIGGER_AXIS(0));
            #endif

            #if defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
            trigger_value = GET_ANALOG_TRIGGER_AXIS(blu_analog_trigger_get_value(BLU_ANALOG_TRIGGER_RIGHT));
            bleGamepad.setRightTrigger(trigger_value);
            if (RIGHT_TRIGGER_BTN > 0)
            {
                if (CHECK_IF_TRIGGER_IS_PRESSED(trigger_value, RIGHT_TRIGGER_ACT))
                    bleGamepad.press(RIGHT_TRIGGER_BTN);
                else
                    bleGamepad.release(RIGHT_TRIGGER_BTN);
            }
            #else
            bleGamepad.setRightTrigger(GET_ANALOG_TRIGGER_AXIS(0));
            #endif

            bleGamepad.sendReport();
        }
        blucontrol_handle_buttons();
    }
}

extern "C" void app_main(void)
{
    printf("BluControl Generic Mode. HEAP=%#010lx\n", esp_get_free_heap_size());

    blu_init_hardware();
    blucontrol_mode_init();

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

    printf("BluControl: Buttons length: %d\n", bleGamepadConfig.getButtonCount());

    bleGamepadConfig.setAxesMin(0x0000);
    bleGamepadConfig.setAxesMax(BLU_JOYSTICK_ABS_MAX * 2);
    bleGamepadConfig.setWhichSpecialButtons(BUTTON_START_NUMBER <= 0, BUTTON_CAPTURE_NUMBER <= 0, false, BUTTON_HOME_NUMBER <= 0, false, false, false, false);
    bleGamepadConfig.setWhichAxes(AXIS_HAS_LEFT_STICK, AXIS_HAS_LEFT_STICK, AXIS_HAS_RIGHT_STICK, AXIS_HAS_LEFT_TRIGGER, AXIS_HAS_RIGHT_TRIGGER, AXIS_HAS_RIGHT_STICK, false, false);
    bleGamepad.begin(&bleGamepadConfig);

    xTaskCreatePinnedToCore(app_loop, "APP_LOOP", 4096, NULL, tskIDLE_PRIORITY, &loopTaskHandle, 1);
    configASSERT(loopTaskHandle);

    printf("BluControl. Started!\n");
}