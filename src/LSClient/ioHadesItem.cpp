
#include "stdafx.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"

ioHadesItem::ioHadesItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_HadesAttackState = MFS_NONE;
	m_fFlyCurRange = 0.0f;
	m_CurAttackInfo.Init();

	m_dwAttackStartTime = 0;
	m_dwFireMotionTime = 0;

	m_Target.Clear();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;

	m_fCurGauge = 0.0f;
	m_dwWeaponIndex = 0;
}

ioHadesItem::ioHadesItem( const ioHadesItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_AttackInfo( rhs.m_AttackInfo ),
m_HalfAttackInfo( rhs.m_HalfAttackInfo ),
m_bInitGaugeDown( rhs.m_bInitGaugeDown ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
m_fDecreaseGauge( rhs.m_fDecreaseGauge ),
m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
m_fHighDecreaseGauge( rhs.m_fHighDecreaseGauge ),
m_stGatheringAni( rhs.m_stGatheringAni ),
m_fGatheringAniRate( rhs.m_fGatheringAniRate ),
m_fAirAttackTypeCheckHeight( rhs.m_fAirAttackTypeCheckHeight ),
m_JumpHalfChargeInfo( rhs.m_JumpHalfChargeInfo ),
m_JumpFullChargeInfo( rhs.m_JumpFullChargeInfo ),
m_stLockControlBuff( rhs.m_stLockControlBuff ),
m_DashHalfChargeInfo( rhs.m_DashHalfChargeInfo ),
m_DashFullChargeInfo( rhs.m_DashFullChargeInfo ),
m_fGoalOffSetLength( rhs.m_fGoalOffSetLength ),
m_fCheckWeaponLength( rhs.m_fCheckWeaponLength )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt =0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_HadesAttackState = MFS_NONE;
	m_fFlyCurRange = 0.0f;
	m_CurAttackInfo.Init();

	m_dwAttackStartTime = 0;

	m_Target.Clear();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;

	m_fCurGauge = 0.0f;

	m_dwMotionEndTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwWeaponIndex = 0;
	m_dwHitedTime = 0;
	m_fCurMoveRange = 0;

	m_bAutoMove = false;
}

ioHadesItem::~ioHadesItem()
{
}

void ioHadesItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_stGatheringAni = szBuf;
	m_fGatheringAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	LoadHadesInfo( rkLoader );

	m_bInitGaugeDown = rkLoader.LoadBool_e( "init_gauge_down", false );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGauge = rkLoader.LoadFloat_e( "high_decrease_gauge", 3.0f );
}

