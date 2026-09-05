

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioCircleMoveAttackSkill2.h"

#include "WeaponDefine.h"

ioCircleMoveAttackSkill2::ioCircleMoveAttackSkill2()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_dwWaitStartTime = 0;
	m_dwCurSkillProtectTime = 0;
	m_iCurLoopCnt = 0;
}

ioCircleMoveAttackSkill2::ioCircleMoveAttackSkill2( const ioCircleMoveAttackSkill2 &rhs )
: ioNormalSkill( rhs ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_TargetCircleEffect( rhs.m_TargetCircleEffect ),
 m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_fEndMotionRate( rhs.m_fEndMotionRate ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
 m_dwWaitDuration( rhs.m_dwWaitDuration ),
 m_iMaxLoopCnt( rhs.m_iMaxLoopCnt ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_szWaitMotion( rhs.m_szWaitMotion ),
 m_fWaitMotionRate( rhs.m_fWaitMotionRate ),
 m_OwnerBuffList( rhs.m_OwnerBuffList )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_dwWaitStartTime = 0;
	m_dwCurSkillProtectTime = 0;
	m_iCurLoopCnt = 0;
}

ioCircleMoveAttackSkill2::~ioCircleMoveAttackSkill2()
{
}

void ioCircleMoveAttackSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

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

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	// wait
	m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "wait_duration", 0 );
	rkLoader.LoadString_e( "wait_motion", "", szBuf, MAX_PATH );
	m_szWaitMotion = szBuf;
	m_fWaitMotionRate = rkLoader.LoadFloat_e( "wait_motion_rate", FLOAT1 );

	//
	m_iMaxLoopCnt = rkLoader.LoadInt_e( "max_loop_cnt", 1 );

	//
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	// Action Attack
	m_AttackAttribute.Init();
	m_AttackAttribute.m_AttackAnimation = m_SkillMotionName;
	m_AttackAttribute.m_fAttackAniRate  = m_fAnimationTimeRate;
	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay", 0 );

	m_AttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_AttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_AttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 0 );
	m_AttackAttribute.m_vWeaponInfoList.clear();
	m_AttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
	
	// ActionEnd Attack
	m_LandAttackAttribute.Init();
	m_LandAttackAttribute.m_AttackAnimation = m_SkillEndMotion;
	m_LandAttackAttribute.m_fAttackAniRate  = m_fEndMotionRate;

	m_LandAttackAttribute.m_vForceInfoList.clear();
	iForceCnt = rkLoader.LoadInt_e( "land_attack_force_cnt", 0 );
	if( iForceCnt > 0 )
	{
		m_LandAttackAttribute.m_vForceInfoList.reserve( iForceCnt );
	}

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "land_attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "land_attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "land_attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_LandAttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	iAttackTypeCnt = rkLoader.LoadInt_e( "land_attack_type_count", 1 );
	m_LandAttackAttribute.m_vWeaponInfoList.clear();
	m_LandAttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "land_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "land_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "land_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "land_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "land_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_LandAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
	LoadOwnerBuffList( rkLoader );
}

void ioCircleMoveAttackSkill2::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

ioSkill* ioCircleMoveAttackSkill2::Clone()
{
	return new ioCircleMoveAttackSkill2( *this );
}

