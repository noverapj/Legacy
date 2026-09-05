#include "stdafx.h"
#include "ioNuClearSoldierItem.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioNuClearSoldierItem::ioNuClearSoldierItem()
{
	m_dwAimTime				= FLOAT1000;
	m_fAimAngle				= FLOAT40;
	m_fAimRange				= FLOAT1000;
	m_fMinRange				= FLOAT100;

	m_fMarkerRange			= 0.0f;
	m_fFireTimeModifyRate	= FLOAT1;

	m_fAirJumpPower			= 0;
	m_fAirForcePower		= 0;

	m_dwFireStartTime		= 0;
	m_dwFireEndTime			= 0;
	m_dwFireDelayTime		= 0;

	m_dwChargeStartTime		= 0;
	m_dwAimStartTime		= 0;
	m_iCurCombo				= 0;
	m_FireState				= FS_NONE;

	m_bSetSkillAimed		= false;
	m_bSetAimedDir			= false;
	m_bAimJumpAttack		= false;

	m_fCurAngle				= 0.0f;
	m_iCurAirJump			= 0;
	m_fAirJumpPowerRate		= 0.0f;
	m_dwJumpMarkerEndTime	= 0;

	m_dwCheckAimCancelTime	= 0;
	m_dwTrackingEndTime		= 0;
	m_dwFireStateEndTime	= 0;

	m_bZeroAimState			= false;
	m_dwZeroAimStartTime	= 0;

	m_dwCurCoolTime			= 0;
	m_dwCheckCoolTime		= 0;

	m_bSetChargingMoveAni	= false;

	m_iMaxBullet			= 0;
	m_iCurBullet			= 0;

	m_fCurChargeRate		= 0.0f;

	m_dwSpecialStartTime = 0;
	m_dwSpecialEndTime = 0;
	m_dwSpecialFireTime = 0;

	m_bDashEndJumpState	= false;

	m_dwChargingStartTime = 0;
	m_bJumpChargeAttack	= false;


	// 대시 
	m_dwMotionEndTime	= 0;
	m_dwCancelDashAttackTime	= 0;
	m_iChargeComboDashState	= 0;
}

