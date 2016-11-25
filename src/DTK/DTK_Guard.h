
#ifndef __DTK_GUARD_H__  
#define __DTK_GUARD_H__  

#include "DTK_Mutex.h"

//c++ ±àÒëÆ÷²ÅÖ§³Ö
#if defined (CXXCOMPILE)

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

#endif // __DTK_GUARD_H__ 