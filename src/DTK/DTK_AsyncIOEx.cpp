
#include "DTK_Thread.h"
#include "DTK_AsyncIOEX.h"
#include "DTK_Utils.h"
#include "DTK_Mutex.h"
#include "DTK_Guard.h"
#include "DTK_Semaphore.h"
#include "DTK_Socket.h"
#include <deque>
#include <map>

#if defined OS_WINDOWS

#pragma comment(lib, "mswsock.lib")

enum DTK_IO_TYPE			//IO操作类型
{
	IOTYPE_SEND,			//面向连接发送
	IOTYPE_RECV,			//面向连接接收
	IOTYPE_SENDTO,			//面向无连接发送
	IOTYPE_RECVFROM,		//面向无连接接收
	IOTYPE_CONNECT,			//异步connect
	IOTYPE_ACCEPT,          //异步accept
	IOTYPE_WRITEFILE,		//写文件
	IOTYPE_READFILE,		//读文件
	IOTYPE_POSTCOMPLETEQUEUE,//加入完成端口
	IOTYPE_QUIT 			//退出
};

typedef struct	SDTK_IOINFO_T		//IO相关信息结构
{
	HANDLE hIOFd;					//IO描述符
	IOCALLBACK fIOProcessRoutine;	//fIOProcessRoutine为回调处理例程
} SDTK_IOINFO, *LPSDTK_IOINFO;

typedef struct SDTK_IOCPINFO_T				//异步队列相关信息结构
{
	HANDLE hIOCP;							//异步队列描述符
	DTK_INT32 iWorkThreadNum;				//工作例程数目
	DTK_HANDLE *phWorkthreadhandles;		//工作例程句柄列表
	DTK_SEM_T *pquit_sem;
} SDTK_IOCPINFO, *LPSDTK_IOCPINFO;

//IOCP投递上来的数据结构(其中pUsrData作为使用DTK接口用户传入的自定义数据)
typedef struct IO_DATA_T
{
	OVERLAPPED oa;				//Microsoft需要保留的字段,不进行修改,
	DTK_IO_TYPE eIOType;		//IO操作类型
	DTK_VOIDPTR pUsrData;		//pUsrData为用户自定义指针
	DTK_SOCK_T  accpetsock;
	DTK_ADDR_T* pAddr;			//UDP时的源地址(added by schina for udp src addr)
	DTK_INT32 iAddrLen;			//UDP是的源地址长度(added by schina udp src addr)
	DTK_INT32 iErrorCode;
	LPSDTK_IOCPINFO pIOCPInfo;
	IOCALLBACK fIOProcessRoutine;  //直接传回调函数下去
	DTK_INT32 iQuitIndex;
} IO_DATA, *LPIO_DATA;

typedef std::map<DTK_HANDLE, LPSDTK_IOCPINFO> IOCPMAP;	//异步队列相关信息结构管理MAP
typedef std::map<DTK_HANDLE, LPSDTK_IOINFO> IOINFOMAP;	//IO相关信息结构管理MAP

static DTK_Mutex AsyncIOSuperLock;
static DTK_BOOL s_bAIOInit = DTK_FALSE;
static DTK_MUTEX_T s_lockMap;		//全局锁(对map的操作进行加锁)
static IOINFOMAP s_mapIOInfo;		//IO相关信息结构集合
static IOCPMAP s_mapIOQueue;		//异步队列相关信息结构集合

//use mempool to alloc IO_DATA
#define USE_MEMPOOL  1

#if defined(USE_MEMPOOL)
#define MALLOC_SIZE 100000
static IO_DATA premalloc[MALLOC_SIZE];
static std::deque<DTK_VOIDPTR> g_queue;
static DTK_MUTEX_T g_buflock;

DTK_VOIDPTR GetPreMemory(DTK_VOID);
DTK_VOID FreePreMemory(DTK_VOIDPTR memory);
DTK_BOOL IsPreMemoryEmpty(DTK_VOID);

