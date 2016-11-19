
#ifndef __DTK_STRING_H__  
#define __DTK_STRING_H__  

#include "DTK_Types.h"
#include "DTK_Config.h"

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Strcasecmp(const char* s1, const char* s2)
*   @brief ���Դ�Сд�Ƚ��ַ���
*   @param [in] s1 �Ƚ��ַ���1
*   @param [in] s2 �Ƚ��ַ���2
*   @return �ȽϽ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Strcasecmp(const char* s1, const char* s2);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Strncasecmp(const char* s1, const char* s2, int n)
*   @brief ���Դ�Сд�Ƚ�ǰn���ַ���
*   @param [in] s1 �Ƚ��ַ���1
*   @param [in] s2 �Ƚ��ַ���2
*   @param [in] n  �Ƚ϶��ٸ��ַ�
*   @return �ȽϽ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Strncasecmp(const char* s1, const char* s2, int n);

#endif // __DTK_STRING_H__ 