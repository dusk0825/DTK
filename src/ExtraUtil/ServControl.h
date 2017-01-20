
#ifndef __SERVCONTROL_H__  
#define __SERVCONTROL_H__  

#include <Windows.h>
#include <winsvc.h>

typedef int (__stdcall StartServiceCallBack)(void* pUsrData);
typedef int (__stdcall StopServiceCallBack)(void* pUsrData);

/** @fn int ServiceEntry(int argc, char* argv[], char* pSvrName, StartServiceCallBack pfnStartSvr, StopServiceCallBack pfnStopSvr, void* pUsrData)
*   @brief ����װ�����
*   @param [in] argc                ��������
*   @param [in] argv                ��������
*   @param [in] pSvrName            ��������
*   @param [in] pfnStartSvr         ��������ʱ�Ļص�����
*   @param [in] pfnStopSvr          �����˳�ʱ�Ļص�����
*   @param [in] pUsrData            �û��Զ�������
*   @return �ɹ�����0��ʧ��-1
*/
int ServiceEntry(int argc, char* argv[], char* pSvrName, StartServiceCallBack pfnStartSvr, StopServiceCallBack pfnStopSvr, void* pUsrData);

/** @fn int InstallService(const char* pSvrName, const char* pSvrDisplayName)
*   @brief ��װWindows����
*   @param [in] pSvrName        ��������
*   @param [in] pSvrDisplayName ������ʾ����
*   @return �ɹ�����0��ʧ��-1
*/
int InstallService(const char* pSvrName, const char* pSvrDisplayName);

/** @fn int UnInstallService(const char* pSvrName)
*   @brief ж��Windows����
*   @param [in] pSvrName        ��������
*   @return �ɹ�����0��ʧ��-1
*/
int UnInstallService(const char* pSvrName);

/** @fn int StartServiceWin(const char* pSvrName)
*   @brief ��������
*   @param [in] pSvrName        ��������
*   @return �ɹ�����0��ʧ��-1
*/
int StartServiceWin(const char* pSvrName);

/** @fn int StopService(const char* pSvrName)
*   @brief ֹͣ����
*   @param [in] pSvrName        ��������
*   @return �ɹ�����0��ʧ��-1
*/
int StopService(const char* pSvrName);

/** @fn int RestartService(const char* pSvrName)
*   @brief ��������
*   @param [in] pSvrName        ��������
*   @return �ɹ�����0��ʧ��-1
*/
int RestartService(const char* pSvrName);

#endif // __SERVCONTROL_H__ 

