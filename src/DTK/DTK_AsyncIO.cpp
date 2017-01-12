
#include "DTK_Thread.h"
#include "DTK_AsyncIO.h"
#include "DTK_Utils.h"
#include "DTK_Mutex.h"
#include "DTK_Socket.h"
#include "DTK_Guard.h"
#include "DTK_Semaphore.h"
#include "DTK_Socket.h"
#include "DTK_Error.h"
#include <map>

#if defined OS_WINDOWS
//IO��������
typedef enum tagIoType			        
{
	IOTYPE_SEND,			            //�������ӷ���
	IOTYPE_RECV,			            //�������ӽ���
	IOTYPE_SENDTO,			            //���������ӷ���
	IOTYPE_RECVFROM,		            //���������ӽ���
	IOTYPE_WRITEFILE,		            //д�ļ�
	IOTYPE_READFILE,		            //���ļ�
	IOTYPE_POSTCOMPLETEQUEUE,           //������ɶ˿�
	IOTYPE_QUIT,			            //�˳�
}DTK_IO_TYPE;

//IO�����Ϣ�ṹ
typedef struct tagIoInfo		        
{
	DTK_HANDLE hIOFd;					    //IO������
	DTK_AsyncIOCallBack fIOProcessRoutine;  //fIOProcessRoutineΪ�ص���������
}SDTK_IOINFO, *LPSDTK_IOINFO;

//�첽���������Ϣ�ṹ
typedef struct tagIocpInfo				
{
	DTK_HANDLE hIOCP;					//�첽����������
	DTK_INT32 iWorkThreadNum;			//����������Ŀ
	DTK_HANDLE *phWorkthreadhandles;	//�������̾���б�
	DTK_SEM_T *pQuitSem;                //�߳��˳��ź�
}SDTK_IOCPINFO, *LPSDTK_IOCPINFO;

//IOCPͶ�����������ݽṹ
typedef struct tagIoData
{
	OVERLAPPED oa;				        //Microsoft��Ҫ�������ֶ�,�������޸�,
	DTK_IO_TYPE eIOType;		        //IO��������
	DTK_VOIDPTR pUsrData;		        //pUsrDataΪ�û��Զ���ָ��
	LPSDTK_IOINFO pIOInfo;
	DTK_ADDR_T* pAddr;			        //UDPʱ��Դ��ַ
	DTK_INT32 iAddrLen;			        //UDP�ǵ�Դ��ַ����
	DTK_INT32 iErrorCode;
	LPSDTK_IOCPINFO pIOCPInfo;
	DTK_INT32 iQuitIndex;
}IO_DATA, *LPIO_DATA;

typedef std::map<DTK_HANDLE, LPSDTK_IOCPINFO> IOCPMAP;	//�첽���������Ϣ�ṹ����MAP
typedef std::map<DTK_HANDLE, LPSDTK_IOINFO> IOINFOMAP;	//IO�����Ϣ�ṹ����MAP

static DTK_Mutex AsyncIOSuperLock;
static DTK_BOOL s_bAIOInit = DTK_FALSE;
static DTK_MUTEX_T s_lockMap;		    //ȫ����(��map�Ĳ������м���)
static IOINFOMAP s_mapIOInfo;		    //IO�����Ϣ�ṹ����
static IOCPMAP s_mapIocpInfo;		    //�첽���������Ϣ�ṹ����

DTK_INT32 DTK_AIOInit_Inter();
DTK_VOID DTK_AIOFini_Inter();

DTK_INT32 DTK_AIOInit_Inter()
{
	DTK_Guard cguard(&AsyncIOSuperLock);
	if (s_bAIOInit == DTK_FALSE)
	{
		DTK_MutexCreate(&s_lockMap);
		s_bAIOInit = DTK_TRUE;
	}

	return DTK_OK;
}

DTK_VOID DTK_AIOFini_Inter()
{
	DTK_Guard cguard(&AsyncIOSuperLock);
	if (s_bAIOInit == DTK_TRUE)
	{
		DTK_MutexDestroy(&s_lockMap);
		s_bAIOInit = DTK_FALSE;
	}
}

