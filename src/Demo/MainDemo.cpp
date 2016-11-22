
#include <iostream>
#include "DTK_String.h"
#include "DTK_Thread.h"
#include "DTK_Error.h"
#include "DTK_Mutex.h"
#include "DTK_Semaphore.h"

DTK_Mutex lockOne;

#ifdef __linux__
#include <unistd.h>
#endif

DTK_SEM_T g_sem; 

DTK_VOIDPTR CALLBACK ThreadOneCallBack(DTK_VOIDPTR pParam)
{
    lockOne.Lock();
    std::cout << "threadid = " << DTK_Thread_GetSelfId() << " get lock"<< __FUNCTION__ << std::endl;
#if (defined _WIN32 || defined _WIN64)
    Sleep(1000);
#else
    sleep(1);
#endif
    lockOne.Unlock();

    if (DTK_SemWait(&g_sem) < 0)
    {
        std::cout << "err = " << DTK_GetLastError() << std::endl;
    }
    std::cout << "threadid = " << DTK_Thread_GetSelfId() << " get sem"<< std::endl;
    return NULL;
}

DTK_VOIDPTR CALLBACK ThreadTwoCallBack(DTK_VOIDPTR pParam)
{
    lockOne.Lock();
    std::cout << "threadid = " << DTK_Thread_GetSelfId() << " get lock " << __FUNCTION__ << std::endl;
#if (defined _WIN32 || defined _WIN64)
    Sleep(1000);
#else
    sleep(1);
#endif
    lockOne.Unlock();

    if (DTK_SemPost(&g_sem) < 0)
    {
        std::cout << "err = " << DTK_GetLastError() << std::endl;
    }
    std::cout << "threadid = " << DTK_Thread_GetSelfId() << " post sem"<< std::endl;
    return NULL;
}


int main(void)
{
    if (DTK_SemCreate(&g_sem, 0) < 0)
    {
        std::cout << "err = " << DTK_GetLastError() << std::endl;
        return -1;
    }

    DTK_HANDLE hThread = DTK_Thread_Create(ThreadOneCallBack, NULL, 0);
    if (hThread == DTK_INVALID_THREAD)
    {
        std::cout << "err = " << DTK_GetLastError() << std::endl;
        return -1;
    }
    DTK_HANDLE hThreadTwo = DTK_Thread_Create(ThreadTwoCallBack, NULL, 0);
    if (hThread == DTK_INVALID_THREAD)
    {
        std::cout << "err = " << DTK_GetLastError() << std::endl;
        return -1;
    }
    DTK_Thread_Wait(hThread);
    DTK_Thread_Wait(hThreadTwo);

    if (DTK_SemDestroy(&g_sem) < 0)
    {
        std::cout << "err = " << DTK_GetLastError() << std::endl;
        return -1;
    }
    return 0;
}
