
#include "DTK_String.h"

DTK_DECLARE DTK_INT32 CALLBACK DTK_Strcasecmp(const char* s1, const char* s2)
{
    while(toupper((unsigned char)*s1) == toupper((unsigned char)*s2)) 
    {
        if(*s1 == '\0')
        {
            return 0;
        }
        s1++;
        s2++;
    }

    return toupper((unsigned char)*s1) - toupper((unsigned char)*s2);
}

DTK_DECLARE DTK_INT32 CALLBACK DTK_Strncasecmp(const char* s1, const char* s2, int n)
{
    while((n > 0) && (toupper((unsigned char)*s1) == toupper((unsigned char)*s2)))
    {
        if(*s1 == '\0')
        {
            return 0;
        }
        s1++;
        s2++;
        n--;
    }

    if(n == 0)
    {
        return 0;
    }

    return toupper((unsigned char)*s1) - toupper((unsigned char)*s2);
}