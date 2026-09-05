

#include "stdafx.h"

#include "ioSkill.h"
#include "ioRSoldierInfoManager.h"
#include "ItemDefine.h"

ioSkill::ioSkill()
{
	m_pOwnerItem = NULL;

	m_fNeedGauge = 1000000.0f;
	m_fFireDecreaseGauge = 1000000.0f;
	
	m_dwFireMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_dwPreDelayStartTime = 0;
	m_dwEnableReserveTime = 0;

	m_bReduceGauge = false;

	m_fExtraAniRate = FLOAT1;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();

	m_SkillUseType = SUT_NORMAL;

	m_dwSkillMapEffect = -1;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_fCurChargeRate = 0.0f;

	m_ProtectCancelType = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bSetHandMesh = false;
	m_bChangeDirRot = false;

	m_SkillSoundID = 0;
	m_dwWeaponBaseIndex = 0;

	m_bChangeGrapplingTargetPos = true;

	InitExtraAniJump();

	m_dwSkillStartTime = 0;

	m_iCurActiveCount = 0;

	m_fPassiveSkillGaugeRate = FLOAT1;

	m_nEnableObjitemSkillType = OET_NONE;

	m_OwnerPassiveBuffList.clear();

	m_bAutomaticBuffOn = false;

	// 2020-05-09
	m_iCurSkywalkerJumpCount	= 0;
}

ioSkill::ioSkill( const ioSkill &rhs )
: m_Name( rhs.m_Name ),
  m_DescName( rhs.m_DescName ),
  m_DescFullName( rhs.m_DescFullName ),
  m_Desc( rhs.m_Desc ),
  m_DescBubble( rhs.m_DescBubble ),
  m_dwDescBubbleDuration( rhs.m_dwDescBubbleDuration ),
  m_SkillMotionName( rhs.m_SkillMotionName ),
  m_DownSkillMotionName( rhs.m_DownSkillMotionName ),
  m_SkillEffectName( rhs.m_SkillEffectName ),
  m_SkillEndEffectName( rhs.m_SkillEndEffectName ),
  m_SkillMapEffectName( rhs.m_SkillMapEffectName ),
  m_IconName( rhs.m_IconName ),
  m_SkillTextImage( rhs.m_SkillTextImage ),
  m_PreDelayAnimation( rhs.m_PreDelayAnimation ),
  m_DownPreDelayAnimation( rhs.m_DownPreDelayAnimation ),
  m_PreDelayEffect( rhs.m_PreDelayEffect ),
  m_PreDelayOwnerEffect( rhs.m_PreDelayOwnerEffect ),
  m_PreDelayOtherEffect( rhs.m_PreDelayOtherEffect ),
  m_bEnableDownSkill( rhs.m_bEnableDownSkill ),
  m_bEnableJumpingSkill( rhs.m_bEnableJumpingSkill ),
  m_bEnableJumpingSkillOnBlowWound( rhs.m_bEnableJumpingSkillOnBlowWound ),
  m_fEnableJumpingSkillMaxHeight( rhs.m_fEnableJumpingSkillMaxHeight ),
  m_bEnableBlowWoundSkill( rhs.m_bEnableBlowWoundSkill ),
  m_bNoCheckDropSpeed( rhs.m_bNoCheckDropSpeed ),
  m_fEnableBlowWoundSkillHeight( rhs.m_fEnableBlowWoundSkillHeight ),
  m_fEnableBlowWoundSkillMaxHeight( rhs.m_fEnableBlowWoundSkillMaxHeight ),
  m_fSkillEndNextJumpAmt( rhs.m_fSkillEndNextJumpAmt ),
  m_fSkillEndNextLandRate( rhs.m_fSkillEndNextLandRate ),
  m_EnableCharCollisionSkip( rhs.m_EnableCharCollisionSkip ),
  m_bEnableWoundSkill( rhs.m_bEnableWoundSkill ),
  m_bInitJumpPower( rhs.m_bInitJumpPower ),
  m_bCircleNoChangeY( rhs.m_bCircleNoChangeY ),
  m_bNoChangeFallState( rhs.m_bNoChangeFallState ),
  m_bHideEquipItemByRotationWeapon( rhs.m_bHideEquipItemByRotationWeapon ),
  m_fSkillMoveRate( rhs.m_fSkillMoveRate ),
  m_fSkillGravityRate( rhs.m_fSkillGravityRate ),
  m_fSkillGravityDownRate( rhs.m_fSkillGravityDownRate ),
  m_fSkillGravityJumpRate( rhs.m_fSkillGravityJumpRate ),
  m_OwnerSkillStartBuff( rhs.m_OwnerSkillStartBuff ),
  m_OwnerSkillEndBuff( rhs.m_OwnerSkillEndBuff ),
  m_AutoStartBuff( rhs.m_AutoStartBuff ),
  m_bDisableZeroHPSkill( rhs.m_bDisableZeroHPSkill ),
  m_bDisableBossSkill( rhs.m_bDisableBossSkill ),
  m_bDisableKingSkill( rhs.m_bDisableKingSkill ),
  m_bDisableFootballSkill( rhs.m_bDisableFootballSkill ),
  m_bDisableGangSiModeSkill( rhs.m_bDisableGangSiModeSkill ),
  m_iDisableWeaponTypeSkill( rhs.m_iDisableWeaponTypeSkill ),
  m_bDownPreDelayProtect( rhs.m_bDownPreDelayProtect ),
  m_DownPreDelayOwnerEffect( rhs.m_DownPreDelayOwnerEffect ),
  m_DownPreDelayOtherEffect( rhs.m_DownPreDelayOtherEffect ),
  m_JumpSkillMotionName( rhs.m_JumpSkillMotionName ),
  m_fJumpAnimationTimeRate( rhs.m_fJumpAnimationTimeRate ),
  m_bJumpPreDelayProtect( rhs.m_bJumpPreDelayProtect ),
  m_JumpPreDelayAnimation( rhs.m_JumpPreDelayAnimation ),
  m_dwJumpPreDelayTime( rhs.m_dwJumpPreDelayTime ),
  m_JumpPreDelayOwnerEffect( rhs.m_JumpPreDelayOwnerEffect ),
  m_JumpPreDelayOtherEffect( rhs.m_JumpPreDelayOtherEffect ),
  m_SkillSoundName( rhs.m_SkillSoundName ),
  m_dwSwitchSkill( rhs.m_dwSwitchSkill ),
  m_dwSemiSwitchSkill( rhs.m_dwSemiSwitchSkill ),
  m_fMaxDamageRate( rhs.m_fMaxDamageRate ),
  m_fMaxBlowRate( rhs.m_fMaxBlowRate ),
  m_fMaxForceRate( rhs.m_fMaxForceRate ),
  m_fMaxRangeRate( rhs.m_fMaxRangeRate ),
  m_fMaxSpeedRate( rhs.m_fMaxSpeedRate ),
  m_fMaxChargeAniRate( rhs.m_fMaxChargeAniRate ),
  m_fMaxForceSlidingRate( rhs.m_fMaxForceSlidingRate ),
  m_fMaxTargetRangeRate( rhs.m_fMaxTargetRangeRate ),
  m_fMaxTargetAngleRate( rhs.m_fMaxTargetAngleRate ),
  m_fMaxFloatRate( rhs.m_fMaxFloatRate ),
  m_fMaxScaleRate( rhs.m_fMaxScaleRate ),
  m_fMaxExplosionRate( rhs.m_fMaxExplosionRate ),
  m_fMaxLiveTimeRate( rhs.m_fMaxLiveTimeRate ),
  m_fMaxWoundedTimeRate( rhs.m_fMaxWoundedTimeRate ),
  m_fMaxBuffLiveTimeRate( rhs.m_fMaxBuffLiveTimeRate ),
  m_SkillGrapplingInfo( rhs.m_SkillGrapplingInfo ),
  m_bDisableSkillProtect( rhs.m_bDisableSkillProtect ),
  m_fAirForcePower( rhs.m_fAirForcePower ),
  m_fAirJumpPower( rhs.m_fAirJumpPower ),
  m_bUseOnlyDown( rhs.m_bUseOnlyDown ),
  m_bUseOnlyJumpping( rhs.m_bUseOnlyJumpping ),
  m_bUseCustomBlind( rhs.m_bUseCustomBlind ),
  m_SkillBlind( rhs.m_SkillBlind ),
  m_bUseItemMesh( rhs.m_bUseItemMesh ),
  m_bVisibleEquipMesh( rhs.m_bVisibleEquipMesh ),
  m_HandMesh( rhs.m_HandMesh ),
  m_bEnableDirRotNormalSkill( rhs.m_bEnableDirRotNormalSkill ),
  m_bEnableDirRotJumpSkill( rhs.m_bEnableDirRotJumpSkill ),
  m_bEnableDirRotDownSkill( rhs.m_bEnableDirRotDownSkill ),
  m_bEnableIceStateSkill( rhs.m_bEnableIceStateSkill ),
  m_bEnableIFlameStateBuff( rhs.m_bEnableIFlameStateBuff ),
  m_bEnableFlameStateBuffList( rhs.m_bEnableFlameStateBuffList ),
  m_bInitMoveSpeedNormalSkill( rhs.m_bInitMoveSpeedNormalSkill ),
  m_bInitMoveSpeedJumpSkill( rhs.m_bInitMoveSpeedJumpSkill ),
  m_bInitMoveSpeedDownSkill( rhs.m_bInitMoveSpeedDownSkill ),
  m_eDefenseSkillBreakType( rhs.m_eDefenseSkillBreakType ),
  m_bUseLimitHP( rhs.m_bUseLimitHP ),
  m_fLimitHPRate( rhs.m_fLimitHPRate ),
  m_bHideSkillDesc( rhs.m_bHideSkillDesc ),
  m_bEnableCancelBySkill( rhs.m_bEnableCancelBySkill ),
  m_dwEnableCancelBySkillTime( rhs.m_dwEnableCancelBySkillTime ),
  m_GroupName( rhs.m_GroupName ),
  m_TitleName( rhs.m_TitleName ),
  m_CameraBuff( rhs.m_CameraBuff ),
  m_iMaxActiveCount( rhs.m_iMaxActiveCount ),
  m_ExBuffList( rhs.m_ExBuffList ),
  m_bExBuffHPZero( rhs.m_bExBuffHPZero ),
  m_fPassiveSkillGaugeRate( rhs.m_fPassiveSkillGaugeRate ),
  m_bPetForceHide( rhs.m_bPetForceHide ),
  m_bEnableThunderBirdHoldUseSkill( rhs.m_bEnableThunderBirdHoldUseSkill ),
  m_nEnableObjitemSkillType( rhs.m_nEnableObjitemSkillType ),
  m_bSwitchSkillEndJump( rhs.m_bSwitchSkillEndJump ),
  m_bNoCheckSkillmotion( rhs.m_bNoCheckSkillmotion ),
  m_bAutomaticBuffOn( rhs.m_bAutomaticBuffOn ),
  m_iSkywalkerJumpCount( rhs.m_iSkywalkerJumpCount )	// 2020-05-09
{
	m_dwPreDelayTime = rhs.m_dwPreDelayTime;
	m_dwDownPreDelayTime = rhs.m_dwDownPreDelayTime;
	m_ScreenBlindTime = rhs.m_ScreenBlindTime;
	m_dwAnimationEndDelay = rhs.m_dwAnimationEndDelay;

	m_fAnimationTimeRate = rhs.m_fAnimationTimeRate;
	m_fDownAnimationTimeRate = rhs.m_fDownAnimationTimeRate;
	m_fNeedGauge = rhs.m_fNeedGauge;
	m_fFireDecreaseGauge = rhs.m_fFireDecreaseGauge;
	
	m_dwCameraEvent = rhs.m_dwCameraEvent;

	m_iSkillType = rhs.m_iSkillType;
	m_iSubSkillType = rhs.m_iSubSkillType;
    m_bPreDelayProtect = rhs.m_bPreDelayProtect;
	m_ProtectCancelType= rhs.m_ProtectCancelType;

	m_bUseAutoTargetValue = rhs.m_bUseAutoTargetValue;
	m_bAutoTargetDownState = rhs.m_bAutoTargetDownState;
	m_fAutoTargetRange = rhs.m_fAutoTargetRange;
	m_fAutoTargetMinAngle = rhs.m_fAutoTargetMinAngle;
	m_fAutoTargetMaxAngle = rhs.m_fAutoTargetMaxAngle;

	m_pOwnerItem = NULL;

	m_dwFireMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_dwPreDelayStartTime = 0;
	m_dwEnableReserveTime = 0;

	m_bReduceGauge = false;

	m_fExtraAniRate = FLOAT1;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();

	m_SkillUseType = SUT_NORMAL;

	m_dwSkillMapEffect = -1;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_fCurChargeRate = 0.0f;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bSetHandMesh = false;
	m_bChangeDirRot = false;

	m_SkillSoundID = 0;
	m_dwWeaponBaseIndex = 0;
	
	m_bChangeGrapplingTargetPos = true;

	InitExtraAniJump();

	m_dwSkillStartTime = 0;

	m_bNoCheckFallState = false;

	m_OwnerPassiveBuffList = rhs.m_OwnerPassiveBuffList;

	// 2020-05-09
	m_iCurSkywalkerJumpCount	= 0;
}

