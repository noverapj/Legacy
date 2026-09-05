// cClientSocket.cpp: implementation of the cClientSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cSystem.h"
#include "cClientSocket.h"
#include "../include/cSingleton.h"
#include <process.h>


extern DWORD _tinet_addr(const TCHAR *IP);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cClientSocket::cClientSocket()
{
	Init();
}

cClientSocket::~cClientSocket()
{
	Destroy();
}

void cClientSocket::Init()
{
	m_socket = INVALID_SOCKET;

	ZeroMemory( &m_sockAddr, sizeof(SOCKADDR_IN) );
}

void cClientSocket::Destroy()
{
	CloseSocket();
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////
BOOL cClientSocket::Startup()
{
	WSADATA wsaData;
	ZeroMemory( &wsaData, sizeof(WSADATA) );

    if( WSAStartup(MAKEWORD( 2, 2 ), &wsaData) != 0 ) 	
	{
		Trace( _T("cClientSocket::Startup() Failed\n") );
		return FALSE;
    }
	return TRUE;
}

void cClientSocket::Cleanup()
{
	WSACleanup();
}

BOOL cClientSocket::Create()
{
	if(INVALID_SOCKET != m_socket)
		return TRUE;

	m_socket = WSASocket(	AF_INET, 
							SOCK_STREAM, 
						    IPPROTO_TCP,
							NULL,
							NULL,
							WSA_FLAG_OVERLAPPED );

	if( m_socket != INVALID_SOCKET )
	{
		return SetSocketOption();
	}
	return FALSE;
}

BOOL cClientSocket::Connect( const TCHAR *IP, const uint16 port )
{
	if( IP )	// IP와 포트가 입력됐을 경우 새로운 주소로 접속을 시도한다
	{
		m_sockAddr.sin_family		= AF_INET;
		m_sockAddr.sin_addr.s_addr	= _tinet_addr(IP);
		m_sockAddr.sin_port			= htons(port);	
	}
	else
	{
		
		int32 nResult = connect(m_socket, 
								(LPSOCKADDR)&m_sockAddr, 
								sizeof(SOCKADDR_IN) );
		if( (nResult != SOCKET_ERROR) || (WSAGetLastError() == WSAEWOULDBLOCK) )
		{
			OnConnected();
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

BOOL cClientSocket::Reconnect()
{
	CloseSocket();
	OnDisconnected();

	while(!Create())
	{
		CloseSocket();
		Sleep( 3000 );
		Debug(_T("-Reconnect failed(socket)\n"));
	}

	while(TRUE)
	{
		if(Connect())
			break;

		Sleep( 2000 );
		Create();
	}
	return TRUE;
}


void cClientSocket::CloseSocket()
{
	if( m_socket != INVALID_SOCKET )
	{
		shutdown( m_socket, SD_BOTH );
		closesocket( m_socket );
	}

	SetSocketHandle( INVALID_SOCKET );
}

//////////////////////////////////////////////////////////////////////
// Socket Control
//////////////////////////////////////////////////////////////////////
BOOL cClientSocket::SetSocketOption()
{
	BOOL bSockOpt = TRUE;

	if( setsockopt( m_socket, 
					IPPROTO_TCP, 
					TCP_NODELAY, 
					reinterpret_cast<char*>(&bSockOpt), 
					sizeof(bSockOpt) ) == SOCKET_ERROR ) 
	{
		CloseSocket();
		return FALSE;
	}

	LINGER lingerStruct;
	lingerStruct.l_onoff	= 1;
	lingerStruct.l_linger	= 0;

	if( setsockopt( m_socket, 
					SOL_SOCKET, 
					SO_LINGER,
					reinterpret_cast<char*>(&lingerStruct), 
					sizeof(lingerStruct) ) == SOCKET_ERROR )
	{
		CloseSocket();
		return FALSE;
	}
	return TRUE;
}

BOOL cClientSocket::Send( const uint8 *buffer, const uint32 length )
{
	if( m_socket != INVALID_SOCKET )
	{
		int32 bytesSent = 0, totalSent = 0;

		while( totalSent != length )
		{
			bytesSent	 = send(	m_socket, 
									const_cast<char*>(reinterpret_cast<const char*>(buffer) + totalSent), 
									(length-totalSent), 
									0 );

			if( (bytesSent == SOCKET_ERROR) && (WSAGetLastError() != WSAEWOULDBLOCK) )
			{
				CloseSocket();
				return FALSE;
			}

			totalSent += bytesSent;
		}
		return TRUE;
	}
	CloseSocket();
	return FALSE;
}

BOOL cClientSocket::Receive( uint8* buffer, uint32& length )
{
	length = recv(	m_socket,
					reinterpret_cast<char*>(buffer), 
					length, 
					0 );

	if( (length == SOCKET_ERROR) || (length <= 0) )		return FALSE;
	return TRUE;
}

void cClientSocket::Process()
{
	while( TRUE )
	{
		if( Connect() )
			break;

		Sleep( 1000 );
	}

	const uint32 BUFFERLEN = cSingleton<cSystem>::GetInstance()->GetPacket();

	uint32 	recvBytes = 0;
	uint8*	receiveBuffer = new uint8[BUFFERLEN];

	while( TRUE )
	{
		recvBytes = BUFFERLEN;
		if( Receive( receiveBuffer, recvBytes ) )
		{
			OnReceived( reinterpret_cast<uint8*>(receiveBuffer), recvBytes );	// virtual
		}
		else
		{
			// Reconnect
			Reconnect();
			continue;
		}
	}

	delete []receiveBuffer;
}

