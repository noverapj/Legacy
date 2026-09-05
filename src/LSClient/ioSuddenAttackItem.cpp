

#include "stdafx.h"
#include "ioSuddenAttackItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioSuddenAttackItem::ioSuddenAttackItem()
{
	m_ChargeState = WS_NONE;
	m_dwAttackStartTime = 0;
	m_dwComboAttackTimeEnd = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bUsedExtraAttack = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwRetreatEndTime = 0;

	m_itemMeshType = IMT_NORMAL;

//	m_fCurGauge = 0.f;
}

ioSuddenAttackItem::ioSuddenAttackItem( const ioSuddenAttackItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_AttackReadyAniList_Extend1( rhs.m_AttackReadyAniList_Extend1 ),
  m_AttackReadyAniList_Extend2( rhs.m_AttackReadyAniList_Extend2 ),
  m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
  m_vChargeComboAttListExtend1( rhs.m_vChargeComboAttListExtend1 ),
  m_vChargeComboAttListExtend2( rhs.m_vChargeComboAttListExtend2 ),
  m_ExtraAttackAttribute( rhs.m_ExtraAttackAttribute ),
  m_vDodgeAniList( rhs.m_vDodgeAniList ),
  m_fDodgeAniRate( rhs.m_fDodgeAniRate ),
  m_fDodgeForce( rhs.m_fDodgeForce ),
  m_fDodgeFrictionRate( rhs.m_fDodgeFrictionRate ),
  m_dwMaxComboDuration( rhs.m_dwMaxComboDuration ),
  m_RetreatAttribute( rhs.m_RetreatAttribute ),

  m_fTargetAngle(rhs.m_fTargetAngle ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
  m_fTargetDownHeight( rhs.m_fTargetDownHeight ),

  m_fExtendTargetAngle(rhs.m_fExtendTargetAngle ),
  m_fExtendTargetRange( rhs.m_fExtendTargetRange ),
  m_fExtendTargetUpHeight( rhs.m_fExtendTargetUpHeight ),
  m_fExtendTargetDownHeight( rhs.m_fExtendTargetDownHeight ),

  m_DashAttackMove( rhs.m_DashAttackMove ),

  m_DashAnimation( rhs.m_DashAnimation ),
  m_fDashAniRate( rhs.m_fDashAniRate ),
  m_fDashSpeed( rhs.m_fDashSpeed ),
  m_fDashRange( rhs.m_fDashRange ),
  m_dwDashWaitMaxTime( rhs.m_dwDashWaitMaxTime ),

  m_DashEndAnimation( rhs.m_DashEndAnimation ),
  m_fDashEndAniRate( rhs.m_fDashEndAniRate ),

  m_fDashJumpAmt( rhs.m_fDashJumpAmt ),
  m_DashAttackEndJumpAmt( rhs.m_DashAttackEndJumpAmt ),

  m_vChargeBuffList( rhs.m_vChargeBuffList ),

  m_NormalAttackMesh( rhs.m_NormalAttackMesh ),
  m_ExtraAttackMesh1( rhs.m_ExtraAttackMesh1 ),
  m_ExtraAttackMesh2( rhs.m_ExtraAttackMesh2 ),

  m_DelayAnimation_extra1( rhs.m_DelayAnimation_extra1 ),
  m_RunAnimation_extra1( rhs.m_RunAnimation_extra1 ),
  m_DashAnimation_extra1( rhs.m_DashAnimation_extra1 ),
  m_DefenseAnimation_extra1( rhs.m_DefenseAnimation_extra1 ),

  m_DelayAnimation_extra2( rhs.m_DelayAnimation_extra2 ),
  m_RunAnimation_extra2( rhs.m_RunAnimation_extra2 ),
  m_DashAnimation_extra2( rhs.m_DashAnimation_extra2 ),
  m_DefenseAnimation_extra2( rhs.m_DefenseAnimation_extra2 ),

  m_fAutoTargetRange_monk( rhs.m_fAutoTargetRange_monk ),
  m_fAutoTargetMinAngle_monk( rhs.m_fAutoTargetMinAngle_monk ),
  m_fAutoTargetMaxAngle_monk( rhs.m_fAutoTargetMaxAngle_monk ),

  m_FormalAttack_Extend1( rhs.m_FormalAttack_Extend1 ),
  m_FormalAttack_Extend2( rhs.m_FormalAttack_Extend2 ),

  m_JumpReadyAnimationExtend1( rhs.m_JumpReadyAnimationExtend1 ),
  m_JumpReadyAnimationExtend2( rhs.m_JumpReadyAnimationExtend2 ),

  m_JumppingAnimationExtend1( rhs.m_JumppingAnimationExtend1 ),
  m_JumppingAnimationExtend2( rhs.m_JumppingAnimationExtend2 ),

  m_JumpEndAnimationExtend1( rhs.m_JumpEndAnimationExtend1 ),
  m_JumpEndAnimationExtend2( rhs.m_JumpEndAnimationExtend2 ),

  m_ExtendJumppingAnimationExtend1( rhs.m_ExtendJumppingAnimationExtend1 ),
  m_ExtendJumppingAnimationExtend2( rhs.m_ExtendJumppingAnimationExtend2 ),

  m_ExtendJumpEndAnimationExtend1( rhs.m_ExtendJumpEndAnimationExtend1 ),
  m_ExtendJumpEndAnimationExtend2( rhs.m_ExtendJumpEndAnimationExtend2 ),

  m_fExtend1TargetRange( rhs.m_fExtend1TargetRange ),
  m_fExtend2TargetRange( rhs.m_fExtend2TargetRange ),
  m_dwFullChargeTime( rhs.m_dwFullChargeTime ),
  m_dwChargeTimeExtend2( rhs.m_dwChargeTimeExtend2 ),
  m_dwZeroChargeTimeExtend2( rhs.m_dwZeroChargeTimeExtend2 ),
  m_dwHeadChargeTimeExtend2( rhs.m_dwHeadChargeTimeExtend2 ),

  m_stReadyAimMarker( rhs.m_stReadyAimMarker ),
  m_stAimingMaker( rhs.m_stAimingMaker ),
  m_stAimedMarker( rhs.m_stAimedMarker ),
  m_stZeroAimedMarker( rhs.m_stZeroAimedMarker ),
  m_stHeadAimedMarker( rhs.m_stHeadAimedMarker ),

  m_ReloadAnimation( rhs.m_ReloadAnimation ),
  m_ReloadMoveAnimation( rhs.m_ReloadMoveAnimation ),

  m_AimUpExtend1( rhs.m_AimUpExtend1 ),
  m_AimCenterExtend1( rhs.m_AimCenterExtend1 ),
  m_AimDownExtend1( rhs.m_AimDownExtend1 ),
  m_FireAimUpExtend1( rhs.m_FireAimUpExtend1 ),
  m_FireAimCenterExtend1( rhs.m_FireAimCenterExtend1 ),
  m_FireAimDownExtend1( rhs.m_FireAimDownExtend1 ),

  m_AimUpExtend2( rhs.m_AimUpExtend2 ),
  m_AimCenterExtend2( rhs.m_AimCenterExtend2 ),
  m_AimDownExtend2( rhs.m_AimDownExtend2 ),
  m_FireAimUpExtend2( rhs.m_FireAimUpExtend2 ),
  m_FireAimCenterExtend2( rhs.m_FireAimCenterExtend2 ),
  m_FireAimDownExtend2( rhs.m_FireAimDownExtend2 ),
  m_FireAimUpExtend2_Head( rhs.m_FireAimUpExtend2_Head ),
  m_FireAimCenterExtend2_Head( rhs.m_FireAimCenterExtend2_Head ),
  m_FireAimDownExtend2_Head( rhs.m_FireAimDownExtend2_Head ),

  m_fAngleExtend1( rhs.m_fAngleExtend1 ),
  m_fRangeExtend1( rhs.m_fRangeExtend1 ),
  m_fAngleExtend2( rhs.m_fAngleExtend2 ),
  m_fRangeExtend2( rhs.m_fRangeExtend2 ),

  m_fMoveSpeedRateExtend1( rhs.m_fMoveSpeedRateExtend1 ),
  m_fMoveSpeedRateExtend2( rhs.m_fMoveSpeedRateExtend2 ),
  m_fMoveSpeedRateExtend2_Sniper( rhs.m_fMoveSpeedRateExtend2_Sniper ),

  m_dwAimTime( rhs.m_dwAimTime ),
  m_fTargetAimRate( rhs.m_fTargetAimRate ),

  m_iMaxBullet(rhs.m_iMaxBullet ),
  m_fReloadTimeRate( rhs.m_fReloadTimeRate ),

  m_LevelTimeRate( rhs.m_LevelTimeRate ),

  m_AimMoveFront_Extend2( rhs.m_AimMoveFront_Extend2 ),
  m_AimMoveBack_Extend2( rhs.m_AimMoveBack_Extend2 ),
  m_AimMoveLeft_Extend2( rhs.m_AimMoveLeft_Extend2 ),
  m_AimMoveRight_Extend2( rhs.m_AimMoveRight_Extend2 ),
  m_AimDelayCenter_Extend2( rhs.m_AimDelayCenter_Extend2 ),
  m_fAimMoveAniRate( rhs.m_fAimMoveAniRate ),
  m_dwAimBipeReturnTime( rhs.m_dwAimBipeReturnTime ),
  m_dwExtend1FireTimeGap( rhs.m_dwExtend1FireTimeGap ),

  m_fChargingCamDistanceExtend2( rhs.m_fChargingCamDistanceExtend2 ),
  m_fChargingCamHeightExtend2( rhs.m_fChargingCamHeightExtend2 ),
  m_fChargingCamFovExtend2( rhs.m_fChargingCamFovExtend2 ),
  m_dwChargingCamZoomTimeExtend2( rhs.m_dwChargingCamZoomTimeExtend2 ),

  m_fCamHeightExtend2( rhs.m_fCamHeightExtend2 ),
  m_fCamDistanceExtend2( rhs.m_fCamDistanceExtend2 ),
  m_fCamFovExtend2( rhs.m_fCamFovExtend2 ),
  m_dwCamZoomTimeExtend2( rhs.m_dwCamZoomTimeExtend2 ),

  m_fRunCamHeightExtend2( rhs.m_fRunCamHeightExtend2 ),
  m_fRunCamDistanceExtend2( rhs.m_fRunCamDistanceExtend2 ),
  m_fRunCamFovExtend2( rhs.m_fRunCamFovExtend2 ),
  m_dwRunCamZoomTimeExtend2( rhs.m_dwRunCamZoomTimeExtend2 ),

  m_JumpAimUpExtend1( rhs.m_JumpAimUpExtend1 ),
  m_JumpAimCenterExtend1( rhs.m_JumpAimCenterExtend1 ),
  m_JumpAimDownExtend1( rhs.m_JumpAimDownExtend1 ),

  m_DelayCameraBuffExtend2( rhs.m_DelayCameraBuffExtend2 )
{
	m_ChargeState = WS_NONE;
	m_dwAttackStartTime = 0;
	m_dwComboAttackTimeEnd = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bUsedExtraAttack = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwRetreatEndTime = 0;

//	m_fCurGauge = 0.f;
	m_dwDashWaitStartTime = 0;

	m_AimedTarget.Clear();

	m_OwnerPreCharState = CS_DELAY;

	m_fCurAngle = 0.f;

	m_dwAimStartTime = 0;
	m_dwTrackingEndTime = 0;

	if( m_iMaxBullet > 0 )
		m_iCurBullet = m_iMaxBullet;
	else
		m_iCurBullet = 0;

	m_Extend2Mode = ECM_NONE;
	m_Extend2FireMode = ECM_NONE;

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = false;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bChangeDir = false;
	m_dwExtend1FireTimeStart = 0;

	m_itemMeshType = IMT_NORMAL;
}

ioSuddenAttackItem::~ioSuddenAttackItem()
{
	m_LevelTimeRate.clear();
}

void ioSuddenAttackItem::LoadProperty( ioINILoader &rkLoader )
{
	m_itemMeshType = IMT_NORMAL;

	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	//char szKey[MAX_PATH];

	// ready ani
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}


	// combo att normal
	int iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt", 1 );
	m_vChargeComboAttList.clear();
	m_vChargeComboAttList.reserve( iComboMax );
	for( int i=0; i < iComboMax; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttList.push_back( kAttribute );
	}
	
	//////////////////////////////////////////////////////////////////////////
	// combo att extend1
	iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt_extend1", 1 );
	m_vChargeComboAttListExtend1.clear();
	m_vChargeComboAttListExtend1.reserve( iComboMax );
	for( int i=0; i < iComboMax; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack_extend1_%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttListExtend1.push_back( kAttribute );
	}

	// combo att extend2
	iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt_extend2", 1 );
	m_vChargeComboAttListExtend2.clear();
	m_vChargeComboAttListExtend2.reserve( iComboMax );
	for( int i=0; i < iComboMax; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack_extend2_%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttListExtend2.push_back( kAttribute );
	}
	//////////////////////////////////////////////////////////////////////////

	m_dwMaxComboDuration = (DWORD)rkLoader.LoadInt_e( "charge_combo_max_time", 1 );

	// extend att ( key : defense )
	m_ExtraAttackAttribute.Init();
	LoadAttackAttribute_e( "charge_extra_attack", m_ExtraAttackAttribute, rkLoader );

	// retreat
	LoadAttackAttribute_e( "retreat_attack", m_RetreatAttribute, rkLoader );

	// etc
	m_fDodgeForce = rkLoader.LoadFloat_e( "dodge_force", 0.0f );
	m_fDodgeFrictionRate = rkLoader.LoadFloat_e( "dodge_friction_rate", 0.0f );

	m_fDodgeAniRate = rkLoader.LoadFloat_e( "dodge_ani_rate", FLOAT1 );

	m_vDodgeAniList.clear();
	m_vDodgeAniList.reserve( 4 );

	rkLoader.LoadString_e( "dodge_animation_front", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_right", "", szBuf, MAX_PATH );

	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_back", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_left", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );

	//////////////////////////////////////////////////////////////////////////
	m_fTargetAngle			= rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange			= rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetUpHeight		= rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetDownHeight		= rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	m_fExtendTargetAngle			= rkLoader.LoadFloat_e( "extend_target_angle", 0.0f );
	m_fExtendTargetRange			= rkLoader.LoadFloat_e( "extend_target_range", 0.0f );
	m_fExtendTargetUpHeight		= rkLoader.LoadFloat_e( "extend_target_up_height", 0.0f );
	m_fExtendTargetDownHeight		= rkLoader.LoadFloat_e( "extend_target_down_height", 0.0f );

	//dash wait
	m_dwDashWaitMaxTime = (DWORD)rkLoader.LoadInt_e( "dash_wait_max_time", 0 );

	// attack move
	wsprintf_e( szBuf, "dash_attack_move" );
	LoadAttackAttribute( szBuf, m_DashAttackMove, rkLoader );

	rkLoader.LoadString_e( "dash_animation", "", szBuf, MAX_PATH );
	m_DashAnimation = szBuf;
	m_fDashAniRate = rkLoader.LoadFloat_e( "dash_ani_rate", FLOAT1 );
	m_fDashSpeed = rkLoader.LoadFloat_e( "dash_speed", 0.f );
	m_fDashRange = rkLoader.LoadFloat_e( "dash_range", 0.f );

	rkLoader.LoadString_e( "dash_end_ani", "", szBuf, MAX_PATH );
	m_DashEndAnimation = szBuf;
	m_fDashEndAniRate = rkLoader.LoadFloat_e( "dash_end_ani_rate", FLOAT1 );

	m_fDashJumpAmt = rkLoader.LoadFloat_e( "dash_jump_amt", 0.f );
	m_DashAttackEndJumpAmt = rkLoader.LoadFloat_e( "dash_extend_jump_amt", 0.f );

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "normal_attack_mesh", "", szBuf, MAX_PATH );
	m_NormalAttackMesh = szBuf;
	rkLoader.LoadString_e( "extra_attack_mesh1", "", szBuf, MAX_PATH );
	m_ExtraAttackMesh1 = szBuf;
	rkLoader.LoadString_e( "extra_attack_mesh2", "", szBuf, MAX_PATH );
	m_ExtraAttackMesh2 = szBuf;

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "delay_animation_extend1", "", szBuf, MAX_PATH );
	m_DelayAnimation_extra1 = szBuf;

	rkLoader.LoadString_e( "run_animation_extend1", "", szBuf, MAX_PATH );
	m_RunAnimation_extra1 = szBuf;

	rkLoader.LoadString_e( "dash_animation_extend1", "", szBuf, MAX_PATH );
	m_DashAnimation_extra1 = szBuf;

	rkLoader.LoadString_e( "defense_animation_extend1", "", szBuf, MAX_PATH );
	m_DefenseAnimation_extra1 = szBuf;
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "delay_animation_extend2", "", szBuf, MAX_PATH );
	m_DelayAnimation_extra2 = szBuf;

	rkLoader.LoadString_e( "run_animation_extend2", "", szBuf, MAX_PATH );
	m_RunAnimation_extra2 = szBuf;

	rkLoader.LoadString_e( "dash_animation_extend2", "", szBuf, MAX_PATH );
	m_DashAnimation_extra2 = szBuf;

	rkLoader.LoadString_e( "defense_animation_extend2", "", szBuf, MAX_PATH );
	m_DefenseAnimation_extra2 = szBuf;
	//////////////////////////////////////////////////////////////////////////


	m_fAutoTargetRange_monk = rkLoader.LoadFloat_e( "autotarget_range_extra", 300.0f );
	m_fAutoTargetMinAngle_monk = rkLoader.LoadFloat_e( "autotarget_min_angle_extra", 60.0f );
	m_fAutoTargetMaxAngle_monk = rkLoader.LoadFloat_e( "autotarget_max_angle_extra", 150.0f );
	
	m_FormalAttack_Extend1.LoadProperty_Extend(rkLoader, this, 1);
	m_FormalAttack_Extend2.LoadProperty_Extend(rkLoader, this, 2);

	//extend1_2 ready motion
	rkLoader.LoadString( "attack_ready_extend1_animation", "", szBuf, MAX_PATH );
	m_AttackReadyAniList_Extend1 = szBuf;

	rkLoader.LoadString( "attack_ready_extend2_animation", "", szBuf, MAX_PATH );
	m_AttackReadyAniList_Extend2 = szBuf;

	rkLoader.LoadString_e( "jump_ready_extend1_animation", "", szBuf, MAX_PATH );
	m_JumpReadyAnimationExtend1 = szBuf;
	rkLoader.LoadString_e( "jump_ready_extend1_animation", "", szBuf, MAX_PATH );
	m_JumpReadyAnimationExtend2 = szBuf;

	rkLoader.LoadString_e( "jumpping_extend1_animation", "", szBuf, MAX_PATH );
	m_JumppingAnimationExtend1 = szBuf;
	rkLoader.LoadString_e( "jumpping_extend2_animation", "", szBuf, MAX_PATH );
	m_JumppingAnimationExtend2 = szBuf;

	rkLoader.LoadString_e( "jump_end_extend1_animation", "", szBuf, MAX_PATH );
	m_JumpEndAnimationExtend1 = szBuf;
	rkLoader.LoadString_e( "jump_end_extend2_animation", "", szBuf, MAX_PATH );
	m_JumpEndAnimationExtend2 = szBuf;

	rkLoader.LoadString_e( "extend_jumpping_extend1_animation", "", szBuf, MAX_PATH );
	m_ExtendJumppingAnimationExtend1 = szBuf;
	rkLoader.LoadString_e( "extend_jumpping_extend2_animation", "", szBuf, MAX_PATH );
	m_ExtendJumppingAnimationExtend2 = szBuf;

	rkLoader.LoadString_e( "extend_jump_end_extend1_animation", "", szBuf, MAX_PATH );
	m_ExtendJumpEndAnimationExtend1 = szBuf;
	rkLoader.LoadString_e( "extend_jump_end_extend2_animation", "", szBuf, MAX_PATH );
	m_ExtendJumpEndAnimationExtend2 = szBuf;

	//////////////////////////////////////////////////////////////////////////
	m_fExtend1TargetRange = rkLoader.LoadFloat_e( "aim_extend_1_range", 0.f );
	m_fExtend2TargetRange = rkLoader.LoadFloat_e( "aim_extend_2_range", 0.f );
	//////////////////////////////////////////////////////////////////////////

	m_dwFullChargeTime = rkLoader.LoadInt( "full_charge_time", 5000 );
	m_dwChargeTimeExtend2 = rkLoader.LoadInt( "extend2_charge_time", 15000 );
	m_dwZeroChargeTimeExtend2 = rkLoader.LoadInt( "extend2_zero_charge_time", 25000 );
	m_dwHeadChargeTimeExtend2 = rkLoader.LoadInt( "extend2_head_charge_time", 35000 );

	rkLoader.LoadString_e( "ready_marker", "", szBuf, MAX_PATH );
	m_stReadyAimMarker = szBuf;
	rkLoader.LoadString_e( "aiming_marker", "", szBuf, MAX_PATH );
	m_stAimingMaker = szBuf;
	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_stAimedMarker = szBuf;
	rkLoader.LoadString_e( "zero_aimed_marker", "", szBuf, MAX_PATH );
	m_stZeroAimedMarker = szBuf;
	rkLoader.LoadString_e( "head_aimed_marker", "", szBuf, MAX_PATH );
	m_stHeadAimedMarker = szBuf;
	
	//////////////////////////////////////////////////////////////////////////
	//재장전
	rkLoader.LoadString_e( "reload_animation", "", szBuf, MAX_PATH );
	m_ReloadAnimation = szBuf;
	rkLoader.LoadString_e( "reload_move_animation", "", szBuf, MAX_PATH );
	m_ReloadMoveAnimation = szBuf;

	//////////////////////////////////////////////////////////////////////////
	//상하좌우 조전점
	rkLoader.LoadString_e( "aim_up_animation_extend1", "", szBuf, MAX_PATH );
	m_AimUpExtend1 = szBuf;
	rkLoader.LoadString_e( "aim_center_animation_extend1", "", szBuf, MAX_PATH );
	m_AimCenterExtend1 = szBuf;
	rkLoader.LoadString_e( "aim_down_animation_extend1", "", szBuf, MAX_PATH );
	m_AimDownExtend1 = szBuf;

	rkLoader.LoadString_e( "fire_up_animation_extend1", "", szBuf, MAX_PATH );
	m_FireAimUpExtend1 = szBuf;
	rkLoader.LoadString_e( "fire_center_animation_extend1", "", szBuf, MAX_PATH );
	m_FireAimCenterExtend1 = szBuf;
	rkLoader.LoadString_e( "fire_down_animation_extend1", "", szBuf, MAX_PATH );
	m_FireAimDownExtend1 = szBuf;

	rkLoader.LoadString_e( "extend1_jump_aim_up_animation", "", szBuf, MAX_PATH );
	m_JumpAimUpExtend1 = szBuf;
	rkLoader.LoadString_e( "extend1_jump_aim_center_animation", "", szBuf, MAX_PATH );
	m_JumpAimCenterExtend1 = szBuf;
	rkLoader.LoadString_e( "extend1_jump_aim_down_animation", "", szBuf, MAX_PATH );
	m_JumpAimDownExtend1 = szBuf;


	rkLoader.LoadString_e( "aim_up_animation_extend2", "", szBuf, MAX_PATH );
	m_AimUpExtend2 = szBuf;
	rkLoader.LoadString_e( "aim_center_animation_extend2", "", szBuf, MAX_PATH );
	m_AimCenterExtend2 = szBuf;
	rkLoader.LoadString_e( "aim_down_animation_extend2", "", szBuf, MAX_PATH );
	m_AimDownExtend2 = szBuf;

	rkLoader.LoadString_e( "fire_up_animation_extend2", "", szBuf, MAX_PATH );
	m_FireAimUpExtend2 = szBuf;
	rkLoader.LoadString_e( "fire_center_animation_extend2", "", szBuf, MAX_PATH );
	m_FireAimCenterExtend2 = szBuf;
	rkLoader.LoadString_e( "fire_down_animation_extend2", "", szBuf, MAX_PATH );
	m_FireAimDownExtend2 = szBuf;
	rkLoader.LoadString_e( "fire_up_animation_extend2_head", "", szBuf, MAX_PATH );
	m_FireAimUpExtend2_Head = szBuf;
	rkLoader.LoadString_e( "fire_center_animation_extend2_head", "", szBuf, MAX_PATH );
	m_FireAimCenterExtend2_Head = szBuf;
	rkLoader.LoadString_e( "fire_down_animation_extend2_head", "", szBuf, MAX_PATH );
	m_FireAimDownExtend2_Head = szBuf;
	//////////////////////////////////////////////////////////////////////////
	m_fAngleExtend1 = rkLoader.LoadFloat_e( "target_angle_extend1", 0.f );
	m_fRangeExtend1 = rkLoader.LoadFloat_e( "target_range_extend1", 0.f );

	m_fAngleExtend2 = rkLoader.LoadFloat_e( "target_angle_extend2", 0.f );
	m_fRangeExtend2 = rkLoader.LoadFloat_e( "target_range_extend2", 0.f );
	//////////////////////////////////////////////////////////////////////////
	m_fMoveSpeedRateExtend1 = rkLoader.LoadFloat_e("move_speed_rate_extend1", 0.f );
	m_fMoveSpeedRateExtend2 = rkLoader.LoadFloat_e("move_speed_rate_extend2", 0.f );
	m_fMoveSpeedRateExtend2_Sniper = rkLoader.LoadFloat_e("move_speed_rate_extend2_sniper", 0.f );
	//////////////////////////////////////////////////////////////////////////
	m_dwAimTime = rkLoader.LoadInt_e( "aim_time", 1000 );
	m_fTargetAimRate = rkLoader.LoadFloat_e( "target_aim_rate", FLOAT08 );
	//////////////////////////////////////////////////////////////////////////
	m_iMaxBullet = rkLoader.LoadInt_e( "max_bullet", 0 );
	m_fReloadTimeRate  = rkLoader.LoadFloat_e( "reload_time_rate", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	m_LevelTimeRate.clear();
	int iLevelCount = rkLoader.LoadInt_e( "level_count", 0 );
	for( i=0; i<iLevelCount; i++ )
	{
		wsprintf_e( szBuf, "level_time_rate%d", i);

		float fRate = rkLoader.LoadFloat( szBuf, -FLOAT1 );
		if( fRate == -FLOAT1 && i > 0 )
		{
			if( i>0 )
				fRate = m_LevelTimeRate[i-1];
			else
				fRate = FLOAT1;
		}

		m_LevelTimeRate.push_back( fRate );
	}
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "aim_delay_extend2", "", szBuf, MAX_PATH );
	m_AimDelayCenter_Extend2 = szBuf;

	rkLoader.LoadString_e( "aim_move_front_extend2", "", szBuf, MAX_PATH );
	m_AimMoveFront_Extend2 = szBuf;
	rkLoader.LoadString_e( "aim_move_back_extend2", "", szBuf, MAX_PATH );
	m_AimMoveBack_Extend2 = szBuf;
	rkLoader.LoadString_e( "aim_move_left_extend2", "", szBuf, MAX_PATH );
	m_AimMoveLeft_Extend2 = szBuf;
	rkLoader.LoadString_e( "aim_move_right_extend2", "", szBuf, MAX_PATH );
	m_AimMoveRight_Extend2 = szBuf;

	m_fAimMoveAniRate = rkLoader.LoadFloat_e( "aim_move_ani_rate_extend2", FLOAT1 );
	m_dwAimBipeReturnTime = (DWORD)rkLoader.LoadInt_e( "aim_return_bipe_time", 500 );

	m_dwExtend1FireTimeGap = (DWORD)rkLoader.LoadInt_e( "extend1_fire_time_gap", 60000 );

	m_fChargingCamDistanceExtend2 = rkLoader.LoadFloat( "charging_cam_distance_extend2", 0.f );
	m_fChargingCamHeightExtend2 = rkLoader.LoadFloat( "charging_cam_height_extend2", 0.f );
	m_fChargingCamFovExtend2 = rkLoader.LoadFloat( "charging_cam_fov_extend2", 0.f );
	m_dwChargingCamZoomTimeExtend2 = (DWORD)rkLoader.LoadInt( "charging_cam_zoom_distance_extend2", 0 );

	m_fCamHeightExtend2 = rkLoader.LoadFloat_e( "item_cam_height_extend2", 450.0f );
	m_fCamDistanceExtend2 = rkLoader.LoadFloat_e( "item_cam_distance_extend2", FLOAT1000 );
	m_fCamFovExtend2 = rkLoader.LoadFloat_e( "item_cam_fov_extend2", 35.0f );
	m_dwCamZoomTimeExtend2 = rkLoader.LoadInt_e( "item_cam_zoom_time_extend2", 1000 );

	m_fRunCamHeightExtend2 = rkLoader.LoadFloat_e( "run_cam_height_extend2", 450.0f );
	m_fRunCamDistanceExtend2 = rkLoader.LoadFloat_e( "run_cam_distance_extend2", 1400.0f );
	m_fRunCamFovExtend2 = rkLoader.LoadFloat_e( "run_cam_fov_extend2", 35.0f );
	m_dwRunCamZoomTimeExtend2 = rkLoader.LoadInt_e( "run_cam_zoom_time_extend2", 1000 );

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString( "delay_camera_buff_extend2", "", szBuf, MAX_PATH );
	m_DelayCameraBuffExtend2 = szBuf;
}

