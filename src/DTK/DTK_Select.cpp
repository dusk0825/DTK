
#include "DTK_Select.h"
#include "DTK_Time.h"
#include "DTK_Utils.h"

DTK_DECLARE DTK_INT32 CALLBACK DTK_FD_ZERO( fd_set *set )
{
    return FD_ZERO(set);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_FD_SET( DTK_INT32 iFd, fd_set *set )
{
    FD_SET(iFd, set);
    return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_FD_CLR( DTK_INT32 iFd, fd_set *set )
{
    FD_CLR(iFd, set);
    return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_FD_ISSET(DTK_INT32 iFd, fd_set *set)
{
	return FD_ISSET(iFd, set);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Select(DTK_INT32 iMaxFd, fd_set* readFds, fd_set* writeFds, fd_set* errFds, timeval* tvTime)
{
#if defined (OS_WINDOWS)
	return ::select(iMaxFd, readFds, writeFds, errFds, tvTime);
#elif defined (OS_POSIX)
	if (NULL == tvTime)
	{
		return ::select(iMaxFd, readFds, writeFds, errFds, NULL);
	}

    //linux的select使用上有区别，linux会改变timeval
    timeval tmTime = *tvTime;
	return ::select(iMaxFd, readFds, writeFds, errFds, &tmTime);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SelectEx(DTK_INT32 iMaxFd, fd_set* readFds, fd_set* writeFds, fd_set* errFds, timeval* tvTime)
{
#if defined (OS_WINDOWS)

	if (NULL == tvTime)
	{
		return select(iMaxFd, readFds, writeFds, errFds, NULL);
	}

	DTK_INT32 iStartTime = (DTK_INT32)DTK_GetTimeTick();
	DTK_INT32 iRet = ::select(iMaxFd, readFds, writeFds, errFds, tvTime);
	DTK_INT32 iTimeDiff = (DTK_INT32)DTK_GetTimeTick() - iStartTime;

	if (iTimeDiff > 0)
	{
		if (tvTime->tv_sec > (iTimeDiff/1000))
		{
			tvTime->tv_sec -= (iTimeDiff/1000);
		}
		else
		{
			tvTime->tv_sec = 0;
		}

		if (tvTime->tv_usec > ((iTimeDiff%1000)*1000))
		{
			tvTime->tv_usec -= ((iTimeDiff%1000)*1000);
		}
		else
		{
			tvTime->tv_usec = 0;
		}
	}
// 	else if (iTimeDiff == 0)
// 	{
//         //do no thing
// 	}
	else if (iTimeDiff < 0)
	{
		tvTime->tv_sec = 0;
		tvTime->tv_usec = 0;
	}

	return iRet;

#elif defined (OS_POSIX)
    timeval tmTime = *tvTime;
	return ::select(iMaxFd, readFds, writeFds, errFds, &tmTime);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Poll(struct pollfd* fds, DTK_INT32 iFds, DTK_INT32 iTimeO)
{
#if defined (OS_POSIX)
    return poll(fds, iFds, iTimeO);
#endif

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_PollEx(struct pollfd* fds,DTK_INT32 iFds, DTK_INT32* iTimeO)
{
#if defined (OS_POSIX)
	DTK_INT32 iRet = DTK_ERROR;
	DTK_INT32 iStartTime;
	DTK_INT32 iTimeDiff;

	if ((iTimeO == NULL) || (*iTimeO == -1))
	{
RETRY:
        iRet = poll(fds, iFds, -1);
    	if(iRet == -1 && errno == EINTR)
    	{
    	    goto RETRY;
    	}

    	return iRet;
	}

REPOLL:
	iStartTime = (DTK_INT32)DTK_GetTimeTick();
	iRet = poll(fds, iFds, *iTimeO);
	iTimeDiff = (DTK_INT32)DTK_GetTimeTick() - iStartTime;

	if(iRet == -1 && errno == EINTR)
	{
	    *iTimeO -= iTimeDiff;
		if (*iTimeO > 0)
		{
			goto REPOLL;
		}
	}

	if (*iTimeO > iTimeDiff)
	{
		*iTimeO -= iTimeDiff;
	}
	else
	{
		*iTimeO = 0;
	}

	return iRet;
#endif

	return DTK_OK;
}
