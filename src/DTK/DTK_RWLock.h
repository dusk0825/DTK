#ifndef __DTK_RWLOCK_H__  
#define __DTK_RWLOCK_H__  

#include "DTK_Types.h"
#include "DTK_Config.h"
#include "DTK_Dtk.h"
#include "DTK_Mutex.h"

#if (defined(OS_POSIX))
    #include<sys/sem.h>
    #include<semaphore.h>
    #include<pthread.h>

    union tagSem
    {
        int iValue;
        struct semid_ds *pstBuf;
        ushort* pusSemInfo;
        void* pPad;
    };
#endif

//c++ ��������֧��
#if defined (CXXCOMPILE)
class DTK_DECLARE_CLASS ReadWriteLock
{
public:
    ReadWriteLock(void);
    virtual ~ReadWriteLock(void);

public:

    void EnterRead();

    void LeaveRead();

    void EnterWrite();

    void LeaveWrite();


public:

    ///>>����
    class ReadLock
    {
    public:

        ReadLock(ReadWriteLock* pRwLock)
        {
            m_pLock = pRwLock;
            m_pLock->EnterRead();
        }

        ~ReadLock()
        {
            try
            {
                m_pLock->LeaveRead();
            }
            catch(...)
            {

            }
        }

    private:

        ReadWriteLock* m_pLock;
    };

    ///>>д��
    class WriteLock
    {
    public:

        WriteLock(ReadWriteLock* pWrLock)
        {
            m_pLock = pWrLock;
            m_pLock->EnterWrite();
        }

        ~WriteLock()
        {
            try
            {
                m_pLock->LeaveWrite();
            }
            catch(...)
            {

            }
        }

        ReadWriteLock* m_pLock;
    };

private:

#if (defined(_WIN32) || defined(_WIN64))
    //���ű�
    HANDLE m_hReadSem;
    //д�¼�
    HANDLE m_hWriteEvent;
    //���ü����ٽ���
    CRITICAL_SECTION m_csCounter;
#else
    //���ű꣬hpr�е��ź�ֻ��һ���ͷ�һ���ź�
    int m_hReadSem;
    //д�¼�, HPR�����Զ������¼���ʹ���ź����
    sem_t m_hWriteEvent;
    //���ü�����
    HPR_MUTEX_T m_csCounter;

#endif

    //��ǰ�Ķ�д����
    int m_iRwNum;    //��ֵ����д��0�����޶�д�����������

    //����дʱ���ȴ���������
    unsigned int m_uiWaitingReadNum;

    //���ڶ�ʱ���ȴ�д������
    unsigned int m_uiWaitingWriteNum;

    //��ǰд�̵߳�id��0����û���߳���д
    unsigned long m_ulWritingThread;

};

typedef ReadWriteLock::ReadLock CReadLock;
typedef ReadWriteLock::WriteLock CWriteLock;

//ReadWriteLock::ReadLock rLock(&m_lockClient);
//ReadWriteLock::WriteLock wLock(&m_lockClient);

#endif

#endif // __DTK_RWLOCK_H__ 
