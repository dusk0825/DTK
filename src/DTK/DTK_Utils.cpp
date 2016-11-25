
#include "DTK_Utils.h"

DTK_DECLARE DTK_VOID CALLBACK DTK_Sleep(DTK_INT32 millisecond)
{
#if defined(OS_WINDOWS)
    Sleep(millisecond);
#else
    usleep(millisecond*1000);
#endif
}

DTK_DECLARE DTK_VOID CALLBACK DTK_USleep(DTK_INT64 nUSec)
{
#if defined(OS_WINDOWS)
    Sleep((DWORD)nUSec/1000);
#else
    usleep(nUSec);
#endif
}

DTK_DECLARE DTK_UINT32 CALLBACK DTK_Rand()
{
#if defined (OS_WINDOWS)
    srand(GetTickCount());
    return rand();
#else
    //srandom(DTK_GetTimeTick());
    //return random();
    return 0;
#endif
}

DTK_DECLARE DTK_VOID CALLBACK DTK_ZeroMemory(void* pMem, int iLen)
{
    memset(pMem, 0, iLen);
    return;
}
