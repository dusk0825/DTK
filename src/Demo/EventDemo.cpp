
#include "MainDemo.h"
#if (defined __linux__)
#include <pthread.h>
#endif

DTK_EVENT_T hEvent;
DTK_MUTEX_T  mtxLock;
bool bExit = false;

DTK_VOIDPTR CALLBACK EventThreadOneCallBack(DTK_VOIDPTR pParam)
{
    while (!bExit)
    {
         if (DTK_WaitForSingleObject(&hEvent, &mtxLock) != DTK_WAIT_OBJECT_0)
         {
             std::cout << "err = "<< DTK_GetLastError() << std::endl;
             return NULL;
         }

        std::cout << "threadid = " << DTK_Thread_GetSelfId() << " get event"<< __FUNCTION__ << std::endl;
        DTK_Sleep(1000);
    }
    return NULL;
}

DTK_VOIDPTR CALLBACK EventThreadTwoCallBack(DTK_VOIDPTR pParam)
{
    while (!bExit)
    {
        DTK_Sleep(1500);
        DTK_MutexLock(&mtxLock);
        std::cout << "threadid = " << DTK_Thread_GetSelfId() << " before set event"<< __FUNCTION__ << std::endl;
        if (!DTK_SetEvent(&hEvent))
        {
            std::cout << "err = "<< DTK_GetLastError() << std::endl;
            return NULL;
        }
        DTK_MutexUnlock(&mtxLock);
    }
    return NULL;
}

void Test_Event()
{
     if (!DTK_CreateEvent(&hEvent))
     {
         std::cout << "err = "<< DTK_GetLastError() << std::endl;
         return;
     }
     DTK_MutexCreate(&mtxLock);
 
     DTK_HANDLE hThread = DTK_Thread_Create(EventThreadOneCallBack, NULL, 0);
     if (hThread == DTK_INVALID_THREAD)
     {
         std::cout << "err = " << DTK_GetLastError() << std::endl;
         return;
     }

     DTK_HANDLE hThreadTwo = DTK_Thread_Create(EventThreadTwoCallBack, NULL, 0);
     if (hThread == DTK_INVALID_THREAD)
     {
         std::cout << "err = " << DTK_GetLastError() << std::endl;
         return;
     }
 
     DTK_Sleep(10000);
     bExit = true;
     DTK_Thread_Wait(hThread);
     DTK_Thread_Wait(hThreadTwo);
 
     DTK_MutexDestroy(&mtxLock);
     DTK_CloseEvent(&hEvent);
}

