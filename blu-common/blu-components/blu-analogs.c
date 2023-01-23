#include "blu-analogs.h"

#define LOG_TAG "BLU_ANALOGS"

adc_cali_handle_t blu_adc1_cali_handle;
adc_oneshot_unit_handle_t blu_adc1_handle;

adc_cali_handle_t blu_adc2_cali_handle;
adc_oneshot_unit_handle_t blu_adc2_handle;

static bool analog_initialized = false;
void blu_analog_init(void)
{
    if (analog_initialized)
    {
        return;
    }

    ESP_LOGD(LOG_TAG, "Starting analogs.");
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = BLU_ADC_ATTENUATION,
        .bitwidth = BLU_ADC_BITWIDTH,
        .default_vref = CONFIG_BLUCONTROL_ANALOG_VREF
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &blu_adc1_cali_handle));

    cali_config.unit_id = ADC_UNIT_2;
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &blu_adc2_cali_handle));

    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &blu_adc1_handle));

    adc_config.unit_id = ADC_UNIT_2;
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &blu_adc2_handle));

    analog_initialized = true;
}

adc_unit_t unit_id = ADC_UNIT_1;
adc_channel_t channel = ADC_CHANNEL_0;
adc_channel_t blu_analog_get_channel(gpio_num_t gpio)
{
    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(gpio, &unit_id, &channel));
    return channel;
}
adc_cali_handle_t *blu_analog_get_unit_cali(gpio_num_t gpio)
{
    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(gpio, &unit_id, &channel));
    return unit_id == ADC_UNIT_1 ? &blu_adc1_cali_handle : &blu_adc2_cali_handle;
}
adc_oneshot_unit_handle_t *blu_analog_get_unit_oneshot(gpio_num_t gpio)
{
    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(gpio, &unit_id, &channel));
    return unit_id == ADC_UNIT_1 ? &blu_adc1_handle : &blu_adc2_handle;
}