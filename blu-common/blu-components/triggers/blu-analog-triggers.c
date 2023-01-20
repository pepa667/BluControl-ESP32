#include "blu-analog-triggers.h"

#ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG
int left_trigger_center = 0;
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG
int right_trigger_center = 0;
#endif

void blu_analog_triggers_init(void)
{
    printf("BluControl: Starting analogs triggers.\n");
    #if defined(CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG)
    blu_analog_init();

    adc_oneshot_chan_cfg_t adc_channel_config = {
        .bitwidth = BLU_ADC_BITWIDTH,
        .atten = BLU_ADC_ATTENUATION,
    };

    #ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG
    printf("BluControl: Starting left analog trigger.\n");
    ESP_ERROR_CHECK(adc_oneshot_config_channel(blu_adc1_handle, get_adc_channel_from_gpio(LEFT_TRIGGER_GPIO), &adc_channel_config));
    //Axis Calibration
    ESP_ERROR_CHECK(adc_oneshot_read(blu_adc1_handle, get_adc_channel_from_gpio(LEFT_TRIGGER_GPIO), &left_trigger_center));
    adc_cali_raw_to_voltage(blu_adc_cali_handle, left_trigger_center, &left_trigger_center);
    #endif
    #ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG
    printf("BluControl: Starting right analog stick.\n");
    ESP_ERROR_CHECK(adc_oneshot_config_channel(blu_adc1_handle, get_adc_channel_from_gpio(RIGHT_TRIGGER_GPIO), &adc_channel_config));
    //Axis Calibration
    ESP_ERROR_CHECK(adc_oneshot_read(blu_adc1_handle, get_adc_channel_from_gpio(RIGHT_TRIGGER_GPIO), &right_trigger_center));
    adc_cali_raw_to_voltage(blu_adc_cali_handle, right_trigger_center, &right_trigger_center);
    #endif
    #endif
}

int analog_tmp_value = 0;
int blu_analog_trigger_get_value(char trigger)
{
    analog_tmp_value = 0;

    #ifdef CONFIG_BLUCONTROL_LEFT_TRIGGER_ANALOG
    if (trigger == BLU_ANALOG_TRIGGER_LEFT)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(blu_adc1_handle, get_adc_channel_from_gpio(LEFT_TRIGGER_GPIO), &analog_tmp_value));
        adc_cali_raw_to_voltage(blu_adc_cali_handle, analog_tmp_value, &analog_tmp_value);
        analog_tmp_value = (analog_tmp_value - left_trigger_center) * LEFT_TRIGGER_MULTIPLIER;
    }
    #endif

    #ifdef CONFIG_BLUCONTROL_RIGHT_TRIGGER_ANALOG
    if (trigger == BLU_ANALOG_TRIGGER_RIGHT)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(blu_adc1_handle, get_adc_channel_from_gpio(RIGHT_TRIGGER_GPIO), &analog_tmp_value));
        adc_cali_raw_to_voltage(blu_adc_cali_handle, analog_tmp_value, &analog_tmp_value);
        analog_tmp_value = (analog_tmp_value - right_trigger_center) * RIGHT_TRIGGER_MULTIPLIER;
    }
    #endif

    //Capping X to 0 <= X <= 2400
    if (analog_tmp_value < 0)
    {
        analog_tmp_value = 0;
    }
    else if (analog_tmp_value > BLU_ANALOG_TRIGGER_MAX)
    {
        analog_tmp_value = BLU_ANALOG_TRIGGER_MAX;
    }

    return analog_tmp_value;
}