
#include "DTK_Addr.h"

#if defined (OS_POSIX)
#include <arpa/inet.h>
#define SOCKADDR sockaddr
#define PSOCKADDR sockaddr*
#endif

#define DTK_IN6ADDRSZ   16
#define DTK_INADDRSZ    4
#define DTK_INT16SZ     2

/*
 * Function:	DTK_InetPton4_Inter
 * Desc:		convert string formated ipv4 address to number address.
 * Input:		@param pSrc: string formated
 * Output:		@param pDst: number address
 * Return:		1 success otherwise fail
 * */
static int DTK_InetPton4_Inter(const char* pSrc, DTK_VOIDPTR pDst)
{
	const char szDigits[] = "0123456789";
	int iSawDigit;
	int iOctets;
	int iCh;
	unsigned char chTmp[DTK_INADDRSZ];
	unsigned char *pTp;

	if ( !pSrc || !pDst)
	{
		return -1;
	}

	iSawDigit = 0;
	iOctets = 0;
	*(pTp = chTmp) = 0;
	while ((iCh = *pSrc++) != '\0') 
	{
		const char *pCh;

		if ((pCh = (char *) strchr(szDigits, iCh)) != NULL) 
		{
			unsigned int nNew = *pTp * 10 + (int)(pCh - szDigits);

			if (nNew > 255)
			{
				return (0);
			}
			*pTp = (unsigned char)(nNew);
			if ( !iSawDigit ) 
			{
				if (++iOctets > 4)
				{
					return (0);
				}
				iSawDigit = 1;
			}
		} 
		else if (iCh == '.' && iSawDigit) 
		{
			if (iOctets == 4)
			{
				return (0);
			}
			*++pTp = 0;
			iSawDigit = 0;
		} 
		else
		{
			return (0);
		}
	}

	if (iOctets < 4)
	{
		return (0);
	}

	memcpy(pDst, chTmp, DTK_INADDRSZ);

	return (1);
}

/**
 * Function:	DTK_InetPton6_Inter
 * Desc:		convert string formated ipv6 address to number address.
 * Input:		@param pSrc: string formated
 * Output:		@param pDst: number address
 * Return:		1 success otherwise fail
 * */
static int DTK_InetPton6_Inter(const char* pSrc, DTK_VOIDPTR pDst)
{
	const char szXDigits_L[] = "0123456789abcdef";
	const char szXDigits_U[] = "0123456789ABCDEF";
	unsigned char szTmp[DTK_IN6ADDRSZ];
	unsigned char*pT;
	unsigned char *pEnd;
	unsigned char *pColon;
	const char *pXDigits;
	const char *pCurTok;
	int iCh;
	int iSawXDigit;
	unsigned nVal;

	if (!pSrc || !pDst)
	{
		return 0;
	}

	memset((pT = szTmp), 0, DTK_IN6ADDRSZ);
	pEnd = pT + DTK_IN6ADDRSZ;
	pColon = NULL;
	/* Leading :: requires some special handling. */
	if (*pSrc == ':')
	{
		if (*++pSrc != ':')
		{
			return (0);
		}
	}
	pCurTok = pSrc;
	iSawXDigit = 0;
	nVal = 0;
	while ((iCh = *pSrc++) != '\0') 
	{
		const char *pCh;

		if ((pCh = (char *) strchr((pXDigits = szXDigits_L), iCh)) == NULL)
		{
			pCh = (char *) strchr((pXDigits = szXDigits_U), iCh);
		}
		if (pCh != NULL) 
		{
			nVal <<= 4;
			nVal |= (pCh - pXDigits);
			if (nVal > 0xffff)
			{
				return (0);
			}
			iSawXDigit = 1;
			if (*pSrc != 0)
			{
				continue;
			}
		}
		if (iCh == ':' || *pSrc == 0) 
		{
			pCurTok = pSrc;
			if (!iSawXDigit) 
			{
				if (pColon)
				{
					return (0);
				}
				pColon = pT;
				continue;
			}
			if (pT + DTK_INT16SZ > pEnd)
			{
				return (0);
			}
			*pT++ = (unsigned char)((unsigned char) (nVal >> 8) & 0xff);
			*pT++ = (unsigned char)((unsigned char) nVal & 0xff);
			iSawXDigit = 0;
			nVal = 0;
			//结尾前出现"::"
			if ((iCh = *pCurTok) == ':' && pCurTok < (char *)(pSrc + sizeof(szTmp) -1))
			{
				unsigned char *stamp = pEnd - 2;
				char *tmp = (char *)pCurTok;
				while ((tmp = strchr(tmp + 1, ':')) != NULL)
				{
					stamp -= 2;
				}
				pT = stamp;
			}
			continue;
		}
		if (iCh == '.' && ((pT + DTK_INADDRSZ) <= pEnd) &&
			DTK_InetPton4_Inter(pCurTok, pT) > 0) 
		{
			pT += DTK_INADDRSZ;
			iSawXDigit = 0;
			break;	/* '\0' was seen by inet_pton4(). */
		}
		return (0);
	}

	memcpy(pDst, szTmp, sizeof(szTmp));
	return (1);
}

