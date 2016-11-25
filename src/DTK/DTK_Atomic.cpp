
#include "DTK_Atomic.h"

DTK_DECLARE DTK_VOID CALLBACK DTK_AtomicInc(DTK_ATOMIC_T* pVal)
{
#if OS_WINDOWS
	InterlockedIncrement(pVal);
#elif OS_POSIX
    atomic_inc(pVal);
#endif
}

DTK_DECLARE DTK_VOID CALLBACK DTK_AtomicDec(DTK_ATOMIC_T* pVal)
{
#if OS_WINDOWS
	InterlockedDecrement(pVal);
#elif OS_POSIX
    atomic_dec(pVal);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicSet(DTK_ATOMIC_T* pDst,DTK_INT32 nVal)
{
#if OS_WINDOWS
    return InterlockedExchange(pDst, nVal);
#elif OS_POSIX
    DTK_INT32 iOldValue = pDst->counter; 
    atomic_set(pDst, nVal);
    return iOldValue;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicRead(DTK_ATOMIC_T* pVal)
{
#if OS_WINDOWS
    return InterlockedExchange(pVal, *pVal);
#elif OS_POSIX
    return atomic_read(pVal);
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicAdd(DTK_ATOMIC_T* pDst, DTK_INT32 nVal)
{
#if OS_WINDOWS
	return InterlockedExchangeAdd(pDst,nVal);
#elif OS_POSIX
    DTK_INT32 iOldValue = pDst->counter;
    atomic_add(nVal, pDst);
    return iOldValue;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicSub(DTK_ATOMIC_T* pDst, DTK_INT32 nVal)
{
#if OS_WINDOWS
    return InterlockedExchangeAdd(pDst, -nVal);
#elif OS_POSIX
    DTK_INT32 iOldValue = pDst->counter;
    atomic_sub(nVal, pDst);
    return iOldValue;
#endif
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicCmpExChg(DTK_ATOMIC_T* pDst,DTK_INT32 nExchg,DTK_INT32 nCmp)
{
#if OS_WINDOWS
    return InterlockedCompareExchange(pDst, nExchg, nCmp);
#elif OS_POSIX
    DTK_INT32 iOldValue = pDst->counter;
    atomic_cmpxchg(pDst, nCmp, nExchg);
    return iOldValue;
#endif
}
