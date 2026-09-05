
#include "stdafx.h"

#include "ioGaugeHelper.h"
#include "WeaponDefine.h"
#include "SkillDefine.h"
#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

ioDummyChar::ioDummyChar( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioPlayEntity( pGrp, pMode )
{
	m_State = DCS_NONE;
	m_TraceState = DCTS_NONE;

	m_dwCreateTime = FRAMEGETTIME();
	m_dwProcessTime = FRAMEGETTIME();

	m_fGravityAmt = 0.0f;
	m_fCurJumpPowerAmt = 0.0f;
	m_dwCheckHPTime = 0;

	m_dwStartAniEndTime = 0;
	m_dwAttackAniEndTime = 0;

	m_pSpecialBox = NULL;

	m_iReserveAttackCode = 0;

	m_vForceDir = ioMath::UNIT_Z;
	m_fForceAmt = 0.0f;
	m_fForceFrictionRate = FLOAT1;
	m_bForcePowerLinear = false;

	m_bMale = false;

	m_dwExtraAniJumpTime = 0;
	m_fExtraAniJumpPower = 0.0f;

	m_iCurExtraAniJump = 0;
	m_vExtraAniJumpList.clear();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwAttackKeyCheckTime = 0;
	m_iChargeWeaponIndex = 0;

	m_dwTeamEffect = -1;

	m_fStartAngle = 0.0f;

	m_dwSensingStartTime = 0;
	m_dwTracingEndTime = 0;

	m_dwNextFireTime = 0;

	m_bPiercingProtected = false;
	m_bGrowthUp = true;

	m_PierceStartSoundID = 0;
	m_bAreadyPierceStart = false;

	m_PreEffectID = -1;

	m_nAttackTowerAttackCnt = 0;
	m_dwAttackTowerTime = FRAMEGETTIME();

	m_bCurseDollLandState = false;

	m_dwLifeTime = 0;
	m_dwStartTime = 0;

	m_dwDropZoneCheckStartTime = 0;

	m_fJumpSpeed = 0.0f;
	m_bTraceOwnerSync = false;
	m_bCurMountEnable = true;
	m_bAlreadyCallEndWeapon = false;
	m_iCurDummySkillAniCode = 0;
	m_bIsNoCheckPreCollision = false;
}

ioDummyChar::~ioDummyChar()
{
	SAFEDELETE( m_pGauge );
	SAFEDELETE( m_pSpecialBox );

	m_vMountEntityList.clear();
	m_vMountWeaponList.clear();

	m_DummyAttackMap.clear();

	if( m_dwTeamEffect != -1 )
	{
		if( m_pModeParent && m_pModeParent->GetCreator() )
		{
			ioMapEffect *pMapEffect = m_pModeParent->GetCreator()->FindMapEffect( m_dwTeamEffect );
			if( pMapEffect )
			{
				pMapEffect->EndEffectForce();
			}
		}

		m_dwTeamEffect = -1;
	}

	if( m_DummyCharType	== 	DCT_TARGET_TRACE4 )
	{
		ioBaseChar *pOwner = GetOwner();
		if( !m_stNoOverLapTargetName.IsEmpty() && pOwner )
			g_DummyCharMgr.EraseNoOverLapTrackingDummy( pOwner->GetCharName(), m_stNoOverLapTargetName );
	}
}

void ioDummyChar::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "", szKey[MAX_PATH];

	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;
	m_DummyCharType = (DummyCharType)rkLoader.LoadInt_e( "dummy_char_type", DCT_OWNER_TRACE );

	// status
	m_ArmorType	= (ArmorType)rkLoader.LoadInt_e( "armor_type", AT_UNARMOR );	

	m_bDisableHPVisible = rkLoader.LoadBool_e( "disable_gauge_visible", false );
	m_eEntityColSkip = (EntityColSkip)rkLoader.LoadInt_e( "entity_col_skip", ECS_NONE );
	m_CharColSkip = (CharColType)rkLoader.LoadInt_e( "char_col_skip", CCT_NONE );
	m_DummyCharColSkip = (DummyCharColSkip)rkLoader.LoadInt_e( "dummy_char_col_skip", DCCS_NONE );

	m_HP.m_fMaxValue = rkLoader.LoadFloat_e( "max_hp", FLOAT10 );
	m_HP.m_fCurValue = m_HP.m_fMaxValue;

	m_HP.m_fRecovery = rkLoader.LoadFloat_e( "hp_recovery", 0.0f );
	m_HP.m_dwRecoveryTick = rkLoader.LoadInt_e( "hp_recovery_tick", 0 );

	m_dwTicTime = rkLoader.LoadInt_e( "hp_decrease_tick", 0 );
	m_dwTicTimeByTraining = rkLoader.LoadInt_e( "hp_decrease_training_tick", 0 );
	m_fDecreaseHP = rkLoader.LoadFloat_e( "hp_decrease", 0.0f );

	m_fGravityRate = rkLoader.LoadFloat_e( "dummy_gravity_rate", FLOAT1 );
	m_fControlOutRange = rkLoader.LoadFloat_e( "control_out_range", 0.0f );
	m_fControlMinRange = rkLoader.LoadFloat_e( "control_min_range", 0.0f );

	rkLoader.LoadString_e( "block_buff", "", szBuf, MAX_PATH );
	m_BlockBuff = szBuf;

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedTeamEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueTeamEffect = szBuf;

	m_bNoDamage = rkLoader.LoadBool_e( "no_damage", false );

	m_bDestroyMapCol = rkLoader.LoadBool_e( "destroy_map_col", false );
	m_bOwnerRotate = rkLoader.LoadBool_e( "owner_rotate", false );
	m_bOneTimeAttack = rkLoader.LoadBool_e( "one_time_attack", false );

	// attack
	m_fWeaponOffset_Front_Back = rkLoader.LoadFloat_e("dummy_attack_offset_front_back", 0.f );
	m_fWeaponOffset_Up_Down = rkLoader.LoadFloat_e( "dummy_attack_offset_up_down", 0.0f );

	int iAttackCnt = rkLoader.LoadInt_e( "dummy_attack_cnt", 0 );
	m_DummyAttackMap.clear();

	for( int i=0 ; i<iAttackCnt ; i++ )
	{
		wsprintf_e( szBuf, "dummy_attack%d_code", i+1 );
		int iAttackCode = rkLoader.LoadInt( szBuf, 0 );

		if( iAttackCode == 0 )
			continue;

		DummyAttackInfo kInfo;

		wsprintf_e( szBuf, "dummy_attack%d_sub_type", i+1 );
		kInfo.m_iSubUseType = rkLoader.LoadInt( szBuf, DSUT_NONE );

		wsprintf_e( szBuf, "dummy_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kInfo.m_AttackAttribute, rkLoader );

		m_DummyAttackMap.insert( DummyAttackMap::value_type( iAttackCode, kInfo ) );
	}

	// animation
	rkLoader.LoadString_e( "start_animation", "", szBuf, MAX_PATH );
	m_StartAnimation = szBuf;
	m_fStartAniRate = rkLoader.LoadFloat_e( "start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );
	m_DelayAnimation = szBuf;
	m_fDelayAniRate = rkLoader.LoadFloat_e( "delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_MoveAnimation = szBuf;
	m_fMoveAniRate = rkLoader.LoadFloat_e( "move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "dash_animation", "", szBuf, MAX_PATH );
	m_DashAnimation = szBuf;
	m_fDashAniRate = rkLoader.LoadFloat_e( "dash_ani_rate", FLOAT1 );

	// etc
	rkLoader.LoadString_e( "destroy_effect", "", szBuf, MAX_PATH );
	m_DestroyEffect = szBuf;
	rkLoader.LoadString_e( "dummy_effect", "", szBuf, MAX_PATH );
	m_DummyEffect = szBuf;

	// target trace
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwTraceStateDuration = (DWORD)rkLoader.LoadInt_e( "target_trace_state_duration", 0 );
	m_dwDelayDuration = (DWORD)rkLoader.LoadInt_e( "target_trace_delay", 0 );

	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "target_trace_rotate_speed", 0 );
	m_fEnableTraceMoveHeight = rkLoader.LoadFloat_e( "target_trace_enable_height", 0.0f );
	m_fTraceMoveSpeed = rkLoader.LoadFloat_e( "target_trace_move_speed", 0.0f );

	m_dwSensingDuration = (DWORD)rkLoader.LoadInt_e( "target_trace_sensing_duration", 0 );
	m_fSensingRange = rkLoader.LoadFloat_e( "target_trace_sensing_range", 0.0f );
	m_fSensingRange2 = rkLoader.LoadFloat_e( "target_trace_sensing_range2", 0.0f );
	m_fSensingAngle = rkLoader.LoadFloat_e( "target_trace_sensing_angle", 0.0f );

	m_fTraceSpeed = rkLoader.LoadFloat_e( "target_trace_speed", 0.0f );
	m_fTraceRange = rkLoader.LoadFloat_e( "target_trace_range", 0.0f );
	m_fFirstTraceRange = rkLoader.LoadFloat_e( "target_first_trace_range", 0.0f );
	m_dwTraceDuration = (DWORD)rkLoader.LoadInt_e( "target_trace_duration", 0 );
	
	m_bCharColOnlyAttack = rkLoader.LoadBool_e( "char_col_only_attack", false );
	m_fTraceAttackRange = rkLoader.LoadFloat_e( "target_trace_attack_range", 0.0f );
	m_fTraceAttackAngle = rkLoader.LoadFloat_e( "target_trace_attack_angle", 0.0f );
	m_iTraceNormalAttack = rkLoader.LoadInt_e( "target_trace_normal_attack", 0 );

	rkLoader.LoadString_e( "target_trace_sensing_ani", "", szBuf, MAX_PATH );
	m_SensingAni = szBuf;
	m_fSensingAniRate = rkLoader.LoadFloat_e( "target_trace_sensing_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "target_trace_tracing_ani", "", szBuf, MAX_PATH );
	m_TracingAni = szBuf;
	m_fTracingAniRate = rkLoader.LoadFloat_e( "target_trace_tracing_ani_rate", FLOAT1 );

	m_fDummyCharHeightOffset = rkLoader.LoadFloat_e( "start_height", 0.0f );
	m_bFindTargetAttack = rkLoader.LoadBool_e("find_target_attack", false );

	//악어
	m_dwLifeTime = (DWORD)rkLoader.LoadInt_e( "life_time", 0 );
	m_nCanEatCnt = rkLoader.LoadInt_e( "can_eat_count", 4 );

	// owner offset
	m_fOwnerFrontOffset = rkLoader.LoadFloat_e( "front_offset", 0.0f );
	m_fOwnerSideOffset = rkLoader.LoadFloat_e( "side_offset", 0.0f );
	m_fOwnerHeightOffset = rkLoader.LoadFloat_e( "Height_offset", 0.0f );

	// owner trace
	m_fFrontOffset = rkLoader.LoadFloat_e( "owner_front_offset", 0.0f );
	m_fSideOffset = rkLoader.LoadFloat_e( "owner_side_offset", 0.0f );
	m_fHeightOffset = rkLoader.LoadFloat_e( "owner_Height_offset", 0.0f );
	m_bKeepStartAngle = rkLoader.LoadBool_e( "keep_start_angle", false );

	// owner control
	m_fCameraHeight	= rkLoader.LoadFloat_e( "owner_control_camera_height", 0.0f );
	m_bAutoDescent  = rkLoader.LoadBool_e( "owner_control_auto_descent", false );
	m_bAttackToDestroyDummy = rkLoader.LoadBool_e( "attack_key_to_destroy_dummy", false );

	// Owner Attach2
	m_fFindTargetRange = rkLoader.LoadFloat_e( "find_target_range", 0.0f );
	m_fFindTargetAngle = rkLoader.LoadFloat_e( "find_target_angle", 0.0f );
	m_fFindTargetMinAngle = rkLoader.LoadFloat_e( "find_target_min_angle", 0.0f );
	m_fFindTargetMaxAngle = rkLoader.LoadFloat_e( "find_target_max_angle", 0.0f );
	m_bDisableColGrap	= rkLoader.LoadBool_e( "find_target_disable_grap", 0 );
	
	m_AttackInfoList.clear();
	m_AttackInfoList.reserve(3);

	Attach2AttackInfo kInfo;
	kInfo.Init();
	kInfo.m_dwFireGapTime = (DWORD)rkLoader.LoadInt_e( "fire_gap_time", 0 );
	kInfo.m_iAutoFireCode = (DWORD)rkLoader.LoadInt_e( "auto_fire_code", -1 );
	m_AttackInfoList.push_back( kInfo );

	kInfo.Init();
	kInfo.m_dwFireGapTime = (DWORD)rkLoader.LoadInt_e( "fire_gap_time_air", 0 );
	kInfo.m_iAutoFireCode = (DWORD)rkLoader.LoadInt_e( "auto_fire_code_air", -1 );
	m_AttackInfoList.push_back( kInfo );

	kInfo.Init();
	kInfo.m_dwFireGapTime = (DWORD)rkLoader.LoadInt_e( "fire_gap_time_down", 0 );
	kInfo.m_iAutoFireCode = (DWORD)rkLoader.LoadInt_e( "auto_fire_code_down", -1 );
	m_AttackInfoList.push_back( kInfo );
	

	m_fTargetAimRate	= rkLoader.LoadFloat_e( "target_aim_rate", FLOAT05 );
	m_fDownTargetAimRate= rkLoader.LoadFloat_e( "down_target_aim_rate", 0.0f );

	// Target Trace 3
	m_fDelayMoveRange = rkLoader.LoadFloat_e( "delay_move_range", 0.0f );
	m_iMaxFireCount = rkLoader.LoadInt_e( "max_fire_count", 0 );

	// Disable Recalculate vector by Terrain
	m_bDisableRecalcByTerrain = rkLoader.LoadBool_e( "disable_recalculate_by_terrain", false );

	m_fMoveSpeed	= rkLoader.LoadFloat_e( "owner_control_move_speed", FLOAT1 );
	m_fMoveFriction = rkLoader.LoadFloat_e( "owner_control_move_friction", 0.0f );
	m_fMoveSpeedOnCharging = rkLoader.LoadFloat_e( "owner_control_move_speed_on_Charging", FLOAT1 );
	m_fMoveFrictionOnCharging = rkLoader.LoadFloat_e( "owner_control_move_friction_on_Charging", 0.0f );
	m_fUpDownSpeed	= rkLoader.LoadFloat_e( "owner_control_updown_speed", 0.0f );
	m_fMinHeight	= rkLoader.LoadFloat_e( "owner_control_min_height", 0.0f );
	m_fMaxHeight	= rkLoader.LoadFloat_e( "owner_control_max_height", 0.0f );
	m_dwFireDuration	= (DWORD)rkLoader.LoadInt_e( "owner_control_fire_duration", 0 );

	m_dwAttackChargeTime = (DWORD)rkLoader.LoadInt_e( "owner_control_attack_charge_time", 0 );
	m_dwAttackChargeRepeatTime = (DWORD)rkLoader.LoadInt_e( "owner_control_attack_charge_repeat_time", 0 );

	m_iControlNormalAttack = rkLoader.LoadInt_e( "owner_control_normal_attack_code", 0 );

	rkLoader.LoadString_e( "weapon_attribute_animation", "", szBuf, MAX_PATH );
	m_ControlChargeWeaponInfo.m_Animation	= szBuf;
	m_ControlChargeWeaponInfo.m_fAniRate = rkLoader.LoadFloat_e( "weapon_attribute_ani_rate", 0 );
	m_ControlChargeWeaponInfo.m_iWeaponIdx		= rkLoader.LoadInt_e( "weapon_attribute_index", 0 );
	m_ControlChargeWeaponInfo.m_iResistanceIdx	= rkLoader.LoadInt_e( "weapon_attribute_resist_index", 0 );

	rkLoader.LoadString_e( "weapon_wounded_animation", "", szBuf, MAX_PATH );
	m_ControlChargeWeaponInfo.m_WoundedAnimation	= szBuf;
	m_ControlChargeWeaponInfo.m_dwWoundedDuration	= rkLoader.LoadInt_e( "weapon_wounded_duration", 0 );
	m_ControlChargeWeaponInfo.m_bWoundedLoopAni		= rkLoader.LoadBool_e( "weapon_wounded_loop_ani", false );

	rkLoader.LoadString_e( "owner_control_attack_sound", "", szBuf, MAX_PATH );
	m_szControlAttackSound = szBuf;
	m_bSoundLoop = rkLoader.LoadBool_e( "owner_control_attack_sound_loop", false );

	rkLoader.LoadString_e( "background_sound", "", szBuf, MAX_PATH );
	m_szBGM = szBuf;
	m_bBGMLoop = rkLoader.LoadBool_e( "background_sound_loop", false );
	
	//for ioSummonWoodSkill
	rkLoader.LoadString_e( "wood_growth_up_effect", "", szBuf, MAX_PATH );
	m_WoodGrowthUpEffect = szBuf;
	
	m_PierceWeapon.iAttributeIdx = rkLoader.LoadInt_e( "pierce_weapon", NO_WEAPON_ATTRIBUTE );

	m_PierceWeapon.iResistanceIdx = rkLoader.LoadInt_e( "pierce_weapon_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "pierce_weapon_wounded", "", szBuf, MAX_PATH );
	m_PierceWeapon.szWoundedAni = szBuf;

	m_PierceWeapon.dwWoundedAniDuration = rkLoader.LoadInt_e( "pierce_weapon_wounded_time", 0 );

	m_bEffectVisible = rkLoader.LoadBool_e( "wood_growth_effect_visible", true );
	m_dwPreDuration = (DWORD)rkLoader.LoadInt_e( "pre_duration", 0 );
	rkLoader.LoadString_e( "pre_effect", "", szBuf, MAX_PATH );
	m_szPreEffect = szBuf;

	rkLoader.LoadString_e( "perice_start_sound", "", szBuf, MAX_PATH );
	m_PierceStartSound = szBuf;

	// dummy tower
	m_dwDummyTowerCheckGapTime = (DWORD)rkLoader.LoadInt_e( "dummy_tower_gap_time", 0 );
	m_fDummyTowerCheckRange = rkLoader.LoadFloat_e( "dummy_tower_range", 0.0f );
	
	rkLoader.LoadString_e( "dummy_tower_dummy_name", "", szBuf, MAX_PATH );
	m_DummyTowerDummyName = szBuf;

	m_bVisibleMiniMap = rkLoader.LoadBool_e( "visible_minimap", false );

	// Rush
	int iSize = rkLoader.LoadInt_e( "attach_weapon_type_count", 0 );
	for( int i=0; i<iSize; ++i )
	{
		WeaponInfo kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "attach_weapon_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attach_weapon_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attach_weapon_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attach_weapon_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attach_weapon_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttachWeaponInfoList.push_back( kInfo );
	}

	//CurseDoll
	m_fCurseDollCheckLandHeight = rkLoader.LoadFloat_e( "check_land_min_height", FLOAT100 );
	m_dwCurseDollDamageTime = (DWORD)rkLoader.LoadInt_e( "check_damamge_time", 0 );
	m_bCurseDollAutoAttackDisable = rkLoader.LoadBool_e( "curse_doll_auto_attack_disable", false );

	m_dwDropZoneLiveTime = (DWORD)rkLoader.LoadInt_e( "dropzone_live_time", 0 );

	m_fDamageRate		= rkLoader.LoadFloat_e( "damage_rate", FLOAT1 );
	m_fAbsoluteDamage	= rkLoader.LoadFloat_e( "absolute_damage", FLOAT1 );

	m_bOwnerAttached	= rkLoader.LoadBool_e( "owner_attached", false );
	
	// Jump
	m_fJumpAttackRate = rkLoader.LoadFloat_e( "jump_attack_rate", FLOAT1 );
	m_fJumpAttackAccurate = rkLoader.LoadFloat_e( "jump_attack_Accurate", 0.0f );
	m_fMinRange = rkLoader.LoadFloat_e("min_range", 0.0f );

	// BuffSkip
	int iBuffSize = rkLoader.LoadInt_e( "target_trace_skip_buff_cnt", 0 );
	for( int i=0; i<iBuffSize; ++i )
	{
		wsprintf_e( szKey, "target_trace_skip_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH);
		ioHashString SkipBuff = szBuf;

		m_vSkipBuffList.push_back( SkipBuff );
	}

	m_bCheckWounedState = rkLoader.LoadBool_e( "target_trace_wounded_check", 0 );

	m_bTraceOwnerSync = rkLoader.LoadBool_e( "trace_owner_sync", false );
	m_bDummyDeadDestoryWeapon = rkLoader.LoadBool_e( "dummy_dead_destory_weapon", false );

	m_bMountEnable = rkLoader.LoadBool_e( "mount_enable", false );
	m_bOwnerMountEnable = rkLoader.LoadBool_e( "owner_mount_enable", true );

	m_bDisableMapCollision = rkLoader.LoadBool_e( "disable_map_collision", false );
	m_bDiableOwnerWoundDestroy = rkLoader.LoadBool_e( "disable_owner_wound_dummy_dead", false );
	m_bNoChangeEffectHeight = rkLoader.LoadBool_e( "no_change_effect_height", false );

	m_bStartAttchWeapon = rkLoader.LoadBool_e( "start_attach_weapon", false );
	m_bNoGravityMove = rkLoader.LoadBool_e( "no_gravity_move", false );

	m_bIgnoreChargeWeaponDead = rkLoader.LoadBool_e( "ignore_charge_weapon_dead", false );

	LoadWeaponInfoList( "end_weapon", m_EndWeaponInfoList, rkLoader );
	m_bEndWeaponAttachOwner = rkLoader.LoadBool_e( "end_weapon_attach_owner", false );
	
	int iMaxSkillDummyAniCount = rkLoader.LoadInt_e( "max_skill_dummy_ani_count", 0 );
	for( int i=0 ; i<iMaxSkillDummyAniCount ; ++i )
	{
		wsprintf_e( szBuf, "skill_dummy_ani%d_code", i+1 );
		int iAniCode = rkLoader.LoadInt( szBuf, 0 );

		if( iAniCode == 0 )
			continue;

		DummySkillAniInfo kInfo;

		wsprintf_e( szKey, "skill_dummy_ani%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH);
		kInfo.szAniName = szBuf;

		wsprintf_e( szBuf, "skill_dummy_ani%d_rate", i+1 );
		kInfo.fAniRate = rkLoader.LoadInt( szBuf, DSUT_NONE );

		m_DummySkillAniInfoMap.insert( DummySkillAniInfoMap::value_type( iAniCode, kInfo ) );
	}
	m_iColDieType = rkLoader.LoadInt_e( "collision_die_type", 0 );
	m_bIsNoCheckPreCollision = rkLoader.LoadBool_e( "no_check_precollision", false );

	m_bCheckMapCollision		= rkLoader.LoadBool_e( "check_map_collision", true );
	m_bCheckStructCollision		= rkLoader.LoadBool_e( "check_struct_collision", true );

	m_bSyncForOtherUser	= rkLoader.LoadBool_e( "sync_for_other_user", true );
}

void ioDummyChar::InitResource()
{
	m_pGauge = new ioGaugeHelper;
	m_pGauge->InitResource();
}

void ioDummyChar::InitDummyCharInfo( int iIndex,
									 DWORD dwCreateGapTime,
									 const D3DXVECTOR3 &vPos,
									 const D3DXVECTOR3 &vScale,
									 float fStartAngle, bool bNetWork )
{
	m_fTestValue = 0;
	m_bFirst = true;

	m_State = DCS_START;
	m_TraceState = DCTS_NONE;

	m_BGMID = 0;
	m_AttackSoundID = 0;

	m_iCreateIndex = iIndex;
	m_iReserveAttackCode = 0;

	m_vForceDir = ioMath::UNIT_Z;
	m_fForceAmt = 0.0f;
	m_fForceFrictionRate = FLOAT1;
	m_bForcePowerLinear = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwAttackKeyCheckTime = 0;
	m_ControlSkillName.Clear();

	m_fStartAngle = fStartAngle;
	D3DXVECTOR3 vNewPos = vPos;
	m_bMoveEffectCreated = false;

	if( m_fDummyCharHeightOffset > 0.0f && !bNetWork )
		vNewPos.y += m_fDummyCharHeightOffset;

	SetWorldPosition( vNewPos );
	SetWorldScale( vScale );

	m_dwCheckHPTime = FRAMEGETTIME();

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{	
		SetTeam( pOwner->GetTeam() );

		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			ioHashString szCurTeamEffect;

			if( pOwner->GetTeam() == TEAM_RED && !m_RedTeamEffect.IsEmpty() )
			{
				szCurTeamEffect = m_RedTeamEffect;
			}
			else if( pOwner->GetTeam() == TEAM_BLUE )
			{
				szCurTeamEffect = m_BlueTeamEffect;
			}
			else
			{
				if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
					szCurTeamEffect = m_BlueTeamEffect;
				else
					szCurTeamEffect = m_RedTeamEffect;
			}

			D3DXVECTOR3 vBottomPos = vPos;
			vBottomPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y ) + FLOAT1;

			ioMapEffect *pMapEffect = pStage->CreateMapEffect( szCurTeamEffect, vBottomPos, vScale );
			if( pMapEffect )
			{
				m_dwTeamEffect = pMapEffect->GetUniqueID();
			}
		}
	}

	// Start
	if( !m_StartAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_StartAnimation );
		if( iAniID != -1 )
		{
			float fAniRate = FLOAT1;
			if( m_fStartAniRate > 0.0f )
				fAniRate = m_fStartAniRate;

			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate );

			m_dwStartAniEndTime = FRAMEGETTIME();
			m_dwStartAniEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

			if( !m_DummyEffect.IsEmpty() )
			{
				m_pGroup->AttachEffect( m_DummyEffect, NULL );
				m_bMoveEffectCreated = true;
			}
		}
	}

	if( m_fStartAngle != 0.0f )
	{
		Rotate( ioMath::UNIT_Y, m_fStartAngle );
	}

	if( pOwner )
	{
		m_BGMID = PlaySound( pOwner, m_szBGM, m_bBGMLoop );

		pOwner->CheckDummyCharCreate( m_iCreateIndex );
	}

	D3DXQUATERNION qtRot = GetWorldOrientation();
	m_vTracingMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_vAttackDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	
	m_dwNextFireTime = 0;
	m_AttachAttackType = AT_NORMAL;

	m_szCurTargetName.Clear();
	m_szPreTargetName.Clear();

	m_fCurMoveRange = 0.0f;
	m_dwSeed = 0;
	m_bEnableAttack = true;
	m_iCurFireCount = 0;

	m_dwAttackAniEndTime = 0;
	m_dwDummyTowerCheckTime = 0;

	m_dwStartTime = FRAMEGETTIME();

	m_bCurMountEnable = true;
	m_bAlreadyCallEndWeapon = false;
}

void ioDummyChar::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	float fSecPerTime = fTimePerSec * FLOAT1000;
	m_pGroup->Update( fSecPerTime );

	if( m_bMountEnable )
	{
		UpdateMountedEntityList();
		UpdateMountedWeaponList();
	}

	ProcessState( fTimePerSec, dwPreTime );

	CheckDieByHp();
	DropZoneCheck();
	CheckDestroyRange();
	
	if( CheckOwnerDestroyState() )
	{
		SendDieState( true );
	}
}

void ioDummyChar::UpdateAfter( float fFrameGap )
{
	ioPlayEntity::UpdateAfter( fFrameGap );

	if( m_dwTeamEffect != -1 )
	{
		if( m_pModeParent && m_pModeParent->GetCreator() )
		{
			ioMapEffect *pMapEffect = m_pModeParent->GetCreator()->FindMapEffect( m_dwTeamEffect );
			if( pMapEffect )
			{
				D3DXVECTOR3 vPos = GetWorldPosition();
				if ( m_bNoChangeEffectHeight )
					vPos.y += FLOAT1;
				else
					vPos.y = GetBottomHeight() + FLOAT1;

				pMapEffect->SetWorldPosition( vPos );
			}
		}
	}
}

void ioDummyChar::NeedUpdateBottomHeight()
{
	if( !m_pModeParent )
		return;

	if( m_DummyCharType	!= 	DCT_TARGET_TRACE4 ) 
		ioPlayEntity::NeedUpdateBottomHeight();

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage )
		return;

	MountedEntityList::iterator iter = m_vMountEntityList.begin();
	while( iter != m_vMountEntityList.end() )
	{
		ioGameEntity *pEntity = *iter;

		if( !pStage->CheckEnableEntity( pEntity ) )
		{
			iter = m_vMountEntityList.erase( iter );
		}
		else
		{
			pEntity->NeedUpdateBottomHeight();
			++iter;
		}
	}
}

void ioDummyChar::UpdateBottomHeight() const
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	m_fCurBottomHeight = m_pModeParent->GetMapHeight( vCurPos.x, vCurPos.z, this );
}
 
