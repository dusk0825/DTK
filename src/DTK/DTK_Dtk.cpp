
#include "DTK_Dtk.h"
#include "DTK_Guard.h"

extern DTK_INT32 DTK_InitNetwork_Inter();
extern DTK_INT32 DTK_FiniNetwork_Inter();

extern DTK_INT32 StartLogService(void);
extern DTK_INT32 StopLogService();

extern DTK_INT32 DTK_InitTimer_Inter();
extern DTK_INT32 DTK_FiniTimer_Inter();

extern DTK_INT32 DTK_InitThreadPool_Inter();
extern DTK_VOID DTK_FiniThreadPool_Inter();

static DTK_Mutex s_DTKmutex;
static DTK_INT32 s_iInitCount = 0;
static DTK_BOOL  s_bDTKInitNetwork = DTK_FALSE;

static DTK_INT32 DTK_InitResource_Inter();
static DTK_VOID DTK_ClearResource_Inter();

#if defined (OS_WINDOWS)
#include <Iphlpapi.h>

HMODULE g_hIphlpapiDll = NULL;

typedef DWORD (CALLBACK * pfGetAdapterInfo)(PIP_ADAPTER_INFO, PULONG);
typedef ULONG (CALLBACK * pfGetAdapterAddr)(ULONG, ULONG, PVOID, PIP_ADAPTER_ADDRESSES, PULONG);

pfGetAdapterInfo g_pInfoFunc = NULL;
pfGetAdapterAddr g_pAddrFunc = NULL;

extern DTK_INT32 DTK_AIOInit_Inter();
extern DTK_VOID DTK_AIOFini_Inter();

#endif


static DTK_INT32 DTK_InitResource_Inter()
{
    s_bDTKInitNetwork = DTK_FALSE;

    do 
    {
        StartLogService();
        DTK_InitTimer_Inter();
        DTK_InitThreadPool_Inter(); 
        if ( DTK_InitNetwork_Inter() )
        {
            break;
        }
        
#if defined (OS_WINDOWS)
        g_hIphlpapiDll = LoadLibrary(TEXT("Iphlpapi.dll"));
        if (g_hIphlpapiDll != NULL)
        {
            g_pInfoFunc = (pfGetAdapterInfo)GetProcAddress(g_hIphlpapiDll, "GetAdaptersInfo");
            g_pAddrFunc = (pfGetAdapterAddr)GetProcAddress(g_hIphlpapiDll, "GetAdaptersAddresses");
        }

        DTK_AIOInit_Inter();
#endif

        s_bDTKInitNetwork = DTK_TRUE;

        return DTK_OK;
    } while (0);
    
    DTK_ClearResource_Inter();
    return DTK_ERROR;
}

static DTK_VOID DTK_ClearResource_Inter()
{
    if (s_bDTKInitNetwork)
    {
        DTK_FiniNetwork_Inter();
        DTK_FiniThreadPool_Inter();
        DTK_FiniTimer_Inter();
        StopLogService();

#if defined (OS_WINDOWS)

        DTK_AIOFini_Inter();

        if (g_hIphlpapiDll != NULL)
        {
            FreeLibrary(g_hIphlpapiDll);
            g_hIphlpapiDll = NULL;
        }
#endif

        s_bDTKInitNetwork = DTK_FALSE;
    }

    return;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Init()
{
    DTK_Guard guard(&s_DTKmutex);

    if (0 == s_iInitCount)
    {
        if (DTK_OK != DTK_InitResource_Inter())
        {
            return DTK_ERROR;
        }
    }

    ++ s_iInitCount;
    return DTK_OK;
}

DTK_DECLARE DTK_VOID CALLBACK DTK_Fini()
{
    DTK_Guard guard(&s_DTKmutex);

    -- s_iInitCount;
    if (s_iInitCount == 0)
    {
        DTK_ClearResource_Inter();
    }
}