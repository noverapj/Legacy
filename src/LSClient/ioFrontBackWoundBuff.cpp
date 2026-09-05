#include "stdafx.h"

#include "ioFrontBackWoundBuff.h"

#include "ioBaseChar.h"



ioFrontBackWoundBuff::ioFrontBackWoundBuff()
{
}

ioFrontBackWoundBuff::ioFrontBackWoundBuff( const ioFrontBackWoundBuff &rhs )
: ioBuff( rhs ),
m_FrontBackWoundInfo( rhs.m_FrontBackWoundInfo ),
m_bDisableDownState(rhs.m_bDisableDownState)
{
}

ioFrontBackWoundBuff::~ioFrontBackWoundBuff()
{
}

void ioFrontBackWoundBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	// Normal
	rkLoader.LoadString_e( "front_wound_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_FrontBuff = szBuf;
	rkLoader.LoadString_e( "front_attacker_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_FrontAttackerBuff = szBuf;

	m_FrontBackWoundInfo.m_fFrontDamageRate = rkLoader.LoadFloat_e( "front_wound_damage_rate", FLOAT1 );

	rkLoader.LoadString_e( "back_wound_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_BackBuff = szBuf;
	rkLoader.LoadString_e( "back_attacker_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_BackAttackerBuff = szBuf;

	m_FrontBackWoundInfo.m_fBackDamageRate = rkLoader.LoadFloat_e( "back_wound_damage_rate", FLOAT1 );


	// Down
	rkLoader.LoadString_e( "down_front_wound_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_DownFrontBuff = szBuf;
	rkLoader.LoadString_e( "down_front_attacker_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_DownFrontAttackerBuff = szBuf;

	m_FrontBackWoundInfo.m_fDownFrontDamageRate = rkLoader.LoadFloat_e( "down_front_wound_damage_rate", FLOAT1 );

	rkLoader.LoadString_e( "down_back_wound_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_DownBackBuff = szBuf;
	rkLoader.LoadString_e( "down_back_attacker_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_DownBackAttackerBuff = szBuf;

	m_FrontBackWoundInfo.m_fDownBackDamageRate = rkLoader.LoadFloat_e( "down_back_wound_damage_rate", FLOAT1 );


	// Air
	rkLoader.LoadString_e( "air_front_wound_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_AirFrontBuff = szBuf;
	rkLoader.LoadString_e( "air_front_attacker_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_AirFrontAttackerBuff = szBuf;

	m_FrontBackWoundInfo.m_fAirFrontDamageRate = rkLoader.LoadFloat_e( "air_front_wound_damage_rate", FLOAT1 );

	rkLoader.LoadString_e( "air_back_wound_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_AirBackBuff = szBuf;
	rkLoader.LoadString_e( "air_back_attacker_buff", "", szBuf, MAX_PATH );
	m_FrontBackWoundInfo.m_AirBackAttackerBuff = szBuf;

	m_FrontBackWoundInfo.m_fAirBackDamageRate = rkLoader.LoadFloat_e( "air_back_wound_damage_rate", FLOAT1 );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioFrontBackWoundBuff::Clone()
{
	return new ioFrontBackWoundBuff( *this );
}

void ioFrontBackWoundBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_FrontBackWoundInfo.m_BuffName = m_Name;
	pOwner->SetFrontBackWoundInfo( m_FrontBackWoundInfo );
}

bool ioFrontBackWoundBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_FrontBackWoundInfo.m_BuffName = m_Name;
	pOwner->SetFrontBackWoundInfo( m_FrontBackWoundInfo );
	return true;
}

void ioFrontBackWoundBuff::ProcessBuff( float fTimePerSec )
{
	CheckOwnerStateCheck();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioFrontBackWoundBuff::EndBuff()
{
	m_pOwner->RemoveFrontBackWoundInfo( m_Name );

	ioBuff::EndBuff();
}

void ioFrontBackWoundBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			SetReserveEndBuff();
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			SetReserveEndBuff();
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
		else if( eState == CS_DROP_ZONE_DOWN )
			SetReserveEndBuff();
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
	}
}


