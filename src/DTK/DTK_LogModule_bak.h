
#if 0

#ifndef __DTK_LOGMODULE_H__  
#define __DTK_LOGMODULE_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

//��־����
typedef enum  tagLogLevel
{
    TRACE_LEVEL         = 1,    //������Ϣ
    DEBUG_LEVEL         = 2,    //������Ϣ
    INFO_LEVEL          = 3,    //֪ͨ��Ϣ
    WARN_LEVEL          = 4,    //������Ϣ
    ERROR_LEVEL         = 5     //������Ϣ
}LOG_LEVEL_E;

//��ǰ���ṩ����־�����ʽ
typedef enum tagLogOutMode 
{
    LOCAL_CONSOLE       = 1,    //���ؿ���̨      
    LOCAL_DEBUG         = 2,    //���ص�����     
    LOCAL_FILE          = 3     //�����ļ�
}LOG_MODE_E;

/** @fn DTK_INT32 WriteLog(LOG_LEVEL_E level, char const *file, int line, char const *content, ...)
*   @brief д��־
*   @param [in] level       ��־����
*   @param [in] file        �ļ���
*   @param [in] line        �к�
*   @param [in] content     ��־����
*   @return �ɹ�����0��ʧ�ܷ���-1
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