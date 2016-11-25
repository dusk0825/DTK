
#ifndef __DTK_EVENT_H__  
#define __DTK_EVENT_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"
#include "DTK_Mutex.h"

#if defined (OS_WINDOWS)
typedef DTK_HANDLE              DTK_EVENT_T;           
#define DTK_WAIT_OBJECT_0       WAIT_OBJECT_0				//��һ���¼�
#define DTK_WAIT_TIMEOUT		WAIT_TIMEOUT				//��ʱ
#define DTK_WAIT_INVALID_PARAM	((DTK_UINT32)0x7ffffffe)	//��������
#define DTK_WAIT_INVALID_EVENT	((DTK_UINT32)0x7ffffffd)	//��Ч�¼�
#define DTK_WAIT_FAILED			((DTK_UINT32)0x7ffffffc)	//�ȴ�ʧ��
#elif defined (OS_POSIX)
typedef pthread_cond_t          DTK_EVENT_T;
#define DTK_WAIT_OBJECT_0		((DTK_UINT32)0x0)
#define DTK_WAIT_TIMEOUT		((DTK_UINT32)0x7fffffff)
#define DTK_WAIT_INVALID_PARAM	((DTK_UINT32)0x7ffffffe)
#define DTK_WAIT_INVALID_EVENT	((DTK_UINT32)0x7ffffffd)
#define DTK_WAIT_FAILED			((DTK_UINT32)0x7ffffffc)
#endif

/** @fn DTK_DECLARE DTK_EVENT_T CALLBACK DTK_CreateEvent(DTK_BOOL bManualReset, DTK_BOOL bInitState)
*   @brief ����һ���¼����
*   @param [in] pEvent          �¼�ָ��
*   @param [in] bManualReset    �¼��ָ���ʽ��DTK_TRUE���ֶ��ָ���DTK_FALSE���Զ��ָ���
*   @param [in] bInitState      �¼���ʼ״̬��DTK_TRUE������״̬��DTK_FALSE���Ǵ���״̬��
*   @return DTK_TRUE �ɹ���DTK_FALSEʧ��
*/
DTK_DECLARE DTK_BOOL CALLBACK DTK_CreateEvent(DTK_EVENT_T* pEvent, DTK_BOOL bManualReset = DTK_FALSE, DTK_BOOL bInitState = DTK_FALSE);

/** @fn DTK_DECLARE DTK_BOOL CALLBACK DTK_CloseEvent(DTK_EVENT_T hEvent)
*   @brief �ر�һ���¼����
*   @param [in] hEvent  �¼����          
*   @return DTK_TRUE �ɹ���DTK_FALSEʧ��
*/
DTK_DECLARE DTK_BOOL CALLBACK DTK_CloseEvent(DTK_EVENT_T* pEvent);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForSingleObject(DTK_EVENT_T hEvent, DTK_MUTEX_T* pMutex = NULL)
*   @brief �ȴ������¼���Ϣ����
*   @param [in] hEvent  �¼����
*   @param [in] pMutex  ������
*   @return DTK_WAIT_OBJECT_0�ɹ���DTK_WAIT_TIMEOUT��ʱʧ�ܣ�����ֵʧ��
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForSingleObject(DTK_EVENT_T* pEvent, DTK_MUTEX_T* pMutex = NULL);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForSingleObject(DTK_EVENT_T hEvent, DTK_UINT32 nTimeOut)
*   @brief �ȴ������¼���Ϣ����
*   @param [in] hEvent  �¼����
*   @param [in] nTimeOut �ȴ���ʱʱ��
*   @return DTK_WAIT_OBJECT_0�ɹ���DTK_WAIT_TIMEOUT��ʱʧ�ܣ�����ֵʧ��
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForSingleObjectEx(DTK_EVENT_T* pEvent, DTK_UINT32 nTimeOut, DTK_MUTEX_T* pMutex = NULL);

/** @fn DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForMultipleObjects(DTK_UINT32 nEventCount, DTK_EVENT_T* hEvents, DTK_BOOL bWaitAll, DTK_UINT32 nTimeOut)
*   @brief �ȴ�����¼���Ϣ����
*   @param [in] nEventCount �¼�����
*   @param [in] hEvents     �¼��������
*   @param [in] bWaitAll    �Ƿ�ȫ���ȴ�����֧�֣�ֻ�ܵȴ����е�һ���¼�����
*   @param [in] nTimeOut    �ȴ���ʱʱ��
*   @return DTK_WAIT_OBJECT_0�ɹ���DTK_WAIT_TIMEOUT��ʱʧ�ܣ�����ֵʧ��
*/
DTK_DECLARE DTK_UINT32 CALLBACK DTK_WaitForMultipleObjects(DTK_UINT32 nEventCount, DTK_EVENT_T* hEvents, DTK_BOOL bWaitAll, DTK_UINT32 nTimeOut);

/** @fn DTK_DECLARE DTK_BOOL CALLBACK DTK_ResetEvent(DTK_EVENT_T hEvent)
*   @brief �����¼���Ϣ
*   @param [in] hEvent  �¼����
*   @return DTK_TRUE �ɹ���DTK_FALSEʧ��.
*/
DTK_DECLARE DTK_BOOL CALLBACK DTK_ResetEvent(DTK_EVENT_T* pEvent);

/** @fn DTK_DECLARE DTK_BOOL CALLBACK DTK_SetEvent(DTK_EVENT_T hEvent)
*   @brief �����¼���������������һ���ȴ��߳̿��յ����¼����Զ��¼�/�ֶ������¼���
*   @param [in] hEvent  �¼����
*   @return DTK_TRUE �ɹ���DTK_FALSEʧ��.
*   @note ��win�¼���֮ͬ����������������ʱ�����û���̵߳ȴ������Զ���λ��
*/
DTK_DECLARE DTK_BOOL CALLBACK DTK_SetEvent(DTK_EVENT_T* pEvent);

/** @fn DTK_DECLARE DTK_BOOL CALLBACK DTK_SetEventEx(DTK_EVENT_T hEvent)
*   @brief �����¼������������������еȴ��߳̾����յ���
*   @param [in] hEvent  �¼����
*   @return DTK_TRUE �ɹ���DTK_FALSEʧ��.
*/
DTK_DECLARE DTK_BOOL CALLBACK DTK_SetEventEx(DTK_EVENT_T* pEvent);

#endif // __DTK_EVENT_H__ 