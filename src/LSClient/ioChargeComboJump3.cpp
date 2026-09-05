

#include "stdafx.h"

#include "ItemDefine.h"
#include "ioChargeComboJump3.h"

ioChargeComboJump3::ioChargeComboJump3()
{
	Init();

	m_bUsedChargeAttack			= false;
	m_bUsedExcaliburAtt			= false;
	m_bUsedStrikerChargeAttack	= false;
	m_bUsedDashJumpAttack		= false;
	m_bSecondJump				= false;	
	m_bNoDropExtendAttack		= false;	

	m_bLanded					= true;

	m_iAddJumpCnt				= 0;

	m_dwBubbleFighterSpecialAttack_S_Time = 0;
	m_dwBubbleFighterSpecialAttack_E_Time = 0;

	m_bEnableDefenseAttack = true;
	m_dwStrangerSpecialAttackTime = 0;
	m_dwStrangerSpecialAttackIndex = -1;
}

ioChargeComboJump3::ioChargeComboJump3( const ioChargeComboJump3 &rhs )
: ioExtendJump( rhs ),
m_fJumpSpeedRateFR( rhs.m_fJumpSpeedRateFR ),
m_fJumpSpeedRateSI( rhs.m_fJumpSpeedRateSI ),
m_fJumpSpeedRateBG( rhs.m_fJumpSpeedRateBG ),
m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
m_ComboAttackList( rhs.m_ComboAttackList ),
m_ComboSecondAttackList( rhs.m_ComboSecondAttackList ),
m_ChargeComboAttackList( rhs.m_ChargeComboAttackList ),
m_FlashDelayAni( rhs.m_FlashDelayAni ),
m_fFlashDelayRate( rhs.m_fFlashDelayRate ),
m_fFlashForce( rhs.m_fFlashForce ),
m_fFlashGravityAmt( rhs.m_fFlashGravityAmt ),
m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
m_FlashJumpAttack( rhs.m_FlashJumpAttack ),
m_bEnableFlashJump( rhs.m_bEnableFlashJump ),
m_bEnableDoubleJump( rhs.m_bEnableDoubleJump ),
m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
m_DashJumpAttack( rhs.m_DashJumpAttack ),
m_JumpDashLandAni( rhs.m_JumpDashLandAni ),
m_fJumpDashLandAniRate( rhs.m_fJumpDashLandAniRate ),
m_fJumpDashGravityAmt( rhs.m_fJumpDashGravityAmt ),
m_ComboDashType( rhs.m_ComboDashType ),
m_bUseNewJumpTypeDash( rhs.m_bUseNewJumpTypeDash ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_dwHalfChargingTime( rhs.m_dwHalfChargingTime ),
m_ChargingAniList( rhs.m_ChargingAniList ),
m_fChargeJumpPower( rhs.m_fChargeJumpPower ),
m_fChargeGravityRate( rhs.m_fChargeGravityRate ),
m_fChargeBaseRange( rhs.m_fChargeBaseRange ),
m_ChargeAttackLandAni( rhs.m_ChargeAttackLandAni ),
m_fChargeAttackLandAniRate( rhs.m_fChargeAttackLandAniRate ),
m_JumpExtendType( rhs.m_JumpExtendType ),
m_BuffAddAttribute( rhs.m_BuffAddAttribute ),
m_vBuffAddList( rhs.m_vBuffAddList ),
m_JumpMechanicsFly( rhs.m_JumpMechanicsFly ),
m_vExcaliburAttList( rhs.m_vExcaliburAttList ),
m_fExcaliburForceAmt( rhs.m_fExcaliburForceAmt ),
m_ExtendAttackList( rhs.m_ExtendAttackList ),
m_JumpLuciferFly( rhs.m_JumpLuciferFly ),
m_JumpDestroyerFly( rhs.m_JumpDestroyerFly ),
m_JumpPeterpanFly( rhs.m_JumpPeterpanFly ),
m_ItemLandAttackInfo( rhs.m_ItemLandAttackInfo ),
m_JumpGhostStealer( rhs.m_JumpGhostStealer ),
m_JumpDevilFly( rhs.m_JumpDevilFly ),
m_JumpBladeFly( rhs.m_JumpBladeFly ),
m_StrkerJumpPowerAttack( rhs.m_StrkerJumpPowerAttack ),
m_StrkerJumpPowerAddAttack( rhs.m_StrkerJumpPowerAddAttack ),
m_fStrikerDasnEndJumpPower( rhs.m_fStrikerDasnEndJumpPower ),
m_fStrikerDashHitAddJumpPower( rhs.m_fStrikerDashHitAddJumpPower ),
m_fStrikerChargeAttackEnableHeight( rhs.m_fStrikerChargeAttackEnableHeight ),
m_bUseLandJumpAttack( rhs.m_bUseLandJumpAttack ),
m_fGravityAmt( rhs.m_fGravityAmt ),
m_fShadowRangerUseGauge( rhs.m_fShadowRangerUseGauge ),
m_bDashJumpAttackCharLookAtLock( rhs.m_bDashJumpAttackCharLookAtLock ),
m_vLookAtLockDashAniList( rhs.m_vLookAtLockDashAniList ),
m_fLookAtLockDashAniRate( rhs.m_fLookAtLockDashAniRate ),
m_vLookAtLockDashForceInfoList( rhs.m_vLookAtLockDashForceInfoList ),
m_bLookAtLocInputDirRotate( rhs.m_bLookAtLocInputDirRotate ),
m_fNoDropExtendAttackEndJumpPower( rhs.m_fNoDropExtendAttackEndJumpPower ),
m_bExtendAttackUseAirPower( rhs.m_bExtendAttackUseAirPower ),
m_Cuchulain_S_Attack( rhs.m_Cuchulain_S_Attack ),
m_fCuchulain_S_Attack_End_Jump_Power( rhs.m_fCuchulain_S_Attack_End_Jump_Power ),
m_fOnimushaChargeUseGauge( rhs.m_fOnimushaChargeUseGauge ),
m_fOnimushaObjectChargeUseGauge( rhs.m_fOnimushaObjectChargeUseGauge ),
m_bJumpDashCombo( rhs.m_bJumpDashCombo ),
m_BubbleFighterSpecialAttack( rhs.m_BubbleFighterSpecialAttack ),
m_dwBubbleFighterSpecialAttack_S( rhs.m_dwBubbleFighterSpecialAttack_S ),
m_dwBubbleFighterSpecialAttack_E( rhs.m_dwBubbleFighterSpecialAttack_E ),
m_fDefenseAttackEnableHeight( rhs.m_fDefenseAttackEnableHeight ),
m_StrangerSpecialAttack( rhs.m_StrangerSpecialAttack ),
m_szStrangerSpecialLoopAni( rhs.m_szStrangerSpecialLoopAni ),
m_fStrangerSpecialLoopAniRate( rhs.m_fStrangerSpecialLoopAniRate ),
m_fStrangerSpeicialGravityAmt( rhs.m_fStrangerSpeicialGravityAmt )
{
	Init();

	m_bUsedChargeAttack			= false;
	m_bUsedExcaliburAtt			= false;
	m_bUsedStrikerChargeAttack	= false;
	m_bUsedDashJumpAttack		= false;
	m_bSecondJump				= false;
	m_bNoDropExtendAttack		= false;

	m_bLanded					= true;	

	m_iAddJumpCnt				= 0;

	m_bCanSuperDimentionDash = true;

	m_dwBubbleFighterSpecialAttack_S_Time = 0;
	m_dwBubbleFighterSpecialAttack_E_Time = 0;

	m_bEnableDefenseAttack = true;
	m_dwStrangerSpecialAttackTime = 0;
	m_dwStrangerSpecialAttackIndex = -1;

	m_bUsedFly = false;
}

ioExtendJump* ioChargeComboJump3::Clone()
{
	return new ioChargeComboJump3( *this );
}

ioChargeComboJump3::~ioChargeComboJump3()
{
}

JumpType ioChargeComboJump3::GetType() const
{
	return JT_CHARGE_COMBO3;
}

void ioChargeComboJump3::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	m_bEnableFlashJump = rkLoader.LoadBool_e( "enable_flash_jump", false );
	m_bEnableDoubleJump = rkLoader.LoadBool_e( "enable_double_jump", false );
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );

	// combo
	m_fJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	LoadComboList( rkLoader );
	LoadChargeComboList( rkLoader );

	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );
	m_dwHalfChargingTime = (DWORD)rkLoader.LoadInt_e( "half_charging_time", 0 );

	int iMaxChargeAni = rkLoader.LoadInt_e( "charging_jump_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "charging_jump_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ChargingAniList.push_back( szBuf );
	}

	// double
	m_iMaxJumpCnt = rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt = max( 1, m_iMaxJumpCnt );

	m_fDoubleJumpPower = rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce = rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_dwDoubleJumpEnableTime = rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_fDoubleJumpEnableHeight = rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );

	m_fDoubleJumpAniRate = rkLoader.LoadFloat_e( "double_jump_ani_rate", FLOAT1 );

	m_vDoubleJumpAniList.clear();
	m_vDoubleJumpAniList.reserve( 4 );

	rkLoader.LoadString_e( "double_jump_animation_front", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "double_jump_animation_right", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "double_jump_animation_back", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "double_jump_animation_left", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );

	m_fDoubleJumpSpeedRateFR = rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI = rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG = rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

	// flash
	rkLoader.LoadString_e( "flash_jump_delay_animation", "", szBuf, MAX_PATH );
	m_FlashDelayAni = szBuf;

	m_fFlashDelayRate = rkLoader.LoadFloat_e( "flash_jump_delay_ani_rate", FLOAT1 );

	m_fFlashForce = rkLoader.LoadFloat_e( "flash_jump_force", 0.0f );
	m_fFlashGravityAmt = rkLoader.LoadFloat_e( "flash_jump_gravity_amt", FLOAT1 );

	m_FlashJumpAttack.Init();
	LoadAttackAttribute_e( "flash_jump_attack", m_FlashJumpAttack, rkLoader );

	// dash
	m_ComboDashType = (ComboDashType)rkLoader.LoadInt_e( "jump_dash_type", CDT_GRAVITY );

	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "dash_jump_attack", m_DashJumpAttack, rkLoader );

	m_bDashJumpAttackCharLookAtLock = rkLoader.LoadBool_e( "jump_dash_attack_char_look_at_lock", false );

	rkLoader.LoadString_e( "jump_dash_land_ani", "", szBuf, MAX_PATH );
	m_JumpDashLandAni = szBuf;
	m_fJumpDashLandAniRate = rkLoader.LoadFloat_e( "jump_dash_land_ani_rate", FLOAT1 );


	m_fJumpDashGravityAmt = rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );

	m_bUseNewJumpTypeDash = rkLoader.LoadBool_e( "use_new_jump_type_dash", false );

	m_fChargeJumpPower = rkLoader.LoadFloat_e( "jump_charge_jump_power", 0.0f );
	m_fChargeGravityRate = rkLoader.LoadFloat_e( "jump_charge_gravity_rate", FLOAT1 );
	m_fChargeBaseRange = rkLoader.LoadFloat_e( "jump_charge_base_range", 0.0f );

	rkLoader.LoadString_e( "jump_charge_land", "", szBuf, MAX_PATH );
	m_ChargeAttackLandAni = szBuf;
	m_fChargeAttackLandAniRate = rkLoader.LoadFloat_e( "jump_charge_land_time_rate", FLOAT1 );
	
	// JumpAttack
	m_bUseLandJumpAttack = rkLoader.LoadBool_e( "use_land_jump_attack", false );
	m_fGravityAmt = rkLoader.LoadFloat_e( "land_jump_attack_gravity", 0.0f );
	
	// jump extend type
	m_JumpExtendType = (JumpExtendType)rkLoader.LoadInt_e( "jump_extend_type", JET_NONE );

	// buff add
	LoadAttackAttribute_e( "jump_buff_add_attack", m_BuffAddAttribute, rkLoader );

	m_vBuffAddList.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "jump_buff_add_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "jump_buff_add%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuffName = szBuf;
		m_vBuffAddList.push_back( szBuffName );
	}

	LoadPropertyByJumpType( rkLoader );

	// excalibur
	m_fExcaliburForceAmt = rkLoader.LoadFloat_e( "excalibur_force_amt", 0.0f );

	m_vExcaliburAttList.clear();
	int iMaxExcaliburAtt = rkLoader.LoadInt_e( "excalibur_att_cnt", 0 );

	if( iMaxExcaliburAtt > 0 )
	{
		m_vExcaliburAttList.reserve( iMaxExcaliburAtt );

		for( i=0; i < iMaxExcaliburAtt; ++i )
		{
			AttackAttribute kAttack;

			wsprintf_e( szKey, "excalibur_special_attack%d", i+1 );
			LoadAttackAttribute( szKey, kAttack, rkLoader );

			m_vExcaliburAttList.push_back( kAttack );
		}
	}

	// extend combo
	int iMax = rkLoader.LoadInt_e( "extend_jump_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szKey, "extend_jump_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_ExtendAttackList.push_back( eAttribute );
	}

	m_fNoDropExtendAttackEndJumpPower = rkLoader.LoadFloat_e( "extend_jump_attack_end_jump_power", 0.0f );	

	m_bExtendAttackUseAirPower = rkLoader.LoadBool_e( "etxtend_attack_use_air_power", false );
	m_bJumpDashCombo = rkLoader.LoadBool_e( "enable_jump_dash_combo", false );
}

void ioChargeComboJump3::LoadPropertyByJumpType( ioINILoader &rkLoader )
{
	// mechanics fly
	LoadMechanicsFlyInfo( rkLoader );
	//루시퍼
	LoadLuciferFlyInfo( rkLoader );
	//사이보그
	LoadDestroyerFlyInfo( rkLoader );
	//피터펜
	if( m_JumpExtendType == JET_PETERPAN )
		LoadPeterpanFlyInfo( rkLoader );
	//영혼도둑
	else if( m_JumpExtendType == JET_GHOST_STEALER )
		LoadGhostStealerInfo( rkLoader );
	//데빌
	else if( m_JumpExtendType == JET_DEVIL )
		LoadDevilFlyInfo( rkLoader );
	//스트라이커
	else if( m_JumpExtendType == JET_STRIKER )
		LoadStrikerInfo( rkLoader );
	//쉐도우 레인저
	else if( m_JumpExtendType == JET_SHADOW_RANGER )
		LoadShadowRangerExtendInfo( rkLoader );
	//캐릭터 이동방향 고정형태 대쉬
	else if( m_JumpExtendType == JET_LOOK_AT_DASH )
		LoadLookAtLockDash( rkLoader );
	//쿠훌린
	else if( m_JumpExtendType == JET_CUCHULAIN_EXTEND )
		LoadCuchulainIfo( rkLoader );
	else if( m_JumpExtendType == JET_BLADE )
		LoadBladeFlyInfo( rkLoader );
	//귀무사
	else if( m_JumpExtendType == JET_ONIMUSHA )
		m_fOnimushaChargeUseGauge = rkLoader.LoadFloat_e( "jump_onimusha_charge_use_gauge", 0.0f );
	else if( m_JumpExtendType == JET_ONIMUSHA_OBJECT )
		m_fOnimushaObjectChargeUseGauge = rkLoader.LoadFloat_e( "jump_onimusha_object_charge_use_gauge", 0.0f );
	//버블파이터
	else if( m_JumpExtendType == JET_BUBBLE_FIGHTER )
		LoadBubbleFighterInfo( rkLoader );
	else if ( m_JumpExtendType == JET_STRANGER )
		LoadStrangerInfo( rkLoader );
}

void ioChargeComboJump3::LoadMechanicsFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_JumpMechanicsFly.Init();

	rkLoader.LoadString_e( "jump_mechanics_fly_effect", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_MechanicsFlyEffect = szBuf;

	rkLoader.LoadString_e( "jump_mechanics_fly_move_effect", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_MechanicsFlyMoveEffect = szBuf;

	wsprintf_e( szBuf, "jump_mechanics_fly_attack" );
	LoadAttackAttribute( szBuf, m_JumpMechanicsFly.m_MechanicsAttack, rkLoader );

	rkLoader.LoadString_e( "jump_mechanics_fly_start_ani", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_StartAni = szBuf;
	m_JumpMechanicsFly.m_fStartAniRate = rkLoader.LoadFloat_e( "jump_mechanics_fly_start_ani_rate", FLOAT1 );

	m_JumpMechanicsFly.m_fStartAngle = rkLoader.LoadFloat_e( "jump_mechanics_fly_start_angle", 0.0f );
	m_JumpMechanicsFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_mechanics_fly_start_max_range", 0.0f );
	m_JumpMechanicsFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "jump_mechanics_fly_start_duration", 0 );

	rkLoader.LoadString_e( "jump_mechanics_fly_delay_ani", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_DelayAni = szBuf;
	m_JumpMechanicsFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "jump_mechanics_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_mechanics_fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_MoveAniFr = szBuf;
	m_JumpMechanicsFly.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "jump_mechanics_fly_move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "jump_mechanics_fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_MoveAniBk = szBuf;
	m_JumpMechanicsFly.m_fMoveAniBkRate = rkLoader.LoadFloat_e( "jump_mechanics_fly_move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "jump_mechanics_fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_MoveAniRt = szBuf;
	m_JumpMechanicsFly.m_fMoveAniRtRate = rkLoader.LoadFloat_e( "jump_mechanics_fly_move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "jump_mechanics_fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_MoveAniLt = szBuf;
	m_JumpMechanicsFly.m_fMoveAniLtRate = rkLoader.LoadFloat_e( "jump_mechanics_fly_move_ani_rate_lt", FLOAT1 );

	rkLoader.LoadString_e( "jump_mechanics_fly_move_ani_up", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_MoveAniUp = szBuf;
	m_JumpMechanicsFly.m_fMoveAniUpRate = rkLoader.LoadFloat_e( "jump_mechanics_fly_move_ani_rate_up", FLOAT1 );

	rkLoader.LoadString_e( "jump_mechanics_fly_move_ani_down", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_MoveAniDown = szBuf;
	m_JumpMechanicsFly.m_fMoveAniDownRate = rkLoader.LoadFloat_e( "jump_mechanics_fly_move_ani_rate_down", FLOAT1 );

	m_JumpMechanicsFly.m_fMoveForceAmt = rkLoader.LoadFloat_e( "jump_fly_force_move", 0.0f );
	m_JumpMechanicsFly.m_fMoveForceFric = rkLoader.LoadFloat_e( "jump_fly_force_friction", FLOAT1 );
	m_JumpMechanicsFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "jump_fly_updown_speed", 0.0f );

	m_JumpMechanicsFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_fly_target_wound_type", TWT_ALL );
	m_JumpMechanicsFly.m_fTargetAngle = rkLoader.LoadFloat_e( "jump_fly_target_angle", 45.0f );
	m_JumpMechanicsFly.m_fTargetRange = rkLoader.LoadFloat_e( "jump_fly_target_range", 45.0f );
	m_JumpMechanicsFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "jump_fly_target_up_height", 45.0f );
	m_JumpMechanicsFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "jump_fly_target_down_height", 45.0f );

	m_JumpMechanicsFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "jump_fly_target_marker_range", 45.0f );

	rkLoader.LoadString_e( "jump_fly_attack_move_ani", "", szBuf, MAX_PATH );
	m_JumpMechanicsFly.m_AttackMoveAni = szBuf;
	m_JumpMechanicsFly.m_fAttackMoveAniRate = rkLoader.LoadFloat_e( "jump_fly_attack_move_ani_rate", FLOAT1 );

	m_JumpMechanicsFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "jump_fly_attack_move_speed", 0.0f );
	m_JumpMechanicsFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "jump_fly_attack_move_range", FLOAT100 );
	m_JumpMechanicsFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "jump_fly_attack_move_rate", FLOAT1 );

	m_JumpMechanicsFly.m_fMechanicsFlyJumpAmt = rkLoader.LoadFloat_e( "jump_mechanics_fly_jump_amt", 0.0f );
}

