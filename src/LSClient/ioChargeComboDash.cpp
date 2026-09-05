#include "stdafx.h"

#include "ioChargeComboDash.h"
#include "ItemDefine.h"
#include "WeaponDefine.h"
#include "FindPredicateImpl.h"

ioChargeComboDash::ioChargeComboDash()
{
	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwRunRotateStartTime = 0;

	m_bSetCombo = false;
	m_bNormalAttacked = false;
	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;
	m_DashState = DS_NONE;

	m_dwCancelCheckTime = 0;
	m_dwInputCancelCheckTime = 0;

	m_fCurChargeRate = 0.0f;
	m_GatheringEffectID = -1;

	m_szInputEventAnimation.Clear();
	m_fInputEventAnimationRate = FLOAT1;
	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0;

	m_dwEnableKeyTime = 0;
	m_nCurChargeCombo = 0;

	m_dwChargeAttackDummyFireTime = 0;

	m_nCurReserveDash = 0;
	m_nHalfAttackCount = 0;
	m_bRotateFirstAttack = false;
}

ioChargeComboDash::ioChargeComboDash( const ioChargeComboDash &rhs )
: ioExtendDash( rhs ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_dwHalfChargingTime( rhs.m_dwHalfChargingTime ),
m_dwExtendChargingTime( rhs.m_dwExtendChargingTime ),
m_DashExtendType( rhs.m_DashExtendType ),
m_BuffAddAttribute( rhs.m_BuffAddAttribute ),
m_vBuffAddList( rhs.m_vBuffAddList ),
m_DashMechanicsFly( rhs.m_DashMechanicsFly ),
m_ExtendAttackList( rhs.m_ExtendAttackList ),
m_ExtendAttackList2( rhs.m_ExtendAttackList2 ),
m_ChargingAniList( rhs.m_ChargingAniList ),
m_ExtendChargingAniList( rhs.m_ExtendChargingAniList ),
m_fRunSpeed( rhs.m_fRunSpeed ),
m_vRunDurationVec( rhs.m_vRunDurationVec ),
m_dwRotateSpeed( rhs.m_dwRotateSpeed ),
m_iRunRotateAttackMaxCount( rhs.m_iRunRotateAttackMaxCount ),
m_RunRotateAttackList( rhs.m_RunRotateAttackList ),
m_RunRotateAttackEnd( rhs.m_RunRotateAttackEnd ),
m_dwRunRotateJumpTime( rhs.m_dwRunRotateJumpTime ),
m_dwRunRotateChargeTime( rhs.m_dwRunRotateChargeTime ),
m_bRotateChargeEnable( rhs.m_bRotateChargeEnable ),
m_PowerChargeAni( rhs.m_PowerChargeAni ),
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
m_PowerChargeAttack( rhs.m_PowerChargeAttack ),
m_bPowerChargeUseDefense( rhs.m_bPowerChargeUseDefense ),
m_DashGatheringEffect( rhs.m_DashGatheringEffect ),
m_DashLuciferFly( rhs.m_DashLuciferFly ),
m_bPlayDashExtendWoundAni( rhs.m_bPlayDashExtendWoundAni ),
m_DestroyerFly( rhs.m_DestroyerFly ),
m_DashMichaelFly( rhs.m_DashMichaelFly ),
m_DashGoomihoFly( rhs.m_DashGoomihoFly ),
m_DashDraculaFly( rhs.m_DashDraculaFly ),
m_EvyBlinkInfo( rhs.m_EvyBlinkInfo ),
m_DashSuccubusChain( rhs.m_DashSuccubusChain ),
m_DashMidnightDashInfo( rhs.m_DashMidnightDashInfo ),
m_FlyDashAttack( rhs.m_FlyDashAttack ),
m_FlyDashAttackEnd( rhs.m_FlyDashAttackEnd ),
m_dwFlyDashDuration( rhs.m_dwFlyDashDuration ),
m_fFlyDashSpeed( rhs.m_fFlyDashSpeed ),
m_fFlyDashDecreseGauge( rhs.m_fFlyDashDecreseGauge ),
m_FlyDashLandingAni( rhs.m_FlyDashLandingAni ),
m_fFlyDashLandingAniRate( rhs.m_fFlyDashLandingAniRate ),
m_SlasherRunInfo( rhs.m_SlasherRunInfo ),
m_vDummyMoveInfoCombo( rhs.m_vDummyMoveInfoCombo ),
m_TopBl_Normal( rhs.m_TopBl_Normal ),
m_TopBl_ChargeInfo( rhs.m_TopBl_ChargeInfo ),
m_TopBl_ChargeAttack( rhs.m_TopBl_ChargeAttack ),
m_dwChargeAttackDummyDelayTime( rhs.m_dwChargeAttackDummyDelayTime ),
m_KaelthasExtendAttribute( rhs.m_KaelthasExtendAttribute ),
m_LeeDragonExtendAttribute( rhs.m_LeeDragonExtendAttribute ),
m_stDrangonAttackBuff( rhs.m_stDrangonAttackBuff ),
m_stDrangonAttackCancelBuff( rhs.m_stDrangonAttackCancelBuff ),
m_bAniJumpInputS( rhs.m_bAniJumpInputS ),
m_DragonNestAssassinPreAttribute( rhs.m_DragonNestAssassinPreAttribute ),
m_DragonNestAssassinTeleportAttackAttribute( rhs.m_DragonNestAssassinTeleportAttackAttribute ),
m_vDragonNestAssassinTeleportOffSet( rhs.m_vDragonNestAssassinTeleportOffSet ),
m_fDragonNestAssassinExtendTeleportEndJump( rhs.m_fDragonNestAssassinExtendTeleportEndJump ),
m_fThunderBirdUseGauge( rhs.m_fThunderBirdUseGauge ),
m_bMaxDashLastChargeAttack( rhs.m_bMaxDashLastChargeAttack ),
m_bExceptionInputJumpSCheck( rhs.m_bExceptionInputJumpSCheck ),
m_bInitActionDelayTime_NormalAttack( rhs.m_bInitActionDelayTime_NormalAttack ),
m_szParkilpyoGatherAni( rhs.m_szParkilpyoGatherAni ),
m_fParkilpyoGatherAniRate( rhs.m_fParkilpyoGatherAniRate ),
m_ParkilpyoChargeAttack( rhs.m_ParkilpyoChargeAttack ),
m_ParkilpyoChargeComboList( rhs.m_ParkilpyoChargeComboList ),
m_fParkilpyoUseGauge( rhs.m_fParkilpyoUseGauge ),
m_bSetEnableReserveDash( rhs.m_bSetEnableReserveDash ),
m_JackChargeAttack( rhs.m_JackChargeAttack ),
m_fJackUseGauge( rhs.m_fJackUseGauge ),
m_nMaxReserveDash( rhs.m_nMaxReserveDash ),
m_fShadowRangerUseGauge( rhs.m_fShadowRangerUseGauge ),
m_bKeyInputLastAttack( rhs.m_bKeyInputLastAttack ),
m_AttackKeyAttribute( rhs.m_AttackKeyAttribute ),
m_DefenseKeyAttribute( rhs.m_DefenseKeyAttribute ),
m_JumpKeyAttribute( rhs.m_JumpKeyAttribute ),
m_JeonWooChiDashInfo( rhs.m_JeonWooChiDashInfo ),
m_CuchulainMiddleAttack( rhs.m_CuchulainMiddleAttack ),
m_CuchulainChargeAttack( rhs.m_CuchulainChargeAttack ),
m_CuchulainChargeComboAttack( rhs.m_CuchulainChargeComboAttack ),
m_CuchulainFlyInfo( rhs.m_CuchulainFlyInfo ),
m_OnimushaChargeAttack( rhs.m_OnimushaChargeAttack ),
m_fOnimushaChargeUseGauge( rhs.m_fOnimushaChargeUseGauge ),
m_fOnimushaObjectChargeUseGauge( rhs.m_fOnimushaObjectChargeUseGauge ),
m_HalfExtendAttack( rhs.m_HalfExtendAttack ),
m_bEnableHalfExtendAttack( rhs.m_bEnableHalfExtendAttack ),
m_bExtendDashInpuDashEnable( rhs.m_bExtendDashInpuDashEnable ),
m_fAzazelRange( rhs.m_fAzazelRange ),
m_fAzazelAngle( rhs.m_fAzazelAngle ),
m_vAzazelAttackOffset( rhs.m_vAzazelAttackOffset ),
m_NormalSAttribute( rhs.m_NormalSAttribute ),
m_NormalAAttribute( rhs.m_NormalAAttribute ),
m_ChargeSAttribute( rhs.m_ChargeSAttribute ),
m_ChargeAAttribute( rhs.m_ChargeAAttribute ),
m_fGhostExtendDashJumpRate( rhs.m_fGhostExtendDashJumpRate )
{
	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwRunRotateStartTime = 0;

	m_bSetCombo = false;
	m_bNormalAttacked = false;
	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;
	m_DashState = DS_NONE;

	m_dwCancelCheckTime = 0;
	m_dwInputCancelCheckTime = 0;

	m_fCurChargeRate = 0.0f;
	m_GatheringEffectID = -1;

	m_szInputEventAnimation.Clear();
	m_fInputEventAnimationRate = FLOAT1;
	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0;

	m_dwEnableKeyTime = 0;
	m_nCurChargeCombo = 0;

	m_nCurReserveDash = 0;
	m_nHalfAttackCount = 0;
	m_bRotateFirstAttack = false;
}

ioExtendDash* ioChargeComboDash::Clone()
{
	return new ioChargeComboDash( *this );
}

ioChargeComboDash::~ioChargeComboDash()
{
}

DashType ioChargeComboDash::GetType() const
{
	return DT_CHARGE_COMBO;
}

void ioChargeComboDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];

	m_dwChargingTime = rkLoader.LoadInt_e( "dash_charge_time", 0 );
	m_dwHalfChargingTime = rkLoader.LoadInt_e("dash_half_charge_time", 0 );
	m_dwExtendChargingTime = rkLoader.LoadInt_e( "dash_extend_charge_time", 0 );

	int iMaxChargeAni = rkLoader.LoadInt_e( "charging_dash_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "charging_dash_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ChargingAniList.push_back( szBuf );
	}
	iMaxChargeAni = rkLoader.LoadInt_e( "extend_charging_dash_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "extend_charging_dash_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ExtendChargingAniList.push_back( szBuf );
	}

	m_DashExtendType = (DashExtendType)rkLoader.LoadInt_e( "dash_extend_type", DET_NONE );

	// buff add
	LoadAttackAttribute_e( "dash_buff_add_attack", m_BuffAddAttribute, rkLoader );

	m_vBuffAddList.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "dash_buff_add_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "dash_buff_add%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuffName = szBuf;
		m_vBuffAddList.push_back( szBuffName );
	}

	// mechanics fly info
	LoadMechanicsFlyInfo( rkLoader );

	// 루시퍼
	if( m_DashExtendType == DET_LUCIFER_FLY )
		LoadLuciferFlyInfo( rkLoader );

	//싸이
	if( m_DashExtendType == DET_DESTROYER_FLY )
		LoadDestroyerFlyInfo( rkLoader );

	// michael
	if( m_DashExtendType == DET_MICHAEL_FLY )
		LoadMichaelFlyInfo( rkLoader );

	//구미호
	if( m_DashExtendType == DET_GOOMIHO_FLY )
		LoadGoomihoFlyInfo( rkLoader );

	if( m_DashExtendType == DET_SUCCUBUS_CHAIN )
		LoadSuccubusStateInfo( rkLoader );

	if( m_DashExtendType == DET_DRACULA )
		LoadDraculaStateInfo( rkLoader );

	if( m_DashExtendType == DET_EVY )
		LoadEvyStateInfo( rkLoader );
		
	if( m_DashExtendType == DET_SLASHER )
		LoadSlasherStateInfo( rkLoader );

	//탑블레이트
	if( m_DashExtendType == DET_TOP_BL )
		LoadTopBlDummyInfo( rkLoader );

	//신비술사
	if( m_DashExtendType == DET_KAELTHAS )
		LoadKaelthasExtendAttackInfo( rkLoader );

	//드래곤 네스트 어쎄신
	if ( m_DashExtendType == DET_DRAGONNEST_ASS )
		LoadDragonNestAssExtendInfo( rkLoader );

	//박일표
	if ( m_DashExtendType == DET_PARKILPYO_NORMAL )
		LoadParkilpyoNormalExtendInfo( rkLoader );

	if ( m_DashExtendType == DET_PARKILPYO_OBJECT )
		LoadParkilpyoObjectExtendInfo( rkLoader );

	if ( m_DashExtendType == DET_JACK )
		LoadJackExtendInfo( rkLoader );

	if ( m_DashExtendType == DET_SHADOW_RANGER )
		LoadShadowRangerExtendInfo( rkLoader );

	if( m_DashExtendType == DET_JEONWOOCHI )
		LoadJeonWooChiExtendDashInfo( rkLoader );

	if( m_DashExtendType == DET_CUCHULAIN )
		LoadCuchulainInfo( rkLoader );

	if( m_DashExtendType == DET_ONIMUSHA )
		LoadOniMushaInfo( rkLoader );

	if( m_DashExtendType == DET_AZAZEL )
		LoadAzazelInfo( rkLoader );

	if( m_DashExtendType == DET_MIDNIGHT )
		LoadMidnightStateInfo( rkLoader );

	//약 D~
	m_bEnableHalfExtendAttack = rkLoader.LoadBool_e( "enable_half_extend_attack", false );
	if( m_bEnableHalfExtendAttack )
		LoadAttackAttribute( "half_extend_attack", m_HalfExtendAttack, rkLoader );

	//LeeDragon
	if( m_DashExtendType == DET_LEE )
	{
		LoadAttackAttribute( "dash_Lee_extend_attack", m_LeeDragonExtendAttribute, rkLoader );
		rkLoader.LoadString_e( "dash_charge_special_cancel_buff", "", szBuf, MAX_PATH );
		m_stDrangonAttackCancelBuff = szBuf;
		rkLoader.LoadString_e( "dash_charge_special_buff", "", szBuf, MAX_PATH );
		m_stDrangonAttackBuff = szBuf;
	}

	// Extend combo & Red_Hood
	int iMax = rkLoader.LoadInt_e( "extend_dash_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szKey, "extend_dash_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_ExtendAttackList.push_back( eAttribute );
	}

	// Extend combo2
	iMax = rkLoader.LoadInt_e( "extend2_dash_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szKey, "extend2_dash_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_ExtendAttackList2.push_back( eAttribute );
	}

	m_bAniJumpInputS = rkLoader.LoadBool_e( "extend_dash_input_jump_s_enable", false );

	// run rotate
	m_fRunSpeed = rkLoader.LoadFloat_e( "extend_run_speed", 0.0f );
	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "extend_rotate_speed", 0 );

	m_dwRunRotateJumpTime = (DWORD)rkLoader.LoadInt_e( "extend_run_jump_time", 0 );
	m_dwRunRotateChargeTime = (DWORD)rkLoader.LoadInt_e( "extend_run_charge_time", 0 );
	m_bRotateChargeEnable = rkLoader.LoadBool_e( "extend_run_charge_enable", false );

	m_iRunRotateAttackMaxCount = rkLoader.LoadInt_e( "extend_run_attack_max_cnt", 0 );
	if( m_iRunRotateAttackMaxCount == 0 )
	{
		AttackAttribute eAttribute;
		LoadAttackAttribute_e( "extend_run_attack", eAttribute, rkLoader );

		m_RunRotateAttackList.push_back( eAttribute );		

		DWORD dwRunDuration = (DWORD)rkLoader.LoadInt_e( "extend_run_duration", 0 );
		m_vRunDurationVec.push_back( dwRunDuration );
	}
	else
	{
		for( int i=0; i < m_iRunRotateAttackMaxCount; ++i )
		{
			AttackAttribute eAttribute;
			wsprintf_e( szKey, "extend_run_attack%d", i+1);
			LoadAttackAttribute( szKey, eAttribute, rkLoader );
			m_RunRotateAttackList.push_back( eAttribute );

			wsprintf_e( szKey, "extend_run_duration%d", i+1);
			DWORD dwRunDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );
			m_vRunDurationVec.push_back( dwRunDuration );
		}
	}
	
	LoadAttackAttribute_e( "extend_run_attack_end", m_RunRotateAttackEnd, rkLoader );

	// fly dash attack
	LoadAttackAttribute_e( "extend_fly_dash_attack", m_FlyDashAttack, rkLoader );
	LoadAttackAttribute_e( "extend_fly_dash_attack_end", m_FlyDashAttackEnd, rkLoader );

	m_fFlyDashSpeed	= rkLoader.LoadFloat_e( "extend_fly_dash_speed", 0.0f );
	m_dwFlyDashDuration = (DWORD)rkLoader.LoadInt_e( "extend_fly_dash_duration", 0 );

	m_fFlyDashLandingAniRate = rkLoader.LoadFloat_e( "extend_fly_dash_landing_ani_rate", 0.f );
	rkLoader.LoadString_e( "extend_fly_dash_landing_ani", "", szBuf, MAX_PATH );
	m_FlyDashLandingAni = szBuf;

	m_dwMaxChargeTime = rkLoader.LoadInt_e( "dash_gathering_max_time", 0 );
	m_fMaxDamageRate = rkLoader.LoadFloat_e( "dash_gathering_max_damage_rate", FLOAT1 );
	m_fMaxForceRate = rkLoader.LoadFloat_e( "dash_gathering_max_force_rate", FLOAT1 );
	m_fMaxBlowRate = rkLoader.LoadFloat_e( "dash_gathering_max_blow_rate", FLOAT1 );

	m_fMaxRangeRate = rkLoader.LoadFloat_e( "dash_gathering_max_range_rate", FLOAT1 );
	m_fMaxSpeedRate = rkLoader.LoadFloat_e( "dash_gathering_max_speed_rate", FLOAT1 );
	m_fMaxChargeAniRate = rkLoader.LoadFloat_e( "dash_gathering_max_ani_rate", 0.0f );
	m_fMaxForceSlidingRate = rkLoader.LoadFloat_e( "dash_gathering_max_force_sliding_rate", 0.0f );

	m_fMaxTargetRangeRate = rkLoader.LoadFloat_e( "dash_gathering_max_target_range_rate", 0.0f );
	m_fMaxTargetAngleRate = rkLoader.LoadFloat_e( "dash_gathering_max_target_angle_rate", 0.0f );

	m_fMaxFloatRate = rkLoader.LoadFloat_e( "dash_gathering_max_float_rate", 0.0f );

	m_fMaxScaleRate = rkLoader.LoadFloat_e( "dash_gathering_max_scale_rate", 0.0f );
	m_fMaxExplosionRate = rkLoader.LoadFloat_e( "dash_gathering_max_explosion_rate", 0.0f );
	m_fMaxWoundedTimeRate = rkLoader.LoadFloat_e( "dash_gathering_max_wounded_time_rate", FLOAT1 );
	m_fMaxLiveTimeRate = rkLoader.LoadFloat_e( "dash_gathering_max_livetime_rate", FLOAT1 );

	rkLoader.LoadString_e( "dash_power_charge_ani", "", szBuf, MAX_PATH );
	m_PowerChargeAni = szBuf;

	LoadAttackAttribute_e( "dash_power_charge_attack", m_PowerChargeAttack, rkLoader );

	rkLoader.LoadString_e( "dash_power_charge_effect", "", szBuf, MAX_PATH );
	m_DashGatheringEffect = szBuf;

	m_bPlayDashExtendWoundAni = rkLoader.LoadBool_e( "dash_damage_play_ani_wounded", false );
	m_bPowerChargeUseDefense = rkLoader.LoadBool_e( "dash_power_charge_use_defense", false );

	m_fFlyDashDecreseGauge = rkLoader.LoadFloat_e( "dash_fly_need_item_gauge", FLOAT0001 );
	
	m_fThunderBirdUseGauge = rkLoader.LoadFloat_e( "dash_thunder_bird_use_gauge", 0.0f );
	m_bMaxDashLastChargeAttack = rkLoader.LoadBool_e( "max_dash_and_last_charge_attack", false );

	m_bExceptionInputJumpSCheck = rkLoader.LoadBool_e( "dash_exception_input_jump_s_check", false );
	m_bInitActionDelayTime_NormalAttack = rkLoader.LoadBool_e( "dash_init_actiondelaytime_normal", false );

	m_bSetEnableReserveDash = rkLoader.LoadBool_e( "set_enable_reserve_dash", false );

	m_bKeyInputLastAttack = rkLoader.LoadBool_e( "dash_key_input_last_attack", false );
	LoadAttackAttribute_e( "dash_attack_key_attack", m_AttackKeyAttribute, rkLoader );
	LoadAttackAttribute_e( "dash_defense_key_attack", m_DefenseKeyAttribute, rkLoader );
	LoadAttackAttribute_e( "dash_jump_key_attack", m_JumpKeyAttribute, rkLoader );
	
	m_fOnimushaObjectChargeUseGauge = rkLoader.LoadFloat_e( "dash_onimusha_object_charge_use_gauge", 0.0f );

	m_bExtendDashInpuDashEnable = rkLoader.LoadBool_e( "extend_dash_input_dash_enable", false );

	LoadAttackAttribute_e( "dash_normal_s_attack", m_NormalSAttribute, rkLoader );
	LoadAttackAttribute_e( "dash_normal_a_attack", m_NormalAAttribute, rkLoader );
	LoadAttackAttribute_e( "dash_charge_s_attack", m_ChargeSAttribute, rkLoader );
	LoadAttackAttribute_e( "dash_charge_a_attack", m_ChargeAAttribute, rkLoader );

	m_fGhostExtendDashJumpRate = rkLoader.LoadFloat_e( "ghost_extend_dash_jump_rate", FLOAT1 );
}

void ioChargeComboDash::LoadJeonWooChiExtendDashInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	LoadAttackAttribute( "jeon_woo_chi_extend_attack", m_JeonWooChiDashInfo.m_PowerChargeAttribute, rkLoader );
	
	rkLoader.LoadString_e( "jeon_woo_chi_extend_dash_ani", "", szBuf, MAX_PATH );
	m_JeonWooChiDashInfo.m_stMoveAni = szBuf;
	m_JeonWooChiDashInfo.m_fMoveAniRate = rkLoader.LoadFloat_e( "jeon_woo_chi_extend_dash_ani_rate", FLOAT1 );
	m_JeonWooChiDashInfo.m_fMoveSpeed = rkLoader.LoadFloat_e( "jeon_woo_chi_extend_dash_speed", FLOAT10 );
	m_JeonWooChiDashInfo.m_fMaxMoveLength = rkLoader.LoadFloat_e( "jeon_woo_chi_extend_dash_max_length", 0.f );

	LoadAttackAttribute( "jeon_woo_chi_extend_move_attack", m_JeonWooChiDashInfo.m_MoveAttack, rkLoader );
	LoadAttackAttribute( "jeon_woo_chi_extend_move_add_attack", m_JeonWooChiDashInfo.m_AddMoveAttack, rkLoader );
}

void ioChargeComboDash::LoadCuchulainInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	LoadAttackAttribute( "dash_cuchulain_middle_attack", m_CuchulainMiddleAttack, rkLoader );

	LoadAttackAttribute( "dash_cuchulain_charge_attack", m_CuchulainChargeAttack, rkLoader );

	int iMax = rkLoader.LoadInt( "extend_dash_cuchulain_charge_combo_attack_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szKey, "extend_dash_cuchulain_charge_combo_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_CuchulainChargeComboAttack.push_back( eAttribute );
	}

	rkLoader.LoadString_e( "dash_special_fly_start_ani", "", szBuf, MAX_PATH );
	m_CuchulainFlyInfo.m_StartAni = szBuf;
	m_CuchulainFlyInfo.m_fStartAniRate = rkLoader.LoadFloat( "dash_special_fly_start_ani_rate", FLOAT1 );
	m_CuchulainFlyInfo.m_dwStartWaitTime = (int)rkLoader.LoadInt( "dash_special_fly_start_wait_time", 100000 );
	
	rkLoader.LoadString_e( "dash_special_fly_move_ani", "", szBuf, MAX_PATH );
	m_CuchulainFlyInfo.m_FlyAni = szBuf;
	m_CuchulainFlyInfo.m_fFlyAniRate = rkLoader.LoadFloat( "dash_special_fly_move_ani_rate", FLOAT1 );
	m_CuchulainFlyInfo.m_fFlyAngle = rkLoader.LoadFloat( "dash_special_fly_move_angle", 0.f );
	m_CuchulainFlyInfo.m_fFlySpeed = rkLoader.LoadFloat( "dash_special_fly_move_speed", 10000.f );
	m_CuchulainFlyInfo.m_fFlyMaxRange = rkLoader.LoadFloat( "dash_special_fly_move_range", 0.f );

	m_CuchulainFlyInfo.m_fEndJumpPower = rkLoader.LoadFloat( "dash_special_fly_end_jump_power",  0.f );

	rkLoader.LoadString_e( "dash_special_fly_add_buff", "", szBuf, MAX_PATH );
	m_CuchulainFlyInfo.m_stBuff= szBuf;
}

