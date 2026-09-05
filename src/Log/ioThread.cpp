#include "stdafx.h"
#include "ioThread.h"


ioThread::ioThread(void) : m_hThread(0L), m_threadID(0)
{
}

ioThread::~ioThread(void)
{
	if(m_hThread)
	{
		CloseHandle(m_hThread);
		m_threadID = 0;
	}
}

unsigned WINAPI ioThread::HandleRunner(LPVOID parameter)
{
	ioThread *pThread = (ioThread *)parameter;
	if(pThread == NULL)
		return 0;

	pThread->Run();
	return 0;
}

BOOL ioThread::Begin()
{
	HANDLE threadHandle = reinterpret_cast<HANDLE>( _beginthreadex(
		NULL, 
		0, 
		HandleRunner, 
		this, 
		0, 
		&m_threadID ));
	if(!threadHandle)
	{
		return FALSE;
	}

	m_hThread = threadHandle;

	return (m_hThread == 0) ? FALSE : TRUE;
}