ioSkill::~ioSkill()
{
	m_vAniRotateInfoList.clear();

	m_OwnerPassiveBuffList.clear();
	
	InitExtraAniJump();
}

ioSkill* ioSkill::Clone()
{
	return new ioSkill( *this );
}

int ioSkill::GetTypeID() const
{
	return m_iSkillType;
}

void ioSkill::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "name", "", szBuf, MAX_PATH );
	m_Name = szBuf;

	rkLoader.LoadString_e( "desc_name", "", szBuf, MAX_PATH );
	m_DescName = szBuf;
	rkLoader.LoadString_e( "desc_full_name", "", szBuf, MAX_PATH );
	m_DescFullName = szBuf;
	rkLoader.LoadString_e( "desc", "", szBuf, MAX_PATH );
	m_Desc = szBuf;

	rkLoader.LoadString_e( "desc_bubble", "", szBuf, MAX_PATH );
	m_DescBubble = szBuf;

	m_dwDescBubbleDuration = (DWORD)rkLoader.LoadInt_e( "desc_bubble_duration", 0 );

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", 1000000.0f );
	m_fFireDecreaseGauge = rkLoader.LoadFloat_e( "need_fire_gauge", 1000000.0f );

	// camera
	rkLoader.LoadString_e( "camera_buff", "", szBuf, MAX_PATH );
	m_CameraBuff = szBuf;

	// Normal
	rkLoader.LoadString_e( "skill_motion", "", szBuf, MAX_PATH );
	m_SkillMotionName = szBuf;

	m_fAnimationTimeRate = rkLoader.LoadFloat_e( "animation_time_rate", FLOAT1 );

	m_bDisableSkillProtect = rkLoader.LoadBool_e( "disable_skill_protect", false );
	m_ProtectCancelType = rkLoader.LoadInt_e( "protect_cancel_type", 0 );
	m_bPreDelayProtect  = rkLoader.LoadBool_e( "pre_delay_protect", false );
	rkLoader.LoadString_e( "pre_delay_animation", "", szBuf, MAX_PATH );
	m_PreDelayAnimation = szBuf;

	m_dwPreDelayTime = rkLoader.LoadInt_e( "pre_delay_time", 0 );
	m_dwAnimationEndDelay = rkLoader.LoadInt_e( "animation_end_delay", 0 );

	rkLoader.LoadString_e( "pre_delay_effect", "", szBuf, MAX_PATH );
	m_PreDelayOwnerEffect = szBuf;

	std::string szOtherBuf;
	szOtherBuf = ioStringConverter::AddTailToFileName( m_PreDelayOwnerEffect.c_str(), "_other" );

	m_PreDelayOtherEffect = ioHashString( szOtherBuf.c_str() );

	// Down
	rkLoader.LoadString_e( "down_skill_motion", "", szBuf, MAX_PATH );
	m_DownSkillMotionName = szBuf;

	m_fDownAnimationTimeRate = rkLoader.LoadFloat_e( "down_animation_time_rate", FLOAT1 );

	m_bDownPreDelayProtect = rkLoader.LoadBool_e( "down_pre_delay_protect", false );
	rkLoader.LoadString_e( "down_pre_delay_animation", "", szBuf, MAX_PATH );
	m_DownPreDelayAnimation = szBuf;

	m_dwDownPreDelayTime = rkLoader.LoadInt_e( "down_pre_delay_time", 0 );

	rkLoader.LoadString_e( "down_pre_delay_effect", "", szBuf, MAX_PATH );
	m_DownPreDelayOwnerEffect = szBuf;

	std::string szDownOtherBuf;
	szDownOtherBuf = ioStringConverter::AddTailToFileName( m_DownPreDelayOwnerEffect.c_str(), "_other" );

	m_DownPreDelayOtherEffect = ioHashString( szDownOtherBuf.c_str() );

	// Jump
	rkLoader.LoadString_e( "jump_skill_motion", "", szBuf, MAX_PATH );
	m_JumpSkillMotionName = szBuf;

	m_fJumpAnimationTimeRate = rkLoader.LoadFloat_e( "jump_animation_time_rate", FLOAT1 );

	m_bJumpPreDelayProtect = rkLoader.LoadBool_e( "jump_pre_delay_protect", false );
	rkLoader.LoadString_e( "jump_pre_delay_animation", "", szBuf, MAX_PATH );
	m_JumpPreDelayAnimation = szBuf;

	m_dwJumpPreDelayTime = rkLoader.LoadInt_e( "jump_pre_delay_time", 0 );

	rkLoader.LoadString_e( "jump_pre_delay_effect", "", szBuf, MAX_PATH );
	m_JumpPreDelayOwnerEffect = szBuf;

	std::string szJumpOtherBuf;
	szJumpOtherBuf = ioStringConverter::AddTailToFileName( m_JumpPreDelayOwnerEffect.c_str(), "_other" );

	m_JumpPreDelayOtherEffect = ioHashString( szJumpOtherBuf.c_str() );
	
	rkLoader.LoadString_e( "skill_effect", "", szBuf, MAX_PATH );
	m_SkillEffectName = szBuf;

	rkLoader.LoadString_e( "skill_end_effect", "", szBuf, MAX_PATH );
	m_SkillEndEffectName = szBuf;

	rkLoader.LoadString_e( "skill_map_effect", "", szBuf, MAX_PATH );
	m_SkillMapEffectName = szBuf;

	rkLoader.LoadString_e( "skill_sound", "", szBuf, MAX_PATH );
	m_SkillSoundName = szBuf;

	rkLoader.LoadString_e( "icon", "", szBuf, MAX_PATH );
	m_IconName = szBuf;

	rkLoader.LoadString_e( "skill_text_image", "", szBuf, MAX_PATH );
	m_SkillTextImage = szBuf;

	rkLoader.LoadString_e( "pre_delay_all_effect", "", szBuf, MAX_PATH );
	m_PreDelayEffect = szBuf;

	m_ScreenBlindTime.m_dwUpTime     = rkLoader.LoadInt_e( "screen_blind_up_time", 0 );
	m_ScreenBlindTime.m_dwCenterTime = max( 1, rkLoader.LoadInt_e( "screen_blind_center_time", 1 ) );
	m_ScreenBlindTime.m_dwDownTime   = max( 1, rkLoader.LoadInt_e( "screen_blind_down_time", 1 ) );

	m_bUseCustomBlind = rkLoader.LoadBool_e( "use_skill_custom_blind", false );

	int iRed, iGreen, iBlue;
	iRed   = rkLoader.LoadInt_e( "skill_color_r", 0 );
	iGreen = rkLoader.LoadInt_e( "skill_color_g", 0 );
	iBlue  = rkLoader.LoadInt_e( "skill_color_b", 0 );
	m_SkillBlind.m_dwColor = D3DCOLOR_ARGB( 0, iRed, iGreen, iBlue );
	m_SkillBlind.m_iMaxAlpha = rkLoader.LoadInt_e( "skill_max_alpha", 0 );

	m_dwCameraEvent = rkLoader.LoadInt_e( "camera_event", 0 );
	m_iSubSkillType = rkLoader.LoadInt_e( "sub_type", 0 );
	m_iSkillType = rkLoader.LoadInt_e( "type", 0 );

	m_dwSwitchSkill = (DWORD)rkLoader.LoadInt_e( "switch_skill", 0 );
	m_dwSemiSwitchSkill = (DWORD)rkLoader.LoadInt_e( "semi_switch_skill", 0 );

	m_bUseAutoTargetValue = rkLoader.LoadBool_e( "use_auto_target_value", false );
	m_bAutoTargetDownState = rkLoader.LoadBool_e( "autotarget_down_state", false );
	m_fAutoTargetRange = rkLoader.LoadFloat_e( "autotarget_range", 300.0f );
	m_fAutoTargetMinAngle = rkLoader.LoadFloat_e( "autotarget_min_angle", 60.0f );
	m_fAutoTargetMaxAngle = rkLoader.LoadFloat_e( "autotarget_max_angle", 150.0f );

	m_EnableCharCollisionSkip = (SkillCharColType)rkLoader.LoadInt_e( "enable_char_collision_skip", SCCT_NONE );

	// For DownSkill
	m_bEnableDownSkill = rkLoader.LoadBool_e( "enable_down_skill", false );
	m_bUseOnlyDown = rkLoader.LoadBool_e( "use_only_down_skill", false );

	// For JumpingSkill
	m_bEnableJumpingSkill = rkLoader.LoadBool_e( "enable_jumping_skill", false );
	m_bEnableJumpingSkillOnBlowWound = rkLoader.LoadBool_e( "enable_jumping_skill_on_blow_wound", false );
	m_bUseOnlyJumpping = rkLoader.LoadBool_e( "use_only_jumppling_skill", false );

	m_fEnableJumpingSkillMaxHeight = rkLoader.LoadFloat_e( "enable_jumping_skill_max_height", 0.0f );

	// For BlowWoundSkill
	m_bEnableBlowWoundSkill = rkLoader.LoadBool_e( "enable_blow_wound_skill", false );
	
	m_bNoCheckDropSpeed = rkLoader.LoadBool_e( "no_check_drop_speed", false );
	m_fEnableBlowWoundSkillHeight = rkLoader.LoadFloat_e( "enable_blow_wound_skill_height", 0.0f );
	m_fEnableBlowWoundSkillMaxHeight = rkLoader.LoadFloat_e( "enable_blow_wound_skill_max_height", 0.0f );

	// For Wound Skill
	m_bEnableWoundSkill = rkLoader.LoadBool_e( "enable_wound_skill", false );

	// For IceState Skill
	m_bEnableIceStateSkill = rkLoader.LoadBool_e( "enable_ice_state_skill", false );

	// for FlameState Skill(buff)
	char szKey[MAX_PATH] = "";

	int iCnt = rkLoader.LoadInt_e( "enable_flame_state_buff_cnt", 0 );
	if( 0 < iCnt )
		m_bEnableIFlameStateBuff = true;
	else
		m_bEnableIFlameStateBuff = false;

	m_bEnableFlameStateBuffList.clear();
	m_bEnableFlameStateBuffList.reserve(iCnt);

	for( int i=0 ; i<iCnt ; i++ )
	{
		wsprintf_e( szKey, "enable_flame_state_buff%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_bEnableFlameStateBuffList.push_back( szBuf );
	}
	
	// For Skill End Next Jump
	m_fSkillEndNextJumpAmt = rkLoader.LoadFloat_e( "skill_end_next_jump_amt", 0.0f );
	m_fSkillEndNextLandRate = rkLoader.LoadFloat_e( "skill_end_next_land_rate", 0.0f );

	m_bInitJumpPower = rkLoader.LoadBool_e( "init_jump_power", false );
	m_bCircleNoChangeY = rkLoader.LoadBool_e( "circle_no_change_height", false );

	// For SkillMove
	m_fSkillMoveRate = rkLoader.LoadFloat_e( "skill_move_speed_rate", FLOAT1 );

	m_bNoChangeFallState = rkLoader.LoadBool_e( "no_change_fall_state", false );
	m_bHideEquipItemByRotationWeapon = rkLoader.LoadBool_e( "hide_mesh_by_rotation", false );

	// For GravityRate
	m_fSkillGravityRate = rkLoader.LoadFloat_e( "skill_gravity_rate", FLOAT1 );
	m_fSkillGravityDownRate = rkLoader.LoadFloat_e( "skill_gravity_down_rate", 0.0f );
	m_fSkillGravityJumpRate = rkLoader.LoadFloat_e( "skill_gravity_jump_rate", 0.0f );

	// ZeroHP, Boss, King
	m_bDisableZeroHPSkill = rkLoader.LoadBool_e( "disable_zero_hp", false );
	m_bDisableBossSkill = rkLoader.LoadBool_e( "disable_boss", false );
	m_bDisableKingSkill = rkLoader.LoadBool_e( "disable_king", false );
	m_bDisableFootballSkill = rkLoader.LoadBool_e( "disable_football", false );
	m_bDisableGangSiModeSkill = rkLoader.LoadBool_e( "disable_gangsi_mode", false );
	m_iDisableWeaponTypeSkill = rkLoader.LoadInt_e( "disable_weapon_type", 0 );

	rkLoader.LoadString_e( "owner_skill_end_buff", "", szBuf, MAX_PATH );
	m_OwnerSkillEndBuff = szBuf;
	rkLoader.LoadString_e( "owner_skill_start_buff", "", szBuf, MAX_PATH );
	m_OwnerSkillStartBuff = szBuf;
	rkLoader.LoadString_e( "auto_start_buff", "", szBuf, MAX_PATH );
	m_AutoStartBuff = szBuf;

	m_fMaxDamageRate = rkLoader.LoadFloat_e( "skill_max_damage_rate", FLOAT1 );
	m_fMaxForceRate = rkLoader.LoadFloat_e( "skill_max_force_rate", FLOAT1 );
	m_fMaxBlowRate = rkLoader.LoadFloat_e( "skill_max_blow_rate", FLOAT1 );

	m_fMaxRangeRate = rkLoader.LoadFloat_e( "skill_max_range_rate", FLOAT1 );
	m_fMaxSpeedRate = rkLoader.LoadFloat_e( "skill_max_speed_rate", FLOAT1 );
	m_fMaxChargeAniRate = rkLoader.LoadFloat_e( "skill_max_ani_rate", 0.0f );
	m_fMaxForceSlidingRate = rkLoader.LoadFloat_e( "skill_max_force_sliding_rate", 0.0f );

	m_fMaxTargetRangeRate = rkLoader.LoadFloat_e( "skill_max_target_range_rate", 0.0f );
	m_fMaxTargetAngleRate = rkLoader.LoadFloat_e( "skill_max_target_angle_rate", 0.0f );

	m_fMaxFloatRate = rkLoader.LoadFloat_e( "skill_max_float_rate", 0.0f );

	m_fMaxScaleRate = rkLoader.LoadFloat_e( "skill_max_scale_rate", 0.0f );
	m_fMaxExplosionRate = rkLoader.LoadFloat_e( "skill_max_explosion_rate", 0.0f );

	m_fMaxLiveTimeRate = rkLoader.LoadFloat_e( "skill_max_livetime_rate", FLOAT1 );
	m_fMaxWoundedTimeRate = rkLoader.LoadFloat_e( "skill_max_wounded_time_rate", FLOAT1 );

	m_fMaxBuffLiveTimeRate = rkLoader.LoadFloat_e( "skill_max_buff_live_time_rate", FLOAT1 );

	// grappling
	m_SkillGrapplingInfo.Init();
	m_SkillGrapplingInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "target_reverse_rotate", false );
	m_SkillGrapplingInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "wound_grappling_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "wound_grappling_loop_motion", "", szBuf, MAX_PATH );
	m_SkillGrapplingInfo.m_GrapplingLoopMotion = szBuf;
	m_SkillGrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "wound_grappling_loop_motion_rate", FLOAT1 );

	// air jump
	m_fAirJumpPower = rkLoader.LoadFloat_e( "jump_air_power", 0.0f );
	m_fAirForcePower = rkLoader.LoadFloat_e( "jump_air_force_power", 0.0f );

	// use hand mesh
	m_bUseItemMesh = rkLoader.LoadBool_e( "use_item_mesh", false );
	m_bVisibleEquipMesh = rkLoader.LoadBool_e( "visible_equip_mesh", false );
	rkLoader.LoadString_e( "hand_mesh", "", szBuf, MAX_PATH );
	m_HandMesh = szBuf;

	// dir rot
	m_bEnableDirRotNormalSkill = rkLoader.LoadBool_e( "enable_dir_rot_normal_skill", false );
	m_bEnableDirRotJumpSkill = rkLoader.LoadBool_e( "enable_dir_rot_jump_skill", false );
	m_bEnableDirRotDownSkill = rkLoader.LoadBool_e( "enable_dir_rot_down_skill", false );

	// init movespeed
	m_bInitMoveSpeedNormalSkill = rkLoader.LoadBool_e( "init_move_speed_normal_skill", false );
	m_bInitMoveSpeedJumpSkill = rkLoader.LoadBool_e( "init_move_speed_jump_skill", false );
	m_bInitMoveSpeedDownSkill = rkLoader.LoadBool_e( "init_move_speed_down_skill", false );

	m_eDefenseSkillBreakType = (DefenseBreakType)rkLoader.LoadInt_e( "defense_skill_break_type", DBT_NONE );

	// limit hp rate
	m_bUseLimitHP = rkLoader.LoadBool_e( "use_limit_hp", false );
	m_fLimitHPRate = rkLoader.LoadFloat_e( "limit_hp_rate", 0.0f );

	// for cancel skill
	m_bEnableCancelBySkill = rkLoader.LoadBool_e( "enable_cancel_skill", false );
	m_dwEnableCancelBySkillTime = (DWORD)rkLoader.LoadInt_e( "enable_cancel_skill_time", 0 );

	// for hide skill desc
	m_bHideSkillDesc = rkLoader.LoadBool_e( "hide_skill_desc", false );

	// for ActiveCount
	m_iMaxActiveCount = rkLoader.LoadInt_e( "max_active_count", 0 );

	m_bExBuffHPZero = rkLoader.LoadBool_e( "exception_buff_hpzero",false );
	int iExCnt = rkLoader.LoadInt_e( "exception_buff_cnt", 0 );
	m_ExBuffList.clear();
	m_ExBuffList.reserve( iExCnt );
	for( int i=0; i<iExCnt; ++i )
	{
		wsprintf_e( szKey, "exception_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ExBuffList.push_back( szBuf );
	}

	m_fPassiveSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_inc_rate", FLOAT1 );

	m_bPetForceHide = rkLoader.LoadBool_e( "pet_force_hide", false );

	m_bEnableThunderBirdHoldUseSkill = rkLoader.LoadBool_e( "enable_thunder_bird_hold", false );

	m_nEnableObjitemSkillType = (ObjEnableType)rkLoader.LoadInt_e( "objitem_enable_skill_type", (int)OET_NONE );

	m_bSwitchSkillEndJump = rkLoader.LoadBool_e( "switch_skill_end_jump", false );
	m_bNoCheckSkillmotion = rkLoader.LoadBool_e( "no_check_skill_motion", false );

	int iCount = rkLoader.LoadInt_e( "owner_passive_buff_cnt", 0 );
	for( int i = 0; i < iCount; ++i )
	{
		wsprintf_e( szKey, "owner_passive_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioBuffInfo BuffInfo;
		BuffInfo.m_BuffName = szBuf;

		if( !BuffInfo.m_BuffName.IsEmpty() )
		{			
			BuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( BuffInfo.m_BuffName );			
			if( BuffInfo.m_iBuffType != BT_NONE )
			{	
				m_OwnerPassiveBuffList.push_back( BuffInfo );
			}
		}
	}

	// 2020-05-09
	m_iSkywalkerJumpCount = rkLoader.LoadInt_e( "skywalker_jump_count", 0 );
}

void ioSkill::SetGroupName( const ioHashString& szGroupName )
{
	m_GroupName = szGroupName;
}

void ioSkill::SetTitleName( const ioHashString& szTitleName )
{
	m_TitleName = szTitleName;
}

void ioSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
}

