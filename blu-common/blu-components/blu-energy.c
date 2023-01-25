#include "blu-energy.h"

#define LOG_TAG "BLU_ENERGY"

#ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
TaskHandle_t clockTaskHandle = NULL;
int64_t clock_start_time = 0;
bool is_power_led_on = false;

int power_buttons[POWER_BUTTONS_LENGTH] = 
{
    POWER_BUTTON_1,
#if POWER_BUTTONS_LENGTH > 1
    POWER_BUTTON_2,
#endif
#if POWER_BUTTONS_LENGTH > 2
    POWER_BUTTON_3,
#endif
};
#endif

void deep_sleep_clock(void *params)
{
    #ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
    while (clock_start_time > 0)
    {
        if ((esp_timer_get_time() - clock_start_time) / 1000000UL >= POWER_OFF_SECONDS)
        {
            ESP_LOGI(LOG_TAG, "Turning off");

            uint64_t wakeMask = 0;

            for (int i = 0; i < POWER_BUTTONS_LENGTH; i++)
            {
                if (power_buttons[i] < 0)
                {
                    ESP_LOGW(LOG_TAG, "Ignoring Button #%i because it has a negative value. You may need reset the controller with the RESET button or EN pin.", i + 1);
                    continue;
                }
                wakeMask += pow(2, power_buttons[i]);
                rtc_gpio_hold_en((gpio_num_t)power_buttons[i]);
            }

            if (wakeMask == 0)
            {
                ESP_LOGE(LOG_TAG, "There is no button for waking up the controller. You'll need to reset the controller with the RESET button or EN pin.");
            }

            esp_sleep_enable_ext1_wakeup(wakeMask, POWER_WAKE_MODE);

            esp_deep_sleep_start();
        }

        if (POWER_LED_GPIO >= 0)
        {
            gpio_set_level(POWER_LED_GPIO, is_power_led_on ? LED_POWER_ON : !LED_POWER_ON);
            is_power_led_on = !is_power_led_on;
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    #endif
}

void blu_energy_init()
{
    #ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
    if (POWER_LED_GPIO >= 0)
    {
        gpio_reset_pin(POWER_LED_GPIO);
        gpio_set_direction(POWER_LED_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(POWER_LED_GPIO, LED_POWER_ON);
    }
    for (int i = 0; i < POWER_BUTTONS_LENGTH; i++)
    {
        if (power_buttons[i] < 0)
        {
            ESP_LOGW(LOG_TAG, "Ignoring Button #%i because it has a negative value.", i + 1);
            continue;
        }
        gpio_set_direction(power_buttons[i], GPIO_MODE_INPUT);
        #ifdef CONFIG_BLUCONTROL_ENERGY_WAKING_MODE_LOW
            gpio_set_pull_mode(power_buttons[i], GPIO_PULLUP_ONLY);
        #else
            gpio_set_pull_mode(power_buttons[i], GPIO_PULLDOWN_ONLY);
        #endif
    }
    #endif
}

bool blu_energy_is_clock_running()
{
    #ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
    return clockTaskHandle != NULL;
    #else
    return false;
    #endif
}

void blu_energy_start_clock()
{
    #ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
    if (clockTaskHandle == NULL)
    {
        clock_start_time = esp_timer_get_time();
        xTaskCreatePinnedToCore(deep_sleep_clock, "DEEP_SLEEP_CLOCK", 2048, NULL, tskIDLE_PRIORITY, &clockTaskHandle, 1);
        configASSERT(clockTaskHandle);
    }
    #endif
}
void blu_energy_stop_clock()
{
    #ifdef CONFIG_BLUCONTROL_ENERGY_MODE_SOFTWARE
    if (clockTaskHandle != NULL)
    {
        vTaskDelete(clockTaskHandle);
        clockTaskHandle = NULL;
    }
    clock_start_time = 0;
    if (POWER_LED_GPIO >= 0)
    {
        gpio_set_level(POWER_LED_GPIO, LED_POWER_ON);
    }
    #endif
}