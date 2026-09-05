

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioMineItem.h"

#include "WeaponDefine.h"

ioMineItem::ioMineItem()
{
	m_dwChargeStartTime = 0;
	m_dwBuryStartTime = 0;
	m_dwBuryEndTime = 0;
	m_dwBuryMotionEndTime = 0;
	m_dwBuryDelayTime = 0;
	
	m_iMaxMineCnt = 1;
	
	m_MineState = MS_NONE;
	m_iCurCombo = 0;
}

ioMineItem::ioMineItem( const ioMineItem &rhs )
: ioWeaponItem( rhs ),
 m_BranchDelayAni( rhs.m_BranchDelayAni ),
 m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
 m_iMaxMineCnt( rhs.m_iMaxMineCnt )
{
	m_MineState = MS_NONE;
	m_iCurCombo = 0;
	m_dwChargeStartTime = 0;
	m_dwBuryStartTime = 0;
	m_dwBuryEndTime = 0;
	m_dwBuryMotionEndTime = 0;
	m_dwBuryDelayTime = 0;
}

ioMineItem::~ioMineItem()
{
}

void ioMineItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_iMaxMineCnt = rkLoader.LoadInt_e( "mine_max_cnt", 1 );

	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAni = szBuf;

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}
}

ioItem* ioMineItem::Clone()
{
	return new ioMineItem( *this );
}

void ioMineItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		m_MineState = MS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		pOwner->SetState( CS_ATTACK );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		
		int iAniID = pGrp->GetAnimationIdx( m_BranchDelayAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
	else
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_MineState = MS_NONE;
	}
}

void ioMineItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	ioAttackableItem::SetNormalAttack( pOwner, m_iCurCombo );
	m_MineState = MS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_MineState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMineItem::ChangeToBurying( ioBaseChar *pOwner )
{
	const AttackAttribute &rkAttr = m_vExtendAttributeList[m_iCurCombo];

	pOwner->StopAutoTargetTracking();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( rkAttr.m_AttackAnimation );
	float fTimeRate   = rkAttr.m_fAttackAniRate;
	DWORD dwPreDelay  = rkAttr.m_dwPreDelay;
	m_dwBuryDelayTime  = rkAttr.m_dwEndDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwBuryStartTime = FRAMEGETTIME();
	m_dwBuryEndTime = FRAMEGETTIME() + pOwner->GetFirstFireTime( rkAttr.m_AttackAnimation ) * fTimeRate;
	m_dwBuryMotionEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwPreDelay;


	m_MineState = MS_BURYING;
	g_TutorialMgr.ActionMineBurying();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_MineState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMineItem::ChangeToBuryEnd( ioBaseChar *pOwner )
{
	pOwner->NormalAttackFireExplicit( m_vExtendAttributeList[m_iCurCombo].m_vWeaponInfoList.front() );
	m_MineState = MS_BURYEND;

	int iExtendAttackCnt = m_vExtendAttributeList.size();
	if( iExtendAttackCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	pOwner->SetComboCheckTime( m_dwKeyReserveTime, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_MineState;
		kPacket << m_iCurCombo;
		kPacket << m_dwKeyReserveTime;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMineItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_MineState )
	{
	case MS_CHARGING:
		OnCharging( pOwner );
		break;
	case MS_NORMAL_ATTACK:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case MS_BURYING:
		OnBurying( pOwner );
		break;
	case MS_BURYEND:
		OnBuryEnd( pOwner );
		break;
	}
}

void ioMineItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToBurying( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioMineItem::OnBurying( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwBuryEndTime > 0 && m_dwBuryEndTime < FRAMEGETTIME() )
	{
		ChangeToBuryEnd( pOwner );
	}
}

void ioMineItem::OnBuryEnd( ioBaseChar *pOwner )
{
	if( pOwner->CheckDefaultNormalAttack( m_dwBuryStartTime, m_dwBuryMotionEndTime, m_dwBuryDelayTime ) )
	{
		m_MineState = MS_NONE;
	}
}

void ioMineItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case MS_NORMAL_ATTACK:
		ioAttackableItem::SetNormalAttack( pOwner, m_iCurCombo );
		m_MineState = MS_NORMAL_ATTACK;
		break;
	case MS_BURYING:
		ChangeToBurying( pOwner );
		break;
	case MS_BURYEND:
		rkPacket >> m_dwKeyReserveTime;
		ChangeToBuryEnd( pOwner );
		break;
	case MS_NONE:
		pOwner->SetState( CS_DELAY );
		m_MineState = MS_NONE;
		break;
	}
}

ioWeaponItem::WeaponSubType ioMineItem::GetSubType() const
{
	return WST_MINE;
}

void ioMineItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioMineWeapon *pMine = ToMineWeapon( pWeapon );
	if( pMine )
	{
		pMine->SetSensingStartTime( m_dwBuryEndTime );
		g_WeaponMgr.CheckMaxWeaponCnt( pWeapon, m_iMaxMineCnt );
	}
}

bool ioMineItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_MineState == MS_CHARGING ||
			m_MineState == MS_BURYING ||
			m_MineState == MS_BURYEND )
			return true;
	}

	return false;
}

int ioMineItem::GetMaxBullet()
{
	return m_iMaxMineCnt;
}

const WeaponInfo& ioMineItem::GetExtendAttributeIdx() const
{
	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
	{
		return m_vExtendAttributeList[m_iCurCombo].m_vWeaponInfoList.front();
	}

	static WeaponInfo tempWeaponInfo;
	return tempWeaponInfo;
}
