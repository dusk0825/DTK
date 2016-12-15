
#ifndef __DTK_DTK_H__  
#define __DTK_DTK_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Init()
*   @brief 初始化DTK库
*   @return 0成功，-1失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Init();

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_Fini()
*   @brief 反初始化DTK库
*   @return 无
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_Fini();

#endif // __DTK_DTK_H__ 