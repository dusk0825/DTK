
#ifndef __DTK_THREADPOOLFLEX_H__  
#define __DTK_THREADPOOLFLEX_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

/** @fn DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPoolFlex_Create(DTK_UINT32 nMaxThreadNum, DTK_UINT32 nIldeTime, DTK_VOIDPTR(CALLBACK *fWorkRoutine)(DTK_VOIDPTR), DTK_UINT32 nStackSize = 0)
*   @brief �����̳߳�
*   @param [in] nMaxThreadNum   �̳߳�����߳���
*   @param [in] nIldeTime       �߳̿����˳�ʱ�䣬�Ժ���ƣ���������ʱ��û�������߳��Զ��˳�
*   @param [in] fWorkRoutine    �߳�ִ������
*   @param [in] nStackSize      �̶߳�ջ��С�����ֽڼƣ�0ΪĬ�ϴ�С
*   @return �ɹ������̳߳ؾ����ʧ��NULL
*/
DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPoolFlex_Create(DTK_UINT32 nMaxThreadNum, DTK_UINT32 nIldeTime, DTK_VOIDPTR(CALLBACK *fWorkRoutine)(DTK_VOIDPTR), DTK_UINT32 nStackSize = 0);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPoolFlex_Destroy(DTK_HANDLE hHandle)
*   @brief �����̳߳�
*   @param [in] hHandle     �̳߳ؾ��
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPoolFlex_Destroy(DTK_HANDLE hHandle);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPoolFlex_Work(DTK_HANDLE hHandle, DTK_VOIDPTR pParam)
*   @brief Ͷ����������
*   @param [in] hHandle     �̳߳ؾ��
*   @param [in] pParam      ��������
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPoolFlex_Work(DTK_HANDLE hHandle, DTK_VOIDPTR pParam);

#endif // __DTK_THREADPOOLFLEX_H__ 