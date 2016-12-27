
#if 0

#ifndef __DTK_LOGMODULE_H__  
#define __DTK_LOGMODULE_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

//日志级别
typedef enum  tagLogLevel
{
    TRACE_LEVEL         = 1,    //跟踪信息
    DEBUG_LEVEL         = 2,    //调试信息
    INFO_LEVEL          = 3,    //通知信息
    WARN_LEVEL          = 4,    //警告信息
    ERROR_LEVEL         = 5     //错误信息
}LOG_LEVEL_E;

//当前可提供的日志输出方式
typedef enum tagLogOutMode 
{
    LOCAL_CONSOLE       = 1,    //本地控制台      
    LOCAL_DEBUG         = 2,    //本地调试器     
    LOCAL_FILE          = 3     //本地文件
}LOG_MODE_E;

/** @fn DTK_INT32 WriteLog(LOG_LEVEL_E level, char const *file, int line, char const *content, ...)
*   @brief 写日志
*   @param [in] level       日志级别
*   @param [in] file        文件名
*   @param [in] line        行号
*   @param [in] content     日志内容
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK WriteLog(LOG_LEVEL_E eLevel, const char* pModule, const char* pFilePath, const char* pFuncName, int iLine, const char *pContent, ...);

#ifdef OS_WINDOWS
#define LOG_ERROR(fmt, ...)         WriteLog(ERROR_LEVEL, "DTK", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)          WriteLog(WARN_LEVEL,  "DTK", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)    
#define LOG_INFO(fmt, ...)          WriteLog(INFO_LEVEL,  "DTK", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)    
#define LOG_DEBUG(fmt, ...)         WriteLog(DEBUG_LEVEL, "DTK", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)    
#define LOG_TRACE(fmt, ...)         WriteLog(TRACE_LEVEL, "DTK", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)    
#elif defined OS_POSIX
#define LOG_ERROR(fmt, ...)         WriteLog(ERROR_LEVEL, "DTK", __FILE__, __FUNCTION__, __LINE__, fmt, ##args)
#define LOG_WARN(fmt, ...)          WriteLog(WARN_LEVEL,  "DTK", __FILE__, __FUNCTION__, __LINE__, fmt, ##args)    
#define LOG_INFO(fmt, ...)          WriteLog(INFO_LEVEL,  "DTK", __FILE__, __FUNCTION__, __LINE__, fmt, ##args)    
#define LOG_DEBUG(fmt, ...)         WriteLog(DEBUG_LEVEL, "DTK", __FILE__, __FUNCTION__, __LINE__, fmt, ##args)    
#define LOG_TRACE(fmt, ...)         WriteLog(TRACE_LEVEL, "DTK", __FILE__, __FUNCTION__, __LINE__, fmt, ##args)   
#endif


#endif // __DTK_LOGMODULE_H__ 

#endif