bool ioSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	return true;
}

bool ioSkill::IsCanUseZeroHP( ioBaseChar *pChar ) const
{
	if( ioPlayMode::IsZeroHP() && ( HasBuff( BT_FLYING_CONTRAL ) || HasBuff( BT_MAGNETIC_FLYING_CONTRAL ) ) )	// 2020-12-09
	{
		if( pChar->HasBuff(BT_FLYING_CONTRAL) || pChar->HasBuff(BT_MAGNETIC_FLYING_CONTRAL) )
			return true;

		if( ioPlayMode::GetModeType() == MT_CATCH || ioPlayMode::GetModeType() == MT_CATCH_RUNNINGMAN ||
			ioPlayMode::GetModeType() == MT_UNDERWEAR || ioPlayMode::GetModeType() == MT_CBT || ioPlayMode::GetModeType() == MT_FARMING )
		{
			if( m_bDisableZeroHPSkill )
				return false;
		}

		return true;
	}

	if( ioPlayMode::IsZeroHP() && m_bDisableZeroHPSkill )
		return false;

	if( pChar && pChar->GetHP().m_fCurValue == 0.0f )
	{
		if( HasBuff( BT_HP_CONVERT ) )
			return false;
	}

	return true;
}

bool ioSkill::IsCanUseSkillWithCrown( ioBaseChar *pChar )
{
	if( !pChar->IsHasCrown() )
		return true;

	if( m_bDisableKingSkill )
		return false;

	return true;
}