//yunga
void ioChargeComboDash::LoadLuciferFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_DashLuciferFly.Init();

	rkLoader.LoadString_e( "lucifer_fly_effect", "", szBuf, MAX_PATH );
	m_DashLuciferFly.m_FlyEffect = szBuf;

	rkLoader.LoadString_e( "lucifer_fly_move_effect", "", szBuf, MAX_PATH );
	m_DashLuciferFly.m_FlyMoveEffect = szBuf;

	wsprintf_e( szBuf, "lucifer_fly_attack" );
	LoadAttackAttribute( szBuf, m_DashLuciferFly.m_Attack, rkLoader );

	rkLoader.LoadString_e( "dash_lucifer_fly_start_ani", "", szBuf, MAX_PATH );
	m_DashLuciferFly.m_StartAni = szBuf;
	m_DashLuciferFly.m_fStartAniRate = rkLoader.LoadFloat_e( "dash_lucifer_fly_start_ani_rate", FLOAT1 );

	m_DashLuciferFly.m_fStartAngle = rkLoader.LoadFloat_e( "dash_lucifer_fly_start_angle", 0.0f );
	m_DashLuciferFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "dash_lucifer_fly_start_max_range", 0.0f );
	m_DashLuciferFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "dash_lucifer_fly_start_duration", 0 );

	rkLoader.LoadString_e( "lucifer_fly_delay_ani", "", szBuf, MAX_PATH );
	m_DashLuciferFly.m_DelayAni = szBuf;
	m_DashLuciferFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "lucifer_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "lucifer_fly_move_ani", "", szBuf, MAX_PATH );
	m_DashLuciferFly.m_MoveAni =szBuf;
	m_DashLuciferFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "lucifer_fly_move_ani_rate", FLOAT1 );

	m_DashLuciferFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "lucifer_fly_rotate_speed", 0 );

	rkLoader.LoadString_e( "lucifer_fly_move_ani_up", "", szBuf, MAX_PATH );
	m_DashLuciferFly.m_MoveAniUp = szBuf;
	m_DashLuciferFly.m_fMoveAniUpRate = rkLoader.LoadFloat_e( "lucifer_fly_move_ani_rate_up", FLOAT1 );

	rkLoader.LoadString_e( "lucifer_fly_move_ani_down", "", szBuf, MAX_PATH );
	m_DashLuciferFly.m_MoveAniDown = szBuf;
	m_DashLuciferFly.m_fMoveAniDownRate = rkLoader.LoadFloat_e( "lucifer_fly_move_ani_rate_down", FLOAT1 );

	m_DashLuciferFly.m_fMoveSpeed =rkLoader.LoadFloat_e( "fly_move_speed", 0.0f);
	m_DashLuciferFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	m_DashLuciferFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_DashLuciferFly.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_DashLuciferFly.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_DashLuciferFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_DashLuciferFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );
	m_DashLuciferFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );

	m_DashLuciferFly.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "lucifer_fly_jump_amt", 0.0f );
}

void ioChargeComboDash::LoadMechanicsFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_DashMechanicsFly.Init();

	rkLoader.LoadString_e( "dash_mechanics_fly_effect", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_MechanicsFlyEffect = szBuf;

	rkLoader.LoadString_e( "dash_mechanics_fly_move_effect", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_MechanicsFlyMoveEffect = szBuf;

	wsprintf_e( szBuf, "dash_mechanics_fly_attack" );
	LoadAttackAttribute( szBuf, m_DashMechanicsFly.m_MechanicsAttack, rkLoader );

	rkLoader.LoadString_e( "dash_mechanics_fly_start_ani", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_StartAni = szBuf;
	m_DashMechanicsFly.m_fStartAniRate = rkLoader.LoadFloat_e( "dash_mechanics_fly_start_ani_rate", FLOAT1 );

	m_DashMechanicsFly.m_fStartAngle = rkLoader.LoadFloat_e( "dash_mechanics_fly_start_angle", 0.0f );
	m_DashMechanicsFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "dash_mechanics_fly_start_max_range", 0.0f );
	m_DashMechanicsFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "dash_mechanics_fly_start_duration", 0 );

	rkLoader.LoadString_e( "dash_mechanics_fly_delay_ani", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_DelayAni = szBuf;
	m_DashMechanicsFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "dash_mechanics_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "dash_mechanics_fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_MoveAniFr = szBuf;
	m_DashMechanicsFly.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "dash_mechanics_fly_move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "dash_mechanics_fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_MoveAniBk = szBuf;
	m_DashMechanicsFly.m_fMoveAniBkRate = rkLoader.LoadFloat_e( "dash_mechanics_fly_move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "dash_mechanics_fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_MoveAniRt = szBuf;
	m_DashMechanicsFly.m_fMoveAniRtRate = rkLoader.LoadFloat_e( "dash_mechanics_fly_move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "dash_mechanics_fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_MoveAniLt = szBuf;
	m_DashMechanicsFly.m_fMoveAniLtRate = rkLoader.LoadFloat_e( "dash_mechanics_fly_move_ani_rate_lt", FLOAT1 );

	rkLoader.LoadString_e( "dash_mechanics_fly_move_ani_up", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_MoveAniUp = szBuf;
	m_DashMechanicsFly.m_fMoveAniUpRate = rkLoader.LoadFloat_e( "dash_mechanics_fly_move_ani_rate_up", FLOAT1 );

	rkLoader.LoadString_e( "dash_mechanics_fly_move_ani_down", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_MoveAniDown = szBuf;
	m_DashMechanicsFly.m_fMoveAniDownRate = rkLoader.LoadFloat_e( "dash_mechanics_fly_move_ani_rate_down", FLOAT1 );

	m_DashMechanicsFly.m_fMoveForceAmt = rkLoader.LoadFloat_e( "dash_fly_force_move", 0.0f );
	m_DashMechanicsFly.m_fMoveForceFric = rkLoader.LoadFloat_e( "dash_fly_force_friction", FLOAT1 );
	m_DashMechanicsFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "dash_fly_updown_speed", 0.0f );

	m_DashMechanicsFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "dash_fly_target_wound_type", TWT_ALL );
	m_DashMechanicsFly.m_fTargetAngle = rkLoader.LoadFloat_e( "dash_fly_target_angle", 45.0f );
	m_DashMechanicsFly.m_fTargetRange = rkLoader.LoadFloat_e( "dash_fly_target_range", 45.0f );
	m_DashMechanicsFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "dash_fly_target_up_height", 45.0f );
	m_DashMechanicsFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "dash_fly_target_down_height", 45.0f );
	m_DashMechanicsFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "dash_fly_target_marker_range", 45.0f );

	rkLoader.LoadString_e( "dash_fly_attack_move_ani", "", szBuf, MAX_PATH );
	m_DashMechanicsFly.m_AttackMoveAni = szBuf;
	m_DashMechanicsFly.m_fAttackMoveAniRate = rkLoader.LoadFloat_e( "dash_fly_attack_move_ani_rate", FLOAT1 );

	m_DashMechanicsFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "dash_fly_attack_move_speed", 0.0f );
	m_DashMechanicsFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "dash_fly_attack_move_range", FLOAT100 );
	m_DashMechanicsFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "dash_fly_attack_move_rate", FLOAT1 );

	m_DashMechanicsFly.m_fMechanicsFlyJumpAmt = rkLoader.LoadFloat_e( "dash_mechanics_fly_jump_amt", 0.0f );
}

void ioChargeComboDash::LoadMichaelFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	// 아이템쪽에서 MichaelFlyInfo의 필요정보만 취한다!
	m_DashMichaelFly.Init();

	// start
	rkLoader.LoadString_e( "dash_michaelfly_start_ani", "", szBuf, MAX_PATH );
	m_DashMichaelFly.m_StartAni = szBuf;
	m_DashMichaelFly.m_fStartAniRate = rkLoader.LoadFloat_e( "dash_michaelfly_start_ani_rate", FLOAT1 );

	m_DashMichaelFly.m_fStartAngle = rkLoader.LoadFloat_e( "dash_michaelfly_start_angle", 0.0f );
	m_DashMichaelFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "dash_michaelfly_start_max_range", 0.0f );
	m_DashMichaelFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "dash_michaelfly_start_duration", 0 );

	// 타겟 대상 검색용
	m_DashMichaelFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "dash_michaelfly_target_wound_type", TWT_ALL );
	m_DashMichaelFly.m_fTargetAngle = rkLoader.LoadFloat_e( "dash_michaelfly_target_angle", 0.0f );
	m_DashMichaelFly.m_fTargetRange = rkLoader.LoadFloat_e( "dash_michaelfly_target_range", 0.0f );
	m_DashMichaelFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "dash_michaelfly_target_up_height", 0.0f );
	m_DashMichaelFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "dash_michaelfly_target_down_height", 0.0f );

	// attack move
	wsprintf_e( szKey, "dash_michaelfly_attack_move" );
	LoadAttackAttribute( szKey, m_DashMichaelFly.m_AttackMove, rkLoader );

	m_DashMichaelFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "dash_michaelfly_attack_move_speed", 0.0f );
	m_DashMichaelFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "dash_michaelfly_attack_move_range", 0.0f );
	m_DashMichaelFly.m_fAttackMoveMinRange = rkLoader.LoadFloat_e( "dash_michaelfly_attack_move_min_range", 0.0f );
	m_DashMichaelFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "dash_michaelfly_attack_move_rate", FLOAT1 );

	// grappling
	m_DashMichaelFly.m_GrapplingInfo.Init();
	m_DashMichaelFly.m_GrapplingInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "dash_michaelfly_target_reverse_rotate", false );
	m_DashMichaelFly.m_GrapplingInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "dash_michaelfly_wound_grappling_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "dash_michaelfly_wound_grappling_loop_motion", "", szBuf, MAX_PATH );
	m_DashMichaelFly.m_GrapplingInfo.m_GrapplingLoopMotion = szBuf;
	m_DashMichaelFly.m_GrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_DashMichaelFly.m_GrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "dash_michaelfly_wound_grappling_loop_motion_rate", FLOAT1 );
	
	// grappling loop
	m_DashMichaelFly.m_fGrapplingRange = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_gap_range", 0.0f );
	m_DashMichaelFly.m_fGrapplingHeightRate = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_height_rate", 0.0f );

	wsprintf_e( szKey, "dash_michaelfly_grappling_loop_ani" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_DashMichaelFly.m_GrapplingLoopAni = szBuf;
	m_DashMichaelFly.m_fGrapplingLoopAniRate = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_loop_ani_rate", FLOAT1 );
	m_DashMichaelFly.m_dwGrapplingLoopDuration = rkLoader.LoadInt_e( "dash_michaelfly_grappling_loop_duration", 0 );
	m_DashMichaelFly.m_dwGrapplingEnableMoveTime = rkLoader.LoadInt_e( "dash_michaelfly_grappling_enable_move_time", 0 );

	// grappling move
	wsprintf_e( szKey, "dash_michaelfly_grappling_move_ani" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_DashMichaelFly.m_GrapplingMoveAni = szBuf;
	m_DashMichaelFly.m_fGrapplingMoveAniRate = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_move_ani_rate", FLOAT1 );

	wsprintf_e( szKey, "dash_michaelfly_grappling_dir_move_ani" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_DashMichaelFly.m_GrapplingDirMoveAni = szBuf;
	m_DashMichaelFly.m_fGrapplingDirMoveAniRate = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_dir_move_ani_rate", FLOAT1 );

	m_DashMichaelFly.m_fGrapplingMoveSpeed = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_move_speed", 0.0f );
	m_DashMichaelFly.m_fGrapplingMoveAngle = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_move_angle", 0.0f );
	m_DashMichaelFly.m_fGrapplingMoveRange = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_move_range", 0.0f );

	m_DashMichaelFly.m_fGrapplingDirMoveSpeed = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_dir_move_speed", 0.0f );
	m_DashMichaelFly.m_fGrapplingDirMoveAngle = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_dir_move_angle", 0.0f );
	m_DashMichaelFly.m_fGrapplingDirMoveRange = rkLoader.LoadFloat_e( "dash_michaelfly_grappling_dir_move_range", 0.0f );

	// end attack
	wsprintf_e( szKey, "dash_michaelfly_end_attack" );
	LoadAttackAttribute( szKey, m_DashMichaelFly.m_EndAttack, rkLoader );

	m_DashMichaelFly.m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "dash_michaelfly_end_jump_amt", 0.0f );
}

void ioChargeComboDash::LoadGoomihoFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_DashGoomihoFly.Init();

	// start
	rkLoader.LoadString_e( "dash_goomihofly_start_ani", "", szBuf, MAX_PATH );
	m_DashGoomihoFly.m_StartAni = szBuf;
	m_DashGoomihoFly.m_fStartAniRate = rkLoader.LoadFloat_e( "dash_goomihofly_start_ani_rate", FLOAT1 );

	m_DashGoomihoFly.m_fStartAngle = rkLoader.LoadFloat_e( "dash_goomihofly_start_angle", 0.0f );
	m_DashGoomihoFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "dash_goomihofly_start_max_range", 0.0f );
	m_DashGoomihoFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "dash_goomihofly_start_duration", 0 );

	// 타겟 대상 검색용
	m_DashGoomihoFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "dash_goomihofly_target_wound_type", TWT_ALL );
	m_DashGoomihoFly.m_fTargetAngle = rkLoader.LoadFloat_e( "dash_goomihofly_target_angle", 0.0f );
	m_DashGoomihoFly.m_fTargetRange = rkLoader.LoadFloat_e( "dash_goomihofly_target_range", 0.0f );
	m_DashGoomihoFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "dash_goomihofly_target_up_height", 0.0f );
	m_DashGoomihoFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "dash_goomihofly_target_down_height", 0.0f );

	// attack move
	wsprintf_e( szKey, "dash_goomihofly_attack_move" );
	LoadAttackAttribute( szKey, m_DashGoomihoFly.m_AttackMove, rkLoader );

	m_DashGoomihoFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "dash_goomihofly_attack_move_speed", 0.0f );
	m_DashGoomihoFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "dash_goomihofly_attack_move_range", 0.0f );
	m_DashGoomihoFly.m_fAttackMoveMinRange = rkLoader.LoadFloat_e( "dash_goomihofly_attack_move_min_range", 0.0f );
	m_DashGoomihoFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "dash_goomihofly_attack_move_rate", FLOAT1 );
	m_DashGoomihoFly.m_fAttackMoveAngle = rkLoader.LoadFloat_e( "dash_goomihofly_attack_move_angle", 0.0f );
	m_DashGoomihoFly.m_fAttackerHeightRate = rkLoader.LoadFloat_e( "dash_goomihofly_attack_move_height_rate_attacker", FLOAT05 );
	m_DashGoomihoFly.m_fTargetHeightRate = rkLoader.LoadFloat_e( "dash_goomihofly_attack_move_height_rate_target", FLOAT05 );
	
	int iExtendCnt = rkLoader.LoadInt_e( "dash_extend_attack_cnt", 0 );
	m_DashGoomihoFly.m_vExtendAttributeList.clear();
	m_DashGoomihoFly.m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "dash_extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_DashGoomihoFly.m_vExtendAttributeList.push_back( kAttribute );
	}

	iExtendCnt = rkLoader.LoadInt_e( "dash_extend_plus_attack_cnt", 0 );
	m_DashGoomihoFly.m_vExtendPlusAttributeList.clear();
	m_DashGoomihoFly.m_vExtendPlusAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "dash_extend_plus_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_DashGoomihoFly.m_vExtendPlusAttributeList.push_back( kAttribute );
	}

	// end attack
	wsprintf_e( szKey, "dash_goomihofly_end_attack" );
	LoadAttackAttribute( szKey, m_DashGoomihoFly.m_EndAttack, rkLoader );

	// 점프 전환용
	m_DashGoomihoFly.m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "dash_goomihofly_end_jump_amt", 0.0f );
	m_DashGoomihoFly.m_bJumpAttack = false;
}

void ioChargeComboDash::LoadSuccubusStateInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	m_DashSuccubusChain.Init();

	// start
	rkLoader.LoadString_e( "first_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_DashSuccubusChain.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "first_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_DashSuccubusChain.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "first_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_DashSuccubusChain.m_StartAniDown = szBuf;

	//시작
	wsprintf_e( szKey, "first_attack" );
	LoadAttackAttribute( szKey, m_DashSuccubusChain.m_FirstAttack, rkLoader );
	
	// 타겟 대상 검색용
	m_DashSuccubusChain.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_DashSuccubusChain.m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_DashSuccubusChain.m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_DashSuccubusChain.m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_DashSuccubusChain.m_fTargetDownHeight = rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	rkLoader.LoadString_e( "delay_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_DashSuccubusChain.m_AttackLoopAniCenter = szBuf;
	rkLoader.LoadString_e( "delay_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_DashSuccubusChain.m_AttackLoopAniUp = szBuf;
	rkLoader.LoadString_e( "delay_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_DashSuccubusChain.m_AttackLoopAniDown = szBuf;
	m_DashSuccubusChain.m_fAttackLoopAniRate = rkLoader.LoadFloat_e( "delay_attack_actionAni_Rate", FLOAT1 );

	m_DashSuccubusChain.m_dwWaitMaxDelayTime = (DWORD)rkLoader.LoadInt_e( "delay_attack_max_wait_time", 0 );

	rkLoader.LoadString_e( "add_attack_actionAni", "", szBuf, MAX_PATH );
	m_DashSuccubusChain.m_AddAttackAni = szBuf;

	// attack move
	wsprintf_e( szKey, "add_attack" );
	LoadAttackAttribute( szKey, m_DashSuccubusChain.m_AddAttack, rkLoader );
}

void ioChargeComboDash::LoadDestroyerFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_DestroyerFly.Init();

	rkLoader.LoadString_e( "destroyer_fly_effect", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_DestroyerFlyEffect = szBuf;

	rkLoader.LoadString_e( "destroyer_fly_move_effect", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_DestroyerFlyMoveEffect = szBuf;

	wsprintf_e( szBuf, "destroyer_fly_attack" );
	LoadAttackAttribute( szBuf, m_DestroyerFly.m_DestroyerAttack, rkLoader );

	m_DestroyerFly.m_dwChargingTime = rkLoader.LoadInt_e( "fly_charging_time", 100 );
	rkLoader.LoadString_e( "fly_charging", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_ChargingAni = szBuf;
	m_DestroyerFly.m_dwChargingAniPreDelay = rkLoader.LoadInt_e( "fly_charging_predely", 0 );
	m_DestroyerFly.m_fChargingAniRate = rkLoader.LoadFloat_e( "fly_charging_rate", FLOAT1 );

	rkLoader.LoadString_e( "fly_charged", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_ChargedAni = szBuf;
	m_DestroyerFly.m_fChargedAniRate = rkLoader.LoadFloat_e( "fly_charged_rate", FLOAT1 );

	wsprintf_e( szBuf, "fly_charge_attack" );
	LoadAttackAttribute( szBuf, m_DestroyerFly.m_DestroyerChargeAttack, rkLoader );

	rkLoader.LoadString_e( "destroyer_fly_start_ani", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_StartAni = szBuf;
	m_DestroyerFly.m_fStartAniRate = rkLoader.LoadFloat_e( "destroyer_fly_start_ani_rate", FLOAT1 );

	m_DestroyerFly.m_fStartAngle = rkLoader.LoadFloat_e( "destroyer_fly_start_angle", 0.0f );
	m_DestroyerFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "destroyer_fly_start_max_range", 0.0f );
	m_DestroyerFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "destroyer_fly_start_duration", 0 );

	rkLoader.LoadString_e( "destroyer_fly_delay_ani", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_DelayAni = szBuf;
	m_DestroyerFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "destroyer_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_MoveAniFr = szBuf;
	m_DestroyerFly.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_MoveAniBk = szBuf;
	m_DestroyerFly.m_fMoveAniBkRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_MoveAniRt = szBuf;
	m_DestroyerFly.m_fMoveAniRtRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_MoveAniLt = szBuf;
	m_DestroyerFly.m_fMoveAniLtRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_lt", FLOAT1 );

	m_DestroyerFly.m_fMoveForceAmt = rkLoader.LoadFloat_e( "fly_force_move", 0.0f );
	m_DestroyerFly.m_fMoveForceFric = rkLoader.LoadFloat_e( "fly_force_friction", FLOAT1 );
	m_DestroyerFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	m_DestroyerFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_DestroyerFly.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_DestroyerFly.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_DestroyerFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_DestroyerFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );

	m_DestroyerFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );

	rkLoader.LoadString_e( "fly_attack_move_ani", "", szBuf, MAX_PATH );
	m_DestroyerFly.m_AttackMoveAni = szBuf;
	m_DestroyerFly.m_fAttackMoveAniRate = rkLoader.LoadFloat_e( "fly_attack_move_ani_rate", FLOAT1 );

	m_DestroyerFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "fly_attack_move_speed", 0.0f );
	m_DestroyerFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "fly_attack_move_range", FLOAT100 );
	m_DestroyerFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "fly_attack_move_rate", FLOAT1 );

	m_DestroyerFly.m_fDestroyerFlyJumpAmt = rkLoader.LoadFloat_e( "destroyer_fly_jump_amt", 0.0f );
}

void ioChargeComboDash::LoadDraculaStateInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_DashDraculaFly.Init();

	m_DashDraculaFly.m_fStartAngle = rkLoader.LoadFloat_e( "dash_fly_start_angle", 0.0f );
	m_DashDraculaFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "dash_fly_start_max_range", 0.0f );
	m_DashDraculaFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "dash_fly_start_duration", 0 );

	m_DashDraculaFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "dash_fly_rotate_speed", 0 );
	m_DashDraculaFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "dash_fly_move_speed", 0.0f );
	m_DashDraculaFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "dash_fly_updown_speed", 0.0f );
	m_DashDraculaFly.m_vBuffList.clear();

	int nBuffCnt = rkLoader.LoadInt_e( "special_buff_cnt", 0 );
	for( int k=0; k < nBuffCnt; ++k )
	{
		wsprintf_e( szKey, "special_buff%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_DashDraculaFly.m_vBuffList.push_back( szBuff );
	}
}

void ioChargeComboDash::LoadEvyStateInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_EvyBlinkInfo.Init();

	rkLoader.LoadString_e( "dash_special_dash_animation", "", szBuf, MAX_PATH );
	m_EvyBlinkInfo.m_stDashAni = szBuf;
	
	m_EvyBlinkInfo.m_fDashAniRate = rkLoader.LoadFloat_e( "dash_special_dash_ani_rate", FLOAT1 );
	m_EvyBlinkInfo.m_fDashSpeed = rkLoader.LoadFloat_e( "dash_special_dash_speed", 0.f );
	m_EvyBlinkInfo.m_fDashLength = rkLoader.LoadFloat_e( "dash_special_dash_range", 0.f );

	rkLoader.LoadString_e( "dash_special_dash_end_ani", "", szBuf, MAX_PATH );
	m_EvyBlinkInfo.m_stDashEndAni = szBuf;

	m_EvyBlinkInfo.m_fDashEndAniRate = rkLoader.LoadFloat_e( "dash_special_dash_end_ani_rate", FLOAT1 );

	m_EvyBlinkInfo.m_fMaxBlinkLength = rkLoader.LoadFloat_e( "dash_max_blink_length", 0.f );
	m_EvyBlinkInfo.m_fMapHeightGap = rkLoader.LoadFloat_e( "dash_max_height_gap", 0.f );

	rkLoader.LoadString_e( "dash_blink_start_ani", "", szBuf, MAX_PATH );
	m_EvyBlinkInfo.m_StartBlinkMotion = szBuf;
	m_EvyBlinkInfo.m_fStartBlinkMotionRate = rkLoader.LoadFloat_e( "dash_blink_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "dash_blink_end_ani", "", szBuf, MAX_PATH );
	m_EvyBlinkInfo.m_EndBlinkMotion = szBuf;
	m_EvyBlinkInfo.m_fEndBlinkMotionRate = rkLoader.LoadFloat_e( "dash_blink_end_ani_rate", FLOAT1 );
}