/** @fn static DTK_VOIDPTR CALLBACK AysncIO_WorkRoutine_Local(DTK_VOIDPTR pParam)
*   @brief ������ɶ˿ڵĹ����߳�
*   @return ��
*/
static DTK_VOIDPTR CALLBACK AysncIO_WorkRoutine_Local(DTK_VOIDPTR pParam)
{
	LPSDTK_IOCPINFO pIocpInfo = (LPSDTK_IOCPINFO)pParam;
    if (NULL == pIocpInfo)
    {
        return NULL;
    }

    DWORD dwTransCount = 0;
	DTK_HANDLE hIOFd = NULL;
	LPIO_DATA pIOData = NULL;
    DTK_BOOL bRet = DTK_FALSE;
	
	while (1)
    {
		bRet = GetQueuedCompletionStatus(pIocpInfo->hIOCP, &dwTransCount, (PULONG_PTR)&hIOFd, (LPOVERLAPPED *)&pIOData, DTK_INFINITE);
        if (DTK_FALSE == bRet)
        {
            //IO���������socket�ر�
            DTK_MutexLock(&s_lockMap);
            if (s_mapIOInfo.find(hIOFd) != s_mapIOInfo.end() && NULL != pIOData && pIOData->pIOInfo->fIOProcessRoutine)
            {
                if (pIOData->eIOType == IOTYPE_SEND 
                    || pIOData->eIOType == IOTYPE_RECV 
                    || pIOData->eIOType == IOTYPE_SENDTO 
                    || pIOData->eIOType == IOTYPE_RECVFROM)
                {
                    (*(pIOData->pIOInfo->fIOProcessRoutine))(DTK_GetSocketLastError(), dwTransCount, pIOData->pUsrData);
                }
                else
                {
                    (*(pIOData->pIOInfo->fIOProcessRoutine))(DTK_GetLastError(), dwTransCount, pIOData->pUsrData);
                }
            }
            DTK_MutexUnlock(&s_lockMap);

            if (NULL != pIOData)
            {
                delete pIOData;
                pIOData = NULL;
            }
        }
		else	
		{
            if (NULL == pIOData)
            {
                continue;
            }
            if (pIOData->eIOType == IOTYPE_QUIT)
            {
                DTK_SemPost(&(pIOData->pIOCPInfo->pQuitSem[pIOData->iQuitIndex]));
                delete pIOData;
                pIOData = NULL;
                break;
            }
            else if (pIOData->eIOType == IOTYPE_POSTCOMPLETEQUEUE)
            {
                DTK_MutexLock(&s_lockMap);
                if (s_mapIOInfo.find(hIOFd) != s_mapIOInfo.end())
                {
                    if (pIOData->pIOInfo->fIOProcessRoutine)
                    {
                        (*(pIOData->pIOInfo->fIOProcessRoutine))(pIOData->iErrorCode, dwTransCount, pIOData->pUsrData);
                    }
                }
                DTK_MutexUnlock(&s_lockMap);
            }
            else 
            {
                DTK_MutexLock(&s_lockMap);
                if (s_mapIOInfo.find(hIOFd) != s_mapIOInfo.end())
                {
                    if (pIOData->pIOInfo->fIOProcessRoutine)
                    {
                        (*(pIOData->pIOInfo->fIOProcessRoutine))(0, dwTransCount, pIOData->pUsrData);
                    }
                }
                DTK_MutexUnlock(&s_lockMap);
            }

            delete pIOData;
            pIOData = NULL;
		}
	}

	return NULL;
}

