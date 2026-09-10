
#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingSwing2Skill.h"

#include "ioGrowthUpInfo.h"

ioGrapplingSwing2Skill::ioGrapplingSwing2Skill()
{
	ClearData();
}

ioGrapplingSwing2Skill::ioGrapplingSwing2Skill( const ioGrapplingSwing2Skill &rhs )
	: ioAttackSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_SkillGrapLoopMotion( rhs.m_SkillGrapLoopMotion ),
	m_fSkillGrapLoopMotionRate( rhs.m_fSkillGrapLoopMotionRate ),
	m_SkillSwingMotion( rhs.m_SkillSwingMotion ),
	m_fSkillSwingMotionRate( rhs.m_fSkillSwingMotionRate ),
	m_SkillSwingedMotion( rhs.m_SkillSwingedMotion ),
	m_fSkillSwingedMotionRate( rhs.m_fSkillSwingedMotionRate ),
	m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
	m_dwLoopRotateTime( rhs.m_dwLoopRotateTime ),
	m_dwEnableSwingTime( rhs.m_dwEnableSwingTime ),
	m_bReverseLoopRotate( rhs.m_bReverseLoopRotate ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_bEnableSwingMove( rhs.m_bEnableSwingMove ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_dwMoveRotateTime( rhs.m_dwMoveRotateTime ),
	m_fCharHeightRate( rhs.m_fCharHeightRate ),
	m_SkillLoopType( rhs.m_SkillLoopType ),
	m_LoopDelayAnimation( rhs.m_LoopDelayAnimation ),
	m_fLoopDelayAnimationRate( rhs.m_fLoopDelayAnimationRate ),
	m_LoopMoveAnimation( rhs.m_LoopMoveAnimation ),
	m_fLoopMoveAnimationRate( rhs.m_fLoopMoveAnimationRate ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_SkillGrapplingTeamInfo( rhs.m_SkillGrapplingTeamInfo ),
	m_SkillGrapplingEnemyInfo( rhs.m_SkillGrapplingEnemyInfo )
{
	ClearData();
}

ioGrapplingSwing2Skill::~ioGrapplingSwing2Skill()
{
}

void ioGrapplingSwing2Skill::ClearData()
{
	m_SkillState = SS_NONE;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vCurMoveDir = ioMath::VEC3_ZERO;

	m_bSetHandMesh = false;

	m_dwLoopStartTime = 0;
	m_bSetMoveAni = false;

	// 성장관련 요소
	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;
	
	m_SkillGrapLoopStartTime = 0;
}

void ioGrapplingSwing2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bDisableGrowth= rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic	= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime		= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	rkLoader.LoadString_e( "skill_grap_loop_motion", "", szBuf, MAX_PATH );
	m_SkillGrapLoopMotion = szBuf;
	m_fSkillGrapLoopMotionRate = rkLoader.LoadFloat_e( "skill_grap_loop_motion_rate", FLOAT1 );

	m_fTargetLoopGapRange = rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );

	rkLoader.LoadString_e( "skill_swing_motion", "", szBuf, MAX_PATH );
	m_SkillSwingMotion = szBuf;
	m_fSkillSwingMotionRate = rkLoader.LoadFloat_e( "skill_swing_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "skill_swinged_motion", "", szBuf, MAX_PATH );
	m_SkillSwingedMotion = szBuf;
	m_fSkillSwingedMotionRate = rkLoader.LoadFloat_e( "skill_swinged_motion_rate", FLOAT1 );

	m_SkillLoopType = (SkillLoopType)rkLoader.LoadInt_e( "skill_loop_type", SLT_NORMAL );
	m_dwLoopRotateTime = rkLoader.LoadInt_e( "skill_loop_rotate_speed", 0 );
	m_dwEnableSwingTime = rkLoader.LoadInt_e( "skill_loop_swing_enable_time", 0 );

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_bReverseLoopRotate = rkLoader.LoadBool_e( "reverse_loop_rotate", false );

	m_fCharHeightRate = rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );

	m_bEnableSwingMove = rkLoader.LoadBool_e( "enable_swing_move", false );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "swing_move_speed", 0.0f );
	m_dwMoveRotateTime = (DWORD)rkLoader.LoadInt_e( "swing_move_rotate_speed", 0 );

	int i;
	int iBuffCnt = rkLoader.LoadInt_e( "swing_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_wound_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_wound_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingWoundBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_cancel_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingCancelBuff.push_back( szBuf );
	}

	//loop motion
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );	
	m_LoopDelayAnimation = szBuf;
	m_fLoopDelayAnimationRate = rkLoader.LoadFloat_e( "delay_animation_rate", FLOAT1 );
	
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_LoopMoveAnimation	= szBuf;
	m_fLoopMoveAnimationRate = rkLoader.LoadFloat_e( "move_animation_rate", FLOAT1 );
	m_dwLoopDuration = rkLoader.LoadInt_e( "move_duration", 0 );

	iBuffCnt = rkLoader.LoadInt_e( "swing_team_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_team_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingTeamInfo.m_SwingBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_enemy_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_enemy_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingEnemyInfo.m_SwingBuff.push_back( szBuf );
	}
}

