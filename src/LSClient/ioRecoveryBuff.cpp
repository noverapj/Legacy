

#include "stdafx.h"

#include "ioRecoveryBuff.h"
#include "ioBaseChar.h"

#include "ioGrowthUpInfo.h"

ioRecoveryBuff::ioRecoveryBuff()
{
	m_dwTickGap = 0;
	m_bNowRecovery = true;

	m_fCurRecoveryHP = 0.0f;
	m_fExtraHPRecoveryValue = 0.0f;
}

ioRecoveryBuff::ioRecoveryBuff( const ioRecoveryBuff &rhs )
: ioBuff( rhs ),
m_StartEffect( rhs.m_StartEffect ),
m_fRecoveryHP( rhs.m_fRecoveryHP ),
m_dwRecoveryTick( rhs.m_dwRecoveryTick )
{
	m_dwTickGap = 0;
	m_bNowRecovery = true;

	m_fCurRecoveryHP = 0.0f;
	m_fExtraHPRecoveryValue = 0.0f;
}

ioRecoveryBuff::~ioRecoveryBuff()
{
}

void ioRecoveryBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fRecoveryHP = rkLoader.LoadFloat_e( "hp_recovery_value", 0.0f );
	m_dwRecoveryTick = rkLoader.LoadInt_e( "hp_recovery_tick", 0 );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "buff_start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
}

ioBuff* ioRecoveryBuff::Clone()
{
	return new ioRecoveryBuff( *this );
}

void ioRecoveryBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( !m_ItemName.IsEmpty() )
	{
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoForItem(GT_HEAL_VALUE_UP));
		if( pUpInfo )
		{
			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			m_fExtraHPRecoveryValue = pUpInfo->GetValue(pStage);
		}
	}

	if( !m_StartEffect.IsEmpty() )
		m_pOwner->AttachEffect( m_StartEffect, NULL );

	m_fCurRecoveryHP = 0.0f;
}

void ioRecoveryBuff::EndBuff()
{
	if( !m_StartEffect.IsEmpty() )
		m_pOwner->EndEffect( m_StartEffect, false );

	// 회복한 힐량 처리
	float fAbsDamage = fabsf( m_fCurRecoveryHP );
	if( !m_CreateChar.IsEmpty() && ( ( (int)fAbsDamage ) > 0 ) )
	{
		// 힐 데미지 기여도 보정
		fAbsDamage *= 1.2f;
		m_pOwner->NotifyOwnerDamaged( m_CreateChar, fAbsDamage, m_dwCreateWeaponItemCode );

		ioBaseChar *pCreateChar = m_pOwner->GetBaseChar( m_CreateChar );
		if( pCreateChar )
		{
			pCreateChar->CheckTeamBuffAttackCnt( m_pOwner );
		}

		m_CreateChar.Clear();
		g_AbuseMgr.AddDamage( (int)fAbsDamage );
	}

	ioBuff::EndBuff();
}

void ioRecoveryBuff::ProcessBuff( float fTimePerSec )
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

	if( m_iOperationType == OT_PASSIVE )
		CheckOwnerState();

	if( m_bNowRecovery )
	{
		m_dwTickGap += (DWORD)( fTimePerSec * FLOAT1000 );
		if( m_dwTickGap > m_dwRecoveryTick )
		{
			float fTotalRecoveryHP = m_fRecoveryHP + m_fExtraHPRecoveryValue;
			if( fTotalRecoveryHP != 0.0f )
			{
				m_pOwner->RecoveryHP( fTotalRecoveryHP );

				if( m_iOperationType == OT_ACTIVE )
				{
					StatusValue rkHP = m_pOwner->GetHP();
					if( rkHP.m_fCurValue > 0.0f && rkHP.m_fCurValue != rkHP.m_fMaxValue )
					{
						m_fCurRecoveryHP += fTotalRecoveryHP;
					}
				}
			}

			m_dwTickGap -= m_dwRecoveryTick;
		}
	}
}

void ioRecoveryBuff::CheckOwnerState()
{
	bool bPreState = m_bNowRecovery;

	m_bNowRecovery = true;

	if( m_pOwner->IsNonePlayState() )
	{
		m_bNowRecovery = false;
	}
	else
	{
		if( m_pOwner->GetState() != CS_DELAY &&
			m_pOwner->GetState() != CS_RUN )
		{
			if( !m_pOwner->IsCanUpdateGauge() )
				m_bNowRecovery = false;
		}
	}

	if( m_pOwner->HasOtherBuff( this ) )
		m_bNowRecovery = false;

	if( bPreState == m_bNowRecovery )
		return;

	if( m_bNowRecovery )
	{
		m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );
	}
	else
	{
		m_pOwner->EndEffect( m_BuffEffect, NULL );
	}
}

bool ioRecoveryBuff::IsLiveButStopped() const
{
	if( m_bLive && !m_bNowRecovery )
		return true;

	return false;
}

void ioRecoveryBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_fExtraHPRecoveryValue;
}

void ioRecoveryBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fExtraHPRecoveryValue;
}



