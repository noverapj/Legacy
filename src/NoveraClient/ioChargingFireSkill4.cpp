

#include "stdafx.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioChargingFireSkill4.h"

#include "WeaponDefine.h"

ioChargingFireSkill4::ioChargingFireSkill4()
{
	m_State = SS_NONE;
	m_LoopAimState = LAS_NONE;

	m_dwChargingEffect = -1;

	m_bSetHandMesh = false;

	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

	m_fCurChargeRate = 0.0f;

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	m_szTargetName.Clear();
	m_dwAimStartTime = 0;

	m_bSetAimedTarget = false;
	m_vAimedPos = ioMath::VEC3_ZERO;
}

ioChargingFireSkill4::ioChargingFireSkill4( const ioChargingFireSkill4 &rhs )
: ioNormalSkill( rhs ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_dwMaxChargingTime( rhs.m_dwMaxChargingTime ),
 m_SkillChargingEffect( rhs.m_SkillChargingEffect ),
 m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_SkillFailMotion( rhs.m_SkillFailMotion ),
 m_fSkillFailMotionRate( rhs.m_fSkillFailMotionRate ),
 m_dwAimTime( rhs.m_dwAimTime ),
 m_fAimAngle( rhs.m_fAimAngle ),
 m_fAimMinRange( rhs.m_fAimMinRange ),
 m_fAimMaxRange( rhs.m_fAimMaxRange ),
 m_fMarkerRange( rhs.m_fMarkerRange ),
 m_szReadyMarker( rhs.m_szReadyMarker ),
 m_szAimingMarker( rhs.m_szAimingMarker ),
 m_szAimedMarker( rhs.m_szAimedMarker ),
 m_szZeroAimedMarker( rhs.m_szZeroAimedMarker ),
 m_bEnableDownAim( rhs.m_bEnableDownAim ),
 m_fTargetAimRate( rhs.m_fTargetAimRate ),
 m_fDownTargetAimRate( rhs.m_fDownTargetAimRate )
{
	m_State = SS_NONE;
	m_LoopAimState = LAS_NONE;

	m_dwChargingEffect = -1;

	m_bSetHandMesh = false;

	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

	m_fCurChargeRate = 0.0f;

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	m_szTargetName.Clear();
	m_dwAimStartTime = 0;

	m_bSetAimedTarget = false;
	m_vAimedPos = ioMath::VEC3_ZERO;
}

ioChargingFireSkill4::~ioChargingFireSkill4()
{
}

void ioChargingFireSkill4::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_dwMaxChargingTime = rkLoader.LoadInt_e( "charging_max_time", 0 );

	rkLoader.LoadString_e( "skill_charging_effect", "", szBuf, MAX_PATH );
	m_SkillChargingEffect = szBuf;

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );

	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	// Action Attack
	m_AttackAttribute.Init();
	LoadAttackAttribute_e( "charging_attack", m_AttackAttribute, rkLoader );

	// fail
	rkLoader.LoadString_e( "skill_charge_fail_motion", "", szBuf, MAX_PATH );
	m_SkillFailMotion = szBuf;

	m_fSkillFailMotionRate = rkLoader.LoadFloat_e( "skill_charge_fail_motion_rate", FLOAT1 );

	// aim, target
	m_dwAimTime = (DWORD)rkLoader.LoadInt_e( "target_aim_time", 0 );

	m_fAimMinRange = rkLoader.LoadFloat_e( "target_aim_min_range", 0.0f );
	m_fAimMaxRange = rkLoader.LoadFloat_e( "target_aim_max_range", 0.0f );

	m_fAimAngle = rkLoader.LoadFloat_e( "target_aim_angle", 0.0f );

	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );

	rkLoader.LoadString_e( "ready_marker", "", szBuf, MAX_PATH );
	m_szReadyMarker = szBuf;
	rkLoader.LoadString_e( "aiming_marker", "", szBuf, MAX_PATH );
	m_szAimingMarker = szBuf;
	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_szAimedMarker = szBuf;
	rkLoader.LoadString_e( "zero_aimed_marker", "", szBuf, MAX_PATH );
	m_szZeroAimedMarker = szBuf;

	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	m_fTargetAimRate = rkLoader.LoadFloat_e( "target_aim_rate", FLOAT05 );
	m_fDownTargetAimRate = rkLoader.LoadFloat_e( "down_target_aim_rate", 0.0f );
}

