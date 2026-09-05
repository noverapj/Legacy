
#include "stdafx.h"

#include "ioActiveCountBuff2.h"
#include "ioBaseChar.h"

ioActiveCountBuff2::ioActiveCountBuff2()
{
}

ioActiveCountBuff2::ioActiveCountBuff2( const ioActiveCountBuff2 &rhs )
	: ioBuff( rhs ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_bDisableDropDownState( rhs.m_bDisableDropDownState ),
	  m_bSetUseGauge( rhs.m_bSetUseGauge )
{
	m_iCurActiveCount = m_iMaxActiveCount;
}

ioActiveCountBuff2::~ioActiveCountBuff2()
{
}

void ioActiveCountBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropDownState = rkLoader.LoadBool_e( "disable_drop_down_state", false );
	m_bSetUseGauge = rkLoader.LoadBool_e( "set_use_gauge", false );
}

ioBuff* ioActiveCountBuff2::Clone()
{
	return new ioActiveCountBuff2( *this );
}

void ioActiveCountBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioActiveCountBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioActiveCountBuff2::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	if( m_bSetUseGauge && !CheckSwitchType( fTimePerSec ) )
	{
		SetReserveEndBuff();
		return;
	}
}

void ioActiveCountBuff2::CheckActiveCount()
{
	if ( m_iMaxActiveCount > 0 )
	{
		m_iCurActiveCount--;
		m_iCurActiveCount = max( m_iCurActiveCount, 0 );
	}
}

void ioActiveCountBuff2::EndBuff()
{
	ioBuff::EndBuff();
}

bool ioActiveCountBuff2::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	if ( eState != CS_ATTACK && m_iMaxActiveCount > 0 && m_iCurActiveCount <= 0 )
	{
		SetReserveEndBuff();
		return true;
	}

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

bool ioActiveCountBuff2::CheckRestoreStateCheck()
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