DTK_BOOL IsPreMemoryEmpty(DTK_VOID)
{
	DTK_MutexLock(&g_buflock);
	DTK_BOOL bempty = DTK_FALSE;
	if (g_queue.size() == 0)
	{
		bempty = DTK_TRUE;
	}

	DTK_MutexUnlock(&g_buflock);
	return bempty;
}

DTK_VOIDPTR GetPreMemory(DTK_VOID)
{
	DTK_MutexLock(&g_buflock);
	if (IsPreMemoryEmpty())
	{
		DTK_MutexUnlock(&g_buflock);
		return NULL;
	}

	DTK_VOIDPTR tmpptr = g_queue.front();
	g_queue.pop_front();
	DTK_MutexUnlock(&g_buflock);

	return tmpptr;
}

DTK_VOID FreePreMemory(DTK_VOIDPTR memory)
{
	if (!memory)
	{
		return;
	}

	DTK_MutexLock(&g_buflock);
	g_queue.push_back(memory);
	DTK_MutexUnlock(&g_buflock);
	return ;
}
#endif

DTK_INT32 DTK_AIOInit_InterEx();

void DTK_AIOFini_InterEx();

DTK_INT32 DTK_AIOInit_InterEx()
{
	DTK_Guard cguard(&AsyncIOSuperLock);
	if (s_bAIOInit == DTK_FALSE)
	{
		DTK_MutexCreate(&s_lockMap);

#if defined(USE_MEMPOOL)
		DTK_MutexCreate(&g_buflock);
		g_queue.clear();
		for (int i = 0; i < MALLOC_SIZE; i++)
		{
			g_queue.push_back(&(premalloc[i]));
		}
#endif

		s_bAIOInit = DTK_TRUE;
	}

	return DTK_OK;
}

void DTK_AIOFini_InterEx()
{
	DTK_Guard cguard(&AsyncIOSuperLock);
	if (s_bAIOInit == DTK_TRUE)
	{
#if defined(USE_MEMPOOL)
		g_queue.clear();
		DTK_MutexDestroy(&g_buflock);
#endif
		DTK_MutexDestroy(&s_lockMap);
		s_bAIOInit = DTK_FALSE;
	}
}

static DTK_VOIDPTR CALLBACK AysncIO_WorkRoutine_Local(DTK_VOIDPTR pParam)
{
	LPSDTK_IOCPINFO hIOCPInfo = (LPSDTK_IOCPINFO)pParam;
	HANDLE hIOFd = NULL;
	DWORD dwTransCount = 0;
	LPIO_DATA pIOData = NULL;
	BOOL bRet = FALSE;

	for (;;)
    {
		bRet = GetQueuedCompletionStatus(hIOCPInfo->hIOCP, &dwTransCount, (PULONG_PTR)&hIOFd, (LPOVERLAPPED *)&pIOData, INFINITE);
		if (bRet)
		{
			if (pIOData == NULL)
			{
				continue;
			}
			else if (pIOData->eIOType == IOTYPE_QUIT)
			{
				//int index = pIOData->iQuitIndex;
				DTK_SemPost(&(pIOData->pIOCPInfo->pquit_sem[pIOData->iQuitIndex]));
#if defined(USE_MEMPOOL)
				FreePreMemory((DTK_VOIDPTR)pIOData);
#else
				delete pIOData;
#endif
				pIOData = NULL;
				break;
			}
			else if (pIOData->eIOType == IOTYPE_POSTCOMPLETEQUEUE)
			{
				if ((pIOData->fIOProcessRoutine) != NULL)
				{
					(*(pIOData->fIOProcessRoutine))(pIOData->iErrorCode, dwTransCount, pIOData->pUsrData);
				}
			}
			else if (pIOData->eIOType == IOTYPE_ACCEPT)
			{
				if ((pIOData->fIOProcessRoutine) != NULL)
				{
				    (*(pIOData->fIOProcessRoutine))(0, pIOData->accpetsock, pIOData->pUsrData);
				}
			}
			else if (pIOData->eIOType == IOTYPE_CONNECT)
			{
				if ((pIOData->fIOProcessRoutine) != NULL)
				{
					(*(pIOData->fIOProcessRoutine))(0, dwTransCount, pIOData->pUsrData);
				}
			}
			else if ((pIOData->fIOProcessRoutine) != NULL)
			{
				(*(pIOData->fIOProcessRoutine))(0, dwTransCount, pIOData->pUsrData);
			}
			
#if defined(USE_MEMPOOL)
			FreePreMemory((DTK_VOIDPTR)pIOData);
#else
			delete pIOData;
#endif
			pIOData = NULL;
		}
		else	//IO操作错误或socket关闭bret会返回0
		{
            if (pIOData != NULL)
            {
                if (pIOData->eIOType == IOTYPE_ACCEPT)
                {
                    DTK_CloseSocket(pIOData->accpetsock);
                    pIOData->accpetsock = DTK_INVALID_SOCKET;
                }

                if ((pIOData->fIOProcessRoutine) != NULL)
                {
                    (*(pIOData->fIOProcessRoutine))(GetLastError(), 0, pIOData->pUsrData);
                }
            }


#if defined(USE_MEMPOOL)
			FreePreMemory((DTK_VOIDPTR)pIOData);
#else
			delete pIOData;
#endif
			pIOData = NULL;
		}
	}

	return NULL;
}

