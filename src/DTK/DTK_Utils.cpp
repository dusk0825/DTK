
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

DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebug(const char* format, ...)
{
    va_list al;
    va_start(al, format);

    char szDebug[5 * 1024] = {0};
    int retval = DTK_Vsnprintf(szDebug, sizeof(szDebug) - 2, format, al);
    szDebug[retval] = '\n';
    szDebug[retval + 1] = '\0';

#if defined (OS_WINDOWS)
    //OutputDebugString((LPCSTR)szDebug);

    wchar_t wDebug[5 * 1024] = {0};
    MultiByteToWideChar(CP_ACP, 0, szDebug, strlen(szDebug), wDebug, sizeof(wDebug));
    OutputDebugString(wDebug);
#elif defined (OS_POSIX)
    fprintf(stderr, "%s", szDebug);
#endif

    va_end(al);
}

DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebugString(const char* lpOutputString)
{
#if defined OS_WINDOWS
    //OutputDebugString(lpOutputString);

    int len = strlen(lpOutputString);
    //需要多分配一个wchar_t用于放置结束符
    wchar_t *tmp = (wchar_t *)calloc(len + sizeof(wchar_t), sizeof(wchar_t));
    if (tmp == NULL)
    {
        return;
    }
    MultiByteToWideChar(CP_ACP, 0, lpOutputString, len, tmp, len * sizeof(wchar_t));
    tmp[len] = 0;
    OutputDebugString(tmp);
    free(tmp);
#elif defined OS_POSIX
    fprintf(stderr, "%s", lpOutputString);
#endif
}
