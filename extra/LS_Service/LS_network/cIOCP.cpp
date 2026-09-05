#include "stdafx.h"
#include "cIOCP.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cIOCP::cIOCP(void)
{
	Init();	
}

cIOCP::~cIOCP(void)
{
	Destroy();
}

void cIOCP::Init()
{
	m_completionPort = NULL;
}

void cIOCP::Destroy()
{
	Close();
}

//////////////////////////////////////////////////////////////////////
// Operation
//////////////////////////////////////////////////////////////////////

// create new I/O completion port
BOOL cIOCP::Create()
{
	m_completionPort = CreateIoCompletionPort(	INVALID_HANDLE_VALUE, 
												NULL, 
												0, 
												0 );

	return (m_completionPort ? TRUE : FALSE);
}

// destroy I/O completion port
void cIOCP::Close()
{
	if( m_completionPort ) 
	{
		CloseHandle( m_completionPort );
		m_completionPort = NULL;
	}
}

// associate an instance
BOOL cIOCP::AssocInstance( HANDLE socket, DWORD ioKey )
{
	HANDLE	hIocpPort = CreateIoCompletionPort(	socket,
												m_completionPort,
												ioKey,
												0 );
	return (hIocpPort ? TRUE : FALSE); 
}

// dequeue an I/O completion packet from IOCP
BOOL cIOCP::GetIocpStatus(	LPDWORD bytesTransferred, 
							PULONG completionKey, 
							LPOVERLAPPED* overlapped, 
							DWORD milliseconds  )
{
	return GetQueuedCompletionStatus(	m_completionPort,
										bytesTransferred,
										completionKey,
										overlapped,
										milliseconds ) ? TRUE : FALSE;
}