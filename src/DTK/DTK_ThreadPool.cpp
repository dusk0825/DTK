
#include "DTK_ThreadPool.h"
#include "DTK_Mutex.h"
#include "DTK_Semaphore.h"
#include "DTK_Thread.h"
#include "DTK_Utils.h"

#define MAX_THREADPOOL_SIZE         64      //最大线程池数量
#define MAX_THREAD_PER_THREADPOOL   512     //最大线程数量

typedef struct tagThreadInfo
{
    DTK_UINT32  nWorkIndex;     //线程队列中的位置
    DTK_VOIDPTR pThreadPool;    //线程池索引
    DTK_BOOL    bIdle;          //线程是否空闲的意思, TRUE代表空闲, FALSE代表有任务
    DTK_BOOL    bQuit;          //线程是否退出
    DTK_HANDLE  hThreadId;      //线程ID, 同时用这个来判断线程是否创建
    DTK_SEM_T   hSem;           //唤醒睡眠线程
    DTK_ThreadPool_WorkRoutine fWork;//执行线程回调函数
    DTK_VOIDPTR pParam;

    tagThreadInfo()
    {
        nWorkIndex = 0;
        pThreadPool = NULL;
        bIdle = DTK_FALSE;
        bQuit = DTK_TRUE;
        hThreadId = DTK_INVALID_THREAD;
        fWork = NULL;
        pParam = NULL;
    }
}THREAD_INFO_T;

typedef struct tagThreadPool
{
    DTK_BOOL    bUsed;          //使用标识
    DTK_BOOL    bQuit;          //退出标识
    DTK_UINT32  nMaxCount;      //线程数量最大值
    DTK_UINT32  nInitCount;     //线程数量初始值
    DTK_UINT32  nCurrCount;     //线程数量当前值
    DTK_UINT32  nStackSize;     //线程堆栈大小
    DTK_UINT32  nTimeOut;       //空闲时间（ms）, 如果指定时间线程没有任务,会自动退出

    DTK_MUTEX_T hLock;
    THREAD_INFO_T struWorks[MAX_THREAD_PER_THREADPOOL];

    tagThreadPool()
    {
        bUsed = DTK_FALSE;
        bQuit = DTK_TRUE;
        nMaxCount = 0;
        nInitCount = 0;
        nCurrCount = 0;
        nStackSize = 0;
        nTimeOut = 0;
    }
}THREAD_POOL_T;

static DTK_MUTEX_T   g_csLock;
static THREAD_POOL_T g_stThreadPools[MAX_THREADPOOL_SIZE];

DTK_INT32 DTK_InitThreadPool_Inter();
DTK_VOID DTK_FiniThreadPool_Inter();


DTK_INT32 DTK_InitThreadPool_Inter()
{
    return DTK_MutexCreate(&g_csLock);
}

DTK_VOID DTK_FiniThreadPool_Inter()
{
    DTK_MutexDestroy(&g_csLock);
}

/** @fn DTK_INT32 DTK_MoveToIdle_Local(THREAD_POOL_T* pThreadPool, THREAD_INFO_T* pThreadInfo)
*   @brief 将线程池中的线程状态调整为空闲
*   @param [in] pThreadPool 线程池指针
*   @param [in] pThreadInfo 线程信息
*   @return 成功返回0，失败-1
*/
DTK_INT32 DTK_MoveToIdle_Local(THREAD_POOL_T* pThreadPool, THREAD_INFO_T* pThreadInfo)
{
    if (NULL == pThreadPool || NULL == pThreadInfo)
    {
        return DTK_ERROR;
    }

    DTK_MutexLock(&pThreadPool->hLock);
    pThreadPool->struWorks[pThreadInfo->nWorkIndex].bIdle = DTK_TRUE;
    DTK_MutexUnlock(&pThreadPool->hLock);

    return DTK_OK;
}

