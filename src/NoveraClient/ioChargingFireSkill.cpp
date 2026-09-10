

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioChargingFireSkill.h"

#include "WeaponDefine.h"

ioChargingFireSkill::ioChargingFireSkill()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;
	m_dwChargingEffect = -1;

	m_bSetHandMesh = false;

	m_dwPreActionEndTime = 0;
	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwFireStartTime = 0;

	m_fCurChargeRate = 0.0f;

	m_vCharMoveDir = ioMath::VEC3_ZERO;
}

ioChargingFireSkill::ioChargingFireSkill( const ioChargingFireSkill &rhs )
: ioNormalSkill( rhs ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_TargetCircleEffect( rhs.m_TargetCircleEffect ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_fCharMoveSpeed( rhs.m_fCharMoveSpeed ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_dwMaxChargingTime( rhs.m_dwMaxChargingTime ),
 m_SkillChargingEffect( rhs.m_SkillChargingEffect ),
 m_dwLoopDuration( rhs.m_dwLoopDuration )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;
	m_dwChargingEffect = -1;

	m_bSetHandMesh = false;

	m_dwPreActionEndTime = 0;
	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwFireStartTime = 0;

	m_fCurChargeRate = 0.0f;

	m_vCharMoveDir = ioMath::VEC3_ZERO;
}

ioChargingFireSkill::~ioChargingFireSkill()
{
}

void ioChargingFireSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "target_circle_effect", "", szBuf, MAX_PATH );
	m_TargetCircleEffect = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_fCharMoveSpeed = rkLoader.LoadFloat_e( "skill_move_speed", 0.0f );
	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

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
}

ioSkill* ioChargingFireSkill::Clone()
{
	return new ioChargingFireSkill( *this );
}

bool ioChargingFireSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwPreActionEndTime = 0;
	m_dwLoopStartTime = 0;
	m_dwAttackEndTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;

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

	CheckCharMoveDir( pChar );

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetPreActionState( pChar );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioChargingFireSkill::OnSkillEnd( ioBaseChar *pChar )
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

	pChar->SetMagicCircleTarget( "" );

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	if( m_dwTargetCircleEffect != -1 )
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );

	if( m_dwChargingEffect != -1 )
	{
		pChar->EndMapEffect( m_dwChargingEffect, false );
	}

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;
	m_dwChargingEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_fCurChargeRate = 0.0f;
	m_dwFireStartTime = 0;

	m_State = SS_NONE;
}

void ioChargingFireSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				SetPreActionState( pChar );
			}
		}
		break;
	case SS_PRE_ACTION:
		{
			if( m_dwPreActionEndTime > 0 && m_dwPreActionEndTime < dwCurTime )
			{
				SetLoopState( pChar );
				return;
			}

			ProcessPreAction( pChar, pStage );
		}
		break;
	case SS_LOOP:
		{
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

			ProcessLoopState( pChar, pStage );

			if( m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
			{
				SetActionState( pChar, pStage, false );
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
		if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
		{
			if( m_dwChargingEffect != -1 )
			{
				pChar->EndMapEffect( m_dwChargingEffect, false );
				m_dwChargingEffect = -1;
			}

			m_dwFireStartTime = 0;
		}

		if( m_dwChargingEffect != -1 )
		{
			pChar->UpdateAttachMapEffect( m_dwChargingEffect );
		}
		break;
	}
}

bool ioChargingFireSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioChargingFireSkill::SetPreActionState( ioBaseChar *pChar )
{
	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar );

	m_State = SS_PRE_ACTION;
	pChar->SetJumpPower( m_fCharJumpPower );

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioChargingFireSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_dwPreActionEndTime = dwCurTime;
	m_dwPreActionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	pChar->AttachEffect( m_StartEffect );
}

void ioChargingFireSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

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

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}

	if( !m_TargetCircleEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_TargetCircleEffect, vPos );

		if( pMapEffect )
		{
			if( m_dwTargetCircleEffect != -1 )
				pChar->EndMapEffect( m_dwTargetCircleEffect, false );

			m_dwTargetCircleEffect = pMapEffect->GetUniqueID();
		}
	}

	if( m_dwChargingEffect != -1 )
	{
		pChar->EndEffect( m_dwChargingEffect, false );
	}

	ioMapEffect *pChargingEff = pChar->CreateMapEffectAttachChar( m_SkillChargingEffect );
	if( pChargingEff )
	{
		m_dwChargingEffect = pChargingEff->GetUniqueID();
	}

	/*
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LOOP;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	*/
}

void ioChargingFireSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	if( m_State == SS_ACTION )
		return;

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

	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	float fAniRate = m_AttackAttribute.m_fAttackAniRate;
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

	float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
	if( fReserveTime > 0.0f )
	{
		m_dwEnableReserveTime = dwCurTime + fReserveTime +dwPreDelay;
	}

	float fFireStart = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;
	if( fFireStart > 0.0f )
	{
		m_dwFireStartTime = dwCurTime + fFireStart + dwPreDelay;
	}

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fAniRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd + dwPreDelay;
	else
		m_dwAttackEndTime = 0;

	pChar->AttachEffect( m_EndEffect );

	//
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, true, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	//
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		}
		else
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
		}
	}

	if( pMapEffect )
	{
		if( m_dwCurMapEffect != -1 )
			pChar->EndMapEffect( m_dwCurMapEffect, false );

		m_dwCurMapEffect = pMapEffect->GetUniqueID();

		if( m_dwTargetCircleEffect != -1 )
		{
			pChar->EndMapEffect( m_dwTargetCircleEffect, false );
			m_dwTargetCircleEffect = -1;
		}
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

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

void ioChargingFireSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_LOOP:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos >> qtRot;
			pChar->SetWorldPosition( vPos );
			pChar->SetTargetRotToRotate( qtRot, true, false );

			SetLoopState( pChar );
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
	}
}

bool ioChargingFireSkill::IsProtected( int iDefenseBreakType ) const
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
	case SS_PRE_ACTION:
	case SS_LOOP:
		return false;
	case SS_ACTION:
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioChargingFireSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioChargingFireSkill::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioChargingFireSkill::IsNoDropState() const
{
	if( m_State == SS_LOOP || m_State == SS_ACTION )
		return true;

	return false;
}

bool ioChargingFireSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioChargingFireSkill::ProcessLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( m_dwChargingEffect == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pChar->UpdateAttachMapEffect( m_dwChargingEffect );

	// Charging Rate
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

void ioChargingFireSkill::ProcessPreAction( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	if( m_fCharMoveSpeed == 0.0f ) return;
	if( m_vCharMoveDir == ioMath::VEC3_ZERO ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = m_vCharMoveDir * m_fCharMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			pChar->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );
}

void ioChargingFireSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pChar = pWeapon->GetOwner();

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}

	ioBombMissileWeapon *pBombM = ToBombMissileWeapon( pWeapon );
	if( pBombM && pChar )
	{
		D3DXVECTOR3 vDiff = m_vCirclePos - pChar->GetMidPositionByRate();
		D3DXVec3Normalize( &vDiff, &vDiff );

		pBombM->SetMoveDir( vDiff );
	}
}

void ioChargingFireSkill::CheckCharMoveDir( ioBaseChar *pChar )
{
	if( !pChar ) return;

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	if( pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pChar->CheckCurDirKey();

		float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pChar->SetTargetRotToRotate( qtRotate, true, false );

		m_vCharMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}
}






