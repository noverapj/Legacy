

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioRestorationBuff.h"

ioRestorationBuff::ioRestorationBuff()
{
	m_fGapHP = 0.0f;
	m_fTickPerHPRecovery = 0.0f;
	m_dwTickGap = 0;
}

ioRestorationBuff::ioRestorationBuff( const ioRestorationBuff &rhs )
: ioBuff( rhs )
{
	m_fRecoveryHP = rhs.m_fRecoveryHP;
	m_fDefaultHP = rhs.m_fDefaultHP;
	m_fTickPerHPRecovery = 0.0f;
	m_fGapHP = 0.0f;
	m_dwTickGap = 0;
}

ioRestorationBuff::~ioRestorationBuff()
{
}

void ioRestorationBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fDefaultHP = rkLoader.LoadFloat_e( "hp_recovery_value", 0.0f );
	m_fRecoveryHP = m_fDefaultHP;

	m_dwRecoveryTick = rkLoader.LoadInt_e( "hp_recovery_tick", 0 );
}

ioBuff* ioRestorationBuff::Clone()
{
	return new ioRestorationBuff( *this );
}

void ioRestorationBuff::StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime )
{
	ioBuff::StartBuffWithAniTime( pOwner, dwAniTime );

	m_fGapHP = 0.0f;
	SetBuffDuration( dwAniTime );
	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();

	m_fRecoveryHP = g_MathManager.GetRestorationValue( pOwner->GetCurPlayingTeamUserCnt(), pOwner->GetCurModeType() );

	if( dwAniTime > 0 )
		m_fTickPerHPRecovery = ( m_fRecoveryHP * FLOAT1000 ) / (float)dwAniTime;
	else
		m_fTickPerHPRecovery = m_fRecoveryHP;
}

void ioRestorationBuff::ProcessBuff( float fTimePerSec )
{
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
	
	if( m_fRecoveryHP > 0.0f )
	{
		float fCurRecoverHP = m_fTickPerHPRecovery * fTimePerSec;
		if( m_fRecoveryHP > fCurRecoverHP )
		{
			m_pOwner->RecoveryHP( fCurRecoverHP );
			m_fRecoveryHP -= fCurRecoverHP;
			m_fGapHP += fCurRecoverHP;
			
			StatusValue svHP = m_pOwner->GetHP();
			if( svHP.m_fCurValue == svHP.m_fMaxValue )
			{
				m_fRecoveryHP = 0.0f;
			}
		}
		else
		{
			m_pOwner->RecoveryHP( m_fRecoveryHP );
			m_fGapHP += m_fRecoveryHP;
			m_fRecoveryHP = 0.0f;
		}
	}
}

void ioRestorationBuff::EndBuff()
{
	int iGapHp = (int)m_fGapHP;

	if( iGapHp > 0 )
		m_pOwner->AddHeadDamageInfo( iGapHp, true );

	if(m_pOwner->IsOwnerChar() && !m_CreateChar.IsEmpty() && ( ( (int)m_fGapHP ) > 0 ) )
	{
		// 힐 데미지 기여도 보정
		m_fGapHP *= 1.2f;
		m_pOwner->NotifyOwnerDamaged( m_CreateChar, m_fGapHP, m_dwCreateWeaponItemCode );

		ioBaseChar *pCreateChar = m_pOwner->GetBaseChar( m_CreateChar );
		if( pCreateChar )
		{
			pCreateChar->CheckTeamBuffAttackCnt( m_pOwner );
		}

		m_CreateChar.Clear();
		g_AbuseMgr.AddDamage( (int)m_fGapHP );
	}

	ioBuff::EndBuff();
}
