
#ifndef __ATOMIC_H__  
#define __ATOMIC_H__  

#if (defined OS_POSIX)

//Check GCC version, just to be safe
#if !defined(__GNUC__) || (__GNUC__ < 4) || (__GNUC_MINOR__ < 1)  
# error atomic.h works only with GCC newer than version 4.1  
#endif /* GNUC >= 4.1 */  
  
//Atomic type
typedef struct
{  
    volatile int counter;
}atomic_t;  
  
#define ATOMIC_INIT(i)  { (i) }
  
static inline int atomic_read(const atomic_t *v)  
{  
    return v->counter;  
}

static inline void atomic_set(atomic_t *v, int i)  
{  
    v->counter = i;  
}

static inline void atomic_add( int i, atomic_t *v )  
{  
    (void)__sync_add_and_fetch(&v->counter, i);  
}
 
static inline void atomic_sub( int i, atomic_t *v )  
{  
    (void)__sync_sub_and_fetch(&v->counter, i);  
}

static inline int atomic_sub_and_test( int i, atomic_t *v )  
{  
    return !(__sync_sub_and_fetch(&v->counter, i));  
}

static inline void atomic_inc( atomic_t *v )  
{  
    (void)__sync_fetch_and_add(&v->counter, 1);  
}

static inline void atomic_dec( atomic_t *v )  
{  
    (void)__sync_fetch_and_sub(&v->counter, 1);  
}

static inline int atomic_dec_and_test( atomic_t *v )  
{  
    return !(__sync_sub_and_fetch(&v->counter, 1));  
}
 
static inline int atomic_inc_and_test( atomic_t *v )  
{  
    return !(__sync_add_and_fetch(&v->counter, 1));  
}
 
static inline int atomic_add_negative( int i, atomic_t *v )  
{  
    return (__sync_add_and_fetch(&v->counter, i) < 0);  
}

static inline int atomic_cmpxchg(atomic_t *v, int old_val, int new_val)
{
    return (__sync_val_compare_and_swap(&v->counter, old_val, new_val));
}

#endif

#endif // __ATOMIC_H__ 