void ioHadesItem::LoadHadesInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	m_AttackInfo.Init();
	m_HalfAttackInfo.Init();

	// full charging
	rkLoader.LoadString_e( "full_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "full_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "full_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAniDown = szBuf;

	// half charging
	rkLoader.LoadString_e( "half_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_HalfAttackInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "half_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_HalfAttackInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "half_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_HalfAttackInfo.m_StartAniDown = szBuf;

	//공격끝 모션	
	rkLoader.LoadString_e( "half_attack_end_motion", "", szBuf, MAX_PATH );
	m_HalfAttackInfo.m_AttackEndAni = szBuf;
	m_HalfAttackInfo.m_fAttackEndAniRate = rkLoader.LoadFloat_e( "half_attack_end_motion_rate", FLOAT1 );
	m_HalfAttackInfo.m_fEndJumpPower = rkLoader.LoadFloat_e( "half_attack_move_end_jump_power", 0.f );
	m_HalfAttackInfo.m_fAttackMoveMaxRange = rkLoader.LoadFloat_e( "half_attack_move_max_range", 0.f );
	m_HalfAttackInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "half_attack_move_speed", 0.f );
	
	//시작
	wsprintf_e( szKey, "full_attack" );
	LoadAttackAttribute( szKey, m_AttackInfo.m_FirstAttack, rkLoader );

	//시작
	wsprintf_e( szKey, "half_attack" );
	LoadAttackAttribute( szKey, m_HalfAttackInfo.m_FirstAttack, rkLoader );

	//이동공격
	wsprintf_e( szKey, "hades_full_move_attack" );
	LoadAttackAttribute( szKey, m_AttackInfo.m_MoveAttackInfo, rkLoader );

	wsprintf_e( szKey, "hades_half_move_attack" );
	LoadAttackAttribute( szKey, m_HalfAttackInfo.m_MoveAttackInfo, rkLoader );
	
	// 타겟 대상 검색용
	m_AttackInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "full_target_wound_type", TWT_ALL );
	m_AttackInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "full_target_angle", 0.0f );
	m_AttackInfo.m_fTargetRange = rkLoader.LoadFloat_e( "full_target_range", 0.0f );
	m_AttackInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "full_target_up_height", 0.0f );
	m_AttackInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "full_target_down_height", 0.0f );

	//잡고 있을때 모션 정보와 기다리는 시간
	rkLoader.LoadString_e( "full_delay_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackLoopAniCenter = szBuf;
	rkLoader.LoadString_e( "full_delay_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackLoopAniUp = szBuf;
	rkLoader.LoadString_e( "full_delay_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackLoopAniDown = szBuf;

	m_AttackInfo.m_fAttackLoopAniRate = rkLoader.LoadFloat_e( "full_delay_attack_actionAni_Rate", FLOAT1 );
	m_AttackInfo.m_dwWaitMaxDelayTime = (DWORD)rkLoader.LoadInt_e( "full_delay_attack_max_wait_time", 0 );

	//공격끝 모션	
	rkLoader.LoadString_e( "full_attack_end_motion", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackEndAni = szBuf;
	m_AttackInfo.m_fAttackEndAniRate = rkLoader.LoadFloat_e( "full_attack_end_motion_rate", FLOAT1 );
	m_AttackInfo.m_fEndJumpPower = rkLoader.LoadFloat_e( "full_attack_move_end_jump_power", 0.f );
	m_AttackInfo.m_fAttackMoveMaxRange = rkLoader.LoadFloat_e( "full_attack_move_max_range", 0.f );
	m_AttackInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "full_attack_move_speed", 0.f );

	// 타겟 대상 검색용
	m_HalfAttackInfo.m_fAttackMoveAngle = rkLoader.LoadFloat_e( "half_attack_move_angle",  0 );
	m_HalfAttackInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "half_target_wound_type", TWT_ALL );
	m_HalfAttackInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "half_target_angle", 0.0f );
	m_HalfAttackInfo.m_fTargetRange = rkLoader.LoadFloat_e( "half_target_range", 0.0f );
	m_HalfAttackInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "half_target_up_height", 0.0f );
	m_HalfAttackInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "half_target_down_height", 0.0f );

	rkLoader.LoadString_e( "half_delay_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_HalfAttackInfo.m_AttackLoopAniCenter = szBuf;
	rkLoader.LoadString_e( "half_delay_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_HalfAttackInfo.m_AttackLoopAniUp = szBuf;
	rkLoader.LoadString_e( "half_delay_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_HalfAttackInfo.m_AttackLoopAniDown = szBuf;

	m_HalfAttackInfo.m_fAttackLoopAniRate = rkLoader.LoadFloat_e( "half_delay_attack_actionAni_Rate", FLOAT1 );
	m_HalfAttackInfo.m_dwWaitMaxDelayTime = (DWORD)rkLoader.LoadInt_e( "half_delay_attack_max_wait_time", 0 );
	m_HalfAttackInfo.m_fEndJumpPower = rkLoader.LoadFloat_e( "half_attack_move_end_jump_power", 0.f );
	m_HalfAttackInfo.m_fAttackMoveMaxRange = rkLoader.LoadFloat_e( "half_attack_move_max_range", 0.f );
	m_HalfAttackInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "half_attack_move_speed", 0.f );

	// D~잡고 추가타 정보
	int nMaxAddAttack = rkLoader.LoadInt_e( "full_add_attack_cnt", 1 );
	for( int i=0; i < nMaxAddAttack; ++i )
	{
		AttackAttribute kAttribute;
		AttackAttribute kAirAttribute;
		wsprintf_e( szBuf, "full_add_attack%d", i+1 );
		LoadAttackAttribute( szBuf,kAttribute, rkLoader );
		m_AttackInfo.m_vExtendPlusAttributeList.push_back( kAttribute );

		wsprintf_e( szBuf, "full_add_air_attack%d", i+1 );
		LoadAttackAttribute( szBuf,kAirAttribute, rkLoader );
		m_AttackInfo.m_vExtendPlusAirAttributeList.push_back( kAirAttribute );
	}

	nMaxAddAttack = rkLoader.LoadInt_e( "half_add_attack_cnt", 1 );
	for( int i=0; i < nMaxAddAttack; ++i )
	{
		AttackAttribute kAttribute;
		AttackAttribute kAirAttribute;
		wsprintf_e( szBuf, "half_add_attack%d", i+1 );
		LoadAttackAttribute( szBuf,kAttribute, rkLoader );
		m_HalfAttackInfo.m_vExtendPlusAttributeList.push_back( kAttribute );

		wsprintf_e( szBuf, "half_add_air_attack%d", i+1 );
		LoadAttackAttribute( szBuf,kAirAttribute, rkLoader );
		m_HalfAttackInfo.m_vExtendPlusAirAttributeList.push_back( kAirAttribute );
	}

	m_fAirAttackTypeCheckHeight = rkLoader.LoadFloat_e( "air_attack_type_check_height", 0.f );

	//점프
	m_JumpHalfChargeInfo.Init();
	m_JumpFullChargeInfo.Init();

	//시작
	wsprintf_e( szBuf, "jump_hades_half_attack" );
	LoadAttackAttribute( szBuf, m_JumpHalfChargeInfo.m_FirstAttack, rkLoader );

	// half charging
	rkLoader.LoadString_e( "jump_hades_half_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_JumpHalfChargeInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "jump_hades_half_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_JumpHalfChargeInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "jump_hades_half_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_JumpHalfChargeInfo.m_StartAniDown = szBuf;

	m_JumpHalfChargeInfo.m_fAttackMoveAngle = rkLoader.LoadFloat_e( "jump_hades_half_attack_move_angle",  0 );
	m_JumpHalfChargeInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_hades_half_target_wound_type", TWT_ALL );
	m_JumpHalfChargeInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "jump_hades_half_target_angle", 0.0f );
	m_JumpHalfChargeInfo.m_fTargetRange = rkLoader.LoadFloat_e( "jump_hades_half_target_range", 0.0f );
	m_JumpHalfChargeInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "jump_hades_half_target_up_height", 0.0f );
	m_JumpHalfChargeInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "jump_hades_half_target_down_height", 0.0f );
	m_JumpHalfChargeInfo.m_bLandCheck = rkLoader.LoadBool_e( "jump_hades_half_move_landcheck", false );
	m_JumpHalfChargeInfo.m_fEndJumpPower = rkLoader.LoadFloat_e( "jump_hades_half_attack_move_end_jump_power", 0.f );
	m_JumpHalfChargeInfo.m_fAttackMoveMaxRange = rkLoader.LoadFloat_e( "jump_hades_half_attack_move_max_range", 0.f );
	m_JumpHalfChargeInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "jump_hades_half_attack_move_speed", 0.f );

	//시작
	wsprintf_e( szBuf, "jump_hades_full_attack" );
	LoadAttackAttribute( szBuf, m_JumpFullChargeInfo.m_FirstAttack, rkLoader );

	// half charging
	rkLoader.LoadString_e( "jump_hades_full_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_JumpFullChargeInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "jump_hades_full_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_JumpFullChargeInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "jump_hades_full_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_JumpFullChargeInfo.m_StartAniDown = szBuf;

	m_JumpFullChargeInfo.m_fAttackMoveAngle = rkLoader.LoadFloat_e( "jump_hades_full_attack_move_angle",  0 );
	m_JumpFullChargeInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_hades_full_target_wound_type", TWT_ALL );
	m_JumpFullChargeInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "jump_hades_full_target_angle", 0.0f );
	m_JumpFullChargeInfo.m_fTargetRange = rkLoader.LoadFloat_e( "jump_hades_full_target_range", 0.0f );
	m_JumpFullChargeInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "jump_hades_full_target_up_height", 0.0f );
	m_JumpFullChargeInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "jump_hades_full_target_down_height", 0.0f );
	m_JumpFullChargeInfo.m_bLandCheck = rkLoader.LoadBool_e( "jump_hades_full_move_landcheck", false );
	m_JumpFullChargeInfo.m_fEndJumpPower = rkLoader.LoadFloat_e( "jump_full_attack_move_end_jump_power", 0.f );
	m_JumpFullChargeInfo.m_fAttackMoveMaxRange = rkLoader.LoadFloat_e( "jump_hades_full_attack_move_max_range", 0.f );
	m_JumpFullChargeInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "jump_hades_full_attack_move_speed", 0.f );

	//이동공격
	wsprintf_e( szKey, "jump_hades_full_move_attack" );
	LoadAttackAttribute( szKey, m_JumpFullChargeInfo.m_MoveAttackInfo, rkLoader );

	wsprintf_e( szKey, "jump_hades_half_move_attack" );
	LoadAttackAttribute( szKey, m_JumpHalfChargeInfo.m_MoveAttackInfo, rkLoader );

	//대쉬
	m_DashHalfChargeInfo.Init();
	m_DashFullChargeInfo.Init();

	// full charging
	rkLoader.LoadString_e( "dash_hades_full_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_DashFullChargeInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "dash_hades_full_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_DashFullChargeInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "dash_hades_full_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_DashFullChargeInfo.m_StartAniDown = szBuf;

	// half charging
	rkLoader.LoadString_e( "dash_hades_half_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_DashHalfChargeInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "dash_hades_half_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_DashHalfChargeInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "dash_hades_half_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_DashHalfChargeInfo.m_StartAniDown = szBuf;
	
	//시작
	wsprintf_e( szKey, "dash_hades_full_attack" );
	LoadAttackAttribute( szKey, m_DashFullChargeInfo.m_FirstAttack, rkLoader );

	//시작
	wsprintf_e( szKey, "dash_hades_half_attack" );
	LoadAttackAttribute( szKey, m_DashHalfChargeInfo.m_FirstAttack, rkLoader );
	
	// 타겟 대상 검색용
	m_DashFullChargeInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "dash_hades_full_target_wound_type", TWT_ALL );
	m_DashFullChargeInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "dash_hades_full_target_angle", 0.0f );
	m_DashFullChargeInfo.m_fTargetRange = rkLoader.LoadFloat_e( "dash_hades_full_target_range", 0.0f );
	m_DashFullChargeInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "dash_hades_full_target_up_height", 0.0f );
	m_DashFullChargeInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "dash_hades_full_target_down_height", 0.0f );

	//잡고 있을때 모션 정보와 기다리는 시간
	rkLoader.LoadString_e( "dash_hades_full_delay_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_DashFullChargeInfo.m_AttackLoopAniCenter = szBuf;
	rkLoader.LoadString_e( "dash_hades_full_delay_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackLoopAniUp = szBuf;
	rkLoader.LoadString_e( "dash_hades_full_delay_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_DashFullChargeInfo.m_AttackLoopAniDown = szBuf;
	m_DashFullChargeInfo.m_fAttackLoopAniRate = rkLoader.LoadFloat_e( "dash_hades_full_delay_attack_actionAni_Rate", FLOAT1 );
	m_DashFullChargeInfo.m_dwWaitMaxDelayTime = (DWORD)rkLoader.LoadInt_e( "dash_hades_full_delay_attack_max_wait_time", 0 );

	//공격끝 모션	
	rkLoader.LoadString_e( "dash_hades_full_attack_end_motion", "", szBuf, MAX_PATH );
	m_DashFullChargeInfo.m_AttackEndAni = szBuf;
	m_DashFullChargeInfo.m_fAttackEndAniRate = rkLoader.LoadFloat_e( "dash_hades_full_attack_end_motion_rate", FLOAT1 );
	m_DashFullChargeInfo.m_fEndJumpPower = rkLoader.LoadFloat_e( "dash_hades_full_attack_move_end_jump_power", 0.f );
	m_DashFullChargeInfo.m_fAttackMoveMaxRange = rkLoader.LoadFloat_e( "dash_hades_full_attack_move_max_range", 0.f );
	m_DashFullChargeInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "dash_hades_full_attack_move_speed", 0.f );

	// 타겟 대상 검색용
	m_DashHalfChargeInfo.m_fAttackMoveAngle = rkLoader.LoadFloat_e( "dash_hades_half_attack_move_angle",  0 );
	m_DashHalfChargeInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "dash_hades_half_target_wound_type", TWT_ALL );
	m_DashHalfChargeInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "dash_hades_half_target_angle", 0.0f );
	m_DashHalfChargeInfo.m_fTargetRange = rkLoader.LoadFloat_e( "dash_hades_half_target_range", 0.0f );
	m_DashHalfChargeInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "dash_hades_half_target_up_height", 0.0f );
	m_DashHalfChargeInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "dash_hades_half_target_down_height", 0.0f );

	rkLoader.LoadString_e( "dash_hades_half_delay_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_DashHalfChargeInfo.m_AttackLoopAniCenter = szBuf;
	rkLoader.LoadString_e( "dash_hades_half_delay_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_DashHalfChargeInfo.m_AttackLoopAniUp = szBuf;
	rkLoader.LoadString_e( "dash_hades_half_delay_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_DashHalfChargeInfo.m_AttackLoopAniDown = szBuf;
	m_DashHalfChargeInfo.m_fAttackLoopAniRate = rkLoader.LoadFloat_e( "dash_hades_half_delay_attack_actionAni_Rate", FLOAT1 );
	m_DashHalfChargeInfo.m_dwWaitMaxDelayTime = (DWORD)rkLoader.LoadInt_e( "dash_hades_half_delay_attack_max_wait_time", 0 );
	m_DashHalfChargeInfo.m_fEndJumpPower = rkLoader.LoadFloat_e( "dash_hades_half_attack_move_end_jump_power", 0.f );
	m_DashHalfChargeInfo.m_fAttackMoveMaxRange = rkLoader.LoadFloat_e( "dash_hades_half_attack_move_max_range", 0.f );
	m_DashHalfChargeInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "dash_hades_half_attack_move_speed", 0.f );

	//이동공격
	wsprintf_e( szKey, "dash_hades_full_move_attack" );
	LoadAttackAttribute( szKey, m_DashFullChargeInfo.m_MoveAttackInfo, rkLoader );

	wsprintf_e( szKey, "dash_hades_half_move_attack" );
	LoadAttackAttribute( szKey, m_DashHalfChargeInfo.m_MoveAttackInfo, rkLoader );

	// D~잡고 추가타 정보
	nMaxAddAttack = rkLoader.LoadInt_e( "dash_hades_full_add_attack_cnt", 1 );
	for( int i=0; i < nMaxAddAttack; ++i )
	{
		AttackAttribute kAttribute;
		AttackAttribute kAirAttribute;
		wsprintf_e( szBuf, "dash_hades_full_add_attack%d", i+1 );
		LoadAttackAttribute( szBuf,kAttribute, rkLoader );
		m_DashFullChargeInfo.m_vExtendPlusAttributeList.push_back( kAttribute );

		wsprintf_e( szBuf, "dash_hades_full_add_air_attack%d", i+1 );
		LoadAttackAttribute( szBuf,kAirAttribute, rkLoader );
		m_DashFullChargeInfo.m_vExtendPlusAirAttributeList.push_back( kAirAttribute );
	}

	nMaxAddAttack = rkLoader.LoadInt_e( "dash_hades_half_add_attack_cnt", 1 );
	for( int i=0; i < nMaxAddAttack; ++i )
	{
		AttackAttribute kAttribute;
		AttackAttribute kAirAttribute;
		wsprintf_e( szBuf, "dash_hades_half_add_attack%d", i+1 );
		LoadAttackAttribute( szBuf,kAttribute, rkLoader );
		m_DashHalfChargeInfo.m_vExtendPlusAttributeList.push_back( kAttribute );

		wsprintf_e( szBuf, "dash_hades_half_add_air_attack%d", i+1 );
		LoadAttackAttribute( szBuf,kAirAttribute, rkLoader );
		m_DashHalfChargeInfo.m_vExtendPlusAirAttributeList.push_back( kAirAttribute );
	}

	rkLoader.LoadString_e( "hades_lock_control_buff", "", szBuf, MAX_PATH );
	m_stLockControlBuff = szBuf;

	m_fGoalOffSetLength = rkLoader.LoadFloat_e( "hades_goal_offset_length", 0.f );
	m_fCheckWeaponLength = rkLoader.LoadFloat_e( "hades_weapon_check_max_length", 0.f );
}

ioItem* ioHadesItem::Clone()
{
	return new ioHadesItem( *this );
}

ioWeaponItem::WeaponSubType ioHadesItem::GetSubType() const
{
	return WST_HADES_ITEM;
}

void ioHadesItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	ClearAttackState( pOwner );

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioHadesItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioHadesItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeGatheringState( pOwner );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioHadesItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionEndTime <= FRAMEGETTIME() )
			SetHadesAttackState( pOwner, m_AttackInfo, false, true );
	}
	else
		SetHadesHalfAttackState( pOwner, m_HalfAttackInfo, false, true );
}

void ioHadesItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHadesItem::ChangeGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_dwGatheringStartTime = dwCurTime;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_stGatheringAni );
	float fTimeRate  = m_fGatheringAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_GATHERING;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHadesItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qRot;

	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
		break;
	case CS_GATHERING:
		rkPacket >> vPos;
		rkPacket >> qRot;
		pOwner->SetWorldPosition( vPos );
		pOwner->SetTargetRotToRotate( qRot, true );
		ChangeGatheringState( pOwner );
		break;
		
	}
}

void ioHadesItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_STATE:
		CheckHadesState( pOwner, rkPacket );
		break;
	case SST_ENDJUMP:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );

			SetMoveAttackEndJump( pOwner );
		}
		break;
	}
}

void ioHadesItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioUroborusWeapon7 *pUroborus7 = ToUroborusWeapon7( pWeapon );
	if( pUroborus7 )
	{
		m_dwWeaponIndex = pWeapon->GetWeaponIndex();

		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &vDir, &vDir );
				}
			}
			else
				vDir = m_vFlyMoveDir;

			pUroborus7->SetMoveDir( vDir );
		}
	}

	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	if( pMissile )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					D3DXVECTOR3 vPos = pTargetChar->GetWorldPosition();
					vPos.y = pTargetChar->GetMidHeightByRate();
					pMissile->SetPosition( vPos );
				}
			}	
		}
	}

	ioContactWeapon *pContact = ToContactWeapon( pWeapon );
	if( pContact && m_HadesAttackState == MFS_ATTACK_ADD )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar && !m_Target.IsEmpty() )
		{
			ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
			if( pTargetChar )
			{
				D3DXVECTOR3 vPos = pTargetChar->GetWorldPosition();
				vPos.y = pTargetChar->GetMidHeightByRate();
				pContact->SetPosition( vPos );
			}
		}
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}

	ioOwnerAttackWeapon* pOwnerWepon = ToOwnerAttackWeapon(pWeapon);
	if( pOwnerWepon && !m_Target.IsEmpty() )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			ioPlayStage* pStage = pChar->GetCreator();
			if( pStage )
				pOwnerWepon->SetTargetName( m_Target, pStage );
		}
		
	}
}

