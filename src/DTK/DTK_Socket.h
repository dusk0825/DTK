
#ifndef __DTK_SOCKET_H__  
#define __DTK_SOCKET_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"
#include "DTK_Addr.h"


#if defined(OS_WINDOWS)
	#define DTK_SOCK_ERROR SOCKET_ERROR
	#define DTK_INVALID_SOCKET INVALID_SOCKET
	#define DTK_SHUT_RD		SD_RECEIVE
	#define DTK_SHUT_WR		SD_SEND
	#define DTK_SHUT_RDWR	SD_BOTH
	static DTK_BOOL g_bWSAStartup = DTK_FALSE;
#elif defined(OS_POSIX)
	#define DTK_SOCK_ERROR		(-1)
	#define DTK_INVALID_SOCKET (-1)
	#define DTK_SHUT_RD		SHUT_RD
	#define DTK_SHUT_WR		SHUT_WR
	#define DTK_SHUT_RDWR	SHUT_RDWR
#else
	#error OS Not Implement Yet.
#endif

/** @fn DTK_DECLARE DTK_SOCK_T CALLBACK DTK_CreateSocket(DTK_INT32 iAf, DTK_INT32 iType, DTK_INT32 iProto)
*   @brief �����׽���
*   @param [in] iAf         ��ַ�����ͣ�AF_INET(IPv4Э��)��AF_INET6(IPv6Э��)
*   @param [in] iType       �׽������ͣ�SOCK_STREAM(�ֽ���)��SOCK_DGRAM(���ݱ�)��SOCK_RAW(ԭʼ�׽ӿ�)
*   @param [out] iProto     ��Ӧ�Ĵ���Э�飬ʹ��Ĭ��ֵ0
*   @return �ɹ������׽���
*/
DTK_DECLARE DTK_SOCK_T CALLBACK DTK_CreateSocket(DTK_INT32 iAf, DTK_INT32 iType, DTK_INT32 iProto);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_CloseSocket(DTK_SOCK_T iSockFd, DTK_BOOL bForce = DTK_FALSE)
*   @brief �ر��׽���
*   @param [in] iSockFd     �׽���
*   @param [in] bForce      �Ƿ�ǿ�ƹر�
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_CloseSocket(DTK_SOCK_T iSockFd, DTK_BOOL bForce = DTK_FALSE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ShutDown(DTK_SOCK_T iSockFd, DTK_INT32 iHow)
*   @brief ��ֹ��һ���׽ӿ��Ͻ������ݵĽ����뷢��
*   @param [in] iSockFd     �׽���
*   @param [in] iHow        ��ֹ��Щ����(DTK_SHUT_RD/DTK_SHUT_WR/DTK_SHUT_RDWR) 
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ShutDown(DTK_SOCK_T iSockFd, DTK_INT32 iHow);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Bind(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr)
*   @brief ��һ��̶��ĵ�ַ�󶨵��׽�����
*   @param [in] iSockFd     �׽���
*   @param [in] pDTKAddr    ��ַ
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Bind(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Listen(DTK_SOCK_T iSockFd, DTK_INT32 iBackLog)
*   @brief ���õȴ����Ӷ��е���󳤶�
*   @param [in] iSockFd     �׽���
*   @param [in] iBackLog    �ȴ����Ӷ��е���󳤶ȣ�posix���ֵ128
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Listen(DTK_SOCK_T iSockFd, DTK_INT32 iBackLog);

/** @fn DTK_DECLARE DTK_SOCK_T CALLBACK DTK_Accept(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr = NULL, DTK_UINT32 nTimeOut = DTK_INFINITE)
*   @brief ����Զ������
*   @param [in] iSockFd     �����׽���
*   @param [in] pDTKAddr    �ͻ��˵�ַ
*   @param [in] nTimeOut    ��ʱʱ�䣬����
*   @return �ɹ����ؿͻ���socket
*/
DTK_DECLARE DTK_SOCK_T CALLBACK DTK_Accept(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr = NULL, DTK_UINT32 nTimeOut = DTK_INFINITE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ConnectWithTimeOut(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr, DTK_UINT32 nTimeOut=DTK_INFINITE)
*   @brief ���ӷ���ˣ������ӳ�ʱ
*   @param [in] iSockFd     �����׽���
*   @param [in] pDTKAddr    ����˵�ַ
*   @param [in] nTimeOut    ��ʱʱ�䣬����
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ConnectWithTimeOut(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr, DTK_UINT32 nTimeOut=DTK_INFINITE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetNonBlock(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo)
*   @brief �����׽���Ϊ��������/����������
*   @param [in] iSockFd     �׽���
*   @param [in] bYesNo      true ��������false ����
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetNonBlock(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetBuffSize(DTK_SOCK_T iSockFd, DTK_INT32 iSndBuffSize, DTK_INT32 iRcvBuffSize)
*   @brief ���÷��ͺͽ��ջ�������С
*   @param [in] iSockFd         �׽���
*   @param [in] iSndBuffSize    ���ͻ�������С
*   @param [in] iRcvBuffSize    ���ջ�������С
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetBuffSize(DTK_SOCK_T iSockFd, DTK_INT32 iSndBuffSize, DTK_INT32 iRcvBuffSize);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetBuffSize(DTK_SOCK_T iSockFd, DTK_INT32* iSndBuffSize, DTK_INT32* iRcvBuffSize)
*   @brief ��ȡ���ͺͽ��ջ�������С
*   @param [in] iSockFd         �׽���
*   @param [in] iSndBuffSize    ���ͻ�������С
*   @param [in] iRcvBuffSize    ���ջ�������С
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetBuffSize(DTK_SOCK_T iSockFd, DTK_INT32* iSndBuffSize, DTK_INT32* iRcvBuffSize);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetReuseAddr(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo)
*   @brief �����׽ӿں�һ������ʹ���еĵ�ַ����
*   @param [in] iSockFd         �׽���
*   @param [in] bYesNo          enable/disable reuse address
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetReuseAddr(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTimeOut(DTK_SOCK_T iSockFd, DTK_INT32 iSndTimeO, DTK_INT32 iRcvTimeO)
*   @brief ���÷��ͺͽ��ճ�ʱ
*   @param [in] iSockFd         �׽���
*   @param [in] iSndTimeO       ���ͳ�ʱʱ��
*   @param [in] iRcvTimeO       ���ճ�ʱʱ��
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTimeOut(DTK_SOCK_T iSockFd, DTK_INT32 iSndTimeO, DTK_INT32 iRcvTimeO);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_BroadCast(DTK_SOCK_T iSockFd)
*   @brief �����׽ӿڴ��͹㲥��Ϣ
*   @param [in] iSockFd         �׽���
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_BroadCast(DTK_SOCK_T iSockFd);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTTL(DTK_SOCK_T iSockFd, DTK_INT32 iTTL)
*   @brief ���ñ�������ʱ��
*   @param [in] iSockFd         �׽���
*   @param [in] iTTL            ʱ��ֵ        
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTTL(DTK_SOCK_T iSockFd, DTK_INT32 iTTL);

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTTL_V6(DTK_SOCK_T iSockFd, DTK_INT32 iTTL);

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetMultiCastTTL(DTK_SOCK_T iSockFd, DTK_INT32 iTTL);

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetMultiCastTTL_V6(DTK_SOCK_T iSockFd, DTK_INT32 iTTL);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTTL(DTK_SOCK_T iSockFd, DTK_INT32* iTTL)
*   @brief ��ȡ��������ʱ��
*   @param [in] iSockFd         �׽���
*   @param [in] iTTL            ʱ��ֵ 
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTTL(DTK_SOCK_T iSockFd, DTK_INT32* iTTL);

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTTL_V6(DTK_SOCK_T iSockFd, DTK_INT32* iTTL);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTOS(DTK_SOCK_T iSockFd, DTK_INT32 iTOS)
*   @brief ���÷�������
*   @param [in] iSockFd         �׽���
*   @param [in] iTOS            ��������
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTOS(DTK_SOCK_T iSockFd, DTK_INT32 iTOS);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTOS(DTK_SOCK_T iSockFd, DTK_INT32 iTOS)
*   @brief ��ȡ��������
*   @param [in] iSockFd         �׽���
*   @param [in] iTOS            ��������
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTOS(DTK_SOCK_T iSockFd, DTK_INT32* iTOS);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_LingerOff(DTK_SOCK_T iSockFd)
*   @brief disable�ӳٹر�����(tcp)
*   @param [in] iSockFd         �׽���
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_LingerOff(DTK_SOCK_T iSockFd);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_LingerOn(DTK_SOCK_T iSockFd,DTK_UINT16 nTimeOutSec)
*   @brief enable�ӳٹر�����tcp�����closesocket����sendδ������ȫ�����ݶ�ʧ��
*   @param [in] iSockFd         �׽���
*   @param [in] nTimeOutSec     �ӳٹر�ʱ�䣬��
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_LingerOn(DTK_SOCK_T iSockFd,DTK_UINT16 nTimeOutSec);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetNoDelay(DTK_SOCK_T iSockFd,DTK_BOOL bYesNo)
*   @brief enable/disabel nagle�㷨��Nagle�㷨ͨ����δȷ�ϵ����ݴ��뻺����ֱ������һ����һ���͵ķ������������������͵�����С���ݰ�����Ŀ��
*   @param [in] iSockFd         �׽���
*   @param [in] bYesNo          enable/disabel
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetNoDelay(DTK_SOCK_T iSockFd,DTK_BOOL bYesNo);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_JoinMultiCastGroup(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pMcastGroupAddr)
*   @brief ��ָ���ӿ��ϼ����鲥��
*   @param [in] iSockFd         �׽���
*   @param [in] pLocal          ���ص�ַ
*   @param [in] pMcastGroupAddr �鲥��ַ
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_JoinMultiCastGroup(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pMcastGroupAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_LeaveMultiCastGroup(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pMcastGroupAddr)
*   @brief �˳��鲥��
*   @param [in] iSockFd         �׽���
*   @param [in] pLocal          ���ص�ַ
*   @param [in] pMcastGroupAddr �鲥��ַ
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_LeaveMultiCastGroup(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pMcastGroupAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Send(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen)
*   @brief ��������(tcp)
*   @param [in] iSockFd         �׽���  
*   @param [in] pBuf            ����ָ��
*   @param [in] iBufLen         ��Ҫ���͵����ݳ���
*   @return ʵ�ʷ��͵����ݳ��ȣ�-1 ����
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Send(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Sendn(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen, DTK_UINT32 nTimeOut=DTK_INFINITE)
*   @brief �ڳ�ʱ��Χ�ڳ��Զ�η��ͣ�ֱ������ȫ���������
*   @param [in] iSockFd         �׽���  
*   @param [in] pBuf            ����ָ��
*   @param [in] iBufLen         ��Ҫ���͵����ݳ���
*   @param [in] nTimeOut        ��ʱʱ��
*   @return ʵ�ʷ��͵����ݳ��ȣ�-1 ����
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Sendn(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen, DTK_UINT32 nTimeOut=DTK_INFINITE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Recv(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount)
*   @brief ��������(tcp)
*   @param [in] iSockFd         �׽���  
*   @param [in] pBuf            ����ָ��
*   @param [in] iBufCount       ��Ҫ���յ����ݳ���
*   @return ʵ�ʽ��յ����ݳ��ȣ�-1 ����
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Recv(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount);
DTK_DECLARE DTK_INT32 CALLBACK DTK_RecvWithTimeOut(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_UINT32 nTimeOut);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Recvn(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_UINT32 nTimeOut=DTK_INFINITE)
*   @brief �ڳ�ʱ��Χ�ڳ��Զ�ν��գ�ֱ������ȫ���������
*   @param [in] iSockFd         �׽���  
*   @param [in] pBuf            ����ָ��
*   @param [in] iBufCount       ��Ҫ���յ����ݳ���
*   @param [in] nTimeOut        ��ʱʱ��
*   @return ʵ�ʷ��͵����ݳ��ȣ�-1 ����
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Recvn(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_UINT32 nTimeOut=DTK_INFINITE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SendTo(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen, DTK_ADDR_T* pDTKAddr)
*   @brief ��������(udp)
*   @param [in] iSockFd         �׽���  
*   @param [in] pBuf            ����ָ��
*   @param [in] iBufLen         ��Ҫ���͵����ݳ���
*   @param [in] pDTKAddr        ���͵�ַ
*   @return ʵ�ʷ��͵����ݳ��ȣ�-1 ����
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SendTo(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_RecvFrom(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_ADDR_T* pDTKAddr)
*   @brief �������ݣ�udp��
*   @param [in] iSockFd         �׽���  
*   @param [in] pBuf            ����ָ��
*   @param [in] iBufCount       ��Ҫ���յ����ݳ���
*   @param [in] pDTKAddr        ���յ�ַ
*   @return ʵ�ʽ��յ����ݳ��ȣ�-1 ����
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_RecvFrom(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Ioctl(DTK_SOCK_T iSockFd, DTK_INT32 iCmd, DTK_UINT32 *nData)
*   @brief �����׽ӿڵ�ģʽ
*   @param [in] iSockFd         �׽���  
*   @param [in] iCmd            ��������
*   @param [in] nData           �������
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Ioctl(DTK_SOCK_T iSockFd, DTK_INT32 iCmd, DTK_UINT32 *nData);

#endif // __DTK_SOCKET_H__ 