void ioDummyChar::Translate( const D3DXVECTOR3 &vMove )
{
	if( m_bMountEnable )
	{
		UpdateMountedEntityList();
		UpdateMountedWeaponList();

		TranslateMountEntity( vMove );
	}

	ioPlayEntity::Translate( vMove );
}

void ioDummyChar::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
	// StructCollision이 false이면 컨베이어로인한 이동 동작하지 않음
	if( !m_bCheckStructCollision )
		return;

	if( m_bMountEnable )
	{
		UpdateMountedEntityList();
		UpdateMountedWeaponList();

		TranslateMountEntity( vMove );
	}
	
	m_pGroup->GetParentNode()->Translate( vMove );
	NeedUpdateBound();
	NeedUpdateBottomHeight();
}

void ioDummyChar::TranslateByWorldEntity( const D3DXVECTOR3 &vMove )
{
	// StructCollision이 false이면 엘리베이터로인한 이동 동작하지 않음
	if( !m_bCheckStructCollision )
		return;

	Translate( vMove );
	NeedUpdateBound();
	NeedUpdateBottomHeight();
}

ioWorldEventReceiver* ioDummyChar::GetWorldEventReceiver()
{
	return dynamic_cast<ioWorldEventReceiver*>(this);
}

void ioDummyChar::OnTelepotation( const D3DXVECTOR3 &vTargetPos )
{
	// StructCollision이 false이면 텔레포트로 이동 동작하지 않음
	if( !m_bCheckStructCollision )
		return;

	SetWorldPosition( vTargetPos );
}

void ioDummyChar::OnFireZoneDamage( float fDamage, const ioHashString &szBuff )
{
	// 데미지 처리?
}

void ioDummyChar::OnPoisonZoneDamage()
{
	if( m_State == DCS_DIE )
		return;

	switch( m_DummyCharType )
	{
	case DCT_MAMAHAHA:
	case DCT_RAPTOR:
	case DCT_VEHICLE:
	case DCT_SUB_CRANE:
	case DCT_OWNER_CONTROL6:
	case DCT_OWNER_TRACE_WOUND_ANI:
		return;
	}

	m_HP.m_fCurValue = 0.0f;

	m_LastAttacker = m_OwnerName;
	SendDieState( true );
}

ioPlayEntity::PlaySubType ioDummyChar::GetSubType() const
{
	return PST_DUMMY_CHAR;
}

ArmorType ioDummyChar::GetArmorType() const
{
	return m_ArmorType;
}

int ioDummyChar::DontMoveEntityLevel() const
{
	if( m_DummyCharType == DCT_GROUND_PIERCE )
	{
		return 3;
	}
	else if( m_DummyCharType == DCT_DUMMY_TOWER || m_DummyCharType == DCT_MINE || m_DummyCharType == DCT_ATTACH_WEAPON || m_DummyCharType == DCT_WARP_MINE )
	{
		return 3;
	}
	else if( m_DummyCharType == DCT_OWNER_CONTROL || m_DummyCharType == DCT_OWNER_CONTROL3 || m_DummyCharType == DCT_OWNER_ATTACH4 || m_DummyCharType == DCT_OWNER_CONTROL6 )
	{
		if( m_bMountEnable )
			return 1;
		else
			return 3;
	}
	else if( m_DummyCharType == DCT_FORKCRANE_DUMMY )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

bool ioDummyChar::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon ) return false;

	if( m_State == DCS_DIE )
		return false;

	switch( m_DummyCharType )
	{
	case DCT_OWNER_ATTACH:
	case DCT_MAMAHAHA:
	case DCT_RAPTOR:
	case DCT_ATTACK_TOWER:
	case DCT_ATTACK_TOWER2:
	case DCT_CROW:
	case DCT_OWNER_CONTROL6:			// 이 타입의 더미는 조준을 위한 더미기때문에 타격을 입지 않음
		return false;
	}

	if( !IsCollisionAvailableRange( pWeapon ) )
		return false;

	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	ioBaseChar *pWeaponOwner = pWeapon->GetOwner();
	if( !pWeaponOwner )
		return false;

	if( m_DummyCharType == DCT_GROUND_PIERCE )
	{
		ioPullWeapon* pPull = ToPullWeapon( pWeapon );
		ioZoneEffectWeapon * pZone = ToZoneEffectWeapon( pWeapon );
		if( pPull )
		{
			int Index = pPull->GetWeaponByDummyChar();		
			if( 0 < Index && Index == m_iCreateIndex )
				return false;
		}
		else if( pZone )
		{		
			int Index = pZone->GetWeaponByDummyChar();			
			if( 0 < Index )			
				return false;
		}
	}
	else if ( m_DummyCharType == DCT_TARGET_TRACE6 )
	{
		if( pOwner->GetTeam() != pWeaponOwner->GetTeam() )
			return false;
	}
	else
	{
		if( pOwner->GetTeam() == pWeaponOwner->GetTeam() )
			return false;
	}

	return true;
}

bool ioDummyChar::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	//더미를 붙이고 있음으로 캐릭터는 Skip
	ioBaseChar* pChar = ToBaseChar( pColEntity );
	if( pChar && pChar->GetState() == CS_OBJECT_ATTACH )
		return true;

	switch( m_eEntityColSkip )
	{
	case ECS_ALL:
		return true;
	case ECS_OWNER:
		if( pChar == GetOwner() )
			return true;
		break;
	}

	switch( m_DummyCharType )
	{
	case DCT_OWNER_ATTACH:
	case DCT_MAMAHAHA:
	case DCT_RAPTOR:
	case DCT_VEHICLE:
	case DCT_MOVE_ATTACK:
		return true;
	//case DCT_MOVE_ATTACK4:
	//	{
	//		ioDummyChar* pDummy = ToDummyChar( pColEntity );
	//		if( pDummy && pDummy->GetType() == DCT_MOVE_ATTACK4 )
	//		{
	//			if( m_HP.m_fCurValue > pDummy->GetHP().m_fCurValue )
	//				pDummy->SetDieState( true );
	//		}
	//	}
	//	break;
	}

	return false;
}

bool ioDummyChar::IsNeedCheckMapCollision() const
{
	switch( m_DummyCharType )
	{
	case DCT_OWNER_ATTACH5:
	case DCT_OWNER_ATTACH4:
	case DCT_OWNER_ATTACH:
	case DCT_NO_CONTROL:
	case DCT_TARGET_TRACE3:
	case DCT_MAMAHAHA:
	case DCT_RAPTOR:
	case DCT_VEHICLE:
	case DCT_TARGET_TRACE4:
	case DCT_MOVE_ATTACK:
	case DCT_MOVE_ATTACK2:
	case DCT_OWNER_CONTROL6:
		return false;
	}

	if ( m_bDisableMapCollision )
		return false;

	if( m_State == DCS_START )
		return false;

	return true;
}

bool ioDummyChar::IsMountingAvailable() const
{	
	switch( m_DummyCharType )
	{
	case DCT_OWNER_ATTACH4:
	case DCT_OWNER_CONTROL:
	case DCT_OWNER_CONTROL3:
	case DCT_OWNER_CONTROL6:
	case DCT_ATTACH_WEAPON:
	case DCT_FORKCRANE_DUMMY:
		{
			if( m_bMountEnable )
				return false;
		}
		break;
	}

	return true;
}

bool ioDummyChar::IsMountAvailable() const
{
	switch( m_DummyCharType )
	{
	case DCT_OWNER_ATTACH4:
	case DCT_OWNER_CONTROL:
	case DCT_OWNER_CONTROL3:
	case DCT_OWNER_CONTROL6:
	case DCT_ATTACH_WEAPON:
	case DCT_FORKCRANE_DUMMY:
		{
			if( m_bMountEnable )
				return true;
		}
		break;
	}

	return false;
}

bool ioDummyChar::IsNowMountEnable() const
{
	switch( m_DummyCharType )
	{
	case DCT_OWNER_ATTACH4:
	case DCT_OWNER_CONTROL:
	case DCT_OWNER_CONTROL3:
	case DCT_OWNER_CONTROL6:
	case DCT_ATTACH_WEAPON:
	case DCT_FORKCRANE_DUMMY:
		{
			if( m_bCurMountEnable && m_bMountEnable )
				return true;			
		}
		break;
	}

	return false;
}

bool ioDummyChar::IsCharCollisionSkipState( TeamType eTeamType, int iState ) const
{
	if( m_CharColSkip == CCT_ALL )
	{
		return true;
	}

	switch( m_DummyCharType )
	{
	case DCT_OWNER_ATTACH:
		return true;
	case DCT_GROUND_PIERCE:
		{			
			if( m_bGrowthUp )
				return true;
			else			
				return false;			
		}
	}

	if( m_CharColSkip == CCT_TEAM )
	{
		if( GetTeam() == eTeamType )
		{
			return true;
		}
	}

	return false;
}

bool ioDummyChar::IsActivateWeapon()
{
	return true;
}

bool ioDummyChar::IsDummyCharCollisionSkipState( const ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( m_CharColSkip == CCT_OWNER )
	{
		if( pOwner->GetCharName() == pChar->GetCharName() )
			return true;
	}

	return false;
}

bool ioDummyChar::IsDummyVsDummyCollisionSkipState( const ioDummyChar *pDummy )
{
	if( !pDummy )
		return false;

	if( m_DummyCharColSkip == DCCS_ALL )
		return true;

	// 로켓 런처 용병의 D꾹 특별 상태에서 소환되는 조준 구체 더미는 다 통과함
	if( pDummy->GetType() == DCT_OWNER_CONTROL6 )
		return true;

	if( GetType() == DCT_OWNER_CONTROL6 )
		return true;

	return false;
}

void ioDummyChar::SetHP( float fCurHP, float fMaxHP )
{
	m_HP.m_fCurValue = fCurHP;
	m_HP.m_fMaxValue = fMaxHP;
}

void ioDummyChar::UpdateGauge()
{
	m_bGaugeVisible = false;
	D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
	vPos.y += 40.0f;

	if( Setting::Check2DInScreen( vPos, m_iGaugeXPos, m_iGaugeYPos ) )
	{
		m_bGaugeVisible = true;
		if( m_pGauge )
			m_pGauge->UpdateGauge( m_HP.m_fCurValue, m_HP.m_fMaxValue );
	}
}

void ioDummyChar::RenderGauge()
{
	if( !m_pGauge || !m_bGaugeVisible || m_bDisableHPVisible ) return;

	bool bCheckMode = g_GUIMgr.IsNoneUIMode();
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( pStage && !bCheckMode )
	{
		if( pStage->HasCameraFixedStyle(CAMERA_STYLE_MOVIE) || pStage->HasCameraFixedStyle(CAMERA_STYLE_VIEW) )
			bCheckMode = true;
	}

	if( bCheckMode && !Setting::NoUIModeHPShow() ) return;

	D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
	vPos.y += 40.0f;

	int iXPos = 0, iYPos = 0;
	if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
	{
		m_pGauge->RenderGauge( iXPos, iYPos + 1 );
	}
}

void ioDummyChar::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_NONE_DELAY:
		ProcessNoneDelayState( fTimePerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	case DCS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimePerSec, dwPreTime );
		break;
	case DCS_CONTROL_ATTACK:
		ProcessChargeOwnerControl( fTimePerSec );
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	case DCS_TRACE:
		ProcessTraceState( fTimePerSec, dwPreTime );
		break;
	case DCS_TRACE_TARGET_ATTACK:
		ProcessTraceTargetAttack( fTimePerSec, dwPreTime );
		break;
	case DCS_LOOP_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessLoopAttackState( fTimePerSec, dwPreTime );
		break;
	case DCS_GROUND_PIERCING_ATTACK:		
		ProcessPiercingState( fTimePerSec );		
		break;
	}
	
	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	ProcessPos( fTimePerSec );
	ProcessEffectUpdate( fTimePerSec );
}

void ioDummyChar::ProcessPos( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
		return;
	}

	ProcessForceMove( fTimePerSec );

	switch( m_DummyCharType )
	{
	case DCT_OWNER_ATTACH4:
	case DCT_OWNER_CONTROL:
	case DCT_OWNER_CONTROL6:
	case DCT_TARGET_TRACE3:
	case DCT_ATTACK_TOWER2:
	case DCT_TARGET_TRACE7:
		return;
	case DCT_OWNER_ATTACH:
		{
			ioBaseChar *pOwner = GetOwner();
			if( !pOwner )
				return;

			SetWorldPosition( pOwner->GetWorldPosition() );
			SetWorldOrientation( pOwner->GetTargetRot() );
			return;
		}
		break;
	default:
		if( m_bOwnerRotate )
		{
			ioBaseChar *pOwner = GetOwner();
			if(	pOwner )
			{
				SetWorldOrientation( pOwner->GetTargetRot() );
			}
		}
		break;
	}

	// 아래는 높이값 처리.
	if(m_DummyCharType != DCT_TARGET_TRACE4 && !m_bNoGravityMove )
	{
		D3DXVECTOR3 vPrePos = GetWorldPosition();
		D3DXVECTOR3 vCurPos = vPrePos;
		float fMapHeight = GetBottomHeight();

		DWORD dwGapTime = dwCurTime - m_dwProcessTime;

		do 
		{
			DWORD dwCurGap = min( dwGapTime, 10 );
			if( dwGapTime >= 10 )
			{
				dwGapTime -= 10;
			}
			else
			{
				dwCurGap = dwGapTime;
				dwGapTime = 0;
			}

			float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

			if( m_fCurJumpPowerAmt > 0.0f )
			{
				vCurPos.y += m_fCurJumpPowerAmt * fNewTimePerSec;
			}

			if( vCurPos.y > fMapHeight )
			{
				float fCurRate = m_fGravityRate;
				if( fCurRate <= 0.0f )
					fCurRate = FLOAT1;

				m_fGravityAmt += (m_pModeParent->GetCreator()->GetRoomGravity() * fCurRate )* fNewTimePerSec;
				vCurPos.y -= m_fGravityAmt * fNewTimePerSec;

				if( vCurPos.y <= fMapHeight )
				{
					m_fGravityAmt = 0.0f;
					m_fCurJumpPowerAmt = 0.0f;
				}
			}
		} while( dwGapTime > 0 );

		if( vCurPos.y < fMapHeight )
		{
			vCurPos.y = fMapHeight;
			m_fGravityAmt = 0.0f;
			m_fCurJumpPowerAmt = 0.0f;
		}

		SetWorldPosition( vCurPos );
	}

	m_dwProcessTime = dwCurTime;
}

void ioDummyChar::ProcessEffectUpdate( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	switch( m_DummyCharType )
	{
	case DCT_GROUND_PIERCE:
		{
			D3DXVECTOR3 vPos = GetWorldPosition();			
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwWoodGrowthUpEffect );
			if( pMapEffect )
			{
				pMapEffect->SetWorldPosition( vPos );
			}

			vPos = GetWorldPosition();			
			pMapEffect = pStage->FindMapEffect( m_PreEffectID );
			if( pMapEffect )
			{
				pMapEffect->SetWorldPosition( vPos );
			}
		}
		break;
	}
}

