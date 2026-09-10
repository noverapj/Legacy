

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioCircleMoveWeapon5Skill.h"

#include "WeaponDefine.h"

ioCircleMoveWeapon5Skill::ioCircleMoveWeapon5Skill()
{
	ClearData();
}

ioCircleMoveWeapon5Skill::ioCircleMoveWeapon5Skill( const ioCircleMoveWeapon5Skill &rhs )
	: ioNormalSkill( rhs ),
	m_dwFireGapTime( rhs.m_dwFireGapTime ),
	m_dwFireGapTime_Second( rhs.m_dwFireGapTime_Second ),
	m_fFireHightOffset( rhs.m_fFireHightOffset ),
	m_fFireFrontOffset( rhs.m_fFireFrontOffset ),
	m_iRandomRange( rhs.m_iRandomRange ),
	m_iRandomRange_second( rhs.m_iRandomRange_second ),
	m_iTotalFireCount( rhs.m_iTotalFireCount ),
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
	m_AttackAttribute( rhs.m_AttackAttribute ),
	m_AttackAttribute_Second( rhs.m_AttackAttribute_Second ),
	m_EndAttackAttribute( rhs.m_EndAttackAttribute ),
	m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
	m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
	m_dwRotateTime( rhs.m_dwRotateTime ),
	m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
	m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
	m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
	m_fWeaponGapDistance( rhs.m_fWeaponGapDistance ),
	m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
	m_fRevisionRate( rhs.m_fRevisionRate ),
	m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
	m_fUpLimitRange( rhs.m_fUpLimitRange ),
	m_fCircleSlowDownSpeedRate( rhs.m_fCircleSlowDownSpeedRate ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_fCharJumpPower( rhs.m_fCharJumpPower ),
	m_fJumpPowerInJump( rhs.m_fJumpPowerInJump ),
	m_fJumpAngle( rhs.m_fJumpAngle ),
	m_fJumpAngleInJump( rhs.m_fJumpAngleInJump ),
	m_fBulletSpeed( rhs. m_fBulletSpeed ),
	m_fCircleStartOffSet( rhs.m_fCircleStartOffSet ),
	m_fCirMaxRange( rhs.m_fCirMaxRange ),

	m_strPreActionAni_Normal( rhs.m_strPreActionAni_Normal ),
	m_fPreActionAniRate_Normal( rhs.m_fPreActionAniRate_Normal ),
	m_strPreActionAni_Jump( rhs.m_strPreActionAni_Jump ),
	m_fPreActionAniRate_Jump( rhs.m_fPreActionAniRate_Jump ),

	m_strActionAni_Normal( rhs.m_strActionAni_Normal ),
	m_fActionAniRate_Normal( rhs.m_fActionAniRate_Normal ),
	m_strActionAni_Jump( rhs.m_strActionAni_Jump ),
	m_fActionAniRate_Jump( rhs.m_fActionAniRate_Jump ),

	m_szSkillLoopMotion( rhs.m_szSkillLoopMotion ),
	m_fSkillLoopAniRate( rhs.m_fSkillLoopAniRate ),
	
	m_WeaponFireCountType( rhs.m_WeaponFireCountType ),
	m_dwMagicCircleFireGapTime( rhs.m_dwMagicCircleFireGapTime ),
	m_dwMagicCircleFireGapTime2( rhs.m_dwMagicCircleFireGapTime2 ),
	m_MagicCircleWeaponInfoList( rhs.m_MagicCircleWeaponInfoList ),
	m_szSkillZoneEffect( rhs.m_szSkillZoneEffect ),
	m_vZoneEffectOffset( rhs.m_vZoneEffectOffset ),
	m_bNotUsePreActionAni( rhs.m_bNotUsePreActionAni ),
	m_dwRepeatTime( rhs.m_dwRepeatTime ),
	m_szAreaWeapon( rhs.m_szAreaWeapon ),
	m_vAreaWeaponOffset( rhs.m_vAreaWeaponOffset ),
	m_bSetCreateAreaWeaponState( rhs.m_bSetCreateAreaWeaponState ),
	m_AreaWeaponAttribute( rhs.m_AreaWeaponAttribute ),
	m_JumpAreaWeaponAttribute( rhs.m_JumpAreaWeaponAttribute ),
	m_dwInputTime( rhs.m_dwInputTime ),
	m_SkillActionLoopUpMotion( rhs.m_SkillActionLoopUpMotion ),
	m_SkillActionLoopCenterMotion( rhs.m_SkillActionLoopCenterMotion ),
	m_SkillActionLoopDownMotion( rhs.m_SkillActionLoopDownMotion )
{
	ClearData();
}


ioCircleMoveWeapon5Skill::~ioCircleMoveWeapon5Skill()
{
}


void ioCircleMoveWeapon5Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_dwFireGapTime = (DWORD)rkLoader.LoadInt_e( "fire_gap_time", 0 );
	m_dwFireGapTime_Second = (DWORD)rkLoader.LoadInt_e( "second_fire_gap_time", 0 );
	m_fFireHightOffset	= rkLoader.LoadFloat_e( "fire_height_offset", 0.0f );
	m_fFireFrontOffset	= rkLoader.LoadFloat_e( "fire_front_offset", 0.0f );
	m_iRandomRange	= rkLoader.LoadFloat_e( "fire_random_range", 0.0f );
	m_iTotalFireCount	= rkLoader.LoadFloat_e( "fire_count", 0 );

	m_iRandomRange_second	= rkLoader.LoadFloat_e( "second_fire_random_range", 0.0f );

	m_fCircleRange		= rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed	= rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap	= rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet		= rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

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

	// Action Attack
	m_AttackAttribute.Init();
	m_AttackAttribute_Second.Init();
	m_AttackAttribute.m_AttackAnimation = m_SkillMotionName;
	m_AttackAttribute.m_fAttackAniRate  = m_fAnimationTimeRate;
	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay", 0 );

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_AttackAttribute.m_AttackEndAnimation = szBuf;
	m_AttackAttribute.m_fAttackEndAniRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

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

	iAttackTypeCnt = rkLoader.LoadInt_e( "second_attack_type_count", 0 );
	m_AttackAttribute_Second.m_vWeaponInfoList.clear();
	m_AttackAttribute_Second.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "second_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "second_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "second_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "second_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "second_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackAttribute_Second.m_vWeaponInfoList.push_back( kInfo );
	}

	//EndAttackAttribute
	int iEndAttackTypeCnt = rkLoader.LoadInt_e( "end_attack_type_count", 0 );
	m_EndAttackAttribute.m_vWeaponInfoList.clear();
	m_EndAttackAttribute.m_vWeaponInfoList.reserve( iEndAttackTypeCnt );

	for( int i=0 ; i<iEndAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "end_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "end_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "end_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "end_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "end_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_EndAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}

	// Character Rotate
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );

	// Weapon
	m_fWeaponGravityRate = rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "weapon_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "weapon_float_min_power", 0.0f );
	m_fWeaponMinSpeed = rkLoader.LoadFloat_e( "weapon_min_speed", 0.0f );
	m_fWeaponGapDistance = rkLoader.LoadFloat_e( "weapon_gap_distance", 0.0f );
	m_fRevisionRate = rkLoader.LoadFloat_e( "weapon_revision_rate", 0.0f );

	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fUpLimitRange = rkLoader.LoadFloat_e( "up_limit_range", 0.0f );

	m_fCircleSlowDownSpeedRate = rkLoader.LoadFloat_e( "magic_circle_slow_down_speed_rate", FLOAT1 );

	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	// yunga add
	//점프
	m_fCharJumpPower = rkLoader.LoadFloat_e( "jump_power", 0 );
	m_fJumpPowerInJump = rkLoader.LoadFloat_e( "jump_power_InJump", 0);

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "pre_skill_motion", "", szBuf, MAX_PATH );
	m_strPreActionAni_Normal = szBuf;
	m_fPreActionAniRate_Normal = rkLoader.LoadFloat_e( "pre_animation_time_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_pre_skill_motion", "", szBuf, MAX_PATH );
	m_strPreActionAni_Jump = szBuf;
	m_fPreActionAniRate_Jump = rkLoader.LoadFloat_e( "jump_pre_animation_time_rate", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////

	rkLoader.LoadString_e( "skill_motion", "", szBuf, MAX_PATH );
	m_strActionAni_Normal = szBuf;
	m_fActionAniRate_Normal = rkLoader.LoadFloat_e( "animation_time_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_skill_motion", "", szBuf, MAX_PATH );
	m_strActionAni_Jump = szBuf;
	m_fActionAniRate_Jump = rkLoader.LoadFloat_e( "jump_animation_time_rate", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////

	m_fJumpAngle = rkLoader.LoadFloat_e( "jump_angle", FLOAT1 );
	m_fJumpAngleInJump = rkLoader.LoadFloat_e( "jump_angle_InJump", FLOAT1 );

	m_fBulletSpeed = rkLoader.LoadFloat_e( "fire_move_speed", FLOAT1 );

	m_fCirMaxRange = rkLoader.LoadFloat_e( "magic_circle_max_range", FLOAT100 );
	m_fCircleStartOffSet = rkLoader.LoadFloat_e( "magic_circle_start_offset", 20.0f);

	//////////////////////////////////////////////////////////////////////////
	// Loop Motion
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szSkillLoopMotion = szBuf;
	m_fSkillLoopAniRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwSkillLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );
	
	//추가 yong jun
	LoadMagicCircleWeaponInfo( rkLoader );
	m_WeaponFireCountType = (WeaponFireCountType)rkLoader.LoadInt_e( "weapon_fire_count_type", WFCS_MULTI );
	m_bNotUsePreActionAni = rkLoader.LoadBool_e( "not_use_pre_skill_motion", false );
	//

	m_dwRepeatTime = rkLoader.LoadInt_e( "repeat_time", 0 );
	rkLoader.LoadString_e( "area_weapon", "", szBuf, MAX_PATH );
	m_szAreaWeapon = szBuf;
	m_vAreaWeaponOffset.x = rkLoader.LoadFloat_e( "area_weapon_offset_x", 0.0f );
	m_vAreaWeaponOffset.y = rkLoader.LoadFloat_e( "area_weapon_offset_y", 0.0f );
	m_vAreaWeaponOffset.z = rkLoader.LoadFloat_e( "area_weapon_offset_z", 0.0f );

	m_bSetCreateAreaWeaponState = rkLoader.LoadBool_e( "set_area_weapon_state", false );

	LoadAttackAttribute( "area_weapon_state", m_AreaWeaponAttribute, rkLoader );
	LoadAttackAttribute( "jump_area_weapon_state", m_JumpAreaWeaponAttribute, rkLoader );

	m_dwInputTime = rkLoader.LoadInt_e( "input_time", 0 );

	//스트라이더 아머( 유니크 장비 )
	rkLoader.LoadString_e( "skill_loop_up_motion", "", szBuf, MAX_PATH );
	m_SkillActionLoopUpMotion = szBuf;
	rkLoader.LoadString_e( "skill_loop_center_motion", "", szBuf, MAX_PATH );
	m_SkillActionLoopCenterMotion = szBuf;
	rkLoader.LoadString_e( "skill_loop_down_motion", "", szBuf, MAX_PATH );
	m_SkillActionLoopDownMotion = szBuf;
}