DTK_VOID DestoryIocpResource_Inter(LPSDTK_IOCPINFO pIOCPInfo)
{
    if ((pIOCPInfo->phWorkthreadhandles) != NULL)
    {
        //Ͷ���˳���Ϣ
        for (int i = 0; i < pIOCPInfo->iWorkThreadNum; i++)
        {
            LPIO_DATA pstTpData = new (std::nothrow) IO_DATA;
            pstTpData->eIOType = IOTYPE_QUIT;
            pstTpData->pIOCPInfo = pIOCPInfo;
            pstTpData->iQuitIndex = i;
            PostQueuedCompletionStatus(pIOCPInfo->hIOCP, 1, 0, (LPOVERLAPPED)pstTpData);
        }

        //��������߳�
        int waiti = 0;
        for (waiti = 0; waiti < pIOCPInfo->iWorkThreadNum; waiti++)
        {
            if ((pIOCPInfo->phWorkthreadhandles[waiti]) != DTK_INVALID_THREAD)
            {
                if (DTK_OK != DTK_SemTimedWait(&(pIOCPInfo->pQuitSem[waiti]), 3000))
                {
                    break;
                }
                CloseHandle(pIOCPInfo->phWorkthreadhandles[waiti]);
                pIOCPInfo->phWorkthreadhandles[waiti] = DTK_INVALID_THREAD;
            }
        }
        if (waiti < pIOCPInfo->iWorkThreadNum)
        {
            for (int i = 0; i < pIOCPInfo->iWorkThreadNum; i++)
            {
                if ((pIOCPInfo->phWorkthreadhandles[i]) != DTK_INVALID_THREAD)
                {
                    CloseHandle(pIOCPInfo->phWorkthreadhandles[waiti]);
                    pIOCPInfo->phWorkthreadhandles[i] = DTK_INVALID_THREAD;
                }
            }
        }

        delete [] pIOCPInfo->phWorkthreadhandles;
        pIOCPInfo->phWorkthreadhandles = NULL;
    }

    //����ź���
    if (pIOCPInfo->pQuitSem != NULL)
    {
        for (int i = 0; i < pIOCPInfo->iWorkThreadNum; i++)
        {
            DTK_SemDestroy(&(pIOCPInfo->pQuitSem[i]));
        }

        delete [] pIOCPInfo->pQuitSem;
        pIOCPInfo->pQuitSem = NULL;
    }

    delete pIOCPInfo;
    pIOCPInfo = NULL;
}