ioSkill* ioGrapplingSwing2Skill::Clone()
{
	return new ioGrapplingSwing2Skill( *this );
}

bool ioGrapplingSwing2Skill::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();
	m_SkillState = SS_NONE;
	m_GrapplingTargetName.Clear();

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;
	m_dwSkillStartTime = m_dwCheckTime = FRAMEGETTIME();
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pChar )
	{
		m_vCurMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		ioPlayStage *pStage = pChar->GetCreator();
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );


			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);			
		}
	}

	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
	
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		SetLoopState( pChar );
	}	
	return true;
}

void ioGrapplingSwing2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float	fCurTicGauge	= m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int		iTickGap		= (int)( dwCurTime - m_dwCheckTime );
	int		iCurTicTime		= m_dwTicTime + m_dwTicTimeByGrowth;

	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetLoopState( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckNormalMoveAni( pChar );
			if( m_dwLoopStartTime > 0 && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
			}

			if ( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				AttackSkillFire( pChar );
			}			
		}
		break;
	case SS_ACTION:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
				SetLoopState( pChar );
			}
		}
		break;
	case SS_GRAP_LOOP:
		{
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_SkillGrapLoopStartTime + g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
					SetSkillEndState( pChar, true );
					return;
				}
			}
			
			// 액션키 체크
			if( m_SkillGrapLoopStartTime + m_dwEnableSwingTime < dwCurTime )
			{
				if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
				{
					pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
					SetSwingState( pChar, true );
					return;
				}
			}

			if( m_bEnableSwingMove )
				CheckSwingMove( pChar );

			if( m_SkillLoopType == SLT_NORMAL )	
				ProcessLoopDir( pChar );
		}
		break;
	case SS_SWING:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			{
				pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
				SetLoopState( pChar );
			}
		}
		break;
	case SS_END:
		break;
	}

	if( m_SkillState != SS_END )
	{
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );
			
			if( fCurTicGauge != 0.0f )
			{
				// 정조준 발사중에는 종료되지 않도록 예외를 둠.
				if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
				{
					if( m_SkillState != SS_END && m_SkillState != SS_GRAP_LOOP )
					{
						SetEndState( pChar );
						return;
					}
					if ( m_SkillState == SS_GRAP_LOOP )
					{
						if ( m_SkillGrapLoopStartTime + m_dwEnableSwingTime < dwCurTime )
						{
							SetSkillEndState( pChar, false );
							return;
						}
					}
				}
			}
		}
	}
}

void ioGrapplingSwing2Skill::CheckSwingMove( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	CheckKeyInput( pChar );
	ProcessRotate( pChar );

	pChar->SetCurMoveSpeed( m_fMoveSpeed );
}

void ioGrapplingSwing2Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	pChar->SetSkillMoveEnable( false, false );
	m_SkillState = SS_NONE;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_vCurMoveDir = ioMath::VEC3_ZERO;

	ClearData();
}

void ioGrapplingSwing2Skill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	m_SkillState = SS_ACTION;

	if( !pChar ) return;
	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage ) return;

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	pChar->SetSkillMoveEnable( false, false );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ATTACK;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingSwing2Skill::SetGrapLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_ACTION )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillGrapLoopMotionRate > 0.0f )
		fAniRate = m_fSkillGrapLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillGrapLoopMotion );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_SkillState = SS_GRAP_LOOP;
	m_SkillGrapLoopStartTime = FRAMEGETTIME();
}

void ioGrapplingSwing2Skill::SetSkillEndState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillSwingMotionRate > 0.0f )
		fAniRate = m_fSkillSwingMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillSwingMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	m_SkillState = SS_END;

	if( bSendNet && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}


	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return;
	CheckTeam( pChar, pTarget );
	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL && pTarget->IsNeedProcess() )
		{
			pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
			pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(),
				m_SkillSwingedMotion, m_fSkillSwingedMotionRate );
		}
	}
}


void ioGrapplingSwing2Skill::SetSwingState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillSwingMotionRate > 0.0f )
		fAniRate = m_fSkillSwingMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillSwingMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	m_SkillState = SS_SWING;

	if( bSendNet && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_SWING;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}


	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return;
	CheckTeam( pChar, pTarget );
	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL && pTarget->IsNeedProcess() )
		{
			pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
			pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(),
				m_SkillSwingedMotion, m_fSkillSwingedMotionRate );
		}
	}
}

void ioGrapplingSwing2Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_NO_CHANGE && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_NO_CHANGE2 && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_EXCEPT && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
}

bool ioGrapplingSwing2Skill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_SkillState != SS_ACTION )		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;

	SetGrapLoopState( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_GRAP_LOOP;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioGrapplingSwing2Skill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetLoopGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fTargetLoopGapRange > 0.0f )
			vPos += m_fTargetLoopGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioGrapplingSwing2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
	case SS_ACTION:
	case SS_GRAP_LOOP:
		return false;
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		return false;
	}

	return false;
}

