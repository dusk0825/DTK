#ifndef _SyncTimeByNtp_h_
#define _SyncTimeByNtp_h_

#include "DTK_Dtk.h"
#include "DTK_LogModule.h"
#include <string>

/** @fn:    DTK_INT32 SyncTimeByNtp(const std::string& sNtpIp, DTK_INT32 nNtpPort)
 *  @brief: Ntp校时客户端程序
 *  param [in] sNtpIp: Ntp服务器ip地址
 *  param [out] NULL: Ntp服务器端口
 *  return: DTK_OK 成功，DTK_ERROR 失败
 */
DTK_INT32 SyncTimeByNtp(const std::string& sNtpIp, DTK_INT32 nNtpPort);

#endif // _SyncTimeByNtp_h_
 