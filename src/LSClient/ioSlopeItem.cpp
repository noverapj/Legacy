
#include "stdafx.h"

#include "ioSlopeItem.h"
#include "WeaponDefine.h"

ioSlopeItem::ioSlopeItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_FlyGuidEffectID = -1;
}

ioSlopeItem::ioSlopeItem( const ioSlopeItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAni( rhs.m_AttackReadyAni ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_fFlyMinAngle( rhs.m_fFlyMinAngle ),
  m_fFlyMaxAngle( rhs.m_fFlyMaxAngle ),
  m_fFlyAttackAngle( rhs.m_fFlyAttackAngle ),
  m_fFlyDefenseAngle( rhs.m_fFlyDefenseAngle ),
  m_fFlyJumpAngle( rhs.m_fFlyJumpAngle ),
  m_dwFlyAngleTime( rhs.m_dwFlyAngleTime ),
  m_FlyGuidEffect( rhs.m_FlyGuidEffect ),
  m_FlyAttack( rhs.m_FlyAttack ),
  m_FlyAttackUpAni( rhs.m_FlyAttackUpAni ),
  m_FlyAttackDownAni( rhs.m_FlyAttackDownAni ),
  m_FlyDelayUpAni( rhs.m_FlyDelayUpAni ),
  m_FlyDelayCenterAni( rhs.m_FlyDelayCenterAni ),
  m_FlyDelayDownAni( rhs.m_FlyDelayDownAni ),
  m_fFlyDelayAniRate( rhs.m_fFlyDelayAniRate ),    
  m_FlyLandAni( rhs.m_FlyLandAni ),
  m_fFlyLandAniRate( rhs.m_fFlyLandAniRate ),
  m_dwFlyDelayRotateTime( rhs.m_dwFlyDelayRotateTime ),
  m_dwFlyAttackRotateTime( rhs.m_dwFlyAttackRotateTime ),
  m_fFlyDelayMoveSpeed( rhs.m_fFlyDelayMoveSpeed ),
  m_fFlyAttackMoveSpeed( rhs.m_fFlyAttackMoveSpeed ),
  m_fFlyEndJumpAmt( rhs.m_fFlyEndJumpAmt ),
  m_fFlyHeightGap( rhs.m_fFlyHeightGap ),
  m_dwFlyAttackChargeTime( rhs.m_dwFlyAttackChargeTime ),
  m_FlyExtraAttack( rhs.m_FlyExtraAttack ),
  m_dwExtraAttackDuration( rhs.m_dwExtraAttackDuration ),
  m_dwExtraAttackFirstDuration( rhs.m_dwExtraAttackFirstDuration ),
  m_fMaxGauge( rhs.m_fMaxGauge ),
  m_fDecreaseGauge( rhs.m_fDecreaseGauge ),
  m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_FlyGuidEffectID = -1;
	ClearState();
}

ioSlopeItem::~ioSlopeItem()
{
}

void ioSlopeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "attack_ready_animation", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;
	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	rkLoader.LoadString_e( "fly_guid_effect", "", szBuf, MAX_PATH );
	m_FlyGuidEffect = szBuf;
	
	m_fFlyMaxAngle = rkLoader.LoadFloat_e( "fly_max_angle", 0.0f );
	m_fFlyMinAngle = rkLoader.LoadFloat_e( "fly_min_angle", 0.0f );
	m_fFlyAttackAngle = rkLoader.LoadFloat_e( "fly_attack_angle", 0.0f );
	m_fFlyDefenseAngle = rkLoader.LoadFloat_e( "fly_defense_angle", 0.0f );
	m_fFlyJumpAngle = rkLoader.LoadFloat_e( "fly_jump_angle", 0.0f );
	m_dwFlyAngleTime = (DWORD)rkLoader.LoadInt_e( "fly_angle_time", 0 );

	LoadAttackAttribute( "fly_attack", m_FlyAttack, rkLoader );
	rkLoader.LoadString_e( "fly_attack_up_ani", "", szBuf, MAX_PATH );
	m_FlyAttackUpAni = szBuf;
	rkLoader.LoadString_e( "fly_attack_down_ani", "", szBuf, MAX_PATH );
	m_FlyAttackDownAni = szBuf;
	rkLoader.LoadString_e( "fly_delay_up_ani", "", szBuf, MAX_PATH );
	m_FlyDelayUpAni = szBuf;
	rkLoader.LoadString_e( "fly_delay_center_ani", "", szBuf, MAX_PATH );
	m_FlyDelayCenterAni = szBuf;
	rkLoader.LoadString_e( "fly_delay_down_ani", "", szBuf, MAX_PATH );
	m_FlyDelayDownAni = szBuf;
	rkLoader.LoadString_e( "fly_land_ani", "", szBuf, MAX_PATH );
	m_FlyLandAni = szBuf;	
	m_fFlyDelayAniRate = rkLoader.LoadFloat_e( "fly_delay_ani_rate", FLOAT1 );	
	m_fFlyLandAniRate = rkLoader.LoadFloat_e( "fly_land_ani_rate", FLOAT1 );
	m_dwFlyDelayRotateTime = (DWORD)rkLoader.LoadInt_e( "fly_delay_rotate_speed", 0 );
	m_dwFlyAttackRotateTime = (DWORD)rkLoader.LoadInt_e( "fly_attack_rotate_speed", 0 );
	m_fFlyDelayMoveSpeed = rkLoader.LoadFloat_e( "fly_delay_move_speed", 0.0f );
	m_fFlyAttackMoveSpeed = rkLoader.LoadFloat_e( "fly_attack_move_speed", 0.0f );
	m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "fly_end_jump_amt", 0.0f );
	m_fFlyHeightGap = rkLoader.LoadFloat_e( "fly_height_gap", 0.0f );
	m_dwFlyAttackChargeTime = (DWORD)rkLoader.LoadInt_e( "fly_attack_charge_time", 0 );

	wsprintf_e( szKey, "fly_extra_attack_type" );
	m_FlyExtraAttack.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
	wsprintf_e( szKey, "fly_extra_attack_type_resist" );
	m_FlyExtraAttack.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
	wsprintf_e( szKey, "fly_extra_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_FlyExtraAttack.m_WoundedAnimation = szBuf;
	wsprintf_e( szKey, "fly_extra_attack_type_wounded_time" );
	m_FlyExtraAttack.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "fly_extra_attack_type_wounded_loop_ani" );
	m_FlyExtraAttack.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	m_dwExtraAttackDuration = (DWORD)rkLoader.LoadInt_e( "fly_extra_attack_duration", 0 );
	m_dwExtraAttackFirstDuration = (DWORD)rkLoader.LoadInt_e( "fly_extra_attack_first_duration", 0 );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