ioItem* ioSuddenAttackItem::Clone()
{
	return new ioSuddenAttackItem( *this );
}

void ioSuddenAttackItem::Initialize( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( pOwner  )
		ChangeAttackType(pOwner, IMT_NORMAL, false, true);
}

void ioSuddenAttackItem::SetInitialize( const ioHashString& szName, ioPlayStage* pStage, int nMeshType )
{
	ioBaseChar *pChar = pStage->GetBaseChar( szName );
	ChangeAttackType(pChar, (ItemMeshType)nMeshType, false, true);
}

ioWeaponItem::WeaponSubType ioSuddenAttackItem::GetSubType() const
{
	return WST_SUDDEN_ATTACK_ITEM;
}

void ioSuddenAttackItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_dwAttackStartTime = 0;
	m_dwComboAttackTimeEnd = 0;
	m_bUsedExtraAttack = false;
	m_iChargeCombo = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwRetreatEndTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = WS_NONE;
		m_iNormalCombo = 0;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		m_bChangeDir = false;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = 0;
		if( m_itemMeshType == IMT_NORMAL )
		{
			if( iCurCombo < (int)m_AttackReadyAniList.size() && iCurCombo >= 0 )
				iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
			else
				iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[0] );
		}
		else if( m_itemMeshType == IMT_EXTRA1 )
			iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList_Extend1 );
		else
			iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList_Extend2 );

		float fTime = m_dwChargeTime;
		float fTime2 = pGrp->GetAnimationFullTime( iAniID );

		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = WS_CHARGING;
		m_dwAttackStartTime = dwCurTime;
		m_iNormalCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );

		if( m_itemMeshType == IMT_NORMAL || m_itemMeshType == IMT_EXTRA2 )
			pOwner->SetAttackMoveEnable( false );
		else
			pOwner->SetAttackMoveEnable( true );

		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	if( m_itemMeshType == IMT_EXTRA1 )
		m_dwExtend1FireTimeStart = dwCurTime;
}

void ioSuddenAttackItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case WS_CHARGING:
		OnCharging( pOwner );
		break;
	case WS_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			CheckAimTarget( pOwner );
			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case WS_EXTEND2_MODE:
		{
			switch( m_Extend2Mode )
			{
			case ECM_AIMING:
				OnAimingExtend2( pOwner );
				break;
			case ECM_AIMED:
				ProcessTargetTracking_Extend_Delay( pOwner, m_AimUpExtend2, m_AimCenterExtend2, m_AimDownExtend2 );
				OnAimedExtend2( pOwner );
				break;
			case ECM_ZERO_AIMED:
				ProcessTargetTracking_Extend_Delay( pOwner, m_AimUpExtend2, m_AimCenterExtend2, m_AimDownExtend2 );
				OnZeroAimedExtend2( pOwner );
				break;
			case ECM_HEAD_AIMED:
				ProcessTargetTracking_Extend_Delay( pOwner, m_AimUpExtend2, m_AimCenterExtend2, m_AimDownExtend2 );
				OnHeadAimedExtend2( pOwner );
				break;
			default: m_AimedTarget.Clear(); break;
			}

			CheckExtend2AimMoveAniState( pOwner );
		}
		break;
	case WS_EXTEND2_AIMED_FIRE:
		{
			OnFireStateExtend2( pOwner );
		}
		break;
	}

	UpdateAimDir( pOwner );
}



