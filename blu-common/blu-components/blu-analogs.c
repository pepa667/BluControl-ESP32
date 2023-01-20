#include "blu-analogs.h"

adc_cali_handle_t blu_adc_cali_handle;
adc_oneshot_unit_handle_t blu_adc1_handle;

static bool analog_initialized = false;
void blu_analog_init(void)
{
    if (analog_initialized)
    {
        return;
    }

    printf("BluControl: Starting analogs.\n");
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = BLU_ADC_ATTENUATION,
        .bitwidth = BLU_ADC_BITWIDTH,
        .default_vref = CONFIG_BLUCONTROL_ANALOG_VREF
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &blu_adc_cali_handle));

    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &blu_adc1_handle));
    analog_initialized = true;
}

adc_channel_t get_adc_channel_from_gpio(gpio_num_t gpio)
{
    switch (gpio)
    {
        default:
        case GPIO_NUM_4:
        case GPIO_NUM_36:
            return ADC_CHANNEL_0;
        case GPIO_NUM_0:
        case GPIO_NUM_37:
            return ADC_CHANNEL_1;
        case GPIO_NUM_2:
        case GPIO_NUM_38:
            return ADC_CHANNEL_2;
        case GPIO_NUM_15:
        case GPIO_NUM_39:
            return ADC_CHANNEL_3;
        case GPIO_NUM_13:
        case GPIO_NUM_32:
            return ADC_CHANNEL_4;
        case GPIO_NUM_12:
        case GPIO_NUM_33:
            return ADC_CHANNEL_5;
        case GPIO_NUM_14:
        case GPIO_NUM_34:
            return ADC_CHANNEL_6;
        case GPIO_NUM_27:
        case GPIO_NUM_35:
            return ADC_CHANNEL_7;
        case GPIO_NUM_25:
            return ADC_CHANNEL_8;
        case GPIO_NUM_26:
            return ADC_CHANNEL_9;
    }
}