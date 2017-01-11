
#ifndef __DTK_ASYNCIO_H__  
#define __DTK_ASYNCIO_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"
#include "DTK_Addr.h"

#if defined(OS_WINDOWS)
	#define DTK_INVALID_ASYNCIOQUEUE NULL
	#define DTK_INVALID_ASYNCIO NULL
#elif defined(OS_POSIX)
	#define DTK_INVALID_ASYNCIOQUEUE (DTK_VOIDPTR)-1
	#define DTK_INVALID_ASYNCIO (DTK_VOIDPTR)-1
#else
	#error OS Not Implement Yet.
#endif

#if defined OS_WINDOWS
/** @fn DTK_DECLARE DTK_HANDLE CALLBACK DTK_AsyncIO_CreateQueue(DTK_INT32 iThreadNum = 0)
*   @brief 创建异步队列
*   @param [in] iThreadNum  监听完成端口的线程数量，默认CPU*2+2
*   @return 成功异步队列句柄，失败DTK_INVALID_ASYNCIOQUEUE
*/
DTK_DECLARE DTK_HANDLE CALLBACK DTK_AsyncIO_CreateQueue(DTK_INT32 iThreadNum = 0);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_DestroyQueue(DTK_HANDLE hIOCP)
*   @brief 销毁异步队列
*   @param [in] hIOCP  异步队列句柄
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_DestroyQueue(DTK_HANDLE hIOCP);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindIOHandleToQueue(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP)
*   @brief 将IO绑定到异步队列
*   @param [in] hIOFd   IO句柄
*   @param [in] hIOCP   异步队列句柄
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindIOHandleToQueue(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_UnBindIOHandle(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP)
*   @brief 将IO从异步队列删除
*   @param [in] hIOFd   IO句柄
*   @param [in] hIOCP   异步队列句柄
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_UnBindIOHandle(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP);

/** @fn DTK_VOID (*DTK_AsyncIOCallBack)(DTK_ULONG nErrorCode, DTK_ULONG nNumberOfBytes, DTK_VOIDPTR pUsrData)
*   @brief 异步IO操作的回调函数
*   @param [in] nErrorCode      错误码
*   @param [in] nNumberOfBytes  数据字节
*   @param [in] pUsrData        用户自定义数据
*   @return 无
*/
typedef DTK_VOID (*DTK_AsyncIOCallBack)(DTK_ULONG nErrorCode, DTK_ULONG nNumberOfBytes, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindCallBackToIOHandle(DTK_HANDLE hIOFd, DTK_AsyncIOCallBack pfnCallBack)
*   @brief 绑定回调函数到相应IO
*   @param [in] hIOFd           IO句柄
*   @param [in] pfnCallBack     回调函数
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindCallBackToIOHandle(DTK_HANDLE hIOFd, DTK_AsyncIOCallBack pfnCallBack);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_PostQueuedCompleteStatus(DTK_HANDLE hIOCP, DTK_HANDLE hIOFd,DTK_INT32 iErrorCode,DTK_UINT32 nNumberOfBytesTransfered, DTK_VOIDPTR pUsrData)
*   @brief 投递完成端口消息
*   @param [in] hIOFd           IO句柄
*   @param [in] hIOCP           异步队列句柄
*   @param [in] nErrorCode      错误码
*   @param [in] nNumberOfBytes  数据字节
*   @param [in] pUsrData        用户自定义数据
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_PostQueuedCompleteStatus(DTK_HANDLE hIOCP, DTK_HANDLE hIOFd,DTK_INT32 iErrorCode,DTK_UINT32 nNumberOfBytesTransfered, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Send(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG BytesToSend, DTK_VOIDPTR pUsrData)
*   @brief 投递异步发送操作(tcp)
*   @param [in] hIOFd           IO句柄
*   @param [in] pBuffer         发送缓冲区
*   @param [in] nBytesToSend    发送数据长度
*   @param [in] pUsrData        用户自定义数据
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Send(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToSend, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Recv(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData)
*   @brief 投递异步接收操作(tcp)
*   @param [in] hIOFd           IO句柄
*   @param [in] pBuffer         接收缓冲区
*   @param [in] BytesToSend     接收缓冲区大小
*   @param [in] pUsrData        用户自定义数据
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Recv(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_SendTo(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToSend, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pDstAddr)
*   @brief 投递异步发送操作(udp)
*   @param [in] hIOFd           IO句柄
*   @param [in] pBuffer         发送缓冲区
*   @param [in] nBytesToSend    发送数据长度
*   @param [in] pUsrData        用户自定义数据
*   @param [in] pDstAddr        发送地址
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_SendTo(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToSend, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pDstAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Recv(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData)
*   @brief 投递异步接收操作(udp)
*   @param [in] hIOFd           IO句柄
*   @param [in] pBuffer         接收缓冲区
*   @param [in] BytesToSend     接收缓冲区大小
*   @param [in] pUsrData        用户自定义数据
*   @param [in] pAddrSrc        接收数据的源地址
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_RecvFrom(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pAddrSrc);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_WriteFile(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToWrite, DTK_VOIDPTR pUsrData)
*   @brief 投递异步写文件的操作
*   @param [in] hIOFd           IO句柄
*   @param [in] pBuffer         数据缓冲区
*   @param [in] nBytesToWrite   写数据长度
*   @param [in] pUsrData        用户自定义数据
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_WriteFile(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToWrite, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_ReadFile(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData)
*   @brief 投递异步读文件的操作
*   @param [in] hIOFd           IO句柄
*   @param [in] pBuffer         数据缓冲区
*   @param [in] nBufferLen      读数据长度
*   @param [in] pUsrData        用户自定义数据
*   @return 成功返回0，失败-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_ReadFile(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData);

class DTK_AsyncIOQueue;

class DTK_AsyncIO
{
public:
	DTK_AsyncIO(DTK_HANDLE IOHandle);
	~DTK_AsyncIO(DTK_VOID);

	DTK_INT32 BindIOHandleToQueue(DTK_AsyncIOQueue *pIOQueue);
	DTK_INT32 BindCallBackToIOHandle(DTK_VOID (*CallBackFunc)(DTK_ULONG nErrorCode, DTK_ULONG NumberOfBytes, DTK_VOIDPTR pUsrData));
	DTK_INT32 IOSend(DTK_VOIDPTR pBuffer, DTK_ULONG BytesToSend, DTK_VOIDPTR pUsrData);
	DTK_INT32 IORecv(DTK_VOIDPTR pBuffer, DTK_ULONG BufferLen, DTK_VOIDPTR pUsrData);
	DTK_INT32 IOSendTo(DTK_VOIDPTR pBuffer, DTK_ULONG BytesToSend, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pToAddr);
	DTK_INT32 IORecvFrom(DTK_VOIDPTR pBuffer, DTK_ULONG BufferLen, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pFromAddr);
	DTK_INT32 IOWriteFile(DTK_VOIDPTR pBuffer, DTK_ULONG BytesToWrite, DTK_VOIDPTR pUsrData);
	DTK_INT32 IOReadFile(DTK_VOIDPTR pBuffer, DTK_ULONG BufferLen, DTK_VOIDPTR pUsrData);

private:
	DTK_HANDLE m_iohandle;
};

class DTK_AsyncIOQueue
{
public:
	DTK_AsyncIOQueue(DTK_VOID);
	~DTK_AsyncIOQueue(DTK_VOID);

	DTK_INT32 Initialize(DTK_VOID);
	DTK_INT32 Release(DTK_VOID);
	DTK_HANDLE GetHandle(DTK_VOID);

private:
	DTK_HANDLE m_queuehandle;
};

#endif

#endif // __DTK_ASYNCIO_H__ 