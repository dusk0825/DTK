
#ifndef __DTK_CONFIG_H__  
#define __DTK_CONFIG_H__  

//÷ß≥÷c++±‡“Î∆˜
#ifdef __cplusplus
#define CXXCOMPILE 1
#elif (defined __linux__)
#define CXXCOMPILE 1
#endif

#if (defined _WIN32 || defined _WIN64)
    #ifndef CALLBACK
    #define CALLBACK __stdcall
    #endif

    #include <ws2tcpip.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <Mswsock.h>

    #define OS_WINDOWS	1
    #define DTK_EXPORTS 1
    #define DTK_INFINITE INFINITE

    #ifdef DTK_STATIC
        #define DTK_DECLARE
        #define DTK_DECLARE_CLASS
        #define DTK_DECLARE_CXX
    #else
        #if defined(DTK_EXPORTS)
            #define DTK_DECLARE extern "C" __declspec(dllexport)
            #define DTK_DECLARE_CLASS __declspec(dllexport)
            #define DTK_DECLARE_CXX __declspec(dllexport)
        #else
            #define DTK_DECLARE extern "C" __declspec(dllimport)
            #define DTK_DECLARE_CLASS __declspec(dllimport)
            #define DTK_DECLARE_CXX __declspec(dllimport)
        #endif
    #endif
#elif (defined __linux__)

    #include <unistd.h>	
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <netdb.h>
    #include <errno.h>
    #include <string.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <net/if.h>
    #include <unistd.h>
    #include <sys/time.h>
    #include <sys/stat.h>
    #include <fcntl.h>

    #define OS_POSIX	1
    #define DTK_INFINITE 0xFFFFFFFF
    #define WAIT_TIMEOUT	258

    #ifdef DTK_STATIC
        #define DTK_DECLARE
    #else
        #define DTK_DECLARE extern "C"
    #endif

    #define DTK_DECLARE_CLASS 
    #define DTK_DECLARE_CXX

    #ifndef CALLBACK
        #define CALLBACK
    #else
        #undef CALLBACK
        #define CALLBACK
    #endif

#endif

#define	DTK_OK			(0)
#define DTK_ERROR		(-1)
#define DTK_NOSUPPORT	(-2)

#define DTK_TIMEOUT     (0)
#define DTK_FOREVER     (-1)

#define OS_SUPPORT_IPV6 1

#endif // __DTK_CONFIG_H__ 