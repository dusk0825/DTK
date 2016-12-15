
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
*   @brief ˯�ߣ��ó�CPU����λ����
*   @param [in] millisecond ����
*   @return ��
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_Sleep(DTK_INT32 millisecond);

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_Sleep(DTK_INT32 millisecond)
*   @brief ˯�ߣ��ó�CPU����λ΢��
*   @param [in] millisecond ����
*   @return ��
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_USleep(DTK_INT64 nUSec);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_Rand()
*   @brief ��ȡ�����,������������ڲ�DTK_GetTimeTick()��ȡ
*   @return �����
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_Rand();

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_ZeroMemory(void* pMem, int iLen)
*   @brief �ڴ�����
*   @param [in] pMem    �ڴ�ָ��
*   @param [in] iLen    �ڴ泤��
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_ZeroMemory(void* pMem, int iLen);

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebug(const char* format, ...)
*   @brief ���������Ϣ
*   @param [in] format  �����ʽ
*   @return ��
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebug(const char* format, ...);

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebugString(const char* lpOutputString)
*   @brief ��������ַ���
*   @param [in] lpOutputString  �ַ���
*   @return ��
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebugString(const char* lpOutputString);


#endif // __DTK_UTILS_H__ 