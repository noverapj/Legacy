#include "iocpSocketDLL.h"


CProcessor* g_processor = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
					   DWORD  ul_reason_for_call, 
					   LPVOID lpReserved )
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
 
}


IOCP_SOCKET_API bool BeginSocket()
{
	WSADATA WOSAdata;
    if(WSAStartup(0x0002, &WOSAdata) != 0)
        return false;
    return true;
}

IOCP_SOCKET_API void EndSocket()
{
	WSACleanup();
}

void PrintTimeAndLog(int debuglv, LPSTR fmt, ... )
{
	char temp[ 2048 ] = { 0, };
			
	// parameter
	va_list args;
	va_start( args, fmt );
	vsprintf_s( temp, fmt, args );
	va_end( args );

	if( g_processor )
		g_processor->PrintTimeAndLog( debuglv, temp );
}

void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt, ... )
{
	char temp[ 2048 ] = { 0, };
			
	// parameter
	va_list args;
	va_start( args, fmt );
	vsprintf_s( temp, fmt, args );
	va_end( args );

	if( g_processor )
		g_processor->DebugLog( debuglv, filename, linenum, temp );
}
