#ifndef BLU_ANALOG_H
#define BLU_ANALOG_H

#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

#define BLU_ADC_ATTENUATION ADC_ATTEN_DB_11
#define BLU_ADC_BITWIDTH ADC_BITWIDTH_11

void blu_analog_init(void);
adc_channel_t blu_analog_get_channel(gpio_num_t gpio);
adc_cali_handle_t *blu_analog_get_unit_cali(gpio_num_t gpio);
adc_oneshot_unit_handle_t *blu_analog_get_unit_oneshot(gpio_num_t gpio);

#endif