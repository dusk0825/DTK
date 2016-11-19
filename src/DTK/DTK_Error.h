
#ifndef __DTK_ERROR_H__  
#define __DTK_ERROR_H__  

#include "DTK_Types.h"
#include "DTK_Config.h"

#define DTK_ERROR_BASE 40000

#define DTK_EPERM		(DTK_ERROR_BASE+1)	//Ȩ�޲���
#define DTK_EBADARG		(DTK_ERROR_BASE+2)	//�������Ϸ�
#define DTK_EDISORDER	(DTK_ERROR_BASE+3)	//����˳�򲻶�
#define DTK_ENORESOURCE	(DTK_ERROR_BASE+4)	//��Դ����
#define DTK_ENOMEM		(DTK_ERROR_BASE+5)	//û���ڴ�
#define DTK_EWOULDBLOCK	(DTK_ERROR_BASE+6)	//��ʱ������ɣ���Ҫ����
#define DTK_ETIMEO		(DTK_ERROR_BASE+7)	//��ʱ
#define DTK_EEOF		(DTK_ERROR_BASE+8)	//EOF
#define DTK_EBUSY		(DTK_ERROR_BASE+9)	//ϵͳæ
#define DTK_EINIT		(DTK_ERROR_BASE+10)	//��ʼ������
#define DTK_ENOTFOUND	(DTK_ERROR_BASE+11)	//û���ҵ�
#define DTK_EBADPATH	(DTK_ERROR_BASE+12)	//·������
#define DTK_EBADIP		(DTK_ERROR_BASE+13)	//���Ϸ���IP��ַ
#define DTK_EBADMASK	(DTK_ERROR_BASE+14)	//��Ч����
#define DTK_EINVALIDSOCK (DTK_ERROR_BASE+15)//��Ч�׽���
#define DTK_ESOCKET		(DTK_ERROR_BASE+16)	//�׽��ֳ���
#define DTK_ESOCKCLOSE	(DTK_ERROR_BASE+17)	//�׽��ֹر�


DTK_DECLARE DTK_UINT32 CALLBACK DTK_GetLastError();

DTK_DECLARE DTK_VOID   CALLBACK DTK_SetLastError(DTK_UINT32 uError);

#endif // __DTK_ERROR_H__ 