
#include "MainDemo.h"
#include "Atomic.h"

bool bAtoExit = false;
DTK_ATOMIC_T iScore;

DTK_VOIDPTR CALLBACK AtomicThreadOneCallBack(DTK_VOIDPTR pParam)
{
    while (!bAtoExit)
    {
        DTK_AtomicInc(&iScore);
        //std::cout << "increment, value: " << DTK_AtomicRead(&iScore) << std::endl;
        printf("increment, value: %d\n", DTK_AtomicRead(&iScore));
        DTK_Sleep(1000);
    }
    return NULL;
}

DTK_VOIDPTR CALLBACK AtomicThreadTwoCallBack(DTK_VOIDPTR pParam)
{
    while (!bAtoExit)
    {
        DTK_AtomicDec(&iScore);
        //std::cout << "decrement, value: " << DTK_AtomicRead(&iScore) << std::endl;
        printf("decrement, value: %d\n", DTK_AtomicRead(&iScore));
        DTK_Sleep(1000);
    }
    return NULL;
}

void Test_Atomic()
{
     DTK_HANDLE hThread = DTK_Thread_Create(AtomicThreadOneCallBack, NULL, 0);
     if (hThread == DTK_INVALID_THREAD)
     {
         std::cout << "err = " << DTK_GetLastError() << std::endl;
         return;
     }

     DTK_HANDLE hThreadTwo = DTK_Thread_Create(AtomicThreadTwoCallBack, NULL, 0);
     if (hThread == DTK_INVALID_THREAD)
     {
         std::cout << "err = " << DTK_GetLastError() << std::endl;
         return;
     }
 
     DTK_Sleep(10000);
     bAtoExit = true;
     DTK_Thread_Wait(hThread);
     DTK_Thread_Wait(hThreadTwo);
}