bool ioHadesItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
			return true;
		}
	}

	return false;
}

void ioHadesItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioHadesItem::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	return true;
}

void ioHadesItem::GetAutoTargetValue( ioBaseChar *pOwner,
										  float &fRange, float &fMinAngle, float &fMaxAngle,
										  AutoTargetType eType )
{
	if( IsChargeAutoTarget(pOwner) )
	{
		float fCurTargetRangeRate, fCurTargetAngleRate;
		fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

		float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
		float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

		fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
		fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

		fRange = m_fAutoTargetRange * fCurTargetRangeRate;
		fMinAngle = m_fAutoTargetMinAngle / fCurTargetAngleRate;
		fMaxAngle = m_fAutoTargetMaxAngle / fCurTargetAngleRate;
	}
	else
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
}

void ioHadesItem::SetHadesHalfAttackState( ioBaseChar *pOwner, const HadesAttackInfo &rkFlyInfo, bool bAuto, bool bSendNet, bool bJump, bool bJumpHalf, bool bDash )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_bAutoMove = bAuto;

	if( pOwner->IsNeedProcess() )
		m_vFlyMoveDir = ioMath::VEC3_ZERO;

	m_nAttackCnt = 0;
	pOwner->SetState( CS_HADES_ATTACK );

	if( bJump )
	{
		if( bJumpHalf )
			m_CurAttackInfo = m_JumpHalfChargeInfo;
		else
			m_CurAttackInfo = m_JumpFullChargeInfo;
	}
	else if( bDash )
		m_CurAttackInfo = m_DashHalfChargeInfo;
	else
		m_CurAttackInfo = rkFlyInfo;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );

		D3DXVECTOR3 vTargetPos;
		vTargetPos = pOwner->GetMidPositionByRate();

		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_CurAttackInfo.m_fAttackMoveAngle) );
		vDir = qtRot * vDir;
		D3DXVec3Normalize( &vDir, &vDir );
		m_vFlyMoveDir = vDir;
	}

	ChangeAttackStart( pOwner );
	m_fFlyCurRange = 0.0f;
	m_dwHitedTime = 0;

	m_nCurAddAttackCnt = 0;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_HadesAttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << true;
		kPacket << m_vFlyMoveDir;
		kPacket << m_fCurGauge;
		kPacket << m_Target;
		kPacket << bJump;
		kPacket << bJumpHalf;
		kPacket << bDash;
		kPacket << true;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	CheckDecreaseGauge();

	m_bAirAddAttckType = false;
}

void ioHadesItem::SetHadesAttackState( ioBaseChar *pOwner, const HadesAttackInfo &rkFlyInfo, bool bAuto, bool bSendNet, bool bDash )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_bAutoMove = bAuto;

	if( pOwner->IsNeedProcess() )
		m_vFlyMoveDir = ioMath::VEC3_ZERO;

	m_nAttackCnt = 0;
	pOwner->SetState( CS_HADES_ATTACK );

	if( bDash )
		m_CurAttackInfo = m_DashFullChargeInfo;
	else
		m_CurAttackInfo = rkFlyInfo;

	if( pOwner->IsNeedProcess() )
	{
		FindFlyAttTarget( pOwner );
		if( CompareTarget( pOwner ) )
		{
			m_Target.Clear();
			m_bAimedBall = true;
		}
	}
	
	if( pOwner->IsNeedProcess() )
	{
		bool bTargetOk = false;
		D3DXVECTOR3 vTargetPos;
		D3DXVECTOR3 vDir;

		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				bTargetOk = true;
				vDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
			if( pTarget )			
			{
				bTargetOk = true;
				vDir = pTarget->GetMidPositionByRate(m_CurAttackInfo.m_fTargetHeightRate) - pOwner->GetMidPositionByRate(m_CurAttackInfo.m_fAttackerHeightRate);
			}
		}

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_CurAttackInfo.m_fAttackMoveAngle) );
			vDir = qtRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			m_vFlyMoveDir = vDir;
		}
		else
		{
			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			m_vFlyMoveDir = vDir;
		}
	}

	ChangeAttackStart( pOwner );
	m_fFlyCurRange = 0.0f;
	m_dwHitedTime = 0;

	m_nCurAddAttackCnt = 0;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_HadesAttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << false;
		kPacket << m_vFlyMoveDir;
		kPacket << m_fCurGauge;
		kPacket << m_Target;
		kPacket << false;
		kPacket << false;
		kPacket << bDash;
		kPacket << false;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	CheckDecreaseGauge();

	m_bAirAddAttckType = false;
}

