// cAcceptThread.cpp: implementation of the cAcceptThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cIocpContext.h"
#include "cAcceptThread.h"

#include "cSystem.h"
#include "cSingleton.h"



void	Trace( char *lpszFormat, ... );



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cAcceptThread::cAcceptThread()
{
	Init();
}

cAcceptThread::~cAcceptThread()
{
	Destroy();
}

void	cAcceptThread::Init()
{
}
void	cAcceptThread::Destroy()
{
}

void cAcceptThread::Process()
{
	/*
	SOCKADDR sa;
	cIocpContext* pIocpContext = NULL;
	
	cSystem* pSystem = cSingleton<cSystem>::GetInstance();
	
	cServerSocket* pServerSocket		= pSystem->GetServerSocket();
	cIocpOperation* pIocpOperation		= pSystem->GetIocpOperation();
	cIocpContextPool* pIocpContextPool	= pSystem->GetIocpContextPool();

	Trace( "AcceptThread :: %lu\n", GetCurrentThreadId() );

	while( !IsTerminated() )
	{
		// wait for a client
		SOCKET hSocket = pServerSocket->AcceptRequest( sa );
		if( hSocket == INVALID_SOCKET ) 	continue;

		// pop a context from queue
		pIocpContext = pIocpContextPool->Pop();
		if( !pIocpContext )		continue;

		// reset all data
		pIocpContext->Reset();

		// sets a socket handle and option
		pIocpContext->SetSocketHandle( hSocket );
		pIocpContext->SetSocketOption();
		
		// associcate a socket 
		if( pIocpOperation->AssocInstance( (HANDLE)hSocket, pIocpContext ) )
		{
			if( pIocpContext->ReceiveRequest() )	
			{
				// associated successfully and issue a receive 
				Trace( "connected, entity = %lu\n", pIocpContext->GetEntity() );
				continue;
			}
		}

		pIocpContext->Sleep();
		pIocpContextPool->Push( pIocpContext );
	}
	*/
}