ioItem* ioSlopeItem::Clone()
{
	return new ioSlopeItem( *this );
}

ioWeaponItem::WeaponSubType ioSlopeItem::GetSubType() const
{
	return WST_SLOPE_ITEM;
}

void ioSlopeItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwKeyReserveTime = 0;

	ClearCancelInfo();
	ClearState();

	if( m_FlyGuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_FlyGuidEffectID, false );
		m_FlyGuidEffectID = -1;
	}

	if( m_AttackReadyAni.IsEmpty() )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioSlopeItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
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
	}
}

void ioSlopeItem::OnCharging( ioBaseChar *pOwner )
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

void ioSlopeItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	
	DWORD dwCurTime = FRAMEGETTIME();

	CheckCurFlyAngle( pOwner );
	UpdateGuidEffect( pOwner );

	if( !pOwner->IsNeedProcess() )	return;

	// 공격버튼땜, 발사.
	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToFlyState( pOwner, m_fFlyCurAngle, true );
		return;
	}

	// 점프키 누름. 점프상태 전환
	if( pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// SD 스킬 입력.
	if( pOwner->IsDefenseKeyDown() )
	{
		int iSkillNum = pOwner->GetSkillKeyInput();
		if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
		{
			pOwner->SetChargingState( false );

			if( m_FlyGuidEffectID != -1 )
			{
				pOwner->EndMapEffect( m_FlyGuidEffectID, false );
				m_FlyGuidEffectID = -1;
			}

			m_ChargeState = CS_NONE;
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << m_fFlyCurAngle;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
}

void ioSlopeItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_GatherAnimation );
	float fCurRate = FLOAT1;

	m_fFlyCurAngle = m_fFlyMinAngle;

	if( m_dwFlyAngleTime > 0 )
	{
		float fAniFull = pGrp->GetAnimationFullTime( iAniID );
		if( fAniFull > 0.0f )
			fCurRate = (float)m_dwFlyAngleTime / fAniFull;
	}

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	
	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	if( m_FlyGuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_FlyGuidEffectID, false );
		m_FlyGuidEffectID = -1;
	}

	CheckCurFlyAngle( pOwner );

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	ioMapEffect *pMapEffect = pOwner->CreateMapEffectBySkill( m_FlyGuidEffect, vPos );
	if( pMapEffect )
	{
		m_FlyGuidEffectID = pMapEffect->GetUniqueID();		
		UpdateGuidEffect( pOwner );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSlopeItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	pOwner->SetChargingState( false );

	if( m_FlyGuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_FlyGuidEffectID, false );
		m_FlyGuidEffectID = -1;
	}

	m_dwCancelCheckTime = dwCurTime;
	m_dwInputCancelCheckTime = dwCurTime;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_fFlyCurAngle;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSlopeItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;
	switch( iState )
	{
	case CS_NONE:
		{
			rkPacket >> m_fFlyCurAngle;

			pOwner->SetChargingState( false );
			if( m_FlyGuidEffectID != -1 )
			{
				pOwner->EndMapEffect( m_FlyGuidEffectID, false );
				m_FlyGuidEffectID = -1;
			}
			m_ChargeState = CS_NONE;
		}
		break;
	case CS_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		rkPacket >> m_fFlyCurAngle;
		break;

	}
}

void ioSlopeItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;
	rkPacket >> qtRot;
	rkPacket >> vPos;

	pOwner->SetTargetRot( qtRot );
	pOwner->SetWorldPosition( vPos );

	switch( iSyncType )
	{
	case LFS_DELAY:
		{
			rkPacket >> m_fFlyCurAngle;
			ChangeToFlyState( pOwner, m_fFlyCurAngle, false );
		}
		break;
	case LFS_ATTACK:
		ChangeToFlyAttackState( pOwner, false );
		break;
	case LFS_LAND:
		ChangeToLandState( pOwner );
		break;
	case SST_ROTATE:
		{
			bool bChangeDir;
			int iDir;
			rkPacket >> bChangeDir;
			rkPacket >> iDir;			

			if( bChangeDir )
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
		}
		break;
	case SST_CHANGE_ANGLE:
		rkPacket >> m_fFlyCurAngle;
		break;
	case LFS_END:
		ChangeToFlyEnd( pOwner );
		break;
	}
}

void ioSlopeItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}

bool ioSlopeItem::IsChargeAttackState( ioBaseChar *pOwner )
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

bool ioSlopeItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

void ioSlopeItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetChargingState( false );

	if( m_FlyGuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_FlyGuidEffectID, false );
		m_FlyGuidEffectID = -1;
	}
}

void ioSlopeItem::GetAutoTargetValue( ioBaseChar *pOwner,
											 float &fRange, float &fMinAngle, float &fMaxAngle,
											 AutoTargetType eType )
{
	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;
}

bool ioSlopeItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioSlopeItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioSlopeItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	SetAirJump( pOwner, iAniID, fTimeRate );
	return false;
}

void ioSlopeItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );

	CheckAirJump( pOwner );
}

void ioSlopeItem::CheckCurFlyAngle( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwFlyAngleTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;

	dwGapTime = dwGapTime % m_dwFlyAngleTime;

	float fGapAngle = fabsf(m_fFlyMaxAngle - m_fFlyMinAngle);
	float fHalfTime = (float)m_dwFlyAngleTime * FLOAT05;

	if( dwGapTime > fHalfTime )
	{
		dwGapTime -= fHalfTime;

		float fRate = (float)dwGapTime / fHalfTime;
		m_fFlyCurAngle = m_fFlyMaxAngle - ( fGapAngle * fRate );
	}
	else if( dwGapTime < fHalfTime )
	{
		float fRate = (float)dwGapTime / fHalfTime;
		m_fFlyCurAngle = m_fFlyMinAngle + ( fGapAngle * fRate );
	}
	else
	{
		m_fFlyCurAngle = m_fFlyMaxAngle;
	}
}

D3DXVECTOR3 ioSlopeItem::CheckFlyDir( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	if( !pOwner )
		return vDir;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

	D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vSightDir, &ioMath::UNIT_Y );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fFlyCurAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	vDir = qtNewRot * vSightDir;
	D3DXVec3Normalize( &vDir, &vDir );

	return vDir;
}

void ioSlopeItem::UpdateGuidEffect( ioBaseChar *pOwner )
{
	// GuidEffect
	ioPlayStage *pStage = pOwner->GetCreator();
	if( m_FlyGuidEffectID != -1 && pStage )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_FlyGuidEffectID );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( pOwner->GetMidPositionByRate() );

			ioEffect *pEff = pMapEffect->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vDir = CheckFlyDir( pOwner );

				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDir );
				pEff->GetParentNode()->SetOrientation( qtRot );
			}
		}
	}
}

