

#include "stdafx.h"

#include "ioCircleMoveAttackSkill8.h"
#include "WeaponDefine.h"

ioCircleMoveAttackSkill8::ioCircleMoveAttackSkill8()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_dwWaitStartTime = 0;
	m_bTargetRot = false;
	m_bComboAttackReserve = false;
}

ioCircleMoveAttackSkill8::ioCircleMoveAttackSkill8( const ioCircleMoveAttackSkill8 &rhs )
: ioNormalSkill( rhs ),
 m_bDisableGrowth( rhs.m_bDisableGrowth ),
 m_fGuagePerTic( rhs.m_fGuagePerTic ),
 m_dwTicTime( rhs.m_dwTicTime ),
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

 m_bEnableCharRot( rhs.m_bEnableCharRot ),
 m_dwCharRotSpeed( rhs.m_dwCharRotSpeed ),
 m_WaitType( rhs.m_WaitType ),
 m_dwWaitLimitTime( rhs.m_dwWaitLimitTime ),
 m_BulletInfoList( rhs.m_BulletInfoList ),
 m_iMaxWeaponWay( rhs.m_iMaxWeaponWay ),

 m_AttackAttribute( rhs.m_AttackAttribute ),

 m_stSkillMotion( rhs.m_stSkillMotion ),
 m_fSkillMotionRate( rhs.m_fSkillMotionRate ),
 m_fCircleLandOffsetHeight( rhs.m_fCircleLandOffsetHeight ),

 m_stMapEffect( rhs.m_stMapEffect ),
 m_vEffectScale( rhs.m_vEffectScale ),

 m_fSkillEndJumpPower( rhs.m_fSkillEndJumpPower ),

 //////////////////////////////////////////////////////////////////////////
 m_dwUndergroundMaxTime( rhs.m_dwUndergroundMaxTime ),
 m_UndergroundEffect( rhs.m_UndergroundEffect ),
 m_stUndergroundAni( rhs.m_stUndergroundAni ),
 m_fUndergroundAniRate( rhs.m_fUndergroundAniRate ),

 m_CollisionPassBuff( rhs.m_CollisionPassBuff ),
 m_dwGroundMaxTime( rhs.m_dwGroundMaxTime),
 m_dwGroundWaitTime( rhs.m_dwGroundWaitTime ),
 m_GroundEffect( rhs.m_GroundEffect ),
 m_stGroundAni( rhs.m_stGroundAni ),
 m_fGroundAniRate( rhs.m_fGroundAniRate ),
 m_dwGroundSpeed( rhs.m_dwGroundSpeed ),
 m_fGround_Under_Length1( rhs.m_fGround_Under_Length1 ),
 m_fGround_Under_Length2( rhs.m_fGround_Under_Length2 ),

 m_vWeaponInfoList( rhs.m_vWeaponInfoList ),

 m_vBuffString( rhs.m_vBuffString ),

 m_NotReadyCircle( rhs.m_NotReadyCircle ),
 m_fCircleToCircleOffset( rhs.m_fCircleToCircleOffset ),
 m_vComboList( rhs.m_vComboList ),
 m_fComboSearchLength( rhs.m_fComboSearchLength ),
 m_fComboSearchMaxHeightGap( rhs.m_fComboSearchMaxHeightGap ),

 m_stKeyReserveWaitMotion( rhs.m_stKeyReserveWaitMotion ),
 m_fKeyReserveWaitMotionRate( rhs.m_fKeyReserveWaitMotionRate ),
 m_bAutoComboAction( rhs.m_bAutoComboAction ),
 m_ShowAttackAttribute( rhs.m_ShowAttackAttribute )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_dwWaitStartTime = 0;
	m_bTargetRot = false;

	m_dwFlyMoveEffect = 0;

	m_dwUndergroundEffect = 0;
}

ioCircleMoveAttackSkill8::~ioCircleMoveAttackSkill8()
{
}