void ioCircleMoveWeapon5Skill::ClearData()
{
	m_State = SS_NONE;

	m_dwCurMapEffect		= -1;
	m_dwTargetCircleEffect	= -1;

	m_bSetHandMesh = false;
	m_bReduceGauge = false;

	m_dwFireStartTime		= 0;
	m_dwSkillLoopStartTime	= 0;
	m_dwNextFireStartTime	= 0;
	m_dwNextFireStartTime_Second = 0;

	m_iCurFireCount = 0;

	m_fFirstTargetDistance = 0.0f;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;
	m_dwCheckTime = 0;

	m_fPreActionAniRate = FLOAT1;
	m_fActionAniRate = FLOAT1;
	m_strPreActionAni.Clear();
	m_strActionAni.Clear();

	m_dwMagicCircleNextFireStartTime = 0;
	m_dwMagicCircleNextFireStartTime2 = 0;
	m_dwSingleWeaponIdx = 0;
	m_dwCircleWeaponIndex = 0;
	m_bCreateSingleWeapon = false;
	m_dwCheckRepeatTime = 0;
	m_dwCheckInputTime = 0;
}

ioSkill* ioCircleMoveWeapon5Skill::Clone()
{
	return new ioCircleMoveWeapon5Skill( *this );
}

bool ioCircleMoveWeapon5Skill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	ClearData();

	m_bSkillInJump = false;
	if( m_SkillUseType == SUT_JUMP )
		m_bSkillInJump = true;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	m_dwCheckRepeatTime = 0;

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

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		return false;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	/*if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}*/

	m_dwCheckTime = FRAMEGETTIME();

	// 성장관련 요소 추가
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

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	SetSkillNameChatBubble( pChar );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}


	if( !m_bSkillInJump )
	{
		m_strPreActionAni	= m_strPreActionAni_Normal;
		m_fPreActionAniRate = m_fPreActionAniRate_Normal;

		m_strActionAni		= m_strActionAni_Normal;
		m_fActionAniRate	= m_fActionAniRate_Normal;
	}
	else
	{
		m_strPreActionAni = m_strPreActionAni_Jump;
		m_fPreActionAniRate = m_fPreActionAniRate_Jump;

		m_strActionAni		= m_strActionAni_Jump;
		m_fActionAniRate	= m_fActionAniRate_Jump;
	}

	//추가 yong jun
	m_dwSingleWeaponIdx = pChar->IncreaseWeaponIndexBase();
	m_dwMagicCircleNextFireStartTime = 0;
	m_dwCircleWeaponIndex = pChar->IncreaseWeaponIndexBase();
	m_dwMagicCircleNextFireStartTime2 = 0;
	m_bCreateSingleWeapon = false;
	//

	return true;
}

