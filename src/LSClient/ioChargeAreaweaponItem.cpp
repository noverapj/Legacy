

#include "stdafx.h"

#include "ioChargeAreaweaponItem.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioChargeAreaweaponItem::ioChargeAreaweaponItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwStartHoldTime = 0;
	m_dwAreaweaponIndex = 0;
}

ioChargeAreaweaponItem::ioChargeAreaweaponItem( const ioChargeAreaweaponItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_PreHoldAnimation( rhs.m_PreHoldAnimation ),
  m_HoldAnimation( rhs.m_HoldAnimation ),
  m_fHoldAniRate( rhs.m_fHoldAniRate ),
  m_AreaWeaponName( rhs.m_AreaWeaponName ),
  m_dwHoldDuration( rhs.m_dwHoldDuration )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwStartHoldTime = 0;
	m_dwAreaweaponIndex = 0;
}

ioChargeAreaweaponItem::~ioChargeAreaweaponItem()
{
}

void ioChargeAreaweaponItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "pre_holding_animation", "", szBuf, MAX_PATH );
	m_PreHoldAnimation = szBuf;

	rkLoader.LoadString_e( "holding_animation", "", szBuf, MAX_PATH );
	m_HoldAnimation = szBuf;

	m_fHoldAniRate = rkLoader.LoadFloat_e( "holding_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_dwHoldDuration = (DWORD)rkLoader.LoadInt_e( "hold_duration", 0 );
}

ioItem* ioChargeAreaweaponItem::Clone()
{
	return new ioChargeAreaweaponItem( *this );
}

ioWeaponItem::WeaponSubType ioChargeAreaweaponItem::GetSubType() const
{
	return WST_CHARGE_AREAWEAPON;
}

void ioChargeAreaweaponItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwStartHoldTime = 0;
	m_dwAreaweaponIndex = 0;

	if( iCurCombo == 0 )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_PreHoldAnimation );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
	else
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
}

void ioChargeAreaweaponItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_HOLDING:
		OnHolding( pOwner );
		break;
	case CS_ATTACK_FIRE:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioChargeAreaweaponItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToHolding( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioChargeAreaweaponItem::OnHolding( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwStartHoldTime > 0 && m_dwStartHoldTime+m_dwHoldDuration < dwCurTime )
		{
			CheckHoldEnd( pOwner );
		}
	}
	else
	{
		CheckHoldEnd( pOwner );
	}
}

void ioChargeAreaweaponItem::ChangeToHolding( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fHoldAniRate > 0.0f )
		fCurRate = m_fHoldAniRate;

	pGrp->SetLoopAni( m_HoldAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	
	m_ChargeState = CS_HOLDING;
	m_dwStartHoldTime = FRAMEGETTIME();

	if( m_dwAreaweaponIndex != 0 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaweaponIndex );
		m_dwAreaweaponIndex = 0;
	}

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "",
															m_AreaWeaponName,
															vPos,
															qtRot,
															ioAreaWeapon::CT_NORMAL );
	
	if( pArea )
	{
		m_dwAreaweaponIndex = pArea->GetAreaWeaponIndex();
	}


	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;

		kPacket << m_AreaWeaponName;
		kPacket << vPos;
		kPacket << qtRot;
		kPacket << (int)ioAreaWeapon::CT_NORMAL;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeAreaweaponItem::ApplyHolding( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fHoldAniRate > 0.0f )
		fCurRate = m_fHoldAniRate;

	pGrp->SetLoopAni( m_HoldAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_ChargeState = CS_HOLDING;
	m_dwStartHoldTime = FRAMEGETTIME();

	if( m_dwAreaweaponIndex != 0 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaweaponIndex );
		m_dwAreaweaponIndex = 0;
	}

	int iCreatType;
	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;
	ioHashString szAreaWeaponName;
	rkPacket >> szAreaWeaponName;
	rkPacket >> vPos >> qtRot >> iCreatType;

	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "",
															szAreaWeaponName,
															vPos,
															qtRot,
															iCreatType );

	if( pArea )
	{
		m_dwAreaweaponIndex = pArea->GetAreaWeaponIndex();
	}
}

void ioChargeAreaweaponItem::CheckHoldEnd( ioBaseChar *pOwner )
{
	m_ChargeState = CS_NONE;
	m_iCurCombo = 0;
	m_dwStartHoldTime = 0;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->SetState( CS_DELAY );

	if( m_dwAreaweaponIndex != 0 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaweaponIndex );
		m_dwAreaweaponIndex = 0;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeAreaweaponItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	//

	if( bCharged )
	{
		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

		if( iExtendCnt <= m_iCurCombo + 1 )
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		else
			pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeAreaweaponItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	bool bCharged;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
		m_ChargeState = CS_NONE;
		break;
	case CS_HOLDING:
		{
			ApplyHolding( pOwner, rkPacket );
		}
		break;
	case CS_ATTACK_FIRE:
		rkPacket >> bCharged;
		ChangeToAttackFire( pOwner, bCharged );
		break;
	}
}

void ioChargeAreaweaponItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		CheckHoldEnd( pOwner );
		break;
	}
}

void ioChargeAreaweaponItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	//
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

bool ioChargeAreaweaponItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_HOLDING:
			return true;
		}
	}

	return false;
}

void ioChargeAreaweaponItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_ChargeState = CS_NONE;
	m_dwStartHoldTime = 0;

	if( m_dwAreaweaponIndex != 0 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaweaponIndex );
		m_dwAreaweaponIndex = 0;
	}
}


