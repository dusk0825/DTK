
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
*   @brief �����첽����
*   @param [in] iThreadNum  ������ɶ˿ڵ��߳�������Ĭ��CPU*2+2
*   @return �ɹ��첽���о����ʧ��DTK_INVALID_ASYNCIOQUEUE
*/
DTK_DECLARE DTK_HANDLE CALLBACK DTK_AsyncIO_CreateQueue(DTK_INT32 iThreadNum = 0);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_DestroyQueue(DTK_HANDLE hIOCP)
*   @brief �����첽����
*   @param [in] hIOCP  �첽���о��
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_DestroyQueue(DTK_HANDLE hIOCP);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindIOHandleToQueue(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP)
*   @brief ��IO�󶨵��첽����
*   @param [in] hIOFd   IO���
*   @param [in] hIOCP   �첽���о��
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindIOHandleToQueue(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_UnBindIOHandle(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP)
*   @brief ��IO���첽����ɾ��
*   @param [in] hIOFd   IO���
*   @param [in] hIOCP   �첽���о��
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_UnBindIOHandle(DTK_HANDLE hIOFd, DTK_HANDLE hIOCP);

/** @fn DTK_VOID (*DTK_AsyncIOCallBack)(DTK_ULONG nErrorCode, DTK_ULONG nNumberOfBytes, DTK_VOIDPTR pUsrData)
*   @brief �첽IO�����Ļص�����
*   @param [in] nErrorCode      ������
*   @param [in] nNumberOfBytes  �����ֽ�
*   @param [in] pUsrData        �û��Զ�������
*   @return ��
*/
typedef DTK_VOID (*DTK_AsyncIOCallBack)(DTK_ULONG nErrorCode, DTK_ULONG nNumberOfBytes, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindCallBackToIOHandle(DTK_HANDLE hIOFd, DTK_AsyncIOCallBack pfnCallBack)
*   @brief �󶨻ص���������ӦIO
*   @param [in] hIOFd           IO���
*   @param [in] pfnCallBack     �ص�����
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_BindCallBackToIOHandle(DTK_HANDLE hIOFd, DTK_AsyncIOCallBack pfnCallBack);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_PostQueuedCompleteStatus(DTK_HANDLE hIOCP, DTK_HANDLE hIOFd,DTK_INT32 iErrorCode,DTK_UINT32 nNumberOfBytesTransfered, DTK_VOIDPTR pUsrData)
*   @brief Ͷ����ɶ˿���Ϣ
*   @param [in] hIOFd           IO���
*   @param [in] hIOCP           �첽���о��
*   @param [in] nErrorCode      ������
*   @param [in] nNumberOfBytes  �����ֽ�
*   @param [in] pUsrData        �û��Զ�������
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_PostQueuedCompleteStatus(DTK_HANDLE hIOCP, DTK_HANDLE hIOFd,DTK_INT32 iErrorCode,DTK_UINT32 nNumberOfBytesTransfered, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Send(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG BytesToSend, DTK_VOIDPTR pUsrData)
*   @brief Ͷ���첽���Ͳ���(tcp)
*   @param [in] hIOFd           IO���
*   @param [in] pBuffer         ���ͻ�����
*   @param [in] nBytesToSend    �������ݳ���
*   @param [in] pUsrData        �û��Զ�������
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Send(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToSend, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Recv(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData)
*   @brief Ͷ���첽���ղ���(tcp)
*   @param [in] hIOFd           IO���
*   @param [in] pBuffer         ���ջ�����
*   @param [in] BytesToSend     ���ջ�������С
*   @param [in] pUsrData        �û��Զ�������
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Recv(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_SendTo(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToSend, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pDstAddr)
*   @brief Ͷ���첽���Ͳ���(udp)
*   @param [in] hIOFd           IO���
*   @param [in] pBuffer         ���ͻ�����
*   @param [in] nBytesToSend    �������ݳ���
*   @param [in] pUsrData        �û��Զ�������
*   @param [in] pDstAddr        ���͵�ַ
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_SendTo(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToSend, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pDstAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_Recv(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData)
*   @brief Ͷ���첽���ղ���(udp)
*   @param [in] hIOFd           IO���
*   @param [in] pBuffer         ���ջ�����
*   @param [in] BytesToSend     ���ջ�������С
*   @param [in] pUsrData        �û��Զ�������
*   @param [in] pAddrSrc        �������ݵ�Դ��ַ
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_RecvFrom(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData, DTK_ADDR_T* pAddrSrc);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_WriteFile(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToWrite, DTK_VOIDPTR pUsrData)
*   @brief Ͷ���첽д�ļ��Ĳ���
*   @param [in] hIOFd           IO���
*   @param [in] pBuffer         ���ݻ�����
*   @param [in] nBytesToWrite   д���ݳ���
*   @param [in] pUsrData        �û��Զ�������
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_WriteFile(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBytesToWrite, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AsyncIO_ReadFile(DTK_HANDLE hIOFd, DTK_VOIDPTR pBuffer, DTK_ULONG nBufferLen, DTK_VOIDPTR pUsrData)
*   @brief Ͷ���첽���ļ��Ĳ���
*   @param [in] hIOFd           IO���
*   @param [in] pBuffer         ���ݻ�����
*   @param [in] nBufferLen      �����ݳ���
*   @param [in] pUsrData        �û��Զ�������
*   @return �ɹ�����0��ʧ��-1
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