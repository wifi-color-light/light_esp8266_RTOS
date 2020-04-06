#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include "pwm.h"
#include "esp_timer.h"
#include "uart.h"
#include "log/log.h"
#include "button.h"
#include "wifi/AP_Service.h"
#include "wifi/wifiEvent.h"
#include "deviceService.h"
#define LOG_MODULE "MAIN"
#define LOG_LEVEL LOG_LEVEL_INFO
/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void display(void)
{
    static uint16 duty = 0;

    for (uint8 i = 0; i < 3; i++)
    {
        pwm_set_duty(duty,i);
    }
    
    duty += 100;
    duty %= 1023;
    pwm_start();
}
void task_blink(void* ignore)
{ 
    uint32 pwm_duty_init[3] = {0};
    uint32 io_info[3][3] = {{PERIPHS_IO_MUX_MTDI_U,FUNC_GPIO12,12},
                        {PERIPHS_IO_MUX_MTCK_U,FUNC_GPIO13,13},
                        {PERIPHS_IO_MUX_MTDO_U,FUNC_GPIO15,15}};

    gpio16_output_conf();

    pwm_init(1000,pwm_duty_init,3,io_info);

    //os_timer_t timer;

    //os_timer_disarm(&timer);
    //os_timer_setfn(&timer,display,NULL);
    //os_timer_arm(&timer,1000,true);

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U,FUNC_GPIO5);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 1);
    for (uint8 i = 0; i < 3; i++)
    {
        pwm_set_duty(1024,i);
    }
    pwm_start();
    while(true) {
        
    	gpio16_output_set(0);
       // GPIO_OUTPUT_SET(GPIO_ID_PIN(5),0);
        vTaskDelay(1000/portTICK_RATE_MS);
    	gpio16_output_set(1);
        //GPIO_OUTPUT_SET(GPIO_ID_PIN(5),1);
        vTaskDelay(1000/portTICK_RATE_MS);
        
    }
    vTaskDelete(NULL);
}
void buttonFunciton(BUTTON_EVENT *buttonEnvent)
{
    switch (*((uint8_t*)buttonEnvent))
    {
        case BUTTON_PRESS:
            LOG_INFO("长按按键\n");
            LOG_INFO("wifi账号密码配置模式\n");
            AP_Service("guihui","guihui00",8487);
            break;
        case BUTTON_CLICK:
            LOG_INFO("单击按键\n");
            break;
        case BUTTON_DOUBLE_CLICK:
            LOG_INFO("双击按键\n");
        default:
            break;
        
    }
}
/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    UART_SetBaudrate(UART0,115200);
    LOG_INFO("RTOS SDK version:%s\n",system_get_sdk_version());
    //buttonInit(buttonFunciton);
    wifi_set_event_handler_cb(wifi_event_handler);
    deviceServiceInit();
    xTaskCreate(&task_blink, "startup", 2048, NULL, 1, NULL);

}
