
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
    //��ʼ���ź�
    sem_init(&m_hWriteEvent, 0, 0);
    //�������ź���
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
    //�Ƿ��ж���д��������ִ��
    bool bWriting = ((m_iRwNum < 0) || (m_uiWaitingWriteNum > 0));

    if (bWriting == true)
    {
        //���ӵȴ���������
        ++m_uiWaitingReadNum;
    }
    else
    {
        //û������д�ģ�����¶�״̬
        ++m_iRwNum;
    }

#if (defined(_WIN32) || defined(_WIN64))
    ::LeaveCriticalSection(&m_csCounter);
#else
    DTK_MutexUnlock(&m_csCounter);
#endif

    if (bWriting == true)
    {
        //�������д����ȴ�
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
            //����еȴ�д�����󣬻���
#if (defined(_WIN32) || defined(_WIN64))
            ::SetEvent(m_hWriteEvent);
#else
            sem_post(&m_hWriteEvent);
#endif

            //����д״̬
            --m_iRwNum;

            //�ȴ�д����-1
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
    //�Ƿ����ڶ�д
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
        //������ڶ�д,���ӵȴ�д������
        ++m_uiWaitingWriteNum;
    }
    else
    {
        if (m_iRwNum == 0)
        {
            //����д���߳�id
#if (defined(_WIN32) || defined(_WIN64))
            m_ulWritingThread = GetCurrentThreadId();
#else
        m_ulWritingThread = pthread_self();
#endif

        }

        //û�ж�д��ǰ�߳���д��������д״̬
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
        //�̵߳�һ�ν���д�������߳���Ϣ
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
        //�Ȱ��߳�ID����Ϊ��Ч
        m_ulWritingThread = 0;

        if (m_uiWaitingWriteNum > 0)
        {
            //�еȴ�д������
#if (defined(_WIN32) || defined(_WIN64))
            ::SetEvent(m_hWriteEvent);
#else
            sem_post(&m_hWriteEvent);
#endif
            --m_uiWaitingWriteNum;
            //��־д״̬
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
            //�����߳��������ޣ���ˣ������ܳ���short���ܱ�ʾ�Ĵ�С
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