ioNuClearSoldierItem::ioNuClearSoldierItem( const ioNuClearSoldierItem &rhs )
: ioWeaponItem( rhs ),
 m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
 m_ExtendZeroAimed( rhs.m_ExtendZeroAimed ),
 m_MissFire( rhs.m_MissFire ),
 m_fMissFireRate( rhs.m_fMissFireRate ),
 m_AimUp( rhs.m_AimUp ),
 m_AimCenter( rhs.m_AimCenter ),
 m_AimDown( rhs.m_AimDown ),
 m_ZeroAimUp( rhs.m_ZeroAimUp ),
 m_ZeroAimCenter( rhs.m_ZeroAimCenter ),
 m_ZeroAimDown( rhs.m_ZeroAimDown ),
 m_JumpAimUp( rhs.m_JumpAimUp ),
 m_JumpAimCenter( rhs.m_JumpAimCenter ),
 m_JumpAimDown( rhs.m_JumpAimDown ),
 m_FireUp( rhs.m_FireUp ),
 m_FireCenter( rhs.m_FireCenter ),
 m_FireDown( rhs.m_FireDown ),
 m_ZeroFireUp( rhs.m_ZeroFireUp ),
 m_ZeroFireCenter( rhs.m_ZeroFireCenter ),
 m_ZeroFireDown( rhs.m_ZeroFireDown ),
 m_JumpFireUp( rhs.m_JumpFireUp ),
 m_JumpFireCenter( rhs.m_JumpFireCenter ),
 m_JumpFireDown( rhs.m_JumpFireDown ),
 m_dwZeroAimedCheckTime( rhs.m_dwZeroAimedCheckTime ),
 m_ZeroAimedEffect( rhs.m_ZeroAimedEffect ),
 m_dwJumpMarkerDuration( rhs.m_dwJumpMarkerDuration ),
 m_AimJumpReadyAnimation( rhs.m_AimJumpReadyAnimation ),
 m_AimJumppingAnimation( rhs.m_AimJumppingAnimation ),
 m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
 m_bEnableZeroAiming( rhs.m_bEnableZeroAiming ),
 m_bEnableDownAim( rhs.m_bEnableDownAim ),
 m_vSecondAttributeList( rhs.m_vSecondAttributeList ),
 m_vSecondZeroAttributeList( rhs.m_vSecondZeroAttributeList ),
 m_fZeroAimSpeedRate( rhs.m_fZeroAimSpeedRate ),
 m_dwNeedCoolTime( rhs.m_dwNeedCoolTime ),
 m_vGaugeInfoList( rhs.m_vGaugeInfoList ),
 m_iMaxBullet( rhs.m_iMaxBullet ),

 m_fMagicCircleRadius( rhs.m_fMagicCircleRadius ),
 m_fRange( rhs.m_fRange ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_fMoveSpeed( rhs.m_fMoveSpeed ),
 m_fHeightGap( rhs.m_fHeightGap ),
 m_GatherEffect( rhs.m_GatherEffect ),
 m_ChargeAttackDelay( rhs.m_ChargeAttackDelay ),
 m_SpecialAttackDelay( rhs.m_SpecialAttackDelay ),
 m_ChargeAttackBlueCircle( rhs.m_ChargeAttackBlueCircle ),
 m_ChargeAttackRedCircle( rhs.m_ChargeAttackRedCircle ), 
 m_iMaxSpecialWeaponCnt( rhs.m_iMaxSpecialWeaponCnt ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_vSpecialWeaponOffset( rhs.m_vSpecialWeaponOffset ),
 m_dwSpecialAttackAttribute( rhs.m_dwSpecialAttackAttribute ),
 m_SpecialAttack( rhs.m_SpecialAttack ),
 m_fSpecialAttackAniRate( rhs.m_fSpecialAttackAniRate ),
 m_GatherAnimation( rhs.m_GatherAnimation ),
 m_EnableCircle( rhs.m_EnableCircle ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
 m_dwPreDelayTime( rhs.m_dwPreDelayTime ),
 m_ChargeAttackAni( rhs.m_ChargeAttackAni ),
 m_fChargeAttackAniRate( rhs.m_fChargeAttackAniRate ),

 m_stExpansionAttackMotion( rhs.m_stExpansionAttackMotion ),
 m_fExpansionAttackMotionRate( rhs.m_fExpansionAttackMotionRate ),
 m_stExpansionAttackEffect_red( rhs.m_stExpansionAttackEffect_red ),
 m_stExpansionAttackEffect_blue( rhs.m_stExpansionAttackEffect_blue ),
 m_fExpansionAttackHeightOffset( rhs.m_fExpansionAttackHeightOffset ),
 m_ExtraAttackInfo( rhs.m_ExtraAttackInfo ),
 m_dwExpansionAttackTic( rhs.m_dwExpansionAttackTic ),
 m_dwExpansionAttackTicFirst( rhs.m_dwExpansionAttackTicFirst ),

 m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
 m_fDashEndJumpPower( rhs.m_fDashEndJumpPower ),

 m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),

 m_NormalCancelDashAttribute( rhs.m_NormalCancelDashAttribute ),
 m_JumpCancelDashAttribute( rhs.m_JumpCancelDashAttribute )
 
{
	m_dwMaxCoolTime			= rhs.m_dwMaxCoolTime;
	m_dwAimTime				= rhs.m_dwAimTime;
	m_fAimAngle				= rhs.m_fAimAngle;
	m_fAimRange				= rhs.m_fAimRange;
	m_fMinRange				= rhs.m_fMinRange;
		
	m_fMarkerRange			= rhs.m_fMarkerRange;

	m_dwFireStartTime		= 0;
	m_dwFireEndTime			= 0;
	m_dwFireDelayTime		= 0;
	m_fFireTimeModifyRate	= FLOAT1;

	m_dwChargeStartTime		= 0;
	m_dwAimStartTime		= 0;
	m_iCurCombo				= 0;
	m_FireState				= FS_NONE;

	m_bSetSkillAimed		= false;
	m_bSetAimedDir			= false;
	m_bAimJumpAttack		= false;

	m_fCurAngle				= 0.0f;
	m_iCurAirJump			= 0;
	m_fAirJumpPowerRate		= 0.0f;
	m_dwJumpMarkerEndTime	= 0;

	m_dwCheckAimCancelTime	= 0;
	m_dwTrackingEndTime		= 0;
	m_dwFireStateEndTime	= 0;

	m_bZeroAimState			= false;
	m_dwZeroAimStartTime	= 0;

	m_dwCurCoolTime			= 0;
	m_dwCheckCoolTime		= 0;

	m_bSetChargingMoveAni	= false;

	m_iCurBullet			= 0;

	m_fCurChargeRate		= 0.0f;


	m_dwCurMapEffect = -1;
	m_dwChargeAttackPreDelayStartTime = 0;
	m_dwSpecialStartTime = 0;
	m_dwSpecialEndTime = 0;
	m_dwSpecialFireTime = 0;
	m_dwChargeAttackFireTime = 0;
	m_dwChargeAttackEndTime = 0;

	m_bDashEndJumpState	= false;

	// 점프 후 D~ 
	m_dwChargingStartTime = 0;
	m_bJumpChargeAttack	= false;

	// 대시 
	m_dwMotionEndTime	= 0;
	m_dwCancelDashAttackTime	= 0;
	m_iChargeComboDashState		= 0;
}

ioNuClearSoldierItem::~ioNuClearSoldierItem()
{
	m_vSecondAttributeList.clear();
	m_vSecondZeroAttributeList.clear();
}

void ioNuClearSoldierItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// 게이지 : 0 ~ 100 D~ 떼기시 공격스킬이 달라진다. 
	// 1단계 : D~ 떼기 시 레이저 발사히지 않고, 실패 애니
	// 2단계 : D~ 떼기 시 일반 레이저 발사 
	// 3단계 : D~ 떼기 시 강경직 레이저 발사 
	// 4단계 : D~ 떼기 시 매직서클을 향해 핵미사일 투하 
	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	m_vGaugeInfoList.clear();
	m_vGaugeInfoList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		// 속성들을 로드하는 함수: extend_attack, _time_rate, _end_animation, _end_time_rate, _pre_delay, _delay, _force_cnt, _force_move,
		//						   _force_friction, _force_angle, _type_count, _type, _type%d_resist, _type%d_wounded,
		//			               _type%d_wounded_time, _type%d_wounded_loop_ani
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader ); 
		m_vExtendAttributeList.push_back( kAttribute );

		// 게이지 Min, Max 
		GaugeInfo kGaugeInfo;
		kGaugeInfo.Init();
		wsprintf_e( szBuf, "extend_attack0%d_gauge_min", i+1 );
		kGaugeInfo.m_nMin = rkLoader.LoadInt( szBuf, 0 );
		wsprintf_e( szBuf, "extend_attack0%d_gauge_max", i+1 );
		kGaugeInfo.m_nMax = rkLoader.LoadInt( szBuf, 0 );
		m_vGaugeInfoList.push_back( kGaugeInfo );
	}

	// 일반공격 캔슬 대시 
	LoadAttackAttribute_e( "dash_action", m_NormalCancelDashAttribute, rkLoader );
	m_NormalCancelDashAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "dash_force_cnt", 0 );
	if( iForceCnt > 0 )
	{
		m_NormalCancelDashAttribute.m_vForceInfoList.reserve( iForceCnt );
	}

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "dash_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "dash_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_NormalCancelDashAttribute.m_vForceInfoList.push_back( kInfo );
	}

	// 점프 일반 공격 캔슬 대시
	rkLoader.LoadString_e( "dash_jump_action", "", szBuf, MAX_PATH );
	m_JumpCancelDashAttribute.m_AttackAnimation = szBuf;

	m_JumpCancelDashAttribute.m_vForceInfoList.clear();
	iForceCnt = rkLoader.LoadInt_e( "dash_jump_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_JumpCancelDashAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "dash_jump_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "dash_jump_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_JumpCancelDashAttribute.m_vForceInfoList.push_back( kInfo );
	}


	// 점프공격 대시 후 한번더 점프
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_fDashEndJumpPower = rkLoader.LoadFloat_e( "dash_end_jump_power", 0.0f );

	// 점프 후 D~ 
	m_dwJumpChargeTime			= rkLoader.LoadInt_e( "jump_charge_time", 0 );

	LoadAttackAttribute_e( "extend_zero_aimed", m_ExtendZeroAimed, rkLoader );

	int iSecondCnt = rkLoader.LoadInt_e( "second_attack_cnt", 0 );
	m_vSecondAttributeList.clear();
	for( i=0; i < iSecondCnt; ++i )
	{
		wsprintf_e( szBuf, "second_attack%d_id", i+1 );
		int iIndex = rkLoader.LoadInt( szBuf, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szBuf, "second_attack%d_limite", i+1 );
		float fLimiteValue = rkLoader.LoadFloat( szBuf, 0.0f );
		
		SecondAttribute kSecondAttr;
		kSecondAttr.m_iAttributeIndex = iIndex;
		kSecondAttr.m_fLimiteValue = fLimiteValue;
		m_vSecondAttributeList.push_back( kSecondAttr );
	}

	m_vSecondZeroAttributeList.clear();
	int iSecondZeroCnt = rkLoader.LoadInt_e( "second_zero_attack_cnt", 0 );
	for( i=0; i < iSecondZeroCnt; ++i )
	{
		wsprintf_e( szBuf, "second_zero_attack%d_id", i+1 );
		int iIndex = rkLoader.LoadInt( szBuf, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szBuf, "second_zero_attack%d_limite", i+1 );
		float fLimiteValue = rkLoader.LoadFloat( szBuf, 0.0f );

		SecondAttribute kSeconZeroAttr;
		kSeconZeroAttr.m_iAttributeIndex = iIndex;
		kSeconZeroAttr.m_fLimiteValue = fLimiteValue;
		m_vSecondZeroAttributeList.push_back( kSeconZeroAttr );
	}

	char szKey[MAX_PATH];
	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	// === 
	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;
	rkLoader.LoadString_e( "charge_attack_delay_ani", "", szBuf, MAX_PATH );
	m_ChargeAttackDelay = szBuf;
	rkLoader.LoadString_e( "special_attack_delay_ani", "", szBuf, MAX_PATH );
	m_SpecialAttackDelay = szBuf;	// INI에 없음
	rkLoader.LoadString_e( "charge_attack_blue_team_circle", "", szBuf, MAX_PATH );
	m_ChargeAttackBlueCircle = szBuf;// INI에 없음
	rkLoader.LoadString_e( "charge_attack_red_team_circle", "", szBuf, MAX_PATH );
	m_ChargeAttackRedCircle = szBuf;
	m_iMaxSpecialWeaponCnt = rkLoader.LoadInt_e( "max_special_weapon_cnt", 0 );
	m_vSpecialWeaponOffset.clear();
	m_vSpecialWeaponOffset.reserve( max(m_iMaxSpecialWeaponCnt, 1) );	
	for( int i = 1; i <=m_iMaxSpecialWeaponCnt; i++  )
	{
		wsprintf_e( szKey, "special_weaponOffset%d", i );	// INI에 없음
		int nValue = rkLoader.LoadInt( szKey, 0 );;
		m_vSpecialWeaponOffset.push_back( nValue );
	}
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	m_dwSpecialAttackAttribute = rkLoader.LoadInt_e( "special_weapon_attribute", 0 );// INI에 없음
	rkLoader.LoadString_e( "special_attack_ani", "", szBuf, MAX_PATH );// INI에 없음
	m_SpecialAttack = szBuf;
	m_fSpecialAttackAniRate = rkLoader.LoadFloat_e( "special_attack_ani_rate", FLOAT1 );// INI에 없음
	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_EnableCircle = szBuf;
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );// INI에 없음
	m_AreaWeaponName = szBuf;
	m_dwPreDelayTime = rkLoader.LoadInt_e( "charge_attack_predelay_time", 800 );// INI에 없음
	rkLoader.LoadString_e( "charge_attack_ani", "", szBuf, MAX_PATH );// INI에 없음
	m_ChargeAttackAni = szBuf;
	m_fChargeAttackAniRate = rkLoader.LoadFloat_e( "charge_attack_ani_rate", FLOAT1 );// INI에 없음
	// ====

	// === 
	rkLoader.LoadString_e( "charge_expansion_attack_ani", "", szBuf, MAX_PATH );
	m_stExpansionAttackMotion = szBuf;

	m_fExpansionAttackMotionRate = rkLoader.LoadFloat_e( "charge_expansion_attack_ani_rate", 350.0f );

	rkLoader.LoadString_e( "extend_attack04_ani_land_red_effect", "", szBuf, MAX_PATH );
	m_stExpansionAttackEffect_red = szBuf;
	rkLoader.LoadString_e( "extend_attack04_ani_land_blue_effect", "", szBuf, MAX_PATH );
	m_stExpansionAttackEffect_blue = szBuf;

	m_fExpansionAttackHeightOffset = rkLoader.LoadFloat_e( "charge_expansion_attack_height_offset", 1000.f );// INI에 없음
	wsprintf_e( szKey, "charge_expansion_attack_type" );
	m_ExtraAttackInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "charge_expansion_attack_type_resist" );
	m_ExtraAttackInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "charge_expansion_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_ExtraAttackInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "charge_expansion_attack_type_wounded_time" );
	m_ExtraAttackInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "charge_expansion_attack_type_wounded_loop_ani" );
	m_ExtraAttackInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );
	// ====

	rkLoader.LoadString_e( "miss_fire_animation", "", szBuf, MAX_PATH );
	m_MissFire = szBuf;
	m_fMissFireRate = rkLoader.LoadFloat_e( "miss_fire_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "aim_up_animation", "", szBuf, MAX_PATH );
	m_AimUp = szBuf;
	rkLoader.LoadString_e( "aim_center_animation", "", szBuf, MAX_PATH );
	m_AimCenter = szBuf;
	rkLoader.LoadString_e( "aim_down_animation", "", szBuf, MAX_PATH );
	m_AimDown = szBuf;

	rkLoader.LoadString_e( "zero_aim_up_animation", "", szBuf, MAX_PATH );
	m_ZeroAimUp = szBuf;
	rkLoader.LoadString_e( "zero_aim_center_animation", "", szBuf, MAX_PATH );
	m_ZeroAimCenter = szBuf;
	rkLoader.LoadString_e( "zero_aim_down_animation", "", szBuf, MAX_PATH );
	m_ZeroAimDown = szBuf;

	rkLoader.LoadString_e( "jump_aim_up_animation", "", szBuf, MAX_PATH );
	m_JumpAimUp = szBuf;
	rkLoader.LoadString_e( "jump_aim_center_animation", "", szBuf, MAX_PATH );
	m_JumpAimCenter = szBuf;
	rkLoader.LoadString_e( "jump_aim_down_animation", "", szBuf, MAX_PATH );
	m_JumpAimDown = szBuf;

	rkLoader.LoadString_e( "fire_up_animation", "", szBuf, MAX_PATH );
	m_FireUp = szBuf;
	rkLoader.LoadString_e( "fire_center_animation", "", szBuf, MAX_PATH );
	m_FireCenter = szBuf;
	rkLoader.LoadString_e( "fire_down_animation", "", szBuf, MAX_PATH );
	m_FireDown = szBuf;

	rkLoader.LoadString_e( "zero_fire_up_animation", "", szBuf, MAX_PATH );
	m_ZeroFireUp = szBuf;
	rkLoader.LoadString_e( "zero_fire_center_animation", "", szBuf, MAX_PATH );
	m_ZeroFireCenter = szBuf;
	rkLoader.LoadString_e( "zero_fire_down_animation", "", szBuf, MAX_PATH );
	m_ZeroFireDown = szBuf;

	rkLoader.LoadString_e( "jump_fire_up_animation", "", szBuf, MAX_PATH );
	m_JumpFireUp = szBuf;
	rkLoader.LoadString_e( "jump_fire_center_animation", "", szBuf, MAX_PATH );
	m_JumpFireCenter = szBuf;
	rkLoader.LoadString_e( "jump_fire_down_animation", "", szBuf, MAX_PATH );
	m_JumpFireDown = szBuf;

	rkLoader.LoadString_e( "aim_jump_ready_animation", "", szBuf, MAX_PATH );
	m_AimJumpReadyAnimation = szBuf;
	rkLoader.LoadString_e( "aim_jumpping_animation", "", szBuf, MAX_PATH );
	m_AimJumppingAnimation = szBuf;

	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_dwNeedCoolTime = (DWORD)rkLoader.LoadInt_e( "use_cool_time", 0 );
	m_bEnableZeroAiming = rkLoader.LoadBool_e( "enable_zero_aiming", false );
	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	m_iMaxBullet = rkLoader.LoadInt_e( "max_bullet", 0 );

	m_dwZeroAimedCheckTime = rkLoader.LoadInt_e( "zero_aimed_check_time", 1 );
	rkLoader.LoadString_e( "zero_aimed_effect", "", szBuf, MAX_PATH );
	m_ZeroAimedEffect = szBuf;

	m_fZeroAimSpeedRate = rkLoader.LoadFloat_e( "zero_aim_speed_rate", 0.0f );

	m_dwJumpMarkerDuration = rkLoader.LoadFloat_e( "jump_marker_duration", 0 );

	m_dwAimTime = rkLoader.LoadInt_e( "aim_time", 1000 );
	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );

	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );

	m_fMagicCircleRadius = rkLoader.LoadFloat_e( "magic_circle_radius", FLOAT100 );
	m_fRange = rkLoader.LoadFloat_e( "magic_circle_range", FLOAT1000 );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 350.0f );
	m_fHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 20.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	// 
	m_dwExpansionAttackTic = (DWORD)rkLoader.LoadInt_e( "extend_attack04_tic", 10000 );
	m_dwExpansionAttackTicFirst = (DWORD)rkLoader.LoadInt_e( "extend_attack04_tic_first", 10000 );

}

ioItem* ioNuClearSoldierItem::Clone()
{
	return new ioNuClearSoldierItem( *this );
}

void ioNuClearSoldierItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	m_fCurChargeRate = 0.0f;	
	m_iCurBullet = 0;

	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		m_FireState				= FS_CHARGING;
		m_dwChargeStartTime		= FRAMEGETTIME();
		m_iCurCombo				= iCurCombo;
		m_fCurAngle				= 0.0f;
		m_dwCheckAimCancelTime	= 0;
		m_dwZeroAimStartTime	= 0;
		m_bZeroAimState			= false;
		m_bAimJumpAttack		= false;
		m_bSetAimedDir			= false;
		m_bAimedState			= false;
		m_bSetChargingMoveAni	= false;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );
		
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pGrp->SetActionAniForTime( m_AttackReadyAniList[iCurCombo], FLOAT100, FLOAT100, m_dwChargeTime );

		m_dwNextExpansionFireTime = 0;
	}
	else   
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_FireState = FS_NONE;

	}
}

