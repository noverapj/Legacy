

#include "../StdAfx.h"

#include "ioPortalHelp.h"

bool ioPortalHelp::InitWinSock()
{
	WSADATA WOSAdata;
    if( WSAStartup(0x0002, &WOSAdata) != 0 )
        return false;

    return true;
}

void ioPortalHelp::CloseWinSock()
{
	WSACleanup();
}

int ioPortalHelp::CheckSocketError( LPSTR szFileName, int iLineNum )
{
    int iErrorCode = WSAGetLastError();

	switch( iErrorCode )
    {
  	case WSAEACCES:
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEACCES");break;
	case WSAEADDRINUSE:
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEADDRINUSE");break;
	case WSAEADDRNOTAVAIL :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEADDRNOTAVAIL ");break;
	case WSAEAFNOSUPPORT :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEAFNOSUPPORT ");break;
	case WSAEALREADY :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEALREADY ");break;
	case WSAECONNABORTED :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAECONNABORTED ");break;
	case WSAECONNREFUSED :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAECONNREFUSED ");break;
	case WSAECONNRESET :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAECONNRESET ");break;
	case WSAEDESTADDRREQ :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEDESTADDRREQ ");break;
	case WSAEFAULT :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEFAULT ");break;
	case WSAEHOSTDOWN :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEHOSTDOWN ");break;
	case WSAEHOSTUNREACH :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEHOSTUNREACH ");break;
	case WSAEINPROGRESS :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEINPROGRESS ");break;
	case WSAEINTR :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEINTR ");break;
	case WSAEINVAL :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEINVAL ");break;
	case WSAEISCONN :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEISCONN ");break;
	case WSAEMFILE :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEMFILE ");break;
	case WSAEMSGSIZE :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEMSGSIZE ");break;
	case WSAENETDOWN :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAENETDOWN ");break;
	case WSAENETRESET :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAENETRESET ");break;
	case WSAENETUNREACH :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAENETUNREACH ");break;
	case WSAENOBUFS :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAENOBUFS ");break;
	case WSAENOPROTOOPT :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAENOPROTOOPT ");break;
	case WSAENOTCONN :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAENOTCONN ");break;
	case WSAENOTSOCK :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAENOTSOCK ");break;
	case WSAEOPNOTSUPP :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEOPNOTSUPP ");break;
	case WSAEPFNOSUPPORT :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEPFNOSUPPORT ");break;
	case WSAEPROCLIM :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEPROCLIM ");break;
	case WSAEPROTONOSUPPORT :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEPROTONOSUPPORT ");break;
	case WSAEPROTOTYPE :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEPROTOTYPE ");break;
	case WSAESHUTDOWN :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAESHUTDOWN ");break;
	case WSAESOCKTNOSUPPORT :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAESOCKTNOSUPPORT ");break;
	case WSAETIMEDOUT :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAETIMEDOUT ");break;
//	case WSATYPE_NOT_FOUND :
//		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSATYPE_NOT_FOUND ");break;
	case WSAEWOULDBLOCK :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEWOULDBLOCK ");break;
	case WSAHOST_NOT_FOUND :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAHOST_NOT_FOUND ");break;
//	case WSA_INVALID_HANDLE :
//		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSA_INVALID_HANDLE ");break;
//	case WSA_INVALID_PARAMETER :
//		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSA_INVALID_PARAMETER ");break;
//	case WSAINVALIDPROCTABLE :
//		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAINVALIDPROCTABLE ");break;
//	case WSAINVALIDPROVIDER :
//		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAINVALIDPROVIDER ");break;
//	case WSA_IO_INCOMPLETE :
//		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSA_IO_INCOMPLETE ");break;
//	case WSA_IO_PENDING :
//		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSA_IO_PENDING ");break;
//	case WSA_NOT_ENOUGH_MEMORY :
//		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSA_NOT_ENOUGH_MEMORY ");break;
	case WSANOTINITIALISED :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSANOTINITIALISED ");break;
	case WSANO_DATA :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSANO_DATA ");break;
	case WSANO_RECOVERY :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSANO_RECOVERY ");break;
//	case WSASYSCALLFAILURE :
//		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSASYSCALLFAILURE ");break;
	case WSASYSNOTREADY :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSASYSNOTREADY ");break;
	case WSATRY_AGAIN :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSATRY_AGAIN ");break;
	case WSAVERNOTSUPPORTED :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAVERNOTSUPPORTED ");break;
	case WSAEDISCON :
		LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError:WSAEDISCON ");break;
    default:
    	LOG.DebugLog(0, szFileName, iLineNum,"CheckSocketError	: Exception (%d)", iErrorCode );
    	break;
    }
	
    return iErrorCode;
}