void ioCircleMoveAttackSkill8::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	m_WaitType = (WaitType)rkLoader.LoadInt_e( "action_wait_type", WT_NORMAL );
	m_dwWaitLimitTime = (DWORD)rkLoader.LoadInt_e( "wait_limit_time", 0 );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "not_ready_circle", "", szBuf, MAX_PATH );
	m_NotReadyCircle = szBuf;

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

	m_bEnableCharRot = rkLoader.LoadBool_e( "enable_char_rotate", false );
	m_dwCharRotSpeed = rkLoader.LoadInt_e( "char_rotate_speed", 0 );

	//////////////////////////////////////////////////////////////////////////
	// Action Attack _ A
	rkLoader.LoadString_e( "skill_motion_a", "", szBuf, MAX_PATH );
	m_stSkillMotion = szBuf;
	m_fSkillMotionRate = rkLoader.LoadFloat_e( "animation_time_rate_a", FLOAT1 );

	m_AttackAttribute.Init();
	m_AttackAttribute.m_AttackAnimation = m_stSkillMotion;
	m_AttackAttribute.m_fAttackAniRate  = m_fSkillMotionRate;
	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay_a", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay_a", 0 );

	m_iMaxWeaponWay = rkLoader.LoadInt_e( "max_bullet_cnt_a", 1 );
	m_iMaxWeaponWay = max( 1, m_iMaxWeaponWay );
	m_BulletInfoList.clear();
	for( int i=0; i < m_iMaxWeaponWay; ++i )
	{
		BulletInfo kInfo;

		wsprintf_e( szKey, "bullet_angle%d_a", i+1 );
		kInfo.m_fYawAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "bullet_range_offset%d_a", i+1 );
		kInfo.m_fRangeOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "bullet_height_offset%d_a", i+1 );
		kInfo.m_fHeightOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		m_BulletInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count_a", 0 );
	m_AttackAttribute.m_vWeaponInfoList.clear();
	m_AttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d_a", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist_a", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded_a", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time_a", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani_a", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}

	m_fCircleLandOffsetHeight = rkLoader.LoadFloat_e( "circle_land_offset_height", 0.0f );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "target_effect", "", szBuf, MAX_PATH );
	m_stMapEffect = szBuf;
	m_vEffectScale.x = rkLoader.LoadFloat_e( "target_effect_scale", FLOAT1 );
	m_vEffectScale.y = rkLoader.LoadFloat_e( "target_effect_scale", FLOAT1 );
	m_vEffectScale.z = rkLoader.LoadFloat_e( "target_effect_scale", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	m_fSkillEndJumpPower = rkLoader.LoadFloat_e( "last_jump_power", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	m_dwUndergroundMaxTime = (DWORD)rkLoader.LoadInt_e( "underground_max_time", 0 );
	rkLoader.LoadString_e( "underground_effect", "", szBuf, MAX_PATH );
	m_UndergroundEffect = szBuf;
	rkLoader.LoadString_e( "underground_ani", "", szBuf, MAX_PATH );
	m_stUndergroundAni = szBuf;
	m_fUndergroundAniRate = rkLoader.LoadFloat_e( "underground_ani_rate", FLOAT1 );


	rkLoader.LoadString_e( "ground_buff", "", szBuf, MAX_PATH );
	m_CollisionPassBuff = szBuf;
	m_dwGroundMaxTime = (DWORD)rkLoader.LoadInt_e( "ground_max_time", 0 );
	m_dwGroundWaitTime = (DWORD)rkLoader.LoadInt_e( "ground_wait_time", 0 );

	rkLoader.LoadString_e( "ground_effect", "", szBuf, MAX_PATH );
	m_GroundEffect = szBuf;
	rkLoader.LoadString_e( "ground_ani", "", szBuf, MAX_PATH );
	m_stGroundAni = szBuf;
	m_fGroundAniRate = rkLoader.LoadFloat_e( "ground_ani_rate", FLOAT1 );

	m_dwGroundSpeed = (DWORD)rkLoader.LoadInt_e( "ground_speed", 0);
	m_fGround_Under_Length1 = rkLoader.LoadFloat_e( "ground_under_length1", 0.f );
	m_fGround_Under_Length2 = rkLoader.LoadFloat_e( "ground_under_length2", 0.f );

	int nBuffCnt = rkLoader.LoadInt_e( "buff_cnt", 0 );
	for( int i = 0; i < nBuffCnt; i++ )
	{
		wsprintf_e( szKey, "add_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString szString;
		szString = szBuf;

		m_vBuffString.push_back( szString );
	}
	//////////////////////////////////////////////////////////////////////////

	m_fCircleToCircleOffset =  rkLoader.LoadFloat_e( "circle_to_circle_offset", 0.0f );

	//
	int iComboCnt = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	m_vComboList.clear();

	if( iComboCnt > 0 )
	{
		m_vComboList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboList.push_back( kComboAttack );
		}
	}

	LoadAttackAttribute( "show_attack", m_ShowAttackAttribute, rkLoader );


	m_fComboSearchLength = rkLoader.LoadFloat_e( "linear_search_length", 0.f );
	m_fComboSearchMaxHeightGap = rkLoader.LoadFloat_e( "linear_search_max_height_gap", 0.f );

	rkLoader.LoadString_e( "wait_key_motion", "", szBuf, MAX_PATH );
	m_stKeyReserveWaitMotion = szBuf;

	m_fKeyReserveWaitMotionRate = rkLoader.LoadFloat_e("wait_key_motion_rate", FLOAT1 );

	m_bAutoComboAction = rkLoader.LoadBool_e("auto_combo_action", false );
}

ioSkill* ioCircleMoveAttackSkill8::Clone()
{
	return new ioCircleMoveAttackSkill8( *this );
}


bool ioCircleMoveAttackSkill8::OnSkillStart( ioBaseChar *pChar )
{	
	m_bComboAttackReserve = false;

	// 성장관련 요소
	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;
	m_iCurLinearCombo = 0;

	m_dwKeyReserveStateMaxTime = 0;

	m_dwWaitMotionKeyReserveTime = 0;
	//////////////////////////////////////////////////////////////////////////
	m_dwFireStartTime = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_dwWaitStartTime = 0;
	m_bTargetRot = false;

	m_fCurChargeRate = 0.0f;
	m_iWeaponWay = 0;

	m_bSkillInJump = false;
	if( m_SkillUseType == SUT_JUMP )
		m_bSkillInJump = true;

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
		return false;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SetSkillProtection();

	//SetUndergroundState( pChar );
	SetPreDelayState(pChar);

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	if( m_bSkillInJump )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
	}

	return true;
}

