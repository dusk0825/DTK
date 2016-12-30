
#ifndef __DTK_THREADPOOLFLEX_H__  
#define __DTK_THREADPOOLFLEX_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

/** @fn DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPoolFlex_Create(DTK_UINT32 nMaxThreadNum, DTK_UINT32 nIldeTime, DTK_VOIDPTR(CALLBACK *fWorkRoutine)(DTK_VOIDPTR), DTK_UINT32 nStackSize = 0)
*   @brief 创建线程池
*   @param [in] nMaxThreadNum   线程池最大线程数
*   @param [in] nIldeTime       线程空闲退出时间，以毫秒计，超过空闲时间没有任务线程自动退出
*   @param [in] fWorkRoutine    线程执行任务
*   @param [in] nStackSize      线程堆栈大小，以字节计，0为默认大小
*   @return 成功返回线程池句柄，失败NULL
*/
DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPoolFlex_Create(DTK_UINT32 nMaxThreadNum, DTK_UINT32 nIldeTime, DTK_VOIDPTR(CALLBACK *fWorkRoutine)(DTK_VOIDPTR), DTK_UINT32 nStackSize = 0);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPoolFlex_Destroy(DTK_HANDLE hHandle)
*   @brief 销毁线程池
*   @param [in] hHandle     线程池句柄
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPoolFlex_Destroy(DTK_HANDLE hHandle);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPoolFlex_Work(DTK_HANDLE hHandle, DTK_VOIDPTR pParam)
*   @brief 投递任务数据
*   @param [in] hHandle     线程池句柄
*   @param [in] pParam      任务数据
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPoolFlex_Work(DTK_HANDLE hHandle, DTK_VOIDPTR pParam);

#endif // __DTK_THREADPOOLFLEX_H__ 