void ioCircleMoveWeapon5Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	if ( m_WeaponFireCountType == WFCS_SINGLE )
		RemoveWeapon( pChar );

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

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;

	ClearData();
}

void ioCircleMoveWeapon5Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
	{
		fValue = pUpInfo->GetValue(pStage);
	}

	float fCurRange = m_fCircleRange + fValue;

	switch( m_State )
	{
	case SS_NONE://처음 점프를 하기 전까지(PreDelay)
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				if ( m_bSetCreateAreaWeaponState )
				{
					if ( m_bSkillInJump )
						SetCreateAreaWeaponState( pChar, m_JumpAreaWeaponAttribute );
					else
						SetCreateAreaWeaponState( pChar, m_AreaWeaponAttribute );
				}
				else
				{
					//추가 및 수정 yong jun
					if ( !m_bNotUsePreActionAni )
						SetPreAction( pChar );
					else
						SetNormalSkill( pChar );
					//
				}
			}
		}
		break;
	case SS_CREATE_AREA_WEAPON:
		{
			if( m_dwMotionEndTime < dwCurTime )
				SetPreAction( pChar );
			else
				CreateAreaWeapon( pChar, pStage );
		}
		break;
	case SS_PREACTION://점프중~
		{
			if( m_dwPreActionEndTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetNormalSkill( pChar );//Skill_motion
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{
			if( m_dwSkillLoopDuration > 0 && m_dwSkillLoopStartTime + m_dwSkillLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}
			
			UpdateCircle( pChar );
			//ProcessRotate( pChar );
			OnLoopState( pChar, pStage );

			//추가 및 수정 yong jun
			if ( m_WeaponFireCountType == WFCS_MULTI )
				AttackFire( pChar );
			/*else if ( m_WeaponFireCountType == WFCS_SINGLE )
			{
				AttackSingleFire( pChar );
				CheckRotate( pChar );
				CheckWeapon( pChar );
			}
			else if ( m_WeaponFireCountType == WFCS_INPUT )
			{
				CheckRotate( pChar );

				if ( pChar->IsNeedProcess() && !CheckInputAttack( pChar ) && m_dwCheckRepeatTime + m_dwRepeatTime < dwCurTime )
				{
					SetEndStatePacket( pChar );
					return;
				}
			}*/
			CheckRotate( pChar );

			MagicCircleFire( pChar );

			CheckMotion( pChar );
			//
		}
		break;
	case SS_END:
		break;
	}

	if( m_State != SS_END )
	{
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

			if( fCurTicGauge != 0.0f )
			{				
				if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
				{
					SetEndState( pChar );					
				}
			}
		}
	}
}

