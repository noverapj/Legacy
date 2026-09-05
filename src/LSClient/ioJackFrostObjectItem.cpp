

#include "stdafx.h"

#include "ioJackFrostObjectItem.h"
#include "WeaponDefine.h"

ioJackFrostObjectItem::ioJackFrostObjectItem()
: m_ChargeState( CS_NONE ),
m_dwAttackStartTime( 0 ),
m_iCurCombo( 0 ),
m_GatheringEffectID( -1 ),
m_GatheringMaxEffectID( -1 ),
m_bSetChargingMoveAni( false ),
m_vCirclePos( ioMath::VEC3_ZERO ),
m_fCircleOffset( 0.0f ),
m_CircleMapEffectID( -1 ),
m_fCircleLengthRate( 0.0f )
{
}

ioJackFrostObjectItem::ioJackFrostObjectItem( const ioJackFrostObjectItem &rhs )
: ioObjectItem( rhs ),
m_ChargeState( rhs.m_ChargeState ),
m_dwAttackStartTime( rhs.m_dwAttackStartTime ),
m_iCurCombo( rhs.m_iCurCombo ),
m_GatheringEffectID( rhs.m_GatheringEffectID ),
m_GatheringMaxEffectID( rhs.m_GatheringMaxEffectID ),
m_bSetChargingMoveAni( rhs.m_bSetChargingMoveAni ),
m_vCirclePos( rhs.m_vCirclePos ),
m_fCircleOffset( rhs.m_fCircleOffset ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
m_GatherAnimation( rhs.m_GatherAnimation ),
m_GatheringEffect( rhs.m_GatheringEffect ),
m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
m_fGatherAniRate( rhs.m_fGatherAniRate ),
m_GatherMove( rhs.m_GatherMove ),
m_bEnableChargeFireAutoTarget( rhs.m_bEnableChargeFireAutoTarget ),
m_szCircleMapEffect( rhs.m_szCircleMapEffect ),
m_CircleMapEffectID( rhs.m_CircleMapEffectID ),
m_fCircleLengthRate( rhs.m_fCircleLengthRate ),
m_iMaxUseCount( rhs.m_iMaxUseCount ),
m_ReleaseAnimation( rhs.m_ReleaseAnimation ),
m_fReleaseAniRate( rhs.m_fReleaseAniRate ),
m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
m_fRevisionRate( rhs.m_fRevisionRate ),
m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
m_fUpLimitRange( rhs.m_fUpLimitRange )
{
	m_iCurUseCount = 0;
}

ioJackFrostObjectItem::~ioJackFrostObjectItem()
{
}

void ioJackFrostObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

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

	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}
	wsprintf_e( szBuf, "gathering_max_attack" );
	LoadAttackAttribute( szBuf, m_ExtendMaxAttribute, rkLoader );
	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	rkLoader.LoadString_e( "gathering_move", "", szBuf, MAX_PATH );
	m_GatherMove = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_effect", "", szBuf, MAX_PATH );
	m_GatheringMaxEffect = szBuf;

	rkLoader.LoadString_e( "release_animation", "", szBuf, MAX_PATH );
	m_ReleaseAnimation = szBuf;
	m_fReleaseAniRate = rkLoader.LoadFloat_e( "release_ani_rate", FLOAT1 );

	m_iMaxUseCount = rkLoader.LoadInt_e( "max_use_count", 0 );

	rkLoader.LoadString_e( "charging_circle", "", szBuf, MAX_PATH );
	m_szCircleMapEffect = szBuf;
	m_fCircleOffset = rkLoader.LoadFloat_e( "charging_circle_offset", 0.0f );
	m_fCircleLengthRate = rkLoader.LoadFloat_e( "circle_length_rate", FLOAT1 );

	m_fWeaponGravityRate = rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "gathering_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "gathering_float_min_power", 0.0f );
	m_fWeaponMinSpeed = rkLoader.LoadFloat_e( "gathering_min_speed", 0.0f );

	m_fRevisionRate = rkLoader.LoadFloat_e( "gathering_revision_rate", 0.0f );
	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fUpLimitRange = rkLoader.LoadFloat_e( "up_limit_range", 0.0f );
}

ioItem* ioJackFrostObjectItem::Clone()
{
	return new ioJackFrostObjectItem( *this );
}

void ioJackFrostObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( !pWeapon ) return;

	ioObjectItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	if ( m_ChargeState == CS_ATTACK_FIRE )
	{
		ioFloatThrowWeapon *pFloatWeapon = ToFloatThrowWeapon(pWeapon);
		if( !bJumpAttack && pFloatWeapon )
		{
			pFloatWeapon->SetMoveDir( m_vWeaponDir );
			pFloatWeapon->SetMoveSpeed( m_fCurWeaponMoveSpeed );
			pFloatWeapon->SetFloatPower( m_fCurWeaponFloatPower );
			pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
		}
		m_vCirclePos = ioMath::VEC3_ZERO;
	}
}

ioObjectItem::ObjectSubType ioJackFrostObjectItem::GetObjectSubType() const
{
	return OST_JACK_FROST;
}

void ioJackFrostObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;
	DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

	int iExtendCnt = m_vExtendAttributeList.size();
	if( iExtendCnt == 0 || !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
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

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioJackFrostObjectItem::CheckNormalAttack( ioBaseChar *pOwner )
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
			if( m_bCharged )
				pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( m_bCharged )
				{
					if( pOwner->CheckExtendAttackEndJump() )
						return;
				}

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_RELEASE:
		OnRelease( pOwner );
		break;
	}
	CheckChargingMoveState( pOwner );
	CheckCircleMove( pOwner );
}

void ioJackFrostObjectItem::OnCharging( ioBaseChar *pOwner )
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
		ChangeToChargeAttack( pOwner );
	}
}

void ioJackFrostObjectItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
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
		if( m_dwGatheringStartTime + m_dwMaxChargeTime < dwCurTime && m_GatheringMaxEffectID == -1 )
		{
			DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
			ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
			if( pEffect )
			{
				m_GatheringMaxEffectID = pEffect->GetUniqueID();
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)CS_MAX_GATHERING;
				kPacket << m_iCurCombo;
				kPacket << false;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		ChangeToChargeAttack( pOwner );
	}
}

void ioJackFrostObjectItem::OnRelease( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( m_dwReleaseEndTime <= FRAMEGETTIME() )
	{
		pOwner->ReleaseObjectItem( __FUNCTION__, CS_DELAY );
	}
}

void ioJackFrostObjectItem::ChangeToGathering( ioBaseChar *pOwner )
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

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeState = CS_GATHERING;

	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
		m_GatheringEffectID = pEffect->GetUniqueID();

	pOwner->SetAttackMoveEnable( true );
	pOwner->SetChargingState( true );

	m_dwGatheringStartTime = dwCurTime;
	CreateCircleMapEffect( pOwner );
	m_vCirclePos = ioMath::VEC3_ZERO;

	UpdateCircleMapEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJackFrostObjectItem::ChangeToChargeAttack( ioBaseChar *pOwner )
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

	CheckWeaponFloatInfo( pOwner );
	DeleteCircleMapEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << qtTargetRot;
		kPacket << m_vCirclePos;
		kPacket << m_fCurChargeRate;
		if( m_bEnableChargeFireAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioJackFrostObjectItem::CheckChargingMoveState( ioBaseChar *pOwner )
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

void ioJackFrostObjectItem::CheckCircleMove( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	if ( m_CircleMapEffectID == -1 )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if ( m_fCurChargeRate == FLOAT1 && !pOwner->IsSettedDirection() )
		return;

	UpdateCircleMapEffect( pOwner );
}

void ioJackFrostObjectItem::CreateCircleMapEffect( ioBaseChar *pOwner )
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
}

void ioJackFrostObjectItem::UpdateCircleMapEffect( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if ( m_CircleMapEffectID == -1 )
		return;

	D3DXVECTOR3 vPlayerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_vCirclePos = vPlayerPos;
	float fOwnerPosHeight = vPlayerPos.y;
	m_vCirclePos += ( vDir * m_fCircleOffset );
	m_vCirclePos += ( vDir * 100.f * m_fCurChargeRate * m_fCircleLengthRate );
	m_vCirclePos.y = fOwnerPosHeight;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_CircleMapEffectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( m_vCirclePos );
		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

void ioJackFrostObjectItem::DeleteCircleMapEffect( ioBaseChar *pOwner )
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
}

void ioJackFrostObjectItem::DeleteOwnerAttachEffect( ioBaseChar *pOwner, UniqueObjID& EffectID, bool bGrace /* = true */ )
{
	if ( !pOwner )
		return;
	if( EffectID != -1 )
	{
		pOwner->EndEffect( EffectID, bGrace );
		EffectID = -1;
	}
}

void ioJackFrostObjectItem::CheckAttackCount( ioBaseChar* pOwner )
{
	m_iCurUseCount++;
}

bool ioJackFrostObjectItem::IsUseDelete() const
{
	if( m_iCurUseCount >= m_iMaxUseCount  )
		return true;
	
	return false;
}

bool ioJackFrostObjectItem::IsEnableDefense() const
{
	return true;
}