void ioSuddenAttackItem::CallItemProcess( ioBaseChar *pOwner )
{
	if( pOwner->GetState() != CS_DELAY && pOwner->GetState() != CS_RUN && pOwner->GetState() != CS_ATTACK && pOwner->GetState() != CS_JUMP )
	{
		if( m_OwnerPreCharState== CS_DELAY || m_OwnerPreCharState == CS_RUN || m_OwnerPreCharState == CS_ATTACK )
		{
			pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
			m_AimedTarget.Clear();
		}

		if( m_itemMeshType == IMT_EXTRA1 && pOwner->GetState() == CS_JUMP )
		{
			ProcessTargetTracking_Extend_Delay( pOwner, m_AimUpExtend1, m_AimCenterExtend1, m_AimDownExtend1 );
		}

		m_OwnerPreCharState =  pOwner->GetState();
		return;
	}

	if( m_itemMeshType == IMT_EXTRA1 && (pOwner->GetState() == CS_DELAY || pOwner->GetState() == CS_RUN || pOwner->GetState() == CS_ATTACK || pOwner->GetState() == CS_JUMP) )
	{
		if( pOwner->GetTargetMarker() == NULL )
		{
			if( !m_ExtraAttackMesh1.IsEmpty() )
				pOwner->DestroyEntity( m_ExtraAttackMesh1 );
			if( !m_ExtraAttackMesh2.IsEmpty() )
				pOwner->DestroyEntity( m_ExtraAttackMesh2 );
			if( !m_NormalAttackMesh.IsEmpty() )
				pOwner->DestroyEntity( m_NormalAttackMesh );

			if( !m_ExtraAttackMesh1.IsEmpty() )
				pOwner->AddEntity( m_ExtraAttackMesh1 );

			pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
				m_stReadyAimMarker, m_stReadyAimMarker, m_stReadyAimMarker, m_stReadyAimMarker, m_stReadyAimMarker );

			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMING );
			pOwner->SetTargetMarkerRange( m_fExtend1TargetRange );

			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

		}
	}
	//if( m_OwnerPreCharState != CS_DELAY && m_OwnerPreCharState != CS_RUN && m_OwnerPreCharState != CS_ATTACK && m_OwnerPreCharState != CS_JUMP && m_OwnerPreCharState != CS_EXTEND_DASH )
		//ChangeAttackType( pOwner, m_itemMeshType, false, false, true );


	CharState oldCharState = m_OwnerPreCharState;
	m_OwnerPreCharState = pOwner->GetState();

	if( m_itemMeshType == IMT_NORMAL )
		return;

	if( m_itemMeshType == IMT_EXTRA1 )
	{
		if( m_OwnerPreCharState == CS_DELAY || m_OwnerPreCharState == CS_RUN || m_OwnerPreCharState == CS_ATTACK || m_OwnerPreCharState == CS_JUMP )
		{
			CheckAimTarget( pOwner );
			if( oldCharState != m_OwnerPreCharState && m_OwnerPreCharState == CS_DELAY )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					pGrp->ClearAllLoopAni( 0.f, true );
					pGrp->ClearAllActionAni( 0.f, true );

					pGrp->SetLoopAni( m_AimUpExtend1, 0.0f, 0.0f );
					pGrp->SetLoopAni( m_AimDownExtend1, 0.0f, 0.0f );

					//if( !pGrp->GetLoopInPlay( m_AimCenterExtend1 ) )
						pGrp->SetLoopAni( m_AimCenterExtend1, 0.0f, FLOAT1 );
				}
			}

			switch( m_OwnerPreCharState )
			{
			case CS_JUMP:
				ProcessTargetTracking_Extend_Delay( pOwner, m_JumpAimUpExtend1, m_JumpAimCenterExtend1, m_JumpAimDownExtend1 );
				break;
			case CS_DELAY:
				ProcessTargetTracking_Extend_Delay( pOwner, m_AimUpExtend1, m_AimCenterExtend1, m_AimDownExtend1 );
				break;
			case CS_RUN:
				ProcessTargetTracking_Extend_Delay( pOwner, m_AimUpExtend1, m_AimCenterExtend1, m_AimDownExtend1 );
				break;
			case CS_ATTACK:
				ProcessTargetTracking_Extend1_Attack( pOwner, m_FireAimUpExtend1, m_FireAimCenterExtend1, m_FireAimDownExtend1 );
				break;
			}

			UpdateAimDir( pOwner );

		}
	}
	else if( m_itemMeshType == IMT_EXTRA2 )
	{
		/*if( (m_Extend2Mode == ECM_NONE || m_Extend2Mode == ECM_AIMING ) && m_ChargeState != WS_EXTEND2_AIMED_FIRE  )
		{
			if( m_OwnerPreCharState != CS_JUMP )
			{
				m_vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				m_AimedTarget.Clear();
			}
		}*/
		if( m_OwnerPreCharState == CS_RUN || m_OwnerPreCharState == CS_DELAY )
			m_AimedTarget.Clear();

		if( m_OwnerPreCharState == CS_ATTACK || m_OwnerPreCharState == CS_RUN || m_OwnerPreCharState == CS_DELAY )
		{
			/*if( oldCharState != m_OwnerPreCharState )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
				}
			}*/

			UpdateAimDir( pOwner );
		}
	}
}


void ioSuddenAttackItem::ProcessTargetTracking_Extend_Delay( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	//if( m_itemMeshType == IMT_EXTRA1 )
	{
		pUp = pGrp->GetLoopInPlay( stUp );
		pCenter = pGrp->GetLoopInPlay( stCenter );
		pDown = pGrp->GetLoopInPlay( stDown );
	}
	/*else if( m_itemMeshType == IMT_EXTRA2 )
	{
		pUp = pGrp->GetLoopInPlay( m_AimUpExtend2 );
		pCenter = pGrp->GetLoopInPlay( m_AimCenterExtend2 );
		pDown = pGrp->GetLoopInPlay( m_AimDownExtend2 );
	}*/

	D3DXVECTOR3 vAimedDir(0, 0, 0);
	if( pTarget )
		//vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		vAimedDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
	else if( m_bAimedBall && pBall )
		vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}

		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}

	m_vAimedDir = vAimedDir;
	//m_bSetAimedDir = true;
	//m_bAimedState = true;
}


void ioSuddenAttackItem::ProcessTargetTracking_Extend1_Attack( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	pUp = pGrp->GetActionInPlay( stUp );
	pCenter = pGrp->GetActionInPlay( stCenter );
	pDown = pGrp->GetActionInPlay( stDown );

	D3DXVECTOR3 vAimedDir;
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else if( m_bAimedBall && pBall )
		vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}

		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}

	m_vAimedDir = vAimedDir;
}

float ioSuddenAttackItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}


void ioSuddenAttackItem::CheckAimTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_itemMeshType == IMT_NORMAL )
	{
		m_AimedTarget.Clear();
		return;
	}

	if( !pOwner->IsNeedProcess() ) return;

	bool bChange = false;
	int nTargetMarkerState = 0;
	if( m_AimedTarget.IsEmpty() && !m_bAimedBall )
	{
		FindAttackTarget( pOwner, m_itemMeshType );

		/*if( CompareTarget( pOwner ) )
		{
			m_AimedTarget.Clear();
			m_bAimedBall = true;
		}*/

		if( !m_AimedTarget.IsEmpty() )//|| m_bAimedBall )
		{
			bChange = true;
			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			m_dwAimStartTime = FRAMEGETTIME();

			nTargetMarkerState = ioTargetMarker::MS_AIMED;
		}
		else
		{
			if( pOwner->GetMarkerState() != ioTargetMarker::MS_AIMING )
			{
				bChange = true;
				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMING );

				nTargetMarkerState = ioTargetMarker::MS_AIMING;
			}
		}
	}
	else if( !CheckTargetValidate( pOwner, m_itemMeshType ) )
	{
		bChange = true;
		m_dwTrackingEndTime = FRAMEGETTIME();

		FindAttackTarget( pOwner, m_itemMeshType );

		/*if( CompareTarget( pOwner ) )
		{
			m_AimedTarget.Clear();
		}*/
		if( !m_AimedTarget.IsEmpty() )//|| m_bAimedBall )
		{
			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			m_dwAimStartTime = FRAMEGETTIME();
			nTargetMarkerState = ioTargetMarker::MS_AIMED;
		}
		else
		{
			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			nTargetMarkerState = ioTargetMarker::MS_READY;
		}
	}

	if( pOwner->IsNeedSendNetwork() && bChange )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_MARKER;
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		kPacket << nTargetMarkerState;				// change MS_READY
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioSuddenAttackItem::FindAttackTarget(  ioBaseChar *pOwner, ItemMeshType eType )
{
	m_AimedTarget.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	float fTargetAngle;
	float fTargetRange;
	//float fTargetUpHeight;
	//float fTargetDownHeight;
	if( eType == IMT_EXTRA1 )
	{
		fTargetAngle = m_fAngleExtend1;
		fTargetRange = m_fRangeExtend1;
	}
	else
	{
		fTargetAngle = m_fAngleExtend2;
		fTargetRange = m_fRangeExtend2;
	}

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = fTargetRange * fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == pOwner->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		/*float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurFly.m_fTargetDownHeight )
			continue;*/

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_AimedTarget = szTarget;
		}
	}
}

bool ioSuddenAttackItem::CheckTargetValidate( ioBaseChar *pOwner, ItemMeshType eType )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fTargetAngle;
	float fTargetRange;
	if( eType == IMT_EXTRA1 )
	{
		fTargetAngle = m_fAngleExtend1;
		fTargetRange = m_fRangeExtend1;
	}
	else
	{
		fTargetAngle = m_fAngleExtend2;
		fTargetRange = m_fRangeExtend2;
	}

	float fCurAngle = ioMath::ArrangeHead( fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = fTargetRange * fTargetRange;

	/*if( m_bAimedBall )
	{
		ioBall *pBall = pOwner->FindBall();
		if( !pBall ) return false;

		D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				return false;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurFly.m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}
	else*/
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( !pTarget )
			return false;

		if( !IsEnableTargetState( pTarget ) )
			return false;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			return false;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				return false;
		}

		// 높이 체크
		/*float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurFly.m_fTargetDownHeight )
			return false;*/

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}

	return true;
}


void ioSuddenAttackItem::OnCharging( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();
	m_dwMotionEndTime = 0;
	m_bKeyReserved = false;
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
	m_fDashMovedLenth = 0;
	m_dwDashWaitStartTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !pOwner->IsNeedProcess() )	return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_itemMeshType == IMT_NORMAL )
	{
		if( pOwner->IsAttackKeyDown() )
		{
			if( (m_dwAttackStartTime + m_dwFullChargeTime) < dwCurTime )
			{
				ChangeAttackType( pOwner, IMT_EXTRA2, true );
				//pOwner->SetState( CS_DELAY );

				m_bFirstAimMoveMotion = true;
				m_bAimMotionSetted = true;
				ioEntityGroup *pGrp = pOwner->GetGroup();
				pOwner->ChangeDirectionExplicitIfDirChanged( true );

				ChangeExtend2AimingMode( pOwner );
				pOwner->ClearReserveKeyInput();

				pStage->SetChargingCamera( false );
			}
		}
		else if( (m_dwAttackStartTime + m_dwChargeTime) < dwCurTime )
		{
			ChangeAttackType( pOwner, IMT_EXTRA1, true );
			pOwner->SetState( CS_DELAY );
		}
		else 
		{
			//ChangeAttackType( pOwner, IMT_NORMAL, true );
			ChangeToAttackFire( pOwner );
		}
	}
	else if( m_itemMeshType == IMT_EXTRA1 )
	{
		if( m_iCurBullet > 0)
		{
			//ChangeAttackType( pOwner, m_itemMeshType, true );
			ChangeToAttackFire( pOwner );
		}
		else
		{
			pOwner->NormalAttackOver();
			if( SetExtendDefenseState( pOwner ) )
			{
				//send packet
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_EXTEND_DEFENSE );
					kPacket << pOwner->GetCharName();
					kPacket << pOwner->GetHP().m_fCurValue;
					kPacket << m_iCurBullet;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		
	}
	else if( m_itemMeshType == IMT_EXTRA2 )
	{
		if( pOwner->IsAttackKeyDown() )
		{
			if( (m_dwAttackStartTime + m_dwChargeTimeExtend2) < dwCurTime )
			{
				m_bFirstAimMoveMotion = true;
				m_bAimMotionSetted = true;
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					pGrp->ClearAllActionAni( FLOAT100, true );
					pGrp->ClearAllLoopAni( FLOAT100, true );

					pGrp->SetLoopAni( m_AimDelayCenter_Extend2, FLOAT100 );
				}
				pOwner->ChangeDirectionExplicitIfDirChanged( true );

				ChangeExtend2AimingMode( pOwner );
			}
		}
		else
		{
			//ChangeAttackType( pOwner, IMT_NORMAL, true );
			m_Extend2FireMode = ECM_NONE;
			ChangeToAttackFire( pOwner );
		}
	}
}


