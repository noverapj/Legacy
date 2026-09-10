

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioCircleMoveAttackSkill4.h"

#include "WeaponDefine.h"

ioCircleMoveAttackSkill4::ioCircleMoveAttackSkill4()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_dwWaitStartTime = 0;
	m_bTargetRot = false;
}

ioCircleMoveAttackSkill4::ioCircleMoveAttackSkill4( const ioCircleMoveAttackSkill4 &rhs )
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

 m_AttackAttribute_A( rhs.m_AttackAttribute_A ),
 m_AttackAttribute_S( rhs.m_AttackAttribute_S ),

 m_stSkillMotion_A( rhs.m_stSkillMotion_A ),
 m_fSkillMotionRate_A( rhs.m_fSkillMotionRate_A ),
 m_fCircleLandOffsetHeight( rhs.m_fCircleLandOffsetHeight ),
 
 m_stSkillMotion_S( rhs.m_stSkillMotion_S ),
 m_fSkillMotionRate_S( rhs.m_fSkillMotionRate_S ),

 m_stSkillMotion_D( rhs.m_stSkillMotion_D ),
 m_fSkillMotionRate_D( rhs.m_fSkillMotionRate_D ),
 m_DummyCharList( rhs.m_DummyCharList ),
 m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),

 m_stMapEffect( rhs.m_stMapEffect ),
 m_vEffectScale( rhs.m_vEffectScale ),

 m_fSkillEndJumpPower( rhs.m_fSkillEndJumpPower )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_dwWaitStartTime = 0;
	m_bTargetRot = false;

	m_dwFlyMoveEffect = 0;
}

ioCircleMoveAttackSkill4::~ioCircleMoveAttackSkill4()
{
}

void ioCircleMoveAttackSkill4::LoadProperty( ioINILoader &rkLoader )
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
	m_stSkillMotion_A = szBuf;
	m_fSkillMotionRate_A = rkLoader.LoadFloat_e( "animation_time_rate_a", FLOAT1 );

	m_AttackAttribute_A.Init();
	m_AttackAttribute_A.m_AttackAnimation = m_stSkillMotion_A;
	m_AttackAttribute_A.m_fAttackAniRate  = m_fSkillMotionRate_A;
	m_AttackAttribute_A.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay_a", 0 );
	m_AttackAttribute_A.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay_a", 0 );

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
	m_AttackAttribute_A.m_vWeaponInfoList.clear();
	m_AttackAttribute_A.m_vWeaponInfoList.reserve( iAttackTypeCnt );

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

		m_AttackAttribute_A.m_vWeaponInfoList.push_back( kInfo );
	}

	m_fCircleLandOffsetHeight = rkLoader.LoadFloat_e( "circle_land_offset_height", 0.0f );
	//////////////////////////////////////////////////////////////////////////
	// Action Attack _ S
	rkLoader.LoadString_e( "skill_motion_s", "", szBuf, MAX_PATH );
	m_stSkillMotion_S = szBuf;
	m_fSkillMotionRate_S = rkLoader.LoadFloat_e( "animation_time_rate_s", FLOAT1 );

	m_AttackAttribute_S.Init();
	m_AttackAttribute_S.m_AttackAnimation = m_stSkillMotion_S;
	m_AttackAttribute_S.m_fAttackAniRate  = m_fSkillMotionRate_S;
	m_AttackAttribute_S.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay_s", 0 );
	m_AttackAttribute_S.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay_s", 0 );

	iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count_s", 0 );
	m_AttackAttribute_S.m_vWeaponInfoList.clear();
	m_AttackAttribute_S.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d_s", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist_s", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded_s", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time_s", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani_s", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackAttribute_S.m_vWeaponInfoList.push_back( kInfo );
	}


	//////////////////////////////////////////////////////////////////////////
	// Action Attack _ D
	rkLoader.LoadString_e( "skill_motion_d", "", szBuf, MAX_PATH );
	m_stSkillMotion_D = szBuf;
	m_fSkillMotionRate_D = rkLoader.LoadFloat_e( "animation_time_rate_d", FLOAT1 );

	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "target_effect", "", szBuf, MAX_PATH );
	m_stMapEffect = szBuf;
	m_vEffectScale.x = rkLoader.LoadFloat_e( "target_effect_scale", FLOAT1 );
	m_vEffectScale.y = rkLoader.LoadFloat_e( "target_effect_scale", FLOAT1 );
	m_vEffectScale.z = rkLoader.LoadFloat_e( "target_effect_scale", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	m_fSkillEndJumpPower = rkLoader.LoadFloat_e( "last_jump_power", FLOAT1 );
}

