
#ifndef __DTK_UTILS_H__  
#define __DTK_UTILS_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

#if defined(OS_WINDOWS)
#define DTK_Vsnprintf _vsnprintf
#define DTK_Snprintf _snprintf
#elif defined(OS_POSIX)
#define DTK_Vsnprintf vsnprintf
#define DTK_Snprintf snprintf
#else
#error OS Not Implement Yet.
#endif

#define SafeDelete(p)       do {if (p) {delete p;p = NULL;}} while (0);
#define SafeDeleteEx(p)     do {if (p) {delete[] p;p = NULL;}} while (0);

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_Sleep(DTK_INT32 millisecond)
*   @brief 睡眠，让出CPU，单位毫秒
*   @param [in] millisecond 毫秒
*   @return 无
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_Sleep(DTK_INT32 millisecond);

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_Sleep(DTK_INT32 millisecond)
*   @brief 睡眠，让出CPU，单位微秒
*   @param [in] millisecond 毫秒
*   @return 无
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_USleep(DTK_INT64 nUSec);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_Rand()
*   @brief 获取随机数,随机数种子由内部DTK_GetTimeTick()获取
*   @return 随机数
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_Rand();

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_ZeroMemory(void* pMem, int iLen)
*   @brief 内存清零
*   @param [in] pMem    内存指针
*   @param [in] iLen    内存长度
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_ZeroMemory(void* pMem, int iLen);

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebug(const char* format, ...)
*   @brief 输出调试信息
*   @param [in] format  输出格式
*   @return 无
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebug(const char* format, ...);

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebugString(const char* lpOutputString)
*   @brief 输出调试字符串
*   @param [in] lpOutputString  字符串
*   @return 无
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebugString(const char* lpOutputString);

#if defined (CXXCOMPILE)
#include <string>
/** @fn DTK_DECLARE_CXX std::string CALLBACK DTK_UTF82A(const char* cont)
*   @brief UTF-8转成多字节
*   @param [in] cont        utf-8字符数据
*   @return 多字节数据
*/
DTK_DECLARE_CXX std::string CALLBACK DTK_UTF82A(const char* cont);

/** @fn DTK_DECLARE_CXX std::string CALLBACK DTK_A2UTF8(const char* cont)
*   @brief 多字节转成UTF-8
*   @param [in] cont        utf-8字符数据
*   @return 多字节数据
*/
DTK_DECLARE_CXX std::string CALLBACK DTK_A2UTF8(const char* cont);
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetCPUNumber()
*   @brief 获取CPU数量
*   @return 返回系统CPU数量
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetCPUNumber();

//获取UUID
typedef unsigned char DTK_UUID[16];
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetUUID(DTK_UUID uuid);
DTK_DECLARE DTK_INT32 CALLBACK DTK_StringFromUUID(DTK_UUID uuid, char* uuidString, DTK_INT32 len);

#endif // __DTK_UTILS_H__ 