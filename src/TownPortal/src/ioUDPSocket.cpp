

#include "../stdafx.h"

#include "ioPortalHelp.h"
#include "ioUDPSocket.h"
#include "Packet.h"
#include "NetworkSecurity.h"

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)


ioUDPSocket::ioUDPSocket()
{
	m_Socket = INVALID_SOCKET;
	m_hWnd   = 0;
	m_WndMsg = 0;
	m_pNS    = NULL;

	ClearIPInfo();
}

ioUDPSocket::~ioUDPSocket()
{
	CloseUDP();
}

void ioUDPSocket::ClearIPInfo()
{
	memset( m_RecentRcvIP, 0, sizeof(m_RecentRcvIP) );
	m_iRecentRcvPort = 0;
	m_iMainPort = 0;

	memset( m_PreRecentRcvIP, 0, sizeof(m_PreRecentRcvIP) );
	m_iPreRecentRcvPort = 0;

	memset( m_szMyIP, 0, sizeof(m_szMyIP) );
}

bool ioUDPSocket::InitUDP( HWND hWnd, UINT uMsg, int iPort )
{
	ClearIPInfo();

	//m_Socket = socket( AF_INET, SOCK_DGRAM, 0 );
	m_Socket = WSASocket( AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( m_Socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] UDPSocket Init Fail.[%d]", WSAGetLastError() );
		ioPortalHelp::CheckSocketError( __FILE__, __LINE__ );
		return false;
	}

	DWORD dwBytesReturned = 0;
	BOOL  bNewBehavior = FALSE;
	DWORD status;
	status = WSAIoctl( m_Socket, SIO_UDP_CONNRESET, &bNewBehavior, sizeof( dwBytesReturned ), NULL, 0, &dwBytesReturned, NULL, NULL );
	if( status == SOCKET_ERROR )
	{
		LOG.PrintTimeAndLog( 0, "ioUDPSocket::InitUDP Opt None SIO_UDP_CONNRESET : %d", WSAGetLastError() );
	}

	m_hWnd = hWnd;
	m_WndMsg = uMsg;

	SOCKADDR_IN kLocalSockIn;
	memset( &kLocalSockIn, 0, sizeof(kLocalSockIn) );
	kLocalSockIn.sin_family		 = AF_INET;
	kLocalSockIn.sin_addr.s_addr = htonl( INADDR_ANY );
	kLocalSockIn.sin_port		 = htons( iPort );
	m_iMainPort = iPort;

	if( bind( m_Socket, (LPSOCKADDR)&kLocalSockIn, sizeof(kLocalSockIn) ) == SOCKET_ERROR )
	{
		DebugMBox(__FILE__,__LINE__,NULL, "ioUDPSocket::InitUDP - bind setting error" );
		return false;
	}

	int iMaxSize = MAX_BUFFER * 8;
	setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (const char *)&iMaxSize,  sizeof(iMaxSize));
	setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (const char *)&iMaxSize,  sizeof(iMaxSize));

	//    BOOL is_flag = TRUE;
	//     setsockopt(m_socket_object, SOL_SOCKET, SO_KEEPALIVE , (const char *)&is_flag,  sizeof(is_flag));

	//     BOOL	no_nagle = TRUE;
	//     setsockopt(m_socket_object, IPPROTO_TCP,  TCP_NODELAY, (const char *)&no_nagle,  sizeof(no_nagle));

	if( WSAAsyncSelect( m_Socket, hWnd, uMsg, FD_READ) > 0 )
	{
		DebugMBox(__FILE__,__LINE__,NULL,"ioUDPSocket::InitUDP - WSAAsyncSelect error");
		return false;
	}

	return true;
}

void ioUDPSocket::SetNS( NetworkSecurity *pNS )
{
	m_pNS = pNS;
}

void ioUDPSocket::CloseUDP()
{
	if( m_Socket != INVALID_SOCKET )
	{
		closesocket( m_Socket );
		m_Socket = INVALID_SOCKET;
	}
}

void ioUDPSocket::RcvPeerInfo( char *iip, int iPort )
{
	//외부 처리.
}

bool ioUDPSocket::EvaluateSocket( WPARAM wParam, LPARAM lParam )
{
	if( WSAGETSELECTEVENT( lParam ) == FD_READ )
		return RecvLoop( wParam );

	return true;
}

