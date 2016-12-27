
#ifndef __DTK_TIMER_H__  
#define __DTK_TIMER_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

/*
��ʱ��������
��ʱ�����ö�ý��ʱ��+�첽ʱ�ӵ�ģʽ,��ý��ʱ������ϵͳ�������֧��16����
�����ý��ʱ�ӷ���ʧ�ܣ�������첽ʱ��

ʱ��ѡ��DTK_SetTimer() bFlag=DTK_FALSE��ʹ�ö�ý��ʱ��

��ʱ�����ȣ���ý��ʱ��1ms,�첽ʱ��1ms.

���ƣ���ʱ�����1000��
*/

typedef DTK_VOIDPTR (* DTK_CBF_Timer)(DTK_INT32 hEvent, DTK_VOIDPTR pUsrData);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTimer(DTK_CBF_Timer pCbf, DTK_VOIDPTR pUsrData,DTK_UINT32 nTimeOut, DTK_INT32* hEvent, DTK_BOOL bFlag)
*   @brief ��ʼ����ʱ��
*   @param [in] pCbf        �û��ص�����ָ��
*   @param [in] pUsrData    �û��Զ�������
*   @param [in] nTimeOut    ��ʱ���������(��λms)
*   @param [in] hEvent      ��ʱ������
*   @param [in] bFlag       true����ý��ʱ�ӣ�false���첽ʱ��
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SetTimer(DTK_CBF_Timer pCbf, DTK_VOIDPTR pUsrData,DTK_UINT32 nTimeOut, DTK_INT32* hEvent, DTK_BOOL bFlag = DTK_FALSE);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_KillTimer(DTK_INT32 hEvent)
*   @brief ���ٶ�ʱ��
*   @param [in] hEvent      ��ʱ������,DTK_SetTimer���óɹ��ķ���ֵ
*   @return �ɹ�����0��ʧ��-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_KillTimer(DTK_INT32 hEvent); 


#endif // __DTK_TIMER_H__ 