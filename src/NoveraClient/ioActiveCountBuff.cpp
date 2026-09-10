
#include "stdafx.h"

#include "ioActiveCountBuff.h"

#include "ioBaseChar.h"
#include "WeaponAttribute.h"


ioActiveCountBuff::ioActiveCountBuff()
{
}

ioActiveCountBuff::ioActiveCountBuff( const ioActiveCountBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bDisableDropDownState( rhs.m_bDisableDropDownState )
{
	m_iCurActiveCount = 0;
	m_iMaxActiveCount = 0;
}

ioActiveCountBuff::~ioActiveCountBuff()
{
}

void ioActiveCountBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropDownState = rkLoader.LoadBool_e( "disable_drop_down_state", false );
}

ioBuff* ioActiveCountBuff::Clone()
{
	return new ioActiveCountBuff( *this );
}

void ioActiveCountBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioActiveCountBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioActiveCountBuff::SetActiveCount(int iCount )
{
	m_iCurActiveCount = m_iMaxActiveCount = iCount;
}


void ioActiveCountBuff::ProcessBuff( float fTimePerSec )
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
		if( m_iMaxActiveCount > 0 && m_iCurActiveCount <= 0 )
			SetReserveEndBuff();
		ioBuff::ProcessBuff( fTimePerSec );	
		break;	
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioActiveCountBuff::CheckActiveCount()
{
	m_iCurActiveCount--;
	m_iCurActiveCount = max( m_iCurActiveCount, 0 );
}

void ioActiveCountBuff::EndBuff()
{
	ioBuff::EndBuff();
}

bool ioActiveCountBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			if( CheckRestoreStateCheck() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	if( m_bDisableDropDownState )
	{
		if( eState == CS_DROP_ZONE_DOWN )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioActiveCountBuff::CheckRestoreStateCheck()
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