void ioChargeComboJump3::LoadLuciferFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_JumpLuciferFly.Init();

	rkLoader.LoadString_e( "lucifer_fly_effect", "", szBuf, MAX_PATH );
	m_JumpLuciferFly.m_FlyEffect = szBuf;

	rkLoader.LoadString_e( "lucifer_fly_move_effect", "", szBuf, MAX_PATH );
	m_JumpLuciferFly.m_FlyMoveEffect = szBuf;

	rkLoader.LoadString_e( "jump_lucifer_fly_start_ani", "", szBuf, MAX_PATH );
	m_JumpLuciferFly.m_StartAni = szBuf;
	m_JumpLuciferFly.m_fStartAniRate = rkLoader.LoadFloat_e( "lucifer_fly_start_ani_rate", FLOAT1 );

	m_JumpLuciferFly.m_fStartAngle = rkLoader.LoadFloat_e( "jump_lucifer_fly_start_angle", 0.0f );
	m_JumpLuciferFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_lucifer_fly_start_max_range", 0.0f );
	m_JumpLuciferFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "jump_lucifer_fly_start_duration", 0 );

	rkLoader.LoadString_e( "lucifer_fly_delay_ani", "", szBuf, MAX_PATH );
	m_JumpLuciferFly.m_DelayAni = szBuf;
	m_JumpLuciferFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "lucifer_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "lucifer_fly_move_ani", "", szBuf, MAX_PATH );
	m_JumpLuciferFly.m_MoveAni = szBuf;
	m_JumpLuciferFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "lucifer_fly_move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "lucifer_fly_move_ani_up", "", szBuf, MAX_PATH );
	m_JumpLuciferFly.m_MoveAniUp = szBuf;
	m_JumpLuciferFly.m_fMoveAniUpRate = rkLoader.LoadFloat_e( "lucifer_fly_move_ani_rate_up", FLOAT1 );

	rkLoader.LoadString_e( "lucifer_fly_move_ani_down", "", szBuf, MAX_PATH );
	m_JumpLuciferFly.m_MoveAniDown = szBuf;
	m_JumpLuciferFly.m_fMoveAniDownRate = rkLoader.LoadFloat_e( "lucifer_fly_move_ani_rate_down", FLOAT1 );

	m_JumpLuciferFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "lucifer_fly_rotate_speed", 0 );
	m_JumpLuciferFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_JumpLuciferFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	m_JumpLuciferFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_JumpLuciferFly.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_JumpLuciferFly.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_JumpLuciferFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_JumpLuciferFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );

	m_JumpLuciferFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );
	m_JumpLuciferFly.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "lucifer_fly_jump_amt", 0.0f );
		
	rkLoader.LoadString_e( "lucifer_jump_attack_delayEffect", "", szBuf, MAX_PATH );
	m_JumpLuciferFly.m_dwAirAttackDelayTime = rkLoader.LoadInt_e( "lucifer_jump_attack_delayTime", 0 );

	rkLoader.LoadString_e( "lucifer_jump_attack_actionAni", "", szBuf, MAX_PATH );
	m_JumpLuciferFly.m_AirAttackActionAni = szBuf;

	m_JumpLuciferFly.m_dwAirAttackActionType = rkLoader.LoadInt_e( "lucifer_jump_attack_actiontype", 0 );

	//잡기 공격 정보
	//랜드어택
	rkLoader.LoadString_e( "lucifer_jump_land_attack_ani_FromJump", "", szBuf, MAX_PATH );
	m_ItemLandAttackInfo.m_LandAttackAttribute.m_AttackAnimation = szBuf;
	WeaponInfo weapon;
	weapon.m_iWeaponIdx = rkLoader.LoadInt_e( "lucifer_jump_land_attack_FromJump", 0 );
	m_ItemLandAttackInfo.m_LandAttackAttribute.m_vWeaponInfoList.push_back( weapon );

	//점프파워
	m_ItemLandAttackInfo.m_fJumpPower = rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_jump_power_FromJump", 0);
	m_ItemLandAttackInfo.m_fGravity = rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_gravity_FromJump", 0);

	//잡고올라갈때 Ani정보
	rkLoader.LoadString_e( "lucifer_jump_attack_grap_loop_ani_FromJump", "", szBuf, MAX_PATH );
	m_ItemLandAttackInfo.m_GrapplingInfo.m_GrapplingLoopMotion = szBuf;
	rkLoader.LoadString_e( "lucifer_jump_attack_grap_loop2_ani_FromJump", "", szBuf, MAX_PATH );
	m_ItemLandAttackInfo.m_GrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_ItemLandAttackInfo.m_GrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_loop_aniRate_FromJump", 0 );
	m_ItemLandAttackInfo.m_bGrapAttackUseMove = rkLoader.LoadBool_e( "lucifer_specialAttack_move_FromJump", false );
}

void ioChargeComboJump3::LoadBladeFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_JumpBladeFly.Init();

	rkLoader.LoadString_e( "jump_blade_fly_start_ani", "", szBuf, MAX_PATH );
	m_JumpBladeFly.m_StartAni = szBuf;
	m_JumpBladeFly.m_fStartAniRate = rkLoader.LoadFloat_e( "jump_blade_fly_start_ani_rate", FLOAT1 );
	
	m_JumpBladeFly.m_fStartAngle = rkLoader.LoadFloat_e( "jump_blade_fly_start_angle", 0.f );
	m_JumpBladeFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_blade_fly_start_max_range", 0.f );
	m_JumpBladeFly.m_fStartSpeed = rkLoader.LoadFloat_e( "jump_blade_fly_start_speed", 0.f );
}

void ioChargeComboJump3::LoadDestroyerFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_JumpDestroyerFly.Init();

	rkLoader.LoadString_e( "jump_destroyer_fly_effect", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_DestroyerFlyEffect = szBuf;

	rkLoader.LoadString_e( "jump_destroyer_fly_move_effect", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_DestroyerFlyMoveEffect = szBuf;

	wsprintf_e( szBuf, "jump_destroyer_fly_attack" );
	LoadAttackAttribute( szBuf, m_JumpDestroyerFly.m_DestroyerAttack, rkLoader );
	
	m_JumpDestroyerFly.m_dwChargingTime = rkLoader.LoadInt_e( "jump_fly_charging_time", 100 );
	rkLoader.LoadString_e( "jump_fly_charging", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_ChargingAni = szBuf;
	m_JumpDestroyerFly.m_dwChargingAniPreDelay = rkLoader.LoadInt_e( "jump_fly_charging_predely", 0 );
	m_JumpDestroyerFly.m_fChargingAniRate = rkLoader.LoadFloat_e( "jump_fly_charging_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_fly_charged", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_ChargedAni = szBuf;
	m_JumpDestroyerFly.m_fChargedAniRate = rkLoader.LoadFloat_e( "jump_fly_charged_rate", FLOAT1 );

	wsprintf_e( szBuf, "fly_charge_attack" );
	LoadAttackAttribute( szBuf, m_JumpDestroyerFly.m_DestroyerChargeAttack, rkLoader );
	
	rkLoader.LoadString_e( "jump_destroyer_fly_start_ani", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_StartAni = szBuf;
	m_JumpDestroyerFly.m_fStartAniRate = rkLoader.LoadFloat_e( "jump_destroyer_fly_start_ani_rate", FLOAT1 );

	m_JumpDestroyerFly.m_fStartAngle = rkLoader.LoadFloat_e( "jump_destroyer_fly_start_angle", 0.0f );
	m_JumpDestroyerFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_destroyer_fly_start_max_range", 0.0f );
	m_JumpDestroyerFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "jump_destroyer_fly_start_duration", 0 );

	rkLoader.LoadString_e( "jump_destroyer_fly_delay_ani", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_DelayAni = szBuf;
	m_JumpDestroyerFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "jump_destroyer_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_destroyer_fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniFr = szBuf;
	m_JumpDestroyerFly.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "jump_destroyer_fly_move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "jump_destroyer_fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniBk = szBuf;
	m_JumpDestroyerFly.m_fMoveAniBkRate = rkLoader.LoadFloat_e( "jump_destroyer_fly_move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "jump_destroyer_fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniRt = szBuf;
	m_JumpDestroyerFly.m_fMoveAniRtRate = rkLoader.LoadFloat_e( "jump_destroyer_fly_move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "jump_destroyer_fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniLt = szBuf;
	m_JumpDestroyerFly.m_fMoveAniLtRate = rkLoader.LoadFloat_e( "jump_destroyer_fly_move_ani_rate_lt", FLOAT1 );
	
	rkLoader.LoadString_e( "jump_destroyer_fly_move_ani_fr_charge", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniFrCharge = szBuf;
	m_JumpDestroyerFly.m_fMoveAniFrRateCharge = rkLoader.LoadFloat_e( "jump_destroyer_fly_move_ani_rate_fr_charge", FLOAT1 );

	rkLoader.LoadString_e( "jump_destroyer_fly_move_ani_bk_charge", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniBkCharge = szBuf;
	m_JumpDestroyerFly.m_fMoveAniBkRateCharge = rkLoader.LoadFloat_e( "jump_destroyer_fly_move_ani_rate_bk_charge", FLOAT1 );

	rkLoader.LoadString_e( "jump_destroyer_fly_move_ani_rt_charge", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniRtCharge = szBuf;
	m_JumpDestroyerFly.m_fMoveAniRtRateCharge = rkLoader.LoadFloat_e( "jump_destroyer_fly_move_ani_rate_rt_charge", FLOAT1 );

	rkLoader.LoadString_e( "jump_destroyer_fly_move_ani_lt_charge", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniLtCharge = szBuf;
	m_JumpDestroyerFly.m_fMoveAniLtRateCharge = rkLoader.LoadFloat_e( "jump_destroyer_fly_move_ani_rate_lt_charge", FLOAT1 );
	
	m_JumpDestroyerFly.m_fMoveForceAmt = rkLoader.LoadFloat_e( "jump_fly_force_move", 0.0f );
	m_JumpDestroyerFly.m_fMoveForceFric = rkLoader.LoadFloat_e( "jump_fly_force_friction", FLOAT1 );
	m_JumpDestroyerFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "jump_fly_updown_speed", 0.0f );

	m_JumpDestroyerFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_fly_target_wound_type", TWT_ALL );
	m_JumpDestroyerFly.m_fTargetAngle = rkLoader.LoadFloat_e( "jump_fly_target_angle", 45.0f );
	m_JumpDestroyerFly.m_fTargetRange = rkLoader.LoadFloat_e( "jump_fly_target_range", 45.0f );
	m_JumpDestroyerFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "jump_fly_target_up_height", 45.0f );
	m_JumpDestroyerFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "jump_fly_target_down_height", 45.0f );

	m_JumpDestroyerFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "jump_fly_target_marker_range", 45.0f );

	rkLoader.LoadString_e( "jump_fly_attack_move_ani", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_AttackMoveAni = szBuf;
	m_JumpDestroyerFly.m_fAttackMoveAniRate = rkLoader.LoadFloat_e( "jump_fly_attack_move_ani_rate", FLOAT1 );

	m_JumpDestroyerFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "jump_fly_attack_move_speed", 0.0f );
	m_JumpDestroyerFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "jump_fly_attack_move_range", FLOAT100 );
	m_JumpDestroyerFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "jump_fly_attack_move_rate", FLOAT1 );

	m_JumpDestroyerFly.m_fDestroyerFlyJumpAmt = rkLoader.LoadFloat_e( "jump_destroyer_fly_jump_amt", 0.0f );
}

void ioChargeComboJump3::LoadPeterpanFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_JumpPeterpanFly.Init();

	rkLoader.LoadString_e( "fly_effect", "", szBuf, MAX_PATH );
	m_JumpPeterpanFly.m_FlyEffect = szBuf;

	rkLoader.LoadString_e( "fly_move_effect", "", szBuf, MAX_PATH );
	m_JumpPeterpanFly.m_FlyMoveEffect = szBuf;

	rkLoader.LoadString_e( "fly_start_ani", "", szBuf, MAX_PATH );
	m_JumpPeterpanFly.m_StartAni = szBuf;
	m_JumpPeterpanFly.m_fStartAniRate = rkLoader.LoadFloat_e( "fly_start_ani_rate", FLOAT1 );

	m_JumpPeterpanFly.m_fStartAngle = rkLoader.LoadFloat_e( "fly_start_angle", 0.0f );
	m_JumpPeterpanFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_fly_start_max_range", 0.0f );
	m_JumpPeterpanFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "jump_fly_start_duration", 0 );

	rkLoader.LoadString_e( "fly_move_ani", "", szBuf, MAX_PATH );
	m_JumpPeterpanFly.m_MoveAni = szBuf;
	m_JumpPeterpanFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "fly_move_ani_rate", FLOAT1 );

	m_JumpPeterpanFly.m_fSpeed = rkLoader.LoadFloat_e( "peterpan_fly_speed", 0.0f );
	m_JumpPeterpanFly.m_fFrictionAmt = rkLoader.LoadFloat_e( "peterpan_fly_frictionamt", 0.0f );
	m_JumpPeterpanFly.m_fGravityAmt = rkLoader.LoadFloat_e( "peterpan_fly_gravityamt", 0.0f );
	m_JumpPeterpanFly.m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "peterpan_fly_rotatespeed", 0 );
	m_JumpPeterpanFly.m_fOffHeight = rkLoader.LoadFloat_e( "peterpan_fly_offheight", 0.0f );
	m_JumpPeterpanFly.m_dwHeightOffTime = (DWORD)rkLoader.LoadInt_e( "peterpan_fly_offtime", 0 );
	m_JumpPeterpanFly.m_fJumpPower = rkLoader.LoadFloat_e( "peterpan_fly_jumppower", 0);

	m_JumpPeterpanFly.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "fly_jump_amt", 0.0f );
}

void ioChargeComboJump3::LoadGhostStealerInfo(  ioINILoader &rkLoader  )
{
	char szBuf[MAX_PATH];
	m_JumpGhostStealer.Init();

	LoadAttackAttribute_e( "jump_ghoststealer_extend_attack", m_JumpGhostStealer.m_ExtendAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "jump_ghoststealer_add_extend_attack", m_JumpGhostStealer.m_AddExtendAttackAttribute, rkLoader );

	rkLoader.LoadString_e( "jump_ghoststealer_ani", "", szBuf, MAX_PATH );
	m_JumpGhostStealer.m_Ani = szBuf;

	m_JumpGhostStealer.m_fAniRate = rkLoader.LoadFloat_e( "jump_ghoststealer_ani_rate", FLOAT1 );

	m_JumpGhostStealer.m_fMoveSpeed = rkLoader.LoadFloat_e( "jump_ghoststealer_move_speed", 0.0f );
	m_JumpGhostStealer.m_fMoveRate = rkLoader.LoadFloat_e( "jump_ghoststealer_move_rate", 0.0f );
	m_JumpGhostStealer.m_fMoveAngle = rkLoader.LoadFloat_e( "jump_ghoststealer_move_angle", 0.0f );

	m_JumpGhostStealer.m_fEndJumpAmt = rkLoader.LoadFloat_e( "jump_ghoststealer_end_jump_amt", 0.0f );
	m_JumpGhostStealer.m_fCurAngle = rkLoader.LoadFloat_e( "jump_ghoststealer_angle", 45.f );

	m_JumpGhostStealer.m_fMoveLength = rkLoader.LoadFloat_e( "jump_ghoststealer_move_length", 0.f );
	m_JumpGhostStealer.m_fMoveHeightRate = rkLoader.LoadFloat_e( "jump_ghoststealer_move_height_rate", FLOAT1 );
}

