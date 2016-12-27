
#include "DTK_Timer.h"
#include "DTK_Mutex.h"
#include "DTK_Thread.h"
#include "DTK_Event.h"

#define MAX_TIMER_EVENT         1000    //��ʱ���������
#define DTK_TIMER_TYPE_MM	    0       //��ý��ʱ��
#define DTK_TIMER_TYPE_ASYN     1       //�첽ʱ��

#ifdef OS_WINDOWS
#pragma comment(lib,"Winmm.lib")

typedef struct DTK_TIMER_EVENT_INTER
{
    DTK_BOOL		bUsed;			//�Ƿ�ʹ��
    DTK_INT32	    hTimerEvent;	//��ʱ��������,���ظ�Ӧ�ò�
    DTK_CBF_Timer	pCbf;			//�û��ص�����
    DTK_VOIDPTR		pUsrData;		//�û��Զ�������
    DTK_UINT32		nTimeOut;		//��ʱ����ʱ����ʱ����
    DTK_INT32		iType;			//��ʱ������
    union
    {
        DTK_HANDLE	hASynTimer;		//�첽��ʱ��
        MMRESULT	hMMTimer;		//��ý��ʱ�Ӷ�ʱ��
    }DTK_TIMER_U;

    //�첽ʱ��ר�г�Ա
    DTK_HANDLE		hQuitEvent;		//�˳��¼�
    DTK_HANDLE      hThread;		//�߳̾��
    DTK_HANDLE		hSynSuccEvent;	//�첽�ɹ��¼�
}DTK_TIMER_EVENT_INTER;

static DTK_TIMER_EVENT_INTER    g_stTimerEvents[MAX_TIMER_EVENT];

#endif

static DTK_BOOL                 g_bTimerInit = DTK_FALSE;
static DTK_MUTEX_T              g_timerMutex;

/** @fn DTK_INT32 DTK_InitTimer_Inter()
*   @brief ��ʼ����ʱ����Դ
*   @return �ɹ�����0��ʧ��-1
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
*   @brief ���ٶ�ʱ����Դ
*   @return �ɹ�����0��ʧ��-1
*/
DTK_INT32 DTK_FiniTimer_Inter()
{
    g_bTimerInit = DTK_FALSE;
    return DTK_MutexDestroy(&g_timerMutex);
}

/** @fn 
*   @brief ��ý��ʱ��ʹ�ö�ʱ���ص�����
*   @param [in] dwUser      ��ʱ���ڲ�����
*   @param [in] uTimerID,uMsg,dw1,dw2: ��Ч
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
	hEvents[0] = pEvent->DTK_TIMER_U.hASynTimer;//�첽��ʱ���¼�
	hEvents[1] = pEvent->hQuitEvent;//�˳��¼�

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

    if (NULL == g_stTimerEvents[i].DTK_TIMER_U.hMMTimer)//���������ý��ʱ��ʧ��,�����첽ʱ��
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
            //������ʱ��ʧ��
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
		//��ý��ʱ�ӽ���timeKillEvent
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