ioSkill* ioChargingFireSkill4::Clone()
{
	return new ioChargingFireSkill4( *this );
}

bool ioChargingFireSkill4::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

	m_LoopAimState = LAS_NONE;

	m_bSetAimedTarget = false;
	m_szTargetName.Clear();
	m_dwAimStartTime = 0;
	m_vAimedPos = ioMath::VEC3_ZERO;

	// set marker none...
	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );

	m_dwChargingEffect = -1;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_fCurChargeRate = 0.0f;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioChargingFireSkill4::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	//DestroyMapEffect
	if( m_dwChargingEffect != -1 )
		pChar->EndMapEffect( m_dwChargingEffect, false );

	m_dwChargingEffect = -1;

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_fCurChargeRate = 0.0f;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

	m_State = SS_NONE;
	m_LoopAimState = LAS_NONE;

	m_bSetAimedTarget = false;
	m_szTargetName.Clear();
	m_dwAimStartTime = 0;
	m_vAimedPos = ioMath::VEC3_ZERO;

	// set marker none...
	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );
}

void ioChargingFireSkill4::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	int iSlot = GetOwnerItem()->GetType() - 1;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckCharRotateKey( pChar );
			ProcessLoopState( pChar, pStage );

			ProcessAimState( pChar, pStage );

			if( m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
			{
				 SetActionState( pChar, pStage, false );
				//SetFailState( pChar );
				return;
			}

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() ||
					pChar->IsDefenseKey() ||
					pChar->IsJumpKey() ||
					pChar->GetSkillKeyInput() == iSlot )
				{
					SetActionState( pChar, pStage, true );
					return;
				}
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_dwFireEndTime > 0 && m_dwFireEndTime < dwCurTime )
			{
				if( m_dwChargingEffect != -1 )
					pChar->EndMapEffect( m_dwChargingEffect, false );

				m_dwFireEndTime = 0;
			}
		}

		if( m_dwChargingEffect != -1 )
		{
			pChar->UpdateAttachMapEffect( m_dwChargingEffect );
		}
		break;
	case SS_FAIL:
		break;
	}
}

bool ioChargingFireSkill4::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioChargingFireSkill4::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar ) return;

	pChar->AttachEffect( m_StartEffect );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioChargingFireSkill4::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	EndPreDelayEffect( pChar );

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;
	m_dwLoopStartTime = dwCurTime;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( m_dwChargingEffect != -1 )
	{
		pChar->EndEffect( m_dwChargingEffect, false );
	}

	ioMapEffect *pChargingEff = pChar->CreateMapEffectAttachChar( m_SkillChargingEffect );
	if( pChargingEff )
	{
		m_dwChargingEffect = pChargingEff->GetUniqueID();
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	//
	SetAimState( pChar, false );
}

void ioChargingFireSkill4::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	if( m_State != SS_LOOP )
		return;

	SetNormalSkill( pChar );

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_ACTION;

	// Charging Rate
	DWORD dwGapTime = dwCurTime - m_dwLoopStartTime;
	if( m_dwLoopStartTime > 0 && m_dwMaxChargingTime > 0 )
	{
		if( m_dwMaxChargingTime > dwGapTime )
			m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargingTime;
		else
			m_fCurChargeRate = FLOAT1;
	}

	float fCurChargeAniRate = FLOAT1;
	float fCurForceSlidingRate = FLOAT1;
	if( m_fCurChargeRate > 0.0f )
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
	//

	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	float fAniRate = m_AttackAttribute.m_fAttackAniRate * fCurChargeAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = m_AttackAttribute.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( iAniID != -1 )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	// Attack
	if( !m_AttackAttribute.m_vWeaponInfoList.empty() )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											m_AttackAttribute.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fAniRate,
											dwPreDelay );
	}

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	float fFireStart = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" );
	if( fFireStart > 0.0f )
	{
		m_dwFireStartTime = dwCurTime + fFireStart + dwPreDelay;
	}

	float fFireEnd = pGrp->GetAnimationEventTime_e( iAniID, "fire_e" ) * fAniRate;
	if( fFireEnd > 0.0f )
	{
		m_dwFireEndTime = dwCurTime + fFireEnd + dwPreDelay;
	}

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fAniRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd + dwPreDelay;
	else
		m_dwAttackEndTime = 0;

	pChar->AttachEffect( m_EndEffect );

	pChar->SetReservedSliding( m_AttackAttribute.m_vForceInfoList, iAniID, fAniRate, dwPreDelay, fCurForceSlidingRate );

	// set marker none...
	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargingFireSkill4::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_LOOP:
		{
			int iSubType;
			rkPacket >> iSubType;

			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotToRotate( qtRot, true, false );

			switch( iSubType )
			{
			case LAS_WAIT:
				SetAimState( pChar, false );
				break;
			case LAS_AIMED:
				{
					ioHashString szTarget;
					rkPacket >> szTarget;

					SetAimedState( pChar, szTarget );
				}
				break;
			}
		}
		break;
	case SSC_ACTION:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotToRotate( qtRot, true, false );

			SetActionState( pChar, pStage, false );
		}
		break;
	case SSC_DIR:
		{
			int iCurKey;
			rkPacket >> iCurKey;

			pChar->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iCurKey );

			SetAimState( pChar, false );
		}
		break;
	}
}

