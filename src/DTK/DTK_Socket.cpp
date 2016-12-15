
#include "DTK_Socket.h"
#include "DTK_Select.h"

#if (defined OS_POSIX)
    #include <netinet/tcp.h>
    #include <signal.h>
    #include <fcntl.h>
    #define closesocket close
    #ifndef SOCKADDR
    typedef sockaddr SOCKADDR;
    typedef sockaddr* PSOCKADDR;
    #endif
#endif

/** @fn DTK_INT32 DTK_InitNetwork_Inter()
*   @brief 初始化网络库
*   @return 0 成功, -1 失败
*/
DTK_INT32 DTK_InitNetwork_Inter();

/** @fn DTK_INT32 DTK_FiniNetwork_Inter()
*   @brief 反初始化网络库
*   @return 0 成功, -1 失败
*/
DTK_INT32 DTK_FiniNetwork_Inter();

DTK_INT32 DTK_InitNetwork_Inter()
{
#if defined(OS_WINDOWS)
	if ( !g_bWSAStartup )
	{
		WSADATA wsaData;
		g_bWSAStartup = DTK_TRUE;
		return ::WSAStartup(MAKEWORD(2,2), &wsaData);
	}
#else
    //struct sigaction sa;
    //sa.sa_handler = SIG_IGN;
    //sigaction( SIGPIPE, &sa, 0 );
#endif

	return DTK_OK;
}

DTK_INT32 DTK_FiniNetwork_Inter()
{
#if defined(OS_WINDOWS)
	if ( g_bWSAStartup )
	{
		::WSACleanup();
		g_bWSAStartup = DTK_FALSE;
	}
#endif

	return DTK_OK;
}

