
#ifndef __DTK_ADDR_H__  
#define __DTK_ADDR_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

#if defined(OS_WINDOWS)
    #pragma comment (lib,"ws2_32")

	typedef SOCKET DTK_SOCK_T;
	#if (_MSC_VER < 1310)
		#ifndef s6_addr
			struct in6_addr {
				union {
					u_char Byte[16];
					u_short Word[8];
				} u;
			};
			#define in_addr6 in6_addr
			/*
			** Defines to match RFC 2553.
			*/
			#define _S6_un     u
			#define _S6_u8     Byte
			#define s6_addr    _S6_un._S6_u8
			/*
			** Defines for our implementation.
			*/
			#define s6_bytes   u.Byte
			#define s6_words   u.Word
		#endif

		typedef struct {
			short   sin6_family;        /* AF_INET6 */
			u_short sin6_port;          /* Transport level port number */
			u_long  sin6_flowinfo;      /* IPv6 flow information */
			struct in6_addr sin6_addr;  /* IPv6 address */
			u_long sin6_scope_id;       /* set of interfaces for a scope */
		}sockaddr_in6_dtk;
	#else
		typedef sockaddr_in6 sockaddr_in6_dtk;
	#endif
#elif defined(OS_POSIX)
	typedef int DTK_SOCK_T;
	typedef sockaddr_in6 sockaddr_in6_dtk;
    #ifndef SOCKADDR
    typedef sockaddr SOCKADDR;
    typedef sockaddr* PSOCKADDR;
    #endif
#else
	#error OS Not Implement Yet.
#endif


typedef struct DTK_ADDR_T
{
	union
	{
		sockaddr_in sin4;       //IPV4 地址
#if defined(OS_SUPPORT_IPV6)
		sockaddr_in6_dtk sin6;  //IPV6 地址
#endif
	}SA;
}DTK_ADDR_T;

typedef struct DTK_ADDR_EXP_T
{
	DTK_INT32 iAf;				//地址族AF_INET或AF_INET6
	union
	{
		DTK_UINT32 nAddr4;		//IPV4整形地址 (网络字节序)
		DTK_UINT8 nAddr6[16];	//IPV6整形地址 (网络字节序)
	}ADDR;
	DTK_UINT16 nPort;			//端口号(主机字节序)
	DTK_UINT16 nReserved;		//保留
}DTK_ADDR_EXP_T;

