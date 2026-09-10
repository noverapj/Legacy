#include "StdAfx.h"
#include "ioAccDamageUpDownBuff.h"

#include "ioGrowthUpInfo.h"

ioAccDamageUpDownBuff::ioAccDamageUpDownBuff()
{
	m_fExtraDamageRate = 0.0f;
}

ioAccDamageUpDownBuff::ioAccDamageUpDownBuff( const ioAccDamageUpDownBuff &rhs )
	: ioBuff( rhs ),
	m_AttackRateList( rhs.m_AttackRateList ),
	m_bRemoveBuffByDrop( rhs.m_bRemoveBuffByDrop ),
	m_bDisableDownState(rhs.m_bDisableDownState),
	m_iAttackCount( rhs.m_iAttackCount )
{
	m_fExtraDamageRate = 0.0f;
}

ioAccDamageUpDownBuff::~ioAccDamageUpDownBuff()
{
}

void ioAccDamageUpDownBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	LoadAttackRateList( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bRemoveBuffByDrop = rkLoader.LoadBool_e( "remove_buff_drop", false );
}

ioBuff* ioAccDamageUpDownBuff::Clone()
{
	return new ioAccDamageUpDownBuff( *this );
}

void ioAccDamageUpDownBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	ioItem *pItem= GetOwnerItem();
	if ( !pItem )
	{
		SetReserveEndBuff();
		return;
	}

	float fNum = 0.f;
	ioSkill *pSkill = pOwner->GetEquipedSkill( m_SkillName );
	if( pSkill )
		fNum =  pSkill->GetAccessoryRandomNum() / FLOAT1000;

	if ( fNum <= 0.0f )
	{
		SetReserveEndBuff();
		return;
	}
	m_fDamageRateByAcc = fNum;

	if( !m_ItemName.IsEmpty() )
	{
		ioGrowthUpInfo *pUpInfo = GetGrowthUpInfoForItem(GT_DAMAGE_BUFF_UP);
		ioGrowthNormalUpInfo *pNormalInfo = ToNormalUpInfo( pUpInfo );
		if( pNormalInfo )
		{
			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			m_fExtraDamageRate = pNormalInfo->GetValue(pStage);
		}
	}

	CheckDamageRate();

	m_pOwner->ModifyDamageRate( m_AttackRateList );
}

bool ioAccDamageUpDownBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	ioItem *pItem= GetOwnerItem();
	if ( !pItem )
	{
		SetReserveEndBuff();
		return false;
	}

	float fNum = 0.f;
	ioSkill *pSkill = pOwner->GetEquipedSkill( m_SkillName );
	if( pSkill )
		fNum =  pSkill->GetAccessoryRandomNum() / FLOAT1000;

	if ( fNum <= 0.0f )
	{
		SetReserveEndBuff();
		return false;
	}
	m_fDamageRateByAcc = fNum;
	CheckDamageRate();
	m_pOwner->ModifyDamageRate( m_AttackRateList );
	return true;
}

void ioAccDamageUpDownBuff::ProcessBuff( float fTimePerSec )
{
	CheckOwnerStateCheck();

	if( m_bRemoveBuffByDrop && m_pOwner->GetState() == CS_DROP_ZONE_DOWN  )
	{
		SetReserveEndBuff();
		return;
	}

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

void ioAccDamageUpDownBuff::EndBuff()
{
	m_pOwner->RemoveDamageRate( m_AttackRateList );
	m_AttackRateList.clear();
	ioBuff::EndBuff();
}

void ioAccDamageUpDownBuff::LoadAttackRateList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_iAttackCount = rkLoader.LoadInt_e( "attack_cnt", 0 );
	m_AttackRateList.clear();

	if( m_iAttackCount == 0 )
	{
		for (int i=0; i < AWT_MAX-1; i++ )
		{
			AttackRate kRate;
			kRate.iType = i+1;
			m_AttackRateList.push_back( kRate );
		}
		return;
	}

	for( int i=0; i < m_iAttackCount; ++i )
	{
		AttackRate kRate;

		wsprintf_e( szBuf, "attack%d_type", i+1 );
		kRate.iType = rkLoader.LoadInt( szBuf, 0 );
		m_AttackRateList.push_back( kRate );
	}
}

void ioAccDamageUpDownBuff::CheckDamageRate()
{
	AttackRateList::iterator iter = m_AttackRateList.begin();
	while( iter != m_AttackRateList.end() )
	{
		(*iter).fRate = m_fDamageRateByAcc + m_fExtraDamageRate;
		++iter;
	}
}

void ioAccDamageUpDownBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_fExtraDamageRate;
}

void ioAccDamageUpDownBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fExtraDamageRate;
}

void ioAccDamageUpDownBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			SetReserveEndBuff();
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			SetReserveEndBuff();
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
		else if( eState == CS_DROP_ZONE_DOWN )
			SetReserveEndBuff();
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
	}
}
