
#include "DTK_Time.h"

DTK_DECLARE DTK_UINT32 CALLBACK DTK_GetTimeTick()
{
#ifdef OS_WINDOWS
    return GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + (tv.tv_usec / 1000));
#endif
}