void ioSlopeItem::ChangeToFlyState( ioBaseChar *pOwner, float fCurAngle, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_bUseFly = true;
	m_fFlyCurAngle = fCurAngle;
	pOwner->SetState( CS_SLOPE_SPECIAL );

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_FlyDelayUpAni, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_FlyDelayDownAni, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_FlyDelayCenterAni, 0.0f, FLOAT1 );

	m_FlyState = LFS_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_FlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fFlyCurAngle;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSlopeItem::ProcessAnimation( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp = pGrp->GetLoopInPlay( stUp );
	ioAnimation *pCenter = pGrp->GetLoopInPlay( stCenter );
	ioAnimation *pDown = pGrp->GetLoopInPlay( stDown );

	if( !pUp || !pCenter || !pDown )
		return;

	float fAniWeight = m_fFlyCurAngle / m_fFlyMaxAngle;
	if( fAniWeight >= 0.0f )
	{
		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 + fAniWeight );
		pDown->ForceWeight( -fAniWeight );
	}
}

void ioSlopeItem::ChangeToFlyAttackState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_SLOPE_SPECIAL )
		pOwner->SetState( CS_SLOPE_SPECIAL );

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_FlyAttackUpAni, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_FlyAttackDownAni, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_FlyAttack.m_AttackAnimation, 0.0f, FLOAT1 );

	int iAniID = pGrp->GetAnimationIdx( m_FlyAttack.m_AttackAnimation );
	pOwner->RefreshFireTimeList( iAniID, m_FlyAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_FlyAttack.m_fAttackAniRate, 0 );

	m_FlyState = LFS_ATTACK;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_fFlyCurAngle = -m_fFlyCurAngle;
	m_dwMotionEndTime = 0;
	m_bStateChange = true;
	m_dwExtraAttackCheckTime = FRAMEGETTIME() + m_dwExtraAttackFirstDuration;

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_FlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioSlopeItem::IsFlyCharCollisionSkipState()
{
	return true;
}

void ioSlopeItem::ProcessFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FlyState )
	{
	case LFS_DELAY:
		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKey() )
			{
				ChangeToFlyAttackState( pOwner, true );
				return;
			}
			else if( m_fCurGauge >= m_fMaxGauge )
			{
				ChangeToFlyAttackState( pOwner, true );
				m_bStateChange = false;
				return;
			}
		}
		ProcessFlyMove( pOwner, m_fFlyDelayMoveSpeed );
		ProcessRotate( pOwner, m_dwFlyDelayRotateTime );
		ProcessAnimation( pOwner, m_FlyDelayUpAni, m_FlyDelayCenterAni, m_FlyDelayDownAni );
		break;
	case LFS_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			bool bChangeAngel = false;
			if( m_fCurGauge <= 0.f )
			{
				ChangeToFlyEnd( pOwner );
				return;
			}
			else if( pOwner->IsAttackKeyDown() && !m_bStateChange )
			{
				if( m_dwMotionEndTime == 0 )
					m_dwMotionEndTime = dwCurTime + m_dwFlyAttackChargeTime;
				else if( m_dwMotionEndTime < dwCurTime )
				{
					ChangeToFlyEnd( pOwner );
					return;
				}
			}
			else if( pOwner->IsAttackKeyRelease() && m_bStateChange )
				m_bStateChange = false;
			else if( m_dwMotionEndTime != 0 )
			{
				m_dwMotionEndTime = 0;
				bChangeAngel = true;
				m_fFlyCurAngle = m_fFlyAttackAngle;
			}
			else if( pOwner->IsDefenseKey() )
			{
				bChangeAngel = true;
				m_fFlyCurAngle = m_fFlyDefenseAngle;
			}
			else if( pOwner->IsJumpKey() )
			{
				bChangeAngel = true;
				m_fFlyCurAngle = m_fFlyJumpAngle;
			}

			if( bChangeAngel && pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_CHANGE_ANGLE;
				kPacket << pOwner->GetTargetRot();
				kPacket << pOwner->GetWorldPosition();
				kPacket << m_fFlyCurAngle;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		ProcessFlyMove( pOwner, m_fFlyAttackMoveSpeed );
		ProcessRotate( pOwner, m_dwFlyAttackRotateTime );
		ProcessExtraAttack( pOwner );
		ProcessAnimation( pOwner, m_FlyAttackUpAni, m_FlyAttack.m_AttackAnimation, m_FlyAttackDownAni );
		break;
	case LFS_LAND:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY );
		}
		break;
	case LFS_END:
		break;
	}
}