void ioCircleMoveAttackSkill8::OnSkillEnd( ioBaseChar *pChar )
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

	if( m_dwTargetCircleEffect != -1 )
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );

	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;

	EndUndergroundMapEffect( pChar );
	
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	if( pGrp )
		pGrp->SetVisible( true );

	EraseBuffList( pChar );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
}

void ioCircleMoveAttackSkill8::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !pChar ) return;

	float fCurRange = m_fCircleRange;
	int iSlot = GetOwnerItem()->GetType() - 1;

	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_State )
	{
	case SS_PREDELAY:
		{
			m_bEnableSummon = true;
			m_bOffCircle = false;
			m_CurCircleEffect = m_ReadyCircle;

			if( dwCurTime > m_dwPreDelayStartTime + m_dwPreDelayTime )
				SetHideState(pChar);
		}
		break;
	case SS_HIDING:
		{
			if( /*pChar->IsNeedProcess() &&*/ (dwCurTime > (m_dwUndergroundStartTime+m_dwUndergroundMaxTime)) )
				SetWaitState(pChar);
		}
		break;
	case SS_WAIT:
		{
			ChangeCircleDropZoneAndPositionGap( pChar, pStage );
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );
			// MagicCircle 위치방향으로 캐릭터 회전
			/*D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );*/
			//////////////////////////////////////////////////////////////////////////
			if( pChar->IsNeedProcess() && m_dwWaitStartTime+m_dwWaitLimitTime < dwCurTime )
			{
				EndPreDelayEffect( pChar );
				SetShowWaitState(pChar, pStage );
			}
			else if( pChar->IsNeedProcess() && pChar->IsAttackKey() && m_bEnableSummon )
			{
				EndPreDelayEffect( pChar );
				SetShowWaitState( pChar, pStage );
			}
		}
		break;
	case SS_SHOWING_WAIT:
		{
			if( ( m_dwGroundStartTime + m_dwGroundWaitTime ) < dwCurTime )
				SetShowState( pChar, pStage );
		}
		break;
	case SS_SHOWING:
		{
			//if( m_dwMotionEndTime < dwCurTime )
				//SetEndState(pChar);
			/*if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
				m_bComboAttackReserve = true;*/

			if( ProcessMove( pChar, pStage ) )
			{
				SetKeyReserveState( pChar, pStage );
				/*if( pChar->IsNeedProcess() )
				{
					if( m_bComboAttackReserve )
						SetLinerState( pChar, pStage );
					else
						SetEndState( pChar );

					return;
				}*/
				return;
			}

			//if( pChar->IsNeedProcess() && (m_dwGroundStartTime + m_dwGroundMaxTime) < dwCurTime )
				//SetEndState(pChar);
			if( (m_dwGroundStartTime + m_dwGroundMaxTime) < dwCurTime )
				SetKeyReserveState( pChar, pStage );
		}
		break;
	case SS_WAIT_KEY:
		{
			if( pChar->IsNeedProcess() && dwCurTime > m_dwWaitMotionKeyReserveTime && pChar->IsAttackKey()  )
			{
				SetLinerState( pChar, pStage );
				return;
			}

			if( pChar->IsNeedProcess() && m_dwKeyReserveStateMaxTime < dwCurTime )
				SetEndState( pChar );
		}
		break;
	case SS_LINER_ACTION:
		if( pChar->IsNeedProcess() )
		{
			if( m_dwEnableReserveTime < dwCurTime && (pChar->IsAttackKey() || m_bAutoComboAction ) )
			{
				m_bComboAttackReserve = true;
			}

			if( m_dwMotionEndTime < dwCurTime )
			{
				if( m_iCurLinearCombo > (int)m_vComboList.size() || !m_bComboAttackReserve )
				{
					SetEndState( pChar );
				}
				else
				{
					SetNextLinearCombo( pChar, m_iCurLinearCombo );
					m_iCurLinearCombo++;
				}

				return;
			}
		}
		break;
	case SS_END:
		break;
	}

	if( m_State != SS_LINER_ACTION )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
	}
}

