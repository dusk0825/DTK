
#include "DTK_FileSystem.h"
#include "DTK_Utils.h"

#ifdef OS_WINDOWS
#include <io.h>
#endif

DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeDir(const char* pDir)
{
	if ( !pDir )
	{
		return DTK_ERROR;
	}

#if (defined OS_WINDOWS)
    //wchar_t wDir[260] = {0};
    //MultiByteToWideChar(CP_ACP, 0, pDir, strlen(pDir), wDir, sizeof(wDir));
    //return (CreateDirectory(wDir,NULL))?DTK_OK:DTK_ERROR;
    return (CreateDirectory((LPCSTR)pDir,NULL))?DTK_OK:DTK_ERROR;
#elif (defined OS_POSIX)
	return mkdir(pDir, DTK_RWXU | DTK_RWXG | DTK_RWXO);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeMutilDir(const char* pDir)
{
    if (NULL == pDir)
    {
        return DTK_ERROR;
    }

    char szFullPath[DTK_MAX_PATH] = {0};
    DTK_Snprintf(szFullPath, sizeof(szFullPath), "%s", pDir);

    if (szFullPath[strlen(szFullPath) - 1] != '/')
    {
        szFullPath[strlen(szFullPath)] = '/';
    }

    int iSegPathNum = 0;
    char szTempPath[DTK_MAX_PATH] = {0};
    char szSegPath[DTK_MAX_PATH/2][DTK_MAX_PATH] = {0};
    for (unsigned int i = 0, j = 0; i < strlen(szFullPath); i++)
    {
        if (szFullPath[i] != '\\' && szFullPath[i] != '/')
        {
            szTempPath[j++] = szFullPath[i];
        }
        else
        {
            strncpy(szSegPath[iSegPathNum++], szTempPath, strlen(szTempPath));
            szTempPath[j++] = '/';
        }
    }

    for (int i = 0; i < iSegPathNum; i++)
    {
        if ((DTK_AccessFile(szSegPath[i], 0 )) >= 0)
        {
            continue;
        }
        if (DTK_MakeDir(szSegPath[i]) < 0)
        {
            return DTK_ERROR;
        }
    }

    return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_RemoveDir(const char* pDir)
{
	if ( !pDir )
	{
		return DTK_ERROR;
	}

#if (defined OS_WINDOWS)
	//wchar_t wDir[260] = {0};
	//MultiByteToWideChar(CP_ACP, 0, pDir, strlen(pDir), wDir, sizeof(wDir));
	//return (RemoveDirectory(wDir))?DTK_OK:DTK_ERROR;
    return (RemoveDirectory((LPCSTR)pDir))?DTK_OK:DTK_ERROR;
#else
	return rmdir(pDir);
#endif
}

#if 0
DTK_DECLARE DTK_INT32 CALLBACK DTK_RemoveMutilDir(const char* pDir);
DTK_DECLARE DTK_INT32 CALLBACK DTK_RemoveMutilDir(const char* pDir)
{
    if (NULL == pDir)
    {
        return DTK_ERROR;
    }

#ifdef OS_WINDOWS
    char szDir[MAX_PATH] = {0};
    char szFileName[MAX_PATH] = {0};
    sprintf_s(szDir, MAX_PATH, "%s\\*.*", pDir);

    long hFile;
    _finddata_t fileinfo;
    if ((hFile = _findfirst(szDir, &fileinfo)) != -1)
    {
        do
        {
            memset(szFileName, 0, MAX_PATH);
            sprintf_s(szFileName, MAX_PATH, "%s\\%s", pDir, fileinfo.name);
            //如果不是目录,则进行处理文件夹下面的文件
            if (!(fileinfo.attrib & _A_SUBDIR))
            {
                DTK_RemoveDir(szFileName);
            }
            else
            {
                //处理目录，递归调用
                if ( strcmp(fileinfo.name, "." ) != 0 && strcmp(fileinfo.name, ".." ) != 0 )
                {
                    DTK_RemoveMutilDir(szFileName);
                }
            }
        } while (_findnext(hFile,&fileinfo) == 0);
        _findclose(hFile);
        _rmdir(pDir);
        return DTK_OK;
    }
#endif

    return DTK_OK;
}
#endif

#if 0
DTK_DECLARE DTK_INT32 CALLBACK DTK_TraverseDir(const char* pDir, std::vector<std::string>& fileList);
DTK_DECLARE DTK_INT32 CALLBACK DTK_TraverseDir(const char* pDir, std::vector<std::string>& fileList)
{
#if (defined _WIN32 || defined _WIN64)
    _finddata_t stFileInfo;
    std::string strFolderPath(pDir);
    std::string strFindPath = strFolderPath + "\\*";
    long iHandle = _findfirst(strFindPath.c_str(), &stFileInfo);

    if (iHandle == -1L)
    {
        return DTK_ERROR;
    }

    do
    {
        if (stFileInfo.attrib & _A_SUBDIR)    
        {
            if( (strcmp(stFileInfo.name, ".") != 0 ) && (strcmp(stFileInfo.name, "..") != 0))  
            {
                std::string strNewPath = strFolderPath + "\\" + stFileInfo.name;
                TraverseFloder(strNewPath, fileList);
            }
        }
        else  
        {
            std::string strFilePath = strFolderPath + "\\" + string(stFileInfo.name);
            fileList.push_back(strFilePath);
        }
    }while (_findnext(iHandle, &stFileInfo) == 0);

    _findclose(iHandle);
#else
    DIR* pDir = NULL;
    struct dirent* pStDirent = NULL;
    char szChildPath[PATH_LEN] = {0};

    pDir = opendir(strFolderPath);
    while ((NULL != pStDirent = readdir(pDir)))
    {
        memset(szChildPath, 0, PATH_LEN);
        sprintf(szChildPath, "%s/%s", strPath, pStDirent->d_name);

        if (pStDirent->d_type & DT_DIR)
        {
            if((strcmp(pStDirent->d_name,".") != 0) && (strcmp(pStDirent->d_name,"..")) == 0)
            {
                TraverseFloder(szChildPath, fileList);
            }
        }
        else
        {
            fileList.push_back(szChildPath);
        }
    }

#endif
    return DTK_OK;
}
#endif

DTK_DECLARE DTK_FILE_HANDLE CALLBACK DTK_OpenFile(const char* pFileName, DTK_UINT32 nFlag, DTK_UINT32 nFileAttr)
{
    DTK_FILE_HANDLE hFile = DTK_INVALID_FILE;

    if ( !pFileName )
    {
        return DTK_INVALID_FILE;
    }

#ifdef OS_WINDOWS
	DWORD dwDesiredAccess = 0;
	DWORD dwShareMode = FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE;
	DWORD dwCreateFlag = 0;
	DWORD dwFlagAttr = FILE_ATTRIBUTE_NORMAL;

	//get file access right
	if (nFlag & DTK_READ)
	{
		dwDesiredAccess |= GENERIC_READ;
	}
	if (nFlag & DTK_WRITE)
	{
		dwDesiredAccess |= GENERIC_WRITE;
	}
	if (nFlag & DTK_EXCL)
	{
		dwDesiredAccess |= GENERIC_EXECUTE;
	}
	//[MOV] this attr is dwFlagAttr flag.
	//if (nFlag & DTK_DIRECT)
	//{
	//	dwCreateFlag |= FILE_FLAG_NO_BUFFERING;
	//}

	///get file open mode
	if (nFlag & DTK_CREATE)
	{
		if ( (nFlag & DTK_EXCL) ) 
		{
			dwCreateFlag = CREATE_NEW;
		}
		else if (nFlag & DTK_TRUNCATE)
		{
			dwCreateFlag = CREATE_ALWAYS;
		}
		else
		{
			dwCreateFlag = OPEN_ALWAYS;
		}
	}
	else if (nFlag & DTK_TRUNCATE)
	{
		dwCreateFlag = TRUNCATE_EXISTING;
	}
	else
	{
		dwCreateFlag = OPEN_EXISTING;
	}

	if ((nFlag & DTK_EXCL) && !(nFlag & DTK_CREATE))
	{
        return DTK_INVALID_FILE;
    }

	//get file attr
	if (nFileAttr & DTK_ATTR_READONLY)
	{
		dwFlagAttr |= FILE_ATTRIBUTE_READONLY;
	}
	if (nFileAttr & DTK_ATTR_HIDDEN)
	{
		dwFlagAttr |= FILE_ATTRIBUTE_HIDDEN;
	}
	if (nFileAttr & DTK_ATTR_SYSTEM)
	{
		dwFlagAttr |= FILE_ATTRIBUTE_SYSTEM;
	}
	if (nFileAttr & DTK_DELONCLOSE)
	{
		dwFlagAttr |= FILE_FLAG_DELETE_ON_CLOSE;
	}
	if (nFlag & DTK_DIRECT)
	{
		dwFlagAttr |= FILE_FLAG_NO_BUFFERING;
	}

    //wchar_t wFileName[260] = {0};
    //MultiByteToWideChar(CP_ACP, 0, pFileName, strlen(pFileName), wFileName, sizeof(wFileName));
    //hFile = CreateFile(wFileName,dwDesiredAccess, dwShareMode,NULL,dwCreateFlag,dwFlagAttr,0);

	hFile = CreateFile((LPCSTR)pFileName,dwDesiredAccess, dwShareMode,NULL,dwCreateFlag,dwFlagAttr,0);

	if (nFlag & DTK_APPEND && hFile != DTK_INVALID_FILE)
	{
		SetFilePointer(hFile,0,NULL,FILE_END);
	}
#elif (defined OS_POSIX)
    if (nFileAttr & DTK_ATTR_WRITE || nFileAttr & DTK_ATTR_READONLY)
    {
        nFileAttr = DTK_RWXU | DTK_RWXG | DTK_RWXO;
    }
    hFile = open(pFileName, nFlag, nFileAttr);
    if (hFile < 0)
    {
        return DTK_INVALID_FILE;
    }
#endif

	return hFile;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_CloseFile(DTK_FILE_HANDLE hFile)
{
	if (hFile == DTK_INVALID_FILE)
	{
		return DTK_ERROR;
	}
#ifdef OS_WINDOWS
	if ( CloseHandle(hFile) )
	{
		return DTK_OK;
	}
#elif defined OS_POSIX
    if (close(hFile) == 0)
    {
        return DTK_OK;
    }
#endif
	return DTK_ERROR;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_DeleteFile(const char* pFileName)
{
	if ( !pFileName )
	{
		return DTK_ERROR;
	}

#if defined OS_WINDOWS
	//wchar_t wFileName[260] = {0};
	//MultiByteToWideChar(CP_ACP, 0, pFileName, strlen(pFileName), wFileName, sizeof(wFileName));
	//return (DeleteFile(wFileName))?DTK_OK:DTK_ERROR;
    return (DeleteFile((LPCSTR)pFileName))?DTK_OK:DTK_ERROR;
#elif defined OS_POSIX
	return remove(pFileName);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_ReadFile(DTK_FILE_HANDLE hFile, void* pBuf, DTK_UINT32 nNumberOfBytesToRead, DTK_UINT32* pNumberOfBytesRead)
{
    if (DTK_INVALID_FILE == hFile)
    {
        return DTK_ERROR;
    }

    if (pNumberOfBytesRead)
    {
        *pNumberOfBytesRead = 0;
    }
    
#ifdef OS_WINDOWS
	DWORD dwNumberOfBytesRead = 0;

	if (ReadFile(hFile,pBuf,nNumberOfBytesToRead,(LPDWORD)&dwNumberOfBytesRead,NULL))
	{
		if (pNumberOfBytesRead)
		{
			*pNumberOfBytesRead = dwNumberOfBytesRead;
		}
		return DTK_OK;
	}
#elif defined OS_POSIX
    DTK_UINT32 uReadByte = read(hFile, pBuf, nNumberOfBytesToRead);
    if (uReadByte >= 0)
    {
        if (pNumberOfBytesRead)
        {
            *pNumberOfBytesRead = uReadByte;
        }
        return DTK_OK;
    }
#endif

	return DTK_ERROR;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_WriteFile(DTK_FILE_HANDLE hFile, void* pBuf, DTK_UINT32 nNumberOfBytesToWrite, DTK_UINT32* pNumberOfBytesWrite)
{
    if (DTK_INVALID_FILE == hFile)
    {
        return DTK_ERROR;
    }

    if (pNumberOfBytesWrite)
    {
        *pNumberOfBytesWrite = 0;
    }

#ifdef OS_WINDOWS
	DWORD dwNumberOfBytesRead = 0;
	if (WriteFile(hFile,pBuf,nNumberOfBytesToWrite,(LPDWORD)&dwNumberOfBytesRead,NULL))
	{
		if (pNumberOfBytesWrite)
		{
			*pNumberOfBytesWrite = dwNumberOfBytesRead;
		}
		return DTK_OK;
	}
#elif defined OS_POSIX
    DTK_UINT32 uWriteByte = write(hFile, pBuf, nNumberOfBytesToWrite);
    if (uWriteByte >= 0)
    {
        if (pNumberOfBytesWrite)
        {
            *pNumberOfBytesWrite = uWriteByte;
        }
        return DTK_OK;
    }
#endif

	return DTK_ERROR;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_FileSeek(DTK_FILE_HANDLE hFile, DTK_INT64 iOffset, DTK_UINT32 nWhence, DTK_INT64* iCurOffset)
{
    if (DTK_INVALID_FILE == hFile)
    {
        return DTK_ERROR;
    }

#ifdef OS_WINDOWS
	LARGE_INTEGER lInt;
	lInt.QuadPart = iOffset;
	lInt.LowPart = SetFilePointer(hFile,lInt.LowPart,&lInt.HighPart,nWhence);
	if ( ((DWORD)(-1) == lInt.LowPart) && (NO_ERROR != GetLastError()) )
	{
		return DTK_ERROR;
	}

	if (iCurOffset)
	{
		*iCurOffset = lInt.QuadPart;
	}
#elif OS_POSIX
    DTK_INT64 iRet = lseek(hFile, iOffset, nWhence);
    if (iRet == -1)
    {
        return DTK_ERROR;
    }
    
    if (iCurOffset)
    {
        *iCurOffset = iRet;
    }
#endif
	return DTK_OK;
}

#if 0
DTK_DECLARE DTK_BOOL CALLBACK DTK_FileIsEOF(DTK_FILE_HANDLE hFile)
{
	if (hFile != DTK_INVALID_FILE)
	{
		return (ERROR_HANDLE_EOF == GetLastError())?DTK_TRUE:DTK_FALSE;
	}

	return DTK_FALSE;
}
#endif

DTK_DECLARE DTK_INT32 CALLBACK DTK_FileFlush(DTK_FILE_HANDLE hFile)
{
#ifdef OS_WINDOWS
	return FlushFileBuffers(hFile)?DTK_OK:DTK_ERROR;
#elif defined OS_POSIX
    return fsync(hFile);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_GetCurExePath(char* pBuf, DTK_INT32 iSize)
{
#ifdef OS_WINDOWS
    char path[DTK_MAX_PATH] = {0};
    int retVal = GetModuleFileName(NULL, path, DTK_MAX_PATH);
    if (retVal == 0)
    {
        return DTK_ERROR;
    }

    int len = (int)strlen(path);
    if (len > iSize)
    {
        return DTK_ERROR;
    }

    for (int i = len -1; i > 0; i--)
    {
        if (path[i] == '\\')
        {
            len = i;
            break;
        }
    }

    memcpy(pBuf, path, len+1);

#elif defined OS_POSIX
    char* pPath = getcwd(pBuf, iSize);
    if (NULL == pPath)
    {
        return DTK_ERROR;
    }
    if (pBuf[strlen(pBuf) - 1] != '/')
    {
        strcat(pBuf, "/");
    }
#endif
    return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AccessFile(const char *pPath, DTK_INT32 iMode)
{
    if (NULL == pPath)
    {
        return DTK_ERROR;
    }

#ifdef OS_WINDOWS
    return _access(pPath, iMode);
#elif defined OS_POSIX
	return access(pPath, iMode);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_CopyFile(const char* pSrc, const char* pDst, DTK_BOOL bFailIfExists)
{
    if (NULL == pSrc || NULL == pDst)
    {
        return DTK_ERROR;
    }

#ifdef OS_WINDOWS
	//wchar_t wsrc[260] = {0};
	//wchar_t wdst[260] = {0};
	//MultiByteToWideChar(CP_ACP, 0, pSrc, strlen(pSrc), wsrc, sizeof(wsrc));
	//MultiByteToWideChar(CP_ACP, 0, pDst, strlen(pDst), wdst, sizeof(wdst));
	//return CopyFile(wsrc, wdst, bFailIfExists) == TRUE ? DTK_OK : DTK_ERROR;
    return CopyFile(pSrc, pDst, bFailIfExists) ? DTK_OK : DTK_ERROR;
#elif defined OS_POSIX
    return DTK_ERROR;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_RenameFile(const char* pOldName, const char* pNewName)
{
	if (NULL == pOldName || NULL == pNewName)
	{
		return DTK_ERROR;
	}

#ifdef OS_WINDOWS
	//wchar_t wsrc[260] = {0};
	//wchar_t wdst[260] = {0};
	//MultiByteToWideChar(CP_ACP, 0, pOldName, strlen(pOldName), wsrc, sizeof(wsrc));
	//MultiByteToWideChar(CP_ACP, 0, pNewName, strlen(pNewName), wdst, sizeof(wdst));
	//return MoveFile(wsrc, wdst) ? DTK_OK : DTK_ERROR;
    return MoveFile(pOldName, pNewName) ? DTK_OK : DTK_ERROR;
#elif defined OS_POSIX
	return DTK_ERROR;
#endif
}

