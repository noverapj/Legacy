

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioPreventDropItemBuff.h"
#include "WeaponAttribute.h"



ioPreventDropItemBuff::ioPreventDropItemBuff()
{
}

ioPreventDropItemBuff::ioPreventDropItemBuff( const ioPreventDropItemBuff &rhs )
: ioBuff( rhs ),
 m_bDisableDownState( rhs.m_bDisableDownState ),
 m_bDisableWoundState( rhs.m_bDisableWoundState )
{
}

ioPreventDropItemBuff::~ioPreventDropItemBuff()
{
}

void ioPreventDropItemBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableWoundState = rkLoader.LoadBool_e( "disable_Wounded_state", false );
}

ioBuff* ioPreventDropItemBuff::Clone()
{
	return new ioPreventDropItemBuff( *this );
}

void ioPreventDropItemBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioPreventDropItemBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioPreventDropItemBuff::ProcessBuff( float fTimePerSec )
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

void ioPreventDropItemBuff::EndBuff()
{
	ioBuff::EndBuff();
}

bool ioPreventDropItemBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableWoundState )
	{
		if( m_pOwner->IsApplyDownState() || m_pOwner->GetState() == CS_WOUNDED )
		{
			SetReserveEndBuff();
			return true;
		}
	}

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