void ioGrapplingSwing2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pChar->SetTargetRotToRotate( qtRot, true, false );

	switch( iState )
	{
	case SSC_GRAP_LOOP:
		{
			ioHashString szTargetName;
			rkPacket >> m_GrapplingTargetName;

			SetGrapLoopState( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetTargetRotToRotate( qtRot, true, false );
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_ATTACK:
		{
			AttackSkillFire( pChar );
		}
		break;
	case SSC_SWING:
		{
			if( m_SkillState != SS_END )
			{
				SetSwingState( pChar, false );

				ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
				if( pTarget && pTarget->IsNeedProcess() && pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
				{
					pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
					pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(),
						m_SkillSwingedMotion, m_fSkillSwingedMotionRate );
				}
			}
		}
		break;
	case SSC_END:
		{
			if( m_SkillState != SS_END )
			{
				SetSkillEndState( pChar, false );

				ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
				if( pTarget && pTarget->IsNeedProcess() && pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
				{
					pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
					pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), GetSkillWoundGrapplingInfo(),
						m_SkillSwingedMotion, m_fSkillSwingedMotionRate );
				}
			}
		}
		break;
	case SSC_KEY:
		{
			int iDir;
			rkPacket >> iDir;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

			rkPacket >> m_vCurMoveDir;

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -m_vCurMoveDir );
			pChar->SetMoveDirByRotate( qtNewRot );
		}
		break;
	}

}

void ioGrapplingSwing2Skill::ProcessLoopDir( ioBaseChar *pChar )
{
	if( m_dwLoopRotateTime == 0 )
		return;

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_dwLoopRotateTime;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXVECTOR3 vAxis = ioMath::UNIT_Y;

	if( m_bReverseLoopRotate )
		vAxis = -ioMath::UNIT_Y;

	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle));

	D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pChar->SetTargetRotToRotate( qtNewRot, true, true );
}

bool ioGrapplingSwing2Skill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	switch( m_SkillState )
	{
	case SS_ACTION:
		return false;
	case SS_LOOP:
	case SS_GRAP_LOOP:
	case SS_END:
		if( m_dwSkillStartTime + m_dwSkillProtectTime < dwCurTime )
			return false;

		return true;
	}

	return false;
}

bool ioGrapplingSwing2Skill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_END:
		if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
			return false;

		if( m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

void ioGrapplingSwing2Skill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;
	if( m_dwMoveRotateTime <= 0 ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_KEY;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			kPacket << m_vCurMoveDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGrapplingSwing2Skill::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwMoveRotateTime <= 0 ) return;

	if( m_vCurMoveDir == ioMath::VEC3_ZERO )
		return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwMoveRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXVECTOR3 vCurMoveDir = m_vCurMoveDir;
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

	if( fDotValue >= FLOAT1 )
	{
		m_vCurMoveDir = vTargetDir;

		switch( m_SkillLoopType )
		{
		case SLT_NORMAL:
			pOwner->SetMoveDirByRotate( qtRotate );
			break;
		case SLT_STOP:
			pOwner->SetTargetRotToRotate( qtRotate, true );
			break;
		}

		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -FLOAT1 )
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

	m_vCurMoveDir = vNewDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );

	switch( m_SkillLoopType )
	{
	case SLT_NORMAL:
		pOwner->SetMoveDirByRotate( qtNewRot );
		break;
	case SLT_STOP:
		pOwner->SetTargetRotToRotate( qtNewRot, true );
		break;
	}	
}

bool ioGrapplingSwing2Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioGrapplingSwing2Skill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar )	return;

	m_SkillState = SS_LOOP;
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = FLOAT1;
	if( m_fLoopDelayAnimationRate > 0.0f )
		fAniRate = m_fLoopDelayAnimationRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iCurSkillAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	pChar->SetSkillMoveEnable( true, true );
	m_GrapplingTargetName.Clear();
	m_bSetMoveAni = false;
}

void ioGrapplingSwing2Skill::CheckNormalMoveAni( ioBaseChar* pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{	
		if( !m_bSetMoveAni )
		{	
			iAniID = pGrp->GetAnimationIdx( m_LoopMoveAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );	
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = false;
		}
	}
}

void ioGrapplingSwing2Skill::SetEndState( ioBaseChar *pChar )
{
	if( m_SkillState == SS_END )	return;

	m_SkillState = SS_END;
}

void ioGrapplingSwing2Skill::CheckTeam( ioBaseChar* pChar, ioBaseChar* pTarget )
{
	if ( !pChar || !pTarget ) return;
	if ( pChar->GetCharName() == pTarget->GetCharName() )
		return;

	if ( pChar->GetTeam() == pTarget->GetTeam() )
	{
		m_SkillGrapplingInfo.m_SwingBuff = m_SkillGrapplingTeamInfo.m_SwingBuff;
	}

	if ( pChar->GetTeam() != pTarget->GetTeam() )
	{
		m_SkillGrapplingInfo.m_SwingBuff = m_SkillGrapplingEnemyInfo.m_SwingBuff;
	}
}
