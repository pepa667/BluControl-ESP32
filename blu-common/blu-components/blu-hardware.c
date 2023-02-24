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

#define LOG_TAG "BLU_HW"

blu_buttons_t blu_buttons = {};

char *buttons_gpio[buttons_length] = 
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
blu_button_t *buttons_ref[buttons_length] = 
{
    &(blu_buttons.button_A),
    &(blu_buttons.button_B),
    &(blu_buttons.button_Y),
    &(blu_buttons.button_X),
    &(blu_buttons.dpad_right),
    &(blu_buttons.dpad_down),
    &(blu_buttons.dpad_left),
    &(blu_buttons.dpad_up),
    &(blu_buttons.trigger_l),
    &(blu_buttons.trigger_zl),
    &(blu_buttons.trigger_r),
    &(blu_buttons.trigger_zr),
    &(blu_buttons.special_start),
    &(blu_buttons.special_select),
    &(blu_buttons.special_home),
    &(blu_buttons.special_capture),
    &(blu_buttons.button_stick_left),
    &(blu_buttons.button_stick_right),
};

void prepare_buttons_gpio();
void prepare_rumbles_gpio();

void blu_init_hardware(void)
{
    prepare_buttons_gpio();
    prepare_rumbles_gpio();

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

bool isButtonPressed = false;
int checkedGPIOs = 0;
void blu_refresh_buttons(void)
{
    for (int i = 0; i < buttons_length; i++)
    {
        isButtonPressed = true;
        checkedGPIOs = 0;
        if (buttons_ref[i]->gpio_length <= 0)
        {
            buttons_ref[i]->value = 0;
            continue;
        }

        for (int o = 0; o < buttons_ref[i]->gpio_length; o++)
        {
            if (buttons_ref[i]->gpios[o] < 0)
            {
                continue;
            }
            checkedGPIOs++;
            if (!blu_get_button_state(buttons_ref[i]->gpios[o]))
            {
                isButtonPressed = false;
                break;
            }
        }

        if (checkedGPIOs == 0)
        {
            buttons_ref[i]->value = 0;
            continue;    
        }

        buttons_ref[i]->value = isButtonPressed;
    }
}

uint8_t blu_get_button_state(gpio_num_t gpio_num)
{
    if (gpio_num >= 0 && gpio_get_level(gpio_num) == BUTTONS_PRESS_STATE)
    {
        return 1;
    }
    return 0;
}

void prepare_buttons_gpio()
{
    int gpioLength = 0;
    int strLength = 0;
    long tmpVal = 0;
    char *spaceStr;
    char *tmpStr;
    char *err;
    gpio_config_t io_conf = {};
    for (int i = 0; i < buttons_length; i++)
    {
        strLength = strlen(buttons_gpio[i]);
        if (strLength == 0)
        {
            buttons_ref[i]->gpio_length = 0;
            continue;
        }

        gpioLength = 1;
        for (int o = 0; o < strLength; o++)
        {
            if (buttons_gpio[i][o] == ' ')
            {
                gpioLength++;
            }
        }
        buttons_ref[i]->gpio_length = gpioLength;
        buttons_ref[i]->gpios = malloc(sizeof(uint8_t) * gpioLength);

        int o = 0;
        tmpStr = malloc(strlen(buttons_gpio[i]));
        strcpy(tmpStr, buttons_gpio[i]);
        spaceStr = strtok(tmpStr, " ");
        while (spaceStr != NULL)
        {
            tmpVal = strtol(spaceStr, &err, 10);
            if (*err)
            {
                ESP_LOGW(LOG_TAG, "%s has extra data (%s) which are not an integer. Ignoring that GPIO", spaceStr, err);
                buttons_ref[i]->gpios[o] = -1;
            }
            else if (tmpVal >= GPIO_NUM_MAX)
            {
                ESP_LOGW(LOG_TAG, "%ld is over the max GPIO count (%d). Ignoring that GPIO", tmpVal, GPIO_NUM_MAX - 1);
                buttons_ref[i]->gpios[o] = -1;
            }
            else if (tmpVal < 0)
            {
                ESP_LOGW(LOG_TAG, "%ld is a negative value. Ignoring that GPIO", tmpVal);
                buttons_ref[i]->gpios[o] = -1;
            }
            else
            {
                buttons_ref[i]->gpios[o] = tmpVal;

                io_conf.intr_type = GPIO_INTR_DISABLE;
                io_conf.pin_bit_mask = (1ULL<<buttons_ref[i]->gpios[o]);
                io_conf.mode = GPIO_MODE_INPUT;
                io_conf.pull_up_en = BUTTONS_PRESS_STATE == 0 ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
                io_conf.pull_down_en = BUTTONS_PRESS_STATE == 1 ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
                gpio_config(&io_conf);
            }

            spaceStr = strtok(NULL, " ");
            o++;
        }
    }
}

void prepare_rumbles_gpio()
{
    #if defined(CONFIG_BLUCONTROL_RUMBLE_LEFT_GPIO) && CONFIG_BLUCONTROL_RUMBLE_LEFT_GPIO >= 0
    gpio_reset_pin(CONFIG_BLUCONTROL_RUMBLE_LEFT_GPIO);
    gpio_set_direction(CONFIG_BLUCONTROL_RUMBLE_LEFT_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(CONFIG_BLUCONTROL_RUMBLE_LEFT_GPIO, 0);
    #endif
    #if defined(CONFIG_BLUCONTROL_RUMBLE_RIGHT_GPIO) && CONFIG_BLUCONTROL_RUMBLE_RIGHT_GPIO >= 0
    gpio_reset_pin(CONFIG_BLUCONTROL_RUMBLE_RIGHT_GPIO);
    gpio_set_direction(CONFIG_BLUCONTROL_RUMBLE_RIGHT_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(CONFIG_BLUCONTROL_RUMBLE_RIGHT_GPIO, 0);
    #endif
}