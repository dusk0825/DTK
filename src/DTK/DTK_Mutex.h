
#ifndef __DTK_MUTEX_H__  
#define __DTK_MUTEX_H__  

#include "DTK_Types.h"
#include "DTK_Config.h"

#if defined(OS_WINDOWS)
typedef CRITICAL_SECTION DTK_MUTEX_T;
#define DTK_MUTEX_FAST			0
#define DTK_MUTEX_RECURSIVE		0
#define DTK_MUTEX_ERRORCHECK	0
#elif defined(OS_POSIX)
#include <pthread.h>
typedef pthread_mutex_t DTK_MUTEX_T;
#define DTK_MUTEX_FAST			PTHREAD_MUTEX_FAST_NP
#define DTK_MUTEX_RECURSIVE		PTHREAD_MUTEX_RECURSIVE_NP
#define DTK_MUTEX_ERRORCHECK	PTHREAD_MUTEX_ERRORCHECK_NP
#endif

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexCreate(DTK_MUTEX_T* pMutex)
*   @brief ����������
*   @param [out] pMutex     ������
*   @return �ɹ�����0������������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexCreate(DTK_MUTEX_T* pMutex); 

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexDestroy(DTK_MUTEX_T* pMutex)
*   @brief ���ٻ�����
*   @param [int] pMutex     ������
*   @return �ɹ�����0������������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexDestroy(DTK_MUTEX_T* pMutex); 

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexLock(DTK_MUTEX_T* pMutex)
*   @brief ����
*   @param [int] pMutex     ������
*   @return �ɹ�����0������������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexLock(DTK_MUTEX_T* pMutex); 

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexTryLock(DTK_MUTEX_T* pMutex)
*   @brief ��������
*   @param [int] pMutex     ������
*   @return �ɹ�����0������������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexTryLock(DTK_MUTEX_T* pMutex);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexUnlock(DTK_MUTEX_T* pMutex)
*   @brief ����
*   @param [int] pMutex     ������
*   @return �ɹ�����0������������
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_MutexUnlock(DTK_MUTEX_T* pMutex); 

//c++ ��������֧��
#if defined (CXXCOMPILE)

class DTK_DECLARE_CLASS DTK_Mutex
{
public:
    DTK_Mutex();
    DTK_Mutex(DTK_UINT32 nFlag);
    ~DTK_Mutex();

    DTK_INT32 Lock();
    DTK_INT32 Unlock();
    DTK_INT32 TryLock();

private:
    DTK_MUTEX_T m_mutex;
};

class DTK_DECLARE_CLASS DTK_Guard
{
public:
    DTK_Guard(DTK_Mutex* pMutex)
        :m_pMutex(pMutex)
    {
        pMutex->Lock();	
    }

    ~DTK_Guard()
    {
        Release();
    }

    void Release()
    {
        if (m_pMutex)
        {
            m_pMutex->Unlock();
            m_pMutex = NULL;
        }
    }

private:
    DTK_Mutex* m_pMutex;
};
#endif

#endif // __DTK_MUTEX_H__ 