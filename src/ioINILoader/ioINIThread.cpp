#include "ioINIThread.h"
#include "stdafx.h"

ioINIThread::ioINIThread(void)
{
}


ioINIThread::~ioINIThread(void)
{
}

unsigned WINAPI ioINIThread::HandleRunner(void* parameter)
{
	ioINIThread *pThread = (ioINIThread*)parameter;
	if(pThread == NULL)
		return 0;

	pThread->Run();
	return 0;
}

BOOL ioINIThread::Begin(int iThreadCount)
{
	UINT threadId = 0;

	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, HandleRunner, this, 0, &threadId ));
	if(!m_hThread)
	{
		return FALSE;
	}

	return (m_hThread == 0) ? FALSE : TRUE;
}
