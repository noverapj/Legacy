

#include "stdafx.h"

#include "ioClientUDPSocket.h"

ioClientUDPSocket::ioClientUDPSocket( int iSecurity )
: ioUDPSocket()
{
	if( iSecurity == 1 )
	{
		m_pNS = new ioUDPClientSecurity;
	}
}

ioClientUDPSocket::~ioClientUDPSocket()
{
	SAFEDELETE( m_pNS );
}

void ioClientUDPSocket::RcvPeerInfo( char *iip, int iPort )
{
	if( m_pNS )
	{
		((ioUDPClientSecurity*)m_pNS)->RcvPeerInfo( iip, iPort );
	}
}

bool ioClientUDPSocket::CheckNS( CPacket &rkPacket )
{
	if( m_pNS == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioClientUDPSocket::CheckNS ( m_pNS == NULL )");
		return true;
	}
	
	if( !m_pNS->IsCheckSum( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "ioClientUDPSocket::CheckNS Check Sum Fail!! [%d]", rkPacket.GetPacketID() );
		return false;
	}
	
	if( !m_pNS->CheckState( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "ioClientUDPSocket::CheckNS Type:%x State Not Same Client:%d, Server:%d", 
							     rkPacket.GetPacketID(),
								 rkPacket.GetState(),
								 m_pNS->GetRcvState() );
		return false;
	}
	
	return true;
}

void ioClientUDPSocket::ProcessPacket( CPacket &rkPacket, char *RcvIP, int iRcvPort )
{
	P2PNetwork::RecvPacket( (SP2Packet&)rkPacket, RcvIP, iRcvPort );
}

//////////////////////////////////////////////////////////////////////////
ioUDPClientSecurity::ioUDPClientSecurity()
{
	m_SndState.InitState();
	m_RcvState.InitState();
}

ioUDPClientSecurity::~ioUDPClientSecurity()
{
}

void ioUDPClientSecurity::RcvPeerInfo( char *iip, int iPort )
{
	sprintf( m_recent_rcv_ip, iip );
	m_recent_rcv_port = iPort;
}

// Packet CheckSum
void ioUDPClientSecurity::EncryptMsg( CPacket &rkPacket )
{
	// Check Sum
	DWORD dwResult = MakeDigest(  (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize()  );
	rkPacket.SetCheckSum( dwResult );
	
	// Encryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Encrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );	
}

void ioUDPClientSecurity::DecryptMsg( CPacket &rkPacket )
{
	// Decryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Decrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );
}

bool ioUDPClientSecurity::IsCheckSum( CPacket &rkPacket )
{
	//Decrypt
	DecryptMsg( rkPacket );

	DWORD	dwTemp = rkPacket.GetCheckSum();   //임시 저장.
	rkPacket.SetCheckSum( 0 );
	
	DWORD dwResult = MakeDigest( (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize() );

	rkPacket.SetCheckSum( dwTemp );

	if( dwResult != dwTemp )
	{
		LOG.PrintTimeAndLog( 0, "Check Sum Fail!: %d != %d", dwResult, dwTemp );
		return false;
	}

	return true;
}

// Packet Replay
int ioUDPClientSecurity::GetSndState()
{
	return m_SndState.GetState();
}

void ioUDPClientSecurity::UpdateSndState()
{
	m_SndState.UpdateState();
}

int ioUDPClientSecurity::GetRcvState()
{
	return m_RcvState.GetState();
}

void ioUDPClientSecurity::UpdateRcvState()
{
	m_RcvState.UpdateState();
}

bool ioUDPClientSecurity::CheckState( CPacket &rkPacket )
{
	if( rkPacket.GetState() != MAGIC_TOKEN_FSM )
	{
		if( GetRcvState() == rkPacket.GetState() )
		{
			if( !strcmp( m_recent_rcv_ip, m_pre_recent_rcv_ip ) && m_recent_rcv_port == m_pre_recent_rcv_port )
			{
				return false;
			}
		}
		
		strcpy(m_pre_recent_rcv_ip, m_recent_rcv_ip);
		m_pre_recent_rcv_port = m_recent_rcv_port;

		m_RcvState.SetState( rkPacket.GetState() );
	}
	return true;
}

void ioUDPClientSecurity::PrepareMsg( CPacket &rkPacket )
{
	if( GetSndState() == MAGIC_TOKEN_FSM )	// First
		m_SndState.SetState( rand() );
	
	rkPacket.SetCheckSum( 0 );
	rkPacket.SetState( GetSndState() );

	EncryptMsg( rkPacket );

	UpdateSndState();
}

void ioUDPClientSecurity::CompletionMsg( CPacket &rkPacket )
{
	DecryptMsg( rkPacket );	
}