void ioChargeComboJump3::LoadDevilFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_JumpDevilFly.Init();

	rkLoader.LoadString_e( "jump_fly_start_ani", "", szBuf, MAX_PATH );
	m_JumpDevilFly.m_StartAni = szBuf;
	m_JumpDevilFly.m_fStartAniRate = rkLoader.LoadFloat_e( "jump_fly_start_ani_rate", FLOAT1 );

	m_JumpDevilFly.m_fStartAngle = rkLoader.LoadFloat_e( "jump_fly_start_angle", 0.0f );
	m_JumpDevilFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_fly_start_max_range", 0.0f );
	m_JumpDevilFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "jump_fly_start_duration", 0 );
	
	rkLoader.LoadString_e( "jump_fly_move_ani", "", szBuf, MAX_PATH );
	m_JumpDevilFly.m_MoveAni = szBuf;
	m_JumpDevilFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "jump_fly_move_ani_rate", FLOAT1 );

	m_JumpDevilFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "jump_fly_rotate_speed", 0 );
	m_JumpDevilFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_JumpDevilFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	m_JumpDevilFly.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "fly_jump_amt", 0.0f );

	//공중 자동 공격
	m_JumpDevilFly.m_MoveAttack.Init();
	LoadAttackAttribute_e( "jump_fly_auto_attack", m_JumpDevilFly.m_MoveAttack, rkLoader );
	m_JumpDevilFly.m_dwFlyAttackTimeGap = (DWORD)rkLoader.LoadInt_e( "jump_fly_auto_attack_time_gap", 1000 );

	//공중 수동 공격
	m_JumpDevilFly.m_FireBallAttack.Init();
	LoadAttackAttribute_e( "jump_fly_manual_attack", m_JumpDevilFly.m_FireBallAttack, rkLoader );
	m_JumpDevilFly.m_fFlyFireNeedGauge = rkLoader.LoadFloat( "jump_fly_manual_attack_fire_gauge", FLOAT0001 );

	m_JumpDevilFly.m_dwCancelTime = (DWORD)rkLoader.LoadInt( "jump_fly_cancel_time", 0 );
}

void ioChargeComboJump3::LoadStrikerInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_StrkerJumpPowerAttack.Init();
	LoadAttackAttribute_e( "jump_striker_extend_attack", m_StrkerJumpPowerAttack, rkLoader );

	m_fStrikerDasnEndJumpPower = rkLoader.LoadFloat( "jump_striker_extend_attack_end_jump_power", 0.f );

	m_StrkerJumpPowerAddAttack.Init();
	LoadAttackAttribute_e( "jump_striker_extend_add_attack", m_StrkerJumpPowerAddAttack, rkLoader );

	m_fStrikerDashHitAddJumpPower = rkLoader.LoadFloat( "jump_striker_extend_add_attack_jump_power", 0.f );

	int iMaxSecondCombo = rkLoader.LoadInt_e( "jump_second_max_combo", 0 );

	m_ComboSecondAttackList.clear();
	for( int i=0 ; i<iMaxSecondCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_second_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboSecondAttackList.push_back( kAttribute );
	}

	m_fStrikerChargeAttackEnableHeight = rkLoader.LoadFloat_e( "jump_striker_charge_attack_height", 0.f );
}

void ioChargeComboJump3::LoadComboList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	int iMaxCombo = rkLoader.LoadInt_e( "jump_max_combo", 0 );

	m_ComboAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboAttackList.push_back( kAttribute );
	}
}

void ioChargeComboJump3::LoadCuchulainIfo( ioINILoader &rkLoader )
{
	LoadAttackAttribute( "jump_cuchulain_s_attack", m_Cuchulain_S_Attack, rkLoader );

	m_fCuchulain_S_Attack_End_Jump_Power = rkLoader.LoadFloat( "jump_cuchulain_s_attack_end_jump_power", 0.f );
}

void ioChargeComboJump3::LoadLookAtLockDash( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	//Dash
	m_bLookAtLocInputDirRotate	= rkLoader.LoadBool_e( "look_at_lock_input_dir_rotate", false );
	m_fLookAtLockDashAniRate	= rkLoader.LoadFloat_e( "look_at_lock_dash_ani_rate", FLOAT1 );
	m_vLookAtLockDashAniList.clear();
	m_vLookAtLockDashAniList.reserve( 4 );

	rkLoader.LoadString_e( "look_at_lock_dash_ani_front", "", szBuf, MAX_PATH );
	m_vLookAtLockDashAniList.push_back( ioHashString( szBuf ) );
	rkLoader.LoadString_e( "look_at_lock_dash_ani_right", "", szBuf, MAX_PATH );
	m_vLookAtLockDashAniList.push_back( ioHashString( szBuf ) );
	rkLoader.LoadString_e( "look_at_lock_dash_ani_back", "", szBuf, MAX_PATH );
	m_vLookAtLockDashAniList.push_back( ioHashString( szBuf ) );
	rkLoader.LoadString_e( "look_at_lock_dash_ani_left", "", szBuf, MAX_PATH );
	m_vLookAtLockDashAniList.push_back( ioHashString( szBuf ) );

	m_vLookAtLockDashForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "look_at_lock_dash_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_vLookAtLockDashForceInfoList.reserve( iForceCnt );

	for( int i = 0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;
		wsprintf_e( szBuf, "look_at_lock_dash_force_move%d", i+1 );
		kInfo.m_fForceMovePower		= rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "look_at_lock_friction%d", i+1 );
		kInfo.m_fForceMoveFriction	= rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_vLookAtLockDashForceInfoList.push_back( kInfo );
	}
}

void ioChargeComboJump3::LoadBubbleFighterInfo( ioINILoader &rkLoader )
{
	m_dwBubbleFighterSpecialAttack_S = rkLoader.LoadFloat_e( "bubble_fighter_special_attack_s", 0.0f );
	m_dwBubbleFighterSpecialAttack_E = rkLoader.LoadFloat_e( "bubble_fighter_special_attack_e", 0.0f );
	LoadAttackAttribute_e( "bubble_fighter_special_attack", m_BubbleFighterSpecialAttack, rkLoader );
}

void ioChargeComboJump3::LoadStrangerInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	LoadAttackAttribute_e( "stranger_jump_special_attack", m_StrangerSpecialAttack, rkLoader );
	rkLoader.LoadString( "stranger_jump_special_loop_ani", "", szBuf, MAX_PATH );
	m_szStrangerSpecialLoopAni = szBuf;
	m_fStrangerSpecialLoopAniRate = rkLoader.LoadFloat_e( "stranger_jump_special_loop_ani_rate", 1.0f );
	m_fStrangerSpeicialGravityAmt = rkLoader.LoadFloat_e( "stranger_jump_special_gravity_amt", 0.0f );
	m_fDefenseAttackEnableHeight = rkLoader.LoadFloat_e( "stranger_jump_special_enable_height", 0.0f );
}

void ioChargeComboJump3::Init()
{
	m_ChargeComboState					= CCS_NONE;

	m_bUseExtendJump					= false;
	m_bUsedComboAttack					= false;
	m_bUsedDoubleJump					= false;
	m_bUsedHighJump = false;
	m_bUsedFlash						= false;
	m_bUseExtendJumpAttack				= false;
	m_bReserveJumpAttack				= false;

	// combo
	m_iCurComboCnt						= 0;
	m_dwEnableComboTime_S				= 0;
	m_dwEnableComboTime_E				= 0;

	m_dwInput_Dash_S_StartTime			= 0;

	m_dwChargingStartTime				= 0;
	m_dwJumpChargingStartTime = 0;
	m_dwDefenseChargingStartTime = 0;
	m_dwFireStartTime					= 0;

	// double
	m_iCurJumpCnt						= 1;

	// flash
	m_dwFlashDelayEndTime				= 0;
	m_dwFlashEndTime					= 0;

	// dash
	m_dwJumpDashStartTime				= 0;
	m_dwJumpDashEndTime					= 0;

	m_GuidEffectID						= -1;
	m_dwFireEndTime						= 0;

	m_dwRedHoodSpecialMoveMacroTime_S	= 0;
	m_dwRedHoodSpecialMoveMacroTime_E	= 0;

	m_dwNoDropExtendAttackEndTime		= 0;
	m_vCharMoveDir						= ioMath::VEC3_ZERO;

	m_fCurCharMoveSpeed					= 0.0f;
}

void ioChargeComboJump3::LoadChargeComboList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_ChargeComboAttackList.clear();
	AttackAttribute kAttribute;
	int nMaxChargeAttackCnt = rkLoader.LoadInt( "max_charge_attack_cnt", 1 );
	for( int i = 0; i < nMaxChargeAttackCnt; i++ )
	{
		wsprintf_e( szBuf, "jump_charge_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ChargeComboAttackList.push_back( kAttribute );
	}	
}

void ioChargeComboJump3::InitJump()
{
	m_bEnableDefenseAttack = true;
}

void ioChargeComboJump3::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	pOwner->SetUsedFlashJump( false );

	Init();

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	if( bFullTime && m_dwJumpChargeTime > 0 )
	{
		m_bUsedHighJump = true;
		m_bUseExtendJump = true;
		float fJumpPower = pOwner->GetJumpPower();
		fJumpPower *= m_fExtraJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	}

	m_bUsedChargeAttack = false;
	m_bUsedExcaliburAtt = false;

	m_iAddJumpCnt++;
	if( m_iAddJumpCnt >= 2 )
		m_bSecondJump = true;

	if ( m_JumpExtendType == JET_BUBBLE_FIGHTER )
	{
		m_dwBubbleFighterSpecialAttack_S_Time = FRAMEGETTIME() +  m_dwBubbleFighterSpecialAttack_S;
		m_dwBubbleFighterSpecialAttack_E_Time = FRAMEGETTIME() +  m_dwBubbleFighterSpecialAttack_E;
	}
}

void ioChargeComboJump3::ProcessLanding( ioBaseChar *pOwner )
{
}

void ioChargeComboJump3::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
		return;

	// 체크 순서 중요!!
	DWORD dwCurTime = FRAMEGETTIME();

	// combo
	if( m_bReserveJumpAttack )
	{
		SetChargingState( pOwner );
		return;
	}
	else if( m_JumpExtendType == JET_BUBBLE_FIGHTER && pOwner->IsAttackKeyDown() )
	{
		if ( IsCanBubbleFighterAttack( pOwner ) )
		{
			SetChargingState( pOwner );
			return;
		}
	}
	else if( m_dwJumpChargingStartTime > 0 && CheckJumpCharging( pOwner ) )
		return;
	else if( m_dwDefenseChargingStartTime > 0 && CheckDefenseCharging( pOwner ) )
		return;

	//점핑 -> 점프 대쉬로 가능한지 체크
	switch( m_JumpExtendType )
	{
	case JET_SUPER_DIMENSION:
		if( IsCanSuperDimensionDash( pOwner ) )
		{
			SetSuperDimensionDashState( pOwner );
			return;
		}
		break;
	case JET_LOOK_AT_DASH:
		if( IsCanJumpDash( pOwner ) )
		{
			SetLookAtLockJumpDash( pOwner );
			return;
		}
		break;
	case JET_APOSTATE_MONK:
		if ( IsCanApostateMonkDash( pOwner ) )
		{
			SetApostateMonkJumpDashAttack( pOwner );
			return;
		}
		break;
	default:	
		if( IsCanJumpDash( pOwner ) )
		{
			pOwner->ClearValkyrieDobuleJumpFly();
			SetJumpDash( pOwner );
			return;
		}
		break;
	}

	// Double
	if( m_JumpExtendType != JET_MURIM_GOSU && IsCanDoubleJump( pOwner, fHeightGap, false ) )
	{
		if( m_fDoubleJumpPower >= 0.0f )
		{
			SetDoubleJump( pOwner );
			return;
		}
	}
	else if( m_JumpExtendType == JET_MURIM_GOSU )
	{
		if( IsCanJumpCharge( pOwner, fHeightGap ) )
		{
			SetJumpChargingState( pOwner );
			return;
		}
		else if( IsCanDefenseCharge( pOwner, fHeightGap ) )
		{
			SetDefenseChargingState( pOwner );
			return;
		}
	}

	if( IsDoubleBuffFlyChange( pOwner ) )
	{
		ChangeToDoubleJumpFly( pOwner );
	}
	else
	{
		switch( m_ChargeComboState )
		{
		case CCS_DASH:
		case CCS_DASH_END:
			ProcessJumpDash( pOwner );
			break;
		}
	}
	
	if ( IsCanDefenseKeyAttack( pOwner, fHeightGap ) )
		return;
}

bool ioChargeComboJump3::IsDoubleBuffFlyChange( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;

	if( !pOwner->IsNeedProcess() )
		return false;

	ioSummonChargeItem3* pSummonChargeItem3 = ToSummonChargeItem3( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pSummonChargeItem3 )
		return false;

	if( pSummonChargeItem3->GetDoubleJumpFlyBuff().IsEmpty() )
		return false;

	if( m_iCurJumpCnt != pSummonChargeItem3->GetBuffUseEnableJumpCount() )
		return false;

	if( pOwner->HasBuff( BT_DOUBLE_JUMP ) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( m_ChargeComboState != CCS_DOUBLE )
		return false;

	return true;
}

bool ioChargeComboJump3::CheckUseExtendJump( ioBaseChar* pOwner )
{
	if( pOwner )
	{
		ioSummonChargeItem3* pSummonChargeItem3 = ToSummonChargeItem3( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pSummonChargeItem3 && 
			!pSummonChargeItem3->GetDoubleJumpFlyBuff().IsEmpty() &&
			pOwner->HasBuff( pSummonChargeItem3->GetDoubleJumpFlyBuff() ) )			
			return false;
	}

	return ioExtendJump::CheckUseExtendJump( pOwner );
}

void ioChargeComboJump3::ChangeToDoubleJumpFly( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioSummonChargeItem3* pSummonChargeItem3 = ToSummonChargeItem3( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSummonChargeItem3 && !pSummonChargeItem3->GetDoubleJumpFlyBuff().IsEmpty() )
	{
		pOwner->AddNewBuff( pSummonChargeItem3->GetDoubleJumpFlyBuff(), pOwner->GetCharName(), pSummonChargeItem3->GetName(), NULL );
	}
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_VALKI_JUMP;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::IsCanCuchulainJumpAttack( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioCuchulainItem *pCuchulainItem = ToCuchulainWeaponItem( pItem );
	if( pCuchulainItem )
	{
		if( (int)pCuchulainItem->GetNeedBullet() <= pCuchulainItem->GetCurBullet() )
			return true;
	}

	return false;
}

// combo
void ioChargeComboJump3::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_NORMAL:
	case CCS_DASH:
	case CCS_DOUBLE:
	case CCS_STRIKER_MOVE_ATTACK:
	case CCS_LOOK_AT_DASH:
	case CCS_CUCHULAIN_S_ATTACK:
	case CCS_JUMP_CHARGE:
	case CCS_JUMP_CHARGE2:
	case CCS_DEFENSE_CHARGE:
		break;
	default:
		return;
	}

	if( m_ChargeComboState == CCS_CUCHULAIN_S_ATTACK )
	{
		m_bReserveJumpAttack = true;
		return;
	}

	if( m_bSecondJump && m_JumpExtendType == JET_STRIKER )
	{
		int iMaxCnt = m_ComboSecondAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			return;
	}
	else if( m_JumpExtendType == JET_MURIM_GOSU )
	{
		int iMaxCnt = m_ComboAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt + 1 ) )
			return;
	}
	else
	{
		int iMaxCnt = m_ComboAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			return;
	}

	if( m_bReserveJumpAttack )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_iCurComboCnt > 0 && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
	{
		return;
	}

	if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
	{
		return;
	}

	m_bReserveJumpAttack = true;
}