void ioHadesItem::ChangeAttackStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_HadesAttackState = MFS_START;
	m_nCurAddAttackCnt = 0;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	//트랙킹
	ChangeTrackingUD_Fire( pOwner );

	//상중하 다 시간은 같다.
	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_StartAniCenter );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_CurAttackInfo.m_FirstAttack.m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurAttackInfo.m_FirstAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate,
		dwPreDelay );

	m_dwKeyReserveTime = FRAMEGETTIME() + (pGrp->GetAnimationEventTime( iAniID, "key_reserve" ) * m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate);
	m_dwAttackStartTime = FRAMEGETTIME(); 
	m_dwMotionEndTime = m_dwAttackStartTime;
	m_dwFireMotionTime = m_dwAttackStartTime;
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate );
}

void ioHadesItem::ChangeAttackHited( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_bAutoMove )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_Target );
	if( !pTarget )
		return;

	m_bAirAddAttckType = true;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	float fMapHeight = pStage->GetMapHeight( pTarget->GetWorldPosition().x, pTarget->GetWorldPosition().z );
	float fOffSet = pTarget->GetWorldPosition().y - fMapHeight;

	if( fOffSet > m_fAirAttackTypeCheckHeight )
		m_bAirAddAttckType = true;
	else
		m_bAirAddAttckType = false;

	SetChangeAttackLoopState( pOwner );
}

void ioHadesItem::SetAirFloatStateChange( ioBaseChar *pOwner )
{
	ioBaseChar* pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
	{
		ioHitCountHoldBuff* pBuff1 = ToHitCountHoldBuff( pTarget->GetBuff( BT_HIT_COUNT_HOLD, pOwner->GetCharName() ) );
		if( pBuff1 )
			pBuff1->SetNotUseFloatState( true );

		ioHitCountHoldBuff2* pBuff2 = ToHitCountHoldBuff2( pTarget->GetBuff( BT_HIT_COUNT_HOLD2, pOwner->GetCharName() ) );
		if( pBuff2 )
			pBuff2->SetNotUseFloatState( true );
	}
}

void ioHadesItem::ChangeAttackAdd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( (int)m_CurAttackInfo.m_vExtendPlusAttributeList.size() <= m_nCurAddAttackCnt )
		return;

	SetAirFloatStateChange( pOwner );

	ioBaseChar* pTargetDest = pOwner->GetBaseChar( m_Target );
	BuffStateChange( pOwner, pTargetDest );

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = 0;
	float fTimeRate = FLOAT1;
	DWORD dwPreDelay = 0;

	if( m_bAirAddAttckType )
	{
		iAniID	    = pGrp->GetAnimationIdx( m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_AttackAnimation );
		fTimeRate	= m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_fAttackAniRate;
		dwPreDelay	= m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_dwPreDelay;
	}
	else
	{
		iAniID	    = pGrp->GetAnimationIdx( m_CurAttackInfo.m_vExtendPlusAttributeList[m_nCurAddAttackCnt].m_AttackAnimation );
		fTimeRate	= m_CurAttackInfo.m_vExtendPlusAttributeList[m_nCurAddAttackCnt].m_fAttackAniRate;
		dwPreDelay	=m_CurAttackInfo.m_vExtendPlusAttributeList[m_nCurAddAttackCnt].m_dwPreDelay;
	}

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_HadesAttackState = MFS_ATTACK_ADD;

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	if( m_bAirAddAttckType )
	{
		pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}
	else
	{
		pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurAttackInfo.m_vExtendPlusAttributeList[m_nCurAddAttackCnt].m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );
	}

	m_dwMotionEndTime = dwCurTime;
	m_dwFireMotionTime = dwCurTime;
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );
	m_dwAddAttackTime = dwCurTime;

	if( pOwner->IsNeedProcess() )
	{
		//패킷을 보내자
		m_CurDirKey = pOwner->CheckCurDirKey();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_HadesAttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		kPacket << m_nCurAddAttackCnt;
	
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_nCurAddAttackCnt++;
}