void ioDummyChar::ProcessStartState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DummyCharType )
	{
	case DCT_GROUND_PIERCE:
		{	
			m_bPiercingProtected = true;

			if( m_pGroup )
				m_pGroup->SetVisible( false );

			ioBaseChar* pOwner = GetOwner();
			if( pOwner )
			{
				ioPlayStage *pStage = pOwner->GetCreator();
				if( pStage )
				{
					if( m_bEffectVisible )
					{
						D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );	
						ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szPreEffect,
																		   GetWorldPosition(),
																		   vScale );

						if( pMapEffect )
							m_PreEffectID = pMapEffect->GetUniqueID();								

					}	
				}
			}
		}
		break;
	case DCT_OWNER_CONTROL:
		{
			if ( m_bStartAttchWeapon )
				SetAttachWeapon();
		}
		break;
	case DCT_OWNER_ATTACH3:
		{
			ioBaseChar* pOwner = GetOwner();
			if( pOwner )
			{
				D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
				D3DXVECTOR3 vScale = pOwner->GetWorldScale();
				D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vDir, &vDir );

				D3DXVECTOR3 vSide;
				D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );

				vPos += vDir * m_fFrontOffset;
				vPos.y += m_fHeightOffset;
				vPos += vSide * m_fSideOffset;
				SetWorldPosition( vPos );
				SetWorldScale( vScale );
				//지정된 회전 앵글을 유지 할떄
				if( m_bKeepStartAngle && m_fStartAngle != 0.0f )
				{
					float fYawD = ioMath::ArrangeHead( m_fStartAngle );
					float fYawR = DEGtoRAD( fYawD );
					D3DXQUATERNION qtRotate;
					D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
					vDir = qtRotate * vDir;					
				}

				D3DXQUATERNION qtRot;
				ioMath::CalcDirectionQuaternion( qtRot, vDir );
				SetWorldOrientation( qtRot );
			}
		}
		break;
	case DCT_CURSE_DOLL:
		{
			ioBaseChar *pOwner = GetOwner();
			ioBaseChar *pTarget = NULL;

			if( pOwner )
				pTarget = pOwner->GetBaseChar( m_stTargetName );

			if( pOwner && pTarget )
			{
				DWORD dwCurTime = FRAMEGETTIME();

				m_fGravityAmt = 0.0f;

				D3DXVECTOR3 vPos = pTarget->GetWorldPosition();
				D3DXVECTOR3 vScale = pTarget->GetWorldScale();
				D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vDir, &vDir );

				D3DXVECTOR3 vSide;
				D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );

				vPos += vDir * m_fFrontOffset;
				vPos.y += m_fHeightOffset;
				vPos += vSide * m_fSideOffset;
				SetWorldPosition( vPos );

				//지정된 회전 앵글을 유지 할떄
				if( m_bKeepStartAngle && m_fStartAngle != 0.0f )
				{
					float fYawD = ioMath::ArrangeHead( m_fStartAngle );
					float fYawR = DEGtoRAD( fYawD );
					D3DXQUATERNION qtRotate;
					D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
					vDir = qtRotate * vDir;					
				}

				D3DXQUATERNION qtRot;
				ioMath::CalcDirectionQuaternion( qtRot, vDir );
				SetWorldOrientation( qtRot );
			}
		}
		break;
	case DCT_COPY_OWNER2:
		{
			ioBaseChar* pOwner = GetOwner();
			if( pOwner )
			{
				D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
				D3DXVECTOR3 vScale = pOwner->GetWorldScale();
				D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vDir, &vDir );

				D3DXVECTOR3 vSide;
				D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );
				
				vPos += vDir * m_fOwnerFrontOffset;
				vPos += vSide * m_fOwnerSideOffset;
				
				ioPlayStage *pStage = pOwner->GetCreator();
				if( pStage )
				{
					float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
					
					if ( vPos.y < fMapHeight )
					{
						vPos.y = fMapHeight;
						if ( pOwner->IsNeedProcess() )
						{
							pOwner->SetWorldPosition( vPos );
						}
					}
					else
					{
						if ( pOwner->IsNeedProcess() )
						{
							pOwner->SetWorldPosition( vPos );
						}
					}
				}				
			}
		}
		break;
	}

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioDummyChar::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DummyCharType )
	{
	case DCT_OWNER_TRACE:
		UpdateOwnerTrace();
		// HARDCODE : break 없음에 주의
	case DCT_NONE_TRACE:
	case DCT_OWNER_ATTACH:
		if( CheckEnableAttackState() )
		{
			SetAttackState();
		}
		break;
	case DCT_TARGET_TRACE:
		if( m_dwDelayStartTime > 0 && m_dwDelayStartTime+m_dwDelayDuration < dwCurTime )
		{
			m_dwDelayStartTime = 0;
			m_iReserveAttackCode = m_iTraceNormalAttack;
			SetAttackState();
		}
		break;
	case DCT_OWNER_CONTROL:
		ProcessDelayOwnerControl( fTimePerSec );
		break;
	case DCT_TARGET_TRACE2:
	case DCT_TARGET_TRACE4:
	case DCT_COPY_OWNER3:
		if( m_dwDelayStartTime > 0 && m_dwDelayStartTime+m_dwDelayDuration < dwCurTime )
		{
			m_dwDelayStartTime = 0;
			SetTraceState( DCTS_SENSING );
		}
		break;
	case DCT_TARGET_TRACE5:
		if( m_dwDelayStartTime > 0 && m_dwDelayStartTime+m_dwDelayDuration < dwCurTime )
		{
			m_dwDelayStartTime = 0;

			if( m_nCanEatCnt && m_nCanEatCnt <= (int)m_vTargetedChar.size() )
			{
				ioBaseChar *pOwner = GetOwner();
				if( pOwner && pOwner->IsNeedProcess() )
				{
					SendDieState( true );
					break;
				}
			}
			
			if ( !m_bFindTargetAttack )
			{
				SetTraceState( DCTS_SENSING );
			}
			else
				SetTraceState( DCTS_TARGET_JUMP );

		}
		break;
	case DCT_NO_CONTROL:
		m_dwDelayStartTime = 0;
		m_iReserveAttackCode = m_iTraceNormalAttack;
		SetAttackState();
		break;
	case DCT_OWNER_ATTACH2:
	case DCT_OWNER_ATTACH3:
		{
			ioBaseChar *pOwner = GetOwner();

			if( pOwner )
			{
				m_fGravityAmt = 0.0f;

				D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
				D3DXVECTOR3 vScale = pOwner->GetWorldScale();
				D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vDir, &vDir );

				D3DXVECTOR3 vSide;
				D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );

				vPos += vDir * m_fFrontOffset;
				vPos.y += m_fHeightOffset;
				vPos += vSide * m_fSideOffset;
				SetWorldPosition( vPos );

				if( !m_bOwnerRotate )
				{
					//지정된 회전 앵글을 유지 할떄
					if( m_bKeepStartAngle && m_fStartAngle != 0.0f )
					{
						float fYawD = ioMath::ArrangeHead( m_fStartAngle );
						float fYawR = DEGtoRAD( fYawD );
						D3DXQUATERNION qtRotate;
						D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
						vDir = qtRotate * vDir;					
					}

					D3DXQUATERNION qtRot;
					ioMath::CalcDirectionQuaternion( qtRot, vDir );
					SetWorldOrientation( qtRot );
				}
				OwnerAttach2AttackType ePreAttackType = m_AttachAttackType;
				ioBaseChar *pTarget = CheckEnableTarget();
				
				if( ePreAttackType != m_AttachAttackType )
				{
					m_dwNextFireTime = 0;
				}

				if( pTarget )
				{
					if( m_dwNextFireTime == 0 )
					{
						m_dwNextFireTime	= FRAMEGETTIME() + m_AttackInfoList[m_AttachAttackType].m_dwFireGapTime;
						m_iReserveAttackCode= m_AttackInfoList[m_AttachAttackType].m_iAutoFireCode;
					}
					else if( m_dwNextFireTime > 0 && m_dwNextFireTime <= FRAMEGETTIME() )
					{
						m_dwNextFireTime = 0;
						SetAttackState();
					}
				}
			}
		}
		break;
	case DCT_TARGET_TRACE3:
		{
			FindTarget( GetOwner() );
			SetTraceTargetMoveState();
		}
		break;
	case DCT_LOOP_ATTACK:
		if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwDelayDuration < dwCurTime )
		{
			m_dwDelayStartTime = 0;
			m_iReserveAttackCode = m_iTraceNormalAttack;
			SetLoopAttackState();
		}
		break;
	case DCT_GROUND_PIERCE:
		{
			SetPiercingPreDelayStartState();

			if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwPreDuration < dwCurTime )
			{	
				m_dwDelayStartTime = 0;
				SetPiercingStartState();
			}
		}
		break;
	case DCT_DUMMY_TOWER:
		CheckDummyTower();
		break;
	case DCT_ATTACK_TOWER:
	case DCT_ATTACK_TOWER2:
		CheckAttackTower();
		break;
	case DCT_CURSE_DOLL:
		CheckCurseDoll();
		break;
	case DCT_RUSH:
		SetMoveState();
		break;
	default:
		{
			if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwPreDuration < dwCurTime )
			{
				m_dwDelayStartTime = 0;

				if( m_pGroup )
					m_pGroup->SetVisible( true );
			}
		}
		break;
	}
}

void ioDummyChar::ProcessNoneDelayState( float fTimePerSec )
{
}

void ioDummyChar::ProcessMoveState( float fTimePerSec )
{
	switch( m_DummyCharType )
	{
	case DCT_OWNER_TRACE:
		UpdateOwnerTrace();
		break;
	case DCT_TARGET_TRACE:
		break;
	case DCT_OWNER_CONTROL:
		ProcessMoveOwnerControl( fTimePerSec );
		break;
	case DCT_NONE_TRACE:
	case DCT_OWNER_ATTACH:
		break;
	case DCT_TARGET_TRACE3:
		ProcessMoveTraceTarget( fTimePerSec );
		CheckEnableAttack();
		break;
	case DCT_RUSH:
		ProcessRush( fTimePerSec );
		break;
	}
}

void ioDummyChar::ProcessDashState( float fTimePerSec )
{
}

void ioDummyChar::ProcessTraceState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_TraceState )
	{
	case DCTS_NONE:
		break;
	case DCTS_SENSING:
		ProcessTraceSensing( fTimePerSec );
		break;
	case DCTS_TRACING:
		if( m_DummyCharType	!= 	DCT_TARGET_TRACE4 )
			ProcessTraceTracing( fTimePerSec );
		else
			ProcessTraceTracingNoneTerrain( fTimePerSec );
		break;
	case DCTS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessTraceAttack( fTimePerSec, dwPreTime );
		break;
	case DCTS_TARGET_JUMP:
		{
			m_fSensingRange = JUMP_RANGE * (float)m_fJumpAttackRate;
			ioHashString szTargetName = GetTraceTarget( pStage, false );
			FindAttackTheTarget( szTargetName );
			SetJumpAttackState();
		}
		break;
	case DCTS_TRACING_WAIT:
		{
			ProcessTraceTracingWait( fTimePerSec, dwPreTime );
		}
	}

	CheckTraceAttack();
	CheckTraceStateEnd();
}

void ioDummyChar::ProcessAttackState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_DummyCharType == DCT_OWNER_ATTACH2 )
	{
		ioBaseChar *pOwner = GetOwner();

		if( pOwner )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			D3DXVECTOR3 vScale = pOwner->GetWorldScale();
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSide;
			D3DXVec3Cross( &vSide, &vDir, &D3DXVECTOR3(0.0f, FLOAT1, 0.0f) );
			
			vPos += vDir * m_fFrontOffset;
			vPos.y += m_fHeightOffset;
			vPos += vSide * m_fSideOffset;

			SetWorldPosition( vPos );

			if( !m_bOwnerRotate )
			{
				ioBaseChar *pTarget = CheckEnableTarget();
				D3DXQUATERNION qtRot;
				if( pTarget )	ioMath::CalcDirectionQuaternion( qtRot, -m_vAttackDir );
				else			ioMath::CalcDirectionQuaternion( qtRot, vDir );
			
				SetWorldOrientation( qtRot );
			}
		}
	}
	else if( m_DummyCharType == DCT_OWNER_ATTACH3 )
	{
		ioBaseChar *pOwner = GetOwner();

		if( pOwner )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			D3DXVECTOR3 vScale = pOwner->GetWorldScale();
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSide;
			D3DXVec3Cross( &vSide, &vDir, &D3DXVECTOR3(0.0f, FLOAT1, 0.0f) );

			vPos += vDir * m_fFrontOffset;
			vPos.y += m_fHeightOffset;
			vPos += vSide * m_fSideOffset;

			D3DXQUATERNION qtRot;
			SetWorldPosition( vPos );
			ioMath::CalcDirectionQuaternion( qtRot, -m_vAttackDir );	
			SetWorldOrientation( qtRot );
		}
	}
	else if( m_DummyCharType == DCT_CURSE_DOLL )
	{
		ioBaseChar *pOwner = GetOwner();
		ioBaseChar *pTarget = NULL;

		if( pOwner )
			pTarget = pOwner->GetBaseChar( m_stTargetName );

		if( pOwner && pTarget )
		{
			DWORD dwCurTime = FRAMEGETTIME();

			m_fGravityAmt = 0.0f;

			D3DXVECTOR3 vPos = pTarget->GetWorldPosition();
			D3DXVECTOR3 vScale = pTarget->GetWorldScale();
			D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSide;
			D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );

			vPos += vDir * m_fFrontOffset;
			vPos.y += m_fHeightOffset;
			vPos += vSide * m_fSideOffset;
			SetWorldPosition( vPos );

			//지정된 회전 앵글을 유지 할떄
			if( m_bKeepStartAngle && m_fStartAngle != 0.0f )
			{
				float fYawD = ioMath::ArrangeHead( m_fStartAngle );
				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
				vDir = qtRotate * vDir;					
			}

			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, vDir );
			SetWorldOrientation( qtRot );
		}
	}
	
	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	DWORD dwLastFireTime = dwPreTime;
	FireTimeList::iterator iter = m_vAttackFireTimeList.begin();
	while( iter != m_vAttackFireTimeList.end() )
	{
		const FireTime &rkFire = *iter;

		DWORD dwFireTime = rkFire.dwStart;
		if( COMPARE( dwFireTime, dwPreTime, dwCurTime ) )
		{
			if( rkFire.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
			{
				ioWeapon *pWeapon = NULL;
				if( m_DummyCharType == DCT_OWNER_ATTACH3 )
					pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, m_vAttackDir, "" );
				else
					pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );
				
				if( pWeapon )
				{
					ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
					if( pZone )
					{
						pZone->SetPosition( GetMidPositionByRate() );
						ioBaseChar *pChar = pZone->GetOwner();
						if( pChar )
						{
							D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
							pZone->SetCollisionDir( vDir );
							pZone->SetZoneValue( true, false );
						}
					}

					//오직 하나의 타겟에만 충돌이 일어 나야 한다.
					if( m_DummyCharType == DCT_CURSE_DOLL )
					{
						ioBaseChar *pTarget = NULL; 
						if( pOwner )
							pTarget = pOwner->GetBaseChar( m_stTargetName );

						ioOneTargetWeapon *pOneTargetWeapon = ToOneTargetWeapon( pWeapon );
						if( pOwner && pTarget && pOneTargetWeapon)
						{
							pOneTargetWeapon->SetOneTargetName( m_stTargetName );

							D3DXVECTOR3 vPos = pTarget->GetWorldPosition();
							vPos.y = pTarget->GetMidHeightByRate( FLOAT05 );			
							pOneTargetWeapon->SetPosition( vPos );
						}
					}
				}
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime < dwCurTime )
	{
		switch( m_DummyCharType )
		{
		case DCT_NO_CONTROL:
			SetDieState( true );
			break;
		case DCT_NONE_TRACE:
			if( m_bOneTimeAttack )
				SetDieState( true );
			else
				SetDelayState( false );
			break;
		case DCT_TARGET_TRACE8:
			SetDieState( true );
			break;
		default:
			SetDelayState( false );
			break;
		}

		return;
	}
}

void ioDummyChar::ProcessDieState( float fTimePerSec )
{
}

void ioDummyChar::UpdateOwnerTrace()
{
	ioBaseChar *pOwnerChar = GetOwner();
	if( !pOwnerChar )
		return;

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	D3DXVECTOR3 vDir = pOwnerChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	D3DXVECTOR3 vTargetPos = pOwnerChar->GetWorldPosition() + ( m_fFrontOffset * vDir );
	vTargetPos += vSideDir * m_fOwnerSideOffset;
	D3DXVECTOR3 vDiff = vTargetPos - vCurPos;

	D3DXVECTOR3 vXZDir( vDiff.x, 0.0f, vDiff.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	if( vXZDir == ioMath::VEC3_ZERO )
		return;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vXZDir );
	SetWorldOrientation( qtRot );
}

void ioDummyChar::CheckDieByHp()
{
	if( m_State == DCS_DIE )
		return;

	DWORD dwTickTime = 0;
	if( m_pModeParent && m_pModeParent->GetModeType() == MT_TRAINING && 0 < m_dwTicTimeByTraining )
	{
		dwTickTime = m_dwTicTimeByTraining;
	}
	else
	{
		dwTickTime = m_dwTicTime;
	}

	if( dwTickTime <= 0 || m_fDecreaseHP <= 0.0f )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	int iTickGap = (int)( dwCurTime - m_dwCheckHPTime );
	if( iTickGap >= (int)dwTickTime )
	{
		m_dwCheckHPTime = dwCurTime - ( iTickGap - dwTickTime );
		m_HP.m_fCurValue -= m_fDecreaseHP;
	}

	bool bDie = false;
	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		bDie = true;
	}

	if( bDie )
	{
		m_LastAttacker = m_OwnerName;
		SendDieState( true );
	}
}

ApplyWeaponExp ioDummyChar::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyChar::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	if( P2PNetwork::IsNetworkPlaying() &&
		m_pModeParent->IsNetworkMode() &&
		!pAttacker->IsNeedProcess() )
		return AWE_EXPLOSION;

	m_LastAttacker = pWeapon->GetOwnerName();

	float fPreCurHP = m_HP.m_fCurValue;
	float fDamage = 0.0f;
	if( 0.0f < m_fAbsoluteDamage )
	{
		fDamage = m_fAbsoluteDamage;
	}
	else
	{
		fDamage = pWeapon->GetDamage( this ) * m_fDamageRate;
	}

	float fPushPower = pWeapon->GetDefensePushPower( fDamage );

	if( !m_bNoDamage && !m_bPiercingProtected )
		m_HP.m_fCurValue -= fDamage;
	AddHeadDamageInfo( (int)fDamage );

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 )
		bContactType = true;

	bool bStructWeapon = false;
	if( pWeapon->CheckWeaponByStruct() || pWeapon->CheckWeaponByDummyChar() )
		bStructWeapon = true;

	bool bOnlyEmoticon = true;
	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);
	if( bContactType && !bStructWeapon && !pWeapon->CheckWeaponBySkill() && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pWeapon->IsChangeJumpping( false, false )
			&& !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );

			bOnlyEmoticon = false;
		}
	}

	if( !m_BlockBuff.IsEmpty() )
		pAttacker->SetDisableAttackEmoticon();

	int iShakeCamera = pWeapon->GetShakeCamera();
	ActionStopType eActionStop = pWeapon->GetActionStopType();

	if( eActionStop == AST_BOTH || eActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
		pWeapon->SetActionStopDuration( ioBaseChar::CalcActionStopDelay( fDamage, false ),
			ioBaseChar::GetActionStopLooseRate( false ) );
	}

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_DAMAGE;
		kPacket << pWeapon->GetOwnerName();
		kPacket << m_HP.m_fCurValue;
		kPacket << m_HP.m_fMaxValue;
		kPacket << bOnlyEmoticon;
		kPacket << vAttackDir;
		kPacket << fDamage;
		kPacket << fPushPower;
		kPacket << (int)eActionStop;
		kPacket << iShakeCamera;
		kPacket << pWeapon->GetWeaponIndex();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		SetDieState( true, true );

		// 여기서 스킬 캔슬 처리 필요
		switch( m_DummyCharType )
		{
		case DCT_OWNER_CONTROL:
		case DCT_OWNER_CONTROL3:
		case DCT_OWNER_CONTROL5:
		case DCT_OWNER_CONTROL6:
		case DCT_OWNER_ATTACH:
			if( !m_ControlSkillName.IsEmpty() )
			{
				if( GetOwner() && GetOwner()->IsCurUsingSkill( m_ControlSkillName ) )
				{
					GetOwner()->ClearAttackFireTimeAndSkill( CAFT_APPLY_WOUNDED_BY_DUMMY );
					m_ControlSkillName.Clear();
				}
			}
			break;
		}
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}
	
	return AWE_EXPLOSION;
}

void ioDummyChar::OnDamaged( SP2Packet &rkPacket, ioBaseChar *pAttacker )
{
	rkPacket >> m_HP.m_fCurValue >> m_HP.m_fMaxValue;

	bool bOnlyEmoticon;
	D3DXVECTOR3 vAttackDir;
	float fDamage, fPushPower;

	rkPacket >> bOnlyEmoticon;
	rkPacket >> vAttackDir;
	rkPacket >> fDamage;
	rkPacket >> fPushPower;

	if( fDamage > 0.0f )
	{
		AddHeadDamageInfo( fDamage );
	}

	if( !bOnlyEmoticon && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );
		}
	}

	if( !m_BlockBuff.IsEmpty() )
		pAttacker->SetDisableAttackEmoticon();

	int iActionStop = 0;
	rkPacket >> iActionStop;

	int iShakeCamera = 0;
	rkPacket >> iShakeCamera;

	if( iActionStop == AST_BOTH || iActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
	}

	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		SetDieState( true );

		// 여기서 스킬 캔슬 처리 필요
		switch( m_DummyCharType )
		{
		case DCT_OWNER_CONTROL:
		case DCT_OWNER_CONTROL3:
		case DCT_OWNER_CONTROL5:
		case DCT_OWNER_CONTROL6:
		case DCT_OWNER_ATTACH:
			if( !m_ControlSkillName.IsEmpty() )
			{
				if( GetOwner() && GetOwner()->IsCurUsingSkill( m_ControlSkillName ) )
				{
					GetOwner()->ClearAttackFireTimeAndSkill( CAFT_APPLY_WOUNDED_BY_DUMMY );
					m_ControlSkillName.Clear();
				}
			}
			break;
		}
	}

	int iWeaponIndex = 0;
	rkPacket >> iWeaponIndex;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pAttacker, iWeaponIndex );
	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}
}

