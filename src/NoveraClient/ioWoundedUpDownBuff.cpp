

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioWoundedUpDownBuff.h"

#include "ioGrowthUpInfo.h"

ioWoundedUpDownBuff::ioWoundedUpDownBuff()
{
	m_fExtraWoundedRate = 0.0f;
}

ioWoundedUpDownBuff::ioWoundedUpDownBuff( const ioWoundedUpDownBuff &rhs )
: ioBuff( rhs ),
 m_WoundedRateList( rhs.m_WoundedRateList ),
 m_iSideType( rhs.m_iSideType ),
 m_bDisableDownState(rhs.m_bDisableDownState),
 m_bUseEndBuffBullet( rhs.m_bUseEndBuffBullet ),
 m_iEndBuffBullet( rhs.m_iEndBuffBullet )
{
	m_fExtraWoundedRate = 0.0f;
}

ioWoundedUpDownBuff::~ioWoundedUpDownBuff()
{
}

void ioWoundedUpDownBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_iSideType = (AttackSide)rkLoader.LoadInt_e( "wounded_side", AS_NONE );

	LoadWoundedRateList( rkLoader );

	m_bUseEndBuffBullet = rkLoader.LoadBool_e( "use_end_buff_bullet", false );
	m_iEndBuffBullet = rkLoader.LoadFloat_e( "end_buff_bullet", 0.0f );
}

ioBuff* ioWoundedUpDownBuff::Clone()
{
	return new ioWoundedUpDownBuff( *this );
}

void ioWoundedUpDownBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( !m_ItemName.IsEmpty() )
	{
		ioGrowthUpInfo *pUpInfo = GetGrowthUpInfoForItem(GT_WOUNDED_BUFF_UP);
		ioGrowthNormalUpInfo *pNormalInfo = ToNormalUpInfo( pUpInfo );
		if( pNormalInfo )
		{
			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			m_fExtraWoundedRate = pNormalInfo->GetValue(pStage);
		}
	}

	CheckWoundedRate();

	m_pOwner->ModifyWoundRate( m_WoundedRateList, m_iSideType );
}

bool ioWoundedUpDownBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_pOwner->ModifyWoundRate( m_WoundedRateList, m_iSideType );
	return true;
}

void ioWoundedUpDownBuff::ProcessBuff( float fTimePerSec )
{
	CheckOwnerStateCheck();

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

	if ( !IsLive() && m_bUseEndBuffBullet )
	{
		ioItem* pOwnerItem = GetOwnerItem();
		if ( pOwnerItem )
			pOwnerItem->SetCurBullet( m_iEndBuffBullet );
	}
}

void ioWoundedUpDownBuff::EndBuff()
{
	m_pOwner->RemoveWoundRate( m_WoundedRateList );
	m_WoundedRateList.clear();
	ioBuff::EndBuff();
}

void ioWoundedUpDownBuff::LoadWoundedRateList( ioINILoader &rkLoader )
{
	int iCnt;
	char szBuf[MAX_PATH];

	iCnt = rkLoader.LoadInt_e( "wounded_cnt", 0 );
	float fWoundedRate = rkLoader.LoadFloat_e( "wounded_rate", FLOAT1 );
	float fSkillWoundedRate = rkLoader.LoadFloat_e( "skill_wounded_rate", FLOAT1 );

	rkLoader.LoadString_e( "wounded_buff", "", szBuf, MAX_PATH );
	ioHashString szWoundedBuff = szBuf;

	m_WoundedRateList.clear();

	if( iCnt == 0 )
	{
		for (int i=0; i < AWT_MAX-1; i++ )
		{
			AttackRate kRate;
			kRate.iType = i+1;
			kRate.fRate = fWoundedRate;
			kRate.fSkillRate = fSkillWoundedRate;
			kRate.szBuff     = szWoundedBuff;
			m_WoundedRateList.push_back( kRate );
		}
		return;
	}

	for( int i=0; i < iCnt; ++i )
	{
		AttackRate kRate;
		
		wsprintf_e( szBuf, "wounded%d_type", i+1 );
		kRate.iType = rkLoader.LoadInt( szBuf, 0 );
		kRate.fRate = fWoundedRate;
		kRate.fSkillRate = fSkillWoundedRate;
		kRate.szBuff     = szWoundedBuff;
		m_WoundedRateList.push_back( kRate );
	}
}

void ioWoundedUpDownBuff::CheckWoundedRate()
{
	AttackRateList::iterator iter = m_WoundedRateList.begin();
	while( iter != m_WoundedRateList.end() )
	{
		(*iter).fRate += m_fExtraWoundedRate;

		++iter;
	}
}

void ioWoundedUpDownBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_fExtraWoundedRate;
}

void ioWoundedUpDownBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fExtraWoundedRate;
}

void ioWoundedUpDownBuff::CheckOwnerStateCheck()
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
