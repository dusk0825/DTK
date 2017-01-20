
#include "ServControl.h"
#include <process.h>
#include <stdio.h>

static char g_szSvrName[512] = {0};
void* g_pUsrData = NULL;
StartServiceCallBack* g_pfnStartSvr = NULL;
StopServiceCallBack* g_pfnStopSvr = NULL;

static char* g_szSvrState[] = {"null","Stopped","Start Pending","Stop Pending","Running","Continue Pending","Pause Pending","Paused"};

HANDLE                  g_hSvrControlThd = NULL;
SERVICE_STATUS          ssStatus;
SERVICE_STATUS_HANDLE   sshStatusHandle;

unsigned __stdcall ServiceControlThdCallBack(void* pParam)
{
    if (NULL != g_pfnStartSvr)
    {
        g_pfnStartSvr(g_pUsrData);
    }

    return 0;
}

BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    if (dwCurrentState == SERVICE_START_PENDING)
    {
        ssStatus.dwControlsAccepted = 0;
    }  
    else
    {
        ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    }

    ssStatus.dwCurrentState = dwCurrentState;
    ssStatus.dwWin32ExitCode = dwWin32ExitCode;
    ssStatus.dwWaitHint = dwWaitHint;

    if ((dwCurrentState == SERVICE_RUNNING ) || (dwCurrentState == SERVICE_STOPPED))
    {
        ssStatus.dwCheckPoint = 0;
    }  
    else
    {
        ssStatus.dwCheckPoint = dwCheckPoint++;
    }

    // Report the status of the service to the service control manager.
    if (!SetServiceStatus(sshStatusHandle, &ssStatus)) 
    {
        printf("SetServiceStatus = %d fail, err = %d\n", dwCurrentState, GetLastError());
        return FALSE;
    }
    return TRUE;
}

VOID WINAPI ServiceControlLocal(DWORD dwCtrlCode)
{
    switch(dwCtrlCode)
    {
        // Stop the service.
        //
        // SERVICE_STOP_PENDING should be reported before
        // setting the Stop Event - hServerStopEvent - in
        // ServiceStop().  This avoids a race condition
        // which may result in a 1053 - The Service did not respond...
        // error.
    case SERVICE_CONTROL_STOP:
        ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);

        //do stop work
        if (NULL != g_pfnStopSvr)
        {
            g_pfnStopSvr(g_pUsrData);

            //需要报告停止状态，否则提示无法停止
            while (!ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0))
            {
                Sleep(1000);
            }
        }
        return;

        // Update the service status.
        //
    case SERVICE_CONTROL_INTERROGATE:
        break;

        // invalid control code
        //
    default:
        break;

    }

    ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
}

void WINAPI LocalServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    sshStatusHandle = RegisterServiceCtrlHandler(g_szSvrName, ServiceControlLocal);
    if (NULL == sshStatusHandle)
    {
        return;
    }

    ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ssStatus.dwServiceSpecificExitCode = 0;

    // service state
    // exit code
    // wait hint
    if (!ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 3000))                 
    {
        return;
    }

    //2分钟内需要报告服务状态，否则SCM认为启动失败，故异步回调
    g_hSvrControlThd = (HANDLE)_beginthreadex(NULL, 0, ServiceControlThdCallBack, NULL, 0, NULL);

    ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);

    return;

    // try to report the stopped status to the service control manager.
//     if (NULL != sshStatusHandle)
//     {
//         ReportStatusToSCMgr(SERVICE_STOPPED,0,0);
//     }
}

int ServiceEntry(int argc, char* argv[], char* pSvrName, StartServiceCallBack pfnStartSvr, StopServiceCallBack pfnStopSvr, void* pUsrData)
{
    if (NULL == pSvrName)
    {
        return -1;
    }

    SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { pSvrName, (LPSERVICE_MAIN_FUNCTION)LocalServiceMain },
        { NULL, NULL }
    };

    if ( (argc > 1) &&
        ((*argv[1] == '-') || (*argv[1] == '/')) )
    {
        if ( _stricmp( "install", argv[1]+1 ) == 0 )
        {
            InstallService(pSvrName, pSvrName);
            exit(0);
        }
        else if ( _stricmp( "uninstall", argv[1]+1 ) == 0 )
        {
            UnInstallService(pSvrName);
            exit(0);
        }
        else if ( _stricmp( "start", argv[1]+1 ) == 0 )
        {
            StartServiceWin(pSvrName);
        }
        else if ( _stricmp( "stop", argv[1]+1 ) == 0 )
        {
            StopService(pSvrName);
        }
        else if ( _stricmp( "restart", argv[1]+1 ) == 0 )
        {
            RestartService(pSvrName);
        }
        else
        {
            goto dispatch;
        }
        exit(0);
    }