void ioDummyChar::SetDelayState( bool bNet )
{
	m_State = DCS_DELAY;
	m_iReserveAttackCode = 0;

	m_dwDelayStartTime = FRAMEGETTIME();

	m_dwExtraAniJumpTime = 0;
	m_fExtraAniJumpPower = 0.0f;

	m_iCurExtraAniJump = 0;
	m_vExtraAniJumpList.clear();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	CheckChargeWeaponDead();

	if( !m_DelayAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			float fAniRate = FLOAT1;
			if( m_fDelayAniRate > 0.0f )
				fAniRate = m_fDelayAniRate;

			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}

	ioBaseChar *pOwner = GetOwner();
	if( bNet && pOwner && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_DELAY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChar::SetNoneDelayState( bool bNet )
{
	m_State = DCS_NONE_DELAY;
	m_iReserveAttackCode = 0;

	CheckChargeWeaponDead();

	if( !m_DelayAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			float fAniRate = FLOAT1;
			if( m_fDelayAniRate > 0.0f )
				fAniRate = m_fDelayAniRate;

			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}

	ioBaseChar *pOwner = GetOwner();
	if( bNet && pOwner && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_NONE_DELAY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChar::SetMoveState()
{
	m_State = DCS_MOVE;

	CheckChargeWeaponDead();

	if( !m_MoveAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = FLOAT1;
			if( m_fMoveAniRate > 0.0f )
				fAniRate = m_fMoveAniRate;

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}

	SetAttachWeapon();
}

void ioDummyChar::SetDashState()
{
	m_State = DCS_DASH;

	CheckChargeWeaponDead();

	if( !m_DashAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DashAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = m_fDashAniRate;
			if( m_fDashAniRate <= 0.0f )
				fAniRate = FLOAT1;

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}

	SetAttachWeapon();
}

void ioDummyChar::SetAttackState()
{
	m_vAttackFireTimeList.clear();
	m_dwAttackKeyCheckTime = 0;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	AttackAttribute &rkCurAttribute = iter->second.m_AttackAttribute;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = DCS_ATTACK;
	m_dwAttackAniEndTime = dwCurTime;

	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = rkCurAttribute.m_fAttackAniRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
		m_dwAttackAniEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

		// fire 처리
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return;

		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;

		int i=0;
		AniEventConstPtrList::iterator iter;

		std::list< float >::iterator iterEndTime = vFireEndList.begin();
		for( iter=vFireStartList.begin() ; iter!=vFireStartList.end() ; ++iter, i++ )
		{
			float fFireStart = (*iter)->fEventTime;

			FireTime kFireTime;
			kFireTime.dwStart = dwStartTime + fFireStart * fAniRate;

			if( iterEndTime != vFireEndList.end() )
			{
				kFireTime.dwDuration = ( *iterEndTime - fFireStart ) * fAniRate;
				++iterEndTime;
			}

			kFireTime.iAttributeIdx  = vInfoList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = vInfoList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni   = vInfoList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration  = vInfoList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = vInfoList[i].m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;

			if( !(*iter)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
				if( vParam.size() >= 1 )
				{
					ioHashString szTypeName = vParam[0].c_str();
					kFireTime.eCollisionType = GetAniCollisionType( szTypeName );

					if( vParam.size() == 2 && kFireTime.eCollisionType == ACT_SPECIAL )
					{
						kFireTime.szSpecialCollisionBox = vParam[1].c_str();
					}
				}
			}

			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
			kFireTime.m_iExtraValue = m_iCreateIndex;

			m_vAttackFireTimeList.push_back( kFireTime );
		}

		SetReservedSliding( rkCurAttribute.m_vForceInfoList, iAniID, fAniRate, rkCurAttribute.m_dwPreDelay );
	
		// 기타 처리
		m_dwExtraAniJumpTime = 0;
		m_fExtraAniJumpPower = 0.0f;
		m_vExtraAniJumpList.clear();

		AniEventConstPtrList vExtraAniJumpList;
		m_pGroup->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );

		AniEventConstPtrList::iterator iter_event = vExtraAniJumpList.begin();
		while( iter_event != vExtraAniJumpList.end() )
		{
			ExtraAniJump kInfo;

			kInfo.m_dwTime = (*iter_event)->fEventTime * fAniRate;
			kInfo.m_dwTime += dwCurTime;

			if( !(*iter_event)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter_event)->szSubInfo.c_str(), " " );
				if( vParam.size() == 1 )
				{
					kInfo.m_fPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
				}
			}

			m_vExtraAniJumpList.push_back(kInfo);

			++iter_event;
		}

		if( !m_vExtraAniJumpList.empty() )
		{
			m_iCurExtraAniJump = 0;
			m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
			m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
		}
	}

	SendAttackInfoToSkill();
}

void ioDummyChar::SetControlAttackState( bool bStart )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_State = DCS_CONTROL_ATTACK;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_vAttackFireTimeList.clear();
	m_iReserveAttackCode = 0;
	if( m_dwAttackChargeRepeatTime != 0 )
		m_dwAttackKeyCheckTime = FRAMEGETTIME();
	else
		m_dwAttackKeyCheckTime = 0;	

	if( bStart )
	{
		if( !m_ControlChargeWeaponInfo.m_Animation.IsEmpty() )
		{
			int iAniID = m_pGroup->GetAnimationIdx( m_ControlChargeWeaponInfo.m_Animation );
			if( iAniID != -1 )
			{
				m_pGroup->ClearAllActionAni( FLOAT10, true );
				m_pGroup->ClearAllLoopAni( FLOAT10, true );

				float fAniRate = FLOAT1;
				if( m_ControlChargeWeaponInfo.m_fAniRate > 0.0f )
					fAniRate = m_ControlChargeWeaponInfo.m_fAniRate;

				m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
			}
		}
		else if( !m_MoveAnimation.IsEmpty() )
		{
			int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
			if( iAniID != -1 )
			{
				m_pGroup->ClearAllActionAni( FLOAT10, true );
				m_pGroup->ClearAllLoopAni( FLOAT10, true );

				float fAniRate = FLOAT1;
				if( m_fMoveAniRate > 0.0f )
					fAniRate = m_fMoveAniRate;

				m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
			}
		}
	}
	CheckChargeWeaponDead();

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;
	kFireTime.iAttributeIdx			= m_ControlChargeWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx		= m_ControlChargeWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni			= m_ControlChargeWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= m_ControlChargeWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni		= m_ControlChargeWeaponInfo.m_bWoundedLoopAni;

	kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex;

	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
	kFireTime.m_iExtraValue = m_iCreateIndex;

	m_iChargeWeaponIndex = 0;
	if( kFireTime.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
	{
		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
		if( pWeapon )
		{
			m_iChargeWeaponIndex = m_dwBaseWeaponIndex;

			// 처음 한 번은 여기서 위치를 조정해 줘야한다.
			if( m_dwAttackChargeRepeatTime == 0 )
				pWeapon->SetPosition( GetMidPositionByRate() );

			ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
			if( pZone )
			{
				ioBaseChar *pChar = pZone->GetOwner();
				if( pChar )
				{
					D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					pZone->SetCollisionDir( vDir );
					pZone->SetZoneValue( true, false );
					pZone->SetLiveTime( m_dwFireDuration );
				}
			}
		}
	}

	SendAttackInfoToSkill();
	m_AttackSoundID = PlaySound( pOwner, m_szControlAttackSound, m_bSoundLoop );
}

void ioDummyChar::SendDieState( bool bEffect )
{
	SetDieState( bEffect );

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_DIE;
		kPacket << bEffect;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChar::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	if( m_State == DCS_DIE )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		pOwner->CheckDummyCharDestroy( m_iCreateIndex );
	}

	StopSound( m_szBGM, m_BGMID );
	CheckChargeWeaponDead();

	m_State = DCS_DIE;
	m_LastAttacker.Clear();

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	if( m_dwTeamEffect != -1 )
	{
		if( m_pModeParent && m_pModeParent->GetCreator() )
		{
			ioMapEffect *pMapEffect = m_pModeParent->GetCreator()->FindMapEffect( m_dwTeamEffect );
			if( pMapEffect )
			{
				pMapEffect->EndEffectForce();
			}
		}

		m_dwTeamEffect = -1;
	}

	if( m_bMoveEffectCreated )
	{
		m_pGroup->EndEffect( m_DummyEffect, true );
		m_bMoveEffectCreated = false;
	}

	if( bEffect )
	{
		ioMapEffect *pEffect = m_pModeParent->GetCreator()->CreateMapEffect( m_DestroyEffect,
																			 GetWorldPosition(),
																			 ioMath::UNIT_ALL );

		if( pEffect )
		{
			pEffect->SetWorldOrientation( GetWorldOrientation() );
		}
	}
	
	SetExtendDieState( m_pModeParent->GetCreator(), pOwner );

	int iCnt = m_vAttachWeaponIndexList.size();
	
	for( int i=0; i<iCnt; ++i )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_vAttachWeaponIndexList[i] );
		ioRotateTargetWeapon *pRotate = ToRotateTargetWeapon( pWeapon );
		if( pRotate )
			continue;

		if( pWeapon )
			pWeapon->SetWeaponDead();
	}
	
	if ( m_bDummyDeadDestoryWeapon && pOwner )
	{
		g_WeaponMgr.DestroyDummyCharWeapon( pOwner->GetCharName(), GetDummyCharIdx() );
	}
		
	DummyDieClearMountEntity();
	m_bCurMountEnable = false;
	CallEndWeapon( bWoundedDie );
}

void ioDummyChar::AddEntityEvent( ioEntity *pEntity )
{
	if( m_pModeParent == NULL )
		return;

	ioBaseChar *pOwnerChar = GetOwner();
	if( pOwnerChar )
	{
		pOwnerChar->AddEntityEvent( pEntity );
	}
}

void ioDummyChar::SetOwner( ioHashString szOwnerName )
{
	m_OwnerName = szOwnerName;
	m_CreatorName = szOwnerName;
}

ioBaseChar* ioDummyChar::GetOwner() const
{
	if( !m_pModeParent )
		return NULL;

	if( !m_pModeParent->GetCreator() )
		return NULL;

	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerName );

	return pOwner;
}

void ioDummyChar::CheckSpecialCollisionBox( const ioHashString &szFileName )
{
	if( szFileName.IsEmpty() ) return;

	SAFEDELETE( m_pSpecialBox );

	m_pSpecialBox = g_CollisionBoxGrpMgr.CreateAniCollisionGrp( szFileName.c_str(), true );
}

ioOrientBox ioDummyChar::GetAttackCollisionBox( AniCollisionType eType, bool bNoScale )
{
	AniCollisionBox *pColBox = NULL;
	float fBoxScaleRate = FLOAT1;
	float fBoxMoveOffset = 0.0f;

	if( !pColBox && eType == ACT_SPECIAL )
	{
		if( m_pSpecialBox && m_pSpecialBox->HasCollisionBox( eType ) )
		{
			pColBox = m_pSpecialBox->GetCollisionBox( eType );
			fBoxMoveOffset = 0.0f;
		}
	}

	ioOrientBox kBox;
	if( pColBox )
	{
		ioNode *pNode = m_pGroup->GetParentNode();
		ioBiped *pBiped = m_pGroup->GetBiped( pColBox->m_LinkBone );
		if( pNode && pBiped )
		{
			kBox = pColBox->m_Box;

			if( fBoxMoveOffset != 0.0f )
			{
				kBox.SetCenter( kBox.GetCenter() - kBox.GetAxis(2) * fBoxMoveOffset );
			}

			D3DXMATRIX matWorld;
			D3DXMatrixMultiply( &matWorld, pBiped->GetBipedSpaceFullXform(), pNode->GetFullTransform() );
			kBox.Transform( &matWorld );
		}
	}
	else
	{
		kBox = GetWorldCollisionBox();
	}

	return kBox;
}

bool ioDummyChar::CheckEnableAttackState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return false;

	if( m_iReserveAttackCode > 0 )
		return true;

	return false;
}

void ioDummyChar::DropZoneCheck()
{
	if( m_dwDropZoneLiveTime > 0 )
	{
		bool bCheckDropZone = false;
		if( m_dwDropZoneCheckStartTime <= 0 )
			m_dwDropZoneCheckStartTime = FRAMEGETTIME();

		D3DXVECTOR3 vCurPos = GetWorldPosition();
		
		if( GetOwner() &&
			GetOwner()->GetCreator() &&
			GetOwner()->GetCreator()->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, vCurPos.y+3.0f ) <= 0.0f )
			bCheckDropZone = true;

		if( !bCheckDropZone )
			m_dwDropZoneCheckStartTime = 0;

		if( bCheckDropZone && m_dwDropZoneCheckStartTime > 0 && m_dwDropZoneCheckStartTime + m_dwDropZoneLiveTime <= FRAMEGETTIME() )
		{
			m_HP.m_fCurValue = 0.0f;
			m_LastAttacker = m_OwnerName;

			SetDieState( true );
		}
	}

	switch( m_State )
	{
	case DCS_START:
	case DCS_DIE:
		return;
	}

	switch( m_DummyCharType )
	{
	case DCT_OWNER_CONTROL:
	case DCT_OWNER_CONTROL3:
	case DCT_OWNER_CONTROL6:
	case DCT_OWNER_ATTACH:
	case DCT_MAMAHAHA:
	case DCT_RAPTOR:
	case DCT_VEHICLE:
	case DCT_MINE:
	case DCT_TARGET_TRACE6:
	case DCT_WARP_MINE:
	case DCT_TARGET_TRACE13:
	case DCT_TARGET_TRACE14:
	case DCT_FORKCRANE_DUMMY:
		return;
	}

	D3DXVECTOR3 vPos = GetWorldPosition();

	if( m_HP.m_fCurValue > 0.0f && vPos.y <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		m_LastAttacker = m_OwnerName;

		SetDieState( true );
	}
}

void ioDummyChar::SetReserveAttackCode( int iCode )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_iReserveAttackCode = iCode;
}

ioEffect* ioDummyChar::AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner )
{
	if( !m_pGroup ) return NULL;

	if( !rkEffect.IsEmpty() )
		return m_pGroup->AttachEffect( rkEffect, pEntityEventOwner );

	return NULL;
}

void ioDummyChar::EndEffect( const ioHashString &rkEffect, bool bGrace )
{
	if( !m_pGroup ) return;

	m_pGroup->EndEffect( rkEffect, bGrace );
}

void ioDummyChar::SetReservedSliding( const ForceInfoList &vForceInfoList,
									int iAniID,
									float fTimeRate,
									DWORD dwPreDelay,
									bool bMoveDir )
{
	if( iAniID == -1 ) return;

	ClearReservedSliding();

	std::list<float> vTimeList;
	m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_sliding", vTimeList );

	int iListCnt = vForceInfoList.size();
	int iCurList = 0;

	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= fTimeRate;
		dwTime += dwPreDelay;

		float fForceAmt = 0.0f;
		float fForceFriction = FLOAT1;
		float fForceAngle = 0.0f;

		if( iListCnt > iCurList )
		{
			fForceAmt = vForceInfoList[iCurList].m_fForceMovePower;
			fForceFriction = vForceInfoList[iCurList].m_fForceMoveFriction;
			fForceAngle = vForceInfoList[iCurList].m_fForceAngle;
			iCurList++;

			AddReservedSliding( dwTime, fForceAmt, fForceFriction, fForceAngle, bMoveDir );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ForceMoveLog(DummyChar) - %s fire_sliding / forcemove ( %d / %d )",
									m_pGroup->GetAnimationName(iAniID).c_str(),
									iCurList+1,
									iListCnt );

			iCurList++;
		}

		iter++;
	}
}

void ioDummyChar::AddReservedSliding( DWORD dwSlidingTime, float fForceAmt, float fFriction, float fAngle, bool bMoveDir )
{
	ReservedSliding kSliding;
	kSliding.m_dwSlidingTime = dwSlidingTime + FRAMEGETTIME();
	kSliding.m_fSlidingForce = fForceAmt;
	kSliding.m_fSlidingFriction = fFriction;
	kSliding.m_fSlidingAngle = fAngle;
	kSliding.m_bMoveDir = bMoveDir;

	m_vReservedSlidingList.push_back( kSliding );
}

void ioDummyChar::CheckReservedSliding()
{
	switch( m_DummyCharType )
	{
	case DCT_OWNER_ATTACH:
		return;
	}

	if( m_vReservedSlidingList.empty() ) return;

	bool bSetForce = false;

	ReservedSlidingList::iterator iter = m_vReservedSlidingList.begin();
	while( iter != m_vReservedSlidingList.end() )
	{
		ReservedSliding kSliding = (ReservedSliding)*iter;

		if( kSliding.m_dwSlidingTime == 0 || kSliding.m_fSlidingForce == 0.0f )
		{
			iter = m_vReservedSlidingList.erase( m_vReservedSlidingList.begin() );
			continue;
		}

		if( kSliding.m_dwSlidingTime > FRAMEGETTIME() )
			break;

		bSetForce = true;

		D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		if( kSliding.m_fSlidingAngle != 0.0f )
		{
			float fYawD = ioMath::ArrangeHead(kSliding.m_fSlidingAngle);
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
			vDir = qtRotate * vDir;
		}

		SetForcePower( vDir, kSliding.m_fSlidingForce, kSliding.m_fSlidingFriction );
		iter = m_vReservedSlidingList.erase( m_vReservedSlidingList.begin() );
	}
}

void ioDummyChar::ClearReservedSliding()
{
	m_vReservedSlidingList.clear();
}

void ioDummyChar::SetForcePower( const D3DXVECTOR3 &vForceDir,
							   float fForceAmt,
							   float fFrictionRate,
							   bool bForceLinear )
{
	m_vForceDir = vForceDir;
	m_fForceAmt = fForceAmt;
	m_fForceFrictionRate = fFrictionRate;

	m_bForcePowerLinear = bForceLinear;
}

void ioDummyChar::ProcessForceMove( float fTimePerSec )
{
	if( m_DummyCharType == DCT_TARGET_TRACE3 )
		return;

	if( m_fForceAmt <= 0.0f )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	D3DXVECTOR3 vMoveAmt;
	float fCurAccelAmt = 0.0f;

	if( !m_bForcePowerLinear )
	{
		fCurAccelAmt -= ( pStage->GetRoomFriction() * m_fForceFrictionRate ) * fTimePerSec;
	}

	// (v*dt) + (a*dt*dt)*0.5f 공식 적용
	// GetRoomFriction() * m_fForceFrictionRate 를 a로 보고
	// fCurAccelAmt 계산시 이미 fTimerPerSec를 한번 곱했기때문에 최종계산에서는 fTimerPerSec를 한번만 곱한다.
	float fFinalMoveAmt = m_fForceAmt * fTimePerSec + fCurAccelAmt * fTimePerSec * FLOAT05;
	
	m_fTestValue += fFinalMoveAmt;

	if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
	{
		vMoveAmt = m_vForceDir * fFinalMoveAmt;
		m_fForceAmt += fCurAccelAmt;
		m_fForceAmt = max( 0.0f, m_fForceAmt );

		if( vMoveAmt.y <= 0.0f )
			vMoveAmt.y = 0.0f;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( this, &vMoveAmt, bCol ) )
		{
			if( m_DummyCharType == DCT_OWNER_CONTROL || m_DummyCharType == DCT_OWNER_CONTROL5 || m_DummyCharType == DCT_OWNER_CONTROL6 )
			{
				ioBaseChar *pOwner = GetOwner();
				if( !pOwner )	return;
				D3DXVECTOR3 vAfterForcePos = GetWorldPosition()+vMoveAmt;
				D3DXVECTOR3 vDiff = vAfterForcePos - pOwner->GetWorldPosition();
				float fDistanceSq = D3DXVec3LengthSq( &vDiff );

				if( m_fControlOutRange * m_fControlOutRange < fDistanceSq )
				{
					return;
				}
			}

			Translate( vMoveAmt );
		}

		if( m_bDestroyMapCol && bCol )
		{
			SendDieState( true );
			return;
		}

		return;
	}

	if( m_fForceAmt <= 5.0f )
		m_fForceAmt = 0.0f;
}

void ioDummyChar::MapCollisionPushingTranslate( const D3DXVECTOR3 &vMoveAmt )
{
	ioPlayEntity::MapCollisionPushingTranslate( vMoveAmt );
}

void ioDummyChar::CheckDestroyRange()
{
	if( m_DummyCharType == DCT_OWNER_CONTROL || m_DummyCharType == DCT_OWNER_CONTROL5 || m_DummyCharType == DCT_OWNER_CONTROL6 ||
		m_DummyCharType == DCT_MOVE_ATTACK2 )
		return;

	if( m_fControlOutRange <= 0.0f )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	switch( m_State )
	{
	case DCS_START:
	case DCS_DIE:
		return;
	}

	D3DXVECTOR3 vDiff = GetWorldPosition() - pOwner->GetWorldPosition();
	float fDistanceSq = D3DXVec3LengthSq( &vDiff );

	if( m_fControlOutRange * m_fControlOutRange < fDistanceSq )
	{
		SendDieState( true );
	}
}

void ioDummyChar::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	ApplyDummyCharSyncByType( iType, rkPacket, pStage );
}