bool ioSkill::IsCanUseBoss( ioBaseChar *pChar ) const
{
	if( ioPlayMode::GetModeType() != MT_BOSS )
		return true;

	if( !pChar )
		return false;

	if( !pChar->IsBoss() )
		return true;

	if( m_bDisableBossSkill )
		return false;

	return true;
}

bool ioSkill::IsCanUseFootball() const
{
	if( ioPlayMode::GetModeType() != MT_FOOTBALL )
		return true;

	if( m_bDisableFootballSkill )
		return false;

	return true;
}

bool ioSkill::IsCanUseGangSiMode() const
{
	if( ioPlayMode::GetModeType() != MT_GANGSI )
		return true;

	if( m_bDisableGangSiModeSkill )
		return false;

	return true;
}

bool ioSkill::IsCanUseByWeaponType( ioBaseChar *pChar ) const
{
	if( m_iDisableWeaponTypeSkill == 0 )
		return true;

	if( g_WeaponMgr.HasWeaponType( pChar, m_iDisableWeaponTypeSkill ) )
		return false;

	return true;
}

bool ioSkill::IsCanUseLimitHP( ioBaseChar *pChar ) const
{
	if( !pChar )
		return false;

	if( !m_bUseLimitHP )
		return true;

	StatusValue rkHP = pChar->GetHP();
	float fCurRate = rkHP.m_fCurValue / rkHP.m_fMaxValue;
	if( fCurRate <= m_fLimitHPRate )
		return false;

	return true;
}

