
#include "DTK_Event.h"

DTK_DECLARE DTK_BOOL CALLBACK DTK_CreateEvent(DTK_EVENT_T* pEvent, DTK_BOOL bManualReset, DTK_BOOL bInitState)
{
#ifdef OS_WINDOWS
    *pEvent = CreateEvent(NULL, bManualReset, bInitState, NULL);
	if (NULL == *pEvent)
    {
        return DTK_FALSE;
    }
#else
    if (pthread_cond_init(pEvent, NULL) != 0)
    {
        return DTK_FALSE;
    }
#endif
    return DTK_TRUE;
}

DTK_DECLARE DTK_BOOL CALLBACK DTK_CloseEvent(DTK_EVENT_T* pEvent)
{
#ifdef OS_WINDOWS
	return CloseHandle(*pEvent);
#else
    return pthread_cond_destroy(pEvent);
#endif
}

DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForSingleObject(DTK_EVENT_T* pEvent, DTK_MUTEX_T* pMutex)
{
#ifdef OS_WINDOWS
	return WaitForSingleObject(*pEvent, DTK_INFINITE);
#else
    DTK_MutexLock(pMutex);
    //TO condition is false
    int iRet = pthread_cond_wait(pEvent, pMutex);
    //TO 
    DTK_MutexUnlock(pMutex);
    if (iRet == 0)
    {
        return DTK_WAIT_OBJECT_0;
    }
    return DTK_WAIT_FAILED;
#endif
}

DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForSingleObjectEx(DTK_EVENT_T* pEvent, DTK_UINT32 nTimeOut, DTK_MUTEX_T* pMutex)
{
#ifdef OS_WINDOWS
    return WaitForSingleObject(*pEvent, nTimeOut);
#else
    struct timespec stTime;
    stTime.tv_sec = nTimeOut / 1000;
    stTime.tv_nsec = (nTimeOut % 1000) * 1000;
    
    DTK_MutexLock(pMutex);
    //TO condition is false
    int iRet = pthread_cond_timedwait(pEvent, pMutex, &stTime);
    //TO 
    DTK_MutexUnlock(pMutex);
    if (iRet == 0)
    {
        return DTK_WAIT_OBJECT_0;
    }
    else if (iRet == ETIMEDOUT)
    {
        return DTK_WAIT_TIMEOUT;
    }
    return DTK_WAIT_FAILED;
#endif
}

DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForMultipleObjects(DTK_UINT32 nEventCount, DTK_EVENT_T* hEvents, DTK_BOOL bWaitAll, DTK_UINT32 nTimeOut)
{
#ifdef OS_WINDOWS
	return WaitForMultipleObjects(nEventCount,hEvents,bWaitAll,nTimeOut);
#else
    return DTK_NOSUPPORT;
#endif
}

DTK_DECLARE DTK_BOOL CALLBACK DTK_ResetEvent(DTK_EVENT_T* pEvent)
{
#ifdef OS_WINDOWS
	return ResetEvent(*pEvent);
#else
    return DTK_FALSE;
#endif
}

DTK_DECLARE DTK_BOOL CALLBACK DTK_SetEvent(DTK_EVENT_T* pEvent)
{
#ifdef OS_WINDOWS
	return SetEvent(*pEvent);
#else
    //TODO app locked
    if (pthread_cond_signal(pEvent) == 0)
    {
        return DTK_TRUE;
    }
    //TODO app unlocked
    return DTK_FALSE;
#endif
}

DTK_DECLARE DTK_BOOL CALLBACK DTK_SetEventEx(DTK_EVENT_T* pEvent)
{
#ifdef OS_WINDOWS
    return DTK_FALSE;
#else
    if (pthread_cond_broadcast(pEvent) == 0)
    {
        return DTK_TRUE;
    }
    return DTK_FALSE;
#endif
}