bool ioCircleMoveAttackSkill8::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}


void ioCircleMoveAttackSkill8::SetKeyReserveState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State =  SS_WAIT_KEY;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
	{
		SetEndState( pChar );
		return;
	}

	int iAniID = pGrp->GetAnimationIdx( m_stKeyReserveWaitMotion );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/m_fKeyReserveWaitMotionRate, 0.0f, 0 );

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwKeyReserveStateMaxTime = dwCurTime;
	m_dwKeyReserveStateMaxTime += pGrp->GetAnimationFullTime( iAniID ) * m_fKeyReserveWaitMotionRate;

	//fire
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_ShowAttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		m_fKeyReserveWaitMotionRate,
		0 );

	m_dwWaitMotionKeyReserveTime = dwCurTime;
	m_dwWaitMotionKeyReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fKeyReserveWaitMotionRate;
}

void ioCircleMoveAttackSkill8::SetShowMotion( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = m_stGroundAni;//GetSkillMotionName();

	int iCurSkillAniID = -1;
	float fAniRate = m_fGroundAniRate;

	iCurSkillAniID = pGrp->GetAnimationIdx( m_stGroundAni );
	fAniRate = m_fSkillMotionRate;
	
	if( iCurSkillAniID == -1 )
		return;

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iCurSkillAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime;
	m_dwFireMotionEndTime = m_dwMotionStartTime;
	m_dwFireStartTime = 0;

	m_dwFireMotionEndTime +=  pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	/*if( !m_AttackAttribute.m_vWeaponInfoList.empty() )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iCurSkillAniID,
			m_AttackAttribute.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			m_AttackAttribute.m_dwPreDelay );
	}*/


	m_dwMotionEndTime  += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	pChar->AttachEffect( m_StartEffect );

	if( pChar->IsNeedProcess() && m_State != SS_END )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
}


