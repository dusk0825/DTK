
#ifndef __DTK_CSingleton_H__  
#define __DTK_CSingleton_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"
#include "DTK_Mutex.h"

template<typename T>
class CSingletonMgr
{
public:
    static inline T* Instance()
    {
        if (m_pInstance == NULL)
        {
            m_lock.Lock();
            if (m_pInstance == NULL)
            {
                m_pInstance = new (std::nothrow) T;
            }
            m_lock.Unlock();
        }
        return m_pInstance;
    }

    static inline void Free()
    {
        m_bFree = true;
        m_lock.Lock();
        if (m_pInstance != NULL)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
        m_lock.Unlock();
    }

protected:
    CSingletonMgr() {}
    virtual ~CSingletonMgr()
    {
        if (!m_bFree)
        {
            m_lock.Lock();
            if (m_pInstance != NULL)
            {
                delete m_pInstance;
                m_pInstance = NULL;
            }
            m_lock.Unlock();
        }
    }
private:
    CSingletonMgr(const CSingletonMgr&) {}
    CSingletonMgr& operator=(const CSingletonMgr&) {}

private:
    static T* m_pInstance;
    static DTK_Mutex m_lock;
    static bool m_bFree;
};

template<typename T>
T* CSingletonMgr<T>::m_pInstance = NULL;

template<typename T>
DTK_Mutex CSingletonMgr<T>::m_lock;

template<typename T>
bool CSingletonMgr<T>::m_bFree = false;


#endif // __DTK_CSingletonMgr_H__ 