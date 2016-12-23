
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

#if defined (CXXCOMPILE)
#include <string>
/** @fn DTK_DECLARE_CXX std::string CALLBACK DTK_UTF82A(const char* cont)
*   @brief UTF-8ת�ɶ��ֽ�
*   @param [in] cont        utf-8�ַ�����
*   @return ���ֽ�����
*/
DTK_DECLARE_CXX std::string CALLBACK DTK_UTF82A(const char* cont);

/** @fn DTK_DECLARE_CXX std::string CALLBACK DTK_A2UTF8(const char* cont)
*   @brief ���ֽ�ת��UTF-8
*   @param [in] cont        utf-8�ַ�����
*   @return ���ֽ�����
*/
DTK_DECLARE_CXX std::string CALLBACK DTK_A2UTF8(const char* cont);
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetCPUNumber()
*   @brief ��ȡCPU����
*   @return ����ϵͳCPU����
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetCPUNumber();

//��ȡUUID
typedef unsigned char DTK_UUID[16];
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetUUID(DTK_UUID uuid);
DTK_DECLARE DTK_INT32 CALLBACK DTK_StringFromUUID(DTK_UUID uuid, char* uuidString, DTK_INT32 len);

#endif // __DTK_UTILS_H__ 