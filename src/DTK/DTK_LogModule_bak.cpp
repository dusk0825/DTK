
#if 0

#include "DTK_LogModule.h"
#include "DTK_FileSystem.h"
#include "DTK_Thread.h"
#include "DTK_Utils.h"
#include "DTK_Error.h"
#include "DTK_Guard.h"
#include "DTK_String.h"
#include "xml/Markup.h"
#include <time.h>
#include <deque>
#include <string>
#include <vector>
#include <map>

#ifdef OS_WINDOWS
#include <sys/timeb.h>
#endif

#define MAX_PRINT_BYTES				(5 * 1024)			//һ�δ�ӡ����ֽ���
#define MAX_LOGBUF_BYTES			(10 * 1024 * 1024)	//�����־�����ֽ���

DTK_INT32 StartLogService(void);
DTK_INT32 StopLogService();

//��־��Ϣ
typedef struct tagLogDetial
{
    char *dataptr;
    unsigned int length;

    tagLogDetial()
    {
        dataptr = NULL;
        length = 0;
    }
}LOG_DETAIL_ST, *PLOG_DETAIL_ST;

//��־������
typedef struct tagLogCfg
{
    std::string strModule;      //��־ģ��
    std::string strFilePath;    //��־�ļ�·��
    LOG_LEVEL_E eLevel;         //��־��ͼ���
    int iMaxFileSize;           //��־�ļ����ֵ����λMB
    int iMaxBackupIndex;        //����ļ�����
    LOG_MODE_E eMode;           //��־�����ʽ

    tagLogCfg()
    {
        strModule = "";
        strFilePath = "";
        eLevel = INFO_LEVEL;
        iMaxFileSize = 0;
        iMaxBackupIndex = 0;
        eMode = LOCAL_FILE;
    }

    const tagLogCfg& operator=(const tagLogCfg& rhs)
    {
        strModule = rhs.strModule;
        strFilePath = rhs.strFilePath;
        eLevel = rhs.eLevel;
        iMaxFileSize = rhs.iMaxFileSize;
        iMaxBackupIndex = rhs.iMaxBackupIndex;
        eMode = rhs.eMode;
        return *this;
    }
}LOG_CFG_ST, *PLOG_CFG_ST;

class CLogModule
{
public:
    CLogModule();
    ~CLogModule();

    int Init(void);

    int Fini(void);

    void SetLogCfg(const LOG_CFG_ST& stCfg);

    LOG_LEVEL_E GetLogLevel() const;

    int PushData(char const *pContent, unsigned int iLen);

private:
    static void* CALLBACK WriteLogProc(void *pParam);

    int InputDataToFile(char const *pBuffer, unsigned int uLen);

    LOG_CFG_ST                  m_logCfg;//��־��������Ϣ
    DTK_HANDLE                  m_hThread;//�߳̾��
    DTK_BOOL                    m_bExit;//�߳��˳���ʶ
    DTK_MUTEX_T                 m_BufLock;
    char*                       m_pLogBufPtr;//����ָ��
    char*                       m_pWritePtr;//����дָ��
    DTK_UINT32                  m_uBufCountCanbeUsed;//�����д�ֽ���
    std::deque<LOG_DETAIL_ST>   m_LogList;//��־����
    DTK_HANDLE                  m_hFile;//Ҫд����ļ����
    DTK_UINT32                  m_uCurFileIndex;//��ǰ����д���ļ�����
};

DTK_Mutex                           g_lockLogModule;
std::map<std::string, CLogModule*>  g_mapLogModule;

CLogModule::CLogModule()
: m_hThread(DTK_INVALID_THREAD)
, m_bExit(DTK_FALSE)
, m_pLogBufPtr(NULL)
, m_pWritePtr(NULL)
, m_uBufCountCanbeUsed(0)
, m_hFile(DTK_INVALID_FILE)
, m_uCurFileIndex(0)
{
    DTK_MutexCreate(&m_BufLock);
}

CLogModule::~CLogModule()
{
    DTK_MutexDestroy(&m_BufLock);
}

