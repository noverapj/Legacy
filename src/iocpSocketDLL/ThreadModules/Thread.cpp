#include "../iocpSocketDLL.h"
#include "Thread.h"
#include "ThreadManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Thread::Thread() : m_hThread(0L),m_threadID(0)
{
}

Thread::~Thread()
{
	if(m_hThread)
	{
		CloseHandle(m_hThread);
		m_threadID = 0;
	}
}

unsigned WINAPI Thread::HandleRunner(LPVOID parameter)
{
	Thread *pThread = (Thread *)parameter;
	if(pThread == NULL)
		return 0;

	pThread->Run();
	return 0;
}

BOOL Thread::Begin(int iThreadCount)
{
	m_hThread = ThreadManager::GetInstance()->Spawn(HandleRunner,this,m_threadID,iThreadCount);
	return (m_hThread == 0) ? FALSE : TRUE;
}