bool ioChargingFireSkill4::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	switch( m_State )
	{
	case SS_LOOP:
		return false;
	case SS_ACTION:
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioChargingFireSkill4::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioChargingFireSkill4::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioChargingFireSkill4::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioChargingFireSkill4::ProcessLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( m_dwChargingEffect == -1 )
		return;

	pChar->UpdateAttachMapEffect( m_dwChargingEffect );

	// Charging Rate
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwLoopStartTime;
	if( m_dwLoopStartTime > 0 && m_dwMaxChargingTime > 0 )
	{
		if( m_dwMaxChargingTime > dwGapTime )
			m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargingTime;
		else
			m_fCurChargeRate = FLOAT1;
	}

	float fCurScaleRate = FLOAT1;
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		fCurScaleRate += fCurMaxScale * m_fCurChargeRate;

		ioMapEffect *pMapEff = pStage->FindMapEffect( m_dwChargingEffect );
		if( pMapEff && fCurScaleRate > FLOAT1 )
		{
			ioEffect *pEff = pMapEff->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vScale( fCurScaleRate, fCurScaleRate, fCurScaleRate );
				pEff->GetParentNode()->SetScale( vScale );
			}
		}
	}
}

void ioChargingFireSkill4::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pChar = pWeapon->GetOwner();
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );

	D3DXVECTOR3 vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
	if( vAimedDir != ioMath::VEC3_ZERO )
		pWeapon->SetMoveDir( vAimedDir );

	if( pZone )
	{
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}

void ioChargingFireSkill4::CheckCharRotateKey( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() )
		return;

	if( pChar->IsSettedDirection() && pChar->IsChangeDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		pChar->SetTargetRotToDirKey( eNewDirKey );

		SetAimState( pChar, false );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_DIR;
			kPacket << (int)eNewDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioChargingFireSkill4::SetFailState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	if( m_State == SS_FAIL )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_FAIL;

	//DestroyMapEffect
	if( m_dwChargingEffect != -1 )
		pChar->EndMapEffect( m_dwChargingEffect, false );

	m_dwChargingEffect = -1;

	int iAniID = pGrp->GetAnimationIdx( m_SkillFailMotion );
	float fAniRate = m_fSkillFailMotionRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = 0;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( iAniID != -1 )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	m_LoopAimState = LAS_NONE;

	m_bSetAimedTarget = false;
	m_szTargetName.Clear();
	m_dwAimStartTime = 0;

	// set marker none...
	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE, TT_OTHER );
}