/** @fn int Init(void)
*   @brief ��ʼ��
*   @return �ɹ�����0������Ϊ������
*/
int CLogModule::Init(void)
{
	if (NULL != m_pLogBufPtr)
	{
        return DTK_ERROR;
	}

    m_pLogBufPtr = new(std::nothrow) char[MAX_LOGBUF_BYTES];
    if (NULL == m_pLogBufPtr)
    {
        return DTK_ERROR;
    }
    DTK_ZeroMemory(m_pLogBufPtr, MAX_LOGBUF_BYTES);

    m_pWritePtr = m_pLogBufPtr;
    m_uBufCountCanbeUsed = MAX_LOGBUF_BYTES;
    m_LogList.clear();
    m_bExit = DTK_FALSE;

    //������־Ŀ¼
    std::string strPath = m_logCfg.strFilePath;
    DTK_UINT32 uIndex = strPath.find_last_of("/");
    if (uIndex != std::string::npos)
    {
        strPath = strPath.substr(0, uIndex);
    }
    DTK_MakeDir(strPath.c_str());

    m_hThread = DTK_Thread_Create(WriteLogProc, this, 0);
	if (DTK_INVALID_THREAD == m_hThread)
	{
        SafeDelete(m_pLogBufPtr);
        m_pWritePtr = NULL;
		return DTK_ERROR;
	}

	return DTK_OK;
}

/** @fn int Fini(bool waitproc = true)
*   @brief ����ʼ��
*   @return �ɹ�����0������Ϊ������
*/
int CLogModule::Fini(void)
{
    m_bExit = DTK_TRUE;
    if (DTK_INVALID_THREAD != m_hThread)
    {
        DTK_Thread_Wait(m_hThread);
    }
    m_hThread = DTK_INVALID_THREAD;

    if (DTK_INVALID_FILE != m_hFile)
    {
        DTK_CloseFile(m_hFile);
        m_hFile = DTK_INVALID_FILE;
    }

    m_LogList.clear();
    SafeDelete(m_pLogBufPtr);
    m_pWritePtr = NULL;

	return DTK_OK;
}

void CLogModule::SetLogCfg( const LOG_CFG_ST& stCfg )
{
    m_logCfg = stCfg;
}

LOG_LEVEL_E CLogModule::GetLogLevel() const
{
    return m_logCfg.eLevel;
}

/** @fn int CLogModule::PushData(char const *pContent, unsigned int iLen)
*   @brief ����־��������������
*   @param [in] pContent    ����ָ��
*   @param [in] iLen        ���ݳ���
*   @return LOG_NOERROR/����ֵ
*/
int CLogModule::PushData(char const *pContent, unsigned int iLen)
{
    DTK_MutexLock(&m_BufLock);

	if (m_uBufCountCanbeUsed < iLen)
	{
        DTK_MutexUnlock(&m_BufLock);
		return DTK_ERROR;
	}

	if (m_pWritePtr != NULL && m_pLogBufPtr != NULL)
	{
		//дָ��ʣ�µ��ֽ���������д���򷵻ص���ʼλ��д
		if ((unsigned int)(m_pLogBufPtr + MAX_LOGBUF_BYTES - m_pWritePtr) < iLen)
		{
			m_pWritePtr = m_pLogBufPtr;
		}

		memcpy(m_pWritePtr, pContent, iLen);
		static LOG_DETAIL_ST stLogInfo;
		stLogInfo.dataptr = m_pWritePtr;
		stLogInfo.length = iLen;

		try
		{
			m_LogList.push_back(stLogInfo);
		}
		catch (...)
		{
			WriteLog(ERROR_LEVEL, "Default", __FILE__, __FUNCTION__, __LINE__, "push_back allo exception");
		}		
		m_pWritePtr += iLen;
		m_uBufCountCanbeUsed -= iLen;
	}
    DTK_MutexUnlock(&m_BufLock);

	return DTK_OK;
}