void ioChargeComboDash::LoadSlasherStateInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_SlasherRunInfo.Init();
	m_SlasherRunInfo.m_fFirstMaxMoveDist = rkLoader.LoadFloat_e( "dash_first_run_dist", 0.0f );
	m_SlasherRunInfo.m_fFirstRunSpeed = rkLoader.LoadFloat_e( "dash_first_run_speed", 0.0f );
	m_SlasherRunInfo.m_dwFirstRunTime = rkLoader.LoadInt_e( "dash_first_run_duration", 0 );

	m_SlasherRunInfo.m_sFirstInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "dash_first_run_attack_type", NO_WEAPON_ATTRIBUTE );
	m_SlasherRunInfo.m_sFirstInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "dash_first_run_attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "dash_first_run_attack_type_wounded", "", szBuf, MAX_PATH );
	m_SlasherRunInfo.m_sFirstInfo.m_WoundedAnimation = szBuf;
	m_SlasherRunInfo.m_sFirstInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "dash_first_run_attack_type_wounded_time", 0 );
	m_SlasherRunInfo.m_sFirstInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "dash_first_run_attack_type_wounded_loop_ani", false );

	m_SlasherRunInfo.m_fSecondMaxMoveDist = rkLoader.LoadFloat_e( "dash_second_run_dist", 0.0f );
	m_SlasherRunInfo.m_fSecondRunSpeed = rkLoader.LoadFloat_e( "dash_second_run_speed", 0.0f );
	m_SlasherRunInfo.m_dwSecondRunTime = rkLoader.LoadInt_e( "dash_second_run_duration", 0 );

	m_SlasherRunInfo.m_sSecondInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "dash_second_run_attack_type", NO_WEAPON_ATTRIBUTE );
	m_SlasherRunInfo.m_sSecondInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "dash_second_run_attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "dash_second_run_attack_type_wounded", "", szBuf, MAX_PATH );
	m_SlasherRunInfo.m_sSecondInfo.m_WoundedAnimation = szBuf;
	m_SlasherRunInfo.m_sSecondInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "dash_second_run_attack_type_wounded_time", 0 );
	m_SlasherRunInfo.m_sSecondInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "dash_second_run_attack_type_wounded_loop_ani", false );
	
	//달리기 애니메이션 관련
	rkLoader.LoadString_e( "dash_first_run_delay_ani", "", szBuf, MAX_PATH );
	m_SlasherRunInfo.m_szFirstRunReadyAni = szBuf;
	m_SlasherRunInfo.m_fFirstRunReadyAniRate = rkLoader.LoadFloat_e( "dash_first_run_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "dash_first_run_loop_ani", "", szBuf, MAX_PATH );
	m_SlasherRunInfo.m_szFirstRunLoopAni = szBuf;
	rkLoader.LoadString_e( "dash_first_run_end_ani", "", szBuf, MAX_PATH );
	m_SlasherRunInfo.m_szFirstRunEndAni = szBuf;
	m_SlasherRunInfo.m_fFirstRunEndAniRate = rkLoader.LoadFloat_e( "dash_first_run_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "dash_second_run_delay_ani", "", szBuf, MAX_PATH );
	m_SlasherRunInfo.m_szSecondRunReadyAni = szBuf;
	m_SlasherRunInfo.m_fSecondRunReadyAniRate = rkLoader.LoadFloat_e( "dash_second_run_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "dash_second_run_loop_ani", "", szBuf, MAX_PATH );
	m_SlasherRunInfo.m_szSecondRunLoopAni = szBuf;
	rkLoader.LoadString_e( "dash_second_run_end_ani", "", szBuf, MAX_PATH );
	m_SlasherRunInfo.m_szSecondRunEndAni = szBuf;
	m_SlasherRunInfo.m_fSecondRunEndAniRate = rkLoader.LoadFloat_e( "dash_second_run_end_ani_rate", FLOAT1 );
}

void ioChargeComboDash::LoadTopBlDummyInfo( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "dash_top_combo_move_cnt", 0 );
	for( int i = 0; i < iCnt; ++i )
	{
		StTopMoveInfo info;
		wsprintf( szKey, "dash_top_combo_rotate_x%d", i+1 );
		info.fRotate_x = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "dash_top_combo_rotate_y%d", i+1 );
		info.fRotate_y = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "dash_top_combo_move_length%d", i+1 );
		info.fLength = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "dash_top_combo_create%d", i+1 );
		info.bCreate = rkLoader.LoadBool( szKey, false );

		m_vDummyMoveInfoCombo.push_back( info );
	}
	m_TopBl_Normal.fRotate_x = rkLoader.LoadFloat_e( "dash_top_normal_rotate_x", 0.f );
	m_TopBl_Normal.fRotate_y = rkLoader.LoadFloat_e( "dash_top_normal_rotate_y", 0.f );
	m_TopBl_Normal.fLength = rkLoader.LoadFloat_e( "dash_top_normal_length", 0.f );
	m_TopBl_Normal.bCreate = rkLoader.LoadBool_e( "dash_top_normal_create", false );

	m_TopBl_ChargeInfo.fRotate_x = rkLoader.LoadFloat_e( "dash_top_charge_rotate_x", 0.f );
	m_TopBl_ChargeInfo.fRotate_y = rkLoader.LoadFloat_e( "dash_top_charge_rotate_y", 0.f );
	m_TopBl_ChargeInfo.fLength = rkLoader.LoadFloat_e( "dash_top_charge_length", 0.f );
	m_TopBl_ChargeInfo.bCreate = rkLoader.LoadBool_e( "dash_top_charge_create", false );

	rkLoader.LoadString_e( "dash_top_charge_return_attack_ani", "", szBuf, MAX_PATH );
	m_TopBl_ChargeAttack.m_AttackAnimation = szBuf;
	m_TopBl_ChargeAttack.m_fAttackAniRate = rkLoader.LoadFloat_e( "dash_top_charge_return_attack_rate", FLOAT1 );

	m_dwChargeAttackDummyDelayTime = rkLoader.LoadInt_e( "dash_top_charge_return_attack_wait_time", 0 );
}

void ioChargeComboDash::LoadKaelthasExtendAttackInfo(  ioINILoader &rkLoader  )
{
	LoadAttackAttribute( "dash_kaelthas_extend_attack", m_KaelthasExtendAttribute, rkLoader );
}

void ioChargeComboDash::LoadMidnightStateInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_DashMidnightDashInfo.Init();

	// start
	rkLoader.LoadString_e( "first_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_DashMidnightDashInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "first_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_DashMidnightDashInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "first_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_DashMidnightDashInfo.m_StartAniDown = szBuf;

	//시작
	wsprintf_e( szKey, "first_attack" );
	LoadAttackAttribute( szKey, m_DashMidnightDashInfo.m_FirstAttack, rkLoader );

	// 타겟 대상 검색용
	m_DashMidnightDashInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_DashMidnightDashInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_DashMidnightDashInfo.m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_DashMidnightDashInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_DashMidnightDashInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	rkLoader.LoadString_e( "delay_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_DashMidnightDashInfo.m_AttackLoopAniCenter = szBuf;
	rkLoader.LoadString_e( "delay_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_DashMidnightDashInfo.m_AttackLoopAniUp = szBuf;
	rkLoader.LoadString_e( "delay_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_DashMidnightDashInfo.m_AttackLoopAniDown = szBuf;
	m_DashMidnightDashInfo.m_fAttackLoopAniRate = rkLoader.LoadFloat_e( "delay_attack_actionAni_Rate", FLOAT1 );

	m_DashMidnightDashInfo.m_dwWaitMaxDelayTime = (DWORD)rkLoader.LoadInt_e( "delay_attack_max_wait_time", 0 );

	rkLoader.LoadString_e( "add_attack_actionAni", "", szBuf, MAX_PATH );
	m_DashMidnightDashInfo.m_AddAttackAni = szBuf;

	// attack move
	wsprintf_e( szKey, "add_attack" );
	LoadAttackAttribute( szKey, m_DashMidnightDashInfo.m_AddAttack, rkLoader );
}

bool ioChargeComboDash::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioExtendDash::StartDash( pOwner );

	m_bSetCombo = false;
	m_bNormalAttacked = false;
	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;
	m_nCurReserveDash = 0;

	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwRunRotateStartTime = 0;

	m_DashState = DS_NONE;

	m_dwInputCancelCheckTime = 0;
	m_dwCancelCheckTime = 0;

	m_fCurChargeRate = 0.0f;

	m_nHalfAttackCount = 0;

	//쿠훌린
	m_bMiddleChargeStarted = false;

	//RedHood
	m_dwRedHoodSpecialMoveMacroTime_S = 0;
	m_dwRedHoodSpecialMoveMacroTime_E = 0;

	m_dwMESpecialMoveMacroTime_S = 0;
	m_dwMESpecialMoveMacroTime_E = 0;

	m_bOnimushaChargeAttack = false;

	pOwner->NormalAttackOver( false );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );

	if( CheckRunRotateState( pOwner ) )
		SetRunRotateState( pOwner, false, true );
	else if( CheckPowerChargeState( pOwner ) )
		SetPowerChargeState( pOwner, false );
	else
		ChangeToCharging( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_NONE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//전우치
	m_JeonWooChiState = JWCST_NONE;
	return true;
}

void ioChargeComboDash::SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName )
{
	if( !pWeapon ) return;

	DWORD dwRunDuration;
	int iCount = m_vRunDurationVec.size();
	if( COMPARE( m_iCurComboCnt, 0, iCount ) )
		dwRunDuration = m_vRunDurationVec[m_iCurComboCnt];
	else if( !m_vRunDurationVec.empty() )
		dwRunDuration = m_vRunDurationVec[0];
	else
		dwRunDuration = 0;

	if( m_DashState == DS_RUN_ROTATE )
		pWeapon->SetLiveTime( dwRunDuration );
	else if( m_DashState == DS_POWER_CHARGE_ATTACK )
		CheckChargeInfo( pWeapon );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	ioMissileWeapon *pMissleWeapon = ToMissileWeapon( pWeapon );
	ioTraceWeapon *pTraceWeapon = ToTraceWeapon( pWeapon );
	ioBombMissileWeapon *pBombMissleWeapon = ToBombMissileWeapon( pWeapon );

	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );

			if( m_DashState == DS_RUN_ROTATE || m_DashState == DS_DRAGONNEST_ASS )
				pZone->SetZoneValue( true, false );
			else if( m_DashState == DS_FLY_DASH_ATTACK )
				pZone->SetZoneValue( true, false );
			else
				pZone->SetZoneValue( false, false );
		}
	}

	if( pMissleWeapon )
	{
		ioBaseChar *pChar = pMissleWeapon->GetOwner();
		if( pChar )
		{
			ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
			ioChargeComboBowItem *pChargeComboBowItem = ToChargeComboBowItem( pItem );
			if( pChargeComboBowItem )
				pChargeComboBowItem->WasteBullet();
		}
	}

	if( pBombMissleWeapon )
	{
		ioBaseChar *pChar = pBombMissleWeapon->GetOwner();
		if( pChar )
		{
			ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
			ioRocketLauncherItem *pRocketLauncherItem = ToRocketLauncherItem( pItem );
			if( pRocketLauncherItem )
				pRocketLauncherItem->WasteBullet( pChar );
		}
	}

	ioTraceWeapon *pTrace = ToTraceWeapon( pWeapon );
	if( pTrace && m_bMiddleChargeStarted && m_DashExtendType == DET_CUCHULAIN )
	{
		ioHashString szTarget = GetCuchulainMiddleTarget( pOwner );
		if( !szTarget.IsEmpty() )
			pTrace->SetTraceTarget( szTarget );
	}
}

ioHashString ioChargeComboDash::GetCuchulainMiddleTarget( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioCuchulainItem *pCuchulainItem = ToCuchulainWeaponItem( pItem );
	if( pCuchulainItem )
		return pCuchulainItem->GetAimedTarget();

	return NULL;
}

void ioChargeComboDash::CheckKeyReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

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
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

		if( pOwner->IsSettedDirection() )
		{
			float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}

		ChangeToCharging( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << DST_STATE;
			kPacket << DS_CHARGE;
			kPacket << m_iCurComboCnt;
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
	else if( pOwner->IsAttackKeyDown() && !m_bEnableAttackState && !bReserveSkill )
	{
		// 2020-05-26 신규 용병 레어(권왕) 연속 2타 이후에 D~ 생성
		ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
		ioGwonwangItem* pGwonwangItem = ToGwonwangItem( pItem );
		if( pGwonwangItem && DS_EXTEND_COMBO != m_DashState )
		{
			ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

			if( pOwner->IsSettedDirection() )
			{
				float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				pOwner->SetTargetRotToRotate( qtRotate, true, false );
			}

			ChangeToCharging( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_EXTEND_DASH );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetHP().m_fCurValue;
				kPacket << DST_STATE;
				kPacket << DS_CHARGE;
				kPacket << m_iCurComboCnt;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
	else if ( m_bSetEnableReserveDash )
	{
		CheckCancelReserveByConditionalSpecialDash( pOwner );
	}
}

void ioChargeComboDash::CheckCancelReserveByConditionalSpecialDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsDirKeyDoubleClick() && m_eSpecialDashCurrDirkey == ioUserKeyInput::DKI_NONE )
	{
		m_bEnableJumpState = false;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = pOwner->CheckCurDirKey();
		m_iSkillInput = -1;
		pOwner->ClearDirDoubleClick();
	}
}

void ioChargeComboDash::CheckExtraReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

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
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		// 2020-06-22
		ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
		ioGwonwangItem* pGwonwangItem = ToGwonwangItem( pItem );
		if( pGwonwangItem && DS_EXTEND_COMBO == m_DashState )
		{
			return;
		}

		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKeyDown() && !m_bEnableAttackState && !bReserveSkill )
	{
		// 2020-05-26 신규 용병 레어(권왕) 연속 2타 이후에 D~ 생성
		ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
		ioGwonwangItem* pGwonwangItem = ToGwonwangItem( pItem );
		if( pGwonwangItem && DS_EXTEND_COMBO != m_DashState )
		{
			ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

			if( pOwner->IsSettedDirection() )
			{
				float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				pOwner->SetTargetRotToRotate( qtRotate, true, false );
			}

			ChangeToCharging( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_EXTEND_DASH );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetHP().m_fCurValue;
				kPacket << DST_STATE;
				kPacket << DS_CHARGE;
				kPacket << m_iCurComboCnt;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
}

void ioChargeComboDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DashState )
	{
	case DS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case DS_NORMAL_ATTACK:
	case DS_COMBO_ATTACK:
	case DS_HALF_EXTEND_ATTACK:
		ProcessNormalComboDash( pOwner );
		break;
	case DS_CUCHULAIN_MIDDLE_ATTACK_STATE:
		if( pOwner->IsNeedProcess() )
			ProcessCuchulainMiddleAttack( pOwner );

		break;
	case DS_EXTEND_COMBO:
	case DS_EXTEND_COMBO2:
		ProcessExtendComboDash( pOwner );
		break;
	case DS_RUN_ROTATE:
		ProcessRunRotate( pOwner );
		break;
	case DS_RUN_ROTATE_END:
		ProcessRunRotateEnd( pOwner );
		break;
	case DS_POWER_CHARGE:
		ProcessPowerCharge( pOwner );
		break;
	case DS_POWER_CHARGE_ATTACK:
		ProcessPowerChargeAttack( pOwner );
		break;
	case DS_FLY_DASH_ATTACK:
		ProcessFlyDashAttack( pOwner );
		break;
	case DS_FLY_DASH_ATTACK_LANDING:
		ProcessFlyDashAttackLanding( pOwner );
		break;
	case DS_FLY_DASH_ATTACK_END:
		ProcessFlyDashAttackEnd( pOwner );
		break;
	case DS_TOP_BL_RETURN:
		ProcessTopBlReturn( pOwner );
		break;
	case DS_KAELTHAS_EXTEND:
		ProcessKaelthasExtendAttack( pOwner );
		break;
	case DS_DRAGONNEST_ASS:
		ProcessDragonNestAssassinState( pOwner );
		break;
	case DS_LEE_DRAGON_ATTACK_STATE:
		ProcessLeeDragonAttackState( pOwner );
		break;
	case DS_PARKILPYO_NORMAL_GATHER:
		ProcessParkilpyoNormalGathering( pOwner );
		break;
	case DS_PARKILPYO_NORMAL_CHARGE_ATTACK:
		{
			ioParkilpyoItem *pParkilpyoItem = ToParkilpyoItem( pOwner->GetWeapon() );
			if( pParkilpyoItem && pParkilpyoItem->CheckCmdInputState( pOwner ) )
				return;

			if( m_dwActionEndTime < dwCurTime )
			{
				pOwner->SetState( CS_DELAY, false );
				return;
			}

			if ( CheckParkilpyoChargeComboInput( pOwner) )
			{
				ChangeToParkilpyoNormalChargeCombo( pOwner );
				return;
			}
		}
		break;
	case DS_PARKILPYO_NORMAL_CHARGE_COMBO:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckParkilpyoCancelReserve( pOwner );
				if( CheckAniJump_Input_Jump_S( pOwner ) )
					return;

				ioParkilpyoItem *pParkilpyoItem = ToParkilpyoItem( pOwner->GetWeapon() );
				if( pParkilpyoItem && pParkilpyoItem->CheckCmdInputState( pOwner ) )
					return;
			}

			if( m_dwActionEndTime < dwCurTime )
			{
				// 방어
				if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
				{
					pOwner->SetDefenseState();
					return;
				}

				// 대기
				pOwner->ClearAttackFireTimeAndSkill();
				pOwner->ClearReservedSliding();

				pOwner->SetState( CS_DELAY );
				return;
			}

			if ( CheckParkilpyoChargeComboInput( pOwner) )
			{
				ChangeToParkilpyoNormalChargeCombo( pOwner );
				return;
			}
		}
		break;
	case DS_JACK_CHARGE_ATTACK:
	case DS_PARKILPYO_OBJECT_CHARGE_ATTACK:
		{
			if( m_dwActionEndTime < dwCurTime )
				pOwner->SetState( CS_DELAY, false );
		}
		break;
	case DS_KEY_INPUT_LAST_ATTACK:
		ProcessKeyInputLastAttack( pOwner );
		break;
	case DS_JEONWOOCHI_DASH:
		ProcessJeonWooChiDashState( pOwner );
		break;
	case DS_ONIMUSHA_CHARGE_ATTACK:
		ProcessOniMushaChargeAttack( pOwner );
		break;
	case DS_AZAZEL:
		ProcessAzazelAttack( pOwner );
		break;
	case DS_SPECIAL_ATTACK:
		ProcessSpecialAttack( pOwner );
		break;
	case DS_EXTEND_CHARGE:
		ProcessExtendCharging( pOwner );
		break;
	}

	CheckSpecialStateByExtendType( pOwner );
}

bool ioChargeComboDash::CheckProcessDash( ioBaseChar *pOwner )
{
	//전우치때 추가
	if( pOwner->IsNeedProcess() && m_DashExtendType == DET_JEONWOOCHI && pOwner->IsDefenseKey() )
	{
		ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
		ioJeonWooChiItem *pJeonWooChiItem = ToJeonWooChiWeaponItem( pItem );
		if( pJeonWooChiItem && pJeonWooChiItem->Find_S_MoveTarget( pOwner ) )
		{
			pJeonWooChiItem->Set_S_Move_Step1( pOwner, false );
			return true;
		}
	}
	return false;
}

void ioChargeComboDash::ChangeCuchulainMiddleAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	const AttackAttribute *pAttr = &m_CuchulainMiddleAttack;
	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	m_szInputEventAnimation = pAttr->m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetComboDash - %s Animation is Not Exist",
			pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_EXTEND_DASH_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );

	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime = dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_CUCHULAIN_MIDDLE_ATTACK_STATE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_CUCHULAIN_MIDDLE_ATTACK_STATE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetComboDash( ioBaseChar *pOwner, bool bCharge )
{
	if( !pOwner ) return;

	int iMaxCnt = pOwner->GetExtraDashAttackMax();

	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();

	const AttackAttribute *pAttr = NULL;

	if( bCharge && m_DashExtendType == DET_CUCHULAIN)
	{
		if( (int)m_CuchulainChargeComboAttack.size() <= m_iCurComboCnt )
		{
			LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetComboDash_Charge_Cnt Err %d, %d", (int)m_CuchulainChargeComboAttack.size(), m_iCurComboCnt );
			return;
		}

		pAttr = &(m_CuchulainChargeComboAttack[m_iCurComboCnt]);
		CuchulainChargeBuffAttack( pOwner );
	}
	else
		pAttr = pOwner->GetAttackAttribute( AT_EXTRA_DASH_ATTACK, m_iCurComboCnt );

	if( !pAttr )
		return;

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	m_szInputEventAnimation = pAttr->m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetComboDash - %s Animation is Not Exist",
								pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_DashExtendType != DET_CUCHULAIN || !bCharge )
	{
		pOwner->RefreshFireTimeList( iAniID,
			pAttr->m_vWeaponInfoList,
			FTT_EXTEND_DASH_ATTACK,
			fTimeRate,
			dwPreDelay );

		pOwner->IncreaseWeaponIndexBase();
	}
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime = dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;
	
	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_bSetCombo = true;
	m_DashState = DS_COMBO_ATTACK;

	if( m_DashExtendType == DET_RED_HOOD || m_DashExtendType == DET_MAGICENGINEER )
		SetMacroTime( pGrp, iAniID, fTimeRate, pAttr->m_dwEndDelay );
		
	switch( m_DashExtendType )
	{
	case DET_THUNDER_BIRD:
		ioThunderBirdObjectItem *pThunderBirdObjectItem = ToThunderBirdObjectItem( pOwner->GetObject() );
		if ( pThunderBirdObjectItem )
			pThunderBirdObjectItem->DecreaseGauge( pOwner, m_fThunderBirdUseGauge );
		break;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_COMBO_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << bCharge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if(m_DashExtendType == DET_TOP_BL && CheckIsExtend_Top_Bl_mod( pOwner ) )
		SetTopBlDummyNormalMove( pOwner, true, m_iCurComboCnt );

	// send 이후
	m_iCurComboCnt++;	
}

void ioChargeComboDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case DST_STATE:
		ApplyDashState( pOwner, rkPacket );
		break;
	case DST_KEY:
		ApplyDashDirKey( pOwner, rkPacket );
		break;
	}
}

bool ioChargeComboDash::CheckEnableBuffAddState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_BUFF_ADD )
		return false;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioBuffAddItem *pBuffAddItem = ToBuffAddItem( pItem );
	if( !pBuffAddItem )
		return false;

	if( pBuffAddItem->GetCurBullet() >= pBuffAddItem->GetMaxBullet() )
		return true;

	return false;
}

void ioChargeComboDash::SetBuffAddState( ioBaseChar *pOwner )
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

	m_DashState = DS_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_BUFF_ADD;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::ExtendDashEnd( ioBaseChar *pOwner )
{
	m_DashState = DS_NONE;

	m_bSetCombo = false;
	m_iCurComboCnt = 0;
	m_dwChargingStartTime = 0;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;	
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	if( pOwner )
		pOwner->InitExtendAttackTagInfo();

	if( !m_DashGatheringEffect.IsEmpty() )
	{
		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}
	}

	RemoveRundRotateBuff( pOwner );

	ClearMiddleAim_Cuchulain( pOwner );
}

void ioChargeComboDash::ClearMiddleAim_Cuchulain( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioCuchulainItem *pCuchulainItem = ToCuchulainWeaponItem( pItem );
	if( pCuchulainItem )
		pCuchulainItem->ClearAimInfo( pOwner );
}

void ioChargeComboDash::ChangeToCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_ChargingAniList.empty() )
		return;

	ioRocketLauncherItem* pRocketLauncherItem = ToRocketLauncherItem( pOwner->GetWeapon() );
	if( pRocketLauncherItem && pRocketLauncherItem->GetCurBullet() <= 0 )
	{
		pRocketLauncherItem->SetExtendDefenseState( pOwner );
		return;
	}

	ioHashString szCurChargingAni;

	int iChargingCnt = m_ChargingAniList.size();
	int iCurIndex = m_iCurComboCnt;

	if( m_bSetCombo )
		iCurIndex = m_iCurComboCnt+1;

	if( COMPARE( iCurIndex, 0, iChargingCnt ) )
		szCurChargingAni = m_ChargingAniList[iCurIndex];
	else
		szCurChargingAni = m_ChargingAniList[0];

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( szCurChargingAni, FLOAT100 );

	m_dwChargingStartTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;	
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	m_DashState = DS_CHARGE;
}

void ioChargeComboDash::ChangeToExtendCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_ExtendChargingAniList.empty() )
		return;

	ioHashString szCurChargingAni;

	int iChargingCnt = m_ExtendChargingAniList.size();
	int iCurIndex = m_iCurComboCnt;

	if( m_bSetCombo )
		iCurIndex = m_iCurComboCnt+1;

	if( COMPARE( iCurIndex, 0, iChargingCnt ) )
		szCurChargingAni = m_ExtendChargingAniList[iCurIndex];
	else
		szCurChargingAni = m_ExtendChargingAniList[0];

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( szCurChargingAni, FLOAT100 );

	m_dwChargingStartTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;	
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	m_DashState = DS_EXTEND_CHARGE;
}

void ioChargeComboDash::CuchulainChargeBuffAttack( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioCuchulainItem *pCuchulainItem = ToCuchulainWeaponItem( pItem );
	if( pCuchulainItem )
		pCuchulainItem->ChargeBuffSearch( pOwner, false );
}

