
#ifndef __DTK_TYPES_H__  
#define __DTK_TYPES_H__  

//base datatype define
typedef signed char		DTK_INT8;
typedef unsigned char	DTK_UINT8;
typedef signed short	DTK_INT16;
typedef unsigned short	DTK_UINT16;
typedef void*			DTK_VOIDPTR;
typedef signed long		DTK_LONG;
typedef unsigned long	DTK_ULONG;

#if (defined _WIN32 || defined _WIN64)
    #include <winsock2.h>
    #include <windows.h>
    typedef signed int		    DTK_INT32;
    typedef unsigned int	    DTK_UINT32;
    typedef signed __int64      DTK_INT64;
    typedef unsigned __int64    DTK_UINT64;
    typedef HANDLE			    DTK_HANDLE;
#elif (defined __linux__)
    #include <stdint.h>
    typedef int32_t             DTK_INT32;
    typedef uint32_t            DTK_UINT32;
    typedef int64_t             DTK_INT64;
    typedef uint64_t            DTK_UINT64;
    typedef void*               DTK_HANDLE;
#endif

#ifndef DTK_BOOL
    #define DTK_BOOL DTK_INT32
    #define DTK_TRUE  1
    #define DTK_FALSE 0
#endif

#define DTK_VOID void
#define DTK_INVALID_HANDLE NULL


#endif // __DTK_TYPES_H__ 