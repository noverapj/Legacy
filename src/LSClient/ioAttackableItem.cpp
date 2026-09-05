#include "stdafx.h"

#include "ioAttackableItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ItemDefine.h"

ioAttackableItem::ioAttackableItem()
{
	m_fJumpSpeedRate = FLOAT1;
	m_fJumpPowerRate = FLOAT1;
	m_fBlowGetupTimeRate = FLOAT1;
	m_fDefenseDamageRate = FLOAT1;
	m_bEquipedOnHand = false;
	m_dwItemFeature = 0;
	
	m_dwExtendDefenseWoundedTime = 0;

	m_pExtendDash = NULL;
	m_pExtendJump = NULL;
	m_pCounterAttack = NULL;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

	m_dwDefenseAttackEnd = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();

	m_bReserveJumpReload = false;
	m_iCurJumpReloadCnt = 0;
	m_dwJRComboEnable_S = 0;
	m_dwJRComboEnable_E = 0;
	m_JumpReloadInfoList.clear();

	m_bUsedWereWolf = false;
	m_dwChangeWereWolfTime = 0;

	ClearCancelInfo();

	// air_jump
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();

	//
	m_fCurExtraGauge = 0.0f;
}

ioAttackableItem::ioAttackableItem( const ioAttackableItem &rhs )
: ioItem( rhs ),
  m_ExtendChargeType( rhs.m_ExtendChargeType ),
  m_FormalAttack( rhs.m_FormalAttack ),
  m_Priority( rhs.m_Priority ),
  m_DelayAnimation( rhs.m_DelayAnimation ),
  m_RunAnimation( rhs.m_RunAnimation ),
  m_DashRunAnimation( rhs.m_DashRunAnimation ),
  m_JumpReadyAnimation( rhs.m_JumpReadyAnimation ),
  m_JumppingAnimation( rhs.m_JumppingAnimation ),
  m_JumpEndAnimation( rhs.m_JumpEndAnimation ),
  m_fWeakAttackPushRate( rhs.m_fWeakAttackPushRate ),
  m_fDashSpeedRate( rhs.m_fDashSpeedRate ),
  m_fDashAttackSpeedRate( rhs.m_fDashAttackSpeedRate ),
  m_fDashEndSpeedRate( rhs.m_fDashEndSpeedRate ),
  m_fDefenseSpeedRate( rhs.m_fDefenseSpeedRate ),
  m_fAttackSpeedRate( rhs.m_fAttackSpeedRate ),
  m_DashType( rhs.m_DashType ),
  m_JumpType( rhs.m_JumpType ),
  m_bJumpAttack( rhs.m_bJumpAttack ),
  m_bJump_D_Attack( rhs.m_bJump_D_Attack ),
  m_bRotateJumpAttack( rhs.m_bRotateJumpAttack ),
  m_fJumpNeedGauge( rhs.m_fJumpNeedGauge ),
  m_fJumpExtraGauge( rhs.m_fJumpExtraGauge ),
  m_dwExtendDefenseCheckTime( rhs.m_dwExtendDefenseCheckTime ),
  m_ExtendJumpEndAnimation( rhs.m_ExtendJumpEndAnimation ),
  m_fExtendLandingRate( rhs.m_fExtendLandingRate ),
  m_fJumpLandingRate( rhs.m_fJumpLandingRate ),
  m_fJumpAttackEnableHeight( rhs.m_fJumpAttackEnableHeight ),
  m_dwJumpAttackEnableTime( rhs.m_dwJumpAttackEnableTime ),
  m_bGaugeUpdateChargeAttack( rhs.m_bGaugeUpdateChargeAttack ),
  m_bGaugeUpdateExtendDefense( rhs.m_bGaugeUpdateExtendDefense ),
  m_DefenseEffect( rhs.m_DefenseEffect ),
  m_bEnableExtendDefenseEffect( rhs.m_bEnableExtendDefenseEffect ),
  m_DefenseAnimation( rhs.m_DefenseAnimation ),
  m_DefenseWeakWoundedAni( rhs.m_DefenseWeakWoundedAni ),
  m_DefenseStrongWoundedAni( rhs.m_DefenseStrongWoundedAni ),
  m_DefenseSkillWoundedAni( rhs.m_DefenseSkillWoundedAni ),
  m_DefenseWeakAttackPushBuff( rhs.m_DefenseWeakAttackPushBuff ),
  m_bDefenseBackSide( rhs.m_bDefenseBackSide ),
  m_bDisableWoundDefense( rhs.m_bDisableWoundDefense ),
  m_fDefenseWeakWoundedRate( rhs.m_fDefenseWeakWoundedRate ),
  m_fDefenseStrongWoundedRate( rhs.m_fDefenseStrongWoundedRate ),
  m_fDefenseSkillWoundedRate( rhs.m_fDefenseSkillWoundedRate ),
  m_fLandAttackEnableGravity( rhs.m_fLandAttackEnableGravity ),
  m_fJumpSpeedRate( rhs.m_fJumpSpeedRate ),
  m_fBackJumpSpeedRate( rhs.m_fBackJumpSpeedRate ),
  m_ExtendJumppingAnimation( rhs.m_ExtendJumppingAnimation ),
  m_dwDodgeDefenseCheckTime( rhs.m_dwDodgeDefenseCheckTime ),
  m_dwDefenseAttackCheckTime( rhs.m_dwDefenseAttackCheckTime ),
  m_fJumpDefensePushRate( rhs.m_fJumpDefensePushRate ),
  m_bExtendDefenseDash( rhs.m_bExtendDefenseDash ),
  m_bExtendDefenseJump( rhs.m_bExtendDefenseJump ),
  m_bEnableJumpReload( rhs.m_bEnableJumpReload ),
  m_bChargeCharRotate( rhs.m_bChargeCharRotate ),
  m_bChargeCharDash( rhs.m_bChargeCharDash ),  
  m_bChargeCharDefense( rhs.m_bChargeCharDefense ),
  m_bChargeCharJump( rhs.m_bChargeCharJump ),
  m_bChargeAutoTarget( rhs.m_bChargeAutoTarget ),
  m_bNewJumpAttackState( rhs.m_bNewJumpAttackState ),
  m_fNewJumpAttackGravity( rhs.m_fNewJumpAttackGravity ),
  m_bDefenseRoate( rhs.m_bDefenseRoate ),
  m_bDefenseMove( rhs.m_bDefenseMove ),
  m_DefenseAttack( rhs.m_DefenseAttack ),
  m_dwMaxChargeTime( rhs.m_dwMaxChargeTime ),
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
  m_fMaxWoundedTimeRate( rhs.m_fMaxWoundedTimeRate ),
  m_fMaxLiveTimeRate( rhs.m_fMaxLiveTimeRate ),
  m_bExtraDashAttack( rhs.m_bExtraDashAttack ),
  m_DashAttackEndDash( rhs.m_DashAttackEndDash ),
  m_bExtraOtherDashAttack( rhs.m_bExtraOtherDashAttack ),
  m_JumpReloadInfoList( rhs.m_JumpReloadInfoList ),
  m_bJumpLandEndDash( rhs.m_bJumpLandEndDash ),
  m_bEnableWereWolf( rhs.m_bEnableWereWolf ),
  m_ChangeWereWolfAttack( rhs.m_ChangeWereWolfAttack ),
  m_ReleaseWereWolfAttack( rhs.m_ReleaseWereWolfAttack ),
  m_vWereWolfMeshList( rhs.m_vWereWolfMeshList ),
  m_vWereWolfMeshListW( rhs.m_vWereWolfMeshListW ),
  m_vWereWolfBuffList( rhs.m_vWereWolfBuffList ),
  m_vReleaseWereWolfBuffList( rhs.m_vReleaseWereWolfBuffList ),
  m_dwWereWolfObjectItem( rhs.m_dwWereWolfObjectItem ),
  m_fAutoTargetRange( rhs.m_fAutoTargetRange ),
  m_fAutoTargetMinAngle( rhs.m_fAutoTargetMinAngle ),
  m_fAutoTargetMaxAngle( rhs.m_fAutoTargetMaxAngle ),
  m_fDashAutoTargetRange( rhs.m_fDashAutoTargetRange ),
  m_fDashAutoTargetMinAngle( rhs.m_fDashAutoTargetMinAngle ),
  m_fDashAutoTargetMaxAngle( rhs.m_fDashAutoTargetMaxAngle ),
  m_fJumpAutoTargetRange( rhs.m_fJumpAutoTargetRange ),
  m_fJumpAutoTargetMinAngle( rhs.m_fJumpAutoTargetMinAngle ),
  m_fJumpAutoTargetMaxAngle( rhs.m_fJumpAutoTargetMaxAngle ),
  m_fCountAutoTargetRange( rhs.m_fCountAutoTargetRange ),
  m_fCountAutoTargetMinAngle( rhs.m_fCountAutoTargetMinAngle ),
  m_fCountAutoTargetMaxAngle( rhs.m_fCountAutoTargetMaxAngle ),
  m_bEnableSummonDummyChar( rhs.m_bEnableSummonDummyChar ),
  m_dwSummonDummyCharGapTime( rhs.m_dwSummonDummyCharGapTime ),
  m_SummonDummyCharMap( rhs.m_SummonDummyCharMap ),
  m_fAirForcePower( rhs.m_fAirForcePower ),
  m_fAirJumpPower( rhs.m_fAirJumpPower ),
  m_fExtraGaugeAttack( rhs.m_fExtraGaugeAttack ),
  m_fExtraGaugeAttackDef( rhs.m_fExtraGaugeAttackDef ),
  m_fExtraGaugeWound( rhs.m_fExtraGaugeWound ),
  m_fExtraGaugeWoundDef( rhs.m_fExtraGaugeWoundDef ),
  m_fExtraGaugeAttackDec( rhs.m_fExtraGaugeAttackDec ),
  m_fExtraGaugeAttackDefDec( rhs.m_fExtraGaugeAttackDefDec ),
  m_fExtraGaugeWoundDec( rhs.m_fExtraGaugeWoundDec ),
  m_fExtraGaugeWoundDefDec( rhs.m_fExtraGaugeWoundDefDec ),
  m_fExtraGaugeBaseInc( rhs.m_fExtraGaugeBaseInc ),
  m_fExtraGaugeBaseDec( rhs.m_fExtraGaugeBaseDec ),
  m_fMaxExtraGauge( rhs.m_fMaxExtraGauge ),
  m_stDefenceCounterDelayAni( rhs.m_stDefenceCounterDelayAni ),
  m_fDefenceCounterDelayAniRate( rhs.m_fDefenceCounterDelayAniRate ),
  m_DefenceCounterAttack( rhs.m_DefenceCounterAttack ),
  m_bEnableJumpLandingExtraState( rhs.m_bEnableJumpLandingExtraState ),
  m_vCounterAttackBuff( rhs.m_vCounterAttackBuff )
{
	m_fJumpPowerRate = rhs.m_fJumpPowerRate;
	m_dwDefensePreDuration = rhs.m_dwDefensePreDuration;
	m_dwDefenseEndDuration = rhs.m_dwDefenseEndDuration;
	m_dwDefenseDuration = rhs.m_dwDefenseDuration;
	m_dwDefenseDurationEx = rhs.m_dwDefenseDurationEx;
	m_fBlowGetupTimeRate = rhs.m_fBlowGetupTimeRate;
	m_fDefenseDamageRate = rhs.m_fDefenseDamageRate;
	m_bEquipedOnHand = false;

	if( rhs.m_pExtendDash )
		m_pExtendDash = rhs.m_pExtendDash->Clone();
	else
		m_pExtendDash = NULL;

	if( rhs.m_pExtendJump )
		m_pExtendJump = rhs.m_pExtendJump->Clone();
	else
		m_pExtendJump = NULL;

	if( rhs.m_pCounterAttack )
		m_pCounterAttack = rhs.m_pCounterAttack->Clone();
	else
		m_pCounterAttack = NULL;

	m_dwExtendDefenseWoundedTime = 0;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

	m_dwDefenseAttackEnd = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_bReserveJumpReload = false;
	m_iCurJumpReloadCnt = 0;
	m_dwJRComboEnable_S = 0;
	m_dwJRComboEnable_E = 0;

	m_bUsedWereWolf = false;
	m_dwChangeWereWolfTime = 0;

	ClearCancelInfo();

	// air_jump
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();

	//
	m_fCurExtraGauge = 0.0f;
}

