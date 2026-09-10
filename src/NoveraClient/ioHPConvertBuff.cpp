

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioGrowthUpInfo.h"

#include "ioHPConvertBuff.h"

ioHPConvertBuff::ioHPConvertBuff()
{
}

ioHPConvertBuff::ioHPConvertBuff( const ioHPConvertBuff &rhs )
: ioBuff( rhs ),
m_fConvertHP( rhs.m_fConvertHP ),
m_fConvertRate( rhs.m_fConvertRate )
{
}

ioHPConvertBuff::~ioHPConvertBuff()
{
}

void ioHPConvertBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fConvertHP = rkLoader.LoadFloat_e( "convert_hp", 0.0f );
	m_fConvertRate = rkLoader.LoadFloat_e( "convert_rate", FLOAT1 );
}

ioBuff* ioHPConvertBuff::Clone()
{
	return new ioHPConvertBuff( *this );
}

void ioHPConvertBuff::StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime )
{
	ioBuff::StartBuffWithAniTime( pOwner, dwAniTime );

	SetBuffDuration( dwAniTime );
	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoForItem(GT_CONVERT_RATE));
	if( pUpInfo )
	{
		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	float fCurRate = m_fConvertRate + fValue;

	// 체력을 회복할 게이지 값으로 전환
	StatusValue svHP = m_pOwner->GetHP();
	float fCurHP = min( svHP.m_fCurValue, m_fConvertHP );
	m_fRecoveryGauge = fCurHP * fCurRate;
	
	// 체력 감소
	m_pOwner->RecoveryHP( -fCurHP );

	if( dwAniTime > 0 )
		m_fTickPerRecoveryGauge = ( m_fRecoveryGauge * FLOAT1000 ) / (float)dwAniTime;
	else
		m_fTickPerRecoveryGauge = m_fRecoveryGauge;
}

void ioHPConvertBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if( m_fRecoveryGauge > 0.0f )
	{
		float fCurGauge = m_fTickPerRecoveryGauge * fTimePerSec;
		float fCurRecoveryGauge = 0.0f;

		if( m_fRecoveryGauge > fCurGauge )
		{
			m_fRecoveryGauge -= fCurGauge;
			fCurRecoveryGauge = fCurGauge;
		}
		else
		{
			fCurRecoveryGauge = m_fRecoveryGauge;
			m_fRecoveryGauge = 0.0f;
		}

		ioItem *pOwnerItem = m_pOwner->GetEquipedItem( m_ItemName );

		for( int iSlot=0; iSlot < 4; ++iSlot )
		{
			ioItem *pItem = m_pOwner->GetEquipedItem( iSlot );
			ioSkill *pSkill = m_pOwner->GetEquipedSkill( iSlot );

			if( !pItem || !pSkill ) continue;
			//if( pItem == m_pOwnerItem ) continue;
			if( pItem == pOwnerItem ) continue;

			if( g_WeaponMgr.CheckRotationWeaponByItem(m_pOwner->GetCharName(), pItem->GetItemCreateIndex()) )
				continue;

			float fCurGauge = pItem->GetCurSkillGuage();
			fCurGauge += fCurRecoveryGauge;
			pItem->SetCurSkillGauge( fCurGauge );
		}
	}
}

void ioHPConvertBuff::EndBuff()
{
	ioBuff::EndBuff();
}
