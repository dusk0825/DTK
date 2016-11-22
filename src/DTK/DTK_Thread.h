#ifndef __DTK_THREAD_H__  
#define __DTK_THREAD_H__  

#include "DTK_Types.h"
#include "DTK_Config.h"

#ifdef OS_WINDOWS
#define DTK_INVALID_THREAD (DTK_HANDLE)(NULL)
#elif defined OS_POSIX
#define DTK_INVALID_THREAD (DTK_HANDLE)(pthread_t)(-1)
#endif


/** @fn DTK_DECLARE DTK_HANDLE CALLBACK DTK_Thread_Create(DTK_VOIDPTR (CALLBACK *StartAddress)(DTK_VOIDPTR), DTK_VOID* Params, DTK_UINT32 StackSize,\
                        DTK_BOOL IsSuspend = DTK_FALSE, DTK_INT32 Priority = 0, DTK_INT32 SchedPolicy = 0);
*   @brief 创建线程
*   @param [in] StartAddress    线程执行体
*   @param [in] Params          执行体参数
*   @param [in] StackSize       线程堆栈大小
*   @param [in] IsSuspend       是否挂起
*   @param [in] Priority        线程优先级
*   @param [in] SchedPolicy     线程调度策略
*   @return 成功返回线程句柄，失败返回DTK_INVALID_HANDLE
*/
DTK_DECLARE DTK_HANDLE CALLBACK DTK_Thread_Create(DTK_VOIDPTR (CALLBACK *StartAddress)(DTK_VOIDPTR), DTK_VOID* Params, DTK_UINT32 StackSize,\
						DTK_BOOL IsSuspend = DTK_FALSE, DTK_INT32 Priority = 0, DTK_INT32 SchedPolicy = 0);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Wait(DTK_HANDLE ThreadHandle)
*   @brief 等待线程线程的退出并关闭相应的句柄
*   @param [in] ThreadHandl     线程句柄
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Wait(DTK_HANDLE ThreadHandle);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Suspend(DTK_HANDLE ThreadHandle)
*   @brief 挂起线程
*   @param [in] ThreadHandle    线程句柄
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Suspend(DTK_HANDLE ThreadHandle);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Resume(DTK_HANDLE ThreadHandle)
*   @brief 恢复线程
*   @param [in] ThreadHandle    线程句柄
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Resume(DTK_HANDLE ThreadHandle);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Resume(DTK_HANDLE ThreadHandle)
*   @brief 设置线程的优先级
*   @param [in] ThreadHandle    线程句柄
*   @param [in] Priority        线程优先级
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_SetPriority(DTK_HANDLE ThreadHandle, DTK_INT32 Priority);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_SetSchedPolicy(DTK_HANDLE ThreadHandle, DTK_INT32 SchedPolicy)
*   @brief 设置调度策略
*   @param [in] ThreadHandle    线程句柄
*   @param [in] Priority        调度策略
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_SetSchedPolicy(DTK_HANDLE ThreadHandle, DTK_INT32 SchedPolicy);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Exit()
*   @brief 退出线程
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Exit();

/** @fn DTK_DECLARE DTK_HANDLE CALLBACK DTK_Thread_GetSelfId()
*   @brief  获取线程ID
*   @return 线程ID
*/
DTK_DECLARE DTK_HANDLE CALLBACK DTK_Thread_GetSelfId();

#endif // __DTK_THREAD_H__ 