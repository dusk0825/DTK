
#include "DTK_Utils.h"
#include "DTK_Time.h"

#ifdef OS_WINDOWS
#pragma comment(lib, "Rpcrt4.lib")
#elif OS_POSIX
#include <iconv.h>
//#include <uuid/uuid.h>
#endif


DTK_DECLARE DTK_VOID CALLBACK DTK_Sleep(DTK_INT32 millisecond)
{
#if defined(OS_WINDOWS)
    Sleep(millisecond);
#else
    usleep(millisecond*1000);
#endif
}

DTK_DECLARE DTK_VOID CALLBACK DTK_USleep(DTK_INT64 nUSec)
{
#if defined(OS_WINDOWS)
    Sleep((DWORD)nUSec/1000);
#else
    usleep(nUSec);
#endif
}

DTK_DECLARE DTK_UINT32 CALLBACK DTK_Rand()
{
#if defined (OS_WINDOWS)
    srand(DTK_GetTimeTick());
    return rand();
#else
    srandom(DTK_GetTimeTick());
    return random();
#endif
}

DTK_DECLARE DTK_VOID CALLBACK DTK_ZeroMemory(void* pMem, int iLen)
{
    memset(pMem, 0, iLen);
    return;
}

DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebug(const char* format, ...)
{
    va_list al;
    va_start(al, format);

    char szDebug[5 * 1024] = {0};
    int retval = DTK_Vsnprintf(szDebug, sizeof(szDebug) - 2, format, al);
    szDebug[retval] = '\n';
    szDebug[retval + 1] = '\0';

#if defined (OS_WINDOWS)
    OutputDebugString((LPCSTR)szDebug);

    //wchar_t wDebug[5 * 1024] = {0};
    //MultiByteToWideChar(CP_ACP, 0, szDebug, strlen(szDebug), wDebug, sizeof(wDebug));
    //OutputDebugString(wDebug);
#elif defined (OS_POSIX)
    fprintf(stderr, "%s", szDebug);
#endif

    va_end(al);
}

DTK_DECLARE DTK_VOID CALLBACK DTK_OutputDebugString(const char* lpOutputString)
{
#if defined OS_WINDOWS
    OutputDebugString(lpOutputString);

    //int len = strlen(lpOutputString);
    //需要多分配一个wchar_t用于放置结束符
    //wchar_t *tmp = (wchar_t *)calloc(len + sizeof(wchar_t), sizeof(wchar_t));
    //if (tmp == NULL)
    //{
    //    return;
    //}
    //MultiByteToWideChar(CP_ACP, 0, lpOutputString, len, tmp, len * sizeof(wchar_t));
    //tmp[len] = 0;
    //OutputDebugString(tmp);
    //free(tmp);
#elif defined OS_POSIX
    fprintf(stderr, "%s", lpOutputString);
#endif
}

#if defined (CXXCOMPILE)
DTK_DECLARE_CXX std::string CALLBACK DTK_UTF82A(const char* cont)
{
#ifdef OS_WINDOWS
    if (NULL == cont)
    {
        return std::string("");
    }
    int num = MultiByteToWideChar(CP_ACP, NULL, cont, -1, NULL, NULL);
    wchar_t* buffw = new wchar_t[num];
    MultiByteToWideChar(CP_ACP, NULL, cont, -1, buffw, num);
    int len = WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, NULL, NULL, NULL, NULL); 
    char* lpsz = new char[len + 1]; 
    WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, lpsz, len, NULL, NULL);
    lpsz[len]='\0';
    delete[] buffw;
    std::string rtn(lpsz);
    delete[] lpsz;
    return rtn ;
#elif defined OS_POSIX
    char* outbuf = 0;
    std::string str;
    if (cont)
    {
        iconv_t cd = iconv_open("EUC-CN", "UTF-8");
        if (cd)
        {
            size_t contlen = strlen(cont);
            size_t outbuflen = contlen * 3 + 1;
            outbuf = new char[outbuflen];
            memset(outbuf, 0, outbuflen);

            char* inptr = const_cast<char*>(cont);
            char* outptr = outbuf;

            size_t inlen = contlen;
            size_t outlen = outbuflen;
            if (iconv(cd, &inptr, &inlen, &outptr, &outlen) == (size_t)(-1))
            {
                outbuf = 0;
            }

            iconv_close(cd);
            str = outbuf;
            delete[] outbuf;
        }
    }
    return str;
#else
    return std::string("");
#endif
}

DTK_DECLARE_CXX std::string CALLBACK DTK_A2UTF8(const char* cont)
{
    if (NULL == cont)
    {
        return std::string("");
    }

#ifdef OS_WINDOWS
    int num = MultiByteToWideChar(CP_UTF8, NULL, cont, -1, NULL, NULL);
    wchar_t* buffw = new wchar_t[num];
    MultiByteToWideChar(CP_UTF8, NULL, cont, -1, buffw, num);

    int len = WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, NULL, NULL, NULL, NULL); 
    char* lpsz = new char[len + 1]; 
    WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, lpsz, len, NULL, NULL);
    lpsz[len]='\0';
    delete[] buffw;

    std::string rtn(lpsz);
    delete[] lpsz;
    return rtn;
#elif defined OS_POSIX
    char* outbuf = 0;
    std::string str;
    if (cont)
    {
        iconv_t cd = iconv_open("UTF-8", "EUC-CN");
        if (cd)
        {
            size_t contlen = strlen(cont);
            size_t outbuflen = contlen * 3 + 1;
            outbuf = new char[outbuflen];
            memset(outbuf, 0, outbuflen);

            char* inptr = const_cast<char*>(cont);
            char* outptr = outbuf;

            size_t inlen = contlen;
            size_t outlen = outbuflen;
            if (iconv(cd, &inptr, &inlen, &outptr, &outlen) == (size_t)(-1))
            {
                outbuf = 0;
            }

            iconv_close(cd);
            str = outbuf;
            delete[] outbuf;
        }
    }
    return str;
#else
    return std::string("");
#endif
}
#endif

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetCPUNumber()
{
#if defined (OS_WINDOWS)
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    return systemInfo.dwNumberOfProcessors;
#elif defined (OS_POSIX)
    return sysconf(_SC_NPROCESSORS_CONF);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetUUID(DTK_UUID uuid)
{
#if defined (OS_WINDOWS)
    return UuidCreate((UUID*)uuid) == RPC_S_OK ? DTK_OK : DTK_ERROR;
#elif (defined OS_POSIX)
    //uuid_generate(uuid);
    return DTK_OK;
#endif
}

typedef struct
{
    DTK_UINT32  Data1;
    DTK_UINT16  Data2;
    DTK_UINT16  Data3;
    DTK_UINT8   Data4[8];
}DTK_GUID;

#define GUID_STRING_LEN (sizeof(DTK_GUID) * 2 + 4)
DTK_DECLARE DTK_INT32 CALLBACK DTK_StringFromUUID(DTK_UUID uuid, char* uuidString, DTK_INT32 len)
{
    if(len < (DTK_INT32)GUID_STRING_LEN)

    {
        return DTK_ERROR;        
    }

    DTK_GUID* guid = (DTK_GUID*)uuid;
    DTK_Snprintf(uuidString, len, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", 
        guid->Data1, guid->Data2, guid->Data3,
        guid->Data4[0], guid->Data4[1], guid->Data4[2],
        guid->Data4[3], guid->Data4[4], guid->Data4[5],
        guid->Data4[6], guid->Data4[7]);

    return DTK_OK;                 
}