bool ioSkill::IsCanUseThunderBirdHold( ioBaseChar *pChar ) const
{
	if( !pChar )
		return false;

	if ( pChar->GetState() != CS_THUNDER_BIRD_HOLD )
		return true;

	if( !m_bEnableThunderBirdHoldUseSkill )
		return false;

	return true;
}

bool ioSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	pChar->SetSkillProtection();

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	m_SkillSoundID = 0;

	InitExtraAniJump();

	m_bReduceGauge = false;
	m_bChangeDirRot = false;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = dwCurTime + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = dwCurTime + m_dwSemiSwitchSkill;

	m_bChangeGrapplingTargetPos = true;
	m_dwSkillStartTime = dwCurTime;

	SetSkillNameChatBubble( pChar );

	m_iCurActiveCount = 0;

	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( pChar );
		if ( pPet )
			pPet->SetForceHide( true );
	}

	return true;
}

void ioSkill::OnSkillEndByWounded( ioBaseChar *pChar )
{
	//피격에 의해 스킬이 취소 됬을때
}

void ioSkill::OnSkillEnd( ioBaseChar *pChar )
{
	m_bReduceGauge = false;
	m_bChangeGrapplingTargetPos = true;
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();

	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );

	ioMechanicsItem *pMechanics = ToMechanicsItem( GetOwnerItem() );
	ioPhantomItem *pPhantom = ToPhantomItem( GetOwnerItem() );
	ioDestroyerItem *pDestroyer = ToDestroyerItem( GetOwnerItem() );

	if( pMechanics )
		pMechanics->ClearMechanicsTargetSkill();
	else if( pPhantom )
		pPhantom->ClearPhantomTargetSkill();
	else if( pDestroyer )
		pDestroyer->ClearDestroyerTargetSkill();
	
	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( pChar );
		if ( pPet )
			pPet->SetForceHide( false );
	}
}

void ioSkill::OnProcessNotUseSkill( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if(!m_bAutomaticBuffOn && !m_OwnerPassiveBuffList.empty())
		SetOwnerAutomaticBuff(pChar, pStage);
}

void ioSkill::OnSkillItemRelease( ioBaseChar *pChar )
{
	if(!m_OwnerPassiveBuffList.empty() && pChar)
	{
		for(int i = 0; i < (int)m_OwnerPassiveBuffList.size(); i++)
		{
			pChar->RemoveBuff(m_OwnerPassiveBuffList[i].m_BuffName);
		}
	}
}

bool ioSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

void ioSkill::SetMotionTime( DWORD dwStartTime, DWORD dwEndTime )
{
	m_dwMotionStartTime = dwStartTime;
	m_dwMotionEndTime = dwEndTime;
}

void ioSkill::SetKeyReserveTime( DWORD dwEnableTime )
{
	m_dwEnableReserveTime = dwEnableTime;
}

bool ioSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioSkill::IsCanSwitchSkill() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( !m_bReduceGauge )
		return false;

	if( m_dwEnableSwitchTime > 0 && m_dwEnableSwitchTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioSkill::IsCanSemiSwitchSkill() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( !m_bReduceGauge )
		return false;

	if( m_dwEnableSemiSwitchTime > 0 && m_dwEnableSemiSwitchTime < FRAMEGETTIME() )
		return true;

	return false;
}

// 2020-05-09
bool ioSkill::IsCurSkywalkerJumpCount() const
{
	if( m_iCurSkywalkerJumpCount <= 0 )
	{
		return false;
	}

	return true;
}

bool ioSkill::IsJumpState() const
{
	return false;
}

bool ioSkill::IsDownState() const
{
	return false;
}

SkillCharColType ioSkill::IsCharSkipState() const
{
	return m_EnableCharCollisionSkip;
}

bool ioSkill::IsEntityCollisionSkipState() const
{
	return false;
}

bool ioSkill::IsEndState() const
{
	return false;
}

bool ioSkill::IsAttackEndState() const
{
	return false;
}

bool ioSkill::IsNoDropState() const
{
	return false;
}

bool ioSkill::HasBuff( int iBuffType ) const
{
	return false;
}

bool ioSkill::IsOnOffEnable()
{
	return false;
}

void ioSkill::GetAutoTargetValue( float &fRange, float &fMinAngle, float &fMaxAngle )
{
	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;
}

void ioSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( GetOwnerItem() )
	{
		pWeapon->SetCreateItem( GetOwnerItem()->GetName() );
	}

	float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
	float fCurScaleRate, fCurExplosionRate, fCurLiveTimeRate, fCurWoundedTimeRate;
	float fCurBuffLiveTimeRate;

	fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
	fCurScaleRate = fCurExplosionRate = fCurLiveTimeRate = fCurWoundedTimeRate = FLOAT1;
	fCurBuffLiveTimeRate = FLOAT1;

	if( m_fCurChargeRate > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );

		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );

		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );

		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );
		float fCurMaxWoundedTime = max( 0.0f, (m_fMaxWoundedTimeRate - FLOAT1) );

		float fCurMaxBuffLiveTime = max( 0.0f, (m_fMaxBuffLiveTimeRate - FLOAT1) );

		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
		fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

		fCurRangeRate += fCurMaxRange * m_fCurChargeRate;
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRate;

		fCurFloatRate += fCurMaxFloat * m_fCurChargeRate;

		fCurScaleRate += fCurMaxScale * m_fCurChargeRate;
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRate;

		fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;
		fCurWoundedTimeRate += fCurMaxWoundedTime * m_fCurChargeRate;

		fCurBuffLiveTimeRate += fCurMaxBuffLiveTime * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );

		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );

		pWeapon->SetOwnerChargeScaleRate( fCurScaleRate );
		pWeapon->SetOwnerChargeExplosionRate( fCurExplosionRate );

		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
		pWeapon->SetOwnerChargeWoundedTimeRate( fCurWoundedTimeRate );

		pWeapon->SetOwnerChargeBuffLiveTimeRate( fCurBuffLiveTimeRate );
	}
}

void ioSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
}

void ioSkill::ApplyEquipSlotSkillExtraInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
}

bool ioSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	ioHashString szPreDelay = GetPreDelayAniName();

	if( szPreDelay.IsEmpty() )
	{
		m_dwPreDelayStartTime = 0;
		return false;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( szPreDelay, FLOAT100 );

	m_dwPreDelayStartTime = FRAMEGETTIME();
	StartPreDelayEffect( pChar );
	
	return true;
}

void ioSkill::StartPreDelayEffect( ioBaseChar *pChar )
{
	if( pChar->IsOwnerChar() )
	{
		pChar->AttachEffect( GetPreDelayOwnerEffect() );
	}
	else
	{
		pChar->AttachEffect( GetPreDelayOtherEffect() );
	}

	pChar->AttachEffect( m_PreDelayEffect );
}

void ioSkill::EndPreDelayEffect( ioBaseChar *pChar )
{
	if( pChar->IsOwnerChar() )
		pChar->EndEffect( GetPreDelayOwnerEffect() );
	else
		pChar->EndEffect( GetPreDelayOtherEffect() );

	pChar->EndEffect( m_PreDelayEffect );
}

bool ioSkill::IsUseAutoTargetValue()
{
	return m_bUseAutoTargetValue;
}

bool ioSkill::IsAutoTargetDownState()
{
	if( IsUseAutoTargetValue() )
		return m_bAutoTargetDownState;

	return false;
}