DTK_DECLARE DTK_HANDLE CALLBACK DTK_AsyncIO_CreateQueue(DTK_INT32 iThreadNum)
{
    DTK_HANDLE hIOCP = NULL;
    LPSDTK_IOCPINFO pIOCPInfo = NULL;

    do
    {
        hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (DTK_INVALID_HANDLE == hIOCP)
        {
            break;
        }

        pIOCPInfo = new (std::nothrow) SDTK_IOCPINFO;
        if (NULL == pIOCPInfo)
        {
            break;
        }

        pIOCPInfo->hIOCP = hIOCP;
        pIOCPInfo->iWorkThreadNum = DTK_GetCPUNumber() * 2 + 2;
        if (iThreadNum > 0)
        {
            pIOCPInfo->iWorkThreadNum = iThreadNum;
        }
        
        pIOCPInfo->pQuitSem = new (std::nothrow) DTK_SEM_T[pIOCPInfo->iWorkThreadNum];
        if (NULL == pIOCPInfo->pQuitSem)
        {
            break;
        }
        for (int i = 0; i < pIOCPInfo->iWorkThreadNum; i++)
        {
            DTK_SemCreate(&(pIOCPInfo->pQuitSem[i]), 1);
        }
        pIOCPInfo->phWorkthreadhandles = new (std::nothrow) DTK_HANDLE[pIOCPInfo->iWorkThreadNum];
        if (NULL == pIOCPInfo->phWorkthreadhandles)
        {
            break;
        }
        memset(pIOCPInfo->phWorkthreadhandles, 0, pIOCPInfo->iWorkThreadNum * sizeof(DTK_HANDLE));

        bool bError = false;
        for (int i = 0; i < pIOCPInfo->iWorkThreadNum; i++)
        {
            DTK_SemWait(&(pIOCPInfo->pQuitSem[i]));
            pIOCPInfo->phWorkthreadhandles[i] = DTK_Thread_Create(AysncIO_WorkRoutine_Local, pIOCPInfo, 0);
            if (DTK_INVALID_THREAD == pIOCPInfo->phWorkthreadhandles[i])
            {
                DTK_SemPost(&(pIOCPInfo->pQuitSem[i]));
                bError = true;
                break;
            }
        }
        if (bError)
        {
            break;
        }

        DTK_MutexLock(&s_lockMap);
        s_mapIocpInfo[hIOCP] = pIOCPInfo;
        DTK_MutexUnlock(&s_lockMap);

        return hIOCP;
    } while (0);


	if (NULL != pIOCPInfo)
	{
		DestoryIocpResource_Inter(pIOCPInfo);
	}

	if (NULL != hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;
	}

	return DTK_INVALID_ASYNCIOQUEUE;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_DestroyQueue(DTK_HANDLE hIOCP)
{
	if (hIOCP == DTK_INVALID_ASYNCIOQUEUE)
	{
		return DTK_ERROR;
	}

	DTK_MutexLock(&s_lockMap);
	if (s_mapIocpInfo.find(hIOCP) == s_mapIocpInfo.end())
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOCPINFO pIOCPInfo = s_mapIocpInfo[hIOCP];
	if (NULL != pIOCPInfo)
	{
		DestoryIocpResource_Inter(pIOCPInfo);
	}

	s_mapIocpInfo.erase(hIOCP);
	DTK_MutexUnlock(&s_lockMap);
	return CloseHandle(hIOCP);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindIOHandleToQueue(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP)
{
	if (DTK_SetNonBlock((DTK_SOCK_T)hIOFd, DTK_TRUE))
	{
		return DTK_ERROR;
	}

	DTK_MutexLock(&s_lockMap);
	if (s_mapIOInfo.find(hIOFd) != s_mapIOInfo.end())
    {
        DTK_MutexUnlock(&s_lockMap);
        return DTK_ERROR;
    }

    LPSDTK_IOINFO pIOInfo = new (std::nothrow) SDTK_IOINFO;
    if (NULL == pIOInfo)
    {
        DTK_MutexUnlock(&s_lockMap);
        return DTK_ERROR;
    }

    DTK_HANDLE hTmpIOCP = CreateIoCompletionPort(hIOFd, hIOCP, (DWORD)(hIOFd), 0);
    if (hTmpIOCP == NULL)
    {
        delete pIOInfo;
        pIOInfo = NULL;
        DTK_MutexUnlock(&s_lockMap);
        return DTK_ERROR;
    }

    pIOInfo->hIOFd = hIOFd;
    s_mapIOInfo[hIOFd] = pIOInfo;
    DTK_MutexUnlock(&s_lockMap);
    return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_UnBindIOHandle(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP)
{
	DTK_MutexLock(&s_lockMap);
	if (s_mapIOInfo.find(hIOFd) == s_mapIOInfo.end())
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOINFO pIOInfo = s_mapIOInfo[hIOFd];
    if (NULL != pIOInfo)
    {
        pIOInfo->fIOProcessRoutine = NULL;
        delete pIOInfo;
        pIOInfo = NULL;
    }
	
	s_mapIOInfo.erase(hIOFd);
	DTK_MutexUnlock(&s_lockMap);

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindCallBackToIOHandle(DTK_HANDLE hIOFd, DTK_AsyncIOCallBack pfnCallBack)
{
	if (hIOFd == DTK_INVALID_ASYNCIO)
	{
		return DTK_ERROR;
	}

	DTK_MutexLock(&s_lockMap);
	if (s_mapIOInfo.find(hIOFd) == s_mapIOInfo.end())
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOINFO pIOInfo = s_mapIOInfo[hIOFd];
	if (NULL == pIOInfo)
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}
	pIOInfo->fIOProcessRoutine = pfnCallBack;
	DTK_MutexUnlock(&s_lockMap);

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_PostQueuedCompleteStatus(DTK_HANDLE hIOCP, DTK_HANDLE hIOFd,DTK_INT32 iErrorCode,DTK_UINT32 nNumberOfBytesTransfered, DTK_VOIDPTR pUsrData)
{
	if (hIOFd == DTK_INVALID_ASYNCIO)
	{
		return DTK_ERROR;
	}

	DTK_MutexLock(&s_lockMap);
	if (s_mapIOInfo.find(hIOFd) == s_mapIOInfo.end())
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOINFO pIOInfo = s_mapIOInfo[hIOFd];
	if (pIOInfo == NULL)
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

    LPIO_DATA pIOData = new (std::nothrow) IO_DATA;
    if (NULL == pIOData)
    {
        DTK_MutexUnlock(&s_lockMap);
        return DTK_ERROR;
    }
	memset(pIOData, 0, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_POSTCOMPLETEQUEUE;
	pIOData->pUsrData = pUsrData;
	pIOData->pIOInfo = pIOInfo;
	pIOData->iErrorCode = iErrorCode;

	if (PostQueuedCompletionStatus(hIOCP, nNumberOfBytesTransfered, 0, (LPOVERLAPPED)pIOData))
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_OK;
	}
	else 
	{
		delete pIOData;
		pIOData = NULL;
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Send(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToSend, DTK_VOIDPTR pUsrData)
{
	if (DTK_INVALID_ASYNCIO == hIOFd)
	{
		return DTK_ERROR;
	}

	DTK_MutexLock(&s_lockMap);
	if (s_mapIOInfo.find(hIOFd) == s_mapIOInfo.end())
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOINFO pIOInfo = s_mapIOInfo[hIOFd];
	if (NULL == pIOInfo)
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}
	DTK_MutexUnlock(&s_lockMap);

    LPIO_DATA pIOData = new (std::nothrow) IO_DATA;
    if (NULL == pIOData)
    {
        return DTK_ERROR;
    }
    DTK_ZeroMemory(pIOData, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_SEND;
	pIOData->pUsrData = pUsrData;
	pIOData->pIOInfo = pIOInfo;

	DWORD dwSend = 0;
	DWORD dwFlags = 0;
	WSABUF wsaSendbuf;
	wsaSendbuf.buf = (char *)pBuffer;
	wsaSendbuf.len = nBytesToSend;
	if (SOCKET_ERROR == WSASend((SOCKET)hIOFd, &wsaSendbuf, 1, &dwSend, dwFlags, (LPOVERLAPPED)(pIOData), NULL))
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			return DTK_OK;
		}

		delete pIOData;
		pIOData = NULL;

		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Recv(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData)
{
	if (DTK_INVALID_ASYNCIO == hIOFd)
	{
		return DTK_ERROR;
	}

	DTK_MutexLock(&s_lockMap);
	if (s_mapIOInfo.find(hIOFd) == s_mapIOInfo.end())
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOINFO pIOInfo = s_mapIOInfo[hIOFd];
	if (NULL == pIOInfo)
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}
	DTK_MutexUnlock(&s_lockMap);

    LPIO_DATA pIOData = new (std::nothrow) IO_DATA;
    if (NULL == pIOData)
    {
        return DTK_ERROR;
    }
    DTK_ZeroMemory(pIOData, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_RECV;
	pIOData->pUsrData = pUsrData;
	pIOData->pIOInfo = pIOInfo;

	DWORD dwRecv = 0;
	DWORD dwFlags = 0;
	WSABUF wsaRecvbuf;
	wsaRecvbuf.buf = (char *)pBuffer;
	wsaRecvbuf.len = nBufferLen;
	if (SOCKET_ERROR == WSARecv((SOCKET)hIOFd, &wsaRecvbuf, 1, &dwRecv, &dwFlags, (LPOVERLAPPED)(pIOData), NULL))
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			return DTK_OK;
		}

		delete pIOData;
		pIOData = NULL;
		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_SendTo(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToSend, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pAddr)
{
	if ((DTK_INVALID_ASYNCIO == hIOFd) || (NULL == pAddr))
	{
		return DTK_ERROR;
	}

	DTK_MutexLock(&s_lockMap);
	if (s_mapIOInfo.find(hIOFd) == s_mapIOInfo.end())
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOINFO pIOInfo = s_mapIOInfo[hIOFd];
	if (pIOInfo == NULL)
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}
	DTK_MutexUnlock(&s_lockMap);

    LPIO_DATA pIOData = new (std::nothrow) IO_DATA;
    if (NULL == pIOData)
    {
        return DTK_ERROR;
    }
    DTK_ZeroMemory(pIOData, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_SENDTO;
	pIOData->pUsrData = pUsrData;
	pIOData->pIOInfo = pIOInfo;

	DWORD dwSend = 0;
	DWORD dwFlags = 0;
	WSABUF wsaSendBuf;
	wsaSendBuf.buf = (char *)pBuffer;
	wsaSendBuf.len = nBytesToSend;
	PSOCKADDR pSockAddr = (PSOCKADDR)&pAddr->SA;
#if defined (OS_SUPPORT_IPV6)
	int addrlen = pSockAddr->sa_family==AF_INET?sizeof(sockaddr_in):sizeof(sockaddr_in6);
#else
	int addrlen = sizeof(sockaddr_in);
#endif
	
	if (SOCKET_ERROR == WSASendTo((SOCKET)hIOFd, &wsaSendBuf, 1, &dwSend, dwFlags, (SOCKADDR *)&pAddr->SA, addrlen, (LPOVERLAPPED)(pIOData), NULL))
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			return DTK_OK;
		}

		delete pIOData;
		pIOData = NULL;
		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_RecvFrom(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pAddr)
{
	if (DTK_INVALID_ASYNCIO == hIOFd)
	{
		return DTK_ERROR;
	}

	DTK_MutexLock(&s_lockMap);
	if (s_mapIOInfo.find(hIOFd) == s_mapIOInfo.end())
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOINFO pIOInfo = s_mapIOInfo[hIOFd];
	if (pIOInfo == NULL)
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}
	DTK_MutexUnlock(&s_lockMap);

    LPIO_DATA pIOData = new (std::nothrow) IO_DATA;
    if (NULL == pIOData)
    {
        return DTK_ERROR;
    }
    DTK_ZeroMemory(pIOData, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_RECVFROM;
	pIOData->pUsrData = pUsrData;
	pIOData->pIOInfo = pIOInfo;

    DTK_INT32 iRet = -1;
	DWORD dwRecv = 0;
	DWORD dwFlags = 0;
	WSABUF wsaRecvBuf;
	wsaRecvBuf.buf = (char *)pBuffer;
	wsaRecvBuf.len = nBufferLen;
	if (pAddr != NULL) //����Ѱѵ�ַ���ݽ�ȥ,IOCPͶ�ݹ���ʱ��ص�����
	{
		PSOCKADDR pSockAddr = (PSOCKADDR)&pAddr->SA;
#if defined (OS_SUPPORT_IPV6)
		pIOData->iAddrLen = pSockAddr->sa_family==AF_INET?sizeof(sockaddr_in):sizeof(sockaddr_in6);
#else
		pIOData->iAddrLen = sizeof(sockaddr_in);
#endif
		pIOData->pAddr = pAddr;
		iRet = WSARecvFrom((SOCKET)hIOFd, &wsaRecvBuf, 1, &dwRecv, &dwFlags, (SOCKADDR *)&pIOData->pAddr->SA, &pIOData->iAddrLen, (LPOVERLAPPED)(pIOData), NULL);
	}
	else
	{
		iRet = WSARecvFrom((SOCKET)hIOFd, &wsaRecvBuf, 1, &dwRecv, &dwFlags, NULL, NULL, (LPOVERLAPPED)(pIOData), NULL);
	}

	if (SOCKET_ERROR == iRet)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			return DTK_OK;
		}
		delete pIOData;
		pIOData = NULL;
		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_WriteFile(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToWrite, DTK_VOIDPTR pUsrData)
{
	if (DTK_INVALID_ASYNCIO == hIOFd)
	{
		return DTK_ERROR;
	}

    DTK_MutexLock(&s_lockMap);
	if (s_mapIOInfo.find(hIOFd) == s_mapIOInfo.end())
	{
        DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOINFO pIOInfo = s_mapIOInfo[hIOFd];
	if (NULL == pIOInfo)
	{
        DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}
    DTK_MutexUnlock(&s_lockMap);

    LPIO_DATA pIOData = new (std::nothrow) IO_DATA;
    if (NULL == pIOData)
    {
        return DTK_ERROR;
    }
    DTK_ZeroMemory(pIOData, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_WRITEFILE;
	pIOData->pUsrData = pUsrData;
	pIOData->pIOInfo = pIOInfo;

	DWORD dwWrite = 0;

	if (0 == WriteFile((HANDLE)hIOFd, pBuffer, nBytesToWrite, &dwWrite, (LPOVERLAPPED)(pIOData)))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			delete pIOData;
			pIOData = NULL;

			return DTK_ERROR;
		}
	}
	
	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_ReadFile(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData)
{
	if (DTK_INVALID_ASYNCIO == hIOFd)
	{
		return DTK_ERROR;
	}

    DTK_MutexLock(&s_lockMap);
	if (s_mapIOInfo.find(hIOFd) == s_mapIOInfo.end())
	{
        DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOINFO pIOInfo = s_mapIOInfo[hIOFd];
	if (pIOInfo == NULL)
	{
        DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}
    DTK_MutexUnlock(&s_lockMap);

    LPIO_DATA pIOData = new (std::nothrow) IO_DATA;
    if (NULL == pIOData)
    {
        return DTK_ERROR;
    }
    DTK_ZeroMemory(pIOData, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_READFILE;
	pIOData->pUsrData = pUsrData;
	pIOData->pIOInfo = pIOInfo;

	DWORD dwRead = 0;

	if (0 == ReadFile((HANDLE)hIOFd, pBuffer, nBufferLen, &dwRead, (LPOVERLAPPED)(pIOData)))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			delete pIOData;
			pIOData = NULL;

			return DTK_ERROR;
		}
	}

	return DTK_OK;
}



DTK_AsyncIO::DTK_AsyncIO(DTK_HANDLE IOHandle)
: m_iohandle(IOHandle)
{
}

DTK_AsyncIO::~DTK_AsyncIO(DTK_VOID)
{
}

DTK_INT32 DTK_AsyncIO::BindIOHandleToQueue(DTK_AsyncIOQueue *pIOQueue)
{
	if (!pIOQueue)
	{
		return DTK_ERROR;
	}
	
	return DTK_AsyncIO_BindIOHandleToQueue(m_iohandle, pIOQueue->GetHandle());
}

DTK_INT32 DTK_AsyncIO::BindCallBackToIOHandle(DTK_VOID (*CallBackFunc)(DTK_ULONG ErrorCode, DTK_ULONG NumberOfBytes, DTK_VOIDPTR pUsrData))
{
	if (!CallBackFunc)
	{
		return DTK_ERROR;
	}

	return DTK_AsyncIO_BindCallBackToIOHandle(m_iohandle, CallBackFunc);
}

DTK_INT32 DTK_AsyncIO::IOSend(DTK_VOIDPTR Buffer, DTK_ULONG BytesToSend, DTK_VOIDPTR pUsrData)
{
	return DTK_AsyncIO_Send(m_iohandle, Buffer, BytesToSend, pUsrData);
}

DTK_INT32 DTK_AsyncIO::IORecv(DTK_VOIDPTR Buffer, DTK_ULONG BufferLen, DTK_VOIDPTR pUsrData)
{
	return DTK_AsyncIO_Recv(m_iohandle, Buffer, BufferLen, pUsrData);
}

DTK_INT32 DTK_AsyncIO::IOSendTo(DTK_VOIDPTR Buffer, DTK_ULONG BytesToSend, DTK_VOIDPTR pUsrData, DTK_ADDR_T* lpTo)
{
	return DTK_AsyncIO_SendTo(m_iohandle, Buffer, BytesToSend, pUsrData, lpTo);
}

DTK_INT32 DTK_AsyncIO::IORecvFrom(DTK_VOIDPTR Buffer, DTK_ULONG BufferLen, DTK_VOIDPTR pUsrData, DTK_ADDR_T* lpFrom)
{
	return DTK_AsyncIO_RecvFrom(m_iohandle, Buffer, BufferLen, pUsrData, lpFrom);
}

DTK_INT32 DTK_AsyncIO::IOWriteFile(DTK_VOIDPTR Buffer, DTK_ULONG BytesToWrite, DTK_VOIDPTR pUsrData)
{
	return DTK_AsyncIO_WriteFile(m_iohandle, Buffer, BytesToWrite, pUsrData);
}

DTK_INT32 DTK_AsyncIO::IOReadFile(DTK_VOIDPTR Buffer, DTK_ULONG BufferLen, DTK_VOIDPTR pUsrData)
{
	return DTK_AsyncIO_ReadFile(m_iohandle, Buffer, BufferLen, pUsrData);
}

DTK_AsyncIOQueue::DTK_AsyncIOQueue(DTK_VOID)
: m_queuehandle(NULL)
{
}

DTK_AsyncIOQueue::~DTK_AsyncIOQueue(DTK_VOID)
{
}

DTK_INT32 DTK_AsyncIOQueue::Initialize(DTK_VOID)
{
	m_queuehandle = (DTK_HANDLE)CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_queuehandle)
	{
		return DTK_OK;
	}
	else
	{
		return DTK_ERROR;
	}
}

DTK_INT32 DTK_AsyncIOQueue::Release(DTK_VOID)
{
	if (m_queuehandle)
	{
		BOOL bret = CloseHandle(m_queuehandle);
		m_queuehandle = NULL;
		if (bret)
		{
			return DTK_OK;
		}
		else
		{
			return DTK_ERROR;
		}
	}
	return DTK_ERROR;
}

DTK_HANDLE DTK_AsyncIOQueue::GetHandle(DTK_VOID)
{
	return m_queuehandle;
}

#endif
