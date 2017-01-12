//SyncTimeByNtp.cpp
//Ntp校时客户端源代码

#include "SyncTimeByNtp.h"
#include "DTK_Socket.h"
#include "DTK_Netdb.h"
#include "DTK_Error.h"

#include <time.h>
#include <stdlib.h>

#ifdef _WIN32
#include <sys/timeb.h>
#else
#include   <unistd.h>
#include   <sys/time.h>
#endif

#define JAN_1970      0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */

#define NTPFRAC(x) (4294 * (x) + ((1981 * (x))>>11))
#define USEC(x) (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))

//ntp时间戳结构体
struct ntptime 
{
    DTK_UINT32 coarse;
    DTK_UINT32 fine;
};

//1.准备通信环境，创建绑定socket
DTK_INT32 PrepareSocket(DTK_SOCK_T& iSockFd, DTK_ADDR_T& struLocalAddr);
//2.发送校时信息到NTP服务器
DTK_INT32 SendMsg2Ntp(DTK_SOCK_T& iSockFd, DTK_ADDR_T& struToAddr);
//3.接收NTP对校时信令的响应
DTK_INT32 RecvMsgFromNtpSvr(DTK_SOCK_T& iSockFd, DTK_UINT32* buf, DTK_INT32 len);
//4.从响应的消息中获取同步时间戳
DTK_INT32 GetSyncTimeStamp(DTK_UINT32* buf, struct timeval* syncTime);
//5.调用系统API同步本地时间
DTK_INT32 SyncLocalTime(struct timeval* syncTime);

//NTP校时入口(sNtpIp:服务器IP地址 nNtpPort:服务器端口)
DTK_INT32 SyncTimeByNtp(const std::string& sNtpIp, DTK_INT32 nNtpPort)
{
    int ret = DTK_OK;
    DTK_SOCK_T iSockFd;
    DTK_ADDR_T struToAddr;
    DTK_ADDR_T struLocalAddr;

    DTK_MakeAddrByString(AF_INET, sNtpIp.c_str(), (DTK_UINT16)nNtpPort, &struToAddr);
    DTK_MakeAddr4ByInt(0, 0, &struLocalAddr);

    do 
    {
        ret = PrepareSocket(iSockFd, struLocalAddr);
        if (DTK_OK != ret)
        {
            LOG_WARN("Prepare socket failed");
            break;
        }

        ret = SendMsg2Ntp(iSockFd, struToAddr);
        if (DTK_OK != ret)
        {
            LOG_WARN("Send sync time msg to ntp server failed");
            break;
        }

        DTK_UINT32 buf[12] = {0};
        ret = RecvMsgFromNtpSvr(iSockFd, buf, sizeof(buf));
        if (DTK_OK != ret)
        {
            LOG_WARN("Recv sync time msg from ntp server failed");
            break;
        }

        struct timeval syncTime = {0};
        GetSyncTimeStamp(buf, &syncTime);

        ret = SyncLocalTime(&syncTime);
        if (DTK_OK != ret)
        {
            LOG_WARN("Sync local time failed");
            break;
        }

        LOG_INFO("Sync time by ntp server succ");
    } while (false);

    DTK_CloseSocket(iSockFd);
    return ret;
}

//准备通信环境，创建绑定socket
DTK_INT32 PrepareSocket(DTK_SOCK_T& iSockFd, DTK_ADDR_T& struLocalAddr)
{
    DTK_INT32 iTimeOut = 3000;
    iSockFd = DTK_CreateSocket(AF_INET, SOCK_DGRAM, 0);
    if (DTK_INVALID_SOCKET == iSockFd)
    {
        LOG_ERROR("Create socket errno = %d", DTK_GetSocketLastError());
        return DTK_ERROR;
    }

    if (DTK_OK != DTK_SetTimeOut(iSockFd, iTimeOut, iTimeOut))
    {
        LOG_ERROR("Set timeout error, errno = %d", DTK_GetSocketLastError());
        return DTK_ERROR;
    }

    if (DTK_OK != DTK_Bind(iSockFd,&struLocalAddr))
    {
        LOG_ERROR("Bind socket error, errno = %d", DTK_GetSocketLastError());
        return DTK_ERROR;
    }

    return DTK_OK;
}

//发送校时信息到NTP服务器
DTK_INT32 SendMsg2Ntp(DTK_SOCK_T& iSockFd, DTK_ADDR_T& struToAddr)
{
#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4 
#define PREC -6

    DTK_UINT32 data[12];
    memset((char*)data, 0, sizeof(data));
    struct timeval nowTime;

    data[0] = DTK_Htonl((LI << 30) | (VN << 27) | (MODE << 24) | 
        (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));
    data[1] = DTK_Htonl(1 << 16);  /* Root Delay (seconds) */
    data[2] = DTK_Htonl(1 << 16);  /* Root Dispersion (seconds) */

#ifdef _WIN32
    struct _timeb tb;
    _ftime(&tb);
    nowTime.tv_sec = (long)tb.time;
    nowTime.tv_usec = tb.millitm * 1000;
#else
    gettimeofday(&nowTime, NULL);
#endif    

    data[10] = DTK_Htonl(nowTime.tv_sec + JAN_1970);     /* Transmit Timestamp coarse */
    data[11] = DTK_Htonl(NTPFRAC(nowTime.tv_usec));      /* Transmit Timestamp fine   */

    if (DTK_ERROR == DTK_SendTo(iSockFd, (DTK_VOIDPTR)data, (DTK_INT32)sizeof(data), &struToAddr))
    {
        LOG_ERROR("Send data to ntp server error, errno = %d", DTK_GetSocketLastError());
        return DTK_ERROR;
    }

    return DTK_OK;
}