DTK_DECLARE DTK_SOCK_T CALLBACK DTK_CreateSocket( DTK_INT32 iAf, DTK_INT32 iType, DTK_INT32 iProto )
{
    return ::socket(iAf, iType, iProto);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_CloseSocket( DTK_SOCK_T iSockFd, DTK_BOOL bForce /*= DTK_FALSE*/ )
{
    if (bForce)
    {
        DTK_ShutDown(iSockFd, DTK_SHUT_RDWR);
    }

    return ::closesocket(iSockFd);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_ShutDown( DTK_SOCK_T iSockFd, DTK_INT32 iHow )
{
    return ::shutdown(iSockFd, iHow);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Bind( DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr )
{
    if ( !pDTKAddr )
    {
        return DTK_ERROR;
    }

    if (pDTKAddr->SA.sin4.sin_family == AF_INET)
    {
        return ::bind(iSockFd,(PSOCKADDR)&pDTKAddr->SA,sizeof(sockaddr_in));
    }
    else
    {
        return ::bind(iSockFd,(PSOCKADDR)&pDTKAddr->SA,sizeof(DTK_ADDR_T));
    }
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Listen( DTK_SOCK_T iSockFd, DTK_INT32 iBackLog )
{
    return ::listen(iSockFd, iBackLog);
}

DTK_DECLARE DTK_SOCK_T CALLBACK DTK_Accept( DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr /*= NULL*/, DTK_UINT32 nTimeOut /*= DTK_INFINITE*/ )
{
    int iAddrSize = 0;
    DTK_SOCK_T iTmpSockFd = DTK_INVALID_SOCKET;
    
    if (nTimeOut == DTK_INFINITE)
    {
        if (pDTKAddr != NULL)
        {
            iAddrSize = sizeof(pDTKAddr->SA);
            iTmpSockFd = ::accept(iSockFd, (sockaddr*)&pDTKAddr->SA, (socklen_t*)&iAddrSize);
        }
        else
        {
            iTmpSockFd = ::accept(iSockFd, NULL, NULL);
        }
    }
    else
    {
#if defined (OS_WINDOWS)
        fd_set fdset_r;
        timeval tv = {nTimeOut/1000, (nTimeOut%1000)*1000};
        int iSelectRet = -1;

        FD_ZERO(&fdset_r);
        FD_SET(iSockFd, &fdset_r);

        iSelectRet = DTK_SelectEx((int)iSockFd+1, &fdset_r, NULL, NULL, &tv);
        if (iSelectRet > 0 && FD_ISSET(iSockFd, &fdset_r))
#elif defined (OS_POSIX)
        struct pollfd fds[1] = {0};
        int iPollRet = -1;
        fds[0].fd = iSockFd;
        fds[0].events = POLLRDNORM;
        iPollRet = DTK_PollEx(fds,1,(DTK_INT32*)&nTimeOut);
        if ((iPollRet > 0) && (fds[0].revents & POLLRDNORM))
#else
#error OS Not Implement.
#endif
        {
            if (pDTKAddr != NULL)
            {
                iAddrSize = sizeof(pDTKAddr->SA);
                iTmpSockFd = ::accept(iSockFd, (sockaddr*)&pDTKAddr->SA, (socklen_t*)&iAddrSize);
            }
            else
            {
                iTmpSockFd = ::accept(iSockFd, NULL, NULL);
            }
        }
    }

    return iTmpSockFd;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_ConnectWithTimeOut( DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr, DTK_UINT32 nTimeOut/*=DTK_INFINITE*/ )
{
    if (!pDTKAddr)
    {
        return DTK_ERROR;
    }

    if (nTimeOut == DTK_INFINITE)
    {
        return ::connect(iSockFd,(PSOCKADDR)&pDTKAddr->SA,sizeof(sockaddr_in));
    }

    timeval tv;
    DTK_INT32 iRet = DTK_ERROR;
    tv.tv_sec = nTimeOut/1000;
    tv.tv_usec = (nTimeOut%1000)*1000;

    fd_set fdset_w;
    FD_ZERO(&fdset_w);
    FD_SET(iSockFd,&fdset_w);

    ::DTK_SetNonBlock(iSockFd, DTK_TRUE);
    if (pDTKAddr->SA.sin4.sin_family == AF_INET)
    {
        iRet = ::connect(iSockFd,(PSOCKADDR)&pDTKAddr->SA.sin4,sizeof(pDTKAddr->SA.sin4));
    }
    else
    {  
        iRet = ::connect(iSockFd,(PSOCKADDR)&pDTKAddr->SA.sin6,sizeof(pDTKAddr->SA.sin6));
    }

    if (iRet == 0)//if success, return succ immediately.
    {
        return iRet;
    }

    //判断是否是正常连接中
#if defined (OS_WINDOWS)
    if ( WSAGetLastError() != WSAEWOULDBLOCK)
    {
        return iRet;
    }
#else
    if (errno != EINPROGRESS)
    {
        return iRet;
    }
#endif

    int iSystemLastError = 0;
    DTK_INT32 iSelectRet = ::DTK_Select((int)iSockFd+1,NULL,&fdset_w,NULL,&tv);
    if (iSelectRet>0 && FD_ISSET(iSockFd,&fdset_w))
    {
        DTK_INT32 iError;
        DTK_INT32 iErrorLen;
        iErrorLen = sizeof(iError);
        iRet = ::getsockopt(iSockFd,SOL_SOCKET,SO_ERROR,(char*)&iError,(socklen_t*)&iErrorLen);
        if (!iRet && !iError )
        {
            iRet = 0;
        }
        else
        {
#if defined (OS_WINDOWS)
            //DTK_OutputDebug("schina !!! DTK_ConnectWithTimeOut getsockopt !iError return error, %d, Error\n", WSAGetLastError());
            ///[ADD][zhaoyiji][2012-07-27]  bug fixed if error, last errno is overwritten.
            iSystemLastError = WSAGetLastError();
#elif defined (OS_POSIX)
            //DTK_OutputDebug("schina !!! DTK_ConnectWithTimeOut getsockopt !iError return error, %d, Error\n", errno);
            ///[ADD][zhaoyiji][2012-07-27]  bug fixed if error, last errno is overwritten.
            iSystemLastError = errno;
#else
#endif
        }
    }
#if defined (OS_WINDOWS)
    else if (iSelectRet < 0)
    {
        //DTK_OutputDebug("schina !!! DTK_ConnectWithTimeOut select/poll return error, %d, Error\n", WSAGetLastError());
        ///[ADD][zhaoyiji][2012-07-27]  bug fixed if error, last errno is overwritten.
        iSystemLastError = WSAGetLastError();
    }
#elif defined (OS_POSIX)
    else if (iSelectRet < 0)
    {
        //DTK_OutputDebug("schina !!! DTK_ConnectWithTimeOut select/poll return error, %d, Error\n", errno);
        ///[ADD][zhaoyiji][2012-07-27]  bug fixed if error, last errno is overwritten.
        iSystemLastError = errno;
    }
#endif

    //这里存在bug，该接口修改了套接字的原始属性，
    //如果是非阻塞的套接字，改变了套接字的阻塞属性。
    //bNonBlock = DTK_FALSE;
    ::DTK_SetNonBlock(iSockFd, DTK_FALSE);
    ///bug fixed if error, last errno is overwritten.
    if (iRet != 0)
    {
#if defined (OS_WINDOWS)
        WSASetLastError(iSystemLastError);
#elif defined (OS_POSIX)
        errno = iSystemLastError;
#endif
    }

    return iRet;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetNonBlock(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo)
{
#if defined (OS_WINDOWS)
    return ::ioctlsocket(iSockFd, FIONBIO, (u_long*)&bYesNo);
#else
    int flags = fcntl(iSockFd, F_GETFL, 0);
    if (bYesNo)
    {
        return (fcntl(iSockFd, F_SETFL, flags | O_NONBLOCK)==-1)?DTK_ERROR:DTK_OK;
    }
    else
    {
        return (fcntl(iSockFd, F_SETFL, flags &(~O_NONBLOCK))==-1)?DTK_ERROR:DTK_OK;
    }
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetBuffSize(DTK_SOCK_T iSockFd, DTK_INT32 iSndBuffSize, DTK_INT32 iRcvBuffSize)
{
    DTK_INT32 iRet1 = DTK_OK;
    DTK_INT32 iRet2 = DTK_OK;

    if ( iRcvBuffSize )
    {
        iRet1 = ::setsockopt(iSockFd,SOL_SOCKET,SO_RCVBUF,(char*)&iRcvBuffSize, sizeof(iRcvBuffSize));
    }
    if ( iSndBuffSize )
    {
        iRet2 = ::setsockopt(iSockFd,SOL_SOCKET,SO_SNDBUF,(char*)&iSndBuffSize, sizeof(iSndBuffSize));
    }

    return (iRet1==DTK_OK && iRet2==DTK_OK)?DTK_OK:DTK_ERROR;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetBuffSize(DTK_SOCK_T iSockFd, DTK_INT32* iSndBuffSize, DTK_INT32* iRcvBuffSize)
{
    DTK_INT32 iRet1 = DTK_OK;
    DTK_INT32 iRet2 = DTK_OK;
    DTK_INT32 iSize1 = sizeof(DTK_INT32);
    DTK_INT32 iSize2 = sizeof(DTK_INT32);

    if ( iRcvBuffSize )
    {
        iRet1 = ::getsockopt(iSockFd,SOL_SOCKET,SO_RCVBUF,(char*)iRcvBuffSize, (socklen_t*)&iSize1);
    }
    if ( iSndBuffSize )
    {
        iRet2 = ::getsockopt(iSockFd,SOL_SOCKET,SO_SNDBUF,(char*)iSndBuffSize, (socklen_t*)&iSize2);
    }

    return (iRet1==DTK_OK && iRet2==DTK_OK)?DTK_OK:DTK_ERROR;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetReuseAddr(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo)
{
	return ::setsockopt(iSockFd,SOL_SOCKET,SO_REUSEADDR,(char*)&bYesNo, sizeof(bYesNo));
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTimeOut(DTK_SOCK_T iSockFd, DTK_INT32 iSndTimeO, DTK_INT32 iRcvTimeO)
{
	DTK_INT32 iRet1 = DTK_OK;
	DTK_INT32 iRet2 = DTK_OK;

#if defined (OS_WINDOWS)
	if ( iRcvTimeO )
	{
		iRet1 = ::setsockopt(iSockFd,SOL_SOCKET,SO_RCVTIMEO,(char*)&iRcvTimeO, sizeof(iRcvTimeO));
	}
	if ( iSndTimeO )
	{
		iRet2 = ::setsockopt(iSockFd,SOL_SOCKET,SO_SNDTIMEO,(char*)&iSndTimeO, sizeof(iSndTimeO));
	}
#elif defined(OS_POSIX)
	if ( iRcvTimeO )
	{
		timeval tv = {iRcvTimeO/1000,iRcvTimeO%1000};
		iRet1 = setsockopt(iSockFd,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv, sizeof(tv));
	}
	if ( iSndTimeO )
	{
		timeval tv = {iSndTimeO/1000,iSndTimeO%1000};
		iRet2 = setsockopt(iSockFd,SOL_SOCKET,SO_SNDTIMEO,(char*)&tv, sizeof(tv));
	}
#endif

	return (iRet1==DTK_OK && iRet2==DTK_OK)?DTK_OK:DTK_ERROR;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_BroadCast( DTK_SOCK_T iSockFd )
{
    DTK_BOOL bYes = DTK_TRUE;
    return ::setsockopt(iSockFd, SOL_SOCKET, SO_BROADCAST, (char*)&bYes, sizeof(bYes));
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTTL(DTK_SOCK_T iSockFd, DTK_INT32 iTTL)
{
	return ::setsockopt(iSockFd, IPPROTO_IP, IP_TTL, (char*)&iTTL, sizeof(iTTL));
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTTL_V6(DTK_SOCK_T iSockFd, DTK_INT32 iTTL)
{
#if defined (OS_SUPPORT_IPV6)
	return ::setsockopt(iSockFd, IPPROTO_IPV6, IPV6_UNICAST_HOPS, (char*)&iTTL, sizeof(iTTL));
#else
	return DTK_NOSUPPORT;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetMultiCastTTL(DTK_SOCK_T iSockFd, DTK_INT32 iTTL)
{
	return ::setsockopt(iSockFd, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&iTTL, sizeof(iTTL));
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetMultiCastTTL_V6(DTK_SOCK_T iSockFd, DTK_INT32 iTTL)
{
#if defined (OS_SUPPORT_IPV6)
	return ::setsockopt(iSockFd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*)&iTTL, sizeof(iTTL));
#else
	return DTK_NOSUPPORT;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTTL(DTK_SOCK_T iSockFd, DTK_INT32* iTTL)
{
	int iSize = sizeof(iTTL);
	return ::getsockopt(iSockFd, IPPROTO_IP, IP_TTL, (char*)iTTL, (socklen_t*)&iSize);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTTL_V6(DTK_SOCK_T iSockFd, DTK_INT32* iTTL)
{
#if defined (OS_SUPPORT_IPV6)
	int iSize = sizeof(iTTL);
	return ::getsockopt(iSockFd, IPPROTO_IPV6, IPV6_UNICAST_HOPS, (char*)iTTL, (socklen_t*)&iSize);
#else
	return DTK_NOSUPPORT;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTOS(DTK_SOCK_T iSockFd, DTK_INT32 iTOS)
{
	return ::setsockopt(iSockFd, IPPROTO_IP, IP_TOS, (char*)&iTOS, sizeof(iTOS));
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTOS(DTK_SOCK_T iSockFd, DTK_INT32* iTOS)
{
	int iSize = sizeof(*iTOS);
	return ::getsockopt(iSockFd, IPPROTO_IP, IP_TOS, (char*)iTOS, (socklen_t*)&iSize);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_LingerOff(DTK_SOCK_T iSockFd)
{
	linger lin = {0};
	lin.l_onoff = 0;

	return ::setsockopt(iSockFd, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(linger));
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_LingerOn(DTK_SOCK_T iSockFd,DTK_UINT16 nTimeOutSec)
{
	linger lin = {0};
	lin.l_onoff = 1;
	lin.l_linger = nTimeOutSec;

	return ::setsockopt(iSockFd, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(linger));
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetNoDelay(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo)
{
	return ::setsockopt(iSockFd, IPPROTO_TCP, TCP_NODELAY, (char*)&bYesNo, sizeof(DTK_BOOL));
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_JoinMultiCastGroup(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pMcastGroupAddr)
{
	if (!pLocal || !pMcastGroupAddr)
	{
		return DTK_ERROR;
	}

	PSOCKADDR pSockAddr = (PSOCKADDR)&(pMcastGroupAddr->SA);	

	if (pSockAddr->sa_family == AF_INET)
	{
		struct ip_mreq struMerq = {0};
		struMerq.imr_multiaddr.s_addr = pMcastGroupAddr->SA.sin4.sin_addr.s_addr;
		struMerq.imr_interface.s_addr = pLocal->SA.sin4.sin_addr.s_addr;
		return ::setsockopt(iSockFd, IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&struMerq,sizeof(struMerq));
	}
#if defined (OS_SUPPORT_IPV6)
	else if (pSockAddr->sa_family == AF_INET6)
	{
		struct ipv6_mreq struMerq6;
		struMerq6.ipv6mr_interface = pMcastGroupAddr->SA.sin4.sin_addr.s_addr;
		memcpy(struMerq6.ipv6mr_multiaddr.s6_addr, pMcastGroupAddr->SA.sin6.sin6_addr.s6_addr, 16);
        return ::setsockopt(iSockFd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char*)&struMerq6,sizeof(struMerq6));
	}
#endif
	else
	{
		return DTK_ERROR;
	}
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_LeaveMultiCastGroup(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pMcastGroupAddr)
{
	if (!pLocal || !pMcastGroupAddr)
	{
		return DTK_ERROR;
	}

	PSOCKADDR pSockAddr = (PSOCKADDR)&(pMcastGroupAddr->SA);
	if (pSockAddr->sa_family == AF_INET)
	{
		struct ip_mreq struMerq;
		struMerq.imr_multiaddr.s_addr = pMcastGroupAddr->SA.sin4.sin_addr.s_addr;
		struMerq.imr_interface.s_addr = pLocal->SA.sin4.sin_addr.s_addr;
		//InetPton(iAf,pAddr,&struMerq.imr_multiaddr);
		//InetPton(iAfLocal,pLocalAddr,&struMerq.imr_interface);
		return ::setsockopt(iSockFd, IPPROTO_IP,IP_DROP_MEMBERSHIP,(char*)&struMerq,sizeof(struMerq));
	}
#if defined (OS_SUPPORT_IPV6)
	else if (pSockAddr->sa_family == AF_INET6)
	{
		struct ipv6_mreq struMerq6;
		struMerq6.ipv6mr_interface = pMcastGroupAddr->SA.sin4.sin_addr.s_addr;
		memcpy(struMerq6.ipv6mr_multiaddr.s6_addr, pMcastGroupAddr->SA.sin6.sin6_addr.s6_addr, 16);
        return setsockopt(iSockFd, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, (char*)&struMerq6, sizeof(struMerq6));
	}
#endif
	else
	{
		return DTK_ERROR;
	}
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Send( DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen )
{
    return ::send(iSockFd,(char*)pBuf, iBufLen, 0);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Sendn(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen, DTK_UINT32 nTimeOut)
{
    int iSendLen = 0;
    int iTmpLen = 0;
    int iRet = 0;

#if defined (OS_WINDOWS)
    timeval tv;
    fd_set fdset_w;

    if (nTimeOut != DTK_INFINITE)
    {
        tv.tv_sec = nTimeOut/1000;
        tv.tv_usec = (nTimeOut%1000)/1000;
    }
    else
    {
        tv.tv_sec = 0;
        tv.tv_usec= 0;
    }
#endif

    do
    {
#if defined (OS_WINDOWS)
        FD_ZERO(&fdset_w);
        FD_SET(iSockFd,&fdset_w);

        //这里没有使用DTK_Select是为了利用posix的select自动会对tv进行时间减操作
        if (nTimeOut == DTK_INFINITE)
        {
            iRet = ::DTK_SelectEx((int)iSockFd+1,NULL,&fdset_w,NULL,NULL);
        }
        else
        {
            iRet = ::DTK_SelectEx((int)iSockFd+1,NULL,&fdset_w,NULL,&tv);
        }
        nTimeOut = tv.tv_sec*1000 + tv.tv_usec;

        if (iRet>0 && FD_ISSET(iSockFd,&fdset_w))
#elif defined (OS_POSIX)
        struct pollfd fds[1] = {0};
        fds[0].fd = iSockFd;
        fds[0].events = POLLWRNORM;
        iRet = DTK_PollEx(fds,1,(DTK_INT32*)&nTimeOut);
        if ((iRet > 0) && (fds[0].revents & POLLWRNORM))
#else
#error OS Not Implement.
#endif
        {
            iTmpLen = ::DTK_Send(iSockFd, (char*)pBuf+iSendLen, iBufLen-iSendLen);
            if (iTmpLen > 0)
            {
                iSendLen += iTmpLen;
                if (iSendLen == iBufLen)
                {
                    break;
                }
            }
            else
            {
                return -1;
            }
        }
        else if (iRet < 0)
        {
            return -1;
        }
    }while(nTimeOut > 0);

    return iSendLen;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Recv( DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount )
{
    return ::recv(iSockFd,(char*)pBuf, iBufCount, 0);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Recvn(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_UINT32 nTimeOut)
{
    int iRecvLen = 0;
    int iTmpLen = 0;
    int iRet = -1;
#if defined (OS_WINDOWS)
    timeval tv;
    fd_set fdset_r;

    if (nTimeOut != DTK_INFINITE)
    {
        //tv进行初始化
        tv.tv_sec = nTimeOut/1000;
        tv.tv_usec = (nTimeOut%1000)*1000;
    }
#endif

    do
    {
#if defined (OS_WINDOWS)
        FD_ZERO(&fdset_r);
        FD_SET(iSockFd,&fdset_r);

        if (nTimeOut == DTK_INFINITE)
        {
            iRet = ::DTK_SelectEx((int)iSockFd+1,&fdset_r,NULL,NULL,NULL);
        }
        else
        {
            iRet = ::DTK_SelectEx((int)iSockFd+1,&fdset_r,NULL,NULL,&tv);
            nTimeOut = tv.tv_sec*1000 + tv.tv_usec;
        }

        if (iRet>0 && FD_ISSET(iSockFd,&fdset_r))
#elif defined (OS_POSIX)
        struct pollfd fds[1] = {0};
        fds[0].fd = iSockFd;
        fds[0].events = POLLRDNORM;
        iRet = DTK_PollEx(fds,1,(DTK_INT32*)&nTimeOut);
        if ((iRet > 0) && (fds[0].revents & POLLRDNORM))
#else
#error OS Not Implement.
#endif
        {
            iTmpLen = ::DTK_Recv(iSockFd, (char*)pBuf+iRecvLen, iBufCount-iRecvLen);
            if (iTmpLen > 0)
            {
                iRecvLen += iTmpLen;
                if (iRecvLen == iBufCount)
                {
                    break;
                }
            }
            else
            {
                return -1;
            }
        }
        else if (iRet < 0)
        {
            return -1;
        }
    }while(nTimeOut > 0);

    return iRecvLen;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_RecvWithTimeOut(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_UINT32 nTimeOut)
{
    int iRecvLen = 0;
    int iRet = 0;

#if defined (OS_WINDOWS)
    timeval tv;
    if (nTimeOut != DTK_INFINITE)
    {
        tv.tv_sec = nTimeOut/1000;
        tv.tv_usec = (nTimeOut%1000)*1000;
    }

    fd_set fdset_r;
    FD_ZERO(&fdset_r);
    FD_SET(iSockFd,&fdset_r);

    if (nTimeOut == DTK_INFINITE)
    {
        iRet = ::DTK_SelectEx((int)iSockFd+1,&fdset_r,NULL,NULL,NULL);
    }
    else
    {
        iRet = ::DTK_SelectEx((int)iSockFd+1,&fdset_r,NULL,NULL,&tv);
    }

    if (iRet>0 && FD_ISSET(iSockFd,&fdset_r))
#elif defined (OS_POSIX)
    struct pollfd fds[1] = {0};
    int iPollRet = -1;
    fds[0].fd = iSockFd;
    fds[0].events = POLLRDNORM;
    iPollRet = DTK_PollEx(fds,1,(DTK_INT32*)&nTimeOut);
    if ((iRet > 0) && (fds[0].revents & POLLRDNORM))
#else
#error OS Not Implement.
#endif
    {
        iRecvLen = ::DTK_Recv(iSockFd, (char*)pBuf, iBufCount);
        if (iRecvLen > 0)
        {
            return iRecvLen;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SendTo(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen, DTK_ADDR_T* pDTKAddr)
{
	if (pDTKAddr->SA.sin4.sin_family == AF_INET)
	{
		return ::sendto(iSockFd, (char*)pBuf, iBufLen, 0, (PSOCKADDR)&pDTKAddr->SA, sizeof(pDTKAddr->SA.sin4));
	}
	else
	{
		return ::sendto(iSockFd, (char*)pBuf, iBufLen, 0, (PSOCKADDR)&pDTKAddr->SA, sizeof(pDTKAddr->SA.sin6));
	}
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_RecvFrom(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_ADDR_T* pDTKAddr)
{
	int iAddrSize = sizeof(DTK_ADDR_T);
	return ::recvfrom(iSockFd, (char*)pBuf, iBufCount, 0, (PSOCKADDR)&pDTKAddr->SA, (socklen_t*)&iAddrSize);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Ioctl(DTK_SOCK_T iSockFd, DTK_INT32 iCmd, DTK_UINT32 *nData)
{
#if defined OS_WINDOWS
	return ::ioctlsocket(iSockFd, iCmd, (unsigned long *)nData);
#elif defined OS_POSIX
	return ::ioctl(iSockFd, iCmd, (unsigned long *)nData);
#endif
}