void ioChargeComboDash::ChangeToNormalAttack( ioBaseChar *pOwner, bool bCancelAttack, bool bChargeAttack )
{
	const AttackAttribute *pAttr = NULL;

	if( bChargeAttack )
		pAttr = &m_CuchulainChargeAttack;
	else
		pAttr = pOwner->GetAttackAttribute( AT_DASH_ATTACK );
	if( !pAttr )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	m_szInputEventAnimation = pAttr->m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::ChangeToNormalAttack - %s Animation is Not Exist",
								pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = pAttr->m_fAttackAniRate;
	m_fInputEventAnimationRate = fTimeRate;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( bChargeAttack && m_DashExtendType == DET_CUCHULAIN )
	{
		CuchulainChargeBuffAttack( pOwner );
	}
	else
	{
		pOwner->RefreshFireTimeList( iAniID,
			pAttr->m_vWeaponInfoList,
			FTT_EXTEND_DASH_ATTACK,
			fTimeRate,
			pAttr->m_dwPreDelay );

		pOwner->IncreaseWeaponIndexBase();
	}
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)pAttr->m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = pAttr->m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime= dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, pAttr->m_dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, pAttr->m_dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + pAttr->m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	
	m_bNormalAttacked = true;
	m_bSetCombo = true;
	m_DashState = DS_NORMAL_ATTACK;
	
	//탑블 예외 처리
	if(m_DashExtendType == DET_TOP_BL && CheckIsExtend_Top_Bl_mod( pOwner ) )
		SetTopBlDummyNormalMove( pOwner, false, 0 );

	if( bCancelAttack )
	{
		if( !m_stDrangonAttackCancelBuff.IsEmpty() )
			pOwner->AddNewBuff( m_stDrangonAttackCancelBuff, pOwner->GetCharName(), "", NULL );
	}

	if( m_bInitActionDelayTime_NormalAttack )
	{
		pOwner->InitExtendAttackTagInfo();
		pOwner->InitActionStopDelay( true );
	}

	CheckKeyReserveTime( pOwner, iAniID, pAttr->m_fAttackAniRate );
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, pAttr->m_dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, pAttr->m_dwPreDelay );

	if( m_DashExtendType == DET_RED_HOOD || m_DashExtendType == DET_MAGICENGINEER )
		SetMacroTime( pGrp, iAniID, fTimeRate, pAttr->m_dwEndDelay );

	SetAniRotate( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_NORMAL_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << bCancelAttack;
		kPacket << bChargeAttack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_DashExtendType == DET_EVY && CheckEnableEvyState(pOwner) )
	{
		SetEvyState( pOwner );
		return;
	}
	else if ( CheckBubbleFighterBullet( pOwner ) )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			switch( m_DashExtendType )
			{
			case DET_BUFF_ADD:
				if( CheckEnableBuffAddState( pOwner ) )
				{
					SetBuffAddState( pOwner );
					return;
				}
				break;
			case DET_MECHANICS_FLY:
				if( CheckEnableMechanicsFlyState( pOwner ) )
				{
					SetMechanicsFlyState( pOwner );
					return;
				}
				break;
			case DET_EXTEND_COMBO:
			case DET_UFC:
			case DET_GHOST:
			case DET_HANZO:
				if( CheckEnableExtendComboState( pOwner ) )
				{
					m_iCurComboCnt = 0;
					SetExtendComboState( pOwner );
					return;
				}
				break;
			case DET_RED_HOOD:
				if( CheckEnableExtendComboState( pOwner ) )
				{
					SetExtendComboState( pOwner );
					return;
				}
				break;
			case DET_RUN_ROTATE:
				if( CheckRunRotateState( pOwner ) )
				{
					SetRunRotateChargeState( pOwner, true );
					return;
				}
				break;
			case DET_RUN_ROTATE2:
				if( CheckRunRotateState2( pOwner ) )
				{
					SetRunRotateState( pOwner, true, false );
					return;
				}
				break;
			case DET_PHANTOM_FLY:
				if( CheckEnablePhantomFlyState( pOwner ) )
				{
					SetPhantomFlyState( pOwner );
					return;
				}
				break;
			case DET_LUCIFER_FLY:
				if( CheckEnableLuciferFlyState( pOwner ) )
				{
					SetLuciferFlyState( pOwner );
					return;
				}
				break;
			case DET_DESTROYER_FLY:
				if( CheckEnableDestroyerFlyState( pOwner ) )
				{
					SetDestroyerFlyState( pOwner );
					return;
				}
				break;
			case DET_MICHAEL_FLY:
				if( CheckEnableMichaelFlyState( pOwner ) )
				{
					SetMichaelFlyState( pOwner );
					return;
				}
				break;
			case DET_GOOMIHO_FLY:
				if( CheckEnableGoomihoFlyState( pOwner ) )
				{
					SetGoomihoFlyState( pOwner );
					return;
				}
				break;
			case DET_SUCCUBUS_CHAIN:
				if( CheckEnableSuccubusState( pOwner ) )
				{
					SetSuccubusState( pOwner );
					return;
				}
				break;
			case DET_DRACULA:
				if( CheckEnableDraculaState( pOwner ) )
				{
					SetDraculaState( pOwner );
					return;
				}
				break;
			case DET_FLY_DASH_ATTACK:
				if( CheckEnableFlyDashAttackState( pOwner ) )
				{
					SetFlyDashAttackState( pOwner, true );
					return;
				}
				break;
			case DET_FLASH:
				if( CheckEnableFlashState( pOwner ) )
				{
					SetFlashState( pOwner );
					return;
				}
				break;
			case DET_SLASHER:
				if( CheckEnableSlasherState( pOwner ) )
				{
					SetSlasherState( pOwner );
					return;
				}
				break;
			case DET_TOP_BL:
				if( CheckIsExtend_Top_Bl_mod( pOwner ) )//현재 변신 상태인가?
				{
					//회수
					SetTopBlExtendAttack( pOwner );
					return;
				}
				else if( CheckEnalbe_Top_Bl_ExtendMod( pOwner ) )//변신이 가능한가??
				{
					//변신
					SetTopBlExtendMod( pOwner );
					return;
				}
				//아니면 일반타가 나가겠지
				break;
			case DET_KAELTHAS:
				if( CheckKaelthasExtendAttack( pOwner) )
				{
					SetKaelthasExtendAttack( pOwner );
					return;
				}
				break;
			case DET_DRAGONNEST_ASS:
				if( CheckDragonNestAssExtendState( pOwner) )
				{
					SetDragonNestAssExtendPreState( pOwner );
					return;
				}
				break;
			case DET_THUNDER_BIRD:
				if( CheckEnableThunderBirdState( pOwner ) )
				{
					SetThunderBirdState( pOwner );
					return;
				}
				break;
			case DET_EQUIP_OBJECT:				
				if( CheckEnableEquipObjectItemState( pOwner ) )
				{
					SetObjectEquipState( pOwner );
					return;
				}				
				break;
			case DET_LEE:
				if( CheckEnableLeeDragonAttackState( pOwner ) )
				{
					SetLeeDragonAttackState( pOwner );
					return;
				}
				break;
			case DET_PARKILPYO_NORMAL:
				ChangeToParkilpyoNormalGathering( pOwner );
				return;
			case DET_PARKILPYO_OBJECT:
				ChangeToParkilpyoObjectChargeAttack( pOwner );
				return;
			case DET_JACK:
				if( CheckEnableJackChargeAttack( pOwner ) )
				{
					ChangeToJackChargeAttack( pOwner );
					return;
				}
				break;
			case DET_SHADOW_RANGER:
				if( CheckEnableShadowRangerState( pOwner ) )
				{
					SetShadowRangerMoveState( pOwner );
					return;
				}
				break;
			case DET_JUMPER:
				if( CheckEnableJumperState( pOwner ) )
				{
					SetJumperState( pOwner );
					return;
				}
				break;
			case DET_SABER:
				if( CheckEnableSaberExtendState( pOwner ) )
				{
					SetSaberExtendState( pOwner );
					return;
				}
				break;
			case DET_ANGEL_OF_DEATH:
				if( CheckEnableAngelOfDeathPowerAttackState( pOwner ) )
				{
					SetAngelOfDeathPowerAttackState( pOwner );
					return;
				}
				break;
			case DET_HADES:
				if( CheckEnableHadesState( pOwner ) )
				{
					SetHadesFullChargeState( pOwner );
					return;
				}
				break;
			case DET_MAGIC_SWORD:
				if( CheckEnableMagicSwordAttackState( pOwner ) )
				{
					SetMagicSwordAttackState( pOwner );
					return;
				}
				break;
			case DET_NATA:
				if( CheckEnableNataState( pOwner ) )
				{
					SetNataComboState( pOwner );
					return;
				}
				break;
			case DET_RESEARCHER_REAR_FLY:
				if( CheckEnableResearcherRearFlyState( pOwner ) )
				{
					SetResearcherRearFlyState( pOwner );
					return;
				}
				break;
			case DET_BULLFIGHT:
				if( CheckEnableBullFightState( pOwner ) )
				{
					SetBullFightState( pOwner );
					return;
				}
				break;
			case DET_JEONWOOCHI:
				if( CheckEnableJeonWooChiChargeAttack(pOwner) )
				{
					SetJeonWooChiChargeState( pOwner );
					return;
				}
				break;
			case DET_SAWANG_SLIDE:
				if( CheckEnableSawangState( pOwner ) )
				{
					SetSawangState( pOwner );
					return;
				}
				break;
			case DET_CHEMISTRY:
				if( CheckEnableChemistyChargeAttack( pOwner) )
				{
					SetChemistryChargeState( pOwner );
					return;
				}
				break;
			case DET_STRIDER:
				if ( CheckEnableStriderExtendState( pOwner ) )
				{
					SetStriderExtendState( pOwner );
					return;
				}
				break;
			case DET_APOSTATE_MONK:
				if ( CheckEnableApostateMonkExtendState( pOwner ) )
				{
					SetApostateMonkExtendState( pOwner );
					return;
				}
				break;
			case DET_CUCHULAIN:
				{
					ClearMiddleAim_Cuchulain( pOwner );
					if( m_bSetCombo )
						SetComboDash( pOwner, true );
					else
						ChangeToNormalAttack(pOwner, false, true );
					return;
				}
				break;
			case DET_ONIMUSHA:
				{
					if ( m_bOnimushaChargeAttack && CheckEnableOniMushaRunState( pOwner ) )
					{
						ChangeToOniMushaRunState( pOwner );
						return;
					}
					else if( CheckEnableEquipObjectItemState( pOwner ) )
					{
						SetOniMushaObjectEquipState( pOwner );
						return;
					}		
				}		
				break;
			case DET_DRUID:
				if( CheckEnableDruidBearState( pOwner ) )
				{
					SetDruidBearState( pOwner );
					return;
				}				
				break;
			case DET_SASINMU:
				if( CheckEnableSasinmuState( pOwner ) )
				{
					SetSasinmuState( pOwner );
					return;
				}
				break;
			case DET_PREDATOR:
				if( CheckEnablePredatorState( pOwner ) )
				{
					SetPredatorState( pOwner );
					return;
				}
				break;
			case DET_CROW:
				if( CheckEnableCrowSpecialDashState( pOwner ) )
				{
					SetCrowSpecialDashState( pOwner );
					return;
				}
				break;
			case DET_BLACK_SPHEAR:
				if( CheckEnableExtendComboState( pOwner ) )
				{
					m_iCurComboCnt = 0;
					SetExtendComboState( pOwner );
					return;
				}
				break;

			case DET_MEPHISTO:
				if( CheckEnableMephistoSpecialDashState( pOwner ) )
				{
					SetMephistoSpecialDashState( pOwner );
					return;
				}
				break;
			case DET_BUBBLE_FIGHTER:
				if( CheckEnableBubbleFighterDashAttack( pOwner ) )
				{
					SetBubbleFighterDashAttack( pOwner );
					return;
				}
				break;
			case DET_URIEL:
				if( CheckEnableUrielChargeDashAttack( pOwner ) )
				{
					SetUrielChargeDashAttack( pOwner );
					return;
				}
				break;
			case DET_WINDGOD:
				if ( CheckEnableWindGodDashAttack( pOwner ) )
				{
					SetWindGodDashAttack( pOwner );
					return;
				}
				break;
			case DET_STRANGER:
				if ( CheckEnableStrangerState( pOwner ) )
				{
					SetStrangerState( pOwner );
					return;
				}
				break;
			case DET_HACKER:
				if ( CheckEnableHackerDashAttackState( pOwner ) )
				{
					SetHackerDashAttackState( pOwner );
					return;
				}
				break;
			case DET_MAGICENGINEER:
				if( CheckEnableMagicEngineerState( pOwner ) )
				{
					SetMagicEngineerChargeState( pOwner, false );
					return;
				}
				break;
			case DET_NIGHTMARE:
				if( CheckEnableNightmareState( pOwner ) )
				{
					SetNightmareState( pOwner );
					return;
				}
				break;
			case DET_AZAZEL:
				if( CheckEnableAzazelSpecialState( pOwner ) )
				{
					SetAzazelSpecialState( pOwner );
					return;
				}
				else if( CheckEnableAzazelState( pOwner ) )
				{
					SetAzazelState( pOwner, true );
					return;
				}
				break;
			case DET_NEO:
				if( CheckEnableNeoExtendState( pOwner, false ) )
				{
					SetNeoExtendState( pOwner, false );
					return;
				}
				else if( CheckEnableNeoExtendState( pOwner, true ) )
				{
					SetNeoExtendState( pOwner, true );
					return;
				}
				break;
			case DET_AUTOMATON:
				if( CheckEnableAutomatonState( pOwner ) )
				{
					SetAutomatonState( pOwner );
					return;
				}
				break;
			case DET_ROCKET_LAUNCHER:
			case DET_VULCAN:
				if( CheckEnableVulcanState( pOwner ) )
				{
					SetVulcanState( pOwner );
					return;
				}
				break;
			case DET_CHANGE_ON:
				if( CheckEnableChangeOnState( pOwner ) )
				{
					SetChangeOnState( pOwner );
					return;
				}
				break;
			case DET_CHANGE_ON_WOLF:
				if( CheckEnableChangeOnWolfState( pOwner ) )
				{
					SetChangeOnWolfState( pOwner );
					return;
				}
				break;
			case DET_OMEN:
				if( CheckEnableOmenState( pOwner ) )
				{
					SetOmenState( pOwner );
					return;
				}
				break;
			case DET_MIDNIGHT:
				if( CheckEnableMidnightState( pOwner ) )
				{
					SetMidnightState( pOwner );
					return;
				}
				break;
			case DET_HERACLES:
				if( CheckRunRotateState2( pOwner ) )
				{
					SetRunRotateState( pOwner, true, false );
					return;
				}
				break;
			}

			if( m_bSetCombo && !CheckCanComboDash( pOwner ) )
				return;

			if( m_bSetCombo )
				SetComboDash( pOwner );
			else
				ChangeToNormalAttack( pOwner );
			return;
		}
		else if( m_dwChargingStartTime + m_dwHalfChargingTime < dwCurTime && m_DashExtendType == DET_CUCHULAIN )
		{
			if( !m_bMiddleChargeStarted )
			{
				m_bMiddleChargeStarted = true;
				MiddleAimInit_Cuchulain( pOwner );
			}

			CheckMiddleCharge_Cuchulain( pOwner );
		}
	}
	else
	{
		if( m_dwChargingStartTime + m_dwHalfChargingTime < dwCurTime )
		{
			switch( m_DashExtendType )
			{
			case DET_HADES:
				if( CheckEnableHadesState( pOwner ))
				{
					SetHadesHalfChargeState( pOwner );
					return;
				}
				break;
			case DET_CUCHULAIN:
				ChangeCuchulainMiddleAttack( pOwner );
				return;
			case DET_ONIMUSHA:
				if( CheckEnableOniMushaHalfChargeState( pOwner ))
				{
					ChangeToOniMushaChargeAttack( pOwner );
					return;
				}
				break;
			case DET_MAGICENGINEER:
				if( CheckEnableMagicEngineerState( pOwner ))
				{
					SetMagicEngineerChargeState( pOwner, true );
					return;
				}
				break;
			case DET_NEO:
				if( CheckEnableNeoExtendState( pOwner, true ) )
				{
					SetNeoExtendState( pOwner, true );
					return;
				}
				break;
			default:
				if( m_bEnableHalfExtendAttack && m_nHalfAttackCount == 0 )
				{
					ChangeHalfExtendAttack( pOwner );
					return;
				}
				break;
			}
		}
		if( m_bSetCombo && !CheckCanComboDash( pOwner ) )
			return;

		if( m_bMaxDashLastChargeAttack && CheckMaxComboSetDelayState( pOwner ) )
			return;

		if( m_bSetCombo )
			SetComboDash( pOwner );
		else 
			ChangeToNormalAttack( pOwner );

		return;
	}
}

void ioChargeComboDash::ProcessExtendCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime+m_dwExtendChargingTime < dwCurTime )
		{
			switch( m_DashExtendType )
			{
			case DET_GHOST:
				if( CheckEnableGhostState( pOwner ))
				{
					SetGhostState( pOwner );
					return;
				}
				break;
			}

			SetExtendComboState( pOwner );
		}
	}
	else
		SetExtendComboState( pOwner );
}

void ioChargeComboDash::SetCuchulainSpecialMove( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioCuchulainItem *pCuchulainItem = ToCuchulainWeaponItem( pItem );
	if( pCuchulainItem )
		pCuchulainItem->SetSpecialMoveState( pOwner, m_CuchulainFlyInfo, true );
}

bool ioChargeComboDash::IsEnableCuchulainSpecialMove( ioBaseChar *pOwner )
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

void ioChargeComboDash::MiddleAimInit_Cuchulain( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioCuchulainItem *pCuchulainItem = ToCuchulainWeaponItem( pItem );
	if( pCuchulainItem )
		pCuchulainItem->AimTargetInit( pOwner, true );
}

void ioChargeComboDash::CheckMiddleCharge_Cuchulain(ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioCuchulainItem *pCuchulainItem = ToCuchulainWeaponItem( pItem );
	if( pCuchulainItem )
		pCuchulainItem->CheckMiddleSearch( pOwner );
}

bool ioChargeComboDash::CheckCanComboDash( ioBaseChar *pOwner )
{
	if( m_bSetCombo && m_DashExtendType == DET_EXTEND_COMBO )
	{
		ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
		ioChargeComboBowItem *pChargeComboBowItem = ToChargeComboBowItem( pItem );
		if( pChargeComboBowItem )
		{
			if( pChargeComboBowItem->GetCurBullet() == 0 )
			{
				//ExtendDashEnd( pOwner );
				m_bSetCombo = false;
				pOwner->ClearAttackFireTimeAndSkill();
				pOwner->ClearReservedSliding();

				pOwner->SetState( CS_DELAY );
				return false;
			}
		}
	}

	if( m_bSetCombo && m_DashExtendType == DET_THUNDER_BIRD )
	{
		ioThunderBirdObjectItem *pThunderBirdObjectItem = ToThunderBirdObjectItem( pOwner->GetObject() );
		if( pThunderBirdObjectItem && pThunderBirdObjectItem->CheckThunderBirdGauge( pOwner ) )
		{
			m_bSetCombo = false;
			pOwner->ClearAttackFireTimeAndSkill();
			pOwner->ClearReservedSliding();

			pOwner->SetState( CS_DELAY );
			return false;
		}
	}

	return true;
}

void ioChargeComboDash::ProcessNormalComboDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	ioParkilpyoItem *pParkilpyoItem = ToParkilpyoItem( pOwner->GetWeapon() );
	if( pParkilpyoItem && pParkilpyoItem->CheckCmdInputState( pOwner ) )
		return;

	if( CheckAniDash_Input_Speical_Dash( pOwner ) )
		return;

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		//Bruce Lee때 추가 by 윤태철
		if( pOwner->IsNeedProcess() && m_bExceptionInputJumpSCheck && pOwner->IsJumpKey() )
		{
			if( CheckAniJump_Input_Jump_S_Exception( pOwner ) )
				return;
		}

		//쿠훌린 S키 조작
		if( pOwner->IsNeedProcess() && m_DashExtendType == DET_CUCHULAIN &&	pOwner->IsDefenseKey() && IsEnableCuchulainSpecialMove( pOwner ) )
		{
			SetCuchulainSpecialMove( pOwner );
			return;
		}
		if( pOwner->IsNeedProcess() && m_DashExtendType == DET_UFC && m_dwReserveEnableTime < dwCurTime )
		{
			if( pOwner->IsDefenseKey() )
			{
				ChangeToSpecialAttackFire( pOwner, SS_NORMAL_S );
				return;
			}
			else if( pOwner->IsJumpKey() )
			{
				ChangeToSpecialAttackFire( pOwner, SS_NORMAL_A );
				return;
			}
		}
		if( pOwner->IsNeedProcess() && m_DashExtendType == DET_CHANGE_ON_WOLF && pOwner->IsDefenseKey() && m_dwReserveEnableTime < dwCurTime )
		{
			ioObjectWereWolfItem *pObjectWere = ToObjectWereWolfItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pObjectWere && pObjectWere->IsEnableDefenseRelease() )
			{
				pObjectWere->ChangeToRelease( pOwner );

				pOwner->ClearAttackFireTimeAndSkill();
				pOwner->ClearReservedSliding();
				pOwner->SetState( CS_DELAY );
				return;
			}
		}
		if( pOwner->IsNeedProcess() && m_DashExtendType == DET_HERACLES && pOwner->IsJumpKey() )
		{
			CheckHeraclesJump_Input_Jump_S( pOwner );

			return;
		}
		if( m_dwReserveEnableEndTime == 0 && m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt && !m_bMaxDashLastChargeAttack )
			{
				CheckExtraReserve( pOwner );
			}
			else
			{
				CheckKeyReserve( pOwner );
			}
			
			CheckAniJump_Input_Jump_S( pOwner );

			if ( m_bKeyInputLastAttack && m_iCurComboCnt >= iMaxCnt )
			{
				if ( m_bEnableDefenseState || m_bEnableAttackState || m_bEnableJumpState )
				{
					ChangeToKeyInputAttack( pOwner );
					return;
				}
			}

			return;
		}
		else if( m_dwReserveEnableEndTime > 0 && m_dwReserveEnableTime > 0 && 
			m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime &&
			m_dwReserveEnableEndTime + pOwner->GetActionStopTotalDelay() >= dwCurTime )
		{
			// 예약체크
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt && !m_bMaxDashLastChargeAttack )
				CheckExtraReserve( pOwner );
			else
				CheckKeyReserve( pOwner );
			
			return;
		}

		CheckAniRotate( pOwner );
	}
	else
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}

void ioChargeComboDash::ChangeToSpecialAttackFire( ioBaseChar *pOwner, int iSpecialState )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_DashState = DS_SPECIAL_ATTACK;
	m_bEnableJumpState = false;
	m_bEnableAttackState = false;
	m_bEnableDefenseState = false;		
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
	m_iSkillInput = -1;
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}
	AttackAttribute cInfo;
	switch( iSpecialState )
	{
	case SS_NORMAL_S:	cInfo = m_NormalSAttribute;	break;
	case SS_NORMAL_A:	cInfo = m_NormalAAttribute;	break;
	case SS_CHARGE_S:	cInfo = m_ChargeSAttribute;	break;
	case SS_CHARGE_A:	cInfo = m_ChargeAAttribute;	break;
	}

	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->SetNormalAttackByAttribute( cInfo, true );

	int iAniID = pGrp->GetAnimationIdx( cInfo.m_AttackAnimation );
	float fTimeRate = cInfo.m_fAttackAniRate;
	DWORD dwPreDelay = cInfo.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwActionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * cInfo.m_fAttackAniRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << m_DashState;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << iSpecialState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::ProcessCuchulainMiddleAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		//쿠훌린 S키 조작
		if( pOwner->IsNeedProcess() && m_DashExtendType == DET_CUCHULAIN &&	pOwner->IsDefenseKey() && IsEnableCuchulainSpecialMove( pOwner ) )
		{
			SetCuchulainSpecialMove( pOwner );
			return;
		}

		pOwner->CheckExtraAniJump();

		if( m_dwReserveEnableEndTime == 0 && m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			CheckKeyReserve( pOwner );
		}
		else if( m_dwReserveEnableEndTime > 0 && m_dwReserveEnableTime > 0 && 
			m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime &&
			m_dwReserveEnableEndTime + pOwner->GetActionStopTotalDelay() >= dwCurTime )
		{
			CheckKeyReserve( pOwner );
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

void ioChargeComboDash::ProcessKeyReserve( ioBaseChar *pOwner )
{
	// 공격키가 눌러져 있으면 공격 상태 처리
	if( pOwner->IsAttackKeyDown() && 
		!pOwner->IsJumpKeyDown() &&
		!pOwner->IsDefenseKeyDown() &&
		!pOwner->IsChatModeState(true) &&
		!pOwner->IsFloating() )
	{	
		// 2020-06-25
		ioAttackableItem	*pAttackableItem = pOwner->GetPriorityItem( SPT_ATTACK );
		ioGwonwangItem* pGwonwangItem = ToGwonwangItem( pAttackableItem );
		if( pGwonwangItem && pGwonwangItem->IsCanFire( pOwner ) )
		{
			// 대기
			pOwner->ClearAttackFireTimeAndSkill();
			pOwner->ClearReservedSliding();
			pOwner->SetState( CS_DELAY );
			return;
		}

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
	ioUFCItem *pUFCItem = ToUFCItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pUFCItem && pUFCItem->CheckOnHand() && m_iSkillInput > 0 )
	{
		pUFCItem->ChangeToSpecialAttackFire( pOwner, ioUFCItem::SS_CHARGE_A + m_iSkillInput );
		m_iSkillInput = -1;
		return;
	}
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
	if( m_DashExtendType == DET_GHOST && pOwner->IsFloating() )
	{
		pOwner->SetChangeJumppingState( m_fGhostExtendDashJumpRate, false, 0 );
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
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetState( CS_DELAY );
}

bool ioChargeComboDash::CheckEnableLuciferFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_LUCIFER_FLY )
		return false;

	if( pOwner->IsEnableLuciferFly() )
		return true;

	return false;
}