ioAttackableItem::~ioAttackableItem()
{
	SAFEDELETE( m_pExtendDash );
	SAFEDELETE( m_pExtendJump );
	SAFEDELETE( m_pCounterAttack );

	m_vAniRotateInfoList.clear();
	m_JumpReloadInfoList.clear();
	m_vAirJumpTimeList.clear();

	m_SummonDummyCharMap.clear();
}

void ioAttackableItem::LoadProperty( ioINILoader &rkLoader )
{
	ioItem::LoadProperty( rkLoader );

	m_FormalAttack.LoadProperty( rkLoader, this );
	m_Priority.LoadPriority( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	//
	m_ExtendChargeType = (ExtendChargeType)rkLoader.LoadInt_e( "extend_charge_type", ECT_NONE );

	// Gathering
	m_dwMaxChargeTime = rkLoader.LoadInt_e( "gathering_max_time", 0 );
	m_fMaxDamageRate = rkLoader.LoadFloat_e( "gathering_max_damage_rate", FLOAT1 );
	m_fMaxForceRate = rkLoader.LoadFloat_e( "gathering_max_force_rate", FLOAT1 );
	m_fMaxBlowRate = rkLoader.LoadFloat_e( "gathering_max_blow_rate", FLOAT1 );

	m_fMaxRangeRate = rkLoader.LoadFloat_e( "gathering_max_range_rate", FLOAT1 );
	m_fMaxSpeedRate = rkLoader.LoadFloat_e( "gathering_max_speed_rate", FLOAT1 );
	m_fMaxChargeAniRate = rkLoader.LoadFloat_e( "gathering_max_ani_rate", 0.0f );
	m_fMaxForceSlidingRate = rkLoader.LoadFloat_e( "gathering_max_force_sliding_rate", 0.0f );

	m_fMaxTargetRangeRate = rkLoader.LoadFloat_e( "gathering_max_target_range_rate", 0.0f );
	m_fMaxTargetAngleRate = rkLoader.LoadFloat_e( "gathering_max_target_angle_rate", 0.0f );

	m_fMaxFloatRate = rkLoader.LoadFloat_e( "gathering_max_float_rate", 0.0f );

	m_fMaxScaleRate = rkLoader.LoadFloat_e( "gathering_max_scale_rate", 0.0f );
	m_fMaxExplosionRate = rkLoader.LoadFloat_e( "gathering_max_explosion_rate", 0.0f );
	m_fMaxWoundedTimeRate = rkLoader.LoadFloat_e( "gathering_max_wounded_time_rate", FLOAT1 );
	m_fMaxLiveTimeRate = rkLoader.LoadFloat_e( "gathering_max_livetime_rate", FLOAT1 );

	//
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );
	m_DelayAnimation = szBuf;
	rkLoader.LoadString_e( "run_animation", "", szBuf, MAX_PATH );
	m_RunAnimation = szBuf;
	rkLoader.LoadString_e( "dash_animation", "", szBuf, MAX_PATH );
	m_DashRunAnimation = szBuf;

	// 방어
	m_bEnableExtendDefenseEffect = rkLoader.LoadBool_e( "enable_extend_defense_effect", false );
	rkLoader.LoadString_e( "defense_effect", "", szBuf, MAX_PATH );
	m_DefenseEffect = szBuf;
	rkLoader.LoadString_e( "defense_animation", "", szBuf, MAX_PATH );
	m_DefenseAnimation = szBuf;
	rkLoader.LoadString_e( "defense_weak_wounded_ani", "", szBuf, MAX_PATH );
	m_DefenseWeakWoundedAni = szBuf;
	rkLoader.LoadString_e( "defense_strong_wounded_ani", "", szBuf, MAX_PATH );
	m_DefenseStrongWoundedAni = szBuf;
	rkLoader.LoadString_e( "defense_skill_wounded_ani", "", szBuf, MAX_PATH );
	m_DefenseSkillWoundedAni = szBuf;

	rkLoader.LoadString_e( "defense_weak_attack_push_buff", "", szBuf, MAX_PATH );
	m_DefenseWeakAttackPushBuff = szBuf;

	m_fDefenseWeakWoundedRate = rkLoader.LoadFloat_e( "defense_weak_wounded_rate", FLOAT1 );
	m_fDefenseStrongWoundedRate = rkLoader.LoadFloat_e( "defense_strong_wounded_rate", FLOAT1 );
	m_fDefenseSkillWoundedRate = rkLoader.LoadFloat_e( "defense_skill_wounded_rate", FLOAT1 );

	m_dwDefensePreDuration = rkLoader.LoadInt_e( "defense_pre_duration", 50 );
	m_dwDefenseEndDuration = rkLoader.LoadInt_e( "defense_end_duration", 100 );
	m_dwDefenseDuration = rkLoader.LoadInt_e( "defense_duration", 300 );
	//HARDCODE: 기본 방어 시간보다 유지방어 시간이 짧아지지 않게 처리.(설정하지 않은 경우)
	m_dwDefenseDurationEx = rkLoader.LoadInt_e( "defense_duration_extend", m_dwDefenseDuration );

	m_bDefenseBackSide = rkLoader.LoadBool_e( "defense_backside", false );
	m_bDisableWoundDefense = rkLoader.LoadBool_e( "disable_wound_defense", false );

	//common
	rkLoader.LoadString_e( "jump_ready_animation", "", szBuf, MAX_PATH );
	m_JumpReadyAnimation = szBuf;
	rkLoader.LoadString_e( "jumpping_animation", "", szBuf, MAX_PATH );
	m_JumppingAnimation = szBuf;
	rkLoader.LoadString_e( "jump_end_animation", "", szBuf, MAX_PATH );
	m_JumpEndAnimation = szBuf;
	rkLoader.LoadString_e( "extend_jumpping_animation", "", szBuf, MAX_PATH );
	m_ExtendJumppingAnimation = szBuf;
	rkLoader.LoadString_e( "extend_jump_end_animation", "", szBuf, MAX_PATH );
	m_ExtendJumpEndAnimation = szBuf;

	m_bGaugeUpdateChargeAttack = rkLoader.LoadBool_e( "gauge_update_charge_attack", false );
	m_bGaugeUpdateExtendDefense = rkLoader.LoadBool_e( "gauge_update_extend_defense", false );

	m_fJumpLandingRate = rkLoader.LoadFloat_e( "jump_landing_rate", 0.0f );
	m_fExtendLandingRate = rkLoader.LoadFloat_e( "extend_landing_rate", FLOAT1 );
	m_fLandAttackEnableGravity = rkLoader.LoadFloat_e( "land_jump_attack_enable_gravity", 0.0f );

	m_fJumpSpeedRate = rkLoader.LoadFloat_e( "jump_speed_rate", FLOAT1 );
	m_fBackJumpSpeedRate = rkLoader.LoadFloat_e( "jump_back_speed_rate", -FLOAT1 );
	m_fJumpPowerRate = rkLoader.LoadFloat_e( "jump_power_rate", FLOAT1 );

	m_bJumpAttack = rkLoader.LoadBool_e( "jump_attack_enable", false );
	m_bJump_D_Attack = rkLoader.LoadBool_e( "jump_d_attack_enable", false );
	m_bRotateJumpAttack = rkLoader.LoadBool_e( "rotate_jump_attack_enable", false );

	m_fJumpNeedGauge = rkLoader.LoadFloat_e( "jump_need_gauge", 0.0f );
	m_fJumpExtraGauge = rkLoader.LoadFloat_e( "jump_extra_gauge", 0.0f );
	m_fJumpAttackEnableHeight = rkLoader.LoadFloat_e( "jump_attack_enable_height", 0.0f );
	m_dwJumpAttackEnableTime = rkLoader.LoadInt_e( "jump_attack_enable_time", 0 );

	m_fBlowGetupTimeRate = rkLoader.LoadFloat_e( "blow_getup_time_rate", FLOAT1 );
	m_fDefenseDamageRate = rkLoader.LoadFloat_e( "defense_damage_rate", FLOAT1 );
	m_fWeakAttackPushRate = rkLoader.LoadFloat_e( "weak_attack_push_rate", FLOAT1 );
	
	m_fDashSpeedRate = rkLoader.LoadFloat_e( "dash_run_max_speed_rate", 1.3f );
	m_fDashAttackSpeedRate = rkLoader.LoadFloat_e( "dash_attack_speed_rate", 0.0f );
	m_fDashEndSpeedRate = rkLoader.LoadFloat_e( "dash_end_speed_rate", FLOAT1 );

	m_fDefenseSpeedRate = rkLoader.LoadFloat_e( "defense_max_speed_rate", 0.0f );
	m_fAttackSpeedRate = rkLoader.LoadFloat_e( "attack_max_speed_rate", FLOAT1 );

	m_fJumpDefensePushRate = rkLoader.LoadFloat_e( "jump_defense_push_rate", FLOAT1 );

	if( rkLoader.LoadBool_e( "enable_counter_attack", false ) )
		m_dwItemFeature |= IF_CAN_COUNTER_ATTACK;

	if( rkLoader.LoadBool_e( "defense_weak_attack_push", false ) )
		m_dwItemFeature |= IF_DEFENSE_WEAK_ATTACK_PUSH;
	
	if( rkLoader.LoadBool_e( "defense_blow_attack_push", false ) )
		m_dwItemFeature |= IF_DEFENSE_BLOW_ATTACK_PUSH;

	if( rkLoader.LoadBool_e( "item_must_drop_in_blow_down", false ) )
		m_dwItemFeature |= IF_ITEM_MUST_DROP_IN_BLOW_DOWN;
	
	if( rkLoader.LoadBool_e( "has_extend_defense", false ) )
		m_dwItemFeature |= IF_HAS_EXTEND_DEFENSE;

	if( rkLoader.LoadBool_e( "has_skill_defense", false ) )
		m_dwItemFeature |= IF_HAS_SKILL_DEFENSE;

	if( rkLoader.LoadBool_e( "has_dodge_defense", false ) )
		m_dwItemFeature |= IF_HAS_DODGE_DEFENSE;

	if( rkLoader.LoadBool_e( "has_defense_attack", false ) )
		m_dwItemFeature |= IF_HAS_DEFENSE_ATTACK;

	m_dwExtendDefenseCheckTime = rkLoader.LoadInt_e( "extend_defense_check_time", 0 );
	m_dwDodgeDefenseCheckTime = rkLoader.LoadInt_e( "dodge_defense_check_time", 0 );
	m_dwDefenseAttackCheckTime = rkLoader.LoadInt_e( "defense_attack_check_time", 0 );

	m_pExtendJump = m_pCreator->CreateExtendJump( rkLoader );
	rkLoader.LoadString_e( "jump_type", "", szBuf, MAX_PATH );
	CheckJumpType( szBuf );

	m_pExtendDash = m_pCreator->CreateExtendDash( rkLoader );
	rkLoader.LoadString_e( "dash_type", "", szBuf, MAX_PATH );
	CheckDashType( szBuf );

	m_pCounterAttack = m_pCreator->CreateCounterAttack( rkLoader );

	m_bExtendDefenseDash = rkLoader.LoadBool_e( "extend_defense_dash", false );
	m_bExtendDefenseJump = rkLoader.LoadBool_e( "extend_defense_jump", false );

	m_bEnableJumpReload = rkLoader.LoadBool_e( "enable_jump_reload", false );

	//
	m_bChargeCharRotate = rkLoader.LoadBool_e( "enable_charge_rotate", false );
	m_bChargeCharDash = rkLoader.LoadBool_e( "enable_charge_dash", false );
	m_bChargeCharDefense = rkLoader.LoadBool_e( "enable_charge_defense", false );
	m_bChargeCharJump = rkLoader.LoadBool_e( "enable_charge_jump", false );	

	m_bChargeAutoTarget = rkLoader.LoadBool_e( "charge_auto_target", false );

	m_bNewJumpAttackState = rkLoader.LoadBool_e( "enable_new_jump_attack_state", false );
	m_fNewJumpAttackGravity = rkLoader.LoadFloat_e( "new_jump_attack_gravity", 0.0f );

	m_bDefenseRoate = rkLoader.LoadBool_e( "enable_defense_rotate", false );
	m_bDefenseMove = rkLoader.LoadBool_e( "enable_defense_move", false );

	m_bExtraDashAttack = rkLoader.LoadBool_e( "enable_extra_dash_attack", false );
	m_bExtraOtherDashAttack = rkLoader.LoadBool_e( "enable_extra_other_dash_attack", false );

	m_bJumpLandEndDash = rkLoader.LoadBool_e( "enable_jump_land_end_dash", false );
	m_bEnableJumpLandingExtraState = rkLoader.LoadBool_e( "enable_jump_land_extra_state", false );

	m_DashAttackEndDash = (DashAttEndType)rkLoader.LoadInt_e( "dash_attack_end_dash", DAET_NONE );

	//
	LoadAttackAttribute_e( "defense_attack", m_DefenseAttack, rkLoader );

	//
	m_JumpReloadInfoList.clear();
	int iJumpReloadCnt = rkLoader.LoadInt_e( "jump_reload_info_cnt", 0 );
	for( int i=0; i < iJumpReloadCnt; ++i )
	{
		JumpReloadInfo kInfo;

		wsprintf_e( szKey, "jump_reload%d_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_szReloadAni = szBuf;

		wsprintf_e( szKey, "jump_reload%d_ani_rate", i+1 );
		kInfo.m_fReloadAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "jump_reload%d_reload", i+1 );
		kInfo.m_iReloadCnt = rkLoader.LoadInt( szKey, 0 );

		m_JumpReloadInfoList.push_back( kInfo );
	}

	// werewolf
	m_bEnableWereWolf = rkLoader.LoadBool_e( "enable_change_werewolf", false );

	wsprintf_e( szBuf, "change_werewolf_attack" );
	LoadAttackAttribute( szBuf, m_ChangeWereWolfAttack, rkLoader );

	wsprintf_e( szBuf, "release_werewolf_attack" );
	LoadAttackAttribute( szBuf, m_ReleaseWereWolfAttack, rkLoader );

	m_vWereWolfMeshList.clear();
	m_vWereWolfMeshListW.clear();
	m_vWereWolfBuffList.clear();
	m_vReleaseWereWolfBuffList.clear();

	int iMeshCnt = rkLoader.LoadInt_e( "werewolf_mesh_cnt", 0 );
	for( int i=0; i < iMeshCnt; ++i )
	{
		wsprintf_e( szKey, "werewolf_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szMesh = szBuf;
		m_vWereWolfMeshList.push_back( szMesh );
	}

	int iMeshWCnt = rkLoader.LoadInt_e( "werewolf_female_mesh_cnt", 0 );
	for( int j=0; j < iMeshWCnt; ++j )
	{
		wsprintf_e( szKey, "werewolf_female_mesh%d", j+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szMesh = szBuf;
		m_vWereWolfMeshListW.push_back( szMesh );
	}

	int iBuffCnt = rkLoader.LoadInt_e( "werewolf_buff_cnt", 0 );
	for( int k=0; k < iBuffCnt; ++k )
	{
		wsprintf_e( szKey, "werewolf_buff%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_vWereWolfBuffList.push_back( szBuff );
	}

	int iReleaseBuffCnt = rkLoader.LoadInt_e( "release_werewolf_buff_cnt", 0 );
	for( int l=0; l < iReleaseBuffCnt; ++l )
	{
		wsprintf_e( szKey, "release_werewolf_buff%d", l+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_vReleaseWereWolfBuffList.push_back( szBuff );
	}

	m_dwWereWolfObjectItem = (DWORD)rkLoader.LoadInt_e( "werewolf_object_item", 0 );

	// counter attack buff
	int iCount	= rkLoader.LoadInt_e( "counter_attack_buff_cnt", 0 );
	for( int i = 0; i < iCount; i++ )
	{
		sprintf_s( szKey, "counter_attack_buff%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vCounterAttackBuff.push_back( szBuf );
	}

	// autotarget
	m_fAutoTargetRange = rkLoader.LoadFloat_e( "autotarget_range", 300.0f );
	m_fAutoTargetMinAngle = rkLoader.LoadFloat_e( "autotarget_min_angle", 60.0f );
	m_fAutoTargetMaxAngle = rkLoader.LoadFloat_e( "autotarget_max_angle", 150.0f );

	m_fDashAutoTargetRange = rkLoader.LoadFloat_e( "dash_autotarget_range", 0.0f );
	m_fDashAutoTargetMinAngle = rkLoader.LoadFloat_e( "dash_autotarget_min_angle", 0.0f );
	m_fDashAutoTargetMaxAngle = rkLoader.LoadFloat_e( "dash_autotarget_max_angle", 0.0f );

	m_fJumpAutoTargetRange = rkLoader.LoadFloat_e( "jump_autotarget_range", 0.0f );
	m_fJumpAutoTargetMinAngle = rkLoader.LoadFloat_e( "jump_autotarget_min_angle", 0.0f );
	m_fJumpAutoTargetMaxAngle = rkLoader.LoadFloat_e( "jump_autotarget_max_angle", 0.0f );

	m_fCountAutoTargetRange = rkLoader.LoadFloat_e( "count_autotarget_range", 0.0f );
	m_fCountAutoTargetMinAngle = rkLoader.LoadFloat_e( "count_autotarget_min_angle", 0.0f );
	m_fCountAutoTargetMaxAngle = rkLoader.LoadFloat_e( "count_autotarget_max_angle", 0.0f );

	LoadSummonDummyChar( rkLoader );

	// air_jump
	m_fAirJumpPower = rkLoader.LoadFloat_e( "jump_air_power", 0.0f );
	m_fAirForcePower = rkLoader.LoadFloat_e( "jump_air_force_power", 0.0f );

	// extra gauge
	m_fExtraGaugeAttack = rkLoader.LoadFloat_e( "extra_gauge_att_rate", 0.0f );
	m_fExtraGaugeAttackDef = rkLoader.LoadFloat_e( "extra_gauge_att_def_rate", 0.0f );
	m_fExtraGaugeWound = rkLoader.LoadFloat_e( "extra_gauge_wound_rate", 0.0f );
	m_fExtraGaugeWoundDef = rkLoader.LoadFloat_e( "extra_gauge_wound_def_rate", 0.0f );

	m_fExtraGaugeAttackDec = rkLoader.LoadFloat_e( "extra_gauge_att_dec_rate", 0.0f );
	m_fExtraGaugeAttackDefDec = rkLoader.LoadFloat_e( "extra_gauge_att_def_dec_rate", 0.0f );
	m_fExtraGaugeWoundDec = rkLoader.LoadFloat_e( "extra_gauge_wound_dec_rate", 0.0f );
	m_fExtraGaugeWoundDefDec = rkLoader.LoadFloat_e( "extra_gauge_wound_def_dec_rate", 0.0f );

	m_fExtraGaugeBaseInc = rkLoader.LoadFloat_e( "extra_gauge_base_inc_rate", 0.0f );
	m_fExtraGaugeBaseDec = rkLoader.LoadFloat_e( "extra_gauge_base_dec_rate", 0.0f );

	m_fMaxExtraGauge = rkLoader.LoadFloat_e( "max_extra_gauge", FLOAT100 );

	// defence counter attack
	rkLoader.LoadString_e( "defence_counter_delay_ani", "", szBuf, MAX_PATH );
	m_stDefenceCounterDelayAni = szBuf;
	m_fDefenceCounterDelayAniRate = rkLoader.LoadFloat_e( "defence_counter_delay_ani_rate", FLOAT1 );

	wsprintf_e( szBuf, "defence_counter_attack" );
	LoadAttackAttribute( szBuf, m_DefenceCounterAttack, rkLoader );
}

void ioAttackableItem::LoadSummonDummyChar( ioINILoader &rkLoader )
{
	m_SummonDummyCharMap.clear();
	m_bEnableSummonDummyChar = (bool)rkLoader.LoadBool_e( "enable_summon_dummychar", false );
	m_dwSummonDummyCharGapTime = (DWORD)rkLoader.LoadInt_e( "summon_dummychar_gap_time", 1000 );

	if( m_bEnableSummonDummyChar )
	{
		LoadSummonDummyCharInfo( rkLoader, SDT_DELAY );
		LoadSummonDummyCharInfo( rkLoader, SDT_RUN );
		LoadSummonDummyCharInfo( rkLoader, SDT_DASH );
		LoadSummonDummyCharInfo( rkLoader, SDT_DASH_ATT );
		LoadSummonDummyCharInfo( rkLoader, SDT_JUMP );
		LoadSummonDummyCharInfo( rkLoader, SDT_JUMP_ATT );
		LoadSummonDummyCharInfo( rkLoader, SDT_JUMP_LAND );
		LoadSummonDummyCharInfo( rkLoader, SDT_NEW_JUMP_ATT );
		LoadSummonDummyCharInfo( rkLoader, SDT_NORMAL_ATT );
		LoadSummonDummyCharInfo( rkLoader, SDT_EXTEND_ATT );
		LoadSummonDummyCharInfo( rkLoader, SDT_COUNTER );
	}
}

void ioAttackableItem::LoadSummonDummyCharInfo( ioINILoader &rkLoader, int iSubState )
{
	char szTitle[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	switch( iSubState )
	{
	case SDT_DELAY:
		wsprintf_e( szTitle, "delay_dummy_char" );
		break;
	case SDT_RUN:
		wsprintf_e( szTitle, "run_dummy_char" );
		break;
	case SDT_DASH:
		wsprintf_e( szTitle, "dash_dummy_char" );
		break;
	case SDT_DASH_ATT:
		wsprintf_e( szTitle, "dash_att_dummy_char" );
		break;
	case SDT_JUMP:
		wsprintf_e( szTitle, "jump_dummy_char" );
		break;
	case SDT_JUMP_ATT:
		wsprintf_e( szTitle, "jump_att_dummy_char" );
		break;
	case SDT_JUMP_LAND:
		wsprintf_e( szTitle, "jump_land_dummy_char" );
		break;
	case SDT_NEW_JUMP_ATT:
		wsprintf_e( szTitle, "new_jump_att_dummy_char" );
		break;
	case SDT_NORMAL_ATT:
		wsprintf_e( szTitle, "normal_att_dummy_char" );
		break;
	case SDT_EXTEND_ATT:
		wsprintf_e( szTitle, "extend_att_dummy_char" );
		break;
	case SDT_COUNTER:
		wsprintf_e( szTitle, "counter_dummy_char" );
		break;
	}

	wsprintf_e( szKey, "%s_cnt", szTitle );
	int iDummyCnt = rkLoader.LoadInt( szKey, 0 );
	if( iDummyCnt > 0 )
	{
		SummonDummyChar rkDummyInfo;

		wsprintf_e( szKey, "%s_gauge", szTitle );
		rkDummyInfo.m_iNeedGauge = rkLoader.LoadInt( szKey, 10 );
		rkDummyInfo.m_DummyCharList.clear();

		if( iDummyCnt > 0 )
		{
			for( int i=0; i < iDummyCnt; ++i )
			{
				DummyCharLoadInfo kLoadInfo;

				wsprintf_e( szKey, "%s%d_name", szTitle, i+1 );
				rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				
				kLoadInfo.m_DummyCharName = szBuf;

				wsprintf_e( szKey, "%s%d_offset", szTitle, i+1 );
				kLoadInfo.m_fDummyCharOffset = rkLoader.LoadFloat( szKey, 0.0f );

				wsprintf_e( szKey, "%s%d_side_offset", szTitle, i+1 );
				kLoadInfo.m_fDummyCharSideOffset = rkLoader.LoadFloat( szKey, 0.0f );

				wsprintf_e( szKey, "%s%d_start_angle", szTitle, i+1 );
				kLoadInfo.m_fDummyCharStartAngle = rkLoader.LoadFloat( szKey, 0.0f );

				rkDummyInfo.m_DummyCharList.push_back( kLoadInfo );
			}
		}

		m_SummonDummyCharMap.insert( SummonDummyCharMap::value_type( iSubState, rkDummyInfo ) );
	}
}

void ioAttackableItem::OnEquiped( ioBaseChar *pOwner )
{
	// 해킹 방지 : 강시모드가 아닌 상황에서 강시손톱을 끼고 있다면 강퇴 시킨다.
	if( ioPlayMode::GetModeType() != MT_GANGSI && pOwner && pOwner->GetCharName() == g_MyInfo.GetPublicID() )
	{
		if( GetItemCode() == 11011 || GetItemCode() == 11012 )
		{
			SP2Packet kPacket( CTPK_EXIT_ROOM );
			kPacket << EXIT_ROOM_QUICK_OUT << 0 << true;
			TCPNetwork::SendToServer( kPacket );

			return;
		}
	}

	ioItem::OnEquiped( pOwner );

	if( m_fBlowGetupTimeRate != FLOAT1 )
	{
		pOwner->ModifyGetUpTimeRate( m_fBlowGetupTimeRate );
	}
}

void ioAttackableItem::OnReleased( ioBaseChar *pOwner )
{
	if( m_fBlowGetupTimeRate != FLOAT1 )
	{
		pOwner->ModifyGetUpTimeRate( FLOAT1/m_fBlowGetupTimeRate );
	}

	OnEndExtendAttack( pOwner, true );
	OnEndExtendJump( pOwner );

	ioItem::OnReleased( pOwner );
}

void ioAttackableItem::OnEndExtendJump( ioBaseChar *pOwner )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->OnEndExtendJump( pOwner );
	}
}

bool ioAttackableItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	return true;
}

void ioAttackableItem::CallItemProcess( ioBaseChar* pOwner )
{

}

void ioAttackableItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	pWeapon->SetCreateItem( m_Name );
}

void ioAttackableItem::CheckChargeInfo( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( !pWeapon ) return;

	float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
	float fCurScaleRate, fCurExplosionRate, fCurWoundedTimeRate, fCurLiveTimeRate;

	fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
	fCurScaleRate = fCurExplosionRate = fCurWoundedTimeRate = fCurLiveTimeRate = FLOAT1;

	if( m_fCurChargeRate > 0.0f && !bJumpAttack )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );

		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );

		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );

		float fCurMaxWoundedTime = max( 0.0f, (m_fMaxWoundedTimeRate - FLOAT1) );
		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );

		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
		fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

		fCurRangeRate += fCurMaxRange * m_fCurChargeRate;
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRate;

		fCurFloatRate += fCurMaxFloat * m_fCurChargeRate;

		fCurScaleRate += fCurMaxScale * m_fCurChargeRate;
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRate;

		fCurWoundedTimeRate += fCurMaxWoundedTime * m_fCurChargeRate;
		fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );

		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );

		pWeapon->SetOwnerChargeScaleRate( fCurScaleRate );
		pWeapon->SetOwnerChargeExplosionRate( fCurExplosionRate );

		pWeapon->SetOwnerChargeWoundedTimeRate( fCurWoundedTimeRate );
		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
	}
}

