// 
// #include <unistd.h>
// #include <sys/types.h>      
// #include <sys/socket.h>      
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <sys/epoll.h>
// #include <sys/ioctl.h>
// #include <fcntl.h>
// #include <sys/resource.h>
// #include <stdlib.h>
// #include <errno.h>
// #include <stdio.h>
// #include <string.h>
// 
// #define PORT            5556
// #define MAXEPOLLSIZE    2048
// #define MAXLINE         1024
// 
// int main(int argc, char **argv)
// {
//     struct sockaddr_in stSrvAddr;
//     bzero(&stSrvAddr, sizeof(stSrvAddr));
//     stSrvAddr.sin_family = AF_INET; 
//     stSrvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     stSrvAddr.sin_port = htons(PORT);
// 
//     int iListenFd = socket(AF_INET, SOCK_STREAM, 0); 
//     if (iListenFd == -1) 
//     {
//         perror("can't create socket file");
//         return -1;
//     }
// 
//     int iOpt = 1;
//     setsockopt(iListenFd, SOL_SOCKET, SO_REUSEADDR, &iOpt, sizeof(iOpt));
// 
//     if (ioctl(iListenFd, FIONBIO, &iOpt) < 0)
//     {
//         perror("ioctl");
//         return -1;
//     }
// 
//     if (bind(iListenFd, (struct sockaddr *) &stSrvAddr, sizeof(struct sockaddr)) == -1) 
//     {
//         perror("bind error");
//         return -1;
//     } 
//     if (listen(iListenFd, 32) == -1) 
//     {
//         perror("listen error");
//         return -1;
//     }
// 
//     struct epoll_event stEvent;
//     struct epoll_event arrEvent[MAXEPOLLSIZE];
// 
//     int iEpollFd = epoll_create(MAXEPOLLSIZE);
//     if (iEpollFd < 0)
//     {
//         perror("epoll_create");
//         return -1;
//     }
// 
//     stEvent.events = EPOLLIN | EPOLLET;
//     stEvent.data.fd = iListenFd;
//     if (epoll_ctl(iEpollFd, EPOLL_CTL_ADD, iListenFd, &stEvent) < 0) 
//     {
//         perror("epoll_ctl error");
//         return -1;
//     }
// 
//     printf("start listen port = %d\n", PORT);
// 
//     int iMaxEvent = 256;
//     while (1)
//     {
//         int iFds = epoll_wait(iEpollFd, arrEvent, iMaxEvent, 30 * 1000);
//         if (iFds == -1)
//         {
//             perror("epoll_wait");
//             continue;
//         }
// 
//         for (int i = 0; i < iFds; ++i)
//         {
//             if (arrEvent[i].data.fd == iListenFd) 
//             {
//                 struct sockaddr_in stCltAddr;
//                 socklen_t iSockLen = sizeof(struct sockaddr_in);
//                 int iConnFd = accept(iListenFd, (struct sockaddr *)&stCltAddr, &iSockLen);
//                 if (iConnFd < 0) 
//                 {
//                     perror("accept error");
//                     continue;
//                 }
// 
//                 printf("accept form %s:%d, fd = %d\n", inet_ntoa(stCltAddr.sin_addr), stCltAddr.sin_port, iConnFd);
// 
//                 ioctl(iConnFd, FIONBIO, &iOpt);
//                 stEvent.events = EPOLLIN /*| EPOLLET*/;
//                 stEvent.data.fd = iConnFd;
//                 if (epoll_ctl(iEpollFd, EPOLL_CTL_ADD, iConnFd, &stEvent) < 0)
//                 {
//                     fprintf(stderr, "add socket '%d' to epoll failed: %s\n", iConnFd, strerror(errno));
//                     return -1;
//                 }
//             }
//             else
//             {
//                 char szBuf[MAXLINE] = {0};
//                 int iRead = read(arrEvent[i].data.fd, szBuf, MAXLINE);
//                 if (iRead <= 0)
//                 {
//                     printf("fd = %d close the connection\n",arrEvent[i].data.fd);
//                     close(arrEvent[i].data.fd);
//                     epoll_ctl(iEpollFd, EPOLL_CTL_DEL, arrEvent[i].data.fd, &stEvent);
//                     continue;
//                 }
// 
//                 printf("recv client fd = %d, msg = %s\n", arrEvent[i].data.fd, szBuf);
//                 write(arrEvent[i].data.fd, szBuf, iRead);
//             }
//         }
//     }
// 
//     close(iListenFd);
//     close(iEpollFd);
//     return 0;
// }

#include "MainDemo.h"

int main(void)
{
    DTK_Init();
    DTK_Fini();
    return 0;
}
