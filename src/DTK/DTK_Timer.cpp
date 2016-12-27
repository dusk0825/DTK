
#include "DTK_Timer.h"
#include "DTK_Mutex.h"
#include "DTK_Thread.h"
#include "DTK_Event.h"

#define MAX_TIMER_EVENT         1000    //定时器最大数量
#define DTK_TIMER_TYPE_MM	    0       //多媒体时钟
#define DTK_TIMER_TYPE_ASYN     1       //异步时钟

#ifdef OS_WINDOWS
#pragma comment(lib,"Winmm.lib")

typedef struct DTK_TIMER_EVENT_INTER
{
    DTK_BOOL		bUsed;			//是否被使用
    DTK_INT32	    hTimerEvent;	//定时器索引号,返回给应用层
    DTK_CBF_Timer	pCbf;			//用户回调函数
    DTK_VOIDPTR		pUsrData;		//用户自定义数据
    DTK_UINT32		nTimeOut;		//定时器定时触发时间间隔
    DTK_INT32		iType;			//定时器类型
    union
    {
        DTK_HANDLE	hASynTimer;		//异步定时器
        MMRESULT	hMMTimer;		//多媒体时钟定时器
    }DTK_TIMER_U;

    //异步时钟专有成员
    DTK_HANDLE		hQuitEvent;		//退出事件
    DTK_HANDLE      hThread;		//线程句柄
    DTK_HANDLE		hSynSuccEvent;	//异步成功事件
}DTK_TIMER_EVENT_INTER;

static DTK_TIMER_EVENT_INTER    g_stTimerEvents[MAX_TIMER_EVENT];

#endif

static DTK_BOOL                 g_bTimerInit = DTK_FALSE;
static DTK_MUTEX_T              g_timerMutex;

/** @fn DTK_INT32 DTK_InitTimer_Inter()
*   @brief 初始化定时器资源
*   @return 成功返回0，失败-1
*/
DTK_INT32 DTK_InitTimer_Inter()
{
    if (g_bTimerInit)
    {
        return DTK_OK;
    }

    if (DTK_MutexCreate(&g_timerMutex))
    {
        return DTK_ERROR;
    }

#ifdef OS_WINDOWS
    for (int i=0; i < MAX_TIMER_EVENT; i++)
    {
        memset(&g_stTimerEvents[i], 0, sizeof(DTK_TIMER_EVENT_INTER));
    }
#endif

    g_bTimerInit = DTK_TRUE;

    return DTK_OK;
}

/** @fn DTK_INT32 DTK_FiniTimer_Inter()
*   @brief 销毁定时器资源
*   @return 成功返回0，失败-1
*/
DTK_INT32 DTK_FiniTimer_Inter()
{
    g_bTimerInit = DTK_FALSE;
    return DTK_MutexDestroy(&g_timerMutex);
}

/** @fn 
*   @brief 多媒体时钟使用定时器回调函数
*   @param [in] dwUser      定时器内部索引
*   @param [in] uTimerID,uMsg,dw1,dw2: 无效
*   @return 
*/
#ifdef OS_WINDOWS
static void CALLBACK DTK_MM_OnTimer_Inter(DTK_UINT32 uTimerID, DTK_UINT32 uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	if ( g_stTimerEvents[dwUser].pCbf )
	{
		g_stTimerEvents[dwUser].pCbf(g_stTimerEvents[dwUser].hTimerEvent,g_stTimerEvents[dwUser].pUsrData);
	}
}

static void* CALLBACK DTK_ASYN_OnTimer_Inter(void* pParam)
{
	DTK_TIMER_EVENT_INTER* pEvent = (DTK_TIMER_EVENT_INTER*)(pParam);
    if (NULL == pEvent)
    {
        return NULL;
    }

	LARGE_INTEGER liDueTime;
    liDueTime.QuadPart= -10000;
	if (!SetWaitableTimer(pEvent->DTK_TIMER_U.hASynTimer,&liDueTime,pEvent->nTimeOut,NULL,NULL,TRUE))
	{
		return 0;
	}

	DTK_SetEvent(&pEvent->hSynSuccEvent);

	DTK_HANDLE hEvents[2];
	hEvents[0] = pEvent->DTK_TIMER_U.hASynTimer;//异步定时器事件
	hEvents[1] = pEvent->hQuitEvent;//退出事件

	while (1)
	{
		if (DTK_WAIT_OBJECT_0 != DTK_WaitForMultipleObjects(2,hEvents,DTK_FALSE,DTK_INFINITE))
		{
			DTK_ResetEvent(&hEvents[1]);
			break;
		}

		if (pEvent->pCbf)
		{
			pEvent->pCbf(pEvent->hTimerEvent, pEvent->pUsrData);
		}
	}

	return 0;
}
#endif

DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTimer(DTK_CBF_Timer pCbf, DTK_VOIDPTR pUsrData,DTK_UINT32 nTimeOut, DTK_INT32* hEvent, DTK_BOOL bFlag)
{
#ifdef OS_WINDOWS
	int i = 0;
	DTK_MutexLock(&g_timerMutex);
	for (; i < MAX_TIMER_EVENT; i++)
	{
		if (!g_stTimerEvents[i].bUsed)
		{
			break;
		}
	}
    if (i >= MAX_TIMER_EVENT)
    {
        DTK_MutexUnlock(&g_timerMutex);
        return DTK_ERROR;
    }

    g_stTimerEvents[i].hTimerEvent = i;
    g_stTimerEvents[i].pCbf = pCbf;
    g_stTimerEvents[i].pUsrData = pUsrData;
    g_stTimerEvents[i].nTimeOut = nTimeOut;
    g_stTimerEvents[i].iType = DTK_TIMER_TYPE_MM;
    g_stTimerEvents[i].DTK_TIMER_U.hMMTimer = NULL;

    if (bFlag)
    {
        g_stTimerEvents[i].DTK_TIMER_U.hMMTimer = timeSetEvent(nTimeOut,0,DTK_MM_OnTimer_Inter,i,TIME_PERIODIC);
    }

    if (NULL == g_stTimerEvents[i].DTK_TIMER_U.hMMTimer)//如果创建多媒体时钟失败,创建异步时钟
    {
        timeBeginPeriod(1);

        DTK_HANDLE hAsynTimer = CreateWaitableTimer(NULL,FALSE,NULL);
        if (hAsynTimer == NULL)
        {
            DTK_MutexUnlock(&g_timerMutex);
           return DTK_ERROR;
        }

        g_stTimerEvents[i].iType = DTK_TIMER_TYPE_ASYN;
        g_stTimerEvents[i].DTK_TIMER_U.hASynTimer = hAsynTimer;
        DTK_CreateEvent(&g_stTimerEvents[i].hSynSuccEvent, DTK_FALSE, DTK_FALSE);
        DTK_CreateEvent(&g_stTimerEvents[i].hQuitEvent, DTK_TRUE, DTK_FALSE);
        g_stTimerEvents[i].hThread = DTK_Thread_Create(DTK_ASYN_OnTimer_Inter, &g_stTimerEvents[i], 0);
        if (DTK_INVALID_THREAD == g_stTimerEvents[i].hThread)
        {
            DTK_CloseEvent(&g_stTimerEvents[i].hSynSuccEvent);
            DTK_CloseEvent(&g_stTimerEvents[i].DTK_TIMER_U.hASynTimer);
            DTK_CloseEvent(&g_stTimerEvents[i].hQuitEvent);
            g_stTimerEvents[i].pCbf = NULL;
            g_stTimerEvents[i].pUsrData = NULL;
            DTK_MutexUnlock(&g_timerMutex);
            return DTK_ERROR;
        }

        if(DTK_WAIT_TIMEOUT == DTK_WaitForSingleObjectEx(&g_stTimerEvents[i].hSynSuccEvent, 3*1000))
        {
            //启动定时器失败
            DTK_SetEvent(&g_stTimerEvents[i].hQuitEvent);
            DTK_CloseEvent(&g_stTimerEvents[i].hSynSuccEvent);
            DTK_CloseEvent(&g_stTimerEvents[i].DTK_TIMER_U.hASynTimer);
            DTK_Thread_Wait(g_stTimerEvents[i].hThread);
            DTK_CloseEvent(&g_stTimerEvents[i].hQuitEvent);
            g_stTimerEvents[i].pCbf = NULL;
            g_stTimerEvents[i].pUsrData = NULL;
            DTK_MutexUnlock(&g_timerMutex);
            return DTK_ERROR;
        }
    }

    *hEvent = i;
    g_stTimerEvents[i].bUsed = DTK_TRUE;
	DTK_MutexUnlock(&g_timerMutex);
#endif

	return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_KillTimer(DTK_INT32 hEvent)
{
    DTK_BOOL bRet = DTK_TRUE;
#ifdef OS_WINDOWS
    if (hEvent >= MAX_TIMER_EVENT)
    {
        return DTK_ERROR;
    }
	
	DTK_TIMER_EVENT_INTER* pTimerEvent = &g_stTimerEvents[hEvent];
    if (NULL == pTimerEvent)
    {
        return DTK_ERROR;
    }
	
	if (pTimerEvent->iType == DTK_TIMER_TYPE_MM)
	{
		//多媒体时钟仅需timeKillEvent
		if (TIMERR_NOERROR != timeKillEvent(pTimerEvent->DTK_TIMER_U.hMMTimer))
		{
			bRet = FALSE;
		}
	}
	else
	{
		DTK_SetEvent(&pTimerEvent->hQuitEvent);
		DTK_CloseEvent(&pTimerEvent->DTK_TIMER_U.hASynTimer);
		DTK_CloseEvent(&pTimerEvent->hQuitEvent);
		DTK_CloseEvent(&pTimerEvent->hSynSuccEvent);
		DTK_Thread_Wait(pTimerEvent->hThread);

		timeEndPeriod(1);
	}

	DTK_MutexLock(&g_timerMutex);
	pTimerEvent->bUsed = DTK_FALSE;
	pTimerEvent->pCbf = NULL;
	pTimerEvent->pUsrData = NULL;
	DTK_MutexUnlock(&g_timerMutex);
#endif
	return bRet?DTK_OK:DTK_ERROR;
}