void ioNuClearSoldierItem::ChangeToAiming( ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		return;
	}

	pOwner->StopAutoTargetTracking();

	//DWORD dwCurTime = FRAMEGETTIME();
	m_dwChargingStartTime = FRAMEGETTIME();

	if( m_bEnableZeroAiming && m_bZeroAimState )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
	}
	else
	{
		SetZeroAim( pOwner, false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );		
	}

	if( m_bEnableZeroAiming )
	{
		if( m_dwZeroAimStartTime == 0 )
		{
			m_dwZeroAimStartTime = m_dwChargingStartTime;
		}
	}
	else
	{
		m_dwZeroAimStartTime = 0;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	bool bJumpState = pOwner->IsJumpState();
	if( bJumpState )
	{
		pGrp->SetLoopAni( m_JumpAimCenter, FLOAT100 );
	}
	else
	{
		pGrp->SetLoopAni( m_AimCenter, FLOAT100 );
	}

	m_FireState = FS_AIMING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetAimedDir = false;
	m_bAimedState = false;

	g_TutorialMgr.ActionChangeToAiming();
}

bool ioNuClearSoldierItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
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
	DWORD dwInputSpecialTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialTime )
	{
		dwInputSpecialTime *= m_fCurAniRate;
		dwInputSpecialTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialTime && dwCurTime > dwInputSpecialTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

bool ioNuClearSoldierItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( m_FireState == FS_SPECIAL_DASH )
	{
		return	false;
	}

	return	true;
}

void ioNuClearSoldierItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	AttackAttribute	cancelDash;
	if( pOwner->GetState() == CS_ATTACK )
	{
		cancelDash	= m_NormalCancelDashAttribute;
	}
	else
	{
		////cancelDash	= m_JumpCancelDashAttribute;
		////pOwner->SetUsedJumpAttack(false);
		////pOwner->SetJumppingState();
		//ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
		//if( pItem )
		//{			
		//	// 점프공격 대시 이후 한번더 점프
		//	ioAimJump *pAimJump = ToAimJump( pItem->GetCurExtendJump() );
		//	if( pAimJump )
		//	{				
		//		pAimJump->JumpCancelDashAttack( pOwner );
		//		m_bEnableDashState = false;
		//		//if( pAimJump && !pAimJump->GetCancelDashEnd() )
		//		//{
		//		//	m_bDashEndJumpState = true;
		//		//	m_FireState = FS_NONE;
		//		//	pOwner->SetSKillEndJumpState( m_fDashEndJumpPower, false, false, true, false );
		//		//	return;
		//		//}
		//	}			
		//}

		m_FireState = FS_SPECIAL_DASH;
		return;
	}

	int iAniID = pGrp->GetAnimationIdx( cancelDash.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = cancelDash.m_dwPreDelay;
	DWORD dwEndDelay = cancelDash.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = cancelDash.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( cancelDash.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		cancelDash.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	m_dwKeyReserveTime	= 0;
	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	DWORD dwInputSpecialDashTime = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( dwInputSpecialDashTime > 0 )
		m_dwCancelDashAttackTime = dwStartTime + dwInputSpecialDashTime;

	m_iChargeComboDashState	= 0;
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioAimJump *pAimJump = ToAimJump( pItem->GetCurExtendJump() );
	if( pAimJump )
	{
		m_iChargeComboDashState = pAimJump->GetCurJumpState();
	}

	m_FireState	= FS_SPECIAL_DASH;
	pOwner->SetState( CS_NUCLEARSOLDIER_SPECIAL );

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );	
		kPacket << false;	//AimJump
		kPacket << static_cast<int>( m_FireState );
		kPacket << m_iCurCombo;		
		kPacket << vPos;
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNuClearSoldierItem::ProcessSpecialDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		m_FireState	= FS_NONE;
		//ClearSpecialState( pOwner );
		ChangeReserveKeyInputState( pOwner );

		return;
	}

	//if( m_iChargeComboDashState == ioAimJump::AJS_DASH )
	//{
	//	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() && m_dwCancelDashAttackTime > 0 && m_dwCancelDashAttackTime < dwCurTime )
	//	{

	//	}
	//}
	//if( m_iChargeComboDashState == ioChargeComboDash::DS_NORMAL_ATTACK )
	//{
	//	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() && m_dwCancelDashAttackTime > 0 && m_dwCancelDashAttackTime < dwCurTime )
	//		ChangeToAttackAfterSpecialDash( pOwner, pOwner->CheckCurDirKey() );
	//}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}
}

void ioNuClearSoldierItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
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
				if ( !COMPARE( iCurCombo, 0, GetMaxCombo() ) )
					iCurCombo = 0;
				pOwner->SetNormalAttack( iCurCombo );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

void ioNuClearSoldierItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FireState )
	{
		case FS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;
		case FS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserveByConditionalSpecialDash( pOwner );
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;

				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;
			}

			ioAttackableItem::CheckNormalAttack( pOwner );
		}
		break;
	case FS_GATHERING:
		OnGathering( pOwner );
		break;
	case FS_CHARGE_ATTACK_DELAY:
		if( m_dwChargeAttackPreDelayStartTime > 0 && m_dwChargeAttackPreDelayStartTime + m_dwPreDelayTime < dwCurTime )
		{		
			m_dwChargeAttackPreDelayStartTime = 0;
			SetChargeAttackState( pOwner );
		}
		break;
	case FS_CHARGE_ATTACK:
		if( m_dwChargeAttackFireTime != 0 && m_dwChargeAttackFireTime < dwCurTime )
		{
			CreateArea( pOwner );
			CheckDecreaseGauge();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			m_dwChargeAttackFireTime = 0;
		}
		else if( m_dwChargeAttackEndTime != 0 && m_dwChargeAttackEndTime < dwCurTime )
		{
			pOwner->EndMapEffect( m_dwCurMapEffect );
			pOwner->DestroyMagicCircle();

			m_FireState = FS_NONE;
			pOwner->SetState( CS_DELAY );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			m_dwChargeAttackEndTime = 0;
		}
		break;
	case FS_AIMING:
		OnAiming( pOwner );
		break;
	case FS_AIM_WAIT:
		OnAimWait( pOwner );
		break;
	case FS_AIMED:
		OnAimed( pOwner );
		break;
	case FS_NORMAL_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserveByConditionalSpecialDash( pOwner );
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;

				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;
			}

			OnFire( pOwner );
		}
		break;
	case FS_AIMED_FIRE:
	case FS_FIREEXPANSION_ATTACK: 
		OnFire( pOwner );
		break;
	}

	if( m_FireState == FS_AIMING || m_FireState == FS_AIM_WAIT || m_FireState == FS_AIMED )
	{
		bool bIsCDEKeyChanged = pOwner->ChangeDirectionExplicitIfKeyChanged( false );
		if( bIsCDEKeyChanged )
		{
			if( m_FireState != FS_AIMING )
			{
				m_AimedTarget.Clear();
				m_bAimedBall = false;
				                                                                                                                                                                                               
				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
				ChangeToAiming( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;	//AimJump
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					kPacket << false;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}

	if( m_FireState != FS_AIMED && m_FireState != FS_AIMING && m_FireState != FS_AIM_WAIT && m_dwZeroAimStartTime > 0 )
	{
		m_dwZeroAimStartTime = 0;
	}

	if( !m_bZeroAimState )
	{
		pOwner->EndEffect( m_ZeroAimedEffect );
	}
}

void ioNuClearSoldierItem::OnCharging( ioBaseChar *pOwner )
{
	bool bIsNeedProcess = !pOwner->IsNeedProcess();
	if( bIsNeedProcess )
	{
		return;
	}

	bool bIsAttackKeyDown = pOwner->IsAttackKeyDown();
	if( bIsAttackKeyDown )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			pOwner->EndEffect( m_GatherEffect );
			ChangeToGathering( pOwner );
			//ChangeToAiming( pOwner );	// 게이지 부분
			//if( pOwner->IsNeedSendNetwork() )
			//{
			//	SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			//	FillItemGaugePacketHeader( pOwner, kPacket );
			//	kPacket << false;	//AimJump
			//	kPacket << (int)m_FireState;
			//	kPacket << m_iCurCombo;
			//	kPacket << false;
			//	P2PNetwork::SendToAllPlayingUser( kPacket );
			//}
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioNuClearSoldierItem::OnAiming( ioBaseChar *pOwner )
{
	bool bIsNeedProcess = !pOwner->IsNeedProcess();
	if( bIsNeedProcess )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	bool bIsAttackKeyDown = !pOwner->IsAttackKeyDown();
	CharState cstate = pOwner->GetState();
	if( bIsAttackKeyDown && cstate != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		if( IsCanFire( pOwner, 1 ) )
		{
			ChangeToNormalFire( pOwner );
			return;
		}
		else
		{
			pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );
			m_AimedTarget.Clear();
			m_bAimedBall = false;

			m_FireState = FS_NONE;
			
			bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
			if( bIsNeedSendNetwork )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << false;	//AimJump
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				kPacket << true;	// MissFire
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}

	bool bSetAimJumpState = !SetAimJumpState( pOwner );
	if( bSetAimJumpState )
	{
		return;
	}

	bool IsDefenseKeyDown = pOwner->IsDefenseKeyDown();
	cstate = pOwner->GetState();
	if( cstate != CS_JUMP && IsDefenseKeyDown )
	{
		bool bIsCanFire = IsCanFire(0);
		if( bIsCanFire )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
			{
				m_FireState = FS_NONE;
				return;
			}
		}
	}

	if( m_bEnableZeroAiming )
	{
		bool bZeroAim = CheckZeroAim( pOwner );
		bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
		if( bZeroAim && bIsNeedSendNetwork )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_AimedTarget;
			kPacket << m_bAimedBall;
			kPacket << m_iCurCombo;
			kPacket << m_bZeroAimState;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}				

		if( bZeroAim )
		{
			g_TutorialMgr.ActionZeroAimed();
		}
		else
		{
			g_TutorialMgr.ActionAimed();
		}
	}

	ioHashString szTarget;
	bool bCheckAimedTarget = pOwner->CheckAimedTarget( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim );
	if( bCheckAimedTarget )
	{
		m_AimedTarget = szTarget;
	}

	// 타겟 비교
	bool bCompareAimedTarget = CompareAimedTarget( pOwner );
	if( bCompareAimedTarget )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = true;
	}

	bool bIsEmpty = !m_AimedTarget.IsEmpty();
	if( bIsEmpty || m_bAimedBall )
	{
		ChangeToAimWaitState( pOwner, m_AimedTarget );	// 타겟 세팅됨
	}
}

void ioNuClearSoldierItem::OnAimWait( ioBaseChar *pOwner )
{
	bool bIsNeedProcess = !pOwner->IsNeedProcess();
	if( bIsNeedProcess )
	{
		return;
	}

	bool bIsAttackKeyDown = !pOwner->IsAttackKeyDown();
	if( bIsAttackKeyDown )
	{
		int i = 0;
	}
	CharState cstate = pOwner->GetState();
	if( bIsAttackKeyDown  && cstate != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		bool bIsCanFire = IsCanFire( pOwner, 1 );
		if( bIsCanFire )
		{
			ChangeToNormalFire( pOwner );
			return;
		}
		else
		{
			pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );
			m_AimedTarget.Clear();
			m_bAimedBall = false;

			m_FireState = FS_NONE;
			bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
			if( bIsNeedSendNetwork )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << false;	//AimJump
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				kPacket << true;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}

	bool bSetAimJumpState = !SetAimJumpState( pOwner );
	if( bSetAimJumpState )
	{
		return;
	}

	cstate = pOwner->GetState();
	bool bIsDefenseKeyDown = pOwner->IsDefenseKeyDown();
	if( cstate != CS_JUMP && bIsDefenseKeyDown )
	{
		bool bIsCanFire = IsCanFire(0);
		if( bIsCanFire )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			bool bIsBuffLimitSkill = !pOwner->IsBuffLimitSkill();
			bool bSetUseSkill = pOwner->SetUseSkill(0, SUT_NORMAL);
			if( iSkillNum == 0 && bIsBuffLimitSkill && bSetUseSkill )
			{
				m_FireState = FS_NONE;
				return;
			}
		}
	}

	DWORD dwTimeGap = FRAMEGETTIME() - m_dwAimStartTime;
	cstate = pOwner->GetState();
	if( cstate != CS_JUMP && dwTimeGap > m_dwAimTime )	// 조준 완료
	{
		ChangeToAimedState( pOwner );

		bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
		if( bIsNeedSendNetwork )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_AimedTarget;
			kPacket << m_bAimedBall;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	//else if( pOwner->GetState() == CS_JUMP && dwTimeGap > m_dwAimTime/2 )	// 조준 완료
	else if( pOwner->GetState() == CS_JUMP && dwTimeGap > m_dwAimTime )	// 조준 완료 2020-02-24
	{			
		ChangeToAimedState( pOwner );

		bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
		if( bIsNeedSendNetwork )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_AimedTarget;
			kPacket << m_bAimedBall;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		bool bCheckCurrentTargetValid = !CheckCurrentTargetValid( pOwner, true );
		if( bCheckCurrentTargetValid )
		{
			m_dwTrackingEndTime = FRAMEGETTIME();
			
			// 대상을 제거한후 조준대기로 넘겨야함
			m_AimedTarget.Clear();
			m_bAimedBall = false;

			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			ChangeToAiming( pOwner );
		}
	}
}