/*
 * Function:	DTK_InetPton_Inter
 * Desc:		convert string formated ip address to number address.
 * Input:		@param iAf:address family, AF_INET for IPV4, AF_INET6 for IPV6.
				@param pSrc: string formated
 * Output:		@param pDst: number address
 * Return:		DTK_OK if success otherwise fail
 * */
static DTK_INT32 DTK_InetPton_Inter(DTK_INT32 iAf, const char* pSrc, DTK_VOIDPTR pDst)
{
	if (!pSrc || !pDst)
	{
		return DTK_ERROR;
	}

	switch (iAf)
	{
	case AF_INET:
		if (DTK_InetPton4_Inter(pSrc, (unsigned char*)pDst))
		{
			return DTK_OK;
		}
		break;
#if defined(OS_SUPPORT_IPV6)
	case AF_INET6:
		if (DTK_InetPton6_Inter(pSrc, (unsigned char*)pDst))
		{
			return DTK_OK;
		}
		break;
#endif
	default:
		break;
	}

	return DTK_ERROR;
}

//////////////////////////////////////////////////////////////////

/*
 * Function:	InetNtop4
 * Desc:		convert ipv4 address to string format ipv4.	
 * Input:		@param pSrc:	adress need to convert	
				@param pDst:	destination of the result to store.
				@param iSize:	destination length.
 * Output:		@param pDst:	destination of the result to store.
 * Return:		string formated ipv4 address if succ, othersize NULL.
 * */
static const char* DTK_InetNtop4(const unsigned char* pSrc, char* pDst, int iSize)
{
	static const char szFmt[] = "%u.%u.%u.%u";
	char szTmp[sizeof "255.255.255.255"];

	if (!pSrc || !pDst || !iSize)
	{
		return NULL;
	}

#if (_MSC_VER >= 1500 && !defined _WIN32_WCE)
	sprintf_s(szTmp, sizeof(szTmp), szFmt, pSrc[0], pSrc[1], pSrc[2], pSrc[3]);
#else
	sprintf(szTmp, szFmt, pSrc[0], pSrc[1], pSrc[2], pSrc[3]);
#endif

	if ((int)strlen(szTmp) > iSize) 
	{
		return (NULL);
	}

#if (_MSC_VER >= 1500 && !defined _WIN32_WCE)
	strcpy_s(pDst,iSize,szTmp);
#else
	strncpy(pDst, szTmp, iSize);
#endif

	return (pDst);
}

/*
 * Function:	InetNtop6
 * Desc:		convert ipv6 address to string format ipv6.	
 * Input:		@param pSrc:	adress need to convert	
				@param pDst:	destination of the result to store.
				@param iSize:	destination length.
 * Output:		@param pDst:	destination of the result to store.
 * Return:		string formated ipv6 address if succ, othersize NULL.
 * */