bool ioSkill::CheckGaugeMax( const ioHashString &szName )
{
	CEncrypt<bool> bRtnValue = false;
	if( GetOwnerItem()->HasSkillGauge() )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage && pStage->GetBaseChar(szName) )
		{
			int iState = pStage->GetBaseChar(szName)->GetState();
			if(pStage->GetModeType() == MT_HEADQUARTERS && iState != CS_USING_SKILL && iState != CS_FLY)
			{
				if(g_KeyManager.IsCoolTimeResetPress())
				{
					GetOwnerItem()->SetCurSkillGauge(GetOwnerItem()->GetMaxSkillGauge(szName));
					return false;
				}
			}
		}
		if( GetOwnerItem()->GetCurSkillGuage() >= GetOwnerItem()->GetMaxSkillGauge(szName) )
			bRtnValue = true;
	}
	
	return (bool)bRtnValue;
}

bool ioSkill::CheckUseBuff( ioBaseChar *pChar )
{
	return false;
}

void ioSkill::CheckSkillDesc( ioBaseChar *pOwner )
{
	if( Setting::IsHideSkillBuffEmoticon() )
		return;

	if( m_bHideSkillDesc )
		return;

	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	const vHelpText *pHelpList = g_ModeHelpMgr.GetHelpText( ioModeHelpManager::HT_SKILL );
	if( !pHelpList )
	{
		LOG.PrintTimeAndLog( 0, "ioSkill::CheckSkillDesc() Emoticon is Not Exist" );
		return;
	}

	if( pHelpList->empty() ) return;

	const HelpText &rkText = (*pHelpList->begin());
			
	DWORD dwColor = rkText.m_dwColor;

	ioHashString szDesc;
	char szBuf[MAX_PATH] = "";

	int iCurLevel = GetGrowthLevel( pOwner->GetCharName() );
	int iCurReinforce = GetGrowthExtraLevel();
	iCurLevel += iCurReinforce;

	int iClassType = g_MyInfo.GetClassType();
	MedalSyncInfoVec vMedalItemTypeVec;
	g_MyInfo.GetMedalItemTypeVec( vMedalItemTypeVec, iClassType );

	if( GetOwnerItem() )
	{
		int iSlotIndex = GetOwnerItem()->GetType();
		if( COMPARE( iSlotIndex, ioItem::IT_WEAPON, ioItem::IT_OBJECT ) )
		{
			int iMedalSize = vMedalItemTypeVec.size();
			for (int i = 0; i < iMedalSize ; i++)
			{
				if( vMedalItemTypeVec[i].m_iCustomIndex == 0 )
					iCurLevel += g_MedalItemMgr.GetMedalItemGrowth( vMedalItemTypeVec[i].m_iItemType, false, iSlotIndex-1 );
				else
					iCurLevel += vMedalItemTypeVec[i].m_iStat[MAX_ITEM_GROWTH + iSlotIndex - 1];
			}
		}
		
		//펫 성장
		CurPetInfo sCurPetInfo;
		pStage->GetCurPetInfo( pOwner->GetCharName(), sCurPetInfo );

		if ( sCurPetInfo.m_nPetCode > 0)
			iCurLevel += g_PetInfoMgr.GetTotalStat( sCurPetInfo.m_nPetCode, iSlotIndex - 1 + MAX_ITEM_GROWTH, sCurPetInfo.m_nCurLevel );

		const CHARACTER& rkCharInfo = pOwner->GetCharacterInfo();
		if( g_RSoldierInfoManager.IsRSoldierType( rkCharInfo ) )
			iCurLevel += g_RSoldierInfoManager.GetSoldierAddStat( iSlotIndex - 1 + MAX_ITEM_GROWTH, pOwner->GetCurRSoldierCount() );

		if ( g_RSoldierInfoManager.IsSoldierSet( rkCharInfo ) )
			iCurLevel += g_RSoldierInfoManager.GetSoldierSetAddStat( iSlotIndex - 1 + MAX_ITEM_GROWTH, rkCharInfo.m_class_type, pOwner->GetCurSoldierSetCnt() );
	}

	if( iCurLevel > 0 )
	{
		wsprintf_e( szBuf, "%s +%d", m_DescName.c_str(), iCurLevel );
		szDesc = szBuf;
	}
	else
		szDesc = m_DescName;

	pOwner->SetNamedEmoticon( m_IconName, szDesc, dwColor );
}

bool ioSkill::CheckNeedGauge( ioBaseChar *pOwner, bool bHelpMsg )
{
	// 게이지 부족에의해 사용불가시 도움말 출력
	if( !CheckGaugeMax(pOwner->GetCharName()) )
	{
		if( pOwner->IsOwnerChar() && bHelpMsg )
		{
			switch( GetOwnerItem()->GetType() )
			{
			case ioItem::IT_WEAPON:
				pOwner->CheckNamedEmoticon( ioModeHelpManager::HT_NEED_GAUGE_W, false );
				break;
			case ioItem::IT_ARMOR:
				pOwner->CheckNamedEmoticon( ioModeHelpManager::HT_NEED_GAUGE_A, false );
				break;
			case ioItem::IT_HELMET:
				pOwner->CheckNamedEmoticon( ioModeHelpManager::HT_NEED_GAUGE_H, false );
				break;
			case ioItem::IT_CLOAK:
				pOwner->CheckNamedEmoticon( ioModeHelpManager::HT_NEED_GAUGE_C, false );
				break;
			}
		}

		return false;
	}

	return true;
}

float ioSkill::GetNeedGauge( const ioHashString &szName )
{
	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, szName));
	if( pUpInfo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	//fValue = min( fValue, FLOAT100 );
	fValue = FLOAT1 + (fValue/FLOAT100);

	float fNeedGauge = m_fNeedGauge / fValue;
	fNeedGauge = max( 0.0f, fNeedGauge );

	return fNeedGauge;
}

float ioSkill::GetFireNeedGauge( const ioHashString &szName )
{
	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, szName));
	if( pUpInfo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	fValue = FLOAT1 + (fValue/FLOAT100);

	float fNeedGauge = m_fFireDecreaseGauge / fValue;
	fNeedGauge = max( 0.0f, fNeedGauge );

	return fNeedGauge;
}

ioGrowthUpInfo* ioSkill::GetGrowthUpInfoByType( int iType, const ioHashString &szName )
{
	int iLevel = 0;
	int iInfoNum = 0;
	int iClassType = 0;

	ioGrowthLevel* pLevel = NULL;
	ioBaseChar *pChar = NULL;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			pLevel = pStage->GetGrowthLevel( szName );

			pChar = pStage->GetBaseChar( szName );
			if( pChar )
			{
				iClassType = pChar->GetCharacterInfo().m_class_type;
			}
		}

		if( pLevel )
		{
			int iGrowthNum = 0;
			iInfoNum = pItem->GetGrowthInfoNumByType( iType, iGrowthNum );

			int iSlot = pItem->GetType() - 1;
			iLevel = pLevel->GetItemGrowthLevel( iClassType, iSlot, false );
			
			int nReinforce = pItem->GetItemReinforce();
			//강화레벨에 따른 추가 스탯
			if ( nReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				iLevel += nReinforce;
			else
				iLevel += g_GrowthInfoMgr.GetReinforceAddStat( nReinforce );

			if( pChar )
			{
				pChar->GetMedalItemGrowth( iLevel, false, iSlot );
				pChar->GetPetGrowth( iLevel, false, iSlot );

				//각성
				pChar->GetAwakeCharGrowth( iLevel, iSlot );
			}

			return g_GrowthInfoMgr.GetGrowthUpInfo( iInfoNum, iLevel );
		}
	}

	return NULL;
}

int ioSkill::GetGrowthLevel( const ioHashString &szName )
{
	int iLevel = 0;
	int iInfoNum = 0;

	ioGrowthLevel* pLevel = NULL;
	if( GetOwnerItem() )
	{
		iLevel = GetOwnerItem()->GetGrowthLevel( szName );
	}

	return iLevel;
}

int ioSkill::GetGrowthExtraLevel()
{
	int iLevel = 0;

	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		int nReinforce = pItem->GetItemReinforce();
		//강화레벨에 따른 추가 스탯
		if ( nReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
			iLevel += nReinforce;
		else
			iLevel += g_GrowthInfoMgr.GetReinforceAddStat( nReinforce );
	}

	return iLevel;
}

void ioSkill::SetExtraAniRate( float fRate )
{
	if( fRate > 0.0f )
		m_fExtraAniRate = FLOAT1 - fRate/FLOAT100;
	else
		m_fExtraAniRate = FLOAT1;

	if( m_fExtraAniRate <= 0.0f )
		m_fExtraAniRate = FLOAT1;
}