void ioHadesItem::ChangeAttackMove( ioBaseChar *pOwner )
{
	m_bMoveIsDone = false;
	m_bCurMaxMoveSeted = false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	SetAirFloatStateChange( pOwner );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	m_fCurMoveRange = 0.f;
	m_vMoveAttackDir = ioMath::VEC3_ZERO;

	m_vMoveAttackStartPos = pOwner->GetWorldPosition();

	ioBaseChar* pTargetDest = pOwner->GetBaseChar( m_Target );
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );
	ioUroborusWeapon7 *pUroborus7 =	ToUroborusWeapon7( pWeapon );
	if( !pUroborus7 )
	{
		SetChangeEndState( pOwner, true );
		return;
	}
	
	D3DXVECTOR3 vDestPos = pUroborus7->GetPosition();
	D3DXVECTOR3 vSrcPos  = pOwner->GetMidPositionByRate();

	m_vMoveAttackDir = vDestPos - vSrcPos;
	m_fCurMaxMoveRange = D3DXVec3Length( &m_vMoveAttackDir );
	D3DXVec3Normalize( &m_vMoveAttackDir, &m_vMoveAttackDir );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_MoveAttackInfo.m_AttackAnimation );
	float fTimeRate  = m_CurAttackInfo.m_MoveAttackInfo.m_fAttackAniRate;
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, m_CurAttackInfo.m_MoveAttackInfo.m_dwPreDelay, true );

	if( m_HadesAttackState == MFS_ATTACK_LOOP )
	{
		m_fCurMaxMoveRange = min( m_CurAttackInfo.m_fAttackMoveMaxRange, m_fCurMaxMoveRange );
		m_bCurMaxMoveSeted = true;
		m_fCurMaxMoveRange = m_fCurMaxMoveRange - m_fGoalOffSetLength;
	}
	else
	{
		m_fCurMaxMoveRange = m_CurAttackInfo.m_fAttackMoveMaxRange;
	}

	m_HadesAttackState = MFS_ATTACK_MOVE;

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_CurAttackInfo.m_MoveAttackInfo.m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurAttackInfo.m_MoveAttackInfo.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		m_CurAttackInfo.m_MoveAttackInfo.m_fAttackAniRate,
		dwPreDelay );

	//SendPacket
	if( pOwner->IsNeedProcess() )
	{
		//패킷을 보내자
		m_CurDirKey = pOwner->CheckCurDirKey();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_HadesAttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHadesItem::SetMoveAttackEndJump( ioBaseChar *pOwner )
{
	RemoveWeapon( pOwner );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.f );

	pOwner->SetSKillEndJumpState( m_CurAttackInfo.m_fEndJumpPower, false, true, false, true );

	if( pOwner && pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ENDJUMP;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHadesItem::CheckDummyCreate( ioBaseChar *pOwner )
{
	if( pOwner->IsDefenseKey() && pOwner->CheckSummonDummyChar( ioAttackableItem::SDT_EXTEND_ATT ) )
		pOwner->ClearReserveKeyInput();
}

void ioHadesItem::ProcessHadesState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_HadesAttackState )
	{
	case MFS_START:		
		if( pOwner->IsNeedProcess() && (dwCurTime > m_dwMotionEndTime) )
			SetChangeEndState( pOwner, true );

		if( m_bAutoMove )
		{
			if( pOwner->IsNeedProcess() && m_dwKeyReserveTime < dwCurTime )
			{
				ChangeAttackMove(pOwner);
				return;
			}
		}
		else if( pOwner->IsNeedProcess() && m_dwKeyReserveTime < dwCurTime && pOwner->IsJumpKey() )
		{
			ChangeAttackMove(pOwner);
			return;
		}
		break;
	case MFS_ATTACK_HITED:
		if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
		{
			ChangeAttackAdd(pOwner);
			return;
		}

		if( pOwner->IsNeedProcess() && pOwner->IsJumpKey() )
		{
			ChangeAttackMove(pOwner);
			return;
		}

		if( pOwner->IsNeedProcess() && m_dwHitedTime && (m_dwHitedTime+m_CurAttackInfo.m_dwWaitMaxDelayTime) < dwCurTime )
		{
			ChangeAttackAdd(pOwner);
			return;
		}	
		break;
	case MFS_ATTACK_ADD:
		if( dwCurTime > m_dwMotionEndTime )
			SetChangeAttackLoopState( pOwner );

		break;
	case MFS_ATTACK_LOOP:
		if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
		{
			ChangeAttackAdd( pOwner );
			return;
		}

		if( pOwner->IsNeedProcess() && pOwner->IsJumpKey() )
		{
			ChangeAttackMove(pOwner);
			return;
		}

		if( pOwner->IsNeedProcess() && dwCurTime > m_dwAttackWaitLoopEndTime )
		{
			SetChangeAttackEndState( pOwner );
			return;
		}
		TrackingTarget( pOwner, m_CurAttackInfo.m_AttackLoopAniUp, m_CurAttackInfo.m_AttackLoopAniCenter, m_CurAttackInfo.m_AttackLoopAniDown );
		break;
	case MFS_ATTACK_MOVE:
		if(!m_bMoveIsDone && !Process_A_AttackMove( pOwner ) )
		{
			m_bMoveIsDone = true;
			if( pOwner->IsNeedProcess() )
			{
				D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
				float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
				if( (fHeight+m_fAirAttackTypeCheckHeight) < vPos.y )
				{
					SetMoveAttackEndJump( pOwner );
					return;
				}
				else
				{
					SetChangeEndState( pOwner, true );
					return;
				}
			}
			
		}
		break;
	case MFS_ATTACK_END:
		if( dwCurTime > m_dwMotionEndTime )
			SetChangeEndState( pOwner, false );
		break;
	
	case MFS_END:
		break;
	}
	
	if( m_HadesAttackState == MFS_ATTACK_HITED || m_HadesAttackState == MFS_ATTACK_ADD || m_HadesAttackState == MFS_ATTACK_LOOP )
		CheckAttackHited( pOwner );

	if( m_HadesAttackState == MFS_ATTACK_HITED )
		ProcessWeaponLine( pOwner );

	if( m_HadesAttackState == MFS_ATTACK_HITED || m_HadesAttackState == MFS_ATTACK_LOOP || m_HadesAttackState == MFS_ATTACK_ADD )
		ProcessLookRotate( pOwner );

	//더미 소환 체크
	if( pOwner->IsNeedProcess() && IsEnableGauge() )
		CheckDummyCreate( pOwner );

	if(	pOwner->IsNeedProcess() &&	(m_HadesAttackState == MFS_ATTACK_HITED || 
		m_HadesAttackState == MFS_ATTACK_ADD ||
		m_HadesAttackState == MFS_ATTACK_LOOP ||
		m_HadesAttackState == MFS_ATTACK_MOVE) )
		WeaponCheck( pOwner );

	if( pOwner->IsNeedProcess() && !CheckWeaponLength( pOwner ) )
	{
		SetChangeEndState( pOwner, true );
		return;
	}
}

bool ioHadesItem::CheckWeaponLength( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );
	ioUroborusWeapon7 *pUroborus7 =	ToUroborusWeapon7( pWeapon );

	//d웨폰이 없으면 패스
	if( !pUroborus7 )
		return true;

	D3DXVECTOR3 vSrcPos  = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDestPos = pUroborus7->GetPosition();
	D3DXVECTOR3 vLength;
	vLength = vDestPos - vSrcPos;
	float fLength = D3DXVec3Length( &vLength );
	if( fLength > m_fCheckWeaponLength )
		return false;
	
	return true;
}

bool ioHadesItem::Process_A_AttackMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXVECTOR3 vMoveDir = m_vMoveAttackDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( !m_bCurMaxMoveSeted )
	{
		ioBaseChar* pTargetDest = pOwner->GetBaseChar( m_Target );
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );
		ioUroborusWeapon7 *pUroborus7 =	ToUroborusWeapon7( pWeapon );
		if( pUroborus7 &&  (pUroborus7->GetState() == ioUroborusWeapon7::US_LOOP || pUroborus7->GetState() == ioUroborusWeapon7::US_END ) )
		{
			m_bCurMaxMoveSeted = true;
			D3DXVECTOR3 vDestPos = pUroborus7->GetPosition();
			D3DXVECTOR3 vLength;
			vLength = vDestPos - m_vMoveAttackStartPos;
			m_fCurMaxMoveRange = D3DXVec3Length( &vLength );

			m_fCurMaxMoveRange = m_fCurMaxMoveRange - m_fGoalOffSetLength;
		}
	}
	
	float fCurSpeed = m_CurAttackInfo.m_fAttackMoveSpeed;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fCurMoveRange+fDistance < m_fCurMaxMoveRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fCurMoveRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_fCurMaxMoveRange - m_fCurMoveRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		float fPreHeight = pOwner->GetBottomHeight();
		pOwner->Translate( vMoveAmt );
		float fMoveHeight = pOwner->GetBottomHeight();
		if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
		{
			D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

			float fTopHeight = pStage->GetMapHeight( vCurPos.x,
				vCurPos.z,
				pOwner,
				false,
				pOwner->GetMidHeightByRate( FLOAT1 ) );

			if( fTopHeight > FLOAT1 )
			{
				vCurPos.y = max( fTopHeight, fPreHeight );
				pOwner->SetWorldPosition( vCurPos );
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();
	bool bJump = false;

	if( vPos.y < fPreHeight )
	{
		vPos.y = fPreHeight;
		bJump = true;
	}

	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	if(pOwner->IsNeedProcess() && bJump && m_CurAttackInfo.m_bLandCheck )
	{
		SetMoveAttackEndJump( pOwner );
		return true;
	}

	return bRangeLimit;
}

void ioHadesItem::ProcessLookRotate( ioBaseChar *pOwner )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
	{
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vDiff, &vDiff );
		pOwner->SetTargetRotToDir( vDiff, true );
	}
}

