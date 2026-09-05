

#include "stdafx.h"

#include "ioPotionBuff.h"
#include "ioBaseChar.h"

ioPotionBuff::ioPotionBuff()
{
	m_fRecoveryHP = 0.0f;
	m_fDefaultHP = 0.0f;
	m_fGapHP = 0.0f;
	m_fTickPerHPRecovery = 0.0f;

	m_fRecoveryGauge = 0.0f;
	m_fDefaultGauge = 0.0f;
	m_fTickPerGauge = 0.0f;
	m_fGapGauge = 0.0f;
}

ioPotionBuff::ioPotionBuff( const ioPotionBuff &rhs )
: ioBuff( rhs )
{
	m_fRecoveryHP = rhs.m_fRecoveryHP;
	m_fDefaultHP = rhs.m_fDefaultHP;
	m_fTickPerHPRecovery = 0.0f;
	m_fGapHP = 0.0f;

	m_fRecoveryGauge = rhs.m_fRecoveryGauge;
	m_fDefaultGauge = rhs.m_fDefaultGauge;
	m_fTickPerGauge = 0.0f;
	m_fGapGauge = 0.0f;
}

ioPotionBuff::~ioPotionBuff()
{
}

void ioPotionBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fDefaultHP = rkLoader.LoadFloat_e( "hp_recovery_value", 0.0f );
	m_fRecoveryHP = m_fDefaultHP;

	m_fDefaultGauge = rkLoader.LoadFloat_e( "gauge_recovery_value", 0.0f );
	m_fRecoveryGauge = m_fDefaultGauge;
}

ioBuff* ioPotionBuff::Clone()
{
	return new ioPotionBuff( *this );
}

void ioPotionBuff::StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime )
{
	ioBuff::StartBuffWithAniTime( pOwner, dwAniTime );

	m_fGapHP = 0.0f;
	m_fGapGauge = 0.0f;
	SetBuffDuration( dwAniTime );
	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();

	if( dwAniTime > 0 )
	{
		m_fTickPerHPRecovery = ( m_fRecoveryHP * FLOAT1000 ) / (float)dwAniTime;
		m_fTickPerGauge = ( m_fRecoveryGauge * FLOAT1000 ) / (float)dwAniTime;
	}
	else
	{
		m_fTickPerHPRecovery = m_fRecoveryHP;
		m_fTickPerGauge = m_fRecoveryGauge;
	}
}

void ioPotionBuff::ProcessBuff( float fTimePerSec )
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

	if( m_pOwner->GetState() != CS_DRINK )
	{
		SetReserveEndBuff();
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

	if( m_fRecoveryGauge > 0.0f )
	{
		// 2007. 10.31 현재 사용되지 않음
		float fCurRecoverGauge = m_fTickPerGauge * fTimePerSec;

		if( m_fRecoveryGauge > fCurRecoverGauge )
		{
			m_fRecoveryGauge -= fCurRecoverGauge;
			m_fGapGauge += fCurRecoverGauge;
		}
		else
		{
			m_fGapGauge += m_fRecoveryGauge;
			m_fRecoveryGauge = 0.0f;
		}
	}
}

void ioPotionBuff::EndBuff()
{
	int iGapHp = (int)m_fGapHP;
	int iGapGauge = (int)m_fGapGauge;

	if( iGapHp > 0 )
		m_pOwner->AddHeadDamageInfo( iGapHp, true );
	if( iGapGauge > 0 )
		m_pOwner->AddHeadDamageInfo( iGapGauge, true );

	if( m_pOwner->GetState() == CS_DRINK )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}

	ioBuff::EndBuff();
}
