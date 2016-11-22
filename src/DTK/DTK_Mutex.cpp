
#include "DTK_Mutex.h"

DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexCreate(DTK_MUTEX_T* pMutex)
{
#ifdef OS_WINDOWS
	__try
	{
		InitializeCriticalSection(pMutex);
	}
	__except(GetExceptionCode() == STATUS_NO_MEMORY)
	{
		return DTK_ERROR;
	}
#elif OS_POSIX
    pthread_mutexattr_t stAttr; 
    pthread_mutexattr_init(&stAttr); 
    //…Ë÷√recursive Ù–‘£¨‘ –Ì«∂Ã◊À¯
    pthread_mutexattr_settype(&stAttr, PTHREAD_MUTEX_RECURSIVE_NP); 
    pthread_mutex_init(pMutex, &stAttr);
#endif
	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexDestroy(DTK_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return DTK_ERROR;
	}

#ifdef OS_WINDOWS
	DeleteCriticalSection(pMutex);
#elif OS_POSIX
    pthread_mutex_destroy(pMutex);
#endif
	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexLock(DTK_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return DTK_ERROR;
	}
#ifdef OS_WINDOWS
	EnterCriticalSection(pMutex);
#elif OS_POSIX
    pthread_mutex_lock(pMutex);
#endif
	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexTryLock(DTK_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return DTK_ERROR;
	}

#ifdef OS_WINDOWS
	return (TryEnterCriticalSection(pMutex))?DTK_OK:DTK_ERROR;
#elif OS_POSIX
    return (pthread_mutex_trylock(pMutex))?DTK_OK:DTK_ERROR;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexUnlock(DTK_MUTEX_T* pMutex)
{
	if (!pMutex)
	{
		return DTK_ERROR;
	}
#ifdef OS_WINDOWS
	LeaveCriticalSection(pMutex);
#elif OS_POSIX
    pthread_mutex_unlock(pMutex);
#endif
	return DTK_OK;
}

#if defined (CXXCOMPILE)
DTK_Mutex::DTK_Mutex()
{
	DTK_MutexCreate(&m_mutex);
}

DTK_Mutex::~DTK_Mutex()
{
	DTK_MutexDestroy(&m_mutex);
}

DTK_INT32 DTK_Mutex::Lock()
{
	return DTK_MutexLock(&m_mutex);
}

DTK_INT32 DTK_Mutex::Unlock()
{
	return DTK_MutexUnlock(&m_mutex);
}

DTK_INT32 DTK_Mutex::TryLock()
{
	return DTK_MutexTryLock(&m_mutex);
}
#endif
