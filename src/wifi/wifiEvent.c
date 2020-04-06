#include "wifiEvent.h"
#include "log/log.h"
#define LOG_MODULE "wifiEvent"
#define LOG_LEVEL LOG_LEVEL_INFO
void ICACHE_FLASH_ATTR wifi_event_handler(System_Event_t *event)
{
   switch (*(uint8_t*)(event))
    {
        case EVENT_SOFTAPMODE_STACONNECTED :
            LOG_INFO("a station connected to ESP8266 soft-AP\n");
            break;
        case EVENT_SOFTAPMODE_STADISCONNECTED:
            LOG_INFO("a station disconnected to ESP8266 soft-AP\n");
            break;
        case EVENT_STAMODE_SCAN_DONE:
            LOG_INFO("ESP8266 station finish scanning AP\n");
        case EVENT_STAMODE_CONNECTED:
            LOG_INFO("ESP8266 station connected to AP\n");
            break;
        case EVENT_STAMODE_DISCONNECTED:
            LOG_INFO("ESP8266 station disconnected to AP\n");
            break;
        default:
            break;
    }
}