void ioSuddenAttackItem::ChangeExtend2AimingMode( ioBaseChar *pOwner, bool bSendNet )
{
	//if( pOwner->IsNeedProcess() )
	{
		pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
			m_stAimingMaker, m_stAimingMaker, m_stAimedMarker, m_stZeroAimedMarker,m_stHeadAimedMarker );

		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		pOwner->SetTargetMarkerRange( m_fExtend2TargetRange );

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pGrp->SetLoopAni( m_AimDelayCenter_Extend2, FLOAT100 );
	}

	m_ChargeState = WS_EXTEND2_MODE;
	m_Extend2Mode = ECM_AIMING;

	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( true );
	SetChargeRotate( false );

	m_AimStartDirKey = pOwner->GetCurDirKey();
	m_AimedTarget.Clear();

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = true;

	if( pOwner->IsNeedSendNetwork() && bSendNet )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_AIM_STATE;
		kPacket << (int)m_ChargeState;
		kPacket << (int)m_Extend2Mode;
		kPacket << (int)m_Extend2FireMode;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioSuddenAttackItem::ChangeExtend2AimedMode( ioBaseChar *pOwner, bool bSendNet )
{
	//if( pOwner->IsNeedProcess() )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	}

	m_ChargeState = WS_EXTEND2_MODE;
	m_Extend2Mode = ECM_AIMED;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( 0.0f, true );
		pGrp->ClearAllLoopAni( 0.0f, true );

		pGrp->SetLoopAni( m_AimUpExtend2, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_AimDownExtend2, 0.0f, 0.0f );

		//if( !pGrp->GetLoopInPlay( m_AimCenterExtend2 ) )
			pGrp->SetLoopAni( m_AimCenterExtend2, 0.0f, FLOAT1 );
	}

	m_bFirstAimMoveMotion = true;

	m_dwAimStartTime = FRAMEGETTIME();
	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( true );
	SetChargeRotate( false );

	if( pOwner->IsNeedSendNetwork() && bSendNet )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_AIM_STATE;
		kPacket << (int)m_ChargeState;
		kPacket << (int)m_Extend2Mode;
		kPacket << (int)m_Extend2FireMode;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioSuddenAttackItem::ChangeExtend2ZeroAimedMode( ioBaseChar *pOwner )
{
	//if( pOwner->IsNeedProcess() )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	}

	m_ChargeState = WS_EXTEND2_MODE;
	m_Extend2Mode = ECM_ZERO_AIMED;

	//m_dwAimStartTime = FRAMEGETTIME();
	pOwner->SetChargingState( true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_AIM_STATE;
		kPacket << (int)m_ChargeState;
		kPacket << (int)m_Extend2Mode;
		kPacket << (int)m_Extend2FireMode;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSuddenAttackItem::ChangeExtend2HeadAimedMode( ioBaseChar *pOwner )
{
	//if( pOwner->IsNeedProcess() )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_HEAD_AIMED );
		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	}

	m_ChargeState = WS_EXTEND2_MODE;
	m_Extend2Mode = ECM_HEAD_AIMED;

	pOwner->SetChargingState( true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_AIM_STATE;
		kPacket << (int)m_ChargeState;
		kPacket << (int)m_Extend2Mode;
		kPacket << (int)m_Extend2FireMode;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioSuddenAttackItem::OnAimingExtend2( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !pOwner->IsNeedProcess() )	return;
	
	if( !pOwner->IsAttackKeyDown() )
	{
		m_Extend2FireMode = ECM_AIMING;
		ChangeToAttackFire( pOwner );
		//m_Extend2Mode = ECM_NONE;
		return;
	}

	if( pOwner->IsJumpKeyDown() )
	{
		pOwner->SetAimJumpState( false );
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_JUMP;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}

	ioHashString szTarget;
	m_AimedTarget.Clear();

	FindAttackTarget( pOwner, IMT_EXTRA2 );

	if( !m_AimedTarget.IsEmpty() )
		ChangeExtend2AimedMode( pOwner );
}


void ioSuddenAttackItem::OnAimedExtend2( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )
	{
		m_Extend2FireMode = ECM_AIMED;
		ChangeToAttackFire( pOwner );
		m_Extend2Mode = ECM_NONE;
		return;
	}

	if( pOwner->IsJumpKeyDown() )
	{
		pOwner->SetAimJumpState( false );
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_JUMP;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( !CheckTargetValidate( pOwner, m_itemMeshType ) )
	{
		m_dwTrackingEndTime = FRAMEGETTIME();
		ChangeExtend2AimingMode( pOwner );
		return;
	}


	if( (m_dwAimStartTime + m_dwZeroChargeTimeExtend2) < dwCurTime)
		ChangeExtend2ZeroAimedMode( pOwner );
}


void ioSuddenAttackItem::OnZeroAimedExtend2( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )
	{
		m_Extend2FireMode = ECM_ZERO_AIMED;
		ChangeToAttackFire( pOwner );
		//m_Extend2Mode = ECM_NONE;
		return;
	}

	if( pOwner->IsJumpKeyDown() )
	{
		pOwner->SetAimJumpState( false );
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_JUMP;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( !CheckTargetValidate( pOwner, m_itemMeshType ) )
	{
		m_dwTrackingEndTime = FRAMEGETTIME();
		ChangeExtend2AimingMode( pOwner );
		return;
	}

	if( (m_dwAimStartTime + m_dwHeadChargeTimeExtend2) < dwCurTime)
		ChangeExtend2HeadAimedMode( pOwner );
}


void ioSuddenAttackItem::OnHeadAimedExtend2( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )
	{
		m_Extend2FireMode = ECM_HEAD_AIMED;
		ChangeToAttackFire( pOwner );
		//m_Extend2Mode = ECM_NONE;
		return;
	}

	if( pOwner->IsJumpKeyDown() )
	{
		pOwner->SetAimJumpState( false );
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_JUMP;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( !CheckTargetValidate( pOwner, m_itemMeshType ) )
	{
		m_dwTrackingEndTime = FRAMEGETTIME();
		ChangeExtend2AimingMode( pOwner );
		return;
	}
}


void ioSuddenAttackItem::OnFireStateExtend2( ioBaseChar *pOwner )
{
	/*if( m_bAimEffect && m_dwFireStateEndTime < FRAMEGETTIME() )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}*/

	if( pOwner->CheckDefaultNormalAttack( m_dwFireStartTime,
		m_dwFireEndTime,
		m_dwFireDelayTime ) )
	{
		m_Extend2Mode = ECM_NONE;
		pOwner->SetState( CS_DELAY );
	}
}



void ioSuddenAttackItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	ClearCancelInfo();

	if( m_itemMeshType == IMT_NORMAL )
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iNormalCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
		}
	}
	else if( m_itemMeshType == IMT_EXTRA1 )
	{
		const AttackAttribute *pAttribute = m_FormalAttack_Extend1.GetAttribute( AT_NORMAL, m_iNormalCombo );
		if( pAttribute )
		{
			//Center Animation
			pOwner->SetNormalAttackByAttribute( *pAttribute );
			
			//Up,Down Animation
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllLoopAni( 0.0f, true );
				pGrp->ClearAllActionAni( 0.0f, true );

				pGrp->SetActionAni( m_FireAimUpExtend1, 0.0f, 0.0f );
				pGrp->SetActionAni( m_FireAimDownExtend1, 0.0f, 0.0f );
				pGrp->SetActionAni( m_FireAimCenterExtend1, 0.0f, FLOAT1 );
			}


			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
		}
	}
	else if( m_itemMeshType == IMT_EXTRA2 )
	{
		pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
		pOwner->SetAttackMoveEnable( false );
		SetChargeRotate( false );

		if( m_Extend2Mode == ECM_NONE || m_Extend2Mode == ECM_AIMING )
		{
			const AttackAttribute *pAttribute = m_FormalAttack_Extend2.GetAttribute( AT_NORMAL, (int)m_Extend2FireMode );
			if( pAttribute )
			{
				pOwner->SetNormalAttackByAttribute( *pAttribute );

				m_szCurAni = pAttribute->m_AttackAnimation;
				m_fCurAniRate = pAttribute->m_fAttackAniRate;

				m_dwInputCancelCheckTime = dwCurTime;
				m_dwCancelCheckTime = dwCurTime;
			}

			m_Extend2Mode = ECM_NONE;
		}
		else
		{
			//m_Extend2Mode = ECM_NONE;
			ChangeToExtend2AimedFire( pOwner );
			return;
		}
	}

	pOwner->ApplyNormalAttackResult( m_iNormalCombo );
	m_ChargeState = WS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iNormalCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << (int)m_Extend2FireMode;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioSuddenAttackItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_ATTACK_FIRE:
		{
			rkPacket >> m_iNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			int nTemp;
			rkPacket >> nTemp;
			m_Extend2FireMode = (Extend2ChargeMode)nTemp;

			ChangeToAttackFire( pOwner );
		}
		break;
	case WE_END:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeEnd(pOwner);
		}
		break;
	case WS_CHANGE_MODE:
		{
			int nMode;
			rkPacket >> nMode;
			ChangeAttackType(pOwner, (ItemMeshType)nMode, false);
			//pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

void ioSuddenAttackItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_MARKER:
		{
			bool bAimedBall;
			int  nAimedEffect;

			rkPacket >> m_AimedTarget;
			rkPacket >> bAimedBall;
			rkPacket >> nAimedEffect;
			
			pOwner->SetTargetMarkerTarget( m_AimedTarget, bAimedBall );
			pOwner->SetTargetMarkerState( (ioTargetMarker::MarkerState)nAimedEffect );
		}
		break;
	case SST_AIM_STATE:
		{
			int nTemp;
			rkPacket >> nTemp;
			m_ChargeState = (SuddenCharState)nTemp;
			rkPacket >> nTemp;
			m_Extend2Mode = (Extend2ChargeMode)nTemp;
			rkPacket >> nTemp;
			m_Extend2FireMode = (Extend2ChargeMode)nTemp;
			rkPacket >> m_AimedTarget;

			if( m_ChargeState == WS_EXTEND2_MODE )
			{
				if( m_Extend2Mode == ECM_AIMING )
				{
					ChangeExtend2AimingMode( pOwner );
				}
				else if( m_Extend2Mode == ECM_AIMED )
				{
					ChangeExtend2AimedMode( pOwner );
				}
				else if( m_Extend2Mode == ECM_ZERO_AIMED )
				{
					ChangeExtend2ZeroAimedMode( pOwner );
				}
				else if( m_Extend2Mode == ECM_HEAD_AIMED )
				{
					ChangeExtend2HeadAimedMode( pOwner );
				}
				else
					ChangeExtend2AimingMode( pOwner );
			}
			else if( m_ChargeState == WS_EXTEND2_AIMED_FIRE )
			{
				pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
				pOwner->SetAttackMoveEnable( false );
				SetChargeRotate( false );
				ChangeToExtend2AimedFire( pOwner );
				if( pOwner->GetState() != CS_ATTACK )
					pOwner->SetState( CS_ATTACK );
			}

		}
		break;
	case SST_JUMP:
		pOwner->SetAimJumpState( false );
		break;
	}

}

void ioSuddenAttackItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	//
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}

	D3DXVECTOR3 vAimedDir = m_vAimedDir;

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	if( pArrow && !m_AimedTarget.IsEmpty())
	{
		vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
		pWeapon->SetMoveDir( vAimedDir );
	}

	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	if( pMissile && !m_AimedTarget.IsEmpty())
	{
		vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
		pWeapon->SetMoveDir( vAimedDir );
	}

	if( pArrow && m_itemMeshType == IMT_EXTRA1 )
	{
		WasteBullet();
	}

	if( m_itemMeshType == IMT_EXTRA2 )
		m_AimedTarget.Clear();
}

bool ioSuddenAttackItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == WS_CHARGING )
			return true;

		//if( m_ChargeState == WS_EXTEND2_MODE && m_Extend2Mode != ECM_NONE )
			//return true;
	}

	return false;
}

bool ioSuddenAttackItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case WS_ATTACK_FIRE:
			//if( m_itemMeshType == IMT_EXTRA2 )
				//return false;

			return true;
		/*case WS_EXTEND2_AIMED_FIRE:
			return false;*/

		case WS_EXTEND2_MODE:
			return true;
		}
	}

	return true;
}

bool ioSuddenAttackItem::SetDodgeDefenseState( ioBaseChar *pOwner )
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

	int iAniID = -1;
	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir = -vMoveDir;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
	}

	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
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

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	//

	pOwner->SetState( CS_DODGE_DEFENSE );

	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;
	
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DODGE_DEFENSE );
		kPacket << pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		kPacket << vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioSuddenAttackItem::ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	D3DXVECTOR3 vMoveDir;
	rkPacket >> vMoveDir;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
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

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
	//

	pOwner->SetState( CS_DODGE_DEFENSE );

	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
}

void ioSuddenAttackItem::CheckDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDodgeDefenseEnd > dwCurTime )
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
	}
}

bool ioSuddenAttackItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
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
			return false;

		iter++;
	}

	return false;
}

bool ioSuddenAttackItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioSuddenAttackItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioSuddenAttackItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioSuddenAttackItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}


void ioSuddenAttackItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
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

	// 대기
	pOwner->SetState( CS_DELAY );
}


void ioSuddenAttackItem::ChangeEnd( ioBaseChar *pOwner )
{
	m_ChargeState = WE_END;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetState( CS_DELAY );
}


void ioSuddenAttackItem::FindDashTarget( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_stDashTarget = szTarget;
		}
	}
}


void ioSuddenAttackItem::FindExtendAttackTarget( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fExtendTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fExtendTargetRange * m_fExtendTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fExtendTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fExtendTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_stDashTarget = szTarget;
		}
	}
}


void ioSuddenAttackItem::CheckDashMoveDir( ioBaseChar *pOwner )
{
	DWORD dwMoveDuration = 0;
	if( pOwner->IsNeedProcess() )
	{
		m_vDashMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				m_vDashMoveDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
				m_vDashMoveDir.y = 0.f;
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_stDashTarget );
			if( pTarget )			
			{
				m_vDashMoveDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
				m_vDashMoveDir.y = 0.f;
			}
		}

		D3DXVec3Normalize( &m_vDashMoveDir, &m_vDashMoveDir );
	}
}


bool ioSuddenAttackItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget( false ) )
		return false;

	if( pTarget->IsApplyDownState(false) )
		return false;

	return true;
}


void ioSuddenAttackItem::KeyReserve( ioBaseChar *pOwner )
{
	//if( pOwner->IsAttackKeyDown() )
	if( pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}


int ioSuddenAttackItem::GetMaxBullet()
{
	return m_iMaxBullet;
}

int ioSuddenAttackItem::GetCurBullet()
{
	return m_iCurBullet;
}

int ioSuddenAttackItem::GetNeedBullet()
{
	return m_iCurBullet;
}

void ioSuddenAttackItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	m_iCurBullet = max( 0, min( iCurBullet, m_iMaxBullet ) );
}


/*void ioSuddenAttackItem::DecreaseGauge()
{
	m_fCurGauge = m_fCurGauge - m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}*/


/*void ioSuddenAttackItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_WINDLORD )
			fTypeRate = m_fExtraGaugeAttack;
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_WINDLORD )
			fTypeRate = m_fExtraGaugeAttackDef;
		break;
	case EGT_WOUNDED:
		fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	m_fCurExtraGauge += fCurGauge;
	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
	{
		m_iCurSpecialAttCnt++;
		m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );

		if( m_iCurSpecialAttCnt < m_iMaxSpecialAttCnt )
		{
			m_fCurExtraGauge -= m_fMaxExtraGauge;
			m_fCurExtraGauge = max( 0.0f, min(m_fCurExtraGauge, m_fMaxExtraGauge) );
		}
		else
			m_fCurExtraGauge = m_fMaxExtraGauge;
	}
}*/


void ioSuddenAttackItem::ChangeAttackType( ioBaseChar *pOwner, ItemMeshType meshtype, bool bMustSend, bool bFirst, bool bMustChange )
{
	if( !pOwner)
	{
		LOG.PrintTimeAndLog( 0, "ChangeAttackType Char NULL"  );
		return;
	}

	//같으면 아무것도 하지 말자
	if( m_itemMeshType == meshtype && bFirst == false && bMustSend == false && !bMustChange )
		return;

	if( m_itemMeshType != meshtype )
		m_iNormalCombo = 0;

	m_itemMeshType = meshtype;

	if( !m_ExtraAttackMesh1.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh1 );
	if( !m_ExtraAttackMesh2.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh2 );
	if( !m_NormalAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_NormalAttackMesh );

	if( meshtype == IMT_NORMAL )
	{
		if( !m_NormalAttackMesh.IsEmpty() )
			pOwner->AddEntity( m_NormalAttackMesh );

		pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
		pOwner->SetAttackMoveEnable( false );
		SetChargeRotate( false );

		m_bExtendDefenseDash = false;
		m_bExtendDefenseJump = false;
	}

	if( meshtype == IMT_EXTRA1 )
	{
		m_bExtendDefenseDash = true;
		m_bExtendDefenseJump = true;

		if( !m_ExtraAttackMesh1.IsEmpty() )
			pOwner->AddEntity( m_ExtraAttackMesh1 );

		pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
			m_stReadyAimMarker, m_stReadyAimMarker, m_stReadyAimMarker, m_stReadyAimMarker,m_stReadyAimMarker );

		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMING );
		pOwner->SetTargetMarkerRange( m_fExtend1TargetRange );

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

		pOwner->SetAttackMoveEnable( true );

		//////////////////////////////////////////////////////////////////////////
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( m_AimUpExtend1, 0.0f, 0.0f );
			pGrp->SetLoopAni( m_AimDownExtend1, 0.0f, 0.0f );

			if( !pGrp->GetLoopInPlay( m_AimCenterExtend1 ) )
				pGrp->SetLoopAni( m_AimCenterExtend1, 0.0f, FLOAT1 );
		}
		//////////////////////////////////////////////////////////////////////////
		SetChargeRotate( true );
	}

	if( meshtype == IMT_EXTRA2 )
	{
		m_bExtendDefenseDash = false;
		m_bExtendDefenseJump = false;

		if( !m_ExtraAttackMesh2.IsEmpty() )
			pOwner->AddEntity( m_ExtraAttackMesh2 );

		pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
		pOwner->SetAttackMoveEnable( false );
		SetChargeRotate( false );
	}

	if( !bFirst )
		ItemEffectSet(pOwner);

	//Send Packet
	if(bMustSend && pOwner->IsNeedSendNetwork () )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_CHANGE_MODE;
		kPacket << m_itemMeshType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioSuddenAttackItem::HasDelayAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::HasDelayAnimation();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return !m_DelayAnimation_extra1.IsEmpty();
	return !m_DelayAnimation_extra2.IsEmpty();
}

const ioHashString& ioSuddenAttackItem::GetDelayAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetDelayAnimation();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_DelayAnimation_extra1;
	
	return m_DelayAnimation_extra2;
}


bool ioSuddenAttackItem::HasRunAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::HasRunAnimation();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return !m_RunAnimation_extra1.IsEmpty();

	return !m_RunAnimation_extra2.IsEmpty();
}


const ioHashString& ioSuddenAttackItem::GetRunAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetRunAnimation();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_RunAnimation_extra1;

	return m_RunAnimation_extra2;
}


bool ioSuddenAttackItem::HasDashRunAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::HasDashRunAnimation();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return !m_DashAnimation_extra1.IsEmpty();

	return !m_DashAnimation_extra2.IsEmpty();
}


const ioHashString& ioSuddenAttackItem::GetDashRunAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetDashRunAnimation();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_DashAnimation_extra1;

	return m_DashAnimation_extra2;
}


ioHashString ioSuddenAttackItem::GetDefenseAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetDefenseAnimation();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_DefenseAnimation_extra1;

	return m_DefenseAnimation_extra2;
}


int ioSuddenAttackItem::GetMaxCombo() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetNormalMaxCombo();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_FormalAttack_Extend1.GetNormalMaxCombo();
	
	return m_FormalAttack_Extend2.GetNormalMaxCombo();
}


int ioSuddenAttackItem::GetExtend_1_Combo() const
{
	return m_FormalAttack_Extend1.GetNormalMaxCombo();
}

int ioSuddenAttackItem::GetExtend_2_Combo() const
{
	return m_FormalAttack_Extend2.GetNormalMaxCombo();
}

void ioSuddenAttackItem::GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType )
{
	float fRangeCur;
	float fmin_angleCur;
	float fmax_angleCur;

	if( m_itemMeshType == IMT_NORMAL )
	{
		fRangeCur = m_fAutoTargetRange;
		fmin_angleCur = m_fAutoTargetMinAngle;
		fmax_angleCur = m_fAutoTargetMaxAngle;
	}
	else
	{
		fRangeCur = m_fAutoTargetRange_monk;
		fmin_angleCur = m_fAutoTargetMinAngle_monk;
		fmax_angleCur = m_fAutoTargetMaxAngle_monk;
	}

	switch( eType )
	{
	case ATT_NORMAL:
		fRange = fRangeCur;
		fMinAngle = fmin_angleCur;
		fMaxAngle = fmax_angleCur;
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

	fRange = fRangeCur;
	fMinAngle = fmin_angleCur;
	fMaxAngle = fmax_angleCur;

	return;
}

const AttackAttribute* ioSuddenAttackItem::GetAttackAttribute( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetAttribute( eType, iSubIdx );
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_FormalAttack_Extend1.GetAttribute( eType, iSubIdx );
	
	return m_FormalAttack_Extend2.GetAttribute( eType, iSubIdx );
}


const WeaponInfoList& ioSuddenAttackItem::GetAttackAttributeIdx( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetAttackAttributeIdx( eType, iSubIdx );
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_FormalAttack_Extend1.GetAttackAttributeIdx( eType, iSubIdx );

	return m_FormalAttack_Extend2.GetAttackAttributeIdx( eType, iSubIdx );
}


const ioHashString& ioSuddenAttackItem::GetAniFile( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetAniFile( eType, iSubIdx );
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_FormalAttack_Extend1.GetAniFile( eType, iSubIdx );

	return m_FormalAttack_Extend2.GetAniFile( eType, iSubIdx );
}

float ioSuddenAttackItem::GetAttackAniRate( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetAttackAniRate( eType, iSubIdx );
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_FormalAttack_Extend1.GetAttackAniRate( eType, iSubIdx );

	return m_FormalAttack_Extend2.GetAttackAniRate( eType, iSubIdx );
}

const ioHashString& ioSuddenAttackItem::GetEndAniFile( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetEndAniFile( eType, iSubIdx );
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_FormalAttack_Extend1.GetEndAniFile( eType, iSubIdx );

	return m_FormalAttack_Extend2.GetEndAniFile( eType, iSubIdx );
}

float ioSuddenAttackItem::GetAttackEndAniRate( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetAttackEndAniRate( eType, iSubIdx );
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_FormalAttack_Extend1.GetAttackEndAniRate( eType, iSubIdx );

	return m_FormalAttack_Extend2.GetAttackEndAniRate( eType, iSubIdx );
}


const ForceInfoList& ioSuddenAttackItem::GetForceInfoList( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetForceInfoList( eType, iSubIdx );
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_FormalAttack_Extend1.GetForceInfoList( eType, iSubIdx );

	return m_FormalAttack_Extend2.GetForceInfoList( eType, iSubIdx );
}


int ioSuddenAttackItem::GetExtraDashAttackMax() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetExtraDashAttackMax();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_FormalAttack_Extend1.GetExtraDashAttackMax();

	return m_FormalAttack_Extend2.GetExtraDashAttackMax();
}