dispatch:
    printf( "-install      to install the service\n");
    printf( "-uninstall    to uninstall the service\n");
    printf( "-start        to start the service\n");
    printf( "-stop         to stop the service\n");
    printf( "-restart      to restart the service\n");
    printf( "\nStartServiceCtrlDispatcher being called.\n");
    printf( "This may take several seconds.  Please wait.\n");

    strcpy(g_szSvrName, pSvrName);
    g_pUsrData = pUsrData;
    g_pfnStartSvr = pfnStartSvr;
    g_pfnStopSvr = pfnStopSvr;

    if (!StartServiceCtrlDispatcher(dispatchTable))
    {
        return -1;
    }

    return 0;
}

int InstallService(const char* pSvrName, const char* pSvrDisplayName)
{
    if (NULL == pSvrName || NULL == pSvrDisplayName)
    {
        return -1;
    }

    char szPath[512] = {0};
    if (GetModuleFileName(NULL, szPath, 512) == 0)
    {
        return -1;
    }

    // machine (NULL == local)
    // database (NULL == default)
    // access required
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == schSCManager)
    {
        return -1;
    }

    SC_HANDLE schService = CreateService(
        schSCManager,               // SCManager database
        pSvrName,                   // name of service
        pSvrDisplayName,            // name to display
        SERVICE_ALL_ACCESS,         // desired access
        SERVICE_WIN32_OWN_PROCESS,//|SERVICE_INTERACTIVE_PROCESS,  // service type
        SERVICE_AUTO_START,         // start type
        SERVICE_ERROR_NORMAL,       // error control type
        szPath,                     // service's binary
        NULL,                       // no load ordering group
        NULL,                       // no tag identifier
        NULL,                       // dependencies
        NULL,                       // LocalSystem account
        NULL);                      // no password

    if (NULL == schService)
    {
        printf("install service %s fail\n", pSvrName);
        CloseServiceHandle(schSCManager);
        return -1;
    }

    printf("install service %s suc\n", pSvrName);
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return 0;
}

int UnInstallService(const char* pSvrName)
{
    int iRet = -1;
    SC_HANDLE schService = NULL;
    SC_HANDLE schSCManager = NULL;

    if (NULL == pSvrName)
    {
        return -1;
    }

    do 
    {
        schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (NULL == schSCManager)
        {
            break;
        }

        schService = OpenService(schSCManager, pSvrName, SERVICE_ALL_ACCESS);
        if (NULL == schService)
        {
            break;
        }

        SERVICE_STATUS ssStatus;
        if (FALSE == ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus))
        {
            //break;
        }

        while (QueryServiceStatus(schService, &ssStatus))
        {
            if (ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
            {  
                break;
            }
            printf("wait service %s stop, state is %d", pSvrName, ssStatus.dwCurrentState);
            Sleep(1000);
        }

        if (ssStatus.dwCurrentState == SERVICE_STOPPED)
        {
            printf("stop service %s suc.\n", pSvrName);
        }
        else
        {
            printf("stop service %s fail.\n", pSvrName);
        }

        if (DeleteService(schService))
        {
            iRet = 0;
            printf("uninstall service %s suc.\n", pSvrName);
        }
        else
        {
            printf("uninstall service %s fail\n", pSvrName);
        }

    } while (0);

    if (NULL != schService)
    {
        CloseServiceHandle(schService);
    }
    if (NULL != schSCManager)
    {
        CloseServiceHandle(schSCManager);
    }

    return iRet;
}

