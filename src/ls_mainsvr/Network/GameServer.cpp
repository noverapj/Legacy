#include "../stdafx.h"
//#include "../Window.h"
#include "../ioProcessChecker.h"

#include "GameServer.h"
#include "../NodeInfo/ServerNode.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../NodeInfo/MgrToolNode.h"
#include "../NodeInfo/MgrToolNodeManager.h"
#include "../NodeInfo/AcceptorServerNode.h"
#include "../NodeInfo/AcceptorMgrToolNode.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

//////////////////////////////////////////////////////////////////////////
ioServerBind::ioServerBind()
{
	SetAcceptor(new AcceptorServerNode, ITPK_ACCEPT_SESSION);
}


//////////////////////////////////////////////////////////////////////////
ioMgrToolBind::ioMgrToolBind()
{
	SetAcceptor(new AcceptorMgrToolNode, ITPK_ACCEPT_SESSION);
}

//////////////////////////////////////////////////////////////////////////
ServerSecurity::ServerSecurity()
{
	InitState( INVALID_SOCKET );
	m_iRcvCount		= 0;
	m_iMaxRcvCheck	= 0;
	m_dwRcvCurTimer = 0;
	m_iCurMagicNum  = 0;
}

ServerSecurity::~ServerSecurity()
{
}

void ServerSecurity::InitDoSAttack( int iMaxRcvCount )
{
	m_iMaxRcvCheck = iMaxRcvCount;
}

void ServerSecurity::InitState( SOCKET csocket )
{
	m_SndState.InitState();
	m_RcvState.InitState();
	m_Socket = csocket;

	m_iRcvCount		= 0;
	m_dwRcvCurTimer = 0;
	m_iCurMagicNum  = 0;
}

// DoS Attack
bool ServerSecurity::UpdateReceiveCount()
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
void ServerSecurity::EncryptMsg( CPacket &rkPacket )
{
	// Check Sum
	DWORD dwResult = MakeDigest(  (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize()  );
	rkPacket.SetCheckSum( dwResult );
	
	// Encryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Encrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );	
}

void ServerSecurity::DecryptMsg( CPacket &rkPacket )
{
	// Decryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Decrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );
}

bool ServerSecurity::IsCheckSum( CPacket &rkPacket )
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
int ServerSecurity::GetSndState()
{
	return m_SndState.GetState();
}

void ServerSecurity::UpdateSndState()
{
	m_SndState.UpdateState();
}

int ServerSecurity::GetRcvState()
{
	return m_RcvState.GetState();
}

void ServerSecurity::UpdateRcvState()
{
	m_RcvState.UpdateState();
}

bool ServerSecurity::CheckState( CPacket &rkPacket )
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

void ServerSecurity::PrepareMsg( CPacket &rkPacket )
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

void ServerSecurity::CompletionMsg( CPacket &rkPacket )
{
//	DecryptMsg( rkPacket );	
}

