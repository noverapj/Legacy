#include "../iocpSocketDLL.h"
#include "ioUDPSecurity.h"


ioUDPSecurity::ioUDPSecurity()
{
	m_SndState.InitState();
	m_RcvState.InitState();

	m_recent_rcv_port = 0;
	m_recent_rcv_ip = 0;
	m_pre_recent_rcv_port = 0;
	m_pre_recent_rcv_ip = 0;
}

ioUDPSecurity::~ioUDPSecurity()
{
}

void ioUDPSecurity::RcvPeerInfo( DWORD dwIp, int iPort )
{
	m_recent_rcv_ip = dwIp;
	m_recent_rcv_port = iPort;
}

void ioUDPSecurity::RcvPeerInfo( sockaddr_in *peer_addr )
{
	RcvPeerInfo( peer_addr->sin_addr.S_un.S_addr, ntohs(peer_addr->sin_port) );		
}

// Packet CheckSum
void ioUDPSecurity::EncryptMsg( CPacket &rkPacket )
{
	// Check Sum
	DWORD dwResult = MakeDigest(  (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize()  );
	rkPacket.SetCheckSum( dwResult );

	// Encryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Encrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );	
}

void ioUDPSecurity::DecryptMsg( CPacket &rkPacket )
{
	// Decryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Decrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );
}

bool ioUDPSecurity::IsCheckSum( CPacket &rkPacket )
{
	//Decrypt
	DecryptMsg( rkPacket );
	DWORD	dwTemp = rkPacket.GetCheckSum();   //임시 저장.
	rkPacket.SetCheckSum( 0 );
	DWORD dwResult = MakeDigest( (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	rkPacket.SetCheckSum( dwTemp );
	if( dwResult != rkPacket.GetCheckSum() )
	{
		PrintTimeAndLog( 0, "ioUDPSecurity::IsCheckSum Check Sum Fail!! [0x%x](%d:%d)", rkPacket.GetPacketID(), m_recent_rcv_ip, m_recent_rcv_port );
		return false;
	}
	return true;
}

int ioUDPSecurity::GetSndState()
{
	return m_SndState.GetState();
}

void ioUDPSecurity::UpdateSndState()
{
	m_SndState.UpdateState();
}

int ioUDPSecurity::GetRcvState()
{
	return m_RcvState.GetState();
}

void ioUDPSecurity::UpdateRcvState()
{
	m_RcvState.UpdateState();
}

bool ioUDPSecurity::CheckState( CPacket &rkPacket )
{
	if( rkPacket.GetState() != MAGIC_TOKEN_FSM )
	{
		if( GetRcvState() == rkPacket.GetState() )
		{
			if( m_recent_rcv_ip == m_pre_recent_rcv_ip && m_recent_rcv_port == m_pre_recent_rcv_port )
			{
				PrintTimeAndLog( 0, "ioUDPSecurity::CheckState Type:[0x%x] State Not Same %d(%d:%d)", 
					rkPacket.GetPacketID(), rkPacket.GetState(), m_recent_rcv_ip, m_recent_rcv_port );
				return false;
			}
		}

		m_pre_recent_rcv_ip = m_recent_rcv_ip;
		m_pre_recent_rcv_port = m_recent_rcv_port;
		m_RcvState.SetState( rkPacket.GetState() );
	}
	return true;
}

void ioUDPSecurity::PrepareMsg( CPacket &rkPacket )
{
	if( GetSndState() == MAGIC_TOKEN_FSM )	// First
	{
		m_SndState.SetState( rand() );  
	}	

	rkPacket.SetCheckSum( 0 ); 
	rkPacket.SetState( UpdateAndGetSndState() );
	EncryptMsg( rkPacket );
}

void ioUDPSecurity::CompletionMsg( CPacket &rkPacket )
{
}

long ioUDPSecurity::UpdateAndGetSndState()
{
	return m_SndState.UpdateState();
}