bool ioCircleMoveAttackSkill2::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwFireStartTime = 0;
	m_fCharMoveSpeed = 0.0f;
	m_fCurCharMoveSpeed = 0.0f;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	m_dwWaitStartTime = 0;
	m_dwEnableReserveTime = 0;

	m_fCurChargeRate = 0.0f;

	m_State = SS_NONE;
	m_bReduceGauge = false;
	m_bSetHandMesh = false;

	m_iCurLoopCnt = 0;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = dwCurTime + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = dwCurTime + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetWaitState( pChar );
		
		// 사용 아이템 처리
		if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
		{
			pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
			m_bSetHandMesh = true;
		}
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

	m_dwCurSkillProtectTime = 0;
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = dwCurTime + m_dwSkillProtectTime;

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioCircleMoveAttackSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fCurRange = m_fCircleRange;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_State )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;

			EndPreDelayEffect( pChar );
			SetWaitState( pChar );
			SetOwnerBuffList( pChar, pStage );

			// 사용 아이템 처리
			if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
			{
				pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
				m_bSetHandMesh = true;
			}
		}
		break;
	case SS_WAIT:
		{
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

			if( pChar->IsNeedProcess() )
			{
				// 시간 체크
				if( m_dwWaitStartTime+m_dwWaitDuration < dwCurTime )
				{
					SetActionState( pChar, pStage );
					return;
				}
			}
		}
		break;
	case SS_ACTION:
		if( CheckLanding( pChar ) )
		{
			SetEndState( pChar );
		}
		else
		{
			ProcessAction( pChar );
			ProcessMove( pChar, pStage );
		}
		break;
	case SS_END:
		{
			ProcessEnd( pChar );
		}
		break;
	}
}

void ioCircleMoveAttackSkill2::OnSkillEnd( ioBaseChar *pChar )
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
	RemoveOwnerBuffList( pChar );

	pChar->SetMagicCircleTarget( "" );

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	if( m_dwTargetCircleEffect != -1 )
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwWaitStartTime = 0;
	m_iCurLoopCnt = 0;

	m_State = SS_NONE;
}

bool ioCircleMoveAttackSkill2::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCircleMoveAttackSkill2::SetNormalSkill( ioBaseChar *pChar )
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
	
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;

	// Attack
	if( !m_AttackAttribute.m_vWeaponInfoList.empty() )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iCurSkillAniID,
											m_AttackAttribute.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											m_AttackAttribute.m_dwPreDelay );
	}

	pChar->AttachEffect( m_StartEffect );

	if( !m_bReduceGauge && m_State != SS_END )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioCircleMoveAttackSkill2::SetWaitState( ioBaseChar *pChar )
{
	m_dwPreDelayStartTime = 0;

	if( pChar )
	{
		float fCurGravity = pChar->GetCharGravity();
		float fFlightDuration = GetFlightDuration( fCurGravity );

		if( fFlightDuration > 0.0f )
		{
			m_fCharMoveSpeed = m_fCircleRange / (fFlightDuration / FLOAT1000);
		}
	}
	
	m_dwWaitStartTime = FRAMEGETTIME();
	m_State = SS_WAIT;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szWaitMotion );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fWaitMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( 5.0f );
	pGrp->ClearAllActionAni( 5.0f );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

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

void ioCircleMoveAttackSkill2::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;

	float fCurRange = m_fCircleRange;

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
	m_vCharMoveDir = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		fRate = fRange / m_fCircleRange;
		m_fCurCharMoveSpeed = m_fCharMoveSpeed * fRate;

		D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
		pChar->SetTargetRotAndMoveDirChange( qtRot, false );
		pChar->SetWorldOrientation( qtRot );
	}

	pChar->SetJumpPower( m_fCharJumpPower );
	//

	SetNormalSkill( pChar );

	m_iCurLoopCnt++;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMoveAttackSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_ACTION:
		SetActionState( pChar, pStage );
		break;
	}
}

bool ioCircleMoveAttackSkill2::IsEndState() const
{
	if( m_State == SS_END )
		return true;
	return false;
}

void ioCircleMoveAttackSkill2::SetEndState( ioBaseChar *pChar )
{
	m_State = SS_END;
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

	if( m_SkillEndMotion.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_LandAttackAttribute.m_fAttackAniRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;
	
	pGrp->ClearAllLoopAni( 5.0f );
	pGrp->ClearAllActionAni( 5.0f );
	pGrp->SetActionAni( iCurSkillAniID, 5.0f, 5.0f, FLOAT1, FLOAT1/fCurEndRate );

	m_dwFireStartTime = 0;
	m_dwMotionStartTime = dwCurTime;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iCurSkillAniID,
										m_LandAttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fCurEndRate,
										m_LandAttackAttribute.m_dwPreDelay );

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fCurEndRate;

	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwEnableReserveTime = 0;

	if( m_iCurLoopCnt >= m_iMaxLoopCnt )
	{
		// Key Reserve
		m_dwEnableReserveTime = dwCurTime;
		m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" );

		pChar->AttachEffect( m_EndEffect );
	}
}