/** @fn int CLogModule::InputDataToFile(char const *buffer, unsigned int length)
*   @brief ����־д���ļ�
*   @param [in] pBuffer     ����ָ��
*   @param [in] uLen        ���ݳ���
*   @return LOG_NOERROR/����ֵ
*/
int CLogModule::InputDataToFile(char const *pBuffer, unsigned int uLen)
{
	//�ж��ļ��Ƿ񼴽�������������
    DTK_INT64 iFilesize = 0;
    DTK_FileSeek(m_hFile, (DTK_INT64)0, DTK_SEEK_END, &iFilesize);
    if (iFilesize + uLen > (m_logCfg.iMaxFileSize * 1024 * 1024))
    {
        DTK_CloseFile(m_hFile);
        m_hFile = DTK_INVALID_FILE;

        char szOldFileName[DTK_MAX_PATH] = {0};
        char szNewFileName[DTK_MAX_PATH] = {0};

        //�Զ��ع���־�ļ�
        for (DTK_UINT32 i = m_uCurFileIndex + 1; i > 0; i--)
        {
            DTK_ZeroMemory(szOldFileName, DTK_MAX_PATH);
            DTK_ZeroMemory(szNewFileName, DTK_MAX_PATH);
            if (i == 1)
            {
                DTK_Snprintf(szOldFileName, sizeof(szOldFileName), "%s", m_logCfg.strFilePath.c_str());
            }
            else
            {
                DTK_Snprintf(szOldFileName, sizeof(szOldFileName), "%s.%d", m_logCfg.strFilePath.c_str(), i-1);
            }
            
            DTK_Snprintf(szNewFileName, sizeof(szNewFileName), "%s.%d", m_logCfg.strFilePath.c_str(), i);

            if (i == m_logCfg.iMaxBackupIndex)
            {
                DTK_DeleteFile(szOldFileName);
                continue;
            }

            rename(szOldFileName, szNewFileName);
        }

        m_uCurFileIndex = (m_uCurFileIndex + 1) % m_logCfg.iMaxBackupIndex;
    }

	if (m_hFile == DTK_INVALID_FILE)
	{
        char szFileName[DTK_MAX_PATH] = {0};
        DTK_Snprintf(szFileName, sizeof(szFileName), "%s", m_logCfg.strFilePath.c_str());
		m_hFile = DTK_OpenFile(szFileName, DTK_CREATE | DTK_WRITE | DTK_APPEND, DTK_ATTR_WRITE);
		if (m_hFile == DTK_INVALID_FILE)
		{
			DTK_OutputDebug("CLogModule OpenFile[%s] failed", szFileName);
			return DTK_ERROR;
		}
	}

	DTK_UINT32 nBytes = 0;
	DTK_WriteFile(m_hFile, (void *)pBuffer, uLen, &nBytes);

	return DTK_OK;
}

/** @fn void* CALLBACK CLogModule::WriteLogProc(void *pParam)
*   @brief ��־�������߳�
*   @param [in] pParam  �̲߳���
*   @return ��
*/
void* CALLBACK CLogModule::WriteLogProc(void *pParam)
{
    CLogModule* pThis = static_cast<CLogModule*> (pParam);
    if (NULL == pThis)
    {
        return NULL;
    }

    LOG_DETAIL_ST stCurInfo;

    while (true)
    {
        if (pThis->m_bExit)
        {
            //�˳��߳���Ҫ�ж��Ƿ������д�꣬д�������˳�
            if (pThis->m_LogList.empty())
            {
                break;
            }
        }

        DTK_MutexLock(&pThis->m_BufLock);
        if (pThis->m_LogList.empty())
        {
            DTK_MutexUnlock(&pThis->m_BufLock);
            DTK_Sleep(5);
            continue;
        }
        else
        {
            memcpy(&stCurInfo, &(pThis->m_LogList.front()), sizeof(stCurInfo));
            pThis->m_LogList.pop_front();
            //���¿����ֽ���
            pThis->m_uBufCountCanbeUsed += stCurInfo.length;
            DTK_MutexUnlock(&pThis->m_BufLock);

            if (pThis->m_logCfg.eMode == LOCAL_CONSOLE)
            {
                fprintf(stdout, "%s", stCurInfo.dataptr);
            }
            if (pThis->m_logCfg.eMode == LOCAL_DEBUG)
            {
                DTK_OutputDebugString(stCurInfo.dataptr);
            }
            if (pThis->m_logCfg.eMode == LOCAL_FILE)
            {
                //д�ļ�ʱ���ȼ�1����Ҫ��ȥ���Ľ�����
                pThis->InputDataToFile(stCurInfo.dataptr, stCurInfo.length - 1);
            }
        }
    }

    return NULL;
}