//yunga
bool ioChargeComboDash::CheckEnableMechanicsFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_MECHANICS_FLY )
		return false;

	if( pOwner->IsEnableMechanicsFly() )
		return true;

	return false;
}

bool ioChargeComboDash::CheckEnableDestroyerFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_DESTROYER_FLY )
		return false;

	if( pOwner->IsEnableDestroyerFly() )
		return true;

	return false;
}

void ioChargeComboDash::SetLuciferFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetLuciferFlyState( m_DashLuciferFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_LUCIFER_FLY;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetMechanicsFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetMechanicsFlyState( m_DashMechanicsFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_MECHANICS_FLY;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableHadesState( ioBaseChar *pOwner )
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

void ioChargeComboDash::SetHadesHalfChargeState( ioBaseChar *pOwner )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioHadesItem			*pHadesItem = ToHadesItem( pItem );

	if( !pHadesItem )
		return;

	HadesAttackInfo dummyInfo;
	dummyInfo.Init();

	pHadesItem->SetHadesHalfAttackState( pOwner, dummyInfo, false, true, false, false, true );
}

void ioChargeComboDash::SetHadesFullChargeState( ioBaseChar *pOwner )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioHadesItem			*pHadesItem = ToHadesItem( pItem );

	if( !pHadesItem )
		return;

	HadesAttackInfo dummyInfo;
	dummyInfo.Init();

	pHadesItem->SetHadesAttackState( pOwner, dummyInfo, false, true, true );
}

bool ioChargeComboDash::CheckEnableAutomatonState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioAutomatonItem		*pAutomatonItem = ToAutomatonItem( pItem );

	if( pAutomatonItem )
	{
		if( pOwner->IsHasCrown() )
			return false;

		if( pAutomatonItem->IsEnableGauge() )
			return true;
	}
	return false;
}

void ioChargeComboDash::SetAutomatonState( ioBaseChar *pOwner )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioAutomatonItem		*pAutomatonItem = ToAutomatonItem( pItem );

	if( !pAutomatonItem )
		return;

	pAutomatonItem->SetSpecailAttackState( pOwner, true );
}

bool ioChargeComboDash::CheckEnableVulcanState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioVulcanItem		*pVulcanItem = ToVulcanItem( pItem );
	if( pVulcanItem )
	{
		if( pOwner->IsHasCrown() )
			return false;

		if( pVulcanItem->GetCurBullet() > 0 )
			return true;
	}
	return false;
}

void ioChargeComboDash::SetVulcanState( ioBaseChar *pOwner )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioVulcanItem		*pVulcanItem = ToVulcanItem( pItem );
	if( !pVulcanItem )
		return;

	pVulcanItem->ChangeToChargeAttackStart( pOwner );
}

bool ioChargeComboDash::CheckEnableChangeOnState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioChangeOnItem* pChangeOn = ToChangeOnItem( pItem );
	if( pChangeOn )
	{
		if( pOwner->IsHasCrown() )
			return false;

		if( pChangeOn->CheckChangeWereWolfState( pOwner ) )
			return true;
	}
	return false;
}

void ioChargeComboDash::SetChangeOnState( ioBaseChar *pOwner )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioChangeOnItem* pChangeOn = ToChangeOnItem( pItem );
	if( !pChangeOn )
		return;

	pChangeOn->ChangeToWereWolfState( pOwner, ioChangeOnItem::WSC_DASH );
}

bool ioChargeComboDash::CheckEnableChangeOnWolfState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioObjectWereWolfItem *pObjWereWolf = ToObjectWereWolfItem( pItem );
	if( pObjWereWolf )
		return true;

	return false;
}

void ioChargeComboDash::SetChangeOnWolfState( ioBaseChar *pOwner )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioObjectWereWolfItem *pObjWereWolf = ToObjectWereWolfItem( pItem );
	if( !pObjWereWolf )
		return;

	pObjWereWolf->ChangeToComboState( pOwner );
}

bool ioChargeComboDash::CheckEnableGhostState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioGhostItem *pGhostItem = ToGhostItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGhostItem && pGhostItem->IsEnableGhostGauge() )
		return true; 

	return false;
}

void ioChargeComboDash::SetGhostState( ioBaseChar *pOwner )
{
	ioGhostItem *pGhostItem = ToGhostItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pGhostItem )
		return;

	pGhostItem->SetGhostFlyState( pOwner, true );
}

bool ioChargeComboDash::CheckEnableMagicEngineerState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioMagicEngineerItem	*pMagicEngineerItem = ToMagicEngineerItem( pItem );
	if( pMagicEngineerItem && pMagicEngineerItem->GetDummyCharIndex() == -1 )
		return true;

	return false;
}

void ioChargeComboDash::SetMagicEngineerChargeState( ioBaseChar *pOwner, bool bHalf )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioMagicEngineerItem	*pMagicEngineerItem = ToMagicEngineerItem( pItem );
	if( !pMagicEngineerItem )
		return;

	pMagicEngineerItem->SetAttackState( pOwner, true, false, true, bHalf );
}

bool ioChargeComboDash::CheckEnableOmenState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioOmenItem		*pOmenItem = ToOmenItem( pItem );
	if( pOmenItem )
	{
		if( pOwner->IsHasCrown() )
			return false;

		if( pOmenItem->GetCurBullet() >= pOmenItem->GetNeedBulletDashChargeAttack() )
			return true;
	}
	return false;
}

void ioChargeComboDash::SetOmenState( ioBaseChar* pOwner )
{
	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioOmenItem		*pOmenItem = ToOmenItem( pItem );
	if( !pOmenItem )
		return;

	pOmenItem->ChangeToDashChargeAttack( pOwner );
}

bool ioChargeComboDash::CheckEnableMidnightState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_MIDNIGHT )
		return false;

	if( pOwner->IsEnableMidnightAttack() )
		return true;

	return false;
}

void ioChargeComboDash::SetMidnightState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioMidnightItem		*pMidnightItem = ToMidnightItem( pItem );
	if( !pMidnightItem )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetMidnightStateByDash( m_DashMidnightDashInfo );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_MIDNIGHT;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableAngelOfDeathPowerAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_ANGEL_OF_DEATH )
		return false;

	if( m_ExtendAttackList2.empty() )
		return false;

	return true;
}

void ioChargeComboDash::SetAngelOfDeathPowerAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iMaxCnt = m_ExtendAttackList2.size();

	int nCheckComboCnt = m_iCurComboCnt;
	//콤보타일경우 extran값으로 다시 가므로 서로 인덱스가 안맞는다.
	if( m_bSetCombo )
		nCheckComboCnt++;

	if( !COMPARE( nCheckComboCnt, 0, iMaxCnt ) )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	const AttackAttribute &rkAttribute = m_ExtendAttackList2[nCheckComboCnt];

	int iAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );
	m_szInputEventAnimation = rkAttribute.m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetAngelOfDeathPowerAttackState - %s Animation is Not Exist",
			rkAttribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = rkAttribute.m_fAttackAniRate;
	m_fInputEventAnimationRate = fTimeRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pOwner->RefreshFireTimeList( iAniID, rkAttribute.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + rkAttribute.m_dwEndDelay;

	DWORD dwEndTime = pGrp->GetAnimationEventTime_e( iAniID, "landing" );
	if( dwEndTime == 0 )
	{
		dwEndTime = pGrp->GetAnimationFullTime( iAniID );
	}

	int iEndAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackEndAnimation );
	if( iEndAniID == -1 )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

		CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if( fReserveTime > 0.0f )
		{
			m_dwPreReserveEnableTime = dwCurTime + fReserveTime;
		}

		float fWaitTime = dwEndTime * fTimeRate + dwPreDelay;
		float fEndTimeRate = rkAttribute.m_fAttackEndAniRate;

		pGrp->SetActionAni( iEndAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/fEndTimeRate, fWaitTime, 0.0f, true );

		CheckKeyReserveTime( pOwner, iEndAniID, fEndTimeRate );
		if( m_dwReserveEnableTime > 0 )
		{
			m_dwReserveEnableTime = dwDuration;
		}

		dwDuration += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate + rkAttribute.m_dwEndDelay;
	}

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime = dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_bSetCombo = true;
	m_DashState = DS_EXTEND_COMBO2;

	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

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
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_EXTEND_COMBO2;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 이후
	m_iCurComboCnt++;
}

bool ioChargeComboDash::CheckEnableExtendComboState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_EXTEND_COMBO && m_DashExtendType != DET_RED_HOOD && 
		m_DashExtendType != DET_BLACK_SPHEAR && m_DashExtendType != DET_UFC && 
		m_DashExtendType != DET_GHOST && m_DashExtendType != DET_HANZO )
		return false;

	if( m_ExtendAttackList.empty() )
		return false;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioChargeComboBowItem *pChargeComboBowItem = ToChargeComboBowItem( pItem );
	if( pChargeComboBowItem )
	{
		if( pChargeComboBowItem->GetCurBullet() == 0 )
			return false;
	}

	return true;
}

void ioChargeComboDash::SetExtendComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iMaxCnt = m_ExtendAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int nCurComboCnt = 0;
	nCurComboCnt = m_iCurComboCnt;

	if( m_DashExtendType == DET_RED_HOOD && m_bNormalAttacked )
		nCurComboCnt = m_iCurComboCnt+1;

	const AttackAttribute &rkAttribute = m_ExtendAttackList[nCurComboCnt];

	int iAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackAnimation );
	m_szInputEventAnimation = rkAttribute.m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetExtendComboState - %s Animation is Not Exist",
								rkAttribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = rkAttribute.m_fAttackAniRate;
	m_fInputEventAnimationRate = fTimeRate;
	DWORD dwPreDelay = rkAttribute.m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pOwner->RefreshFireTimeList( iAniID, rkAttribute.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + rkAttribute.m_dwEndDelay;

	DWORD dwEndTime = pGrp->GetAnimationEventTime_e( iAniID, "landing" );
	if( dwEndTime == 0 )
	{
		dwEndTime = pGrp->GetAnimationFullTime( iAniID );
	}

	int iEndAniID = pGrp->GetAnimationIdx( rkAttribute.m_AttackEndAnimation );
	if( iEndAniID == -1 )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

		CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if( fReserveTime > 0.0f )
		{
			m_dwPreReserveEnableTime = dwCurTime + fReserveTime;
		}

		float fWaitTime = dwEndTime * fTimeRate + dwPreDelay;
		float fEndTimeRate = rkAttribute.m_fAttackEndAniRate;

		pGrp->SetActionAni( iEndAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/fEndTimeRate, fWaitTime, 0.0f, true );

		CheckKeyReserveTime( pOwner, iEndAniID, fEndTimeRate );
		if( m_dwReserveEnableTime > 0 )
		{
			m_dwReserveEnableTime = dwDuration;
		}

		dwDuration += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate + rkAttribute.m_dwEndDelay;
	}

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime = dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	pOwner->SetReservedSliding( rkAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_dwFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_bSetCombo = true;
	m_DashState = DS_EXTEND_COMBO;

	SetMacroTime( pGrp, iAniID, fTimeRate, rkAttribute.m_dwEndDelay);

	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

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
	
	ioOniMushaItem* pOniMushaItem = ToOniMushaItem( pOwner->GetWeapon() );
	if ( pOniMushaItem )
		pOniMushaItem->SetUseExtraGauge( m_fOnimushaObjectChargeUseGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_EXTEND_COMBO;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 이후
	m_iCurComboCnt++;
}

bool ioChargeComboDash::CheckEnablePhantomFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_PHANTOM_FLY )
		return false;

	if( pOwner->IsEnablePhantomFly() )
		return true;

	return false;
}

void ioChargeComboDash::SetPhantomFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetPhantomFlyStateAttack();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_PHANTOM_FLY;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetDestroyerFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetDestroyerFlyState( m_DestroyerFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_DESTROYER_FLY;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::ProcessExtendComboDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	ioParkilpyoItem *pParkilpyoItem = ToParkilpyoItem( pOwner->GetWeapon() );
	if( pParkilpyoItem && pParkilpyoItem->CheckCmdInputState( pOwner ) )
		return;

	if ( m_bExtendDashInpuDashEnable && CheckAniDash_Input_Speical_Dash( pOwner ) )
		return;

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();
		CheckAniRotate( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( pOwner->IsNeedProcess() && m_DashExtendType == DET_UFC && m_dwReserveEnableTime < dwCurTime )
		{
			if( pOwner->IsDefenseKey() )
			{
				ChangeToSpecialAttackFire( pOwner, SS_CHARGE_S );
				return;
			}
			else if( pOwner->IsJumpKey() )
			{
				ChangeToSpecialAttackFire( pOwner, SS_CHARGE_A );
				return;
			}
		}

		if( pOwner->IsNeedProcess() && CheckEnableHanzoState( pOwner ) && m_dwReserveEnableTime < dwCurTime )
		{
			if( pOwner->IsDefenseKey() )
			{
				SetHanzoState( pOwner, SS_CHARGE_S );
				return;
			}
			else if( pOwner->IsJumpKey() )
			{
				SetHanzoState( pOwner, SS_CHARGE_A );
				return;
			}
		}

		if( m_dwPreReserveEnableTime > 0 && m_dwPreReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			if( CheckPreReserve( pOwner ) && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return;
			}
		}

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			int iMaxCnt = (m_DashState == DS_EXTEND_COMBO ? m_ExtendAttackList.size() : m_ExtendAttackList2.size());

			if( m_DashExtendType == DET_RED_HOOD || m_DashExtendType == DET_MAGICENGINEER )
			{
				CheckExtraReserve( pOwner );
			}
			else
			{
				if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
					CheckExtraReserve( pOwner );
				else
					CheckExtendReserve( pOwner );
			}
			
			if( m_bAniJumpInputS )
				CheckAniJump_Input_Jump_S( pOwner );

			return;
		}

		if( m_dwFireTime > 0 && m_dwFireTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			m_dwFireTime = 0;
			CheckBaseballBatting( pOwner );
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

bool ioChargeComboDash::CheckPreReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( pOwner->IsJumpKey() )
	{
		return true;
	}

	return false;
}

void ioChargeComboDash::CheckExtendReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

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
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		if( m_DashState == DS_EXTEND_COMBO2 )
		{
			m_bEnableJumpState = false;
			m_bEnableAttackState = true;
			m_bEnableDefenseState = false;
			m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
			m_iSkillInput = -1;
		}
		else
		{
			ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

			if( pOwner->IsSettedDirection() )
			{
				float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				pOwner->SetTargetRotToRotate( qtRotate, true, false );
			}
			if( m_DashExtendType != DET_GHOST )
				SetExtendComboState( pOwner );
			else
			{
				ChangeToExtendCharging( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_EXTEND_DASH );
					kPacket << pOwner->GetCharName();
					kPacket << (int)GetType();
					kPacket << pOwner->GetHP().m_fCurValue;
					kPacket << DST_STATE;
					kPacket << DS_EXTEND_CHARGE;
					kPacket << m_iCurComboCnt;
					kPacket << pOwner->GetTargetRot();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}

		return;
	}
}

void ioChargeComboDash::ProcessFlyDashAttackLanding( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime <= dwCurTime )
	{
		pOwner->SetCurMoveSpeed( 0.0f );
		pOwner->ClearAttackFireTimeAndSkill();
		pOwner->ClearReservedSliding();
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetState( CS_DELAY );
	}
}

void ioChargeComboDash::ProcessFlyDashAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;

			if( pOwner->IsAttackKey() )
			{
				SetFlyDashAttackStateEnd( pOwner );
				return;
			}
		}
		pOwner->SetCurMoveSpeed( m_fFlyDashSpeed );
	}
	else
	{
		SetFlyDashAttackLandingState( pOwner );
	}
}

void ioChargeComboDash::ProcessRunRotate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			// 점프키 누름. 점프상태 전환
			bool bEnableJumpTime = false;
			if( m_dwRunRotateStartTime+m_dwRunRotateJumpTime < dwCurTime )
				bEnableJumpTime = true;
			
			if( bEnableJumpTime && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
			{
				if( m_DashExtendType == DET_HERACLES )
				{
					ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
					if( pItem )
					{
						ioHeraclesItem* pHeracles = ToHeraclesItem( pItem );
						if( pHeracles && pHeracles->IsEnableJumpGauge() )
						{
							pOwner->ClearAttackFireTimeAndSkill();
							pHeracles->SetCancelJump( pOwner );
						}
					}
				}
				else
				{
					pOwner->SetJumpState();
				}
				return;
			}
			if( !pOwner->IsAttackKeyDown() )
				m_bRotateFirstAttack = false;
	
			if( m_bRotateFirstAttack && m_bRotateChargeEnable && m_dwRunRotateStartTime + m_dwRunRotateChargeTime < dwCurTime )
			{
				SetRunRotateChargeState( pOwner, true );
				return;
			}
			if( !m_bRotateFirstAttack && m_dwReserveKeyTime < dwCurTime && pOwner->IsAttackKeyDown() && CheckRunRotateState( pOwner, true ) )
			{
				SetRunRotateState( pOwner, true, false );
				return;
			}
			
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );

		pOwner->SetCurMoveSpeed( m_fRunSpeed );
	}
	else
	{
		SetRunRotateStateEnd( pOwner );
	}
}

void ioChargeComboDash::ProcessRunRotateEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			CheckExtraReserve( pOwner );
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

void ioChargeComboDash::ProcessFlyDashAttackEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			CheckExtraReserve( pOwner );
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

bool ioChargeComboDash::CheckRunRotateState( ioBaseChar *pOwner, bool bCheckComboCount )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_RUN_ROTATE )
		return false;

	if( bCheckComboCount && m_iCurComboCnt >= m_iRunRotateAttackMaxCount )
		return false;

	return true;
}

bool ioChargeComboDash::CheckRunRotateState2( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_RUN_ROTATE2 && m_DashExtendType != DET_HERACLES )
		return false;

	return true;
}

//Fly Dash Attack
bool ioChargeComboDash::CheckEnableFlyDashAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_FLY_DASH_ATTACK )
		return false;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDevilItem* pDevilItem = ToDevilItem( pItem );
	if( !pDevilItem )
		return false;

	if( pDevilItem->GetCurGauge() < m_fFlyDashDecreseGauge )
		return false;

	return true;
}

void ioChargeComboDash::SetFlyDashAttackState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;


	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDevilItem* pDevilItem = ToDevilItem( pItem );
	if( !pDevilItem )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_FlyDashAttack.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetFlyDashAttackState - %s Animation is Not Exist",
			m_FlyDashAttack.m_AttackAnimation.c_str() );
		return;
	}

	pDevilItem->DecreaseFlyDashGauge( m_fFlyDashDecreseGauge );

	float fTimeRate = m_FlyDashAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_FlyDashAttack.m_dwPreDelay;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwFlyDashAttackStartTime = dwCurTime;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, m_FlyDashAttack.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_FlyDashAttack.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;//m_dwFlyDashDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_FlyDashAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_FLY_DASH_ATTACK;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_FLY_DASH_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << dwCurBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::CheckAniRotate( ioBaseChar *pChar )
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

void ioChargeComboDash::SetRunRotateState( ioBaseChar *pOwner, bool bSendNet, bool bFirstAttack )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iCurCombo = m_iCurComboCnt;
	if( !bFirstAttack )
	{
		pOwner->InitActionStopDelay( true );
		m_iCurComboCnt = iCurCombo;
	}
	AttackAttribute cAttackInfo;
	int iAttackCount = m_RunRotateAttackList.size();
	if( COMPARE( m_iCurComboCnt, 0, iAttackCount ) )
		cAttackInfo = m_RunRotateAttackList[m_iCurComboCnt];
	else if( !m_RunRotateAttackList.empty() )
		cAttackInfo = m_RunRotateAttackList[0];
	else
		return;

	int iAniID = pGrp->GetAnimationIdx( cAttackInfo.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetRunRotateState - %s Animation is Not Exist",
			cAttackInfo.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = cAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = cAttackInfo.m_dwPreDelay;
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwRunRotateStartTime = dwCurTime;
	m_dwReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_bRotateFirstAttack = bFirstAttack;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, cAttackInfo.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + cAttackInfo.m_dwEndDelay;

	DWORD dwRunDuration;
	int iCount = m_vRunDurationVec.size();
	if( COMPARE( m_iCurComboCnt, 0, iCount ) )
		dwRunDuration = m_vRunDurationVec[m_iCurComboCnt];
	else if( !m_vRunDurationVec.empty() )
		dwRunDuration = m_vRunDurationVec[0];
	else
		dwRunDuration = dwDuration;

	m_dwActionEndTime = dwCurTime + dwRunDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( cAttackInfo.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_RUN_ROTATE;

	SetRunRotateBuff( pOwner );

	m_iCurComboCnt++;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_RUN_ROTATE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_bRotateFirstAttack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetRunRotateChargeState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;
	m_bRotateFirstAttack = false;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetRunRotateChargeState();

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_ROTATE_CHARGE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetFlyDashAttackLandingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_FlyDashLandingAni );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetFlyDashAttackLandingState - %s Animation is Not Exist",
			m_FlyDashLandingAni.c_str() );
		return;
	}

	float fTimeRate = m_fFlyDashLandingAniRate;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwFlyDashAttackStartTime = dwCurTime;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwActionEndTime = dwCurTime + dwDuration;//m_dwFlyDashDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_DashState = DS_FLY_DASH_ATTACK_LANDING;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
}

