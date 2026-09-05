#include "StdAfx.h"
#include <stdio.h>
#include "ioCSocket.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ioCSocket::ioCSocket(void) : m_socket(INVALID_SOCKET), m_connected(FALSE), m_error(0), m_threadId(0)
{
	Init();
}

ioCSocket::~ioCSocket(void)
{
	Destroy();
}

void ioCSocket::Init()
{
	ZeroMemory(&m_sockAddr, sizeof(m_sockAddr));
}

void ioCSocket::Destroy()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL ioCSocket::Startup()
{
	WSADATA wsaData;
	WORD version = MAKEWORD( 2, 2 );

    if( WSAStartup( version, &wsaData ) == SOCKET_ERROR ) 	
	{
	   return FALSE;
    }
	return TRUE;
}

void ioCSocket::Cleanup()
{
	WSACleanup();
}

void ioCSocket::Debug(const TCHAR *format, ...)
{
	TCHAR buffer[2048]; 
	va_list marker; 

	va_start( marker, format ); 
	_vsnprintf( buffer, sizeof(buffer), format, marker ); 
	va_end( marker );

#ifdef _DEBUG
	OutputDebugString( buffer ); 
#endif
}

void ioCSocket::Error()
{
	LPVOID buffer = NULL;

	FormatMessage( 	FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, 
					GetLastError(), 
					0, // Default language 
					reinterpret_cast<LPTSTR>(&buffer),
					0,	
					NULL );

	Debug( "ERROR MESSAGE : %s\n", buffer );
	LocalFree( buffer );
}