void ioChargeComboJump3::SetCuchulainJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
		
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioCuchulainItem *pCuchulainItem = ToCuchulainWeaponItem( pItem );

	if( !pCuchulainItem )
		return;

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	if( pOwner->IsNeedProcess() )
	{
		pCuchulainItem->CheckMiddleSearch( pOwner, false );
		m_stCuchulain_S_Attack_Target = pCuchulainItem->GetAimedTarget();

		pCuchulainItem->DecreseBullet();
	}
	else

		pCuchulainItem->SetAimedTarget( m_stCuchulain_S_Attack_Target );

	m_iCurComboCnt = 0;

	DWORD dwCurTime = FRAMEGETTIME();

	AttackAttribute* pAttack = &m_Cuchulain_S_Attack;
	m_ChargeComboState = CCS_CUCHULAIN_S_ATTACK;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( pAttack->m_AttackAnimation );
	float fTimeRate = pAttack->m_fAttackAniRate;
	DWORD dwPreDelay = pAttack->m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, pAttack->m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_Cuchulain_S_EndTime = dwCurTime + dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->IncreaseWeaponIndexBase();

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_bUseExtendJump = true;

	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_CUCHULAIN_S_ATTACK;
		kPacket << m_stCuchulain_S_Attack_Target;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::SetComboJumpAttack( ioBaseChar *pOwner )
{
	AttackAttribute* pAttack = NULL;
	if( m_bSecondJump && m_JumpExtendType == JET_STRIKER )
	{
		int iMaxCnt = m_ComboSecondAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			return;

		pAttack = &(m_ComboSecondAttackList[m_iCurComboCnt]);
	}
	else
	{
		int iMaxCnt = m_ComboAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			return;

		pAttack = &(m_ComboAttackList[m_iCurComboCnt]);
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_NORMAL;

	pOwner->ClearAttackFireTimeAndSkill();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	switch( m_JumpExtendType )	
	{
	case JET_LUCIFER_FLY:
		{
			ioWeaponItem *pWeapon = pOwner->GetWeapon();
			ioLuciferItem *pLuciferItem = ToLuciferItem( pWeapon );
			if( pLuciferItem )
				pLuciferItem->SetJumpLandAttackAttribute( m_ItemLandAttackInfo );
		}
		break;
	case JET_GEOMJE:
		{
			ioGeomjeItem* pGeomJe = ToGeomjeItem( pOwner->GetPriorityItem( SPT_JUMP ) );
			if( pGeomJe )
				pGeomJe->SetJumpAttack( pOwner, m_iCurComboCnt );
		}
		break;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( pAttack->m_AttackAnimation );
	float fTimeRate = pAttack->m_fAttackAniRate;
	DWORD dwPreDelay = pAttack->m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, pAttack->m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	DWORD dwInputDashS = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( 0 < dwInputDashS )
		m_dwInput_Dash_S_StartTime = dwStartTime + dwInputDashS;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	//RedHood
	if( m_JumpExtendType == JET_RED_HOOD )
		SetMacroTime( pGrp, iAniID, fTimeRate, pAttack->m_dwEndDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();
}

void ioChargeComboJump3::SetStrikerDashAddAttack( ioBaseChar *pOwner, ioBaseChar *pWounder )
{
	pOwner->SetJumpPower( m_fStrikerDashHitAddJumpPower );

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeComboState = CCS_STRIKER_MOVE_ADD_ATTACK;

	if( m_JumpExtendType == JET_STRIKER )
	{
		ioWeaponItem *pWeapon = pOwner->GetWeapon();
		ioStrikerItem *pStriker = ToStrikerItem( pWeapon );
		if( pStriker )
			pStriker->SetJumpLandAttackAttribute( m_ItemLandAttackInfo );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( m_StrkerJumpPowerAddAttack.m_AttackAnimation );
	float fTimeRate = m_StrkerJumpPowerAddAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_StrkerJumpPowerAddAttack.m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_StrkerJumpPowerAddAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pWounder && pWounder->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_STRIKER_MOVE_ADD_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
}

bool ioChargeComboJump3::CheckStrikerChareJumpAttack( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;
	float fMapHeight      = pStage->GetMapHeight( (float)pOwner->GetWorldPosition().x, (float)pOwner->GetWorldPosition().z );
	float fCap = pOwner->GetWorldPosition().y - fMapHeight;
	if( fCap >= m_fStrikerChargeAttackEnableHeight )
		return true;

	return false;
}

void ioChargeComboJump3::SetStrikerChargeJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();

	if( eDir != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, false );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	m_ChargeComboState = CCS_STRIKER_MOVE_ATTACK;
	m_bUseExtendJump = true;
	m_bUsedStrikerChargeAttack = true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->ClearReservedSliding();

	int iAniID = pGrp->GetAnimationIdx( m_StrkerJumpPowerAttack.m_AttackAnimation );
	float fTimeRate = m_StrkerJumpPowerAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_StrkerJumpPowerAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_StrkerJumpPowerAttack.m_dwEndDelay;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_StrkerJumpPowerAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwStrikerDashStartTime = dwCurTime;
	m_dwStrikerDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_StrkerJumpPowerAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false  );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_JUMP_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetMoveDir();
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::SetComboChargeJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ChargeComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_NONE;

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	if( m_JumpExtendType == JET_NAKORURU )
	{
		ioWeaponItem *pWeapon = pOwner->GetWeapon();
		ioNakoruruItem *pNakoruru = ToNakoruruItem( pWeapon );
		if( pNakoruru )
			pNakoruru->SetJumpLandAttackAttribute( m_ItemLandAttackInfo );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( m_ChargeComboAttackList[m_iCurComboCnt].m_AttackAnimation );
	float fTimeRate = m_ChargeComboAttackList[m_iCurComboCnt].m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargeComboAttackList[m_iCurComboCnt].m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_ChargeComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_ChargeComboAttackList[m_iCurComboCnt].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );
	
	SetAniRotate( pOwner, iAniID, fTimeRate );

	if( m_JumpExtendType == JET_RED_HOOD )
		SetMacroTime( pGrp, iAniID, fTimeRate, m_ChargeComboAttackList[m_iCurComboCnt].m_dwEndDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_NAKORURU_GRAP_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();
}

// double
void ioChargeComboJump3::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_ChargeComboState = CCS_DOUBLE;

	m_bUsedDoubleJump = true;
	m_bUseExtendJump = true;
	m_iCurJumpCnt++;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetJumpPower( m_fDoubleJumpPower );

	D3DXVECTOR3 vJumpDir = pOwner->GetMoveDir();
	if( m_fDoubleJumpForce > 0.0f )
		pOwner->SetForcePower( vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );
	else
		pOwner->SetForcePower( -vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	g_TutorialMgr.ActionDoubleJump();

	SetDoubleJumpAni( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DOUBLE_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << vJumpDir;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap, bool bCharge )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;
	if( !m_bEnableDoubleJump ) return false;
	if( m_bUsedHighJump ) return false;

	if( ( !bCharge && !pOwner->IsJumpKeyPress() ) ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() )
		return false;

	if( pOwner->GetUsedBuffJump() != BJUT_NONE) 
		return false;
	if(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) 
		return false;
	if(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE)
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_DOUBLE:
	case CCS_JUMP_CHARGE:
	case CCS_DEFENSE_CHARGE:
		break;
	default:
		return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

bool ioChargeComboJump3::IsCanJumpCharge( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	if( !pOwner->IsJumpKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_DOUBLE:
	case CCS_NORMAL:
		break;
	default:
		return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) 
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

bool ioChargeComboJump3::IsCanDefenseCharge( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	if( !pOwner->IsDefenseKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_DOUBLE:
	case CCS_NORMAL:
		break;
	default:
		return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) 
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

void ioChargeComboJump3::SetJumpChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_bUseExtendJump = true;

	if( m_ChargeComboState == CCS_NORMAL )
		m_ChargeComboState = CCS_JUMP_CHARGE2;
	else
		m_ChargeComboState = CCS_JUMP_CHARGE;

	m_dwJumpChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::SetDefenseChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_DEFENSE_CHARGE;
	m_dwDefenseChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::SetDoubleJumpAni( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	D3DXVECTOR3 vSightDir = pOwner->GetMoveDir();

	D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDoubleJumpAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDoubleJumpAniList[iIndex].IsEmpty() && !m_vDoubleJumpAniList[iIndex2].IsEmpty() )
		{
			if( fRate <= FLOAT05 )
				pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex], FLOAT100, FLOAT100, (1.0f - fRate), FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
			else
				pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex2], FLOAT100, FLOAT100, fRate, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else if( !m_vDoubleJumpAniList[iIndex].IsEmpty() && m_vDoubleJumpAniList[iIndex2].IsEmpty() )
		{
			pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex], FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else if( !m_vDoubleJumpAniList[iIndex2].IsEmpty() && m_vDoubleJumpAniList[iIndex].IsEmpty() )
		{
			pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex2], FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else
		{
			int iAniID = pOwner->GetJumppingAnimationIdx();
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
	}
	else
	{
		int iAniID = pOwner->GetJumppingAnimationIdx();
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
	}
}

// flash
bool ioChargeComboJump3::IsCanFlashJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !m_bEnableFlashJump ) return false;

	if( m_ChargeComboState == CCS_DOUBLE )
		return true;

	return false;
}

bool ioChargeComboJump3::IsCanSuperDimensionDash( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( !m_bEnableJumpDash ) 
		return false;

	if( !pOwner->IsNeedProcess() ) 
		return false;

	if( !m_bCanSuperDimentionDash )
		return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if(m_JumpExtendType != JET_SUPER_DIMENSION && m_ChargeComboState != CCS_NONE )
		return false;

	int iMaxCnt = m_ComboAttackList.size();
	if( m_iCurComboCnt >= iMaxCnt )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	if( m_JumpExtendType == JET_SUPER_DIMENSION && m_bLanded )
		return true;

	return false;
}

bool ioChargeComboJump3::SetFlashJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	if( !m_FlashDelayAni.IsEmpty() )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pOwner->SetAutoTarget( ATT_JUMP );

		int iAniID = pGrp->GetAnimationIdx( m_FlashDelayAni );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fFlashDelayRate );

		dwDelayTime = pGrp->GetAnimationFullTime( iAniID ) * m_fFlashDelayRate;

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * m_fFlashDelayRate;

		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_bUseExtendJump = true;
	pOwner->SetUsedFlashJump( true );

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );

	m_ChargeComboState = CCS_DELAY;
	m_dwFlashDelayEndTime = dwCurTime + dwDelayTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_iCurJumpCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	return true;
}

// jump dash
bool ioChargeComboJump3::IsCanJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !m_bEnableJumpDash )
		return false;

	if( !pOwner->IsNeedProcess() )
		return false;

	if( pOwner->IsHasCrown()								||
		pOwner->IsCatchMode()								||
		pOwner->IsPrisonerMode()							||
		(pOwner->GetUsedBuffJump() != BJUT_NONE )			||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE )		||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE )	||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if( m_JumpExtendType == JET_DONQUIXOTE )
	{
		if( m_bUsedDashJumpAttack )
			return false;
	}
	else if( ( m_JumpExtendType != JET_MURIM_GOSU && m_ChargeComboState != CCS_NONE ) || 
		( m_JumpExtendType == JET_MURIM_GOSU && m_bUsedDashJumpAttack ) )
		return false;

	ioAliceItem* pAlice = ToAliceItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAlice && pOwner->GetLimitChangeJumppingCnt() > 0 && pOwner->GetLimitChangeJumppingCnt() <= pOwner->GetCurChangeJumppingCnt() )
		return false;
	
	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

bool ioChargeComboJump3::IsJumpDashState()
{
	switch( m_ChargeComboState )
	{
	case CCS_DASH:
	case CCS_DASH_END:
	case CCS_LOOK_AT_DASH:
		return true;
	}

	return false;
}

bool ioChargeComboJump3::IsJumpNoDirectionKeyState()
{
	switch( m_ChargeComboState )
	{
	case CCS_STRIKER_MOVE_ATTACK:
	case CCS_STRIKER_MOVE_ADD_ATTACK:
		return true;
	}

	return false;
}

bool ioChargeComboJump3::IsJumpStrangerSpeialAttackState()
{
	switch( m_ChargeComboState )
	{
	case CCS_STRANGER_SPECIAL_ATTACK:
	case CCS_STRANGER_SPECIAL_LOOP:
		return true;
	}

	return false;
}

bool ioChargeComboJump3::IsStrikerDashState()
{
	switch( m_ChargeComboState )
	{
	case CCS_STRIKER_MOVE_ATTACK:
	case CCS_STRIKER_MOVE_END:
		return true;
	}

	return false;
}

bool ioChargeComboJump3::IsNoProcessGravity()
{
	switch( m_ChargeComboState )
	{
	case CCS_STRIKER_MOVE_ATTACK:
	case CCS_STRIKER_MOVE_END:
		{
			return true;
		}
		break;	
	case CCS_NO_DROP_EXTEND_ATTACK:
		{
			if( m_dwNoDropExtendAttackEndTime > FRAMEGETTIME() )
				return true;
		}
		break;
	}

	return false;
}

void ioChargeComboJump3::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_dwJumpChargingStartTime > 0 && CheckJumpCharging( pOwner ) )
		return;
	else if( m_dwDefenseChargingStartTime > 0 && CheckDefenseCharging( pOwner ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	CheckAniRotate( pOwner );

	switch( m_ChargeComboState )
	{
	case CCS_NORMAL:
		if( m_JumpExtendType != JET_MURIM_GOSU && IsCanDoubleJump( pOwner, fHeightGap, false ) )
		{
			if( m_fDoubleJumpPower >= 0.0f )
			{
				SetDoubleJump( pOwner );
				return;
			}
		}
		else if( m_JumpExtendType == JET_MURIM_GOSU )
		{
			if( IsCanJumpCharge( pOwner, fHeightGap ) )
			{
				SetJumpChargingState( pOwner );
				return;
			}
			else if( IsCanDefenseCharge( pOwner, fHeightGap ) )
			{
				SetDefenseChargingState( pOwner );
				return;
			}
			else if( CheckEnableMurimGosuCmdState( pOwner ) )
				return;
		}
		ProcessComboJumpAttack( pOwner );
		break;
	case CCS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case CCS_CUCHULAIN_S_ATTACK:
		ProcessCuchulain_S_Attack( pOwner );
		break;
	case CCS_EXTEND_ATTACK:
		PowerChargeMove( pOwner, pStage );		
		break;
	case CCS_DELAY:
		ProcessDelay( pOwner );
		break;
	case CCS_FLASH:
		ProcessFlashMove( pOwner );
		break;
	case CCS_FLASH_END:
		ProcessFlashEndMove( pOwner );
		break;
	case CCS_EXTEND_COMBO:
		ProcessExtendComboAttack( pOwner );
		break;
	case CCS_NONE:
		CheckAirJump( pOwner );
		break;
	case CCS_GHOST_STEALER:
	case CCS_GHOST_STEALER_ADD_ATTACK:
		ProcessGhostStealerAttack( pOwner );
		break;
	case CCS_STRIKER_MOVE_ATTACK:
		if( dwCurTime > m_dwStrikerDashEndTime )
			SetStrikerChargeJumpAttackEnd( pOwner );
		break;
	case CCS_STRIKER_MOVE_ADD_ATTACK:
		CheckAirJump( pOwner );
		break;
	case CCS_NO_DROP_EXTEND_ATTACK:
		NoDropExtendAttack( pOwner, pStage );
		break;
	case CCS_DASH:
	case CCS_DASH_END:
		ProcessJumpDash( pOwner );
		break;
	case CCS_BUBBLE_FIGHTER_SPECIAL_ATTACK:
		CheckAirJump( pOwner );
		break;
	case CCS_BUBBLE_FIGHTER_CHARGE_ATTACK:
		{
			if( IsCanJumpDashCombo( pOwner ) )
			{
				SetJumpDash( pOwner );
				return;
			}

			ioBubbleFighterItem *pBubbleFighter = ToBubbleFighterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( !pBubbleFighter )
				return;
			pBubbleFighter->ProcessBubbleFighterJumpAttack( pOwner, m_dwInput_Dash_S_StartTime );
		}
		break;
	case CCS_STRANGER_SPECIAL_ATTACK:
		ProcessStrangerSpecialAttack( pOwner );
		break;
	case CCS_STRANGER_SPECIAL_LOOP:
		ProcessStrangerSpecialLoop( pOwner );
		break;
	case CCS_JUMP_CHARGE:
	case CCS_JUMP_CHARGE2:
		CheckJumpCharging( pOwner );
		break;
	case CCS_DEFENSE_CHARGE:
		CheckDefenseCharging( pOwner );
		break;
	}

	if( pOwner->IsNeedProcess() && m_JumpExtendType == JET_SUPER_DIMENSION && m_dwEnableComboTime_E < dwCurTime )
	{
		if( IsCanSuperDimensionDash( pOwner) )
			SetSuperDimensionDashState( pOwner );
	}
	
	if( pOwner->IsNeedProcess() && m_ChargeComboState == CCS_NORMAL && m_dwInput_Dash_S_StartTime < dwCurTime )
	{
		if( IsCanJumpDashCombo( pOwner ) )
		{
			SetJumpDash( pOwner );
			return;
		}
	}
	
	//RedHood
	if( m_JumpExtendType == JET_RED_HOOD && pOwner->IsNeedProcess() && pOwner->IsDefenseKey() )
	{
		if( m_dwRedHoodSpecialMoveMacroTime_S && m_dwRedHoodSpecialMoveMacroTime_S < dwCurTime && m_dwRedHoodSpecialMoveMacroTime_E > dwCurTime )
		{
			ioRedHoodItem* pRedHoodItem = ToRedHoodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pRedHoodItem->IsEnableGauge() )
			{
				pRedHoodItem->SetSpecailMoveState( pOwner, true );
				return;
			}
		}
	}

	if( m_JumpExtendType == JET_CUCHULAIN_EXTEND && pOwner->IsNeedProcess() &&	m_ChargeComboState != CCS_CUCHULAIN_S_ATTACK &&
		pOwner->IsDefenseKey() && IsCanCuchulainJumpAttack( pOwner ) )
	{
		SetCuchulainJumpAttack( pOwner );
		return;
	}
}

bool ioChargeComboJump3::CheckJumpCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsJumpKeyDown() && !pOwner->IsHasCrown() && CheckEnableMurimGosuState( pOwner ) )
	{
		if( m_dwJumpChargingStartTime + m_dwChargingTime < dwCurTime )
		{
			m_dwJumpChargingStartTime = 0;
			SetMurimGosuState( pOwner, ioMurimGosuItem::SM_UP );
			return true;
		}
	}
	else
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( IsCanDoubleJump( pOwner, fHeightGap, true ) && m_fDoubleJumpPower > 0.0f )
		{
			SetDoubleJump( pOwner );
			return true;
		}
		else
		{
			m_ChargeComboState = CCS_NONE;
			m_dwJumpChargingStartTime = 0;
		}
	}

	return false;
}

bool ioChargeComboJump3::CheckDefenseCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsDefenseKeyDown() && !pOwner->IsHasCrown() && CheckEnableMurimGosuState( pOwner ) )
	{
		if( m_dwDefenseChargingStartTime + m_dwChargingTime < dwCurTime )
		{
			m_dwDefenseChargingStartTime = 0;
			SetMurimGosuState( pOwner, ioMurimGosuItem::SM_DOWN );
			return true;
		}
	}
	else
	{
		m_ChargeComboState = CCS_NONE;
		m_dwDefenseChargingStartTime = 0;
	}

	return false;
}

void ioChargeComboJump3::SetStrikerChargeJumpAttackEnd( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_fStrikerDasnEndJumpPower, FLOAT1, false, true, true );
	}
}

void ioChargeComboJump3::ProcessCuchulain_S_Attack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	CheckAirJump( pOwner );

	if( dwCurTime > m_Cuchulain_S_EndTime )
	{
		bool bReserveAttack = m_bReserveJumpAttack;
		if( bReserveAttack )
		{
			DWORD dwLooseTime = 0;

			if( pOwner )
				dwLooseTime = pOwner->GetActionStopTotalDelay();

			if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
			{
				SetChargingState( pOwner );
			}
		}
	}
}