void ioDummyChar::ApplyDummyCharSyncByType( int iType, SP2Packet &rkPacket, ioPlayStage *pStage )
{
	if( !pStage ) return;

	switch( iType )
	{
	case DUMMY_CHAR_DAMAGE:
		{
			ioHashString szAttacker;
			rkPacket >> szAttacker;

			ioBaseChar *pAttacker = pStage->GetBaseChar( szAttacker );
			if( !pAttacker )
			{
				LOG.PrintTimeAndLog( 0, "ioDummyChar::ApplyDummyCharSync - Attacker(%s) Not Exist", szAttacker.c_str() );
				return;
			}

			OnDamaged( rkPacket, pAttacker );
		}
		break;
	case DUMMY_CHAR_DIE:
		{
			bool bEffect;
			rkPacket >> bEffect;
			SetDieState( bEffect );
		}
		break;
	case DUMMY_CHAR_DELAY:
		{
			SetDelayState( false );
		}
		break;
	case DUMMY_CHAR_NONE_DELAY:
		{
			SetNoneDelayState( false );
		}
		break;
	case DUMMY_CHAR_KEY_CHANGE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bSetUpKey >> m_bSetDownKey;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

			bool bSetMove;
			rkPacket >> bSetMove;

			if( bSetMove )
			{
				SetMoveState();
			}

			D3DXVECTOR3	vPos;
			rkPacket >> vPos;
			SetWorldPosition( vPos );
		}
		break;
	case DUMMY_CHAR_NORMAL_ATT:
		{
			m_iReserveAttackCode = m_iControlNormalAttack;
			SetAttackState();
		}
		break;
	case DUMMY_CHAR_CHARGE_ATT:
		{
			bool bStart;
			rkPacket >> bStart;
			SetControlAttackState( bStart );
		}
		break;
	case DUMMY_CHAR_TRACE:
		{
			bool bFirst;
			rkPacket >> m_TargetTraceName;
			rkPacket >> bFirst;

			if( bFirst )
			{
				m_State = DCS_TRACE;
				SetTracingState( pStage, false );
			}
		}
		break;
	case DUMMY_CHAR_TRACE_TARGET:
		{
			rkPacket >> m_vTracingMoveDir;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			SetWorldPosition( vPos );

			int iTraceState;
			rkPacket >> iTraceState;
			m_TraceState = (DummyCharTraceState)iTraceState;

			rkPacket >> m_szCurTargetName;

			SetMoveState();
		}
		break;
	case DUMMY_CHAR_TRACE_TARGET_ATTACK:
		{
			int iState;
			rkPacket >> iState;
			m_State = (DummyCharState)iState;
			rkPacket >> m_dwBaseWeaponIndex;
			rkPacket >> m_szCurTargetName;
			m_dwNextFireTime = 0;
			SetTraceTargetAttack();
		}
		break;
	case DUMMY_CHAR_DUMMY_TOWER:
		{
			int iCurIndex;
			float fStartAngle;
			D3DXVECTOR3 vTargetPos;
			ioHashString szDummyName;

			rkPacket >> szDummyName >> iCurIndex;
			rkPacket >> fStartAngle;
			rkPacket >> vTargetPos;

			bool bRot;
			rkPacket >> bRot;

			D3DXQUATERNION qtTargetRot;
			if( bRot )
				rkPacket >> qtTargetRot;

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyName, iCurIndex,
														   GetOwnerName(),
														   vTargetPos,
														   fStartAngle,
														   0, false );

			if( pDummy && bRot )
			{
				pDummy->SetWorldOrientationAndDir( qtTargetRot );
			}
		}
		break;
	case DUMMY_CHAR_TRACE_ATTACK_SYNC:
		{
			SetTracingAttack();
		}
		break;
	}
}

bool ioDummyChar::CheckOwnerDestroyState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( m_bDiableOwnerWoundDestroy )
	{
		return false;
	}

	CharState eState = pOwner->GetState();

	switch( m_DummyCharType )
	{
	case DCT_OWNER_CONTROL:
	case DCT_OWNER_CONTROL3:
	case DCT_OWNER_CONTROL5:
	case DCT_OWNER_CONTROL6:
	case DCT_OWNER_ATTACH:
		{
			if( !pOwner->IsCurUsingSkill( m_ControlSkillName ) )
			{
				m_ControlSkillName.Clear();
				return true;
			}
		}
		break;
	case DCT_OWNER_ATTACH2:
	case DCT_TARGET_TRACE3:
	case DCT_MAMAHAHA:
	case DCT_RAPTOR:
	case DCT_VEHICLE:
	case DCT_OWNER_CONTROL2:
	case DCT_CURSE_DOLL:
	case DCT_MOVE_ATTACK2:
	case DCT_COPY_OWNER2:
	case DCT_COPY_OWNER3:
	case DCT_COPY_OWNER5:
	case DCT_MINE:
	case DCT_ATTACH_WEAPON:
	case DCT_OWNER_ATTACH5:
	case DCT_WARP_MINE:
	case DCT_FORKCRANE_DUMMY:
		break;
	default:
		{
			if( eState == CS_FROZEN && pOwner->GetBlowFrozenState() )
				return true;
			else if( eState == CS_BLOW_WOUNDED && !pOwner->IsFloatingState() )
				return true;
			else if( eState == CS_BOUND_BLOW_WOUNDED && !pOwner->IsFloatingState() )
				return true;
			else if( eState == CS_ETC_STATE && pOwner->IsApplyDownState(false) )
				return true;
			else if( eState == CS_STOP_MOTION && pOwner->GetBlowStopMotionState() )
				return true;
			else if( eState == CS_DROP_ZONE_DOWN )
				return true;
			else if( eState == CS_ICE_STATE && pOwner->GetBlowStopMotionState() )
				return true;
		}
		break;
	}

	return false;
}

void ioDummyChar::CheckExtraAniJump()
{
	if( m_vExtraAniJumpList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwExtraAniJumpTime > 0 && m_dwExtraAniJumpTime < dwCurTime )
	{
		if( m_fExtraAniJumpPower > 0.0f )
		{
			m_fCurJumpPowerAmt = m_fExtraAniJumpPower;
		}

		m_iCurExtraAniJump++;

		int iSize = m_vExtraAniJumpList.size();
		if( COMPARE(m_iCurExtraAniJump, 0, iSize) )
		{
			m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
			m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
		}
		else
		{
			m_dwExtraAniJumpTime = 0;
			m_fExtraAniJumpPower = 0.0f;

		}
	}
}

bool ioDummyChar::CheckAttackKeyControl()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return false;

	if( !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bAttackToDestroyDummy )
	{
		if( pOwner->IsAttackKey() )
		{
			SetDieState( true );
			return true;
		}
	}
	else if( m_dwAttackKeyCheckTime == 0 )
	{
		if( pOwner->IsAttackKeyDown() )
		{
			m_dwAttackKeyCheckTime = dwCurTime;
			return false;
		}
	}
	else
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			// 수평공격 발생.
			m_iReserveAttackCode = m_iControlNormalAttack;
			SetAttackState();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DUMMY_CHAR_NORMAL_ATT;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			return true;
		}
		else if( pOwner->IsAttackKeyDown() && m_dwAttackKeyCheckTime+m_dwAttackChargeTime < dwCurTime )
		{
			// 수직공격 발생.
			SetControlAttackState( true );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DUMMY_CHAR_CHARGE_ATT;
				kPacket << true;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return true;
		}
	}

	return false;
}

void ioDummyChar::CheckKeyInputChange()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	bool bChange = false;

	// 방향변화 체크
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChange = true;
		m_CurDirKey = eNewDirKey;
	}

	// Up 체크
	if( !bChange && !m_bSetDownKey )
	{
		if( m_bSetUpKey && !pOwner->IsJumpKeyDown() )
		{
			bChange = true;
			m_bSetUpKey = false;
		}
		else if( !m_bSetUpKey && pOwner->IsJumpKeyDown() )
		{
			bChange = true;
			m_bSetUpKey = true;
		}
	}

	// Down 체크
	if( !bChange && !m_bSetUpKey && !m_bAutoDescent )
	{
		if( m_bSetDownKey && !pOwner->IsDefenseKeyDown() )
		{
			bChange = true;
			m_bSetDownKey = false;
		}
		else if( !m_bSetDownKey && pOwner->IsDefenseKeyDown() )
		{
			bChange = true;
			m_bSetDownKey = true;
		}
	}

	if( bChange && pOwner->IsNeedSendNetwork() && m_bSyncForOtherUser )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_KEY_CHANGE;
		kPacket << (int)m_CurDirKey;
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		kPacket << false;			// setmove...
		kPacket << GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChar::ProcessDelayOwnerControl( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		bool bChange = false;

		// 방향변화 체크
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			bChange = true;
			m_CurDirKey = eNewDirKey;
		}

		// Up 체크
		if( !bChange && !m_bSetDownKey )
		{
			if( pOwner->IsJumpKeyDown() )
			{
				bChange = true;
				m_bSetUpKey = true;
			}
		}

		// Down 체크
		if( !bChange && !m_bSetUpKey && !m_bAutoDescent )
		{
			if( pOwner->IsDefenseKeyDown() )
			{
				bChange = true;
				m_bSetDownKey = true;
			}
		}

		if( bChange )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DUMMY_CHAR_KEY_CHANGE;
			kPacket << (int)m_CurDirKey;
			kPacket << m_bSetUpKey;
			kPacket << m_bSetDownKey;
			kPacket << true;			// setmove...
			kPacket << GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
			SetMoveState();
		}

		// 공격 체크
		if( !bChange && CheckAttackKeyControl() )
			return;
	}

	if( m_bAutoDescent && !m_bSetUpKey )
		SetMoveState();
}

void ioDummyChar::ProcessMoveOwnerControl( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( pOwner->IsNeedProcess() &&
		!pOwner->IsSettedDirection() && !pOwner->IsJumpKeyDown() && ( !pOwner->IsDefenseKeyDown() && !m_bAutoDescent ) )
	{
		SetDelayState( true );
		return;
	}

	// 공격 체크
	if( CheckAttackKeyControl() )
		return;

	CheckKeyInputChange();
	CheckControlMove( fTimePerSec );
}

void ioDummyChar::ProcessChargeOwnerControl( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			m_dwAttackKeyCheckTime = 0;
			SetDelayState( true );
			return;
		}
		else if( m_dwAttackChargeRepeatTime != 0 && pOwner->IsAttackKeyDown() && m_dwAttackKeyCheckTime+m_dwAttackChargeRepeatTime < dwCurTime )
		{
			SetControlAttackState( false );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DUMMY_CHAR_CHARGE_ATT;
				kPacket << false;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	
	CheckKeyInputChange();
	CheckControlMove( fTimePerSec );
}

void ioDummyChar::CheckControlMove( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;

	bool bChargingAttack = false;
	if( m_iChargeWeaponIndex > 0 )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_iChargeWeaponIndex );
		if( pWeapon && pWeapon->IsLive() )
			bChargingAttack = true;
	}

	bool bMove = false;
	
	float fMapHeight = GetBottomHeight();
	float fHeightAmt = 0.0f;

	D3DXVECTOR3 vCurDir = ioMath::VEC3_ZERO;

	// 수평 이동
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		switch(m_CurDirKey)
		{
		case ioUserKeyInput::DKI_UP:
			vCurDir	+= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_DOWN:
			vCurDir	-= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_LEFT:
			vCurDir	-= ioMath::UNIT_X;
			break;
		case ioUserKeyInput::DKI_RIGHT:
			vCurDir	+= ioMath::UNIT_X;
			break;
		case ioUserKeyInput::DKI_LEFTDOWN:
			vCurDir	-= ioMath::UNIT_X;	vCurDir	-= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_LEFTUP:
			vCurDir	-= ioMath::UNIT_X;	vCurDir	+= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_RIGHTDOWN:
			vCurDir	+= ioMath::UNIT_X;	vCurDir	-= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_RIGHTUP:
			vCurDir	+= ioMath::UNIT_X;	vCurDir	+= ioMath::UNIT_Z;
			break;
		};
		bMove = true;
	}

	// 수직이동
	float fUpDownSpeed = m_fUpDownSpeed * fTimePerSec;
	float fOwnerCharHeightGap = ( GetWorldPosition().y + fUpDownSpeed ) - pOwner->GetBottomHeight();
	float fHeightGap = ( GetWorldPosition().y + fUpDownSpeed ) - fMapHeight;

	// 위로
	if( m_bSetUpKey )
	{
		// 상승 최대치에 대한 보정.		
		float fCompareHeightGap = fHeightGap;
		if( GetType() == DCT_OWNER_CONTROL6 )
			fCompareHeightGap = fOwnerCharHeightGap;
		if( fCompareHeightGap < m_fMaxHeight )
		{
			fHeightAmt += fUpDownSpeed;
		}
		bMove = true;
	}
	else if( m_bAutoDescent )
	{
		fHeightAmt -= fUpDownSpeed;
		bMove = true;
	}

	// 아래로
	if( m_bSetDownKey )
	{
		fHeightAmt -= fUpDownSpeed;
		bMove = true;
	}

	if( !bMove )
		return;

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	D3DXVec3Normalize( &vCurDir, &vCurDir );
	D3DXVECTOR3 vTotalMove = ioMath::VEC3_ZERO;

	float fMoveSpeed = 0.0f;

	if( bChargingAttack )
		fMoveSpeed = m_fMoveSpeedOnCharging * fTimePerSec;
	else
		fMoveSpeed = m_fMoveSpeed * fTimePerSec;

	vTotalMove = vCurDir * fMoveSpeed;
	
	D3DXVECTOR3 vDiff = ( vCurPos + vTotalMove ) - pOwner->GetWorldPosition();
	float fDistanceSq = D3DXVec3LengthSq( &vDiff );

	if( m_fControlOutRange * m_fControlOutRange < fDistanceSq )
	{
		if( GetType() == DCT_OWNER_CONTROL6 )
		{
			vTotalMove = vCurDir * ( fMoveSpeed * 0.7f );

			vDiff = ( vCurPos + vTotalMove ) - pOwner->GetWorldPosition();

			D3DXVec3Normalize( &vDiff, &vDiff );
			vDiff *= m_fControlOutRange;

			float fCurHeight = vCurPos.y;
			D3DXVECTOR3 vDummyPos1 = pOwner->GetWorldPosition() + vDiff;
			vDummyPos1.y = fCurHeight;
			SetWorldPosition( vDummyPos1 );
		}
		else
			return;
	}
	else if ( m_fControlMinRange * m_fControlMinRange > fDistanceSq )
	{
		if( GetType() == DCT_OWNER_CONTROL6 )
		{
			fHeightAmt = 0.0f;
			if( m_bSetUpKey )
				fHeightAmt += ( fUpDownSpeed * 0.7f );
			if( m_bSetDownKey )
				fHeightAmt -= ( fUpDownSpeed * 0.7f );

			vTotalMove = vCurDir * ( fMoveSpeed * 0.7f );

			vDiff = ( vCurPos + vTotalMove ) - pOwner->GetWorldPosition();

			D3DXVec3Normalize( &vDiff, &vDiff );
			vDiff *= m_fControlMinRange;

			float fCurHeight = vCurPos.y;
			D3DXVECTOR3 vDummyPos2 = pOwner->GetWorldPosition() + vDiff;
			vDummyPos2.y = fCurHeight;
			SetWorldPosition( vDummyPos2 );
		}
		else if( !m_bSetUpKey )
			return;
	}

	// 더미 회전
	if( vCurDir != ioMath::VEC3_ZERO )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vCurDir );
		SetWorldOrientation( qtRot );
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( m_bMountEnable )
	{
		UpdateMountedEntityList();

		if( pStage && pStage->ReCalculateMoveVectorByTerrain( this, &vTotalMove, bCol, m_vMountEntityList ) )
		{
			Translate( vTotalMove );
		}
	}
	else
	{
		if( pStage && pStage->ReCalculateMoveVectorByTerrain( this, &vTotalMove, bCol ) )
		{
			Translate( vTotalMove );
		}
	}

	m_fGravityAmt = 0.0f;

	// 위치가 갱신되었을테니 다시 값을 가져온다.	
	vCurPos = GetWorldPosition();	
	vCurPos.y += fHeightAmt;
	
	ioOrientBox kBox = GetWorldCollisionBox();
	float fDownHeight = vCurPos.y - kBox.GetExtents(1);
	if( fDownHeight < fMapHeight && m_bCheckMapCollision )
	{
		vCurPos.y = fMapHeight + kBox.GetExtents(1);
	}

	if( pStage && m_bCheckMapCollision )
	{
		pStage->CheckFlyMapLimit( &vCurPos );
		pStage->CheckAxisLimit( &vCurPos );
	}
	
	D3DXVECTOR3 vPrePos = GetWorldPosition();
	SetWorldPosition( vCurPos );
	float fDuumyGap = fabs(vPrePos.y - vCurPos.y);			
	if( 0 < fDuumyGap && m_bCheckMapCollision )
		SetWorldPositionMountEntity( fHeightAmt );
}

void ioDummyChar::CheckChargeWeaponDead()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_iChargeWeaponIndex > 0 && !m_bIgnoreChargeWeaponDead )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_iChargeWeaponIndex );
		if( pWeapon )
		{
			m_iChargeWeaponIndex = 0;
			pWeapon->SetWeaponDead();
		}
	}
	StopSound( m_szControlAttackSound, m_AttackSoundID );
}

void ioDummyChar::SetControlSkillName( const ioHashString &szSkillName )
{
	m_ControlSkillName = szSkillName;
}

D3DXVECTOR3 ioDummyChar::GetCameraPosition()
{
	if( m_fCameraHeight == 0.0f )	return GetWorldPosition();

	D3DXVECTOR3	vPos = GetWorldPosition();
	vPos += D3DXVECTOR3( 0.0f, m_fCameraHeight, 0.0f );
	return vPos;
}

bool ioDummyChar::IsDisableRecalculateByterrain()
{
	return	m_bDisableRecalcByTerrain;
}

void ioDummyChar::SendAttackInfoToSkill()
{
	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
	{
		ioEquipSlot *pSlot = pOwner->GetEquipSlot();
		if( pSlot )
		{
			ioControlDummySkill *pSkill1 = ToControlDummySkill( pSlot->GetEquipedSkill( m_ControlSkillName ) );
			if( pSkill1 )
			{
				pSkill1->StartReduceGuage();
			}

			ioControlDummySkill3 *pSkill3 = ToControlDummySkill3( pSlot->GetEquipedSkill( m_ControlSkillName ) );
			if( pSkill3 )
			{
				pSkill3->StartReduceGuage();
			}
		}
	}
}

UniqueObjID ioDummyChar::PlaySound( ioBaseChar *pChar, ioHashString &szSoundName, bool bLoop )
{
	UniqueObjID eSoundID = 0;
	if( szSoundName.IsEmpty() )
		return eSoundID;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( bLoop )
		eSoundID = g_SoundMgr.PlaySound( szSoundName, pGrp->GetParentSceneNode(), PSM_LOOP | PSM_THREAD );
	else
		eSoundID = g_SoundMgr.PlaySound( szSoundName, pGrp->GetParentSceneNode(), PSM_THREAD );

	return eSoundID;
}

void ioDummyChar::StopSound( ioHashString &szSoundName, UniqueObjID eSoundID )
{
	if( szSoundName.IsEmpty() )	return;

	g_SoundMgr.StopSound( szSoundName, eSoundID );
}

void ioDummyChar::SetTraceState( DummyCharTraceState iTraceState )
{
	m_State = DCS_TRACE;
	m_TraceState = iTraceState;

	m_dwSensingStartTime = FRAMEGETTIME();

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_SensingAni );
	if( 0 <= iAniID )
	{
		float fRate = FLOAT1;
		if( m_fSensingAniRate > 0.0f )
			fRate = m_fSensingAniRate;
		
		m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );
	}
}

void ioDummyChar::SetTraceWaitState()
{
	if( m_TraceState == DCTS_TRACING_WAIT )
		return;

	m_State = DCS_TRACE;
	m_TraceState = DCTS_TRACING_WAIT;

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );

	if( iAniID != -1 )
		m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 );
}

void ioDummyChar::ProcessTraceSensing( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_TargetTraceName.IsEmpty() )
		m_TargetTraceName.Clear();

	// Sensing 시간동안 타겟 잡지 못하면 이동만 한다
	if( m_DummyCharType != DCT_TARGET_TRACE5 && m_DummyCharType != DCT_TARGET_TRACE6 && m_DummyCharType != DCT_COPY_OWNER3 )
	{
		if( m_dwSensingStartTime + m_dwSensingDuration < dwCurTime )
		{
			float fMoveDist = m_fTraceMoveSpeed * fTimePerSec;
			if( m_DummyCharType	!= 	DCT_TARGET_TRACE4 && m_DummyCharType != DCT_TARGET_TRACE7 )
				ProcessTraceMoving( pStage, fMoveDist, fTimePerSec );
			else
				ProcessTraceMovingNoneTerrain( pStage, fMoveDist, fTimePerSec );
			return;
		}

		float fMoveDist = m_fTraceMoveSpeed * fTimePerSec;
		if( m_DummyCharType	!= 	DCT_TARGET_TRACE4 && m_DummyCharType != DCT_TARGET_TRACE7 )
			ProcessTraceMoving( pStage, fMoveDist, fTimePerSec );
		else
			ProcessTraceMovingNoneTerrain( pStage, fMoveDist, fTimePerSec );
	}

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	ioHashString szTarget;
	if( m_DummyCharType	== 	DCT_TARGET_TRACE4 )
	{
		szTarget = GetTraceTargetNoOverlap( pStage, false );

		if( !m_stNoOverLapTargetName.IsEmpty() )
			g_DummyCharMgr.EraseNoOverLapTrackingDummy( pOwner->GetCharName(), m_stNoOverLapTargetName );

		m_stNoOverLapTargetName = szTarget;

		if( !m_stNoOverLapTargetName.IsEmpty() )
			g_DummyCharMgr.AddNoOverLapTrackingDummy( pOwner->GetCharName(), szTarget );
	}
	else
	{
		szTarget = GetTraceTarget( pStage, false );
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_TargetTraceName = szTarget;
			SetTracingState( pStage, true );
		}
	}
}

