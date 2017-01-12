
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


//IP��ַ�ṹ
struct DTK_IPADDR_STRING
{
	char szIPAddr[DTK_MAX_IP_ADDRESS_LEN];	//IP��ַ
	char szNetMask[DTK_MAX_IP_ADDRESS_LEN];	//����
};

//IP��ַ�б�
struct DTK_IPADDRESS
{
	DTK_IPADDRESS* pNext; //��ǰָ��
	DTK_IPADDR_STRING struIpAddrString;//IP��ַ��Ϣ
};

/******************************************************************************
 * windows �������� MIB_IF_TYPE_OTHER/ETHERNET/TOKEN_RING/FDDI/PPP/LOOPBACK/SLIP
 * ��windows �Ļ�ȡ������Ϣ��ȣ���������������
 * 1��WINS Server(Windwos) ��windows �����ַת����
 * 2��DHCP���ʱ�估����ʱ��
 ******************************************************************************/
struct DTK_ADAPTER_INFO
{
	DTK_ADAPTER_INFO* pNext;						//��ǰָ��
	DTK_INT32 iIndex;								//���������ţ���0��ʼ��
	char szAdapterName[DTK_MAX_ADAPTER_NAME_LEN+4];	//��������
	char szDescription[DTK_MAX_DESCRIPTION_LEN+4];	//��������
	DTK_UINT32 nMacAddrLen;							//������ַ����
	unsigned char szMacAddr[DTK_MAX_ADAPTER_ADDRESS_LEN];	//������ַ
	DTK_UINT32 nType;								//�������� (POSIX��Ч)
	DTK_UINT32 nDhcpEnabled;						//�Ƿ���DHCP(POSIX��Ч)
	DTK_IPADDRESS* pIpAddrList;						//IPv4��ַ�б�
	DTK_IPADDRESS* pGatawayList;					//���ص�ַ�б�(POSIX��Ч)
	DTK_IPADDRESS* pDhcpServerList;					//DHCP�������б�(POSIX��Ч)
	DTK_IPADDRESS* pIpAddrv6List;					//IPv6��ַ�б�
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
*   @brief ��16λ�������ֽ������ת��Ϊ�����ֽ������
*   @param [in] nHostShort  �����ֽ������
*   @return �����ֽ������
*/
DTK_DECLARE DTK_UINT16 CALLBACK DTK_Htons(DTK_UINT16 nHostShort);

/** @fn DTK_DECLARE DTK_UINT16 CALLBACK DTK_Ntohs(DTK_UINT16 nNetShort)
*   @brief ��16λ�������ֽ������ת��Ϊ�����ֽ������
*   @param [in] nHostShort  �����ֽ������
*   @return �����ֽ������
*/
DTK_DECLARE DTK_UINT16 CALLBACK DTK_Ntohs(DTK_UINT16 nNetShort);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_Htonl(DTK_UINT32 nHostLong)
*   @brief ��32λ�������ֽ������ת��Ϊ�ֽ������
*   @param [in] nHostShort  �����ֽ������
*   @return �����ֽ������
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_Htonl(DTK_UINT32 nHostLong);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_Ntohl(DTK_UINT32 nNetLong)
*   @brief ��16λ�������ֽ������ת��Ϊ�����ֽ������
*   @param [in] nHostShort  �����ֽ������
*   @return �����ֽ������
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_Ntohl(DTK_UINT32 nNetLong);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetHostName(char* pName, DTK_INT32 iNameLen)
*   @brief ��ȡ������
*   @param [in] pName       �������ڴ��ַ
*   @param [in] iNameLen    �������ڴ��С
*   @return 0�ɹ�����ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetHostName(char* pName, DTK_INT32 iNameLen);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetHostName(char* pName, DTK_INT32 iNameLen)
*   @brief ͨ����������ȡ��ȡ������ַ��Ϣ
*   @param [in] pName       ������
*   @param [in] iNameLen    ����������
*   @return ������ַ��Ϣ,NULL��ʾʧ��
*/
DTK_DECLARE hostent* CALLBACK DTK_GetAddressByName(const char* pName);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetMacAddr(const char* pLocalIP, unsigned char* pMacAddr, DTK_INT32* iMacAddrlen);
*   @brief ����IP��ַ��ȡ���������ַ
*   @param [in] pLocalIP        IP��ַ
*   @param [in] pMacAddr        �����ַ�ڴ�
*   @param [in] iMacAddrlen     �����ַ�ڴ泤��
*   @return 0�ɹ�����ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetMacAddr(const char* pLocalIP, unsigned char* pMacAddr, DTK_INT32* iMacAddrlen);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAdapterInfo(DTK_ADAPTER_INFO** pAdaterInfo)
*   @brief ��ȡ������Ϣ
*   @param [in] pAdaterInfo     ������Ϣ
*   @return �ɹ����غ����*pAdaterInfo��Ϊ�գ���Ҫ��DTK_FreeAdapterInfo�ͷ�
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAdapterInfo(DTK_ADAPTER_INFO** pAdaterInfo);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_FreeAdapterInfo(DTK_ADAPTER_INFO* pAdaterInfo)
*   @brief �ͷ�������Ϣ�ڴ�
*   @param [in] pAdaterInfo     ������Ϣ
*   @return 0�ɹ�����ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_FreeAdapterInfo(DTK_ADAPTER_INFO* pAdaterInfo);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddressInfo(const char *sNodeName, const char *sServerName, DTK_ADDRESS_INFO *Hints, DTK_ADDRESS_INFO **pRes)
*   @brief ��װgetaddrinfo��������Ҫ������������
*   @param [in] sNodeName       �ڵ����ƣ�������������Ҳ������ip��ַ���ַ�����ʽ����Ҫ��0������
*   @param [in] sServerName     �������ƣ��������ķ������ƻ��߶˿ںţ��ַ�����ʽ����Ҫ��0������
*   @param [in] Hints           DTK_ADDRESS_INFO�ṹָ�룬��������ߵľ���Ҫ����ip���ͣ�socket���͵�
*   @param [in] pRes            ���ؽ������ľ����ַ��Ϣ��ָ���ڴ����ڲ����䣬��Ҫ����DTK_FreeAddressInfo�ͷ�
*   @return 0�ɹ�����ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddressInfo(const char *sNodeName, const char *sServerName, DTK_ADDRESS_INFO *Hints, DTK_ADDRESS_INFO **pRes);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_FreeAddressInfo(DTK_ADDRESS_INFO *pAddressInfo)
*   @brief �ͷ�DTK_GetAddressInfo���صĽṹ�����������ڴ�
*   @param [in] pAddressInfo    DTK_ADDRESS_INFO�ṹָ�룬��ָ����DTK_GetAddressInfo����
*   @param [in] 
*   @param [out] 
*   @return 
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_FreeAddressInfo(DTK_ADDRESS_INFO *pAddressInfo);

#endif // __DTK_NETDB_H__ 