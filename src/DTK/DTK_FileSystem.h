
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
#define DTK_READ		    O_RDONLY	//��ֻ����ʽ���ļ�
#define DTK_WRITE		    O_WRONLY	//��ֻд��ʽ���ļ�
#define DTK_RDWR		    O_RDWR	    //�Կɶ�д��ʽ���ļ�
#define DTK_CREATE		    O_CREAT		//�����򿪵��ļ����������Զ��������ļ�
#define DTK_APPEND		    O_APPEND	//����д�ļ�ʱ����ļ�β��ʼ�ƶ�, Ҳ������д������ݻ��Ը��ӵķ�ʽ���뵽�ļ�����
#define DTK_TRUNCATE	    O_TRUNC		//���ļ����ڲ����Կ�д�ķ�ʽ��ʱ, ���������ļ�������Ϊ0, ��ԭ�����ڸ��ļ�������Ҳ����ʧ
#define DTK_EXCL		    O_EXCL		//���O_CREATҲ������, ��ָ���ȥ����ļ��Ƿ����. �ļ����������������ļ�, ���򽫵��´��ļ�����
#define DTK_NOCTTY	        O_NOCTTY    //������򿪵��ļ�Ϊ�ն˻��豸ʱ, �򲻻Ὣ���ն˻����ɽ��̿����ն˻�
#define DTK_NONBLOCK	    O_NONBLOCK	//�Բ�����ϵķ�ʽ���ļ�, Ҳ���������������ݶ�ȡ��ȴ�, �����������ؽ���֮��
#define DTK_NDELAY		    O_NDELAY	//ͬO_NONBLOCK
#define DTK_SYNC	        O_SYNC		//��ͬ���ķ�ʽ���ļ�
#define DTK_NOFOLLOW	    O_NOFOLLOW	//��ָ���ļ�Ϊһ��������, �������ļ�ʧ��
#define DTK_DIRECTORY	    O_DIRECTORY	//�������pathname ��ָ���ļ�����ΪһĿ¼, �������ļ�ʧ��

#define DTK_RWXU            S_IRWXU     //read��write��excute by user, linux only
#define DTK_RWXG            S_IRWXG     //read��write��excute by user, linux only
#define DTK_RWXO            S_IRWXO     //read��write��excute by user, linux only
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
*   @brief ����Ŀ¼
*   @param [in] pDir    ·��
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeDir(const char* pDir);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeDir(const char* pDir)
*   @brief �����༶Ŀ¼
*   @param [in] pDir    ·��
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MakeMutilDir(const char* pDir);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_RemoveDir(const char* pDir)
*   @brief ɾ��Ŀ¼
*   @param [in] pDir    ·��
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_RemoveDir(const char* pDir);

/** @fn DTK_DECLARE DTK_FILE_HANDLE CALLBACK DTK_OpenFile(const char* pFileName, DTK_UINT32 nFlag, DTK_UINT32 nFileAttr)
*   @brief ���ļ�
*   @param [in] pFileName   �ļ�·��
*   @param [in] nFlag       �򿪱�־
*   @param [in] nFileAttr   ���ʷ�ʽ
*   @return �ļ����
*/
DTK_DECLARE DTK_FILE_HANDLE CALLBACK DTK_OpenFile(const char* pFileName, DTK_UINT32 nFlag, DTK_UINT32 nFileAttr);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_CloseFile(DTK_FILE_HANDLE hFile)
*   @brief �ر��ļ�
*   @param [in] hFile       �ļ����
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_CloseFile(DTK_FILE_HANDLE hFile);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_DeleteFile(const char* pFileName)
*   @brief ɾ���ļ�
*   @param [in] pFileName     �ļ�·����
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_DeleteFile(const char* pFileName);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_ReadFile(DTK_FILE_HANDLE hFile, void* pBuf, DTK_UINT32 nNumberOfBytesToRead, DTK_UINT32* pNumberOfBytesRead)
*   @brief ���ļ�����
*   @param [in] hFile                   �ļ����
*   @param [in] pBuf                    ���ݻ���
*   @param [in] nNumberOfBytesToRead    ϣ���������ֽڵ�����
*   @param [out] pNumberOfBytesRead     ʵ�ʶ�ȡ�����ֽ�����
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_ReadFile(DTK_FILE_HANDLE hFile, void* pBuf, DTK_UINT32 nNumberOfBytesToRead, DTK_UINT32* pNumberOfBytesRead);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_WriteFile(DTK_FILE_HANDLE hFile, void* pBuf, DTK_UINT32 nNumberOfBytesToWrite, DTK_UINT32* pNumberOfBytesWrite)
*   @brief д���ݵ��ļ�
*   @param [in] hFile                   �ļ����
*   @param [in] pBuf                    ���ݻ���
*   @param [in] nNumberOfBytesToRead    ϣ��д�����ֽڵ�����
*   @param [out] pNumberOfBytesRead     ʵ��д�����ֽ�����
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_WriteFile(DTK_FILE_HANDLE hFile, void* pBuf, DTK_UINT32 nNumberOfBytesToWrite, DTK_UINT32* pNumberOfBytesWrite);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_FileSeek(DTK_FILE_HANDLE hFile, DTK_INT64 iOffset, DTK_UINT32 nWhence, DTK_INT64* iCurOffset)
*   @brief �����ļ�ָ���λ��
*   @param [in] hFile       �ļ����
*   @param [in] iOffset     ƫ����
*   @param [in] iOffset     ƫ����Ե�λ��
*   @param [out] iCurOffset ��ǰ�ļ�ָ��λ��
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_FileSeek(DTK_FILE_HANDLE hFile, DTK_INT64 iOffset, DTK_UINT32 nWhence, DTK_INT64* iCurOffset);

#if 0
DTK_DECLARE DTK_BOOL CALLBACK DTK_FileIsEOF(DTK_FILE_HANDLE hFile);
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_FileFlush(DTK_FILE_HANDLE hFile)
*   @brief �ѻ��������ļ�����ǿ��д������
*   @param [in] hFile       �ļ����
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_FileFlush(DTK_FILE_HANDLE hFile);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_GetCurExePath(char* pBuf, DTK_INT32 iSize)
*   @brief ��ȡ��ǰ����·��
*   @param [in] pBuf        ·���洢����
*   @param [in] iSize       �����С
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_GetCurExePath(char* pBuf, DTK_INT32 iSize);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AccessFile(const char *pPath, DTK_INT32 iMode)
*   @brief ����ļ���Ȩ��
*   @param [in] pPath       �ļ�·��
*   @param [in] iMode       �ļ�����
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AccessFile(const char *pPath, DTK_INT32 iMode);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_CopyFile(const char* src, const char* dst, DTK_BOOL bFailIfExists)
*   @brief �����ļ�
*   @param [in] pSrc            Դ�ļ�·��
*   @param [in] pDst            Ŀ���ļ�·��
*   @param [in] bFailIfExists   �����ΪTRUE����ôһ��Ŀ���ļ��Ѿ����ڣ��������û�ʧ��
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_CopyFile(const char* pSrc, const char* pDst, DTK_BOOL bFailIfExists);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_RenameFile(const char* oldName, const char* newName)
*   @brief �������ļ�
*   @param [in] pOldName        ���ļ���
*   @param [in] pNewName        ���ļ���
*   @return �ɹ�����0��ʧ�ܷ���-1
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_RenameFile(const char* pOldName, const char* pNewName);

#endif // __DTK_FILESYSTEM_H__ 