int StartServiceWin(const char* pSvrName)
{
    int iRet = -1;
    SC_HANDLE schService = NULL;
    SC_HANDLE schSCManager = NULL;

    if (NULL == pSvrName)
    {
        return -1;
    }

    do 
    {
        schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (NULL == schSCManager)
        {
            break;
        }

        schService = OpenService(schSCManager, pSvrName, SERVICE_ALL_ACCESS);
        if (NULL == schService)
        {
            break;
        }

        SERVICE_STATUS stStatus;
        QueryServiceStatus(schService,&stStatus);
        if (SERVICE_START_PENDING == stStatus.dwCurrentState
            || SERVICE_RUNNING == stStatus.dwCurrentState)
        {
            printf("service %s is %s\n", pSvrName, g_szSvrState[stStatus.dwCurrentState]);
            iRet = 0;
            break;
        }
        else if (SERVICE_STOPPED == stStatus.dwCurrentState
            || SERVICE_PAUSED == stStatus.dwCurrentState)
        {
            StartService(schService, 0, NULL);
            QueryServiceStatus(schService, &stStatus);
            if (SERVICE_RUNNING != stStatus.dwCurrentState 
                && SERVICE_START_PENDING != stStatus.dwCurrentState)
            {
                printf("start service %s fail, state is %s\n", pSvrName, g_szSvrState[stStatus.dwCurrentState]);
                break;
            }

            printf("start service %s suc\n", pSvrName);
            iRet = 0;
        }
        else if (SERVICE_STOP_PENDING == stStatus.dwCurrentState
            || SERVICE_CONTINUE_PENDING == stStatus.dwCurrentState
            || SERVICE_PAUSE_PENDING == stStatus.dwCurrentState)
        {
            //最多等待30秒
            int iTryCount = 30;
            while (iTryCount--)
            {
                QueryServiceStatus(schService, &stStatus);
                if (SERVICE_STOP_PENDING != stStatus.dwCurrentState 
                    && SERVICE_CONTINUE_PENDING != stStatus.dwCurrentState
                    && SERVICE_PAUSE_PENDING != stStatus.dwCurrentState)
                {
                    break;
                }
                printf("waiting for start, service state is %s\n", g_szSvrState[stStatus.dwCurrentState]);
                Sleep(1000);
            }

            if (iTryCount > 0)
            {
                StartService(schService, 0, NULL);
                QueryServiceStatus(schService, &stStatus);
                if (SERVICE_RUNNING != stStatus.dwCurrentState)
                {
                    printf("start service %s fail\n", pSvrName);
                    break;
                }

                printf("start service %s suc\n", pSvrName);
                iRet = 0;
            }
            else
            {
                printf("start service %s fail\n", pSvrName);
            }
        }
    } while (0);

    if (NULL != schService)
    {
        CloseServiceHandle(schService);
    }
    if (NULL != schSCManager)
    {
        CloseServiceHandle(schSCManager);
    }

    return iRet;
}

int StopService(const char* pSvrName)
{
    int iRet = -1;
    SC_HANDLE schService = NULL;
    SC_HANDLE schSCManager = NULL;

    if (NULL == pSvrName)
    {
        return -1;
    }

    do 
    {
        schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (NULL == schSCManager)
        {
            break;
        }

        schService = OpenService(schSCManager, pSvrName, SERVICE_ALL_ACCESS);
        if (NULL == schService)
        {
            break;
        }

        SERVICE_STATUS stStatus;
        QueryServiceStatus(schService,&stStatus);
        if (SERVICE_RUNNING == stStatus.dwCurrentState)
        {
            while (stStatus.dwCurrentState != SERVICE_STOPPED)
            {
                if (!ControlService(schService, SERVICE_CONTROL_STOP, &stStatus))
                {
                    printf("ControlService fail, err = %d\n", GetLastError());
                }
                Sleep(1000);
                QueryServiceStatus(schService, &stStatus);
                printf("waiting for stop, service state is %s\n", g_szSvrState[stStatus.dwCurrentState]);
            }

            printf("stop service %s suc\n", pSvrName);
            iRet = 0;
        }
    } while (0);

    if (NULL != schService)
    {
        CloseServiceHandle(schService);
    }
    if (NULL != schSCManager)
    {
        CloseServiceHandle(schSCManager);
    }

    return iRet;
}

int RestartService(const char* pSvrName)
{
    int iRet = -1;
    SC_HANDLE schService = NULL;
    SC_HANDLE schSCManager = NULL;

    if (NULL == pSvrName)
    {
        return -1;
    }

    do 
    {
        schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (NULL == schSCManager)
        {
            break;
        }

        schService = OpenService(schSCManager, pSvrName, SERVICE_ALL_ACCESS);
        if (NULL == schService)
        {
            break;
        }

        SERVICE_STATUS stStatus;
        QueryServiceStatus(schService,&stStatus);
        if (SERVICE_RUNNING == stStatus.dwCurrentState)
        {
            while (stStatus.dwCurrentState != SERVICE_STOPPED)
            {
                if (!ControlService(schService, SERVICE_CONTROL_STOP, &stStatus))
                {
                    printf("ControlService fail, err = %d\n", GetLastError());
                }
                Sleep(1000);
                QueryServiceStatus(schService, &stStatus);
                printf("waiting for restart, service state is %s\n", g_szSvrState[stStatus.dwCurrentState]);
            }

            StartService(schService, 0, NULL);
            QueryServiceStatus(schService, &stStatus);
            if (SERVICE_RUNNING != stStatus.dwCurrentState)
            {
                printf("start service %s fail\n", pSvrName);
                break;
            }

            printf("start service %s suc\n", pSvrName);
            iRet = 0;
        }
    } while (0);

    if (NULL != schService)
    {
        CloseServiceHandle(schService);
    }
    if (NULL != schSCManager)
    {
        CloseServiceHandle(schSCManager);
    }

    return iRet;
}