void ioChargeComboJump3::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	CheckAirJump( pOwner );

	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( IsEnableInputJumpDashS( pOwner ) )
	{
		switch( m_JumpExtendType )
		{
		case JET_LOOK_AT_DASH:
			SetLookAtLockJumpDash( pOwner );
			break;
		}
		SetInputDashS( pOwner );
	}
	else if( m_bReserveJumpAttack )
	{
		DWORD dwLooseTime = 0;

		if( pOwner )
			dwLooseTime = pOwner->GetActionStopTotalDelay();

		if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
		{
			if ( m_JumpExtendType == JET_BUBBLE_FIGHTER )
			{
				if ( IsCanBubbleFighterAttack( pOwner ) )
					SetChargingState( pOwner );
			}
			else
				SetChargingState( pOwner );
		}
	}
	else if ( pOwner->IsNeedProcess() && dwCurTime >= m_dwEnableComboTime_S + dwLooseTime &&
		pOwner->IsDefenseKey() && m_bEnableDefenseAttack && fHeightGap > m_fDefenseAttackEnableHeight )
	{
		if( m_JumpExtendType == JET_STRANGER )
			ChangeToStrangerSpecialAttack( pOwner );
		else if( m_JumpExtendType == JET_SMILE_JOKER && !m_bUsedFly )
			ChangeToSmileJokerFlyState( pOwner );

		return;
	}
}

void ioChargeComboJump3::ProcessDelay( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFlashDelayEndTime > 0 && m_dwFlashDelayEndTime < dwCurTime )
	{
		m_ChargeComboState = CCS_FLASH;

		int iAniID = pGrp->GetAnimationIdx( m_FlashJumpAttack.m_AttackAnimation );
		float fTimeRate = m_FlashJumpAttack.m_fAttackAniRate;
		DWORD dwPreDelay = m_FlashJumpAttack.m_dwPreDelay;

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		

		pOwner->RefreshFireTimeList( iAniID, m_FlashJumpAttack.m_vWeaponInfoList,
									 FTT_JUMP_ATTACK,
									 fTimeRate,
									 dwPreDelay );

		pOwner->IncreaseWeaponIndexBase();

		m_dwFlashEndTime = dwCurTime + dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	}
}

void ioChargeComboJump3::ProcessFlashMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFlashEndTime > 0 && m_dwFlashEndTime < dwCurTime )
	{
		m_ChargeComboState = CCS_FLASH_END;

		pOwner->SetUsedFlashJump( false );
		pOwner->SetGravityAmt( 0.0f );

		pOwner->SetCharCollisionSkipTime( 0, 0 );
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}
}

void ioChargeComboJump3::ProcessFlashEndMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;
}

void ioChargeComboJump3::ProcessGhostStealerAttack(  ioBaseChar *pOwner  )
{
	DWORD dwCurTime = FRAMEGETTIME();
	CheckAirJump( pOwner );

	if( dwCurTime > m_dwFireEndTime )
	{
		m_ChargeComboState = CCS_NONE;
		return;
	}

	if( m_ChargeComboState == CCS_GHOST_STEALER)
	{
		if(	pOwner->IsNeedProcess() &&	dwCurTime > m_dwKeyReserveTime && pOwner->IsAttackKey() )
		{
			//추가타
			SetGhostStealerState( pOwner, true );
			return;
		}

		if( pOwner->IsNeedProcess() &&	dwCurTime > m_dwKeyReserveTime && pOwner->IsJumpKeyDown() )
		{
			ChangeToGhostStealerState( pOwner );
			return;
		}
	}
}

void ioChargeComboJump3::ChangeToGhostStealerState( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition();

	D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vLook, &ioMath::UNIT_Y );
	D3DXQUATERNION qtNewRot;
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(m_JumpGhostStealer.m_fCurAngle) );
	vLook = qtNewRot * vLook;
	D3DXVec3Normalize( &vLook, &vLook );
	vLook = vLook * m_JumpGhostStealer.m_fMoveLength;
	vTargetPos = vTargetPos + vLook;

	pOwner->SetGhostStealerState( m_JumpGhostStealer.m_Ani, 
		m_JumpGhostStealer.m_fAniRate, 
		vTargetPos, 
		m_JumpGhostStealer.m_fMoveSpeed, 
		m_JumpGhostStealer.m_fMoveRate, 
		m_JumpGhostStealer.m_fEndJumpAmt, m_JumpGhostStealer.m_fMoveHeightRate );

	if(	pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_GHOST_STEALER_STATE;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case CCS_NORMAL:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );
			pOwner->ApplyAutoTargetInfo( rkPacket );

			SetComboJumpAttack( pOwner );
			
		}
		break;
	case CCS_DOUBLE:
		{	
			D3DXQUATERNION qtRotate;			
			rkPacket >> qtRotate;
			pOwner->SetTargetRotToRotate( qtRotate, false );

			SetDoubleJump( pOwner );
			
		}
		break;
	case CCS_DELAY:
		{
			rkPacket >> m_iCurJumpCnt;

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;
			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRotate, vMoveDir, vInputDir );
			SetFlashJumpAttack( pOwner );
		}
		break;
	case CCS_DASH:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRotToRotate( qtRotate, false );
			SetJumpDash( pOwner );
		}
		break;
	case CCS_LOOK_AT_DASH:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRotToRotate( qtRotate, false );
			SetLookAtLockJumpDash( pOwner );
		}
		break;
	case CCS_DASH_END:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;
			pOwner->SetTargetRotToRotate( qtRotate, false );

			SetJumpDashEnd( pOwner );
		}
		break;
	case CCS_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChargingState( pOwner );
		}
		break;
	case CCS_EXTEND_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeToPowerChargeJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_BUFF_ADD:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetBuffAddState( pOwner );
		}
		break;
	case CCS_MECHANICS_FLY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetMechanicsFlyState( pOwner );
		}
		break;
	case CCS_EXCALIBUR:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			SetExcaliburSpecialAtt( pOwner );
		}
		break;
	case CCS_EXTEND_COMBO:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			bool bRefresh;
			rkPacket >> bRefresh;

			SetExtendComboState( pOwner, bRefresh );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_PHANTOM_FLY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetPhantomFlyState( pOwner );
		}
		break;
	case CCS_LUCIFER_FLY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetLuciferFlyState( pOwner );
		}
		break;
	case CCS_BLADE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetBladeFlyState( pOwner );
		}
		break;
	case CCS_DESTROYER:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetDestroyerFlyState( pOwner );
		}
		break;
	case CCS_NAKORURU_GRAP_ATTACK:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			SetComboChargeJumpAttack( pOwner );
		}
		break;
	case CCS_PETERPAN_FLY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetPeterpanFlyState( pOwner );
		}
		break;
	case CCS_GHOST_STEALER:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetGhostStealerState( pOwner, false );
		}
		break;
	case CCS_GHOST_STEALER_ADD_ATTACK:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetGhostStealerState( pOwner, true );
		}
		break;
	case CCS_GHOST_STEALER_STATE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeToGhostStealerState( pOwner );
		}
		break;
	case CCS_DEVIL_FLY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetDevilFlyState( pOwner );
		}
		break;
	case CCS_SUPER_DIMENSION_DASH:
		{
			D3DXQUATERNION qtRot;
			ioHashString stTarget;
			rkPacket >> qtRot;
			rkPacket >> stTarget;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetSuperDimensionDashState( pOwner, stTarget );
		}
		break;
	case CCS_STRIKER_MOVE_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetStrikerChargeJumpAttack( pOwner );
		}
		break;
	case CCS_STRIKER_MOVE_END:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetStrikerChargeJumpAttackEnd( pOwner );
		}
		break;
	case CCS_STRIKER_MOVE_ADD_ATTACK:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			SetStrikerDashAddAttack( pOwner, NULL );
		}
		break;
	case CCS_EQUIP_OBJECT:
		{
			SetObjectEquipState( pOwner );
		}
		break;
	case CCS_SHADOW_RANGER:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetShadowRangerMoveState( pOwner );
		}
		break;
	case CCS_NO_DROP_EXTEND_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			ChangeToNoDropExtendAttack( pOwner );
			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_CUCHULAIN_S_ATTACK:
		{
			rkPacket >> m_stCuchulain_S_Attack_Target;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			pOwner->SetWorldPosition( vPos );

			SetCuchulainJumpAttack(  pOwner );
		}
		break;
	case CCS_DRUID_EAGLE:
		{
			SetDruidTransformEagle( pOwner );
		}
		break;
	case CCS_VALKI_JUMP:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeToDoubleJumpFly( pOwner );
		}
		break;
	case CCS_BUBBLE_FIGHTER_SPECIAL_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetBubbleFighterSpecialAttack( pOwner );
		}
		break;
	case CCS_BUBBLE_FIGHTER_CHARGE_ATTACK:
		{
			ioBubbleFighterItem *pBubbleFighter = ToBubbleFighterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( !pBubbleFighter )
				return;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ioHashString szTarget;
			bool bAimedBall;
			rkPacket >> szTarget;
			rkPacket >> bAimedBall;

			m_ChargeComboState = CCS_BUBBLE_FIGHTER_CHARGE_ATTACK;
			m_bUseExtendJump = true;
			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );
			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
			m_bReserveJumpAttack = false;
			pOwner->SetJumpAttackState();

			pBubbleFighter->ApplyBubbleFighterJumpAttack( pOwner, m_dwInput_Dash_S_StartTime, szTarget, bAimedBall );

		}
		break;
	case CCS_STRANGER_SPECIAL_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			ChangeToStrangerSpecialAttack(pOwner);
		}
		break;
	case CCS_STRANGER_SPECIAL_LOOP:
		ChangeToStrangerSpecialLoop(pOwner);
		break;
	case CCS_STRANGER_SPECIAL_END:
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if ( !pGrp )
				return;
			int iAniID = pOwner->GetJumppingAnimationIdx();
			if( iAniID != -1 )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
			}

			if( m_dwStrangerSpecialAttackIndex > 0 )
			{
				ioWeapon *pWeapon =	g_WeaponMgr.FindWeapon( pOwner, m_dwStrangerSpecialAttackIndex );
				if ( pWeapon )
				{
					pWeapon->SetWeaponDead();
					m_dwStrangerSpecialAttackIndex = 0;
				}
			}

			m_ChargeComboState = CCS_NONE;
		}
		break;
	case CCS_MEGA_AGENT_FLY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			SetMegaAgentFlyState( pOwner );
		}
		break;
	case CCS_SMILE_JOKER_FLY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			ChangeToSmileJokerFlyState( pOwner );
		}
		break;
	case CCS_CHANGE_ON:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChangeOnGriffinState( pOwner );
		}
		break;
	case CCS_JUMP_CHARGE:
	case CCS_JUMP_CHARGE2:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetJumpChargingState( pOwner );
		}
		break;
	case CCS_DEFENSE_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetDefenseChargingState( pOwner );
		}
		break;
	}
}

void ioChargeComboJump3::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_bReserveJumpAttack = false;
	m_bUsedStrikerChargeAttack = false;
	m_bUsedDashJumpAttack = false;
	m_bUseExtendJumpAttack = false;

	m_bCanSuperDimentionDash = true;

	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	m_bLanded = true;
	m_bSecondJump = false;
	m_bNoDropExtendAttack = false;
	m_bUsedFly = false;

	m_iAddJumpCnt = 0;

	if( m_dwStrangerSpecialAttackIndex > 0 )
	{
		ioWeapon *pWeapon =	g_WeaponMgr.FindWeapon( pOwner, m_dwStrangerSpecialAttackIndex );
		if ( pWeapon )
		{
			pWeapon->SetWeaponDead();
			m_dwStrangerSpecialAttackIndex = 0;
		}
	}

	ioJinrahItem *pJinrahItem = ToJinrahItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pJinrahItem )
		pJinrahItem->ClearJumpSpecial();

	ioMurimGosuItem *pMurim = ToMurimGosuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMurim )
		pMurim->ClearSpecialMoveCount();
}

float ioChargeComboJump3::GetCurFlashForceAmt()
{
	switch( m_ChargeComboState )
	{
	case CCS_FLASH:
		return m_fFlashForce;
	}

	return 0.0f;
}

float ioChargeComboJump3::GetCurFlashGravityAmt()
{
	switch( m_ChargeComboState )
	{
	case CCS_FLASH:
		return m_fFlashGravityAmt;
	}

	return 0.0f;
}

bool ioChargeComboJump3::IsNoProcessMove()
{
	switch( m_ChargeComboState )
	{
	case CCS_NO_DROP_EXTEND_ATTACK:
		{
			if( m_dwNoDropExtendAttackEndTime > FRAMEGETTIME() )
				return true;
		}
		break;
	}

	return false;
}

float ioChargeComboJump3::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
{
	D3DXVECTOR3 vCurDir = vDir;

	D3DXVECTOR3 vXZDir( vCurDir.x, 0.0f, vCurDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;

	int iTest = (int)fAngleGap % 360;
	iTest /= 45;

	if( m_ChargeComboState == CCS_DOUBLE )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return m_fDoubleJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fDoubleJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fDoubleJumpSpeedRateBG;
		}
	}
	else if( m_bUseExtendJump )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return m_fJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fJumpSpeedRateBG;
		}
	}

	return m_fJumpSpeedRate;
}

bool ioChargeComboJump3::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_DELAY:
	case CCS_FLASH:
	case CCS_FLASH_END:
	case CCS_DASH:
	case CCS_DASH_END:
	case CCS_NEW_JUMP:
	case CCS_LOOK_AT_DASH:
		return false;
	}

	return true;
}

DWORD ioChargeComboJump3::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

void ioChargeComboJump3::SetChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_ChargingAniList.empty() )
		return;

	pOwner->ClearAttackFireTimeAndSkill();

	DWORD dwCurTime = FRAMEGETTIME();
	ioHashString szCurChargingAni;

	int iChargingCnt = m_ChargingAniList.size();
	if( COMPARE( m_iCurComboCnt, 0, iChargingCnt ) )
		szCurChargingAni = m_ChargingAniList[m_iCurComboCnt];
	else
		szCurChargingAni = m_ChargingAniList[0];

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );
	float fTimeRate = (float)m_dwChargingTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_ChargeComboState = CCS_CHARGE;
	m_dwChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && !m_bUsedStrikerChargeAttack )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			switch( m_JumpExtendType )
			{
			case JET_BUFF_ADD:
				if( CheckEnableBuffAddState( pOwner ) )
					SetBuffAddState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_MECHANICS_FLY:
				if( CheckEnableMechanicsFlyState( pOwner ) )
					SetMechanicsFlyState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_DESTROYER:
				if( CheckEnableDestroyerFlyState( pOwner ) )
					SetDestroyerFlyState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_EXCALIBUR:
				if( CheckEnableExcaliburSpecialAtt( pOwner ) )
					SetExcaliburSpecialAtt( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_EXTEND_COMBO:
				if( CheckEnableExtendCombo( pOwner ) )
				{
					m_iCurComboCnt = 0;
					SetExtendComboState( pOwner, true );
				}
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_PHANTOM_FLY:
				if( CheckEnablePhantomFlyState( pOwner ) )
					SetPhantomFlyState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_LUCIFER_FLY:
				if( CheckEnableLuciferFlyState( pOwner ) )
					SetLuciferFlyState( pOwner );
				else 
					SetComboJumpAttack( pOwner );
				return;
			case JET_NAKORURU:
				SetComboChargeJumpAttack( pOwner );
				return;
			case JET_PETERPAN:
				if( CheckEnablePeterpanFlyState( pOwner ) )
					SetPeterpanFlyState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_GHOST_STEALER:
				if( CheckEnableGhostStealerState( pOwner ) )
					SetGhostStealerState( pOwner, false );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_DEVIL:
				if( CheckEnableDevilFlyState( pOwner) )
					SetDevilFlyState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_SUPER_DIMENSION:
				if( CheckEnableSuperDimensionFireState( pOwner ) )
					SetSuperDimensionFireState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_STRIKER:
				if( !m_bUsedStrikerChargeAttack && CheckStrikerChareJumpAttack( pOwner ) )
				{
					SetStrikerChargeJumpAttack( pOwner );
					return;
				}
				break;
			case JET_EQUIP_OBJECT:
				if( CheckEnableEquipObjectItemState( pOwner ) )
					SetObjectEquipState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_SHADOW_RANGER:
				if( CheckEnableShadowRangerState( pOwner ) )
					SetShadowRangerMoveState( pOwner );
				else 
					SetComboJumpAttack( pOwner );
				return;
			case JET_JUMPER:
				if( CheckEnableJumperAtt( pOwner ) )
					SetJumperAtt( pOwner );
				else 
					SetComboJumpAttack( pOwner );
				return;
			case JET_HADES:
				if( CheckEnableHadesState( pOwner) )
					SetHadesFullChargeState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_BLADE:
				if( CheckEnableBladeFlyState( pOwner ) )
					SetBladeFlyState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_RED_HOOD:
				SetComboChargeJumpAttack( pOwner );
				return;
			case JET_NO_DROP_EXTEND:
				if( CheckEnableDropExtendAttack( pOwner ) )
					ChangeToNoDropExtendAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_CROW:
				if( CheckEnableCrowExtendAttacck(pOwner) )
					ChangeToNoDropExtendAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_MIR_EXTEND:
				if( CheckEnableMirExtendState( pOwner ) )
					ChangeToMirExtendJumpAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_ONIMUSHA:
				if( CheckEnableEquipObjectItemState( pOwner ) )
					SetOniMushaObjectEquipState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_DRUID_EAGLE:
				if( CheckDruidTransformEagleState( pOwner ) )
				{
					SetDruidTransformEagle( pOwner );
				}
				else
				{
					if( pOwner->IsEnableExtendJump() && 0 < m_dwHalfChargingTime )
						ChangeToPowerChargeJumpAttack( pOwner );
					else
						SetComboJumpAttack( pOwner );
				}
				return;
			case JET_PREDATOR:
				if( CheckEnablePredatorState( pOwner ) )
					SetPredatorState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_WITCH_DOCTOR:
				{
					if( pOwner->IsEnableExtendJump() )				
						ChangeToPowerChargeJumpAttack( pOwner );
					else
						SetComboJumpAttack( pOwner );

					if ( CheckEnableWitchDoctorPredatorState( pOwner ) )
						SetWitchDoctorCallDummy( pOwner );
					return;
				}
			case JET_BUBBLE_FIGHTER:
				{
					if( CheckEnableBubbleFighterChargeAttack( pOwner ) )
						SetBubbleFighterChargeAttack( pOwner );
				}
				return;
			case JET_WARP:
				if( CheckEnableWarpState( pOwner ) )
					SetWarpState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_DRILL:
				if( CheckEnableDrillChargeAttackState( pOwner ) )
					SetDrillChargeAttackState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_STRANGER:
				if ( CheckEnableStrangerChargeAttackState( pOwner ) )
					SetStrangerChargeAttackState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_GARGOYLE:
				if ( CheckEnableGargoyleSpecialState( pOwner ) )
					SetGargoyleSpecialState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_JINRAH:
				if( CheckJinrahChargeAttack( pOwner, false ) )
					ChangeToJinrahChargeAttack( pOwner, false );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_MEGA_AGENT:
				if( CheckEnableMegaAgentFlyState( pOwner ) )
					SetMegaAgentFlyState( pOwner );
				else 
					SetComboJumpAttack( pOwner );
				return;
			case JET_CHANGE_ON:
				if( CheckChangeOnGriffinState( pOwner ) )
					SetChangeOnGriffinState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_MURIM_GOSU:
				{
					int iMaxCnt = m_ComboAttackList.size();
					if( CheckEnableMurimGosuState( pOwner ) )
						SetMurimGosuState( pOwner, ioMurimGosuItem::SM_FRONT );
					else if( m_bUsedDashJumpAttack && pOwner->IsEnableExtendJump() )				
						ChangeToPowerChargeJumpAttack( pOwner );
					else if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
						SetComboJumpAttack( pOwner );
					else
						m_ChargeComboState = CCS_NONE;
				}
				return;
			case JET_GHOST:
				if( CheckGhostState( pOwner ) )
					SetGhostState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			}

			if( pOwner->IsEnableExtendJump() )				
				ChangeToPowerChargeJumpAttack( pOwner );
			else
				SetComboJumpAttack( pOwner );
		}
	}
	else
	{
		if( m_dwChargingStartTime+m_dwHalfChargingTime < dwCurTime )
		{
			switch( m_JumpExtendType )
			{
			case JET_HADES:
				if( CheckEnableHadesState( pOwner) )
					SetHadesHalfChargeState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_ONIMUSHA:
				if( CheckEnableOniMushaHalfChargeState( pOwner ) )
					ChangeToPowerChargeJumpAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_DRUID_EAGLE:
				if( pOwner->IsEnableExtendJump() && 0 < m_dwHalfChargingTime )
					ChangeToPowerChargeJumpAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_JINRAH:
				if( CheckJinrahChargeAttack( pOwner, true ) )
					ChangeToJinrahChargeAttack( pOwner, true );
				else
					SetComboJumpAttack( pOwner );
				return;
			}
		}
		int iMaxCnt = m_ComboAttackList.size();
		if( m_bSecondJump && m_JumpExtendType == JET_STRIKER )
			iMaxCnt = m_ComboSecondAttackList.size();
		if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			SetComboJumpAttack( pOwner );
		else
			m_ChargeComboState = CCS_NONE;

		return;
	}
}

