
#ifndef __SERVCONTROL_H__  
#define __SERVCONTROL_H__  

#include <Windows.h>
#include <winsvc.h>

typedef int (__stdcall StartServiceCallBack)(void* pUsrData);
typedef int (__stdcall StopServiceCallBack)(void* pUsrData);

/** @fn int ServiceEntry(int argc, char* argv[], char* pSvrName, StartServiceCallBack pfnStartSvr, StopServiceCallBack pfnStopSvr, void* pUsrData)
*   @brief 服务安装的入口
*   @param [in] argc                参数个数
*   @param [in] argv                参数数组
*   @param [in] pSvrName            服务名称
*   @param [in] pfnStartSvr         服务启动时的回调函数
*   @param [in] pfnStopSvr          服务退出时的回调函数
*   @param [in] pUsrData            用户自定义数据
*   @return 成功返回0，失败-1
*/
int ServiceEntry(int argc, char* argv[], char* pSvrName, StartServiceCallBack pfnStartSvr, StopServiceCallBack pfnStopSvr, void* pUsrData);

/** @fn int InstallService(const char* pSvrName, const char* pSvrDisplayName)
*   @brief 安装Windows服务
*   @param [in] pSvrName        服务名称
*   @param [in] pSvrDisplayName 服务显示名称
*   @return 成功返回0，失败-1
*/
int InstallService(const char* pSvrName, const char* pSvrDisplayName);

/** @fn int UnInstallService(const char* pSvrName)
*   @brief 卸载Windows服务
*   @param [in] pSvrName        服务名称
*   @return 成功返回0，失败-1
*/
int UnInstallService(const char* pSvrName);

/** @fn int StartServiceWin(const char* pSvrName)
*   @brief 启动服务
*   @param [in] pSvrName        服务名称
*   @return 成功返回0，失败-1
*/
int StartServiceWin(const char* pSvrName);

/** @fn int StopService(const char* pSvrName)
*   @brief 停止服务
*   @param [in] pSvrName        服务名称
*   @return 成功返回0，失败-1
*/
int StopService(const char* pSvrName);

/** @fn int RestartService(const char* pSvrName)
*   @brief 重启服务
*   @param [in] pSvrName        服务名称
*   @return 成功返回0，失败-1
*/
int RestartService(const char* pSvrName);

#endif // __SERVCONTROL_H__ 

