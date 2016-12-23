
#ifndef __DTK_SELECT_H__  
#define __DTK_SELECT_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

#if defined (OS_POSIX)
#include <poll.h>
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Select(DTK_INT32 iMaxFd, fd_set* readFds, fd_set* writeFds, fd_set* errFds, timeval* tvTime)
*   @brief 轮询多个文件描述符状态
*   @param [in] iMaxFd      最大文件描述符+1
*   @param [in] readFds     可读文件句柄集合
*   @param [in] writeFds    可写文件句柄集合
*   @param [in] errFds      异常文件句柄集合
*   @param [in] tvTime      超时时间
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Select(DTK_INT32 iMaxFd, fd_set* readFds, fd_set* writeFds, fd_set* errFds, timeval* tvTime); 

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_SelectEx(DTK_INT32 iMaxFd, fd_set* readFds, fd_set* writeFds, fd_set* errFds, timeval* tvTime)
*   @brief 轮询多个文件描述符状态，返回轮询等待时间
*   @param [in] iMaxFd      最大文件描述符+1
*   @param [in] readFds     可读文件句柄集合
*   @param [in] writeFds    可写文件句柄集合
*   @param [in] errFds      异常文件句柄集合
*   @param [in] tvTime      超时时间
*   @return 0 成功, -1 失败
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_SelectEx(DTK_INT32 iMaxFd, fd_set* readFds, fd_set* writeFds, fd_set* errFds, timeval* tvTime);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_Poll(struct pollfd* fds, DTK_INT32 iFds, DTK_INT32 iTimeO)
*   @brief 轮询多个文件描述符状态
*   @param [in] fds         文件描述符相关信息
*   @param [in] iFds        文件描述符数量
*   @param [in] iTimeO      超时时间
*   @return -1 失败，0 超时，大于0 可读文件描述符个数
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_Poll(struct pollfd* fds, DTK_INT32 iFds, DTK_INT32 iTimeO);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_PollEx(struct pollfd* fds, DTK_INT32 iFds, DTK_INT32* iTimeO)
*   @brief 轮询多个文件描述符状态，返回轮询等待时间
*   @param [in] fds         文件描述符相关信息
*   @param [in] iFds        文件描述符数量
*   @param [in] iTimeO      超时时间
*   @return -1 失败，0 超时，大于0 可读文件描述符个数
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_PollEx(struct pollfd* fds, DTK_INT32 iFds, DTK_INT32* iTimeO);


#endif // __DTK_SELECT_H__ 