bool ioAttackableItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	return false;
}

void ioAttackableItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
}

void ioAttackableItem::CheckRetreatState( ioBaseChar *pOwner )
{
}

void ioAttackableItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{

}

void ioAttackableItem::WeaponWounded( ioWeapon *pWeapon, ioBaseChar *pWoundChar )
{

}

void ioAttackableItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
}

bool ioAttackableItem::SetDodgeDefenseState( ioBaseChar *pOwner )
{
	return false;
}

void ioAttackableItem::CheckDodgeDefenseState( ioBaseChar *pOwner )
{
}

void ioAttackableItem::ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
}

bool ioAttackableItem::SetExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
		return m_pExtendDash->StartDash( pOwner );

	return false;
}

void ioAttackableItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		m_pExtendDash->ProcessDash( pOwner );
	}
}

void ioAttackableItem::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->CheckComboJumpAttack( pOwner );
	}
}

bool ioAttackableItem::SetFlashJumpAttack( ioBaseChar *pOwner )
{
	ioFlashJump *pFlash = ToFlashJump( m_pExtendJump );
	if( pFlash )
	{
		return pFlash->SetFlashJumpAttack( pOwner );
	}

	ioJoroExpansionJump *pJoroExpansionJump = ToJoroExpansionJump( m_pExtendJump );
	if( pJoroExpansionJump )
	{
		return pJoroExpansionJump->SetFlashJumpAttack( pOwner );
	}

	return false;
}

