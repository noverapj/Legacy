#include "stdafx.h"

#include "ioGravityUpDownBuff.h"

#include "ioBaseChar.h"

ioGravityUpDownBuff::ioGravityUpDownBuff()
{
}

ioGravityUpDownBuff::ioGravityUpDownBuff( const ioGravityUpDownBuff &rhs )
: ioBuff( rhs ),
 m_fGravityRate( rhs.m_fGravityRate ),
 m_bDisableDownState(rhs.m_bDisableDownState)
{
}

ioGravityUpDownBuff::~ioGravityUpDownBuff()
{
}

void ioGravityUpDownBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fGravityRate = rkLoader.LoadFloat_e( "gravity_rate", FLOAT1 );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioGravityUpDownBuff::Clone()
{
	return new ioGravityUpDownBuff( *this );
}

void ioGravityUpDownBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetGravityBuffInfo( m_Name, m_fGravityRate );
}

bool ioGravityUpDownBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->SetGravityBuffInfo( m_Name, m_fGravityRate );
	return true;
}

void ioGravityUpDownBuff::ProcessBuff( float fTimePerSec )
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

void ioGravityUpDownBuff::EndBuff()
{
	m_pOwner->RemoveGravityBuffInfo( m_Name );

	ioBuff::EndBuff();
}

void ioGravityUpDownBuff::CheckOwnerStateCheck()
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


