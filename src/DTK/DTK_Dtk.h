
#ifndef __DTK_DTK_H__  
#define __DTK_DTK_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Init()
*   @brief ��ʼ��DTK��
*   @return 0�ɹ���-1ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Init();

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_Fini()
*   @brief ����ʼ��DTK��
*   @return ��
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_Fini();

#endif // __DTK_DTK_H__ 