DTK_VOID DestoryIocpResource_InterEx(LPSDTK_IOCPINFO pIOCPInfo)
{
    if (NULL != pIOCPInfo->phWorkthreadhandles)
    {
        for (int i = 0; i < pIOCPInfo->iWorkThreadNum; i++)
        {
#if defined(USE_MEMPOOL)
            LPIO_DATA pTmpIOData = (LPIO_DATA)GetPreMemory();
#else
            LPIO_DATA pTmpIOData = new (::std::nothrow) IO_DATA;
#endif
            pTmpIOData->eIOType = IOTYPE_QUIT;
            pTmpIOData->pIOCPInfo = pIOCPInfo;
            pTmpIOData->iQuitIndex = i;
            PostQueuedCompletionStatus((HANDLE)(pIOCPInfo->hIOCP), 1, 0, (LPOVERLAPPED)pTmpIOData);
        }

        int waiti = 0;
        for (waiti = 0; waiti < pIOCPInfo->iWorkThreadNum; waiti++)
        {
            if ((pIOCPInfo->phWorkthreadhandles[waiti]) != (DTK_HANDLE)DTK_INVALID_THREAD)
            {
                int ret = DTK_SemTimedWait(&(pIOCPInfo->pquit_sem[waiti]), 3000);
                if (ret != DTK_OK)
                {
                    break;
                }

                CloseHandle(pIOCPInfo->phWorkthreadhandles[waiti]);
                pIOCPInfo->phWorkthreadhandles[waiti] = (DTK_HANDLE)DTK_INVALID_THREAD;
            }
        }

        if (waiti < pIOCPInfo->iWorkThreadNum)
        {
            for (int i = 0; i < pIOCPInfo->iWorkThreadNum; i++)
            {
                if ((pIOCPInfo->phWorkthreadhandles[i]) != (DTK_HANDLE)DTK_INVALID_THREAD)
                {
                    CloseHandle(pIOCPInfo->phWorkthreadhandles[i]);
                    pIOCPInfo->phWorkthreadhandles[i] = (DTK_HANDLE)DTK_INVALID_THREAD;
                }
            }
        }

        delete [] pIOCPInfo->phWorkthreadhandles;
        pIOCPInfo->phWorkthreadhandles = NULL;
    }

    if (pIOCPInfo->pquit_sem != NULL)
    {
        for (int i = 0; i < pIOCPInfo->iWorkThreadNum; i++)
        {
            DTK_SemDestroy(&(pIOCPInfo->pquit_sem[i]));
        }

        delete [] pIOCPInfo->pquit_sem;
        pIOCPInfo->pquit_sem = NULL;
    }

    delete pIOCPInfo;
    pIOCPInfo = NULL;
}