void ioChargeComboDash::SetFlyDashAttackStateEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_FlyDashAttackEnd.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetFlyDashAttackStateEnd - %s Animation is Not Exist",
			m_FlyDashAttackEnd.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = m_FlyDashAttackEnd.m_fAttackAniRate;
	DWORD dwPreDelay = m_FlyDashAttackEnd.m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();

	pOwner->RefreshFireTimeList( iAniID, m_FlyDashAttackEnd.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_FlyDashAttackEnd.m_dwEndDelay;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_FlyDashAttackEnd.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_FLY_DASH_ATTACK_END;

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;

	pOwner->SetCurMoveSpeed( 0.0f );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_FLY_DASH_ATTACK_END;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetRunRotateStateEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_RunRotateAttackEnd.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetRunRotateStateEnd - %s Animation is Not Exist",
			m_RunRotateAttackEnd.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = m_RunRotateAttackEnd.m_fAttackAniRate;
	DWORD dwPreDelay = m_RunRotateAttackEnd.m_dwPreDelay;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();

	pOwner->RefreshFireTimeList( iAniID, m_RunRotateAttackEnd.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_RunRotateAttackEnd.m_dwEndDelay;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_RunRotateAttackEnd.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_RUN_ROTATE_END;

	RemoveRundRotateBuff( pOwner );

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_RUN_ROTATE_END;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << DST_KEY;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioChargeComboDash::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwRotateSpeed <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
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

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

void ioChargeComboDash::ApplyDashState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurComboCnt;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true, false );

	bool bCharge;

	switch( iState )
	{
	case DS_NONE:
		StartDash( pOwner );
		break;
	case DS_CHARGE:
		ChangeToCharging( pOwner );
		break;
	case DS_NORMAL_ATTACK:
		{
			bool bCancelAttack;
			rkPacket >> bCancelAttack;
			rkPacket >> bCharge;
			ChangeToNormalAttack( pOwner, bCancelAttack, bCharge );
		}
		break;
	case DS_COMBO_ATTACK:
		rkPacket >> bCharge ;
		SetComboDash( pOwner, bCharge );
		break;
	case DS_CUCHULAIN_MIDDLE_ATTACK_STATE:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			ChangeCuchulainMiddleAttack( pOwner );
		}
		break;
	case DS_BUFF_ADD:
		SetBuffAddState( pOwner );
		break;
	case DS_MECHANICS_FLY:
		SetMechanicsFlyState( pOwner );
		break;
	case DS_EXTEND_COMBO:
		SetExtendComboState( pOwner );
		break;
	case DS_EXTEND_COMBO2:
		SetAngelOfDeathPowerAttackState( pOwner );
		break;
	case DS_RUN_ROTATE:
		rkPacket >> m_bRotateFirstAttack;
		SetRunRotateState( pOwner, false, m_bRotateFirstAttack );
		break;
	case DS_RUN_ROTATE_END:
		SetRunRotateStateEnd( pOwner );
		break;
	case DS_POWER_CHARGE:
		SetPowerChargeState( pOwner, false );
		break;
	case DS_POWER_CHARGE_ATTACK:
		{
			rkPacket >> m_fCurChargeRate;

			SetPowerChargeAttack( pOwner );
		}
		break;
	case DS_PHANTOM_FLY:
		SetPhantomFlyState( pOwner );
		break;
	case DS_LUCIFER_FLY:
		SetLuciferFlyState( pOwner );
		break;
	case DS_DESTROYER_FLY:
		SetDestroyerFlyState( pOwner );
		break;
	case DS_MICHAEL_FLY:
		SetMichaelFlyState( pOwner );
		break;
	case DS_GOOMIHO_FLY:
		SetGoomihoFlyState( pOwner );
		break;
	case DS_SUCCUBUS_CHAIN:
		SetSuccubusState( pOwner );
		break;
	case DS_DRACULA:
		SetDraculaState( pOwner );
		break;
	case DS_FLY_DASH_ATTACK:
		{
			DWORD dwCurBaseIndex;
			rkPacket >> dwCurBaseIndex;

			pOwner->SetWeaponIndexBase( dwCurBaseIndex );
			SetFlyDashAttackState( pOwner, false );
		}
		break;
	case DS_FLY_DASH_ATTACK_END:
		SetFlyDashAttackStateEnd( pOwner );
		break;
	case DS_FLASH:
		SetFlashState( pOwner );
		break;
	case DS_EVY:
		SetEvyState( pOwner );
		break;
	case DS_SLASHER:
		SetSlasherState( pOwner );
		break;
	case DS_TOP_BL_EXTEND:
		SetTopBlExtendMod( pOwner );
		break;
	case DS_TOP_BL_RETURN:
		SetTopBlExtendAttack( pOwner );
		break;
	case DS_KAELTHAS_EXTEND:
		SetKaelthasExtendAttack( pOwner );
		break;
	case DS_DRAGONNEST_ASS:
		{
			DWORD dwCurBaseIndex;
			rkPacket >> dwCurBaseIndex;

			pOwner->SetWeaponIndexBase( dwCurBaseIndex );
			SetDragonNestAssExtendPreState( pOwner );
		}
		break;
	case DS_THUNDER_BIRD:
		SetThunderBirdState( pOwner );
		break;
	case DS_EQUIP_OBJECT:
		SetObjectEquipState( pOwner );
		break;
	case DS_DELAY_STATE:
		CheckMaxComboSetDelayState( pOwner );
		break;
	case DS_LEE_DRAGON_ATTACK_STATE:
		SetLeeDragonAttackState( pOwner );
		break;
	case DS_PARKILPYO_NORMAL_GATHER:
		ChangeToParkilpyoNormalGathering( pOwner );
		break;
	case DS_PARKILPYO_NORMAL_CHARGE_ATTACK:
		ChangeToParkilpyoNormalChargeAttack( pOwner );
		break;
	case DS_PARKILPYO_NORMAL_CHARGE_COMBO:
		{
			int nCurKey;
			rkPacket >> nCurKey;
			rkPacket >> m_nCurChargeCombo;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			ChangeToParkilpyoNormalChargeCombo( pOwner );
		}
		break;
	case DS_PARKILPYO_NORMAL_EQUIP:
		SetParkilpyoNormalEquipState( pOwner );
		break;
	case DS_PARKILPYO_OBJECT_CHARGE_ATTACK:
		ChangeToParkilpyoObjectChargeAttack( pOwner );
		break;
	case DS_JACK_CHARGE_ATTACK:
		ChangeToJackChargeAttack( pOwner );
		break;
	case DS_SHADOW_RANGER:
		SetShadowRangerMoveState( pOwner );
		break;
	case DS_KEY_INPUT_LAST_ATTACK:
		{
			rkPacket >> m_bEnableAttackState;
			rkPacket >> m_bEnableDefenseState;
			rkPacket >> m_bEnableJumpState;
			ChangeToKeyInputAttack( pOwner );
		}
		break;
	case DS_BULLFIGHT:
		SetBullFightState( pOwner );
		break;
	case DS_JEONWOOCHI_DASH:
		{
			int nJeonWooChiState;
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			rkPacket >> nJeonWooChiState;

			pOwner->SetWorldPosition( vPos );

			if( nJeonWooChiState == JWCST_POWER_CHARGE )
			{
				SetJeonWooChiChargeState( pOwner );
			}
			else if( nJeonWooChiState == JWCST_DASH_STATE )
			{
				SetJeonWooChiDashState( pOwner );
			}
			else if( nJeonWooChiState == JWCST_DASH_ATTACK )
			{
				SetJeonWooChiDashAttack( pOwner );
			}
			else if( nJeonWooChiState == JWCST_DASH_ADD_ATTACK )
			{
				SetJeonWooChiDashAddAttack( pOwner );
			}
		}
		break;
	case DS_ONIMUSHA_CHARGE_ATTACK:
		ChangeToOniMushaChargeAttack( pOwner );
		break;
	case DS_ONIMUSHA_RUN:
		ChangeToOniMushaRunState( pOwner );
		break;
	case DS_HALF_EXTEND_ATTACK:
		ChangeHalfExtendAttack( pOwner );
		break;
	case DS_SASINMU:
		SetSasinmuState( pOwner );
		break;
	case DS_BUBBLE_FIGHTER_ATTACK:
		SetBubbleFighterDashAttack( pOwner );
		break;
	case DS_NIGHTMARE:
		SetNightmareState( pOwner );
		break;
	case DS_AZAZEL:
		{
			rkPacket >> m_AzazelAimedTarget;
			SetAzazelState( pOwner, false );
		}
		break;
	case DS_ROTATE_CHARGE:
		SetRunRotateChargeState( pOwner, false );
		break;
	case DS_SPECIAL_ATTACK:
		{
			int iSpecialState;
			rkPacket >> iSpecialState;
			ChangeToSpecialAttackFire( pOwner, iSpecialState );
		}
		break;
	case DS_EXTEND_CHARGE:
		ChangeToExtendCharging( pOwner );
		break;
	case DS_MIDNIGHT:
		SetMidnightState( pOwner );
		break;
	}
}

void ioChargeComboDash::ApplyDashDirKey( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true, false );

	int iDir;
	rkPacket >> iDir;

	m_bTargetRot = false;
	m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
}

bool ioChargeComboDash::CheckJumpRunRoateEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_RunRotateAttackEnd.m_AttackAnimation.IsEmpty() )
		return false;

	int iAniID = pGrp->GetAnimationIdx( m_RunRotateAttackEnd.m_AttackAnimation );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_RunRotateAttackEnd.m_fAttackAniRate;

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


bool ioChargeComboDash::CheckAniJump_Input_Jump_S_Exception( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_szInputEventAnimation.IsEmpty() )
		return false;

	int iAniID = pGrp->GetAnimationIdx( m_szInputEventAnimation );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fInputEventAnimationRate;

		dwTime += m_dwActionStartTime;

		//다른 로직에서 input_jump_s의 기능이 key_reserve를 체크하지 않는데 해당로직에서만 체크하고 있어 버그로 판단하여 수정
		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			if( !pOwner->IsBuffLimitJump() )
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

bool ioChargeComboDash::CheckAniJump_Input_Jump_S( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_szInputEventAnimation.IsEmpty() )
		return false;

	int iAniID = pGrp->GetAnimationIdx( m_szInputEventAnimation );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fInputEventAnimationRate;

		dwTime += m_dwActionStartTime;
		
		//다른 로직에서 input_jump_s의 기능이 key_reserve를 체크하지 않는데 해당로직에서만 체크하고 있어 버그로 판단하여 수정
		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
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

//// 2020-05-25 일반 대시 공격중 캔슬 차지 기능 
//bool ioChargeComboDash::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
//{
//	if( m_szInputEventAnimation.IsEmpty() )
//		return false;
//
//	if( m_dwInputCancelCheckTime == 0 )
//		return false;
//
//	if( !pOwner )
//		return false;
//
//	ioEntityGroup *pGrp = pOwner->GetGroup();
//	if( !pGrp )
//		return false;
//
//	DWORD dwCurTime = FRAMEGETTIME();
//	int iAniID = pGrp->GetAnimationIdx( m_szInputEventAnimation );			
//	DWORD dwInputSpecialTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
//	if( 0 < dwInputSpecialTime )
//	{
//		dwInputSpecialTime *= m_fInputEventAnimationRate;
//		dwInputSpecialTime += m_dwInputCancelCheckTime;		
//	}
//
//	if( 0 < dwInputSpecialTime && dwCurTime > dwInputSpecialTime )
//	{
//		ioAttackableItem *pAttack = pOwner->GetPriorityItem( SPT_ATTACK );
//		if( pAttack && pAttack->IsConditionalSpecialDashReserve( pOwner ) && m_eSpecialDashCurrDirkey != ioUserKeyInput::DKI_NONE )
//		{
//			if( pOwner->IsDirKeyDoubleClick() && pAttack->IsConditionalSpecialDashReserve( pOwner ) )
//			{
//				pAttack->SetChangeConditionalSpecialDash( pOwner, m_eSpecialDashCurrDirkey, ioAttackableItem::CCSD_DASH_ATTACK );
//				return true;
//			}
//		}
//
//	}
//
//	return false;
//}

bool ioChargeComboDash::CheckAniDash_Input_Speical_Dash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_szInputEventAnimation.IsEmpty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szInputEventAnimation );		
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fInputEventAnimationRate;
		dwInputSpecialDashTime += m_dwActionStartTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		ioAttackableItem *pAttack = pOwner->GetPriorityItem( SPT_ATTACK );
		if( pAttack && pAttack->IsConditionalSpecialDashReserve( pOwner ) && m_eSpecialDashCurrDirkey != ioUserKeyInput::DKI_NONE )
		{			
			if ( m_DashExtendType == DET_JACK )
			{
				if ( m_nCurReserveDash < m_nMaxReserveDash )
				{
					pOwner->SetTargetRotToDirKey( m_eSpecialDashCurrDirkey );

					m_iCurComboCnt = 0;
					m_nCurReserveDash++;

					ChangeToNormalAttack( pOwner );
				}
			}
			else
				pAttack->SetChangeConditionalSpecialDash( pOwner, m_eSpecialDashCurrDirkey, ioAttackableItem::CCSD_DASH_ATTACK );

			m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
			return true;
		}
		return false;
	}

	return false;
}


bool ioChargeComboDash::CheckPowerChargeState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_POWER_CHARGE )
		return false;

	return true;
}

void ioChargeComboDash::SetPowerChargeState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_PowerChargeAni, FLOAT100 );

	m_DashState = DS_POWER_CHARGE;
	m_dwChargeStartTime = dwCurTime;
	m_fCurChargeRate = 0.0f;

	if( !m_DashGatheringEffect.IsEmpty() )
	{
		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}

		ioEffect *pEffect = pOwner->AttachEffect( m_DashGatheringEffect );
		if( pEffect )
			m_GatheringEffectID = pEffect->GetUniqueID();
	}

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_POWER_CHARGE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetPowerChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	m_DashState = DS_POWER_CHARGE_ATTACK;

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetNoWoundState( false );

	if( pOwner->IsNeedProcess() )
	{
		DWORD dwGapTime = dwCurTime - m_dwChargeStartTime;
		if( m_dwChargeStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				m_fCurChargeRate = FLOAT1;

			m_fCurChargeRate = min( m_fCurChargeRate, FLOAT1 );
		}
	}

	float fExtraAniRate, fExtraForceRate;
	fExtraAniRate = fExtraForceRate = FLOAT1;

	if( m_fMaxChargeAniRate > 0.0f )
	{
		float fCurMaxAniRate = max( 0.0f, (m_fMaxChargeAniRate - FLOAT1) );
		fExtraAniRate += fCurMaxAniRate * m_fCurChargeRate;
	}

	if( m_fMaxForceSlidingRate > 0.0f )
	{
		float fCurMaxSlidingRate = max( 0.0f, (m_fMaxForceSlidingRate - FLOAT1) );
		fExtraForceRate += fCurMaxSlidingRate * m_fCurChargeRate;
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_PowerChargeAttack.m_AttackAnimation );
	DWORD dwPreDelay = m_PowerChargeAttack.m_dwPreDelay;
	float fTimeRate = m_PowerChargeAttack.m_fAttackAniRate * fExtraAniRate;

	pOwner->SetAutoTarget( ATT_DASH );

	pOwner->RefreshFireTimeList( iAniID, m_PowerChargeAttack.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_PowerChargeAttack.m_dwEndDelay;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_PowerChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, fExtraForceRate );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	if( !m_DashGatheringEffect.IsEmpty() )
	{
		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_POWER_CHARGE_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurChargeRate;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::ProcessPowerCharge( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			SetPowerChargeAttack( pOwner );
			return;
		}
	}

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );
}


void ioChargeComboDash::ProcessTopBlReturn( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwChargeAttackDummyFireTime && dwCurTime > m_dwChargeAttackDummyFireTime )
	{
		m_dwChargeAttackDummyFireTime = 0;

		ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
		ioDummyCharItem2 *pDummyItem2 = ToDummyCharItem2( pItem );
		if( pDummyItem2 )
		{
			if( m_TopBl_ChargeInfo.bCreate )
				pDummyItem2->ChangeToDummyChar( pOwner );

			pDummyItem2->SetMoveDummyPos( pOwner, m_TopBl_ChargeInfo );
		}
	}

	if( dwCurTime > m_dwActionEndTime )
		pOwner->SetState( CS_DELAY );
}


void ioChargeComboDash::ProcessJeonWooChiDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_JeonWooChiState )
	{
	case JWCST_POWER_CHARGE:
		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKey() )
			{
				SetJeonWooChiDashState( pOwner );
				return;
			}

			if( dwCurTime > m_dwActionEndTime )
			{
				pOwner->SetState( CS_DELAY );
				return;
			}
		}
		break;
	case JWCST_DASH_STATE:
		if( m_fJeonWooChiCurMoveLength < m_fJeonWooChiNeedMoveLength )
			OnJeconWooChiDashMove( pOwner );
		else if( pOwner->IsNeedProcess() )
			SetJeonWooChiDashAttack( pOwner );
		
		break;
	case JWCST_DASH_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			if( dwCurTime > m_dwReserveEnableTime && pOwner->IsAttackKey() )
			{
				SetJeonWooChiDashAddAttack( pOwner );
				return;
			}

			if( dwCurTime > m_dwActionEndTime )
				pOwner->SetState( CS_DELAY );
		}
		break;
	case JWCST_DASH_ADD_ATTACK:
		if( dwCurTime > m_dwActionEndTime )
			pOwner->SetState( CS_DELAY, false );
		break;
	}
}


void ioChargeComboDash::ProcessLeeDragonAttackState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();
		CheckAniRotate( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( m_dwPreReserveEnableTime > 0 && m_dwPreReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			if( CheckPreReserve( pOwner ) && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return;
			}
		}

		if( pOwner->IsNeedProcess() && m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			if( pOwner->IsAttackKey() )
			{
				ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

				if( pOwner->IsSettedDirection() )
				{
					float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
					fYawD = ioMath::ArrangeHead( fYawD );

					float fYawR = DEGtoRAD( fYawD );
					D3DXQUATERNION qtRotate;
					D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

					pOwner->SetTargetRotToRotate( qtRotate, true, false );
				}

				m_iCurComboCnt = 0;
				ChangeToNormalAttack( pOwner, true );
			}
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

void ioChargeComboDash::ProcessPowerChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			if( pOwner->IsAttackKey() && !m_ExtendAttackList.empty() )
			{
				m_iCurComboCnt = 0;
				SetExtendComboState( pOwner );
				return;
			}
			else
			{
				CheckExtraReserve( pOwner );
				return;
			}
		}
	}
	else
	{
		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			if( !m_ExtendAttackList.empty() )
			{
				m_iCurComboCnt = 0;
				SetExtendComboState( pOwner );
			}
			else if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
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
			if( !m_ExtendAttackList.empty() )
			{
				m_iCurComboCnt = 0;
				SetExtendComboState( pOwner );
			}
			else if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}

			return;
		}

		// 대기
		pOwner->ClearAttackFireTimeAndSkill();
		pOwner->ClearReservedSliding();
		pOwner->SetState( CS_DELAY );
	}
}

void ioChargeComboDash::CheckChargeInfo( ioWeapon *pWeapon )
{
	if( !pWeapon ) return;

	float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
	float fCurScaleRate, fCurExplosionRate, fCurWoundedTimeRate, fCurLiveTimeRate;

	fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
	fCurScaleRate = fCurExplosionRate = fCurWoundedTimeRate = fCurLiveTimeRate = FLOAT1;

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

bool ioChargeComboDash::GetAutoTargetValue( IN float &fDashRange, IN float &fDashMinAngle, IN float &fDashMaxAngle,
											OUT float &fRange, OUT float &fMinAngle, OUT float &fMaxAngle )
{
	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	if( m_DashState != DS_POWER_CHARGE_ATTACK )
		return false;

	float fCurTargetRangeRate, fCurTargetAngleRate;
	fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

	float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
	float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

	fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
	fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

	fRange = fDashRange * fCurTargetRangeRate;
	fMinAngle = fDashMinAngle / fCurTargetAngleRate;
	fMaxAngle = fDashMaxAngle / fCurTargetAngleRate;

	return true;
}


bool ioChargeComboDash::IsCanPlayWoundedAniFromExtendDash()
{
	if( m_bPlayDashExtendWoundAni )
		return m_DashState==DS_EXTEND_COMBO ? true : false; 

	return false;

}
bool ioChargeComboDash::IsEnableDefense()
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DashState )
	{
	case DS_POWER_CHARGE:
		if( m_bPowerChargeUseDefense )
			return true;
		break;
	}

	return false;
}

bool ioChargeComboDash::IsPowerChargingState()
{
	switch( m_DashState )
	{
	case DS_POWER_CHARGE:
		return true;
	}

	return false;
}

bool ioChargeComboDash::CheckEnableMichaelFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_MICHAEL_FLY )
		return false;

	if( pOwner->IsEnableMichaelFly() )
		return true;

	return false;
}

void ioChargeComboDash::SetMichaelFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetMichaelFlyStateByJumpDash( m_DashMichaelFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_MICHAEL_FLY;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableGoomihoFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_GOOMIHO_FLY )
		return false;

	if( pOwner->IsEnableGoomihoFly() )
		return true;

	return false;
}

void ioChargeComboDash::SetGoomihoFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetGoomihoFlyStateByJumpDash( m_DashGoomihoFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_GOOMIHO_FLY;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableSuccubusState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_SUCCUBUS_CHAIN )
		return false;

	if( pOwner->IsEnableSuccubusAttack() )
		return true;

	return false;
}

void ioChargeComboDash::SetSuccubusState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetSuccubusStateByDash( m_DashSuccubusChain );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_SUCCUBUS_CHAIN;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioChargeComboDash::CheckEnableEvyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_EVY )
		return false;

	return true;
}


void ioChargeComboDash::SetEvyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetEvyBlinkStateByJumpDash(m_EvyBlinkInfo);

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_EVY;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableDraculaState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_DRACULA )
		return false;

	if( pOwner->IsEnableDraculaFly() )
		return true;

	return false;
}

void ioChargeComboDash::SetDraculaState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetDraculaFlyState(m_DashDraculaFly);

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_DRACULA;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableNightmareState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_NIGHTMARE )
		return false;

	if( pOwner->IsEnableNightmareFly() )
		return true;

	return false;
}

void ioChargeComboDash::SetNightmareState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	bool bFly = pOwner->SetNightmareFlyState();

	if( bFly && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_NIGHTMARE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableAzazelState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_AZAZEL )
		return false;

	if( pOwner->IsEnableAzazelAttack() )
		return true;

	return false;
}

bool ioChargeComboDash::CheckEnableAzazelSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_AZAZEL )
		return false;

	if( pOwner->IsEnableAzazelSpecialAttack() )
		return true;

	return false;
}

void ioChargeComboDash::SetAzazelState( ioBaseChar *pOwner, bool bAim )
{
	if( !pOwner )
		return;

	m_DashState = DS_AZAZEL;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_ExtendAttackList[0].m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	ioAzazelItem *pAzazel = ToAzazelItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAzazel )
		pAzazel->CheckDecreaseGauge();

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	DWORD dwPreDelay = m_ExtendAttackList[0].m_dwPreDelay;
	float fTimeRate = 1.0f;
	if ( m_ExtendAttackList[0].m_fAttackAniRate > 0.0f )
		fTimeRate = m_ExtendAttackList[0].m_fAttackAniRate;

	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;
	m_dwActionEndTime = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_ExtendAttackList[0].m_dwEndDelay;
	m_dwFireTime = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_ExtendAttackList[0].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	if( bAim )
	{
		FD_AimTarget_Bow kFindPred( pOwner, m_fAzazelAngle, 0.f, m_fAzazelRange, false );
		ioBaseChar *pTarget = pOwner->GetCreator()->FindUser( kFindPred );
		if( pTarget )
			m_AzazelAimedTarget = pTarget->GetCharName();
		else
			m_AzazelAimedTarget.Clear();
	}
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_AZAZEL;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_AzazelAimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetAzazelSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioAzazelItem *pAzazel = ToAzazelItem( pItem );
	if( !pAzazel )
		return;

	pAzazel->SetPushState( pOwner, true );
}

bool ioChargeComboDash::CheckEnableHanzoState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_HANZO )
		return false;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioHanzoItem *pHanzo = ToHanzoItem( pItem );
	if( !pHanzo || !pHanzo->IsEnableGauge() )
		return false;

	return true;
}

void ioChargeComboDash::SetHanzoState( ioBaseChar *pOwner, int iType )
{
	if( !pOwner )
		return;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioHanzoItem *pHanzo = ToHanzoItem( pItem );
	if( !pHanzo )
		return;

	if( iType == SS_CHARGE_S )
		pHanzo->ChangeToDefensePosition( pOwner );
	else if( iType == SS_CHARGE_A )
		pHanzo->ChangeToJumpPosition( pOwner, true );
}

bool ioChargeComboDash::CheckEnableFlashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_FLASH )
		return false;

	if( pOwner->IsEnableFlashSpecial() )
		return true;

	return false;
}

void ioChargeComboDash::SetFlashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetFlashSpecialState( true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_FLASH;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableSlasherState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_SLASHER )
		return false;

	if( pOwner->IsEnableSlasherSpecial() )
		return true;

	return false;
}

void ioChargeComboDash::SetSlasherState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetSlasherSpecialState( m_SlasherRunInfo );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_SLASHER;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetRunRotateBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int iBuffCnt = m_vBuffAddList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		ioHashString szBuffName = m_vBuffAddList[i];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
	}
}

void ioChargeComboDash::RemoveRundRotateBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int iBuffCnt = m_vBuffAddList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		ioHashString szBuffName = m_vBuffAddList[i];
		pOwner->RemoveBuff( szBuffName );
	}
}


bool ioChargeComboDash::IsNoDropState()
{
	if( m_DashExtendType == DET_FLY_DASH_ATTACK && 
		(m_DashState == DS_FLY_DASH_ATTACK || m_DashState == DS_FLY_DASH_ATTACK_END) )
		return true;

	if( m_DashExtendType == DET_DRAGONNEST_ASS && m_DashState == DS_DRAGONNEST_ASS )
	{
		switch ( m_DragonNestAssSubState )
		{
		case DNASS_Pre:
		case DNASS_Teleport:
			return true;
		}
	}
	return false;
}


bool ioChargeComboDash::CheckEnalbe_Top_Bl_ExtendMod( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDummyCharItem2 *pDummyItem2 = ToDummyCharItem2( pItem );
	if( pDummyItem2 && pDummyItem2->IsEnableGauge() )
		return true;


	return false;
}


bool ioChargeComboDash::CheckIsExtend_Top_Bl_mod( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDummyCharItem2 *pDummyItem2 = ToDummyCharItem2( pItem );
	if( pDummyItem2 && pDummyItem2->GetMeshType() == ioDummyCharItem2::IMT_EXTRA )
		return true;

	return false;
}


