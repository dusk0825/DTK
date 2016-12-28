
#ifndef __DTK_THREADPOOL_H__  
#define __DTK_THREADPOOL_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

/** @fn DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPool_Create(DTK_UINT32 InitThreadNum, DTK_UINT32 MaxThreadNum, DTK_UINT32 StackSize = 0)
*   @brief 创建线程池
*   @param [in] InitThreadNum   初始线程数
*   @param [in] MaxThreadNum    最大线程数
*   @param [in] StackSize       线程堆栈大小
*   @param [in] TimeOut         线程空闲退出时间
*   @return 成功返回线程池句柄，失败返回NULL
*/
DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPool_Create(DTK_UINT32 InitThreadNum, DTK_UINT32 MaxThreadNum, DTK_UINT32 StackSize = 0);
DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPool_CreateFlex(DTK_UINT32 InitThreadNum, DTK_UINT32 MaxThreadNum, DTK_UINT32 StackSize, DTK_UINT32 TimeOut);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_Destroy(DTK_HANDLE ThreadPoolHandle)
*   @brief 销毁线程池
*   @param [in] ThreadPoolHandle    线程池句柄
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_Destroy(DTK_HANDLE ThreadPoolHandle);

/** @fn typedef DTK_VOIDPTR (CALLBACK *DTK_ThreadPool_WorkRoutine)(DTK_VOIDPTR)
*   @brief 线程池任务执行体函数指针
*   @return 无
*/
typedef DTK_VOIDPTR (CALLBACK *DTK_ThreadPool_WorkRoutine)(DTK_VOIDPTR);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_Work(DTK_HANDLE ThreadPoolHandle, DTK_VOIDPTR(CALLBACK *WorkRoutine)(DTK_VOIDPTR), DTK_VOIDPTR Params)
*   @brief 投递任务到线程池
*   @param [in] ThreadPoolHandle    线程池句柄
*   @param [in] WorkRoutine         任务执行体指针
*   @param [in] Params              任务参数指针
*   @param [in] bWaitForIdle        无空闲线程时，是否等待
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_Work(DTK_HANDLE ThreadPoolHandle, DTK_ThreadPool_WorkRoutine pfnWork, DTK_VOIDPTR Params);
DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_WorkEx(DTK_HANDLE ThreadPoolHandle, DTK_ThreadPool_WorkRoutine pfnWork, DTK_VOIDPTR Params, DTK_BOOL bWaitForIdle);

#endif // __DTK_THREADPOOL_H__ 