int ioChargeComboJump3::GetCurChargeComboState()
{
	return m_ChargeComboState;
}

void ioChargeComboJump3::CuchulainChargeBuffAttack( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioCuchulainItem *pCuchulainItem = ToCuchulainWeaponItem( pItem );
	if( pCuchulainItem )
		pCuchulainItem->ChargeBuffSearch( pOwner, false );
}

void ioChargeComboJump3::ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTEND_JUMP_ATTACK );
	if( !pAttr )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	switch( m_JumpExtendType )	
	{
	case JET_GEOMJE:
		{
			ioGeomjeItem* pGeomJe = ToGeomjeItem( pOwner->GetPriorityItem( SPT_JUMP ) );
			if( pGeomJe )
				pGeomJe->SetJumpExtendAttack( pOwner, 0 );
		}
		break;
	case JET_ONIMUSHA:
		{
			ioOniMushaItem* pOniMushaItem = ToOniMushaItem( pOwner->GetWeapon() );
			if ( pOniMushaItem )
				pOniMushaItem->SetUseExtraGauge( m_fOnimushaChargeUseGauge );
		}
		break;
	case JET_ONIMUSHA_OBJECT:
		{
			ioOniMushaItem* pOniMushaItem = ToOniMushaItem( pOwner->GetWeapon() );
			if( pOniMushaItem )
				pOniMushaItem->SetUseExtraGauge( m_fOnimushaObjectChargeUseGauge );
		}
		break;
	case JET_WINDGOD:
		{
			ioWindGodItem* pWindGodItem = ToWindGodItem( pOwner->GetWeapon() );
			if( pWindGodItem )
				pWindGodItem->DecreseChargeJumpAttackGauge();
		}
		break;
	}

	pOwner->SetAutoTarget( ATT_JUMP );

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	if( !m_bUseLandJumpAttack && m_JumpExtendType != JET_CUCHULAIN_EXTEND )
	{
		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
	}
	else if( m_JumpExtendType == JET_CUCHULAIN_EXTEND )
	{
		CuchulainChargeBuffAttack( pOwner );
	}


	m_ChargeComboState		= CCS_EXTEND_ATTACK;
	m_bUsedChargeAttack		= true;
	m_bUseExtendJumpAttack	= true;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	float fCurGravity = pOwner->GetCharGravity();
	float fFlightDuration = GetFlightDuration( fCurGravity );
	if( fFlightDuration > 0.0f )
	{
		m_fCurCharMoveSpeed = m_fChargeBaseRange / (fFlightDuration / FLOAT1000);
	}
	else
	{
		m_fCurCharMoveSpeed = 0.0f;
	}

	m_dwFlightTime = (DWORD)fFlightDuration;

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir.y = 0.0f;
	D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
	pOwner->SetTargetRotAndMoveDirChange( qtRot, false );
	pOwner->SetWorldOrientation( qtRot );
	pOwner->SetJumpPower( m_fChargeJumpPower );
	pOwner->NotifyExtendJumpUse();

	//RedHood
	if( m_JumpExtendType == JET_RED_HOOD )
		SetMacroTime( pGrp, iAniID, fTimeRate, pAttr->m_dwEndDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

float ioChargeComboJump3::GetFlightDuration( float fCurGravity )
{
	float fTopTime, fTopHeight;
	fTopTime = fTopHeight = 0.0f;

	float fNewGravity = fCurGravity * m_fChargeGravityRate;
	if( fNewGravity > 0.0f )
	{
		fTopTime = m_fChargeJumpPower / fNewGravity;
		fTopHeight = (m_fChargeJumpPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fResult = 2 * fTopTime * FLOAT1000;

	return fResult;
}

void ioChargeComboJump3::PowerChargeMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_ChargeComboState != CCS_EXTEND_ATTACK )
		return;

	if( !pChar ) return;
	if( !pStage ) return;
	if( m_fCurCharMoveSpeed == 0.0f ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( m_bExtendAttackUseAirPower )
		CheckAirJump( pChar );

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

bool ioChargeComboJump3::IsChargeAttackState()
{
	return m_bUsedChargeAttack;
}

bool ioChargeComboJump3::CheckEnableBuffAddState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_JumpExtendType != JET_BUFF_ADD )
		return false;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioBuffAddItem *pBuffAddItem = ToBuffAddItem( pItem );
	if( !pBuffAddItem )
		return false;

	if( pBuffAddItem->GetCurBullet() >= pBuffAddItem->GetMaxBullet() )
		return true;

	return false;
}

void ioChargeComboJump3::SetBuffAddState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioBuffAddItem *pBuffAddItem = ToBuffAddItem( pItem );
	if( !pBuffAddItem )
		return;

	if( pOwner->IsNeedProcess() )
	{
		pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), false, false );

		if( pOwner->IsCanRotateJumpAttack() )
		{
			ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
			if( eDir != ioUserKeyInput::DKI_NONE )
			{
				float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				pOwner->SetTargetRotToRotate( qtRotate, false, false );
				pOwner->SetInputDirection( pOwner->GetMoveDir() );
			}
		}
	}

	// change state & motion
	pOwner->SetWereWolfState( m_BuffAddAttribute.m_AttackAnimation, m_BuffAddAttribute.m_fAttackAniRate );

	pBuffAddItem->SetCurBullet( 0 );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_BuffAddAttribute.m_AttackAnimation );
	float fTimeRate = m_BuffAddAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_BuffAddAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, m_BuffAddAttribute.m_dwPreDelay );
	pOwner->SetReservedSliding( m_BuffAddAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add buff
	int iBuffCnt = m_vBuffAddList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vBuffAddList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), pBuffAddItem->GetName(), NULL );
	}

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_BUFF_ADD;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckEnableDevilFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->IsEnableDevilFly() )
		return true;

	return false;
}

void ioChargeComboJump3::SetDevilFlyState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->ClearAttackFireTimeAndSkill();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->SetDevilFlyState( m_JumpDevilFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_DEVIL_FLY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckEnableLuciferFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->IsEnableLuciferFly() )
		return true;

	return false;
}

void ioChargeComboJump3::SetLuciferFlyState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->ClearAttackFireTimeAndSkill();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->SetLuciferFlyState( m_JumpLuciferFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_LUCIFER_FLY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckEnableMechanicsFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->IsEnableMechanicsFly() )
		return true;

	return false;
}

void ioChargeComboJump3::SetMechanicsFlyState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->SetMechanicsFlyState( m_JumpMechanicsFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_MECHANICS_FLY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckEnableDestroyerFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;


	if( pOwner->IsEnableDestroyerFly() )
		return true;

	return false;
}

void ioChargeComboJump3::SetDestroyerFlyState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->SetDestroyerFlyState( m_JumpDestroyerFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_DESTROYER;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckEnablePeterpanFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->IsEnablePeterpanFly() )
		return true;

	return false;
}

void ioChargeComboJump3::SetPeterpanFlyState( ioBaseChar *pOwner )
{
	if( !pOwner)
		return;

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->SetPeterpanFlyState( m_JumpPeterpanFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_PETERPAN_FLY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckEnableExcaliburSpecialAtt( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioExcaliburItem *pExcalibur = ToExcaliburItem(pOwner->GetPriorityItem( SPT_JUMP ));
	if( pExcalibur && pExcalibur->IsEnableSpcialAttack() )
		return true;

	return false;
}

void ioChargeComboJump3::SetExcaliburSpecialAtt( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioExcaliburItem *pExcalibur = ToExcaliburItem(pOwner->GetPriorityItem( SPT_JUMP ));
	if( !pExcalibur )
		return;

	int iCurCnt = pExcalibur->GetCurBullet();
	int iIndex = iCurCnt - 1;
	if( !COMPARE( iIndex, 0, (int)m_vExcaliburAttList.size() ) )
		return;

	AttackAttribute &rkAttribute = m_vExcaliburAttList[iIndex];

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	D3DXVECTOR3 vForceDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vForceDir, &vForceDir );

	if( m_fExcaliburForceAmt > 0.0f )
		pOwner->SetForcePower( vForceDir, fabs(m_fExcaliburForceAmt), FLOAT1, true );
	else if( m_fExcaliburForceAmt < 0.0f )
		pOwner->SetForcePower( -vForceDir, fabs(m_fExcaliburForceAmt), FLOAT1, true );
	
	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );
	float fTimeRate = rkAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, rkAttribute.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_iCurComboCnt = 0;
	m_bUsedExcaliburAtt = true;
	m_ChargeComboState = CCS_EXCALIBUR;

	pOwner->SetJumpPower( m_fChargeJumpPower );

	pExcalibur->DecreaseExtraNeedGauge( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
}

bool ioChargeComboJump3::CheckEnableExtendCombo( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_ExtendAttackList.empty() )
		return false;

	return true;
}

void ioChargeComboJump3::SetExtendComboState( ioBaseChar *pOwner, bool bRefresh )
{
	int iMaxCnt = m_ExtendAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_EXTEND_COMBO;

	float fGravityAmt = 0.0f;
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	if( pItem )
	{
		fGravityAmt = pItem->GetNewJumpAttackGravity();
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = false;

	pOwner->SetCurJumpPowerAmt( 0.0f );

	if( bRefresh )
	{
		pOwner->SetGravityAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
	}

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( m_ExtendAttackList[m_iCurComboCnt].m_AttackAnimation );
	float fTimeRate = m_ExtendAttackList[m_iCurComboCnt].m_fAttackAniRate;
	DWORD dwPreDelay = m_ExtendAttackList[m_iCurComboCnt].m_dwPreDelay;
	DWORD dwEndDelay = m_ExtendAttackList[m_iCurComboCnt].m_dwEndDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_ExtendAttackList[m_iCurComboCnt].m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_ExtendAttackList[m_iCurComboCnt].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	pOwner->SetNewJumpAttackStateInfo( m_dwEnableComboTime_E, m_dwEnableComboTime_S, fGravityAmt, bRefresh );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << bRefresh;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurComboCnt++;
}

bool ioChargeComboJump3::CheckEnablePhantomFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->IsEnablePhantomFly() )
		return true;

	return false;
}

void ioChargeComboJump3::SetPhantomFlyState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->SetPhantomFlyStateAttack();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_PHANTOM_FLY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::ProcessExtendComboAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKey() )
		return;

	CheckExtendComboAttack( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;
	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_bReserveJumpAttack )
	{
		if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
		{
			SetExtendComboState( pOwner, false );
		}
	}
}

void ioChargeComboJump3::CheckExtendComboAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int iMaxCnt = m_ExtendAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	if( m_bReserveJumpAttack )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_iCurComboCnt > 0 && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
		return;

	if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
		return;

	m_bReserveJumpAttack = true;
}

void ioChargeComboJump3::OnEndExtendJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}
	if( m_dwStrangerSpecialAttackIndex > 0 )
	{
		ioWeapon *pWeapon =	g_WeaponMgr.FindWeapon( pOwner, m_dwStrangerSpecialAttackIndex );
		if ( pWeapon )
		{
			pWeapon->SetWeaponDead();
			m_dwStrangerSpecialAttackIndex = 0;
		}
	}
}

int ioChargeComboJump3::GetJumpDashAni( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	D3DXVECTOR3 vSightDir = pOwner->GetMoveDir();

	D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw	= RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );
	fCurYaw		= RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw		= ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iGap = (int)fAngleGap % 360;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fRate		= 0.0f;
	int iCurrIndex	= -1;
	int iNextIndex	= -1;

	iCurrIndex		= iGap / 90;
	iGap			= iGap % 90;

	int iCnt = m_vLookAtLockDashAniList.size();
	if( COMPARE( iCurrIndex, 0, iCnt ) )
	{
		fRate		= (float)iGap /FLOAT90;
		iNextIndex	= (iCurrIndex+1) % iCnt;

		if( !m_vLookAtLockDashAniList[iCurrIndex].IsEmpty() && !m_vLookAtLockDashAniList[iNextIndex].IsEmpty() )
		{
			if( fRate <= FLOAT05 )
				return pGrp->GetAnimationIdx( m_vLookAtLockDashAniList[iCurrIndex] );
			else
				return pGrp->GetAnimationIdx( m_vLookAtLockDashAniList[iNextIndex] );
		}
		else if( !m_vLookAtLockDashAniList[iCurrIndex].IsEmpty() && m_vLookAtLockDashAniList[iNextIndex].IsEmpty() )
		{
			return pGrp->GetAnimationIdx( m_vLookAtLockDashAniList[iNextIndex] );
		}
		else if( !m_vLookAtLockDashAniList[iNextIndex].IsEmpty() && m_vLookAtLockDashAniList[iCurrIndex].IsEmpty() )
		{
			return pGrp->GetAnimationIdx( m_vLookAtLockDashAniList[iNextIndex] );
		}
		else
		{
			return pOwner->GetJumppingAnimationIdx();
		}
	}
	else
	{
		return pOwner->GetJumppingAnimationIdx();
	}
}

void ioChargeComboJump3::SetJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	if( eDir != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, false );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	m_ChargeComboState	= CCS_DASH;
	m_bUseExtendJump	= true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
		
	int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
	float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
								 m_DashJumpAttack.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwJumpDashStartTime = dwCurTime;
	m_dwJumpDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );


	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	switch( m_JumpExtendType )	
	{
	case JET_GEOMJE:
		{
			ioGeomjeItem* pGeomJe = ToGeomjeItem( pOwner->GetPriorityItem( SPT_JUMP ) );
			if( pGeomJe )
				pGeomJe->SetJumpDashAttack( pOwner, 0 );
		}
		break;
	}

	//RedHood
	if( m_JumpExtendType == JET_RED_HOOD )
		SetMacroTime( pGrp, iAniID, fTimeRate, m_DashJumpAttack.m_dwEndDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_JUMP_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetMoveDir();
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bUsedDashJumpAttack = true;

	if( m_bUseNewJumpTypeDash )
	{
		DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
		SetNewJumpTypeDashAttack( pOwner, m_dwJumpDashEndTime, dwKeyReserveTime );
		return;
	}
}

