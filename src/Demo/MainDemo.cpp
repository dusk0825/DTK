
#include "MainDemo.h"

DTK_VOIDPTR CALLBACK ThreadPoolCallBack(DTK_VOIDPTR pParam)
{
    int* pVal = static_cast <int *> (pParam);

    DTK_Sleep(1000);
    LOG_INFO("thread id = %d, param = %d", DTK_Thread_GetSelfId(), *pVal);
    return NULL;
}

int main(void)
{
    DTK_Init();

//     DTK_HANDLE hThreadPool = DTK_ThreadPoolFlex_Create(20, 1000, ThreadPoolCallBack);
//     if (NULL == hThreadPool)
//     {
//         LOG_ERROR("err = %d", DTK_GetLastError());
//         return 0;
//     }
// 
//     for (int i = 0; i < 100; i++)
//     {
//         if (DTK_ThreadPoolFlex_Work(hThreadPool, (DTK_VOIDPTR)&i) < 0)
//         {
//             LOG_ERROR("DTK_ThreadPoolFlex_Work fail i = %d", i);
//         }
//     }
//     
//     getchar();
//     DTK_ThreadPoolFlex_Destroy(hThreadPool);

    //Test_Singleton();
    //Test_AsyncIo();
    

    DTK_Fini();
    return 0;
}
