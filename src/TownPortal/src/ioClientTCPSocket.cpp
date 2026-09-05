

#include "../StdAfx.h"

#include "Packet.h"
#include "ioPortalHelp.h"
#include "ioClientTCPSocket.h"
#include "NetworkSecurity.h"
#include "SendBuffer.h"
#include "RecvBuffer.h"

ioClientTCPSocket::ioClientTCPSocket()
{
	m_Socket = INVALID_SOCKET;
	m_hWnd	 = 0;
	m_WndMsg = 0;

	m_pSndBuf = new SendBuffer;
	m_pRcvBuf = new RecvBuffer;

	memset( m_szConnectedIP, 0, MAX_PATH );
	m_iConnectedPort = 0;

	m_bConnected = false;
	m_pNS = NULL;
	m_bNoNagle   = FALSE;
}

ioClientTCPSocket::~ioClientTCPSocket()
{
	CloseClientTCP();

	SAFEDELETE(m_pSndBuf);
	SAFEDELETE(m_pRcvBuf);
}

bool ioClientTCPSocket::InitClientTCP( HWND hWnd, UINT uMsg, const char *szIP, int iPort )
{
	m_pSndBuf->InitBuffer();
	m_pRcvBuf->InitBuffer();

	//m_Socket = socket( AF_INET, SOCK_STREAM, 0 );
	m_Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if( m_Socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Client TCPSocket Init Fail.[%d]", WSAGetLastError() );
		ioPortalHelp::CheckSocketError( __FILE__, __LINE__ );
		return false;
	}

	m_hWnd = hWnd;
	m_WndMsg = uMsg;
	m_bNoNagle   = FALSE;

	int iMaxSize = MAX_BUFFER * 8;
    setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (const char *)&iMaxSize,  sizeof(int) );
    setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (const char *)&iMaxSize,  sizeof(int) );

	BOOL bFlag = TRUE;
	setsockopt( m_Socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&bFlag, sizeof(BOOL) );

	//IceMawang : TCP Retransmission Setting 추가. ( TCP_MAXRT )
	SetRetransmission( 10 );

	SOCKADDR_IN kLocalSockIn;
    memset( &kLocalSockIn, 0, sizeof(kLocalSockIn) );
    kLocalSockIn.sin_family		 = AF_INET;
    kLocalSockIn.sin_addr.s_addr = htonl(INADDR_ANY);
    kLocalSockIn.sin_port		 = htons(iPort);
    kLocalSockIn.sin_addr.s_addr = inet_addr(szIP);

	DWORD dwError = 0;
	int ret = WSAAsyncSelect( m_Socket, m_hWnd, m_WndMsg, FD_CONNECT);
    if( ret == SOCKET_ERROR )
	{
		DebugMBox( __FILE__, __LINE__, NULL, "ioClientTCPSocket::InitClientTCP-WSAAsyncSelect setting error" );
    	return false;
    }
	else
	{
		int iTimeOut1 = 30000;
		dwError = setsockopt( m_Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&iTimeOut1, sizeof(int) );
		if( dwError != 0 )
		{
			LOG.PrintTimeAndLog( 0, "[ERROR] setsocketopt SNDTIMEO limit: %d", dwError );
		}

    	if( connect( m_Socket, (LPSOCKADDR)&kLocalSockIn, sizeof(kLocalSockIn) ) == SOCKET_ERROR ) 
		{
    	 	if( WSAGetLastError() != WSAEWOULDBLOCK )
			{
				LOG.PrintTimeAndLog( 0, "[ERROR] InitClientTCP-connect error(%d), GetLastError(%d), IP(%s), Port(%d)", dwError, WSAGetLastError(), szIP, iPort );
    	 		return false;
    		}
    	}

		/*
		// timeout after iTimeOut1 * 6
		DWORD dwTick = GetTickCount() + (iTimeOut1 * 6);
		while( true )
		{
	    	if( connect( m_Socket, (LPSOCKADDR)&kLocalSockIn, sizeof(kLocalSockIn) ) == SOCKET_ERROR ) 
			{
				dwError = WSAGetLastError();
				
				if( dwError == WSAEISCONN) // 접속됨
				{
					break;
				}

    	 		if( dwError != WSAEWOULDBLOCK && dwError != WSAEINPROGRESS )
				{
    				LOG.PrintTimeAndLog( 0, "[ERROR] InitClientTCP-connect error(%d)", dwError );
    	 			return false;
    			}
				else
				{
					fd_set Write, Err;
					TIMEVAL Timeout;

					FD_ZERO(&Write);
					FD_ZERO(&Err);
					FD_SET(m_Socket, &Write);
					FD_SET(m_Socket, &Err);
					Timeout.tv_sec = 10;			//TimeoutSec;
					Timeout.tv_usec = 0;			//0;

					int iResult = select( 0,		//ignored
											NULL,		//read
											&Write,	//Write Check
											&Err,		//Error Check
											&Timeout);

					if( iResult == 0 )
					{
						if( GetTickCount() >= dwTick )
						{
							LOG.PrintTimeAndLog( 0, "[ERROR] ClientTCP Connection timeout!" );
							return false;
						}
					}
					else 
					{
						if(FD_ISSET(m_Socket, &Write))
						{
							break;
						}

						if(FD_ISSET(m_Socket, &Err))
						{
							LOG.PrintTimeAndLog( 0, "[ERROR] ClientTCP Connection error! (%d)", WSAGetLastError() );
							return false;
						}
					}
				}
			}

			::SleepEx(100, FALSE);
		}
		*/
    }

	// 추가
	int iTimeOut2 = 0;
	dwError = setsockopt( m_Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&iTimeOut2, sizeof(int) );

	strcpy( m_szConnectedIP, szIP );
	m_iConnectedPort = iPort;

	return true;
}