void ioNuClearSoldierItem::OnAimed( ioBaseChar *pOwner )
{
	bool bIsNeedProcess = pOwner->IsNeedProcess();
	if( bIsNeedProcess )
	{
		CharState cstate = pOwner->GetState();
		if( cstate != CS_JUMP && !m_bZeroAimState )
		{
			bool bZeroAim = CheckZeroAim( pOwner );
			bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();

			if( bZeroAim && bIsNeedSendNetwork )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_FireState;
				kPacket << m_AimedTarget;
				kPacket << m_bAimedBall;
				kPacket << m_iCurCombo;
				kPacket << m_bZeroAimState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			
			if( bZeroAim )
			{
				g_TutorialMgr.ActionZeroAimed();
			}
			else
			{
				g_TutorialMgr.ActionAimed();
			}
		}

		bool bIsAttackKeyDown = !pOwner->IsAttackKeyDown();
		cstate = pOwner->GetState();
		if( bIsAttackKeyDown && cstate != CS_JUMP )
		{
			bool bIsCanFire = IsCanFire( pOwner, 1 );
			if( bIsCanFire )
			{
				ChangeToAimedFire( pOwner );
				bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
				if( bIsNeedSendNetwork )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_FireState;
					kPacket << m_AimedTarget;
					kPacket << m_bAimedBall;
					kPacket << m_iCurCombo;
					kPacket << m_vAimedDir;
					kPacket << m_bSetAimedDir;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				g_TutorialMgr.ActionAimedFireDeveloperK(m_AimedTarget);
			}
			else
			{
				pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );
				m_AimedTarget.Clear();
				m_bAimedBall = false;

				m_FireState = FS_NONE;
				bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
				if( bIsNeedSendNetwork )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;	//AimJump
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					kPacket << true;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}
		else	// AIMED FIRE..
		{
			bool bSetAimJumpState = !SetAimJumpState( pOwner );
			if( bSetAimJumpState )
			{
				return;
			}

			bool bCheckCurrentTargetValid = CheckCurrentTargetValid( pOwner, true );
			if( bCheckCurrentTargetValid )
			{
				pOwner->SetTargetMarkerEnable( IsCanFire( pOwner, 1 ) );
				TrackingTarget( pOwner );

				CharState cstate = pOwner->GetState();
				bool bIsDefenseKeyDown = pOwner->IsDefenseKeyDown();
				if( cstate != CS_JUMP && bIsDefenseKeyDown )
				{
					bool bIsCanFire = IsCanFire(0);
					if( bIsCanFire )
					{
						int iSkillNum = pOwner->GetSkillKeyInput();
						bool bIsBuffLimitSkill = !pOwner->IsBuffLimitSkill();
						bool bSetUseSkill = pOwner->SetUseSkill(0, SUT_NORMAL);
						if(iSkillNum == 0 && bIsBuffLimitSkill && bSetUseSkill )
						{
							pOwner->SetChargingState( false );
							m_bSetSkillAimed = true;
							m_FireState = FS_NONE;
							bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
							if( bIsNeedSendNetwork )
							{
								SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
								FillItemGaugePacketHeader( pOwner, kPacket );
								kPacket << (int)m_FireState;
								kPacket << m_AimedTarget;
								kPacket << m_bAimedBall;
								kPacket << m_iCurCombo;
								kPacket << m_vAimedDir;
								kPacket << m_bSetAimedDir;
								kPacket << m_bSetSkillAimed;
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}							
							return;
						}
					}
				}
			}
			else
			{
				m_dwTrackingEndTime = FRAMEGETTIME();
				
				// 대상을 제거한후 조준대기로 넘겨야함
				m_AimedTarget.Clear();
				m_bAimedBall = false;

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
				ChangeToAiming( pOwner );

				bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
				if( bIsNeedSendNetwork )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;	//AimJump
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					kPacket << false;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
	else
	{
		bool bCheckCurrentTargetValid = CheckCurrentTargetValid( pOwner, false );
		if( bCheckCurrentTargetValid )
		{
			TrackingTarget( pOwner );
		}
	}
}

void ioNuClearSoldierItem::OnFire( ioBaseChar *pOwner )
{
	bool bCheckDefaultNormalAttack = pOwner->CheckDefaultNormalAttack( m_dwFireStartTime, m_dwFireEndTime, m_dwFireDelayTime );
	if( bCheckDefaultNormalAttack )
	{
		m_FireState = FS_NONE;
	}
}

void ioNuClearSoldierItem::OnJumpFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwAttackOverTime = m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime;

	if( dwCurTime >= dwAttackOverTime )
	{
		m_FireState = FS_NONE;
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}
}

void ioNuClearSoldierItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );


	// 현재 애니 
	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	int	iAniID	= pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
	if ( iAniID == -1 )	{	return;	}

	m_szCurAni = pAttribute->m_AttackAnimation;
	m_dwInputCancelCheckTime = FRAMEGETTIME();

	m_FireState = FS_NORMAL_ATTACK;

	bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
	if( bIsNeedSendNetwork )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;	//AimJump
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << false;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNuClearSoldierItem::ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	m_dwAimStartTime = FRAMEGETTIME();
	m_FireState = FS_AIM_WAIT;
	m_AimedTarget = szTarget;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMING );
}

void ioNuClearSoldierItem::ChangeToAimedState( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	if( m_bZeroAimState )
	{
		if( !pGrp->GetLoopInPlay( m_ZeroAimUp ) )
		{
			pGrp->SetLoopAni( m_ZeroAimUp, 0.0f, 0.0f );
		}

		if( !pGrp->GetLoopInPlay( m_ZeroAimDown ) )
		{
			pGrp->SetLoopAni( m_ZeroAimDown, 0.0f, 0.0f );
		}

		if( !pGrp->GetLoopInPlay( m_ZeroAimCenter ) )
		{
			pGrp->SetLoopAni( m_ZeroAimCenter, 0.0f, FLOAT1 );
		}
	}
	else if( pOwner->GetState() != CS_JUMP )
	{
		pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );

		if( !pGrp->GetLoopInPlay( m_AimCenter ) )
		{
			pGrp->SetLoopAni( m_AimCenter, 0.0f, FLOAT1 );
		}
	}
	else
	{
		pGrp->SetLoopAni( m_JumpAimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpAimDown, 0.0f, 0.0f );

		if( !pGrp->GetLoopInPlay( m_JumpAimCenter ) )
		{
			pGrp->SetLoopAni( m_JumpAimCenter, 0.0f, FLOAT1 );
		}
	}

	m_FireState = FS_AIMED;
	m_dwZeroAimStartTime = FRAMEGETTIME();

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	// 발사가능 여부를 먼저 설정해 둘 것
	pOwner->SetTargetMarkerEnable( IsCanFire(pOwner, 1 ) );

	if( m_bZeroAimState )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
	}
	else
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	}

	bool bCheckCurrentTargetValid = CheckCurrentTargetValid( pOwner, false );
	if( bCheckCurrentTargetValid )
	{
		TrackingTarget( pOwner );
	}
}

void ioNuClearSoldierItem::ChangeToNormalFire( ioBaseChar *pOwner, D3DXVECTOR3 vPos )
{
	int iExtendAttackCnt = m_vExtendAttributeList.size();
	if( iExtendAttackCnt <= m_iCurCombo )
		return;

	CharState cstate = pOwner->GetState();
	if( cstate == CS_JUMP )
	{
		return;
	}	

	const AttackAttribute *pAttr = NULL;
	if( m_bZeroAimState )
	{
		pAttr = &m_ExtendZeroAimed;
	}
	else
	{
		pAttr = &m_vExtendAttributeList[m_iCurCombo];
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate   = pAttr->m_fAttackAniRate * m_fFireTimeModifyRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_NORMAL_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	DecreaseCoolTime();

	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;	
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fEndAniRate = pAttr->m_fAttackEndAniRate;
		pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndAniRate,
							m_dwFireEndTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}


	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );


	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_szCurAni = pAttr->m_AttackAnimation;
	m_dwInputCancelCheckTime = FRAMEGETTIME();
	m_FireState = FS_NORMAL_FIRE;

	if( iExtendAttackCnt <= m_iCurCombo + 1 )
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	}
	else
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );	

	bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
	if( bIsNeedSendNetwork )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;	//AimJump
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		//kPacket << false;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	g_TutorialMgr.ActionNormalFire();
}

bool ioNuClearSoldierItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
	{
		return false;
	}
	else if( !pTarget && !pBall )
	{
		m_bAimedBall = false;
		return false;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		if( !pTarget->IsEnableAimTarget(m_bEnableDownAim) )
		{
			return false;
		}

		if( pTarget->HasHideBuff() )
		{
			return false;
		}

		if( pTarget->IsChatModeState( false ) )
		{
			return false;
		}

		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}

	D3DXVECTOR3 vDiff = vTargetPos - pOwner->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > m_fAimRange * m_fAimRange )
	{
		return false;
	}

	if( D3DXVec3LengthSq( &vDiff ) < m_fMinRange * m_fMinRange )
	{
		return false;
	}

	if( bAngleAlsoCheck )
	{
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( pOwner->IsJumpState() )
		{
			float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
			if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
			{
				return false;
			}
		}
		else
		{
			D3DXVECTOR3 vCharDir;
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			vCharDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			
			float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
			if( D3DXVec3Dot( &vCharDir, &vDiff ) < fHalfCosine )
			{
				return false;
			}
		}
	}

	return true;
}

void ioNuClearSoldierItem::TrackingTarget( ioBaseChar *pOwner )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		vTargetPos = pTarget->GetMidPositionByRate();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetMidPositionByRate();
	}

	bool bJumpState = pOwner->IsJumpState();
	if( !bJumpState )
	{
		pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	if( bJumpState )
	{
		pUp = pGrp->GetLoopInPlay( m_JumpAimUp );
		pCenter = pGrp->GetLoopInPlay( m_JumpAimCenter );
		pDown = pGrp->GetLoopInPlay( m_JumpAimDown );
	}
	else if( m_bZeroAimState )
	{
		pUp = pGrp->GetLoopInPlay( m_ZeroAimUp );
		pCenter = pGrp->GetLoopInPlay( m_ZeroAimCenter );
		pDown = pGrp->GetLoopInPlay( m_ZeroAimDown );
	}
	else
	{
		pUp = pGrp->GetLoopInPlay( m_AimUp );
		pCenter = pGrp->GetLoopInPlay( m_AimCenter );
		pDown = pGrp->GetLoopInPlay( m_AimDown );
	}

	D3DXVECTOR3 vAimedDir = vTargetPos - pOwner->GetMidPositionByRate();
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
	m_bSetAimedDir = true;
	m_bAimedState = true;
}

