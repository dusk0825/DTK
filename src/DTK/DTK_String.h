
#ifndef __DTK_STRING_H__  
#define __DTK_STRING_H__  

#include "DTK_Types.h"
#include "DTK_Config.h"

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Strcasecmp(const char* s1, const char* s2)
*   @brief 忽略大小写比较字符串
*   @param [in] s1 比较字符串1
*   @param [in] s2 比较字符串2
*   @return 比较结果
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Strcasecmp(const char* s1, const char* s2);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Strncasecmp(const char* s1, const char* s2, int n)
*   @brief 忽略大小写比较前n个字符串
*   @param [in] s1 比较字符串1
*   @param [in] s2 比较字符串2
*   @param [in] n  比较多少个字符
*   @return 比较结果
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Strncasecmp(const char* s1, const char* s2, int n);

#endif // __DTK_STRING_H__ 