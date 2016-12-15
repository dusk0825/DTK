
#include "DTK_Dtk.h"
#include "DTK_Guard.h"

extern DTK_INT32 DTK_InitNetwork_Inter();
extern DTK_INT32 DTK_FiniNetwork_Inter();

static DTK_Mutex s_DTKmutex;
static DTK_INT32 s_iInitCount = 0;
static DTK_BOOL  s_bDTKInitNetwork = DTK_FALSE;

static DTK_INT32 DTK_InitResource_Inter();
static DTK_VOID DTK_ClearResource_Inter();


static DTK_INT32 DTK_InitResource_Inter()
{
    s_bDTKInitNetwork = DTK_FALSE;

    do 
    {
        if ( DTK_InitNetwork_Inter() )
        {
            break;
        }
        s_bDTKInitNetwork = DTK_TRUE;

        return DTK_OK;
    } while (0);
    
    DTK_ClearResource_Inter();
    return DTK_ERROR;
}

static DTK_VOID DTK_ClearResource_Inter()
{
    if (s_bDTKInitNetwork)
    {
        DTK_FiniNetwork_Inter();
        s_bDTKInitNetwork = DTK_FALSE;
    }

    return;
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Init()
{
    DTK_Guard guard(&s_DTKmutex);

    if (0 == s_iInitCount)
    {
        if (DTK_OK != DTK_InitResource_Inter())
        {
            return DTK_ERROR;
        }
    }

    ++ s_iInitCount;
    return DTK_OK;
}

DTK_DECLARE DTK_VOID CALLBACK DTK_Fini()
{
    DTK_Guard guard(&s_DTKmutex);

    -- s_iInitCount;
    if (s_iInitCount == 0)
    {
        DTK_ClearResource_Inter();
    }
}