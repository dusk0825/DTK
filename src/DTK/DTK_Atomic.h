
#ifndef __DTK_ATOMIC_H__  
#define __DTK_ATOMIC_H__  

#include "DTK_Config.h"
#include "DTK_Types.h"

#if defined(OS_WINDOWS)
	typedef LONG DTK_ATOMIC_T;
#elif defined(OS_POSIX)
    //#include <asm/atomic.h>
    #include "Atomic.h"
    typedef atomic_t DTK_ATOMIC_T;
#else
	#error OS Not Implement Yet.
#endif

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_AtomicInc(DTK_ATOMIC_T* pVal)
*   @brief 原子的自增一
*   @param [in] pVal    自增的变量指针
*   @return 无
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_AtomicInc(DTK_ATOMIC_T* pVal);

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_AtomicDec(DTK_ATOMIC_T* pVal)
*   @brief 原子的自减一
*   @param [in] pVal    自减的变量指针
*   @return 无
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_AtomicDec(DTK_ATOMIC_T* pVal);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicSet(DTK_ATOMIC_T* pDst,DTK_ATOMIC_T nVal)
*   @brief 原子的赋值，将nVal的值，替换到pDst指向的内存中
*   @param [in] pDst    目标地址
*   @param [in] nVal    替换变量
*   @return pDst的初始值
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicSet(DTK_ATOMIC_T* pDst,DTK_INT32 nVal);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicRead(DTK_ATOMIC_T* pVal)
*   @brief 返回原子变量的值
*   @param [in] pVal    原子变量指针
*   @return 原子变量的值
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicRead(DTK_ATOMIC_T* pVal);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicAdd(DTK_ATOMIC_T* pDst,DTK_ATOMIC_T nVal)
*   @brief 原子的加法
*   @param [in] pDst    初值
*   @param [in] nVal    加值
*   @return pDst的初始值
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicAdd(DTK_ATOMIC_T* pDst,DTK_INT32 nVal);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicSub(DTK_ATOMIC_T* pDst,DTK_ATOMIC_T nVal)
*   @brief 原子的减法
*   @param [in] pDst    初值
*   @param [in] nVal    减值
*   @return pDst的初始值
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicSub(DTK_ATOMIC_T* pDst,DTK_INT32 nVal);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicCmpExChg(DTK_ATOMIC_T* pDst,DTK_ATOMIC_T nExchg,DTK_ATOMIC_T nCmp)
*   @brief 原子的比较并赋值。pDst与nCmp值相同，则将nExchg的值赋值给pDst。
*   @param [in] pDst    初值
*   @param [in] nVal    赋值
*   @param [in] nVal    比较值
*   @return pDst的初始值
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicCmpExChg(DTK_ATOMIC_T* pDst,DTK_INT32 nExchg,DTK_INT32 nCmp);

#endif // __DTK_ATOMIC_H__ 