bool ioAttackableItem::SetComboFlashAttack( ioBaseChar *pOwner )
{
	ioComboFlashJump *pCFJump = ToComboFlashJump( m_pExtendJump );
	if( pCFJump )
	{
		return pCFJump->SetFlashJumpAttack( pOwner );
	}

	return false;
}

bool ioAttackableItem::SetPowerChargeJumpAttack( ioBaseChar *pOwner )
{
	ioPowerChargeJump *pPower = ToPowerChargeJump( m_pExtendJump );
	ioChargeFlashJump *pChargeFlash = ToChargeFlashJump( m_pExtendJump );

	if( pPower )
		return pPower->SetPowerChargeJumpAttack( pOwner );
	else if( pChargeFlash )
		return pChargeFlash->SetPowerChargeJumpAttack( pOwner );

	return false;
}

bool ioAttackableItem::SetCollectJumpAttack( ioBaseChar *pOwner )
{
	ioCollectJump *pCollect = ToCollectJump( m_pExtendJump );
	if( pCollect )
	{
		return pCollect->SetCollectJumpAttackState( pOwner );
	}

	return false;
}

void ioAttackableItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	pOwner->SetDefaultNormalAttack( iCurCombo );
}

void ioAttackableItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	pOwner->CheckDefaultNormalAttack();
}

bool ioAttackableItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	return false;
}

void ioAttackableItem::CheckJumpAttack( ioBaseChar *pOwner )
{
}

void ioAttackableItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
}

void ioAttackableItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
}

void ioAttackableItem::OnEndExtendDefense( ioBaseChar *pOwner )
{
}

void ioAttackableItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
}

void ioAttackableItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
}

void ioAttackableItem::FillItemGaugePacketHeader( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket << pOwner->GetCharName();
	rkPacket << pOwner->GetIndex();
	rkPacket << GetItemCode();
	rkPacket << GetItemCreateIndex();
}

void ioAttackableItem::FillNormalAttackExtraInfo( SP2Packet &rkPacket )
{
	rkPacket << GetItemCode();
	rkPacket << GetItemCreateIndex();
}

void ioAttackableItem::ApplyNormalAttackExtraInfo( SP2Packet &rkPacket )
{
}

bool ioAttackableItem::HasExtendDash() const
{
	if( m_pExtendDash )
		return true;

	return false;
}

ioExtendDash* ioAttackableItem::GetCurExtendDash()
{
	return m_pExtendDash;
}

ioExtendJump* ioAttackableItem::GetCurExtendJump()
{
	return m_pExtendJump;
}

bool ioAttackableItem::HasAniFile( AttackType eType, int iSubIdx ) const
{
	return m_FormalAttack.HasAttack( eType, iSubIdx );
}

bool ioAttackableItem::HasEndAniFile( AttackType eType, int iSubIdx ) const
{
	return m_FormalAttack.HasAttackEnd( eType, iSubIdx );
}

const AttackAttribute* ioAttackableItem::GetAttackAttribute( AttackType eType,
															 int iSubIdx ) const
{
	return m_FormalAttack.GetAttribute( eType, iSubIdx );
}

const ioHashString& ioAttackableItem::GetAniFile( AttackType eType, int iSubIdx ) const
{
	return m_FormalAttack.GetAniFile( eType, iSubIdx );
}

float ioAttackableItem::GetAttackAniRate( AttackType eType, int iSubIdx ) const
{
	return m_FormalAttack.GetAttackAniRate( eType, iSubIdx );
}

const ioHashString& ioAttackableItem::GetEndAniFile( AttackType eType, int iSubIdx ) const
{
	return m_FormalAttack.GetEndAniFile( eType, iSubIdx );
}

float ioAttackableItem::GetAttackEndAniRate( AttackType eType, int iSubIdx ) const
{
	return m_FormalAttack.GetAttackEndAniRate( eType, iSubIdx );
}

const WeaponInfoList& ioAttackableItem::GetAttackAttributeIdx( AttackType eType, int iSubIdx ) const
{
	return m_FormalAttack.GetAttackAttributeIdx( eType, iSubIdx );
}

const ForceInfoList& ioAttackableItem::GetForceInfoList( AttackType eType, int iSubIdx ) const
{
	return m_FormalAttack.GetForceInfoList( eType, iSubIdx );
}

DWORD ioAttackableItem::GetAttackPreDelay( AttackType eType, int iSubIdx ) const
{
	return m_FormalAttack.GetAttackPreDelay( eType, iSubIdx );
}

DWORD ioAttackableItem::GetAttackAnimationDelay( AttackType eType, int iSubIdx ) const
{
	return m_FormalAttack.GetAttackAnimationDelay( eType, iSubIdx );
}

int ioAttackableItem::GetPriority( StatePriorityType eType )
{
	if( COMPARE( eType, SPT_DEFENSE, MAX_SPT_TYPE ) )
		return m_Priority.GetPriority( eType );

	LOG.PrintTimeAndLog( 0, "ioAttackableItem::GetPriority - %s overflow(%d/%d)",
							GetName().c_str(), eType, MAX_SPT_TYPE );

	return -1;
}

void ioAttackableItem::SetPriority( StatePriorityType eType , int iValue)
{
	if( COMPARE( eType, SPT_DEFENSE, MAX_SPT_TYPE ) )
		m_Priority.SetPriority( eType, iValue );
	else
		LOG.PrintTimeAndLog( 0, "ioAttackableItem::SetPriority - %s overflow(%d/%d)",
	 	     					 GetName().c_str(), eType, MAX_SPT_TYPE );
}

int ioAttackableItem::GetMaxCombo() const
{
	return m_FormalAttack.GetNormalMaxCombo();
}

int ioAttackableItem::GetExtraDashAttackMax() const
{
	return m_FormalAttack.GetExtraDashAttackMax();
}

int ioAttackableItem::GetExtraOtherDashAttackMax() const
{
	return m_FormalAttack.GetExtraOtherDashAttackMax();
}

float ioAttackableItem::GetWeakAttackPushRate() const
{
	return m_fWeakAttackPushRate;
}