void ioChargeComboDash::SetTopBlExtendMod( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDummyCharItem2 *pDummyItem2 = ToDummyCharItem2( pItem );
	if( pDummyItem2 )
	{
		pDummyItem2->ChangeExtaMotion( pOwner );
		//pOwner->SetState( CS_ATTACK );
		pDummyItem2->SetNormalAttack(pOwner, -1);
	}

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_TOP_BL_EXTEND;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChargeComboDash::SetTopBlExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDummyCharItem2 *pDummyItem2 = ToDummyCharItem2( pItem );
	if( pDummyItem2 )
	{
		/*if( m_TopBl_ChargeInfo.bCreate )
			pDummyItem2->ChangeToDummyChar( pOwner );

		pDummyItem2->SetMoveDummyPos( pOwner, m_TopBl_ChargeInfo );*/

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID = pGrp->GetAnimationIdx( m_TopBl_ChargeAttack.m_AttackAnimation );

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_TopBl_ChargeAttack.m_fAttackAniRate, 0.0f, 0.f, true );

		DWORD dwCurTime = FRAMEGETTIME();
		m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_TopBl_ChargeAttack.m_fAttackAniRate;

		m_dwChargeAttackDummyFireTime = dwCurTime + m_dwChargeAttackDummyDelayTime;

		m_DashState = DS_TOP_BL_RETURN;
	}

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_TOP_BL_RETURN;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetTopBlDummyNormalMove( ioBaseChar *pOwner, bool bCombo, int nCombo )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDummyCharItem2 *pDummyItem2 = ToDummyCharItem2( pItem );
	if( !pDummyItem2 )
		return;

	if( !bCombo )
	{
		if( m_TopBl_Normal.bCreate )
			pDummyItem2->ChangeToDummyChar( pOwner );

		pDummyItem2->SetMoveDummyPos( pOwner, m_TopBl_Normal );
	}
	else
	{
		if( (int)m_vDummyMoveInfoCombo.size() > nCombo && nCombo >= 0)
		{
			if( m_vDummyMoveInfoCombo[nCombo].bCreate )
				pDummyItem2->ChangeToDummyChar( pOwner );

			pDummyItem2->SetMoveDummyPos( pOwner, m_vDummyMoveInfoCombo[nCombo] );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
bool ioChargeComboDash::CheckEnableChemistyChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioChemistryItem *pChemistryItem = ToChemistryItem( pItem );
	if( pChemistryItem && !pOwner->IsHasCrown() )
		return true;

	return false;
}


void ioChargeComboDash::SetChemistryChargeState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioChemistryItem		*pChemistryItem = ToChemistryItem( pItem );
	if( pChemistryItem )
	{
		/*if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
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
		}*/

		pChemistryItem->SetMakeExtendAttack( false, true );
		pOwner->SetNormalAttack(0);
	}
}


//////////////////////////////////////////////////////////////////////////

bool ioChargeComboDash::CheckEnableJeonWooChiChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioJeonWooChiItem *pJeonWooChiItem = ToJeonWooChiWeaponItem( pItem );
	if( pJeonWooChiItem && pJeonWooChiItem->IsEnableGauge() && !pOwner->IsHasCrown() )
		return true;

	return false;
}

void ioChargeComboDash::SetJeonWooChiChargeState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_stJeonWooChiChargeTargeted.Clear();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_JeonWooChiDashInfo.m_PowerChargeAttribute.m_AttackAnimation );
	float fTimeRate = m_JeonWooChiDashInfo.m_PowerChargeAttribute.m_fAttackAniRate;

	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetJeonWooChiChargeState - %s Animation is Not Exist",m_JeonWooChiDashInfo.m_PowerChargeAttribute.m_AttackAnimation.c_str() );
		return;
	}

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioJeonWooChiItem *pJeonWooChiItem = ToJeonWooChiWeaponItem( pItem );
	if( pJeonWooChiItem )
		pJeonWooChiItem->DecreaseBullet();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	//pOwner->SetNoWoundState( false );

	//pOwner->SetState( CS_EXTEND_DASH );
	DWORD dwPreDelay = m_JeonWooChiDashInfo.m_PowerChargeAttribute.m_dwPreDelay;
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_JeonWooChiDashInfo.m_PowerChargeAttribute.m_vWeaponInfoList,
		FTT_EXTEND_DASH_ATTACK,
		fTimeRate,
		dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_JeonWooChiDashInfo.m_PowerChargeAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1 );

	DWORD dwTrackingTime = FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_dwActionEndTime = dwCurTime + dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_DashState = DS_JEONWOOCHI_DASH;
	m_JeonWooChiState = JWCST_POWER_CHARGE;

	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_JEONWOOCHI_DASH;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << JWCST_POWER_CHARGE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChargeComboDash::SetJeonWooChiDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_fJeonWooChiCurMoveLength = 0.f;

	int iAniID = pGrp->GetAnimationIdx( m_JeonWooChiDashInfo.m_stMoveAni );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetJeonWooChiDashState - %s Animation is Not Exist", m_JeonWooChiDashInfo.m_stMoveAni.c_str() );
		return;
	}

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_JeonWooChiDashInfo.m_fMoveAniRate );

	//pOwner->SetCurMoveSpeed( m_JeonWooChiDashInfo.m_fMoveSpeed );

	m_JeonWooChiState = JWCST_DASH_STATE;
	m_fJeonWooChiNeedMoveLength = m_JeonWooChiDashInfo.m_fMaxMoveLength;
	m_vJeonWooChiMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );

	if( m_stJeonWooChiChargeTargeted.IsEmpty() )
	{
		m_fJeonWooChiNeedMoveLength = m_JeonWooChiDashInfo.m_fMaxMoveLength;
	}
	else
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_stJeonWooChiChargeTargeted );
		if( pTarget )
		{
			D3DXVECTOR3 vSrcPos = pOwner->GetWorldPosition();
			D3DXVECTOR3 vDestPos = pTarget->GetWorldPosition();
			D3DXVECTOR3 vDiff = vDestPos - vSrcPos;
			pOwner->SetTargetRotToDir( vDiff, true );
			m_vJeonWooChiMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );

			vDiff = vDestPos - vSrcPos;
			vSrcPos.y = vDestPos.y = 0.f;
			
			m_fJeonWooChiNeedMoveLength = min( D3DXVec3Length( &vDiff), m_JeonWooChiDashInfo.m_fMaxMoveLength );
		}
	}
	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_JEONWOOCHI_DASH;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << JWCST_DASH_STATE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChargeComboDash::SetJeonWooChiDashAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitActionStopDelay( true );

	float fTimeRate = m_JeonWooChiDashInfo.m_MoveAttack.m_fAttackAniRate;
	int iAniID = pGrp->GetAnimationIdx( m_JeonWooChiDashInfo.m_MoveAttack.m_AttackAnimation );
	DWORD dwPreDelay = m_JeonWooChiDashInfo.m_MoveAttack.m_dwPreDelay;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetJeonWooChiDashState - %s Animation is Not Exist", m_JeonWooChiDashInfo.m_MoveAttack.m_AttackAnimation.c_str() );
		return;
	}

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();

	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID,
		m_JeonWooChiDashInfo.m_MoveAttack.m_vWeaponInfoList,
		FTT_EXTEND_DASH_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_JeonWooChiState = JWCST_DASH_ATTACK;
	m_dwActionEndTime = dwCurTime + dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_JeonWooChiDashInfo.m_MoveAttack.m_dwEndDelay;
	m_dwReserveEnableTime = dwCurTime;
	m_dwReserveEnableTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_JEONWOOCHI_DASH;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << JWCST_DASH_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChargeComboDash::SetJeonWooChiDashAddAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitActionStopDelay( true );

	float fTimeRate = m_JeonWooChiDashInfo.m_AddMoveAttack.m_fAttackAniRate;
	int iAniID = pGrp->GetAnimationIdx( m_JeonWooChiDashInfo.m_AddMoveAttack.m_AttackAnimation );
	DWORD dwPreDelay = m_JeonWooChiDashInfo.m_AddMoveAttack.m_dwPreDelay;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetJeonWooChiDashAddAttack - %s Animation is Not Exist", m_JeonWooChiDashInfo.m_AddMoveAttack.m_AttackAnimation.c_str() );
		return;
	}

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetAutoTarget( ATT_DASH );

	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID,
		m_JeonWooChiDashInfo.m_AddMoveAttack.m_vWeaponInfoList,
		FTT_EXTEND_DASH_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_JeonWooChiState = JWCST_DASH_ADD_ATTACK;
	m_dwActionEndTime = dwCurTime + dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_JeonWooChiDashInfo.m_AddMoveAttack.m_dwEndDelay;

	pOwner->SetReservedSliding( m_JeonWooChiDashInfo.m_AddMoveAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_JEONWOOCHI_DASH;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << JWCST_DASH_ADD_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//////////////////////////////////////////////////////////////////////////

bool ioChargeComboDash::CheckKaelthasExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioKaelthasItem* pkealthasItem = ToKaelthasItem( pItem );
	if( !pItem )
		return false;

	if( pkealthasItem->GetCurBullet() >= (int)m_fDashExtraGauge )
		return true;

	return false;
}


void ioChargeComboDash::SetKaelthasExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;


	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioKaelthasItem* pKaelthasItem = ToKaelthasItem( pItem );
	if( !pKaelthasItem )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_KaelthasExtendAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetKaelthasExtendAttack - %s Animation is Not Exist",
			m_KaelthasExtendAttribute.m_AttackAnimation.c_str() );
		return;
	}

	pKaelthasItem->SetCurBullet( max( 0.f, (float)pKaelthasItem->GetCurBullet() - m_fDashExtraGauge) );

	float fTimeRate = m_KaelthasExtendAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_KaelthasExtendAttribute.m_dwPreDelay;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwKaelthasExtendAttackStartTime = dwCurTime;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, m_KaelthasExtendAttribute.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );
	pOwner->SetAutoTarget( ATT_DASH );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_FlyDashAttack.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;//m_dwFlyDashDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_KaelthasExtendAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	//pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_KAELTHAS_EXTEND;

	//pOwner->SetCurJumpPowerAmt( 0.0f );
	//pOwner->SetGravityAmt( 0.0f );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_KAELTHAS_EXTEND;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << dwCurBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::ProcessKaelthasExtendAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();
		CheckAniRotate( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( m_dwPreReserveEnableTime > 0 && m_dwPreReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			if( CheckPreReserve( pOwner ) && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return;
			}
		}

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			int iMaxCnt = m_ExtendAttackList.size();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
				CheckExtraReserve( pOwner );
			else
				CheckExtendReserve( pOwner );
						
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

void ioChargeComboDash::LoadDragonNestAssExtendInfo( ioINILoader &rkLoader )
{
	LoadAttackAttribute( "dash_dragon_nest_ass_pre_attack", m_DragonNestAssassinPreAttribute, rkLoader );
	LoadAttackAttribute( "dash_dragon_nest_ass_teleport_attack", m_DragonNestAssassinTeleportAttackAttribute, rkLoader );

	m_vDragonNestAssassinTeleportOffSet.x = rkLoader.LoadFloat_e( "dash_dragon_nest_ass_teleport_offset_x", 0.0f );
	m_vDragonNestAssassinTeleportOffSet.y = rkLoader.LoadFloat_e( "dash_dragon_nest_ass_teleport_offset_y", 0.0f );
	m_vDragonNestAssassinTeleportOffSet.z = rkLoader.LoadFloat_e( "dash_dragon_nest_ass_teleport_offset_z", 0.0f );

	m_fDragonNestAssassinExtendTeleportEndJump = rkLoader.LoadFloat_e( "dash_dragon_nest_ass_teleport_end_jump_power", 0.0f );
}

bool ioChargeComboDash::CheckDragonNestAssExtendState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	
	if( m_DashExtendType != DET_DRAGONNEST_ASS )
		return false;

	return true;
}

void ioChargeComboDash::CheckDragonNestAssExtendReserveKey( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();

	// 공격키가 눌러져 있으면 공격 상태 처리
	if( pOwner->IsAttackKeyDown() && 
		!pOwner->IsJumpKeyDown() &&
		!pOwner->IsDefenseKeyDown() &&
		!pOwner->IsChatModeState(true) )
	{
		ioObjectItem *pItem = pOwner->GetObject();
		if( ToDrinkItem( pItem ) )
			pOwner->SetDrinkState();
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				pOwner->SetNormalAttack( 0 );
		}
		return;
	}

	pOwner->SetState( CS_DELAY );
}

void ioChargeComboDash::SetDragonNestAssExtendPreState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDragonNestAssassinItem* pDragonNestAssassinItem = ToDragonNestAssassinItem( pItem );
	if( !pDragonNestAssassinItem )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_DragonNestAssassinPreAttribute.m_AttackAnimation );
	if( nAniID == -1 )
		return;

	pOwner->SetNoWoundState( false );

	float fTimeRate = m_DragonNestAssassinPreAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_DragonNestAssassinPreAttribute.m_dwPreDelay;

	m_dwReserveEnableTime = 0;
	m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex, nAniID, m_DragonNestAssassinPreAttribute.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwActionEndTime = dwCurTime;
	m_dwActionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate + dwPreDelay;

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_DASH );
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	CheckKeyReserveTime( pOwner, nAniID, fTimeRate );

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_DragonNestAssassinPreAttribute.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( nAniID, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( nAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_DRAGONNEST_ASS;
	m_DragonNestAssSubState = DNASS_Pre;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_DRAGONNEST_ASS;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << dwCurBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::SetDragonNestAssExtendTeleportState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->ClearAttackFireTimeAndSkill();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_DragonNestAssassinTeleportAttackAttribute.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate = m_DragonNestAssassinTeleportAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_DragonNestAssassinTeleportAttackAttribute.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex, nAniID, m_DragonNestAssassinTeleportAttackAttribute.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwActionEndTime = dwCurTime;
	m_dwActionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate + dwPreDelay;

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_DragonNestAssassinTeleportAttackAttribute.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( nAniID, fTimeRate, dwPreDelay );

	pOwner->SetForcePowerAmt( 0.0f );
	D3DXVECTOR3 vNewPos = pOwner->GetWorldPosition() + pOwner->GetWorldOrientation() * m_vDragonNestAssassinTeleportOffSet;
	pOwner->SetWorldPosition( vNewPos );

	m_DragonNestAssSubState = DNASS_Teleport;
}

void ioChargeComboDash::ProcessDragonNestAssassinState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	switch ( m_DragonNestAssSubState )
	{
	case DNASS_Pre:
		{
			if ( dwEndTime < dwCurTime )
				CheckDragonNestAssExtendReserveKey( pOwner );
		}
		break;
	case DNASS_Teleport:
		{
			if ( dwEndTime < dwCurTime )
				pOwner->SetSKillEndJumpState( m_fDragonNestAssassinExtendTeleportEndJump, false, false, true, true );
		}
		break;
	}
}

void ioChargeComboDash::SetDragonNestEnableTeleport( ioBaseChar *pOwner )
{	
	SetDragonNestAssExtendTeleportState( pOwner );
}

//썬더버드
bool ioChargeComboDash::CheckEnableThunderBirdState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_THUNDER_BIRD )
		return false;

	if( pOwner->IsEnableThunderBirdObjectSpecialState() )
		return true;

	return false;
}

void ioChargeComboDash::SetThunderBirdState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetThunderBirdObjectSpecialState( true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_THUNDER_BIRD;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//박일표
void ioChargeComboDash::LoadParkilpyoNormalExtendInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString_e( "dash_parkilpyo_gather_ani", "", szBuf, MAX_PATH );
	m_szParkilpyoGatherAni = szBuf;
	m_fParkilpyoGatherAniRate = rkLoader.LoadFloat_e( "dash_parkilpyo_gather_ani_rate", FLOAT1 );

	LoadAttackAttribute( "dash_parkilpyo_charge_attack", m_ParkilpyoChargeAttack, rkLoader );

	m_ParkilpyoChargeComboList.clear();
	int nMaxChargeComboCnt = rkLoader.LoadInt_e( "dash_parkilpyo_charge_combo_cnt", 0 );
	for ( int i=0; i<nMaxChargeComboCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "dash_parkilpyo_charge_combo%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_ParkilpyoChargeComboList.push_back( sAttackAttribute );
	}
}

void ioChargeComboDash::ChangeToParkilpyoNormalGathering( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szParkilpyoGatherAni );
	float fTimeRate = m_fParkilpyoGatherAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_DashState = DS_PARKILPYO_NORMAL_GATHER;

	m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_PARKILPYO_NORMAL_GATHER;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::ProcessParkilpyoNormalGathering( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime	= FRAMEGETTIME();

	if( ( CheckEnableParkilpyoNormalState( pOwner ) && pOwner->IsAttackKeyDown() ) && !pOwner->IsHasCrown() )
	{
		if( m_dwActionEndTime < dwCurTime )
			SetParkilpyoNormalEquipState( pOwner );
	}
	else
		ChangeToParkilpyoNormalChargeAttack( pOwner );
}

void ioChargeComboDash::ChangeToParkilpyoNormalChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ParkilpyoChargeAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_ParkilpyoChargeAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_ParkilpyoChargeAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_ParkilpyoChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_ParkilpyoChargeAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwEnableKeyTime = dwCurTime;
	m_dwEnableKeyTime += pGrp->GetAnimationEventTime( iAniID, "key_reserve" ) * fTimeRate;

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_nCurChargeCombo = 0;

	m_DashState = DS_PARKILPYO_NORMAL_CHARGE_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_PARKILPYO_NORMAL_CHARGE_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::ChangeToParkilpyoNormalChargeCombo( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( !COMPARE( m_nCurChargeCombo, 0, (int)m_ParkilpyoChargeComboList.size() ) )
		return;

	pOwner->InitActionStopDelay( true );

	const AttackAttribute &sChargeCombo = m_ParkilpyoChargeComboList[m_nCurChargeCombo];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( sChargeCombo.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate  = sChargeCombo.m_fAttackAniRate;
	DWORD dwPreDelay = sChargeCombo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( sChargeCombo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, sChargeCombo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;

	m_dwEnableKeyTime = dwCurTime;
	m_dwEnableKeyTime += pGrp->GetAnimationEventTime( nAniID, "key_reserve" ) * fTimeRate;

	m_DashState = DS_PARKILPYO_NORMAL_CHARGE_COMBO;
	
	m_szInputEventAnimation = sChargeCombo.m_AttackAnimation;
	m_fInputEventAnimationRate = fTimeRate;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << DST_STATE;
			kPacket << DS_PARKILPYO_NORMAL_CHARGE_COMBO;
			kPacket << (int)eNewDirKey;
			kPacket << m_nCurChargeCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_nCurChargeCombo++;
}

bool ioChargeComboDash::CheckEnableParkilpyoNormalState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_PARKILPYO_NORMAL )
		return false;

	if( pOwner->IsEnableParkilpyoEquipObject() )
		return true;

	return false;
}

void ioChargeComboDash::SetParkilpyoNormalEquipState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_PARKILPYO_NORMAL_EQUIP;
	pOwner->SetParkilpyoEquipObject( ioParkilpyoItem::EOS_DASH );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_PARKILPYO_NORMAL_EQUIP;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckParkilpyoChargeComboInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwEnableKeyTime > 0 && m_dwEnableKeyTime < dwCurTime && pOwner->IsAttackKey() )
		return true;

	return false;
}

void ioChargeComboDash::CheckParkilpyoCancelReserve( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( pOwner->IsJumpKey() && !m_bEnableJumpState )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
}

void ioChargeComboDash::LoadParkilpyoObjectExtendInfo( ioINILoader &rkLoader )
{
	m_fParkilpyoUseGauge = rkLoader.LoadFloat_e( "dash_parkilpyo_use_gauge", 0.0f );
	LoadAttackAttribute( "dash_parkilpyo_charge_attack", m_ParkilpyoChargeAttack, rkLoader );
}

void ioChargeComboDash::ChangeToParkilpyoObjectChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	int iAniID = pGrp->GetAnimationIdx( m_ParkilpyoChargeAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;
	
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	
	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	float fTimeRate = m_ParkilpyoChargeAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_ParkilpyoChargeAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, m_ParkilpyoChargeAttack.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );
	pOwner->SetAutoTarget( ATT_DASH );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_ParkilpyoChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	ioParkilpyoObjectItem *pParkilpyoObjectItem = ToParkilpyoObjectItem( pOwner->GetObject() );
	if ( pParkilpyoObjectItem )
		pParkilpyoObjectItem->DecreaseGauge( pOwner, m_fParkilpyoUseGauge );

	m_DashState = DS_PARKILPYO_OBJECT_CHARGE_ATTACK;
		
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_PARKILPYO_OBJECT_CHARGE_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableEquipObjectItemState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( pOwner->IsEquipEnableObject() )
	{
		return true;
	}

	return false;
}

void ioChargeComboDash::SetObjectEquipState( ioBaseChar *pOwner )
{	
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioAliceItem* pAlice = ToAliceItem( pItem );
	if( pAlice )
	{
		if( pAlice->GetCurBullet() < pAlice->GetNeedBullet() )
		{
			ChangeToNormalAttack( pOwner );
		}
		else
		{
			pAlice->ChangeEquipObjectState( pOwner, ioAliceItem::EOS_DASH );
		}
	}
}

bool ioChargeComboDash::CheckMaxComboSetDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int iMaxCnt = pOwner->GetExtraDashAttackMax();

	if( m_iCurComboCnt < iMaxCnt )
		return false;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();

	pOwner->SetState( CS_DELAY );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_DELAY_STATE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

bool ioChargeComboDash::CheckEnableLeeDragonAttackState( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioLeeItem *pLeeItem = ToLeeItem( pItem );
	if( pLeeItem && pLeeItem->GetCurBullet() )
		return true;

	return false;
}

void ioChargeComboDash::SetLeeDragonAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( !DecreaseLeeGauge( pOwner ) )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_LeeDragonExtendAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeComboDash::SetLeeDragonAttackState - %s Animation is Not Exist",
			m_LeeDragonExtendAttribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = m_LeeDragonExtendAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_LeeDragonExtendAttribute.m_dwPreDelay;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = dwCurTime;
	m_dwPreReserveEnableTime = 0;
	m_dwLeeExtendAttackStartTime = dwCurTime;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, m_LeeDragonExtendAttribute.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );
	pOwner->SetAutoTarget( ATT_DASH );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_FlyDashAttack.m_dwEndDelay;

	m_dwActionEndTime = dwCurTime + dwDuration;//m_dwFlyDashDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_LeeDragonExtendAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	//pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_LEE_DRAGON_ATTACK_STATE;

	//pOwner->SetCurJumpPowerAmt( 0.0f );
	//pOwner->SetGravityAmt( 0.0f );

	if( !m_stDrangonAttackBuff.IsEmpty() )
		pOwner->AddNewBuff( m_stDrangonAttackBuff, pOwner->GetCharName(), "", NULL );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_LEE_DRAGON_ATTACK_STATE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << dwCurBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::DecreaseLeeGauge( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioLeeItem* pLeeItem = ToLeeItem( pItem );
	if( !pLeeItem )
		return false;

	pLeeItem->DecreaseExtraNeedGauge( pOwner );
	return true;
}

//잭
void ioChargeComboDash::LoadJackExtendInfo( ioINILoader &rkLoader )
{	
	m_fJackUseGauge = rkLoader.LoadFloat_e( "dash_jack_use_gauge", 0.0f );
	m_nMaxReserveDash = rkLoader.LoadInt_e( "reserve_dash_max_cnt", 0 );
	LoadAttackAttribute( "dash_jack_charge_attack", m_JackChargeAttack, rkLoader );
}

bool ioChargeComboDash::CheckEnableJackChargeAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioJackItem *pJackItem = ToJackItem( pItem );
	if( pJackItem && pJackItem->GetCurBullet() >= m_fJackUseGauge )
		return true;

	return false;
}

void ioChargeComboDash::ChangeToJackChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	int iAniID = pGrp->GetAnimationIdx( m_JackChargeAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	float fTimeRate = m_JackChargeAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_JackChargeAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, m_JackChargeAttack.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );
	pOwner->SetAutoTarget( ATT_DASH );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_JackChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	ioJackItem *pJackItem = ToJackItem( pOwner->GetWeapon() );
	if ( pJackItem )
		pJackItem->SetUseExtraGauge( m_fJackUseGauge );

	m_DashState = DS_JACK_CHARGE_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_JACK_CHARGE_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//쉐도우레인저
void ioChargeComboDash::LoadShadowRangerExtendInfo( ioINILoader &rkLoader )
{	
	m_fShadowRangerUseGauge = rkLoader.LoadFloat_e( "dash_shadow_ranger_use_gauge", 0.0f );
}