void ioChargingFireSkill4::SetAimState( ioBaseChar *pChar, bool bSend )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_LoopAimState = LAS_WAIT;
	m_dwAimStartTime = dwCurTime;
	m_szTargetName.Clear();
	m_bSetAimedTarget = false;
	m_vAimedPos = ioMath::VEC3_ZERO;

	// set marker
	pChar->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
							m_szReadyMarker,
							m_szAimingMarker,
							m_szAimedMarker,
							m_szZeroAimedMarker,
							"",
							TT_OTHER );

	pChar->SetTargetMarkerRange( m_fMarkerRange, TT_OTHER );
	pChar->SetTargetMarkerState( ioTargetMarker::MS_READY, TT_OTHER );

	// 빈타겟 설정...
	pChar->SetTargetMarkerTarget( m_szTargetName, false, TT_OTHER );

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LOOP;
		kPacket << m_LoopAimState;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargingFireSkill4::SetAimedState( ioBaseChar *pChar, const ioHashString &szTarget )
{
	if( !pChar ) return;

	m_LoopAimState = LAS_AIMED;
	m_szTargetName = szTarget;

	// change marker state...
	pChar->SetTargetMarkerTarget( m_szTargetName, false, TT_OTHER );
	pChar->SetTargetMarkerState( ioTargetMarker::MS_AIMED, TT_OTHER );

	if( CheckTargetValidate( pChar ) )
	{
		TarckingTarget( pChar );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LOOP;
		kPacket << m_LoopAimState;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargingFireSkill4::ProcessAimState(  ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	switch( m_LoopAimState )
	{
	case LAS_WAIT:
		CheckAimedState( pChar, pStage );
		break;
	case LAS_AIMED:
		if( pChar->IsNeedProcess() && !CheckTargetValidate( pChar ) )
		{
			SetAimState( pChar, true );
			return;
		}

		TarckingTarget( pChar );
		break;
	}
}

void ioChargingFireSkill4::TarckingTarget( ioBaseChar *pChar )
{
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget ) return;

	m_bSetAimedTarget = true;

	if( CheckDownAimState( pTarget ) )
		m_vAimedPos = pTarget->GetMidPositionByRate( m_fDownTargetAimRate );
	else
		m_vAimedPos = pTarget->GetMidPositionByRate( m_fTargetAimRate );

	pChar->SetTargetRotToTargetPos( m_vAimedPos, true, false );
}

bool ioChargingFireSkill4::CheckTargetValidate( ioBaseChar *pChar )
{
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )
		return false;

	if( pTarget->HasHideBuff() )
		return false;

	if( pTarget->IsChatModeState( false ) )
		return false;

	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	D3DXVECTOR3 vDiff = vTargetPos - pChar->GetWorldPosition();

	if( D3DXVec3LengthSq( &vDiff ) > m_fAimMaxRange * m_fAimMaxRange )
		return false;

	if( D3DXVec3LengthSq( &vDiff ) < m_fAimMinRange * m_fAimMinRange )
		return false;

	vDiff.y = 0.0f;
	D3DXVec3Normalize( &vDiff, &vDiff );

	D3DXVECTOR3 vCharDir;
	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	vCharDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
	if( D3DXVec3Dot( &vCharDir, &vDiff ) < fHalfCosine )
		return false;

	return true;
}

void ioChargingFireSkill4::CheckAimedState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() )
		return;

	if( m_dwAimStartTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwAimStartTime + m_dwAimTime < dwCurTime )
	{
		ioHashString szTarget;
		if( pChar->CheckAimedTarget( m_fAimAngle, m_fAimMinRange, m_fAimMaxRange, szTarget, m_bEnableDownAim ) )
		{
			if( !szTarget.IsEmpty() )
				SetAimedState( pChar, szTarget );

			return;
		}
	}
}

D3DXVECTOR3 ioChargingFireSkill4::ReCalcMoveDir( ioBaseChar *pChar, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )
		return ioMath::VEC3_ZERO;

	if( m_vAimedPos == ioMath::VEC3_ZERO )
		return ioMath::VEC3_ZERO;

	if( m_bSetAimedTarget )
	{
		if( CheckDownAimState( pTarget ) )
			m_vAimedPos = pTarget->GetMidPositionByRate( m_fDownTargetAimRate );
		else
			m_vAimedPos = pTarget->GetMidPositionByRate( m_fTargetAimRate );

		m_bSetAimedTarget = false;
	}

	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();
	D3DXVECTOR3 vAimedDir =  m_vAimedPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

bool ioChargingFireSkill4::CheckDownAimState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() && m_bEnableDownAim )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() && m_bEnableDownAim )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() && m_bEnableDownAim )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) && m_bEnableDownAim )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) && m_bEnableDownAim )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() && m_bEnableDownAim )
		return true;

	return false;
}

