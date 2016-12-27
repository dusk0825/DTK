
#ifndef __DTK_TIMER_H__  
#define __DTK_TIMER_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

/*
定时器方案：
定时器采用多媒体时钟+异步时钟的模式,多媒体时钟由于系统限制最多支持16个，
如果多媒体时钟分配失败，则分配异步时钟

时钟选择：DTK_SetTimer() bFlag=DTK_FALSE不使用多媒体时钟

定时器精度：多媒体时钟1ms,异步时钟1ms.

限制：定时器最多1000个
*/

typedef DTK_VOIDPTR (* DTK_CBF_Timer)(DTK_INT32 hEvent, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTimer(DTK_CBF_Timer pCbf, DTK_VOIDPTR pUsrData,DTK_UINT32 nTimeOut, DTK_INT32* hEvent, DTK_BOOL bFlag)
*   @brief 初始化定时器
*   @param [in] pCbf        用户回调函数指针
*   @param [in] pUsrData    用户自定义数据
*   @param [in] nTimeOut    定时器触发间隔(单位ms)
*   @param [in] hEvent      定时器索引
*   @param [in] bFlag       true，多媒体时钟；false，异步时钟
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTimer(DTK_CBF_Timer pCbf, DTK_VOIDPTR pUsrData,DTK_UINT32 nTimeOut, DTK_INT32* hEvent, DTK_BOOL bFlag = DTK_FALSE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_KillTimer(DTK_INT32 hEvent)
*   @brief 销毁定时器
*   @param [in] hEvent      定时器索引,DTK_SetTimer调用成功的返回值
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_KillTimer(DTK_INT32 hEvent); 


#endif // __DTK_TIMER_H__ 