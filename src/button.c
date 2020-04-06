#include "esp_common.h"
#include "gpio.h"
#include "log\log.h"
#include "button.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#define LOG_MODULE "BUTTON"
#define LOG_LEVEL LOG_LEVEL_INFO

static BUTTON_EVENT buttonEvent;
static uint32_t lastTime = 0;
static uint32_t currenTime = 0;
static uint32_t pressTime = 0;
static uint32_t relessTime = 0;
static os_timer_t timer;
static bool timerActivity = false;
static xQueueHandle btEvenQue;
static buttonCallback buttonCallbackFunction;

void buttonTask(void* arg);

static void os_timer_function(void*arg)
{
    portBASE_TYPE taskWoken;
    if (currenTime - lastTime < 85000/*850000*5.75*/)
    {
        LOG_INFO("双击按键\n");
        buttonEvent = BUTTON_DOUBLE_CLICK;
        
    }
    else
    {
        LOG_INFO("单击按键\n");
        buttonEvent = BUTTON_CLICK;
    }
    xQueueSendFromISR(btEvenQue,&buttonEvent,&taskWoken);
    timerActivity = false;
    os_timer_disarm(&timer);
}
void GPIO_INTER(void)
{
   portBASE_TYPE taskWoken;
    _xt_isr_mask(1<<ETS_GPIO_INUM); 
    uint32 gpio_status;
    gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    //clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
    
    if (gpio_status == GPIO_Pin_4)
    {
        if (!GPIO_INPUT_GET(GPIO_ID_PIN(4)))
        {
            pressTime = system_get_rtc_time();
        }
        else
        {
            relessTime = system_get_rtc_time();
            if ((relessTime - pressTime) > 550000/*55000*5.75*/)
            {
                buttonEvent = BUTTON_PRESS;
                xQueueSendFromISR(btEvenQue,&buttonEvent,&taskWoken);
                LOG_INFO("长按按键\n");
            }
            else
            {
                lastTime = currenTime;
                currenTime = system_get_rtc_time();
                if (timerActivity == false)
                {
                    timerActivity = true;
                    os_timer_disarm(&timer);
                    os_timer_arm(&timer,500,false);
                }
            }
            

        }
    }
    _xt_isr_unmask(1<<ETS_GPIO_INUM); 
}

void buttonInit(buttonCallback btCb)
{
    _xt_isr_mask(1<<ETS_GPIO_INUM);
    GPIO_ConfigTypeDef gpio_in_cfg;
    gpio_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_ANYEDGE;
    gpio_in_cfg.GPIO_Mode = GPIO_Mode_Input;
    gpio_in_cfg.GPIO_Pullup = GPIO_PullUp_EN;
    gpio_in_cfg.GPIO_Pin = 4;
    
    gpio_config(&gpio_in_cfg);
   
    GPIO_REG_WRITE(GPIO_STATUS_ADDRESS,TRUE);
    gpio_intr_handler_register(GPIO_INTER,NULL);
    gpio_pin_intr_state_set(GPIO_ID_PIN(4),GPIO_PIN_INTR_ANYEDGE);
    _xt_isr_unmask(1<<ETS_GPIO_INUM);
    
    os_timer_setfn(&timer,os_timer_function,NULL);
    buttonCallbackFunction = btCb;
    btEvenQue =  xQueueCreate(1,sizeof(BUTTON_EVENT));
    xTaskCreate(buttonTask,"buttonTask",2048,NULL,3,NULL);
}
void buttonTask(void* arg)
{
    BUTTON_EVENT btEvent;
    while (true)
    {
         if (xQueueReceive(btEvenQue,&btEvent,portMAX_DELAY))
        {
            btEvent = checkButtonEvent();
            buttonCallbackFunction(&btEvent);
            clearButtonEvent();
        }
    }
    
   
}
BUTTON_EVENT checkButtonEvent()
{
    return buttonEvent;
}

void clearButtonEvent()
{
    buttonEvent = BUTTON_CLEAR;
}

