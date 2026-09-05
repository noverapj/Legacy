#include "StdAfx.h"
#include "ioZeusItem.h"

#include "ioBaseChar.h"

#include "ioPowerChargeItem4.h"

#include "WeaponDefine.h"
#include "ioPlayStage.h"
ioZeusItem::ioZeusItem():
m_ChargeState( CS_NONE ),
m_dwAttackStartTime( 0 ),
m_iCurCombo( 0 ),
m_GatheringEffectID( -1 ),
m_GatheringMaxEffectID( -1 ),
m_bSetChargingMoveAni( false ),
m_iCurBullet( 0 ),
m_vCirclePos( ioMath::VEC3_ZERO ),
m_fCircleOffset( 0.0f ),
m_ReserveDirKey( ioUserKeyInput::DKI_NONE ),
m_CircleMapEffectID( -1 ),
m_fCircleLengthRate( 0.0f ),
m_iCurChargingGauege( 0 ),
m_ArrowMapEffectID( -1 ),
m_fDashAttackGravityRate( false )
{
	m_szCircleMapEffect.Clear();
}

ioZeusItem::ioZeusItem( const ioZeusItem &rhs ) :
ioWeaponItem( rhs ),
m_ChargeState( rhs.m_ChargeState ),
m_dwAttackStartTime( rhs.m_dwAttackStartTime ),
m_iCurCombo( rhs.m_iCurCombo ),
m_GatheringEffectID( rhs.m_GatheringEffectID ),
m_GatheringMaxEffectID( rhs.m_GatheringMaxEffectID ),
m_bSetChargingMoveAni( rhs.m_bSetChargingMoveAni ),
m_iCurBullet( rhs.m_iCurBullet ),
m_vCirclePos( rhs.m_vCirclePos ),
m_fCircleOffset( rhs.m_fCircleOffset ),
m_ReserveDirKey( rhs.m_ReserveDirKey ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
m_GatherAnimation( rhs.m_GatherAnimation ),
m_GatheringEffect( rhs.m_GatheringEffect ),
m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
m_fGatherAniRate( rhs.m_fGatherAniRate ),
m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
m_GatherMove( rhs.m_GatherMove ),
m_RetreatAttribute( rhs.m_RetreatAttribute ),
m_bEnableChargeFireAutoTarget( rhs.m_bEnableChargeFireAutoTarget ),
m_iMaxBullet( rhs.m_iMaxBullet ),
m_szCircleMapEffect( rhs.m_szCircleMapEffect ),
m_CircleMapEffectID( rhs.m_CircleMapEffectID ),
m_fCircleLengthRate( rhs.m_fCircleLengthRate ),
m_iCurChargingGauege( rhs.m_iCurChargingGauege ),
m_szArrowMapEffect( rhs.m_szArrowMapEffect ),
m_ArrowMapEffectID( rhs.m_ArrowMapEffectID ),
m_fDashAttackGravityRate( rhs.m_fDashAttackGravityRate )
{
}

ioZeusItem::~ioZeusItem()
{
}

void ioZeusItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

	m_bEnableChargeFireAutoTarget = rkLoader.LoadBool_e( "charge_fire_auto_target", false );

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

	wsprintf_e( szBuf, "gathering_max_attack" );
	LoadAttackAttribute( szBuf, m_ExtendMaxAttribute, rkLoader );

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "gathering_move", "", szBuf, MAX_PATH );
	m_GatherMove = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_effect", "", szBuf, MAX_PATH );
	m_GatheringMaxEffect = szBuf;

	rkLoader.LoadString_e( "gathering_max_sound", "", szBuf, MAX_PATH );
	m_GatheringMaxSound = szBuf;

	// retreat
	LoadAttackAttribute_e( "retreat_attack", m_RetreatAttribute, rkLoader );

	m_iMaxBullet = rkLoader.LoadInt_e( "max_gauge", 0 );

	rkLoader.LoadString_e( "charging_circle", "", szBuf, MAX_PATH );
	m_szCircleMapEffect = szBuf;

	rkLoader.LoadString_e( "charging_circle2", "", szBuf, MAX_PATH );
	m_szArrowMapEffect = szBuf;

	m_fCircleOffset = rkLoader.LoadFloat_e( "charging_circle_offset", 0.0f );
	m_fCircleLengthRate = rkLoader.LoadFloat_e( "circle_length_rate", FLOAT1 );

	m_fDashAttackGravityRate = rkLoader.LoadFloat_e( "dash_attack_gravity_rate", 0.0f );
}

