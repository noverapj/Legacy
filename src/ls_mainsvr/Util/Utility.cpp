#include "../StdAfx.h"
#include <iostream>

using namespace std;

// trace
void Trace( const char *format, ... ) 
{ 
#ifdef _DEBUG
	char buffer[2048]; 
	va_list marker; 

	va_start( marker, format ); 
	_vstprintf_s( buffer, _countof(buffer), format, marker ); 
	va_end( marker );

	OutputDebugString( buffer ); 
#endif
}

// console
void Debug( const char *format, ... )
{
#ifdef _DEBUG
	static char buffer[2048];

	va_list marker; 
	va_start(marker, format); 
	_vstprintf_s(buffer, _countof(buffer), format, marker); 
	va_end(marker);
	
	cout << buffer;
#endif
}

// console
void Information( const char *format, ... )
{
	static char buffer[2048]; 

	va_list marker; 
	va_start( marker, format ); 
	_vstprintf_s( buffer, _countof(buffer), format, marker ); 
	va_end( marker );

	cout << buffer;
}

// last error
void Debug()
{
	DWORD dwError = WSAGetLastError();
	LPTSTR lpMsgBuf = NULL;
	FormatMessage( 	FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, 
					dwError, 
					0, // Default language 
					(LPTSTR)&lpMsgBuf,	
					0,	
					NULL );

	cout << _T("WSAGetLastError[") << dwError << _T("] : ") << lpMsgBuf << endl << flush;
	
	// Free the buffer.
	LocalFree( lpMsgBuf );
}