void ioAttackableItem::CheckJumpType( char* szJumpType )
{
	m_JumpType = JT_DEFAULT;

	if( !strcmp( "DEFENSE_JUMP", szJumpType ) )
		m_JumpType = JT_DEFENSE_ATTACK;
	else if( !strcmp( "HIGH_JUMP", szJumpType ) )
		m_JumpType = JT_HIGH_JUMP;
	else if( !strcmp( "LONG_JUMP", szJumpType ) )
		m_JumpType = JT_LONG_JUMP;
	else if( !strcmp( "DOUBLE_JUMP", szJumpType ) )
		m_JumpType = JT_DOUBLE_JUMP;
	else if( !strcmp( "AIM_JUMP", szJumpType ) )
		m_JumpType = JT_AIM_JUMP;
	else if( !strcmp( "AIM_JUMP2", szJumpType ) )
		m_JumpType = JT_AIM_JUMP2;
	else if( !strcmp( "COMBO_JUMP", szJumpType ) )
		m_JumpType = JT_COMBO_JUMP;
	else if( !strcmp( "LAND_ATTACK_JUMP", szJumpType ) )
		m_JumpType = JT_LAND_ATTACK_JUMP;
	else if( !strcmp( "AUTO_SHOT_JUMP", szJumpType ) )
		m_JumpType = JT_AUTO_SHOT_JUMP;
	else if( !strcmp( "FLASH_JUMP", szJumpType ) )
		m_JumpType = JT_FLASH_JUMP;
	else if( !strcmp( "COMBO_FLASH_JUMP", szJumpType ) )
		m_JumpType = JT_COMBO_FLASH;
	else if( !strcmp( "CHARGE_FLASH_JUMP", szJumpType ) )
		m_JumpType = JT_CHARGE_FLASH_JUMP;
	else if( !strcmp( "BADGUY_JUMP", szJumpType ) )
		m_JumpType = JT_BADGUY_JUMP;
	else if( !strcmp( "KISARAGI_JUMP", szJumpType ) )
		m_JumpType = JT_KISARAGI_JUMP;
	else if( !strcmp( "COLLECT_JUMP", szJumpType ) )
		m_JumpType = JT_COLLECT_JUMP;
	else if( !strcmp( "POWER_CHARGE_JUMP", szJumpType ) )
		m_JumpType = JT_POWER_CHARGE_JUMP;
	else if( !strcmp( "RAGNA_JUMP", szJumpType ) )
		m_JumpType = JT_RAGNA_JUMP;
	else if( !strcmp( "CHARGE_COMBO", szJumpType ) )
		m_JumpType = JT_CHARGE_COMBO;
	else if( !strcmp( "CHARGE_COMBO2", szJumpType ) )
		m_JumpType = JT_CHARGE_COMBO2;
	else if( !strcmp( "CHARGE_COMBO3", szJumpType ) )
		m_JumpType = JT_CHARGE_COMBO3;
	else if( !strcmp( "CHARGE_COMBO4", szJumpType ) )
		m_JumpType = JT_CHARGE_COMBO4;
	else if( !strcmp( "GATOTKACA_JUMP", szJumpType ) )
		m_JumpType = JT_GATOTKACA_JUMP;
	else if( !strcmp( "RACHEL_JUMP", szJumpType ) )
		m_JumpType = JT_RACHEL_JUMP;
	else if( !strcmp( "ILJIMAE_JUMP", szJumpType ) )
		m_JumpType = JT_ILJIMAE_JUMP;
	else if( !strcmp( "DESPERADO_JUMP", szJumpType ) )
		m_JumpType = JT_DESPERADO_JUMP;
	else if( !strcmp( "TRINITY_JUMP", szJumpType ) )
		m_JumpType = JT_TRINITY_JUMP;
	else if( !strcmp( "DOUBLE_SHOT_JUMP", szJumpType ) )
		m_JumpType = JT_DOUBLE_SHOT_JUMP;
	else if( !strcmp( "DOUBLE_JUMP2", szJumpType ) )
		m_JumpType = JT_DOUBLE_JUMP2;
	else if( !strcmp( "CHARGE_COMBO_DEFENSE_JUMP", szJumpType ) )
		m_JumpType = JT_CHARGE_COMBO_DEFENSE;
	else if( !strcmp( "HAKUMEN_JUMP", szJumpType ) )
		m_JumpType = JT_HAKUMEN_JUMP;
	else if( !strcmp( "SUDDEN_JUMP", szJumpType ) )
		m_JumpType = JT_SUDDEN_ATTACK_JUMP;
	else if( !strcmp( "TRACE_JUMP", szJumpType ) )
		m_JumpType = JT_TRACE_JUMP;
	else if( !strcmp( "LEE_JUMP", szJumpType ) )
		m_JumpType = JT_LEE_JUMP;
	else if( !strcmp( "PARKILPYO_JUMP", szJumpType ) )
		m_JumpType = JT_PARKILPYO_JUMP;
	else if( !strcmp( "JORO_EXPANSION_JUMP", szJumpType ) )
		m_JumpType = JT_JORO_EXPANSION_JUMP;
	else if( !strcmp( "DOUBLE_JUMP3", szJumpType ) )
		m_JumpType = JT_DOUBLE_JUMP3;
	else if( !strcmp( "ROBIN_EXPANSION_JUMP", szJumpType ) )
		m_JumpType = JT_ROBIN_EXPANSION_JUMP;
	else if( !strcmp( "SABER_JUMP", szJumpType ) )
		m_JumpType = JT_SABER_JUMP;
	else if( !strcmp( "NATA_JUMP", szJumpType ) )
		m_JumpType = JT_NATA_JUMP;
	else if( !strcmp( "GRAPPLER_EXPANSION_JUMP", szJumpType ) )
		m_JumpType = JT_GRAPPLING_EXPANSION_JUMP;
	else if ( !strcmp( "STRIDER_JUMP", szJumpType ) )
		m_JumpType = JT_STRIDER_JUMP;
	else if ( !strcmp( "URIEL_JUMP", szJumpType ) )
		m_JumpType = JT_URIEL_JUMP;
	else if ( !strcmp( "PHOENIX_JUMP", szJumpType ) )
		m_JumpType = JT_PHOENIX_JUMP;
	else if ( !strcmp( "TIMEGATE_JUMP", szJumpType ) )
		m_JumpType = JT_TIMEGATE_JUMP;
	else if ( !strcmp( "OMEN_JUMP", szJumpType ) )
		m_JumpType = JT_OMEN_JUMP;
	else if ( !strcmp( "POSEIDON_JUMP", szJumpType ) )
		m_JumpType = JT_POSEIDON_JUMP;
	else if ( !strcmp( "SKY_WALKER_JUMP", szJumpType ) )
		m_JumpType = JT_SKY_WALKER_JUMP;
	else if ( !strcmp( "GWONWANG_JUMP", szJumpType ) )		// 2020-06-03 권왕
		m_JumpType = JT_GWONWANG_JUMP;
	else if( !strcmp( "GENISYSARMY_JUMP", szJumpType ) )	// 2020-06-29 제네시스
		m_JumpType = JT_GENISYSARMY_JUMP;
	else if( !strcmp( "HERACLES_JUMP", szJumpType ) )
		m_JumpType = JT_HERACLES_JUMP;
	else if( !strcmp( "SIZECOMBO_JUMP", szJumpType ) )		// 2020-09-23 사이즈
		m_JumpType = JT_SIZECOMBO_JUMP;
	else if( !strcmp( "EVACOMBO_JUMP", szJumpType ) )		// 2020-10-19 이브
		m_JumpType = JT_EVACOMBO_JUMP;
	else if( !strcmp( "MAGNETIC_JUMP", szJumpType ) )		// 마그네틱 2020-11-25
		m_JumpType = JT_MAGNETIC_JUMP;
	else if ( !strcmp( "TAOIST_JUMP", szJumpType ) )
		m_JumpType = JT_TAOIST_JUMP;
}

void ioAttackableItem::CheckDashType( char* szDashType )
{
	m_DashType = DT_ATTACK;

	if( !strcmp( "DASH_ATTACK", szDashType ) )
		m_DashType = DT_ATTACK;
	else if( !strcmp( "SUPER_DASH", szDashType ) )
		m_DashType = DT_SUPER;
	else if( !strcmp( "ROLLING_DASH", szDashType ) )
		m_DashType = DT_ROLLING;
	else if( !strcmp( "DASH_SPEAR", szDashType ) )
		m_DashType = DT_SPEAR;
	else if( !strcmp( "ROLLING_AIM", szDashType ) )
		m_DashType = DT_ROLLING_AIM;
	else if( !strcmp( "ASSAULT_DASH", szDashType ) )
		m_DashType = DT_ASSAULT;
	else if( !strcmp( "DASH_AIM", szDashType ) )
		m_DashType = DT_DASH_AIM;
	else if( !strcmp( "SIMPLE_DASH", szDashType ) )
		m_DashType = DT_SIMPLE;
	else if( !strcmp( "POWER_CHARGE_DASH", szDashType ) )
		m_DashType = DT_POWER_CHARGE;
	else if( !strcmp( "EXTEND_ASSAULT_DASH", szDashType ) )
		m_DashType = DT_EXTEND_ASSAULT;
	else if( !strcmp( "POWER_CHARGE_DASH2", szDashType ) )
		m_DashType = DT_POWER_CHARGE2;
	else if( !strcmp( "POWER_CHARGE_DASH3", szDashType ) )
		m_DashType = DT_POWER_CHARGE3;
	else if( !strcmp( "SHAOLIN_DASH", szDashType ) )
		m_DashType = DT_SHAOLIN;
	else if( !strcmp( "ASSAULT_CHARGE_DASH", szDashType ) )
		m_DashType = DT_ASSAULT_CHARGE;
	else if( !strcmp( "CHARGE_COMBO_DASH", szDashType ) )
		m_DashType = DT_CHARGE_COMBO;
	else if( !strcmp( "RUN_ASSAULT_DASH", szDashType ) )
		m_DashType = DT_RUN_ASSAULT;
	else if( !strcmp( "DASH_ATTACK2", szDashType ) )
		m_DashType = DT_DASH_ATTACK2;
	else if( !strcmp( "NORMAL_COMBO_DASH", szDashType ) )
		m_DashType = DT_NORMAL_COMBO;
	else if( !strcmp( "SUDDEN_ATTACK_DASH", szDashType ) )
		m_DashType = DT_SUDDEN_ATTACK;
	else if( !strcmp( "TELEPORT_DASH", szDashType ) )
		m_DashType = DT_TELEPORT;
	else if( !strcmp( "FROZEN_COMBO_DASH", szDashType ) )	// 얼음신 2020-03-03 
		m_DashType = DT_FROZEN_COMBO;
	else if( !strcmp( "GENISYSARMY_DASH", szDashType ) )	// 제네시스 2020-06-25
		m_DashType = DT_GENISYSARMY;
	else if( !strcmp( "SIZE_COMBO_DASH", szDashType ) )		// 사이즈 2020-09-15
		m_DashType = DT_SIZE_COMBO;
	else if( !strcmp( "EVA_DASH", szDashType ) )			// 이브 2020-10-16
		m_DashType = DT_EVA;
	else if( !strcmp( "MAGNETIC_DASH", szDashType ) )		// 마그네틱 2020-11-25
		m_DashType = DT_MAGNETIC;
	else if( !strcmp( "TAOIST_DASH", szDashType ) )
		m_DashType = DT_TAOIST;
}

void ioAttackableItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
}

void ioAttackableItem::SetJumpState()
{
	if( m_pExtendJump )
	{
		m_pExtendJump->InitJump();
		m_pExtendJump->SetJumpState();
	}
}

void ioAttackableItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	float fJumpPower = pOwner->GetJumpPower() * m_fJumpPowerRate;
	pOwner->SetJumpPower( fJumpPower );

	if( m_pExtendJump )
	{
		m_pExtendJump->SetJumpping( pOwner, bFullTime );
	}
}

