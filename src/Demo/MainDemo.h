
#ifndef __MAINDEMO_H__  
#define __MAINDEMO_H__  

#include <iostream>
#include <vector>

#include "DTK_Dtk.h"
#include "DTK_String.h"
#include "DTK_Thread.h"
#include "DTK_Error.h"
#include "DTK_Mutex.h"
#include "DTK_Semaphore.h"
#include "DTK_Event.h"
#include "DTK_Utils.h"
#include "DTK_Atomic.h"
#include "DTK_Socket.h"
#include "DTK_Select.h"
#include "DTK_FileSystem.h"
#include "DTK_LogModule.h"
#include "DTK_Timer.h"
#include "DTK_ThreadPool.h"
#include "DTK_ThreadPoolFlex.h"
#include "DTK_Singleton.h"
#include "DTK_AsyncIO.h"
#include "DTK_RWLock.h"


void Test_Event();
void Test_Atomic();
void Test_Sock();
void Test_File();
void Test_Singleton();
void Test_AsyncIo();

#endif // __MAINDEMO_H__ 

