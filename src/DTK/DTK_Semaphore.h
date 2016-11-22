
#ifndef __DTK_SEMAPHORE_H__  
#define __DTK_SEMAPHORE_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

#if defined (OS_WINDOWS)
	typedef DTK_HANDLE DTK_SEM_T;
#elif defined (OS_POSIX)
	#include <semaphore.h>
    typedef sem_t DTK_SEM_T;
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SemCreate(DTK_SEM_T* pSem, DTK_UINT32 nInitCount)
*   @brief 创建信号量
*   @param [in] pSem        信号量指针
*   @param [in] nInitCount  信号量初始值
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SemCreate(DTK_SEM_T* pSem, DTK_UINT32 nInitCount);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SemDestroy(DTK_SEM_T* pSem)
*   @brief 销毁信号量
*   @param [in] pSem        信号量指针
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SemDestroy(DTK_SEM_T* pSem);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SemWait(DTK_SEM_T* pSem)
*   @brief 等待信号量
*   @param [in] pSem        信号量指针
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SemWait(DTK_SEM_T* pSem);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SemTimedWait(DTK_SEM_T* pSem, DTK_UINT32 nTimeOut)
*   @brief 超时等待信号量
*   @param [in] pSem        信号量指针
*   @param [in] nTimeOut    超时时间，单位毫秒
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SemTimedWait(DTK_SEM_T* pSem, DTK_UINT32 nTimeOut);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SemPost(DTK_SEM_T* pSem)
*   @brief 释放信号量
*   @param [in] pSem        信号量指针
*   @return 成功返回0，其他为错误码
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SemPost(DTK_SEM_T* pSem);

//c++ 编译器才支持
#if defined (CXXCOMPILE)

class DTK_DECLARE_CLASS DTK_Sema
{
public:
	DTK_Sema(DTK_UINT32 nInitCount);
	~DTK_Sema();

public:
	DTK_INT32 Wait();
	DTK_INT32 TimedWait(DTK_UINT32 nTimeOut);
	DTK_INT32 Post();

private:
	DTK_SEM_T m_sem;
};

#endif

#endif // __DTK_SEMAPHORE_H__ 