void ioCircleMoveWeapon5Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioSimpleBoxWeapon *pSimpleBox = ToSimpleBoxWeapon( pWeapon );
	ioAdhesiveMissileWeapon2 *pAdMissile2 = ToAdhesiveMissileWeapon2( pWeapon );
	ioBombMissileWeapon *pBombMissile =	ToBombMissileWeapon( pWeapon );

	if( pZoneEffect )
	{
		ioBaseChar *pChar = pZoneEffect->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( false, true );
		}
	}
	else if( pMissile )
	{
		D3DXVECTOR3 vNowPos = pMissile->GetPosition();
		D3DXVECTOR3 vDestPos = m_vWeaponMoveInfo_second.m_vMoveDir;

		D3DXVECTOR3 vDestDir = vDestPos - vNowPos;
		D3DXVec3Normalize( &vDestDir, &vDestDir );

		pMissile->SetMoveDir( vDestDir );
		//pMissile->SetMoveSpeed( m_fBulletSpeed );
	}
	else if ( pSimpleBox )
	{
		D3DXVECTOR3 vNowPos = pSimpleBox->GetPosition();
		D3DXVECTOR3 vDestPos = m_vWeaponMoveInfo.m_vMoveDir;

		D3DXVECTOR3 vDestDir = vDestPos - vNowPos;
		D3DXVec3Normalize( &vDestDir, &vDestDir );

		pSimpleBox->SetMoveDir( vDestDir );
		//pSimpleBox->SetMoveSpeed( m_fBulletSpeed );
	}
	else if( pAdMissile2 )
	{
		D3DXVECTOR3 vNowPos = pAdMissile2->GetPosition();
		D3DXVECTOR3 vDestPos = m_vWeaponMoveInfo.m_vMoveDir;

		D3DXVECTOR3 vDestDir = vDestPos - vNowPos;
		D3DXVec3Normalize( &vDestDir, &vDestDir );

		pAdMissile2->SetMoveDir( vDestDir );
	}
	else if( pBombMissile )
	{
		D3DXVECTOR3 vNowPos = pBombMissile->GetPosition();
		D3DXVECTOR3 vDestPos = m_vWeaponMoveInfo_second.m_vMoveDir;

		D3DXVECTOR3 vDestDir = vDestPos - vNowPos;
		D3DXVec3Normalize( &vDestDir, &vDestDir );

		pBombMissile->SetMoveDir( vDestDir );
		//pBombMissile->SetMoveSpeed( m_fBulletSpeed );
	}
	else
	{
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
		float fSpeed = 0.0f;
		float fFloat = 0.0f;

		pWeapon->SetMoveDir( m_vWeaponMoveInfo.m_vMoveDir );
		pWeapon->SetMoveSpeed( m_vWeaponMoveInfo.m_fMoveSpeed );
		pWeapon->SetFloatPower( m_vWeaponMoveInfo.m_fFloatPower );

		ioFloatThrowWeapon *pFloatWeapon = ToFloatThrowWeapon(pWeapon);
		if( pFloatWeapon )
		{
			pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
		}
	}
}


