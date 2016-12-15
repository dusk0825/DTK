
#include "DTK_Error.h"

DTK_DECLARE DTK_UINT32 CALLBACK DTK_GetLastError()
{
#ifdef OS_POSIX
    return (DTK_UINT32)errno;
#elif defined(OS_WINDOWS)
    DTK_UINT32 errCode = GetLastError();
    return errCode;
#endif
}

DTK_DECLARE DTK_VOID CALLBACK DTK_SetLastError(DTK_UINT32 uError)
{
#ifdef OS_POSIX
    errno = (int)uError;
#elif defined(OS_WINDOWS)
    SetLastError(uError);
#endif
}

DTK_DECLARE DTK_UINT32 CALLBACK DTK_GetSocketLastError()
{
#ifdef OS_POSIX
    return (DTK_UINT32)errno;
#elif defined(OS_WINDOWS)
    DTK_UINT32 errCode = WSAGetLastError();
    return errCode;
#endif
}