void ioDummyChar::ProcessTraceTracingNoneTerrain( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwTracingEndTime > dwCurTime )
	{
		CheckTraceTarget( pStage );
		FindTraceTarget( pStage );

		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );
		if( pTarget )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vPrePos;
			D3DXVec3Normalize( &vDiff, &vDiff );
			m_vTracingMoveDir = vDiff;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			if( fRangeSq < m_fTraceRange * m_fTraceRange )
			{
				D3DXVECTOR3 vTargetDir = vDiff;
				D3DXVECTOR3 vTargetDir2 = vDiff;
				vTargetDir.y = 0;
				D3DXVec3Normalize( &vTargetDir, &vTargetDir );
				D3DXVec3Normalize( &vTargetDir2, &vTargetDir2 );

				if( m_dwRotateSpeed > 0 )
				{
					D3DXQUATERNION	qtRot;
					ioMath::CalcDirectionQuaternion( qtRot, -vTargetDir );

					float fAngle = D3DXVec3Dot( &vTargetDir, &m_vTracingMoveDir );

					float fAngleGap = RADtoDEG( acosf(fAngle) );
					float fRate = (fTimePerSec*1000) / m_dwRotateSpeed;
					float fRotateAngle = 360.0f * fRate;

					if( fAngleGap < fRotateAngle )
					{
						m_vTracingMoveDir = vTargetDir2;
					}
					else
					{
						D3DXVECTOR3 vCloneDir = m_vTracingMoveDir;
						vCloneDir.y = 0;
						D3DXVec3Normalize( &vCloneDir, &vCloneDir );

						D3DXQUATERNION qtRotate;
						ioMath::CalcDirectionQuaternion( qtRotate, -vCloneDir );

						bool bLeftRot = false;
						if( D3DXVec3Dot( &vTargetDir, &vCloneDir ) > -FLOAT1 )
						{
							float fCurYaw, fTargetYaw;
							fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
							fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

							fCurYaw = ioMath::ArrangeHead( fCurYaw );
							fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

							float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
							if( fRotAngle < 0.0f )
								bLeftRot = true;
						}

						if( bLeftRot )
							fRotateAngle *= -1;

						fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

						D3DXQUATERNION	qtNewRot;
						D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
						m_vTracingMoveDir = qtNewRot * m_vTracingMoveDir;
					}
				}
				else
				{
					m_vTracingMoveDir = vTargetDir2;
				}
			}
		}
		else
		{
			m_TargetTraceName.Clear();
		}
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	ProcessTraceMovingNoneTerrain( pStage, fMoveDist, fTimePerSec );
}

void ioDummyChar::ProcessTraceTracing( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	if( m_dwTracingEndTime > dwCurTime )
	{
		CheckTraceTarget( pStage );
		FindTraceTarget( pStage );

		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );		
		if( !IsTargetWoundedState( pTarget ) )
		{
			SetTraceWaitState();
			return;
		}

		if( pTarget )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			if( fRangeSq < m_fTraceRange * m_fTraceRange )
			{
				D3DXVECTOR3 vTargetDir = vDiff;
				vTargetDir.y = 0;
				D3DXVec3Normalize( &vTargetDir, &vTargetDir );

				if( m_dwRotateSpeed > 0 )
				{
					D3DXQUATERNION	qtRot;
					ioMath::CalcDirectionQuaternion( qtRot, -vTargetDir );

					float fAngle = D3DXVec3Dot( &vTargetDir, &m_vTracingMoveDir );

					float fAngleGap = RADtoDEG( acosf(fAngle) );
					float fRate = (fTimePerSec*1000) / m_dwRotateSpeed;
					float fRotateAngle = 360.0f * fRate;

					if( fAngleGap < fRotateAngle )
					{
						m_vTracingMoveDir = vTargetDir;
					}
					else
					{
						D3DXQUATERNION qtRotate;
						ioMath::CalcDirectionQuaternion( qtRotate, -m_vTracingMoveDir );

						bool bLeftRot = false;
						if( D3DXVec3Dot( &vTargetDir, &m_vTracingMoveDir ) > -FLOAT1 )
						{
							float fCurYaw, fTargetYaw;
							fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
							fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

							fCurYaw = ioMath::ArrangeHead( fCurYaw );
							fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

							float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
							if( fRotAngle < 0.0f )
								bLeftRot = true;
						}

						if( bLeftRot )
							fRotateAngle *= -1;

						fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

						D3DXQUATERNION	qtNewRot;
						D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
						m_vTracingMoveDir = qtNewRot * m_vTracingMoveDir;
					}
				}
				else
				{
					m_vTracingMoveDir = vTargetDir;
				}
			}
		}
		else
		{
			m_TargetTraceName.Clear();
		}
	}
	else if( m_DummyCharType == DCT_TARGET_TRACE5 || m_DummyCharType == DCT_TARGET_TRACE6 )
	{
		SetTraceState( DCTS_SENSING );
		return;
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	ProcessTraceMoving( pStage, fMoveDist, fTimePerSec );
}

void ioDummyChar::ProcessTraceTracingWait( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioHashString szTarget = GetTraceTarget( pStage, false );
	ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );		
	if( !pTarget )
		SetDelayState( false );

	if( IsTargetWoundedState( pTarget ) )
	{
		SetDelayState( false );
		return;
	}
}

void ioDummyChar::ProcessTraceAttack( float fTimePerSec, DWORD dwPreTime )
{
	static int i = 0;
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	DWORD dwLastFireTime = dwPreTime;
	FireTimeList::iterator iter = m_vAttackFireTimeList.begin();
	while( iter != m_vAttackFireTimeList.end() )
	{
		const FireTime &rkFire = *iter;

		DWORD dwFireTime = rkFire.dwStart;
		if( COMPARE( dwFireTime, dwPreTime, dwCurTime ) )
		{
			if( rkFire.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
			{
				pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime < dwCurTime )
	{
		if( m_DummyCharType == DCT_TARGET_TRACE5 || m_DummyCharType == DCT_TARGET_TRACE6 )
		{
			m_LastAttacker = m_OwnerName;
			SetDelayState( false );
			return;
		}
		else
		{
			// die 처리
			m_LastAttacker = m_OwnerName;
			SendDieState( true );
			return;
		}
	}
}

void ioDummyChar::ProcessTraceMovingNoneTerrain( ioPlayStage *pStage, float fMoveAmt, float fTimePerSec )
{
	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vPos = vPrePos;

	D3DXVec3Normalize( &m_vTracingMoveDir, &m_vTracingMoveDir );

	vPos += m_vTracingMoveDir * fMoveAmt;

	SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vTemp = m_vTracingMoveDir;
	vTemp.y = 0;
	D3DXVec3Normalize( &vTemp, &vTemp);
	ioMath::CalcDirectionQuaternion( qtRot, -vTemp );
	SetWorldOrientation( qtRot );
}

void ioDummyChar::ProcessTraceMoving( ioPlayStage *pStage, float fMoveAmt, float fTimePerSec )
{
	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vPos = vPrePos;

	m_vTracingMoveDir.y = 0.0f;
	D3DXVec3Normalize( &m_vTracingMoveDir, &m_vTracingMoveDir );

	vPos += m_vTracingMoveDir * fMoveAmt;

	float fTopHeight = m_fEnableTraceMoveHeight * 4.0f;
	float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+fTopHeight );
	float fGapHeight = fMapHeight - vPos.y;

	if( m_fEnableTraceMoveHeight < fabs(fGapHeight) )
	{
		if( fGapHeight <= 0.0f )
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -m_vTracingMoveDir );
			SetWorldOrientation( qtRot );

			if( m_bCharColOnlyAttack )
			{
				m_LastAttacker = m_OwnerName;
				SendDieState( true );
			}
			else
			{
				SetTracingAttack();
			}
			
			return;
		}
		else
		{
			float fNewMapHeight = 0.0f;
			D3DXVECTOR3 vDiff = vPrePos;

			if( fabs(m_vTracingMoveDir.x) >= fabs(m_vTracingMoveDir.z) )
			{
				if( m_vTracingMoveDir.x >= 0.0f )
					vDiff += D3DXVECTOR3( FLOAT1, 0.0f, 0.0f ) * fMoveAmt;
				else
					vDiff += D3DXVECTOR3( -FLOAT1, 0.0f, 0.0f ) * fMoveAmt;
			}
			else
			{
				if( m_vTracingMoveDir.z >= 0.0f )
					vDiff += D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 ) * fMoveAmt;
				else
					vDiff += D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * fMoveAmt;
			}

			fNewMapHeight = pStage->GetMapHeightNoneEntity( vDiff.x, vDiff.z, vDiff.y+fTopHeight );

			if( m_fEnableTraceMoveHeight < fabs(vDiff.y - fNewMapHeight) )
				return;

			vPos = vDiff;
			fMapHeight = fNewMapHeight;
		}
	}

	vPos.y = fMapHeight;
	SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vTracingMoveDir );
	SetWorldOrientation( qtRot );
}

bool ioDummyChar::IsEnableTargetState( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( pChar->IsSystemState() )
		return false;
		
	if( !pChar->IsEnableTarget() ) 	
		return false;

	// 쓰러짐 관련 체크
	if( !pChar->CheckTargetWoundType( m_TargetWoundType ) )
		return false;

	return true;
}

void ioDummyChar::SetTracingState( ioPlayStage *pStage, bool bSendNetwork )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_TraceState = DCTS_TRACING;
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwTracingEndTime = dwCurTime + m_dwTraceDuration;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DUMMY_CHAR_TRACE;
			kPacket << m_TargetTraceName;
			kPacket << true;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_TracingAni );
	float fRate = FLOAT1;
	if( m_fTracingAniRate > 0.0f )
		fRate = m_fTracingAniRate;

	if( iAniID != -1 )
		m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );

	SetAttachWeapon();
}

void ioDummyChar::FindTraceTarget( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	bool bSecondRange = false;
	if( !m_TargetTraceName.IsEmpty() )
		bSecondRange = true;

	DWORD dwCurTime = FRAMEGETTIME();

	ioHashString szTarget;
	if( m_DummyCharType	== 	DCT_TARGET_TRACE4 )
	{
		if( m_TargetTraceName.IsEmpty() )
		{
			szTarget = GetTraceTargetNoOverlap( pStage, false );

			if( !m_stNoOverLapTargetName.IsEmpty() )
				g_DummyCharMgr.EraseNoOverLapTrackingDummy( pOwner->GetCharName(), m_stNoOverLapTargetName );

			m_stNoOverLapTargetName = szTarget;

			if( !m_stNoOverLapTargetName.IsEmpty() )
				g_DummyCharMgr.AddNoOverLapTrackingDummy( pOwner->GetCharName(), szTarget );
		}
	}
	else
		szTarget = GetTraceTarget( pStage, false );

	if( !szTarget.IsEmpty() && szTarget != m_TargetTraceName )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_TargetTraceName = szTarget;

			if( pTarget->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DUMMY_CHAR_TRACE;
				kPacket << m_TargetTraceName;
				kPacket << false;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioDummyChar::CheckTraceTarget( ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( m_TargetTraceName.IsEmpty() ) return;

	ioHashString szTarget = m_TargetTraceName;
	bool bLost = false;
	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );
	if( pTarget )
	{
		if( !m_TargetTraceName.IsEmpty() && pTarget->IsSystemState() )
		{
			m_TargetTraceName.Clear();
		}
		
		if( !m_TargetTraceName.IsEmpty() && !pTarget->IsEnableTarget() )
		{
			m_TargetTraceName.Clear();
		}

		if( !m_TargetTraceName.IsEmpty() )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			float fRange = D3DXVec3Length( & vDiff );
			if( m_fTraceRange > 0.0f && fRangeSq >= m_fTraceRange * m_fTraceRange )
			{
				m_TargetTraceName.Clear();
			}
		}
	}
	else
	{
		m_TargetTraceName.Clear();
	}

	if( m_TargetTraceName.IsEmpty() )
		bLost = true;

	if( bLost )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DUMMY_CHAR_TRACE;
			kPacket << m_TargetTraceName;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

ioHashString ioDummyChar::GetTraceTarget( ioPlayStage *pStage, bool bSecond )
{
	ioHashString szTarget;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return szTarget;
	if( !pStage )
		return szTarget;

	if( pOwner->HasBuff( BT_SCREEN_BLIND ) || pOwner->HasBuff( BT_SCREEN_BLIND2 ) )
		return szTarget;

	float fSensingRangeSq = 0.0f;
	if( bSecond )
		fSensingRangeSq = m_fSensingRange2 * m_fSensingRange2;
	else
		fSensingRangeSq = m_fSensingRange * m_fSensingRange;

	if( fSensingRangeSq == 0.0f )
		return szTarget;

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetWorldPosition();

	float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;

		if( !pChar ) continue;
		if( pChar->GetTeam() == pOwner->GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( !IsEnableTargetState( pChar ) )
			continue;

		if(( m_DummyCharType == DCT_TARGET_TRACE5 || m_DummyCharType == DCT_TARGET_TRACE6 )
			&& pChar->IsCanNoWound() )
			continue;

		if( m_bCheckWounedState && !IsTargetWoundedState( pChar ) )
			continue;

		bool bPass = false;

		auto SkipBuffIter = m_vSkipBuffList.begin();
		for ( ; SkipBuffIter != m_vSkipBuffList.end(); ++SkipBuffIter )
		{
			ioHashString& szBuffName = *SkipBuffIter;
			if ( pChar->HasBuff( szBuffName ) )
			{				
				bPass = true;
				break;
			}
		}

		if( bPass )
			continue;

		bPass = false;
		for( size_t i = 0; i < m_vTargetedChar.size(); i++ )
		{
			const ioHashString& stTargetedName = m_vTargetedChar[i];
			if( pChar->GetCharName() == stTargetedName )
			{
				bPass = true;
				break;
			}
		}

		if( bPass )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition();
		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( m_fSensingAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2 = vTargetPos - GetWorldPosition();
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fSensingRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
		}
	}

	return szTarget;
}

ioHashString ioDummyChar::GetTraceTargetNoOverlap( ioPlayStage *pStage, bool bSecond )
{
	ioHashString szTarget;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return szTarget;
	if( !pStage )
		return szTarget;

	float fSensingRangeSq = 0.0f;
	if( bSecond )
		fSensingRangeSq = m_fSensingRange2 * m_fSensingRange2;
	else
		fSensingRangeSq = m_fSensingRange * m_fSensingRange;

	if( fSensingRangeSq == 0.0f )
		return szTarget;

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetWorldPosition();

	float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;

		if( !pChar ) continue;
		if( pChar->GetTeam() == pOwner->GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( !IsEnableTargetState( pChar ) )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition();
		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( m_fSensingAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2 = vTargetPos - GetWorldPosition();
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		if( !g_DummyCharMgr.IsNoOverLapTrackingDummy( pOwner->GetCharName(), pChar->GetCharName() ) )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fSensingRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
		}
	}

	return szTarget;
}

void ioDummyChar::CheckTraceAttack()
{
	if( m_TargetTraceName.IsEmpty() )
		return;

	if( m_State != DCS_TRACE )
		return;
	if( m_TraceState != DCTS_TRACING )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );
	if( pTarget )
	{
		D3DXVECTOR3 vPrePos = GetWorldPosition();
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vPrePos;

		// 각도 체크
		bool bEnableAngle = false;
		float fBoomHalfAnigle = m_fTraceAttackAngle * FLOAT05;
		fBoomHalfAnigle = ioMath::ArrangeHead(fBoomHalfAnigle);
		float fHalfValue = cosf( DEGtoRAD(fBoomHalfAnigle) );

		if( m_fTraceAttackAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2;
			D3DXVec3Normalize( &vDiff2, &vDiff );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fHalfValue <= fCurValue )
				bEnableAngle = true;
		}
		else
		{
			bEnableAngle = true;
		}

		float fRangeSq = D3DXVec3LengthSq( & vDiff );
		if( bEnableAngle && fRangeSq < m_fTraceAttackRange * m_fTraceAttackRange )
		{
			if( m_bTraceOwnerSync )
			{
				if( pOwner->IsNeedProcess() )
				{
					// Attack 처리
					SetTracingAttack();

					SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
					kPacket << m_OwnerName;
					kPacket << m_iCreateIndex;
					kPacket << DUMMY_CHAR_TRACE_ATTACK_SYNC;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			else
			{
				SetTracingAttack();
			}
		}
	}
}

void ioDummyChar::SetTracingAttack()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_TraceState = DCTS_ATTACK;
	m_iReserveAttackCode = m_iTraceNormalAttack;

	m_vAttackFireTimeList.clear();
	m_dwAttackKeyCheckTime = 0;

	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	AttackAttribute &rkCurAttribute = iter->second.m_AttackAttribute;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackAniEndTime = dwCurTime;

	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = rkCurAttribute.m_fAttackAniRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
		m_dwAttackAniEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

		// fire 처리
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return;

		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;

		int i=0;
		AniEventConstPtrList::iterator iter;

		std::list< float >::iterator iterEndTime = vFireEndList.begin();
		for( iter=vFireStartList.begin() ; iter!=vFireStartList.end() ; ++iter, i++ )
		{
			float fFireStart = (*iter)->fEventTime;

			FireTime kFireTime;
			kFireTime.dwStart = dwStartTime + fFireStart * fAniRate;

			if( iterEndTime != vFireEndList.end() )
			{
				kFireTime.dwDuration = ( *iterEndTime - fFireStart ) * fAniRate;
				++iterEndTime;
			}

			kFireTime.iAttributeIdx  = vInfoList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = vInfoList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni   = vInfoList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration  = vInfoList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = vInfoList[i].m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;

			if( !(*iter)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
				if( vParam.size() >= 1 )
				{
					ioHashString szTypeName = vParam[0].c_str();
					kFireTime.eCollisionType = GetAniCollisionType( szTypeName );

					if( vParam.size() == 2 && kFireTime.eCollisionType == ACT_SPECIAL )
					{
						kFireTime.szSpecialCollisionBox = vParam[1].c_str();
					}
				}
			}

			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
			kFireTime.m_iExtraValue = m_iCreateIndex;

			m_vAttackFireTimeList.push_back( kFireTime );
		}

		SetReservedSliding( rkCurAttribute.m_vForceInfoList, iAniID, fAniRate, rkCurAttribute.m_dwPreDelay );
		
		// 기타 처리
		m_dwExtraAniJumpTime = 0;
		m_fExtraAniJumpPower = 0.0f;
		m_vExtraAniJumpList.clear();

		AniEventConstPtrList vExtraAniJumpList;
		m_pGroup->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );

		AniEventConstPtrList::iterator iter_event = vExtraAniJumpList.begin();
		while( iter_event != vExtraAniJumpList.end() )
		{
			ExtraAniJump kInfo;

			kInfo.m_dwTime = (*iter_event)->fEventTime * fAniRate;
			kInfo.m_dwTime += dwCurTime;

			if( !(*iter_event)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter_event)->szSubInfo.c_str(), " " );
				if( vParam.size() == 1 )
				{
					kInfo.m_fPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
				}
			}

			m_vExtraAniJumpList.push_back(kInfo);

			++iter_event;
		}

		if( !m_vExtraAniJumpList.empty() )
		{
			m_iCurExtraAniJump = 0;
			m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
			m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
		}
	}
}

bool ioDummyChar::CheckEnableDummySyncByType()
{
	if( GetType() == DCT_OWNER_CONTROL6 )
		return false;
	switch( GetType() )
	{
	case DCT_OWNER_CONTROL6:
		return false;
	default:
		return true;
	}
	return true;
}

void ioDummyChar::CheckTraceStateEnd()
{
	if( m_State != DCS_TRACE )
		return;

	switch( m_TraceState )
	{
	case DCTS_SENSING:
	case DCTS_TRACING:
	case DCTS_TARGET_JUMP:
		break;
	default:
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_DummyCharType != DCT_TARGET_TRACE5 && m_DummyCharType != DCT_TARGET_TRACE6 )
	{	
		if( m_dwTraceStateDuration > 0 && m_dwCreateTime + m_dwTraceStateDuration < dwCurTime )
		{
			if( m_bCharColOnlyAttack )
			{
				m_LastAttacker = m_OwnerName;
				SendDieState( true );
			}
			else
			{
				SetTracingAttack();
			}
		}
	}
}