void ioClientTCPSocket::SetNS( NetworkSecurity *pNS )
{
	m_pNS = pNS;
}

bool ioClientTCPSocket::ReConnect()
{
	if( m_hWnd == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioClientTCPSocket::ReConnect -"
								    "Window Handle NULL" );
		return false;
	}

	if( strcmp( m_szConnectedIP, "" ) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "ioClientTCPSocket::ReConnect -"
								    " Connect IP Zero" );
		return false;
	}

	if( m_iConnectedPort == 0 )
	{
		LOG.PrintTimeAndLog( 0, "ioClientTCPSocket::ReConnect -"
								    " Connect PORT Zero" );
		return false;
	}

	if( m_WndMsg == 0 )
	{
		LOG.PrintTimeAndLog( 0, "ioClientTCPSocket::ReConnect -"
								    " Window Message Zero" );
		return false;
	}

	return InitClientTCP( m_hWnd, m_WndMsg, m_szConnectedIP, m_iConnectedPort );
}

void ioClientTCPSocket::CloseClientTCP()
{
	if( m_Socket != INVALID_SOCKET )
		closesocket( m_Socket );

	m_Socket = INVALID_SOCKET;
	m_bConnected = false;
	m_bNoNagle   = FALSE;
}

bool ioClientTCPSocket::EvaluateSocket( WPARAM wParam, LPARAM lParam )
{
	switch( WSAGETSELECTEVENT(lParam) )
	{
	case FD_CONNECT:
		if( WSAAsyncSelect( m_Socket, m_hWnd, m_WndMsg, FD_READ|FD_WRITE|FD_CLOSE) == SOCKET_ERROR )
    	{
    		LOG.PrintTimeAndLog( 0, "ioClientTCPSocket::EvaluateSocket -"
								    "AsyncSelect can not request FD_READ|FD_WRITE|FD_CLOSE" );
    		return false;
    	}
		break;
	case FD_CLOSE:
		CloseClientTCP();
		DisconnectProcess();
		break;
	case FD_WRITE:
		if( !m_bConnected )
		{
			m_bConnected = true;
			ConnectProcess();
		}
		else
		{
			m_pSndBuf->SetBlock( false );
			BufferSendLoop();
		}
		break;
	case FD_READ:
		RecvLoop( (SOCKET)wParam );
		break;
	default:
		break;
	}

	return true;
}

bool ioClientTCPSocket::SendLoop( CPacket &rkPacket )
{
	if( m_Socket == INVALID_SOCKET || !m_bConnected )
		return false;

	if( m_pNS )
		m_pNS->PrepareMsg( rkPacket );

	if( m_pSndBuf->EnqueueData( rkPacket.GetBuffer(), rkPacket.GetBufferSize() ) )
	{
		bool bResult = BufferSendLoop();
		
		if( m_pNS )
			m_pNS->CompletionMsg( rkPacket );

		return bResult;
	}

	LOG.DebugLog( 0, __FILE__, __LINE__, "ioClientTCPSocket::SendLoop - Buffer Over Flow!!!" );
	return false;
}

