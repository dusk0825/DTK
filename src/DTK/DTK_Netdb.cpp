
#include "DTK_Netdb.h"
#include "DTK_Utils.h"
#include "DTK_String.h"
#include "DTK_Dtk.h"
#include <new>
using namespace std;

#if defined (OS_WINDOWS)
    #include <Iphlpapi.h>
#elif defined (OS_POSIX)
    #include "DTK_Socket.h"
    #include "DTK_Addr.h"
	#include <net/if_arp.h>
	#include <asm/types.h>
	#include <linux/netlink.h>
	#include <linux/rtnetlink.h>
#endif


#if defined (OS_WINDOWS)

	typedef DWORD (CALLBACK * pfGetAdapterInfo)(PIP_ADAPTER_INFO, PULONG);
	typedef ULONG (CALLBACK * pfGetAdapterAddr)(ULONG, ULONG, PVOID, PIP_ADAPTER_ADDRESSES, PULONG);

	extern pfGetAdapterInfo g_pInfoFunc;
	extern pfGetAdapterAddr g_pAddrFunc;
	extern HMODULE g_hIphlpapiDll;

    //获取网卡信息(WindowsDTK内部使用)
    static DTK_INT32 GetAdapterInfo_Inter_Windows(DTK_ADAPTER_INFO** pAdaterInfo);
    //获取网卡链表
    static DTK_INT32 GetAdapterList_Inter_Windows(IP_ADAPTER_INFO** pIpAdapterInfo, IP_ADAPTER_ADDRESSES** pAdapterAddr);
    //获取特定网卡的IP地址链表
    static DTK_INT32 GetIpList_Inter_Windows(IP_ADDR_STRING& struIpAddrString, DTK_IPADDRESS** pAddress);
	//获取特定网卡的IPv6地址
	static DTK_INT32 GetIpv6List_Inter_Windows(BYTE *byMac, IP_ADAPTER_ADDRESSES* pIpAdapterAddr, DTK_IPADDRESS** pAddress);
#elif defined (OS_POSIX)
    //获取网卡信息(Linux DTK内部使用)
    static DTK_INT32 GetAdapterInfo_Inter_Posix(DTK_ADAPTER_INFO** pAdaterInfo);
	//获取IPv6地址信息并添加到网卡信息中
	static DTK_INT32 GetIPv6ToAdapterInfo_Inter_Posix(DTK_ADAPTER_INFO* pAdaterInfo);
    //获取网卡链表
    static DTK_INT32 GetAdapterList_Inter_Posix(DTK_SOCK_T iFd, struct ifreq* buf,  struct ifconf* pIfc);
    //获取特定网卡的IP地址链表
    static DTK_INT32 GetIpList_Inter_Posix(DTK_SOCK_T iFd, struct ifreq*, DTK_IPADDRESS** pAddress);
    //获取网关IP地址链表(没有实现)
    static DTK_INT32 GetGatewayList_Inter_Posix(DTK_SOCK_T iFd, struct ifreq* ifr, DTK_IPADDRESS** pAddress);
#endif

//获取网卡信息(DTK内部使用)
static DTK_INT32 GetAdapterInfo_Inter(DTK_ADAPTER_INFO** pAdaterInfo);

//释放IP地址链表(DTK内部使用)
static DTK_INT32 Free_Address_Inter(DTK_IPADDRESS* pAddInfo);



