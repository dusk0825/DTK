
#ifndef __DTK_SELECT_H__  
#define __DTK_SELECT_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

#if defined (OS_POSIX)
#include <poll.h>
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Select(DTK_INT32 iMaxFd, fd_set* readFds, fd_set* writeFds, fd_set* errFds, timeval* tvTime)
*   @brief ��ѯ����ļ�������״̬
*   @param [in] iMaxFd      ����ļ�������+1
*   @param [in] readFds     �ɶ��ļ��������
*   @param [in] writeFds    ��д�ļ��������
*   @param [in] errFds      �쳣�ļ��������
*   @param [in] tvTime      ��ʱʱ��
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Select(DTK_INT32 iMaxFd, fd_set* readFds, fd_set* writeFds, fd_set* errFds, timeval* tvTime); 

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SelectEx(DTK_INT32 iMaxFd, fd_set* readFds, fd_set* writeFds, fd_set* errFds, timeval* tvTime)
*   @brief ��ѯ����ļ�������״̬��������ѯ�ȴ�ʱ��
*   @param [in] iMaxFd      ����ļ�������+1
*   @param [in] readFds     �ɶ��ļ��������
*   @param [in] writeFds    ��д�ļ��������
*   @param [in] errFds      �쳣�ļ��������
*   @param [in] tvTime      ��ʱʱ��
*   @return 0 �ɹ�, -1 ʧ��
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SelectEx(DTK_INT32 iMaxFd, fd_set* readFds, fd_set* writeFds, fd_set* errFds, timeval* tvTime);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Poll(struct pollfd* fds, DTK_INT32 iFds, DTK_INT32 iTimeO)
*   @brief ��ѯ����ļ�������״̬
*   @param [in] fds         �ļ������������Ϣ
*   @param [in] iFds        �ļ�����������
*   @param [in] iTimeO      ��ʱʱ��
*   @return -1 ʧ�ܣ�0 ��ʱ������0 �ɶ��ļ�����������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Poll(struct pollfd* fds, DTK_INT32 iFds, DTK_INT32 iTimeO);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_PollEx(struct pollfd* fds, DTK_INT32 iFds, DTK_INT32* iTimeO)
*   @brief ��ѯ����ļ�������״̬��������ѯ�ȴ�ʱ��
*   @param [in] fds         �ļ������������Ϣ
*   @param [in] iFds        �ļ�����������
*   @param [in] iTimeO      ��ʱʱ��
*   @return -1 ʧ�ܣ�0 ��ʱ������0 �ɶ��ļ�����������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_PollEx(struct pollfd* fds, DTK_INT32 iFds, DTK_INT32* iTimeO);


#endif // __DTK_SELECT_H__ 