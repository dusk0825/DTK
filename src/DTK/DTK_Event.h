
#ifndef __DTK_EVENT_H__  
#define __DTK_EVENT_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"
#include "DTK_Mutex.h"

#if defined (OS_WINDOWS)
typedef DTK_HANDLE              DTK_EVENT_T;           
#define DTK_WAIT_OBJECT_0       WAIT_OBJECT_0				//第一个事件
#define DTK_WAIT_TIMEOUT		WAIT_TIMEOUT				//超时
#define DTK_WAIT_INVALID_PARAM	((DTK_UINT32)0x7ffffffe)	//参数出错
#define DTK_WAIT_INVALID_EVENT	((DTK_UINT32)0x7ffffffd)	//无效事件
#define DTK_WAIT_FAILED			((DTK_UINT32)0x7ffffffc)	//等待失败
#elif defined (OS_POSIX)
typedef pthread_cond_t          DTK_EVENT_T;
#define DTK_WAIT_OBJECT_0		((DTK_UINT32)0x0)
#define DTK_WAIT_TIMEOUT		((DTK_UINT32)0x7fffffff)
#define DTK_WAIT_INVALID_PARAM	((DTK_UINT32)0x7ffffffe)
#define DTK_WAIT_INVALID_EVENT	((DTK_UINT32)0x7ffffffd)
#define DTK_WAIT_FAILED			((DTK_UINT32)0x7ffffffc)
#endif

/** @fn DTK_DECLARE DTK_EVENT_T CALLBACK DTK_CreateEvent(DTK_BOOL bManualReset, DTK_BOOL bInitState)
*   @brief 创建一个事件句柄
*   @param [in] pEvent          事件指针
*   @param [in] bManualReset    事件恢复方式（DTK_TRUE：手动恢复，DTK_FALSE：自动恢复）
*   @param [in] bInitState      事件初始状态（DTK_TRUE：触发状态，DTK_FALSE：非触发状态）
*   @return DTK_TRUE 成功，DTK_FALSE失败
*/
DTK_DECLARE DTK_BOOL CALLBACK DTK_CreateEvent(DTK_EVENT_T* pEvent, DTK_BOOL bManualReset = DTK_FALSE, DTK_BOOL bInitState = DTK_FALSE);

/** @fn DTK_DECLARE DTK_BOOL CALLBACK DTK_CloseEvent(DTK_EVENT_T hEvent)
*   @brief 关闭一个事件句柄
*   @param [in] hEvent  事件句柄          
*   @return DTK_TRUE 成功，DTK_FALSE失败
*/
DTK_DECLARE DTK_BOOL CALLBACK DTK_CloseEvent(DTK_EVENT_T* pEvent);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForSingleObject(DTK_EVENT_T hEvent, DTK_MUTEX_T* pMutex = NULL)
*   @brief 等待单个事件消息到来
*   @param [in] hEvent  事件句柄
*   @param [in] pMutex  互斥锁
*   @return DTK_WAIT_OBJECT_0成功，DTK_WAIT_TIMEOUT超时失败，其他值失败
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForSingleObject(DTK_EVENT_T* pEvent, DTK_MUTEX_T* pMutex = NULL);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForSingleObject(DTK_EVENT_T hEvent, DTK_UINT32 nTimeOut)
*   @brief 等待单个事件消息到来
*   @param [in] hEvent  事件句柄
*   @param [in] nTimeOut 等待超时时间
*   @return DTK_WAIT_OBJECT_0成功，DTK_WAIT_TIMEOUT超时失败，其他值失败
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForSingleObjectEx(DTK_EVENT_T* pEvent, DTK_UINT32 nTimeOut, DTK_MUTEX_T* pMutex = NULL);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForMultipleObjects(DTK_UINT32 nEventCount, DTK_EVENT_T* hEvents, DTK_BOOL bWaitAll, DTK_UINT32 nTimeOut)
*   @brief 等待多个事件消息到来
*   @param [in] nEventCount 事件个数
*   @param [in] hEvents     事件句柄数组
*   @param [in] bWaitAll    是否全部等待，不支持，只能等待其中的一个事件到来
*   @param [in] nTimeOut    等待超时时间
*   @return DTK_WAIT_OBJECT_0成功，DTK_WAIT_TIMEOUT超时失败，其他值失败
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForMultipleObjects(DTK_UINT32 nEventCount, DTK_EVENT_T* hEvents, DTK_BOOL bWaitAll, DTK_UINT32 nTimeOut);

/** @fn DTK_DECLARE DTK_BOOL CALLBACK DTK_ResetEvent(DTK_EVENT_T hEvent)
*   @brief 重置事件消息
*   @param [in] hEvent  事件句柄
*   @return DTK_TRUE 成功，DTK_FALSE失败.
*/
DTK_DECLARE DTK_BOOL CALLBACK DTK_ResetEvent(DTK_EVENT_T* pEvent);

/** @fn DTK_DECLARE DTK_BOOL CALLBACK DTK_SetEvent(DTK_EVENT_T hEvent)
*   @brief 触发事件，【条件变量：一个等待线程可收到；事件：自动事件/手动重置事件】
*   @param [in] hEvent  事件句柄
*   @return DTK_TRUE 成功，DTK_FALSE失败.
*   @note 与win事件不同之处，条件变量触发时，如果没有线程等待，则自动复位。
*/
DTK_DECLARE DTK_BOOL CALLBACK DTK_SetEvent(DTK_EVENT_T* pEvent);

/** @fn DTK_DECLARE DTK_BOOL CALLBACK DTK_SetEventEx(DTK_EVENT_T hEvent)
*   @brief 触发事件，【条件变量：所有等待线程均可收到】
*   @param [in] hEvent  事件句柄
*   @return DTK_TRUE 成功，DTK_FALSE失败.
*/
DTK_DECLARE DTK_BOOL CALLBACK DTK_SetEventEx(DTK_EVENT_T* pEvent);

#endif // __DTK_EVENT_H__ 