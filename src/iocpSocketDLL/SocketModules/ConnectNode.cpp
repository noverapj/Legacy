#include "../iocpSocketDLL.h"
#include <winbase.h>
#include "ConnectNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnectNode::CConnectNode( 
						   SOCKET s, 
						   DWORD dwSendBufSize, 
                           DWORD dwRecvBufSize ) : m_socket( s ), m_bSendBlock( false ), m_client_port( 0 ), m_pNS( NULL )
{
	if(GetSocketHandle() != INVALID_SOCKET)
	{
		bool opt = true;	
		::setsockopt(GetSocketHandle(),SOL_SOCKET,SO_KEEPALIVE,(const char*)&opt,sizeof(opt));
	}
	memset( m_public_ip, 0, sizeof( m_public_ip ) );
	memset( m_private_ip, 0, sizeof( m_private_ip ) );
	
	m_recvIO.Init( dwRecvBufSize );
	m_recvIO.InitRecvIO();
}

CConnectNode::~CConnectNode()
{
	if(GetSocketHandle() != INVALID_SOCKET)
	{
		closesocket( GetSocketHandle() );
		SetSocketHandle(INVALID_SOCKET);
	}
}

void CConnectNode::SetSocket(SOCKET csocket)
{
	SetSocketHandle( csocket );
	if(GetSocketHandle() != INVALID_SOCKET)
	{
		bool opt = true;	
		::setsockopt(GetSocketHandle(),SOL_SOCKET,SO_KEEPALIVE,(const char*)&opt,sizeof(opt));
	}
}

void CConnectNode::SetNagleAlgorithm( bool bOn )
{
	if( bOn )
	{
		// Nagle On
		BOOL no_nagle = FALSE;
		::setsockopt( GetSocketHandle(), IPPROTO_TCP, TCP_NODELAY, (const char*)&no_nagle, sizeof(no_nagle) ); 
	}
	else
	{
		// Nagle Off
		BOOL no_nagle = TRUE;
		::setsockopt( GetSocketHandle(), IPPROTO_TCP, TCP_NODELAY, (const char*)&no_nagle, sizeof(no_nagle) ); 
	}
}

void CConnectNode::SetNS( NetworkSecurity *pNS )
{
	m_pNS = pNS;	

	// 추가.
	CSendIO::SetNS( pNS );
}

void CConnectNode::OnCreate()
{
	if(GetSocketHandle() == INVALID_SOCKET)
		return;

	SetActive( true );
	SetSendBlock( false );
	CSendIO::Reset();
}

void CConnectNode::OnDestroy()
{
	CloseConnection();
	SetActive( false );
	SetSendBlock( true );

	m_recvIO.InitRecvIO();

	// Check
	CSendBuffer* pSendBuffer = GetSendBufferMemberPointer();
	if( pSendBuffer != NULL )
	{
		SetMemberPointer( NULL );
		ReturnSendBufferMemoryPool( pSendBuffer );
	}
}

bool CConnectNode::AfterCreate()
{
	return WaitForPacketReceive();
}

BOOL CConnectNode::EnableSend(const LONG count)
{
	if( GetSendCount() > count )
	{
		return FALSE;
	}

	return TRUE;
}

void CConnectNode::CloseConnection()
{
	if(INVALID_SOCKET != GetSocketHandle())
	{
		LINGER	solinger;
		solinger.l_linger = 0;
		solinger.l_onoff  = 1;
		setsockopt(GetSocketHandle(), SOL_SOCKET,SO_LINGER,(char*)&solinger,sizeof(solinger));
		//::shutdown(GetSocketHandle(),SD_BOTH);//kyg 셧다운 삭제 
		::closesocket(GetSocketHandle());
		SetSocketHandle( INVALID_SOCKET );
		memset(m_public_ip,0,sizeof(m_public_ip));
		memset(m_private_ip,0,sizeof(m_private_ip));
	}
}

void CConnectNode::SetClientAddr( sockaddr_in client_addr )
{
	//사용자 외부 아이피
	sprintf_s( m_public_ip,"%d.%d.%d.%d",
		client_addr.sin_addr.s_net,
		client_addr.sin_addr.s_host,
		client_addr.sin_addr.s_lh,
		client_addr.sin_addr.s_impno);
	m_client_port = ntohs(client_addr.sin_port);	
}

void CConnectNode::SetIPMapping( const char *privateIP )
{
	//내부 아이피.
	sprintf_s( m_private_ip,"%s",privateIP);
	
	//외부 아이피.
	sockaddr_in client_addr;
	int			client_addr_size = sizeof(sockaddr_in);
	getpeername(GetSocketHandle(),(struct sockaddr*)&client_addr,&client_addr_size);
	sprintf_s( m_public_ip,"%d.%d.%d.%d",
			client_addr.sin_addr.s_net,
			client_addr.sin_addr.s_host,
			client_addr.sin_addr.s_lh,
			client_addr.sin_addr.s_impno);
}

