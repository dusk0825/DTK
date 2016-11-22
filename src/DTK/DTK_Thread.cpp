
#include "DTK_Thread.h"

#ifdef OS_WINDOWS
#include <process.h>
#elif OS_POSIX
#include <pthread.h>
#endif


DTK_DECLARE DTK_HANDLE CALLBACK DTK_Thread_Create(DTK_VOIDPTR (CALLBACK *StartAddress)(DTK_VOIDPTR), DTK_VOID* Params, DTK_UINT32 StackSize,\
						DTK_BOOL IsSuspend, DTK_INT32 Priority, DTK_INT32 SchedPolicy)
{
#ifdef OS_WINDOWS
    unsigned int initflag = 0;
    if (IsSuspend)
    {
        initflag = CREATE_SUSPENDED;
    }
	return (DTK_HANDLE)_beginthreadex(NULL, StackSize, (unsigned (__stdcall*)(void*))StartAddress, Params, initflag, NULL);
#elif OS_POSIX
    pthread_attr_t stAttr;
    pthread_attr_init (&stAttr);
    if (StackSize > 0)
    {
        pthread_attr_setstacksize(&stAttr, StackSize);
    }
    pthread_t thId = 0;
	if (pthread_create(&thId, &stAttr, (void* (CALLBACK*)(void*))StartAddress, Params) != 0)
    {
        return DTK_INVALID_THREAD;
    }
    return (DTK_HANDLE)thId;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Wait(DTK_HANDLE ThreadHandle)
{
	if (ThreadHandle == (DTK_HANDLE)DTK_INVALID_THREAD)
	{
		return DTK_ERROR;
	}

#ifdef OS_WINDOWS
	if (WAIT_OBJECT_0 == WaitForSingleObject(ThreadHandle, INFINITE))
	{
		CloseHandle(ThreadHandle);
		return DTK_OK;
	}
#elif OS_POSIX
    pthread_join((pthread_t)ThreadHandle, NULL);
    ThreadHandle = DTK_INVALID_THREAD;
#endif

	return DTK_ERROR;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Suspend(DTK_HANDLE ThreadHandle)
{
#ifdef OS_WINDOWS
	if (ThreadHandle == (DTK_HANDLE)DTK_INVALID_THREAD)
	{
		return DTK_ERROR;
	}

	DWORD ret = SuspendThread(ThreadHandle);
	if (ret == 0xFFFFFFFF)
	{
		return DTK_ERROR;
	}
	else
	{
		return DTK_OK;
	}
#elif OS_POSIX
    return DTK_NOSUPPORT;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Resume(DTK_HANDLE ThreadHandle)
{
#ifdef OS_WINDOWS
	if (ThreadHandle == (DTK_HANDLE)DTK_INVALID_THREAD)
	{
		return DTK_ERROR;
	}

	DWORD ret = ResumeThread(ThreadHandle);
	if (ret == 0xFFFFFFFF)
	{
		return DTK_ERROR;
	}
	else
	{
		return DTK_OK;
	}
#elif OS_POSIX
    return DTK_NOSUPPORT;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_SetPriority(DTK_HANDLE ThreadHandle, DTK_INT32 Priority)
{
#ifdef OS_WINDOWS
	if (ThreadHandle == (DTK_HANDLE)DTK_INVALID_THREAD)
	{
		return DTK_ERROR;
	}

	BOOL bret = SetThreadPriority(ThreadHandle, Priority);
	if (bret)
	{
		return DTK_OK;
	}
	else
	{
		return DTK_ERROR;
	}
#elif OS_POSIX
    return DTK_NOSUPPORT;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_SetSchedPolicy(DTK_HANDLE ThreadHandle, DTK_INT32 SchedPolicy)
{
#ifdef OS_WINDOWS
	if (ThreadHandle == (DTK_HANDLE)DTK_INVALID_THREAD)
	{
		return DTK_ERROR;
	}
	BOOL bret = SetPriorityClass(GetCurrentProcess(), SchedPolicy);
	if (bret)
	{
		return DTK_OK;
	}
	else
	{
		return DTK_ERROR;
	}
#else
	return DTK_NOSUPPORT;
#endif
}

DTK_DECLARE DTK_HANDLE CALLBACK DTK_Thread_GetSelfId()
{
#ifdef OS_WINDOWS
	return (DTK_HANDLE)(LPARAM)::GetCurrentThreadId();
#elif OS_POSIX
    return (DTK_HANDLE)pthread_self();
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Exit()
{
#ifdef OS_WINDOWS
    _endthread();
#elif OS_POSIX
    pthread_exit(NULL);
#endif
    return 0;
}