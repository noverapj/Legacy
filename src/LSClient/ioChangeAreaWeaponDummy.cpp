#include "StdAfx.h"
#include "ioChangeAreaWeaponDummy.h"


ioChangeAreaWeaponDummy::ioChangeAreaWeaponDummy( ioEntityGroup *pGrp, ioPlayMode *pMode ) : ioDummyChar( pGrp, pMode )
{
	m_DummyState	= DS_START;

	m_bIsReactionWeapon	= false;

	m_AreaWeaponFirstIdx	= -1;
	m_AreaWeaponSecondIdx	= -1;
}

ioChangeAreaWeaponDummy::~ioChangeAreaWeaponDummy(void)
{
}

void	ioChangeAreaWeaponDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char	szBuf[MAX_PATH];

	rkLoader.LoadString_e( "first_area_weapon", "", szBuf, MAX_PATH );
	m_szAreaWeaponFirst	= szBuf;
	rkLoader.LoadString_e( "second_area_weapon", "", szBuf, MAX_PATH );
	m_szAreaWeaponSecond	= szBuf;

	m_ReactionType	= (ReactionType)rkLoader.LoadInt_e( "reaction_type", RT_OWNER );
	m_dwReactionWeaponIdx	= rkLoader.LoadInt_e( "reaction_weapon", 0 );

	m_bNotUseTranslateByConveyer = rkLoader.LoadBool_e( "not_use_translate_by_conveyer", false );
}

void	ioChangeAreaWeaponDummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) 
		return;	

	ioBaseChar*	pOwner	= GetOwner();
	if( !pOwner )
		return;

	int state;
	rkPacket >> state;

	switch ( (DummyState)state )
	{
	case DS_DIE:
		{
			bool bIsReactionWeapon;
			rkPacket >> bIsReactionWeapon;

			if( bIsReactionWeapon )
				SetDummyDieState( pOwner );
		}
		break;
	}
}

void	ioChangeAreaWeaponDummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	pStage->RemoveSummonDummyInfo( m_iCreateIndex, false );

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

void	ioChangeAreaWeaponDummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
	{
		SetDieState( false );
		return;
	}

	switch( m_DummyState )
	{
	case	DS_START:
		{
			ProcessStartState( pOwner );
		}
		break;

	case	DS_DELAY:
		{
			ProcessDelayState( pOwner );
		}
		break;
	}

	ioPlayStage*	pStage	= pOwner->GetCreator();

	ioAreaWeapon*	pWeapon	= g_AreaWeaponMgr.FindAreaWeaponByIndex( m_AreaWeaponFirstIdx );
	if( pWeapon )
		pWeapon->SetAreaPosition( GetWorldPosition(), pStage );

	pWeapon	= g_AreaWeaponMgr.FindAreaWeaponByIndex( m_AreaWeaponSecondIdx );
	if( pWeapon )
		pWeapon->SetAreaPosition( GetWorldPosition(), pStage );

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() )
		//SendDieState( true );
		SetDieState( true );
}

void	ioChangeAreaWeaponDummy::ProcessStartState( ioBaseChar* pOwner )
{
	ioAreaWeapon*	pWeapon	= g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_szAreaWeaponFirst, GetWorldPosition(), GetWorldOrientation(), ioAreaWeapon::CT_NORMAL );
	m_AreaWeaponFirstIdx	= pWeapon->GetAreaWeaponIndex();

	m_DummyState	= DS_DELAY;
}

void	ioChangeAreaWeaponDummy::ProcessDelayState( ioBaseChar* pOwner )
{
	if( m_bIsReactionWeapon && pOwner->IsNeedProcess() )
	{
		ioAreaWeapon*	pWeapon	= g_AreaWeaponMgr.FindAreaWeaponByIndex( m_AreaWeaponFirstIdx );
		if( pWeapon )
		{
			pWeapon->DestroyAreaWeapon( pOwner->GetCreator() );
		}

		pWeapon	= g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_szAreaWeaponSecond, GetWorldPosition(), GetWorldOrientation(), ioAreaWeapon::CT_NORMAL );
		m_AreaWeaponSecondIdx	= pWeapon->GetAreaWeaponIndex();

		SetDieState( true );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DS_DIE;
			kPacket << m_bIsReactionWeapon;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool	ioChangeAreaWeaponDummy::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon ) 
		return	false;

	if( m_State == DCS_DIE )
		return	false;

	return	true;
}

ApplyWeaponExp	ioChangeAreaWeaponDummy::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar*	pOwner	= GetOwner();
	ioBaseChar* pWeaponOwner	= pWeapon->GetOwner();

	if( pOwner && pWeaponOwner )
	{
		if( pWeapon->GetAttributeIndex() == m_dwReactionWeaponIdx )
		{
			switch( m_ReactionType )
			{
			case	RT_OWNER:
				{
					if( pWeaponOwner == pOwner )
						m_bIsReactionWeapon	= true;
				}
				break;

			case	RT_TEAM:
				{
					if( pOwner->GetTeam() == pWeaponOwner->GetTeam() )
						m_bIsReactionWeapon	= true;
				}
				break;

			case	RT_ENEMY:
				{
					if( pOwner->GetTeam() != pWeaponOwner->GetTeam() )
						m_bIsReactionWeapon	= true;
				}
				break;

			case	RT_ALL:
				{
					m_bIsReactionWeapon	= true;
				}
				break;
			}
		}
	}

	return	ioDummyChar::ApplyWeapon( pWeapon );
}

void	ioChangeAreaWeaponDummy::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
	if ( m_bNotUseTranslateByConveyer )
		return;
	else
		ioDummyChar::TranslateByConveyer( vMove );
}

void	ioChangeAreaWeaponDummy::SetDummyDieState( ioBaseChar* pOwner )
{
	ioAreaWeapon*	pWeapon	= g_AreaWeaponMgr.FindAreaWeaponByIndex( m_AreaWeaponFirstIdx );
	if( pWeapon )
	{
		pWeapon->DestroyAreaWeapon( pOwner->GetCreator() );
	}

	pWeapon	= g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_szAreaWeaponSecond, GetWorldPosition(), GetWorldOrientation(), ioAreaWeapon::CT_NORMAL );
	m_AreaWeaponSecondIdx	= pWeapon->GetAreaWeaponIndex();

	SetDieState( true );
}