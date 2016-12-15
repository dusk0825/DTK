
#ifndef __DTK_ERROR_H__  
#define __DTK_ERROR_H__  

#include "DTK_Types.h"
#include "DTK_Config.h"

#define DTK_ERROR_BASE 40000

#define DTK_EPERM		(DTK_ERROR_BASE+1)	//权限不足
#define DTK_EBADARG		(DTK_ERROR_BASE+2)	//参数不合法
#define DTK_EDISORDER	(DTK_ERROR_BASE+3)	//调用顺序不对
#define DTK_ENORESOURCE	(DTK_ERROR_BASE+4)	//资源不足
#define DTK_ENOMEM		(DTK_ERROR_BASE+5)	//没有内存
#define DTK_EWOULDBLOCK	(DTK_ERROR_BASE+6)	//暂时不能完成，需要重试
#define DTK_ETIMEO		(DTK_ERROR_BASE+7)	//超时
#define DTK_EEOF		(DTK_ERROR_BASE+8)	//EOF
#define DTK_EBUSY		(DTK_ERROR_BASE+9)	//系统忙
#define DTK_EINIT		(DTK_ERROR_BASE+10)	//初始化错误
#define DTK_ENOTFOUND	(DTK_ERROR_BASE+11)	//没有找到
#define DTK_EBADPATH	(DTK_ERROR_BASE+12)	//路径错误
#define DTK_EBADIP		(DTK_ERROR_BASE+13)	//不合法的IP地址
#define DTK_EBADMASK	(DTK_ERROR_BASE+14)	//无效掩码
#define DTK_EINVALIDSOCK (DTK_ERROR_BASE+15)//无效套接字
#define DTK_ESOCKET		(DTK_ERROR_BASE+16)	//套接字出错
#define DTK_ESOCKCLOSE	(DTK_ERROR_BASE+17)	//套接字关闭


DTK_DECLARE DTK_UINT32 CALLBACK DTK_GetLastError();

DTK_DECLARE DTK_VOID   CALLBACK DTK_SetLastError(DTK_UINT32 uError);

DTK_DECLARE DTK_UINT32 CALLBACK DTK_GetSocketLastError();

#endif // __DTK_ERROR_H__ 