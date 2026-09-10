

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioCircleMoveAttackSkill3.h"

#include "WeaponDefine.h"

ioCircleMoveAttackSkill3::ioCircleMoveAttackSkill3()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;

	m_dwPreActionEndTime = 0;
	m_dwWaitStartTime = 0;

	m_fCurCharMoveSpeed = 0.0f;
}

ioCircleMoveAttackSkill3::ioCircleMoveAttackSkill3( const ioCircleMoveAttackSkill3 &rhs )
: ioNormalSkill( rhs ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_TargetCircleEffect( rhs.m_TargetCircleEffect ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_WaitAni( rhs.m_WaitAni ),
 m_dwWaitDuration( rhs.m_dwWaitDuration ),
 m_dwWaitProtectionTime( rhs.m_dwWaitProtectionTime ),
 m_ActionLoopAni( rhs.m_ActionLoopAni ),
 m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
 m_ExtraAttackAttribute( rhs.m_ExtraAttackAttribute )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;

	m_dwPreActionEndTime = 0;
	m_dwWaitStartTime = 0;

	m_fCurCharMoveSpeed = 0.0f;
}

ioCircleMoveAttackSkill3::~ioCircleMoveAttackSkill3()
{
}

void ioCircleMoveAttackSkill3::LoadProperty( ioINILoader &rkLoader )
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

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	// wait
	rkLoader.LoadString_e( "wait_ani", "", szBuf, MAX_PATH );
	m_WaitAni = szBuf;

	m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "wait_duration", 0 );
	m_dwWaitProtectionTime = (DWORD)rkLoader.LoadInt_e( "wait_protection_time", 0 );

	// action loop
	rkLoader.LoadString_e( "action_loop_ani", "", szBuf, MAX_PATH );
	m_ActionLoopAni = szBuf;

	// Action Attack
	m_LandAttackAttribute.Init();
	LoadAttackAttribute_e( "action_attack", m_LandAttackAttribute, rkLoader );

	// ExtraAction Attack
	m_ExtraAttackAttribute.Init();
	LoadAttackAttribute_e( "extra_action_attack", m_ExtraAttackAttribute, rkLoader );
}

ioSkill* ioCircleMoveAttackSkill3::Clone()
{
	return new ioCircleMoveAttackSkill3( *this );
}

bool ioCircleMoveAttackSkill3::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_State = SS_NONE;
	m_bReduceGauge = false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_dwWaitStartTime = 0;
	m_fCurChargeRate = 0.0f;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;

	m_dwPreActionEndTime = 0;
	m_dwWaitStartTime = 0;

	m_fCurCharMoveSpeed = 0.0f;

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
		SetPreAction( pChar );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioCircleMoveAttackSkill3::OnSkillEnd( ioBaseChar *pChar )
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

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwWaitStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;
}

void ioCircleMoveAttackSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	int iSlot = GetOwnerItem()->GetType() - 1;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetPreAction( pChar );
			}
		}
		break;
	case SS_PRE_ACTION:
		if( m_dwPreActionEndTime < dwCurTime )
		{
			SetWaitState( pChar );
		}
		break;
	case SS_WAIT:
		if( m_dwWaitStartTime + m_dwWaitDuration >= dwCurTime )
		{
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
			pChar->UpdateMagicCirclePosAll2( m_fCircleMoveSpeed, m_fCircleRange, vCharPos.y, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() ||
					pChar->IsDefenseKey() ||
					pChar->IsJumpKey() ||
					pChar->GetSkillKeyInput() == iSlot )
				{
					SetActionLoopState( pChar );
					return;
				}
			}
		}
		break;
	case SS_ACTION_LOOP:
		{
			if( CheckLanding( pChar ) )
				SetActionState( pChar );
			else
				ProcessMove( pChar, pStage );
		}
		break;
	case SS_ACTION:
		if( m_dwEnableReserveTime < dwCurTime )
		{
			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetExtraActionState( pChar );
				return;
			}
		}
		break;
	case SS_EXTRA_ACTION:
		{
		}
		break;
	}
}

bool ioCircleMoveAttackSkill3::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCircleMoveAttackSkill3::SetNormalSkill( ioBaseChar *pChar )
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

	pChar->SetJumpPower( m_fCharJumpPower );

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwPreActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_State = SS_PRE_ACTION;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioCircleMoveAttackSkill3::SetPreAction( ioBaseChar *pChar )
{
	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar );
}

void ioCircleMoveAttackSkill3::SetWaitState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwWaitStartTime = dwCurTime;
	m_State = SS_WAIT;

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_WaitAni );
	float fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( 5.0f );
	pGrp->ClearAllActionAni( 5.0f );
	pGrp->SetLoopAni( iCurSkillAniID, 5.0f );

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
}

void ioCircleMoveAttackSkill3::SetActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	if( m_dwTargetCircleEffect != -1 )
	{
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );
		m_dwTargetCircleEffect = -1;
	}

	pChar->DestroyMagicCircle();

	m_State = SS_ACTION;

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_LandAttackAttribute.m_AttackAnimation );

	float fTimeRate = m_LandAttackAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( 5.0f );
	pGrp->ClearAllActionAni( 5.0f );
	pGrp->SetActionAni( iCurSkillAniID, 5.0f, 5.0f, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iCurSkillAniID,
										m_LandAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										m_LandAttackAttribute.m_dwPreDelay );

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" );

	pChar->CheckCharColSkipSkillTime( iCurSkillAniID, fTimeRate, m_LandAttackAttribute.m_dwPreDelay );
	pChar->SetReservedSliding( m_LandAttackAttribute.m_vForceInfoList, iCurSkillAniID, fTimeRate, m_LandAttackAttribute.m_dwPreDelay );
}

void ioCircleMoveAttackSkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SS_ACTION_LOOP:
		SetActionLoopState( pChar );
		break;
	case SS_EXTRA_ACTION:
		SetExtraActionState( pChar );
		break;
	}
}

bool ioCircleMoveAttackSkill3::IsEndState() const
{
	if( m_State == SS_WAIT )
	{
		if( m_dwWaitStartTime+m_dwWaitDuration < FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioCircleMoveAttackSkill3::IsProtected( int iDefenseBreakType ) const
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

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		break;
	case SS_PRE_ACTION:
		return true;
	case SS_WAIT:
	case SS_ACTION_LOOP:
		if( m_dwWaitStartTime + m_dwWaitProtectionTime > dwCurTime )
			return true;
		break;
	case SS_ACTION:
	case SS_EXTRA_ACTION:
		if( m_dwFireMotionEndTime > dwCurTime )
			return true;
		break;
	}

	return false;
}

bool ioCircleMoveAttackSkill3::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
	case SS_PRE_ACTION:
	case SS_ACTION_LOOP:
	case SS_WAIT:
		return false;
	case SS_ACTION:
	case SS_EXTRA_ACTION:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
			return true;

		break;
	}
	
	return false;
}

bool ioCircleMoveAttackSkill3::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioCircleMoveAttackSkill3::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();
	
	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}

void ioCircleMoveAttackSkill3::ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	if( m_fCurCharMoveSpeed == 0.0f ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = m_vCharMoveDir * m_fCurCharMoveSpeed * fDivideTimePerSec;
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

void ioCircleMoveAttackSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}

float ioCircleMoveAttackSkill3::GetFlightDuration( const D3DXVECTOR3 &vCharPos, float fCurGravity )
{
	float fGapHeight = fabsf(vCharPos.y-m_vCirclePos.y);
	float fGapTime = 0.0f;

	float fNewGravity = fCurGravity * GetSkillGravityRate();
	if( fNewGravity > 0.0f )
	{
		fGapTime = sqrt( 2.0f*fGapHeight/fNewGravity );
	}

	return fGapTime;
}

SkillCharColType ioCircleMoveAttackSkill3::IsCharSkipState() const
{
	if( m_EnableCharCollisionSkip == SCCT_ALL )
	{
		switch( m_State )
		{
		case SS_ACTION_LOOP:
			return SCCT_ALL;
		default:
			return SCCT_NONE;
		}
	}

	return m_EnableCharCollisionSkip;
}

void ioCircleMoveAttackSkill3::SetActionLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	m_State = SS_ACTION_LOOP;

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_ActionLoopAni );
	float fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( 5.0f );
	pGrp->ClearAllActionAni( 5.0f );
	pGrp->SetLoopAni( iCurSkillAniID, 5.0f );

	float fCurRange = m_fCircleRange;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

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

	// CharMove
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	float fCurGravity = pChar->GetCharGravity();
	float fFlightDuration = GetFlightDuration( vCharPos, fCurGravity );

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vDir = m_vCirclePos - vCharPos;
	vDir.y = 0.0f;

	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		m_fCurCharMoveSpeed = 0.0f;
		if( fFlightDuration > 0.0f )
		{
			m_fCurCharMoveSpeed = fRange / fFlightDuration;
		}

		D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
		pChar->SetTargetRotAndMoveDirChange( qtRot, false );
		pChar->SetWorldOrientation( qtRot );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_ACTION_LOOP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMoveAttackSkill3::SetExtraActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_EXTRA_ACTION;

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	pChar->ClearSkillReserveKey();
	
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_ExtraAttackAttribute.m_AttackAnimation );

	float fTimeRate = m_ExtraAttackAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( 5.0f );
	pGrp->ClearAllActionAni( 5.0f );
	pGrp->SetActionAni( iCurSkillAniID, 5.0f, 5.0f, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iCurSkillAniID,
										m_ExtraAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										m_ExtraAttackAttribute.m_dwPreDelay );

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" );

	pChar->CheckCharColSkipSkillTime( iCurSkillAniID, fTimeRate, m_ExtraAttackAttribute.m_dwPreDelay );
	pChar->SetReservedSliding( m_ExtraAttackAttribute.m_vForceInfoList, iCurSkillAniID, fTimeRate, m_ExtraAttackAttribute.m_dwPreDelay );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_EXTRA_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioCircleMoveAttackSkill3::IsNoDropState() const
{
	if( m_State == SS_WAIT )
		return true;

	return false;
}

float ioCircleMoveAttackSkill3::GetSkillGravityRate()
{
	if( m_State != SS_ACTION_LOOP )
		return FLOAT1;

	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_fSkillGravityRate;
	case SUT_DOWN:
		if( m_fSkillGravityDownRate > 0.0f )
			return m_fSkillGravityDownRate;
		return m_fSkillGravityRate;
	case SUT_JUMP:
		if( m_fSkillGravityJumpRate > 0.0f )
			return m_fSkillGravityJumpRate;
		return m_fSkillGravityRate;
	}

	return m_fSkillGravityRate;
}

bool ioCircleMoveAttackSkill3::IsEnableReserve() const
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
	case SS_PRE_ACTION:
	case SS_WAIT:
	case SS_ACTION_LOOP:
		return false;
	}

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < dwCurTime )
		return true;

	return false;
}


