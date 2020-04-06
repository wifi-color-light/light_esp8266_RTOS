#include "esp_common.h"
#include "stdint.h"
#include "espconn.h"
#include "log/log.h"
#include "cJSON.h"
#include "led.h"
#define LOG_MODULE "deviceService"
#define LOG_LEVEL LOG_LEVEL_INFO
#define SERVICE_PORT 8228
typedef enum
{
    ACTION_CLOSE = 0,
    ACTION_OPEN = 1,
    ACTION_BRIGHT = 2,
    ACTION_SCAN = 3,
}DEVICE_ACTION;

static LED_MODE deviceMode;
static LED_SWITCH deviceStatus;
static uint8_t brightness = 10;
static char broadcastAddress[4] = {255, 255, 255, 255};
static os_timer_t timer;
static struct  espconn conn;
static void ICACHE_FLASH_ATTR recv_callback_function(void *arg, char *pdata, short unsigned  len);
static void ICACHE_FLASH_ATTR sent_callback_function(void *arg, char *pdata, short unsigned  len);
static void os_timer_function(void*arg)
{
    memcpy(conn.proto.udp->remote_ip,broadcastAddress,4);
    LOG_INFO("定时广播本设备IP地址\n");
    struct ip_info info;
    wifi_get_ip_info(0x00,&info);
    char IP[12] = {0};
    sprintf(IP,"%d.%d.%d.%d\n",(uint8_t)(info.ip.addr),(uint8_t)(info.ip.addr >> 8),(uint8_t)(info.ip.addr >> 16),(uint8_t)(info.ip.addr >> 24));
    espconn_send(&conn,IP,strlen(IP));
}
void deviceServiceInit(void)
{
    led_init();
    conn.proto.udp = (esp_udp*)os_zalloc(sizeof(esp_udp));
    conn.type = ESPCONN_UDP;
    conn.proto.udp->local_port = SERVICE_PORT;
    conn.proto.udp->remote_port = SERVICE_PORT;
    
    espconn_regist_recvcb(&conn,recv_callback_function);
    espconn_regist_sentcb(&conn,sent_callback_function);
    espconn_create(&conn);

    //os_timer_disarm(&timer);
    //os_timer_setfn(&timer,os_timer_function,NULL);
    //os_timer_arm(&timer,10000,TRUE);
   
}
static void ICACHE_FLASH_ATTR recv_callback_function(void *arg, char *pdata, unsigned short len)
{
    struct espconn* connPtr = (struct espconn*)arg;
    char *rspStr;
    LOG_INFO("收到来自：%d.%d.%d.%d的一条消息 消息长度:%d 消息内容:%s\n",connPtr->proto.udp->remote_ip[0],connPtr->proto.udp->remote_ip[1],connPtr->proto.udp->remote_ip[2],connPtr->proto.udp->remote_ip[3],len,pdata);
    LOG_INFO("JSON 数据解析\n");

    cJSON *root = cJSON_Parse(pdata);
    cJSON * result = cJSON_CreateObject();
    LOG_INFO("JSON 数据完成\n");
    if (!root) 
    {
        cJSON_AddNumberToObject(result,"status",3);
        cJSON_AddStringToObject(result,"msg","JSON error");
        
    }
    else
    {
        LOG_INFO("获取JSON中的数据\n");
        cJSON *action_JSON = cJSON_GetObjectItem(root,"action");
        if (action_JSON == NULL)
        {
            LOG_INFO("获取JSON中的数据失败\n");
            cJSON_AddNumberToObject(result,"status",3);
            cJSON_AddStringToObject(result,"msg","JSON error");
            goto end;
        }
        int action = action_JSON->valueint;
        LOG_INFO("获取JSON 数据完成\n");

        switch (action)
        {
            case ACTION_OPEN:
            {
                
                cJSON *mode_json = cJSON_GetObjectItem(root,"mode");
                
                uint8_t mode = mode_json->valueint;
                
                led_control(LED_ON,mode,brightness);
                deviceStatus = LED_ON;
                deviceMode = mode;
                cJSON_AddNumberToObject(result,"status",0);
                cJSON_AddNumberToObject(result,"action",ACTION_OPEN);
                
                /* code */
                break;
            }
            case ACTION_CLOSE:
                led_control(LED_OFF,deviceMode,brightness);
                deviceStatus = LED_OFF;
                cJSON_AddNumberToObject(result,"status",0);
                cJSON_AddNumberToObject(result,"action",ACTION_CLOSE);
                break;
            case ACTION_BRIGHT:
            {
                
                cJSON* bright_json = cJSON_GetObjectItem(root,"bright");
                brightness = (uint8_t)bright_json->valueint;
                led_control(LED_ON,deviceMode,brightness);
                deviceStatus = LED_ON;
                cJSON_AddNumberToObject(result,"status",0);
                cJSON_AddNumberToObject(result,"action",ACTION_BRIGHT);
                break;
            }
            case ACTION_SCAN:
            {
                cJSON *remot_ip_JSON = cJSON_GetObjectItem(root,"IP");
                int remot_ip = remot_ip_JSON->valueint;
                memcpy(connPtr->proto.udp->remote_ip,&remot_ip,4);
                cJSON_AddNumberToObject(result,"status",0);
                cJSON_AddNumberToObject(result,"action",ACTION_SCAN);
                break;
            }
            default:
                break;
        }
        uint32 local_ip;
        memcpy(&local_ip,connPtr->proto.udp->local_ip,4);
        uint32 chip_id = system_get_chip_id();
        cJSON_AddNumberToObject(result,"ID",chip_id);
        cJSON_AddNumberToObject(result,"IP",local_ip);
        
        cJSON_AddNumberToObject(result,"deviceStatus",deviceStatus);
        cJSON_AddNumberToObject(result,"mode",deviceMode);
        cJSON_AddNumberToObject(result,"bright",brightness);

        end:
        rspStr = cJSON_Print(result);
        if (rspStr)
        {
            LOG_INFO("%s\n",rspStr);
            espconn_send(connPtr,rspStr,strlen(rspStr));
            os_free(rspStr);
            
        }
        if (root)
            cJSON_Delete(root);
    }
    
    
}
static void ICACHE_FLASH_ATTR sent_callback_function(void *arg, char *pdata, unsigned short len)
{
    LOG_INFO("数据发送成功\n");
}
void deviceServiceTask(void *arg)
{

}