#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include ".\ioserversecurity.h"

//////////////////////////////////////////////////////////////////////////
ioServerSecurity::ioServerSecurity()
{
	InitState( INVALID_SOCKET );
	m_iRcvCount		= 0;
	m_iMaxRcvCheck	= 0;
	m_dwRcvCurTimer = 0;
	m_iCurMagicNum  = 0;
}

ioServerSecurity::~ioServerSecurity()
{
}

void ioServerSecurity::InitDoSAttack( int iMaxRcvCount )
{
	m_iMaxRcvCheck = iMaxRcvCount;
}

void ioServerSecurity::InitState( SOCKET csocket )
{
	m_SndState.InitState();
	m_RcvState.InitState();
	m_Socket = csocket;

	m_iRcvCount		= 0;
	m_dwRcvCurTimer = 0;
	m_iCurMagicNum  = 0;
}

// DoS Attack
bool ioServerSecurity::UpdateReceiveCount()
{
	const DWORD dwCurTime = TIMEGETTIME();
	
	if( m_dwRcvCurTimer == 0 )	// First
	{
		m_dwRcvCurTimer = dwCurTime;
		m_iRcvCount     = 1;
	}
	else
	{
		m_iRcvCount++;
		
		if( dwCurTime - m_dwRcvCurTimer < 1000 )     //초당 패킷 받을 수 있는 패킷 제한.
		{
			if( m_iRcvCount >= m_iMaxRcvCheck )
				return false;	// Error
		}
		else	// Init Again
		{
			m_dwRcvCurTimer = dwCurTime;
			m_iRcvCount     = 1;
		}
	}

	return true;
}

// Packet CheckSum
void ioServerSecurity::EncryptMsg( CPacket &rkPacket )
{
	// Check Sum
	DWORD dwResult = MakeDigest(  (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize()  );
	rkPacket.SetCheckSum( dwResult );
	
	// Encryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Encrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );	
}

void ioServerSecurity::DecryptMsg( CPacket &rkPacket )
{
	// Decryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Decrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );
}

bool ioServerSecurity::IsCheckSum( CPacket &rkPacket )
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
int ioServerSecurity::GetSndState()
{
	return m_SndState.GetState();
}

void ioServerSecurity::UpdateSndState()
{
	m_SndState.UpdateState();
}

int ioServerSecurity::GetRcvState()
{
	return m_RcvState.GetState();
}

void ioServerSecurity::UpdateRcvState()
{
	m_RcvState.UpdateState();
}

bool ioServerSecurity::CheckState( CPacket &rkPacket )
{
	if( rkPacket.GetState() != MAGIC_TOKEN_FSM )
	{
		if( GetRcvState() != rkPacket.GetState() )
		{
			return false;
		}
		
		UpdateRcvState();
	}
	else
	{
		AddMagicNum();
		if( GetMagicNum() > MAX_BYPASS_MAGIC_TOKEN )
		{			
			return false;
		}
	}
	return true;
}

void ioServerSecurity::PrepareMsg( CPacket &rkPacket )
{
	if( GetSndState() == MAGIC_TOKEN_FSM )	// First
	{
		m_SndState.SetState( m_Socket );
		m_RcvState.SetState( m_Socket );
		UpdateRcvState();		
	}
	rkPacket.SetState( GetSndState() );

	EncryptMsg( rkPacket );

	UpdateSndState();
}

void ioServerSecurity::CompletionMsg( CPacket &rkPacket )
{
//	DecryptMsg( rkPacket );	
}