/** @fn THREAD_INFO_T* DTK_GetIdle_Local(THREAD_POOL_T* pThreadPool)
*   @brief 获取线程池中空闲状态的线程
*   @param [in] pThreadPool 线程池指针
*   @return 成功返回线程句柄，失败返回NULL
*/
THREAD_INFO_T* DTK_GetIdle_Local(THREAD_POOL_T* pThreadPool)
{
    if (NULL == pThreadPool)
    {
        return NULL;
    }

    THREAD_INFO_T* pTemThdPool = NULL;

    DTK_MutexLock(&pThreadPool->hLock);
    for (DTK_UINT32 i = 0; i < pThreadPool->nMaxCount; i++)
    {
        if ((pThreadPool->struWorks[i].hThreadId != DTK_INVALID_THREAD)
            && (pThreadPool->struWorks[i].bIdle == DTK_TRUE))
        {
            // 线程存在且空闲
            pThreadPool->struWorks[i].bIdle = DTK_FALSE;
            pTemThdPool = &pThreadPool->struWorks[i];
            break;
        }
    }
    DTK_MutexUnlock(&pThreadPool->hLock);

    return pTemThdPool;
}

/** @fn DTK_VOIDPTR CALLBACK GolbalThreadPoolCallBack(DTK_VOIDPTR pParam)
*   @brief 线程执行体
*   @param [in] pParam  线程参数
*   @return NULL
*/
DTK_VOIDPTR CALLBACK GolbalThreadPoolCallBack(DTK_VOIDPTR pParam)
{
    THREAD_INFO_T* pThreadInfo = (THREAD_INFO_T*) (pParam);
    if (NULL == pThreadInfo)
    {
        DTK_OutputDebug("GolbalThreadPoolCallBack pThreadInfo == NULL");
        return NULL;
    }

    while (1)
    {
        THREAD_POOL_T* pThreadPool = (THREAD_POOL_T*)(pThreadInfo->pThreadPool);

        // 前nInitCount个线程不会退出
        if (pThreadPool->nTimeOut == DTK_INFINITE || pThreadInfo->nWorkIndex < pThreadPool->nInitCount)
        {
            if (DTK_SemWait(&pThreadInfo->hSem) == DTK_ERROR)
            {
                //信号量异常返回有可能是收到中断信号，并非真正的有数据时会返回-1
                continue;
            }
        }
        else
        {
            if (DTK_SemTimedWait(&pThreadInfo->hSem, pThreadPool->nTimeOut) == DTK_ERROR)
            {
                DTK_HANDLE hThreadId = pThreadInfo->hThreadId;        
                DTK_MutexLock(&pThreadPool->hLock);
                if (DTK_SemTimedWait(&pThreadInfo->hSem, 0) == DTK_ERROR)
                {
                    // 因为在线程池全局锁里边,所以执行这段代码时, 调用创建线程不能同时执行
                    // DTK_DestroyThread_Local释放线程函数时会发送信号量,所以不可能进入到这个里边
                    DTK_SemDestroy(&pThreadInfo->hSem);
                    memset(pThreadInfo, 0x0, sizeof(THREAD_INFO_T));
                    pThreadInfo->hThreadId = DTK_INVALID_THREAD;

                    pThreadPool->nCurrCount--;
                    DTK_MutexUnlock(&pThreadPool->hLock);

                    // 运行到这里后, 这个线程就已经和线程池无任何关系了
                    // 这个WorkIndex可以留给新创建的线程了
#ifdef OS_WINDOWS
                    CloseHandle(hThreadId);
#elif defined(OS_POSIX)
                    pthread_detach((pthread_t)hThreadId);
#else
#error OS Not Implement Yet.
#endif
                    return NULL; 
                }

                DTK_MutexUnlock(&pThreadPool->hLock);
            }
        }

        if (pThreadInfo->bQuit)
        {
            break;
        }
        else
        {
            if (pThreadInfo->fWork)
            {
                pThreadInfo->fWork(pThreadInfo->pParam);
                pThreadInfo->fWork = NULL;
                pThreadInfo->pParam = NULL;
            }

            DTK_MoveToIdle_Local((THREAD_POOL_T*)(pThreadInfo->pThreadPool), pThreadInfo);
        }
    }

    return NULL;
}

