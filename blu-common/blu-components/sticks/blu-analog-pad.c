#include "blu-analog-pad.h"

adc_channel_t get_adc_channel_from_gpio(gpio_num_t gpio);
adc_cali_handle_t adc_cali_handle;
adc_oneshot_unit_handle_t adc1_handle;

#define ADC_ATTENUATION ADC_ATTEN_DB_11
#define ADC_BITWIDTH ADC_BITWIDTH_11

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
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTENUATION,
        .bitwidth = ADC_BITWIDTH,
        .default_vref = CONFIG_BLUCONTROL_ANALOG_VREF
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle));

    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &adc1_handle));

    adc_oneshot_chan_cfg_t adc_channel_config = {
        .bitwidth = ADC_BITWIDTH,
        .atten = ADC_ATTENUATION,
    };

    #ifdef CONFIG_BLUCONTROL_LEFT_STICK_ANALOG
    printf("BluControl: Starting left analog stick.\n");
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, get_adc_channel_from_gpio(LEFT_STICK_X), &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, get_adc_channel_from_gpio(LEFT_STICK_Y), &adc_channel_config));
    //Axis Calibration
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, get_adc_channel_from_gpio(LEFT_STICK_X), &tmp_read));
    adc_cali_raw_to_voltage(adc_cali_handle, tmp_read, &left_stick_center_data.x_axis);
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, get_adc_channel_from_gpio(LEFT_STICK_Y), &tmp_read));
    adc_cali_raw_to_voltage(adc_cali_handle, tmp_read, &left_stick_center_data.y_axis);
    #endif
    #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG
    printf("BluControl: Starting right analog stick.\n");
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, get_adc_channel_from_gpio(RIGHT_STICK_X), &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, get_adc_channel_from_gpio(RIGHT_STICK_Y), &adc_channel_config));
    //Axis Calibration
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, get_adc_channel_from_gpio(LEFT_STICK_X), &tmp_read));
    adc_cali_raw_to_voltage(adc_cali_handle, tmp_read, &right_stick_center_data.x_axis);
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, get_adc_channel_from_gpio(LEFT_STICK_Y), &tmp_read));
    adc_cali_raw_to_voltage(adc_cali_handle, tmp_read, &right_stick_center_data.y_axis);
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
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, get_adc_channel_from_gpio(LEFT_STICK_X), &tmp_read));
        adc_cali_raw_to_voltage(adc_cali_handle, tmp_read, &analog_tmp_data.x_axis);
        analog_tmp_data.x_axis = (analog_tmp_data.x_axis - left_stick_center_data.x_axis) * LEFT_STICK_X_MULTIPLIER;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, get_adc_channel_from_gpio(LEFT_STICK_Y), &tmp_read));
        adc_cali_raw_to_voltage(adc_cali_handle, tmp_read, &analog_tmp_data.y_axis);
        analog_tmp_data.y_axis = (analog_tmp_data.y_axis - left_stick_center_data.y_axis) * LEFT_STICK_Y_MULTIPLIER;
    }
    #endif

    #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_ANALOG
    if (stick == BLU_ANALOG_PAD_RIGHT)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, get_adc_channel_from_gpio(RIGHT_STICK_X), &tmp_read));
        adc_cali_raw_to_voltage(adc_cali_handle, tmp_read, &analog_tmp_data.x_axis);
        analog_tmp_data.x_axis = (analog_tmp_data.x_axis - right_stick_center_data.x_axis) * RIGHT_STICK_X_MULTIPLIER;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, get_adc_channel_from_gpio(RIGHT_STICK_Y), &tmp_read));
        adc_cali_raw_to_voltage(adc_cali_handle, tmp_read, &analog_tmp_data.y_axis);
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