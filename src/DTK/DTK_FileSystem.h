
#ifndef __DTK_FILESYSTEM_H__  
#define __DTK_FILESYSTEM_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

#if (defined OS_WINDOWS)
    typedef PSID DTK_UID_T;
    typedef PSID DTK_GID_T;
    typedef DWORD DTK_FILETYPE_T;
    typedef DTK_INT64 DTK_INODE_T;
    typedef DTK_INT64 DTK_SIZE_T;
    typedef DTK_HANDLE DTK_FILE_HANDLE;
    #define DTK_INVALID_FILE INVALID_HANDLE_VALUE
#elif (defined OS_POSIX)
    typedef uid_t DTK_UID_T;
    typedef gid_t DTK_GID_T;
    typedef mode_t DTK_FILETYPE_T;
    typedef DTK_INT64 DTK_INODE_T;
    typedef DTK_INT64 DTK_SIZE_T;
    typedef DTK_INT32 DTK_FILE_HANDLE;
    #define DTK_INVALID_FILE (DTK_FILE_HANDLE)-1 
#endif

#define DTK_MAX_PATH	260			    //windows MAX_PATH

    //(file create flag)
#ifdef OS_WINDOWS
#define DTK_READ		    0x0001		//read  file
#define DTK_WRITE		    0x0002		//write  file
#define DTK_CREATE		    0x0004		//create file if not exist
#define DTK_APPEND		    0x0008		//append to the end of file
#define DTK_TRUNCATE	    0x0010		//open file and truncate the length to 0
#define DTK_BINARY		    0x0020		//open file in binary mode
#define DTK_EXCL		    0x0040		//open file will be failed if exist
#define DTK_BUFFERED	    0x0080		//open file of buffered io
#define DTK_SHARELOCK	    0x0100		//share the rwlock
#define DTK_XTHREAD		    0x0200		//multithread
#define DTK_LARGEFILE	    0x0400		//large file
#define DTK_DIRECT          0x0800      //direct open file. according to the test, this flags should be used with pre-allocate

#elif defined OS_POSIX
#define DTK_READ		    O_RDONLY	//以只读方式打开文件
#define DTK_WRITE		    O_WRONLY	//以只写方式打开文件
#define DTK_RDWR		    O_RDWR	    //以可读写方式打开文件
#define DTK_CREATE		    O_CREAT		//若欲打开的文件不存在则自动建立该文件
#define DTK_APPEND		    O_APPEND	//当读写文件时会从文件尾开始移动, 也就是所写入的数据会以附加的方式加入到文件后面
#define DTK_TRUNCATE	    O_TRUNC		//若文件存在并且以可写的方式打开时, 此旗标会令文件长度清为0, 而原来存于该文件的资料也会消失
#define DTK_EXCL		    O_EXCL		//如果O_CREAT也被设置, 此指令会去检查文件是否存在. 文件若不存在则建立该文件, 否则将导致打开文件错误
#define DTK_NOCTTY	        O_NOCTTY    //如果欲打开的文件为终端机设备时, 则不会将该终端机当成进程控制终端机
#define DTK_NONBLOCK	    O_NONBLOCK	//以不可阻断的方式打开文件, 也就是无论有无数据读取或等待, 都会立即返回进程之中
#define DTK_NDELAY		    O_NDELAY	//同O_NONBLOCK
#define DTK_SYNC	        O_SYNC		//以同步的方式打开文件
#define DTK_NOFOLLOW	    O_NOFOLLOW	//所指的文件为一符号连接, 则会令打开文件失败
#define DTK_DIRECTORY	    O_DIRECTORY	//如果参数pathname 所指的文件并非为一目录, 则会令打开文件失败

#define DTK_RWXU            S_IRWXU     //read、write、excute by user, linux only
#define DTK_RWXG            S_IRWXG     //read、write、excute by user, linux only
#define DTK_RWXO            S_IRWXO     //read、write、excute by user, linux only
#endif

//(file attribute flag)
#define DTK_ATTR_READONLY	0x1000		//readonly
#define DTK_ATTR_WRITE		0x2000		//write
#define DTK_ATTR_EXECUTABLE	0x4000		//exeute
#define DTK_ATTR_HIDDEN		0x8000		//hidden
#define DTK_ATTR_SYSTEM		0x0100		//system file, windows only
#define DTK_DELONCLOSE		0x0200		//delete file when close

#define DTK_UREAD           0x0400      //Read by user
#define DTK_UWRITE          0x0200      //Write by user
#define DTK_UEXECUTE        0x0100      //Execute by user
#define DTK_GREAD           0x0040      //Read by group
#define DTK_GWRITE          0x0020      //Write by group
#define DTK_GEXECUTE        0x0010      //Execute by group
#define DTK_WREAD           0x0004      //Read by others
#define DTK_WWRITE          0x0002      //Write by others
#define DTK_WEXECUTE        0x0001      //Execute by others

//for windows
#define DTK_USETID          0x8000      //Set user id
#define DTK_GSETID          0x4000      //Set group id
#define DTK_WSTICKY         0x2000      //Sticky bit
#define DTK_OS_DEFAULT      0x0FFF      //use OS's default permissions

//(file TYPE)
#define DTK_TYPE_CHAR		0x0001		//char dev
#define DTK_TYPE_DISK		0x0002		//disk dev
#define DTK_TYPE_PIPE		0x0004		//pipe dev
#define DTK_TYPE_UNKNOWN	0x0008		//unknown dev
#define DTK_TYPE_SOCK		0x0010		//sock dev
#define DTK_TYPE_FIFO		0x0020		//fifo
#define DTK_TYPE_LINK		0x0040		//symbolic link
#define DTK_TYPE_BLK		0x0080		//block dev
#define DTK_TYPE_DIR		0x0100		//dir dev
#define DTK_TYPE_REG		0x0200		//reg dev