bool ioUDPSocket::SendLoop( const char *szIP, int iPort, CPacket &rkPacket )
{
	SOCKADDR_IN server_sin;
	server_sin.sin_family	   = AF_INET;
	server_sin.sin_addr.s_addr = inet_addr( szIP );
	server_sin.sin_port		   = htons( iPort );

	int server_sin_len = sizeof( server_sin );

	if( m_pNS )
		m_pNS->PrepareMsg( rkPacket );
	
	int iSendSize = sendto( m_Socket, rkPacket.GetBuffer(), rkPacket.GetBufferSize(), 0, (struct sockaddr FAR	*)&server_sin, server_sin_len );
	if( iSendSize == 0 )
	{
		LOG.PrintTimeAndLog( 0, "ioUDPSocket::SendLoop(%s:%d) : SizeZero( %d ) PacketType:%d", szIP, iPort, WSAGetLastError(), rkPacket.GetPacketID()  );
		ioPortalHelp::CheckSocketError(__FILE__, __LINE__);
	}
	else if( iSendSize == SOCKET_ERROR )
	{
		LOG.PrintTimeAndLog( 0, "ioUDPSocket::SendLoop(%s:%d) : SocketError( %d ) PacketType:%d", szIP, iPort, WSAGetLastError(), rkPacket.GetPacketID()  );
		ioPortalHelp::CheckSocketError(__FILE__, __LINE__);
	}

	if( m_pNS )
		m_pNS->CompletionMsg( rkPacket );

	return true;
}

bool ioUDPSocket::RecvLoop( SOCKET cSocket )
{
	int iRecvSize;
	
	SOCKADDR_IN kPeerSockIn;
	int iPeerSockInSize = sizeof(kPeerSockIn);

	iRecvSize = recvfrom( cSocket, 
		                  m_szRcvBuf,
						  MAX_BUFFER,
						  0,
						  (struct sockaddr FAR *)&kPeerSockIn,
						  &iPeerSockInSize );

	if( iRecvSize > 0 )
	{
		wsprintf( m_RecentRcvIP, "%d.%d.%d.%d", kPeerSockIn.sin_addr.s_net, 
												kPeerSockIn.sin_addr.s_host, 
												kPeerSockIn.sin_addr.s_lh,
												kPeerSockIn.sin_addr.s_impno );

		m_iRecentRcvPort = ntohs(kPeerSockIn.sin_port);
		
		CPacket kPacket( m_szRcvBuf, iRecvSize );
		if( kPacket.IsValidPacket() && kPacket.GetBufferSize() == iRecvSize )
		{
			RcvPeerInfo( m_RecentRcvIP, m_iRecentRcvPort );
			if( CheckNS( kPacket ) == false ) return false;
			
			ProcessPacket( kPacket, m_RecentRcvIP, m_iRecentRcvPort );
			return true;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioUDPSocket::RecvLoop - Packet Error" );
			LOG.PrintTimeAndLog( 0, "ioUDPSocket::RecvLoop - Valid(%d), BufSize:(%d), ID(%d), RecvSize(%d)",
									kPacket.IsValidPacket(), kPacket.GetBufferSize(),
									kPacket.GetPacketID(), iRecvSize );
			ioPortalHelp::CheckSocketError( __FILE__, __LINE__ );
			return false;
		}
	}
	else if( iRecvSize == 0 )
	{
		LOG.PrintTimeAndLog( 0, "ioUDPSocket::RecvLoop - UDP Socket Graceful Close");
		ioPortalHelp::CheckSocketError(__FILE__, __LINE__);
		return false;
	}
	else if( iRecvSize==SOCKET_ERROR )
	{
		int iWSAError = WSAGetLastError();
		if( iWSAError != WSAEWOULDBLOCK )
		{
			LOG.PrintTimeAndLog( 0, "ioUDPSocket::RecvLoop - UDP Socket SOCKET_ERROR(%d)", iWSAError );
			ioPortalHelp::CheckSocketError(__FILE__, __LINE__);
			return false;
		}
	}

	LOG.PrintTimeAndLog( 0, "ioUDPSocket::RecvLoop - Except Unknown Error" );
	ioPortalHelp::CheckSocketError( __FILE__, __LINE__ );

	return false;
}

bool ioUDPSocket::CheckNS( CPacket &rkPacket )
{
	if( !m_pNS ) return true;
	
	if( !m_pNS->UpdateReceiveCount() ) 
		return false;
	
	if( !m_pNS->IsCheckSum( rkPacket ) )
		return false;
	
	if( !m_pNS->CheckState( rkPacket ) )
		return false;
	
	return true;
}

void ioUDPSocket::SocketErrorMsg( const char *szError )
{
	if( szError )
	{
		LOG.PrintTimeAndLog( 0, "%s:%d - %s", m_RecentRcvIP, m_iRecentRcvPort, szError );
	}
}

void ioUDPSocket::GetRecentIPPort( char *szIP, int &iPort )
{
	strcpy( szIP, m_RecentRcvIP );
	iPort = m_iRecentRcvPort;
}

const char* ioUDPSocket::GetMyIP()
{
	char szHost[MAX_PATH];
	gethostname( szHost, MAX_PATH );

	HOSTENT *pHost = gethostbyname( szHost );
	if( !pHost )
	{   
		sprintf(m_szMyIP, "%d.%d.%d.%d", 127,0,0,1);
	}
	else
	{
		sprintf(m_szMyIP, "%d.%d.%d.%d", (BYTE)pHost->h_addr_list[0][0],
										 (BYTE)pHost->h_addr_list[0][1],
										 (BYTE)pHost->h_addr_list[0][2],
										 (BYTE)pHost->h_addr_list[0][3]);
	}

	return m_szMyIP;
}