void ioNuClearSoldierItem::ChangeToAimedFire( ioBaseChar *pOwner )
{
	int iExtendAttackCnt = m_vExtendAttributeList.size();
	if( iExtendAttackCnt <= m_iCurCombo )
	{
		return;
	}

	if( pOwner->GetState() == CS_JUMP )
	{
		return;
	}

	const AttackAttribute *pAttr = NULL;
	if( m_bZeroAimState )
	{
		pAttr = &m_ExtendZeroAimed;
	}
	else
		pAttr = &m_vExtendAttributeList[m_iCurCombo];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_NORMAL_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	DecreaseCoolTime();

	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}
	}
	else
	{
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false );
		}

		float fEndAniRate = pAttr->m_fAttackEndAniRate;
		pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndAniRate,
							m_dwFireEndTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}


	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );


	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_FireState = FS_AIMED_FIRE;

	if( iExtendAttackCnt <= m_iCurCombo + 1 )
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	}
	else
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );
}

float ioNuClearSoldierItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir,
											  float fMaxAngle )
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

void ioNuClearSoldierItem::AimedFireToUp( ioEntityGroup *pGrp,
									  float fWeight,
									  float fTimeRate,
									  DWORD dwPreDelay,
									  bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
	{
		fFadeOut = 0.0f;
	}

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;
	if( m_bAimJumpAttack )
	{
		szFireUp = m_JumpFireUp;
		szFireCenter = m_JumpFireCenter;
	}
	else if( m_bZeroAimState )
	{
		szFireUp = m_ZeroFireUp;
		szFireCenter = m_ZeroFireCenter;
	}
	else
	{
		szFireUp = m_FireUp;
		szFireCenter = m_FireCenter;
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

void ioNuClearSoldierItem::AimedFireToDown( ioEntityGroup *pGrp,
									    float fWeight,
									    float fTimeRate,
										DWORD dwPreDelay,
										bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
	{
		fFadeOut = 0.0f;
	}

	bool bAniFx1 = true;
	bool bAniFx2 = false;
	
	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}
	
	ioHashString szFireDown, szFireCenter;
	if( m_bAimJumpAttack )
	{
		szFireDown = m_JumpFireDown;
		szFireCenter = m_JumpFireCenter;
	}
	else if( m_bZeroAimState )
	{
		szFireDown = m_ZeroFireDown;
		szFireCenter = m_ZeroFireCenter;
	}
	else
	{
		szFireDown = m_FireDown;
		szFireCenter = m_FireCenter;
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

void ioNuClearSoldierItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	bool bZeroAimState;
	rkPacket >> iFireState;
	rkPacket >> m_AimedTarget;
	rkPacket >> m_bAimedBall;
	rkPacket >> m_iCurCombo;

	D3DXVECTOR3 vPos = D3DXVECTOR3( 0.f, 0.f, 0.f);	

	switch( iFireState )
	{
	case FS_SPECIAL_ATTACK:
		{
			m_vSpecialWeaponPosList.clear();
			int nCnt = 0;
			rkPacket >> nCnt;
			m_iCurSpecialWeaponCnt = nCnt;
			for( int i = 0; i < nCnt; i++ )
			{
				D3DXVECTOR3 vPos;
				rkPacket >> vPos;
				m_vSpecialWeaponPosList.push_back( vPos );
			}
			ChangeToSpecialAttackState( pOwner );
		}
		break;
	case FS_SPECIAL_ATTACK_DELAY:
		{
			D3DXVECTOR3 vCirclePos;
			rkPacket >> vCirclePos;
			pOwner->SetMagicCirclePos( false, vCirclePos );

			ChangeToSpecialDelayState( pOwner );
		}
		break;
	case FS_SPECIAL_END:
		ChangeToSpecialEndState( pOwner );
		break;

	case FS_AIMED_FIRE:
		rkPacket >> m_vAimedDir;
		rkPacket >> m_bSetAimedDir;
		ChangeToAimedFire( pOwner );
		break;
	case FS_AIMING:
		rkPacket >> bZeroAimState;
		if( bZeroAimState )
			SetZeroAim( pOwner, bZeroAimState );
		break;
	case FS_AIMED:
		rkPacket >> bZeroAimState;
		if( bZeroAimState )
			SetZeroAim( pOwner, bZeroAimState );
		else
			ChangeToAimedState( pOwner );
		break;
	case FS_NONE:
		rkPacket >> m_vAimedDir;
		rkPacket >> m_bSetAimedDir;
		rkPacket >> m_bSetSkillAimed;
		break;
		case FS_FIREEXPANSION_ATTACK:
		{
			rkPacket >> vPos;
			FireExpansionAttack( pOwner, vPos );
			break;
		}
	}
}

void ioNuClearSoldierItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	bool bAimJumpState;
	rkPacket >> bAimJumpState;	

	if( bAimJumpState )
	{
		int iAimState;
		rkPacket >> iAimState;

		bool bFullTime = false;

		switch( iAimState )
		{
		case AJS_READY:
			SetAimJumpReady( pOwner );
			break;
		case AJS_JUMPPING:
			rkPacket >> bFullTime;
			SetAimJumpping( pOwner, bFullTime );
			break;
		case AJS_CANCEL:
			SetAimJumpCancel( pOwner );
			break;
		}

		return;
	}

	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	bool bMissFire = false;

	D3DXVECTOR3 vPos = D3DXVECTOR3( 0.f, 0.f, 0.f);


	switch( iFireState )
	{
		case FS_NORMAL_ATTACK:
		{
			ChangeToNormalAttack( pOwner );
			break;
		}
		case FS_GATHERING:
		{
			pOwner->EndEffect( m_GatherEffect );
			ChangeToGathering( pOwner );
			break;
		}
	case FS_CHARGING:
		pOwner->NormalAttackOver();
		break;
	case FS_AIM_WAIT:
		rkPacket >> m_AimedTarget;
		rkPacket >> m_bAimedBall;
		m_dwAimStartTime = FRAMEGETTIME();
		break;
	case FS_AIMING:
		// 1. FS_AIM_WAIT 중에 Lost Target
		// 2. FS_AIMED 중에 Lost Target
		m_dwTrackingEndTime = FRAMEGETTIME();
		
		// 대상을 제거한후 조준대기로 넘겨야함
		m_AimedTarget.Clear();
		m_bAimedBall = false;

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		ChangeToAiming( pOwner );
		break;
	case FS_NORMAL_FIRE:
		// 1. FS_AIMING 중에 Key off
		// 2. FS_AIM_WAIT 중에 key off		
		ChangeToNormalFire( pOwner, vPos );
		break;
	case FS_NONE:
		rkPacket >> bMissFire;

		m_FireState = FS_NONE;
		pOwner->EndEffect( m_GatherEffect );
		m_AimedTarget.Clear();
		m_bAimedBall = false;

		if( pOwner->GetState() != CS_JUMP )
		{
			if( !bMissFire )
				pOwner->SetState( CS_DELAY );
			else
				pOwner->SetMissFireState( m_MissFire, m_fMissFireRate );
		}
		break;
	case FS_CHARGE_ATTACK_DELAY:
		rkPacket >> m_vCreatePos;
		m_FireState = FS_CHARGE_ATTACK_DELAY;
		ChangeToChargeAttack( pOwner );
		break;
	case FS_CHARGE_ATTACK:
		CreateArea( pOwner );
		break;
	case FS_SPECIAL_DASH:
		{			
			D3DXVECTOR3	vPosition;
			rkPacket >> vPosition;
			pOwner->SetWorldPosition( vPosition );

			int	iCurDir, iCallerType;
			rkPacket >> iCurDir;
			rkPacket >> iCallerType;

			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iCurDir, iCallerType );
		}
		break;
	}
}

void ioNuClearSoldierItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );

	if( bJumpAttack )
	{
		if( m_bAimedState )
		{
			D3DXVECTOR3 vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
		}
		m_bSetAimedDir = false;

		if( pMissile && m_vSecondAttributeList.size() > 0 )
			pMissile->SetSecondAttribute( m_vSecondAttributeList );
	}
	else
	{
		if( m_FireState == FS_AIMED_FIRE && m_bSetAimedDir )
		{
			D3DXVECTOR3 vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
			m_bSetAimedDir = false;
		}

		if( m_bZeroAimState )
		{
			if( pMissile && m_vSecondZeroAttributeList.size() > 0 )
				pMissile->SetSecondAttribute( m_vSecondZeroAttributeList );
		}
		else
		{
			if( pMissile && m_vSecondAttributeList.size() > 0 )
				pMissile->SetSecondAttribute( m_vSecondAttributeList );
		}
	}
}

ioWeaponItem::WeaponSubType ioNuClearSoldierItem::GetSubType() const
{
	return WST_NUCLEARSOLDIER_ITEM;
}

void ioNuClearSoldierItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_FIRE_ITEM,
							 m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker,"" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );

	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioNuClearSoldierItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );

	m_fFireTimeModifyRate = FLOAT1;

	ioWeaponItem::OnReleased( pOwner );
}

bool ioNuClearSoldierItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	if( bNormalAttack )
		return true;

	if( iFireCnt > 0 )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		DWORD dwGapTime = dwCurTime - m_dwCheckCoolTime;

		DWORD dwCurCoolTime = m_dwCurCoolTime + dwGapTime;
		dwCurCoolTime = max( 0, min( dwCurCoolTime, m_dwMaxCoolTime ) );

		if( dwCurCoolTime < m_dwNeedCoolTime )
		{
			return false;
		}
	}

	return true;
}

void ioNuClearSoldierItem::ModifyFireTimeRate( float fRate )
{
	m_fFireTimeModifyRate *= fRate;
}

void ioNuClearSoldierItem::SetAimedSkillUse( bool bSkillUsed )
{
	m_bSetSkillAimed = bSkillUsed;
}

void ioNuClearSoldierItem::SetAimedDirSet( bool bAimedSet )
{
	m_bSetAimedDir = bAimedSet;
}

D3DXVECTOR3 ioNuClearSoldierItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
	{
		return m_vAimedDir;
	}

	ioBall *pBall = pOwner->FindBall();
	if( !pTarget && !pBall )
	{
		return m_vAimedDir;
	}

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

	if( pTarget )
	{
		vTargetPos = pTarget->GetMidPositionByRate( FLOAT08);
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetMidPositionByRate();
	}

	D3DXVECTOR3 vAimedDir = vTargetPos - pWeapon->GetPosition();
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	
	vAimedDir.x = m_vAimedDir.x;
	vAimedDir.z = m_vAimedDir.z;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

void ioNuClearSoldierItem::SetFireStartTime( DWORD dwFireTime )
{
	m_dwFireStartTime = dwFireTime;
}

bool ioNuClearSoldierItem::CheckAimState(bool bAimMove)
{
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
		return true;
	}

	return false;
}

bool ioNuClearSoldierItem::SetAimState( ioBaseChar *pChar )
{
	CharState cstate = pChar->GetState();
	bool bCheckAimState = CheckAimState();
	if( cstate == CS_JUMP && bCheckAimState )
	{
		m_dwCheckAimCancelTime = 0;
		pChar->GetGroup()->ClearAllActionAni( FLOAT100, true );
		pChar->SetState( CS_ATTACK );

		if( m_FireState == FS_AIMED || m_FireState == FS_AIMING )
		{
			m_dwZeroAimStartTime = FRAMEGETTIME();
		}
	}
	else
	{
		ChangeToAiming( pChar );

		bool bIsJumpState = !pChar->IsJumpState();
		if( bIsJumpState )
		{
			pChar->SetState( CS_ATTACK );
		}
	}

	pChar->SetChargingState( true );
	m_bAimJumpAttack = false;

	return true;
}

void ioNuClearSoldierItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimedState = false;

	m_bZeroAimState = false;
	m_dwZeroAimStartTime = 0;

	//m_FireState = FS_NONE;
	m_bJumpChargeAttack = false;
}

bool ioNuClearSoldierItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;

	// D~ 2020-02-24
	DWORD dwCurTime = FRAMEGETTIME();
	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	DWORD dwJumpChargeTime = m_dwChargingStartTime+m_dwJumpChargeTime;
	//if( (m_FireState == FS_AIMED) && ( dwJumpChargeTime < dwCurTime ) )
	//if( m_FireState == FS_AIMED )
	if( dwJumpChargeTime < dwCurTime  )
	{
		ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
		ioAimJump *pAimJump = ToAimJump( pItem->GetCurExtendJump() );
		if( pAimJump && pAimJump->GetEnableJumpChargeAttack() )
		{
			pAttr = pAimJump->GetJumpChargeAttack();
		}		
	}

	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioNuClearSoldierItem::SetJumpAttack - AttackAttribute Not Exist" );		
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioNuClearSoldierItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	m_szCurAni = pAttr->m_AttackAnimation;
	m_dwInputCancelCheckTime = FRAMEGETTIME();

	fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_JUMP_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate + 30;
	m_dwFireStateEndTime += FRAMEGETTIME();

	m_bAimJumpAttack = true;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	// 에니메이션 처리
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	DecreaseCoolTime();

	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	if( m_FireState == FS_AIMED )
	{
		float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
		if( pAttr->m_AttackEndAnimation.IsEmpty() )
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
			}
		}
		else
		{
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}

			float fEndAniRate = pAttr->m_fAttackEndAniRate;
			pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
								0.0f, FLOAT100,
								1.0f, FLOAT1/fEndAniRate,
								m_dwFireEndTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
		}
	}
	else
	{
		if( pAttr->m_AttackEndAnimation.IsEmpty() )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		}
		else
		{
			pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

			float fEndAniRate = pAttr->m_fAttackEndAniRate;
			pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
								0.0f, FLOAT100,
								1.0f, FLOAT1/fEndAniRate,
								m_dwFireEndTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
		}
	}

	if( m_FireState == FS_AIMED )
	{
		m_FireState = FS_AIMED_FIRE;
	}
	else
	{
		m_FireState = FS_NORMAL_FIRE;
	}

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pOwner->EndEffect( m_ZeroAimedEffect );

	return true;
}

void ioNuClearSoldierItem::CheckJumpAttack( ioBaseChar *pOwner )
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
			
			iter++;
		}

		if( dwCurAirJumpTime > 0 && dwCurAirJumpTime < FRAMEGETTIME() )
		{
			if( m_fAirJumpPower >= 0.0f )
			{
				D3DXQUATERNION qtTargetRot = pOwner->GetTargetRot();
				pOwner->SetAirJumpPower( m_fAirJumpPower*m_fAirJumpPowerRate );
				D3DXVECTOR3 vAttDir =  qtTargetRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
				if( m_fAirForcePower < 0.0f )
				{
					pOwner->SetForcePower( -vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
				}
				else
				{
					pOwner->SetForcePower( vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
				}
			}
			m_iCurAirJump++;
		}
	}

	UpdateAimDir( pOwner );
}

bool ioNuClearSoldierItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	CharState cstate = pOwner->GetState();
	if( cstate == CS_ATTACK )
	{
		//if( m_FireState == FS_CHARGING || m_FireState == FS_AIMING || m_FireState == FS_AIM_WAIT ||	m_FireState == FS_AIMED )
		if( m_FireState == FS_GATHERING || m_FireState == FS_CHARGING )
		{
			return true;
		}
	}

	return false;
}

void ioNuClearSoldierItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_iCurBullet = 0;
	pOwner->EndEffect( m_ZeroAimedEffect );
	pOwner->EndEffect( m_GatherEffect );
}

bool ioNuClearSoldierItem::SetAimJumpState( ioBaseChar *pChar )
{
	CharState cstate = pChar->GetState();
	if( cstate == CS_JUMP )	// 점프중일때는 무조건 CS_JUMP 
	{
		return true;
	}	

	JumpType Jumptype		= GetJumpType();
	bool bIsJumpKeyPress	= pChar->IsJumpKeyPress();
	bool bIsCanJumpState	= pChar->IsCanJumpState();
	bool bIsBuffLimitJump	= !pChar->IsBuffLimitJump();
	if( Jumptype != JT_AIM_JUMP && bIsJumpKeyPress && bIsCanJumpState && bIsBuffLimitJump )
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pChar->SetJumpState();
		m_FireState = FS_NONE;
		g_TutorialMgr.ActionAimingCancel();
		return false;
	}

	if( pChar->IsAttackKeyDown() )
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		ChangeToGathering( pChar );	
		return false;
	}

	return true;
}

void ioNuClearSoldierItem::SetAimJumpReady( ioBaseChar *pChar )
{
	m_dwCheckAimCancelTime = FRAMEGETTIME();
	m_dwCheckAimCancelTime += GetJumpChargeTime( pChar );

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_AimJumpReadyAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( pChar->GetJumpReadyAnimationIdx(), FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );
	}
	else
	{
		pGrp->SetActionAni( m_AimJumpReadyAnimation, FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );
	}

	m_AimJumpState = AJS_READY;

	bool bIsNeedSendNetwork = pChar->IsNeedSendNetwork();
	if( bIsNeedSendNetwork )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << true;		// AimJumpState
		kPacket << (int)m_AimJumpState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNuClearSoldierItem::SetAimJumpping( ioBaseChar *pChar, bool bFullTime )
{
	m_dwCheckAimCancelTime = 0;

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_JumpAimCenter, FLOAT100, FLOAT1 );
	pGrp->SetLoopAni( m_JumpAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_JumpAimDown, 0.0f, 0.0f );

	if( m_bZeroAimState )
	{
		switch( m_FireState )
		{
		case FS_AIMING:
		case FS_AIM_WAIT:
			pChar->SetTargetMarkerState( ioTargetMarker::MS_READY );
			break;
		case FS_AIMED:
			// 발사가능 여부를 먼저 설정해 둘 것
			pChar->SetTargetMarkerEnable( IsCanFire( pChar, 1 ) );
			pChar->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			break;
		}

		SetZeroAim( pChar, false );
	}
	pChar->SetAimJumpState( bFullTime );

	m_AimJumpState = AJS_JUMPPING;
	m_dwTrackingEndTime = 0;
	bool bIsNeedSendNetwork = pChar->IsNeedSendNetwork();
	if( bIsNeedSendNetwork )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << true;		// AimJumpState
		kPacket << (int)m_AimJumpState;
		kPacket << bFullTime;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNuClearSoldierItem::SetAimJumpCancel( ioBaseChar *pChar )
{
	m_dwCheckAimCancelTime = 0;
	m_FireState = FS_NONE;

	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pChar->SetState( CS_DELAY );
	pChar->EndEffect( m_ZeroAimedEffect );

	m_bSetAimedDir = false;
	m_bAimedState = false;
	m_AimJumpState = AJS_CANCEL;

	bool bIsNeedSendNetwork = pChar->IsNeedSendNetwork();
	if( bIsNeedSendNetwork )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << true;		// AimJumpState
		kPacket << (int)m_AimJumpState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNuClearSoldierItem::UpdateAimDir( ioBaseChar *pOwner )
{
	bool bIsJumpState = !pOwner->IsJumpState();
	if( bIsJumpState )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bStateCheck = false;
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
	case FS_AIMED_FIRE:
		bStateCheck = true;
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !bStateCheck )
	{
		m_fCurAngle = 0.0f;
		pBiped->ReleaseExtraRotation();
		return;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}

	float fRotAngle = 0.0f;

	if( m_bAimJumpAttack )
	{
		if( m_dwFireStateEndTime < dwCurTime )
		{
			float fTotal, fGap;
			DWORD dwEndTime = m_dwFireStartTime + m_dwFireEndTime;
			fTotal = fGap = 0.0f;

			if( dwEndTime > m_dwFireStateEndTime )
			{
				fTotal = (float)(dwEndTime - m_dwFireStateEndTime);
			}

			if( dwEndTime > dwCurTime )
			{
				fGap = (float)(dwEndTime - dwCurTime);
			}

			float fRate = 0.0f;
			if( fTotal > 0.0f )
			{
				fRate = fGap / fTotal;
			}

			fRotAngle = m_fCurAngle * fRate;
		}
		else
		{
			fRotAngle = m_fCurAngle;
		}
	}
	else
	{
		bool bTarget = true;
		if( pTarget && !pTarget->IsEnableAimTarget(m_bEnableDownAim) )
		{
			bTarget = false;
		}
		else if( m_bAimedBall && !pBall )
		{
			bTarget = false;
		}
		else if( !pTarget && !m_bAimedBall )
		{
			bTarget = false;
		}

		if( !bTarget )
		{
			float fTotal, fGap;
			DWORD dwEndTime = m_dwTrackingEndTime + 500;
			fTotal = fGap = 0.0f;

			fTotal = FLOAT500;
			if( dwEndTime > dwCurTime )
			{
				fGap = (float)(dwEndTime - dwCurTime);
			}
			else
			{
				m_dwTrackingEndTime = 0;
			}

			if( m_dwTrackingEndTime == 0 )
			{
				m_fCurAngle = 0.0f;
				pBiped->ReleaseExtraRotation();
				return;
			}

			float fRate = 0.0f;
			if( fTotal > 0.0f )
			{
				fRate = fGap / fTotal;
			}

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

			float fAimTimeRate = FLOAT1;
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

const ioHashString& ioNuClearSoldierItem::GetJumppingAnimation() const
{
	bool bAim = false;
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
	case FS_AIMED_FIRE:
		bAim = true;
	}

	if( bAim && !m_AimJumppingAnimation.IsEmpty() )
		return m_AimJumppingAnimation;

	return ioAttackableItem::GetJumppingAnimation();
}

void ioNuClearSoldierItem::CheckAimJumpState( ioBaseChar *pChar )
{
	switch( m_FireState )
	{
	case FS_AIMING:
		OnAiming( pChar );
		break;
	case FS_AIM_WAIT:
		OnAimWait( pChar );
		break;
	case FS_AIMED:
		OnAimed( pChar );
		break;
	case FS_NORMAL_FIRE:
	case FS_AIMED_FIRE:
		OnJumpFire( pChar );
		break;
	}

	if( m_FireState == FS_AIM_WAIT || m_FireState == FS_AIMING || m_FireState == FS_AIMED )
	{
		bool bIsNeedProcess = pChar->IsNeedProcess();
		bool bIsDefenseKeyDown = pChar->IsDefenseKeyDown();
		if( bIsNeedProcess && bIsDefenseKeyDown )
		{
			if(IsCanFire(0))
			{
				bool bAimedDir = m_bSetAimedDir;
				ioHashString szTarget = m_AimedTarget;

				int iSkillNum = pChar->GetSkillKeyInput();
				if(iSkillNum == 0 && pChar->CheckJumpingSkill( 0 ) )
				{
					pChar->SetChargingState( false );
					
					m_bSetSkillAimed = true;
					m_bSetAimedDir = bAimedDir;
					m_AimedTarget = szTarget;

					m_FireState = FS_NONE;
					bool bIsNeedSendNetwork = pChar->IsNeedSendNetwork();
					if( bIsNeedSendNetwork )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pChar, kPacket );
						kPacket << (int)m_FireState;
						kPacket << m_AimedTarget;
						kPacket << m_bAimedBall;
						kPacket << m_iCurCombo;
						kPacket << m_vAimedDir;
						kPacket << m_bSetAimedDir;
						kPacket << m_bSetSkillAimed;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}							
					return;
				}
			}
		}
	}

	if( m_FireState != FS_AIMED && m_FireState != FS_AIMING &&
		m_FireState != FS_AIM_WAIT && m_dwZeroAimStartTime > 0 )
	{
		m_dwZeroAimStartTime = 0;
	}

	if( m_bZeroAimState )
	{
		pChar->EndEffect( m_ZeroAimedEffect );
	}

	UpdateAimDir( pChar );
}


