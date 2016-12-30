
#include "DTK_ThreadPoolFlex.h"
#include "DTK_Mutex.h"
#include "DTK_Thread.h"
#include "DTK_Semaphore.h"
#include "DTK_Atomic.h"
#include <sys/timeb.h>


void gettimeofnow(struct timeval* tms)
{
#ifdef OS_WINDOWS
    struct _timeb tb;
    _ftime(&tb);
#elif defined OS_POSIX
    struct timeb tb;
    ftime(&tb);
#endif

    tms->tv_sec = (long)tb.time;
    tms->tv_usec = tb.millitm * 1000;
}

typedef struct work_item_tag 
{
    struct work_item_tag *next;
    void *data;
    struct timeval time_queued;
} work_item_t;

typedef struct work_queue_tag 
{
    work_item_t *head;
    work_item_t *tail;
    int item_count;
} work_queue_t;

typedef enum 
{
    POOL_VALID,
    POOL_EXIT
} pool_state_t;

typedef struct threadpool_tag 
{
    int thr_stacksize;
    pool_state_t state;
    int thr_max;
    int thr_alive;
    int thr_idle;
    int idle_timeout;

    void (CALLBACK *handler)(void *);

    DTK_SEM_T pool_sem;	//����֪ͨ���һ���߳��˳�
    DTK_SEM_T pool_sig_sem;	//����������֪ͨΪ�ź���֪ͨ
    DTK_MUTEX_T pool_mutex;	//����Ϊ����������в���

    work_queue_t *queue;
} threadpool_t;


//////////////////////////////////////////////////////////////////////////

static work_queue_t *work_queue_new(void)
{
    work_queue_t *work_q = (work_queue_t *)malloc(sizeof(work_queue_t));
    if (NULL == work_q) 
    {
        return NULL;
    }

    memset(work_q, 0, sizeof(*work_q));
    return work_q;
}

static DTK_BOOL work_queue_add(work_queue_t *work_q, void *data)
{
    if (NULL == work_q) 
    {
        return DTK_FALSE;
    }

    work_item_t *work_item = (work_item_t *)malloc(sizeof(work_item_t));
    if (NULL ==work_item) 
    {
        return DTK_FALSE;
    }

    work_item->next = NULL;
    work_item->data = data;
    gettimeofnow(&(work_item->time_queued));

    if (work_q->head == NULL) 
    {
        work_q->head = work_q->tail = work_item;
        work_q->item_count = 1;
    } 
    else 
    {
        work_q->tail->next = work_item;
        work_q->tail = work_item;
        work_q->item_count += 1;
    }

    return DTK_TRUE;
}

static DTK_BOOL work_queue_pop(work_queue_t *work_q, void** pparam)
{
    if (NULL == work_q || NULL == work_q->head) 
    {
        return DTK_FALSE;
    }

    work_item_t *work_item = work_q->head;
    *pparam = work_item->data;
    work_q->head = work_item->next;
    work_q->item_count -= 1;
    if (work_q->head == NULL) 
    {
        work_q->tail = NULL;
    }
    free(work_item);

    return DTK_TRUE;
}

static void work_queue_free(work_queue_t *work_q)
{
    if (NULL == work_q)
    {
        return;
    }

    void* tmp = NULL;
    while (work_q->item_count != 0)
    {
        work_queue_pop(work_q, &tmp);
    }

    free(work_q);
}

//////////////////////////////////////////////////////////////////////////

void thrmgr_destroy(void *p)
{
    threadpool_t *threadpool = (threadpool_t *)(p);

    if (threadpool == NULL || (threadpool->state != POOL_VALID)) 
    {
        return;
    }

    threadpool->state = POOL_EXIT;

    int existed = 0;
    //���Ӵ˱�������ж��Ƿ���Ҫ�ȴ������źţ�����������ʱ����ֶ���������
    DTK_BOOL bNeedWait = DTK_FALSE;
    //����Ҫ����������destroy��ʱ����dispatch������һ���߳��ٷ�һ���ź���ɶ���
    DTK_MutexLock(&(threadpool->pool_mutex));
    //����ʱȡ��ǰ����߳��������Ͷ�Ӧ�������ź�֪ͨ���߳��˳�
    if ((existed = threadpool->thr_alive) > 0) 
    {
        bNeedWait = DTK_TRUE;
        while (existed--)
        {
            DTK_SemPost(&(threadpool->pool_sig_sem));
        }
    }
    DTK_MutexUnlock(&(threadpool->pool_mutex));

    //�ȴ����һ���߳��˳�
    if (bNeedWait)
    {
        DTK_SemWait(&threadpool->pool_sem);
    }

    DTK_SemDestroy(&(threadpool->pool_sem));
    DTK_SemDestroy(&(threadpool->pool_sig_sem));
    DTK_MutexDestroy(&(threadpool->pool_mutex));

    work_queue_free(threadpool->queue);
    free(threadpool);
    return;
}

