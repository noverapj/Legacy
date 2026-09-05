#include "stdafx.h"

#include "ioPoisoningBuff.h"

#include "ioBaseChar.h"

#include "ioGrowthUpInfo.h"

ioPoisoningBuff::ioPoisoningBuff()
{
	m_dwTickGap = 0;
	m_fCurDamage = 0.0f;
	m_fExtraPosionValue = 0.0f;
}

ioPoisoningBuff::ioPoisoningBuff( const ioPoisoningBuff &rhs )
: ioBuff( rhs ),
 m_fPoisonPerTick( rhs.m_fPoisonPerTick ),
 m_dwPoisonTick( rhs.m_dwPoisonTick ),
 m_bUseLimitHP( rhs.m_bUseLimitHP ),
 m_fLimitHPRate( rhs.m_fLimitHPRate )
{
	m_dwTickGap = 0;
	m_fCurDamage = 0.0f;
	m_fExtraPosionValue = 0.0f;
}

ioPoisoningBuff::~ioPoisoningBuff()
{
}

void ioPoisoningBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fPoisonPerTick = rkLoader.LoadFloat_e( "poison_per_tick", 0.0f );
	m_dwPoisonTick = (DWORD)rkLoader.LoadInt_e( "poison_tick_time", 0 );

	m_bUseLimitHP = rkLoader.LoadBool_e( "use_limit_hp", false );
	m_fLimitHPRate = rkLoader.LoadFloat_e( "limit_hp_rate", 0.0f );
}

ioBuff* ioPoisoningBuff::Clone()
{
	return new ioPoisoningBuff( *this );
}

void ioPoisoningBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( !m_ItemName.IsEmpty() )
	{
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoForItem(GT_POISON_UP));
		if( pUpInfo )
		{
			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			m_fExtraPosionValue = pUpInfo->GetValue(pStage);
		}
	}
}

void ioPoisoningBuff::ProcessBuff( float fTimePerSec )
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
	
	m_dwTickGap += (DWORD)( fTimePerSec * FLOAT1000 );
	if( m_dwTickGap > m_dwPoisonTick )
	{
		if( m_fPoisonPerTick != 0.0f )
		{
			float fCurPoison = m_fPoisonPerTick + m_fExtraPosionValue;

			TeamType eTeam = TEAM_NONE;
			if( m_pOwner->GetTeam() == TEAM_RED )
				eTeam = TEAM_BLUE;
			else if( m_pOwner->GetTeam() == TEAM_BLUE )
				eTeam = TEAM_RED;

			float fRate = m_pOwner->GetMemberBalanceRateForDamage( eTeam );
			fCurPoison *= fRate;

			if( m_pOwner->HasBuff(BT_MANA_SHIELD) )
				m_pOwner->CheckSkillGaugeByDamage( -fCurPoison );
			else
				m_pOwner->RecoveryHP( fCurPoison );

			StatusValue rkHP = m_pOwner->GetHP();
			if( m_iOperationType == OT_ACTIVE && rkHP.m_fCurValue > 0.0f )
			{
				m_fCurDamage += (-fCurPoison);
			}

			if( m_bUseLimitHP )
			{
				float fCurRate = rkHP.m_fCurValue / rkHP.m_fMaxValue;
				if( fCurRate <= m_fLimitHPRate )
				{
					m_pOwner->CheckUseBuffSkill( m_Name, false );
					SetReserveEndBuff();
					return;
				}
			}
		}		
		m_dwTickGap -= m_dwPoisonTick;
	}	
}

void ioPoisoningBuff::EndBuff()
{
	// 데미지 처리
	if( !m_CreateChar.IsEmpty() && ( ( (int)m_fCurDamage ) > 0 ) )
	{
		m_pOwner->NotifyOwnerDamaged( m_CreateChar, m_fCurDamage, m_dwCreateWeaponItemCode );
		m_CreateChar.Clear();

		g_AbuseMgr.AddDamage( (int)m_fCurDamage );
	}

	ioBuff::EndBuff();
}

void ioPoisoningBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_fExtraPosionValue;
}

void ioPoisoningBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fExtraPosionValue;
}


