
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
*   @brief 创建套接字
*   @param [in] iAf         地址族类型：AF_INET(IPv4协议)、AF_INET6(IPv6协议)
*   @param [in] iType       套接字类型：SOCK_STREAM(字节流)、SOCK_DGRAM(数据报)、SOCK_RAW(原始套接口)
*   @param [out] iProto     相应的传输协议，使用默认值0
*   @return 成功返回套接字
*/
DTK_DECLARE DTK_SOCK_T CALLBACK DTK_CreateSocket(DTK_INT32 iAf, DTK_INT32 iType, DTK_INT32 iProto);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_CloseSocket(DTK_SOCK_T iSockFd, DTK_BOOL bForce = DTK_FALSE)
*   @brief 关闭套接字
*   @param [in] iSockFd     套接字
*   @param [in] bForce      是否强制关闭
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_CloseSocket(DTK_SOCK_T iSockFd, DTK_BOOL bForce = DTK_FALSE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ShutDown(DTK_SOCK_T iSockFd, DTK_INT32 iHow)
*   @brief 禁止在一个套接口上进行数据的接收与发送
*   @param [in] iSockFd     套接字
*   @param [in] iHow        禁止哪些操作(DTK_SHUT_RD/DTK_SHUT_WR/DTK_SHUT_RDWR) 
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ShutDown(DTK_SOCK_T iSockFd, DTK_INT32 iHow);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Bind(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr)
*   @brief 将一组固定的地址绑定到套接字上
*   @param [in] iSockFd     套接字
*   @param [in] pDTKAddr    地址
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Bind(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Listen(DTK_SOCK_T iSockFd, DTK_INT32 iBackLog)
*   @brief 设置等待连接队列的最大长度
*   @param [in] iSockFd     套接字
*   @param [in] iBackLog    等待连接队列的最大长度，posix最大值128
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Listen(DTK_SOCK_T iSockFd, DTK_INT32 iBackLog);

/** @fn DTK_DECLARE DTK_SOCK_T CALLBACK DTK_Accept(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr = NULL, DTK_UINT32 nTimeOut = DTK_INFINITE)
*   @brief 接收远程连接
*   @param [in] iSockFd     监听套接字
*   @param [in] pDTKAddr    客户端地址
*   @param [in] nTimeOut    超时时间，毫秒
*   @return 成功返回客户端socket
*/
DTK_DECLARE DTK_SOCK_T CALLBACK DTK_Accept(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr = NULL, DTK_UINT32 nTimeOut = DTK_INFINITE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ConnectWithTimeOut(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr, DTK_UINT32 nTimeOut=DTK_INFINITE)
*   @brief 连接服务端，带连接超时
*   @param [in] iSockFd     监听套接字
*   @param [in] pDTKAddr    服务端地址
*   @param [in] nTimeOut    超时时间，毫秒
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ConnectWithTimeOut(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKAddr, DTK_UINT32 nTimeOut=DTK_INFINITE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetNonBlock(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo)
*   @brief 设置套接字为阻塞类型/非阻塞类型
*   @param [in] iSockFd     套接字
*   @param [in] bYesNo      true 非阻塞，false 阻塞
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetNonBlock(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetBuffSize(DTK_SOCK_T iSockFd, DTK_INT32 iSndBuffSize, DTK_INT32 iRcvBuffSize)
*   @brief 设置发送和接收缓冲区大小
*   @param [in] iSockFd         套接字
*   @param [in] iSndBuffSize    发送缓存区大小
*   @param [in] iRcvBuffSize    接收缓存区大小
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetBuffSize(DTK_SOCK_T iSockFd, DTK_INT32 iSndBuffSize, DTK_INT32 iRcvBuffSize);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetBuffSize(DTK_SOCK_T iSockFd, DTK_INT32* iSndBuffSize, DTK_INT32* iRcvBuffSize)
*   @brief 获取发送和接收缓冲区大小
*   @param [in] iSockFd         套接字
*   @param [in] iSndBuffSize    发送缓存区大小
*   @param [in] iRcvBuffSize    接收缓存区大小
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetBuffSize(DTK_SOCK_T iSockFd, DTK_INT32* iSndBuffSize, DTK_INT32* iRcvBuffSize);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetReuseAddr(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo)
*   @brief 允许套接口和一个已在使用中的地址捆绑
*   @param [in] iSockFd         套接字
*   @param [in] bYesNo          enable/disable reuse address
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetReuseAddr(DTK_SOCK_T iSockFd, DTK_BOOL bYesNo);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTimeOut(DTK_SOCK_T iSockFd, DTK_INT32 iSndTimeO, DTK_INT32 iRcvTimeO)
*   @brief 设置发送和接收超时
*   @param [in] iSockFd         套接字
*   @param [in] iSndTimeO       发送超时时间
*   @param [in] iRcvTimeO       接收超时时间
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTimeOut(DTK_SOCK_T iSockFd, DTK_INT32 iSndTimeO, DTK_INT32 iRcvTimeO);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_BroadCast(DTK_SOCK_T iSockFd)
*   @brief 允许套接口传送广播信息
*   @param [in] iSockFd         套接字
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_BroadCast(DTK_SOCK_T iSockFd);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTTL(DTK_SOCK_T iSockFd, DTK_INT32 iTTL)
*   @brief 设置报文生存时间
*   @param [in] iSockFd         套接字
*   @param [in] iTTL            时间值        
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTTL(DTK_SOCK_T iSockFd, DTK_INT32 iTTL);

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTTL_V6(DTK_SOCK_T iSockFd, DTK_INT32 iTTL);

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetMultiCastTTL(DTK_SOCK_T iSockFd, DTK_INT32 iTTL);

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetMultiCastTTL_V6(DTK_SOCK_T iSockFd, DTK_INT32 iTTL);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTTL(DTK_SOCK_T iSockFd, DTK_INT32* iTTL)
*   @brief 获取报文生存时间
*   @param [in] iSockFd         套接字
*   @param [in] iTTL            时间值 
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTTL(DTK_SOCK_T iSockFd, DTK_INT32* iTTL);

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTTL_V6(DTK_SOCK_T iSockFd, DTK_INT32* iTTL);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTOS(DTK_SOCK_T iSockFd, DTK_INT32 iTOS)
*   @brief 设置服务类型
*   @param [in] iSockFd         套接字
*   @param [in] iTOS            服务类型
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTOS(DTK_SOCK_T iSockFd, DTK_INT32 iTOS);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTOS(DTK_SOCK_T iSockFd, DTK_INT32 iTOS)
*   @brief 获取服务类型
*   @param [in] iSockFd         套接字
*   @param [in] iTOS            服务类型
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetTOS(DTK_SOCK_T iSockFd, DTK_INT32* iTOS);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_LingerOff(DTK_SOCK_T iSockFd)
*   @brief disable延迟关闭连接(tcp)
*   @param [in] iSockFd         套接字
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_LingerOff(DTK_SOCK_T iSockFd);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_LingerOn(DTK_SOCK_T iSockFd,DTK_UINT16 nTimeOutSec)
*   @brief enable延迟关闭连接tcp（规避closesocket导致send未发送完全的数据丢失）
*   @param [in] iSockFd         套接字
*   @param [in] nTimeOutSec     延迟关闭时间，秒
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_LingerOn(DTK_SOCK_T iSockFd,DTK_UINT16 nTimeOutSec);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetNoDelay(DTK_SOCK_T iSockFd,DTK_BOOL bYesNo)
*   @brief enable/disabel nagle算法（Nagle算法通过将未确认的数据存入缓冲区直到蓄足一个包一起发送的方法，来减少主机发送的零碎小数据包的数目）
*   @param [in] iSockFd         套接字
*   @param [in] bYesNo          enable/disabel
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetNoDelay(DTK_SOCK_T iSockFd,DTK_BOOL bYesNo);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_JoinMultiCastGroup(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pMcastGroupAddr)
*   @brief 在指定接口上加入组播组
*   @param [in] iSockFd         套接字
*   @param [in] pLocal          本地地址
*   @param [in] pMcastGroupAddr 组播地址
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_JoinMultiCastGroup(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pMcastGroupAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_LeaveMultiCastGroup(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pMcastGroupAddr)
*   @brief 退出组播组
*   @param [in] iSockFd         套接字
*   @param [in] pLocal          本地地址
*   @param [in] pMcastGroupAddr 组播地址
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_LeaveMultiCastGroup(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pMcastGroupAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Send(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen)
*   @brief 发送数据(tcp)
*   @param [in] iSockFd         套接字  
*   @param [in] pBuf            数据指针
*   @param [in] iBufLen         需要发送的数据长度
*   @return 实际发送的数据长度，-1 出错
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Send(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Sendn(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen, DTK_UINT32 nTimeOut=DTK_INFINITE)
*   @brief 在超时范围内尝试多次发送，直至数据全部发送完成
*   @param [in] iSockFd         套接字  
*   @param [in] pBuf            数据指针
*   @param [in] iBufLen         需要发送的数据长度
*   @param [in] nTimeOut        超时时间
*   @return 实际发送的数据长度，-1 出错
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Sendn(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen, DTK_UINT32 nTimeOut=DTK_INFINITE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Recv(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount)
*   @brief 接收数据(tcp)
*   @param [in] iSockFd         套接字  
*   @param [in] pBuf            数据指针
*   @param [in] iBufCount       需要接收的数据长度
*   @return 实际接收的数据长度，-1 出错
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Recv(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount);
DTK_DECLARE DTK_INT32 CALLBACK DTK_RecvWithTimeOut(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_UINT32 nTimeOut);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Recvn(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_UINT32 nTimeOut=DTK_INFINITE)
*   @brief 在超时范围内尝试多次接收，直至数据全部接收完成
*   @param [in] iSockFd         套接字  
*   @param [in] pBuf            数据指针
*   @param [in] iBufCount       需要接收的数据长度
*   @param [in] nTimeOut        超时时间
*   @return 实际发送的数据长度，-1 出错
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Recvn(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_UINT32 nTimeOut=DTK_INFINITE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SendTo(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen, DTK_ADDR_T* pDTKAddr)
*   @brief 发送数据(udp)
*   @param [in] iSockFd         套接字  
*   @param [in] pBuf            数据指针
*   @param [in] iBufLen         需要发送的数据长度
*   @param [in] pDTKAddr        发送地址
*   @return 实际发送的数据长度，-1 出错
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SendTo(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufLen, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_RecvFrom(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_ADDR_T* pDTKAddr)
*   @brief 接收数据（udp）
*   @param [in] iSockFd         套接字  
*   @param [in] pBuf            数据指针
*   @param [in] iBufCount       需要接收的数据长度
*   @param [in] pDTKAddr        接收地址
*   @return 实际接收的数据长度，-1 出错
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_RecvFrom(DTK_SOCK_T iSockFd, DTK_VOIDPTR pBuf, DTK_INT32 iBufCount, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Ioctl(DTK_SOCK_T iSockFd, DTK_INT32 iCmd, DTK_UINT32 *nData)
*   @brief 控制套接口的模式
*   @param [in] iSockFd         套接字  
*   @param [in] iCmd            控制命令
*   @param [in] nData           命令参数
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Ioctl(DTK_SOCK_T iSockFd, DTK_INT32 iCmd, DTK_UINT32 *nData);

#endif // __DTK_SOCKET_H__ 