/** @fn int FormatLogContent(char *pBuffer, unsigned int uSize, LOG_LEVEL_E eLevel, const char* pFileName, const char* pFuncName, int iLine)
*   @brief ��ʽ����־��Ϣ
*   @param [in] pBuffer     ������
*   @param [in] uSize       ��������С
*   @param [in] eLevel      ��־����
*   @param [in] pFileName   �ļ���
*   @param [in] pFuncName   ������
*   @param [in] iLine       �к�
*   @return ��ʽ����ĳ��ȣ�-1��ʾ����
*/
int FormatLogContent(char *pBuffer, unsigned int uSize, LOG_LEVEL_E eLevel, const char* pModule, const char* pFileName, const char* pFuncName, int iLine)
{
    //��־���������ַ���
    static const char *LEVEL_TEXT[] = {"DISABLE", "TRACE", "DEBUG", "INFO", "WARN", "ERROR"};

#if defined OS_WINDOWS
	struct _timeb mi_now;
	_ftime(&mi_now);
#elif defined OS_POSIX
	struct timeb mi_now;
	ftime(&mi_now);
#endif
	struct tm* tmNow = localtime(&mi_now.time);
    if (NULL == tmNow)
    {
        return DTK_ERROR;
    }

    std::string strFileName(pFileName);
    DTK_UINT32 uIndex = strFileName.find_last_of("\\");
    if (uIndex != std::string::npos)
    {
        strFileName = strFileName.substr(uIndex + 1);
    }
    else if ((uIndex = strFileName.find_last_of("/")) != std::string::npos)
    {
        strFileName = strFileName.substr(uIndex+ 1);
    }
	
    return DTK_Snprintf(pBuffer, uSize, "%04u-%02u-%02u %02u:%02u:%02u.%03u [0x%08x] %s\t<%s>\t<%s>\t<%s>\t<%d> "
        ,tmNow->tm_year+1900, tmNow->tm_mon+1, tmNow->tm_mday,tmNow->tm_hour, tmNow->tm_min, tmNow->tm_sec, mi_now.millitm
        ,DTK_Thread_GetSelfId(), LEVEL_TEXT[eLevel], pModule, strFileName.c_str(), pFuncName, iLine);
}

DTK_DECLARE int CALLBACK WriteLog(LOG_LEVEL_E eLevel, const char* pModule, const char* pFilePath, const char* pFuncName, int iLine, const char *pContent, ...)
{
	if (pFilePath == NULL || pFuncName == NULL || pModule == NULL)
	{
		return DTK_ERROR;
	}

	char szBuffer[MAX_PRINT_BYTES] = {0};
	int iLength = FormatLogContent(szBuffer, sizeof(szBuffer) - 1, eLevel, pModule, pFilePath, pFuncName, iLine);
	if (iLength == -1)
	{
		return DTK_ERROR;
	}

	va_list args;
	va_start(args, pContent);
	int valen = DTK_Vsnprintf(szBuffer + iLength, (sizeof(szBuffer) - 1) - (unsigned int)iLength, pContent, args);
	va_end(args);
	if (valen == -1)
	{
		return DTK_ERROR;
	}
	iLength += valen;
	szBuffer[iLength++] = '\r';
	szBuffer[iLength++] = '\n';
	szBuffer[iLength++] = 0;

    DTK_Guard guard(&g_lockLogModule);
    if (g_mapLogModule.find(pModule) != g_mapLogModule.end())
    {
        if (eLevel < g_mapLogModule[pModule]->GetLogLevel())
        {
            return DTK_ERROR;
        }
        return g_mapLogModule[pModule]->PushData(szBuffer, (unsigned int)iLength);
    }
    else if (g_mapLogModule.find("Default") != g_mapLogModule.end())
    {
        if (eLevel < g_mapLogModule["Default"]->GetLogLevel())
        {
            return DTK_ERROR;
        }
        return g_mapLogModule["Default"]->PushData(szBuffer, (unsigned int)iLength);
    }

	return DTK_ERROR;
}

