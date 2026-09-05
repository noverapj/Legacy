#include "../StdAfx.h"
#include "cryption.h"
#include "SecurityUDP.h"

SecurityUDP::SecurityUDP(void)
{
	m_SndState.InitState();
	m_RcvState.InitState();

	m_recent_rcv_port = 0;
	memset(m_recent_rcv_ip, 0, sizeof(m_recent_rcv_ip));	
	m_pre_recent_rcv_port = 0;
	memset( m_pre_recent_rcv_ip, 0, sizeof(m_pre_recent_rcv_ip) );
}

SecurityUDP::~SecurityUDP(void)
{
}

void SecurityUDP::RcvPeerInfo( char *iip, int iPort )
{
	sprintf( m_recent_rcv_ip, iip );
	m_recent_rcv_port = iPort;
}

// Packet CheckSum
void SecurityUDP::EncryptMsg( CPacket &rkPacket )
{
	// Check Sum
	DWORD dwResult = MakeDigest(  (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize()  );
	rkPacket.SetCheckSum( dwResult );
	
	// Encryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Encrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );	
}

void SecurityUDP::DecryptMsg( CPacket &rkPacket )
{
	// Decryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Decrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );
}

bool SecurityUDP::IsCheckSum( CPacket &rkPacket )
{
	//Decrypt
	DecryptMsg( rkPacket );

	DWORD	dwTemp = rkPacket.GetCheckSum();   //임시 저장.
	rkPacket.SetCheckSum( 0 );
	
	DWORD dwResult = MakeDigest( (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize() );

	rkPacket.SetCheckSum( dwTemp );
	
	// Check Sum
	if( dwResult != rkPacket.GetCheckSum() )
	{
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SecurityUDP::IsCheckSum Check Sum Fail!! [0x%x](%s:%d)", rkPacket.GetPacketID(), m_recent_rcv_ip, m_recent_rcv_port );
		return false;
	}
	return true;
}

// Packet Replay
int SecurityUDP::GetSndState()
{
	return m_SndState.GetState();
}

void SecurityUDP::UpdateSndState()
{
	m_SndState.UpdateState();
}

int SecurityUDP::GetRcvState()
{
	return m_RcvState.GetState();
}

void SecurityUDP::UpdateRcvState()
{
	m_RcvState.UpdateState();
}

bool SecurityUDP::CheckState( CPacket &rkPacket )
{
	if( rkPacket.GetState() != MAGIC_TOKEN_FSM )
	{
		if( GetRcvState() == rkPacket.GetState() )
		{
			if( !strcmp( m_recent_rcv_ip, m_pre_recent_rcv_ip ) && m_recent_rcv_port == m_pre_recent_rcv_port )
			{
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SecurityUDP::CheckState Type:[0x%x] State Not Same %d(%s:%d)", 
				//						rkPacket.GetPacketID(), rkPacket.GetState(), m_recent_rcv_ip, m_recent_rcv_port );
				return false;
			}
		}
		
		strcpy(m_pre_recent_rcv_ip, m_recent_rcv_ip);
		m_pre_recent_rcv_port = m_recent_rcv_port;

		m_RcvState.SetState( rkPacket.GetState() );
	}
	return true;
}

void SecurityUDP::PrepareMsg( CPacket &rkPacket )
{
	if( GetSndState() == MAGIC_TOKEN_FSM )	// First
		m_SndState.SetState( rand() );
	
	rkPacket.SetCheckSum( 0 ); 
	rkPacket.SetState( GetSndState() );

	EncryptMsg( rkPacket );

	UpdateSndState();
}

void SecurityUDP::CompletionMsg( CPacket &rkPacket )
{
//	DecryptMsg( rkPacket );	
}