bool ioSlopeItem::ProcessFlyMove( ioBaseChar *pOwner, float fMoveSpeed )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = CheckFlyDir( pOwner );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurMoveSpeed = fMoveSpeed * fTimePerSec;
	D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

	bool bCol = false;
	if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
		pOwner->Translate( vMoveAmt );

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );

	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
	if( vCurPos.y - fMapHeight < m_fFlyHeightGap )
		ChangeToLandState( pOwner );
	
	return true;
}

void ioSlopeItem::ProcessRotate( ioBaseChar *pOwner, DWORD dwRotateTime )
{
	if( dwRotateTime == 0 )
		return;

	CheckKeyInput( pOwner );
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

void ioSlopeItem::ProcessExtraAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if( m_FlyExtraAttack.m_iWeaponIdx <= 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwExtraAttackCheckTime < dwCurTime )
	{
		pOwner->NormalAttackFireExplicit( m_FlyExtraAttack );

		m_dwExtraAttackCheckTime = dwCurTime + m_dwExtraAttackDuration;
	}
}

void ioSlopeItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_CurDirKey = eNewDirKey;
	}

	if( bChangeDir && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ROTATE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << bChangeDir;
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSlopeItem::ChangeToFlyEnd( ioBaseChar *pOwner )
{
	m_FlyState = LFS_END;
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetSKillEndJumpState( m_fFlyEndJumpAmt, false, false, false, true );
	pOwner->ClearDirDoubleClick();
	pOwner->ClearAttackFireTimeAndSkill();

	if( pOwner->IsNeedProcess() )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << LFS_END;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSlopeItem::ChangeToLandState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();
	pOwner->ClearAttackFireTimeAndSkill();

	m_FlyState = LFS_LAND;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_FlyLandAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fFlyLandAniRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fFlyLandAniRate );

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_FlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSlopeItem::ClearState( CharState eNewState )
{
	if( eNewState != CS_JUMP && eNewState != CS_USING_SKILL )
		m_bUseFly = false;

	m_FlyState = LFS_NONE;

	m_dwExtraAttackCheckTime = 0;
	m_fCurGauge = 0.f;
}

int ioSlopeItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioSlopeItem::GetNeedBullet()
{
	return 0.f;
}

int ioSlopeItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioSlopeItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioSlopeItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if( CS_SLOPE_SPECIAL != pOwner->GetState() )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( m_FlyState )
	{
	case LFS_DELAY:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	case LFS_ATTACK:
		if( m_fCurGauge > 0.f )
		{
			float fGauge = m_fDecreaseGauge * fTimePerSec;

			m_fCurGauge -= fGauge;
			m_fCurGauge = max( m_fCurGauge, 0.f );
		}
		return;
	}
}

void ioSlopeItem::SetLandingState( ioBaseChar *pChar )
{
	if( pChar->GetNextState() != CS_USING_SKILL )
		m_bUseFly = false;

	ioAttackableItem::SetLandingState( pChar );
}

void ioSlopeItem::OnReleased( ioBaseChar *pOwner )
{
	if( m_FlyGuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_FlyGuidEffectID, false );
		m_FlyGuidEffectID = -1;
	}

	ioWeaponItem::OnReleased( pOwner );
}

void ioSlopeItem::OnReleasedByEndGame( ioBaseChar *pOwner )
{
	if( m_FlyGuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_FlyGuidEffectID, false );
		m_FlyGuidEffectID = -1;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ioSlopeFlySpecialState::ioSlopeFlySpecialState()
{
}

ioSlopeFlySpecialState::~ioSlopeFlySpecialState()
{
}

void ioSlopeFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	if( eCurrState == CS_JUMP )
	{
		ioSlopeItem *pSlopeItem = ToSlopeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pSlopeItem )
			pSlopeItem->ioAttackableItem::SetLandingState( pOwner );
	}
}

void ioSlopeFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	if( eNewState != CS_SLOPE_SPECIAL )
	{
		ioSlopeItem *pSlopeItem = ToSlopeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pSlopeItem )
			pSlopeItem->ClearState( eNewState );
	}
}

bool ioSlopeFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioSlopeItem *pSlopeItem = ToSlopeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSlopeItem && pSlopeItem->IsFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioSlopeFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSlopeItem *pSlopeItem = ToSlopeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSlopeItem )
		pSlopeItem->ProcessFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSlopeFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSlopeFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}

void ioSlopeFlySpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
}