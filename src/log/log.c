/**
  ******************************************************************************
  * @file    log.c
  * @author  Xu
  * @version V1.0.0
  * @date    2019/03/09
  * @brief   code file for the logging system
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2019 BangMart</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int curr_log_level_main = LOG_LEVEL_INFO;          //LOG_CONF_LEVEL_MAIN;
int curr_log_level_db = LOG_LEVEL_INFO;            //LOG_CONF_LEVEL_DB;
int curr_log_level_hal = LOG_LEVEL_INFO;           //LOG_CONF_LEVEL_HAL;
int curr_log_level_proc = LOG_LEVEL_INFO;          //LOG_CONF_LEVEL_PROC;

struct log_module all_modules[] =
{
// mode             current log level                    max log level allow
  {"main",          &curr_log_level_main,                LOG_CONF_LEVEL_MAIN},
  {"db",            &curr_log_level_db,                  LOG_CONF_LEVEL_DB},
  {"hal",           &curr_log_level_hal,                 LOG_CONF_LEVEL_HAL},
  {"proc",          &curr_log_level_proc,                LOG_CONF_LEVEL_PROC},
  {NULL, NULL, 0},
};

/*---------------------------------------------------------------------------*/
void LogSetLevel(const char *module, int level)
{
  if(level >= LOG_LEVEL_NONE && level <= LOG_LEVEL_DBG)
  {
    int i = 0;
    int module_all = !strcmp("all", module);
    while(all_modules[i].name != NULL)
    {
      if(module_all || !strcmp(module, all_modules[i].name))
      {
        *all_modules[i].curr_log_level = MIN(level, all_modules[i].max_log_level);
      }
      i++;
    }
  }
}

/*---------------------------------------------------------------------------*/
int LogGetLevel(const char *module)
{
  int i = 0;
  if(module == NULL)
  {
    return -1;
  }
  while(all_modules[i].name != NULL)
  {
    if(!strcmp(module, all_modules[i].name))
    {
      return *all_modules[i].curr_log_level;
    }
    i++;
  }
  return -1;
}

/*---------------------------------------------------------------------------*/
const char *LogLevelToStr(int level)
{
  switch(level)
  {
    case LOG_LEVEL_NONE:
      return "None";
    case LOG_LEVEL_ERR:
      return "Errors";
    case LOG_LEVEL_WARN:
      return "Warnings";
    case LOG_LEVEL_INFO:
      return "Info";
    case LOG_LEVEL_DBG:
      return "Debug";
    default:
      return "N/A";
  }
}

/*---------------------------------------------------------------------------*/