void ioCircleMoveAttackSkill8::SetPreDelayState( ioBaseChar *pChar )
{
	m_State = SS_PREDELAY;
}

void ioCircleMoveAttackSkill8::SetWaitState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	m_dwWaitStartTime = FRAMEGETTIME();
	m_State = SS_WAIT;

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

	if( pChar->IsNeedProcess() && !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}

	if(pChar->IsNeedProcess() && m_WaitType == WT_IMMEDIATE )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( 0.0f, m_fCircleRange, m_fCircleHeightGap, false );
	}

	pGrp->SetVisible( false );

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	vPos.y -= m_fGround_Under_Length1;
	pChar->SetWorldPosition( vPos );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );

	/*if( pChar->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_WAIT;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}


void ioCircleMoveAttackSkill8::SetHideState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	m_dwUndergroundStartTime = FRAMEGETTIME();
	m_State = SS_HIDING;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_stUndergroundAni );
	if( iAniID == -1 )	
		return;

	pChar->InitActionStopDelay( true );

	float fTimeRate = m_fUndergroundAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0 );

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	ioMapEffect *pMapEffect = NULL;

	pMapEffect = pChar->CreateMapEffectBySkill( m_UndergroundEffect, vPos );

	if( pMapEffect )
		m_dwUndergroundEffect = pMapEffect->GetUniqueID();

	pChar->SetSkillMoveEnable( false, false );

	AddBuffList( pChar );

	//Send Packet
}



void ioCircleMoveAttackSkill8::SetLinerState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_LINER_ACTION;

	if( pChar->IsNeedProcess() )
		LookEnemy( pChar, pStage );

	
	SetNextLinearCombo( pChar, 0 );
	m_iCurLinearCombo++;
}


void ioCircleMoveAttackSkill8::SetShowState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_SHOWING;
}

void ioCircleMoveAttackSkill8::SetShowWaitState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	pGrp->SetVisible( true );

	for( int i = 0; i < (int)m_vBuffString.size(); i++ )
	{
		ioHashString stBuff;
		stBuff = m_vBuffString[ i ];
		if( !stBuff.IsEmpty() )
			pChar->RemoveBuff( stBuff );
	}

	m_State = SS_SHOWING_WAIT;

	pChar->SetGravityAmt( 0.f );

	if( pChar->IsNeedProcess() )
	{
		if( m_bEnableSummon )
			m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
		else
		{
			m_vCirclePos = pChar->GetWorldPosition();
			m_vCirclePos.y += m_fGround_Under_Length1;
		}

		pChar->SetMagicCircleTarget( "" );
		pChar->DestroyMagicCircle();
	}

	EndUndergroundMapEffect( pChar );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwGroundStartTime = dwCurTime;

	int iAniID = pGrp->GetAnimationIdx( m_stGroundAni );
	if( iAniID == -1 )	return;

	//pChar->SetCharCollisionSkipTime( m_dwGroundStartTime, m_dwGroundStartTime + m_dwGroundMaxTime );

	float fCurRange = m_fCircleRange;
	m_dwPreDelayStartTime = 0;
	/*if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}*/

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

	D3DXVECTOR3 vPos( m_vCirclePos );
	
	m_fShowGoalHeight = m_vCirclePos.y;
	
	vPos.y -= m_fGround_Under_Length2;
	pChar->SetWorldPosition( vPos );


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

	EndPreDelayEffect( pChar );
	SetShowMotion( pChar,pStage );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if(pChar->IsNeedProcess() &&  GetOwnerItem() )
		pChar->SetCurItemSkillGauge(GetOwnerItem()->GetName() , 0.0f );

	/*if( !m_CollisionPassBuff.IsEmpty() )
		pChar->AddNewBuff( m_CollisionPassBuff, pChar->GetCharName(), GetName(), NULL );*/

	

	if( pChar->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;

		kPacket << m_vCirclePos;

		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetTargetRot();

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//////////////////////////////////////////////////////////////////////////
	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}

	ioMapEffect *pMapEffectTarget = pStage->CreateMapEffect( m_stMapEffect, m_vCirclePos, m_vEffectScale );
	if( pMapEffectTarget )
	{
		m_dwFlyMoveEffect = pMapEffectTarget->GetUniqueID();
	}
}

