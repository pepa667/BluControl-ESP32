#ifndef BLU_ANALOG_H
#define BLU_ANALOG_H

#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

#define BLU_ADC_ATTENUATION ADC_ATTEN_DB_11
#define BLU_ADC_BITWIDTH ADC_BITWIDTH_11

void blu_analog_init(void);
adc_channel_t get_adc_channel_from_gpio(gpio_num_t gpio);

extern adc_cali_handle_t blu_adc_cali_handle;
extern adc_oneshot_unit_handle_t blu_adc1_handle;

#endif