ioSkill* ioCircleMoveAttackSkill4::Clone()
{
	return new ioCircleMoveAttackSkill4( *this );
}

bool ioCircleMoveAttackSkill4::OnSkillStart( ioBaseChar *pChar )
{	
	// 성장관련 요소
	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;
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

	SetWaitState( pChar );

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

void ioCircleMoveAttackSkill4::OnSkillEnd( ioBaseChar *pChar )
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
	/*if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );*/

	if( m_dwTargetCircleEffect != -1 )
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;
}

void ioCircleMoveAttackSkill4::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	/*float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;*/

	if( !pChar ) return;

	float fCurRange = m_fCircleRange;
	int iSlot = GetOwnerItem()->GetType() - 1;

	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_State )
	{
	case SS_WAIT:
		{
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );
			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );
			//////////////////////////////////////////////////////////////////////////
			if( pChar->IsNeedProcess() && m_dwWaitStartTime+m_dwWaitLimitTime < dwCurTime )
			{
				EndPreDelayEffect( pChar );
				SetActionState( pChar, pStage, IT_D );
			}
			else if( pChar->IsNeedProcess() && pChar->IsJumpKey() )
			{
				EndPreDelayEffect( pChar );
				SetActionState( pChar, pStage, IT_A );
			}
			else if( pChar->IsNeedProcess() && pChar->IsDefenseKey() )
			{
				EndPreDelayEffect( pChar );
				SetActionState( pChar, pStage, IT_S );
			}
			else if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				EndPreDelayEffect( pChar );
				SetActionState( pChar, pStage, IT_D );
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
				SetEndState(pChar);
		}
		break;
	case SS_END:
		{
			ProcessEnd( pChar );
		}
		break;
	}

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );

	/*if( m_State != SS_END )
	{
		//첫번째 타격 이후는 게이지 감소를 하지 않음
		//if( m_dwSetLoopCnt <= 0 )
		{
			if( iTickGap >= iCurTicTime )
			{
				m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

				if( fCurTicGauge != 0.0f )
				{				
					if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
					{
						if( //m_SkillState != SS_ZERO_AIM_FIRE && 
							m_State != SS_ACTION &&
							m_State != SS_END )
						{
							SetEndState( pChar );
						}
					}
				}
			}
		}
	}*/
}