ioItem* ioZeusItem::Clone()
{
	return new ioZeusItem( *this );
}

ioWeaponItem::WeaponSubType ioZeusItem::GetSubType() const
{
	return WST_ZEUS_ITEM;
}

void ioZeusItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwRetreatEndTime = 0;
	m_fCurChargeRate = 0.0f;

	m_iCurBullet = 0;

	DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_bSetChargingMoveAni = false;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioZeusItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			pOwner->CheckExtraAniJump();

			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();
			DWORD dwKeyReserveTime = pOwner->GetCharKeyReserveEnableTime();

			if( dwKeyReserveTime > 0 && dwKeyReserveTime + dwLooseTime < dwCurTime )
			{
				if( pOwner->IsNeedProcess() )
				{
					if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
					{
						pOwner->ChangeDirectionByInputDir( false );
						pOwner->SetDashState( false );
						pOwner->SendDashState( false );
						return;
					}
				}
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_RETREAT:
		OnRetreatState( pOwner );
		break;
	}

	CheckChargingMoveState( pOwner );
	CheckCircleMove( pOwner );
}

void ioZeusItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioZeusItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 점프키 누름. 점프상태 전환
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		DeleteCircleMapEffect( pOwner );
		m_vCirclePos = ioMath::VEC3_ZERO;
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;
		m_fCurChargeRate = 0.0f;
		if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				m_fCurChargeRate = FLOAT1;
		}

		if( pOwner->IsDefenseKeyDown() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				pOwner->SetAttackMoveEnable( false );
				pOwner->SetChargingState( false );
				m_bSetChargingMoveAni = false;

				DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
				DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

				m_ChargeState = CS_NONE;

				DeleteCircleMapEffect( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (BYTE)ST_NONE;
					kPacket << m_iCurCombo;
					kPacket << false;		// delaystate
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}

		m_iCurBullet = 100 * m_fCurChargeRate;

		if( (m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime) &&
			m_GatheringMaxEffectID == -1 )
		{
			DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
			ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
			if( pEffect )
			{
				m_GatheringMaxEffectID = pEffect->GetUniqueID();
			}

			if( !m_GatheringMaxSound.IsEmpty() )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
				}
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (BYTE)ST_MAX_GATHRING;
				kPacket << m_iCurCombo;
				kPacket << false;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}

		if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
		{
			ChangeToRetreatAttack( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (BYTE)ST_RETREAT;
				kPacket << m_iCurCombo;
				kPacket << pOwner->GetTargetRot();
				kPacket << pOwner->GetMoveDir();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
	else
	{
		ChangeToChargeAttack( pOwner );
	}
}

void ioZeusItem::ChangeToGathering( ioBaseChar *pOwner, bool bRefresh )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;
	
	m_bSetChargingMoveAni = false;

	if ( !pOwner->IsSettedDirection() )
		pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	else
	{
		m_bSetChargingMoveAni = true;
		pGrp->SetLoopAni( m_GatherMove, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	}

	m_iCurChargingGauege = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeState = CS_GATHERING;

	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
		m_GatheringEffectID = pEffect->GetUniqueID();

	pOwner->SetAttackMoveEnable( true );

	if ( bRefresh )
	{
		m_dwGatheringStartTime = dwCurTime;
		CreateCircleMapEffect( pOwner );
		m_vCirclePos = ioMath::VEC3_ZERO;
	}
	else
		SetVisibleCircleMapEffect( pOwner, true );

	UpdateCircleMapEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)ST_GATHERING;
		kPacket << m_iCurCombo;
		kPacket << bRefresh;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioZeusItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

	m_fCurChargeRate = 0.0f;
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );
		m_szCurAni = pAttribute->m_AttackAnimation;

		m_dwCancelCheckTime = dwCurTime;
		m_dwInputCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)ST_NORMAL_ATTACK;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioZeusItem::ChangeToChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
	  return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXQUATERNION qtTargetRot = pOwner->GetTargetRot();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

	pOwner->SetTargetRotToRotate( qtTargetRot, true, false );

	float fCurChargeAniRate = FLOAT1;
	float fCurForceSlidingRate = FLOAT1;
	if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
	{
		if( m_fMaxChargeAniRate > 0.0f )
		{
			float fCurMaxAniRate = max( 0.0f, (m_fMaxChargeAniRate - FLOAT1) );
			fCurChargeAniRate += fCurMaxAniRate * m_fCurChargeRate;
		}

		if( m_fMaxForceSlidingRate > 0.0f )
		{
			float fCurMaxSlidingRate = max( 0.0f, (m_fMaxForceSlidingRate - FLOAT1) );
			fCurForceSlidingRate += fCurMaxSlidingRate * m_fCurChargeRate;
		}
	}

	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
	{
		if( m_bEnableChargeFireAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );
		else
			pOwner->StopAutoTargetTracking();

		int iAniID = -1;
		float fTimeRate = FLOAT1;
		DWORD dwPreDelay = 0;
		ioEntityGroup *pGrp = pOwner->GetGroup();

		if( m_fCurChargeRate == FLOAT1 && !m_ExtendMaxAttribute.m_AttackAnimation.IsEmpty() )
		{
			pOwner->SetNormalAttackByAttribute( m_ExtendMaxAttribute, true, fCurChargeAniRate, fCurForceSlidingRate );

			iAniID = pGrp->GetAnimationIdx( m_ExtendMaxAttribute.m_AttackAnimation );
			fTimeRate = m_ExtendMaxAttribute.m_fAttackAniRate;
			dwPreDelay = m_ExtendMaxAttribute.m_dwPreDelay;
		}
		else
		{
			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo], true, fCurChargeAniRate, fCurForceSlidingRate );

			iAniID = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
			fTimeRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
			dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;
		}

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	}

	if( iExtendCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_ATTACK_FIRE;

	DeleteCircleMapEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)ST_ATTACK_FIRE;
		kPacket << m_iCurCombo;
		kPacket << qtTargetRot;
		kPacket << m_vCirclePos;
		kPacket << m_fCurChargeRate;
		if( m_bEnableChargeFireAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

void ioZeusItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btState;
	rkPacket >> btState;
	rkPacket >> m_iCurCombo;

	switch( btState )
	{
	case ST_NONE:
		{
			bool bDelayState;
			rkPacket >> bDelayState;

			pOwner->SetAttackMoveEnable( false );
			pOwner->SetChargingState( false );
			m_bSetChargingMoveAni = false;

			DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
			DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

			m_ChargeState = CS_NONE;

			if( bDelayState )
				pOwner->SetState( CS_DELAY );
		}
		break;
	case ST_GATHERING:
		{
			bool bRefresh;
			rkPacket >> bRefresh;

			ChangeToGathering( pOwner, bRefresh );
		}
		break;
	case ST_MAX_GATHRING:
		{
			DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );

			if( m_GatheringMaxEffectID == -1 )
			{
				ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
				if( pEffect )
				{
					m_GatheringMaxEffectID = pEffect->GetUniqueID();
				}
			}

			if( !m_GatheringMaxSound.IsEmpty() )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
				}
			}
		}
		break;
	case ST_ATTACK_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			rkPacket >> m_vCirclePos;
			rkPacket >> m_fCurChargeRate;
			pOwner->SetTargetRot( qtRot );

			ChangeToChargeAttack( pOwner );

			if( m_bEnableChargeFireAutoTarget )
				pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case ST_NORMAL_ATTACK:
		{
			if( !m_bChargeAutoTarget )
				pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeToNormalAttack( pOwner );
		}
		break;
	case ST_RETREAT:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			D3DXVECTOR3 vDir;
			rkPacket >> vDir;
			pOwner->SetMoveDirection( vDir );

			ChangeToRetreatAttack( pOwner );
		}
		break;
	}
}

void ioZeusItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

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

	if ( m_ChargeState == CS_ATTACK_FIRE )
	{
		if( m_vCirclePos != ioMath::VEC3_ZERO )
		{
			D3DXVECTOR3 vStartPos =	pWeapon->GetPosition();
			D3DXVECTOR3 vDir = m_vCirclePos - vStartPos;
			D3DXVec3Normalize( &vDir, &vDir );
			pWeapon->SetMoveDir(vDir );
		}
	}
}

bool ioZeusItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

bool ioZeusItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !bCheck ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		case CS_ATTACK_FIRE:
			return true;
		}
	}

	return false;
}

void ioZeusItem::CheckChargingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsCanAttackMove() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;
	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection()  )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bSetChargingMoveAni = true;

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAni( m_GatherMove, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
	else
	{
		if( m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_GatherAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetChargingMoveAni = false;
		}
	}
}

void ioZeusItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	m_fCurChargeRate = 0.0f;

	m_iCurBullet = 0;

	DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

	m_ChargeState = CS_NONE;
}

bool ioZeusItem::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	return true;
}

void ioZeusItem::GetAutoTargetValue( ioBaseChar *pOwner,
	float &fRange, float &fMinAngle, float &fMaxAngle,
	AutoTargetType eType )
{
	switch( eType )
	{
	case ATT_NORMAL:
		if( IsChargeAutoTarget(pOwner) )
		{
			float fCurTargetRangeRate, fCurTargetAngleRate;
			fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

			float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
			float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

			fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
			fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

			fRange = m_fAutoTargetRange * fCurTargetRangeRate;
			fMinAngle = m_fAutoTargetMinAngle / fCurTargetAngleRate;
			fMaxAngle = m_fAutoTargetMaxAngle / fCurTargetAngleRate;
		}
		else
		{
			fRange = m_fAutoTargetRange;
			fMinAngle = m_fAutoTargetMinAngle;
			fMaxAngle = m_fAutoTargetMaxAngle;
		}
		return;
	case ATT_DASH:
		if( m_fDashAutoTargetRange == 0.0f )
			break;

		fRange = m_fDashAutoTargetRange;
		fMinAngle = m_fDashAutoTargetMinAngle;
		fMaxAngle = m_fDashAutoTargetMaxAngle;
		return;
	case ATT_JUMP:
		if( m_fJumpAutoTargetRange == 0.0f )
			break;

		fRange = m_fJumpAutoTargetRange;
		fMinAngle = m_fJumpAutoTargetMinAngle;
		fMaxAngle = m_fJumpAutoTargetMaxAngle;
		return;
	case ATT_COUNTER:
		if( m_fCountAutoTargetRange == 0.0f )
			break;

		fRange = m_fCountAutoTargetRange;
		fMinAngle = m_fCountAutoTargetMinAngle;
		fMaxAngle = m_fCountAutoTargetMaxAngle;
		return;
	}


	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;

	return;
}

