#include "esp_common.h"
#include "esp_softap.h"
#include "esp_sta.h"
#include "log\log.h"
#include "espconn.h"
#include "esp_libc.h"
#include "AP_Service.h"
#include "cJSON.h"
#define LOG_MODULE "AP_Service"
#define LOG_LEVEL LOG_LEVEL_INFO


static void ICACHE_FLASH_ATTR connect_callback_function(void *arg);
static void ICACHE_FLASH_ATTR reconnect_callback_function (void *arg, sint8 err);
static void ICACHE_FLASH_ATTR recv_callback_function(void *arg, char *pdata, unsigned short len);

static struct  espconn conn;

int AP_Service(const char* ssid,const char* password,unsigned int port)
{
    
    struct softap_config config;
    
    if (ssid == NULL)
    {
        LOG_ERR("ssid 为空\n");
        return -1;
    }
    if (strlen(ssid) > sizeof(config.ssid))
    {
        LOG_ERR("ssid 超过最大字节数%s\n",sizeof(config.ssid));
        return -2;
    }
    if (strlen(password) > sizeof(config.password))
    {
        LOG_ERR("password 超过最大字节数\n",sizeof(config.password));
    }
    strcpy(config.ssid,ssid);
    config.ssid_len = strlen(ssid);
    strcpy(config.password,password);
    wifi_set_opmode_current(SOFTAP_MODE);
    
    config.authmode = AUTH_WPA2_PSK;
    config.beacon_interval = 100;
    config.channel = 1;
    config.max_connection = 4;
    config.ssid_hidden = false;


    wifi_softap_set_config_current(&config);
    
    conn.proto.tcp = (esp_tcp*)os_zalloc(sizeof(esp_tcp));
    conn.type = ESPCONN_TCP;
    conn.proto.tcp->local_port = port;

    espconn_init();
    espconn_accept(&conn);
    espconn_regist_connectcb(&conn,connect_callback_function);
    espconn_regist_reconcb(&conn,reconnect_callback_function);
    espconn_regist_time(&conn,180,0);
    
}


static void ICACHE_FLASH_ATTR connect_callback_function(void *arg)
{
    LOG_INFO("有一个客户端连接至本服务端\n");
    espconn_regist_recvcb((struct espconn*)arg,recv_callback_function);
    //espconn_send((struct espconn*)arg,"hello",strlen("hello"));
    
}

static void ICACHE_FLASH_ATTR recv_callback_function(void *arg, char *pdata, unsigned short len)
{
    static char lastData[4] = {0};
    static char lastDataLen = 0;
    char *rspStr;
    LOG_INFO("收到一条消息 消息长度:%d\n",len);
    LOG_INFO(pdata);
    if (len < 4)
    {
        memcpy(lastData,pdata,len);
        lastDataLen = len;
        return;
    }
    char *dataPack = (char*)os_zalloc(1024);
    if (lastDataLen < 4)
    {
        memcpy(dataPack,lastData,lastDataLen);
        memcpy(dataPack+lastDataLen,pdata,len);
        lastDataLen = len;
    }
    else
    {
        memcpy(dataPack,pdata,len);
    }
    

    LOG_INFO("JSON 数据解析\n");

    cJSON *root = cJSON_Parse(dataPack);
    os_free(dataPack);
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
        int status = 0;
        cJSON *ssid_JSON = cJSON_GetObjectItem(root,"ssid");
        
        cJSON *password_JSON = cJSON_GetObjectItem(root,"password");
        cJSON *status_JSON = cJSON_GetObjectItem(root,"status");
        if (status_JSON == NULL)
        {
            LOG_INFO("获取JSON中的数据失败\n");
            cJSON_AddNumberToObject(result,"status",3);
            cJSON_AddStringToObject(result,"msg","JSON error");
            goto end;
        }
        status = status_JSON->valueint;
        LOG_INFO("获取JSON中的数据完成\n");

        switch (status)
        {
            case 0:
                if (STATION_GOT_IP == wifi_station_get_connect_status())
                {
                    cJSON_AddNumberToObject(result,"status",0);
                    cJSON_AddStringToObject(result,"msg","onnect succeed!");
                }
                else
                {
                    cJSON_AddNumberToObject(result,"status",1);
                    cJSON_AddStringToObject(result,"msg","connect faill");
                }
                break;
            case 1:
                LOG_INFO("配置路由账号和密码\n");
                if (ssid_JSON && password_JSON)
                {
                    char *ssid = NULL,*password = NULL;
                    ssid = ssid_JSON->valuestring;
                    password =  password_JSON->valuestring;

                    wifi_set_opmode(STATION_MODE);

                    struct station_config config;
                    strcpy(config.ssid,ssid);
                    strcpy(config.password,password);
                    wifi_station_set_config(&config);
                    wifi_station_connect();
                    wifi_station_set_auto_connect(true);
                    cJSON_AddNumberToObject(result,"status",2);
                    cJSON_AddStringToObject(result,"msg","AP connectting!");
                }
                break;
            default:
                break;
        }
    }
    
end:
    rspStr = cJSON_Print(result);
    if (rspStr)
    {
        LOG_INFO("%s\n",rspStr);
        espconn_send((struct espconn*)arg,rspStr,strlen(rspStr));
        os_free(rspStr);
    }
    if (root)
        cJSON_Delete(root);
    
}

static void ICACHE_FLASH_ATTR reconnect_callback_function (void *arg, sint8 err)
{
    LOG_INFO("重新连接\n");
}