void ioCircleMoveWeapon5Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch ( nState )
	{
	case SS_CREATE_WEAPON:
		{
			rkPacket >> m_vCirclePos;
			rkPacket >> m_iCurFireCount;
			CreateWeapon( pChar );
		}
		break;
	case SS_END:
		{
			SetEndState( pChar );
		}
		break;
	}
}


bool ioCircleMoveWeapon5Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}


bool ioCircleMoveWeapon5Skill::IsProtected( int iDefenseBreakType ) const
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

	/*if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}*/


	if( m_State == SS_NONE || m_State == SS_PREACTION || m_State == SS_ACTION || m_State == SS_CREATE_AREA_WEAPON )
		return true;


	if( m_State == SS_LOOP )
	{
		if( m_dwSkillLoopProtectTime > 0 &&
			m_dwSkillLoopStartTime + m_dwSkillLoopProtectTime > FRAMEGETTIME() )
		{
			return true;
		}
	}

	return false;
}


bool ioCircleMoveWeapon5Skill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}


void ioCircleMoveWeapon5Skill::SetPreAction( ioBaseChar *pChar )
{
	EndPreDelayEffect( pChar );

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_strPreActionAni );
	if( iCurSkillAniID == -1 )	return;

	float fTimeRate = m_fPreActionAniRate;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	float fJumpAngle = m_bSkillInJump ?	m_fJumpAngleInJump : m_fJumpAngle;

	//////////////////////////////////////////////////////////////////////////
	float fAngleH = ioMath::ArrangeHead( 0 );
	float fAngleV = ioMath::ArrangeHead( fJumpAngle );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	vTargetDir = qtRot * vTargetDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	vTargetDir = qtRot * vTargetDir;
	//////////////////////////////////////////////////////////////////////////

	/*pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );*/

	if( !m_bSkillInJump )
		pChar->SetForcePower( vTargetDir, fabs(m_fCharJumpPower), FLOAT1, true );
	else
		pChar->SetForcePower( vTargetDir, fabs(m_fJumpPowerInJump), FLOAT1, true );

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwPreActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_State = SS_PREACTION;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}


void ioCircleMoveWeapon5Skill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	//ioHashString szSkillMotion = GetSkillMotionName();
	GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_strActionAni );
	if( iCurSkillAniID == -1 )	return;

	//float fAniRate = GetSkillMotionRate();

	float fTimeRate = m_fActionAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	pChar->AttachEffect( m_StartEffect );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_State = SS_ACTION;

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
}

void ioCircleMoveWeapon5Skill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_LOOP;

	//if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );

		D3DXVECTOR3 vDir =	pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vDir = vDir * m_fCircleStartOffSet;

		m_vCirclePos = pChar->GetMagicCirclePos();
		pChar->SetMagicCirclePos( false, m_vCirclePos + vDir );
	}

	m_vCirclePos = pChar->GetMagicCirclePos();

	m_dwSkillLoopStartTime = m_dwFireStartTime = FRAMEGETTIME();
	m_dwNextFireStartTime = m_dwFireStartTime + m_dwFireGapTime;
	m_dwNextFireStartTime_Second = m_dwNextFireStartTime;

	ioEntityGroup *pGrp = pChar->GetGroup();

	/*int iAniID = pGrp->GetAnimationIdx( m_szSkillLoopMotion );
	if( iAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fSkillLoopAniRate );*/

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	/*if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;*/

	pGrp->SetLoopAni( m_SkillActionLoopUpMotion,   0.0f, 0.0f, FLOAT1/fAniRate );
	pGrp->SetLoopAni( m_SkillActionLoopDownMotion, 0.0f, 0.0f, FLOAT1/fAniRate );
	pGrp->SetLoopAni( m_SkillActionLoopCenterMotion,     0.0f, FLOAT1, FLOAT1/fAniRate );


	m_dwMagicCircleNextFireStartTime = FRAMEGETTIME();
	m_dwMagicCircleNextFireStartTime2 = FRAMEGETTIME();
	m_dwCheckRepeatTime = FRAMEGETTIME();
}


void ioCircleMoveWeapon5Skill::SetEndState( ioBaseChar *pChar )
{
	m_State = SS_END;

	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackEndAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate = m_AttackAttribute.m_fAttackEndAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

void ioCircleMoveWeapon5Skill::OnLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	// 뱡향 변화 체크
	pChar->CheckChangeDirectionForMagicCircle();
	m_vCirclePos = pChar->GetMagicCirclePos();
	return;
}


void ioCircleMoveWeapon5Skill::UpdateCircle( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	//if( !pOwner->IsNeedProcess() ) return;

	pOwner->CheckChangeDirectionForMagicCircle();

	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	pOwner->UpdateMagicCircleToCirclePosAll( m_fCircleMoveSpeed, m_fCirMaxRange, m_fCircleOffSet, 0, 0, 0, vCharPos, false, false );
}

