
#ifndef __DTK_NETDB_H__  
#define __DTK_NETDB_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"
#include "DTK_Addr.h"

#define DTK_MAX_ADAPTER_NAME_LEN 256
#define DTK_MAX_ADAPTER_ADDRESS_LEN 8
#define DTK_MAX_DESCRIPTION_LEN 128
#define DTK_MAX_IP_ADDRESS_LEN	16
#define DTK_MAX_NIC_NUM			16
#define DTK_DEF_ADAPTER_ADDRESS_LEN 6


//IP地址结构
struct DTK_IPADDR_STRING
{
	char szIPAddr[DTK_MAX_IP_ADDRESS_LEN];	//IP地址
	char szNetMask[DTK_MAX_IP_ADDRESS_LEN];	//掩码
};

//IP地址列表
struct DTK_IPADDRESS
{
	DTK_IPADDRESS* pNext; //向前指针
	DTK_IPADDR_STRING struIpAddrString;//IP地址信息
};

/******************************************************************************
 * windows 网卡类型 MIB_IF_TYPE_OTHER/ETHERNET/TOKEN_RING/FDDI/PPP/LOOPBACK/SLIP
 * 与windows 的获取网卡信息相比，少了两个东西：
 * 1是WINS Server(Windwos) （windows 网络地址转换）
 * 2是DHCP获得时间及到期时间
 ******************************************************************************/
struct DTK_ADAPTER_INFO
{
	DTK_ADAPTER_INFO* pNext;						//向前指针
	DTK_INT32 iIndex;								//网卡索引号（从0开始）
	char szAdapterName[DTK_MAX_ADAPTER_NAME_LEN+4];	//网卡名称
	char szDescription[DTK_MAX_DESCRIPTION_LEN+4];	//网卡描述
	DTK_UINT32 nMacAddrLen;							//网卡地址长度
	unsigned char szMacAddr[DTK_MAX_ADAPTER_ADDRESS_LEN];	//网卡地址
	DTK_UINT32 nType;								//网卡类型 (POSIX无效)
	DTK_UINT32 nDhcpEnabled;						//是否开启DHCP(POSIX无效)
	DTK_IPADDRESS* pIpAddrList;						//IPv4地址列表
	DTK_IPADDRESS* pGatawayList;					//网关地址列表(POSIX无效)
	DTK_IPADDRESS* pDhcpServerList;					//DHCP服务器列表(POSIX无效)
	DTK_IPADDRESS* pIpAddrv6List;					//IPv6地址列表
};

struct DTK_ADDRESS_INFO
{
	DTK_INT32              iFlags;
	DTK_INT32              iFamily;
	DTK_INT32              iSockType;
	DTK_INT32              iProtocol;
	DTK_ADDR_T             Address;
	char                   *sCanonName;
	DTK_ADDRESS_INFO       *pNext;
};

/** @fn DTK_DECLARE DTK_UINT16 CALLBACK DTK_Htons(DTK_UINT16 nHostShort)
*   @brief 将16位的主机字节序变量转化为网络字节序变量
*   @param [in] nHostShort  主机字节序变量
*   @return 网络字节序变量
*/
DTK_DECLARE DTK_UINT16 CALLBACK DTK_Htons(DTK_UINT16 nHostShort);

/** @fn DTK_DECLARE DTK_UINT16 CALLBACK DTK_Ntohs(DTK_UINT16 nNetShort)
*   @brief 将16位的网络字节序变量转化为主机字节序变量
*   @param [in] nHostShort  网络字节序变量
*   @return 主机字节序变量
*/
DTK_DECLARE DTK_UINT16 CALLBACK DTK_Ntohs(DTK_UINT16 nNetShort);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_Htonl(DTK_UINT32 nHostLong)
*   @brief 将32位的网络字节序变量转化为字节序变量
*   @param [in] nHostShort  主机字节序变量
*   @return 网络字节序变量
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_Htonl(DTK_UINT32 nHostLong);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_Ntohl(DTK_UINT32 nNetLong)
*   @brief 将16位的网络字节序变量转化为主机字节序变量
*   @param [in] nHostShort  网络字节序变量
*   @return 主机字节序变量
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_Ntohl(DTK_UINT32 nNetLong);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetHostName(char* pName, DTK_INT32 iNameLen)
*   @brief 获取主机名
*   @param [in] pName       主机名内存地址
*   @param [in] iNameLen    主机名内存大小
*   @return 0成功否则失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetHostName(char* pName, DTK_INT32 iNameLen);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetHostName(char* pName, DTK_INT32 iNameLen)
*   @brief 通过主机名获取获取主机地址信息
*   @param [in] pName       主机名
*   @param [in] iNameLen    主机名长度
*   @return 主机地址信息,NULL表示失败
*/
DTK_DECLARE hostent* CALLBACK DTK_GetAddressByName(const char* pName);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetMacAddr(const char* pLocalIP, unsigned char* pMacAddr, DTK_INT32* iMacAddrlen);
*   @brief 根据IP地址获取网卡物理地址
*   @param [in] pLocalIP        IP地址
*   @param [in] pMacAddr        物理地址内存
*   @param [in] iMacAddrlen     物理地址内存长度
*   @return 0成功否则失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetMacAddr(const char* pLocalIP, unsigned char* pMacAddr, DTK_INT32* iMacAddrlen);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAdapterInfo(DTK_ADAPTER_INFO** pAdaterInfo)
*   @brief 获取网卡信息
*   @param [in] pAdaterInfo     网卡信息
*   @return 成功返回后，如果*pAdaterInfo不为空，需要用DTK_FreeAdapterInfo释放
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAdapterInfo(DTK_ADAPTER_INFO** pAdaterInfo);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_FreeAdapterInfo(DTK_ADAPTER_INFO* pAdaterInfo)
*   @brief 释放网卡信息内存
*   @param [in] pAdaterInfo     网卡信息
*   @return 0成功否则失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_FreeAdapterInfo(DTK_ADAPTER_INFO* pAdaterInfo);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddressInfo(const char *sNodeName, const char *sServerName, DTK_ADDRESS_INFO *Hints, DTK_ADDRESS_INFO **pRes)
*   @brief 封装getaddrinfo函数，主要用作域名解析
*   @param [in] sNodeName       节点名称，可以是域名，也可以是ip地址，字符串形式，需要有0结束符
*   @param [in] sServerName     服务名称，传入具体的服务名称或者端口号，字符串形式，需要有0结束符
*   @param [in] Hints           DTK_ADDRESS_INFO结构指针，传入调用者的具体要求，如ip类型，socket类型等
*   @param [in] pRes            返回解析出的具体地址信息，指针内存由内部分配，需要调用DTK_FreeAddressInfo释放
*   @return 0成功否则失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddressInfo(const char *sNodeName, const char *sServerName, DTK_ADDRESS_INFO *Hints, DTK_ADDRESS_INFO **pRes);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_FreeAddressInfo(DTK_ADDRESS_INFO *pAddressInfo)
*   @brief 释放DTK_GetAddressInfo返回的结构中所包含的内存
*   @param [in] pAddressInfo    DTK_ADDRESS_INFO结构指针，该指针由DTK_GetAddressInfo返回
*   @param [in] 
*   @param [out] 
*   @return 
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_FreeAddressInfo(DTK_ADDRESS_INFO *pAddressInfo);

#endif // __DTK_NETDB_H__ 