const ioHashString& ioSuddenAttackItem::GetJumppingAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetJumppingAnimation();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_JumppingAnimationExtend1;

	return m_JumppingAnimationExtend2;
}


const ioHashString& ioSuddenAttackItem::GetExtendJumppingAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetExtendJumppingAnimation();
	else if( m_itemMeshType == IMT_EXTRA1 )
		return m_ExtendJumppingAnimationExtend1;

	return m_ExtendJumppingAnimationExtend1;
}

void ioSuddenAttackItem::ShowWeapon( ioBaseChar *pOwner )
{
	if( !m_ExtraAttackMesh1.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh1 );
	if( !m_ExtraAttackMesh2.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh2 );
	if( !m_NormalAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_NormalAttackMesh );

	if( m_itemMeshType == IMT_NORMAL )
	{
		if( !m_NormalAttackMesh.IsEmpty() )
			pOwner->AddEntity( m_NormalAttackMesh );
	}

	if( m_itemMeshType == IMT_EXTRA1 )
	{
		if( !m_ExtraAttackMesh1.IsEmpty() )
			pOwner->AddEntity( m_ExtraAttackMesh1 );
	}

	if( m_itemMeshType == IMT_EXTRA2 )
	{
		if( !m_ExtraAttackMesh2.IsEmpty() )
			pOwner->AddEntity( m_ExtraAttackMesh2 );
	}
}


void ioSuddenAttackItem::HideWeapon( ioBaseChar *pOwner )
{
	if( !m_ExtraAttackMesh1.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh1 );
	if( !m_ExtraAttackMesh2.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh2 );
	if( !m_NormalAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_NormalAttackMesh );
}


void ioSuddenAttackItem::OnReleased( ioBaseChar *pOwner )
{
	//m_fCurGauge = 0.0f;
	if( !m_ExtraAttackMesh1.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh1 );
	if( !m_ExtraAttackMesh2.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh2 );
	if( !m_NormalAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_NormalAttackMesh );

	m_itemMeshType = IMT_NORMAL;
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
	m_AimedTarget.Clear();

	pOwner->SetAttackMoveEnable( false );

	ioAttackableItem::OnReleased( pOwner );
}

void ioSuddenAttackItem::ItemEffectSet( ioBaseChar *pOwner )
{
	bool bBaseEquipMesh = false;

	if( pOwner )
	{
		if( m_itemMeshType == IMT_NORMAL )
		{
			if( !m_CurItemEquipEffect.IsEmpty() )
				pOwner->EndEffect( m_CurItemEquipEffect, false );

			if( pOwner->IsMale() )
			{
				if( !m_ItemEquipEffect.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffect );
					m_CurItemEquipEffect = m_ItemEquipEffect;
				}
			}
			else
			{
				if( !m_ItemEquipEffectHw.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffectHw );
					m_CurItemEquipEffect = m_ItemEquipEffectHw;
				}
				else if( !m_ItemEquipEffect.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffect );
					m_CurItemEquipEffect = m_ItemEquipEffect;
				}
			}
		}
		else
		{
			if( !m_CurItemEquipEffect.IsEmpty() )
			{
				pOwner->EndEffect( m_CurItemEquipEffect, false );
				m_CurItemEquipEffect = "";
			}
		}
	}
}


void ioSuddenAttackItem::SetVisible( ioBaseChar *pOwner, bool bVisible )
{
	m_bVisible = bVisible;

	/*bool bBaseEquipMesh = false;
	if( g_ItemMaker.IsBaseEquipMesh() && (GetType() == ES_WEAPON ||	GetType() == ES_OBJECT) )
		bBaseEquipMesh = true;*/

	if( bVisible )
	{
		if( m_itemMeshType == IMT_NORMAL )
			pOwner->AddEntity( m_NormalAttackMesh );
		else if( m_itemMeshType == IMT_EXTRA1 )
			pOwner->AddEntity( m_ExtraAttackMesh1 );
		else if(m_itemMeshType == IMT_EXTRA2 )
			pOwner->AddEntity( m_ExtraAttackMesh2 );
		else 
			pOwner->AddEntity( m_NormalAttackMesh );
	}
	else
	{
		pOwner->DestroyEntity( m_NormalAttackMesh );
		pOwner->DestroyEntity( m_ExtraAttackMesh1 );
		pOwner->DestroyEntity( m_ExtraAttackMesh2 );
	}

	return;

	/*if( pOwner && !bBaseEquipMesh && m_itemMeshType == IMT_NORMAL )
	{
		if( !m_CurItemEquipEffect.IsEmpty() )
			pOwner->EndEffect( m_CurItemEquipEffect, false );

		if( m_bVisible && !m_ItemEquipEffect.IsEmpty() )
		{
			if( pOwner->IsMale() )
			{
				if( !m_ItemEquipEffect.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffect );
					m_CurItemEquipEffect = m_ItemEquipEffect;
				}
			}
			else
			{
				if( !m_ItemEquipEffectHw.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffectHw );
					m_CurItemEquipEffect = m_ItemEquipEffectHw;
				}
				else if( !m_ItemEquipEffect.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffect );
					m_CurItemEquipEffect = m_ItemEquipEffect;
				}
			}
		}
	}*/
}

void ioSuddenAttackItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( m_itemMeshType == IMT_NORMAL )
	{
		pOwner->DestroyEntity( m_ExtraAttackMesh1 );
		pOwner->DestroyEntity( m_ExtraAttackMesh2 );
	}
	else if( m_itemMeshType == IMT_EXTRA1 )
	{
		pOwner->DestroyEntity( m_ExtraAttackMesh2 );
		pOwner->DestroyEntity( m_NormalAttackMesh );
	}
	else
	{
		pOwner->DestroyEntity( m_ExtraAttackMesh1 );
		pOwner->DestroyEntity( m_NormalAttackMesh );
	}

	ChangeAttackType(pOwner, IMT_NORMAL, false, true);
}


float ioSuddenAttackItem::GetAttackSpeedRate() const
{
	switch( m_itemMeshType )
	{
	case IMT_NORMAL:
		return FLOAT1;
	case IMT_EXTRA1:
		return m_fMoveSpeedRateExtend1;
	case IMT_EXTRA2:
		if( m_Extend2Mode == ECM_NONE )
			return m_fMoveSpeedRateExtend2;

		return m_fMoveSpeedRateExtend2_Sniper;
	}
	
	return 0.f;
}


void ioSuddenAttackItem::UpdateAimDir( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bStateCheck = false;

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	/*if( !pTarget )
	{
		m_fCurAngle = 0.0f;
		pBiped->ReleaseExtraRotation();
		return;
	}*/

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
		vTargetPos = pTarget->GetWorldPosition();

	float fRotAngle = 0.0f;

	/*if( m_bAimJumpAttack || m_FireState == FS_AIMED_FIRE )
	{
		if( m_dwFireStateEndTime < dwCurTime )
		{
			float fTotal, fGap;
			DWORD dwEndTime = m_dwFireStartTime + m_dwFireEndTime;
			fTotal = fGap = 0.0f;

			if( dwEndTime > m_dwFireStateEndTime )
				fTotal = (float)(dwEndTime - m_dwFireStateEndTime);

			if( dwEndTime > dwCurTime )
				fGap = (float)(dwEndTime - dwCurTime);

			float fRate = 0.0f;
			if( fTotal > 0.0f )
				fRate = fGap / fTotal;

			fRotAngle = m_fCurAngle * fRate;
		}
		else
		{
			fRotAngle = m_fCurAngle;
		}
	}
	else*/
	{
		bool bTarget = true;
		if( pTarget && !pTarget->IsEnableAimTarget(false) )
			bTarget = false;
		else if( m_bAimedBall && !pBall )
			bTarget = false;
		else if( !pTarget && !m_bAimedBall )
			bTarget = false;

		if( !bTarget )
		{
			float fTotal, fGap;
			DWORD dwEndTime = m_dwTrackingEndTime + m_dwAimBipeReturnTime;
			fTotal = fGap = 0.0f;

			fTotal = FLOAT500;
			if( dwEndTime > dwCurTime )
				fGap = (float)(dwEndTime - dwCurTime);
			else
				m_dwTrackingEndTime = 0;

			if( m_dwTrackingEndTime == 0 )
			{
				m_fCurAngle = 0.0f;
				pBiped->ReleaseExtraRotation();
				return;
			}

			float fRate = 0.0f;
			if( fTotal > 0.0f )
				fRate = fGap / fTotal;

			fRotAngle = m_fCurAngle * fRate;
		}
		else
		{
			D3DXVECTOR3 vSightDir = vTargetPos - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &vSightDir, &vSightDir );

			D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
			D3DXVec3Normalize( &vXZDir, &vXZDir );

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

			float fCurYaw, fTargetYaw;
			fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
			fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
			fCurYaw = RADtoDEG( ioMath::QuaterToYaw(pOwner->GetTargetRot()) );
			fCurYaw = ioMath::ArrangeHead( fCurYaw );

			m_fCurAngle = DEGtoRAD( ioMath::AngleGap( fCurYaw, fTargetYaw ) );

			float fAimTimeRate = 1.0;
			DWORD dwTimeGap = dwCurTime - m_dwAimStartTime;
			if( dwTimeGap < m_dwAimTime )
			{
				fAimTimeRate = (float)dwTimeGap / (float)m_dwAimTime;
			}

			fRotAngle = m_fCurAngle * fAimTimeRate;
		}
	}

	D3DXQUATERNION qtExtraRot;
	D3DXQuaternionRotationYawPitchRoll( &qtExtraRot, 0.0f, fRotAngle, 0.0f );
	pBiped->SetExtraRotation( qtExtraRot, true );
}


D3DXVECTOR3 ioSuddenAttackItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget )//&& !m_bAimedBall )
	{
		m_vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		return m_vAimedDir;
	}

	//ioBall *pBall = pOwner->FindBall();
	//if( !pTarget && !pBall ) return m_vAimedDir;

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();
	vTargetPos = pTarget->GetMidPositionByRate( m_fTargetAimRate );

	D3DXVECTOR3 vAimedDir =  vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	/*vAimedDir.x = m_vAimedDir.x;
	vAimedDir.z = m_vAimedDir.z;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );*/

	m_vAimedDir = vAimedDir;

	return vAimedDir;
}