void ioJackFrostObjectItem::SetObjectReleaseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetCurNormalAttackItem( this );
	pOwner->SetState( CS_ATTACK );

	float fCurRate = FLOAT1;
	if( m_fReleaseAniRate > 0.0f )
		fCurRate = m_fReleaseAniRate;
	
	int iAniID = pGrp->GetAnimationIdx( m_ReleaseAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeState = CS_RELEASE;
	m_iCurCombo = 0;

	m_dwReleaseEndTime = dwCurTime;
	m_dwReleaseEndTime += pGrp->GetAnimationFullTime( iAniID ) * fCurRate;
	m_vCirclePos = ioMath::VEC3_ZERO;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioJackFrostObjectItem::GetMaxBullet()
{
	return m_iMaxUseCount;
}

int ioJackFrostObjectItem::GetCurBullet()
{
	int iCurCount = m_iMaxUseCount - m_iCurUseCount;
	iCurCount = max( 0, min( iCurCount, m_iMaxUseCount ) );

	return iCurCount;
}

void ioJackFrostObjectItem::CheckWeaponFloatInfo( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir = m_vCirclePos - vCharPos;
	vDir.y = 0.0f;

	float fRange = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );

	// CharDir
	if( m_vCirclePos == ioMath::VEC3_ZERO )
	{
		vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
	}
	else
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vDir );
		pOwner->SetTargetRotAndMoveDirChange( qtRot, false );
		pOwner->SetWorldOrientation( qtRot );
	}

	float fCurWeaponFloatPower = m_fWeaponFloatMinPower;
	float fCurWeaponMoveSpeed = m_fWeaponMinSpeed;
	if( m_fCurChargeRate > 0.f )
	{
		// CurFloat
		fCurWeaponFloatPower = m_fWeaponFloatPower * m_fCurChargeRate;
		fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

		float fCurGravity = 0.0f;
		float fWeaponMoveSpeed = 0.0f;
		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		if( pStage )
			fCurGravity = pStage->GetRoomGravity() * m_fWeaponGravityRate;

		float fFlightDuration = 0.0f;
		float fGapHeight = pOwner->GetMidHeightByRate(m_fRevisionRate) - vCharPos.y;
		float fGapHeight2 = vCharPos.y - m_vCirclePos.y;
		if( fGapHeight2 > 0.0f )
		{
			fGapHeight += fGapHeight2;
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
		else if( COMPARE(fGapHeight2, -m_fUpLimitHeight, -20.0f) && fRange > m_fUpLimitRange )
		{
			float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
			fFlightDuration = GetTargetTime( fCurGravity, fNewGap, fCurWeaponFloatPower );
			fRange -= 35.0f;

			if( fFlightDuration > 0.0f )
				fCurWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);
		}
		else
		{
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
	}

	m_vWeaponDir = vDir;
	m_fCurWeaponMoveSpeed = fCurWeaponMoveSpeed;
	m_fCurWeaponFloatPower = fCurWeaponFloatPower;
}

float ioJackFrostObjectItem::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
	}

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01;
		fGapTime += 0.01;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

float ioJackFrostObjectItem::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;
	
	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

void ioJackFrostObjectItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case CS_ATTACK_FIRE:
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
	case CS_RELEASE:
		SetObjectReleaseState( pOwner );
		break;
	case CS_MAX_GATHERING:
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
		}
		break;
	}
}

void ioJackFrostObjectItem::OnReleased( ioBaseChar *pOwner )
{
	m_vCirclePos = ioMath::VEC3_ZERO;

	m_dwGatheringStartTime = 0;
	m_dwAttackStartTime = 0;
	DeleteCircleMapEffect( pOwner );
	ioObjectItem::OnReleased( pOwner );
}

bool ioJackFrostObjectItem::SetDefenseAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioObjectItem *pObject = pOwner->GetObject();
	if( pObject && pObject->IsObjectReleaseTCPSend() )
		return false;

	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	int iAniID = pGrp->GetAnimationIdx( m_DefenseAttack.m_AttackAnimation );
	float fRate = m_DefenseAttack.m_fAttackAniRate;
	if( fRate <= 0.0f )
		fRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_DefenseAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fRate, m_DefenseAttack.m_dwPreDelay, false, pOwner->GetMidPositionByRate() );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate, 0.0f, m_DefenseAttack.m_dwPreDelay );

	pOwner->SetState( CS_DEFENSE_ATTACK );

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetReservedSliding( m_DefenseAttack.m_vForceInfoList, iAniID, fRate, m_DefenseAttack.m_dwPreDelay );

	m_dwDefenseAttackEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DEFENSE_ATTACK );
		kPacket << pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		//pOwner->IncreaseWeaponIndexBase();
		kPacket << pOwner->GetWeaponIndexBase();
		kPacket << pOwner->GetMidPositionByRate();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioJackFrostObjectItem::ApplyDefenseAttackState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	int iAniID = pGrp->GetAnimationIdx( m_DefenseAttack.m_AttackAnimation );
	float fRate = m_DefenseAttack.m_fAttackAniRate;
	if( fRate <= 0.0f )
		fRate = FLOAT1;

	D3DXVECTOR3	vPos;
	rkPacket >> vPos;
	pOwner->RefreshFireTimeListWithBase( pOwner->GetWeaponIndexBase(), iAniID, m_DefenseAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, FLOAT1/fRate, m_DefenseAttack.m_dwPreDelay, false, vPos );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate, 0.0f, m_DefenseAttack.m_dwPreDelay );

	pOwner->SetState( CS_DEFENSE_ATTACK );

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetReservedSliding( m_DefenseAttack.m_vForceInfoList, iAniID, fRate, m_DefenseAttack.m_dwPreDelay );

	m_dwDefenseAttackEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
}

void ioJackFrostObjectItem::CheckDefenseAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDefenseAttackEnd > dwCurTime )
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
		}
	}
	else
	{
		//ProcessReserveKeyInput( pOwner );

		if( GetType() == IT_OBJECT )
			pOwner->ReleaseObjectItem( __FUNCTION__, pOwner->GetState() );
	}
}