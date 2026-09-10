

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioWallCollisionBuff.h"


ioWallCollisionBuff::ioWallCollisionBuff()
{
}

ioWallCollisionBuff::ioWallCollisionBuff( const ioWallCollisionBuff &rhs )
: ioBuff( rhs ),
  m_WallCollisionBuff( rhs.m_WallCollisionBuff )
{
}

ioWallCollisionBuff::~ioWallCollisionBuff()
{
}

void ioWallCollisionBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iBuffCnt = rkLoader.LoadInt_e( "wall_collision_buff_cnt", 0 );
	m_WallCollisionBuff.clear();
	m_WallCollisionBuff.reserve( iBuffCnt );
	for( int i=0; i<iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "wall_collision_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_WallCollisionBuff.push_back( szBuf );
	}
}

ioBuff* ioWallCollisionBuff::Clone()
{
	return new ioWallCollisionBuff( *this );
}

void ioWallCollisionBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioWallCollisionBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioWallCollisionBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_DELAY:
	case OT_SWITCH:
	case OT_ACTIVE:
	case OT_PASSIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	}
}

bool ioWallCollisionBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		return true;
	}

	return false;
}

void ioWallCollisionBuff::CheckWallCollision()
{
	if( !IsLive() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioHashString szCreatorName = GetCreatorName();
	if( !szCreatorName.IsEmpty() )
	{
		int iBuffCnt = m_WallCollisionBuff.size();
		for( int i=0; i<iBuffCnt; ++i )
		{
			pOwner->AddNewBuff( m_WallCollisionBuff[i], szCreatorName, "", NULL );
			//pOwner->ReserveAddNewBuff( m_WallCollisionBuff[i], szCreatorName, "", NULL );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -BuffAddBuff1 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}

			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pOwner->GetCharName();
			kPacket << pOwner->GetCharName();
			kPacket << "";
			kPacket << pOwner->GetRandomSeed();
			kPacket << false;		// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << 0.0f;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iBuffCnt;

			for( i=0; i < iBuffCnt; i++ )
			{
				kPacket << m_WallCollisionBuff[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		SetReserveEndBuff();
	}

	
}