void ioChargeComboJump3::SetLookAtLockJumpDash( ioBaseChar *pOwner )
{
	m_ChargeComboState	= CCS_LOOK_AT_DASH;
	m_bUseExtendJump	= true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
		
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	if( eDir != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetMoveDirByRotate( qtRotate );
		if( m_bLookAtLocInputDirRotate )
		{
			pOwner->SetTargetRotToRotate( qtRotate, true, true );
		}
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = GetJumpDashAni( pOwner );
	float fTimeRate = m_fLookAtLockDashAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	if( iAniID != -1 )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

		DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

		m_dwJumpDashStartTime	= FRAMEGETTIME();
		m_dwJumpDashEndTime		= m_dwJumpDashStartTime + dwDuration;

		pOwner->SetReservedSliding( m_vLookAtLockDashForceInfoList, iAniID, fTimeRate, 0 );

		DWORD dwKeyReserveTime = FRAMEGETTIME() + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
		pOwner->SetNewJumpAttackStateInfo( m_dwJumpDashEndTime, dwKeyReserveTime, m_fJumpDashGravityAmt );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_JUMP_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetMoveDir();
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::SetJumpDashEnd( ioBaseChar *pOwner )
{
	m_dwJumpDashEndTime = 0;
	m_ChargeComboState = CCS_DASH_END;

	int iAniID = pOwner->GetJumppingAnimationIdx();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_JUMP_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetMoveDir();
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::ProcessJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( m_ComboDashType )
	{
	case CDT_NO_GRAVITY:
		ProcessJumpDashByNoGravity( pOwner );
		break;
	}
}

void ioChargeComboJump3::ProcessJumpDashByNoGravity( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeComboState )
	{
	case CCS_DASH:
		if( m_dwJumpDashEndTime > 0 && m_dwJumpDashEndTime < dwCurTime )
		{
			SetJumpDashEnd( pOwner );
		}
		break;
	case CCS_DASH_END:
		break;
	}
}

void ioChargeComboJump3::SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime )
{
	if( !pOwner ) return;

	m_bUseExtendJump = false;
	m_ChargeComboState = CCS_NEW_JUMP;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, m_fJumpDashGravityAmt );
}

float ioChargeComboJump3::GetCurGravityAmt()
{
	if( m_ChargeComboState == CCS_DASH || m_ChargeComboState == CCS_LOOK_AT_DASH )
	{
		if( m_ComboDashType == CDT_GRAVITY )
			return m_fJumpDashGravityAmt;
		else if( m_ComboDashType == CDT_NO_GRAVITY )
			return -FLOAT1;
	}

	if( m_ChargeComboState == CCS_EXTEND_ATTACK && m_bUseLandJumpAttack && m_bUseExtendJumpAttack )
		return m_fGravityAmt;

	if ( m_ChargeComboState == CCS_STRANGER_SPECIAL_ATTACK || m_ChargeComboState == CCS_STRANGER_SPECIAL_LOOP )
		return m_fStrangerSpeicialGravityAmt;

	return 0.0f;
}

bool ioChargeComboJump3::IsEnableJumpLandEndDash()
{
	if( m_ChargeComboState == CCS_DASH || m_ChargeComboState == CCS_DASH_END )
		return true;

	return false;
}

bool ioChargeComboJump3::IsJumpAttackReserve()
{
	return false;
}

bool ioChargeComboJump3::CheckEnableSuperDimensionFireState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioSuperDimensionItem *pSuperDimensionItem = ToSuperDimensionItem( pItem );

	if( pSuperDimensionItem )
	{
		if( pOwner->IsHasCrown() )
			return false;

		if( pSuperDimensionItem->GetCurBullet() < pSuperDimensionItem->GetNeedBullet() )
			return false;

		return true;
	}

	return false;
}


void ioChargeComboJump3::SetSuperDimensionFireState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->ClearAttackFireTimeAndSkill();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioSuperDimensionItem *pSuperDimensionItem = ToSuperDimensionItem( pItem );

	//패킷은 아이템 쪽에서 보낸다.
	if( pSuperDimensionItem )
		pSuperDimensionItem->SetSuperDimensionFireState( pOwner, true );

	m_bCanSuperDimentionDash = true;
}

void ioChargeComboJump3::SetSuperDimensionDashState( ioBaseChar *pOwner,ioHashString stTarget )
{
	if( !pOwner ) return;

	m_bCanSuperDimentionDash = false;

	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioSuperDimensionItem *pSuperDimensionDashItem = ToSuperDimensionItem( pWeapon );
	if( !pSuperDimensionDashItem )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();

	if( pOwner->IsNeedProcess() && eDir != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, false );
	}

	if( pOwner->IsNeedProcess() )
		stTarget = pSuperDimensionDashItem->FindDashAttTarget( pOwner ) ;

	m_bUseExtendJump = true;
	
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_ChargeComboState = CCS_SUPER_DIMENSION_DASH;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pSuperDimensionDashItem->SetSuperDimensionDashGrapState( pOwner, stTarget );

	m_bLanded = false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_SUPER_DIMENSION_DASH;
		kPacket << pOwner->GetTargetRot();
		kPacket << stTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckEnableGhostStealerState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioGhostStealer		*pGhostStealerItem = ToGhostStealerItem( pItem );

	if( pGhostStealerItem )
	{
		if( pOwner->IsHasCrown() )
			return false;

		if( pGhostStealerItem->GetCurBullet() < pGhostStealerItem->GetNeedBullet() )
			return false;

		return true;
	}

	return false;
}

void ioChargeComboJump3::SetGhostStealerState( ioBaseChar *pOwner, bool bAddAttack )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioGhostStealer		*pGhostStealerItem = ToGhostStealerItem( pItem );

	if( !pGhostStealerItem )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_GHOST_STEALER;
	if( bAddAttack )
		m_ChargeComboState = CCS_GHOST_STEALER_ADD_ATTACK;
	else
		pGhostStealerItem->DecreaseCoolTime();

	pOwner->ClearAttackFireTimeAndSkill();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( bAddAttack ? 
		m_JumpGhostStealer.m_AddExtendAttackAttribute.m_AttackAnimation : 
		m_JumpGhostStealer.m_ExtendAttackAttribute.m_AttackAnimation );

	float fTimeRate  = bAddAttack ? m_JumpGhostStealer.m_AddExtendAttackAttribute.m_fAttackAniRate : m_JumpGhostStealer.m_ExtendAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = bAddAttack ? m_JumpGhostStealer.m_AddExtendAttackAttribute.m_dwPreDelay : m_JumpGhostStealer.m_ExtendAttackAttribute.m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwFireEndTime = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, bAddAttack ? 
		m_JumpGhostStealer.m_AddExtendAttackAttribute.m_vWeaponInfoList : 
		m_JumpGhostStealer.m_ExtendAttackAttribute.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwKeyReserveTime = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
}

bool ioChargeComboJump3::CheckEnableBladeFlyState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioBladeItem			*pBladeItem = ToBladeItem( pItem );
	if( pBladeItem )
	{
		if( pOwner->IsHasCrown() )
			return false;

		if( pBladeItem->IsEnableGauge())
			return true;
	}
	else
		return false;


	return false;
}

void ioChargeComboJump3::SetBladeFlyState( ioBaseChar *pOwner )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioBladeItem			*pBladeItem = ToBladeItem( pItem );
	if( !pBladeItem )
		return;

	pBladeItem->SetFlyState( pOwner, m_JumpBladeFly, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_BLADE;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckEnableHadesState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioHadesItem			*pHadesItem = ToHadesItem( pItem );

	if( pHadesItem )
	{
		if( pOwner->IsHasCrown() )
			return false;

		if( pHadesItem->IsEnableGauge() )
			return true;
	}
	else
		return false;
	
	return false;
}

void ioChargeComboJump3::SetHadesHalfChargeState( ioBaseChar *pOwner )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioHadesItem			*pHadesItem = ToHadesItem( pItem );

	if( !pHadesItem )
		return;

	HadesAttackInfo dummyInfo;
	dummyInfo.Init();

	pHadesItem->SetHadesHalfAttackState( pOwner, dummyInfo, true, true, true, true );
}

void ioChargeComboJump3::SetHadesFullChargeState( ioBaseChar *pOwner )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioHadesItem			*pHadesItem = ToHadesItem( pItem );

	if( !pHadesItem )
		return;

	HadesAttackInfo dummyInfo;
	dummyInfo.Init();

	pHadesItem->SetHadesHalfAttackState( pOwner, dummyInfo, true, true, true, false );
}

bool ioChargeComboJump3::CheckEnableEquipObjectItemState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;
		
	if( pOwner->IsEquipEnableObject() )
	{
		return true;
	}

	return false;
}

void ioChargeComboJump3::SetObjectEquipState( ioBaseChar *pOwner )
{	
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioAliceItem* pAlice = ToAliceItem( pItem );
	if( pAlice )
	{
		if( pAlice->GetCurBullet() < pAlice->GetNeedBullet() )
		{
			SetComboJumpAttack( pOwner );
		}
		else
		{
			pAlice->ChangeEquipObjectState( pOwner, ioAliceItem::EOS_JUMP );
		}
	}
}

bool ioChargeComboJump3::IsEnableJumpDash( ioBaseChar *pOwner )
{
	//주의
	//ioBaseChar에서 기본적으로 if( m_CurJumpType == JT_CHARGE_COMBO3 && m_JumpState == JS_JUMPPING ) 일때 true다.
	switch( m_JumpExtendType )
	{
	case JET_SUPER_DIMENSION:
	case JET_APOSTATE_MONK:
	case JET_MIR_EXTEND:
		return true;		
	}

	if( 0 < m_dwInput_Dash_S_StartTime )
		return true;

	return false;
}

void ioChargeComboJump3::SetNextJumpItemState( ioBaseChar *pOwner, ioBaseChar *pWounder, bool bDefense, ChangeNextItemType type, ioWeapon* pWeapon, float fDamage )
{
	if( m_JumpExtendType == JET_STRIKER )
	{
		if( type == CNIT_TYPE2 && bDefense == false )
		{
			if( fDamage > 0 )
				SetStrikerDashAddAttack( pOwner, pWounder );
			else if( pWeapon )
				pWeapon->SetWeaponDead();
		}
	}
}

bool ioChargeComboJump3::IsUseLandJumpAttack()
{
	if( m_bUseLandJumpAttack && m_bUseExtendJumpAttack )
		return true;

	return false;
}

//쉐도우레인저
void ioChargeComboJump3::LoadShadowRangerExtendInfo( ioINILoader &rkLoader )
{	
	m_fShadowRangerUseGauge = rkLoader.LoadFloat_e( "jump_shadow_ranger_use_gauge", 0.0f );
}

bool ioChargeComboJump3::CheckEnableShadowRangerState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	
	if( pOwner->IsEnableShadowRangerSpecial( m_fShadowRangerUseGauge ) )
		return true;

	return false;
}

void ioChargeComboJump3::SetShadowRangerMoveState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	pOwner->ClearAttackFireTimeAndSkill();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	pOwner->SetShadowRangerSpecialMoveState( m_fShadowRangerUseGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_SHADOW_RANGER;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::IsEnableInputJumpDashS( ioBaseChar* pOwner )
{
	if( pOwner->IsHasCrown() || pOwner->IsCatchMode() || pOwner->IsPrisonerMode() )
		return false;

	if( pOwner->GetUsedBuffJump() != BJUT_NONE )
		return false;

	if( pOwner->GetUsedBuffFlyJump() != BJUT_NONE )
		return false;

	if( pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	if( 0 < m_dwInput_Dash_S_StartTime && m_dwInput_Dash_S_StartTime < FRAMEGETTIME() )
	{
		ioAttackableItem *pAttack = pOwner->GetPriorityItem( SPT_ATTACK );
		if( pAttack && pAttack->IsConditionalSpecialDashReserve( pOwner ) )
			return true;
	}

	return false;
}

void ioChargeComboJump3::SetInputDashS( ioBaseChar *pOwner )
{
	ioAttackableItem *pAttack = pOwner->GetPriorityItem( SPT_ATTACK );
	if( pAttack )
		pAttack->SetChangeConditionalSpecialDash( pOwner, pOwner->CheckCurDirKey(), ioAttackableItem::CCSD_JUMP );
}

bool ioChargeComboJump3::CheckEnableJumperAtt( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioJumperItem *pJumper = ToJumperItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pJumper )
		return false;
		
	if( pJumper->GetCurBullet() < pJumper->GetNeedBullet() )
		return false;

	return true;
}

void ioChargeComboJump3::SetJumperAtt( ioBaseChar *pOwner )
{
	ioJumperItem *pJumper = ToJumperItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( pJumper )
	{
		pJumper->ChangeToExtendAttackFire( pOwner, FTT_JUMP_ATTACK );
	}
}

void ioChargeComboJump3::SetReserveJumpAttack( ioBaseChar *pOwner )
{
	if( !COMPARE( m_iCurComboCnt, 0, (int)m_ComboAttackList.size() ) )
	{
		m_iCurComboCnt	= 0;
	}
	m_bReserveJumpAttack = true;
}

void ioChargeComboJump3::SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime )
{
	m_dwRedHoodSpecialMoveMacroTime_S = 0;
	m_dwRedHoodSpecialMoveMacroTime_E = 0;
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwSpecialMoveMacroTime_S = pGrp->GetAnimationEventTime_e( iAniID, "red_hood_s" ) * fTimeRate;
	DWORD dwSpecialMoveMacroTime_E = pGrp->GetAnimationEventTime_e( iAniID, "red_hood_e" ) * fTimeRate;
	if( dwSpecialMoveMacroTime_S )
	{
		m_dwRedHoodSpecialMoveMacroTime_S = dwCurTime + dwSpecialMoveMacroTime_S;
		m_dwRedHoodSpecialMoveMacroTime_E = dwCurTime + dwSpecialMoveMacroTime_E + dwEndTime;
	}
}

//파계승
bool ioChargeComboJump3::IsCanApostateMonkDash( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return false;

	if( !m_bEnableJumpDash ) 
		return false;

	if( !pOwner->IsNeedProcess() ) 
		return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

bool ioChargeComboJump3::CheckEnableDropExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( 0 < m_dwNoDropExtendAttackEndTime  )
		return false;

	if( m_bNoDropExtendAttack )
		return false;

	return true;
}

bool ioChargeComboJump3::CheckEnableCrowExtendAttacck( ioBaseChar *pOwner )
{
	ioCrowItem *pCrowItem = ToCrowItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pCrowItem && pCrowItem->IsEnableGauge() && CheckEnableDropExtendAttack(pOwner ) )
	{
		pCrowItem->CheckDecreaseGauge();
		return true;
	}

	return false;
}

void ioChargeComboJump3::SetApostateMonkJumpDashAttack( ioBaseChar* pOwner )
{
	m_ChargeComboState = CCS_NONE;
	ioApostatemonkItem *pApostatemon = ToApostatemonkItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pApostatemon )
		return;

	if ( !pApostatemon->IsEnableJumpDashAttack( pOwner ) )
		return;

	pApostatemon->SetJumpDashAttackState( pOwner );
}

void ioChargeComboJump3::ChangeToNoDropExtendAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTEND_JUMP_ATTACK );
	if( !pAttr )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}
	pOwner->SetAutoTarget( ATT_JUMP );

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	m_dwNoDropExtendAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;	
	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	

	m_ChargeComboState		= CCS_NO_DROP_EXTEND_ATTACK;
	m_bUsedChargeAttack		= true;
	m_bUseExtendJumpAttack	= true;
	m_bNoDropExtendAttack	= true;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );	
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::NoDropExtendAttack( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( 0 < m_dwNoDropExtendAttackEndTime && m_dwNoDropExtendAttackEndTime < FRAMEGETTIME() )
	{
		pChar->SetForcePowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
		pChar->SetExtendAttackEndJump( m_fNoDropExtendAttackEndJumpPower, FLOAT1, false, false );
		return;
	}
}

bool ioChargeComboJump3::CheckEnableMirExtendState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioMirWarrior* pMir = ToMirWarrior( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( pMir && pMir->GetNeedBullet() <= pMir->GetCurBullet() )
	{
		return true;
	}

	return false;
}

void ioChargeComboJump3::ChangeToMirExtendJumpAttack( ioBaseChar* pOwner )
{
	ioMirWarrior* pMir = ToMirWarrior( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( pMir )
		pMir->SetUseExtraGauge( pMir->GetNeedBullet() );

	ChangeToPowerChargeJumpAttack( pOwner );
}

bool ioChargeComboJump3::CheckJinrahChargeAttack( ioBaseChar *pOwner, bool bHalf )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_JumpExtendType != JET_JINRAH )
		return false;

	ioJinrahItem *pJinrahItem = ToJinrahItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pJinrahItem )
	{
		if( !bHalf && !pJinrahItem->IsSpecialAttack() )
			return true;
		if( bHalf && !pJinrahItem->IsHalfSpecialAttack() )
			return true; 
	}
	return false;
}

void ioChargeComboJump3::ChangeToJinrahChargeAttack( ioBaseChar *pOwner, bool bHalf )
{
	if ( !pOwner )
		return;
	if( m_JumpExtendType != JET_JINRAH )
		return;

	ioJinrahItem *pJinrahItem = ToJinrahItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pJinrahItem )
		return;

	if( bHalf )
		pJinrahItem->SetHalfAttackState( pOwner, true, true );
	else
		pJinrahItem->SetAttackState( pOwner, true, true );
}

//귀무사
void ioChargeComboJump3::SetOniMushaObjectEquipState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsEnableOnimushaEquipObject() )
		pOwner->SetOnimushaEquipObject( ioOniMushaItem::EOS_JUMP );
	else
		ChangeToPowerChargeJumpAttack( pOwner );
}

bool ioChargeComboJump3::IsCanJumpDashCombo( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( !m_bJumpDashCombo )
		return false;

	if( !pOwner->IsNeedProcess() ) 
		return false;
	
	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;
	
	if ( m_JumpExtendType != JET_BUBBLE_FIGHTER )
	{
		int iMaxCnt = m_ComboAttackList.size();
		if( m_iCurComboCnt >= iMaxCnt )
			return false;
	}
	
	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) 
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;
	
	return true;
}

bool ioChargeComboJump3::IsJumpDashComboState()
{
	switch( m_ChargeComboState )
	{
	case CCS_DASH:
	case CCS_DASH_END:
		return true;
	}

	return false;
}

bool ioChargeComboJump3::CheckEnableOniMushaHalfChargeState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( pOwner->IsHasCrown() )
		return false;

	ioOniMushaItem *pOniMushaItem = ToOniMushaItem( pOwner->GetWeapon() );
	if ( !pOniMushaItem )
		return false;

	if ( pOniMushaItem->GetCurExtraGauge() < pOniMushaItem->GetNeedBullet() )
		return false;

	return true;
}

