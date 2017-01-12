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

//c++ 编译器才支持
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

    ///>>读锁
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

    ///>>写锁
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
    //读信标
    HANDLE m_hReadSem;
    //写事件
    HANDLE m_hWriteEvent;
    //引用计数临界区
    CRITICAL_SECTION m_csCounter;
#else
    //读信标，hpr中的信号只能一次释放一个信号
    int m_hReadSem;
    //写事件, HPR中无自动重置事件，使用信号替代
    sem_t m_hWriteEvent;
    //引用计数锁
    HPR_MUTEX_T m_csCounter;

#endif

    //当前的读写数量
    int m_iRwNum;    //负值代表写，0代表无读写，正数代表读

    //正在写时，等待读的数量
    unsigned int m_uiWaitingReadNum;

    //正在读时，等待写的数量
    unsigned int m_uiWaitingWriteNum;

    //当前写线程的id，0代表没有线程在写
    unsigned long m_ulWritingThread;

};

typedef ReadWriteLock::ReadLock CReadLock;
typedef ReadWriteLock::WriteLock CWriteLock;

//ReadWriteLock::ReadLock rLock(&m_lockClient);
//ReadWriteLock::WriteLock wLock(&m_lockClient);

#endif

#endif // __DTK_RWLOCK_H__ 