DTK_DECLARE DTK_INT32 CALLBACK DTK_GetMacAddr(const char* pLocalIP, unsigned char* pMacAddr, DTK_INT32* iMacAddrLen)
{
    if ((pLocalIP == NULL) || (pMacAddr == NULL ) || (iMacAddrLen == NULL) || (*iMacAddrLen == 0))
    {
        return DTK_ERROR;
    }

    /*
     *  默认MAC地址长度为6个字节,
     *  linux系统在DTK程序中不再改变,Windows会根据AdapterInfo信息中的MAC长度数据改变。
     * */
    *iMacAddrLen = DTK_DEF_ADAPTER_ADDRESS_LEN;

#if defined (OS_WINDOWS)
    IP_ADAPTER_INFO* pAdapterInfo = NULL;
    PIP_ADAPTER_INFO pAdapter = NULL;

    if (GetAdapterList_Inter_Windows(&pAdapterInfo, NULL) == DTK_ERROR)
    {
        return DTK_ERROR;
    }

    for (pAdapter = pAdapterInfo; pAdapter; pAdapter=pAdapter->Next)
    {
        if (strcmp(pAdapter->IpAddressList.IpAddress.String, pLocalIP) == 0)
        {
            memcpy(pMacAddr,pAdapter->Address,pAdapter->AddressLength);
            *iMacAddrLen = pAdapter->AddressLength;
            //内存由GetAdapterList_Inter_Windows分配,需要在外面释放 
            free(pAdapterInfo);
            pAdapterInfo = NULL;
            return DTK_OK;
        }
    }
    
    free(pAdapterInfo);
    pAdapterInfo = NULL;

#elif defined(OS_POSIX)
    DTK_SOCK_T iFd = DTK_INVALID_SOCKET;
    int iInterface = 0;
    struct ifreq buf[DTK_MAX_NIC_NUM];
    struct ifconf ifc;

    iFd = DTK_CreateSocket(AF_INET,SOCK_DGRAM,0);
    if (iFd == DTK_INVALID_SOCKET)
    {
        return DTK_ERROR;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t) buf;
    if (ioctl (iFd, SIOCGIFCONF, (char *) &ifc))
    {
        DTK_CloseSocket(iFd);
        iFd = DTK_INVALID_SOCKET;
        return DTK_ERROR;
    }

    iInterface = ifc.ifc_len / sizeof (struct ifreq);

    while ((iInterface--) > 0)
    {
        if (!(ioctl (iFd, SIOCGIFADDR, (char *) &buf[iInterface])))
        {
            if ((((struct sockaddr_in*)(&buf[iInterface].ifr_addr))->sin_addr.s_addr) == inet_addr(pLocalIP))
            {
                if (!(ioctl (iFd, SIOCGIFHWADDR, (char *) &buf[iInterface])))
                {
                    memcpy(pMacAddr,buf[iInterface].ifr_hwaddr.sa_data,DTK_DEF_ADAPTER_ADDRESS_LEN);
                    DTK_CloseSocket(iFd);
                    iFd = DTK_INVALID_SOCKET;
                    return DTK_OK;
                }
            }
        }
    }

    DTK_CloseSocket(iFd);
    iFd = DTK_INVALID_SOCKET;
#endif

    return DTK_ERROR;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAdapterInfo(DTK_ADAPTER_INFO** pAdaterInfo)
{
    return GetAdapterInfo_Inter(pAdaterInfo);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_FreeAdapterInfo(DTK_ADAPTER_INFO* pAdaterInfo)
{
    DTK_ADAPTER_INFO* pAdapter = pAdaterInfo;
    DTK_ADAPTER_INFO* pAdapterTmp = pAdapter;

    while (pAdapter)
    {
        //地址链表内存也是动态分配,需要释放
        Free_Address_Inter(pAdapter->pIpAddrList);
        Free_Address_Inter(pAdapter->pGatawayList);
        Free_Address_Inter(pAdapter->pDhcpServerList);
		Free_Address_Inter(pAdapter->pIpAddrv6List);

        pAdapterTmp = pAdapter->pNext;
        delete pAdapter;
        pAdapter = pAdapterTmp;
    }

    return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddressInfo(const char *sNodeName, const char *sServerName, DTK_ADDRESS_INFO *Hints, DTK_ADDRESS_INFO **pRes)
{
    struct addrinfo hints = {0};
    struct addrinfo *res = NULL;
    //调用者传入的信息封装转换为addrinfo
    if (Hints != NULL)
    {
        hints.ai_flags = Hints->iFlags;
        hints.ai_family = Hints->iFamily;
        hints.ai_socktype = Hints->iSockType;
        hints.ai_protocol = Hints->iProtocol;
        if (Hints->Address.SA.sin4.sin_family == AF_INET)
        {
            hints.ai_addrlen = sizeof(Hints->Address.SA.sin4);
            hints.ai_addr = (struct sockaddr *)&Hints->Address.SA.sin4;
        }
        else if (Hints->Address.SA.sin4.sin_family == AF_INET6)
        {
            hints.ai_addrlen = sizeof(Hints->Address.SA.sin6);
            hints.ai_addr = (struct sockaddr *)&Hints->Address.SA.sin6;
        }
        hints.ai_canonname = Hints->sCanonName;
    }

    if (getaddrinfo(sNodeName, sServerName, &hints, &res) != 0)
    {
        return DTK_ERROR;
    }

    DTK_ADDRESS_INFO *head = NULL;
    DTK_ADDRESS_INFO *tail = NULL;
    DTK_ADDRESS_INFO *tmp = NULL;

    //这里需要使用临时的tmp_res来替代操作，否则会出现直接操作res导致最后释放时失效造成内存泄漏
    struct addrinfo *tmp_res = res;
    while (tmp_res != NULL)
    {
        tmp = new(::std::nothrow) DTK_ADDRESS_INFO;
        if (tmp == NULL)
        {
            DTK_FreeAddressInfo(head);
            freeaddrinfo(res);
            return DTK_ERROR;
        }
        memset(tmp, 0, sizeof(*tmp));

        if (head == NULL)
        {
            head = tmp;
            tail = head;
        }
        else
        {
            tail->pNext = tmp;
            tail = tmp;
        }

        tmp->iFlags = tmp_res->ai_flags;
        tmp->iFamily = tmp_res->ai_family;
        tmp->iSockType = tmp_res->ai_socktype;
        tmp->iProtocol = tmp_res->ai_protocol;
        //规范化名称不确定长度，因此采用动态分配
        if (tmp_res->ai_canonname != NULL)
        {
            size_t len = strlen(tmp_res->ai_canonname);
            tmp->sCanonName = new(::std::nothrow) char[len + 1];
            if (tmp->sCanonName == NULL)
            {
                DTK_FreeAddressInfo(head);
                freeaddrinfo(res);
                return DTK_ERROR;
            }
            memcpy(tmp->sCanonName, tmp_res->ai_canonname, len);
            tmp->sCanonName[len] = 0;
        }
        //按照协议族拷贝对应的地址信息，不能按照先前的长度来操作
        if (tmp_res->ai_family == PF_INET)
        {
            memcpy(&tmp->Address.SA.sin4, tmp_res->ai_addr, tmp_res->ai_addrlen);
        }
        else if (tmp_res->ai_family == PF_INET6)
        {
            memcpy(&tmp->Address.SA.sin6, tmp_res->ai_addr, tmp_res->ai_addrlen);
        }

        tmp_res = tmp_res->ai_next;
    }

    //最后将获得内存地址赋给传出参数
    *pRes = head;

    //释放获得的内存
    freeaddrinfo(res);
    return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_FreeAddressInfo(DTK_ADDRESS_INFO *pAddressInfo)
{
    while (pAddressInfo != NULL)
    {
        if (pAddressInfo->sCanonName != NULL)
        {
            delete[] pAddressInfo->sCanonName;
        }

        DTK_ADDRESS_INFO *tmp = pAddressInfo;
        pAddressInfo = pAddressInfo->pNext;

        delete tmp;
    }

    return DTK_OK;
}

DTK_DECLARE DTK_UINT16 CALLBACK DTK_Htons(DTK_UINT16 nHostShort)
{
    return htons(nHostShort);
}

DTK_DECLARE DTK_UINT16 CALLBACK DTK_Ntohs(DTK_UINT16 nNetShort)
{
    return ntohs(nNetShort);
}

DTK_DECLARE DTK_UINT32 CALLBACK DTK_Htonl(DTK_UINT32 nHostLong)
{
    return htonl(nHostLong);
}

DTK_DECLARE DTK_UINT32 CALLBACK DTK_Ntohl(DTK_UINT32 nNetLong)
{
    return ntohl(nNetLong);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetHostName(char* pName, DTK_INT32 iNameLen)
{
    return (gethostname(pName, iNameLen) == 0) ? DTK_OK : DTK_ERROR;
}

DTK_DECLARE hostent* CALLBACK DTK_GetAddressByName(const char* pName)
{
    return gethostbyname(pName);
}

static DTK_INT32 GetAdapterInfo_Inter(DTK_ADAPTER_INFO** pAdaterInfo)
{
#if defined (OS_WINDOWS)
    return GetAdapterInfo_Inter_Windows(pAdaterInfo);
#elif defined (OS_POSIX)
    return GetAdapterInfo_Inter_Posix(pAdaterInfo);
#endif

    return DTK_ERROR;
}

static DTK_INT32 Free_Address_Inter(DTK_IPADDRESS* pAddInfo)
{
    DTK_IPADDRESS* pAddr = pAddInfo;
    DTK_IPADDRESS* pTmp = pAddInfo;

    while (pAddr)
    {
        pTmp = pAddr->pNext;
        delete pAddr;
        pAddr = pTmp;
    }

    return DTK_OK;
}


////////////////////////////////////////////////////////////////////////////
////内部函数
#if defined (OS_WINDOWS)
    /**
    * Function: GetAdapterInfo_Inter_Windows
    * Desc: 获取Windows操作系统网卡信息
    * Input:    
    * Output:   pAdapterInfo:网卡信息
    * Return:   0成功否则失败
    * Others:
    */
    static DTK_INT32 GetAdapterInfo_Inter_Windows(DTK_ADAPTER_INFO** pAdaterInfo)
    {
        DTK_INT32 iRet = DTK_ERROR;

        //成功返回时把pAdater指针赋值给(*pAdaterInfo).
        DTK_ADAPTER_INFO* pAdater = NULL;
		DTK_ADAPTER_INFO* pHeadAdater = NULL;
        IP_ADAPTER_INFO* pIpAdapterInfo = NULL;
		IP_ADAPTER_ADDRESSES* pAdapterAddr = NULL;
        
        iRet = GetAdapterList_Inter_Windows(&pIpAdapterInfo, &pAdapterAddr);
        if (iRet == DTK_OK)
        {
            IP_ADAPTER_INFO* pIpAdapter = NULL;
			DTK_ADAPTER_INFO* pAdapterTmp = NULL;
            //把信息记录到pAdaterInfo中去
            for (pIpAdapter = pIpAdapterInfo; pIpAdapter; pIpAdapter = pIpAdapter->Next)
            {
				pAdapterTmp = new(::std::nothrow) DTK_ADAPTER_INFO;
                if (pAdapterTmp != NULL)
                {
                    DTK_ZeroMemory(pAdapterTmp, sizeof(*pAdapterTmp));

                    pAdapterTmp->iIndex = pIpAdapter->Index;
                    pAdapterTmp->nDhcpEnabled = pIpAdapter->DhcpEnabled;
                    pAdapterTmp->nMacAddrLen = pIpAdapter->AddressLength;
                    pAdapterTmp->nType = pIpAdapter->Type;
                    pAdapterTmp->pNext = NULL;

                    if (pAdapterTmp->nMacAddrLen > DTK_MAX_ADAPTER_ADDRESS_LEN)
                    {
						free(pIpAdapterInfo);
						free(pAdapterAddr);
						delete pAdapterTmp;
                        DTK_FreeAdapterInfo(pHeadAdater);
                        return DTK_ERROR;
                    }
                    else
                    {
                        memcpy(pAdapterTmp->szMacAddr,pIpAdapter->Address,pAdapterTmp->nMacAddrLen);
                    }

                    DTK_Snprintf(pAdapterTmp->szAdapterName,sizeof(pAdapterTmp->szAdapterName),"%s",pIpAdapter->AdapterName);
                    DTK_Snprintf(pAdapterTmp->szDescription,sizeof(pAdapterTmp->szDescription),"%s",pIpAdapter->Description);
                    
                    //这里没有判断返回值可能会有问题
                    GetIpList_Inter_Windows(pIpAdapter->IpAddressList, &pAdapterTmp->pIpAddrList);
                    GetIpList_Inter_Windows(pIpAdapter->GatewayList,&pAdapterTmp->pGatawayList);
                    GetIpList_Inter_Windows(pIpAdapter->DhcpServer,&pAdapterTmp->pDhcpServerList);
					GetIpv6List_Inter_Windows(pIpAdapter->Address, pAdapterAddr, &pAdapterTmp->pIpAddrv6List);
                    
					//修改链表增加方式由前插式改为后缀式，以避免颠倒了从系统得到网卡信息的顺序
                    if (pHeadAdater == NULL)
                    {
                        pHeadAdater = pAdapterTmp;
						pAdater = pHeadAdater;
                    }
                    else
                    {
                        pAdater->pNext = pAdapterTmp;
						pAdater = pAdapterTmp;
                    }
                }
                else
                {
					free(pIpAdapterInfo);
					free(pAdapterAddr);
                    //释放已经非配的内存
                    DTK_FreeAdapterInfo(pHeadAdater);
                    return DTK_ERROR;
                }
            }

            //GetAdapterList_Inter_Windows中分配的内存需要在这里释放
            free(pIpAdapterInfo);
			free(pAdapterAddr);

            *pAdaterInfo = pHeadAdater;
            return DTK_OK;
        }

        return DTK_ERROR;
    }

    /**
     * Function: GetAdapterList_Inter_Windowss
     * Desc: 获取网卡链表
     * Input: 
     * Output: pIpAdapterInfo:网卡地址链表
     * Return:  0成功否则失败
     */
    static DTK_INT32 GetAdapterList_Inter_Windows(IP_ADAPTER_INFO** pIpAdapterInfo, IP_ADAPTER_ADDRESSES** pAdapterAddr)
    {
		if (g_hIphlpapiDll == NULL)
		{
			return DTK_ERROR;
		}

        DWORD dwRet = 0;
        //成功返回时把pAdapterInfo指针赋值给(*pIpAdapterInfo).
        PIP_ADAPTER_INFO pAdapterInfo = NULL;
        ULONG lLen = 0;

        pAdapterInfo = (PIP_ADAPTER_INFO)malloc(sizeof(*pAdapterInfo));
        if (pAdapterInfo == NULL)
        {
            return DTK_ERROR;
        }

        DTK_ZeroMemory(pAdapterInfo, sizeof(*pAdapterInfo));
		if (g_pInfoFunc == NULL)
		{
			return DTK_ERROR;
		}
		//这个接口在5.2版本以下的内核中并不支持，因此获取接口失败不返回错误，只置外部指针为NULL
		//并且只在外部需要获取IPv6时才做相关操作
		if (g_pAddrFunc != NULL)
		{
			if (pAdapterAddr != NULL)
			{
				//第一次获取时长度置0以便由api告知我们缓冲区需要多大
				lLen = 0;
				//标志位除指定获取IPv6信息外，将名称等暂不需要的信息去除以减小存储大小
				ULONG flag = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME | GAA_FLAG_SKIP_MULTICAST;
				
				dwRet = g_pAddrFunc(AF_INET6, flag, NULL, *pAdapterAddr, &lLen);
				if (dwRet == ERROR_BUFFER_OVERFLOW)
				{
					*pAdapterAddr = (PIP_ADAPTER_ADDRESSES)malloc(lLen);
					if (*pAdapterAddr != NULL)
					{
						dwRet = g_pAddrFunc(AF_INET6, flag, NULL, *pAdapterAddr, &lLen);
						if (dwRet != ERROR_SUCCESS)
						{
							free(*pAdapterAddr);
							*pAdapterAddr = NULL;
						}
					}
				}
				else
				{
					*pAdapterAddr = NULL;
				}
			}
		}
		else
		{
			if (pAdapterAddr != NULL)
			{
				*pAdapterAddr = NULL;
			}
		}
		//以上IPv6获取操作结束

		lLen = sizeof(*pAdapterInfo);
        dwRet = g_pInfoFunc(pAdapterInfo, &lLen);
        if ((ERROR_SUCCESS != dwRet) && (ERROR_BUFFER_OVERFLOW != dwRet))
        {
			return DTK_ERROR;
        }

        if (ERROR_SUCCESS != dwRet)//第一次获取失败
        {
            free(pAdapterInfo);
			pAdapterInfo = (PIP_ADAPTER_INFO)malloc(lLen);
			if (pAdapterInfo == NULL)
			{
				return DTK_ERROR;
			}
            DTK_ZeroMemory(pAdapterInfo, lLen);

            dwRet = g_pInfoFunc(pAdapterInfo, &lLen);
            if (dwRet != ERROR_SUCCESS)
            {
                free(pAdapterInfo);
                pAdapterInfo = NULL;
                return DTK_ERROR;
            }
        }
        
        *pIpAdapterInfo = pAdapterInfo;
        return DTK_OK;
    }

    /**
     * Function: GetIpList_Inter_Windows
     * Desc:    把Windows地址链表转化为DTK地址链表
     * Input:   struIpAddrString:Windows地址链表
                
     * Output:  pAddress:DTK地址链表
     * Return:  0成功否则失败
     * Others:
     */
    static DTK_INT32 GetIpList_Inter_Windows(IP_ADDR_STRING& struIpAddrString, DTK_IPADDRESS** pAddress)
    {
        //成功返回时把pAddr指针赋值给(*pAddress).
        DTK_IPADDRESS* pAddr = NULL;
		DTK_IPADDRESS* pAddrTail = NULL;
        DTK_IPADDRESS* pAddrTmp = NULL;
		IP_ADDR_STRING* pSysIPAddrTmp = &struIpAddrString;

        while (pSysIPAddrTmp)
        {
            pAddrTmp = new (::std::nothrow) DTK_IPADDRESS;
            if (pAddrTmp == NULL)
            {
                //需要释放已经分配的内存
                Free_Address_Inter(pAddr);
                return DTK_ERROR;
            }
            DTK_ZeroMemory(pAddrTmp, sizeof(*pAddrTmp));
            DTK_Snprintf(pAddrTmp->struIpAddrString.szIPAddr, sizeof(pAddrTmp->struIpAddrString.szIPAddr), "%s", pSysIPAddrTmp->IpAddress.String);
            DTK_Snprintf(pAddrTmp->struIpAddrString.szNetMask, sizeof(pAddrTmp->struIpAddrString.szNetMask), "%s", pSysIPAddrTmp->IpMask.String);

			if (pAddr == NULL)
			{
				pAddr = pAddrTmp;
				pAddrTail = pAddr;
			}
			else
			{
				pAddrTail->pNext = pAddrTmp;
				pAddrTail = pAddrTmp;
			}
			pSysIPAddrTmp = pSysIPAddrTmp->Next;
        }

        *pAddress = pAddr;
        return DTK_OK;
    }

	/**
     * Function: GetIpv6List_Inter_Windows
     * Desc:    把Windows IPv6地址链表转化为DTK IPv6地址链表
     * Input:   byMac:MAC地址，用来在地址信息中寻找对应的网卡
				pIpAdapterAddr:Windows地址信息链表
                
     * Output:  pAddress:DTK地址链表
     * Return:  0成功否则失败
     * Others:
     */
	DTK_INT32 GetIpv6List_Inter_Windows(BYTE *byMac, IP_ADAPTER_ADDRESSES* pIpAdapterAddr, DTK_IPADDRESS** pAddress)
	{
		if (pIpAdapterAddr == NULL)
		{
			return DTK_ERROR;
		}

		PIP_ADAPTER_ADDRESSES pCurrAddresses = pIpAdapterAddr;
		while (pCurrAddresses)
		{
			if (memcmp(byMac, pCurrAddresses->PhysicalAddress, pCurrAddresses->PhysicalAddressLength) == 0)
			{
				DTK_IPADDRESS* pTmpAddr = NULL;
				DTK_IPADDRESS* pTmpNext = NULL;

				PIP_ADAPTER_UNICAST_ADDRESS pUnicastAddress = pCurrAddresses->FirstUnicastAddress;
				while (pUnicastAddress)
				{
					if (pTmpAddr == NULL)
					{
						pTmpAddr = new(::std::nothrow) DTK_IPADDRESS;
						if (pTmpAddr == NULL)
						{
							return DTK_ERROR;
						}
						DTK_ZeroMemory(pTmpAddr, sizeof(*pTmpAddr));
						pTmpNext = pTmpAddr;
					}
					else
					{
						pTmpNext = new(::std::nothrow) DTK_IPADDRESS;
						if (pTmpNext == NULL)
						{
							Free_Address_Inter(pTmpAddr);
							return DTK_ERROR;
						}
						DTK_ZeroMemory(pTmpNext, sizeof(*pTmpNext));
					}
					
					//LPSOCKET_ADDRESS_LIST paddrlist=(LPSOCKET_ADDRESS_LIST)pUnicastAddress;
					//不能使用LPSOCKET_ADDRESS_LIST直接转换pUnicastAddress指针，因为这个类型只在XP有效，同时pUnicastAddress结构与这个类型也不匹配，可以直接从结构中获取地址转换
					sockaddr_in6 *sock = (sockaddr_in6 *)(pUnicastAddress->Address.lpSockaddr);

					memcpy(pTmpNext->struIpAddrString.szIPAddr, sock->sin6_addr.s6_addr\
						, sizeof(pTmpNext->struIpAddrString.szIPAddr));
					
					pTmpNext = pTmpNext->pNext;
					pUnicastAddress = pUnicastAddress->Next;
				}

				*pAddress = pTmpAddr;
				break;
			}

			pCurrAddresses = pCurrAddresses->Next;
		}

		return DTK_OK;
	}

#elif defined(OS_POSIX)
    /**
    * Function: GetAdapterInfo_Inter_Posix
    * Desc: 获取Linux操作系统网卡信息
    * Input:    
    * Output:   pAdapterInfo:网卡信息
    * Return:   0成功否则失败
    * Others:
    */
    static DTK_INT32 GetAdapterInfo_Inter_Posix(DTK_ADAPTER_INFO** pAdaterInfo)
    {
        DTK_INT32 iInterface = 0;
        struct ifreq buf[DTK_MAX_NIC_NUM];
        struct ifconf iFc;

        DTK_SOCK_T iFd = DTK_CreateSocket(AF_INET, SOCK_DGRAM, 0);
        if (iFd == DTK_INVALID_SOCKET)
        {
            return DTK_ERROR;
        }
        if ( GetAdapterList_Inter_Posix(iFd, buf, &iFc) == DTK_ERROR)
        {
            DTK_CloseSocket(iFd);
            iFd = DTK_INVALID_SOCKET;
            return DTK_ERROR;
        }

        iInterface = iFc.ifc_len / sizeof (struct ifreq);

        //成功返回时把pAdater指针赋值给(*pAdaterInfo).
        DTK_ADAPTER_INFO* pAdater = NULL;
		DTK_ADAPTER_INFO* pTmpAdater = NULL;
		int index = 0;

        while (iInterface > 0)
        {
            DTK_ADAPTER_INFO* pAdaterTmp = NULL;
            //char szDst[DTK_MAX_IP_ADDRESS_LEN] = {0};
            pAdaterTmp = new (::std::nothrow) DTK_ADAPTER_INFO;
            if (pAdaterTmp == NULL)
            {
                break;
            }

			index = iInterface - 1;

            DTK_ZeroMemory(pAdaterTmp, sizeof(*pAdaterTmp));
            pAdaterTmp->iIndex = index;
            pAdaterTmp->nDhcpEnabled = 0;
            pAdaterTmp->nMacAddrLen = DTK_DEF_ADAPTER_ADDRESS_LEN;
            
            //网卡名称
            DTK_Snprintf(pAdaterTmp->szAdapterName,sizeof(pAdaterTmp->szAdapterName),"%s",(char*)&buf[index].ifr_name);
        
            //POSIX没有网卡描述，用名字代替
            //DTK_Snprintf(pAdaterTmp->szDescription,sizeof(pAdaterTmp->szDescription),"%s",(char*)&buf[index].ifr_name);
            
            //物理地址
            if ((ioctl (iFd, SIOCGIFHWADDR, (char *) &buf[index])) < 0)
            {
                break;
            }
            memcpy(pAdaterTmp->szMacAddr,buf[index].ifr_hwaddr.sa_data,DTK_DEF_ADAPTER_ADDRESS_LEN);

            //获取IP地址
            if (DTK_ERROR == GetIpList_Inter_Posix(iFd, &buf[index], &pAdaterTmp->pIpAddrList))
            {
                break;
            }
            
            //获取网卡网关链表
			//2010-03-12 修改链表增加方式由前插式改为后缀式，以避免颠倒了从系统得到网卡信息的顺序
            if (pAdater == NULL)
            {
                pAdater = pAdaterTmp;
				pTmpAdater = pAdater;
            }
            else
            {
				pTmpAdater->pNext = pAdaterTmp;
				pTmpAdater = pAdaterTmp;
            }

			-- iInterface;
        }
        
        DTK_CloseSocket(iFd);
        iFd = DTK_INVALID_SOCKET;

        //printf("posix run to here 0 ,iInterface = %d\n", iInterface);

        if (iInterface > 0)
        {
            DTK_FreeAdapterInfo(pAdater);
            return DTK_ERROR;
        }

		//在获取到网卡信息之后尝试加入IPv6地址信息
        GetIPv6ToAdapterInfo_Inter_Posix(pAdater);

        *pAdaterInfo = pAdater;

        return DTK_OK;
    }

	/**
    * Function: GetIPv6ToAdapterInfo_Inter_Posix
    * Desc: 获取Linux操作系统IPv6地址信息，并添加到网卡信息中
    * Input:    
    * Output:   pAdapterInfo:网卡信息
    * Return:   0成功否则失败
    * Others:
    */
	DTK_INT32 GetIPv6ToAdapterInfo_Inter_Posix(DTK_ADAPTER_INFO* pAdaterInfo)
	{
		int fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
		if (fd == -1)
		{
			return DTK_ERROR;
		}

		struct sockaddr_nl addrnl = {0};
		addrnl.nl_family = PF_NETLINK;
		addrnl.nl_pid = getpid();
		if (bind(fd, (struct sockaddr *)&addrnl, sizeof(addrnl)) < 0)
		{
			return DTK_ERROR;
		}

		struct 
		{
			struct nlmsghdr nh;
			struct ifinfomsg ifi;
		}struReq = {0};

		struReq.nh.nlmsg_len = sizeof(struReq);
		struReq.nh.nlmsg_type = RTM_GETLINK;
		struReq.nh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
		struReq.nh.nlmsg_pid = getpid();
		struReq.ifi.ifi_family = PF_INET6;

		struct  
		{
			int if_index;
			unsigned char mac[6];
			unsigned char res[2];
		}if_macaddr[DTK_MAX_NIC_NUM] = {0};
		int mac_index = 0;

		struct
		{
			int if_index;
			unsigned char ipv6[16];
		}if_ipv6[DTK_MAX_NIC_NUM] = {0};
		int ip_index = 0;

		char buffer[2048] = {0};
		int len = 0;
		int msg_len = 0;
		int loop = 0;

		//循环两次，第一次获取LINK信息以得到MAC地址，第二次获取ADDR信息以得到IPv6地址
		//MAC地址用于与之前获取的网卡信息做对应，LINK信息与ADDR信息之间通过网卡索引对应
		while (loop < 2)
		{
			memset(&addrnl, 0, sizeof(addrnl));
			addrnl.nl_family = PF_NETLINK;
			if (sendto(fd, &struReq, struReq.nh.nlmsg_len, 0, (struct sockaddr *)&addrnl, sizeof(addrnl)) < 0)
			{
				return -1;
			}

			int keeploop = 1;
			while (keeploop)
			{
				memset(buffer, 0, sizeof(buffer));
				len = recv(fd, buffer, sizeof(buffer), 0);
				if (len <= 0)
				{
					break;
				}

				//对收到的数据进行解析，然后存储在分配的结构中，全部完成后再到网卡结构中填写对应的信息
				for (struct nlmsghdr *nl_msg = (struct nlmsghdr *)buffer; \
					NLMSG_OK(nl_msg, (unsigned)len); nl_msg = NLMSG_NEXT(nl_msg, len))
				{
					if (nl_msg->nlmsg_type == NLMSG_DONE || nl_msg->nlmsg_type == NLMSG_ERROR)
					{
						keeploop = 0;
						break;
					}
					//两次循环首先在获取的LINK信息中解析MAC地址并得到网卡索引
					//然后在获取到的ADDR信息中解析IPv6地址并得到网卡索引，这两个索引将用于比对
					if (loop == 0)
					{
						struct ifinfomsg *ifinfo = (ifinfomsg *)NLMSG_DATA(nl_msg);
						struct rtattr *ifi = IFLA_RTA(ifinfo);
						msg_len = NLMSG_PAYLOAD(nl_msg, sizeof(struct ifinfomsg));
						while (RTA_OK(ifi, msg_len) && mac_index < DTK_MAX_NIC_NUM)
						{
							if (ifi->rta_type == IFLA_ADDRESS)
							{
								memcpy(if_macaddr[mac_index].mac, (unsigned char *)RTA_DATA(ifi), 6);
								if_macaddr[mac_index].if_index = ifinfo->ifi_index;
								++ mac_index;
							}
							ifi = RTA_NEXT(ifi, msg_len);
						}
					}
					else
					{
						struct ifaddrmsg *ifmsg = (ifaddrmsg *)NLMSG_DATA(nl_msg);
						msg_len = NLMSG_PAYLOAD(nl_msg, sizeof(struct ifaddrmsg));
						struct rtattr *ifa = IFA_RTA(ifmsg);
						while (RTA_OK(ifa, msg_len) && ip_index < DTK_MAX_NIC_NUM)
						{
							if (ifa->rta_type == IFA_ADDRESS)
							{
								memcpy(if_ipv6[ip_index].ipv6, (char *)RTA_DATA(ifa), 16);
								if_ipv6[ip_index].if_index = ifmsg->ifa_index;
								++ ip_index;
							}
							ifa = RTA_NEXT(ifa, msg_len);
						}
					}
				}
			}

			//第二次操作使用GETADDR以获取IPv6地址信息
			struReq.nh.nlmsg_type = RTM_GETADDR;
			++ loop;
		}

		close(fd);
		//fprintf(stderr, "loop[%d] find %d mac and %d ip\n", loop, mac_index, ip_index);

		DTK_ADAPTER_INFO *pTmpAddr = pAdaterInfo;
		DTK_IPADDRESS *pTmpIP = NULL;
		for (int i = 0; i < mac_index; ++ i)
		{
			//将从NETLINK中获取到的网卡信息与之前获取到的进行比对，MAC地址是唯一的比对标识
			pTmpAddr = pAdaterInfo;
			while (pTmpAddr)
			{
				if (memcmp(pTmpAddr->szMacAddr, if_macaddr[i].mac, sizeof(if_macaddr[i].mac)) == 0)
				{
					//比对成功后再将通过NETLINK获取到的IPv6信息与网卡信息进行比对，网卡索引是唯一的比对标识
					pTmpIP = NULL;
					for (int j = 0; j < ip_index; ++ j)
					{
						if (if_ipv6[j].if_index == if_macaddr[i].if_index)
						{
							if (pTmpAddr->pIpAddrv6List == NULL)
							{
								pTmpAddr->pIpAddrv6List = new(::std::nothrow) DTK_IPADDRESS;
								if (pTmpAddr->pIpAddrv6List == NULL)
								{
									return DTK_ERROR;
								}
								DTK_ZeroMemory(pTmpAddr->pIpAddrv6List, sizeof(DTK_IPADDRESS));
								pTmpIP = pTmpAddr->pIpAddrv6List;
							}
							else if (pTmpIP == NULL)
							{
								pTmpIP = new(::std::nothrow) DTK_IPADDRESS;
								if (pTmpIP == NULL)
								{
									Free_Address_Inter(pTmpAddr->pIpAddrv6List);
									return DTK_ERROR;
								}
								DTK_ZeroMemory(pTmpIP, sizeof(DTK_IPADDRESS));
							}
							memcpy(pTmpIP->struIpAddrString.szIPAddr, if_ipv6[j].ipv6, sizeof(if_ipv6[j].ipv6));
							pTmpIP = pTmpIP->pNext;
						}
					}
					break;
				}
				pTmpAddr = pTmpAddr->pNext;
			}
		}

		return DTK_OK;
	}

    /**
     * Function: GetAdapterInfo_Inter_Posix
     * Desc:    获取Linux操作系统网卡链表
     * Input:   iFd:socket 描述符
                buf:网卡链表
                pIfc:所有网卡信息
     * Output:  pAdapterInfo:网卡信息
     * Return:  0成功否则失败
     * Others:
     */
    static DTK_INT32 GetAdapterList_Inter_Posix(DTK_SOCK_T iFd, struct ifreq* buf, struct ifconf* pIfc)
    {
        pIfc->ifc_len = sizeof(*buf)*DTK_MAX_NIC_NUM;
        pIfc->ifc_buf = (caddr_t) (buf);
        if (ioctl (iFd, SIOCGIFCONF, (char *)pIfc))
        {
            return DTK_ERROR;
        }

        return DTK_OK;
    }

    /**
     * Function: GetAdapterInfo_Inter_Posix
     * Desc:    获取Linux操作系统对应网卡的IP链表
     * Input:   
     * Output:  pAdapterInfo:网卡信息
     * Return:  0成功否则失败
     * Others:
     */
    static DTK_INT32 GetIpList_Inter_Posix(DTK_SOCK_T iFd, struct ifreq* buf, DTK_IPADDRESS** pAddress)
    {
        DTK_IPADDRESS* pAddr = NULL;
        //DTK_IPADDRESS* pAddrTmp = NULL;

        char szDst[DTK_MAX_IP_ADDRESS_LEN] = {0};
        //主机地址
        if ((ioctl (iFd, SIOCGIFADDR, (char *)buf)))
        {
            return DTK_ERROR;
        }

        pAddr = new (::std::nothrow) DTK_IPADDRESS;
        if (pAddr == NULL)
        {
            return DTK_ERROR;
        }

        DTK_ZeroMemory(pAddr, sizeof(DTK_IPADDRESS));
        DTK_Snprintf(pAddr->struIpAddrString.szIPAddr,sizeof(pAddr->struIpAddrString.szIPAddr), \
            "%s", inet_ntoa(((struct sockaddr_in *)(&buf->ifr_addr))->sin_addr));
            /*DTK_InetNtop(AF_INET,((struct sockaddr_in*)(&(buf->ifr_addr)))->sin_addr, szDst, sizeof(szDst))*/
        
        //printf("schina GetIpList_Inter_Posix szIp = %s\n", pAddr->struIpAddrString.szIPAddr);
        //掩码
        DTK_ZeroMemory(szDst, sizeof(szDst));
        if ((ioctl (iFd, SIOCGIFNETMASK, (char *)buf)))
        {
            delete pAddr;
            pAddr = NULL;
            return DTK_ERROR;
        }

        DTK_Snprintf(pAddr->struIpAddrString.szNetMask,sizeof(pAddr->struIpAddrString.szNetMask), \
            "%s", inet_ntoa(((struct sockaddr_in *)(&buf->ifr_ifru.ifru_netmask))->sin_addr));
            /*DTK_InetNtop(AF_INET,((struct sockaddr_in*)(&(buf->ifr_ifru.ifru_netmask)))->sin_addr.s_addr, szDst, sizeof(szDst))*/

        *pAddress = pAddr;

        return DTK_OK;
    }

    /**
     * Function: GetGatewayList_Inter_Posix
     * Desc:    获取网关链表
     * Input:
     * Output:
     * Return:  0成功否则失败
     * Others:
     */
    static DTK_INT32 GetGatewayList_Inter_Posix(DTK_SOCK_T iFd, struct ifreq* buf, DTK_IPADDRESS** pAddress)
    {
        DTK_IPADDRESS* pAddr = NULL;

        if(ioctl(iFd,SIOCGIFNETMASK,(char *)buf) < 0) 
        {
            return DTK_ERROR;
        }

        pAddr = new (::std::nothrow) DTK_IPADDRESS;
        if (pAddr == NULL)
        {
            return DTK_ERROR;
        }

        DTK_ZeroMemory(pAddr, sizeof(DTK_IPADDRESS));
        DTK_Snprintf(pAddr->struIpAddrString.szIPAddr,sizeof(pAddr->struIpAddrString.szIPAddr),"%s", inet_ntoa(((struct sockaddr_in *)(&buf->ifr_ifru.ifru_netmask))->sin_addr));
        
        *pAddress = pAddr;

        return DTK_OK;
    }

#endif
