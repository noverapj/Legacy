// Thread.cpp: implementation of the Thread class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "Thread.h"
#include "ThreadManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Thread::Thread() : m_hThred(0L),m_threadID(0)
{
}

Thread::~Thread()
{
	CloseHandle(m_hThred);
	m_threadID = 0;
}

DWORD WINAPI Thread::HandleRunner(LPVOID parameter)
{
	Thread *pThread = (Thread *)parameter;
	if(pThread == NULL)
		return 0;

	pThread->Run();
	return 0;
}

void Thread::Begin()
{
	m_hThred = ThreadManager::GetInstance()->Spawn(HandleRunner,this,&m_threadID);
	if(m_hThred == 0)
		return;
}