void ioCircleMoveAttackSkill8::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_ACTION:
		{
			int iInputType;

			rkPacket >> m_vCirclePos;//맵이펙트를 뿌려주자

			rkPacket >> iInputType;

			SetShowWaitState( pChar, pStage );
		}
		break;
		/*case SSC_WAIT:
		{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;

		pChar->SetTargetRotToRotate( qtRot, true, false );
		SetWaitState(pChar);
		}
		break;*/
	case SSC_LINER_ACTION:
		{
			m_State = SS_LINER_ACTION;
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos;
			rkPacket >> qtRot;
			rkPacket >> m_iCurLinearCombo;
			pChar->SetTargetRotToRotate( qtRot, true, false );

			SetNextLinearCombo( pChar, m_iCurLinearCombo );
		}
		break;
	case SSC_END:
		SetEndState( pChar );
		break;
	}
}

bool ioCircleMoveAttackSkill8::IsEndState() const
{
	if( m_State == SS_END )
		return true;
	return false;
}

void ioCircleMoveAttackSkill8::SetEndState( ioBaseChar *pChar )
{
	m_State = SS_END;
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwTargetCircleEffect != -1 )
	{
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );
		m_dwTargetCircleEffect = -1;
	}

	pChar->DestroyMagicCircle();

	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}

	//SendPacket
	if( pChar->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioCircleMoveAttackSkill8::IsProtected( int iDefenseBreakType ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireMotionEndTime && dwCurTime < m_dwFireMotionEndTime )
		return true;

	if( m_State == SS_PREDELAY || m_State == SS_WAIT || m_State == SS_HIDING )
		return true;

	return false;
}

bool ioCircleMoveAttackSkill8::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioCircleMoveAttackSkill8::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}



void ioCircleMoveAttackSkill8::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	ioTraceWeapon4 *pTrace4 = ToTraceWeapon4( pWeapon );
	ioBall2Weapon *pBall2Weapon = ToBall2Weapon( pWeapon );
	ioArrowWeapon *pArrowWeapon = ToArrowWeapon( pWeapon );

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
	
	D3DXVECTOR3 vTargetPos = m_vCirclePos;
	vTargetPos = vTargetPos + D3DXVECTOR3( 0, m_fCircleLandOffsetHeight, 0 );

	if( pArrowWeapon )
	{
		if( m_iMaxWeaponWay <= m_iWeaponWay )
			return;

		float fCurAngle = 0.0f;
		fCurAngle = m_BulletInfoList[m_iWeaponWay].m_fYawAngle;
		fCurAngle = ioMath::ArrangeHead( fCurAngle );
		pArrowWeapon->RotateMoveDir( fCurAngle, 0.0f );

		float fCheckPitch = 0;
		float fPitchAngle = 0;
		ioBaseChar *pChar = pArrowWeapon->GetOwner();
		D3DXVECTOR3 vPos = pArrowWeapon->GetPosition();
		D3DXVECTOR3 vRemembervPos = vPos;
		D3DXVECTOR3 vCurDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vCurDir, &vCurDir );
		vPos = vTargetPos - vPos;
		D3DXVec3Normalize( &vPos, &vPos );

		fCheckPitch = D3DXVec3Dot( &vPos, &vCurDir );
		fPitchAngle = acos(fCheckPitch);
		float fDeg    = RADtoDEG(fPitchAngle);

		if( vRemembervPos.y >= vTargetPos.y )
			pArrowWeapon->RotateMoveDir( 0.0f, fDeg );
		else
			pArrowWeapon->RotateMoveDir( 0.0f, 360.0f - fDeg );

		m_iWeaponWay++;
	}

	if( pBall2Weapon )
	{
		D3DXVECTOR3 vPos = pBall2Weapon->GetPosition();
		vPos = vTargetPos - vPos;
		D3DXVec3Normalize( &vPos, &vPos );

		pBall2Weapon->SetMoveDir( vPos );
	}
}


