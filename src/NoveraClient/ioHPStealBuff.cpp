

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioHPStealBuff.h"

ioHPStealBuff::ioHPStealBuff()
{
	m_bGetEffectUse = false;
}

ioHPStealBuff::ioHPStealBuff( const ioHPStealBuff &rhs )
: ioBuff( rhs ),
m_fLoseHP( rhs.m_fLoseHP ),
m_fGetHP( rhs.m_fGetHP ),
m_fGetGauge( rhs.m_fGetGauge ),
m_bSelfGauge( rhs.m_bSelfGauge ),
m_dwStealTic( rhs.m_dwStealTic ),
m_GetEffect( rhs.m_GetEffect )
{
	m_bGetEffectUse = false;
}

ioHPStealBuff::~ioHPStealBuff()
{
}

void ioHPStealBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fLoseHP = rkLoader.LoadFloat_e( "lose_hp", 0.0f );
	m_fGetHP = rkLoader.LoadFloat_e( "get_hp", 0.0f );
	m_fGetGauge = rkLoader.LoadFloat_e( "get_gauge", 0.0f );

	m_bSelfGauge = rkLoader.LoadBool_e( "target_self", false );

	m_dwStealTic = (DWORD)rkLoader.LoadInt_e( "steal_tic", 0 );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "get_effect", "", szBuf, MAX_PATH );
	m_GetEffect = szBuf;
}

ioBuff* ioHPStealBuff::Clone()
{
	return new ioHPStealBuff( *this );
}

void ioHPStealBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	
	m_dwCheckTicTime = FRAMEGETTIME();

	m_bGetEffectUse = false;
}

bool ioHPStealBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );

	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
		m_dwBuffStartTime = m_dwBuffEndTime - GetBuffDuration();
	}

	m_bGetEffectUse = false;
	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	return true;
}

void ioHPStealBuff::ProcessBuff( float fTimePerSec )
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

	CheckSteal();
}

void ioHPStealBuff::CheckSteal()
{
	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator || !pCreator->IsEnableStealHP() )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_dwStealTic == 0 )
	{
		SetReserveEndBuff();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwCheckTicTime;

	TeamType eTeam = TEAM_NONE;
	if( m_pOwner->GetTeam() == TEAM_RED )
		eTeam = TEAM_BLUE;
	else if( m_pOwner->GetTeam() == TEAM_BLUE )
		eTeam = TEAM_RED;

	float fRate = m_pOwner->GetMemberBalanceRateForDamage( eTeam );

	while( dwGapTime > m_dwStealTic )
	{
		// lose
		float fLoseHP = 0.0f;
		StatusValue rkHP = m_pOwner->GetHP();

		float fCurLoseHP = m_fLoseHP * fRate;
		if( rkHP.m_fCurValue > 0.0f )
		{
			if( rkHP.m_fCurValue >= fCurLoseHP )
				fLoseHP = fCurLoseHP;
			else
				fLoseHP = rkHP.m_fCurValue;

			m_pOwner->RecoveryHP( -fLoseHP );
		}
		else
		{
			SetReserveEndBuff();
			return;
		}

		// get hp
		pCreator->RecoveryHP( m_fGetHP );

		// get gauge
		if( m_bSelfGauge )
		{
			for( int iSlot=0; iSlot < 4; ++iSlot )
			{
				ioItem *pItem = pCreator->GetEquipedItem( iSlot );
				ioSkill *pSkill = pCreator->GetEquipedSkill( iSlot );

				if( !pItem || !pSkill ) continue;
				if( pItem->GetName() != m_ItemName ) continue;

				float fMaxGauge = pItem->GetMaxSkillGauge(pCreator->GetCharName());
				float fCurGauge = pItem->GetCurSkillGuage();

				fCurGauge += m_fGetGauge;
				pItem->SetCurSkillGauge( fCurGauge );
				break;
			}
		}
		else
		{
			for( int iSlot=0; iSlot < 4; ++iSlot )
			{
				ioItem *pItem = pCreator->GetEquipedItem( iSlot );
				ioSkill *pSkill = pCreator->GetEquipedSkill( iSlot );

				if( !pItem || !pSkill ) continue;
				if( pSkill->CheckUseBuff( pCreator ) ) continue;

				float fMaxGauge = pItem->GetMaxSkillGauge(pCreator->GetCharName());
				float fCurGauge = pItem->GetCurSkillGuage();

				fCurGauge += m_fGetGauge;
				pItem->SetCurSkillGauge( fCurGauge );
			}
		}

		dwGapTime -= m_dwStealTic;

		m_dwCheckTicTime = dwCurTime - dwGapTime;

		if( !m_bGetEffectUse && !m_GetEffect.IsEmpty() )
		{
			pCreator->AttachEffect( m_GetEffect, NULL );
			m_bGetEffectUse = true;
		}
	}
}

void ioHPStealBuff::EndBuff()
{
	/*
	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if( pCreator && m_bGetEffectUse && !m_GetEffect.IsEmpty() )
	{
		pCreator->EndEffect( m_GetEffect, false );
		m_bGetEffectUse = false;
	}
	*/

	if( m_bGetEffectUse )
		m_bGetEffectUse = false;

	ioBuff::EndBuff();
}
