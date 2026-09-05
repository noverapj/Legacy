
#include "stdafx.h"
#include "ioCSocketUDP.h"


UINT WINAPI UDPSocketThread( LPVOID pHandle );


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ioCSocketUDP::ioCSocketUDP() : m_peerIP(0), m_peerPort(0), m_port(0)
{
	Init();
}

ioCSocketUDP::~ioCSocketUDP()
{
	Destroy();
}

void ioCSocketUDP::Init()
{
}

void ioCSocketUDP::Destroy()
{
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL ioCSocketUDP::Create()
{
	m_socket = ::socket( AF_INET, SOCK_DGRAM, 0 );
	if( m_socket == INVALID_SOCKET )
	{
		return FALSE;
	}
	return TRUE;
}

BOOL ioCSocketUDP::Bind()
{
	SOCKADDR_IN sockAddr;
	ZeroMemory( &sockAddr, sizeof(sockAddr) );

	while(TRUE)
	{
		sockAddr.sin_family		 = AF_INET;
		sockAddr.sin_port		 = htons( m_port );
		sockAddr.sin_addr.s_addr = htonl( INADDR_ANY );

		if( bind(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR )
		{
			break;
		}
	}
	return TRUE;
}

BOOL ioCSocketUDP::Connect(const char *IP, int port)
{
	Disconnect();
	Create();
	Bind();

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

	m_connected = TRUE;
	OnConnect();
	return TRUE;
}

void ioCSocketUDP::Disconnect()
{
	if(m_socket != INVALID_SOCKET)
	{
		::closesocket(m_socket);
		m_socket	= INVALID_SOCKET;
		m_connected = FALSE;

		OnDisconnect();
	}
}

BOOL ioCSocketUDP::Send(const char *buffer, int length)
{
	::sendto( m_socket, 
		buffer, 
		length,
		0,
		NULL,
		NULL );

	OnSend();
	return TRUE;
}

BOOL ioCSocketUDP::Receive(char *buffer, int& length)
{
	SOCKADDR_IN sockAddr;
	int lengthOfSockAddr;

	int received = ::recvfrom(	m_socket, 
								buffer, 
								length, 
								0, 
								(SOCKADDR*)&sockAddr, 
								&lengthOfSockAddr );

	if(received > 0)
	{
		length		= received;

		m_peerIP	= sockAddr.sin_addr.S_un.S_addr;
		m_peerPort	= sockAddr.sin_port;
		return TRUE;
	}

	if(0 == received)
	{
		length	= 0;
		m_error = 0;
		return FALSE;
	}
	else
	{
		length	= 0;
		m_error = WSAGetLastError();
		return FALSE;
	}
}

BOOL ioCSocketUDP::CreateThread()
{
	if(	_beginthreadex( NULL, 
						0,	
						UDPSocketThread, 
						this, 
						0, 
						&m_threadId ) == 0 )
	{
		return FALSE;
	}

	return TRUE;
}

UINT WINAPI UDPSocketThread( LPVOID handle )
{
	ioCSocketUDP *sockUDP	= reinterpret_cast<ioCSocketUDP*>(handle);
	if(!sockUDP) return 0;

	char buffer[MAX_PACKET_SIZE * 2];

	while( TRUE )
	{
		int length = sizeof(buffer);
		BOOL result = sockUDP->Receive(buffer, length);
		if(TRUE == result)
		{
			sockUDP->OnReceive(buffer, length);
		}
		else
		{
			sockUDP->Error();

			length = 0;
			sockUDP->OnReceive(NULL, length);
		}
	}

	return 0;
}