SkillCharColType ioCircleMoveAttackSkill8::IsCharSkipState() const
{
	if( m_EnableCharCollisionSkip == SCCT_ALL )
	{
		switch( m_State )
		{
		//case SS_LINER_ACTION:
		case SS_END:
			return SCCT_ALL;
		default:
			return SCCT_NONE;
		}
	}

	return m_EnableCharCollisionSkip;
}


bool ioCircleMoveAttackSkill8::IsSkipState() const
{
	if( m_State == SS_SHOWING )
		return true;

	return false;
}


void ioCircleMoveAttackSkill8::CheckCharRotate( ioBaseChar *pChar )
{
	if( m_dwCharRotSpeed == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwCharRotSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();
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

	if( fGapAngle < 2.0f )
	{
		m_bTargetRot = true;
		pChar->SetTargetRotToRotate( qtRotate, true );
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

	if( vAxis == ioMath::VEC3_ZERO )
		vAxis = ioMath::UNIT_Y;

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pChar->SetTargetRotToRotate( qtNewRot, true );
}

bool ioCircleMoveAttackSkill8::IsJumpSkill()
{
	return m_bSkillInJump;
}


int ioCircleMoveAttackSkill8::GetState()
{
	return m_State;
}

float ioCircleMoveAttackSkill8::GetEndJumpPower()
{
	return m_fSkillEndJumpPower;
}


void ioCircleMoveAttackSkill8::EndUndergroundMapEffect( ioBaseChar *pChar )
{
	if( m_dwUndergroundEffect == 0 )
		return;

	pChar->EndMapEffect(m_dwUndergroundEffect, false );
}


bool ioCircleMoveAttackSkill8::ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage ) 
		return false;
	
	if( m_dwGroundSpeed == 0.0f ) 
		return false;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	D3DXVECTOR3 vUp( 0.f, FLOAT1, 0.f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = vUp * m_dwGroundSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		//if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			pChar->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
	{
		if( vPos.y >= m_fShowGoalHeight )
		{
			vPos.y = m_fShowGoalHeight;
			pChar->SetWorldPosition( vPos );
			return true;
		}
		else
			pChar->SetWorldPosition( vPos );
	}



	return false;
}

bool ioCircleMoveAttackSkill8::IsAttackEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}


void ioCircleMoveAttackSkill8::AddBuffList( ioBaseChar *pChar )
{
	for( int i = 0; i < (int)m_vBuffString.size(); i++ )
	{
		ioHashString stBuff;
		stBuff = m_vBuffString[ i ];
		if( !stBuff.IsEmpty() )
			pChar->AddNewBuff( stBuff, pChar->GetCharName(), GetName(), NULL );
	}
}


void ioCircleMoveAttackSkill8::EraseBuffList( ioBaseChar *pChar )
{
	for( int i = 0; i < (int)m_vBuffString.size(); i++ )
	{
		ioHashString stBuff;
		stBuff = m_vBuffString[ i ];
		if( !stBuff.IsEmpty() )
			pChar->RemoveBuff( stBuff );
	}
}