void ioCircleMoveWeapon5Skill::AttackFire( ioBaseChar *pChar )
{
	if( m_dwFireStartTime <= 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	//m_dwFireStartTime
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vFirePos = pChar->GetWorldPosition();
	vFirePos.y += m_fFireHightOffset;
	vFirePos += ( vDir * m_fFireFrontOffset );

	int iAttCnt = m_AttackAttribute.m_vWeaponInfoList.size();
	if( m_dwNextFireStartTime < dwCurTime )
	{
		for( int i=0; i<iAttCnt; i++ )
		{
			CheckWeaponFloatInfo( pChar );			

			pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList[i],
				GetName(),
				m_dwWeaponBaseIndex + m_iCurFireCount,
				vFirePos );

			m_iCurFireCount++;
		}		
		m_dwNextFireStartTime = dwCurTime + m_dwFireGapTime;
	}

	iAttCnt = m_AttackAttribute_Second.m_vWeaponInfoList.size();
	if( m_dwNextFireStartTime_Second < dwCurTime )
	{
		for( int i=0; i<iAttCnt; i++ )
		{
			CheckWeaponFloatInfo_Second( pChar );			

			pChar->SkillFireExplicit( m_AttackAttribute_Second.m_vWeaponInfoList[i],
				GetName(),
				m_dwWeaponBaseIndex + m_iCurFireCount,
				vFirePos );

			m_iCurFireCount++;
		}		
		m_dwNextFireStartTime_Second = dwCurTime + m_dwFireGapTime_Second;
	}

	/*if( m_iCurFireCount >= m_iTotalFireCount*iAttCnt )
	SetEndState( pChar );*/
}


float ioCircleMoveWeapon5Skill::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		//fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
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

float ioCircleMoveWeapon5Skill::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
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

void ioCircleMoveWeapon5Skill::CheckWeaponFloatInfo( ioBaseChar *pChar )
{
	// CharDir
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vCharDir = m_vCirclePos - vCharPos;
	vCharDir.y = 0.0f;

	float fFirstRange = D3DXVec3Length( &vCharDir );
	if( fFirstRange == 0 )
		return;

	D3DXVec3Normalize( &vCharDir, &vCharDir );

	// WeaponMove
	m_iCurWeaponCnt = 0;

	IORandom random;
	DWORD dwSeed = 0;
	int iRand = 0;
	dwSeed = pChar->GetRandomSeed();
	random.SetRandomSeed( dwSeed + m_iCurFireCount );

	int iRandX = random.Random( m_iRandomRange*2 ) - m_iRandomRange;
	int iRandZ = random.Random( m_iRandomRange*2 ) - m_iRandomRange;

	D3DXVECTOR3 vTargetPos = m_vCirclePos;

	vTargetPos.x += (float)iRandX;
	vTargetPos.z += (float)iRandZ;

	float fGapDistance = m_fWeaponGapDistance;
	D3DXVECTOR3 vPos = vTargetPos + (vCharDir * fGapDistance);
	D3DXVECTOR3 vDir = vPos - vCharPos;
	
	m_vWeaponMoveInfo.m_vMoveDir = vPos;
	//m_vWeaponMoveInfo.m_fMoveSpeed = m_fBulletSpeed;
	
	return;
}


void ioCircleMoveWeapon5Skill::CheckWeaponFloatInfo_Second( ioBaseChar *pChar )
{
	// CharDir
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vCharDir = m_vCirclePos - vCharPos;
	vCharDir.y = 0.0f;

	float fFirstRange = D3DXVec3Length( &vCharDir );
	if( fFirstRange == 0 )
		return;

	D3DXVec3Normalize( &vCharDir, &vCharDir );

	// WeaponMove
	m_iCurWeaponCnt = 0;

	IORandom random;
	DWORD dwSeed = 0;
	int iRand = 0;
	dwSeed = pChar->GetRandomSeed();
	random.SetRandomSeed( dwSeed + m_iCurFireCount );

	int iRandX = random.Random( m_iRandomRange_second*2 ) - m_iRandomRange_second;
	int iRandZ = random.Random( m_iRandomRange_second*2 ) - m_iRandomRange_second;

	D3DXVECTOR3 vTargetPos = m_vCirclePos;

	vTargetPos.x += (float)iRandX;
	vTargetPos.z += (float)iRandZ;

	float fGapDistance = m_fWeaponGapDistance;
	D3DXVECTOR3 vPos = vTargetPos + (vCharDir * fGapDistance);
	D3DXVECTOR3 vDir = vPos - vCharPos;

	m_vWeaponMoveInfo_second.m_vMoveDir = vPos;
	//m_vWeaponMoveInfo.m_fMoveSpeed = m_fBulletSpeed;

	return;
}