LOG_LEVEL_E GetCfgLogLevel(const std::string& strLevel)
{
    if (0 == DTK_Strcasecmp("TRACE", strLevel.c_str()))
    {
        return TRACE_LEVEL;
    }
    else if (0 == DTK_Strcasecmp("DEBUG", strLevel.c_str()))
    {
        return DEBUG_LEVEL;
    }
    else if (0 == DTK_Strcasecmp("WARN", strLevel.c_str()))
    {
        return WARN_LEVEL;
    }
    else if (0 == DTK_Strcasecmp("INFO", strLevel.c_str()))
    {
        return INFO_LEVEL;
    }
    else if (0 == DTK_Strcasecmp("ERROR", strLevel.c_str()))
    {
        return ERROR_LEVEL;
    }

    return INFO_LEVEL;
}

LOG_MODE_E GetOutMode(const std::string& strMode)
{
    if (0 == DTK_Strcasecmp("FILE", strMode.c_str()))
    {
        return LOCAL_FILE;
    }
    else if (0 == DTK_Strcasecmp("DEBUG", strMode.c_str()))
    {
        return LOCAL_DEBUG;
    }
    else if (0 == DTK_Strcasecmp("CONSOLE", strMode.c_str()))
    {
        return LOCAL_CONSOLE;
    }

    return LOCAL_FILE;
}

DTK_INT32 StartLogService(void)
{
    int iRet = DTK_OK;
    CMarkupSTL localXml;

    DTK_Guard guard(&g_lockLogModule);
    g_mapLogModule.clear();

    char szPath[DTK_MAX_PATH] = {0};
    DTK_GetCurExePath(szPath, sizeof(szPath));
    std::string strPath = std::string(szPath) + "logcfg.xml";
    if (!localXml.Load(strPath.c_str()))
    {
        return DTK_ERROR;
    }
    if (!localXml.FindElem("Config"))
    {
        return DTK_ERROR;
    }
    localXml.IntoElem();
    while (localXml.FindElem("logcfg"))
    {
        LOG_CFG_ST stLogCfg;
        localXml.IntoElem();
        if (!localXml.FindElem("Module"))
        {
            iRet = DTK_ERROR;
            break;
        }
        stLogCfg.strModule = localXml.GetData();
        if (!localXml.FindElem("FilePath"))
        {
            iRet = DTK_ERROR;
            break;
        }
        stLogCfg.strFilePath = localXml.GetData();
        if (!localXml.FindElem("Level"))
        {
            iRet = DTK_ERROR;
            break;
        }
        stLogCfg.eLevel = GetCfgLogLevel(localXml.GetData());
        if (!localXml.FindElem("MaxFileSize"))
        {
            iRet = DTK_ERROR;
            break;
        }
        stLogCfg.iMaxFileSize = atoi(localXml.GetData().c_str());
        if (!localXml.FindElem("MaxBackupIndex"))
        {
            iRet = DTK_ERROR;
            break;
        }
        stLogCfg.iMaxBackupIndex = atoi(localXml.GetData().c_str());
        if (!localXml.FindElem("OutMode"))
        {
            iRet = DTK_ERROR;
            break;
        }
        stLogCfg.eMode = GetOutMode(localXml.GetData());

        CLogModule* pModule = new (std::nothrow) CLogModule;
        if (NULL == pModule)
        {
            iRet = DTK_ERROR;
            break;
        }
        pModule->SetLogCfg(stLogCfg);
        pModule->Init();
        g_mapLogModule.insert(std::make_pair(stLogCfg.strModule, pModule));

        localXml.OutOfElem();
    }

    if (iRet == DTK_ERROR)
    {
        StopLogService();
    }

	return DTK_OK;
}

DTK_INT32 StopLogService()
{
    DTK_Guard guard(&g_lockLogModule);
    for (std::map<std::string, CLogModule*>::iterator itor = g_mapLogModule.begin(); itor != g_mapLogModule.end(); ++itor)
    {
        if (NULL != itor->second)
        {
            itor->second->Fini();
            delete itor->second;
        }
    }

    g_mapLogModule.swap(std::map<std::string, CLogModule*>());
    return DTK_OK;
}

#endif