char *CConnectNode::GetPublicIP()
{
	return m_public_ip;
}

char *CConnectNode::GetPrivateIP()
{
	return m_private_ip;
}

int CConnectNode::GetUDP_port()
{
	return m_client_port;
}


bool CConnectNode::Dispatch( DWORD bytesTransferred, OVERLAPPED *ov, CPacket& packet )
{
	IOBufferedContext* lpIOBufferedContext = (IOBufferedContext*)ov;
	if( lpIOBufferedContext->m_flags == ASYNCFLAG_RECEIVE )
	{
		DispatchReceive( packet, bytesTransferred );
		return true;
	}
	else if( lpIOBufferedContext->m_flags == ASYNCFLAG_SEND )
	{
		CSendBuffer* SendBuffer = reinterpret_cast< CSendBuffer* >( lpIOBufferedContext );
		if( SendBuffer != NULL )
		{
			ReturnSendBufferMemoryPool( SendBuffer );
			DecreaseSendCount();
		}
		return true;
	}

	return false;
}

bool CConnectNode::SendMessage( CPacket &rkPacket, const BOOL immediatelySend )
{
	if( GetSocketHandle() == INVALID_SOCKET )
		return false;
	if( GetSendBlock() )
		return false;

	// 추가.
	return CSendIO::SendMessage( rkPacket, immediatelySend );
}

bool CConnectNode::SendMessage( const char* buffer, const int size, const BOOL immediatelySend )
{
	if( GetSocketHandle() == INVALID_SOCKET )
		return false;
	if( GetSendBlock() )
		return false;

	uint32 errorNum = 0;
	bool result = CSendIO::SendMessage( buffer, size, immediatelySend, errorNum );
	if( result == false )
	{
		PrintTimeAndLog( 0, "Error)) SendMessage Fail.. SendBuffer empty code : %u", errorNum );
	}

	return result;
}

void CConnectNode::DispatchReceive( CPacket& packet, DWORD bytesTransferred )
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		packet.SetBufferCopy( m_recvIO.GetBuffer(), m_recvIO.GetBytesTransferred() );
		if( (packet.IsValidPacket()) && (m_recvIO.GetBytesTransferred() >= (DWORD)packet.GetBufferSize()) )
		{
			int packetSize = packet.GetBufferSize();

			if( !CheckNS( packet ) ) return;

			ReceivePacket( packet );

			m_recvIO.AfterReceive( packetSize );
		}
		else 
			break;
	}

	WaitForPacketReceive();
}

bool CConnectNode::GetPeerIP(char* remoteIP, const int size, int& remotePort)
{
	SOCKADDR_IN sockAddr;
	int len = sizeof(SOCKADDR_IN);
	
	int result = getpeername( GetSocketHandle(), (SOCKADDR *)&sockAddr, &len );
	if( SOCKET_ERROR != result )
	{
		if (inet_ntop(AF_INET, &sockAddr.sin_addr, remoteIP, size) == NULL)
			return false;
		remotePort = ntohs(sockAddr.sin_port);
		return true;
	}
	return false;
}

bool CConnectNode::WaitForPacketReceive()
{
	if( GetSocketHandle() == INVALID_SOCKET ) return false;

	if(!m_recvIO.Receivable() )
	{
		// 2012-08-07 youngdie, 패킷아이디를 남겨 문제생긴 패킷을 확인한다
		CPacket packet;
		packet.SetBufferCopy( m_recvIO.GetBuffer(), m_recvIO.GetBytesTransferred() );

		PrintTimeAndLog( 0, "Session WaitForPacketReceive Buffer Overflow[%lu,%d]!!!", packet.GetPacketID(), packet.GetBufferSize() );
		ExceptionClose( 0 );
		return false;
	}

	DWORD dwFlags = 0;
	DWORD dwBytes = 0;	
	
	m_recvIO.ReadyToReceive();
	
	int nResult = ::WSARecv( 
		GetSocketHandle(), 
		m_recvIO.GetWsaBuf(), 
		1, 
		&dwBytes, 
		&dwFlags,
		m_recvIO.GetOveralapped(), NULL );

	if(nResult == SOCKET_ERROR) 
	{
		DWORD dwError = ::WSAGetLastError();
		if( dwError != WSA_IO_PENDING ) 
		{
			PrintTimeAndLog( 0, "Session WaitForPacketReceive FAILED:%d", dwError );
			ExceptionClose( dwError );
			return false;
		}
	}

	return true;
}

void CConnectNode::ExceptionClose( int lasterr )
{
	if(IsActive())
	{
		SessionClose();
	}
}

bool CConnectNode::CheckNS( CPacket &rkPacket )
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
