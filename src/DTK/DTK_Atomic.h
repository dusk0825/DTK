
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
*   @brief ԭ�ӵ�����һ
*   @param [in] pVal    �����ı���ָ��
*   @return ��
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_AtomicInc(DTK_ATOMIC_T* pVal);

/** @fn DTK_DECLARE DTK_VOID CALLBACK DTK_AtomicDec(DTK_ATOMIC_T* pVal)
*   @brief ԭ�ӵ��Լ�һ
*   @param [in] pVal    �Լ��ı���ָ��
*   @return ��
*/
DTK_DECLARE DTK_VOID CALLBACK DTK_AtomicDec(DTK_ATOMIC_T* pVal);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicSet(DTK_ATOMIC_T* pDst,DTK_ATOMIC_T nVal)
*   @brief ԭ�ӵĸ�ֵ����nVal��ֵ���滻��pDstָ����ڴ���
*   @param [in] pDst    Ŀ���ַ
*   @param [in] nVal    �滻����
*   @return pDst�ĳ�ʼֵ
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicSet(DTK_ATOMIC_T* pDst,DTK_INT32 nVal);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicRead(DTK_ATOMIC_T* pVal)
*   @brief ����ԭ�ӱ�����ֵ
*   @param [in] pVal    ԭ�ӱ���ָ��
*   @return ԭ�ӱ�����ֵ
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicRead(DTK_ATOMIC_T* pVal);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicAdd(DTK_ATOMIC_T* pDst,DTK_ATOMIC_T nVal)
*   @brief ԭ�ӵļӷ�
*   @param [in] pDst    ��ֵ
*   @param [in] nVal    ��ֵ
*   @return pDst�ĳ�ʼֵ
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicAdd(DTK_ATOMIC_T* pDst,DTK_INT32 nVal);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicSub(DTK_ATOMIC_T* pDst,DTK_ATOMIC_T nVal)
*   @brief ԭ�ӵļ���
*   @param [in] pDst    ��ֵ
*   @param [in] nVal    ��ֵ
*   @return pDst�ĳ�ʼֵ
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicSub(DTK_ATOMIC_T* pDst,DTK_INT32 nVal);

/** @fn DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicCmpExChg(DTK_ATOMIC_T* pDst,DTK_ATOMIC_T nExchg,DTK_ATOMIC_T nCmp)
*   @brief ԭ�ӵıȽϲ���ֵ��pDst��nCmpֵ��ͬ����nExchg��ֵ��ֵ��pDst��
*   @param [in] pDst    ��ֵ
*   @param [in] nVal    ��ֵ
*   @param [in] nVal    �Ƚ�ֵ
*   @return pDst�ĳ�ʼֵ
*/
DTK_DECLARE DTK_INT32 CALLBACK DTK_AtomicCmpExChg(DTK_ATOMIC_T* pDst,DTK_INT32 nExchg,DTK_INT32 nCmp);

#endif // __DTK_ATOMIC_H__ 