/** @fn DTK_INT32 DTK_CreateThread_Local(THREAD_POOL_T* pThreadPool)
*   @brief 为线程池创建线程
*   @param [in] pThreadPool 线程池信息
*   @return 成功返回0，失败-1
*/
DTK_INT32 DTK_CreateThread_Local(THREAD_POOL_T* pThreadPool)
{
    if (pThreadPool->nCurrCount == pThreadPool->nMaxCount)
    {
        DTK_OutputDebug("DTK_CreateThread_Local (pThreadPool->nCurrCount == pThreadPool->nMaxCount) error");
        return DTK_ERROR;
    }

    THREAD_INFO_T* pThreadInfo = NULL;
    DTK_HANDLE hThreadId = DTK_INVALID_THREAD;
    
    //找到空闲的索引，创建线程
    for (DTK_UINT32 index = 0; index < pThreadPool->nMaxCount; index++)
    {
        pThreadInfo = &(pThreadPool->struWorks[index]);
        if(pThreadInfo->hThreadId != DTK_INVALID_THREAD)
        {
            continue;
        }

        if (DTK_ERROR == DTK_SemCreate(&pThreadInfo->hSem, 0))
        {
            DTK_OutputDebug("DTK_CreateThread_Local DTK_SemCreate error");
            return DTK_ERROR;
        }

        pThreadInfo->fWork = NULL;
        pThreadInfo->bQuit = DTK_FALSE;
        pThreadInfo->pThreadPool = pThreadPool;
        pThreadInfo->nWorkIndex = index;

        DTK_INT32 nStackSize = pThreadPool->nStackSize == 0 ? (1024 * 1024) : pThreadPool->nStackSize;
        hThreadId = DTK_Thread_Create(GolbalThreadPoolCallBack, pThreadInfo, nStackSize);
        if(hThreadId == DTK_INVALID_THREAD)
        {
            DTK_OutputDebug("DTK_CreateThread_Local DTK_Thread_Create error");

            DTK_SemDestroy(&pThreadInfo->hSem);
            memset(pThreadInfo, 0x0, sizeof(THREAD_INFO_T));
            pThreadInfo->hThreadId = DTK_INVALID_THREAD;
            return DTK_ERROR;
        }

        pThreadInfo->bIdle = DTK_TRUE;
        pThreadInfo->hThreadId = hThreadId;   
        pThreadPool->nCurrCount++;
        return DTK_OK;
    }

    return DTK_ERROR;
}

/** @fn DTK_INT32 DTK_CreateThread_Local(THREAD_POOL_T* pThreadPool)
*   @brief 为线程池销毁线程
*   @param [in] pThreadPool 线程池信息
*   @return 成功返回0，失败-1
*/
DTK_INT32 DTK_DestroyThread_Local(THREAD_POOL_T* pThreadPool)
{
    DTK_MutexLock(&pThreadPool->hLock);
    if(pThreadPool->nCurrCount == 0)
    {
        DTK_MutexUnlock(&pThreadPool->hLock);
        return DTK_ERROR;
    }

    //找到空闲的线程索引，销毁线程
    THREAD_INFO_T* pThreadInfo = NULL;
    for (DTK_UINT32 index = 0; index < pThreadPool->nMaxCount; index++)
    {
        pThreadInfo = &(pThreadPool->struWorks[index]);
        if(pThreadInfo->hThreadId == DTK_INVALID_THREAD)
        {
            continue;
        }

        // 此时bIdle必须为FALSE，代表有任务，这样这个线程就不会被分配任务，从而可以退出了
        pThreadInfo->bIdle = DTK_FALSE;
        pThreadInfo->bQuit = DTK_TRUE;
        DTK_SemPost(&pThreadInfo->hSem);
        DTK_MutexUnlock(&pThreadPool->hLock);

        //不放在锁内部的原因是线程在放入空闲队列时需要这个锁,会造成死锁.
        DTK_Thread_Wait(pThreadInfo->hThreadId);

        //必须放在锁里边,因为这个线程的处理还未完全退出,否则这个WorkIndex就被分配出去了
        DTK_MutexLock(&pThreadPool->hLock);
        DTK_SemDestroy(&pThreadInfo->hSem);
        memset(pThreadInfo, 0x0, sizeof(THREAD_INFO_T));
        pThreadInfo->hThreadId = DTK_INVALID_THREAD;
        pThreadPool->nCurrCount--;

        break;
    }

    DTK_MutexUnlock(&pThreadPool->hLock);
    return DTK_OK;
}

