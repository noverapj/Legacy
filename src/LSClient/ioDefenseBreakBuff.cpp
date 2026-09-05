

#include "stdafx.h"

#include "ioDefenseBreakBuff.h"

#include "ioBaseChar.h"
#include "WeaponAttribute.h"



ioDefenseBreakBuff::ioDefenseBreakBuff()
{
}

ioDefenseBreakBuff::ioDefenseBreakBuff( const ioDefenseBreakBuff &rhs )
: ioBuff( rhs ),
 m_bDisableDownState( rhs.m_bDisableDownState )
{
}

ioDefenseBreakBuff::~ioDefenseBreakBuff()
{
}

void ioDefenseBreakBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioDefenseBreakBuff::Clone()
{
	return new ioDefenseBreakBuff( *this );
}

void ioDefenseBreakBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioDefenseBreakBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioDefenseBreakBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

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

void ioDefenseBreakBuff::EndBuff()
{
	ioBuff::EndBuff();
}

bool ioDefenseBreakBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_DROP_ZONE_DOWN )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
		{
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}