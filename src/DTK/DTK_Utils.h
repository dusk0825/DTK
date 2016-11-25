
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

#endif // __DTK_UTILS_H__ 