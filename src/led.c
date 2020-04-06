#include "led.h"
#include "gpio.h"

void led_init()
{
    uint32 pwm_duty_init[3] = {0};
    uint32 io_info[3][3] = {{PERIPHS_IO_MUX_MTDI_U,FUNC_GPIO12,12},
                        {PERIPHS_IO_MUX_MTCK_U,FUNC_GPIO13,13},
                        {PERIPHS_IO_MUX_MTDO_U,FUNC_GPIO15,15}};

    gpio16_output_conf();

    pwm_init(1000,pwm_duty_init,3,io_info);
    for (uint8 i = 0; i < 3; i++)
    {
        pwm_set_duty(0,i);
    }
    pwm_start();
}
void led_control(LED_SWITCH sw,LED_MODE mode, uint8_t bright)
{
    if (sw == LED_ON)
    {
        if (mode == MODE_READ)
        {
            gpio16_output_set(1);
        }
        else
        {
            /* code */
        }
        for (uint8 i = 0; i < 3; i++)
        {
            pwm_set_duty(bright,i);
        }
    }
    else
    {
        gpio16_output_set(0);
        for (uint8 i = 0; i < 3; i++)
        {
            pwm_set_duty(0,i);
        }
    }
    
}
