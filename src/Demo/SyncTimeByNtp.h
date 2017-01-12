#ifndef _SyncTimeByNtp_h_
#define _SyncTimeByNtp_h_

#include "DTK_Dtk.h"
#include "DTK_LogModule.h"
#include <string>

/** @fn:    DTK_INT32 SyncTimeByNtp(const std::string& sNtpIp, DTK_INT32 nNtpPort)
 *  @brief: NtpУʱ�ͻ��˳���
 *  param [in] sNtpIp: Ntp������ip��ַ
 *  param [out] NULL: Ntp�������˿�
 *  return: DTK_OK �ɹ���DTK_ERROR ʧ��
 */
DTK_INT32 SyncTimeByNtp(const std::string& sNtpIp, DTK_INT32 nNtpPort);

#endif // _SyncTimeByNtp_h_
 