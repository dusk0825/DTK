
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
		sockaddr_in sin4;       //IPV4 ��ַ
#if defined(OS_SUPPORT_IPV6)
		sockaddr_in6_dtk sin6;  //IPV6 ��ַ
#endif
	}SA;
}DTK_ADDR_T;

typedef struct DTK_ADDR_EXP_T
{
	DTK_INT32 iAf;				//��ַ��AF_INET��AF_INET6
	union
	{
		DTK_UINT32 nAddr4;		//IPV4���ε�ַ (�����ֽ���)
		DTK_UINT8 nAddr6[16];	//IPV6���ε�ַ (�����ֽ���)
	}ADDR;
	DTK_UINT16 nPort;			//�˿ں�(�����ֽ���)
	DTK_UINT16 nReserved;		//����
}DTK_ADDR_EXP_T;

#define DTK_AF_INET		AF_INET
#if defined(OS_SUPPORT_IPV6)
#define DTK_AF_INET6	AF_INET6
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddrByString(DTK_INT32 iAf, const char* pAddr, DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr)
*   @brief ͨ����ַ������,�ַ�����ʽ�ĵ�ַ���˿ں��齨DTK�����ַ�ṹ
*   @param [in] iAf     ��ַ�����ͣ�AF_INET �� AF_INET6��
*   @param [in] pAddr   �ַ�����ʽ�ĵ�ַ
*   @param [in] nPort   ����˿ں�(�����ֽ���)
*   @param [out] pAddr  DTK�����ַ�ṹ
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddrByString(DTK_INT32 iAf, const char* pAddr, DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr4ByInt(DTK_UINT32 nAddr, DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr)
*   @brief ͨ�����ε�ַ���˿ں��齨DTK�����ַ�ṹ��IPV4��
*   @param [in] nAddr       ���������ַ
*   @param [in] nPort       ����˿ںţ������ֽ���
*   @param [out] pDTKAddr   DTK�����ַ�ṹ
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr4ByInt(DTK_UINT32 nAddr, DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr6ByInt(DTK_UINT8 nAddr[16], DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr)
*   @brief ͨ�����ε�ַ���˿ں��齨DTK�����ַ�ṹ��IPV6��
*   @param [in] nAddr       ���������ַ
*   @param [in] nPort       ����˿ںţ������ֽ���
*   @param [out] pDTKAddr   DTK�����ַ�ṹ
*   @return 0 �ɹ�, -1 ʧ��
*/
#if defined(OS_SUPPORT_IPV6)
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr6ByInt(DTK_UINT8 nAddr[16], DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr);
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr(DTK_VOIDPTR pSockAddr, DTK_INT32 iSockAddrLen, DTK_ADDR_T* pDTKAddr)
*   @brief ͨ��SOCKADDR�ṹ�齨DTK�����ַ�ṹ
*   @param [in] pSockAddr       SOCKADDR��ַ�ṹָ��
*   @param [in] iSockAddrLen    SOCKADDR��ַ�ṹ����
*   @param [out] pDTKAddr       DTK�����ַ�ṹָ��
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr(DTK_VOIDPTR pSockAddr, DTK_INT32 iSockAddrLen, DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetAddrPort(DTK_ADDR_T* pDTKAddr, DTK_UINT16 nPort)
*   @brief ����DTK�����ַ�ṹ�˿ں�
*   @param [in] pDTKAddr    DTK�����ַ�ṹ��
*   @param [in] nPort       ����˿ں�(�����ֽ���)
*   @return 0�ɹ���-1ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetAddrPort(DTK_ADDR_T* pDTKAddr, DTK_UINT16 nPort);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrType(DTK_ADDR_T* pDTKAddr)
*   @brief ͨ��DTK�����ַ�ṹ��ȡ��ַ�壨AF_INET��AF_INET6��
*   @param [in] pDTKAddr    DTK�����ַ�ṹָ��
*   @return -1ʧ�ܣ����򷵻�AF_INET��AF_INET6
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrType(DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE const char* CALLBACK DTK_GetAddrString(DTK_ADDR_T* pDTKAddr)
*   @brief ͨ��DTK�����ַ�ṹ��ȡ�ַ�����ʽ��ַ
*   @param [in] pDTKAddr    DTK�����ַ�ṹ
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE const char* CALLBACK DTK_GetAddrString(DTK_ADDR_T* pDTKAddr);
DTK_DECLARE const char* CALLBACK DTK_GetAddrStringEx(DTK_ADDR_T* pDTKAddr, char* pAddrBuf, size_t nBuflen);

/** @fn DTK_DECLARE DTK_UINT16 CALLBACK DTK_GetAddrPort(DTK_ADDR_T* pDTKAddr)
*   @brief ��ȡ��ַ�ṹ������˿ں�
*   @param [in] pDTKAddr    ����˿ں�(�����ֽ���)
*   @return �˿ں�
*/
DTK_DECLARE DTK_UINT16 CALLBACK DTK_GetAddrPort(DTK_ADDR_T* pDTKAddr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddr4Int(DTK_ADDR_T* pDTKAddr, DTK_UINT32* nAddr)
*   @brief ��ȡ��ַ�ṹIPV4���α�ʾ�������ַ
*   @param [in] pDTKAddr    DTK��ʽ�����ַ�ṹ
*   @param [out] nAddr      ���������ַ�������ֽ���
*   @return 0�ɹ���-1ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddr4Int(DTK_ADDR_T* pDTKAddr, DTK_UINT32* nAddr);

#if defined (OS_SUPPORT_IPV6)
/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddr6Int(DTK_ADDR_T* pDTKAddr, DTK_UINT8 nAddr[16])
*   @brief ��ȡ��ַ�ṹIPV6���α�ʾ�������ַ
*   @param [in] pDTKAddr    DTK��ʽ�����ַ�ṹ
*   @param [out] nAddr      ���������ַ�������ֽ���
*   @return 0�ɹ���-1ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddr6Int(DTK_ADDR_T* pDTKAddr, DTK_UINT8 nAddr[16]);
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrInfo(DTK_ADDR_T* pDTKAddr, DTK_ADDR_EXP_T* pDTKExp)
*   @brief ��ȡ�����ַ�ṹ��Ϣ
*   @param [in] pDTKAddr    DTK��ʽ�����ַ�ṹ
*   @param [out] pDTKExp    �����ַ�ṹ
*   @return 0�ɹ���-1ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrInfo(DTK_ADDR_T* pDTKAddr, DTK_ADDR_EXP_T* pDTKExp);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrBySockFd(DTK_SOCK_T iSockFd,DTK_ADDR_T* pDTKLocal, DTK_ADDR_T* pRemote)
*   @brief ͨ��socket�׽��ֻ�ȡ���ػ�Զ�˵�����ַ�ṹ
*   @param [in] iSockFd     �׽���
*   @param [in] pDTKLocal   DTK���������ַ
*   @param [out] pRemote    DTK����Զ�˵�ַ
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrBySockFd(DTK_SOCK_T iSockFd, DTK_ADDR_T* pDTKLocal, DTK_ADDR_T* pRemote);


DTK_DECLARE DTK_INT32 CALLBACK DTK_InetPton(const char* pSrc, DTK_VOIDPTR pDst);

DTK_DECLARE const char* CALLBACK DTK_InetNtop(DTK_INT32 iAf, const unsigned char* pSrc, char* pDst, int iCnt);

#endif // __DTK_ADDR_H__ 