void ioHadesItem::ProcessWeaponLine( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_Target.IsEmpty() || m_dwWeaponIndex == 0 )
		return;
	
	ioBaseChar* pTargetDest = pChar->GetBaseChar( m_Target );
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponIndex );
	ioUroborusWeapon7 *pUroborus7 =	ToUroborusWeapon7( pWeapon );
	if( !pUroborus7 )
		return;

	if( pTargetDest && IsLockState(pChar, pTargetDest) )
	{
		ioEntityGroup *pGrp = pTargetDest->GetGroup();
		if( pGrp )
		{
			ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
			if( pBiped )
			{
				D3DXVECTOR3 vBipPos = pBiped->GetDerivedPosition();
				pUroborus7->SetPosition( vBipPos );
			}			
		}
	}
}

void ioHadesItem::CheckHadesState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;

	rkPacket >> iState;
	rkPacket >> vPos;
	rkPacket >> qtRot;

	pOwner->SetWorldPosition( vPos );
	pOwner->SetTargetRotToRotate( qtRot, true );

	switch( iState )
	{
	case MFS_START:
		{
			bool bHalfAttack;
			float fCurGauge;
			bool bJump;
			bool bJumpHalf;
			bool bDash;
			bool bDashHalf;
			rkPacket >> bHalfAttack;
			rkPacket >> m_vFlyMoveDir;
			rkPacket >> fCurGauge;
			rkPacket >> m_Target;
			rkPacket >> bJump;
			rkPacket >> bJumpHalf;
			rkPacket >> bDash;
			rkPacket >> bDashHalf;

			SetCurExtraGauge( fCurGauge );

			if( bJump )
			{
				if( bJumpHalf )
					SetHadesHalfAttackState( pOwner, m_JumpHalfChargeInfo, true, false, true, true );
				else
					SetHadesHalfAttackState( pOwner, m_JumpFullChargeInfo, true, false, true, false );
			}
			else if( bDash )
			{
				if( bDashHalf )
					SetHadesHalfAttackState( pOwner, m_DashHalfChargeInfo, false, false, false, false, true);
				else
					SetHadesAttackState( pOwner, m_DashFullChargeInfo, false, false, true );
			}
			else
			{
				if( bHalfAttack )
					SetHadesHalfAttackState( pOwner, m_HalfAttackInfo, false, false );
				else
					SetHadesAttackState( pOwner, m_AttackInfo, false, false );
			}
		}
		break;
	case MFS_ATTACK_ADD:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;
			rkPacket >> m_nCurAddAttackCnt;

			SetCurExtraGauge( fCurGauge );

			ChangeAttackAdd( pOwner );
		}
		break;
	case MFS_ATTACK_MOVE:
		ChangeAttackMove( pOwner );
		break;
	case MFS_ATTACK_END:
		SetChangeAttackEndState( pOwner );
		break;
	case MFS_END:
		SetChangeEndState( pOwner, false );
		break;
	}
}

int ioHadesItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioHadesItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioHadesItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioHadesItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioHadesItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

void ioHadesItem::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioHadesItem::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}

float ioHadesItem::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioHadesItem::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioHadesItem::CheckEnableGauge( ioBaseChar *pChar )
{
}

void ioHadesItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioHadesItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioHadesItem::IsHadesState()
{
	switch( m_HadesAttackState )
	{
	case MFS_START:
	case MFS_ATTACK_HITED:
	case MFS_ATTACK_ADD:
		return true;
	}

	return false;
}

void ioHadesItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_HADES_ATTACK:
		return;
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

void ioHadesItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_HadesAttackState = MFS_NONE;

	// 순서중요
	m_CurAttackInfo.Init();

	m_dwAttackStartTime = 0;

	m_vFlyMoveDir = ioMath::VEC3_ZERO;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
	{
		ioHitCountHoldBuff*		pBuff1 = ToHitCountHoldBuff( pTarget->GetBuff( BT_HIT_COUNT_HOLD, pTarget->GetCharName() ) );
		ioHitCountHoldBuff2*	pBuff2 = ToHitCountHoldBuff2( pTarget->GetBuff( BT_HIT_COUNT_HOLD2, pTarget->GetCharName() ) );
		ioBuff* pControlBuff = pTarget->GetBuff( m_stLockControlBuff, pOwner->GetCharName() );

		if( pBuff1 )
			pBuff1->SetReserveEndBuff();

		if( pBuff2 )
			pBuff2->SetReserveEndBuff();

		if( pControlBuff )
			pControlBuff->SetReserveEndBuff();
	}
	m_Target.Clear();

	m_dwMotionEndTime = 0;
	m_bAutoMove = false;

	m_vMoveAttackStartPos = ioMath::VEC3_ZERO;

	RemoveWeapon(pOwner);
}

bool ioHadesItem::IsFlyCharCollisionSkipState()
{
	switch( m_HadesAttackState )
	{
	case MFS_START:
		return true;
	}

	return false;
}

void ioHadesItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioHadesItem::OnReleased( ioBaseChar *pOwner )
{
	ClearAttackState( pOwner );

	m_fCurGauge = 0.0f;
	if( pOwner )
		pOwner->InitCurSkillEndNextLandRate();

	ioAttackableItem::OnReleased( pOwner );
}

bool ioHadesItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurAttackInfo.m_TargetWoundType ) )
		return false;

	return true;
}

void ioHadesItem::FindFlyAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_Target.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_CurAttackInfo.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurAttackInfo.m_fTargetRange * m_CurAttackInfo.m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_CurAttackInfo.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurAttackInfo.m_fTargetDownHeight )
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
			m_Target = szTarget;
		}
	}
}

bool ioHadesItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurAttackInfo.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurAttackInfo.m_fTargetRange * m_CurAttackInfo.m_fTargetRange;

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
	if( fHeightGap > 0.0f && fHeightGap > m_CurAttackInfo.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurAttackInfo.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

bool ioHadesItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioHadesItem::IncreaseChangeDamage( float fDamage, int iType, bool bBuff )
{
	if( iType == SSGT_FOX || iType == SSGT_NONE )
	{
		m_fCurGauge += fDamage;
		m_fCurGauge = max( 0.0f, m_fCurGauge );
		m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
	}
}

bool ioHadesItem::SetTarget( const ioHashString &szTargetName, ioBaseChar *pOwner )
{
	//명중 
	m_Target = szTargetName;
	m_dwHitedTime = FRAMEGETTIME();
	ChangeAttackHited( pOwner );
	return true;
}

void ioHadesItem::RemoveWeapon( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );

	if( pWeapon )
	{
		ioUroborusWeapon7 *pUroborus7 =	ToUroborusWeapon7( pWeapon );
		if( pUroborus7 )
			pUroborus7->SetEndState( false, false, false, true );
	}
}

void ioHadesItem::CheckAttackHited( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioBaseChar* pTarget = pOwner->GetBaseChar(m_Target);
	if( !pTarget )
	{
		SetChangeEndState( pOwner, false );
		return;
	}
	if( !IsLockState(pOwner, pTarget) )
	{
		RemoveWeapon(pOwner);
		SetChangeEndState( pOwner, false );
		return;
	}
}

