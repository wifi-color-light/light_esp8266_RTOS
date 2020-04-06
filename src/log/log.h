
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "log_conf.h"
/* Exported types ------------------------------------------------------------*/
 /* Per-module log level */
 struct log_module {
   const char *name;
   int *curr_log_level;
   int max_log_level;
 };
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifndef MAX
#define MAX(n, m)   (((n) < (m)) ? (m) : (n))
#endif

#ifndef MIN
#define MIN(n, m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef ABS
#define ABS(n)      (((n) < 0) ? -(n) : (n))
#endif

/* The different log levels available */
#define LOG_LEVEL_NONE         0 /* No log */
#define LOG_LEVEL_ERR          1 /* Errors */
#define LOG_LEVEL_WARN         2 /* Warnings */
#define LOG_LEVEL_INFO         3 /* Basic info */
#define LOG_LEVEL_DBG          4 /* Detailled debug */

#define LOG_LEVEL_MAIN         MIN((LOG_CONF_LEVEL_MAIN), curr_log_level_main)
#define LOG_LEVEL_DB           MIN((LOG_CONF_LEVEL_DB), curr_log_level_db)
#define LOG_LEVEL_HAL          MIN((LOG_CONF_LEVEL_DB), curr_log_level_hal)
#define LOG_LEVEL_PROC         MIN((LOG_CONF_LEVEL_DB), curr_log_level_proc)

 /* Main log function */
#define LOG(newline, level, levelstr, ...) do {  \
                             if(level <= (LOG_LEVEL)) { \
                               if(newline) { \
                                 if(LOG_WITH_MODULE_PREFIX) { \
                                   LOG_OUTPUT_PREFIX(level, levelstr, LOG_MODULE); \
                                 } \
                                 if(LOG_WITH_LOC) { \
                                   LOG_OUTPUT("[%s: %d] ", __FILE__, __LINE__); \
                                 } \
                               } \
                               LOG_OUTPUT(__VA_ARGS__); \
                             } \
                           } while (0)


 #define LOG_HEX(level,STR,COUNT) do {  \
                 if(level <= (LOG_LEVEL)) { \
                     for(unsigned short log_hex_index = 0; log_hex_index < COUNT; log_hex_index++) \
					 { \
                    	 LOG_OUTPUT("%02X", STR[log_hex_index]); \
					 } \
                     LOG_OUTPUT("\r\n"); \
                 } \
             } while (0)

 /* More compact versions of LOG macros */
#if LOG_USE_MINI
// min log function
#define LOG_ERR(...)           do{LOG_OUTPUT("[ERR]: "); LOG_OUTPUT(__VA_ARGS__);}while(0)
#define LOG_WARN(...)          do{LOG_OUTPUT("[WARN]: "); LOG_OUTPUT(__VA_ARGS__);}while(0)
#define LOG_INFO(...)          do{LOG_OUTPUT("[INFO]: "); LOG_OUTPUT(__VA_ARGS__);}while(0)
#define LOG_DBG(...)           do{LOG_OUTPUT("[DBG]: "); LOG_OUTPUT(__VA_ARGS__);}while(0)

#define LOG_PRINT_(...)        LOG_OUTPUT(__VA_ARGS__)
#define LOG_ERR_(...)          LOG_OUTPUT(__VA_ARGS__)
#define LOG_WARN_(...)         LOG_OUTPUT(__VA_ARGS__)
#define LOG_INFO_(...)         LOG_OUTPUT(__VA_ARGS__)
#define LOG_DBG_(...)          LOG_OUTPUT(__VA_ARGS__)
#else
// full log function
#define LOG_PRINT(...)         LOG(1, 0, "PRI", __VA_ARGS__)
#define LOG_ERR(...)           LOG(1, LOG_LEVEL_ERR, "ERR", __VA_ARGS__)
#define LOG_WARN(...)          LOG(1, LOG_LEVEL_WARN, "WARN", __VA_ARGS__)
#define LOG_INFO(...)          LOG(1, LOG_LEVEL_INFO, "INFO", __VA_ARGS__)
#define LOG_DBG(...)           LOG(1, LOG_LEVEL_DBG, "DBG", __VA_ARGS__)

#define LOG_PRINT_(...)        LOG(0, 0, "PRI", __VA_ARGS__)
#define LOG_ERR_(...)          LOG(0, LOG_LEVEL_ERR, "ERR", __VA_ARGS__)
#define LOG_WARN_(...)         LOG(0, LOG_LEVEL_WARN, "WARN", __VA_ARGS__)
#define LOG_INFO_(...)         LOG(0, LOG_LEVEL_INFO, "INFO", __VA_ARGS__)
#define LOG_DBG_(...)          LOG(0, LOG_LEVEL_DBG, "DBG", __VA_ARGS__)
#endif

 /* For checking log level.
    As this builds on curr_log_level variables, this should not be used
    in pre-processor macros. Use in a C 'if' statement instead, e.g.:
    if(LOG_INFO_ENABLED) { ... }
    Note that most compilers will still be able to strip the code out
    for low enough log levels configurations. */
 #define LOG_ERR_ENABLED        ((LOG_LEVEL) >= LOG_LEVEL_ERR)
 #define LOG_WARN_ENABLED       ((LOG_LEVEL) >= LOG_LEVEL_WARN)
 #define LOG_INFO_ENABLED       ((LOG_LEVEL) >= LOG_LEVEL_INFO)
 #define LOG_DBG_ENABLED        ((LOG_LEVEL) >= LOG_LEVEL_DBG)
/* Private macro -------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern int curr_log_level_main;
extern int curr_log_level_db;
extern int curr_log_level_hal;
extern int curr_log_level_proc;

extern struct log_module all_modules[];

/* Exported functions --------------------------------------------------------*/
 /**
  * Sets a log level at run-time. Logs are included in the firmware via
  * the compile-time flags in log-conf.h, but this allows to force lower log
  * levels, system-wide.
  * \param module The target module string descriptor
  * \param level The log level
 */
 void LogSetLevel(const char *module, int level);

 /**
  * Returns the current log level.
  * \param module The target module string descriptor
  * \return The current log level
 */
 int LogGetLevel(const char *module);

 /**
  * Returns a textual description of a log level
  * \param level log level
  * \return The textual description
 */
 const char *LogLevelToStr(int level);

#ifdef __cplusplus
}
#endif

#endif /* __LOG_H */

/************************ (C) COPYRIGHT BangMart *****END OF FILE****/

