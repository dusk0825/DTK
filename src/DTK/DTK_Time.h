
#ifndef __DTK_TIME_H__  
#define __DTK_TIME_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

typedef DTK_INT64 DTK_TIME_T; //(usec)

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_GetTimeTick()
*   @brief 获取从系统启动到现在的时间（毫秒）
*   @return 时间
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_GetTimeTick();

#endif // __DTK_TIME_H__ 