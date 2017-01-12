
#include"DTK_RWLock.h"

ReadWriteLock::ReadWriteLock(void)
: m_iRwNum(0)
, m_uiWaitingReadNum(0)
, m_uiWaitingWriteNum(0)
, m_ulWritingThread(0)
{
#if (defined(_WIN32) || defined(_WIN64))
    m_hReadSem = ::CreateSemaphore(NULL, 0, 1024, NULL);
    m_hWriteEvent = ::CreateEvent(NULL, DTK_FALSE, DTK_FALSE, NULL);
    ::InitializeCriticalSectionAndSpinCount(&m_csCounter, 4000);
#else
    //初始化信号
    sem_init(&m_hWriteEvent, 0, 0);
    //创建读信号量
    m_hReadSem = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union tagSem unSemUnion;
    unSemUnion.iValue = 0;
    semctl(m_hReadSem, 0, SETVAL, unSemUnion);
    DTK_MutexCreate(&m_csCounter);
#endif
}

ReadWriteLock::~ReadWriteLock(void)
{
#if (defined(_WIN32) || defined(_WIN64))
    ::DeleteCriticalSection(&m_csCounter);
    ::CloseHandle(m_hWriteEvent);
    ::CloseHandle(m_hReadSem);
#else
    sem_destroy(&m_hWriteEvent);
    union tagSem unSemUnion;
    unSemUnion.iValue = 0;
    semctl(m_hReadSem, 0, IPC_RMID, unSemUnion);
    DTK_MutexDestroy(&m_csCounter);
#endif
}

void ReadWriteLock::EnterRead()
{
#if (defined(_WIN32) || defined(_WIN64))
    ::EnterCriticalSection(&m_csCounter);
#else
    DTK_MutexLock(&m_csCounter);
#endif
    //是否有读或写请求正在执行
    bool bWriting = ((m_iRwNum < 0) || (m_uiWaitingWriteNum > 0));

    if (bWriting == true)
    {
        //增加等待读的数量
        ++m_uiWaitingReadNum;
    }
    else
    {
        //没有正在写的，则更新读状态
        ++m_iRwNum;
    }

#if (defined(_WIN32) || defined(_WIN64))
    ::LeaveCriticalSection(&m_csCounter);
#else
    DTK_MutexUnlock(&m_csCounter);
#endif

    if (bWriting == true)
    {
        //如果正在写，则等待
#if (defined(_WIN32) || defined(_WIN64))
        ::WaitForSingleObject(m_hReadSem, INFINITE);
#else
        struct sembuf stSemB;
        stSemB.sem_num = 0;
        stSemB.sem_op = -1;
        stSemB.sem_flg = SEM_UNDO;

        semop(m_hReadSem, &stSemB, 1);
#endif
    }
}

void ReadWriteLock::LeaveRead()
{
#if (defined(_WIN32) || defined(_WIN64))
    ::EnterCriticalSection(&m_csCounter);
#else
    DTK_MutexLock(&m_csCounter);
#endif

    --m_iRwNum;

    if(m_iRwNum == 0)
    {
        if (m_uiWaitingWriteNum > 0)
        {
            //如果有等待写的请求，唤醒
#if (defined(_WIN32) || defined(_WIN64))
            ::SetEvent(m_hWriteEvent);
#else
            sem_post(&m_hWriteEvent);
#endif

            //设置写状态
            --m_iRwNum;

            //等待写计数-1
            --m_uiWaitingWriteNum;
        }
    }

#if (defined(_WIN32) || defined(_WIN64))
    ::LeaveCriticalSection(&m_csCounter);
#else
    DTK_MutexUnlock(&m_csCounter);
#endif
}

void ReadWriteLock::EnterWrite()
{
    //是否正在读写
    bool bReadWrite;
#if (defined(_WIN32) || defined(_WIN64))
    ::EnterCriticalSection(&m_csCounter);
#else
    DTK_MutexLock(&m_csCounter);
#endif

#if (defined(_WIN32) || defined(_WIN64))
    if ((m_iRwNum < 0 && (GetCurrentThreadId() == m_ulWritingThread)) || (m_iRwNum == 0))
#else
    if ((m_iRwNum < 0 && (pthread_self() == m_ulWritingThread)) || (m_iRwNum == 0))
#endif
    {
        bReadWrite = false;
    }
    else
    {
        bReadWrite = true;
    }

    if (bReadWrite == true)
    {
        //如果正在读写,增加等待写的数量
        ++m_uiWaitingWriteNum;
    }
    else
    {
        if (m_iRwNum == 0)
        {
            //正在写的线程id
#if (defined(_WIN32) || defined(_WIN64))
            m_ulWritingThread = GetCurrentThreadId();
#else
        m_ulWritingThread = pthread_self();
#endif

        }

        //没有读写或当前线程在写，则设置写状态
        --m_iRwNum;
    }

#if (defined(_WIN32) || defined(_WIN64))
    ::LeaveCriticalSection(&m_csCounter);
#else
    DTK_MutexUnlock(&m_csCounter);
#endif

    if (bReadWrite)
    {
#if (defined(_WIN32) || defined(_WIN64))
        ::WaitForSingleObject(m_hWriteEvent, INFINITE);
        //线程第一次进入写，设置线程信息
        m_ulWritingThread = GetCurrentThreadId();
#else
        sem_wait(&m_hWriteEvent);
        m_ulWritingThread = pthread_self();
#endif
    }
}

void ReadWriteLock::LeaveWrite()
{
#if (defined(_WIN32) || defined(_WIN64))
    ::EnterCriticalSection(&m_csCounter);
#else
    DTK_MutexLock(&m_csCounter);
#endif

    ++m_iRwNum;

    if(m_iRwNum == 0)
    {
        //先把线程ID设置为无效
        m_ulWritingThread = 0;

        if (m_uiWaitingWriteNum > 0)
        {
            //有等待写的请求
#if (defined(_WIN32) || defined(_WIN64))
            ::SetEvent(m_hWriteEvent);
#else
            sem_post(&m_hWriteEvent);
#endif
            --m_uiWaitingWriteNum;
            //标志写状态
            --m_iRwNum;
        }
        else if(m_uiWaitingReadNum > 0)
        {
            m_iRwNum = m_uiWaitingReadNum;
            m_uiWaitingReadNum = 0;
#if (defined(_WIN32) || defined(_WIN64))
            ::ReleaseSemaphore(m_hReadSem, m_iRwNum, NULL);
#else
            struct sembuf semB;
            semB.sem_num = 0;
            //由于线程数量有限，因此，不可能超过short所能表示的大小
            semB.sem_op = (short)m_iRwNum;
            semB.sem_flg = SEM_UNDO;

            semop(m_hReadSem, &semB, 1);      
#endif
        }
    }

#if (defined(_WIN32) || defined(_WIN64))
    ::LeaveCriticalSection(&m_csCounter);
#else
    DTK_MutexUnlock(&m_csCounter);
#endif
}
