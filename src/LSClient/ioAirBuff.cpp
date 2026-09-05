#include "stdafx.h"

#include "ioAirBuff.h"

#include "ioBaseChar.h"

#include "ioGrowthUpInfo.h"

ioAirBuff::ioAirBuff()
{
	m_bDisableDownState = false;
}

ioAirBuff::ioAirBuff( const ioAirBuff &rhs )
	: ioBuff( rhs ),
	m_bDisableDownState(rhs.m_bDisableDownState),
	m_bNoStopMotionCheck(rhs.m_bNoStopMotionCheck)
{
}

ioAirBuff::~ioAirBuff()
{
}

void ioAirBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bNoStopMotionCheck  = rkLoader.LoadBool_e( "no_stop_motion_check", false );
}

ioBuff* ioAirBuff::Clone()
{
	return new ioAirBuff( *this );
}

void ioAirBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioAirBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioAirBuff::ProcessBuff( float fTimePerSec )
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

void ioAirBuff::EndBuff()
{
	ioBuff::EndBuff();
}


void ioAirBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return;

	CharState eState = m_pOwner->GetState();

	if( !m_bNoStopMotionCheck && eState != CS_STOP_MOTION )
		SetReserveEndBuff();

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
