
#ifndef __DTK_MUTEX_H__  
#define __DTK_MUTEX_H__  

#include "DTK_Types.h"
#include "DTK_Config.h"

#if defined(OS_WINDOWS)
typedef CRITICAL_SECTION DTK_MUTEX_T;
#define DTK_MUTEX_FAST			0
#define DTK_MUTEX_RECURSIVE		0
#define DTK_MUTEX_ERRORCHECK	0
#elif defined(OS_POSIX)
#include <pthread.h>
typedef pthread_mutex_t DTK_MUTEX_T;
#define DTK_MUTEX_FAST			PTHREAD_MUTEX_FAST_NP
#define DTK_MUTEX_RECURSIVE		PTHREAD_MUTEX_RECURSIVE_NP
#define DTK_MUTEX_ERRORCHECK	PTHREAD_MUTEX_ERRORCHECK_NP
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexCreate(DTK_MUTEX_T* pMutex)
*   @brief 创建互斥体
*   @param [out] pMutex     互斥体
*   @return 成功返回0，其他错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexCreate(DTK_MUTEX_T* pMutex); 

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexDestroy(DTK_MUTEX_T* pMutex)
*   @brief 销毁互斥体
*   @param [int] pMutex     互斥体
*   @return 成功返回0，其他错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexDestroy(DTK_MUTEX_T* pMutex); 

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexLock(DTK_MUTEX_T* pMutex)
*   @brief 上锁
*   @param [int] pMutex     互斥体
*   @return 成功返回0，其他错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexLock(DTK_MUTEX_T* pMutex); 

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexTryLock(DTK_MUTEX_T* pMutex)
*   @brief 尝试上锁
*   @param [int] pMutex     互斥体
*   @return 成功返回0，其他错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexTryLock(DTK_MUTEX_T* pMutex);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexUnlock(DTK_MUTEX_T* pMutex)
*   @brief 解锁
*   @param [int] pMutex     互斥体
*   @return 成功返回0，其他错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexUnlock(DTK_MUTEX_T* pMutex); 

//c++ 编译器才支持
#if defined (CXXCOMPILE)

class DTK_DECLARE_CLASS DTK_Mutex
{
public:
    DTK_Mutex();
    DTK_Mutex(DTK_UINT32 nFlag);
    ~DTK_Mutex();

    DTK_INT32 Lock();
    DTK_INT32 Unlock();
    DTK_INT32 TryLock();

private:
    DTK_MUTEX_T m_mutex;
};

#endif

#endif // __DTK_MUTEX_H__ 