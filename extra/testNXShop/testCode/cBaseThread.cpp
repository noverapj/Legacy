// cBaseThread.cpp: implementation of the cBaseThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cBaseThread.h"
#include <process.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cBaseThread::cBaseThread()
{
	Init();
}

cBaseThread::~cBaseThread()
{
	Destroy();
}

void cBaseThread::Init()
{
	m_threadId		= 0;
	m_thread		= 0;

	m_terminated	= FALSE;
}

void cBaseThread::Destroy()
{
	// if still alive, terminate thread
	if( !IsTerminated() )
    {
        Terminate();
    }

 	// close thread handle
    if( m_thread )
	{
		CloseHandle( m_thread );
		m_thread = NULL;
	}
}


//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

// Thread시작
BOOL cBaseThread::StartThread()
{
    if( IsRunning() ) return FALSE;

    m_thread = reinterpret_cast<HANDLE>(_beginthreadex(	NULL, 
					0, 
					&ThreadProc, 
					this, 
					0, 
					reinterpret_cast<UINT*>(&m_threadId) ) );
    if( m_thread == 0 )
    {
        m_threadId = 0;
        return FALSE;
    }
    return TRUE;
}

unsigned __stdcall cBaseThread::ThreadProc( void* handle )
{
	unsigned retCode = 0;

    cBaseThread* thread = static_cast<cBaseThread*>(handle);
	
	while( !thread->IsRunning() )
	{
		// 스레드 핸들을 제대로 넘겨받지 못했을 경우 기다린다.
		Sleep( 1 );
	}

	// thread loop
    thread->Process();

    _endthreadex( retCode );
    return retCode;
}

 