ioBaseChar* ioDummyChar::CheckEnableTarget()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return NULL;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return NULL;

	float fPrevDistSq = m_fFindTargetRange*m_fFindTargetRange;
	ioBaseChar *pTargetChar = pOwner;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;		

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 	
			continue;

		if( CheckTargetWoundType( pTarget ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fPrevDistSq )	// Enemy
		{
			D3DXVECTOR3 vPos = GetMidPositionByRate();
			D3DXVECTOR3 vTargetPos;
			if( CheckDownAimState( pTarget ) )
			{
				vTargetPos = pTarget->GetMidPositionByRate( m_fDownTargetAimRate );
			}
			else
				vTargetPos = pTarget->GetMidPositionByRate( m_fTargetAimRate );
			
			D3DXVECTOR3 vDir = vTargetPos - vPos;
			D3DXVec3Normalize( &vDir, &vDir );

			//Min, Max 앵글 값으로 찾기
			if( 0 <= m_fFindTargetMinAngle && 0 < m_fFindTargetMaxAngle )
			{
				D3DXVECTOR3 vOwnerDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vOwnerDir, &vOwnerDir );

				D3DXVECTOR3 vTargetDir = vTargetPos - pOwner->GetWorldPosition();
				D3DXVec3Normalize( &vTargetDir, &vTargetDir );

				float fOwnerAngle = RADtoDEG( atan2( vOwnerDir.x, vOwnerDir.z ) );
				float fTargetAngle  = RADtoDEG( atan2( vTargetDir.x, vTargetDir.z ) );
				
				float fValue = fOwnerAngle - fTargetAngle;
				if ( fValue < 0 )
					 fValue = 360.0f + fValue;
				
				if( m_fFindTargetMinAngle <= fValue && fValue <= m_fFindTargetMaxAngle )
				{
					pTargetChar = pTarget;
					fPrevDistSq = fDistSq;
					m_vAttackDir = vDir;
				}
			}
			else
			{
				D3DXVECTOR3 vSearchDir;
				vSearchDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 );
				D3DXVec3Normalize( &vSearchDir, &vSearchDir );

				float fHalfCosine = cosf( DEGtoRAD( m_fFindTargetAngle ) * FLOAT05 );

				if( D3DXVec3Dot( &vSearchDir, &vDir ) >= fHalfCosine )
				{
					pTargetChar = pTarget;
					fPrevDistSq = fDistSq;
					m_vAttackDir = vDir;
				}
			}
		}
	}

	if( pTargetChar == pOwner )
		return NULL;

	if( CheckDownAimState( pTargetChar ) )
		m_AttachAttackType = AT_DOWN;
	else if( pTargetChar->IsFloating() && pTargetChar->IsFloatingState() )
		m_AttachAttackType = AT_AIR;
	else
		m_AttachAttackType = AT_NORMAL;

	return pTargetChar;
}

bool ioDummyChar::CheckDownAimState( ioBaseChar *pTarget ) const
{
	if( !pTarget ) return false;

	if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT )
		return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() )
		return true;

	return false;
}

bool ioDummyChar::CheckTargetWoundType( ioBaseChar *pTarget ) const
{
	if( !pTarget )	return false;

	// 쓰러짐 관련 체크
	if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
	{
		if( pTarget->IsApplyDownState() && !pTarget->IsApplyDownState(false) )
		{
			return true;
		}
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
	{
		if( pTarget->IsApplyDownState(false) )
		{
			return true;
		}
	}
	else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
	{
		if( m_bDisableColGrap && pTarget->IsApplyDownState() ||
			( pTarget->GetState() == CS_GRAPPLING_WOUNDED ||
			pTarget->GetState() == CS_GRAPPLING_PUSHED ||
			pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
			pTarget->GetState() == CS_GRAPPLING_WOUNDED_SWING ||
			pTarget->GetState() == CS_GRAPPLING_WOUNDED_ITEM ) )
		{
			return true;
		}
		else if( pTarget->IsApplyDownState() )
		{
			return true;
		}
	}
	else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
	{
		if( !pTarget->IsFloating() )
		{
			return true;
		}
	}		
	else if( m_TargetWoundType == TWT_DOWN_WOUNDED )  //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
	{		
		if( pTarget->IsApplyDownState() && !pTarget->IsFloating() )
		{
			return true;
		}
	}

	return false;
}
// Target Trace 3
void ioDummyChar::FindTarget( ioBaseChar *pChar )
{
	if( !pChar )	return;
	
	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )	return;

	if( !pChar->IsNeedProcess() )
		return;

	m_vTargetInfoList.clear();
	m_szPreTargetName = m_szCurTargetName;
	m_szCurTargetName.Clear();

	ioBaseChar *pTargetChar = pChar;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )
			continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 	
			continue;

		if( CheckTargetWoundType( pTarget ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		TargetDistInfo	kInfo;
		kInfo.m_TargetName = pTarget->GetCharName();
		kInfo.m_fLenth = fDistSq;

		float fEnableDistSq = 0.0f;
		if( m_bFirst )
		{
			fEnableDistSq = m_fFirstTraceRange*m_fFirstTraceRange;
			m_bFirst = false;
		}
		else
		{
			fEnableDistSq = m_fTraceRange*m_fTraceRange;
		}

		if( fDistSq > fEnableDistSq )	continue;

		m_vTargetInfoList.push_back(kInfo);
	}

	if( !m_vTargetInfoList.empty() )
	{
		std::sort( m_vTargetInfoList.begin(), m_vTargetInfoList.end(), DistSort() );

		int iInfoList = m_vTargetInfoList.size();
		for( int i=0; i<iInfoList; ++i )
		{
			bool bArleadyAttack = false;
			int iCnt = m_vTargetList.size();
			for( int j=0; j<iCnt; ++j )
			{
				if( m_vTargetInfoList[i].m_TargetName == m_vTargetList[j] )
				{
					bArleadyAttack = true;
				}
			}

			if( !bArleadyAttack )
			{
				pTargetChar = pStage->GetBaseChar( m_vTargetInfoList[i].m_TargetName );
				break;
			}
		}

		if( pTargetChar == pChar )
		{
			pTargetChar = pStage->GetBaseChar( m_szPreTargetName );
			if( pTargetChar )
			{
				m_vTargetList.clear();
				m_szCurTargetName.Clear();
				m_vTargetList.push_back( pTargetChar->GetCharName() );
			}
			else
			{
				m_vTargetList.clear();
				m_szCurTargetName.Clear();
				SetDelayState( true );
			}
		}
		else if( pTargetChar )
		{
			m_szCurTargetName = pTargetChar->GetCharName();			
			m_vTargetList.push_back( pTargetChar->GetCharName() );
		}
		else
		{
			m_vTargetList.clear();
			m_szCurTargetName.Clear();
			SetDelayState( true );
		}
	}
}

void ioDummyChar::SetTraceTargetMoveState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetDelayState( true );
		return;
	}

	if( !pOwner->IsNeedProcess() )
		return;

	SetMoveState();

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szCurTargetName );
	if( m_vTargetInfoList.size() <= 1 && !m_bEnableAttack )
	{
		D3DXVECTOR3 vPos = GetWorldPosition();
		D3DXVECTOR3 vMoveDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		DWORD dwSeed = pOwner->GetRandomSeed();
		IORandom random;
		random.SetRandomSeed( dwSeed + m_dwSeed );
		m_dwSeed++;
		float fRange = (float)random.Random(360);

		D3DXVECTOR3 vYAxis = ioMath::UNIT_Y;
		D3DXQUATERNION qtRandomRot;
		D3DXQuaternionRotationAxis( &qtRandomRot, &vYAxis, fRange );

		vMoveDir = qtRandomRot * vMoveDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		m_vTracingMoveDir = vMoveDir;
		m_bEnableAttack = false;

		m_TraceState = DCTS_DELAY;
	}
	else if( pTarget )
	{
		D3DXVECTOR3 vPos = GetWorldPosition();
		//D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vTargetPos.y += m_fHeightOffset;

		D3DXVECTOR3 vMoveDir = vTargetPos - vPos;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		m_vTracingMoveDir = vMoveDir;
		m_bEnableAttack = true;

		m_TraceState = DCTS_TRACING;
	}
	else
	{
		SetDelayState( true );
		m_bEnableAttack = false;

		m_TraceState = DCTS_NONE;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_TRACE_TARGET;
		kPacket << m_vTracingMoveDir;
		kPacket << GetWorldPosition();
		kPacket << (int)m_TraceState;
		kPacket << m_szCurTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChar::ProcessMoveTraceTarget( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetDelayState( true );
		return;
	}

	m_fGravityAmt = 0.0f;

	switch( m_TraceState )
	{
	case DCTS_NONE:
		SetDelayState( true );
		return;
	case DCTS_DELAY:
		{
			float fMoveValue = fTimePerSec * m_fTraceMoveSpeed;
			m_fCurMoveRange += fMoveValue;

			if( m_fCurMoveRange >= m_fDelayMoveRange && pOwner->IsNeedProcess() )
			{
				m_fCurMoveRange = 0.0f;
				m_bEnableAttack = true;
				m_vTargetList.clear();
				SetDelayState( true );				
			}
		}
		break;
	case DCTS_TRACING:
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_szCurTargetName );
			if( !pTarget )
			{
				SetDelayState( true );
				return;
			}

			if( !pTarget->IsVisibleChar() || pTarget->HasHideBuff() )
			{
				SetDelayState( true );
				return;
			}

			D3DXVECTOR3 vPos = GetWorldPosition();
			D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
			vTargetPos.y += m_fHeightOffset;

			D3DXVECTOR3 vMoveDir = vTargetPos - vPos;
			D3DXVec3Normalize( &vMoveDir, &vMoveDir );
			m_vTracingMoveDir = vMoveDir;
		}
		break;
	}

	D3DXVECTOR3 vPos = GetWorldPosition();

	float fMoveValue = fTimePerSec * m_fTraceMoveSpeed;
	vPos += m_vTracingMoveDir * fMoveValue;
	SetWorldPosition( vPos );

	D3DXVECTOR3 vDir = m_vTracingMoveDir;
	vDir.y = 0.0f;
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetWorldOrientation( qtRot );
}

void ioDummyChar::CheckEnableAttack()
{
	if( !m_bEnableAttack )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szCurTargetName );
	if( !pTarget )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	D3DXVECTOR3 vOwnerPos = GetWorldPosition();
	D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();

	D3DXVECTOR3 vDiff = vTargetPos - vOwnerPos;
	float fDistSq = D3DXVec3LengthSq( &vDiff );
	float fEnableDistSq = m_fTraceAttackRange*m_fTraceAttackRange;
	if( fDistSq <= fEnableDistSq )
	{
		SetTraceTargetAttack();
	}
}

void ioDummyChar::SetTraceTargetAttack()
{
	m_vAttackFireTimeList.clear();

	m_iReserveAttackCode = m_iTraceNormalAttack;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;

	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	AttackAttribute &rkCurAttribute = iter->second.m_AttackAttribute;
	int iCnt = rkCurAttribute.m_vWeaponInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		FireTime kFireTime;
		kFireTime.iAttributeIdx  = rkCurAttribute.m_vWeaponInfoList[i].m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkCurAttribute.m_vWeaponInfoList[i].m_iResistanceIdx;
		kFireTime.szWoundedAni   = rkCurAttribute.m_vWeaponInfoList[i].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkCurAttribute.m_vWeaponInfoList[i].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkCurAttribute.m_vWeaponInfoList[i].m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;

		kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
		kFireTime.m_iExtraValue = m_iCreateIndex;

		m_vAttackFireTimeList.push_back( kFireTime );
	}

	m_State = DCS_TRACE_TARGET_ATTACK;

	m_dwNextFireTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_TRACE_TARGET_ATTACK;
		kPacket << (int)m_State;
		kPacket << (int)m_dwBaseWeaponIndex;
		kPacket << m_szCurTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChar::ProcessTraceTargetAttack( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szCurTargetName );
	if( !pTarget )	return;

	if( m_dwNextFireTime == 0 )
	{
		D3DXVECTOR3 vCurPos = GetMidPositionByRate();
		D3DXVECTOR3 vTargetPos;
		if( CheckDownAimState( pTarget ) )
		{
			vTargetPos = pTarget->GetMidPositionByRate( m_fDownTargetAimRate );
		}
		else
		{
			vTargetPos = pTarget->GetMidPositionByRate( m_fTargetAimRate );
		}
		D3DXVECTOR3 vAttackDir = vTargetPos - vCurPos;
		D3DXVec3Normalize( &vAttackDir, &vAttackDir );

		FireTimeList::iterator iter = m_vAttackFireTimeList.begin();
		while( iter != m_vAttackFireTimeList.end() )
		{
			const FireTime &rkFire = *iter;

			DWORD dwFireTime = rkFire.dwStart;

			if( rkFire.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
			{
				pOwner->ExtendFireExplicit( rkFire, vCurPos, vAttackDir, "" );
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}

		m_iCurFireCount++;

		// 동기화를 위해 딜레이를 준다.
		m_dwNextFireTime = FRAMEGETTIME() + 100;
	}
	else if( m_dwNextFireTime <= FRAMEGETTIME() )
	{
		if( pOwner->IsNeedProcess() )
		{
			if( m_iMaxFireCount > 0 && m_iCurFireCount >= m_iMaxFireCount )
			{
				SendDieState( true );
				return;
			}

			m_bEnableAttack = false;

			int iTargetCnt = m_vTargetInfoList.size();
			if( iTargetCnt > 1 && !m_bEnableAttack )
				m_bEnableAttack = true;
		}

		SetDelayState( false );
	}
}

bool ioDummyChar::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	return true;
}

void ioDummyChar::CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense )
{
	switch( m_DummyCharType )
	{
	case DCT_TARGET_TRACE5:
		if( m_iColDieType == CDT_WOUND && !bDefense )
		{
			SendDieState( true );
		}
		break;
	}
}

void ioDummyChar::CheckMapCollision( ioPlayStage *pStage )
{
	if( !m_bDestroyMapCol )
		return;

	SendDieState( true );
}

void ioDummyChar::SetVisibleDummy( bool bVisible )
{
	m_pGroup->SetVisible( bVisible );
}

void ioDummyChar::SetLoopAttackState()
{
	m_vAttackFireTimeList.clear();
	m_dwAttackKeyCheckTime = 0;

	m_State = DCS_LOOP_ATTACK;

	CheckNextLoopAttack();

	if( m_iReserveAttackCode == 0 )
	{
		SetDieState( true );
	}
}

void ioDummyChar::CheckNextLoopAttack()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	AttackAttribute &rkCurAttribute = iter->second.m_AttackAttribute;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackAniEndTime = dwCurTime;

	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = rkCurAttribute.m_fAttackAniRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
		m_dwAttackAniEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

		// fire 처리
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return;

		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;

		int i=0;
		AniEventConstPtrList::iterator iter;

		std::list< float >::iterator iterEndTime = vFireEndList.begin();
		for( iter=vFireStartList.begin() ; iter!=vFireStartList.end() ; ++iter, i++ )
		{
			float fFireStart = (*iter)->fEventTime;

			FireTime kFireTime;
			kFireTime.dwStart = dwStartTime + fFireStart * fAniRate;

			if( iterEndTime != vFireEndList.end() )
			{
				kFireTime.dwDuration = ( *iterEndTime - fFireStart ) * fAniRate;
				++iterEndTime;
			}

			kFireTime.iAttributeIdx  = vInfoList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = vInfoList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni   = vInfoList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration  = vInfoList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = vInfoList[i].m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;

			if( !(*iter)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
				if( vParam.size() >= 1 )
				{
					ioHashString szTypeName = vParam[0].c_str();
					kFireTime.eCollisionType = GetAniCollisionType( szTypeName );

					if( vParam.size() == 2 && kFireTime.eCollisionType == ACT_SPECIAL )
					{
						kFireTime.szSpecialCollisionBox = vParam[1].c_str();
					}
				}
			}

			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
			kFireTime.m_iExtraValue = m_iCreateIndex;

			m_vAttackFireTimeList.push_back( kFireTime );
		}

		SetReservedSliding( rkCurAttribute.m_vForceInfoList, iAniID, fAniRate, rkCurAttribute.m_dwPreDelay );
		// 기타 처리

		m_dwExtraAniJumpTime = 0;
		m_fExtraAniJumpPower = 0.0f;
		m_vExtraAniJumpList.clear();

		AniEventConstPtrList vExtraAniJumpList;
		m_pGroup->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );

		AniEventConstPtrList::iterator iter_event = vExtraAniJumpList.begin();
		while( iter_event != vExtraAniJumpList.end() )
		{
			ExtraAniJump kInfo;

			kInfo.m_dwTime = (*iter_event)->fEventTime * fAniRate;
			kInfo.m_dwTime += dwCurTime;

			if( !(*iter_event)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter_event)->szSubInfo.c_str(), " " );
				if( vParam.size() == 1 )
				{
					kInfo.m_fPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
				}
			}

			m_vExtraAniJumpList.push_back(kInfo);

			++iter_event;
		}

		if( !m_vExtraAniJumpList.empty() )
		{
			m_iCurExtraAniJump = 0;
			m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
			m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
		}
	}
}

void ioDummyChar::ProcessLoopAttackState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwAttackAniEndTime > 0 && m_dwAttackAniEndTime < dwCurTime )
	{
		CheckNextLoopAttack();

		if( m_iReserveAttackCode == 0 )
		{
			SetDieState( true );
		}

		return;
	}

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	DWORD dwLastFireTime = dwPreTime;
	FireTimeList::iterator iter = m_vAttackFireTimeList.begin();
	while( iter != m_vAttackFireTimeList.end() )
	{
		const FireTime &rkFire = *iter;

		DWORD dwFireTime = rkFire.dwStart;
		if( COMPARE( dwFireTime, dwPreTime, dwCurTime ) )
		{
			if( rkFire.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
			{
				pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioDummyChar::SetPiercingPreDelayStartState()
{
	if( m_bAreadyPierceStart )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{	
		DWORD dwCurTime = FRAMEGETTIME();
		D3DXVECTOR3 vCurPos = GetWorldPosition();
		vCurPos.y -= 50.0f;

		m_PierceWeapon.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
		ioWeapon* pWeapon = pOwner->ExtendFireExplicit( m_PierceWeapon, vCurPos, D3DXVECTOR3(0.0f, FLOAT1, 0.0f), "" );
		if( pWeapon )
		{
			pWeapon->SetWeaponByDummyChar( m_iCreateIndex );
			ioPullWeapon *pPull = ToPullWeapon( pWeapon );
			if(	pPull )
			{
				pPull->SetWeaponPreInfo( dwCurTime, m_dwPreDuration );
				pPull->SetCollisionDir( D3DXVECTOR3(0.0f, FLOAT1, 0.0f) );
				pPull->SetCallStartPosition( vCurPos );				
			}

			m_dwBaseWeaponIndex = pWeapon->GetWeaponIndex();
			m_bAreadyPierceStart = true;
		}
	}
}

void ioDummyChar::SetPiercingStartState()
{
	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			if( m_bEffectVisible )
			{
				D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );	
				ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_WoodGrowthUpEffect, GetWorldPosition(), vScale );

				if( pMapEffect )						
					m_dwWoodGrowthUpEffect = pMapEffect->GetUniqueID();								

			}
			SetPiercingState();			
		}
	}
}

void ioDummyChar::SetPiercingState()
{
	m_State = DCS_GROUND_PIERCING_ATTACK;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		if( !m_PierceStartSound.IsEmpty() )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			m_PierceStartSoundID = g_SoundMgr.PlaySound( m_PierceStartSound, pGrp->GetParentSceneNode() );			
		}
	}
}

void ioDummyChar::SetExtendDieState( ioPlayStage* pStage, ioBaseChar* pOwner )
{
	if( !pStage )
		return;
	if( !pOwner )
		return;

	switch( m_DummyCharType )	
	{
	case DCT_GROUND_PIERCE:
		{
			ioHashStringVec vPierceTargetList;
			ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwBaseWeaponIndex );
			if( pWeapon )
			{
				ioPullWeapon* pPullWeapon = ToPullWeapon( pWeapon );
				if( pPullWeapon )
				{
					vPierceTargetList.clear();
					vPierceTargetList = pPullWeapon->GetTargetList();

					if( !vPierceTargetList.empty() )
					{
						ioHashStringVec::iterator iter = vPierceTargetList.begin();
						for( ; iter != vPierceTargetList.end(); ++iter )
						{
							ioBaseChar* pTarget = pStage->GetBaseChar( (*iter) );
							if( pTarget )
							{
								if( pTarget->GetState() == CS_SKIP_STUN )
								{
									pTarget->RemoveBuff( pPullWeapon->GetRemoveSkipStunBuffName() );
								}
							}
						}
					}

					pPullWeapon->SetTargetBuff( pStage );
					pPullWeapon->SetWeaponDead();			

					if( m_dwWoodGrowthUpEffect != -1 )
						pStage->DestroyMapEffect( m_dwWoodGrowthUpEffect );

					if( !m_PierceStartSound.IsEmpty() )
					{			
						g_SoundMgr.StopSound( m_PierceStartSound, m_PierceStartSoundID );	
					}

				}
			}

			pStage->RemoveSummonDummyInfo( m_iCreateIndex );
		}
		break;

	default:
		{			
			if( m_PreEffectID != -1 )
			{
				pStage->DestroyMapEffect( m_PreEffectID );
				m_PreEffectID = -1;
			}
		}
		break;
	}
}

void ioDummyChar::ProcessPiercingState( float fTimePerSec )
{	
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{	
		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwBaseWeaponIndex );
		if( pWeapon )
		{	
			ioPullWeapon* pPullWeapon = ToPullWeapon( pWeapon );
			if( pPullWeapon )
			{
				if( pPullWeapon->isMoveComplete() )
				{
					m_bPiercingProtected = false;
					m_bGrowthUp = false;
				}
			}
		}
	}
}

