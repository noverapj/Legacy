
#include "stdafx.h"

#include "ioActiveCountBuff3.h"

#include "ioBaseChar.h"
#include "WeaponAttribute.h"


ioActiveCountBuff3::ioActiveCountBuff3()
{
}

ioActiveCountBuff3::ioActiveCountBuff3( const ioActiveCountBuff3 &rhs )
	: ioBuff( rhs ),
	m_bDisableDownState( rhs.m_bDisableDownState ),
	m_bDisableDropDownState( rhs.m_bDisableDropDownState ),
	m_nMaxActiveCount( rhs.m_nMaxActiveCount ),
	m_vstEffectList( rhs.m_vstEffectList )
{
	m_iCurActiveCount = 0;
}

ioActiveCountBuff3::~ioActiveCountBuff3()
{
}

void ioActiveCountBuff3::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropDownState = rkLoader.LoadBool_e( "disable_drop_down_state", false );

	m_nMaxActiveCount = rkLoader.LoadInt_e( "set_active_max_buff_cnt", 0 );

	int n = rkLoader.LoadInt_e( "active_effect_max_cnt", 0 );
	m_vstEffectList.clear();

	for( int i = 0 ; i < n ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szKey, "active_effect%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vstEffectList.push_back( szBuf );
	}
}

ioBuff* ioActiveCountBuff3::Clone()
{
	return new ioActiveCountBuff3( *this );
}

void ioActiveCountBuff3::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioActiveCountBuff3::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioActiveCountBuff3::ProcessBuff( float fTimePerSec )
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
		if( m_nMaxActiveCount > 0 && m_iCurActiveCount >= m_nMaxActiveCount )
			SetReserveEndBuff();
		ioBuff::ProcessBuff( fTimePerSec );	
		break;	
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioActiveCountBuff3::CheckActiveCount()
{
	if( m_pOwner )
	{
		m_pOwner->EndEffect( m_BuffEffect, false );
		for( int n = 0; n < (int)m_vstEffectList.size(); n++ )
		{
			ioHashString stEffect = m_vstEffectList[n];
			m_pOwner->EndEffect( stEffect, false );
		}

		if( (int)m_vstEffectList.size() > m_iCurActiveCount )
		{
			ioHashString stEffect = m_vstEffectList[m_iCurActiveCount];
			m_pOwner->AttachEffect( stEffect );
		}
	}

	m_iCurActiveCount++;
	m_iCurActiveCount = max( m_iCurActiveCount, 0 );
}

void ioActiveCountBuff3::EndBuff()
{
	ioBuff::EndBuff();

	if( m_pOwner )
	{
		for( int n = 0; n < (int)m_vstEffectList.size(); n++ )
		{
			ioHashString stEffect = m_vstEffectList[n];
			m_pOwner->EndEffect( stEffect, false );
		}
	}
}

bool ioActiveCountBuff3::CheckOwnerStateCheck()
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

bool ioActiveCountBuff3::CheckRestoreStateCheck()
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