void ioSuddenAttackItem::WasteBullet()
{
	if( m_iMaxBullet == 0 ) return;

	m_iCurBullet--;
	m_iCurBullet = max( m_iCurBullet, 0 );
}


bool ioSuddenAttackItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	float fTimeRate = m_fReloadTimeRate;

	int iLevelCount = m_LevelTimeRate.size();
	if( iLevelCount > 1 )
	{
		int iDividCnt, iNeedCnt;
		iDividCnt = m_iMaxBullet / iLevelCount;
		iNeedCnt = m_iMaxBullet - m_iCurBullet;

		LevelTimeRate::iterator iter = m_LevelTimeRate.begin();
		for( int i=1; i <= iLevelCount; i++ )
		{
			if( iNeedCnt <= iDividCnt * i )
			{
				fTimeRate *= *iter;
				break;
			}

			++iter;
		}
	}
	else
	{
		fTimeRate = m_fReloadTimeRate;
	}

	m_fReloadTimeModifyRate = m_fReloadTimeRate;

	m_bSetReloadMoveAni = false;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );

	m_dwReloadStartTime = FRAMEGETTIME();
	m_dwReloadEndTime = m_dwReloadStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate - FLOAT100;

	m_dwReloadGapTime = ( m_dwReloadEndTime - m_dwReloadStartTime ) / m_iMaxBullet;
	m_dwNextReloadTime = m_dwReloadStartTime + m_dwReloadGapTime;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetReloadMoveEnable( true );

	/*if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->ShowWeaponItemMesh( false );
		pOwner->AttachEffect( m_ReloadEffect );
	}*/

	return true;
}


void ioSuddenAttackItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwReloadEndTime + pOwner->GetActionStopTotalDelay() > dwCurTime )
	{
		if( m_dwNextReloadTime < dwCurTime )
		{
			m_iCurBullet++;
			m_iCurBullet = min( m_iCurBullet, m_iMaxBullet );
			m_dwNextReloadTime += m_dwReloadGapTime;
		}
	}
	else
	{
		m_iCurBullet = m_iMaxBullet;
		m_bSetReloadMoveAni = false;
		m_ChargeState = WS_NONE;
		pOwner->SetReloadMoveEnable( false );
		pOwner->SetState( CS_DELAY );

		/*if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << false;	//AimJump
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			kPacket << m_iChargeCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}*/
	}

	g_TutorialMgr.ActionWeaponCharge();
}


void ioSuddenAttackItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanReloadMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetReloadMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ReloadMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ReloadAnimation );

			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, false );
			pGrp->ClearAllLoopAni( FLOAT100, false );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = true;
		}
	}
	else
	{
		if( m_bSetReloadMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ReloadAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ReloadMoveAnimation );

			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, false );
			pGrp->ClearAllLoopAni( FLOAT100, false );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = false;
		}
	}
}


bool ioSuddenAttackItem::CheckExtendDefenseEnable( ioBaseChar *pChar )
{
	if( m_itemMeshType != IMT_EXTRA1 )
		return false;

	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	return true;
}


void ioSuddenAttackItem::CheckExtend2AimMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstAimMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_AimMoveFront_Extend2);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveBack_Extend2);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveLeft_Extend2);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveRight_Extend2);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );

			m_bAimMotionSetted = false;
			m_bFirstAimMoveMotion = false;

			if( bChangeDir )
				m_bChangeDir = true;

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_AimMoveFront_Extend2, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
			}
			else
			{
				ioUserKeyInput::DirKeyInput eXformDir;
				eXformDir = pOwner->GetXformDir( m_AimStartDirKey );

				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_AimMoveFront_Extend2, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_AimMoveRight_Extend2, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_AimMoveBack_Extend2, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_AimMoveLeft_Extend2, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				}
			}

			g_TutorialMgr.ActionAimingMove();
		}
		else
		{
			m_bChangeDir = false;
		}
	}
	else
	{
		if( !m_bAimMotionSetted )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_AimMoveFront_Extend2);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveBack_Extend2);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveLeft_Extend2);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveRight_Extend2);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bAimMotionSetted = true;
			m_bChangeDir = false;
		}
	}
}


void ioSuddenAttackItem::ChangeToExtend2AimedFire( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttribute = m_FormalAttack_Extend2.GetAttribute( AT_NORMAL, (int)m_Extend2FireMode );
	//if( pOwner->GetState() == CS_JUMP || !pAttribute  )
		//return;
	if( !pAttribute  )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int	  iAniID	  = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
	float fTimeRate   = pAttribute->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttribute->m_dwPreDelay;
	m_dwFireDelayTime = pAttribute->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, pAttribute->m_vWeaponInfoList,
		FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	if( m_Extend2FireMode == ECM_HEAD_AIMED )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget )
		{
			pOwner->SetTargetRotToTargetPos( pTarget, true );
		}

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	}
	else
	{
		float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
		if( pAttribute->m_AttackEndAnimation.IsEmpty() )
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp_Extend2( pGrp, fWeight, fTimeRate, dwPreDelay, true );
			}
			else
			{
				AimedFireToDown_Extend2( pGrp, fWeight, fTimeRate, dwPreDelay, true );
			}
		}
		else
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp_Extend2( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}
			else
			{
				AimedFireToDown_Extend2( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}

			float fEndAniRate = pAttribute->m_fAttackEndAniRate;
			pGrp->SetActionAni( pAttribute->m_AttackEndAnimation,
				0.0f, FLOAT100,
				1.0f, FLOAT1/fEndAniRate,
				m_dwFireEndTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( pAttribute->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
		}
	}
	

	pOwner->SetReservedSliding( pAttribute->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeState = WS_EXTEND2_AIMED_FIRE;
	m_Extend2Mode = ECM_NONE;

	pOwner->ApplyNormalAttackResult( 0, true );
	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_AIM_STATE;
		kPacket << (int)m_ChargeState;
		kPacket << (int)m_Extend2Mode;
		kPacket << (int)m_Extend2FireMode;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioSuddenAttackItem::AimedFireToUp_Extend2( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;
	/*if( m_bAimJumpAttack )
	{
		szFireUp = m_JumpFireUp;
		szFireCenter = m_JumpFireCenter;
	}
	else 
	*/if( m_Extend2FireMode == ECM_HEAD_AIMED )
	{
		szFireUp = m_FireAimUpExtend2_Head;
		szFireCenter = m_FireAimCenterExtend2_Head;
	}
	else
	{
		szFireUp = m_FireAimUpExtend2;
		szFireCenter = m_FireAimCenterExtend2;
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp,
			FLOAT100, fFadeOut,
			fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter,
		FLOAT100, fFadeOut,
		1.0f - fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}

void ioSuddenAttackItem::AimedFireToDown_Extend2( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;
	bool bAniFx1 = true;
	bool bAniFx2 = false;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	ioHashString szFireDown, szFireCenter;
	/*if( m_bAimJumpAttack )
	{
		szFireDown = m_JumpFireDown;
		szFireCenter = m_JumpFireCenter;
	}
	else*/ 
	if( m_Extend2FireMode == ECM_HEAD_AIMED )
	{
		szFireDown = m_FireAimDownExtend2_Head;
		szFireCenter = m_FireAimCenterExtend2_Head;
	}
	else
	{
		szFireDown = m_FireAimDownExtend2;
		szFireCenter = m_FireAimCenterExtend2;
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter,
			FLOAT100, fFadeOut,
			1.0f - fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireDown,
		FLOAT100, fFadeOut,
		fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}


const ioHashString& ioSuddenAttackItem::GetAimedTarget()
{
	return m_AimedTarget;
}


bool ioSuddenAttackItem::CheckLanding( ioBaseChar *pChar )
{
	if( m_itemMeshType == IMT_EXTRA2 )
	{
		if( pChar->IsNeedProcess() )
		{
			if( !m_AimedTarget.IsEmpty() )
			{
				if( !pChar->IsAttackKeyDown() )
				{
					m_Extend2FireMode = ECM_NONE;
					ChangeToAttackFire( pChar );
				}
			}
			/*if( !pChar->IsAttackKeyDown() )
			{
				pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
				m_AimedTarget.Clear();
				return true;
			}*/
		}
		
		/*if( !m_AimedTarget.IsEmpty() )
		{
			ChangeExtend2AimedMode( pChar );
		}
		else
		{
			ChangeExtend2AimingMode( pChar );
		}*/

		return true;
	}

	/*if( m_itemMeshType != IMT_EXTRA1 )
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		m_AimedTarget.Clear();
	}*/
	
	//SetNoneState();
	return true;
}

const ioHashString& ioSuddenAttackItem::GetAimReadyMarker()
{
	return m_stReadyAimMarker;
}


const ioHashString& ioSuddenAttackItem::GetAimingMarker()
{
	return m_stAimingMaker;
}


const ioHashString& ioSuddenAttackItem::GetAimedMarker()
{
	return m_stAimedMarker;
}


const ioHashString& ioSuddenAttackItem::GetZeroAimedMarker()
{
	return m_stZeroAimedMarker;
}


const ioHashString& ioSuddenAttackItem::GetHeadAimedMarker()
{
	return m_stHeadAimedMarker;
}


float ioSuddenAttackItem::GetExtend2Range()
{
	return m_fExtend2TargetRange;
}


bool ioSuddenAttackItem::HasJumpEndAnimation( bool bUseAttack, ioBaseChar *pOwner ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::HasJumpEndAnimation( bUseAttack, pOwner );

	if( m_itemMeshType == IMT_EXTRA1 )
		return !m_JumpEndAnimationExtend1.IsEmpty();

	return !m_JumpEndAnimationExtend2.IsEmpty();
}

const ioHashString& ioSuddenAttackItem::GetJumpEndAnimation( bool bUseAttack, ioBaseChar *pOwner ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetJumpEndAnimation( bUseAttack, pOwner );

	if( m_itemMeshType == IMT_EXTRA1 )
		return m_JumpEndAnimationExtend1;

	return m_JumpEndAnimationExtend2;
}

bool ioSuddenAttackItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	/*if( m_itemMeshType != IMT_EXTRA1 )
		return true;

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime > m_dwExtend1FireTimeStart+m_dwExtend1FireTimeGap )
		return true;

	return false;*/

	return true;
}


void ioSuddenAttackItem::ClearAimedTarget()
{
	m_AimedTarget.Clear();
}


void ioSuddenAttackItem::ProcessLanding( ioBaseChar *pOwner )
{
	/*if( pOwner->IsNeedProcess() )
	{
		if( m_itemMeshType == IMT_EXTRA2 && !m_AimedTarget.IsEmpty() && !pOwner->IsAttackKeyDown() )
		{
			m_Extend2FireMode = ECM_AIMED;
			ChangeToAttackFire( pOwner );
			pOwner->SetState( CS_ATTACK );
			return;
		}

		if( m_itemMeshType == IMT_EXTRA2 && m_AimedTarget.IsEmpty() && pOwner->GetMarkerState() != ioTargetMarker::MS_NONE && !pOwner->IsAttackKeyDown() )
		{
			m_Extend2FireMode = ECM_AIMED;
			ChangeToAttackFire( pOwner );
			pOwner->SetState( CS_ATTACK );
			return;
		}
	}*/
}


const ioHashString& ioSuddenAttackItem::GetDelayCameraBuffExtend2() const
{
	return m_DelayCameraBuffExtend2;
}