static const char* DTK_InetNtop6(const unsigned char* pSrc, char* pDst, int iSize)
{
	//static char szTmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"];
	char szTmp[128] = {0};
	char *pTp;
	struct struType
	{ 
		int iBase;
		int iLen; 
	}; 
	struType struBest = {0,0};
	struType struCur = {0,0};

	unsigned int nWords[DTK_IN6ADDRSZ / DTK_INT16SZ];
	int i;

	if (!pSrc || !pDst || !iSize)
	{
		return NULL;
	}

	memset(nWords, 0, sizeof(nWords));
	for (i=0; i < DTK_IN6ADDRSZ; i++)	
	{
		nWords[i/2] |= (pSrc[i] << ((1-(i%2)) << 3));
	}

	struBest.iBase = -1;
	struCur.iBase = -1;
	for (i = 0; i < (DTK_IN6ADDRSZ / DTK_INT16SZ); i++) 
	{
		if (nWords[i] == 0) 
		{
			if (struCur.iBase == -1)
			{
				struCur.iBase = i, struCur.iLen = 1;
			}
			else
			{
				struCur.iLen++;
			}
		} 
		else 
		{
			if (struCur.iBase != -1) 
			{
				if (struBest.iBase == -1 || struCur.iLen > struBest.iLen)
				{
					struBest = struCur;
				}
				struCur.iBase = -1;
			}
		}
	}
	if (struCur.iBase != -1) 
	{
		if (struBest.iBase == -1 || struCur.iLen > struBest.iLen)
		{
			struBest = struCur;
		}
	}
	if (struBest.iBase != -1 && struBest.iLen < 2)
	{
		struBest.iBase = -1;
	}

	pTp = szTmp;
	for (i=0; i < (DTK_IN6ADDRSZ / DTK_INT16SZ); i++) 
	{
		/* Are we inside the best run of 0x00's? */
		if (struBest.iBase != -1 && i >= struBest.iBase && i < (struBest.iBase+struBest.iLen)) 
		{
			if (i == struBest.iBase)
			{
				*pTp++ = ':';
			}
			continue;
		}
		/* Are we following an initial run of 0x00s or any real hex? */
		if (i != 0)
		{
			*pTp++ = ':';
		}
		/* Is this address an encapsulated IPv4? */
		if (i == 6 && struBest.iBase == 0 && (struBest.iLen == 6 || (struBest.iLen == 5 && nWords[5] == 0xffff))) 
		{
			if (!DTK_InetNtop4(pSrc+12, pTp, sizeof(szTmp-(pTp-szTmp))))
			{
				return (NULL);
			}
			pTp += strlen(pTp);
			break;
		}
		sprintf(pTp, "%x", nWords[i]);
		pTp += strlen(pTp);
	}
	/* Was it a trailing run of 0x00's? */
	if (struBest.iBase != -1 && (struBest.iBase+struBest.iLen) == (DTK_IN6ADDRSZ / DTK_INT16SZ))
	{
		*pTp++ = ':';
	}
	*pTp++ = '\0';

	/*
	 * Check for overflow, copy, and we're done.
	 */
	if ((int) (pTp - szTmp) > iSize) 
	{
		return (NULL);
	}

	strncpy(pDst, szTmp, iSize);
	return (pDst);
}

/**
 * Function:	InetNtop
 * Desc:		convert ipv address to string format ipv.	
 * Input:		@param iAf:		address family AF_INET for IPV4, AF_INET6 for IPV6.
				@param pSrc:	adress need to convert	
				@param pDst:	destination of the result to store.
				@param iSize:	destination length.
 * Output:		@param pDst:	destination of the result to store.
 * Return:		string formated ipv address if succ, othersize NULL.
 * */
