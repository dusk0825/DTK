
#ifndef __DTK_THREADPOOL_H__  
#define __DTK_THREADPOOL_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

/** @fn DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPool_Create(DTK_UINT32 InitThreadNum, DTK_UINT32 MaxThreadNum, DTK_UINT32 StackSize = 0)
*   @brief �����̳߳�
*   @param [in] InitThreadNum   ��ʼ�߳���
*   @param [in] MaxThreadNum    ����߳���
*   @param [in] StackSize       �̶߳�ջ��С
*   @param [in] TimeOut         �߳̿����˳�ʱ��
*   @return �ɹ������̳߳ؾ����ʧ�ܷ���NULL
*/
DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPool_Create(DTK_UINT32 InitThreadNum, DTK_UINT32 MaxThreadNum, DTK_UINT32 StackSize = 0);
DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPool_CreateFlex(DTK_UINT32 InitThreadNum, DTK_UINT32 MaxThreadNum, DTK_UINT32 StackSize, DTK_UINT32 TimeOut);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_Destroy(DTK_HANDLE ThreadPoolHandle)
*   @brief �����̳߳�
*   @param [in] ThreadPoolHandle    �̳߳ؾ��
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_Destroy(DTK_HANDLE ThreadPoolHandle);

/** @fn typedef DTK_VOIDPTR (CALLBACK *DTK_ThreadPool_WorkRoutine)(DTK_VOIDPTR)
*   @brief �̳߳�����ִ���庯��ָ��
*   @return ��
*/
typedef DTK_VOIDPTR (CALLBACK *DTK_ThreadPool_WorkRoutine)(DTK_VOIDPTR);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_Work(DTK_HANDLE ThreadPoolHandle, DTK_VOIDPTR(CALLBACK *WorkRoutine)(DTK_VOIDPTR), DTK_VOIDPTR Params)
*   @brief Ͷ�������̳߳�
*   @param [in] ThreadPoolHandle    �̳߳ؾ��
*   @param [in] WorkRoutine         ����ִ����ָ��
*   @param [in] Params              �������ָ��
*   @param [in] bWaitForIdle        �޿����߳�ʱ���Ƿ�ȴ�
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_Work(DTK_HANDLE ThreadPoolHandle, DTK_ThreadPool_WorkRoutine pfnWork, DTK_VOIDPTR Params);
DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPool_WorkEx(DTK_HANDLE ThreadPoolHandle, DTK_ThreadPool_WorkRoutine pfnWork, DTK_VOIDPTR Params, DTK_BOOL bWaitForIdle);

#endif // __DTK_THREADPOOL_H__ 