void ioCircleMoveWeapon5Skill::GenerateEndAttack( const D3DXVECTOR3 &vPos, ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	int iAttCnt = m_EndAttackAttribute.m_vWeaponInfoList.size();
	for( int i=0; i<iAttCnt; i++ )
	{
		pOwner->SkillFireExplicit( m_EndAttackAttribute.m_vWeaponInfoList[i],
			GetName(),
			m_dwWeaponBaseIndex+i,
			vPos );
	}		
}


bool ioCircleMoveWeapon5Skill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}


bool ioCircleMoveWeapon5Skill::IsNoDropState() const
{
	if( m_State == SS_ACTION || m_State == SS_LOOP )
		return true;

	return false;
}


bool ioCircleMoveWeapon5Skill::IsAttackEndState() const
{
	return IsEndState();
}


bool ioCircleMoveWeapon5Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_State == SS_END && m_fSkillEndNextJumpAmt == 0.0f )
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//추가 yong jun
void ioCircleMoveWeapon5Skill::LoadMagicCircleWeaponInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	int iAttackTypeCnt = rkLoader.LoadInt_e( "magic_circle_attack_type_count", 0 );
	m_MagicCircleWeaponInfoList.clear();
	m_MagicCircleWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "magic_circle_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "magic_circle_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "magic_circle_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "magic_circle_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "magic_circle_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_MagicCircleWeaponInfoList.push_back( kInfo );
	}

	m_dwMagicCircleFireGapTime = (DWORD)rkLoader.LoadInt_e( "magic_circle_fire_gap_time", 0 );
	m_dwMagicCircleFireGapTime2 = (DWORD)rkLoader.LoadInt_e( "magic_circle_fire_gap_time2", 0 );

	rkLoader.LoadString_e( "circle_zone_effect", "", szBuf, MAX_PATH );
	m_szSkillZoneEffect = szBuf;

	m_vZoneEffectOffset.x = rkLoader.LoadFloat_e( "circle_zone_effect_offset_x", 0.0f );
	m_vZoneEffectOffset.y = rkLoader.LoadFloat_e( "circle_zone_effect_offset_y", 0.0f );
	m_vZoneEffectOffset.z = rkLoader.LoadFloat_e( "circle_zone_effect_offset_z", 0.0f );

}
void ioCircleMoveWeapon5Skill::CheckRotate( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	//마법진 방향으로 캐릭터 회전
	pChar->SetTargetRotToTargetPos( m_vCirclePos, true, false );
}

void ioCircleMoveWeapon5Skill::MagicCircleFire( ioBaseChar *pChar )
{
	if ( !pChar || m_MagicCircleWeaponInfoList.empty() )
		return;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if ( !pStage )
		return;
	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCirclePos = m_vCirclePos;
	vCirclePos += m_vZoneEffectOffset;
	float fHeight = pStage->GetMapHeightNoneEntity( vCirclePos.x, vCirclePos.z);
	if ( fHeight <= 0)
		return;
	int iAttCnt = m_MagicCircleWeaponInfoList.size();
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

	if( m_dwMagicCircleNextFireStartTime < dwCurTime )
	{
		for( int i=0; i<iAttCnt; i++ )
		{
			pChar->SkillFireExplicit( m_MagicCircleWeaponInfoList[i],
				GetName(),
				m_dwCircleWeaponIndex++,
				vCirclePos );
		}
		m_dwMagicCircleNextFireStartTime = dwCurTime + m_dwMagicCircleFireGapTime;
		if ( !m_szSkillZoneEffect.IsEmpty() )
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szSkillZoneEffect, vCirclePos, vScale );
	}


	if( m_dwMagicCircleNextFireStartTime2 < dwCurTime )
	{
		/*for( int i=0; i<iAttCnt; i++ )
		{
			pChar->SkillFireExplicit( m_MagicCircleWeaponInfoList[i],
				GetName(),
				m_dwCircleWeaponIndex++,
				vCirclePos );
		}*/
		m_dwMagicCircleNextFireStartTime2 = dwCurTime + m_dwMagicCircleFireGapTime2;
	}
}

void ioCircleMoveWeapon5Skill::RemoveWeapon( ioBaseChar *pChar )
{
	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwSingleWeaponIdx );
	if( pWeapon && pWeapon->IsLive() )
	{
		pWeapon->SetWeaponDead();
	}
}

void ioCircleMoveWeapon5Skill::CheckWeapon( ioBaseChar *pChar )
{
	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwSingleWeaponIdx );
	if ( !pWeapon)
		return;
	ioSimpleBoxWeapon* pSimple = ToSimpleBoxWeapon( pWeapon );
	if( !pSimple )
	{
		SetEndState( pChar );
		return;
	}

	pSimple->SetPointDir( pChar->GetMagicCirclePos() );		

}

