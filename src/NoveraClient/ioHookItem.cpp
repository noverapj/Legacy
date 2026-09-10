

#include "stdafx.h"

#include "ioHookItem.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioHookItem::ioHookItem()
{
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_HookState = HS_NONE;

	m_dwExplosionDuration = 0;
	
	m_iWoundedDropRate = 100;
	
	m_dwExplosionStartTime = 0;
	m_bWoundedDrop = false;
	m_bEquipedOnHand = false;
	m_bSetChargingMoveAni = false;

	m_dwEquipBombTime = 0;
	m_dwEquipEndTime = 0;
}

ioHookItem::ioHookItem( const ioHookItem &rhs )
: ioWeaponItem( rhs ),
m_EquipAnimation( rhs.m_EquipAnimation ),
m_BranchDelayAnimation( rhs.m_BranchDelayAnimation ),
m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
m_BombEquipMesh( rhs.m_BombEquipMesh ),
m_BombEquipEffect( rhs.m_BombEquipEffect ),
m_fEquipTimeRate( rhs.m_fEquipTimeRate ),
m_ChargingMoveAnimation( rhs.m_ChargingMoveAnimation )
{
	m_dwExplosionDuration = rhs.m_dwExplosionDuration;
	
	m_iWoundedDropRate = rhs.m_iWoundedDropRate;
	
	m_iCurCombo = 0;
	
	m_dwExplosionStartTime = 0;
	m_bWoundedDrop = false;
	m_bEquipedOnHand = false;
	m_bSetChargingMoveAni = false;

	m_dwChargeStartTime = 0;

	m_HookState = HS_NONE;
	m_dwEquipBombTime = 0;
	m_dwEquipEndTime = 0;
}

ioHookItem::~ioHookItem()
{
}

void ioHookItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "bomb_equip_mesh", "",szBuf, MAX_PATH );
	m_BombEquipMesh = szBuf;
	rkLoader.LoadString_e( "bomb_equip_effect", "", szBuf, MAX_PATH );
	m_BombEquipEffect = szBuf;
	
	m_dwExplosionDuration = rkLoader.LoadInt_e( "explosion_duration", 5000 );

	m_iWoundedDropRate		= rkLoader.LoadInt_e( "wounded_drop_rate", 100 );
	
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

	rkLoader.LoadString_e( "equip_animation", "", szBuf, MAX_PATH );
	m_EquipAnimation = szBuf;

	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAnimation = szBuf;
	rkLoader.LoadString_e( "attack_move_ani", "", szBuf, MAX_PATH );
	m_ChargingMoveAnimation = szBuf;
	
	m_fEquipTimeRate = rkLoader.LoadFloat_e( "equip_time_rate", FLOAT1 );
}

ioItem* ioHookItem::Clone()
{
	return new ioHookItem( *this );
}

void ioHookItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();
	ClearKeyReserve();

	if( !m_bEquipedOnHand )
	{
		int iMaxCombo = GetMaxCombo();
		if( COMPARE( iCurCombo, 0, iMaxCombo ) )
		{
			m_HookState = HS_CHARGING;
			m_dwChargeStartTime = FRAMEGETTIME();
			m_dwExplosionStartTime = 0;
			m_dwEquipBombTime = 0;
			m_iCurCombo = iCurCombo;

			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			int iAniID = pGrp->GetAnimationIdx( m_BranchDelayAnimation );
			float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

			pOwner->SetState( CS_ATTACK );
			pOwner->SetChargingState( true );
		}
		else
		{
			ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
			pOwner->SetAttackMoveEnable( false );
			m_bSetChargingMoveAni = false;
		}
	}
	else
	{
		int iComboSize = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iComboSize ) )
		{
			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );
			pOwner->SetState( CS_ATTACK );
			pOwner->SetChargingState( false );
			m_HookState = HS_FIRE;
		}
		else if( iComboSize > 0 )
		{
			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[0] );
			pOwner->SetState( CS_ATTACK );
			pOwner->SetChargingState( false );
			m_HookState = HS_FIRE;
		}
	}
}

void ioHookItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_HookState )
	{
	case HS_CHARGING:
		OnCharging( pOwner );
		break;
	case HS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case HS_BOMB_EQUIP:
		OnBombEquip( pOwner );
		break;
	case HS_FIRE:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	}

	CheckChargingMoveState( pOwner );
}

void ioHookItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( m_dwChargeStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToBombEquip( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		pOwner->SetAttackMoveEnable( false );
		m_bSetChargingMoveAni = false;
		ChangeToNormalAttack( pOwner );
	}
}

void ioHookItem::OnBombEquip( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEquipBombTime > 0 && m_dwEquipBombTime < dwCurTime )
	{
		ChangeToBombEquiped( pOwner );
		m_dwEquipBombTime = 0;
	}

	if( m_dwEquipEndTime > 0 && m_dwEquipEndTime < dwCurTime )
	{
		ChangeToBombEquipEnd( pOwner );
	}
}

void ioHookItem::ChangeToBombEquip( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();

	pOwner->StopAutoTargetTracking();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_EquipAnimation );
	if( pOwner->IsNeedProcess() )
	{
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fEquipTimeRate );

		m_dwEquipEndTime = pGrp->GetAnimationFullTime( iAniID ) * m_fEquipTimeRate;
		m_dwEquipEndTime -= 100;
		m_dwEquipEndTime += dwCurTime;


		m_dwEquipBombTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_fEquipTimeRate;
		m_dwEquipBombTime += dwCurTime;
	}
	else
	{
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fEquipTimeRate );
		m_dwEquipBombTime = 0;
	}

	m_HookState = HS_BOMB_EQUIP;
	pOwner->SetAttackMoveEnable( true );
	m_bEquipedOnHand = false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_HookState;
		kPacket << m_iCurCombo;
		kPacket << m_bEquipedOnHand;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHookItem::ChangeToBombEquiped( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	AddBombEntity( pOwner );

	m_bEquipedOnHand = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_HookState;
		kPacket << m_iCurCombo;
		kPacket << m_bEquipedOnHand;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHookItem::ChangeToBombEquipEnd( ioBaseChar *pOwner )
{
	m_HookState = HS_NONE;
	m_dwEquipEndTime = 0;
	m_dwExplosionStartTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT1, true );

	pOwner->SetAttackMoveEnable( false );
	m_bSetChargingMoveAni = false;
	pOwner->SetState( CS_DELAY );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_HookState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHookItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_HookState = HS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_HookState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHookItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
	ioThrowBombWeapon *pBomb = ToThrowBombWeapon( pWeapon );
	ioBoundBombWeapon *pBound = ToBoundBombWeapon( pWeapon );

	if( bJumpAttack && m_bEquipedOnHand )
	{
		if( pBomb )
		{
			pBomb->SetJumpThrow();
			g_TutorialMgr.ActionBombFire();
		}
		else if( pBound )
		{
			pBound->SetJumpThrow();
		}
	}
	else if( m_bWoundedDrop )
	{
		if( pBomb )
		{
			pBomb->SetBombDrop();
		}
		else if( pBound )
		{
			pBound->SetBombDrop();
		}

		pWeapon->SetMoveDir( m_vDropMoveDir );
		m_bWoundedDrop = false;
	}
	else
	{
		if( pBomb )
		{
			g_TutorialMgr.ActionBombFire();
		}
	}

	RemoveBombEntity( pWeapon->GetOwner() );
}

void ioHookItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEndNormalAttack( pOwner );

	if( m_HookState == HS_FIRE )
	{
		RemoveBombEntity( pOwner );
	}

	m_HookState = HS_NONE;
}

void ioHookItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iHookState;
	rkPacket >> iHookState;
	rkPacket >> m_iCurCombo;

	bool bEquipHand = false;

	switch( iHookState )
	{
	case HS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case HS_BOMB_EQUIP:
		rkPacket >> bEquipHand;
		if( bEquipHand )
			ChangeToBombEquiped( pOwner );
		else
			ChangeToBombEquip( pOwner );
		break;
	case HS_NONE:
		ChangeToBombEquipEnd( pOwner );
		break;
	}
}

ioWeaponItem::WeaponSubType ioHookItem::GetSubType() const
{
	return WST_HOOK;
}

bool ioHookItem::CheckExplosionOnHand( ioBaseChar *pOwner )
{
	if( m_dwExplosionStartTime == 0 )
		return false;

	if( m_bEquipedOnHand && GetRemainDuration() == 0 )
	{
		RemoveBombEntity( pOwner );
		return true;
	}
	
	return false;
}

const WeaponInfo& ioHookItem::GetExplosionAttributeIdx() const
{
	return m_vExtendAttributeList[m_iCurCombo].m_vWeaponInfoList.front();
}

DWORD ioHookItem::GetRemainDuration() const
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwExplosionTime = m_dwExplosionStartTime + m_dwExplosionDuration;

	if( dwCurTime < dwExplosionTime )
		return dwExplosionTime - dwCurTime;

	return 0;
}

void ioHookItem::ExplosionOnHand( ioBaseChar *pOwner )
{
	pOwner->NormalAttackFireExplicit( GetExplosionAttributeIdx() );
	m_HookState = HS_NONE;
}

void ioHookItem::ChangeToBombOnDrop( const D3DXVECTOR3 &vMoveDir, ioBaseChar *pOwner )
{
	m_HookState = HS_NONE;
	m_bWoundedDrop = true;
	m_vDropMoveDir = vMoveDir;
}

void ioHookItem::AddBombEntity( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	if( !pOwner->HasEntity( m_BombEquipMesh ) )
	{
		pOwner->AddEntity( m_BombEquipMesh );
		pOwner->AttachEffect( m_BombEquipEffect );
	}
}

void ioHookItem::RemoveBombEntity( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	while( pOwner->HasEntity( m_BombEquipMesh ) )
	{
		pOwner->DestroyEntity( m_BombEquipMesh );
	}

	m_bEquipedOnHand = false;

	pOwner->SetChargingState( false );
	pOwner->EndEffect( m_BombEquipEffect );
}

bool ioHookItem::CheckWoundedDropCondition()
{
	int iRand = rand() % 100 + 1;
	if( m_iWoundedDropRate >= iRand )
		return true;
	
	return false;
}

void ioHookItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );
	
	m_HookState = HS_NONE;
	m_bWoundedDrop = false;

	RemoveBombEntity( pOwner );
}

void ioHookItem::FillNormalAttackExtraInfo( SP2Packet &rkPacket )
{
	ioAttackableItem::FillNormalAttackExtraInfo( rkPacket );
	rkPacket << m_bEquipedOnHand;
}

void ioHookItem::ApplyNormalAttackExtraInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_bEquipedOnHand;
}

bool ioHookItem::CheckOnHand()
{
	return m_bEquipedOnHand;
}

bool ioHookItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_HookState == HS_CHARGING ||
			m_HookState == HS_BOMB_EQUIP )
			return true;
	}

	return false;
}

bool ioHookItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_HookState == HS_CHARGING ||
			m_HookState == HS_BOMB_EQUIP )
			return true;
	}

	return false;
}

void ioHookItem::CheckChargingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ChargingMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_EquipAnimation );
			if( pAni )
			{
				dwPlayTime = pAni->GetCurTime();
			}

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fEquipTimeRate );
			m_bSetChargingMoveAni = true;
		}
	}
	else
	{
		if( m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_EquipAnimation );
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ChargingMoveAnimation );
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fEquipTimeRate );
			m_bSetChargingMoveAni = false;
		}
	}
}