bool ioChargeComboDash::CheckEnableShadowRangerState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_SHADOW_RANGER )
		return false;

	if( pOwner->IsEnableShadowRangerSpecial( m_fShadowRangerUseGauge ) )
		return true;

	return false;
}

void ioChargeComboDash::SetShadowRangerMoveState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->ClearAttackFireTimeAndSkill();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetShadowRangerSpecialMoveState( m_fShadowRangerUseGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_SHADOW_RANGER;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableJumperState( ioBaseChar * pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_JUMPER )
		return false;

	ioJumperItem *pJumper = ToJumperItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pJumper )
		return false;

	if( pJumper->GetCurBullet() < pJumper->GetNeedBullet() )
		return false;

	return true;
}

void ioChargeComboDash::SetJumperState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	ioJumperItem *pJumper = ToJumperItem( pOwner->GetPriorityItem( SPT_DASH ) );
	if( pJumper )
	{
		pJumper->ChangeToExtendAttackFire( pOwner, FTT_DASH_ATTACK );
	}
}

bool ioChargeComboDash::CheckEnableSaberExtendState( ioBaseChar * pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_SABER )
		return false;

	ioSaberItem *pSaber = ToSaberItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pSaber )
		return false;

	if( pSaber->GetCurBullet() <= 0.0f )
		return false;

	return true;
}

void ioChargeComboDash::SetSaberExtendState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	ioSaberItem *pSaber = ToSaberItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pSaber )
		return;

	pSaber->SetSaberExtendDashState( pOwner );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
}

bool ioChargeComboDash::CheckEnableNeoExtendState( ioBaseChar *pOwner, bool bHalf )
{
	if( !pOwner )
		return false;

	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pNeoItem )
		return false;

	if( !pNeoItem->IsEnableChargeState( bHalf ) )
		return false;

	return true;
}

void ioChargeComboDash::SetNeoExtendState( ioBaseChar *pOwner, bool bHalf )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pNeoItem )
		return;

	if( bHalf )
		pNeoItem->SetHalfAttackState( pOwner, true, ioNeoItem::OBS_DASH );
	else
		pNeoItem->SetAttackState( pOwner, true, ioNeoItem::OBS_DASH );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
}

void ioChargeComboDash::ChangeToKeyInputAttack( ioBaseChar * pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	int iAniID = -1;
	float fTimeRate = FLOAT1;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	AttackAttribute Attr;
	Attr.Init();

	if ( m_bEnableAttackState )
		Attr = m_AttackKeyAttribute;
	else if ( m_bEnableDefenseState )
		Attr = m_DefenseKeyAttribute;
	else if ( m_bEnableJumpState )
		Attr = m_JumpKeyAttribute;

	iAniID = pGrp->GetAnimationIdx( Attr.m_AttackAnimation );
	m_szInputEventAnimation = Attr.m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Animation is Not Exist", __FUNCTION__, Attr.m_AttackAnimation.c_str() );
		return;
	}


	fTimeRate = Attr.m_fAttackAniRate;
	dwPreDelay = Attr.m_dwPreDelay;
	dwEndDelay = Attr.m_dwEndDelay;

	m_dwActionEndTime = 0;
	m_dwPreReserveEnableTime = 0;
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
		Attr.m_vWeaponInfoList,
		FTT_EXTEND_DASH_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + Attr.m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime = dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( Attr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_bSetCombo = true;
	m_DashState = DS_KEY_INPUT_LAST_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << m_DashState;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_bEnableAttackState;
		kPacket << m_bEnableDefenseState;
		kPacket << m_bEnableJumpState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
}

void ioChargeComboDash::ProcessKeyInputLastAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( CheckAniDash_Input_Speical_Dash( pOwner ) )
		return;

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;

			if( CheckAniJump_Input_Jump_S( pOwner ) )
				return;
		}

		if( m_dwReserveEnableEndTime == 0 && m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt && !m_bMaxDashLastChargeAttack )
				CheckExtraReserve( pOwner );
			else
				CheckKeyReserve( pOwner );
		}
		else if( m_dwReserveEnableEndTime > 0 && m_dwReserveEnableTime > 0 && 
			m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime &&
			m_dwReserveEnableEndTime + pOwner->GetActionStopTotalDelay() >= dwCurTime )
		{
			// 예약체크
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt && !m_bMaxDashLastChargeAttack )
				CheckExtraReserve( pOwner );
			else
				CheckKeyReserve( pOwner );
			
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}

bool ioChargeComboDash::CheckEnableMagicSwordAttackState( ioBaseChar * pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_MAGIC_SWORD )
		return false;

	ioMagicSwordItem *pMagicSwordItem = ToMagicSwordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMagicSwordItem )
		return false;

	if ( !pMagicSwordItem->IsEnableMagicSwordGauge() )
		return false;

	return true;
}

void ioChargeComboDash::SetMagicSwordAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	ioMagicSwordItem *pMagicSwordItem = ToMagicSwordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMagicSwordItem )
		return;

	pOwner->ClearAttackFireTimeAndSkill();
	pMagicSwordItem->SetMagicSwordAttackState( pOwner, ioMagicSwordItem::ECS_DASH );
	
}

bool ioChargeComboDash::CheckEnableNataState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_NATA )
		return false;

	if( pOwner->IsEnableNataSpecialCombo( false, true, false ) )
		return true;

	return false;
}

void ioChargeComboDash::SetNataComboState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;
	
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetNataSpecialComboState( false, true, false );
}

bool ioChargeComboDash::CheckEnableResearcherRearFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_RESEARCHER_REAR_FLY )
		return false;

	ioResearcher* pResearcher = ToResearcher( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pResearcher )
		return false;

	if( pResearcher->IsOverHeat() )
		return false;

	return true;
}

void ioChargeComboDash::SetResearcherRearFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	ioResearcher* pResearcher = ToResearcher( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pResearcher )
		pResearcher->ChangeToRearFlySpecialState( pOwner, ioResearcher::RS_DASH );
}

bool ioChargeComboDash::CheckEnableBullFightState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_DashExtendType != DET_BULLFIGHT )
		return false;

	ioBullFightItem* pBullFightItem = ToBullFightItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pBullFightItem &&
		pBullFightItem->GetCurBullet() >= pBullFightItem->GetNeedBullet() )
		return true;

	return false;
}

void ioChargeComboDash::SetBullFightState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetState( CS_ATTACK );

	if ( pOwner->IsNeedProcess() )
	{
		ioBullFightItem* pBullFightItem = ToBullFightItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pBullFightItem )
		{
			pOwner->SetCurNormalAttackItem( pBullFightItem );
			pBullFightItem->ChangeToChargeAttack( pOwner );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_BULLFIGHT;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::OnJeconWooChiDashMove( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_JeonWooChiDashInfo.m_fMoveSpeed > 0.0f && m_vJeonWooChiMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_JeonWooChiDashInfo.m_fMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vJeonWooChiMoveDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;

				m_fJeonWooChiCurMoveLength += fFinalMoveAmt;
			}
		}

		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
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
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();
	
	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}


void ioChargeComboDash::SetNextDashState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	m_stJeonWooChiChargeTargeted.Clear();

	if( pWoundChar && !bDefense )
		m_stJeonWooChiChargeTargeted = pWoundChar->GetCharName();
}

bool ioChargeComboDash::CheckEnableSawangState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_DashExtendType != DET_SAWANG_SLIDE )
		return false;

	ioSawangItem* pSawangItem= ToSawangItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSawangItem && pSawangItem->IsEnableGauge() )
		return true;

	return false;
}

void ioChargeComboDash::SetSawangState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioSawangItem* pSawangItem = ToSawangItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pSawangItem )
		return;

	pSawangItem->SetSlideState( pOwner, ioSawangItem::SST_JUMP );
}


void ioChargeComboDash::SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime )
{
	m_dwRedHoodSpecialMoveMacroTime_S = 0;
	m_dwRedHoodSpecialMoveMacroTime_E = 0;
	m_dwMESpecialMoveMacroTime_S = 0;
	m_dwMESpecialMoveMacroTime_E = 0;
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

	dwSpecialMoveMacroTime_S = pGrp->GetAnimationEventTime_e( iAniID, "magic_eng_s" ) * fTimeRate;
	dwSpecialMoveMacroTime_E = pGrp->GetAnimationEventTime_e( iAniID, "magic_eng_e" ) * fTimeRate;
	if( dwSpecialMoveMacroTime_S )
	{
		m_dwMESpecialMoveMacroTime_S = dwCurTime + dwSpecialMoveMacroTime_S;
		m_dwMESpecialMoveMacroTime_E = dwCurTime + dwSpecialMoveMacroTime_E + dwEndTime;
	}
}

bool ioChargeComboDash::CheckEnableStriderExtendState( ioBaseChar * pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_STRIDER )
		return false;

	ioStriderItem *pStrider = ToStriderItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pStrider )
		return false;

	if( !pStrider->IsStriderExtendDashGauge() )
		return false;

	return true;
}

void ioChargeComboDash::SetStriderExtendState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	ioStriderItem *pStrider = ToStriderItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pStrider )
		return;

	pStrider->SetStriderExtendDashState( pOwner );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
}

bool ioChargeComboDash::CheckEnableApostateMonkExtendState( ioBaseChar * pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_APOSTATE_MONK )
		return false;

	ioApostatemonkItem *pApostatemon = ToApostatemonkItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pApostatemon )
		return false;

	return true;
}

void ioChargeComboDash::SetApostateMonkExtendState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	ioApostatemonkItem *pApostatemon = ToApostatemonkItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pApostatemon )
		return;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pApostatemon->SetExtendDash( pOwner );
}

void ioChargeComboDash::LoadOniMushaInfo( ioINILoader &rkLoader )
{
	LoadAttackAttribute( "dash_onimusha_charge_attack", m_OnimushaChargeAttack, rkLoader );
	m_fOnimushaChargeUseGauge = rkLoader.LoadFloat_e( "dash_onimusha_charge_use_gauge", 0.0f );
}

void ioChargeComboDash::SetOniMushaObjectEquipState( ioBaseChar *pOwner )
{	
	if ( !pOwner )
		return;

	if( pOwner->IsEnableOnimushaEquipObject() )
		pOwner->SetOnimushaEquipObject( ioOniMushaItem::EOS_DASH );
	else
		ChangeToOniMushaChargeAttack( pOwner );
}

bool ioChargeComboDash::CheckEnableOniMushaHalfChargeState( ioBaseChar *pOwner )
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

bool ioChargeComboDash::CheckEnableOniMushaRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( pOwner->IsHasCrown() )
		return false;

	ioOniMushaItem *pOniMushaItem = ToOniMushaItem( pOwner->GetWeapon() );
	if ( !pOniMushaItem )
		return false;

	if ( pOniMushaItem->GetCurExtraGauge() < pOniMushaItem->GetChargeUseGauge() )
		return false;

	return true;
}

void ioChargeComboDash::ChangeToOniMushaRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetState( CS_ATTACK );
	
	ioOniMushaItem *pOniMushaItem = ToOniMushaItem( pOwner->GetWeapon() );
	if ( pOniMushaItem )
	{
		pOwner->SetCurNormalAttackItem( pOniMushaItem );

		if ( pOwner->IsNeedProcess() )
			pOniMushaItem->ChangeToRunState( pOwner );
	}
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_ONIMUSHA_RUN;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::ChangeToOniMushaChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	int iAniID = pGrp->GetAnimationIdx( m_OnimushaChargeAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	float fTimeRate = m_OnimushaChargeAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_OnimushaChargeAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, m_OnimushaChargeAttack.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );
	pOwner->SetAutoTarget( ATT_DASH );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_OnimushaChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	
	m_DashState = DS_ONIMUSHA_CHARGE_ATTACK;
	m_bOnimushaChargeAttack = true;

	ioOniMushaItem* pOniMushaItem = ToOniMushaItem( pOwner->GetWeapon() );
	if ( pOniMushaItem )
		pOniMushaItem->SetUseExtraGauge( m_fOnimushaChargeUseGauge );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_ONIMUSHA_CHARGE_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboDash::ProcessOniMushaChargeAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( m_dwReserveEnableEndTime == 0 && m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			CheckKeyReserve( pOwner );
		}
		else if( m_dwReserveEnableEndTime > 0 && m_dwReserveEnableTime > 0 && 
			m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime &&
			m_dwReserveEnableEndTime + pOwner->GetActionStopTotalDelay() >= dwCurTime )
		{
			CheckKeyReserve( pOwner );
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

void ioChargeComboDash::LoadAzazelInfo( ioINILoader &rkLoader )
{
	m_fAzazelAngle = rkLoader.LoadFloat_e( "azazel_dash_aim_angle", 45.0f );
	m_fAzazelRange = rkLoader.LoadFloat_e( "azazel_dash_aim_range", FLOAT1000 );

	m_vAzazelAttackOffset.x = rkLoader.LoadFloat_e( "azazel_dash_offset_x", 0.0f );
	m_vAzazelAttackOffset.y = rkLoader.LoadFloat_e( "azazel_dash_offset_y", 0.0f );
	m_vAzazelAttackOffset.z = rkLoader.LoadFloat_e( "azazel_dash_offset_z", 0.0f );
}

void ioChargeComboDash::ProcessAzazelAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwActionEndTime < dwCurTime )
	{
		ProcessKeyReserve( pOwner );
		return;
	}

	if( m_dwFireTime != 0 && m_dwFireTime < dwCurTime )
	{
		m_dwFireTime = 0;
		
		int iAttackSize = m_ExtendAttackList[0].m_vWeaponInfoList.size();
		if ( iAttackSize <= 0 )
			return;

		ioPlayStage *pStage = pOwner->GetCreator();
		if( !pStage )
			return;

		DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + m_vAzazelAttackOffset;
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		ioBaseChar* pTarget = pStage->GetBaseChar( m_AzazelAimedTarget );
		if( pTarget )
			vDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();

		D3DXVec3Normalize( &vDir, &vDir );	
		m_AzazelAimedTarget.Clear();

		for ( int i = 0; i<iAttackSize ; ++i )
		{
			WeaponInfo sWeaponInfo = m_ExtendAttackList[0].m_vWeaponInfoList[i];

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();

			kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
			
			pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		}
	}
}

void ioChargeComboDash::ProcessSpecialAttack( ioBaseChar *pOwner )
{
	CheckCancelReserveByConditionalSpecialDash( pOwner );
	if( CheckAniDash_Input_Speical_Dash( pOwner ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwActionEndTime < dwCurTime )
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}

bool ioChargeComboDash::CheckEnableDruidBearState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( pOwner->IsEquipEnableObject() )
	{
		return true;
	}

	return false;
}

void ioChargeComboDash::SetDruidBearState( ioBaseChar *pOwner )
{	
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioDruidItem* pDruid = ToDruidItem( pItem );
	if( pDruid )
	{
		if( pDruid->GetCurBullet() < pDruid->GetNeedBullet() )
		{
			ChangeHalfExtendAttack( pOwner );
		}
		else
		{
			pOwner->ClearDirDoubleClick();
			pDruid->ChangeEquipObjectState( pOwner, ioDruidItem::EOS_DASH );
		}
	}
}

void ioChargeComboDash::ChangeHalfExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	const AttackAttribute *pAttr = &m_HalfExtendAttack;
	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	m_szInputEventAnimation = pAttr->m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Animation is Not Exist", __FUNCTION__, pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate				= pAttr->m_fAttackAniRate;
	DWORD dwPreDelay			= pAttr->m_dwPreDelay;
	m_dwActionEndTime			= 0;
	m_dwPreReserveEnableTime	= 0;
	m_dwReserveEnableTime		= 0;
	m_nHalfAttackCount++;

	m_iSkillInput				= -1;
	m_bEnableDefenseState		= false;
	m_bEnableAttackState		= false;
	m_bEnableJumpState			= false;
	m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime	= dwCurTime;
	m_dwActionStartTime	= dwCurTime;
	m_dwActionEndTime	= dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_DashState = DS_HALF_EXTEND_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_HALF_EXTEND_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckEnableSasinmuState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_SASINMU )
		return false;

	ioSasinmuItem* pSasinmuItem = ToSasinmuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSasinmuItem && !pSasinmuItem->HasPowerDownBuff( pOwner ) )
		return true;

	return false;
}

void ioChargeComboDash::SetSasinmuState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetState( CS_SASINMU_SPECIAL );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_SASINMU;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if ( pOwner->IsNeedProcess() )
	{
		ioSasinmuItem* pSasinmuItem = ToSasinmuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pSasinmuItem )
			pSasinmuItem->SetSpecialChargeDashState( pOwner );
	}
}

bool ioChargeComboDash::CheckEnablePredatorState( ioBaseChar *pOwner )
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

void ioChargeComboDash::SetPredatorState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	ioPredatorItem *pPredatorItem = ToPredatorItem( pOwner->GetWeapon() );
	if( pPredatorItem )
		pPredatorItem->SetPredatorSpecialState( pOwner, false );
}

bool ioChargeComboDash::CheckEnableCrowSpecialDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_CROW )
		return false;

	ioCrowItem* pCrowItem = ToCrowItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pCrowItem && pCrowItem->IsEnableGauge() )
		return true;

	return false;
}


void ioChargeComboDash::SetCrowSpecialDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioCrowItem* pCrowItem = ToCrowItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pCrowItem )
		pCrowItem->SetSpecialState( pOwner, true, true );
}

void ioChargeComboDash::CheckSpecialStateByExtendType( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	//전우치때 추가
	CheckProcessDash( pOwner );

	if( m_DashExtendType == DET_JEONWOOCHI && pOwner->IsDefenseKey() )
	{
		ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
		ioJeonWooChiItem *pJeonWooChiItem = ToJeonWooChiWeaponItem( pItem );
		if( pJeonWooChiItem && pJeonWooChiItem->Find_S_MoveTarget( pOwner ) )
		{
			pJeonWooChiItem->Set_S_Move_Step1( pOwner, false );
			return;
		}
	}
	else if( m_DashExtendType == DET_RED_HOOD && pOwner->IsDefenseKey() )
	{
		if( m_dwRedHoodSpecialMoveMacroTime_S && m_dwRedHoodSpecialMoveMacroTime_S < dwCurTime && m_dwRedHoodSpecialMoveMacroTime_E > dwCurTime )
		{
			ioRedHoodItem* pRedHoodItem = ToRedHoodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pRedHoodItem && pRedHoodItem->IsEnableGauge() )
			{
				pRedHoodItem->SetSpecailMoveState( pOwner, true );
				return;
			}
		}
	}
	else if( m_DashExtendType == DET_BLACK_SPHEAR )
	{
		if ( m_DashState != DS_NORMAL_ATTACK && m_DashState != DS_COMBO_ATTACK )
			return;
		if ( pOwner->IsDefenseKey() )
		{
			ioBlackSpearItem* pBlackSpearItem = ToBlackSpearItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if ( pBlackSpearItem )
			{
				pBlackSpearItem->CheckBrandBuff( pOwner );
				return;
			}
		}
	}
	else if( m_DashExtendType == DET_MAGICENGINEER && pOwner->IsJumpKey() )
	{
		if( m_dwMESpecialMoveMacroTime_S && m_dwMESpecialMoveMacroTime_S < dwCurTime && m_dwMESpecialMoveMacroTime_E > dwCurTime )
		{
			ioMagicEngineerItem* pMEItem = ToMagicEngineerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pMEItem && pMEItem->IsEnableGauge() )
			{
				pMEItem->SetSpecailMoveState( pOwner, true, ioMagicEngineerItem::SM_UP );
				return;
			}
		}
	}
}

bool ioChargeComboDash::CheckEnableMephistoSpecialDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_DashExtendType != DET_MEPHISTO )
		return false;

	ioMephistoItem* pMephistoItem = ToMephistoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMephistoItem && pMephistoItem->EnableMephistoDashState() )
		return true;

	return false;
}

void ioChargeComboDash::SetMephistoSpecialDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioMephistoItem* pMephistoItem = ToMephistoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMephistoItem )
		pMephistoItem->SetMephistoDashState( pOwner );
}

bool ioChargeComboDash::CheckEnableBubbleFighterDashAttack( ioBaseChar *pOwner )
{
	ioBubbleFighterItem *pBubbleFighter = ToBubbleFighterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pBubbleFighter )
		return false;

	if ( !pBubbleFighter->EnableBulletAttack() )
		return false;

	return true;
}

void ioChargeComboDash::SetBubbleFighterDashAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_DashState = DS_NONE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetState( CS_ATTACK );

	if ( pOwner->IsNeedProcess() )
	{
		ioBubbleFighterItem* pBubbleFighter = ToBubbleFighterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pBubbleFighter )
		{
			pOwner->SetCurNormalAttackItem( pBubbleFighter );
			pBubbleFighter->ChangeToChargeAttackState( pOwner );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_BUBBLE_FIGHTER_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboDash::CheckBubbleFighterBullet( ioBaseChar *pOwner )
{
	if( m_DashExtendType != DET_BUBBLE_FIGHTER )
		return false;

	ioBubbleFighterItem* pBubbleFighter = ToBubbleFighterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pBubbleFighter || pBubbleFighter->EnableBulletAttack() )
		return false;

	if( pBubbleFighter->SetExtendDefenseState( pOwner ) )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_DEFENSE );
			kPacket << pOwner->GetCharName();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << pBubbleFighter->GetCurBullet();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return true;
	}

	return false;
}

bool ioChargeComboDash::CheckEnableUrielChargeDashAttack( ioBaseChar *pOwner )
{
	if( m_DashExtendType != DET_URIEL )
		return false;

	ioUrielItem* pUrielItem = ToUrielItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pUrielItem || !pUrielItem->CheckChargeAttackGauge() )
		return false;

	return true;
}

void ioChargeComboDash::SetUrielChargeDashAttack( ioBaseChar *pOwner )
{
	ioUrielItem* pUrielItem = ToUrielItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pUrielItem )
		return;

	pUrielItem->SetChargeAttack( pOwner, true );
}

bool ioChargeComboDash::CheckEnableWindGodDashAttack( ioBaseChar *pOwner )
{
	if( m_DashExtendType != DET_WINDGOD )
		return false;
	
	ioWindGodItem* pWindGodItem = ToWindGodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pWindGodItem || !pWindGodItem->IsEnableExtendDash() )
		return false;

	return true;
}

void ioChargeComboDash::SetWindGodDashAttack( ioBaseChar *pOwner )
{
	ioWindGodItem* pWindGodItem = ToWindGodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pWindGodItem )
		return;

	pWindGodItem->SetWindGodDashMoveState( pOwner );
}

bool ioChargeComboDash::CheckEnableStrangerState( ioBaseChar *pOwner )
{
	if( m_DashExtendType != DET_STRANGER )
		return false;

	ioStrangerItem* pStrangerItem = ToStrangerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pStrangerItem || !pStrangerItem->IsEnableStrangerDashTeleportState() )
		return false;

	return true;
}

void ioChargeComboDash::SetStrangerState( ioBaseChar *pOwner )
{
	ioStrangerItem* pStrangerItem = ToStrangerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pStrangerItem )
		return;

	pStrangerItem->SetStrangerTeleportState( pOwner );
}

bool ioChargeComboDash::CheckEnableHackerDashAttackState( ioBaseChar *pOwner )
{
	if( m_DashExtendType != DET_HACKER )
		return false;

	ioHackerItem* pHackerItem = ToHackerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pHackerItem )
		return false;

	return true;
}

void ioChargeComboDash::SetHackerDashAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioHackerItem* pHackerItem = ToHackerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pHackerItem )
		return;

	m_DashState = DS_NONE;
	pHackerItem->SetHackerDashAttack( pOwner );
}

void ioChargeComboDash::CheckBaseballBatting( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioBaseballItem* pBaseballItem = ToBaseballItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pBaseballItem )
		return;

	pBaseballItem->CheckBattingWeapon( pOwner );
}

void ioChargeComboDash::InitAniRotate()
{
	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();
}

void ioChargeComboDash::SetAniRotate( ioBaseChar *pChar, int iAniID, float fAniRate )
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

bool ioChargeComboDash::CheckHeraclesJump_Input_Jump_S( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	if( !pItem )
		return false;

	ioHeraclesItem* pHeracles = ToHeraclesItem( pItem );
	if( !pHeracles )
		return false;

	if( m_szInputEventAnimation.IsEmpty() )
		return false;

	int iAniID = pGrp->GetAnimationIdx( m_szInputEventAnimation );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fInputEventAnimationRate;

		dwTime += m_dwActionStartTime;

		//다른 로직에서 input_jump_s의 기능이 key_reserve를 체크하지 않는데 해당로직에서만 체크하고 있어 버그로 판단하여 수정
		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			if( !pOwner->IsBuffLimitJump() && pHeracles->IsEnableJumpGauge() )
			{				
				pHeracles->SetCancelJump( pOwner );
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