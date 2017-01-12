
#include "MainDemo.h"
#include <map>

typedef struct tagCltInfo
{
    char szBuffer[1024];
    DTK_SOCK_T iCltSock;
    DTK_ADDR_T stCltAddr;
}CLIENT_INFO_ST;

DTK_Mutex g_cltInfoLock;
std::map<DTK_SOCK_T, CLIENT_INFO_ST> g_cltInfoMap;

#ifdef OS_WINDOWS
DTK_VOID AsyncIoSendCB(DTK_ULONG nErrorCode, DTK_ULONG nNumberOfBytes, DTK_VOIDPTR pUsrData);

DTK_VOID AsyncIoRecvCB(DTK_ULONG nErrorCode, DTK_ULONG nNumberOfBytes, DTK_VOIDPTR pUsrData)
{
    if (NULL == pUsrData)
    {
        return;
    }

    DTK_SOCK_T* pSockFd =  (DTK_SOCK_T*) pUsrData;
    LOG_DEBUG("err = %d, rev bytes = %d", nErrorCode, nNumberOfBytes);

    CLIENT_INFO_ST stCltInfo = {0};
    g_cltInfoLock.Lock();
    if (g_cltInfoMap.find(*pSockFd) == g_cltInfoMap.end())
    {
        g_cltInfoLock.Unlock();
        return;
    }
    LOG_DEBUG("recv = %s", g_cltInfoMap[*pSockFd].szBuffer);
    stCltInfo.stCltAddr = g_cltInfoMap[*pSockFd].stCltAddr;
    stCltInfo.iCltSock = g_cltInfoMap[*pSockFd].iCltSock;
    strcpy(stCltInfo.szBuffer, "hell world!!!");
    g_cltInfoLock.Unlock();

    DTK_AsyncIO_BindCallBackToIOHandle((DTK_HANDLE)stCltInfo.iCltSock, AsyncIoSendCB);
    DTK_AsyncIO_Send((DTK_HANDLE)stCltInfo.iCltSock, stCltInfo.szBuffer, strlen(stCltInfo.szBuffer), pSockFd);
}

DTK_VOID AsyncIoSendCB(DTK_ULONG nErrorCode, DTK_ULONG nNumberOfBytes, DTK_VOIDPTR pUsrData)
{
    if (NULL == pUsrData)
    {
        return;
    }

    DTK_SOCK_T* pSockFd = (DTK_SOCK_T*) pUsrData;
    LOG_DEBUG("err = %d, sen bytes = %d", nErrorCode, nNumberOfBytes);

    CLIENT_INFO_ST stCltInfo = {0};
    g_cltInfoLock.Lock();
    if (g_cltInfoMap.find(*pSockFd) == g_cltInfoMap.end())
    {
        g_cltInfoLock.Unlock();
        return;
    }
    stCltInfo.stCltAddr = g_cltInfoMap[*pSockFd].stCltAddr;
    stCltInfo.iCltSock = g_cltInfoMap[*pSockFd].iCltSock;
    g_cltInfoLock.Unlock();

    DTK_AsyncIO_BindCallBackToIOHandle((DTK_HANDLE)stCltInfo.iCltSock, AsyncIoRecvCB);
    DTK_AsyncIO_Recv((DTK_HANDLE)stCltInfo.iCltSock, g_cltInfoMap[*pSockFd].szBuffer, sizeof(stCltInfo.szBuffer), &g_cltInfoMap[*pSockFd].iCltSock);
}
#endif

void Test_AsyncIo()
{
    DTK_SOCK_T iSockFd = DTK_CreateSocket(DTK_AF_INET, SOCK_STREAM, 0);
    if (DTK_INVALID_SOCKET == iSockFd)
    {
        LOG_ERROR("create socket fail, err = %d", DTK_GetSocketLastError());
        return;
    }
    
    DTK_ADDR_T stSvrAddr;
    DTK_MakeAddrByString(DTK_AF_INET, "10.20.134.19", 5656, &stSvrAddr);
    DTK_SetReuseAddr(iSockFd, DTK_TRUE);
    DTK_Bind(iSockFd, &stSvrAddr);
    DTK_Listen(iSockFd, 15);

#ifdef OS_WINDOWS
    DTK_HANDLE hIocp = DTK_AsyncIO_CreateQueue();
    if (NULL == hIocp)
    {
        LOG_ERROR("DTK_AsyncIO_CreateQueue fail, err = %d", DTK_GetLastError());
        return;
    }

    while (1)
    {
        CLIENT_INFO_ST stCltInfo = {0};
        stCltInfo.iCltSock = DTK_Accept(iSockFd, &stCltInfo.stCltAddr);
        if (DTK_INVALID_SOCKET == stCltInfo.iCltSock)
        {
            LOG_ERROR("accept fail, err = %d", DTK_GetSocketLastError());
            continue;
        }

        LOG_DEBUG("client ip = %s, port = %d, sockfd = %d", DTK_GetAddrString(&stCltInfo.stCltAddr), DTK_GetAddrPort(&stCltInfo.stCltAddr), stCltInfo.iCltSock);

        DTK_SetNonBlock(stCltInfo.iCltSock, DTK_TRUE);

        g_cltInfoLock.Lock();
        g_cltInfoMap.insert(std::make_pair(stCltInfo.iCltSock, stCltInfo));
        g_cltInfoLock.Unlock();

        DTK_AsyncIO_BindIOHandleToQueue((DTK_HANDLE)stCltInfo.iCltSock, hIocp);
        DTK_AsyncIO_BindCallBackToIOHandle((DTK_HANDLE)stCltInfo.iCltSock, AsyncIoRecvCB);
        DTK_AsyncIO_Recv((DTK_HANDLE)stCltInfo.iCltSock, g_cltInfoMap[stCltInfo.iCltSock].szBuffer, sizeof(stCltInfo.szBuffer), &g_cltInfoMap[stCltInfo.iCltSock].iCltSock);
    }
#endif
    
}
