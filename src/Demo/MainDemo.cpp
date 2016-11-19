
#include <iostream>
#include "DTK_String.h"

int main(void)
{
    int iRet = 0;

    iRet = DTK_Strcasecmp("ABCdef", "abcdef");

    iRet = DTK_Strcasecmp("ABCdefd", "abcdef");

    iRet = DTK_Strcasecmp("ACdef", "abcdef");
    return 0;
}
