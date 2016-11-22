
#include "DTK_Semaphore.h"

DTK_DECLARE DTK_INT32 CALLBACK DTK_SemCreate(DTK_SEM_T* pSem, DTK_UINT32 nInitCount)
{
#ifdef OS_WINDOWS
	HANDLE hHandle = CreateSemaphore(NULL, nInitCount, 65535, NULL);
	if (hHandle == NULL)
	{
		return DTK_ERROR;
	}
	*pSem = hHandle;
#elif OS_POSIX
    if (sem_init(pSem, 0, nInitCount) != 0)
    {
        return DTK_ERROR;
    }
#endif

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SemDestroy(DTK_SEM_T* pSem)
{
    if (!pSem)
    {
        return DTK_ERROR;
    }

#ifdef OS_WINDOWS
	return (CloseHandle(*pSem)) ? DTK_OK : DTK_ERROR;
#elif OS_POSIX
    return sem_destroy(pSem);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SemWait(DTK_SEM_T* pSem)
{
    if (!pSem)
    {
        return DTK_ERROR;
    }

#ifdef OS_WINDOWS
	if (WAIT_OBJECT_0 == WaitForSingleObject(*pSem, INFINITE))
	{
		return DTK_OK;
	}
    return DTK_ERROR;

#elif OS_POSIX
    return sem_wait(pSem);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SemTimedWait(DTK_SEM_T* pSem, DTK_UINT32 nTimeOut)
{
    if (!pSem)
    {
        return DTK_ERROR;
    }

#ifdef OS_WINDOWS
	if (WaitForSingleObject(*pSem, nTimeOut) == WAIT_OBJECT_0)
	{
		return DTK_OK;
	}
	return DTK_ERROR;

#elif OS_POSIX
    struct timespec stTime;
    stTime.tv_sec = nTimeOut / 1000;
    stTime.tv_nsec = (nTimeOut % 1000) * 1000;
    return sem_timedwait(pSem, &stTime);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SemPost(DTK_SEM_T* pSem)
{
    if (!pSem)
    {
        return DTK_ERROR;
    }

#ifdef OS_WINDOWS
	return (ReleaseSemaphore(*pSem, 1, NULL)) ? DTK_OK : DTK_ERROR;
#elif OS_POSIX
    return sem_post(pSem);
#endif
}

//c++ ±‡“Î∆˜≤≈÷ß≥÷
#if defined (CXXCOMPILE)

DTK_Sema::DTK_Sema(DTK_UINT32 nInitCount)
{
	DTK_SemCreate(&m_sem, nInitCount);
}

DTK_Sema::~DTK_Sema()
{
	DTK_SemDestroy(&m_sem);
}

DTK_INT32 DTK_Sema::Wait()
{
	return DTK_SemWait(&m_sem);
}

DTK_INT32 DTK_Sema::TimedWait(DTK_UINT32 nTimeOut)
{
	return DTK_SemTimedWait(&m_sem, nTimeOut);
}

DTK_INT32 DTK_Sema::Post()
{
	return DTK_SemPost(&m_sem);
}

#endif
