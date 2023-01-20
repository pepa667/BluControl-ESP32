#include "blu-analog-pad.h"

#ifdef CONFIG_BLUCONTROL_LEFT_STICK_ANALOG
blu_analog_stick_data_t left_stick_center_data = {};
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG
blu_analog_stick_data_t right_stick_center_data = {};
#endif

int tmp_read = 0;
void blu_analog_stick_init(void)
{
    printf("BluControl: Starting analogs sticks.\n");
    #if defined(CONFIG_BLUCONTROL_LEFT_STICK_ANALOG) || defined(CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG)
    blu_analog_init();

    adc_oneshot_chan_cfg_t adc_channel_config = {
        .bitwidth = BLU_ADC_BITWIDTH,
        .atten = BLU_ADC_ATTENUATION,
    };

    #ifdef CONFIG_BLUCONTROL_LEFT_STICK_ANALOG
    printf("BluControl: Starting left analog stick.\n");
    ESP_ERROR_CHECK(adc_oneshot_config_channel(*blu_analog_get_unit_oneshot(LEFT_STICK_X), blu_analog_get_channel(LEFT_STICK_X), &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(*blu_analog_get_unit_oneshot(LEFT_STICK_Y), blu_analog_get_channel(LEFT_STICK_Y), &adc_channel_config));
    //Axis Calibration
    ESP_ERROR_CHECK(adc_oneshot_read(*blu_analog_get_unit_oneshot(LEFT_STICK_X), blu_analog_get_channel(LEFT_STICK_X), &tmp_read));
    adc_cali_raw_to_voltage(*blu_analog_get_unit_cali(LEFT_STICK_X), tmp_read, &left_stick_center_data.x_axis);
    ESP_ERROR_CHECK(adc_oneshot_read(*blu_analog_get_unit_oneshot(LEFT_STICK_Y), blu_analog_get_channel(LEFT_STICK_Y), &tmp_read));
    adc_cali_raw_to_voltage(*blu_analog_get_unit_cali(LEFT_STICK_Y), tmp_read, &left_stick_center_data.y_axis);
    #endif
    #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG
    printf("BluControl: Starting right analog stick.\n");
    ESP_ERROR_CHECK(adc_oneshot_config_channel(*blu_analog_get_unit_oneshot(RIGHT_STICK_X), blu_analog_get_channel(RIGHT_STICK_X), &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(*blu_analog_get_unit_oneshot(RIGHT_STICK_Y), blu_analog_get_channel(RIGHT_STICK_Y), &adc_channel_config));
    //Axis Calibration
    ESP_ERROR_CHECK(adc_oneshot_read(*blu_analog_get_unit_oneshot(RIGHT_STICK_X), blu_analog_get_channel(RIGHT_STICK_X), &tmp_read));
    adc_cali_raw_to_voltage(*blu_analog_get_unit_cali(RIGHT_STICK_X), tmp_read, &right_stick_center_data.x_axis);
    ESP_ERROR_CHECK(adc_oneshot_read(*blu_analog_get_unit_oneshot(RIGHT_STICK_Y), blu_analog_get_channel(RIGHT_STICK_Y), &tmp_read));
    adc_cali_raw_to_voltage(*blu_analog_get_unit_cali(RIGHT_STICK_Y), tmp_read, &right_stick_center_data.y_axis);
    #endif
    #endif
}

blu_analog_stick_data_t analog_tmp_data = {};
blu_analog_stick_data_t *blu_analog_stick_get_data(char stick)
{
    analog_tmp_data.x_axis = 0;
    analog_tmp_data.y_axis = 0;

    #ifdef CONFIG_BLUCONTROL_LEFT_STICK_ANALOG
    if (stick == BLU_ANALOG_PAD_LEFT)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(*blu_analog_get_unit_oneshot(LEFT_STICK_X), blu_analog_get_channel(LEFT_STICK_X), &tmp_read));
        adc_cali_raw_to_voltage(*blu_analog_get_unit_cali(LEFT_STICK_X), tmp_read, &analog_tmp_data.x_axis);
        analog_tmp_data.x_axis = (analog_tmp_data.x_axis - left_stick_center_data.x_axis) * LEFT_STICK_X_MULTIPLIER;
        ESP_ERROR_CHECK(adc_oneshot_read(*blu_analog_get_unit_oneshot(LEFT_STICK_Y), blu_analog_get_channel(LEFT_STICK_Y), &tmp_read));
        adc_cali_raw_to_voltage(*blu_analog_get_unit_cali(LEFT_STICK_Y), tmp_read, &analog_tmp_data.y_axis);
        analog_tmp_data.y_axis = (analog_tmp_data.y_axis - left_stick_center_data.y_axis) * LEFT_STICK_Y_MULTIPLIER;
    }
    #endif

    #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG
    if (stick == BLU_ANALOG_PAD_RIGHT)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(*blu_analog_get_unit_oneshot(RIGHT_STICK_X), blu_analog_get_channel(RIGHT_STICK_X), &tmp_read));
        adc_cali_raw_to_voltage(*blu_analog_get_unit_cali(RIGHT_STICK_X), tmp_read, &analog_tmp_data.x_axis);
        analog_tmp_data.x_axis = (analog_tmp_data.x_axis - right_stick_center_data.x_axis) * RIGHT_STICK_X_MULTIPLIER;
        ESP_ERROR_CHECK(adc_oneshot_read(*blu_analog_get_unit_oneshot(RIGHT_STICK_Y), blu_analog_get_channel(RIGHT_STICK_Y), &tmp_read));
        adc_cali_raw_to_voltage(*blu_analog_get_unit_cali(RIGHT_STICK_Y), tmp_read, &analog_tmp_data.y_axis);
        analog_tmp_data.y_axis = (analog_tmp_data.y_axis - right_stick_center_data.x_axis) * RIGHT_STICK_Y_MULTIPLIER;
    }
    #endif

    //Capping X to -1200 <= X <= 1200
    if (analog_tmp_data.x_axis < 0 && analog_tmp_data.x_axis < -BLU_ANALOG_JOYSTICK_MAX_X)
    {
        analog_tmp_data.x_axis = -BLU_ANALOG_JOYSTICK_MAX_X;
    }
    else if (analog_tmp_data.x_axis > 0 && analog_tmp_data.x_axis > BLU_ANALOG_JOYSTICK_MAX_X)
    {
        analog_tmp_data.x_axis = BLU_ANALOG_JOYSTICK_MAX_X;
    }
    //Capping Y to -1200 <= X <= 1200
    if (analog_tmp_data.y_axis < 0 && analog_tmp_data.y_axis < -BLU_ANALOG_JOYSTICK_MAX_Y)
    {
        analog_tmp_data.y_axis = -BLU_ANALOG_JOYSTICK_MAX_Y;
    }
    else if (analog_tmp_data.y_axis > 0 && analog_tmp_data.y_axis > BLU_ANALOG_JOYSTICK_MAX_Y)
    {
        analog_tmp_data.y_axis = BLU_ANALOG_JOYSTICK_MAX_Y;
    }

    return &analog_tmp_data;
}