bool ioCircleMoveAttackSkill2::IsProtected( int iDefenseBreakType ) const
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
	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioCircleMoveAttackSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State != SS_END )
		return false;

	if( m_iCurLoopCnt < m_iMaxLoopCnt )
		return false;

	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioCircleMoveAttackSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioCircleMoveAttackSkill2::CheckLanding( ioBaseChar *pChar )
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

void ioCircleMoveAttackSkill2::ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioCircleMoveAttackSkill2::ProcessAction( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_State != SS_ACTION ) return;
	if( m_dwFireStartTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireStartTime < FRAMEGETTIME() && !m_AttackAttribute.m_vWeaponInfoList.empty() )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList.front(),
								  GetName(),
								  m_dwWeaponBaseIndex,
								  vPos );

		m_dwFireStartTime = 0;
	}
}

void ioCircleMoveAttackSkill2::ProcessEnd( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_State != SS_END ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		pChar->SkillFireExplicit( m_LandAttackAttribute.m_vWeaponInfoList.front(),
								  GetName(),
								  m_dwWeaponBaseIndex,
								  vPos );

		m_dwFireStartTime = 0;
	}

	if( m_iCurLoopCnt < m_iMaxLoopCnt && m_dwMotionEndTime < dwCurTime )
	{
		SetWaitState( pChar );
		return;
	}
}

void ioCircleMoveAttackSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

float ioCircleMoveAttackSkill2::GetFlightDuration( float fCurGravity )
{
	float fTopTime, fTopHeight;
	fTopTime = fTopHeight = 0.0f;

	float fNewGravity = fCurGravity * m_fSkillGravityRate;
	if( fNewGravity > 0.0f )
	{
		fTopTime = m_fCharJumpPower / fNewGravity;
		fTopHeight = (m_fCharJumpPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fResult = 2 * fTopTime * FLOAT1000;

#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "CirCleMoveAttack - [ExpectionValue] TopHeight: %.2f, FlightDuration: %.2f", fTopHeight, fResult );
	}
#endif

	return fResult;
}

SkillCharColType ioCircleMoveAttackSkill2::IsCharSkipState() const
{
	if( m_EnableCharCollisionSkip == SCCT_ALL )
	{
		switch( m_State )
		{
		case SS_ACTION:
		case SS_END:
			return SCCT_ALL;
		default:
			return SCCT_NONE;
		}
	}

	return m_EnableCharCollisionSkip;
}

bool ioCircleMoveAttackSkill2::SetOwnerBuffList( ioBaseChar *pChar, ioPlayStage* pStage )
{
	if( !pChar->IsNeedProcess() ) return false;
	if( m_OwnerBuffList.empty() ) return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpInfo )
	{		
		fValue = pUpInfo->GetValue(pStage);
	}

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i].m_BuffName;

		if( pChar->IsHasCrown() && m_bDisableKingSkill )
			continue;
		else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
			continue;
		
		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuff( szBuffName, pChar->GetCharName(), szItemName, this );
			if( pBuff )
			{				
				pBuff->SetExtraDurationByGrowth( fValue );

				bResult = true;
				vBuffList.push_back( szBuffName );
			}
		}
	}

	if( bResult )
	{
		int iCurBuffCnt = vBuffList.size();
		if( pChar->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff1 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pChar->GetCharName();
			kPacket << szItemName;
			kPacket << pChar->GetRandomSeed();
			kPacket << true;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << fValue;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return bResult;
}

void ioCircleMoveAttackSkill2::RemoveOwnerBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioBuff *pBuff = pOwner->GetBuff( m_OwnerBuffList[i].m_BuffName, pOwner->GetCharName() );
		if ( pBuff )
			pBuff->SetReserveEndBuff();
	}
}