const ioHashString& ioSkill::GetSkillMotionName() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_SkillMotionName;
	case SUT_DOWN:
		if( !m_DownSkillMotionName.IsEmpty() )
			return m_DownSkillMotionName;
		return m_SkillMotionName;
	case SUT_JUMP:
		if( !m_JumpSkillMotionName.IsEmpty() )
			return m_JumpSkillMotionName;
		return m_SkillMotionName;
	}

	return m_SkillMotionName;
}

bool ioSkill::HasSkillMotion() const
{
	return !GetSkillMotionName().IsEmpty();
}

float ioSkill::GetSkillMotionRate() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_fAnimationTimeRate;
	case SUT_DOWN:
		if( !m_DownSkillMotionName.IsEmpty() )
			return m_fDownAnimationTimeRate;
		return m_fAnimationTimeRate;
	case SUT_JUMP:
		if( !m_JumpSkillMotionName.IsEmpty() )
			return m_fJumpAnimationTimeRate;
		return m_fAnimationTimeRate;
	}

	return m_fAnimationTimeRate;
}

bool ioSkill::IsPreDelayProtect() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_bPreDelayProtect;
	case SUT_DOWN:
		if( !m_DownSkillMotionName.IsEmpty() )
			return m_bDownPreDelayProtect;
		return m_bPreDelayProtect;
	case SUT_JUMP:
		if( !m_JumpSkillMotionName.IsEmpty() )
			return m_bJumpPreDelayProtect;
		return m_bPreDelayProtect;
	}

	return m_bPreDelayProtect;
}

const ioHashString& ioSkill::GetPreDelayAniName() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_PreDelayAnimation;
	case SUT_DOWN:
		if( !m_DownPreDelayAnimation.IsEmpty() )
			return m_DownPreDelayAnimation;
		return m_PreDelayAnimation;
	case SUT_JUMP:
		if( !m_JumpPreDelayAnimation.IsEmpty() )
			return m_JumpPreDelayAnimation;
		return m_PreDelayAnimation;
	}

	return m_PreDelayAnimation;
}

DWORD ioSkill::GetPreDelayTime() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_dwPreDelayTime;
	case SUT_DOWN:
		if( !m_DownPreDelayAnimation.IsEmpty() )
			return m_dwDownPreDelayTime;
		return m_dwPreDelayTime;
	case SUT_JUMP:
		if( !m_JumpPreDelayAnimation.IsEmpty() )
			return m_dwJumpPreDelayTime;
		return m_dwPreDelayTime;
	}

	return m_dwPreDelayTime;
}

const ioHashString& ioSkill::GetPreDelayOwnerEffect() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_PreDelayOwnerEffect;
	case SUT_DOWN:
		if( !m_DownPreDelayOwnerEffect.IsEmpty() )
			return m_DownPreDelayOwnerEffect;
		return m_PreDelayOwnerEffect;
	case SUT_JUMP:
		if( !m_JumpPreDelayOwnerEffect.IsEmpty() )
			return m_JumpPreDelayOwnerEffect;
		return m_PreDelayOwnerEffect;
	}

	return m_PreDelayOwnerEffect;
}

const ioHashString& ioSkill::GetPreDelayOtherEffect() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_PreDelayOtherEffect;
	case SUT_DOWN:
		if( !m_DownPreDelayOtherEffect.IsEmpty() )
			return m_DownPreDelayOtherEffect;
		return m_PreDelayOtherEffect;
	case SUT_JUMP:
		if( !m_JumpPreDelayOtherEffect.IsEmpty() )
			return m_JumpPreDelayOtherEffect;
		return m_PreDelayOtherEffect;
	}

	return m_PreDelayOtherEffect;
}

bool ioSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioSkill::CheckAniRotate( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_vAniRotateInfoList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwAniRotateTime > 0 && m_dwAniRotateTime < dwCurTime )
	{
		if( m_fAniRotateAngle > 0.0f )
		{
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(m_fAniRotateAngle) );
			D3DXVECTOR3 vMoveDir = qtRot * pChar->GetMoveDir();

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

			pChar->SetTargetRotAndMoveDirChange( qtTargetRot );
		}

		m_iCurAniRotate++;

		int iSize = m_vAniRotateInfoList.size();
		if( COMPARE(m_iCurAniRotate, 0, iSize) )
		{
			m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
			m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
		}
		else
		{
			m_dwAniRotateTime = 0;
			m_fAniRotateAngle = 0.0f;
		}
	}
}

float ioSkill::GetSkillGravityRate()
{
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

bool ioSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	return false;
}

GrapplingSkillWoundedInfo ioSkill::GetSkillWoundGrapplingInfo()
{
	return m_SkillGrapplingInfo;
}

D3DXVECTOR3 ioSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	return ioMath::VEC3_ZERO;
}

D3DXVECTOR3 ioSkill::GetGrapplingTargetPos( ioBaseChar *pAttacker, ioBaseChar *pTarget )
{
	return ioMath::VEC3_ZERO;
}

void ioSkill::SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage )
{
}

float ioSkill::GetSkillMoveRate() const
{
	return m_fSkillMoveRate;
}

const ScreenBlindSetting* ioSkill::GetScreenBlindSetting() const
{
	if( !m_bUseCustomBlind )
		return NULL;

	return &m_SkillBlind;
}

void ioSkill::CheckChangeDirRot( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eDir = pOwner->GetDirKey();

	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		if( m_bEnableDirRotNormalSkill )
		{
			pOwner->SetTargetRotToDirKey( eDir );
			m_bChangeDirRot = true;
		}
		break;
	case SUT_DOWN:
		if( m_bEnableDirRotDownSkill )
		{
			pOwner->SetTargetRotToDirKey( eDir );
			m_bChangeDirRot = true;
		}
		break;
	case SUT_JUMP:
		if( m_bEnableDirRotJumpSkill )
		{
			pOwner->SetTargetRotToDirKey( eDir );
			m_bChangeDirRot = true;
		}
		break;
	}
}

bool ioSkill::IsCanDefenseStateSkill()
{
	return false;
}

void ioSkill::SetDefenseStateSkill( ioBaseChar *pChar )
{
}

void ioSkill::InitExtraAniJump()
{
	m_dwExtraAniJumpTime = 0;
	m_fExtraAniJumpPower = 0.0f;

	m_iCurExtraAniJump = 0;
	m_vExtraAniJumpList.clear();
}

void ioSkill::SetExtraAniJump( ioBaseChar *pChar, int iAniID, float fTimeRate )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwExtraAniJumpTime = 0;
	m_fExtraAniJumpPower = 0.0f;
	m_vExtraAniJumpList.clear();

	AniEventConstPtrList vExtraAniJumpList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );

	AniEventConstPtrList::iterator iter = vExtraAniJumpList.begin();
	while( iter != vExtraAniJumpList.end() )
	{
		ExtraAniJump kInfo;

		kInfo.m_dwTime = (*iter)->fEventTime * fTimeRate;
		kInfo.m_dwTime += dwCurTime;

		if( !(*iter)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vExtraAniJumpList.push_back(kInfo);

		++iter;
	}

	if( !m_vExtraAniJumpList.empty() )
	{
		m_iCurExtraAniJump = 0;
		m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
		m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
	}
}

void ioSkill::CheckExtraAniJump( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_vExtraAniJumpList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwExtraAniJumpTime > 0 && m_dwExtraAniJumpTime <= dwCurTime )
	{
		if( m_fExtraAniJumpPower > 0.0f )
		{
			pChar->SetJumpPower( m_fExtraAniJumpPower );
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

bool ioSkill::IsCanSendControl() const
{
	return true;
}

bool ioSkill::IsSkipState() const
{
	// CS_USING_SKILL상태에서 CS_SKIP_STATE와 동일한 결과를 원할 때 사용한다.
	return false;
}

void ioSkill::CheckInitMoveSpeed( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		if( m_bInitMoveSpeedNormalSkill )
		{
			pOwner->SetCurMoveSpeed( 0.0f );
			m_bChangeDirRot = true;
		}
		break;
	case SUT_DOWN:
		if( m_bInitMoveSpeedDownSkill )
		{
			pOwner->SetCurMoveSpeed( 0.0f );
			m_bChangeDirRot = true;
		}
		break;
	case SUT_JUMP:
		if( m_bInitMoveSpeedJumpSkill )
		{
			pOwner->SetCurMoveSpeed( 0.0f );
			m_bChangeDirRot = true;
		}
		break;
	}
}

void ioSkill::SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock )
{
}

void ioSkill::SetCollisionByApplySkill()
{
	//Weapon의 CheckStructAndDummy()에 의해서 더미/스트럭 등의 오브젝트와
	//충돌이 발생 했을때 스킬에 알려주는 함수
}

