#ifndef __DTK_THREAD_H__  
#define __DTK_THREAD_H__  

#include "DTK_Types.h"
#include "DTK_Config.h"

#ifdef OS_WINDOWS
#define DTK_INVALID_THREAD (DTK_HANDLE)(NULL)
#elif defined OS_POSIX
#define DTK_INVALID_THREAD (DTK_HANDLE)(pthread_t)(-1)
#endif


/** @fn DTK_DECLARE DTK_HANDLE CALLBACK DTK_Thread_Create(DTK_VOIDPTR (CALLBACK *StartAddress)(DTK_VOIDPTR), DTK_VOID* Params, DTK_UINT32 StackSize,\
                        DTK_BOOL IsSuspend = DTK_FALSE, DTK_INT32 Priority = 0, DTK_INT32 SchedPolicy = 0);
*   @brief �����߳�
*   @param [in] StartAddress    �߳�ִ����
*   @param [in] Params          ִ�������
*   @param [in] StackSize       �̶߳�ջ��С
*   @param [in] IsSuspend       �Ƿ����
*   @param [in] Priority        �߳����ȼ�
*   @param [in] SchedPolicy     �̵߳��Ȳ���
*   @return �ɹ������߳̾����ʧ�ܷ���DTK_INVALID_HANDLE
*/
DTK_DECLARE DTK_HANDLE CALLBACK DTK_Thread_Create(DTK_VOIDPTR (CALLBACK *StartAddress)(DTK_VOIDPTR), DTK_VOID* Params, DTK_UINT32 StackSize,\
						DTK_BOOL IsSuspend = DTK_FALSE, DTK_INT32 Priority = 0, DTK_INT32 SchedPolicy = 0);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Wait(DTK_HANDLE ThreadHandle)
*   @brief �ȴ��߳��̵߳��˳����ر���Ӧ�ľ��
*   @param [in] ThreadHandl     �߳̾��
*   @return �ɹ�����0������Ϊ������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Wait(DTK_HANDLE ThreadHandle);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Suspend(DTK_HANDLE ThreadHandle)
*   @brief �����߳�
*   @param [in] ThreadHandle    �߳̾��
*   @return �ɹ�����0������Ϊ������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Suspend(DTK_HANDLE ThreadHandle);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Resume(DTK_HANDLE ThreadHandle)
*   @brief �ָ��߳�
*   @param [in] ThreadHandle    �߳̾��
*   @return �ɹ�����0������Ϊ������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Resume(DTK_HANDLE ThreadHandle);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Resume(DTK_HANDLE ThreadHandle)
*   @brief �����̵߳����ȼ�
*   @param [in] ThreadHandle    �߳̾��
*   @param [in] Priority        �߳����ȼ�
*   @return �ɹ�����0������Ϊ������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_SetPriority(DTK_HANDLE ThreadHandle, DTK_INT32 Priority);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_SetSchedPolicy(DTK_HANDLE ThreadHandle, DTK_INT32 SchedPolicy)
*   @brief ���õ��Ȳ���
*   @param [in] ThreadHandle    �߳̾��
*   @param [in] Priority        ���Ȳ���
*   @return �ɹ�����0������Ϊ������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_SetSchedPolicy(DTK_HANDLE ThreadHandle, DTK_INT32 SchedPolicy);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Exit()
*   @brief �˳��߳�
*   @return �ɹ�����0������Ϊ������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Thread_Exit();

/** @fn DTK_DECLARE DTK_HANDLE CALLBACK DTK_Thread_GetSelfId()
*   @brief  ��ȡ�߳�ID
*   @return �߳�ID
*/
DTK_DECLARE DTK_HANDLE CALLBACK DTK_Thread_GetSelfId();

#endif // __DTK_THREAD_H__ 