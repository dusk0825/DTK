
#include "DTK_ThreadPool.h"
#include "DTK_Mutex.h"
#include "DTK_Semaphore.h"
#include "DTK_Thread.h"
#include "DTK_Utils.h"

#define MAX_THREADPOOL_SIZE         64      //����̳߳�����
#define MAX_THREAD_PER_THREADPOOL   512     //����߳�����

typedef struct tagThreadInfo
{
    DTK_UINT32  nWorkIndex;     //�̶߳����е�λ��
    DTK_VOIDPTR pThreadPool;    //�̳߳�����
    DTK_BOOL    bIdle;          //�߳��Ƿ���е���˼, TRUE�������, FALSE����������
    DTK_BOOL    bQuit;          //�߳��Ƿ��˳�
    DTK_HANDLE  hThreadId;      //�߳�ID, ͬʱ��������ж��߳��Ƿ񴴽�
    DTK_SEM_T   hSem;           //����˯���߳�
    DTK_ThreadPool_WorkRoutine fWork;//ִ���̻߳ص�����
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
    DTK_BOOL    bUsed;          //ʹ�ñ�ʶ
    DTK_BOOL    bQuit;          //�˳���ʶ
    DTK_UINT32  nMaxCount;      //�߳��������ֵ
    DTK_UINT32  nInitCount;     //�߳�������ʼֵ
    DTK_UINT32  nCurrCount;     //�߳�������ǰֵ
    DTK_UINT32  nStackSize;     //�̶߳�ջ��С
    DTK_UINT32  nTimeOut;       //����ʱ�䣨ms��, ���ָ��ʱ���߳�û������,���Զ��˳�

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
*   @brief ���̳߳��е��߳�״̬����Ϊ����
*   @param [in] pThreadPool �̳߳�ָ��
*   @param [in] pThreadInfo �߳���Ϣ
*   @return �ɹ�����0��ʧ��-1
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
*   @brief ��ȡ�̳߳��п���״̬���߳�
*   @param [in] pThreadPool �̳߳�ָ��
*   @return �ɹ������߳̾����ʧ�ܷ���NULL
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
            // �̴߳����ҿ���
            pThreadPool->struWorks[i].bIdle = DTK_FALSE;
            pTemThdPool = &pThreadPool->struWorks[i];
            break;
        }
    }
    DTK_MutexUnlock(&pThreadPool->hLock);

    return pTemThdPool;
}

/** @fn DTK_VOIDPTR CALLBACK GolbalThreadPoolCallBack(DTK_VOIDPTR pParam)
*   @brief �߳�ִ����
*   @param [in] pParam  �̲߳���
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

        // ǰnInitCount���̲߳����˳�
        if (pThreadPool->nTimeOut == DTK_INFINITE || pThreadInfo->nWorkIndex < pThreadPool->nInitCount)
        {
            if (DTK_SemWait(&pThreadInfo->hSem) == DTK_ERROR)
            {
                //�ź����쳣�����п������յ��ж��źţ�����������������ʱ�᷵��-1
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
                    // ��Ϊ���̳߳�ȫ�������,����ִ����δ���ʱ, ���ô����̲߳���ͬʱִ��
                    // DTK_DestroyThread_Local�ͷ��̺߳���ʱ�ᷢ���ź���,���Բ����ܽ��뵽������
                    DTK_SemDestroy(&pThreadInfo->hSem);
                    memset(pThreadInfo, 0x0, sizeof(THREAD_INFO_T));
                    pThreadInfo->hThreadId = DTK_INVALID_THREAD;

                    pThreadPool->nCurrCount--;
                    DTK_MutexUnlock(&pThreadPool->hLock);

                    // ���е������, ����߳̾��Ѿ����̳߳����κι�ϵ��
                    // ���WorkIndex���������´������߳���
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
*   @brief Ϊ�̳߳ش����߳�
*   @param [in] pThreadPool �̳߳���Ϣ
*   @return �ɹ�����0��ʧ��-1
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
    
    //�ҵ����е������������߳�
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
*   @brief Ϊ�̳߳������߳�
*   @param [in] pThreadPool �̳߳���Ϣ
*   @return �ɹ�����0��ʧ��-1
*/
DTK_INT32 DTK_DestroyThread_Local(THREAD_POOL_T* pThreadPool)
{
    DTK_MutexLock(&pThreadPool->hLock);
    if(pThreadPool->nCurrCount == 0)
    {
        DTK_MutexUnlock(&pThreadPool->hLock);
        return DTK_ERROR;
    }

    //�ҵ����е��߳������������߳�
    THREAD_INFO_T* pThreadInfo = NULL;
    for (DTK_UINT32 index = 0; index < pThreadPool->nMaxCount; index++)
    {
        pThreadInfo = &(pThreadPool->struWorks[index]);
        if(pThreadInfo->hThreadId == DTK_INVALID_THREAD)
        {
            continue;
        }

        // ��ʱbIdle����ΪFALSE��������������������߳̾Ͳ��ᱻ�������񣬴Ӷ������˳���
        pThreadInfo->bIdle = DTK_FALSE;
        pThreadInfo->bQuit = DTK_TRUE;
        DTK_SemPost(&pThreadInfo->hSem);
        DTK_MutexUnlock(&pThreadPool->hLock);

        //���������ڲ���ԭ�����߳��ڷ�����ж���ʱ��Ҫ�����,���������.
        DTK_Thread_Wait(pThreadInfo->hThreadId);

        //������������,��Ϊ����̵߳Ĵ���δ��ȫ�˳�,�������WorkIndex�ͱ������ȥ��
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
*   @brief ��ʼ���̳߳�
*   @param [in] pThreadPool �̳߳���Ϣ
*   @return �ɹ�����0��ʧ��-1
*/
DTK_INT32 DTK_InitThreadPool_Local(THREAD_POOL_T* pThreadPool)
{
    if (pThreadPool == NULL)
    {
        return DTK_ERROR;
    }

    //����InitCount���߳�
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
*   @brief ����ʼ���̳߳�   
*   @param [in] pThreadPool �̳߳���Ϣ
*   @return ��
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
*   @brief ��ȡ���е��̳߳�
*   @return �ɹ����ؿ��е��̳߳أ�ʧ��ΪNULL
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
*   @brief �����̳߳�
*   @param [in] pThreadPool �̳߳���Ϣ
*   @return ��
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
            //��̬�����߳���
            if (pThreadPool->nCurrCount < pThreadPool->nMaxCount)
            {
                if (DTK_CreateThread_Local(pThreadPool) == DTK_OK)
                {
                    //���������߳̾ͱ�������Ͷ������, ��ֹ�߳�������δͶ��ǰ���˳�
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
                //�����ķ���ʧ����������ģ��������ӱ�ǹ��ϲ�ѡ��ȴ����
                //�޸�bug:�̴߳ﵽ���ֵʱһֱ�ڻ�ȡ�����̣߳�����CPU�ܸߵ�����
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