void ioHadesItem::SetChangeEndState( ioBaseChar *pOwner, bool bSend )
{
	ClearAttackState( pOwner );
	pOwner->SetState( CS_DELAY, false );

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << MFS_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHadesItem::SetChangeAttackLoopState( ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		SetChangeEndState( pOwner, false );
		return;
	}

	m_HadesAttackState = MFS_ATTACK_LOOP;
	m_dwAttackWaitLoopEndTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_AttackLoopAniCenter );
	float fTimeRate  = m_CurAttackInfo.m_fAttackLoopAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_CurAttackInfo.m_AttackLoopAniUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_CurAttackInfo.m_AttackLoopAniDown, 0.0f, 0.0f );

	if( !pGrp->GetLoopInPlay( m_CurAttackInfo.m_AttackLoopAniCenter ) )
		pGrp->SetLoopAni( m_CurAttackInfo.m_AttackLoopAniCenter, 0.0f, FLOAT1 );

	m_dwAttackWaitLoopEndTime += m_CurAttackInfo.m_dwWaitMaxDelayTime;
}

void ioHadesItem::SetChangeAttackEndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	RemoveWeapon( pOwner );

	m_HadesAttackState = MFS_ATTACK_END;
	ioEntityGroup *pGrp = pOwner->GetGroup();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_AttackEndAni );
	float fTimeRate  = m_CurAttackInfo.m_fAttackEndAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_HadesAttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

float ioHadesItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void ioHadesItem::ChangeTrackingUD_Fire( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	D3DXVECTOR3 vAimedDir;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = m_vFlyMoveDir;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	float fTimeRate  = FLOAT1;
	DWORD dwPreDelay = 0;
	if( m_HadesAttackState == MFS_START )
		fTimeRate = m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate;
	else if( m_HadesAttackState == MFS_ATTACK_HITED )
		fTimeRate = m_CurAttackInfo.m_fAttackLoopAniRate;

	DWORD dwCurTime = FRAMEGETTIME();
	
	float fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )
		AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
	else
		AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
}

void ioHadesItem::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
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
	if( m_HadesAttackState == MFS_START )
	{
		szFireUp = m_CurAttackInfo.m_StartAniUp;
		szFireCenter = m_CurAttackInfo.m_StartAniCenter;
	}
	else if( m_HadesAttackState == MFS_ATTACK_HITED )
	{
		szFireUp = m_CurAttackInfo.m_AttackLoopAniUp;
		szFireCenter = m_CurAttackInfo.m_AttackLoopAniCenter;
	}

	if( fWeight > 0.0f )
	{
		if( m_HadesAttackState == MFS_START )
			pGrp->SetActionAni( szFireUp,FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate,	0.0f, dwPreDelay,	bHold, bAniFx1 );
		else if( m_HadesAttackState == MFS_ATTACK_HITED )
			pGrp->SetLoopAni( szFireUp, FLOAT100, fWeight, FLOAT1/fTimeRate );
	}

	if( m_HadesAttackState == MFS_START )
		pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
	else if( m_HadesAttackState == MFS_ATTACK_HITED )
		pGrp->SetLoopAni( szFireCenter, FLOAT100, FLOAT1 - fWeight, FLOAT1/fTimeRate );
}

void ioHadesItem::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
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
	if( m_HadesAttackState == MFS_START )
	{
		szFireDown = m_CurAttackInfo.m_StartAniDown;
		szFireCenter = m_CurAttackInfo.m_StartAniCenter;
	}
	else if( m_HadesAttackState == MFS_ATTACK_HITED )
	{
		szFireDown = m_CurAttackInfo.m_AttackLoopAniDown;
		szFireCenter = m_CurAttackInfo.m_AttackLoopAniCenter;
	}

	if( fWeight > 0.0f )
	{
		if( m_HadesAttackState == MFS_START )
			pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx1 );
		else if( m_HadesAttackState == MFS_ATTACK_HITED )
			pGrp->SetLoopAni( szFireCenter, FLOAT100, FLOAT1 - fWeight, FLOAT1/fTimeRate );
	}

	if( m_HadesAttackState == MFS_START )
		pGrp->SetActionAni( szFireDown, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
	else if( m_HadesAttackState == MFS_ATTACK_HITED )
		pGrp->SetLoopAni( szFireDown, FLOAT100, fWeight, FLOAT1/fTimeRate );
}

ioUserKeyInput::DirKeyInput ioHadesItem::GetCurKey()
{
	return m_CurDirKey;
}

bool ioHadesItem::IsLockState( ioBaseChar *pCreator, ioBaseChar *pTarget )
{
	if( !pCreator || !pTarget )
		return false;

	bool bHas = pTarget->HasBuff( pCreator->GetCharName(), m_stLockControlBuff );
	return bHas;
}

void ioHadesItem::BuffStateChange( ioBaseChar *pCreator, ioBaseChar *pTarget )
{
	if( !pTarget || !pCreator )
		return;

	const ioHashString& stCreatorName = pCreator->GetCharName();

	ioHitCountHoldBuff* pControlBuff1 = ToHitCountHoldBuff( pTarget->GetBuff( BT_HIT_COUNT_HOLD, stCreatorName ) );
	if( pControlBuff1 )
	{
		pControlBuff1->SetFloatState( false );
		pControlBuff1->SetReserveEndBuff();
	}

	ioHitCountHoldBuff2* pControlBuff2 = ToHitCountHoldBuff2( pTarget->GetBuff( BT_HIT_COUNT_HOLD2, stCreatorName ) );
	if( pControlBuff2 )
	{
		pControlBuff2->SetFloatState( false );
		pControlBuff2->SetReserveEndBuff();
	}	
}

void ioHadesItem::TrackingTarget( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;
		
	pUp = pGrp->GetLoopInPlay( stUp );
	pCenter = pGrp->GetLoopInPlay( stCenter );
	pDown = pGrp->GetLoopInPlay( stDown );

	D3DXVECTOR3 vAimedDir;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );

	if( pWeapon )
	{
		D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();
		vAimedDir = vWeaponPos - pOwner->GetMidPositionByRate();
	}
	else
		return;

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
}

void ioHadesItem::WeaponCheck( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );
	ioUroborusWeapon7 *pUroborus7 =	ToUroborusWeapon7( pWeapon );
	if( !pUroborus7 )
	{
		SetChangeEndState( pOwner, true );
		return;
	}
}
//--------------------------------------------------------------------------------------------------------------
ioHadesSpecialState::ioHadesSpecialState()
{
}

ioHadesSpecialState::~ioHadesSpecialState()
{
}

void ioHadesSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioHadesSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioHadesItem *pHades = ToHadesItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHades )
		pHades->ClearAttackState( pOwner );
}

void ioHadesSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioHadesItem *pHades = ToHadesItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHades )
		pHades->ProcessHadesState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioHadesSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioHadesSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}