bool ioAttackableItem::HasJumpEndAnimation( bool bUseAttack, ioBaseChar *pOwner ) const
{
	if( !pOwner ) return false;

	ioLandAttackJump *pLandJump = ToLandAttackJump( m_pExtendJump );
	ioComboJump *pComboJump = ToComboJump( m_pExtendJump );
	ioChargeComboJump2 *pChargeCombo2 = ToChargeComboJump2( m_pExtendJump );
	ioChargeComboJump3 *pChargeCombo3 = ToChargeComboJump3( m_pExtendJump );
	ioChargeComboJump4 *pChargeCombo4 = ToChargeComboJump4( m_pExtendJump );
	ioJoroExpansionJump *pJoroExpansionJump = ToJoroExpansionJump( m_pExtendJump );
	ioPowerChargeJump *pPowerChargeJump = ToPowerChargeJump( m_pExtendJump );
	ioChargeFlashJump *pChargeFlashJump = ToChargeFlashJump( m_pExtendJump );
	ioChargeComboDefenseJump *pChargeComboDefense = ToChargeComboDefenseJump( m_pExtendJump );
	ioParkilpyoJump *pParkilpyoJump = ToParkilpyoJump( m_pExtendJump );
	ioIljimaeJump *pIljimaeJump = ToIljimaeJump( m_pExtendJump );
	ioMagneticJump *pMagneticJump = ToMagneticJump( m_pExtendJump );	// 마그네틱 2020-11-26
	ioTaoistJump *pTaoistJump = ToTaoistJump( m_pExtendJump );

	if( CheckEnableLandAttackByGravity( pOwner ) )
	{
		if( pLandJump && pLandJump->IsUsedHighJump() )
		{
			return !GetAniFile( AT_EXTEND_LAND_JUMP_ATTACK, 0 ).IsEmpty();
		}

		return !GetAniFile( AT_LAND_JUMP_ATTACK, 0 ).IsEmpty();
	}
	else if( pComboJump && pComboJump->IsComboDashState() )
	{
		return !pComboJump->GetComboDashLandAni().IsEmpty();
	}
	else if( pChargeCombo2 && pChargeCombo2->IsChargeAttackState() )
	{
		return !pChargeCombo2->GetChargeLandAni().IsEmpty();
	}
	else if( pChargeCombo3 && pChargeCombo3->IsChargeAttackState() )
	{
		return !pChargeCombo3->GetChargeLandAni().IsEmpty();
	}
	else if( pChargeCombo4 && pChargeCombo4->IsChargeAttackState() )
	{
		return !pChargeCombo4->GetChargeLandAni(pOwner).IsEmpty();
	}
	else if( pJoroExpansionJump && pJoroExpansionJump->IsChargeAttackState() )
	{
		return !pJoroExpansionJump->GetChargeLandAni().IsEmpty();
	}
	else if( pChargeComboDefense && pChargeComboDefense->IsChargeAttackState() )
	{
		return !pChargeComboDefense->GetChargeLandAni().IsEmpty();
	}
	else if( pPowerChargeJump && pPowerChargeJump->IsDashState() )
	{
		return !pPowerChargeJump->GetDashLandAni().IsEmpty();
	}
	else if( pChargeFlashJump && pChargeFlashJump->IsDashState() )
	{
		return !pChargeFlashJump->GetDashLandAni().IsEmpty();
	}
	else if( pParkilpyoJump && pParkilpyoJump->IsChargeAttackState() )
	{
		return !pParkilpyoJump->GetChargeLandAni().IsEmpty();
	}
	else if( pIljimaeJump )
	{
		if( pIljimaeJump->IsChargeAttacked() )
			return !pIljimaeJump->GetChargeLandAni().IsEmpty();
		if( pIljimaeJump->IsComboDash() )
			return !pIljimaeJump->GetComboDashLandAni().IsEmpty();
	}
	else if( pMagneticJump && pMagneticJump->IsChargeAttackState() )	// 마그네틱 2020-12-03 
	{
		return !pMagneticJump->GetChargeLandAni( pOwner ).IsEmpty();
	}
	else if( pTaoistJump && pTaoistJump->IsChargeAttackState() )
	{
		return !pTaoistJump->GetChargeLandAni( pOwner ).IsEmpty();
	}

	if( m_pExtendJump )
	{
		if( m_pExtendJump->CheckUseExtendJump( pOwner ) || bUseAttack )
			return !m_ExtendJumpEndAnimation.IsEmpty();
	}
	else
	{
		if( bUseAttack )
			return !m_ExtendJumpEndAnimation.IsEmpty();
	}

	return !m_JumpEndAnimation.IsEmpty();
}

const ioHashString& ioAttackableItem::GetJumpEndAnimation( bool bUseAttack, ioBaseChar *pOwner ) const
{
	if( !pOwner ) return m_JumpEndAnimation;

	ioLandAttackJump *pLandJump = ToLandAttackJump( m_pExtendJump );
	ioGrapplingExpansionJump *pGrapplingExpansionJump = ToGrapplingExpansionJump( m_pExtendJump );
	ioComboJump *pComboJump = ToComboJump( m_pExtendJump );
	ioChargeComboJump2 *pChargeCombo2 = ToChargeComboJump2( m_pExtendJump );
	ioChargeComboJump3 *pChargeCombo3 = ToChargeComboJump3( m_pExtendJump );
	ioChargeComboJump4 *pChargeCombo4 = ToChargeComboJump4( m_pExtendJump );
	ioJoroExpansionJump *pJoroExpansionJump = ToJoroExpansionJump( m_pExtendJump );
	ioPowerChargeJump *pPowerChargeJump = ToPowerChargeJump( m_pExtendJump );
	ioChargeFlashJump *pChargeFlashJump = ToChargeFlashJump( m_pExtendJump );
	ioChargeComboDefenseJump *pChargeComboDefense = ToChargeComboDefenseJump( m_pExtendJump );
	ioSuddenAttackJump *pSuddenJump = ToSuddenAttackJump( m_pExtendJump );
	ioParkilpyoJump *pParkilpyoJump = ToParkilpyoJump( m_pExtendJump );
	ioIljimaeJump *pIljimaeJump = ToIljimaeJump( m_pExtendJump );
	ioMagneticJump *pMagneticJump = ToMagneticJump( m_pExtendJump );	// 마그네틱 2020-11-26
	ioTaoistJump *pTaoistJump = ToTaoistJump( m_pExtendJump );

	if( CheckEnableLandAttackByGravity( pOwner ) )
	{
		if( pLandJump && pLandJump->IsUsedHighJump() )
		{
			return GetAniFile( AT_EXTEND_LAND_JUMP_ATTACK, 0 );
		}

		if( pGrapplingExpansionJump && pGrapplingExpansionJump->IsUsedHighJump() )
		{
			return GetAniFile( AT_EXTEND_LAND_JUMP_ATTACK, 0 );
		}

		return GetAniFile( AT_LAND_JUMP_ATTACK, 0 );
	}
	else if( pComboJump && pComboJump->IsComboDashState() )
	{
		return pComboJump->GetComboDashLandAni();
	}
	else if( pChargeCombo2 && pChargeCombo2->IsChargeAttackState() )
	{
		return pChargeCombo2->GetChargeLandAni();
	}
	else if( pChargeCombo3 && pChargeCombo3->IsJumpDashState() )
	{
		return pChargeCombo3->GetJumpDashLandAni();
	}
	else if( pChargeCombo3 && pChargeCombo3->IsChargeAttackState() )
	{
		return pChargeCombo3->GetChargeLandAni();
	}
	else if( pChargeCombo4 && pChargeCombo4->IsChargeAttackState() )
	{
		return pChargeCombo4->GetChargeLandAni(pOwner);
	}
	else if( pJoroExpansionJump && pJoroExpansionJump->IsChargeAttackState() )
	{
		return pJoroExpansionJump->GetChargeLandAni();
	}
	else if( pSuddenJump && pSuddenJump->IsJumpDashState() )
	{
		return pSuddenJump->GetJumpDashLandAni();
	}
	else if( pChargeComboDefense && pChargeComboDefense->IsChargeAttackState() )
	{
		return pChargeComboDefense->GetChargeLandAni();
	}
	else if( pPowerChargeJump && pPowerChargeJump->IsDashState() )
	{
		return pPowerChargeJump->GetDashLandAni();
	}
	else if( pChargeFlashJump && pChargeFlashJump->IsDashState() )
	{
		return pChargeFlashJump->GetDashLandAni();
	}
	else if( pParkilpyoJump && pParkilpyoJump->IsChargeAttackState() )
	{
		return pParkilpyoJump->GetChargeLandAni();
	}
	else if( pIljimaeJump )
	{
		if( pIljimaeJump->IsChargeAttacked() )
			return pIljimaeJump->GetChargeLandAni();
		if( pIljimaeJump->IsComboDash() )
			return pIljimaeJump->GetComboDashLandAni();
	}
	else if( pMagneticJump && pMagneticJump->IsChargeAttackState() )	// 마그네틱 2020-12-03
	{
		return pMagneticJump->GetChargeLandAni( pOwner );
	}
	else if( pTaoistJump && pTaoistJump->IsChargeAttackState() )
	{
		return pTaoistJump->GetChargeLandAni( pOwner );
	}

	if( m_pExtendJump )
	{
		if( m_pExtendJump->CheckUseExtendJump( pOwner ) || bUseAttack )
			return m_ExtendJumpEndAnimation;
	}
	else
	{
		if( bUseAttack )
			return m_ExtendJumpEndAnimation;
	}

	return m_JumpEndAnimation;
}

float ioAttackableItem::GetExtendLandingRate( bool bUseAttack, ioBaseChar *pOwner ) const
{
	if( !pOwner ) return 0.0f;

	ioLandAttackJump *pLandJump = ToLandAttackJump( m_pExtendJump );
	ioComboJump *pComboJump = ToComboJump( m_pExtendJump );
	ioChargeComboJump2 *pChargeCombo2 = ToChargeComboJump2( m_pExtendJump );
	ioChargeComboJump3 *pChargeCombo3 = ToChargeComboJump3( m_pExtendJump );
	ioChargeComboJump4 *pChargeCombo4 = ToChargeComboJump4( m_pExtendJump );
	ioJoroExpansionJump *pJoroExpansionJump = ToJoroExpansionJump( m_pExtendJump );
	ioPowerChargeJump *pPowerChargeJump = ToPowerChargeJump( m_pExtendJump );
	ioChargeFlashJump *pChargeFlashJump = ToChargeFlashJump( m_pExtendJump );
	ioChargeComboDefenseJump *pChargeComboDefense = ToChargeComboDefenseJump( m_pExtendJump );
	ioSuddenAttackJump *pSuddenJump = ToSuddenAttackJump( m_pExtendJump ); 
	ioParkilpyoJump *ParkilpyoJump = ToParkilpyoJump( m_pExtendJump );
	ioIljimaeJump *pIljimaeJump = ToIljimaeJump( m_pExtendJump );
	ioMagneticJump *pMagneticJump = ToMagneticJump( m_pExtendJump );	// 마그네틱 2020-11-26
	ioTaoistJump *pTaoistJump = ToTaoistJump( m_pExtendJump );

	if( CheckEnableLandAttackByGravity( pOwner ) )
	{
		if( pLandJump && pLandJump->IsUsedHighJump() )
		{
			return GetAttackAniRate( AT_EXTEND_LAND_JUMP_ATTACK, 0 );
		}

		return GetAttackAniRate( AT_LAND_JUMP_ATTACK, 0 );
	}
	else if( pComboJump && pComboJump->IsComboDashState() )
	{
		return pComboJump->GetComboDashLandAniRate();
	}
	else if( pChargeCombo2 && pChargeCombo2->IsChargeAttackState() )
	{
		return pChargeCombo2->GetChargeLandAniRate();
	}
	else if( pChargeCombo3 && pChargeCombo3->IsJumpDashState() )
	{
		return pChargeCombo3->GetJumpDashLandAniRate();
	}
	else if( pChargeCombo3 && pChargeCombo3->IsChargeAttackState() )
	{
		return pChargeCombo3->GetChargeLandAniRate();
	}
	else if( pChargeCombo4 && pChargeCombo4->IsChargeAttackState() )
	{
		return pChargeCombo4->GetChargeLandAniRate(pOwner);
	}
	else if( pJoroExpansionJump && pJoroExpansionJump->IsChargeAttackState() )
	{
		return pJoroExpansionJump->GetChargeLandAniRate();
	}
	else if( pSuddenJump && pSuddenJump->IsJumpDashState() )
	{
		return pSuddenJump->GetJumpDashLandAniRate();
	}
	else if( pChargeComboDefense && pChargeComboDefense->IsChargeAttackState() )
	{
		return pChargeComboDefense->GetChargeLandAniRate();
	}
	else if( pPowerChargeJump && pPowerChargeJump->IsDashState() )
	{
		return pPowerChargeJump->GetDashLandAniRate();
	}
	else if( pChargeFlashJump && pChargeFlashJump->IsDashState() )
	{
		return pChargeFlashJump->GetDashLandAniRate();
	}
	else if( ParkilpyoJump && ParkilpyoJump->IsChargeAttackState() )
	{
		return ParkilpyoJump->GetChargeLandAniRate();
	}
	else if( pIljimaeJump )
	{
		if( pIljimaeJump->IsChargeAttacked() )
			return pIljimaeJump->GetChargeLandAniRate();
		if( pIljimaeJump->IsComboDash() )
			return pIljimaeJump->GetComboDashLandAniRate();
	}
	else if( pMagneticJump && pMagneticJump->IsChargeAttackState() )	// 마그네틱 2020-12-03
	{
		return pMagneticJump->GetChargeLandAniRate( pOwner );
	}
	else if( pTaoistJump && pTaoistJump->IsChargeAttackState() )
	{
		return pTaoistJump->GetChargeLandAniRate( pOwner );
	}

	if( m_pExtendJump )
	{
		if( m_pExtendJump->CheckUseExtendJump( pOwner ) || bUseAttack )
			return m_fExtendLandingRate;
	}
	else
	{
		if( bUseAttack )
			return m_fExtendLandingRate;
	}

	return m_fJumpLandingRate;
}