void *thrmgr_new(int max_threads, int idle_timeout, void (CALLBACK *handler)(void *), int stack_size)
{
    if (stack_size < 0 || max_threads <= 0 || handler == NULL)
    {
        return NULL;
    }

    threadpool_t *threadpool = (threadpool_t *)malloc(sizeof(threadpool_t));
    if (threadpool == NULL) 
    {
        return NULL;
    }

    threadpool->queue = work_queue_new();
    if (threadpool->queue == NULL) 
    {
        free(threadpool);
        return NULL;
    }	

    DTK_MutexCreate(&(threadpool->pool_mutex));
    DTK_SemCreate(&(threadpool->pool_sem), 0);
    DTK_SemCreate(&(threadpool->pool_sig_sem), 0);

    threadpool->thr_max = max_threads;
    threadpool->thr_alive = 0;
    threadpool->thr_idle = 0;
    threadpool->idle_timeout = idle_timeout;
    threadpool->handler = handler;

    //��0ΪĬ��1MB
    if (stack_size == 0)
    {
        threadpool->thr_stacksize = 1024 * 1024;
    }
    //��Ĭ���߳�ջ��С��Ҫ�޸��߳�����
#ifdef OS_WINDOWS
    else if (stack_size < 64 * 1024) 
    {
        threadpool->thr_stacksize = 64 * 1024;
    }
#elif defined OS_POSIX
    else if (stack_size < 256 * 1024) 
    {
        threadpool->thr_stacksize = 256 * 1024;
    }
#endif
    else
    {
        threadpool->thr_stacksize = stack_size;
    }

    threadpool->state = POOL_VALID;

    return threadpool;
}

static void * CALLBACK thrmgr_worker(void *arg)
{
    threadpool_t *threadpool = (threadpool_t *)arg;
    void *job_data = NULL;
    int retval = DTK_OK;
    DTK_BOOL get_data = DTK_FALSE;
    //����������Ҫ���Լ����Ĳ������ж϶�����Ϊ���У�������ֲ����жϵ��´���
    DTK_BOOL bSingal = DTK_FALSE;

    while (threadpool->state != POOL_EXIT)
    {
        DTK_AtomicInc((DTK_ATOMIC_T*)&(threadpool->thr_idle));

        retval = DTK_SemTimedWait(&(threadpool->pool_sig_sem), (unsigned)threadpool->idle_timeout);
        //ֻʣ��һ���̵߳�ʱ��ֱ���˳�
        //���һ���߳��˳�ʱ��Ϊ�ź���֪ͨ��������������Ӧ��������һ���߳�����
        //����ᵼ���ͷ��źŷ������������ʱ����
        if ((retval == DTK_ERROR && threadpool->thr_alive > 1)  || threadpool->state == POOL_EXIT)
        {
            break;
        }

        DTK_AtomicDec((DTK_ATOMIC_T*)&(threadpool->thr_idle));

        //����ʹ���û�������Ϊ�Ƿ�ȡ�����ݵ��ж����ݣ���Ϊ���û����ݴ��κ�ֵ���п���
        //��ˣ��������������ж�
        DTK_MutexLock(&(threadpool->pool_mutex));
        get_data = work_queue_pop(threadpool->queue, &job_data);
        DTK_MutexUnlock(&(threadpool->pool_mutex));

        if (get_data) 
        {
            threadpool->handler(job_data);
            get_data = DTK_FALSE;
        }
    }

    DTK_MutexLock(&(threadpool->pool_mutex));
    -- threadpool->thr_alive;
    //���������ж�Ϊ0���ñ��һ��Ҫ�ڻ��Ᵽ���н���
    if (threadpool->thr_alive == 0)
    {
        bSingal = DTK_TRUE;
    }
    DTK_MutexUnlock(&(threadpool->pool_mutex));

    //ֻ�����һ���߳��˳�ʱ�������ź�
    if (bSingal) 
    {
        DTK_SemPost(&threadpool->pool_sem);
    }

    return NULL;
}

DTK_BOOL thrmgr_dispatch(void* p, void *user_data)
{
    threadpool_t *threadpool = (threadpool_t *)(p);
    if (threadpool == NULL) 
    {
        return DTK_FALSE;
    }

    DTK_MutexLock(&(threadpool->pool_mutex));
    if (threadpool->state != POOL_VALID) 
    {
        DTK_MutexUnlock(&(threadpool->pool_mutex));
        return DTK_FALSE;
    }

    if (!work_queue_add(threadpool->queue, user_data)) 
    {
        DTK_MutexUnlock(&(threadpool->pool_mutex));
        return DTK_FALSE;
    }

    if ((threadpool->thr_idle == 0) && (threadpool->thr_alive < threadpool->thr_max)) 
    {
        if (!DTK_Thread_Create((void* (CALLBACK*)(void*))thrmgr_worker, threadpool, (unsigned)threadpool->thr_stacksize)) 
        {
            //��һ���߳̾ʹ���ʧ�ܾͷ���ʧ�ܣ�����ʱ�����ٻ���һ���߳̿���
            if (threadpool->thr_alive == 0)
            {
                DTK_MutexUnlock(&(threadpool->pool_mutex));
                return DTK_FALSE;
            }
        }
        else 
        {
            //��һ���߳�Ҫ����һ��������
            ++ threadpool->thr_alive;
        }
    }

    DTK_MutexUnlock(&(threadpool->pool_mutex));

    //ÿ��һ��������������һ���ź�
    DTK_SemPost(&(threadpool->pool_sig_sem));
    return DTK_TRUE;
}


DTK_DECLARE DTK_HANDLE CALLBACK DTK_ThreadPoolFlex_Create(DTK_UINT32 nMaxThreadNum, DTK_UINT32 nIldeTime, DTK_VOIDPTR(CALLBACK *fWorkRoutine)(DTK_VOIDPTR), DTK_UINT32 nStackSize)
{
    return thrmgr_new((int)nMaxThreadNum, (int)nIldeTime, (void(CALLBACK *)(void*))fWorkRoutine, nStackSize);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPoolFlex_Destroy(DTK_HANDLE hHandle)
{
    thrmgr_destroy(hHandle);
    return DTK_OK;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_ThreadPoolFlex_Work(DTK_HANDLE hHandle, DTK_VOIDPTR pParam)
{
    return thrmgr_dispatch(hHandle, pParam) ? DTK_OK : DTK_ERROR;
}