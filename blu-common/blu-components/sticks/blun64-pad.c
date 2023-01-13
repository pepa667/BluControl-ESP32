#include "blun64-pad.h"

int joystick_x_value = 0;
int joystick_y_value = 0;

typedef struct
{
    n64_joystick_data_t *joystick_data;
    char axis_flag;
} n64_joystick_package_t;

#ifdef CONFIG_BLUCONTROL_LEFT_STICK_N64
n64_joystick_data_t n64_left_joystick_data = {
    {
        0,
        LEFT_STICK_X_INT,
        LEFT_STICK_X_Q
    },
    {
        0,
        LEFT_STICK_Y_INT,
        LEFT_STICK_Y_Q
    }
};
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_STICK_N64
n64_joystick_data_t n64_right_joystick_data = {
    {
        0,
        RIGHT_STICK_X_INT,
        RIGHT_STICK_X_Q
    },
    {
        0,
        RIGHT_STICK_Y_INT,
        RIGHT_STICK_Y_Q
    }
};
#endif

void joystick_interrupt_handler(void *params)
{
    n64_joystick_package_t *pkg = (n64_joystick_package_t*)params;

    switch (pkg->axis_flag)
    {
        default:
        case N64_JOYSTICK_X_AXIS:
            if (gpio_get_level(pkg->joystick_data->x_axis.interrupt_pin) == gpio_get_level(pkg->joystick_data->x_axis.counter_pin))
            {
                pkg->joystick_data->x_axis.value--;
            }
            else
            {
                pkg->joystick_data->x_axis.value++;
            }

            //Capping X to -40 <= X <= 40
            if (pkg->joystick_data->x_axis.value < 0 && pkg->joystick_data->x_axis.value < -BLU_JOYSTICK_MAX_X)
            {
                pkg->joystick_data->x_axis.value = -BLU_JOYSTICK_MAX_X;
            }
            else if (pkg->joystick_data->x_axis.value > 0 && pkg->joystick_data->x_axis.value > BLU_JOYSTICK_MAX_X)
            {
                pkg->joystick_data->x_axis.value = BLU_JOYSTICK_MAX_X;
            }
            break;
        case N64_JOYSTICK_Y_AXIS:
            if (gpio_get_level(pkg->joystick_data->y_axis.interrupt_pin) == gpio_get_level(pkg->joystick_data->y_axis.counter_pin))
            {
                pkg->joystick_data->y_axis.value--;
            }
            else
            {
                pkg->joystick_data->y_axis.value++;
            }
            
            //Capping Y to -40 <= Y <= 40
            if (pkg->joystick_data->y_axis.value < 0 && pkg->joystick_data->y_axis.value < -BLU_JOYSTICK_MAX_Y)
            {
                pkg->joystick_data->y_axis.value = -BLU_JOYSTICK_MAX_Y;
            }
            else if (pkg->joystick_data->y_axis.value > 0 && pkg->joystick_data->y_axis.value > BLU_JOYSTICK_MAX_Y)
            {
                pkg->joystick_data->y_axis.value = BLU_JOYSTICK_MAX_Y;   
            }
            break;
    }
}

#ifdef CONFIG_BLUCONTROL_LEFT_STICK_N64
n64_joystick_package_t left_joystick_x;
n64_joystick_package_t left_joystick_x;
#endif
#ifdef CONFIG_BLUCONTROL_RIGHT_STICK_N64
n64_joystick_package_t right_joystick_x;
n64_joystick_package_t right_joystick_x;
#endif
void blun64_init(void)
{
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = BOTH_STICK_Q_BIT_MASK;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = BOTH_STICK_INT_BIT_MASK;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    #ifdef CONFIG_BLUCONTROL_LEFT_STICK_N64
    left_joystick_x = { &n64_left_joystick_data, N64_JOYSTICK_X_AXIS };
    ESP_ERROR_CHECK(gpio_isr_handler_add(JOYSTICK_X_INT_PIN, joystick_interrupt_handler, (void *)(&left_joystick_x)));
    left_joystick_x = { &n64_left_joystick_data, N64_JOYSTICK_Y_AXIS };
    ESP_ERROR_CHECK(gpio_isr_handler_add(JOYSTICK_Y_INT_PIN, joystick_interrupt_handler, (void *)(&left_joystick_y)));
    #endif
    #ifdef CONFIG_BLUCONTROL_RIGHT_STICK_N64
    right_joystick_x = { &n64_right_joystick_data, N64_JOYSTICK_X_AXIS };
    ESP_ERROR_CHECK(gpio_isr_handler_add(JOYSTICK_X_INT_PIN, joystick_interrupt_handler, (void *)(&right_joystick_x)));
    right_joystick_x = { &n64_right_joystick_data, N64_JOYSTICK_Y_AXIS };
    ESP_ERROR_CHECK(gpio_isr_handler_add(JOYSTICK_Y_INT_PIN, joystick_interrupt_handler, (void *)(&right_joystick_y)));
    #endif
}