void ioCircleMoveWeapon5Skill::AttackSingleFire( ioBaseChar *pChar )
{
	//한번만 들어오는 함수.
	if( m_dwFireStartTime <= 0 || m_bCreateSingleWeapon ) return;
	if ( m_AttackAttribute.m_vWeaponInfoList.empty() )
		return;
	DWORD dwCurTime = FRAMEGETTIME();

	//m_dwFireStartTime
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vFirePos = pChar->GetWorldPosition();
	vFirePos.y += m_fFireHightOffset;
	vFirePos += ( vDir * m_fFireFrontOffset );
	
	int iAttCnt = m_AttackAttribute.m_vWeaponInfoList.size();
	for( int i=0; i<iAttCnt; i++ )
	{
		pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList[i],
			GetName(),
			m_dwSingleWeaponIdx,
			vFirePos );

	}		
	m_bCreateSingleWeapon = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioCircleMoveWeapon5Skill::CheckInputAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckInputTime + m_dwInputTime < dwCurTime && pOwner->IsAttackKey() )
	{
		CreateWeapon( pOwner );
		m_dwCheckInputTime = dwCurTime;
		return true;
	}

	return false;
}

void ioCircleMoveWeapon5Skill::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vFirePos = pOwner->GetWorldPosition();
	vFirePos.y += m_fFireHightOffset;
	vFirePos += ( vDir * m_fFireFrontOffset );

	int nCurFireCount = m_iCurFireCount;
	int iAttCnt = m_AttackAttribute.m_vWeaponInfoList.size();
	for( int i=0; i<iAttCnt; i++ )
	{
		m_vWeaponMoveInfo.m_vMoveDir = m_vCirclePos;
		pOwner->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList[i], GetName(), m_dwWeaponBaseIndex + m_iCurFireCount, vFirePos );
		m_iCurFireCount++;
	}		

	m_dwCheckRepeatTime = FRAMEGETTIME();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SS_CREATE_WEAPON;
		kPacket << m_vCirclePos;
		kPacket << nCurFireCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMoveWeapon5Skill::SetEndStatePacket( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	SetEndState( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
	
void ioCircleMoveWeapon5Skill::CreateAreaWeapon( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage || m_szAreaWeapon.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckRepeatTime == 0 || m_dwCheckRepeatTime > dwCurTime )
		return;

	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + qtRot * m_vAreaWeaponOffset;

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( fMapHeight > 0.0f )
		vPos.y = fMapHeight;

	g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_szAreaWeapon, vPos, qtRot, ioAreaWeapon::CT_NORMAL );

	m_dwCheckRepeatTime = 0;
}

void ioCircleMoveWeapon5Skill::SetCreateAreaWeaponState( ioBaseChar *pOwner, const AttackAttribute& Attribute )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( Attribute.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate = Attribute.m_fAttackAniRate;
	DWORD dwPreDelay = Attribute.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex, nAniID, Attribute.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate + dwPreDelay;

	m_dwCheckRepeatTime = dwCurTime;
	m_dwCheckRepeatTime += pGrp->GetAnimationEventTime_e( nAniID, "fire_s" ) * fTimeRate;

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( Attribute.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	m_State = SS_CREATE_AREA_WEAPON;

	if( !m_bReduceGauge )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
		m_bReduceGauge = true;
	}
}


void ioCircleMoveWeapon5Skill::CheckMotion( ioBaseChar* pChar )
{
	if( !pChar )
		return;

	/*m_vAttackDir = pChar->GetMagicCirclePos() - pChar->GetWorldPosition();
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );*/

	D3DXVECTOR3 vAttackDir = pChar->GetMagicCirclePos() - pChar->GetMidPositionByRate();
	D3DXVec3Normalize( &vAttackDir, &vAttackDir );
	//check sight direction
	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	float fAngle = 0.0f;
	float fDotValue = D3DXVec3Dot( &vAttackDir, &vSightDir );
	fAngle = RADtoDEG( acosf(fDotValue) );

	//Check motion	
	float fCurMotionRate = fAngle /FLOAT90;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioAnimation *pCenter, *pDown ,*pUp;
	pUp = pCenter = pDown = NULL;

	pUp = pGrp->GetLoopInPlay( m_SkillActionLoopUpMotion );
	pCenter = pGrp->GetLoopInPlay( m_SkillActionLoopCenterMotion );
	pDown = pGrp->GetLoopInPlay( m_SkillActionLoopDownMotion );

	if( !pCenter || !pDown || !pUp )
	{
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "ioLaserControlSkill::CheckMotion - Motion Error" );
#endif
		return;
	}

	if( vAttackDir.y == 0.0f || fCurMotionRate == 0.0f )		// center
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 );
		pDown->ForceWeight( 0.0f );
	}
	else if( vAttackDir.y > 0.0f )								// center & up
	{
		pUp->ForceWeight( fCurMotionRate );
		pCenter->ForceWeight( FLOAT1 - fCurMotionRate );
		pDown->ForceWeight( 0.0f );
	}
	else															// center & down
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fCurMotionRate );
		pDown->ForceWeight( fCurMotionRate );
	}
}