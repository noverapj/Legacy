#include "stdafx.h"

#include "ioFrictionUpDownBuff.h"

#include "ioBaseChar.h"


ioFrictionUpDownBuff::ioFrictionUpDownBuff()
{
}

ioFrictionUpDownBuff::ioFrictionUpDownBuff( const ioFrictionUpDownBuff &rhs )
: ioBuff( rhs ),
 m_fFrictionRate( rhs.m_fFrictionRate ),
 m_bDisableDownState(rhs.m_bDisableDownState)
{
}

ioFrictionUpDownBuff::~ioFrictionUpDownBuff()
{
}

void ioFrictionUpDownBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fFrictionRate = rkLoader.LoadFloat_e( "friction_rate", FLOAT1 );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioFrictionUpDownBuff::Clone()
{
	return new ioFrictionUpDownBuff( *this );
}

void ioFrictionUpDownBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetFrictionBuffInfo( m_Name, m_fFrictionRate );
}

bool ioFrictionUpDownBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->SetFrictionBuffInfo( m_Name, m_fFrictionRate );
	return true;
}

void ioFrictionUpDownBuff::ProcessBuff( float fTimePerSec )
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

void ioFrictionUpDownBuff::EndBuff()
{
	m_pOwner->RemoveFrictionBuffInfo( m_Name );

	ioBuff::EndBuff();
}

void ioFrictionUpDownBuff::CheckOwnerStateCheck()
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