bool ioNuClearSoldierItem::CheckLanding( ioBaseChar *pChar )
{
	bool bCheckAimState = CheckAimState();
	if( bCheckAimState )
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pChar->GetGroup()->ReleaseAllExtraRotate();
		SetNoneState( pChar );
		return true;
	}


	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return false;

	
	pGrp->ClearAllLoopAni( 0.0f );

	pGrp->SetLoopAni( m_AimCenter, 0.0f );
	pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );
	pChar->GetGroup()->ReleaseAllExtraRotate();

	m_bJumpChargeAttack = false;
	// 2020-02-25
	m_bEnableDashState = false;
	return false;
}

bool ioNuClearSoldierItem::CheckZeroAim( ioBaseChar *pOwner )
{
	if( m_bZeroAimState ) return false;

	if( m_dwZeroAimStartTime == 0 ) return false;

	if( m_dwZeroAimStartTime+m_dwZeroAimedCheckTime > FRAMEGETTIME() )
		return false;

	SetZeroAim( pOwner, true );

	return true;
}

void ioNuClearSoldierItem::SetZeroAim( ioBaseChar *pOwner, bool bSet )
{
	if( bSet )
	{
		m_bZeroAimState = true;
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
		pOwner->AttachEffect( m_ZeroAimedEffect );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pGrp->SetLoopAni( m_ZeroAimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_ZeroAimDown, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_ZeroAimCenter, 0.0f, FLOAT1 );
	}
	else
	{
		m_bZeroAimState = false;
		m_dwZeroAimStartTime = 0;
		pOwner->EndEffect( m_ZeroAimedEffect );
	}
}

float ioNuClearSoldierItem::GetAttackSpeedRate() const
{
	if( m_bZeroAimState && m_fZeroAimSpeedRate > 0.0f )
	{
		return m_fZeroAimSpeedRate;
	}
	else
	{
		return m_fAttackSpeedRate;
	}
}

bool ioNuClearSoldierItem::IsAimedState()
{
	if( m_FireState == FS_AIMED )
	{
		return true;
	}

	return false;
}

bool ioNuClearSoldierItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	ioBaseChar *pChar = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCharDir  = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle ) * FLOAT05 );

	float fLimitDistSq = m_fAimRange * m_fAimRange;
	float fMinDistSq = m_fMinRange * m_fMinRange;

	float fBallDist = std::numeric_limits<float>::infinity();
	float fCharDist = std::numeric_limits<float>::infinity();

	// Ball
	if( pBall )
	{
		vDiff = pBall->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
			{
				return true;
			}
		}
	}

	return false;
}

bool ioNuClearSoldierItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_FireState )
		{
			case FS_NORMAL_ATTACK:
			case FS_NORMAL_FIRE:
			{
				if( !bCheck )
					return	false;

				return	true;
			}
		}
	}

	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
		return true;
	}

	return false;
}

void ioNuClearSoldierItem::InitCoolTime()
{
	m_dwCurCoolTime = 0;
	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioNuClearSoldierItem::MaxCoolTime()
{
	m_dwCurCoolTime = m_dwMaxCoolTime;
	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioNuClearSoldierItem::DecreaseCoolTime()
{
	if( m_dwCurCoolTime < m_dwNeedCoolTime )
	{
		m_dwCurCoolTime = 0;
	}
	else
	{
		m_dwCurCoolTime -= m_dwNeedCoolTime;
	}
}


float ioNuClearSoldierItem::GetCircleRadius()
{
	return m_fMagicCircleRadius;
}

void ioNuClearSoldierItem::OnGathering( ioBaseChar *pOwner )
{
	float fCurRange = m_fRange;
	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pOwner->GetCharName()) );
	if( pUpInfo )
	{
		ioPlayStage *pStage = m_pCreator->GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	fCurRange += fValue;

	// 매직 서클
	if( pOwner->CheckMagicCircle() )
	{
		pOwner->CheckChangeDirectionForMagicCircle();
		pOwner->UpdateMagicCirclePosAll( m_fMoveSpeed, fCurRange, m_fHeightGap, false );

		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );
	}

	bool bIsNeedProcess = !pOwner->IsNeedProcess();
	if( !pOwner || bIsNeedProcess )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	bool bIsAttackKeyDown = pOwner->IsAttackKeyDown();
	if( bIsAttackKeyDown )
	{
		DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;
		float fCurChargeRate = 0.0f;
		DWORD dwMaxChargeTime = m_dwMaxChargeTime;
		if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
			{
				fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			}
			else
			{
				fCurChargeRate = FLOAT1;
			}
		}		

		m_iCurBullet = m_iMaxBullet * fCurChargeRate;
	}

	if( !bIsAttackKeyDown )	// 버튼 뗌
	{
		// D~ 떼기 부분별 검사	
		if( (m_vGaugeInfoList[0].m_nMin >= m_iCurBullet) || (m_vGaugeInfoList[0].m_nMax >= m_iCurBullet) )
		{
			//	D~ 떼기 시 레이저 발사히 않음
			m_iCurCombo = 0;
			ChangeToNormalFire( pOwner );
		}
		else if( (m_vGaugeInfoList[1].m_nMin >= m_iCurBullet) || (m_vGaugeInfoList[1].m_nMax >= m_iCurBullet) )
		{
			//  D~ 떼기 시 일반 레이저 발사
			m_iCurCombo = 1;
			ChangeToNormalFire( pOwner );
		}
		else if( (m_vGaugeInfoList[2].m_nMin >= m_iCurBullet) || (m_vGaugeInfoList[2].m_nMax >= m_iCurBullet) )
		{
			//  D~ 떼기 시 강경직 레이저 발사
			m_iCurCombo = 2;
			ChangeToNormalFire( pOwner );
		}
		else if( (m_vGaugeInfoList[3].m_nMin >= m_iCurBullet) || (m_vGaugeInfoList[3].m_nMax >= m_iCurBullet) )
		{
			//  D~ 떼기 시 매직서클을 향해 핵미사일 투하
			m_iCurCombo = 3;
			FireExpansionAttack( pOwner, D3DXVECTOR3( 0.f, 0.f, 0.f) );
		}
	}

	//float fCurRange = m_fRange;
	//float fValue = 0.0f;
	//ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pOwner->GetCharName()) );
	//if( pUpInfo )
	//{
	//	ioPlayStage *pStage = m_pCreator->GetPlayStage();
	//	fValue = pUpInfo->GetValue(pStage);
	//}

	//fCurRange += fValue;

	//if( pOwner->IsDefenseKeyDown() )
	//{
	//	if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
	//	{
	//		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, fCurRange, m_fHeightGap, true, true );
	//	}

	//	int iSkillNum = pOwner->GetSkillKeyInput();
	//	if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL) )
	//	{
	//		m_FireState = FS_NONE;
	//		pOwner->EndEffect( m_GatherEffect );
	//		return;
	//	}
	//}

	////Gather Gauge
	//if( pOwner->CheckMagicCircle() )
	//{
	//	if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
	//	{
	//		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, fCurRange, m_fHeightGap, true );
	//	}
	//	else
	//	{
	//		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, fCurRange, m_fHeightGap, false );
	//	}

	//	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	//	D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
	//	vCharDir.y = 0.0f;

	//	D3DXVec3Normalize( &vCharDir, &vCharDir );

	//	pOwner->SetTargetRotToDir( vCharDir, true );
	//}
}
void ioNuClearSoldierItem::FireExpansionAttack( ioBaseChar *pOwner, D3DXVECTOR3 vPos )
{
	int iExtendAttackCnt = m_vExtendAttributeList.size();
	if( iExtendAttackCnt <= m_iCurCombo )
		return;

	CharState cstate = pOwner->GetState();
	if( cstate == CS_JUMP )
	{
		return;
	}	

	const AttackAttribute *pAttr = NULL;
	if( m_bZeroAimState )
	{
		pAttr = &m_ExtendZeroAimed;
	}
	else
	{
		pAttr = &m_vExtendAttributeList[m_iCurCombo];
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate   = pAttr->m_fAttackAniRate * m_fFireTimeModifyRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	DecreaseCoolTime();

	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );


	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_szCurAni = pAttr->m_AttackAnimation;
	m_dwInputCancelCheckTime = FRAMEGETTIME();

	if( iExtendAttackCnt <= m_iCurCombo + 1 )
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	}
	else
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );
	
	if( pOwner->IsNeedProcess() )
	{
		vPos = pOwner->GetMagicCirclePos();
	}

	ioMapEffect *pMapEffect = NULL;
	if( pOwner->GetTeam() == TEAM_RED )
	{
		pMapEffect = pOwner->CreateMapEffectBySkill( m_stExpansionAttackEffect_red, vPos );
	}
	else if( pOwner->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pOwner->CreateMapEffectBySkill( m_stExpansionAttackEffect_blue, vPos );
	}
	else
		pMapEffect = pOwner->CreateMapEffectBySkill( m_stExpansionAttackEffect_red, vPos );

	bool bIsNeedProcess = pOwner->IsNeedProcess();
	if( bIsNeedProcess )
	{
		vPos.y += m_fExpansionAttackHeightOffset;
	}

	DWORD dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	ioWeapon* pWeapon = pOwner->DummyExplicit( pAttr->m_vWeaponInfoList[0], vPos, dwBaseWeaponIndex, false, pOwner->GetCharName(), "", true );
	if( pWeapon )
	{
		pWeapon->SetPosition( vPos );
		pWeapon->SetMoveDir( D3DXVECTOR3(0.f, -FLOAT1, 0.f) );
	}

	m_FireState = FS_NORMAL_FIRE;
	bool bIsNeedSendNetwork = pOwner->IsNeedSendNetwork();
	if( bIsNeedSendNetwork )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<int>( FS_FIREEXPANSION_ATTACK );
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		kPacket << m_iCurCombo;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	g_TutorialMgr.ActionNormalFire();

	m_iCurCombo = 0;
}

void ioNuClearSoldierItem::ChangeToChargeAttack( ioBaseChar *pOwner )
{
	ioMapEffect *pMapEffect = NULL;
	if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
		pMapEffect = pOwner->CreateMapEffectBySkill( m_ChargeAttackBlueCircle, m_vCreatePos );
	else
		pMapEffect = pOwner->CreateMapEffectBySkill( m_ChargeAttackRedCircle, m_vCreatePos );

	if( pMapEffect )
		m_dwCurMapEffect = pMapEffect->GetUniqueID();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_ChargeAttackDelay, FLOAT100 );

	m_dwChargeAttackPreDelayStartTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << m_vCreatePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNuClearSoldierItem::ChangeToSpecialDelayState( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_SpecialAttackDelay );
	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	pOwner->SetState( CS_NUCLEARSOLDIER_SPECIAL );
	m_FireState = FS_SPECIAL_ATTACK_DELAY;
	m_dwSpecialStartTime = FRAMEGETTIME();
	m_iCurSpecialWeaponCnt = 0;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	AddSpecialAttackPosition( pOwner, pStage );
	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCirclePos = pOwner->GetMagicCirclePos();
	D3DXVECTOR3 vDir = vCirclePos - vCharPos;
	D3DXVec3Normalize( &vDir, &vDir );
					
	D3DXVECTOR3 vTargetPos = vCirclePos + vDir * m_fCircleOffSet;
	pOwner->SetMagicCirclePos(false, vTargetPos);

	if(pOwner->IsNeedProcess() )
	{
		//Send
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNuClearSoldierItem::AddSpecialAttackPosition( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;

	D3DXVECTOR3 vCirclePos;
	if( m_iCurSpecialWeaponCnt < m_iMaxSpecialWeaponCnt )
	{
		vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, false );
		m_vSpecialWeaponPosList.push_back( vCirclePos );
	}
	
	m_iCurSpecialWeaponCnt++;

	if( !m_ReadyCircle.IsEmpty() )
	{	
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_ReadyCircle, vCirclePos, vScale );
		if( pMapEffect )
		{
			DWORD dwID = pMapEffect->GetUniqueID();
			m_dwEnableCircleIDList.push_back(dwID);
		}
	}
}

void ioNuClearSoldierItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_FireState )
	{
		case FS_SPECIAL_DASH:
		{
			ProcessSpecialDash( pOwner );
		}
		break;

	case FS_SPECIAL_ATTACK_DELAY:
		{
			int nSize = m_vSpecialWeaponPosList.size();
			float fCurRange = 0.f;
			if( m_vSpecialWeaponPosList.size() < m_vSpecialWeaponOffset.size() )
				fCurRange = m_vSpecialWeaponOffset[ nSize ];

			int iSkillNum = pOwner->GetSkillKeyInput();
			if( iSkillNum == 0 )
			{
				if( m_vSpecialWeaponPosList.size() > 1 )
					ChangeToSpecialAttackState( pOwner );
				else
					ChangeToSpecialEndState( pOwner );
			}

			// 뱡향 변화 체크
			pOwner->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
			if( m_vSpecialWeaponPosList.size() != 0 )
			{
				pOwner->UpdateMagicCircleToCirclePosAll( m_fMoveSpeed, fCurRange, m_fCircleOffSet, 0, 0, 0, 
					m_vSpecialWeaponPosList[ m_vSpecialWeaponPosList.size() - 1 ] , false, false );
			}
			else
			{
				pOwner->UpdateMagicCircleToCirclePosAll( m_fMoveSpeed, fCurRange, m_fCircleOffSet, 0, 0, 0, vCharPos, false, false );
			}

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pOwner->GetMagicCirclePos();
			pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );

			if( pOwner->IsNeedProcess() )
			{
				bool bAddPosition = false;
				if( pOwner->IsAttackKey() )
				{					
					bAddPosition = true;
					AddSpecialAttackPosition( pOwner, pStage );
				}

				if( m_iCurSpecialWeaponCnt >= m_iMaxSpecialWeaponCnt )
				{
					ChangeToSpecialAttackState( pOwner );
				}
				else if( bAddPosition )
				{
					//아직 셋팅할 구역이 있다면 Circle을 offset만큼 이동시켜 준다.
					D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
					D3DXVECTOR3 vDir = vTargetPos - vCharPos;
					D3DXVec3Normalize( &vDir, &vDir );
					
					vTargetPos = vTargetPos + vDir * m_fCircleOffSet;
					pOwner->SetMagicCirclePos(false, vTargetPos);
				}
			}
		}
		break;
	case FS_SPECIAL_ATTACK:
		if( m_dwSpecialEndTime < dwCurTime )
		{
			ChangeToSpecialEndState( pOwner );
		}
		else if( m_dwSpecialFireTime > 0 && m_dwSpecialFireTime < FRAMEGETTIME() )
		{
			CreateWeapon( pOwner );

			m_dwSpecialFireTime = 0;
		}
		break;
	}
}

void ioNuClearSoldierItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_dwSpecialStartTime = 0;
	m_dwSpecialEndTime = 0;
	m_dwSpecialFireTime = 0;
	m_vSpecialWeaponPosList.clear();
	m_iCurSpecialWeaponCnt = 0;

	pOwner->DestroyMagicCircle();

	if( !m_dwEnableCircleIDList.empty() )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			int iCnt = m_dwEnableCircleIDList.size();
			for( int i=0; i<iCnt; ++i)
			{
				pStage->DestroyMapEffect( m_dwEnableCircleIDList[i] );
			}
			m_dwEnableCircleIDList.clear();
		}
	}
}

void ioNuClearSoldierItem::ChangeToSpecialEndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ClearAttackState( pOwner );
	//pOwner->SetSKillEndJumpState( 0.1f, false, false, false );
	pOwner->SetSKillEndJumpState( m_fDashEndJumpPower, false, false, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_SPECIAL_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNuClearSoldierItem::CreateWeapon( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_vSpecialWeaponPosList.empty() )	return;

	ioPlayStage *pStage = pChar->GetCreator();

	int iCnt = m_vSpecialWeaponPosList.size();
	D3DXQUATERNION qtRot;
	int iWeaponIndex = pChar->IncreaseWeaponIndexBase();
	
	for( int i=1; i<iCnt; ++i )
	{
		//서로 점과의 방향을 구해서 돌리자
		D3DXVECTOR3 vMoveDir = m_vSpecialWeaponPosList[i] - m_vSpecialWeaponPosList[i-1];
		ioMath::CalcDirectionQuaternion( qtRot, -vMoveDir );
		float fLength = fabs( D3DXVec3Length( &vMoveDir ) );
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.dwDuration = 1;
		kFireTime.iAttributeIdx = m_dwSpecialAttackAttribute;
		kFireTime.dwWeaponIdx = iWeaponIndex++;
		ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, m_vSpecialWeaponPosList[i-1], vMoveDir, "" );
	
		if( pWeapon == 0 )
		{
			LOG.PrintTimeAndLog(0, "ioMagicStaffItem-->NoWeapon");
			assert( 0 );
		}

		ioFlamewaveWeapon  *pFlamewaveWeapon = ToFlamewaveWeapon(pWeapon);
		if( pFlamewaveWeapon )
			pFlamewaveWeapon->SetCustomRange(true, fLength );
	}
	
	m_vSpecialWeaponPosList.clear();

	if( !m_dwEnableCircleIDList.empty() )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			int iCnt = m_dwEnableCircleIDList.size();
			for( int i=0; i<iCnt; ++i)
			{
				pStage->DestroyMapEffect( m_dwEnableCircleIDList[i] );

			}
			m_dwEnableCircleIDList.clear();
		}
	}
}

bool ioNuClearSoldierItem::IsEnableGauge()
{
	if( m_iMaxBullet != 0.f && m_iCurBullet >= 0 )
		return true;

	return false;

}

void ioNuClearSoldierItem::ChangeToSpecialAttackState( ioBaseChar *pOwner )
{
	m_FireState = FS_SPECIAL_ATTACK;

	if( pOwner->CheckMagicCircle() )
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, true, true );
	
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SpecialAttack );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSpecialAttackAniRate );
	
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fSpecialAttackAniRate;
	m_dwSpecialEndTime = dwCurTime + dwDuration;
	m_dwSpecialFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * m_fSpecialAttackAniRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		int iSkillPointSize = m_vSpecialWeaponPosList.size();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << iSkillPointSize;
		for( int i = 0; i < iSkillPointSize; i++ )//중간 참여자를 위해서 결과는 다시 처음부터 보낸다.
			kPacket << m_vSpecialWeaponPosList[i];

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNuClearSoldierItem::ChangeToGathering( ioBaseChar *pOwner )
{
	pOwner->StopAutoTargetTracking();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );
	pOwner->AttachEffect( m_GatherEffect );

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );

	m_FireState = FS_GATHERING;

	m_dwGatheringStartTime = FRAMEGETTIME();
	m_dwNextExpansionFireTime = m_dwGatheringStartTime + m_dwExpansionAttackTicFirst;
	m_dwExpansionFireMotionEndTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;	//AimJump
		kPacket << static_cast<BYTE>( m_FireState );
		kPacket << m_iCurCombo;
		kPacket << false;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNuClearSoldierItem::CreateArea( ioBaseChar *pOwner )
{
	if( m_AreaWeaponName.IsEmpty() ) return;
	if( !pOwner ) return;

	g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", GetName(), m_AreaWeaponName, 
									  m_vCreatePos, pOwner->GetTargetRot(), ioAreaWeapon::CT_NORMAL );
}

void ioNuClearSoldierItem::CheckDecreaseGauge()
{
	m_iCurBullet -= 0;
	m_iCurBullet = (float)max( 0, min( m_iCurBullet, m_iMaxBullet ) );
}

void ioNuClearSoldierItem::SetChargeAttackState( ioBaseChar *pChar )
{
	m_FireState = FS_CHARGE_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_ChargeAttackAni );
	if( iCurSkillAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fChargeAttackAniRate );
	
	m_dwChargeAttackEndTime = dwCurTime;
	m_dwChargeAttackEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fChargeAttackAniRate;

	m_dwChargeAttackFireTime = dwCurTime;
	m_dwChargeAttackFireTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * m_fChargeAttackAniRate;
}

// 점프어택 
void ioNuClearSoldierItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	ioAttackableItem::ProcessJumpAttack( pOwner, fHeightGap );

	switch( m_FireState )
	{
		case FS_NORMAL_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserveByConditionalSpecialDash( pOwner );
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;

				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;
			}
		}
		break;
	}

	bool bIsDirKeyDoubleClick  = pOwner->IsDirKeyDoubleClick();
	if( bIsDirKeyDoubleClick && !m_bDashEndJumpState )
	{
		pOwner->SetUsedJumpAttack(m_bDashEndJumpState);
		ioAttackableItem::ProcessJumpping( pOwner, fHeightGap );
	}
}

bool ioNuClearSoldierItem::ProcessAimJump( ioBaseChar *pOwner, float fHeightGap )
{
//	return  ioAttackableItem::ProcessAimJump( pOwner, fHeightGap );
	return false;
}

// 2020-02-18
void ioNuClearSoldierItem::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap )
{
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	if( pItem )
	{
		ioAimJump *pAimJump = ToAimJump( pItem->GetCurExtendJump() );
		if( !pAimJump ) {	return;	}
		
		if( !m_bDashEndJumpState )
		{		
			ioAttackableItem::ProcessJumpping( pOwner, fHeightGap );
		}

		switch( m_FireState )
		{
			case FS_SPECIAL_DASH: // 2020-02-18
			{
				// 점프 대시 이후에 바닥에 오기전까지는 다시 할수없다.
				if( m_bDashEndJumpState ) {	return;	}
				pAimJump->JumpCancelDashAttack( pOwner );
				m_FireState	= FS_NONE;
			}
			break;
		}

		// 점프공격 대시 이후 한번더 점프		
		if( pAimJump && pAimJump->GetDashEndState() )
		{
			m_FireState	= FS_NONE;
			ClearKeyReserve();
			m_bDashEndJumpState = true;
			pOwner->SetSKillEndJumpState( m_fDashEndJumpPower, false, false, true, false );
			return;
		}
	}
}

void ioNuClearSoldierItem::ProcessLanding( ioBaseChar *pOwner )
{
	ioAttackableItem::ProcessLanding( pOwner );

	m_FireState = FS_NONE;
	m_bDashEndJumpState = false;	
}

bool ioNuClearSoldierItem::IsEnableJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !m_bEnableJumpDash )
		return false;

	if( pOwner->GetJumpState() == JS_JUMPPING )
	{
		return true;
	}

	else if( pOwner->GetJumpState() == JS_AIM_JUMP )
	{
		return true;
	}
	else if( pOwner->GetJumpState() == JS_JUMP_ATTACK )
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
ioNuClearSoldierSpecialState::ioNuClearSoldierSpecialState()
{
}

ioNuClearSoldierSpecialState::~ioNuClearSoldierSpecialState()
{
}

void ioNuClearSoldierSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNuClearSoldierSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioNuClearSoldierItem *pNuClearSoldier = ToNuClearSoldierItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNuClearSoldier )
		pNuClearSoldier->ClearAttackState( pOwner );
}

void ioNuClearSoldierSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNuClearSoldierItem *pNuClearSoldier = ToNuClearSoldierItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNuClearSoldier )
		pNuClearSoldier->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNuClearSoldierSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}