DTK_DECLARE const char* CALLBACK DTK_InetNtop(DTK_INT32 iAf, const unsigned char* pSrc, char* pDst, int iCnt)
{
	if (!pSrc || !pDst || !iCnt)
	{
		return NULL;
	}

	switch (iAf)
	{
	case AF_INET:
		return DTK_InetNtop4((const unsigned char*)pSrc, (char*)pDst, iCnt);
		break;
#if defined(OS_SUPPORT_IPV6)
	case AF_INET6:
		return DTK_InetNtop6((const unsigned char*)pSrc, (char*)pDst, iCnt);
		break;
#endif
	default:
		break;
	}

	return NULL;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddrByString(DTK_INT32 iAf, const char* pAddr, DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr)
{
	DTK_INT32 iRet = DTK_ERROR;
	
	if ( !pDTKAddr )
	{
		return DTK_ERROR;
	}

	if (NULL == pAddr || strlen(pAddr) == 0)
	{
		if (iAf == AF_INET)
		{
			pDTKAddr->SA.sin4.sin_addr.s_addr = INADDR_ANY;
			pDTKAddr->SA.sin4.sin_family = AF_INET;
			pDTKAddr->SA.sin4.sin_port = htons(nPort);
		}
		else if (iAf == AF_INET6)
		{
			memset(&pDTKAddr->SA.sin6.sin6_addr,0,16);
			pDTKAddr->SA.sin6.sin6_family = AF_INET6;
			pDTKAddr->SA.sin6.sin6_port = htons(nPort);
		}
		else
		{
			return DTK_ERROR;
		}

		return DTK_OK;
	}

	switch (iAf)
	{
	case AF_INET:
		{
			pDTKAddr->SA.sin4.sin_addr.s_addr = inet_addr(pAddr);
			if (pDTKAddr->SA.sin4.sin_addr.s_addr == INADDR_NONE)
			{
				break;
			}
			pDTKAddr->SA.sin4.sin_family = AF_INET;
			pDTKAddr->SA.sin4.sin_port = htons(nPort);
			iRet = DTK_OK;
		}
		break;
#if defined(OS_SUPPORT_IPV6)
	case AF_INET6:
		{
			if ( DTK_OK == DTK_InetPton_Inter(AF_INET6,pAddr,&pDTKAddr->SA.sin6.sin6_addr) )
			{
				pDTKAddr->SA.sin6.sin6_family = AF_INET6;
				pDTKAddr->SA.sin6.sin6_port = htons(nPort);
				iRet = DTK_OK;
			}
		}
		break;
#endif
	default:
		break;
	}

	return iRet;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr4ByInt(DTK_UINT32 nAddr, DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr)
{
	if ( !pDTKAddr )
	{
		return DTK_ERROR;
	}

	pDTKAddr->SA.sin4.sin_family = AF_INET;
	pDTKAddr->SA.sin4.sin_port = htons(nPort);
	pDTKAddr->SA.sin4.sin_addr.s_addr = nAddr;

	return DTK_OK;
}

#if defined(OS_SUPPORT_IPV6)
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr6ByInt(DTK_UINT8 nAddr[16], DTK_UINT16 nPort, DTK_ADDR_T* pDTKAddr)
{
	if ( !pDTKAddr )
	{
		return DTK_ERROR;
	}

	pDTKAddr->SA.sin6.sin6_family = AF_INET6;
	pDTKAddr->SA.sin6.sin6_port = htons(nPort);
	memcpy(pDTKAddr->SA.sin6.sin6_addr.s6_addr, nAddr, 16);

	return DTK_OK;
} 
#endif

DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeAddr(DTK_VOIDPTR pSockAddr, DTK_INT32 iSockAddrLen, DTK_ADDR_T* pDTKAddr)
{
	if (!pSockAddr || !iSockAddrLen || !pDTKAddr)
	{
		return DTK_ERROR;
	}

	memcpy(&pDTKAddr->SA.sin4,pSockAddr,iSockAddrLen);

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetAddrPort(DTK_ADDR_T* pDTKAddr, DTK_UINT16 nPort)
{
	if (!pDTKAddr)
	{
		return DTK_ERROR;
	}

	pDTKAddr->SA.sin4.sin_port = htons(nPort);
	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrType(DTK_ADDR_T* pDTKAddr)
{
	if (!pDTKAddr)
	{
		return DTK_ERROR;
	}

	return pDTKAddr->SA.sin4.sin_family;
}

DTK_DECLARE const char* CALLBACK DTK_GetAddrString(DTK_ADDR_T* pDTKAddr)
{
	static char szDst[128] = {0};

	return DTK_GetAddrStringEx(pDTKAddr, szDst, sizeof(szDst));
}

DTK_DECLARE const char* CALLBACK DTK_GetAddrStringEx(DTK_ADDR_T* pDTKAddr, char* pAddrBuf, size_t nBuflen)
{
	if ( !pDTKAddr || !pAddrBuf || nBuflen == 0)
	{
		return NULL;
	}

    switch (((PSOCKADDR)&(pDTKAddr->SA))->sa_family)
    {
    case AF_INET:
        return DTK_InetNtop(AF_INET, (unsigned char*)&(pDTKAddr->SA.sin4.sin_addr), pAddrBuf, nBuflen);
#if defined (OS_SUPPORT_IPV6)
    case AF_INET6:
        return DTK_InetNtop(AF_INET6, (unsigned char*)&(pDTKAddr->SA.sin6.sin6_addr), pAddrBuf, nBuflen);
#endif
    default:
        return NULL;
    }
}

DTK_DECLARE DTK_UINT16 CALLBACK DTK_GetAddrPort(DTK_ADDR_T* pDTKAddr)
{
	if (!pDTKAddr)
	{
		return 0;
	}

	if (pDTKAddr->SA.sin4.sin_family == AF_INET)
	{
		return ntohs(pDTKAddr->SA.sin4.sin_port);
	}
	else
	{
		return ntohs(pDTKAddr->SA.sin6.sin6_port);
	}
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddr4Int(DTK_ADDR_T* pDTKAddr, DTK_UINT32* nAddr)
{
	if (!pDTKAddr || pDTKAddr->SA.sin4.sin_family != AF_INET)
	{
		return DTK_ERROR;
	}

	*nAddr = pDTKAddr->SA.sin4.sin_addr.s_addr;
	return DTK_OK;
}

#if defined (OS_SUPPORT_IPV6)
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddr6Int(DTK_ADDR_T* pDTKAddr, DTK_UINT8 nAddr[16])
{
	if (!pDTKAddr || pDTKAddr->SA.sin4.sin_family == AF_INET)
	{
		return DTK_ERROR;
	}

	memcpy(nAddr,&(pDTKAddr->SA.sin6.sin6_addr),16);
	return DTK_OK;
}
#endif

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrInfo(DTK_ADDR_T* pDTKAddr, DTK_ADDR_EXP_T* pDTKExp)
{
	if (!pDTKAddr || !pDTKExp)
	{
		return DTK_ERROR;
	}

	pDTKExp->iAf = pDTKAddr->SA.sin4.sin_family;
	pDTKExp->nPort = ntohs(pDTKAddr->SA.sin4.sin_port);
	if (pDTKExp->iAf == AF_INET)
	{
		pDTKExp->ADDR.nAddr4 = pDTKAddr->SA.sin4.sin_addr.s_addr;
	}
	else
	{
#if defined (OS_SUPPORT_IPV6)
		memcpy(&pDTKExp->ADDR.nAddr6,&(pDTKAddr->SA.sin6.sin6_addr),16);
#endif
	}

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetAddrBySockFd(DTK_SOCK_T iSockFd, DTK_ADDR_T* pLocal, DTK_ADDR_T* pRemote)
{
	DTK_INT32 iRet = DTK_OK;
	int iAddrSize = sizeof(DTK_ADDR_T);

	if ( !pLocal && !pRemote )
	{
		return DTK_ERROR;
	}

	if ( pRemote )
	{
		iRet = getpeername(iSockFd, (PSOCKADDR)&(pRemote->SA.sin4), (socklen_t*)&iAddrSize);
		if (iRet != 0)
		{
			return DTK_ERROR;
		}
	}

	if ( pLocal )
	{
		iRet = getsockname(iSockFd, (PSOCKADDR)&(pLocal->SA.sin4), (socklen_t*)&iAddrSize);
		if (iRet != 0)
		{
			return DTK_ERROR;
		}
	}

	return DTK_OK;
}

/**
 * Function: DTK_InetPton
 * Desc:	将字符串转化为整型(网络字节序)
 * Input:	pSrc: 原字符串
 * Output:	pDst: 目的字符串
 * Return:  0 成功否则失败
 * Others:
 */
DTK_DECLARE DTK_INT32 CALLBACK DTK_InetPton(const char* pSrc, DTK_VOIDPTR pDst)
{
	if (pSrc == NULL || pDst == NULL)
	{
		return DTK_ERROR;
	}

	//此接口兼容IPv6，以地址串中包含':'识别为v6地址，否则为v4地址
	if (strchr(pSrc, ':') == NULL)
	{
		return DTK_InetPton4_Inter(pSrc, pDst) == 1 ? DTK_OK : DTK_ERROR;
	}
	else
	{
		return DTK_InetPton6_Inter(pSrc, pDst) == 1 ? DTK_OK : DTK_ERROR;
	}
}