void ioZeusItem::ClearCurChargeRateForSkill()
{
	m_fCurChargeRate = 0.0f;
}

void ioZeusItem::ChangeToRetreatAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeState = CS_RETREAT;
	m_dwRetreatEndTime = dwCurTime;

	DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

	m_dwKeyReserveTime = 0;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );


	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pOwner->ClearDirDoubleClick();

	int iAniID = pGrp->GetAnimationIdx( m_RetreatAttribute.m_AttackAnimation );
	float fTimeRate = m_RetreatAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_RetreatAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();

	m_dwRetreatEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pOwner->SetReservedSliding( m_RetreatAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	SetVisibleCircleMapEffect( pOwner, false );
}

void ioZeusItem::OnRetreatState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime < dwCurTime )
	{
		m_ChargeState = CS_NONE;
		if ( m_ReserveDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( m_ReserveDirKey );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );

		m_ReserveDirKey = ioUserKeyInput::DKI_NONE;

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ChangeToGathering( pOwner, false );
			return;
		}

		// 대기
		m_fCurChargeRate = 0.0f;

		DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
		DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

		pOwner->SetState( CS_DELAY );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (BYTE)ST_NONE;
			kPacket << m_iCurCombo;
			kPacket << true;			// delaystate
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
			ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
			if ( eDir != ioUserKeyInput::DKI_NONE )
				m_ReserveDirKey = eDir;
		}
	}
}