DTK_DECLARE DTK_HANDLE CALLBACK DTK_AsyncIO_CreateQueueEx(DTK_INT32 iThreadNum)
{
    if (iThreadNum < 0)
    {
        iThreadNum = DTK_GetCPUNumber() * 2 + 2;
    }

    DTK_HANDLE hIOCP = NULL;
	LPSDTK_IOCPINFO pIOCPInfo = NULL;

    do 
    {
        hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, iThreadNum);
        if (NULL == hIOCP)
        {
            break;
        }

        pIOCPInfo = new (::std::nothrow) SDTK_IOCPINFO;
        if (NULL == pIOCPInfo)
        {
            break;
        }

        pIOCPInfo->hIOCP = hIOCP;
        pIOCPInfo->iWorkThreadNum = iThreadNum;
        pIOCPInfo->pquit_sem = new (::std::nothrow) DTK_SEM_T[pIOCPInfo->iWorkThreadNum];
        if (NULL == pIOCPInfo->pquit_sem)
        {
            break;
        }
        for (int i = 0; i < pIOCPInfo->iWorkThreadNum; i++)
        {
            DTK_SemCreate(&(pIOCPInfo->pquit_sem[i]), 1);
        }
        pIOCPInfo->phWorkthreadhandles = new (::std::nothrow) DTK_HANDLE[pIOCPInfo->iWorkThreadNum];
        if (NULL == pIOCPInfo->phWorkthreadhandles)
        {
            break;
        }
        memset(pIOCPInfo->phWorkthreadhandles, 0, pIOCPInfo->iWorkThreadNum * sizeof(DTK_HANDLE));

        DTK_BOOL bError = false;
        for (int i = 0; i < pIOCPInfo->iWorkThreadNum; i++)
        {
            DTK_SemWait(&(pIOCPInfo->pquit_sem[i]));
            pIOCPInfo->phWorkthreadhandles[i] = DTK_Thread_Create(AysncIO_WorkRoutine_Local, pIOCPInfo, 0);
            if (DTK_INVALID_THREAD == pIOCPInfo->phWorkthreadhandles[i])
            {
                DTK_SemPost(&(pIOCPInfo->pquit_sem[i]));
                bError = DTK_TRUE;
                break;
            }
        }
        if (bError)
        {
            break;
        }

        DTK_MutexLock(&s_lockMap);
        s_mapIOQueue[hIOCP] = pIOCPInfo;
        DTK_MutexUnlock(&s_lockMap);

        return hIOCP;
    } while (0);
	


    if (NULL != pIOCPInfo)
	{
		DestoryIocpResource_InterEx(pIOCPInfo);
	}

	if (NULL != hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;
	}

	return DTK_INVALID_ASYNCIOQUEUE;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_DestroyQueueEx(DTK_HANDLE hIOCP)
{
	if (DTK_INVALID_ASYNCIOQUEUE == hIOCP)
	{
		return DTK_ERROR;
	}

	DTK_MutexLock(&s_lockMap);
	if (s_mapIOQueue.find(hIOCP) == s_mapIOQueue.end())
	{
		DTK_MutexUnlock(&s_lockMap);
		return DTK_ERROR;
	}

	LPSDTK_IOCPINFO pIOCPInfo = s_mapIOQueue[hIOCP];
	if (NULL != pIOCPInfo)
	{
		DestoryIocpResource_InterEx(pIOCPInfo);
	}

	s_mapIOQueue.erase(hIOCP);
	DTK_MutexUnlock(&s_lockMap);
	return CloseHandle(hIOCP);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindIOHandleToQueueEx(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP)
{
	if (DTK_SetNonBlock((int)hIOFd, DTK_TRUE))
	{
		return DTK_ERROR;
	}

	HANDLE hTmpIOCP = CreateIoCompletionPort((HANDLE)hIOFd, (HANDLE)hIOCP, (DWORD)(hIOFd), 0);
	if (hTmpIOCP == NULL)
	{
		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_UnBindIOHandleEx(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP)
{
	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindCallBackToIOHandleEx(DTK_HANDLE hIOFd, DTK_VOID (*fCallBackFunc)(DTK_ULONG ErrorCode, DTK_ULONG NumberOfBytes, DTK_VOIDPTR pUsrData))
{
	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_PostQueuedCompleteStatusEx(DTK_HANDLE hIOCP, DTK_HANDLE hIOFd,DTK_INT32 iErrorCode,DTK_UINT32 nNumberOfBytesTransfered, DTK_VOIDPTR pUsrData, DTK_VOIDPTR pCallBack)
{
	if ((hIOFd == DTK_INVALID_ASYNCIO) || (pCallBack == NULL))
	{
		return DTK_ERROR;
	}

#if defined(USE_MEMPOOL)
	LPIO_DATA pIOData = (LPIO_DATA)GetPreMemory();
#else
	LPIO_DATA pIOData = new (::std::nothrow) IO_DATA;
#endif
	if (pIOData == NULL)
	{
		DTK_OutputDebugString("DTK_AsyncIO_PostQueuedCompleteStatusEx no memory\n");
		return DTK_ERROR;
	}
	
	memset(pIOData, 0, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_POSTCOMPLETEQUEUE;
	pIOData->pUsrData = pUsrData;
	pIOData->fIOProcessRoutine = (IOCALLBACK)pCallBack;
	pIOData->iErrorCode = iErrorCode;

	if (PostQueuedCompletionStatus(hIOCP, nNumberOfBytesTransfered, 0, (LPOVERLAPPED)pIOData))
	{
		return DTK_OK;
	}
	else 
	{
#if defined(USE_MEMPOOL)
		FreePreMemory((DTK_VOIDPTR)pIOData);
#else
		delete pIOData;
#endif
		pIOData = NULL;
		return DTK_ERROR;
	}
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_SendEx(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToSend, DTK_VOIDPTR pUsrData, DTK_VOIDPTR pCallBack)
{
	if ((hIOFd == DTK_INVALID_ASYNCIO) || (pCallBack == NULL))
	{
		return DTK_ERROR;
	}

#if defined(USE_MEMPOOL)
	LPIO_DATA pIOData = (LPIO_DATA)GetPreMemory();
#else
	LPIO_DATA pIOData = new (::std::nothrow) IO_DATA;
#endif
	if (pIOData == NULL)
	{
		DTK_OutputDebugString("DTK_AsyncIO_SendEx no memory\n");
		return DTK_ERROR;
	}

	memset(pIOData, 0, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_SEND;
	pIOData->pUsrData = pUsrData;
	pIOData->fIOProcessRoutine = (IOCALLBACK)pCallBack;

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

#if defined(USE_MEMPOOL)
		FreePreMemory((DTK_VOIDPTR)pIOData);
#else
		delete pIOData;
#endif
		pIOData = NULL;

		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Accept(DTK_HANDLE hIOFd,  DTK_VOIDPTR pBuffer, DTK_SOCK_T sock, DTK_VOIDPTR pUsrData, DTK_VOIDPTR pCallBack)
{
	DTK_BOOL bRet = DTK_FALSE;
	DWORD address_length;
	DWORD bytes_received;
	
	if ((hIOFd == DTK_INVALID_ASYNCIO) || (pCallBack == NULL) || (sock == DTK_INVALID_ASYNCIO))
	{
		return DTK_ERROR;
	}

#if defined(USE_MEMPOOL)
	LPIO_DATA pIOData = (LPIO_DATA)GetPreMemory();
#else
	LPIO_DATA pIOData = new (::std::nothrow) IO_DATA;
#endif
	if (pIOData == NULL)
	{
		DTK_OutputDebugString("DTK_AsyncIO_Accept no memory\n");
		return DTK_ERROR;
	}

	memset(pIOData, 0, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_ACCEPT;
	pIOData->pUsrData = pUsrData;
	pIOData->fIOProcessRoutine = (IOCALLBACK)pCallBack;
	pIOData->accpetsock = sock;

	address_length = sizeof(struct sockaddr_in) + 16;
	bRet = AcceptEx((DTK_SOCK_T)hIOFd, 
		             sock, 
					 pBuffer, 
					 0,              //0 表示接受连接的时候,不接收任何数据
					 address_length, address_length,
					 &bytes_received,
					 (LPOVERLAPPED)(pIOData));
	if (!bRet)
	{
		if (WSAGetLastError() == ERROR_IO_PENDING)
		{
			return DTK_OK;
		}

#if defined(USE_MEMPOOL)
		FreePreMemory((DTK_VOIDPTR)pIOData);
#else
		delete pIOData;
#endif
		pIOData = NULL;

		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAcceptExSockaddrs(DTK_VOIDPTR pBuffer, struct sockaddr* plocal, struct sockaddr* premote)
{
	if (!plocal || !premote || !pBuffer)
	{
		return DTK_ERROR;
	}

	//DTK_INT32 addrlen = sizeof(struct sockaddr_in);
	/*GetAcceptExSockaddrs(pBuffer, 
		                 0,
						 sizeof(struct sockaddr_in) + 16,
						 sizeof(struct sockaddr_in) + 16,
						 plocal, addrlen,
						 premote,addrlen);*/

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_RecvEx(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData, DTK_VOIDPTR pCallBack)
{
	if ((hIOFd == DTK_INVALID_ASYNCIO) || (pCallBack == NULL))
	{
		return DTK_ERROR;
	}

#if defined(USE_MEMPOOL)
	LPIO_DATA pIOData = (LPIO_DATA)GetPreMemory();
#else
	LPIO_DATA pIOData = new (::std::nothrow) IO_DATA;
#endif
	if (pIOData == NULL)
	{
		DTK_OutputDebugString("DTK_AsyncIO_RecvEx no memory\n");
		return DTK_ERROR;
	}

	memset(pIOData, 0, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_RECV;
	pIOData->pUsrData = pUsrData;
	pIOData->fIOProcessRoutine = (IOCALLBACK)pCallBack;

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

#if defined(USE_MEMPOOL)
		FreePreMemory((DTK_VOIDPTR)pIOData);
#else
		delete pIOData;
#endif
		pIOData = NULL;

		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_SendToEx(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToSend, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pAddr, DTK_VOIDPTR pCallBack)
{
	if ((hIOFd == DTK_INVALID_ASYNCIO) || (pAddr == NULL) || (pCallBack == NULL))
	{
		return DTK_ERROR;
	}

#if defined(USE_MEMPOOL)
	LPIO_DATA pIOData = (LPIO_DATA)GetPreMemory();
#else
	LPIO_DATA pIOData = new (::std::nothrow) IO_DATA;
#endif
	if (pIOData == NULL)
	{
		DTK_OutputDebugString("DTK_AsyncIO_SendToEx no memory\n");
		return DTK_ERROR;
	}

	memset(pIOData, 0, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_SENDTO;
	pIOData->pUsrData = pUsrData;
	pIOData->fIOProcessRoutine = (IOCALLBACK)pCallBack;

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

#if defined(USE_MEMPOOL)
		FreePreMemory((DTK_VOIDPTR)pIOData);
#else
		delete pIOData;
#endif
		pIOData = NULL;

		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_RecvFromEx(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pAddr, DTK_VOIDPTR pCallBack)
{
	DTK_INT32 iRet = -1;
	if ((hIOFd == DTK_INVALID_ASYNCIO) || (pCallBack == NULL))
	{
		return DTK_ERROR;
	}

#if defined(USE_MEMPOOL)
	LPIO_DATA pIOData = (LPIO_DATA)GetPreMemory();
#else
	LPIO_DATA pIOData = new (::std::nothrow) IO_DATA;
#endif
	if (pIOData == NULL)
	{
		DTK_OutputDebugString("DTK_AsyncIO_RecvFromEx no memory\n");
		return DTK_ERROR;
	}

	memset(pIOData, 0, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_RECVFROM;
	pIOData->pUsrData = pUsrData;
	pIOData->fIOProcessRoutine = (IOCALLBACK)pCallBack;

	DWORD dwRecv = 0;
	DWORD dwFlags = 0;
	WSABUF wsaRecvBuf;
	wsaRecvBuf.buf = (char *)pBuffer;
	wsaRecvBuf.len = nBufferLen;
	if (pAddr != NULL) //这里把把地址传递进去,IOCP投递过来时会回调上来
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
#if defined(USE_MEMPOOL)
		FreePreMemory((DTK_VOIDPTR)pIOData);
#else
		delete pIOData;
#endif
		pIOData = NULL;

		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_AcceptEx(DTK_HANDLE hIOFd, DTK_ADDR_T* pPeerAddr,DTK_VOIDPTR pBuffer,DTK_INT32 iBufLen,
                                                    DTK_VOIDPTR pUsrData, DTK_VOIDPTR pCallBack, const DTK_INT32 iTimeOut)
{
	DTK_BOOL bRet = DTK_FALSE;
	DWORD address_length;
	DWORD bytes_received;

	if ((hIOFd == DTK_INVALID_ASYNCIO) || (pCallBack == NULL))
	{
		return DTK_ERROR;
	}

	//准备投递一个异步接收请求
	DTK_SOCK_T sock = DTK_CreateSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == DTK_INVALID_ASYNCIO)
	{
		return DTK_ERROR;
	}

#if !defined(USE_MEMPOOL)
	LPIO_DATA pIOData = new (::std::nothrow) IO_DATA;
#else
	LPIO_DATA pIOData = (LPIO_DATA)GetPreMemory();
#endif
	if (pIOData == NULL)
	{
		DTK_OutputDebugString("DTK_AsyncIO_AcceptEx no memory\n");
		return DTK_ERROR;
	}

	memset(pIOData, 0, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_ACCEPT;
	pIOData->pUsrData = pUsrData;
	pIOData->fIOProcessRoutine = (IOCALLBACK)pCallBack;
	pIOData->accpetsock = sock;

	address_length = sizeof(struct sockaddr_in) + 16;
	bRet = AcceptEx((DTK_SOCK_T)hIOFd, 
		sock, 
		pBuffer, 
		0,              //0 表示接受连接的时候,不接收任何数据
		address_length, address_length,
		&bytes_received,
		(LPOVERLAPPED)(pIOData));
	if (!bRet)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			DTK_CloseSocket(sock);
#if !defined(USE_MEMPOOL)
			delete pIOData;
#else
			FreePreMemory((DTK_VOIDPTR)pIOData);
#endif
			pIOData = NULL;

			return DTK_ERROR;
		}
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_ConnectEx(DTK_HANDLE hIOFd, DTK_ADDR_T* pPeerAddr, DTK_VOIDPTR pBuffer,DTK_INT32 iBufLen,
                                                     DTK_VOIDPTR pUsrData,DTK_VOIDPTR pCallBack, const DTK_INT32 iTimeOut)
{
	(void)(iBufLen);

	if ((hIOFd == DTK_INVALID_ASYNCIO) || (pPeerAddr == NULL) || (pCallBack == NULL))
	{
		return DTK_ERROR;
	}

	LPFN_CONNECTEX pConnectEx = NULL;
	//获取ConnectEx函数指针
	{
		DWORD dwBytes = 0;
		GUID guidConnectEx = WSAID_CONNECTEX;
		int dwErr = WSAIoctl((DTK_SOCK_T)hIOFd, 
			SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&guidConnectEx, 
			sizeof(guidConnectEx),
			&pConnectEx,
			sizeof(pConnectEx),
			&dwBytes,
			NULL,
			NULL);
		if (dwErr == SOCKET_ERROR)
		{
			return DTK_ERROR;
		}
	}

#if !defined(USE_MEMPOOL)
	LPIO_DATA pIoData = new(std::nothrow) IO_DATA;
#else
	LPIO_DATA pIoData = (LPIO_DATA)GetPreMemory();
#endif
	if (pIoData == NULL)
	{
		DTK_OutputDebugString("DTK_AsyncIO_ConnectEx no memory\n");
		return DTK_ERROR;
	}
	memset(pIoData, 0, sizeof(IO_DATA));

	pIoData->eIOType = IOTYPE_CONNECT;
	pIoData->pAddr = pPeerAddr;
	pIoData->pUsrData = pUsrData;
	pIoData->fIOProcessRoutine = (IOCALLBACK)pCallBack;

	PSOCKADDR pSockAddr = (PSOCKADDR)&pPeerAddr->SA;
#if defined (OS_SUPPORT_IPV6)
	int addrlen = pSockAddr->sa_family==AF_INET?sizeof(sockaddr_in):sizeof(sockaddr_in6);
#else
	int addrlen = sizeof(sockaddr_in);
#endif

	DTK_BOOL bRet = DTK_FALSE;
	DWORD dwBytesSend = 0;
	bRet = pConnectEx((DTK_SOCK_T)hIOFd, (SOCKADDR *)&pPeerAddr->SA, addrlen, pBuffer, 0, &dwBytesSend, (OVERLAPPED*)pIoData);
	if (bRet == DTK_FALSE)
	{
		if (WSAGetLastError() == ERROR_IO_PENDING)
		{
			return DTK_OK;
		}

		
#if !defined(USE_MEMPOOL)
		delete pIoData;
#else
		FreePreMemory((DTK_VOIDPTR)pIoData);
#endif
		pIoData = NULL;
		
		return DTK_ERROR;
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_WriteFileEx(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToWrite, DTK_VOIDPTR pUsrData, DTK_VOIDPTR pCallBack)
{
	if ((hIOFd == DTK_INVALID_ASYNCIO) || (pCallBack == NULL))
	{
		return DTK_ERROR;
	}

#if defined(USE_MEMPOOL)
	LPIO_DATA pIOData = (LPIO_DATA)GetPreMemory();
#else
	LPIO_DATA pIOData = new (::std::nothrow) IO_DATA;
#endif
	if (pIOData == NULL)
	{
		DTK_OutputDebugString("DTK_AsyncIO_WriteFileEx no memory\n");
		return DTK_ERROR;
	}

	memset(pIOData, 0, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_WRITEFILE;
	pIOData->pUsrData = pUsrData;
	pIOData->fIOProcessRoutine = (IOCALLBACK)pCallBack;

	DWORD dwWrite = 0;

	if (0 == WriteFile((HANDLE)hIOFd, pBuffer, nBytesToWrite, &dwWrite, (LPOVERLAPPED)(pIOData)))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
#if defined(USE_MEMPOOL)
			FreePreMemory((DTK_VOIDPTR)pIOData);
#else
			delete pIOData;
#endif
			pIOData = NULL;

			return DTK_ERROR;
		}
	}
	
	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_ReadFileEx(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData, DTK_VOIDPTR pCallBack)
{
	if ((hIOFd == DTK_INVALID_ASYNCIO) || (pCallBack == NULL))
	{
		return DTK_ERROR;
	}

#if defined(USE_MEMPOOL)
	LPIO_DATA pIOData = (LPIO_DATA)GetPreMemory();
#else
	LPIO_DATA pIOData = new (::std::nothrow) IO_DATA;
#endif
	if (pIOData == NULL)
	{
		DTK_OutputDebugString("DTK_AsyncIO_ReadFileEx no memory\n");
		return DTK_ERROR;
	}

	memset(pIOData, 0, sizeof(IO_DATA));
	pIOData->eIOType = IOTYPE_READFILE;
	pIOData->pUsrData = pUsrData;
	pIOData->fIOProcessRoutine = (IOCALLBACK)pCallBack;

	DWORD dwRead = 0;

	if (0 == ReadFile((HANDLE)hIOFd, pBuffer, nBufferLen, &dwRead, (LPOVERLAPPED)(pIOData)))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
#if defined(USE_MEMPOOL)
			FreePreMemory((DTK_VOIDPTR)pIOData);
#else
			delete pIOData;
#endif
			pIOData = NULL;

			return DTK_ERROR;
		}
	}

	return DTK_OK;
}

#endif