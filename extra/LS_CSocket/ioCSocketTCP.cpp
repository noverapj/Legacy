// TCPSockClient.cpp: implementation of the ioCSocketTCP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ioCSocketTCP.h"


UINT WINAPI TCPSocketThread( LPVOID handle );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ioCSocketTCP::ioCSocketTCP()
{
	Init();
	m_bState = true;
}

ioCSocketTCP::~ioCSocketTCP()
{
	Destroy();
}

void ioCSocketTCP::Init()
{
}

void ioCSocketTCP::Destroy()
{
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL ioCSocketTCP::Create()
{
	m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if(m_socket == INVALID_SOCKET)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL ioCSocketTCP::Connect(const char *IP, int port)
{
	Disconnect();
	Create();
	m_bState = true;
	m_sockAddr.sin_family		= AF_INET;
	m_sockAddr.sin_port			= htons( port );
	m_sockAddr.sin_addr.s_addr	= inet_addr( IP );

	int	result = ::connect( m_socket, 
							reinterpret_cast<SOCKADDR*>(&m_sockAddr),
							sizeof(m_sockAddr));

	if(SOCKET_ERROR == result)
	{
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			return FALSE;
		}
	}

	if(FALSE == SetOptions())
	{
		return FALSE;
	}
	 
	m_connected = TRUE;
	OnConnect();
	CreateThread();
	return TRUE;
}

void ioCSocketTCP::Disconnect()
{
	if(m_socket != INVALID_SOCKET)
	{
		//::shutdown(m_socket, SD_BOTH); TIME_WAIT이 발생함, SO_LINGER 옵션을 무효화 시키는 듯함
		::closesocket(m_socket);
		m_socket	= INVALID_SOCKET;
		m_connected = FALSE;

		OnDisconnect();
	}
	if(0 != m_threadId)
	{
	//	_endthreadex(m_threadId); // 여기서 행걸림  
		m_bState = false;
		
		m_threadId = 0;
	}
	
}

BOOL ioCSocketTCP::SetOptions()
{
	if(m_socket != INVALID_SOCKET)
	{
		// socket reuse
		{
			BOOL reuse = TRUE;
			::setsockopt( 
				m_socket,
				SOL_SOCKET,
				SO_REUSEADDR,
				reinterpret_cast<char*>(&reuse),
				sizeof(reuse));
		}


		// TCP nodelay
		{
			BOOL nodelay = TRUE;
			::setsockopt( 
				m_socket, 
				IPPROTO_TCP, 
				TCP_NODELAY, 
				reinterpret_cast<char*>(&nodelay),
				sizeof(nodelay) );
		}

		// linger option
		{
			LINGER ln;

			ln.l_onoff	= 1;
			ln.l_linger = 0;

			::setsockopt( 
				m_socket, 
				SOL_SOCKET, 
				SO_LINGER,
				reinterpret_cast<char*>(&ln),
				sizeof(ln));
		}
	}
	return TRUE;
}

BOOL ioCSocketTCP::Send(const char *buffer, int length)
{
	int totalBytes = 0;

	while(totalBytes < length)
	{
		int sentBytes = ::send(	m_socket, 
						buffer + totalBytes, 
						length - totalBytes,
						0 );

		if(SOCKET_ERROR == sentBytes)
		{
			m_error = WSAGetLastError();
			Disconnect();
			return FALSE;
		}

		totalBytes += sentBytes;
	}

	OnSend();
	return TRUE;
}

BOOL ioCSocketTCP::Receive(char *buffer, int& length)
{
	int received = ::recv(	m_socket, 
					buffer, 
					length,
					0 );
	if(received > 0)
	{
		length = received;
		return TRUE;
	}

	if(0 == received)
	{
		length = 0;
		m_error = 0;
		return FALSE;
	}
	else
	{
		length = 0;
		m_error = WSAGetLastError();
		return FALSE;
	}
}

BOOL ioCSocketTCP::CreateThread()
{
	if(0 == m_threadId)
	{
		if(	_beginthreadex( NULL, 
							0,	
							TCPSocketThread, 
							this, 
							0, 
							&m_threadId ) == 0 )
		{
			return FALSE;
		}
	}
	return TRUE;
}

void ioCSocketTCP::DestroyThread()
{
	m_threadId = 0;
}

UINT WINAPI TCPSocketThread( LPVOID handle )
{
	ioCSocketTCP *sockTCP	= reinterpret_cast<ioCSocketTCP*>(handle);
	if(!sockTCP) return 0;

	char buffer[MAX_PACKET_SIZE];

	while( sockTCP->m_bState )
	{
		int length = sizeof(buffer);
		BOOL result = sockTCP->Receive(buffer, length);
		if(TRUE == result)
		{
			sockTCP->OnReceive(buffer, length);
		}
		else
		{
			sockTCP->Error();
			sockTCP->Disconnect();
			sockTCP->DestroyThread();
			break;
		}
	}
	sockTCP->Disconnect();
	
	return 0;
}
