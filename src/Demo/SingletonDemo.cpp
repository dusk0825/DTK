
#include "MainDemo.h"

class CTest
{
public:

    void Show()
    {
        std::cout << __FUNCTION__ << std::endl;
    }
};

typedef CSingletonMgr<CTest> CTestSingleton;

void Test_Singleton()
{
    CTestSingleton::Instance()->Show();
}