void ioSkill::SetChargingRateForWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon )
		return;

	ioPowerChargeItem *pPower = ToPowerChargeItem( GetOwnerItem() );
	ioCountChargeItem *pCount = ToCountChargeItem( GetOwnerItem() );
	ioRangeChargeItem *pRange = ToRangeChargeItem( GetOwnerItem() );
	ioRotateChargeItem *pRotate = ToRotateChargeItem( GetOwnerItem() );

	if( pPower || pCount || pRange || pRotate )
	{
		// Charge...
		float fCurDamageRate = GetOwnerItem()->GetChargeDamageRateForSkill();
		float fCurForceRate = GetOwnerItem()->GetChargeForceRateForSkill();
		float fCurBlowRate = GetOwnerItem()->GetChargeBlowRateForSkill();
		float fCurRangeRate = GetOwnerItem()->GetChargeRangeRateForSkill();
		float fCurSpeedRate = GetOwnerItem()->GetChargeSpeedRateForSkill();
		float fCurFloatRate = GetOwnerItem()->GetChargeFloatRateForSkill();
		float fCurScaleRate = GetOwnerItem()->GetChargeScaleRateForSkill();
		float fCurExplosionRate = GetOwnerItem()->GetChargeExplosionRateForSkill();
		float fCurWoundedTimeRate = GetOwnerItem()->GetChargeWoundedTimeRateForSkill();

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );
		pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );
		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );
		pWeapon->SetOwnerChargeScaleRate( fCurScaleRate );
		pWeapon->SetOwnerChargeExplosionRate( fCurExplosionRate );
		pWeapon->SetOwnerChargeWoundedTimeRate( fCurWoundedTimeRate );
	}
}

bool ioSkill::IsEnableDashCheck( ioBaseChar *pChar )
{
	return false;
}

bool ioSkill::IsEnableMultiActive( ioBaseChar *pChar )
{
	return false;
}

void ioSkill::SetOwnerAutomaticBuff( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	//if( !pOwner->IsNeedProcess() ) return;
	if( m_OwnerPassiveBuffList.empty() ) return;

	pOwner->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pOwner->GetCharName()) );
	if( pUpInfo )
	{		
		fValue = pUpInfo->GetValue(pStage);
	}

	int iCnt = m_OwnerPassiveBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerPassiveBuffList[i].m_BuffName;

		if( pOwner->IsHasCrown() && m_bDisableKingSkill )
		{
			continue;
		}
		else if( pOwner->HasBuff( BT_ABSOLUTE_PROTECTION ) )
			continue;


		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), szItemName, this );
			if( pBuff )
			{				
				pBuff->SetExtraDurationByGrowth( fValue );
				pBuff->SetPassiveBuff(true);
				bResult = true;
				m_bAutomaticBuffOn = true;
				vBuffList.push_back( szBuffName );
			}
		}
	}

	if( bResult )
	{
		int iCurBuffCnt = vBuffList.size();
		if( pOwner->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff2 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pOwner->GetCharName();
			kPacket << pOwner->GetCharName();
			kPacket << szItemName;
			kPacket << pOwner->GetRandomSeed();
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

	return;
}

bool ioSkill::IsCanCancelBySkill( ioBaseChar *pOwner, OUT int &iSkillNum ) const
{
	if( !pOwner )
		return false;

	if( !m_bEnableCancelBySkill )
		return false;

	if( m_dwSkillStartTime <= 0 || m_dwEnableCancelBySkillTime <= 0 )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwSkillStartTime+m_dwEnableCancelBySkillTime < dwCurTime )
	{
		int iSkillKey = pOwner->GetSkillKeyInput();
		if( iSkillKey >= 0 && pOwner->GetUseEnableSkill( iSkillKey, SUT_NORMAL ) && !pOwner->IsBuffLimitSkill() )
		{
			iSkillNum = iSkillKey;
			return true;
		}
	}

	return false;
}

void ioSkill::ClearAirJump()
{
	m_vAirJumpTimeList.erase( m_vAirJumpTimeList.begin(), m_vAirJumpTimeList.end() );
	m_iCurAirJump = 0;
}

void ioSkill::SetAirJump( ioBaseChar *pOwner, int iAniID, float fTimeRate )
{
	ClearAirJump();

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	pGrp->GetAnimationEventTimeList_e( iAniID, "air_jump", m_vAirJumpTimeList );
	std::list< float >::iterator iter = m_vAirJumpTimeList.begin();
	while( iter != m_vAirJumpTimeList.end() )
	{
		(*iter) *= fTimeRate;
		(*iter) += dwCurTime;
		iter++;
	}

	// For AirJumpPower
	m_fAirJumpPowerRate = 0.0f;

	float fJumpPowerAmt = pOwner->GetCurJumpPowerAmt();
	if( fJumpPowerAmt > 0.0f )
	{
		m_fAirJumpPowerRate = fabs( fJumpPowerAmt - pOwner->GetGravityAmt() ) / fJumpPowerAmt;
		m_fAirJumpPowerRate = max( 0.0f, min( FLOAT1, m_fAirJumpPowerRate ) );
		m_fAirJumpPowerRate = FLOAT1 - m_fAirJumpPowerRate;
	}
}

void ioSkill::CheckAirJump( ioBaseChar *pChar )
{
	if( !pChar ) return;

	int iMaxCnt = m_vAirJumpTimeList.size();
	if( m_iCurAirJump < iMaxCnt )
	{
		DWORD dwCurAirJumpTime = 0;
		std::list< float >::iterator iter = m_vAirJumpTimeList.begin();

		for( int i=0; iter != m_vAirJumpTimeList.end(); i++ )
		{
			if( m_iCurAirJump == i )
			{
				dwCurAirJumpTime = (DWORD)(*iter);
				break;
			}

			++iter;
		}

		if( dwCurAirJumpTime > 0 && dwCurAirJumpTime < FRAMEGETTIME() )
		{
			if( m_fAirJumpPower >= 0.0f )
			{
				float fCurJumpPower = m_fAirJumpPower*m_fAirJumpPowerRate;
				if( m_fAirJumpPowerRate == 0.0f )
					fCurJumpPower = m_fAirJumpPower;

				pChar->SetAirJumpPower( fCurJumpPower );

				D3DXVECTOR3 vAttDir =  pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
				if( m_fAirForcePower < 0.0f )
					pChar->SetForcePower( -vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
				else
					pChar->SetForcePower( vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
			}

			m_iCurAirJump++;
		}
	}
}

ioHashStringVec& ioSkill::GetEnableFlameStateBuffList()
{
	return m_bEnableFlameStateBuffList;
}

void ioSkill::SetCancelSkill( ioBaseChar *pChar )
{
}

void ioSkill::SetSkillNameChatBubble( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_DescBubble.IsEmpty() ) return;

	pOwner->SetChatBubble( pOwner->GetCharName().c_str(), m_DescBubble.c_str(), false, FRAMEGETTIME(), m_dwDescBubbleDuration );
}

void ioSkill::SetCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_CameraBuff.IsEmpty() )
		pOwner->AddNewBuff( m_CameraBuff, "", "", this );
}

void ioSkill::EndCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->RemoveBuff( m_CameraBuff );
}

bool ioSkill::UseActiveCount( ioBaseChar *pOwner )
{
	if( m_iMaxActiveCount > 0 )
		return true;

	return false;
}

int ioSkill::GetMaxActiveCount( ioBaseChar *pOwner )
{
	return m_iMaxActiveCount;
}

int ioSkill::GetCurActiveCount( ioBaseChar *pOwner )
{
	return m_iCurActiveCount;
}

bool ioSkill::HideHPGauge()
{
	return false;
}

bool ioSkill::CheckExceptionCamera()
{
	return false;
}

D3DXVECTOR3 ioSkill::GetExceptionCameraPos( ioBaseChar *pChar )
{
	if( !pChar )
		return ioMath::VEC3_ZERO;

	return pChar->GetWorldPosition();
}

bool ioSkill::IsFixedExceptionCamera()
{
	return false;
}

bool ioSkill::CheckUseActiveCount()
{
	return false;
}

void ioSkill::SetNextSkillState( ioBaseChar *pWounder , ioWeapon *pWeapon /* = NULL */ )
{
}

float ioSkill::GetIncSkillGaugeRate()
{
	return m_fPassiveSkillGaugeRate;
}

int ioSkill::GetObjEnableSkillType()
{
	return m_nEnableObjitemSkillType;
}

void ioSkill::ApplyExtraPassiveSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
}

int ioSkill::GetBuffType( int iBuffType /*= 0 */ )
{
	return -1;
}