void ioAttackableItem::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpping( pOwner, fHeightGap, m_fJumpNeedGauge );
	}
}

void ioAttackableItem::ProcessJumppingByNoNeedProcess( ioBaseChar *pOwner, float fHeightGap )
{
}

void ioAttackableItem::ProcessLanding( ioBaseChar *pOwner )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessLanding( pOwner );
	}
}

void ioAttackableItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}
}

bool ioAttackableItem::ProcessAimJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		return m_pExtendJump->ProcessAimJump( pOwner, fHeightGap );
	}

	return false;
}

void ioAttackableItem::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ApplyJump( pOwner, rkPacket );
	}
}

void ioAttackableItem::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( m_pExtendDash )
	{
		m_pExtendDash->ApplyDash( pOwner, rkPacket );
	}
}

void ioAttackableItem::ApplyCounterAttack( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( m_pCounterAttack )
	{
		m_pCounterAttack->ApplyCAT( pOwner, rkPacket );
	}
}

DWORD ioAttackableItem::GetJumpChargeTime( ioBaseChar *pChar ) const
{
	if( m_pExtendJump )
	{
		return m_pExtendJump->GetChargingTime( pChar );
	}

	return 0;
}

ioHashString ioAttackableItem::GetJumpAniDefenseWounded()
{
	ioHashString szAniName;
	if( m_pExtendJump )
	{
		szAniName = m_pExtendJump->GetJumpAniDefenseWounded();
	}

	return szAniName;
}

ioHashString ioAttackableItem::GetJumpAniDefenseCriticalWounded()
{
	ioHashString szAniName;
	if( m_pExtendJump )
	{
		szAniName = m_pExtendJump->GetJumpAniDefenseCriticalWounded();
	}

	return szAniName;
}

bool ioAttackableItem::IsCanUpdateGauge( ioBaseChar *pOwner )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pOwner ) )
		return true;

	if( m_bGaugeUpdateExtendDefense && pOwner->GetState() == CS_EXTEND_DEFENSE )
		return true;

	return false;
}

bool ioAttackableItem::CheckUseExtendJump( ioBaseChar* pOwner )
{
	if( m_pExtendJump )
		return m_pExtendJump->CheckUseExtendJump( pOwner );

	return false;
}


bool ioAttackableItem::CheckUseBuffAirDashFromJump() const
{
	if( m_pExtendJump )
		return m_pExtendJump->CheckCanUseBuffAirDash();

	return true;
}

bool ioAttackableItem::IsEnableExtendJump()
{
	return true;
}

void ioAttackableItem::RecevieExtendJumpUse( ioBaseChar *pOwner )
{
}

bool ioAttackableItem::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	if( m_pExtendJump )
		return m_pExtendJump->IsCanJumpingSkill( pOwner );

	return true;
}

void ioAttackableItem::SetLandingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->ClearSylphid();

	if( m_pExtendJump )
		m_pExtendJump->SetLandingState( pOwner );
}

void ioAttackableItem::CheckAimJumpState( ioBaseChar *pChar )
{
}

const ioHashString& ioAttackableItem::GetJumppingAnimation() const
{
	return m_JumppingAnimation;
}

const ioHashString& ioAttackableItem::GetExtendJumppingAnimation() const
{
	return m_ExtendJumppingAnimation;
}

bool ioAttackableItem::CheckLanding( ioBaseChar *pChar )
{
	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	SetNoneState( pChar );
	return true;
}

float ioAttackableItem::GetAttackSpeedRate() const
{
	 return m_fAttackSpeedRate;
}

float ioAttackableItem::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
{
	if( m_pExtendJump )
		return m_pExtendJump->GetExtraJumpSpeedRate( vDir, qtRot );

	return m_fJumpSpeedRate;
}

bool ioAttackableItem::CheckEnableLandAttackByGravity( ioBaseChar *pChar ) const
{
	if( !pChar ) return false;

	return pChar->CheckEnableLandAttackByGravity( m_fLandAttackEnableGravity );
}

float ioAttackableItem::GetDashSpeedRate()
{
	if( m_pExtendDash )
	{
		ioSimpleDash *pSimpleDash = ToSimpleDash(m_pExtendDash);
		if( pSimpleDash && pSimpleDash->IsEndAction() )
			return m_fDashEndSpeedRate;

		ioPowerChargeDash *pPowerChargeDash = ToPowerChargeDash( m_pExtendDash );
		if( pPowerChargeDash && pPowerChargeDash->IsPowerChargingState() )
			return pPowerChargeDash->GetChargingSpeedRate();

		ioShaolinDash *pShaolinDash = ToShaolinDash( m_pExtendDash );
		if( pShaolinDash && pShaolinDash->IsPowerChargingState() )
			return pShaolinDash->GetChargingSpeedRate();

		ioPowerChargeDash2 *pPowerChargeDash2 = ToPowerChargeDash2( m_pExtendDash );
		if( pPowerChargeDash2 && pPowerChargeDash2->IsPowerChargingState() )
		{
			return pPowerChargeDash2->GetCurStateSpeedRate();
		}
	}

	return m_fDashSpeedRate;
}

float ioAttackableItem::GetDashAttackSpeedRate()
{
	return m_fDashAttackSpeedRate;
}

bool ioAttackableItem::IsCanJumpAttack( ioBaseChar *pOwner )
{
	if( !m_bJumpAttack )
		return false;

	if( m_pExtendJump )
		return m_pExtendJump->IsCanJumpAttack( pOwner );

	return true;
}

bool ioAttackableItem::IsCanJump_D_Attack( ioBaseChar *pOwner )
{
	if( !m_bJump_D_Attack )
		return false;

	if( m_pExtendJump )
		return m_pExtendJump->IsCanJumpAttack( pOwner );

	return true;
}

bool ioAttackableItem::IsCanJumpReload( ioBaseChar *pOwner )
{
	if( !m_bEnableJumpReload )
		return false;

	return true;
}

void ioAttackableItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
}

void ioAttackableItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
{
	// 공격키가 눌러져 있으면 공격 상태 처리
	if( pOwner->IsAttackKeyDown() && !pOwner->IsJumpKeyDown() && !pOwner->IsDefenseKeyDown() && !pOwner->IsChatModeState(true) )
	{
		ioObjectItem *pItem = pOwner->GetObject();
		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{	
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		m_bEnableDefenseState = false;
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		m_bEnableJumpState = false;
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		m_bEnableAttackState = false;
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

bool ioAttackableItem::SetDefenseAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	int iAniID = pGrp->GetAnimationIdx( m_DefenseAttack.m_AttackAnimation );
	float fRate = m_DefenseAttack.m_fAttackAniRate;
	if( fRate <= 0.0f )
		fRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_DefenseAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fRate, m_DefenseAttack.m_dwPreDelay );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate, 0.0f, m_DefenseAttack.m_dwPreDelay );

	pOwner->SetState( CS_DEFENSE_ATTACK );

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetReservedSliding( m_DefenseAttack.m_vForceInfoList, iAniID, fRate, m_DefenseAttack.m_dwPreDelay );

	m_dwDefenseAttackEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DEFENSE_ATTACK );
		kPacket << pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		//pOwner->IncreaseWeaponIndexBase();
		kPacket << pOwner->GetWeaponIndexBase();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioAttackableItem::ApplyDefenseAttackState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	int iAniID = pGrp->GetAnimationIdx( m_DefenseAttack.m_AttackAnimation );
	float fRate = m_DefenseAttack.m_fAttackAniRate;
	if( fRate <= 0.0f )
		fRate = FLOAT1;

	pOwner->RefreshFireTimeListWithBase( pOwner->GetWeaponIndexBase(), iAniID, m_DefenseAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, FLOAT1/fRate, m_DefenseAttack.m_dwPreDelay );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate, 0.0f, m_DefenseAttack.m_dwPreDelay );

	pOwner->SetState( CS_DEFENSE_ATTACK );

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetReservedSliding( m_DefenseAttack.m_vForceInfoList, iAniID, fRate, m_DefenseAttack.m_dwPreDelay );

	m_dwDefenseAttackEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
}

void ioAttackableItem::CheckDefenseAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDefenseAttackEnd > dwCurTime )
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
		}
	}
	else
	{
		ProcessReserveKeyInput( pOwner );

		if( GetType() == IT_OBJECT )
			pOwner->ReleaseObjectItem( __FUNCTION__, pOwner->GetState() );
	}
}

void ioAttackableItem::InitAniRotate()
{
	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();
}

void ioAttackableItem::SetAniRotate( ioBaseChar *pChar, int iAniID, float fAniRate )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	InitAniRotate();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fAniRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}
}

void ioAttackableItem::CheckAniRotate( ioBaseChar *pChar )
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

bool ioAttackableItem::SetJumpReloadState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_JumpReloadInfoList.empty() )
		return false;

	if( !ToBowItem(this) && !ToBowItem2(this) && 
		!ToFireWeaponItem(this) && !ToAutoShotItem(this) )
		return false;

	int iMaxBullet = GetMaxBullet();
	if( iMaxBullet <= 0 ) return false;
	if( iMaxBullet <= GetCurBullet() )
		return false;

	m_bReserveJumpReload = false;
	m_iCurJumpReloadCnt = 0;
	m_dwJRComboEnable_S = 0;
	m_dwJRComboEnable_E = 0;

	if( !SetJumpReload( pOwner ) )
		return false;

	m_iCurJumpReloadCnt++;

	return true;
}

bool ioAttackableItem::SetJumpReload( ioBaseChar *pOwner )
{
	if( m_JumpReloadInfoList.empty() )
		return false;

	if( !COMPARE( m_iCurJumpReloadCnt, 0, (int)m_JumpReloadInfoList.size() ) )
		return false;

	if( !pOwner ) return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_JumpReloadInfoList[m_iCurJumpReloadCnt].m_szReloadAni );
	float fAniRate = m_JumpReloadInfoList[m_iCurJumpReloadCnt].m_fReloadAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwJRComboEnable_S = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
	m_dwJRComboEnable_E = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	int iNewBullet = GetCurBullet() + m_JumpReloadInfoList[m_iCurJumpReloadCnt].m_iReloadCnt;
	SetCurBullet( iNewBullet );

	return true;
}

void ioAttackableItem::CheckJumpReloadState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	CheckJumpReloadReserve( pOwner );
	ProcessJumpReloadState( pOwner );
}

void ioAttackableItem::CheckJumpReloadReserve( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsDefenseKey() ) return;
	if( m_bReserveJumpReload ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( !COMPARE( dwCurTime, m_dwJRComboEnable_S, m_dwJRComboEnable_E ) )
	{
		return;
	}

	m_bReserveJumpReload = true;
}