bool ioClientTCPSocket::RecvLoop( SOCKET cSocket )
{
	int iRecvSize = recv( cSocket, m_pRcvBuf->GetFreeSpacePtr(), m_pRcvBuf->GetFreeSpaceSize(), 0 );

	if( iRecvSize > 0 )
	{
		m_pRcvBuf->UpdateRecvSize( iRecvSize );

		while( m_pRcvBuf->GetCurRcvSize() > 0 )
		{
			int iCurBufferRcvSize = m_pRcvBuf->GetCurRcvSize();

			CPacket kPacket( m_pRcvBuf->GetStartPtr(), iCurBufferRcvSize );
			if( kPacket.IsValidPacket() && 
				iCurBufferRcvSize >= kPacket.GetBufferSize() )
			{
				if( CheckNS( kPacket ) )
					ProcessPacket( kPacket );
				m_pRcvBuf->RewindBuffer( kPacket.GetBufferSize() );
			}
			else
			{
				break;	// 아직 패킷을 다 받지 못했다.
			}
		}

		return true;
	}
	else if( iRecvSize == 0 )
	{
		LOG.PrintTimeAndLog( 0, "ioClientTCPSocket::RecvLoop - Client TCP Socket Graceful Close" );
    	return false;
	}
	else if( iRecvSize == SOCKET_ERROR )
	{
		int iLastError = WSAGetLastError();
		if ( iLastError == WSAEWOULDBLOCK )
		{
			return true;
		}
		else
    	{
			LOG.PrintTimeAndLog( 0, "ioClientTCPSocket::RecvLoop - Client TCP Socket recv Error : %d", iLastError );
    		return false;
    	}
	}

	LOG.PrintTimeAndLog( 0, "ioClientTCPSocket::RecvLoop - EXCEPT : Exception TCP recv state" );
	return false;
}

bool ioClientTCPSocket::CheckNS( CPacket &rkPacket )
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

bool ioClientTCPSocket::BufferSendLoop()
{
	if( m_pSndBuf->IsBlocked() )
		return true;

	int nleft = m_pSndBuf->GetRestCnt();
	int nRet = 0;

	while(nleft>0)
	{
		nRet = send( m_Socket, m_pSndBuf->GetQueuePtr(), nleft, 0 );

		if( nRet == SOCKET_ERROR )
		{
			if( WSAGetLastError() == WSAEWOULDBLOCK)
			{
				m_pSndBuf->SetBlock( true );
				return true;
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioClientTCPSocket::BufferSendLoop - Send Error(%d)", WSAGetLastError() );
				return false;
			}
		}
		else
		{
			nleft = m_pSndBuf->DequeueData(nRet);
		}
	};

	return true;
}

const char* ioClientTCPSocket::GetConnectedIP() const
{
	return m_szConnectedIP;
}

int ioClientTCPSocket::GetConnectedPort() const
{
	return m_iConnectedPort;
}

bool ioClientTCPSocket::IsConnected() const
{
	return m_bConnected;
}

SOCKET ioClientTCPSocket::GetSocket() const
{
	return m_Socket;
}

void ioClientTCPSocket::SetNagleAlgorithm( bool bOn )
{
	if( m_Socket == INVALID_SOCKET )
		return;

	if( bOn )
	{
		// Nagle On
		if( m_bNoNagle == FALSE )
			return;

		m_bNoNagle = FALSE;
		::setsockopt( m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&m_bNoNagle, sizeof(m_bNoNagle) ); 
	}
	else
	{
		// Nagle Off
		if( m_bNoNagle == TRUE )
			return;

		m_bNoNagle = TRUE;
		::setsockopt( m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&m_bNoNagle, sizeof(m_bNoNagle) ); 
	}
}

void ioClientTCPSocket::SetRetransmission( char cMaxRT ) // cMaxRT default 10sec. do not user -1 : connection will retransmit forever. Max 60sec
{
	if( m_Socket == INVALID_SOCKET )
		return;

	if( cMaxRT < 0 )
		return;

	if( cMaxRT > 60 )
		cMaxRT = 60;

	char MaxRT = cMaxRT;

	::setsockopt( m_Socket, IPPROTO_TCP, TCP_MAXRT, &MaxRT, sizeof(MaxRT) );
}