bool ioCircleMoveAttackSkill4::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCircleMoveAttackSkill4::SetNormalSkill( ioBaseChar *pChar, InputType inputType, ioPlayStage *pStage )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = -1;
	float fAniRate = FLOAT1;

	if( inputType == IT_A )
	{
		iCurSkillAniID = pGrp->GetAnimationIdx( m_stSkillMotion_A );
		fAniRate = m_fSkillMotionRate_A;
	}
	else if( inputType == IT_S )
	{
		iCurSkillAniID = pGrp->GetAnimationIdx( m_stSkillMotion_S );
		fAniRate = m_fSkillMotionRate_S;
	}
	else if( inputType == IT_D )
	{
		iCurSkillAniID = pGrp->GetAnimationIdx( m_stSkillMotion_D );
		fAniRate = m_fSkillMotionRate_D;
	}

	if( iCurSkillAniID == -1 )
		return;

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime;
	m_dwFireMotionEndTime = m_dwMotionStartTime;
	m_dwFireStartTime = 0;

	m_dwFireMotionEndTime +=  pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	// Attack
	if( inputType == IT_A )
	{
		if( !m_AttackAttribute_A.m_vWeaponInfoList.empty() )
		{
			pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
				iCurSkillAniID,
				m_AttackAttribute_A.m_vWeaponInfoList,
				FTT_SKILL_ATTACK,
				fTimeRate,
				m_AttackAttribute_A.m_dwPreDelay );
		}
	}
	else if( inputType == IT_S )
	{
		if( !m_AttackAttribute_S.m_vWeaponInfoList.empty() )
		{
			pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
				iCurSkillAniID,
				m_AttackAttribute_S.m_vWeaponInfoList,
				FTT_SKILL_ATTACK,
				fTimeRate,
				m_AttackAttribute_S.m_dwPreDelay );
		}
	}
	else if( inputType == IT_D )
	{
		g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vSideDir;
		D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSideDir, &vSideDir );

		int iSize = 0;
		if( pChar->IsNeedProcess() )
		{
			iSize = m_DummyCharList.size();
			m_vDummyCreateList.clear();
		}
		else
		{
			iSize = m_vDummyCreateList.size();
		}
		
		for( int i=0; i < iSize; ++i )
		{
			ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;

			int iCurIndex = 0;
			if( pChar->IsNeedProcess() )
				g_DummyCharMgr.GetCurLocalIndex();
			else
				iCurIndex = m_vDummyCreateList[i].m_iDummyCharIdx;

			float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
			float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
			float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

			D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition() + (fOffset * vDir);
			vTargetPos += fSideOffset * vSideDir;

			if( !pChar->IsNeedProcess() )
				vTargetPos = m_vDummyCreateList[i].m_vTargetPos;

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
														   pChar->GetCharName(),
														   vTargetPos,
														   fStartAngle,
														   0, true );

			if( pDummy )
			{
				pDummy->SetControlSkillName( GetName() );
				D3DXVECTOR3 vCircleOffsetPos = m_vCirclePos;// + D3DXVECTOR3( 0, m_fCircleLandOffsetHeight, 0 );
				D3DXVECTOR3 vLookDir;
				vLookDir = vCircleOffsetPos - vTargetPos;
				D3DXVec3Normalize( &vLookDir, &vLookDir );

				D3DXQUATERNION qtTargetRot;
				ioMath::CalcDirectionQuaternion( qtTargetRot, -vLookDir );
				pDummy->SetWorldOrientationAndDir( qtTargetRot );
			}
			

			DummyCharCreateInfo kInfo;
			kInfo.m_DummyCharName = szDummyCharName;
			kInfo.m_iDummyCharIdx = iCurIndex;
			kInfo.m_fDummyCharStartAngle = fStartAngle;
			kInfo.m_vTargetPos = vTargetPos;

			if( pChar->IsNeedProcess() )
				m_vDummyCreateList.push_back( kInfo );
		}
	}

	m_dwMotionEndTime  += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	pChar->AttachEffect( m_StartEffect );

	if( m_State != SS_END )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
}

void ioCircleMoveAttackSkill4::SetWaitState( ioBaseChar *pChar )
{
	m_dwWaitStartTime = FRAMEGETTIME();
	m_State = SS_WAIT;

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}

	if( m_WaitType == WT_IMMEDIATE )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( 0.0f, m_fCircleRange, m_fCircleHeightGap, false );
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

void ioCircleMoveAttackSkill4::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, InputType inputType )
{
	m_State = SS_ACTION;

	float fCurRange = m_fCircleRange;
	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}

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
	/*m_vCharMoveDir = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		fRate = fRange / m_fCircleRange;

		D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
		pChar->SetTargetRotAndMoveDirChange( qtRot, false );
		pChar->SetWorldOrientation( qtRot );
	}*/

	if( pChar->IsNeedProcess() )
		m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar, inputType, pStage );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if(pChar->IsNeedProcess() &&  GetOwnerItem() )
		pChar->SetCurItemSkillGauge(GetOwnerItem()->GetName() , 0.0f );

	if( pChar->IsNeedProcess() )
	{
		int iCreateCnt = m_vDummyCreateList.size();

		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;

		kPacket << m_vCirclePos;

		kPacket << (int)inputType;
		kPacket << pChar->GetTargetRot();

		kPacket << iCreateCnt;

		for( int i=0; i < iCreateCnt; ++i )
		{
			kPacket << m_vDummyCreateList[i].m_DummyCharName;
			kPacket << m_vDummyCreateList[i].m_iDummyCharIdx;
			kPacket << m_vDummyCreateList[i].m_fDummyCharStartAngle;
			kPacket << m_vDummyCreateList[i].m_vTargetPos;
		}

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

	m_vDummyCreateList.clear();
}