//(file seek flag)
#define DTK_SEEK_SET	    SEEK_SET	//begin
#define DTK_SEEK_CUR	    SEEK_CUR	//current
#define DTK_SEEK_END	    SEEK_END	//end

//file access 
#define DTK_RW_OK           6           //Test for read and write permission
#define DTK_R_OK            4           //Test for read permission
#define DTK_W_OK            2           //Test for write permission
#define DTK_X_OK            1           //Test for execute permission
#define DTK_F_OK            0           //Test for existence


/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeDir(const char* pDir)
*   @brief 创建目录
*   @param [in] pDir    路径
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeDir(const char* pDir);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeDir(const char* pDir)
*   @brief 创建多级目录
*   @param [in] pDir    路径
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeMutilDir(const char* pDir);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_RemoveDir(const char* pDir)
*   @brief 删除目录
*   @param [in] pDir    路径
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_RemoveDir(const char* pDir);

/** @fn DTK_DECLARE DTK_FILE_HANDLE CALLBACK DTK_OpenFile(const char* pFileName, DTK_UINT32 nFlag, DTK_UINT32 nFileAttr)
*   @brief 打开文件
*   @param [in] pFileName   文件路径
*   @param [in] nFlag       打开标志
*   @param [in] nFileAttr   访问方式
*   @return 文件句柄
*/
DTK_DECLARE DTK_FILE_HANDLE CALLBACK DTK_OpenFile(const char* pFileName, DTK_UINT32 nFlag, DTK_UINT32 nFileAttr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_CloseFile(DTK_FILE_HANDLE hFile)
*   @brief 关闭文件
*   @param [in] hFile       文件句柄
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_CloseFile(DTK_FILE_HANDLE hFile);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_DeleteFile(const char* pFileName)
*   @brief 删除文件
*   @param [in] pFileName     文件路径名
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_DeleteFile(const char* pFileName);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ReadFile(DTK_FILE_HANDLE hFile, void* pBuf, DTK_UINT32 nNumberOfBytesToRead, DTK_UINT32* pNumberOfBytesRead)
*   @brief 读文件数据
*   @param [in] hFile                   文件句柄
*   @param [in] pBuf                    数据缓存
*   @param [in] nNumberOfBytesToRead    希望读多少字节的数据
*   @param [out] pNumberOfBytesRead     实际读取多少字节数据
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ReadFile(DTK_FILE_HANDLE hFile, void* pBuf, DTK_UINT32 nNumberOfBytesToRead, DTK_UINT32* pNumberOfBytesRead);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_WriteFile(DTK_FILE_HANDLE hFile, void* pBuf, DTK_UINT32 nNumberOfBytesToWrite, DTK_UINT32* pNumberOfBytesWrite)
*   @brief 写数据到文件
*   @param [in] hFile                   文件句柄
*   @param [in] pBuf                    数据缓存
*   @param [in] nNumberOfBytesToRead    希望写多少字节的数据
*   @param [out] pNumberOfBytesRead     实际写多少字节数据
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_WriteFile(DTK_FILE_HANDLE hFile, void* pBuf, DTK_UINT32 nNumberOfBytesToWrite, DTK_UINT32* pNumberOfBytesWrite);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_FileSeek(DTK_FILE_HANDLE hFile, DTK_INT64 iOffset, DTK_UINT32 nWhence, DTK_INT64* iCurOffset)
*   @brief 控制文件指针的位置
*   @param [in] hFile       文件句柄
*   @param [in] iOffset     偏移量
*   @param [in] iOffset     偏移相对的位置
*   @param [out] iCurOffset 当前文件指针位置
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_FileSeek(DTK_FILE_HANDLE hFile, DTK_INT64 iOffset, DTK_UINT32 nWhence, DTK_INT64* iCurOffset);

#if 0
DTK_DECLARE DTK_BOOL CALLBACK DTK_FileIsEOF(DTK_FILE_HANDLE hFile);
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_FileFlush(DTK_FILE_HANDLE hFile)
*   @brief 把缓存区的文件数据强制写到磁盘
*   @param [in] hFile       文件句柄
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_FileFlush(DTK_FILE_HANDLE hFile);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetCurExePath(char* pBuf, DTK_INT32 iSize)
*   @brief 获取当前工作路径
*   @param [in] pBuf        路径存储缓存
*   @param [in] iSize       缓存大小
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetCurExePath(char* pBuf, DTK_INT32 iSize);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AccessFile(const char *pPath, DTK_INT32 iMode)
*   @brief 检测文件的权限
*   @param [in] pPath       文件路径
*   @param [in] iMode       文件属性
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AccessFile(const char *pPath, DTK_INT32 iMode);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_CopyFile(const char* src, const char* dst, DTK_BOOL bFailIfExists)
*   @brief 拷贝文件
*   @param [in] pSrc            源文件路径
*   @param [in] pDst            目的文件路径
*   @param [in] bFailIfExists   如果设为TRUE，那么一旦目标文件已经存在，则函数调用会失败
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_CopyFile(const char* pSrc, const char* pDst, DTK_BOOL bFailIfExists);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_RenameFile(const char* oldName, const char* newName)
*   @brief 重命名文件
*   @param [in] pOldName        旧文件名
*   @param [in] pNewName        新文件名
*   @return 成功返回0，失败返回-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_RenameFile(const char* pOldName, const char* pNewName);

#endif // __DTK_FILESYSTEM_H__ 