void ioZeusItem::CheckCircleMove( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	if ( m_ArrowMapEffectID == -1 && m_CircleMapEffectID == -1 )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if ( m_iCurChargingGauege == m_iMaxBullet && !pOwner->IsSettedDirection() )
		return;

	m_iCurChargingGauege = m_iCurBullet;
	UpdateCircleMapEffect( pOwner );
}

void ioZeusItem::CreateCircleMapEffect( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;
	ioMapEffect *pMapEffect = NULL;
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	if ( !m_szCircleMapEffect.IsEmpty() )
		pMapEffect = pStage->CreateMapEffect( m_szCircleMapEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );

	if( pMapEffect )
	{
		m_CircleMapEffectID = pMapEffect->GetUniqueID();
		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}

	pMapEffect = NULL;
	if ( !m_szArrowMapEffect.IsEmpty() )
		pMapEffect = pStage->CreateMapEffect( m_szArrowMapEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );

	if( pMapEffect )
	{
		m_ArrowMapEffectID = pMapEffect->GetUniqueID();
		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

void ioZeusItem::UpdateCircleMapEffect( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if ( m_ArrowMapEffectID == -1 && m_CircleMapEffectID == -1 )
		return;

	D3DXVECTOR3 vPlayerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_vCirclePos = vPlayerPos;
	float fOwnerPosHeight = vPlayerPos.y;
	m_vCirclePos += ( vDir * m_fCircleOffset );
	m_vCirclePos += ( vDir * (float)m_iCurBullet * m_fCircleLengthRate );
	m_vCirclePos.y = fOwnerPosHeight;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_ArrowMapEffectID );
	if( pMapEffect )
	{
		float fDist = D3DXVec3Length( &( m_vCirclePos - vPlayerPos ) );
		D3DXVECTOR3 vPos = vPlayerPos + ( vDir * ( fDist * FLOAT05 ) );

		pMapEffect->SetWorldPosition( vPos );
		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );

		ioEffect* pEffect = pMapEffect->GetEffect();
		float fZEffectSize = pEffect->GetBoundBox().GetSize().z;		//이펙트 길이
		float fRate = FLOAT1; 
		if ( fZEffectSize != 0 )
			fRate = fDist/fZEffectSize;
		pEffect->GetParentNode()->SetScale( FLOAT1 , FLOAT1 , fRate );
		
	}

	pMapEffect = pStage->FindMapEffect( m_CircleMapEffectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( m_vCirclePos );
		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

void ioZeusItem::DeleteCircleMapEffect( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 	
		return;
	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_CircleMapEffectID );
	if( pMapEffect )
	{
		pMapEffect->EndEffectForce();
		m_CircleMapEffectID = -1;
	}

	pMapEffect = pStage->FindMapEffect( m_ArrowMapEffectID );
	if( pMapEffect )
	{
		pMapEffect->EndEffectForce();
		m_ArrowMapEffectID = -1;
	}
}

void ioZeusItem::SetVisibleCircleMapEffect( ioBaseChar *pOwner, bool bVisible )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;
	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_CircleMapEffectID );
	if( pMapEffect )
		pMapEffect->SetVisible( bVisible );

	pMapEffect = pStage->FindMapEffect( m_ArrowMapEffectID );
	if( pMapEffect )
		pMapEffect->SetVisible( bVisible );
}

void ioZeusItem::DeleteOwnerAttachEffect( ioBaseChar *pOwner, UniqueObjID& EffectID, bool bGrace /* = true */ )
{
	if ( !pOwner )
		return;
	if( EffectID != -1 )
	{
		pOwner->EndEffect( EffectID, bGrace );
		EffectID = -1;
	}
}

void ioZeusItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_vCirclePos = ioMath::VEC3_ZERO;

	m_dwGatheringStartTime = 0;
	m_dwAttackStartTime = 0;
	m_ReserveDirKey = ioUserKeyInput::DKI_NONE;
	DeleteCircleMapEffect( pOwner );
}
