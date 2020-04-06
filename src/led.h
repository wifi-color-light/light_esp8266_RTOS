#ifndef __LED_H__
#define __LED_H__
#include "esp_common.h"
typedef enum
{
    LED_OFF = 2,
    LED_ON = 1,
}LED_SWITCH;
typedef enum
{
    MODE_READ = 0,
    MODE_WRITE = 1,
}LED_MODE;
void led_init();
void led_control(LED_SWITCH sw,LED_MODE mode, uint8_t bright);
#endif