#define DTK_AF_INET		AF_INET
#if defined(OS_SUPPORT_IPV6)
#define DTK_AF_INET6	AF_INET6
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddrByString(DTK_INT32 iAf, const char* pAddr, DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr)
*   @brief 通过地址族类型,字符串格式的地址及端口号组建DTK网络地址结构
*   @param [in] iAf     地址族类型（AF_INET 或 AF_INET6）
*   @param [in] pAddr   字符串格式的地址
*   @param [in] nPort   网络端口号(主机字节序)
*   @param [out] pAddr  DTK网络地址结构
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddrByString(DTK_INT32 iAf, const char* pAddr, DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr4ByInt(DTK_UINT32 nAddr, DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr)
*   @brief 通过整形地址及端口号组建DTK网络地址结构（IPV4）
*   @param [in] nAddr       整形网络地址
*   @param [in] nPort       网络端口号（主机字节序）
*   @param [out] pDTKAddr   DTK网络地址结构
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr4ByInt(DTK_UINT32 nAddr, DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr6ByInt(DTK_UINT8 nAddr[16], DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr)
*   @brief 通过整形地址及端口号组建DTK网络地址结构（IPV6）
*   @param [in] nAddr       整形网络地址
*   @param [in] nPort       网络端口号（主机字节序）
*   @param [out] pDTKAddr   DTK网络地址结构
*   @return 0 成功, -1 失败
*/
#if defined(OS_SUPPORT_IPV6)
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr6ByInt(DTK_UINT8 nAddr[16], DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr);
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr(DTK_VOIDPTR pSockAddr, DTK_INT32 iSockAddrLen, DTK_ADDR_T* pDTKAddr)
*   @brief 通过SOCKADDR结构组建DTK网络地址结构
*   @param [in] pSockAddr       SOCKADDR地址结构指针
*   @param [in] iSockAddrLen    SOCKADDR地址结构长度
*   @param [out] pDTKAddr       DTK网络地址结构指针
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr(DTK_VOIDPTR pSockAddr, DTK_INT32 iSockAddrLen, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetAddrPort(DTK_ADDR_T* pDTKAddr, DTK_UINT16 nPort)
*   @brief 设置DTK网络地址结构端口号
*   @param [in] pDTKAddr    DTK网络地址结构的
*   @param [in] nPort       网络端口号(主机字节序)
*   @return 0成功，-1失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetAddrPort(DTK_ADDR_T* pDTKAddr, DTK_UINT16 nPort);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrType(DTK_ADDR_T* pDTKAddr)
*   @brief 通过DTK网络地址结构获取地址族（AF_INET或AF_INET6）
*   @param [in] pDTKAddr    DTK网络地址结构指针
*   @return -1失败，否则返回AF_INET或AF_INET6
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrType(DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE const char* CALLBACK DTK_GetAddrString(DTK_ADDR_T* pDTKAddr)
*   @brief 通过DTK网络地址结构获取字符串格式地址
*   @param [in] pDTKAddr    DTK网络地址结构
*   @return 0 成功, -1 失败
*/
DTK_DECLARE const char* CALLBACK DTK_GetAddrString(DTK_ADDR_T* pDTKAddr);
DTK_DECLARE const char* CALLBACK DTK_GetAddrStringEx(DTK_ADDR_T* pDTKAddr, char* pAddrBuf, size_t nBuflen);

/** @fn DTK_DECLARE DTK_UINT16 CALLBACK DTK_GetAddrPort(DTK_ADDR_T* pDTKAddr)
*   @brief 获取地址结构的网络端口号
*   @param [in] pDTKAddr    网络端口号(主机字节序)
*   @return 端口号
*/
DTK_DECLARE DTK_UINT16 CALLBACK DTK_GetAddrPort(DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddr4Int(DTK_ADDR_T* pDTKAddr, DTK_UINT32* nAddr)
*   @brief 获取地址结构IPV4整形表示的网络地址
*   @param [in] pDTKAddr    DTK格式网络地址结构
*   @param [out] nAddr      整形网络地址（网络字节序）
*   @return 0成功，-1失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddr4Int(DTK_ADDR_T* pDTKAddr, DTK_UINT32* nAddr);

#if defined (OS_SUPPORT_IPV6)
/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddr6Int(DTK_ADDR_T* pDTKAddr, DTK_UINT8 nAddr[16])
*   @brief 获取地址结构IPV6整形表示的网络地址
*   @param [in] pDTKAddr    DTK格式网络地址结构
*   @param [out] nAddr      整形网络地址（网络字节序）
*   @return 0成功，-1失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddr6Int(DTK_ADDR_T* pDTKAddr, DTK_UINT8 nAddr[16]);
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrInfo(DTK_ADDR_T* pDTKAddr, DTK_ADDR_EXP_T* pDTKExp)
*   @brief 获取网络地址结构信息
*   @param [in] pDTKAddr    DTK格式网络地址结构
*   @param [out] pDTKExp    网络地址结构
*   @return 0成功，-1失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrInfo(DTK_ADDR_T* pDTKAddr, DTK_ADDR_EXP_T* pDTKExp);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrBySockFd(DTK_SOCK_T iSockFd,DTK_ADDR_T* pDTKLocal, DTK_ADDR_T* pRemote)
*   @brief 通过socket套接字获取本地机远端地网络址结构
*   @param [in] iSockFd     套接字
*   @param [in] pDTKLocal   DTK本地网络地址
*   @param [out] pRemote    DTK网络远端地址
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrBySockFd(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKLocal, DTK_ADDR_T* pRemote);


DTK_DECLARE DTK_INT32 CALLBACK DTK_InetPton(const char* pSrc, DTK_VOIDPTR pDst);

DTK_DECLARE const char* CALLBACK DTK_InetNtop(DTK_INT32 iAf, const unsigned char* pSrc, char* pDst, int iCnt);

#endif // __DTK_ADDR_H__ 