void ioAttackableItem::ProcessJumpReloadState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( !ToBowItem(this) && !ToBowItem2(this) && 
		!ToFireWeaponItem(this) && !ToAutoShotItem(this) )
		return;

	int iMaxBullet = GetMaxBullet();
	if( iMaxBullet <= 0 ) return;

	if( !m_bReserveJumpReload )
		return;

	if( !SetJumpReload( pOwner ) )
		return;

	m_bReserveJumpReload = false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_JUMP_RELOAD );
		kPacket << pOwner->GetCharName();
		kPacket << JUMP_RELOAD_SYNC;
		kPacket << m_iCurJumpReloadCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// packet 전송후 처리
	m_iCurJumpReloadCnt++;
}

void ioAttackableItem::ApplyJumpReload( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket >> m_iCurJumpReloadCnt;

	if( !SetJumpReload( pOwner ) )
		return;

	m_bReserveJumpReload = false;
	m_iCurJumpReloadCnt++;
}

bool ioAttackableItem::IsEnableExtraDashAttack()
{
	if( !m_bExtraDashAttack )
		return false;

	if( m_pExtendDash )
		return m_pExtendDash->IsEnableExtraDashAttack();

	return true;
}

bool ioAttackableItem::IsEnableExtraOtherDashAttack()
{
	if( !m_bExtraOtherDashAttack )
		return false;

	if( m_pExtendDash )
		return m_pExtendDash->IsEnableExtraOtherDashAttack();

	return true;
}

bool ioAttackableItem::IsEnableDashAttackEndDash()
{
	if( m_DashAttackEndDash == DAET_NONE )
		return false;

	if( m_pExtendDash )
		return m_pExtendDash->IsEnableDashAttackEndDash();

	return true;
}

const DashAttEndType& ioAttackableItem::GetDashAttEndType()
{
	return m_DashAttackEndDash;
}

bool ioAttackableItem::IsEnableJumpLandEndDash()
{
	if( !m_bJumpLandEndDash )
		return false;

	if( m_pExtendJump )
		return m_pExtendJump->IsEnableJumpLandEndDash();

	return true;
}

bool ioAttackableItem::CheckChangeWereWolfState( ioBaseChar *pChar )
{
	return false;
}

void ioAttackableItem::SetChangeWereWolfState( ioBaseChar *pChar )
{
	return;
}

bool ioAttackableItem::CheckReleaseWereWolfState( ioBaseChar *pChar )
{
	if( !m_bUsedWereWolf )
		return false;

	// bullet check.
	if( GetCurBullet() > 0 )
		return false;

	ReleseWereWolfState( pChar );

	return true;
}

void ioAttackableItem::ReleseWereWolfState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !m_bUsedWereWolf )
		return;

	// remove mesh
	if( pChar->IsMale() )
	{
		int iMeshCnt = m_vWereWolfMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshList[i];
			pChar->DestroyEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_vWereWolfMeshListW.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshListW[i];
			pChar->DestroyEntity( szMeshName );
		}
	}

	// remove buff
	int iBuffCnt = m_vWereWolfBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vWereWolfBuffList[j];
		pChar->RemoveBuff( szBuffName );
	}

	m_bUsedWereWolf = false;
	m_dwChangeWereWolfTime = 0;
	pChar->SetInvisibleState( false, true );

	if( pChar->IsInvisibleFace() )
		pChar->ShowFaceMesh( false );
}

void ioAttackableItem::ReleseWereWolfStateByOwner( ioBaseChar *pChar )
{
	ReleseWereWolfState( pChar );

	// add buff
	int iBuffCnt = m_vReleaseWereWolfBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		ioHashString szBuffName = m_vReleaseWereWolfBuffList[i];
		pChar->AddNewBuff( szBuffName, pChar->GetCharName(), GetName(), NULL );
	}
}

void ioAttackableItem::IncreaseChangeDamage( float fDamage, int iType, bool bBuff )
{
}

void ioAttackableItem::IncreaseChangeDamageByWeapon( float fDamage )
{
}

void ioAttackableItem::CheckIncreaseChangeDamage( ioBaseChar *pOwner )
{
}

void ioAttackableItem::DecreaseChangeDamage( ioBaseChar *pOwner )
{
}

bool ioAttackableItem::IsWereWolfState()
{
	return m_bUsedWereWolf;
}

bool ioAttackableItem::IsEnableActionStopDelay()
{
	return true;
}

void ioAttackableItem::GetAutoTargetValue( ioBaseChar *pOwner,
										   float &fRange, float &fMinAngle, float &fMaxAngle,
										   AutoTargetType eType )
{
	switch( eType )
	{
	case ATT_NORMAL:
		fRange = m_fAutoTargetRange;
		fMinAngle = m_fAutoTargetMinAngle;
		fMaxAngle = m_fAutoTargetMaxAngle;
		return;
	case ATT_DASH:
		if( m_fDashAutoTargetRange == 0.0f )
			break;
		
		if( m_pExtendDash )
		{
			float fDashRange = 0.0f;
			float fDashMinAngle = 0.0f;
			float fDashMaxAngle = 0.0f;

			if( m_pExtendDash->GetAutoTargetValue( m_fDashAutoTargetRange, m_fDashAutoTargetMinAngle, m_fDashAutoTargetMaxAngle,
												   fDashRange, fDashMinAngle, fDashMaxAngle ) )
			{
				fRange = fDashRange;
				fMinAngle = fDashMinAngle;
				fMaxAngle = fDashMaxAngle;
				return;
			}
		}

		fRange = m_fDashAutoTargetRange;
		fMinAngle = m_fDashAutoTargetMinAngle;
		fMaxAngle = m_fDashAutoTargetMaxAngle;
		return;
	case ATT_JUMP:
		if( m_fJumpAutoTargetRange == 0.0f )
			break;

		fRange = m_fJumpAutoTargetRange;
		fMinAngle = m_fJumpAutoTargetMinAngle;
		fMaxAngle = m_fJumpAutoTargetMaxAngle;
		return;
	case ATT_COUNTER:
		if( m_fCountAutoTargetRange == 0.0f )
			break;

		fRange = m_fCountAutoTargetRange;
		fMinAngle = m_fCountAutoTargetMinAngle;
		fMaxAngle = m_fCountAutoTargetMaxAngle;
		return;
	}

	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;

	return;
}

void ioAttackableItem::ClearKeyReserve()
{
	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;
}

void ioAttackableItem::ClearCancelInfo()
{
	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bEnableDashState = false;
	m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

	m_dwCancelCheckTime = 0;
	m_dwInputCancelCheckTime = 0;

	m_szCurAni.Clear();
	m_fCurAniRate = FLOAT1;
}

void ioAttackableItem::CheckCancelReserve( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( pOwner->IsJumpKey() && !m_bEnableJumpState )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
}

void ioAttackableItem::CheckCancelReserveByConditionalSpecialDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsDirKeyDoubleClick() && !m_bEnableDashState )
	{		
		m_bEnableJumpState = false;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_bEnableDashState = true;
		m_iSkillInput = -1;
		m_ConditionalSpecialDashKey = pOwner->CheckCurDirKey();
	}
}

bool ioAttackableItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	return false;
}

void ioAttackableItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
}

void ioAttackableItem::SetCurAttackAniForInputDashS( const ioHashString &szAni, float fAniRate )
{
	SetCurAttackAniForInputJump( szAni, fAniRate );
}

void ioAttackableItem::SetCurAttackAniForInputJump( const ioHashString &szAni, float fAniRate )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_szCurAni = szAni;
	m_fCurAniRate = fAniRate;

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;
}

bool ioAttackableItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( m_dwInputCancelCheckTime == 0 )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;

			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
		{
			return false;
		}

		iter++;
	}

	return false;
}

bool ioAttackableItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( m_dwInputCancelCheckTime == 0 )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fCurAniRate;
		dwInputSpecialDashTime += m_dwInputCancelCheckTime;		
	}
	
	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			
		}
		return false;
	}

	return false;
}

SummonDummyChar* ioAttackableItem::GetSummonDummyCharInfo( int iSubState )
{
	SummonDummyCharMap::iterator iter = m_SummonDummyCharMap.find( iSubState );
	if( iter != m_SummonDummyCharMap.end() )
	{

		return &(iter->second);
	}

	return NULL;
}

void ioAttackableItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
}

bool ioAttackableItem::IsEnableNewJumpAttack()
{
	if( !m_bNewJumpAttackState )
		return false;

	ioDoubleJump *pDouble = ToDoubleJump( m_pExtendJump );
	if( pDouble && pDouble->IsUseLandJumpAttack() )
		return false;

	return m_bNewJumpAttackState;
}

bool ioAttackableItem::HasCounterAttack() const
{
	if( m_pCounterAttack )
		return true;

	return false;
}

ioCounterAttack* ioAttackableItem::GetCurCounterAttack()
{
	return m_pCounterAttack;
}

CounterAttackType ioAttackableItem::GetCounterAttackType() const
{
	if( !m_pCounterAttack )
		return CAT_NONE;

	return m_pCounterAttack->GetType();
}

void ioAttackableItem::ClearAirJump()
{
	m_vAirJumpTimeList.erase( m_vAirJumpTimeList.begin(), m_vAirJumpTimeList.end() );
	m_iCurAirJump = 0;
}

void ioAttackableItem::SetAirJump( ioBaseChar *pOwner, int iAniID, float fTimeRate )
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

void ioAttackableItem::CheckAirJump( ioBaseChar *pOwner )
{
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

				pOwner->SetAirJumpPower( fCurJumpPower );

				D3DXVECTOR3 vAttDir =  pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
				if( m_fAirForcePower < 0.0f )
					pOwner->SetForcePower( -vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
				else
					pOwner->SetForcePower( vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
			}

			m_iCurAirJump++;
		}
	}
}

void ioAttackableItem::SetUseExtraGauge( float fAmt )
{
}

void ioAttackableItem::InitExtraGauge()
{
	m_fCurExtraGauge = 0.0f;
}

void ioAttackableItem::MaxExtraGauge()
{
	m_fCurExtraGauge = m_fMaxExtraGauge;
}

float ioAttackableItem::GetMaxExtraGauge()
{
	return m_fMaxExtraGauge;
}

float ioAttackableItem::GetCurExtraGauge()
{
	return m_fCurExtraGauge;
}

void ioAttackableItem::SetCurExtraGauge( float fGauge )
{
	m_fCurExtraGauge = fGauge;
	m_fCurExtraGauge = (float)max( 0, min( m_fCurExtraGauge, m_fMaxExtraGauge ) );
}


void ioAttackableItem::SetChargeRotate(bool b)
{
	m_bChargeCharRotate = b;
}

void ioAttackableItem::EquipObjectItemByAttakable( ioBaseChar* pOwner, ioItem* pObjectItem )
{

}

void ioAttackableItem::ReleaseObjectItemByAttack( ioBaseChar* pOwner, ioItem* pObjectItem )
{

}

void ioAttackableItem::WeaponMapColDead( ioWeapon *pWeapon )
{
}

void ioAttackableItem::SetDefenceCounterState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetState( CS_DELAY );
}


bool ioAttackableItem::IsUseDefenceCounterAttack()
{
	return false;
}


void ioAttackableItem::ProcessDefenceCounterAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetState( CS_DELAY );
}

void ioAttackableItem::StartCounterAttackBuff( ioBaseChar *pOwner )
{
	for( int i = 0; i < (int)m_vCounterAttackBuff.size(); i++ )
	{
		pOwner->AddNewBuff( m_vCounterAttackBuff[i], pOwner->GetCharName(), GetName(), NULL );
	}
}

void ioAttackableItem::ProcessExtraJump( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if ( m_pExtendJump )
		m_pExtendJump->ProcessExtraJump( pOwner );
}

bool ioAttackableItem::IsBuffFlyChange( ioBaseChar* pOwner )
{
	if( m_pExtendJump )
		return m_pExtendJump->IsBuffFlyChange( pOwner );

	return true;
}

const char* ioAttackableItem::GetAttackStateText() const
{
	return "";
}

bool ioAttackableItem::SetJumpLandingExtraState( ioBaseChar *pOwner )
{
	return false;
}

void ioAttackableItem::SetEndGrappling( ioBaseChar *pOwner )
{

}