/** @fn DTK_INT32 DTK_InitThreadPool_Local(THREAD_POOL_T* pThreadPool)
*   @brief 初始化线程池
*   @param [in] pThreadPool 线程池信息
*   @return 成功返回0，失败-1
*/
DTK_INT32 DTK_InitThreadPool_Local(THREAD_POOL_T* pThreadPool)
{
    if (pThreadPool == NULL)
    {
        return DTK_ERROR;
    }

    //创建InitCount个线程
    DTK_INT32 iRet = DTK_OK;
    DTK_MutexCreate(&pThreadPool->hLock);
    DTK_MutexLock(&pThreadPool->hLock);
    for (DTK_UINT32 i = 0; i < pThreadPool->nInitCount; i++)
    {
        if (DTK_ERROR == DTK_CreateThread_Local(pThreadPool))
        {
            DTK_OutputDebug("DTK_InitThread_Local DTK_CreateThread_Local error");
            iRet = DTK_ERROR;
            break;
        }
    }
    DTK_MutexUnlock(&pThreadPool->hLock);

    if (iRet == DTK_ERROR)
    {
        for (DTK_UINT32 i = 0; i < pThreadPool->nMaxCount; i++)
        {
            DTK_DestroyThread_Local(pThreadPool);
        }
    }

    return iRet;
}

/** @fn DTK_VOID DTK_FiniThreadPool_Local(THREAD_POOL_T* pThreadPool)
*   @brief 反初始化线程池   
*   @param [in] pThreadPool 线程池信息
*   @return 无
*/
DTK_VOID DTK_FiniThreadPool_Local(THREAD_POOL_T* pThreadPool)
{
    pThreadPool->bQuit = DTK_TRUE;
    for (DTK_UINT32 i = 0; i < pThreadPool->nMaxCount; i++)
    {
        DTK_DestroyThread_Local(pThreadPool);
    }

    DTK_MutexDestroy(&pThreadPool->hLock);

    return;
}

/** @fn THREAD_POOL_T* DTK_GetIdleThreadPool_Local()
*   @brief 获取空闲的线程池
*   @return 成功返回空闲的线程池，失败为NULL
*/
THREAD_POOL_T* DTK_GetIdleThreadPool_Local()
{
    THREAD_POOL_T* pThreadPool = NULL;

    DTK_MutexLock(&g_csLock);

    for (DTK_INT32 i = 0; i<MAX_THREADPOOL_SIZE; i++)
    {
        if (DTK_FALSE == g_stThreadPools[i].bUsed)
        {
            g_stThreadPools[i].bUsed = DTK_TRUE;
            pThreadPool = &g_stThreadPools[i];
            break;
        }
    }
    DTK_MutexUnlock(&g_csLock);

    return pThreadPool;
}

/** @fn DTK_VOID DTK_MoveToIdleThreadPool_Local(THREAD_POOL_T* pThreadPool)
*   @brief 回收线程池
*   @param [in] pThreadPool 线程池信息
*   @return 无
*/
DTK_VOID DTK_MoveToIdleThreadPool_Local(THREAD_POOL_T* pThreadPool)
{
    DTK_MutexLock(&g_csLock);
    pThreadPool->bUsed = DTK_FALSE;
    DTK_MutexUnlock(&g_csLock);
    return;
}
///////////////////////////////////////////////////////////////////////////////

DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPool_Create(DTK_UINT32 nInitCount, DTK_UINT32 nMaxCount, DTK_UINT32 nStackSize)
{
    return DTK_ThreadPool_CreateFlex(nInitCount, nMaxCount, nStackSize, DTK_INFINITE);
}

DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPool_CreateFlex(DTK_UINT32 nInitCount, DTK_UINT32 nMaxCount, DTK_UINT32 nStackSize, DTK_UINT32 nTimeOut)
{
    if (nMaxCount > MAX_THREAD_PER_THREADPOOL)
    {
        DTK_OutputDebug("DTK_ThreadPool_Create nMaxCount > MAX_THREAD_PER_THREADPOOL error");
        return NULL;
    }

    THREAD_POOL_T* pThreadPool = DTK_GetIdleThreadPool_Local();
    if (pThreadPool == NULL)
    {
        DTK_OutputDebug("DTK_ThreadPool_Create DTK_GetIdleThreadPool_Local error");
        return NULL;
    }

    pThreadPool->nInitCount = nInitCount;
    pThreadPool->nMaxCount = nMaxCount;
    pThreadPool->nCurrCount = 0;
    pThreadPool->nStackSize = nStackSize;
    pThreadPool->nTimeOut = nTimeOut;

    if (DTK_InitThreadPool_Local(pThreadPool) == DTK_OK)
    {
        pThreadPool->bQuit = DTK_FALSE;
    }
    else
    {
        DTK_OutputDebug("DTK_ThreadPool_Create DTK_InitThread_Local error");
        DTK_MoveToIdleThreadPool_Local(pThreadPool);
    }

    return pThreadPool;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_Destroy(DTK_HANDLE hHandle)
{
	if (NULL == hHandle)
	{
		return DTK_ERROR;
	}

    DTK_FiniThreadPool_Local((THREAD_POOL_T*)(hHandle));
    DTK_MoveToIdleThreadPool_Local((THREAD_POOL_T*)(hHandle));

    return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_Work(DTK_HANDLE hHandle, DTK_ThreadPool_WorkRoutine pfnWork, void* pParam)
{
    return DTK_ThreadPool_WorkEx(hHandle, pfnWork, pParam, DTK_FALSE);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_WorkEx(DTK_HANDLE hHandle, DTK_ThreadPool_WorkRoutine pfnWork, void* pParam, DTK_BOOL bWaitForIdle)
{
    if ((NULL == hHandle) || (NULL == pfnWork))
    {
        return DTK_ERROR;
    }

    THREAD_INFO_T* pThreadInfo = NULL;
    THREAD_POOL_T* pThreadPool = (THREAD_POOL_T*) (hHandle);

    while (pThreadPool->bQuit == DTK_FALSE)
    {
        DTK_MutexLock(&pThreadPool->hLock);                
        pThreadInfo = DTK_GetIdle_Local(pThreadPool);
        if (NULL == pThreadInfo)
        {
            //动态增加线程数
            if (pThreadPool->nCurrCount < pThreadPool->nMaxCount)
            {
                if (DTK_CreateThread_Local(pThreadPool) == DTK_OK)
                {
                    //当创建了线程就必须立刻投递任务, 防止线程在任务未投递前就退出
                    pThreadInfo = DTK_GetIdle_Local(pThreadPool);
                    pThreadInfo->fWork = pfnWork;
                    pThreadInfo->pParam = pParam;
                    DTK_SemPost(&pThreadInfo->hSem);
                    DTK_MutexUnlock(&pThreadPool->hLock);
                    return DTK_OK;
                }
                else                           
                {
                    DTK_MutexUnlock(&pThreadPool->hLock);
                    return DTK_ERROR; 
                }
            }
            else if (!bWaitForIdle)
            {
                //单纯的返回失败是有问题的，因此这里加标记供上层选择等待与否
                //修改bug:线程达到最大值时一直在获取空余线程，导致CPU很高的问题
                DTK_MutexUnlock(&pThreadPool->hLock);
                return DTK_ERROR;
            }
            else
            {
                DTK_MutexUnlock(&pThreadPool->hLock);
                DTK_Sleep(50);
                continue;
            }
        }
        else
        {
            pThreadInfo->fWork = pfnWork;
            pThreadInfo->pParam = pParam;
            DTK_SemPost(&pThreadInfo->hSem);
        }

        DTK_MutexUnlock(&pThreadPool->hLock); 
        return DTK_OK;
    }

    return DTK_ERROR;
}