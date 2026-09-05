// cNetwork.cpp: implementation of the cNetwork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cIocpContext.h"
#include "cConnectorQueueElem.h"
#include "cIOCP.h"
#include "cSystem.h"
#include "cNetwork.h"
#include "../include/cSingleton.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cNetwork::cNetwork()
{
	Init();
}

cNetwork::~cNetwork()
{
	Destroy();
}

void cNetwork::Init()
{
}

void cNetwork::Destroy()
{
}

BOOL cNetwork::Startup( const uint32 maximum )
{
	WSADATA WsaData;
    if( WSAStartup(MAKEWORD( 2, 2 ), &WsaData) != 0 ) 	
	{
		Trace( _T("WSAStartup Error :: %lu\n"), WSAGetLastError() );
	   return FALSE;
    }

	m_pooler.Create(maximum);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

// Socket 생성
BOOL cNetwork::CreateServerSocket( uint16 port, int32 backlog, uint32& portId )
{
	if(m_serverSocketManager.AddSocket(port, backlog, portId))
	{
		cServerSocket *serverSocket = m_serverSocketManager.GetSocket( portId );
		if(serverSocket)
		{
			cIOCP* IOCP = cSingleton<cSystem>::GetInstance()->GetIOCP();
			return IOCP->AssocInstance(	reinterpret_cast<HANDLE>(serverSocket->GetSocketHandle()), 
											reinterpret_cast<DWORD>(serverSocket) );
			return TRUE;
		}
	}
	return FALSE;
}

// Accept요청
BOOL cNetwork::AcceptReqeust( uint32 portId )
{
	// ServerSocket을 얻어낸다
	cServerSocket* serverSocket = m_serverSocketManager.GetSocket( portId );
	if( !serverSocket )	
	{
		// 이 경우는 발생할 수 없다.
		Information( _T("[%lu]LS system :: accept failed - critical\n"), GetCurrentThreadId() );
		return FALSE;
	}

	// backlog만큼 Accept요청
	uint32 tryCount = 0;
	while( serverSocket->GetAcceptingCount() < serverSocket->GetBacklog() )
	{
		cIocpContext* iocpContext = m_pooler.Pop();
		if( !iocpContext ) 
		{
			// 메모리 풀이 비어있다.
			//Debug( _T("[%lu]LS system :: accept failed - socket pool is empty\n"), GetCurrentThreadId() );
			break;
		}

		iocpContext->SetPortID( portId );
		iocpContext->CleanOverlapped(E_IO_ACCEPT);

		for( tryCount = 1 ; tryCount <= 5 ; tryCount++ )
		{
			if( !serverSocket->AcceptRequest(	iocpContext->GetSockHandle(), 
												iocpContext->GetWsaBuffer(),
												iocpContext->GetOverlapped(E_IO_ACCEPT) ) )
			{
				Debug( _T("[%lu]LS system :: socket create\n"), GetCurrentThreadId() );

				iocpContext->CloseSocket();
				iocpContext->CreateSocket();
			}
			else
			{
				// successful request.
				break;
			}
		}
	}

	if( serverSocket->GetAcceptingCount() == 0 )
	{
		Debug( _T("[%lu]LS system :: empty socket\n"), GetCurrentThreadId() );
	}
	else
	{
		//Debug( _T("[%lu]LS system :: socket(%lu)\n"), GetCurrentThreadId(), serverSocket->GetAcceptingCount() );
	}

	return TRUE;
}

// Accept 요청이 완료된 후의 처리
void cNetwork::AcceptCompleted( uint32 portId )
{
	// ServerSocket을 얻어낸다
	cServerSocket* serverSocket = m_serverSocketManager.GetSocket( portId );
	if( serverSocket )
	{
		serverSocket->DecreaseAcceptingCount();
	}
}

// 모든 일을 마친 Context에 대한 처리
void cNetwork::Finish( cIocpContext* iocpContext )
{
	m_pooler.Push(iocpContext);

	// Accept요청이 없을 경우 요청하도록 함
	const uint32 portId = iocpContext->GetPortID();
	cServerSocket* serverSocket = m_serverSocketManager.GetSocket( portId );
	if( serverSocket )
	{
		AcceptReqeust( portId );
	}
}

// Connector추가
BOOL cNetwork::AddConnector( const uint32 ID, const TCHAR* IP, const uint16 port )
{
	return m_connectorManager.AddConnector( ID, IP, port );
}

// Connector를 통해 전달
void cNetwork::SendConnector( cConnectorQueueElem* command )
{
	m_connectorManager.Execute( command );
}
