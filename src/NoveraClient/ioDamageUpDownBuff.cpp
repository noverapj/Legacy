

#include "stdafx.h"

#include "ioDamageUpDownBuff.h"

#include "ioBaseChar.h"

#include "ioGrowthUpInfo.h"

ioDamageUpDownBuff::ioDamageUpDownBuff()
{
	m_fExtraDamageRate = 0.0f;
}

ioDamageUpDownBuff::ioDamageUpDownBuff( const ioDamageUpDownBuff &rhs )
: ioBuff( rhs ),
 m_AttackRateList( rhs.m_AttackRateList ),
 m_AttackRateNPCList( rhs.m_AttackRateNPCList ),
 m_fDamageRate( rhs.m_fDamageRate ),
 m_bRemoveBuffByDrop( rhs.m_bRemoveBuffByDrop ),
 m_bDisableDownState(rhs.m_bDisableDownState)
{
	m_fExtraDamageRate = 0.0f;
}

ioDamageUpDownBuff::~ioDamageUpDownBuff()
{
}

void ioDamageUpDownBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	LoadAttackRateList( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bRemoveBuffByDrop = rkLoader.LoadBool_e( "remove_buff_drop", false );
}

ioBuff* ioDamageUpDownBuff::Clone()
{
	return new ioDamageUpDownBuff( *this );
}

void ioDamageUpDownBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

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
	m_pOwner->ModifyNPCDamageRate( m_AttackRateNPCList );
}

bool ioDamageUpDownBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	CheckDamageRate();
	m_pOwner->ModifyDamageRate( m_AttackRateList );
	m_pOwner->ModifyNPCDamageRate( m_AttackRateNPCList );
	return true;
}

void ioDamageUpDownBuff::ProcessBuff( float fTimePerSec )
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

void ioDamageUpDownBuff::EndBuff()
{
	m_pOwner->RemoveDamageRate( m_AttackRateList );
	m_pOwner->RemoveNPCDamageRate( m_AttackRateNPCList );
	m_AttackRateList.clear();
	m_AttackRateNPCList.clear();
	ioBuff::EndBuff();
}

void ioDamageUpDownBuff::LoadAttackRateList( ioINILoader &rkLoader )
{
	int iCnt;
	char szBuf[MAX_PATH];

	iCnt = rkLoader.LoadInt_e( "attack_cnt", 0 );
	m_fDamageRate = rkLoader.LoadFloat_e( "attack_rate", FLOAT1 );
	m_AttackRateList.clear();

	if( iCnt == 0 )
	{
		for (int i=0; i < AWT_MAX-1; i++ )
		{
			AttackRate kRate;
			kRate.iType = i+1;
			kRate.fRate = m_fDamageRate;
			m_AttackRateList.push_back( kRate );
		}
	}
	else
	{
		for( int i=0; i < iCnt; ++i )
		{
			AttackRate kRate;
		
			wsprintf_e( szBuf, "attack%d_type", i+1 );
			kRate.iType = rkLoader.LoadInt( szBuf, 0 );
			kRate.fRate = m_fDamageRate;

			m_AttackRateList.push_back( kRate );
		}
	}

	iCnt = rkLoader.LoadInt_e( "npc_attack_cnt", 0 );
	m_fDamageRate = rkLoader.LoadFloat_e( "npc_attack_rate", FLOAT1 );
	m_AttackRateNPCList.clear();

	if( iCnt == 0 )
	{
		for (int i=0; i < AWT_MAX-1; i++ )
		{
			AttackRate kRate;
			kRate.iType = i+1;
			kRate.fRate = m_fDamageRate;
			m_AttackRateNPCList.push_back( kRate );
		}
	}
	else
	{
		for( int i=0; i < iCnt; ++i )
		{
			AttackRate kRate;

			wsprintf_e( szBuf, "npc_attack%d_type", i+1 );
			kRate.iType = rkLoader.LoadInt( szBuf, 0 );
			kRate.fRate = m_fDamageRate;

			m_AttackRateNPCList.push_back( kRate );
		}
	}
}

void ioDamageUpDownBuff::CheckDamageRate()
{
	AttackRateList::iterator iter = m_AttackRateList.begin();
	while( iter != m_AttackRateList.end() )
	{
		(*iter).fRate += m_fExtraDamageRate;

		++iter;
	}

	iter = m_AttackRateNPCList.begin();
	while( iter != m_AttackRateNPCList.end() )
	{
		(*iter).fRate += m_fExtraDamageRate;

		++iter;
	}
}

void ioDamageUpDownBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_fExtraDamageRate;
}

void ioDamageUpDownBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fExtraDamageRate;
}

void ioDamageUpDownBuff::CheckOwnerStateCheck()
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