void ioCircleMoveAttackSkill4::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iCreateCnt;
			rkPacket >> iCreateCnt;
			//m_vDummyCreateList.reserve( iCreateCnt 
			m_vDummyCreateList.clear();
			DummyCharCreateInfo Info;
			for( int i=0; i < iCreateCnt; ++i )
			{
				rkPacket >> Info.m_DummyCharName >> Info.m_iDummyCharIdx;
				rkPacket >> Info.m_fDummyCharStartAngle;
				rkPacket >> Info.m_vTargetPos;

				m_vDummyCreateList.push_back( Info );
			}

			SetActionState( pChar, pStage, (InputType)iInputType );
		}
		break;
	case SSC_DIR:
		{
			int iCurKey;
			rkPacket >> iCurKey;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iCurKey;
		}
		break;
	}
}

bool ioCircleMoveAttackSkill4::IsEndState() const
{
	if( m_State == SS_END )
		return true;
	return false;
}

void ioCircleMoveAttackSkill4::SetEndState( ioBaseChar *pChar )
{
	m_State = SS_END;
	DWORD dwCurTime = FRAMEGETTIME();

	//DestroyMapEffect
	/*if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );*/

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
}

bool ioCircleMoveAttackSkill4::IsProtected( int iDefenseBreakType ) const
{
	/*if( m_bDisableSkillProtect )
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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_State != SS_END )
			return true;

		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}*/

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireMotionEndTime && dwCurTime < m_dwFireMotionEndTime )
		return true;

	return false;
}

bool ioCircleMoveAttackSkill4::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioCircleMoveAttackSkill4::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCircleMoveAttackSkill4::ProcessAction( ioBaseChar *pChar )
{
	/*if( !pChar ) return;
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

	}*/
}

void ioCircleMoveAttackSkill4::ProcessEnd( ioBaseChar *pChar )
{
	/*if( !pChar ) return;
	if( m_State != SS_END ) return;
	if( m_dwFireStartTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireStartTime < FRAMEGETTIME() )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		pChar->SkillFireExplicit( m_LandAttackAttribute.m_vWeaponInfoList.front(),
								  GetName(),
								  m_dwWeaponBaseIndex,
								  vPos );

		m_dwFireStartTime = 0;

	}*/
}

void ioCircleMoveAttackSkill4::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

	/*if( pArrowWeapon )
	{
		if( m_iMaxWeaponWay <= m_iWeaponWay )
			return;

		float fCurAngle = 0.0f;
		fCurAngle = m_BulletInfoList[m_iWeaponWay].m_fAngle;

		fCurAngle = ioMath::ArrangeHead( fCurAngle );
		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		vMoveDir = pArrowWeapon->GetMoveDir();

		vMoveDir = qtAngle * vMoveDir;

		pArrowWeapon->SetMoveDir( vMoveDir );

		m_iWeaponWay++;
	}*/

}


SkillCharColType ioCircleMoveAttackSkill4::IsCharSkipState() const
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

void ioCircleMoveAttackSkill4::CheckCharDirKey( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_DIR;
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioCircleMoveAttackSkill4::CheckCharRotate( ioBaseChar *pChar )
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

bool ioCircleMoveAttackSkill4::IsJumpSkill()
{
	return m_bSkillInJump;
}


int ioCircleMoveAttackSkill4::GetState()
{
	return m_State;
}

float ioCircleMoveAttackSkill4::GetEndJumpPower()
{
	return m_fSkillEndJumpPower;
}