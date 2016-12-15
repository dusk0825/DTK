
#include "MainDemo.h"

void StartListen()
{
    DTK_Init();

    DTK_SOCK_T iListenSock = DTK_CreateSocket(AF_INET, SOCK_STREAM, 0);
    if (DTK_INVALID_SOCKET == iListenSock)
    {
        DTK_OutputDebug("err = %d", DTK_GetSocketLastError());
        return;
    }

    DTK_SetReuseAddr(iListenSock, DTK_TRUE);

    DTK_ADDR_T stSvrAddr;
    DTK_MakeAddrByString(AF_INET, "127.0.0.1", 6666, &stSvrAddr);

    if (DTK_Bind(iListenSock, &stSvrAddr) < 0)
    {
        DTK_OutputDebug("err = %d", DTK_GetSocketLastError());
        return;
    }

    if (DTK_Listen(iListenSock, 32) < 0)
    {
        DTK_OutputDebug("err = %d", DTK_GetSocketLastError());
        return;
    }

    fd_set readSet;
    int iMaxSock = iListenSock;
    std::vector<int> vctClient;
    struct timeval stTm;
    stTm.tv_sec = 60;
    stTm.tv_usec = 0;

    while(1)
    {
        DTK_FD_ZERO(&readSet);
        DTK_FD_SET(iListenSock, &readSet);
        for(std::vector<int>::iterator itor = vctClient.begin(); itor != vctClient.end(); ++itor)
        {
            DTK_FD_SET((*itor), &readSet);
        }

        int iRet = DTK_Select(iMaxSock+1, &readSet, NULL, NULL, &stTm);
        if (iRet > 0)
        {
            if (DTK_FD_ISSET(iListenSock, &readSet))
            {
                DTK_ADDR_T stCltAddr;
                int iCltSocket = DTK_Accept(iListenSock, &stCltAddr);
                if(iCltSocket <= 0)
                {
                    DTK_OutputDebug("err = %d", DTK_GetSocketLastError());
                    continue;
                }

                vctClient.push_back(iCltSocket);
                //DTK_OutputDebug("accept client fd = %d,  %s:%d\n", iCltSocket,inet_ntoa(stCltAddr.sin_addr),ntohs(stCltAddr.sin_port));
                DTK_OutputDebug("accept client fd = %d,  %s:%d", iCltSocket,DTK_GetAddrString(&stCltAddr),DTK_GetAddrPort(&stCltAddr));
                if (iCltSocket > iMaxSock)
                {
                    iMaxSock = iCltSocket;
                }
            }
            else
            {
                for(std::vector<int>::iterator itor = vctClient.begin(); itor != vctClient.end();)
                {
                    if (DTK_FD_ISSET(*itor, &readSet))
                    {
                        char buf[1024] = {0};
                        if(DTK_Recv(*itor, buf, sizeof(buf)) <= 0)
                        {
                            DTK_OutputDebug("client fd = %d close", *itor);
                            DTK_CloseSocket(*itor);
                            DTK_FD_CLR(*itor, &readSet);
                            itor = vctClient.erase(itor);
                            continue;
                        }
                        else
                        {
                            DTK_OutputDebug("recv client fd = %d, msg: %s", *itor, buf);
                            DTK_Send(*itor, buf, sizeof(buf));
                        }
                    }
                    ++itor;
                }
            }
        }
        else if (iRet < 0)
        {
            DTK_OutputDebug("select ret = %d, err = %d", iRet, DTK_GetSocketLastError());
        }
    }

    DTK_Fini();
}

void Test_Sock()
{
    StartListen();
}