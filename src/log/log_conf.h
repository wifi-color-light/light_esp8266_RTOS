
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOG_CONF_H__
#define __LOG_CONF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Use mini log function */
#ifdef LOG_CONF_USE_MINI
#define LOG_USE_MINI LOG_CONF_USE_MINI
#else
#define LOG_USE_MINI 0
#endif

/* Prefix all logs with file name and line-of-code */
#ifdef LOG_CONF_WITH_LOC
#define LOG_WITH_LOC LOG_CONF_WITH_LOC
#else /* LOG_CONF_WITH_LOC */
#define LOG_WITH_LOC 1
#endif /* LOG_CONF_WITH_LOC */

/* Prefix all logs with Module name and logging level */
#ifdef LOG_CONF_WITH_MODULE_PREFIX
#define LOG_WITH_MODULE_PREFIX LOG_CONF_WITH_MODULE_PREFIX
#else /* LOG_CONF_WITH_MODULE_PREFIX */
#define LOG_WITH_MODULE_PREFIX 1
#endif /* LOG_CONF_WITH_MODULE_PREFIX */

/* Cooja annotations */
#ifdef LOG_CONF_WITH_ANNOTATE
#define LOG_WITH_ANNOTATE LOG_CONF_WITH_ANNOTATE
#else /* LOG_CONF_WITH_ANNOTATE */
#define LOG_WITH_ANNOTATE 0
#endif /* LOG_CONF_WITH_ANNOTATE */

/* Custom output function -- default is printf */
#ifdef LOG_CONF_OUTPUT
#define LOG_OUTPUT(...) LOG_CONF_OUTPUT(__VA_ARGS__)
#else /* LOG_CONF_OUTPUT */
#define LOG_OUTPUT(...) printf(__VA_ARGS__)
#endif /* LOG_CONF_OUTPUT */

 /*
  * Custom output function to prefix logs with level and module.
  *
  * This will only be called when LOG_CONF_WITH_MODULE_PREFIX is enabled and
  * all implementations should be based on LOG_OUTPUT.
  *
  * \param level     The log level
  * \param levelstr  The log level as string
  * \param module    The module string descriptor
  */
 #ifdef LOG_CONF_OUTPUT_PREFIX
 #define LOG_OUTPUT_PREFIX(level, levelstr, module) LOG_CONF_OUTPUT_PREFIX(level, levelstr, module)
 #else /* LOG_CONF_OUTPUT_PREFIX */
 #define LOG_OUTPUT_PREFIX(level, levelstr, module) LOG_OUTPUT("[%-4s: %-4s] ", levelstr, module)
 #endif /* LOG_CONF_OUTPUT_PREFIX */

 /******************************************************************************/
 /********************* A list of currently supported modules ******************/
 /******************************************************************************/
#ifndef LOG_CONF_LEVEL_MAIN
#define LOG_CONF_LEVEL_MAIN                        LOG_LEVEL_DBG
#endif /* LOG_CONF_LEVEL_MAIN */

#ifndef LOG_CONF_LEVEL_DB
#define LOG_CONF_LEVEL_DB                          LOG_LEVEL_WARN
#endif /* LOG_CONF_LEVEL_DB */

#ifndef LOG_CONF_LEVEL_HAL
#define LOG_CONF_LEVEL_HAL                         LOG_LEVEL_WARN
#endif /* LOG_CONF_LEVEL_DB */

#ifndef LOG_CONF_LEVEL_PROC
#define LOG_CONF_LEVEL_PROC                        LOG_LEVEL_WARN
#endif /* LOG_CONF_LEVEL_PROC */

/* Private macro -------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __LOG_CONF_H__ */

/************************ (C) COPYRIGHT BangMart *****END OF FILE****/