//接收NTP对校时信令的响应
DTK_INT32 RecvMsgFromNtpSvr(DTK_SOCK_T& iSockFd, DTK_UINT32* buf, DTK_INT32 len)
{
    DTK_ADDR_T struToAddr;
    memset(&struToAddr,0,sizeof(struToAddr));

    int recvLen = DTK_RecvFrom(iSockFd, (DTK_VOIDPTR)buf, len, &struToAddr);
    if (recvLen  <= 0)
    {
        LOG_ERROR("Recv data from ntp server error, errno = %d", DTK_GetSocketLastError());
        return DTK_ERROR;
    }

    return DTK_OK;
}

//从响应的消息中获取同步时间戳
DTK_INT32 GetSyncTimeStamp(DTK_UINT32* buf, struct timeval* syncTime)
{
    struct ntptime nowNtpTime = {0};
    struct timeval nowTime = {0};

#ifdef _WIN32
    struct _timeb tb;
    _ftime(&tb);
    nowTime.tv_sec = (long)tb.time;
    nowTime.tv_usec = tb.millitm * 1000;
#else
    gettimeofday(&nowTime, NULL);
#endif    

    nowNtpTime.coarse = nowTime.tv_sec + JAN_1970;
    nowNtpTime.fine   = NTPFRAC(nowTime.tv_usec);

    struct ntptime oritime, rectime, tratime;
    struct timeval offtime;

#define Data(i) DTK_Ntohl(buf[i])
    oritime.coarse  = Data(6); 
    oritime.fine    = Data(7); 
    rectime.coarse  = Data(8); 
    rectime.fine    = Data(9); 
    tratime.coarse  = Data(10); 
    tratime.fine    = Data(11);
#undef Data

#define  MKSEC(ntpt)        ((ntpt).coarse - JAN_1970) 
#define  MKUSEC(ntpt)       (USEC((ntpt).fine)) 
#define  TTLUSEC(sec, usec) ((long long)(sec)*1000000 + (usec)) 
#define  GETSEC(us)         ((us) / 1000000)  
#define  GETUSEC(us)        ((us) % 1000000)  

    long long  orius, recus, traus, desus, offus; 

    orius = TTLUSEC(MKSEC(oritime), MKUSEC(oritime)); 
    recus = TTLUSEC(MKSEC(rectime), MKUSEC(rectime)); 
    traus = TTLUSEC(MKSEC(tratime), MKUSEC(tratime)); 
    desus = TTLUSEC(nowTime.tv_sec, nowTime.tv_usec); 
    offus = ((recus - orius) + (traus - desus))/2; 

    offtime.tv_sec  = (long)GETSEC(offus); 
    offtime.tv_usec = (long)GETUSEC(offus); 

    //粗略校时 
    //new.tv_sec = tratime.integer - JAN_1970; 
    //new.tv_usec = USEC(tratime.fraction); 

    //精确校时 
    syncTime->tv_sec = nowNtpTime.coarse - JAN_1970 + offtime.tv_sec; 
    syncTime->tv_usec = USEC(nowNtpTime.fine) + offtime.tv_usec;

    //防止微秒为负数导致设置失败
    if (syncTime->tv_usec < 0)
    {
        syncTime->tv_sec--;
        syncTime->tv_usec += 1000000;
    }
    else if (syncTime->tv_usec >= 1000000)
    {
        syncTime->tv_sec++;
        syncTime->tv_usec -= 1000000;
    }

    return DTK_OK;
}

//调用系统API同步本地时间
DTK_INT32 SyncLocalTime(struct timeval* syncTime)
{
#ifdef _WIN32
    time_t t = syncTime->tv_sec;
    struct tm* timePara = localtime(&t);
    SYSTEMTIME localST;
    memset(&localST, 0, sizeof(SYSTEMTIME));
    localST.wYear = (WORD)(timePara->tm_year + 1900);
    localST.wMonth = (WORD)(timePara->tm_mon + 1);
    localST.wDay = (WORD)(timePara->tm_mday);
    localST.wHour = (WORD)(timePara->tm_hour);
    localST.wMinute = (WORD)(timePara->tm_min);
    localST.wSecond = (WORD)(timePara->tm_sec);
    localST.wMilliseconds = (WORD)(syncTime->tv_usec / 1000);
    if (TRUE != SetLocalTime(&localST))
    {
        LOG_ERROR("Set local time failed, err=%d", GetLastError());
        return DTK_ERROR;
    }
#else
    /* need root user. */
    if (0 != getuid() && 0 != geteuid())
    {
        LOG_ERROR("Not root, can not set local time");
        return DTK_ERROR;
    }
    if (-1 == settimeofday(syncTime, NULL))
    {
        LOG_ERROR("Set time of day failed, err=%d", (DTK_UINT32)errno);
        return DTK_ERROR;
    }
#endif

    return DTK_OK;    
}