void ioCircleMoveAttackSkill8::ChangeCircleDropZoneAndPositionGap( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( !pChar ) return;

	D3DXVECTOR3 vPos = pChar->GetMagicCirclePos();

	if( pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y + 3.0f ) > 0.0f )
	{
		bool bOn = true;
		SummonList pList = pStage->GetSummonDummyInfoList();
		SummonList::iterator iter = pList.begin();
		for( ; iter != pList.end(); ++iter )
		{
			D3DXVECTOR3 vPos1 = (*iter).m_vPos;
			D3DXVECTOR3 vPos2 = pChar->GetMagicCirclePos();				
			D3DXVECTOR3 vRange = vPos1 - vPos2;
			float fRange = fabs(D3DXVec3Length( &vRange ));

			if( fRange < m_fCircleToCircleOffset )
				bOn = false;				
		}

		if( !bOn )
		{
			if( !m_bOffCircle && m_CurCircleEffect == m_ReadyCircle )
			{
				ioHashString szDummyCircle;
				pChar->CreateMagicCircle( m_NotReadyCircle, m_NotReadyCircle, szDummyCircle, 0 );
				pChar->SetMagicCirclePos( false, vPos );
				m_bOffCircle = true;
				m_CurCircleEffect = m_NotReadyCircle;
			}

			m_bEnableSummon = false;
		}
		else
		{
			if( m_bOffCircle && m_CurCircleEffect == m_NotReadyCircle )
			{	
				ioHashString szDummyCircle;
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, 0 );
				pChar->SetMagicCirclePos( false, vPos );
				m_bOffCircle = false;
				m_CurCircleEffect = m_ReadyCircle;		
			}

			m_bEnableSummon = true;
		}		
	}
	else
	{
		if( !m_bOffCircle && m_CurCircleEffect == m_ReadyCircle )
		{
			ioHashString szDummyCircle;
			pChar->CreateMagicCircle( m_NotReadyCircle, m_NotReadyCircle, szDummyCircle, 0 );
			pChar->SetMagicCirclePos( false, vPos );
			m_bOffCircle = true;
			m_CurCircleEffect = m_NotReadyCircle;
		}

		m_bEnableSummon = false;
	}
}


void ioCircleMoveAttackSkill8::LookEnemy( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	ioBaseChar *pFindChar = NULL;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == pOwner->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vDiff = vTargetPos - vCurPos;
		float fDiffRange = D3DXVec3Length( &vDiff );

		
		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fComboSearchMaxHeightGap )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fComboSearchMaxHeightGap )
			continue;

		// 거리 체크
		if( fDiffRange < m_fComboSearchLength )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRange;
				szTarget = pTarget->GetCharName();
				pFindChar = pTarget;
			}
			else if( fCurGap >= 0.0f && fDiffRange < fCurGap )
			{
				fCurGap = fDiffRange;
				szTarget = pTarget->GetCharName();
				pFindChar = pTarget;
			}
		}
	}

	if( !szTarget.IsEmpty() && pFindChar )
	{
		D3DXVECTOR3 vLook = pFindChar->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vLook, &vLook );
		pOwner->SetTargetRotToDir( vLook, true );
	}
}


bool ioCircleMoveAttackSkill8::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() ) 
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( TWT_DOWN_WOUND_EXCEPT ) )
		return false;

	return true;
}


void ioCircleMoveAttackSkill8::SetNextLinearCombo( ioBaseChar *pChar, int nIndex )
{
	if( !pChar ) return;

	m_dwEnableReserveTime = 0;
	pChar->ClearSkillReserveKey();


	int iMaxCombo = m_vComboList.size();
	if( !COMPARE( nIndex, 0, iMaxCombo ) )
		return;

	//m_SkillState = SS_COMBO;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )//&& m_bUseChangeDirection )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = m_vComboList[m_iCurLinearCombo];

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	/*if( m_bNoGravityCombo )
	{
		pChar->SetGravityAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
		pChar->SetCurJumpPowerAmt( 0.0f );
	}*/

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
	}

	//if( m_bRefreshFireTimeList )
	{
		if( m_iCurLinearCombo == 0 )
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
			iAniID,
			kAttack.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	//
	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_bComboAttackReserve = false;

	if( pChar->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LINER_ACTION;

		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		kPacket << m_iCurLinearCombo;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}