// cServerSocket.cpp: implementation of the cServerSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cServerSocket.h"



DWORD _tinet_addr(const TCHAR *IP)
{
#ifdef UNICODE
	char temp[16];
	uint32 nRet = 0;
	nRet = WideCharToMultiByte(CP_ACP, 0, IP, _tcslen(IP), temp, 15, NULL, NULL);
	temp[nRet] = 0;
	return inet_addr(temp);
#else
	return inet_addr(IP);
#endif
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cServerSocket::cServerSocket() : m_backlog(0), m_acceptingCount(0), m_serverSocket(INVALID_SOCKET)
{
	Init();
}

cServerSocket::~cServerSocket()
{
	Destroy();
}

void cServerSocket::Init()
{
}

void cServerSocket::Destroy()
{
	CloseSocket();
	Cleanup();
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

// initiates use of WS2_32.DLL 
BOOL cServerSocket::Startup()
{
	WSADATA wsaData;
    if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) 	
	{
	   return FALSE;
    }

	return TRUE;
}

void cServerSocket::Cleanup()
{
	WSACleanup();
}

// creates a socket 
BOOL cServerSocket::CreateSocket()
{
	m_serverSocket = WSASocket(	AF_INET, 
									SOCK_STREAM, 
								    IPPROTO_TCP,
									NULL,
									NULL,
									WSA_FLAG_OVERLAPPED );


	if( m_serverSocket != INVALID_SOCKET )
	{
		LINGER  lingerStruct;
		lingerStruct.l_onoff = 1;
		lingerStruct.l_linger = 0;

		if( setsockopt( m_serverSocket, 
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

	return FALSE;
}


// close a socket 
void cServerSocket::CloseSocket()
{
	if( m_serverSocket != INVALID_SOCKET )
	{
		shutdown( m_serverSocket, SD_BOTH );
		closesocket( m_serverSocket );

		SetSocketHandle( INVALID_SOCKET );
	}
}

// associates a local address with a socket
BOOL cServerSocket::Bind( const uint16 port, const TCHAR* IP )
{
	SOCKADDR_IN sin;

	sin.sin_family	= AF_INET;
	sin.sin_port	= htons( port );

	if( IP )
		sin.sin_addr.s_addr = _tinet_addr( IP );	
	else
		sin.sin_addr.s_addr = INADDR_ANY;

	if( bind(	m_serverSocket, 
				(LPSOCKADDR)&sin, 
				sizeof(sin) ) == SOCKET_ERROR ) 
	{
		Information( _T("[%lu]error #1(%lu)\r\n"), GetCurrentThreadId(), GetLastError() );
		CloseSocket();
		return FALSE;
	}

	
	BOOL on = TRUE; // AcceptEx호출 이전에 OS가 connection을 받아들이지 못하도록 설정, netstat상에 대기중인 포트가 보이지 않게됨
	if( setsockopt( m_serverSocket, 
					SOL_SOCKET, 
					SO_CONDITIONAL_ACCEPT,
					reinterpret_cast<char*>(&on), 
					sizeof(on) ) == SOCKET_ERROR )
	{
		// 2013-06-20 youngdie, 일부 컴퓨터에서 실행이 안되는 경우가 발생함
		//Information( _T("[%lu]error #2(%lu)\r\n"), GetCurrentThreadId(), GetLastError() );
		//CloseSocket();
		//return FALSE;
	}
	

	return TRUE;
}

// listening for an incoming connection
BOOL cServerSocket::Listen( const int32 backlog )
{
    if( listen( m_serverSocket, backlog ) == SOCKET_ERROR ) 
	{
		CloseSocket();
		return FALSE;
    }

	m_backlog = backlog;
	return TRUE;
}

// accepts an incoming connection 
BOOL cServerSocket::AcceptRequest( SOCKET socket, PVOID outBuffer, LPOVERLAPPED overlapped )
{
	DWORD dwBytes = 0;
	BOOL bResult = AcceptEx(	m_serverSocket,
								socket,
								outBuffer,
								0, // 데이타가 들어올때까지 기다리지 않고 바로 접속을 받아들인다
								sizeof(SOCKADDR_IN) + 16,
								sizeof(SOCKADDR_IN) + 16,
								&dwBytes,
								overlapped );
	if( bResult == TRUE )
	{
		IncreaseAcceptingCount();
		return TRUE;
	}
	else
	{
		DWORD dwError = WSAGetLastError();
		if( ERROR_IO_PENDING == dwError )	
		{
			IncreaseAcceptingCount();
			return TRUE;
		}
		Debug();
	}
	return FALSE;
}