bool ioChargeComboJump3::CheckDruidTransformEagleState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	//오브젝트 아이템이 드루이드 오브젝트가 아닌 경우는 패스
	if( pOwner->GetObject() && !ToDruidObjectItem( pOwner->GetObject() ) )
		return false;

	ioDruidItem* pDruid = ToDruidItem( pOwner->GetWeapon() );
	if( pDruid && pDruid->GetCurBullet() < pDruid->GetNeedBullet() )
	{
		return false;
	}

	return true;
}

void ioChargeComboJump3::SetDruidTransformEagle( ioBaseChar *pOwner )
{
	ioItem* pItem = pOwner->GetPriorityItem( SPT_JUMP );
	if( pItem )
	{
		switch( pItem->GetType() )
		{
		case ioItem::IT_WEAPON:
			{
				ioDruidItem* pDruid = ToDruidItem( pItem );
				if( pDruid )
				{
					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_EXTEND_JUMP );
						kPacket << pOwner->GetCharName();
						kPacket << (int)GetType();
						kPacket << pOwner->GetHP().m_fCurValue;
						kPacket << CCS_DRUID_EAGLE;
						kPacket << pOwner->GetTargetRot();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					//상태 변경 전에 먼저 SetDruidTransformEagle()함수가 동기화가 되어야
					//상태 내부에서 동기화 시도시 튀지 않음
					pOwner->SetState( CS_DRUID_EAGLE_FLY );
					return;
				}
				else
				{
					if( 0 < m_dwHalfChargingTime )
						ChangeToPowerChargeJumpAttack( pOwner );
					else
						SetComboJumpAttack( pOwner );
				}
			}
			break;
		case ioItem::IT_OBJECT:
			{
				ioDruidObjectItem* pDruid = ToDruidObjectItem( pItem );
				if( pDruid )
				{
					pDruid->ChangeToEagle( pOwner );
				}
				else
				{
					if( 0 < m_dwHalfChargingTime )
						ChangeToPowerChargeJumpAttack( pOwner );
					else
						SetComboJumpAttack( pOwner );
				}
			}
			break;
		default:
			if( 0 < m_dwHalfChargingTime )
				ChangeToPowerChargeJumpAttack( pOwner );
			else
				SetComboJumpAttack( pOwner );
			break;
		}
	}
	else
	{
		if( 0 < m_dwHalfChargingTime )
			ChangeToPowerChargeJumpAttack( pOwner );
		else
			SetComboJumpAttack( pOwner );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_DRUID_EAGLE;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckDruidTransformBearState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( pOwner->IsObejctEquipState() )
		return false;

	ioDruidItem* pDruid = ToDruidItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( pDruid && pDruid->GetCurBullet() < pDruid->GetNeedBullet() )
	{
		return false;
	}

	return true;
}

void ioChargeComboJump3::SetDruidTransformBear( ioBaseChar *pOwner )
{
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioDruidItem* pDruid = ToDruidItem( pItem );
	if( pDruid )
	{
		if( pDruid->GetCurBullet() < pDruid->GetNeedBullet() )
		{
			if( 0 < m_dwHalfChargingTime )
				ChangeToPowerChargeJumpAttack( pOwner );
			else
				SetComboJumpAttack( pOwner );
		}
		else
		{
			pDruid->ChangeEquipObjectState( pOwner, ioDruidItem::EOS_JUMP );
		}
	}
}

bool ioChargeComboJump3::CheckChangeOnGriffinState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( pOwner->GetObject() && !ToObjectWereWolfItem( pOwner->GetObject() ) )
		return false;

	ioChangeOnItem* pChangeOn = ToChangeOnItem( pOwner->GetWeapon() );
	if( pChangeOn && pChangeOn->IsEnableGauge() )
		return true;

	return false;
}

void ioChargeComboJump3::SetChangeOnGriffinState( ioBaseChar *pOwner )
{
	ioItem* pItem = pOwner->GetPriorityItem( SPT_JUMP );
	if( pItem )
	{
		switch( pItem->GetType() )
		{
		case ioItem::IT_WEAPON:
			{
				ioChangeOnItem* pChangeOn = ToChangeOnItem( pItem );
				if( pChangeOn )
				{
					if( pOwner->IsNeedSendNetwork() )
					{
						ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
						if( eDir != ioUserKeyInput::DKI_NONE )
						{
							float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
							fYawD = ioMath::ArrangeHead( fYawD );

							float fYawR = DEGtoRAD( fYawD );
							D3DXQUATERNION qtRotate;
							D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

							pOwner->SetTargetRotToRotate( qtRotate, true, true );
							pOwner->SetInputDirection( pOwner->GetMoveDir() );
						}

						SP2Packet kPacket( CUPK_EXTEND_JUMP );
						kPacket << pOwner->GetCharName();
						kPacket << (int)GetType();
						kPacket << pOwner->GetHP().m_fCurValue;
						kPacket << CCS_CHANGE_ON;
						kPacket << pOwner->GetTargetRot();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					pOwner->SetState( CS_CHANGE_ON_GRIFFIN_FLY );
					return;
				}
				else
				{
					SetComboJumpAttack( pOwner );
				}
			}
			break;
		case ioItem::IT_OBJECT:
			{
				ioObjectWereWolfItem *pObjectWere = ToObjectWereWolfItem( pItem );
				if( pObjectWere )
				{
					pObjectWere->ChangeToGriffin( pOwner );
				}
				else
				{
					SetComboJumpAttack( pOwner );
				}
			}
			break;
		default:
			SetComboJumpAttack( pOwner );
			break;
		}
	}
	else
	{
		SetComboJumpAttack( pOwner );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_CHANGE_ON;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckEnableMurimGosuState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;
	if ( !pOwner->IsNeedProcess() )
		return false;
	if( m_JumpExtendType != JET_MURIM_GOSU || m_bUsedDashJumpAttack )
		return false;

	ioMurimGosuItem *pMurim = ToMurimGosuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMurim && pMurim->IsCanSpecialMove() )
		return true; 

	return false;
}

bool ioChargeComboJump3::CheckEnableMurimGosuCmdState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;
	if ( !pOwner->IsNeedProcess() )
		return false;
	if( m_JumpExtendType != JET_MURIM_GOSU || m_bUsedDashJumpAttack )
		return false;

	ioMurimGosuItem *pMurim = ToMurimGosuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMurim && pMurim->CheckCmdInputState( pOwner ) )
		return true; 

	return false;
}

void ioChargeComboJump3::SetMurimGosuState( ioBaseChar *pOwner, int iDir )
{
	if ( !pOwner )
		return;
	if( m_JumpExtendType != JET_MURIM_GOSU )
		return;

	ioMurimGosuItem *pMurim = ToMurimGosuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pMurim )
		return;

	pMurim->SetSpecailMoveState( pOwner, true, iDir );
}

bool ioChargeComboJump3::CheckGhostState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;
	if ( !pOwner->IsNeedProcess() )
		return false;
	if( m_JumpExtendType != JET_GHOST )
		return false;

	ioGhostItem *pGhostItem = ToGhostItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGhostItem && pGhostItem->IsEnableGhostGauge() )
		return true; 

	return false;
}

void ioChargeComboJump3::SetGhostState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	if( m_JumpExtendType != JET_GHOST )
		return;

	ioGhostItem *pGhostItem = ToGhostItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pGhostItem )
		return;

	pGhostItem->SetGhostFlyState( pOwner, true );
}

bool ioChargeComboJump3::CheckEnablePredatorState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( pOwner->IsHasCrown() )
		return false;

	ioPredatorItem *pPredatorItem = ToPredatorItem( pOwner->GetWeapon() );
	if ( !pPredatorItem )
		return false;

	if ( pPredatorItem->GetCurExtraGauge() < pPredatorItem->GetNeedBullet() )
		return false;

	return true;
}

void ioChargeComboJump3::SetPredatorState( ioBaseChar *pOwner )
{
	m_ChargeComboState = CCS_NONE;

	ioPredatorItem *pPredatorItem = ToPredatorItem( pOwner->GetWeapon() );
	if( pPredatorItem )
		pPredatorItem->SetPredatorSpecialState( pOwner, true );
}

bool ioChargeComboJump3::CheckEnableWitchDoctorPredatorState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( pOwner->IsHasCrown() )
		return false;

	ioWitchDoctoritem *pWitchDoctorItem = ToWitchDoctoritem( pOwner->GetWeapon() );
	if ( !pWitchDoctorItem )
		return false;

	return true;
}

void ioChargeComboJump3::SetWitchDoctorCallDummy( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioWitchDoctoritem *pWitchDoctorItem = ToWitchDoctoritem( pOwner->GetWeapon() );
	if ( !pWitchDoctorItem )
		return;

	pWitchDoctorItem->CallDummyChar( pOwner );
}

bool ioChargeComboJump3::IsCanBubbleFighterSpecialAttack( ioBaseChar* pOwner )
{
	if ( COMPARE( FRAMEGETTIME(), m_dwBubbleFighterSpecialAttack_S_Time, m_dwBubbleFighterSpecialAttack_E_Time ) )
		return true;
	return false;
}

void ioChargeComboJump3::SetBubbleFighterSpecialAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearAttackFireTimeAndSkill();

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_BubbleFighterSpecialAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	m_ChargeComboState = CCS_BUBBLE_FIGHTER_SPECIAL_ATTACK;

	float fTimeRate = m_BubbleFighterSpecialAttack.m_fAttackAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = 1.0f;

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	DWORD dwPreDelay = m_BubbleFighterSpecialAttack.m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;


	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_BubbleFighterSpecialAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	DWORD dwInputDashS = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( 0 < dwInputDashS )
		m_dwInput_Dash_S_StartTime = dwStartTime + dwInputDashS;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );


	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	
	m_bReserveJumpAttack = false;

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::IsCanBubbleFighterAttack( ioBaseChar* pOwner )
{
	ioBubbleFighterItem *pBubbleFighter = ToBubbleFighterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pBubbleFighter )
		return false;

	if ( !pBubbleFighter->EnableBulletAttack() )
		return false;

	return true;
}

bool ioChargeComboJump3::CheckEnableBubbleFighterChargeAttack( ioBaseChar* pOwner )
{
	ioBubbleFighterItem *pBubbleFighter = ToBubbleFighterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pBubbleFighter )
		return false;

	if ( !pBubbleFighter->EnableBulletAttack() )
		return false;

	return true;
}

void ioChargeComboJump3::SetBubbleFighterChargeAttack( ioBaseChar* pOwner )
{
	ioBubbleFighterItem *pBubbleFighter = ToBubbleFighterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pBubbleFighter )
		return;

	m_ChargeComboState = CCS_BUBBLE_FIGHTER_CHARGE_ATTACK;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	m_bReserveJumpAttack = false;
	pOwner->SetJumpAttackState();
	ioHashString szTarget;
	bool bAimedBall;
	pBubbleFighter->SetBubbleFighterJumpAttack( pOwner, m_dwInput_Dash_S_StartTime, szTarget, bAimedBall );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		kPacket << szTarget;
		kPacket << bAimedBall;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump3::CheckEnableWarpState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioWarpItem *pWarp = ToWarpItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pWarp || !pWarp->IsEnableGauge() )
		return false;
		
	return true;
}

void ioChargeComboJump3::SetWarpState( ioBaseChar *pOwner )
{
	ioWarpItem *pWarp = ToWarpItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( pWarp )
		pWarp->ChangeToTeleportAttack( pOwner, true );
}

bool ioChargeComboJump3::CheckEnableDrillChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioDrillItem *pDrillItem = ToDrillItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pDrillItem || !pDrillItem->CheckEnableChargeAttackGauge() )
		return false;

	return true;
}

void ioChargeComboJump3::SetDrillChargeAttackState( ioBaseChar *pOwner )
{
	ioDrillItem *pDrillItem = ToDrillItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( pDrillItem )
		pDrillItem->ChangeToJumpAttackState( pOwner );
}

bool ioChargeComboJump3::IsCanDefenseKeyAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( !pOwner->IsDefenseKey() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_JumpExtendType == JET_RED_HOOD )
	{
		if( m_dwRedHoodSpecialMoveMacroTime_S && m_dwRedHoodSpecialMoveMacroTime_S < dwCurTime && m_dwRedHoodSpecialMoveMacroTime_E > dwCurTime )
		{
			ioRedHoodItem* pRedHoodItem = ToRedHoodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pRedHoodItem->IsEnableGauge() )
			{
				pRedHoodItem->SetSpecailMoveState( pOwner, true );
				return true;
			}
		}
	}
	else if( m_JumpExtendType == JET_CUCHULAIN_EXTEND && m_ChargeComboState != CCS_CUCHULAIN_S_ATTACK && IsCanCuchulainJumpAttack( pOwner ) )
	{
		SetCuchulainJumpAttack( pOwner );
		return true;
	}
	else if( m_JumpExtendType == JET_BUBBLE_FIGHTER && IsCanBubbleFighterSpecialAttack( pOwner ) )
	{
		SetBubbleFighterSpecialAttack( pOwner );
		return true;
	}
	else if ( m_JumpExtendType == JET_STRANGER && m_bEnableDefenseAttack && fHeightGap > m_fDefenseAttackEnableHeight )
	{
		ChangeToStrangerSpecialAttack( pOwner );
		return true;
	}
	else if ( m_JumpExtendType == JET_SMILE_JOKER && m_bEnableDefenseAttack && fHeightGap > m_fDefenseAttackEnableHeight )
	{
		ChangeToSmileJokerFlyState( pOwner );
		return true;
	}

	return false;
}

void ioChargeComboJump3::ChangeToStrangerSpecialAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_STRANGER_SPECIAL_ATTACK;

	pOwner->ClearAttackFireTimeAndSkill();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_StrangerSpecialAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_StrangerSpecialAttack.m_fAttackAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = 1.0f;

	DWORD dwPreDelay = m_StrangerSpecialAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwInputDashS = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( 0 < dwInputDashS )
		m_dwInput_Dash_S_StartTime = dwStartTime + dwInputDashS;

	//Attack
	if ( !m_StrangerSpecialAttack.m_vWeaponInfoList.empty() )
		m_dwStrangerSpecialAttackTime = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_bEnableDefenseAttack = false;
}

void ioChargeComboJump3::ProcessStrangerSpecialAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if( m_dwEnableComboTime_E < FRAMEGETTIME() )
		{
			ChangeToStrangerSpecialLoop( pOwner );
			return;
		}
	}

	if ( m_dwStrangerSpecialAttackTime && m_dwStrangerSpecialAttackTime < FRAMEGETTIME() 
		&& !m_StrangerSpecialAttack.m_vWeaponInfoList.empty() )
	{
		m_dwStrangerSpecialAttackTime = 0;

		WeaponInfo Info = m_StrangerSpecialAttack.m_vWeaponInfoList.front();
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.iAttributeIdx = Info.m_iWeaponIdx;
		kFireTime.iResistanceIdx = Info.m_iResistanceIdx;
		kFireTime.szWoundedAni  = Info.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = Info.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = Info.m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
		kFireTime.eFireTimeType = FTT_JUMP_ATTACK;

		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
		D3DXVec3Normalize( &vDir, &vDir );
		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
		if( pZone )
		{
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
		if ( pZone )
			m_dwStrangerSpecialAttackIndex = pWeapon->GetWeaponIndex();
	}
	

	CheckAirJump( pOwner );
}

void ioChargeComboJump3::ChangeToStrangerSpecialLoop( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_STRANGER_SPECIAL_LOOP;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szStrangerSpecialLoopAni );
	float fTimeRate = m_fStrangerSpecialLoopAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = 1.0f;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::ProcessStrangerSpecialLoop( ioBaseChar *pOwner )
{
	if ( !pOwner |!pOwner->IsNeedProcess() )
		return;

	if ( !pOwner->IsDefenseKeyDown() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if ( !pGrp )
			return;
		int iAniID = pOwner->GetJumppingAnimationIdx();
		if( iAniID != -1 )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
		}

		if( m_dwStrangerSpecialAttackIndex > 0 )
		{
			ioWeapon *pWeapon =	g_WeaponMgr.FindWeapon( pOwner, m_dwStrangerSpecialAttackIndex );
			if ( pWeapon )
			{
				pWeapon->SetWeaponDead();
				m_dwStrangerSpecialAttackIndex = 0;
			}
		}

		m_ChargeComboState = CCS_NONE;
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << CCS_STRANGER_SPECIAL_END;
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}		
	}
}

bool ioChargeComboJump3::CheckEnableStrangerChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioStrangerItem *pStrangerItem = ToStrangerItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pStrangerItem || !pStrangerItem->IsEnableStarngerSpecialState() )
		return false;

	return true;
}

void ioChargeComboJump3::SetStrangerChargeAttackState( ioBaseChar *pOwner )
{
	ioStrangerItem *pStrangerItem = ToStrangerItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( pStrangerItem )
		pStrangerItem->SetStrangerSpecialState( pOwner, true );
}

bool ioChargeComboJump3::CheckEnableGargoyleSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioGargoyleItem *pGargoyleItem = ToGargoyleItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pGargoyleItem || !pGargoyleItem->IsEnableGargoyleSpecialState() )
		return false;

	return true;
}

void ioChargeComboJump3::SetGargoyleSpecialState( ioBaseChar *pOwner )
{
	ioGargoyleItem *pGargoyleItem = ToGargoyleItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( pGargoyleItem )
		pGargoyleItem->SetGargoyleSpecialState( pOwner, true );
}

bool ioChargeComboJump3::CheckEnableMegaAgentFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_bUsedFly )
		return false;
	if( pOwner->IsEnableMegaAgentFly() )
		return true;

	return false;
}

void ioChargeComboJump3::SetMegaAgentFlyState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_bUsedFly = true;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->SetMegaAgentFlyState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_MEGA_AGENT_FLY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump3::ChangeToSmileJokerFlyState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_bEnableDefenseAttack = false;
	m_bUsedFly = true;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->ChangeToSmileJokerFlyState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_SMILE_JOKER_FLY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}