void ioDummyChar::CheckDummyTower()
{
	if( !m_pModeParent )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	D3DXVECTOR3 vCreatePos = GetWorldPosition();

	// target check
	ioBaseChar *pTarget = NULL;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	float fCurGap = -FLOAT1;
	float fCheckRange = m_fDummyTowerCheckRange * m_fDummyTowerCheckRange;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;

		if( !pChar ) continue;
		if( pChar->GetTeam() == pOwner->GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( !IsEnableTargetState( pChar ) )
			continue;

		D3DXVECTOR3 vDiff = vCreatePos - pChar->GetWorldPosition();
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 거리 체크
		if( fDiffRangeSq < fCheckRange )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				pTarget = pChar;
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				pTarget = pChar;
			}
		}
	}

	if( !pTarget )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	// check time
	if( m_dwDummyTowerCheckTime == 0 || m_dwDummyTowerCheckTime < dwCurTime )
		m_dwDummyTowerCheckTime = dwCurTime + m_dwDummyTowerCheckGapTime;
	else
		return;

	// create dummy
	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	D3DXVECTOR3 vCheckDir( 0.0f, 0.0f, -FLOAT1 );
	
	D3DXVECTOR3 vDiff = vTargetPos - vCreatePos;
	vDiff.y = 0.0f;
	D3DXVec3Normalize( &vDiff, &vDiff );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	float fStartAngle = 0.0f;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyTowerDummyName, iCurIndex,
												   GetOwnerName(),
												   vCreatePos,
												   fStartAngle,
												   0, true );

	bool bSendRot = false;
	D3DXQUATERNION qtTargetRot;
	if( pDummy )
	{
		if( vDiff != vCheckDir )
		{
			bSendRot = true;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vDiff );
			pDummy->SetWorldOrientationAndDir( qtTargetRot );
		}	
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_DUMMY_TOWER;
		kPacket << m_DummyTowerDummyName;
		kPacket << iCurIndex;
		kPacket << fStartAngle;
		kPacket << vCreatePos;
		kPacket << bSendRot;
		if( bSendRot )
			kPacket << qtTargetRot;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChar::CheckAttackTower()
{
	DummyAttackMap::iterator iterTest = m_DummyAttackMap.find( m_iTraceNormalAttack + m_nAttackTowerAttackCnt );
	if( iterTest == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	AttackAttribute &rkCurAttribute = iterTest->second.m_AttackAttribute;

	if( !m_pModeParent )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	D3DXVECTOR3 vCreatePos = GetWorldPosition();

	// target check
	ioBaseChar *pTarget = NULL;

	if( dwCurTime < (m_dwAttackTowerTime+rkCurAttribute.m_dwPreDelay) )
		return;

	FireTime kFireTime;
	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	for( int nLoop = 0; nLoop < (int)rkCurAttribute.m_vWeaponInfoList.size();  nLoop++ )
	{
		D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vCurDir, &vCurDir );

		D3DXVECTOR3 vAddPos = vCurDir*m_fWeaponOffset_Front_Back;
		vAddPos = vAddPos + D3DXVECTOR3(0.f, FLOAT1, 0.f)*m_fWeaponOffset_Up_Down;

		D3DXVECTOR3 vCurPos = GetMidPositionByRate();

		kFireTime.dwStart = dwCurTime;
		kFireTime.dwDuration = 1;
		kFireTime.iAttributeIdx = rkCurAttribute.m_vWeaponInfoList[nLoop].m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkCurAttribute.m_vWeaponInfoList[nLoop].m_iResistanceIdx;
		kFireTime.szWoundedAni = "";
		kFireTime.dwWoundedAniDuration = 0;
		kFireTime.bLoopWoundedAni = "";
		kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + nLoop;
		kFireTime.eCollisionType = ACT_SPECIAL;
		kFireTime.eFireTimeType = FTT_EXTEND_ATTACK;

		vCurPos = vCurPos +	vAddPos;
	
		pOwner->ExtendFireExplicit( kFireTime,
			vCurPos,
			vCurDir,
			"" );
	}

	m_nAttackTowerAttackCnt++;
	m_dwAttackTowerTime = dwCurTime;
}

void ioDummyChar::CheckCurseDoll()
{
	ioBaseChar *pOwner = GetOwner();
	ioBaseChar *pTarget = NULL;

	if( pOwner )
		pTarget = pOwner->GetBaseChar( m_stTargetName );

	if( pOwner && pTarget )
	{
		DWORD dwCurTime = FRAMEGETTIME();

		m_fGravityAmt = 0.0f;

		D3DXVECTOR3 vPos = pTarget->GetWorldPosition();
		D3DXVECTOR3 vScale = pTarget->GetWorldScale();
		D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vSide;
		D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );

		vPos += vDir * m_fFrontOffset;
		vPos.y += m_fHeightOffset;
		vPos += vSide * m_fSideOffset;
		SetWorldPosition( vPos );

		//지정된 회전 앵글을 유지 할떄
		if( m_bKeepStartAngle && m_fStartAngle != 0.0f )
		{
			float fYawD = ioMath::ArrangeHead( m_fStartAngle );
			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
			vDir = qtRotate * vDir;					
		}

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, vDir );
		SetWorldOrientation( qtRot );

		float fHeightGap = 0;
		if( pTarget->CheckGhostState() )
			fHeightGap = pTarget->GetWorldPosition().y - pTarget->GetPreHeightForGhostState();
		else
			fHeightGap = pTarget->GetWorldPosition().y - pTarget->GetBottomHeight();

		if( m_bCurseDollLandState == false )
		{
			if( fHeightGap >= m_fCurseDollCheckLandHeight )
				m_bCurseDollLandState = false;
			else 
				m_bCurseDollLandState = true;

			if( m_bCurseDollLandState )
				m_dwCurseDollCheckDamageTime = dwCurTime;
		}
		else
		{
			if( fHeightGap >= m_fCurseDollCheckLandHeight )
				m_bCurseDollLandState = false;
			else 
				m_bCurseDollLandState = true;
		}

		if( m_bCurseDollLandState && ((m_dwCurseDollCheckDamageTime + m_dwCurseDollDamageTime) < dwCurTime) )
		{
			m_dwCurseDollCheckDamageTime = dwCurTime;

			m_iReserveAttackCode= m_AttackInfoList[m_AttachAttackType].m_iAutoFireCode;
			if( !m_bCurseDollAutoAttackDisable )
				SetAttackState();
		}
	}
}

void ioDummyChar::SetCurseDollTargetName( const ioHashString& stName )
{
	m_stTargetName = stName;
}

void ioDummyChar::SetWorldOrientationAndDir( const D3DXQUATERNION &qtRot )
{
	SetWorldOrientation( qtRot );

	m_vTracingMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_vAttackDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
}

void ioDummyChar::ProcessRush( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	float fMoveValue = fTimePerSec * m_fMoveSpeed;
	vPos += m_vAttackDir * fMoveValue;
	SetWorldPosition( vPos );

	int iCnt = m_vAttachWeaponIndexList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_vAttachWeaponIndexList[i] );
		if( !pWeapon )
			SetDieState( true );
	}
}

void ioDummyChar::SetAttachWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_AttachWeaponInfoList.empty() )
		return;

	if( !m_vAttachWeaponIndexList.empty() )
		return;

	m_vAttachWeaponIndexList.clear();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	int iCnt = m_AttachWeaponInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kInfo = m_AttachWeaponInfoList[i];
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();

		kFireTime.iAttributeIdx  = kInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = kInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni   = kInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration  = kInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = kInfo.m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;
		kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
		kFireTime.m_iExtraValue = m_iCreateIndex;

		if( kFireTime.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
		{
			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
			if( pWeapon )
			{
				ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
				if( pZone )
				{
					pZone->SetCollisionDir( vCurDir );
					pZone->SetZoneValue( true, false );
				}


				ioAdhesiveMissileWeapon3 *pAdhesiveWeapon3 = ToAdhesiveMissileWeapon3( pWeapon );
				if( pAdhesiveWeapon3 )
				{
					pAdhesiveWeapon3->CheckOwnerAttach( true );
				}

				ioAttachAeraWeapon *pAttachAreaWeapon = ToAttachAeraWeapon( pWeapon );
				if ( pAttachAreaWeapon )
				{
					pAttachAreaWeapon->CheckOwnerAttach( true );
				}
			}
		}

		m_vAttachWeaponIndexList.push_back( kFireTime.dwWeaponIdx );
	}
}

bool ioDummyChar::AddWoundedTargetName( const ioHashString& stCharName )
{
	for( size_t i = 0; i < m_vTargetedChar.size(); i++ )
	{
		const ioHashString& stTargetName = m_vTargetedChar[i];
		if( stTargetName == stCharName )
			return false;
	}

	m_vTargetedChar.push_back( stCharName );
	return true;
}

void ioDummyChar::CheckCollisionWeapon( ioBaseChar *pChar )
{
}

void ioDummyChar::FindAttackTheTarget( const ioHashString &szTargetName  )
{
	auto pOwner = GetOwner();
	ioBaseChar *pTarget = pOwner->GetBaseChar( szTargetName );
	
	if ( pTarget )
	{
		D3DXVECTOR3 vPos = GetWorldPosition();
		vPos.y = 0;

		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vTargetPos.y = 0;

		D3DXVECTOR3 vMoveDir = vTargetPos - vPos;
		float fRange = D3DXVec3Length( &vMoveDir );
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );		
	
		D3DXQUATERNION qtRot;
		qtRot = Help::ConvertDirToQuaternion( vMoveDir );
		SetWorldOrientation( qtRot );
		
		fRange = max( m_fMinRange, fRange );
		fRange = min( JUMP_RANGE * m_fJumpAttackRate, fRange );

		m_iReserveAttackCode = m_iTraceNormalAttack;

		float fRate = fRange / (JUMP_RANGE * m_fJumpAttackRate);
		m_fJumpSpeed = (JUMP_SPEED * m_fJumpAttackRate) * ( fRate - ( fRate * m_fJumpAttackAccurate ) );
		m_fCurDummyJumpPower = JUMP_POWER * m_fJumpAttackRate;
	}
}

void ioDummyChar::SetJumpAttackState()
{
	m_vAttackFireTimeList.clear();
	m_dwAttackKeyCheckTime = 0;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	AttackAttribute &rkCurAttribute = iter->second.m_AttackAttribute;

	DWORD dwCurTime = FRAMEGETTIME();
	m_TraceState = DCTS_ATTACK;
	m_dwAttackAniEndTime = dwCurTime;

	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT100 );
		m_pGroup->ClearAllLoopAni( FLOAT100 );
		
		float fAniRate = rkCurAttribute.m_fAttackAniRate * m_fJumpAttackRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
		m_dwAttackAniEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;
		
		// fire 처리
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return;

		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;

		int i=0;
		AniEventConstPtrList::iterator iter;

		std::list< float >::iterator iterEndTime = vFireEndList.begin();
		for( iter=vFireStartList.begin() ; iter!=vFireStartList.end() ; ++iter, i++ )
		{
			float fFireStart = (*iter)->fEventTime;

			FireTime kFireTime;
			kFireTime.dwStart = dwStartTime + fFireStart * fAniRate;

			if( iterEndTime != vFireEndList.end() )
			{
				kFireTime.dwDuration = ( *iterEndTime - fFireStart ) * fAniRate;
				++iterEndTime;
			}

			kFireTime.iAttributeIdx  = vInfoList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = vInfoList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni   = vInfoList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration  = vInfoList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = vInfoList[i].m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;

			if( !(*iter)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
				if( vParam.size() >= 1 )
				{
					ioHashString szTypeName = vParam[0].c_str();
					kFireTime.eCollisionType = GetAniCollisionType( szTypeName );

					if( vParam.size() == 2 && kFireTime.eCollisionType == ACT_SPECIAL )
					{
						kFireTime.szSpecialCollisionBox = vParam[1].c_str();
					}
				}
			}

			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
			kFireTime.m_iExtraValue = m_iCreateIndex;

			m_vAttackFireTimeList.push_back( kFireTime );
		}
		AddReservedSliding( 0, m_fJumpSpeed, FLOAT1, 0.0f, true );
		
		// 기타 처리
		m_dwExtraAniJumpTime = 0;
		m_fExtraAniJumpPower = 0.0f;
		m_vExtraAniJumpList.clear();

		ExtraAniJump kInfo;
		kInfo.m_dwTime = dwCurTime;
		kInfo.m_fPower = m_fCurDummyJumpPower;
		
		m_vExtraAniJumpList.push_back(kInfo);

		if( !m_vExtraAniJumpList.empty() )
		{
			m_iCurExtraAniJump = 0;
			m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
			m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
		}
	}

	SendAttackInfoToSkill();
}

bool ioDummyChar::IsTargetWoundedState( ioBaseChar* pTarget )
{
	if( !pTarget )
		return false;

	if( m_bCheckWounedState )
	{
		CharState eState = pTarget->GetState();
 		if ( eState == CS_WOUNDED )
 			return false;
		if ( eState == CS_BLOW_WOUNDED )
			return false;

		return IsEnableTargetState( pTarget );
	}

	return true;
}

void ioDummyChar::ClearMountEntity()
{
	if( !m_bMountEnable )
		return;

	if( !m_vMountEntityList.empty() )
	{
		m_vMountEntityList.erase( m_vMountEntityList.begin(), m_vMountEntityList.end() );
	}
}
void ioDummyChar::TranslateMountEntity( const D3DXVECTOR3& vMoveAmt )
{
	if( !m_bMountEnable )
		return;

	int iCount = m_vMountEntityList.size();
	for( int i = 0; i < iCount; ++i )
	{
		m_vMountEntityList[i]->TranslateByWorldEntity( vMoveAmt );
	}

	D3DXVECTOR3 vNewPos;
	MountedWeaponList::iterator iter = m_vMountWeaponList.begin();
	for( ; iter!=m_vMountWeaponList.end() ; ++iter )
	{
		ioWeapon *pWeapon = *iter;
		if( pWeapon )		
		{
			vNewPos = pWeapon->GetPosition();
			pWeapon->SetPosition( vNewPos + vMoveAmt );
		}
	}
}

void ioDummyChar::SetWorldPositionMountEntity( float fHeightAmt )
{
	if( !m_bMountEnable )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	ioOrientBox kBox = GetWorldCollisionBox();
	float fDummyHeight = vPos.y + kBox.GetExtents(1);

	if( m_pModeParent && m_pModeParent->GetCreator() )
	{
		int iCount = m_vMountEntityList.size();
		for( int i = 0; i < iCount; ++i )
		{
			ioOrientBox kBox = m_vMountEntityList[i]->GetWorldCollisionBox();
			D3DXVECTOR3 vCurPos = m_vMountEntityList[i]->GetWorldPosition();
			vCurPos.y += fHeightAmt;
			
			if( vCurPos.y <= fDummyHeight )
				continue;

			m_vMountEntityList[i]->SetWorldPosition( vCurPos );
		}
	}
		
	MountedWeaponList::iterator iter = m_vMountWeaponList.begin();
	for( ; iter!=m_vMountWeaponList.end() ; ++iter )
	{
		ioWeapon *pWeapon = *iter;

		D3DXVECTOR3 vCurPos = pWeapon->GetPosition();
		vCurPos.y += fHeightAmt;

		if( vCurPos.y <= fDummyHeight )
			continue;

		pWeapon->SetPosition( vCurPos );
	}
}

void ioDummyChar::UpdateMountedEntityList()
{
	if( !m_bMountEnable )
		return;

	if( m_pModeParent && m_pModeParent->GetCreator() )
	{
		if( m_bOwnerMountEnable )
			m_pModeParent->GetCreator()->UpdateMountedEntityListByDummyChar( this, m_vMountEntityList, NULL );
		else
			m_pModeParent->GetCreator()->UpdateMountedEntityListByDummyChar( this, m_vMountEntityList, GetOwner() );
	}
}

void ioDummyChar::UpdateMountedWeaponList()
{
	if( !m_bMountEnable )
		return;

	if( m_pModeParent && m_pModeParent->GetCreator() )
	{
		m_pModeParent->GetCreator()->UpdateMountedWeaponListByBox( this, m_vMountWeaponList );

		//붙은 무기 중에서
		for( int i=0; i < (int)m_vMountWeaponList.size(); i++ )
		{
			//지뢰무기면서, 다른 팀일 경우 터트림!
			ioWeapon *pWeapon = m_vMountWeaponList[i];
			ioMineWeapon *pMine = ToMineWeapon( m_vMountWeaponList[i] );
			ioMineWeapon2 *pMine2 = ToMineWeapon2( m_vMountWeaponList[i] );
			ioMineWeapon3 *pMine3 = ToMineWeapon3( m_vMountWeaponList[i] );
			ioMineWeapon6 *pMine6 = ToMineWeapon6( m_vMountWeaponList[i] );
		
			if ( GetOwner() && pWeapon->GetTeam() != GetOwner()->GetTeam() )
			{
				if ( pMine && pMine->GetOwner() )
				{
					pMine->ExplosionMine( m_pModeParent->GetCreator(), true );
				}
				else if ( pMine2 && pMine2->GetOwner() )
				{
					pMine2->ExplosionMine( m_pModeParent->GetCreator(), true );
				}
				else if ( pMine3 && pMine3->GetOwner() )
				{
					pMine3->ExplosionMine( m_pModeParent->GetCreator(), true );
				}
				else if ( pMine3 && pMine3->GetOwner() )
				{
					pMine3->ExplosionMine( m_pModeParent->GetCreator(), true );
				}
				else if ( pMine6 && pMine6->GetOwner() )
				{
					pMine6->ExplosionMine( m_pModeParent->GetCreator(), true );
				}
			}
		}
	}
}

void ioDummyChar::DummyDieClearMountEntity()
{
	if( !m_bMountEnable )
		return;

	UpdateMountedEntityList();
	UpdateMountedWeaponList();
		
	int iEntityCount = m_vMountEntityList.size();
	for( int i=0; i<iEntityCount; i++ )
	{
		ioGameEntity *pEntity = m_vMountEntityList[i];
		if( pEntity )
		{
			ioFieldItem *pItem = ToFieldItem( pEntity );
			if( pItem )
			{
				pItem->SetDropState( pItem->GetWorldPosition() );
			}
			else
			{
				pEntity->NeedUpdateBottomHeight();
			}
		}
	}

	int iWeaponCount = m_vMountWeaponList.size();	
	for( i=0; i<iWeaponCount; i++ )
	{
		ioMineWeapon *pMine = ToMineWeapon( m_vMountWeaponList[i] );
		if( !pMine || !pMine->IsCanExplosion() )
			continue;

		ioBaseChar *pKillChar = m_pModeParent->GetCreator()->GetBaseChar( m_LastAttacker );
		if( pKillChar )
		{
			if( pKillChar->IsNeedSendNetwork() )
			{
				pMine->ExplosionMine( m_pModeParent->GetCreator(), true );

				if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
				{
					SP2Packet kPacket( CUPK_MINE_EXPLOSION );
					kPacket << pMine->GetOwnerName();
					kPacket << pMine->GetWeaponIndex();
					kPacket << pMine->GetPosition();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		else
		{
			if( P2PNetwork::IsHost() )
			{
				pMine->ExplosionMine( m_pModeParent->GetCreator(), true );

				if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
				{
					SP2Packet kPacket( CUPK_MINE_EXPLOSION );
					kPacket << pMine->GetOwnerName();
					kPacket << pMine->GetWeaponIndex();
					kPacket << pMine->GetPosition();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
}

float ioDummyChar::GetCurAngle()
{
	D3DXQUATERNION qtRot = GetWorldOrientation();

	float fCurYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	return fCurYaw;
}

int ioDummyChar::GetDummyGrassType()
{
	return DGT_NONE;
}

void ioDummyChar::SetHide( bool bSetHide )
{
	if ( !m_pGroup )
		return;

	m_pGroup->SetVisible( !bSetHide );
}

void ioDummyChar::FillDummyCharMidSync( ioBaseChar* pOwner, SP2Packet &rkPacket )
{
	//중간 입장시 동기화
}

void ioDummyChar::ApplyDummyCharMidSync( ioBaseChar* pOwner, SP2Packet &rkPacket )
{
	//중간 입장시 동기화
}

int ioDummyChar::GetSkillMaxActiveCnt()
{
	return 0;
}

int ioDummyChar::GetSkillCurActiveCnt()
{
	return 0;
}

void ioDummyChar::CallEndWeapon( bool bWoundedDie )
{
	if ( bWoundedDie )
		return;

	if ( m_EndWeaponInfoList.empty() )
		return;

	if ( m_bAlreadyCallEndWeapon )
		return;

	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;
	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	WeaponInfoList::iterator iter;
	for( iter = m_EndWeaponInfoList.begin(); iter != m_EndWeaponInfoList.end(); ++iter )
	{
		WeaponInfo pInfo = (*iter);

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;
		kFireTime.dwDuration = pInfo.m_dwDuration;
		kFireTime.iAttributeIdx = pInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = pInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni = pInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = pInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = pInfo.m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex++;
		kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
		kFireTime.m_iExtraValue = m_iCreateIndex;

		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, GetWorldPosition(), vDir, "" );
		ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
		if ( pZone )
		{
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( m_bEndWeaponAttachOwner, false );
		}
	}

	m_bAlreadyCallEndWeapon = true;
}

void ioDummyChar::SetSkillDummyAniCode( int iSkillDummmyAniCode )
{
	m_iCurDummySkillAniCode = iSkillDummmyAniCode;
}

void ioDummyChar::SetSkillDummyActionAni( DWORD& dwMotionEndTime, DWORD& dwFireStartTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if ( m_iCurDummySkillAniCode == 0 )
		return;

	DummySkillAniInfoMap::iterator iter = m_DummySkillAniInfoMap.find( m_iCurDummySkillAniCode );
	if( iter == m_DummySkillAniInfoMap.end() )
	{
		m_iCurDummySkillAniCode = 0;
		return;
	}

	DummySkillAniInfo rkInfo = iter->second;
	int iAniID = m_pGroup->GetAnimationIdx( rkInfo.szAniName );
	if ( iAniID == -1 )
		return;

	float fAniRate  = rkInfo.fAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();
	dwFireStartTime = dwMotionEndTime = dwCurTime;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
	dwMotionEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

	dwFireStartTime += m_pGroup->GetAnimationEventTime( iAniID, "fire_s" ) * fAniRate;
	m_iCurDummySkillAniCode = 0;
}

void ioDummyChar::SetSkillDummyLoopAni( DWORD& dwFireStartTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if ( m_iCurDummySkillAniCode == 0 )
		return;

	DummySkillAniInfoMap::iterator iter = m_DummySkillAniInfoMap.find( m_iCurDummySkillAniCode );
	if( iter == m_DummySkillAniInfoMap.end() )
	{
		m_iCurDummySkillAniCode = 0;
		return;
	}

	DummySkillAniInfo rkInfo = iter->second;
	int iAniID = m_pGroup->GetAnimationIdx( rkInfo.szAniName );
	if ( iAniID == -1 )
		return;

	float fAniRate  = rkInfo.fAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();
	dwFireStartTime = dwCurTime;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );

	dwFireStartTime += m_pGroup->GetAnimationEventTime( iAniID, "fire_s" ) * fAniRate;
	m_iCurDummySkillAniCode = 0;
}