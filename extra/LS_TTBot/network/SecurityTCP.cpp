#include "../StdAfx.h"
#include "cryption.h"
#include "SecurityTCP.h"

SecurityTCP::SecurityTCP(void)
{
	InitState();
}

SecurityTCP::~SecurityTCP(void)
{
}

void SecurityTCP::InitState()
{
	m_SndState.InitState();
	m_RcvState.InitState();
}

// Packet CheckSum
void SecurityTCP::EncryptMsg( CPacket &rkPacket )
{
	// Check Sum
	DWORD dwResult = MakeDigest(  (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize()  );
	rkPacket.SetCheckSum( dwResult );
	
	// Encryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Encrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );		
}

void SecurityTCP::DecryptMsg( CPacket &rkPacket )
{
	// Decryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Decrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );
}

bool SecurityTCP::IsCheckSum( CPacket &rkPacket )
{
	//Decrypt
	DecryptMsg( rkPacket );

	DWORD	dwTemp = rkPacket.GetCheckSum();   //임시 저장.
	rkPacket.SetCheckSum( 0 );
	
	DWORD dwResult = MakeDigest( (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize() );

	rkPacket.SetCheckSum( dwTemp );

	// Check Sum
	return ( dwResult == rkPacket.GetCheckSum() );
}

// Packet Replay
int SecurityTCP::GetSndState()
{
	return m_SndState.GetState();
}

void SecurityTCP::UpdateSndState()
{
	m_SndState.UpdateState();
}

int SecurityTCP::GetRcvState()
{
	return m_RcvState.GetState();
}

void SecurityTCP::UpdateRcvState()
{
	m_RcvState.UpdateState();
}

bool SecurityTCP::CheckState( CPacket &rkPacket )
{
	if( GetRcvState() == MAGIC_TOKEN_FSM )
	{
		m_RcvState.SetState( rkPacket.GetState() );
		m_SndState.SetState( rkPacket.GetState() );		
		UpdateSndState();
	}
	else
	{
		if( GetRcvState() != rkPacket.GetState() )
			return false;
	}
	
	UpdateRcvState();	
	return true;
}

void SecurityTCP::PrepareMsg( CPacket &rkPacket )
{
	rkPacket.SetState( GetSndState() );

	EncryptMsg( rkPacket );

	UpdateSndState();
}

void SecurityTCP::CompletionMsg( CPacket &rkPacket )
{	
}

