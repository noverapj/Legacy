

#include "stdafx.h"

#include "ioChargeComboJump2.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioChargeComboJump2::ioChargeComboJump2()
{
	m_ChargeComboState = CCS_NONE;
	m_NewJumpState = NJS_NONE;

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedFlash = false;
	m_bChargeAttacked = false;
	m_bUsedSurferCharge = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_dwChargingStartTime = 0;
	m_dwChargeAttackEndTime = 0;
	m_dwFireStartTime = 0;

	// double
	m_iCurJumpCnt = 1;

	// flash
	m_dwFlashDelayEndTime = 0;
	m_dwFlashEndTime = 0;

	// dash
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwExtrajumpEnableKeyTime = 0;
	m_bExtraJumpReserveSkill = false;
	m_GuidEffectID = -1;

	m_bUsedChargeAttack = false;
	m_bUsedExcaliburAtt = false;
	m_bJackExtraJumpReserve = false;
	m_dwComboDashEndTime = 0;

	m_iCurJumpComboDashCount = 0;
	m_bUsingExpansionReaperJumpChargeAttack = false;

	m_dwEnableReaperExpansionTeleportTime = 0;
	m_iJumpDashCurCount = 0;
}

ioChargeComboJump2::ioChargeComboJump2( const ioChargeComboJump2 &rhs )
: ioExtendJump( rhs ),
m_fJumpSpeedRateFR( rhs.m_fJumpSpeedRateFR ),
m_fJumpSpeedRateSI( rhs.m_fJumpSpeedRateSI ),
m_fJumpSpeedRateBG( rhs.m_fJumpSpeedRateBG ),
m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
m_ComboAttackList( rhs.m_ComboAttackList ),
m_FlashDelayAni( rhs.m_FlashDelayAni ),
m_fFlashDelayRate( rhs.m_fFlashDelayRate ),
m_fFlashForce( rhs.m_fFlashForce ),
m_fFlashGravityAmt( rhs.m_fFlashGravityAmt ),
m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
m_bDashEndImmediateAttack( rhs.m_bDashEndImmediateAttack ),
m_fDashEndJumpPower( rhs.m_fDashEndJumpPower ),
m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
m_FlashJumpAttack( rhs.m_FlashJumpAttack ),
m_fJumpDashAniRate( rhs.m_fJumpDashAniRate ),
m_vJumpDashAniList( rhs.m_vJumpDashAniList ),
m_vForceInfoList( rhs.m_vForceInfoList ),
m_bEnableFlashJump( rhs.m_bEnableFlashJump ),
m_bEnableDoubleJump( rhs.m_bEnableDoubleJump ),
m_bEnableDoubleJumpAfterDash( rhs.m_bEnableDoubleJumpAfterDash ),
m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_dwHalfChargingTime( rhs.m_dwHalfChargingTime ),
m_ChargingAniList( rhs.m_ChargingAniList ),
m_fChargeJumpPower( rhs.m_fChargeJumpPower ),
m_fChargeGravityRate( rhs.m_fChargeGravityRate ),
m_fChargeBaseRange( rhs.m_fChargeBaseRange ),
m_JumpExtendType( rhs.m_JumpExtendType ),
m_BuffAddAttribute( rhs.m_BuffAddAttribute ),
m_vBuffAddList( rhs.m_vBuffAddList ),
m_JumpMechanicsFly( rhs.m_JumpMechanicsFly ),
m_JumpDestroyerFly( rhs.m_JumpDestroyerFly ),
m_JumpMichaelFly( rhs.m_JumpMichaelFly ),
m_JumpGoomihoFly( rhs.m_JumpGoomihoFly ),
m_JumpDracula( rhs.m_JumpDracula ),
m_JumpEvy( rhs.m_JumpEvy ),
m_vExcaliburAttList( rhs.m_vExcaliburAttList ),
m_fExcaliburForceAmt( rhs.m_fExcaliburForceAmt ),
m_ExtendAttackList( rhs.m_ExtendAttackList ),
m_ExtendLandAttack( rhs.m_ExtendLandAttack ),
m_bMaxJumpLastChargeAttack( rhs.m_bMaxJumpLastChargeAttack ),
m_JumpSlasher( rhs.m_JumpSlasher ),
m_vTopMoveNormalInfo( rhs.m_vTopMoveNormalInfo ),
m_TopMoveChargeInfo( rhs.m_TopMoveChargeInfo ),
m_TopBl_ChargeAttack( rhs.m_TopBl_ChargeAttack ),
m_JumpTitanExtend( rhs.m_JumpTitanExtend ),
m_KaelthasExtendAttack( rhs.m_KaelthasExtendAttack ),
m_DashJumpAttack( rhs.m_DashJumpAttack ),
m_fJumpDashGravityAmt( rhs.m_fJumpDashGravityAmt ),
m_JumpDashLandAni( rhs.m_JumpDashLandAni ),
m_fJumpDashLandAniRate( rhs.m_fJumpDashLandAniRate ),
m_dwJumpDashLandDelayTime( rhs.m_dwJumpDashLandDelayTime ),
m_bUseNewJumpTypeDash( rhs.m_bUseNewJumpTypeDash ),
m_ComboDashType( rhs.m_ComboDashType ),
m_bUseJumpDashRotate( rhs.m_bUseJumpDashRotate ),
m_bUseDoubleJumpNewJumpDash( rhs.m_bUseDoubleJumpNewJumpDash ),
m_bUseAttackAfterNewJumpDash( rhs.m_bUseAttackAfterNewJumpDash ),
m_fThunderBirdEndJumpPower( rhs.m_fThunderBirdEndJumpPower ),
m_fThunderBirdNormalUseGauge( rhs.m_fThunderBirdNormalUseGauge ),
m_fThunderBirdChargeUseGauge( rhs.m_fThunderBirdChargeUseGauge ),
m_bSetForceActionStopInit( rhs.m_bSetForceActionStopInit ),
m_fParkilpyoChargeUseGauge( rhs.m_fParkilpyoChargeUseGauge ),
m_bSetChargeAttackEndWait( rhs.m_bSetChargeAttackEndWait ),
m_fJackChargeUseGauge( rhs.m_fJackChargeUseGauge ),
m_fJackExtraJumpUseGauge( rhs.m_fJackExtraJumpUseGauge ),
m_fJackExtraJumpUseGaugeDropZone( rhs.m_fJackExtraJumpUseGaugeDropZone ),
m_fJackExtraJumpAttackUseGauge( rhs.m_fJackExtraJumpAttackUseGauge ),
m_JackExtraJumpAttack( rhs.m_JackExtraJumpAttack ),
m_fJackExtraJumpAttackGravityAmt( rhs.m_fJackExtraJumpAttackGravityAmt ),
m_bEnableExtraJump( rhs.m_bEnableExtraJump ),
m_dwExtraJumpEnableTime( rhs.m_dwExtraJumpEnableTime ),
m_fExtrajumpEndPower( rhs.m_fExtrajumpEndPower ),
m_bSetExtraJumpForcePower( rhs.m_bSetExtraJumpForcePower ),
m_fExtraJumpPower( rhs.m_fExtraJumpPower ),
m_fExtraJumpForce( rhs.m_fExtraJumpForce ),
m_ExtraJump( rhs.m_ExtraJump ),
m_bExtendChnageNewJumpAttackState( rhs.m_bExtendChnageNewJumpAttackState ),
m_bKeyInputLastAttack( rhs.m_bKeyInputLastAttack ),
m_AttackKeyAttribute( rhs.m_AttackKeyAttribute ),
m_DefenseKeyAttribute( rhs.m_DefenseKeyAttribute ),
m_JumpKeyAttribute( rhs.m_JumpKeyAttribute ),
m_MagicSwordExtendAttackAttribute( rhs.m_MagicSwordExtendAttackAttribute ),
m_MagicSwordExtendAttackKeyAttribute( rhs.m_MagicSwordExtendAttackKeyAttribute ),
m_MagicSwordExtendDefenseKeyAttribute( rhs.m_MagicSwordExtendDefenseKeyAttribute ),
m_MagicSwordExtendJumpKeyAttribute( rhs.m_MagicSwordExtendJumpKeyAttribute ),
m_fBullFightChargeHeight( rhs.m_fBullFightChargeHeight ),
m_fBullFightUseGauge( rhs.m_fBullFightUseGauge ),
m_BullFightChargeAttack( rhs.m_BullFightChargeAttack ),
m_BullFightChargeExtendAttack( rhs.m_BullFightChargeExtendAttack ),
m_JeonWooChiExtendAttack( rhs.m_JeonWooChiExtendAttack ),
m_fChainMagicianChargeUseGauge( rhs.m_fChainMagicianChargeUseGauge ),
m_fChainMagicianChargeHeight( rhs.m_fChainMagicianChargeHeight ),
m_vNewJumpAddAttackList( rhs.m_vNewJumpAddAttackList ),
m_bEnableJumpComboDash( rhs.m_bEnableJumpComboDash ),
m_iMaxJumpComboDashCount( rhs.m_iMaxJumpComboDashCount ),
m_fDashJumpAttackEndJumpPowerRate( rhs.m_fDashJumpAttackEndJumpPowerRate ),
m_dwReaperExpansionTeleportTicTime( rhs.m_dwReaperExpansionTeleportTicTime ),
m_iJumpDashMaxCount( rhs.m_iJumpDashMaxCount ),
m_bEnableExtendComboColSkip( rhs.m_bEnableExtendComboColSkip )
{
	m_ChargeComboState = CCS_NONE;
	m_NewJumpState = NJS_NONE;

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedFlash = false;
	m_bChargeAttacked = false;
	m_bUsedSurferCharge = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_dwChargingStartTime = 0;
	m_dwChargeAttackEndTime = 0;
	m_dwFireStartTime = 0;

	// double
	m_iCurJumpCnt = 1;

	// flash
	m_dwFlashDelayEndTime = 0;
	m_dwFlashEndTime = 0;

	// dash
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwExtrajumpEnableKeyTime = 0;
	m_bExtraJumpReserveSkill = false;
	m_GuidEffectID = -1;

	m_bUsedChargeAttack = false;
	m_bUsedExcaliburAtt = false;

	m_bCanEvySpecial = true;

	m_dwCreateWeaponIndex = 0;
	m_bJackExtraJumpReserve = false;

	m_bUseExtraSpeedRate = false;
	m_dwComboDashEndTime = 0;

	m_iCurJumpComboDashCount = 0;
	m_bUsingExpansionReaperJumpChargeAttack = false;

	m_dwEnableReaperExpansionTeleportTime = 0;
	m_iJumpDashCurCount = 0;
}

ioExtendJump* ioChargeComboJump2::Clone()
{
	return new ioChargeComboJump2( *this );
}

ioChargeComboJump2::~ioChargeComboJump2()
{
}

JumpType ioChargeComboJump2::GetType() const
{
	return JT_CHARGE_COMBO2;
}

void ioChargeComboJump2::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	m_bEnableFlashJump = rkLoader.LoadBool_e( "enable_flash_jump", false );
	m_bEnableDoubleJump = rkLoader.LoadBool_e( "enable_double_jump", false );
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_bEnableDoubleJumpAfterDash = rkLoader.LoadBool_e( "enable_jump_dash_after_dash", false );
	m_bEnableJumpComboDash = rkLoader.LoadBool_e( "enable_jump_combo_dash", false );
	m_bEnableExtendComboColSkip = rkLoader.LoadBool_e( "enable_extend_combo_col_skip", false );

	// combo
	m_fJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	LoadComboList( rkLoader );

	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );
	m_dwHalfChargingTime = rkLoader.LoadInt_e("half_charging_time", 0 );

	m_iJumpDashMaxCount = rkLoader.LoadInt_e("jump_dash_max_count", 2 );

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

	m_bDashEndImmediateAttack = rkLoader.LoadBool_e( "dash_end_immediate_attack", false );
	m_fDashEndJumpPower = rkLoader.LoadFloat_e( "dash_end_jump_power", 0.0f );
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

	rkLoader.LoadString_e( "jump_dash_land_ani", "", szBuf, MAX_PATH );
	m_JumpDashLandAni = szBuf;
	m_fJumpDashLandAniRate = rkLoader.LoadFloat_e( "jump_dash_land_ani_rate", FLOAT1 );
	m_dwJumpDashLandDelayTime = rkLoader.LoadInt_e( "jump_dash_land_delay_time", 0 );
	m_fJumpDashGravityAmt = rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );

	m_bUseNewJumpTypeDash = rkLoader.LoadBool_e( "use_new_jump_type_dash", false );
	m_bUseJumpDashRotate = rkLoader.LoadBool_e( "use_jump_dash_rotate", false );
	m_bUseDoubleJumpNewJumpDash = rkLoader.LoadBool_e( "use_double_jump_new_jump_type_dash", false );
	m_bUseAttackAfterNewJumpDash = rkLoader.LoadBool_e( "use_jump_attacked_after_jump_type_dash", false );
	
	m_fJumpDashAniRate = rkLoader.LoadFloat_e( "dash_jump_ani_rate", FLOAT1 );

	m_vJumpDashAniList.clear();
	m_vJumpDashAniList.reserve( 4 );

	rkLoader.LoadString_e( "dash_jump_ani_front", "", szBuf, MAX_PATH );
	m_vJumpDashAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dash_jump_ani_right", "", szBuf, MAX_PATH );
	m_vJumpDashAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dash_jump_ani_back", "", szBuf, MAX_PATH );
	m_vJumpDashAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dash_jump_ani_left", "", szBuf, MAX_PATH );
	m_vJumpDashAniList.push_back( ioHashString(szBuf) );

	m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "dash_jump_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "dash_jump_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "dash_jump_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_vForceInfoList.push_back( kInfo );
	}

	m_fChargeJumpPower = rkLoader.LoadFloat_e( "jump_charge_jump_power", 0.0f );
	m_fChargeGravityRate = rkLoader.LoadFloat_e( "jump_charge_gravity_rate", FLOAT1 );
	m_fChargeBaseRange = rkLoader.LoadFloat_e( "jump_charge_base_range", 0.0f );

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

	// mechanics fly
	LoadMechanicsFlyInfo( rkLoader );

	//디스트로이어
	LoadDestroyerFlyInfo( rkLoader );

	// michael
	LoadMichaelFlyInfo( rkLoader );

	//구미호
	if( m_JumpExtendType == JET_GOOMIHO_FLY )
		LoadGoomihoFlyInfo( rkLoader );

	//드라큘라 
	LoadDraculaInfo( rkLoader );

	if( m_JumpExtendType == JET_EVY )
		LoadEvyBlinkInfo( rkLoader );
		
	if( m_JumpExtendType == JET_SLASHER )
		LoadSlasherInfo( rkLoader );

	//탑블레이트
	if( m_JumpExtendType == JET_TOP_BL )
		LoadTopBlInfo( rkLoader );

	//타이탄
	if( m_JumpExtendType == JET_TITAN )
		LoadTitanExtendInfo( rkLoader );

	//신비술사
	if( m_JumpExtendType == JET_KAELTHAS )
		LoadKaelthasExtendInfo( rkLoader );

	//전우치
	if( m_JumpExtendType == JET_JEONWOOCHI )
		LoadJeonWooChiChargeInfo( rkLoader );

	//썬더버드
	if( m_JumpExtendType == JET_THUNDER_BIRD )
		LoadThunderBirdExtendInfo( rkLoader );

	//박일표
	if( m_JumpExtendType == JET_PARKILPYO )
		LoadParkilpyoExtendInfo( rkLoader );

	//잭
	if( m_JumpExtendType == JET_JACK )
		LoadJackExtendInfo( rkLoader );

	//마검사
	if( m_JumpExtendType == JET_MAGIC_SWORD )
		LoadMagicSwordExtendInfo( rkLoader );

	//투우사
	if( m_JumpExtendType == JET_BULLFIGHT )
		LoadBullFightInfo( rkLoader );

	//체임메지션
	if( m_JumpExtendType == JET_CHAIN_MAGICIAN )
		LoadChainMagicianInfo( rkLoader );

	if ( m_JumpExtendType == JET_REAPER_EXPANSION )
		m_dwReaperExpansionTeleportTicTime = (DWORD)rkLoader.LoadInt_e( "reaper_jump_teleport_tic_time", 0 );

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
	
	//뉴점프대쉬 후 추가타 가능하게
	int nMaxDashJumpAttackAddAttackCnt = rkLoader.LoadInt_e( "dash_jump_attack_add_attack_cnt", 0 );
	for( int i = 0; i < nMaxDashJumpAttackAddAttackCnt; i++ )
	{
		AttackAttribute kAttack;

		wsprintf_e( szKey, "dash_jump_attack_add_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttack, rkLoader );

		m_vNewJumpAddAttackList.push_back( kAttack );
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

	m_ExtendLandAttack.Init();
	LoadAttackAttribute_e( "jump_charge_land", m_ExtendLandAttack, rkLoader );
	m_bExtendChnageNewJumpAttackState = rkLoader.LoadBool_e( "extend_change_new_jump_attack_state", true );

	//메이드때 추가
	m_bMaxJumpLastChargeAttack = rkLoader.LoadBool_e( "max_jump_and_last_charge_attack", false );

	m_bSetForceActionStopInit = rkLoader.LoadBool_e( "set_force_action_stop_delay", false );

	//파괴자때 추가 (D꾹 후 애니종료시 랜드어택)
	m_bSetChargeAttackEndWait = rkLoader.LoadBool_e( "set_jump_charge_wait", false );

	//잭때 추가 (점프중 추가 A키 관련 - 더블점프와 같이 사용 불가)
	m_bEnableExtraJump = rkLoader.LoadBool_e( "set_extra_jump", false );
	m_dwExtraJumpEnableTime = rkLoader.LoadInt_e( "extra_jump_enable_time", 0 );
	m_fExtrajumpEndPower = rkLoader.LoadFloat_e( "extra_jump_end_power", 0.0f );
	m_bSetExtraJumpForcePower = rkLoader.LoadBool_e( "set_extra_jump_force_power", false );
	m_fExtraJumpPower = rkLoader.LoadFloat_e( "extra_jump_power", 0.0f );
	m_fExtraJumpForce = rkLoader.LoadFloat_e( "extra_jump_force", 0.0f );

	LoadAttackAttribute_e( "extra_jump", m_ExtraJump, rkLoader );

	m_bKeyInputLastAttack = rkLoader.LoadBool_e( "jump_key_input_last_attack", false );
	LoadAttackAttribute_e( "jump_attack_key_attack", m_AttackKeyAttribute, rkLoader );
	LoadAttackAttribute_e( "jump_defense_key_attack", m_DefenseKeyAttribute, rkLoader );
	LoadAttackAttribute_e( "jump_jump_key_attack", m_JumpKeyAttribute, rkLoader );

	m_iMaxJumpComboDashCount = rkLoader.LoadInt_e( "max_jump_combo_dash_count", 0 );
	m_fDashJumpAttackEndJumpPowerRate = rkLoader.LoadFloat_e( "dash_jump_attack_end_jump_power_rate", 0.0f );
}

void ioChargeComboJump2::LoadMechanicsFlyInfo( ioINILoader &rkLoader )
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

void ioChargeComboJump2::LoadDestroyerFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_JumpDestroyerFly.Init();

	rkLoader.LoadString_e( "destroyer_fly_effect", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_DestroyerFlyEffect = szBuf;

	rkLoader.LoadString_e( "destroyer_fly_move_effect", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_DestroyerFlyMoveEffect = szBuf;

	wsprintf_e( szBuf, "destroyer_fly_attack" );
	LoadAttackAttribute( szBuf, m_JumpDestroyerFly.m_DestroyerAttack, rkLoader );

	m_JumpDestroyerFly.m_dwChargingTime = rkLoader.LoadInt_e( "fly_charging_time", 100 );
	rkLoader.LoadString_e( "fly_charging", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_ChargingAni = szBuf;
	m_JumpDestroyerFly.m_dwChargingAniPreDelay = rkLoader.LoadInt_e( "fly_charging_predely", 0 );
	m_JumpDestroyerFly.m_fChargingAniRate = rkLoader.LoadFloat_e( "fly_charging_rate", FLOAT1 );

	rkLoader.LoadString_e( "fly_charged", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_ChargedAni = szBuf;
	m_JumpDestroyerFly.m_fChargedAniRate = rkLoader.LoadFloat_e( "fly_charged_rate", FLOAT1 );

	wsprintf_e( szBuf, "fly_charge_attack" );
	LoadAttackAttribute( szBuf, m_JumpDestroyerFly.m_DestroyerChargeAttack, rkLoader );
	
	rkLoader.LoadString_e( "destroyer_fly_start_ani", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_StartAni = szBuf;
	m_JumpDestroyerFly.m_fStartAniRate = rkLoader.LoadFloat_e( "destroyer_fly_start_ani_rate", FLOAT1 );

	m_JumpDestroyerFly.m_fStartAngle = rkLoader.LoadFloat_e( "destroyer_fly_start_angle", 0.0f );
	m_JumpDestroyerFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "destroyer_fly_start_max_range", 0.0f );
	m_JumpDestroyerFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "destroyer_fly_start_duration", 0 );

	rkLoader.LoadString_e( "destroyer_fly_delay_ani", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_DelayAni = szBuf;
	m_JumpDestroyerFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "destroyer_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniFr = szBuf;
	m_JumpDestroyerFly.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniBk = szBuf;
	m_JumpDestroyerFly.m_fMoveAniBkRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniRt = szBuf;
	m_JumpDestroyerFly.m_fMoveAniRtRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_MoveAniLt = szBuf;
	m_JumpDestroyerFly.m_fMoveAniLtRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_lt", FLOAT1 );

	m_JumpDestroyerFly.m_fMoveForceAmt = rkLoader.LoadFloat_e( "fly_force_move", 0.0f );
	m_JumpDestroyerFly.m_fMoveForceFric = rkLoader.LoadFloat_e( "fly_force_friction", FLOAT1 );
	m_JumpDestroyerFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	m_JumpDestroyerFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_JumpDestroyerFly.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_JumpDestroyerFly.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_JumpDestroyerFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_JumpDestroyerFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );

	m_JumpDestroyerFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );

	rkLoader.LoadString_e( "fly_attack_move_ani", "", szBuf, MAX_PATH );
	m_JumpDestroyerFly.m_AttackMoveAni = szBuf;
	m_JumpDestroyerFly.m_fAttackMoveAniRate = rkLoader.LoadFloat_e( "fly_attack_move_ani_rate", FLOAT1 );

	m_JumpDestroyerFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "fly_attack_move_speed", 0.0f );
	m_JumpDestroyerFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "fly_attack_move_range", FLOAT100 );
	m_JumpDestroyerFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "fly_attack_move_rate", FLOAT1 );

	m_JumpDestroyerFly.m_fDestroyerFlyJumpAmt = rkLoader.LoadFloat_e( "destroyer_fly_jump_amt", 0.0f );
}

void ioChargeComboJump2::LoadMichaelFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	// 아이템쪽에서 MichaelFlyInfo의 필요정보만 취한다!
	m_JumpMichaelFly.Init();

	// start
	rkLoader.LoadString_e( "jump_michaelfly_start_ani", "", szBuf, MAX_PATH );
	m_JumpMichaelFly.m_StartAni = szBuf;
	m_JumpMichaelFly.m_fStartAniRate = rkLoader.LoadFloat_e( "jump_michaelfly_start_ani_rate", FLOAT1 );

	m_JumpMichaelFly.m_fStartAngle = rkLoader.LoadFloat_e( "jump_michaelfly_start_angle", 0.0f );
	m_JumpMichaelFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_michaelfly_start_max_range", 0.0f );
	m_JumpMichaelFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "jump_michaelfly_start_duration", 0 );

	// 타겟 대상 검색용
	m_JumpMichaelFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_michaelfly_target_wound_type", TWT_ALL );
	m_JumpMichaelFly.m_fTargetAngle = rkLoader.LoadFloat_e( "jump_michaelfly_target_angle", 0.0f );
	m_JumpMichaelFly.m_fTargetRange = rkLoader.LoadFloat_e( "jump_michaelfly_target_range", 0.0f );
	m_JumpMichaelFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "jump_michaelfly_target_up_height", 0.0f );
	m_JumpMichaelFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "jump_michaelfly_target_down_height", 0.0f );

	// attack move
	wsprintf_e( szKey, "jump_michaelfly_attack_move" );
	LoadAttackAttribute( szKey, m_JumpMichaelFly.m_AttackMove, rkLoader );

	m_JumpMichaelFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "jump_michaelfly_attack_move_speed", 0.0f );
	m_JumpMichaelFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "jump_michaelfly_attack_move_range", 0.0f );
	m_JumpMichaelFly.m_fAttackMoveMinRange = rkLoader.LoadFloat_e( "jump_michaelfly_attack_move_min_range", 0.0f );
	m_JumpMichaelFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "jump_michaelfly_attack_move_rate", FLOAT1 );

	// grappling
	m_JumpMichaelFly.m_GrapplingInfo.Init();
	m_JumpMichaelFly.m_GrapplingInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "jump_michaelfly_target_reverse_rotate", false );
	m_JumpMichaelFly.m_GrapplingInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "jump_michaelfly_wound_grappling_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "jump_michaelfly_wound_grappling_loop_motion", "", szBuf, MAX_PATH );
	m_JumpMichaelFly.m_GrapplingInfo.m_GrapplingLoopMotion = szBuf;
	m_JumpMichaelFly.m_GrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_JumpMichaelFly.m_GrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "jump_michaelfly_wound_grappling_loop_motion_rate", FLOAT1 );
	
	// grappling loop
	m_JumpMichaelFly.m_fGrapplingRange = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_gap_range", 0.0f );
	m_JumpMichaelFly.m_fGrapplingHeightRate = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_height_rate", 0.0f );

	wsprintf_e( szKey, "jump_michaelfly_grappling_loop_ani" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_JumpMichaelFly.m_GrapplingLoopAni = szBuf;
	m_JumpMichaelFly.m_fGrapplingLoopAniRate = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_loop_ani_rate", FLOAT1 );
	m_JumpMichaelFly.m_dwGrapplingLoopDuration = rkLoader.LoadInt_e( "jump_michaelfly_grappling_loop_duration", 0 );
	m_JumpMichaelFly.m_dwGrapplingEnableMoveTime = rkLoader.LoadInt_e( "jump_michaelfly_grappling_enable_move_time", 0 );

	// grappling move
	wsprintf_e( szKey, "jump_michaelfly_grappling_move_ani" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_JumpMichaelFly.m_GrapplingMoveAni = szBuf;
	m_JumpMichaelFly.m_fGrapplingMoveAniRate = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_move_ani_rate", FLOAT1 );

	wsprintf_e( szKey, "jump_michaelfly_grappling_dir_move_ani" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_JumpMichaelFly.m_GrapplingDirMoveAni = szBuf;
	m_JumpMichaelFly.m_fGrapplingDirMoveAniRate = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_dir_move_ani_rate", FLOAT1 );

	m_JumpMichaelFly.m_fGrapplingMoveSpeed = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_move_speed", 0.0f );
	m_JumpMichaelFly.m_fGrapplingMoveAngle = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_move_angle", 0.0f );
	m_JumpMichaelFly.m_fGrapplingMoveRange = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_move_range", 0.0f );

	m_JumpMichaelFly.m_fGrapplingDirMoveSpeed = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_dir_move_speed", 0.0f );
	m_JumpMichaelFly.m_fGrapplingDirMoveAngle = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_dir_move_angle", 0.0f );
	m_JumpMichaelFly.m_fGrapplingDirMoveRange = rkLoader.LoadFloat_e( "jump_michaelfly_grappling_dir_move_range", 0.0f );

	// end attack
	wsprintf_e( szKey, "jump_michaelfly_end_attack" );
	LoadAttackAttribute( szKey, m_JumpMichaelFly.m_EndAttack, rkLoader );

	m_JumpMichaelFly.m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "jump_michaelfly_end_jump_amt", 0.0f );
}

void ioChargeComboJump2::LoadGoomihoFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_JumpGoomihoFly.Init();

	// start
	rkLoader.LoadString_e( "jump_goomihofly_start_ani", "", szBuf, MAX_PATH );
	m_JumpGoomihoFly.m_StartAni = szBuf;
	m_JumpGoomihoFly.m_fStartAniRate = rkLoader.LoadFloat_e( "jump_goomihofly_start_ani_rate", FLOAT1 );

	m_JumpGoomihoFly.m_fStartAngle = rkLoader.LoadFloat_e( "jump_goomihofly_start_angle", 0.0f );
	m_JumpGoomihoFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_goomihofly_start_max_range", 0.0f );
	m_JumpGoomihoFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "jump_goomihofly_start_duration", 0 );

	// 타겟 대상 검색용
	m_JumpGoomihoFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_goomihofly_target_wound_type", TWT_ALL );
	m_JumpGoomihoFly.m_fTargetAngle = rkLoader.LoadFloat_e( "jump_goomihofly_target_angle", 0.0f );
	m_JumpGoomihoFly.m_fTargetRange = rkLoader.LoadFloat_e( "jump_goomihofly_target_range", 0.0f );
	m_JumpGoomihoFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "jump_goomihofly_target_up_height", 0.0f );
	m_JumpGoomihoFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "jump_goomihofly_target_down_height", 0.0f );

	// attack move
	wsprintf_e( szKey, "jump_goomihofly_attack_move" );
	LoadAttackAttribute( szKey, m_JumpGoomihoFly.m_AttackMove, rkLoader );

	m_JumpGoomihoFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "jump_goomihofly_attack_move_speed", 0.0f );
	m_JumpGoomihoFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "jump_goomihofly_attack_move_range", 0.0f );
	m_JumpGoomihoFly.m_fAttackMoveMinRange = rkLoader.LoadFloat_e( "jump_goomihofly_attack_move_min_range", 0.0f );
	m_JumpGoomihoFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "jump_goomihofly_attack_move_rate", FLOAT1 );
	m_JumpGoomihoFly.m_fAttackMoveAngle = rkLoader.LoadFloat_e( "jump_goomihofly_attack_move_angle", 0.0f );
	m_JumpGoomihoFly.m_fAttackerHeightRate = rkLoader.LoadFloat_e( "jump_goomihofly_attack_move_height_rate_attacker", FLOAT05 );
	m_JumpGoomihoFly.m_fTargetHeightRate = rkLoader.LoadFloat_e( "jump_goomihofly_attack_move_height_rate_target", FLOAT05 );
	
	int iExtendCnt = rkLoader.LoadInt_e( "jump_extend_attack_cnt", 0 );
	m_JumpGoomihoFly.m_vExtendAttributeList.clear();
	m_JumpGoomihoFly.m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "jump_extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_JumpGoomihoFly.m_vExtendAttributeList.push_back( kAttribute );
	}

	iExtendCnt = rkLoader.LoadInt_e( "jump_extend_plus_attack_cnt", 0 );
	m_JumpGoomihoFly.m_vExtendPlusAttributeList.clear();
	m_JumpGoomihoFly.m_vExtendPlusAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "jump_extend_plus_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_JumpGoomihoFly.m_vExtendPlusAttributeList.push_back( kAttribute );
	}

	// end attack
	wsprintf_e( szKey, "jump_goomihofly_end_attack" );
	LoadAttackAttribute( szKey, m_JumpGoomihoFly.m_EndAttack, rkLoader );

	// 점프 전환용
	m_JumpGoomihoFly.m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "jump_goomihofly_end_jump_amt", 0.0f );
	m_JumpGoomihoFly.m_bJumpAttack = true;
}

void ioChargeComboJump2::LoadDraculaInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_JumpDracula.Init();

	m_JumpDracula.m_fStartAngle = rkLoader.LoadFloat_e( "jump_fly_start_angle", 0.0f );
	m_JumpDracula.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_fly_start_max_range", 0.0f );
	m_JumpDracula.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "jump_fly_start_duration", 0 );

	m_JumpDracula.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "jump_fly_rotate_speed", 0 );
	m_JumpDracula.m_fMoveSpeed = rkLoader.LoadFloat_e( "jump_fly_move_speed", 0.0f );
	m_JumpDracula.m_fUpDownSpeed = rkLoader.LoadFloat_e( "jump_fly_updown_speed", 0.0f );
	m_JumpDracula.m_vBuffList.clear();

	int nBuffCnt = rkLoader.LoadInt_e( "special_buff_cnt", 0 );
	for( int k=0; k < nBuffCnt; ++k )
	{
		wsprintf_e( szKey, "special_buff%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_JumpDracula.m_vBuffList.push_back( szBuff );
	}
}

void ioChargeComboJump2::LoadEvyBlinkInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_JumpEvy.Init();

	m_JumpEvy.m_fMaxBlinkLength = rkLoader.LoadFloat_e( "jump_max_blink_length", 0.f );
	m_JumpEvy.m_fMapHeightGap = rkLoader.LoadFloat_e( "jump_max_height_gap", 0.f );

	rkLoader.LoadString_e( "jump_blink_start_ani", "", szBuf, MAX_PATH );
	m_JumpEvy.m_StartBlinkMotion = szBuf;
	m_JumpEvy.m_fStartBlinkMotionRate = rkLoader.LoadFloat_e( "jump_blink_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_blink_end_ani", "", szBuf, MAX_PATH );
	m_JumpEvy.m_EndBlinkMotion = szBuf;
	m_JumpEvy.m_fEndBlinkMotionRate = rkLoader.LoadFloat_e( "jump_blink_end_ani_rate", FLOAT1 );

	m_JumpEvy.m_fJumpAmt = rkLoader.LoadFloat_e( "jump_blink_end_jump_power", 0.f );
}

void ioChargeComboJump2::LoadSlasherInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_JumpSlasher.Init();
	m_JumpSlasher.m_fFirstMaxMoveDist = rkLoader.LoadFloat_e( "jump_first_run_dist", 0.0f );
	m_JumpSlasher.m_fFirstRunSpeed = rkLoader.LoadFloat_e( "jump_first_run_speed", 0.0f );
	m_JumpSlasher.m_dwFirstRunTime = rkLoader.LoadInt_e( "jump_first_run_duration", 0 );

	m_JumpSlasher.m_sFirstInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "jump_first_run_attack_type", NO_WEAPON_ATTRIBUTE );
	m_JumpSlasher.m_sFirstInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "jump_first_run_attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "jump_first_run_attack_type_wounded", "", szBuf, MAX_PATH );
	m_JumpSlasher.m_sFirstInfo.m_WoundedAnimation = szBuf;
	m_JumpSlasher.m_sFirstInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "jump_first_run_attack_type_wounded_time", 0 );
	m_JumpSlasher.m_sFirstInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "jump_first_run_attack_type_wounded_loop_ani", false );

	m_JumpSlasher.m_fSecondMaxMoveDist = rkLoader.LoadFloat_e( "jump_second_run_dist", 0.0f );
	m_JumpSlasher.m_fSecondRunSpeed = rkLoader.LoadFloat_e( "jump_second_run_speed", 0.0f );
	m_JumpSlasher.m_dwSecondRunTime = rkLoader.LoadInt_e( "jump_second_run_duration", 0 );

	m_JumpSlasher.m_sSecondInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "jump_second_run_attack_type", NO_WEAPON_ATTRIBUTE );
	m_JumpSlasher.m_sSecondInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "jump_second_run_attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "jump_second_run_attack_type_wounded", "", szBuf, MAX_PATH );
	m_JumpSlasher.m_sSecondInfo.m_WoundedAnimation = szBuf;
	m_JumpSlasher.m_sSecondInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "jump_second_run_attack_type_wounded_time", 0 );
	m_JumpSlasher.m_sSecondInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "jump_second_run_attack_type_wounded_loop_ani", false );
	
	//달리기 애니메이션 관련
	rkLoader.LoadString_e( "jump_first_run_delay_ani", "", szBuf, MAX_PATH );
	m_JumpSlasher.m_szFirstRunReadyAni = szBuf;
	m_JumpSlasher.m_fFirstRunReadyAniRate = rkLoader.LoadFloat_e( "jump_first_run_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "jump_first_run_loop_ani", "", szBuf, MAX_PATH );
	m_JumpSlasher.m_szFirstRunLoopAni = szBuf;
	rkLoader.LoadString_e( "jump_first_run_end_ani", "", szBuf, MAX_PATH );
	m_JumpSlasher.m_szFirstRunEndAni = szBuf;
	m_JumpSlasher.m_fFirstRunEndAniRate = rkLoader.LoadFloat_e( "jump_first_run_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_second_run_delay_ani", "", szBuf, MAX_PATH );
	m_JumpSlasher.m_szSecondRunReadyAni = szBuf;
	m_JumpSlasher.m_fSecondRunReadyAniRate = rkLoader.LoadFloat_e( "jump_second_run_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "jump_second_run_loop_ani", "", szBuf, MAX_PATH );
	m_JumpSlasher.m_szSecondRunLoopAni = szBuf;
	rkLoader.LoadString_e( "jump_second_run_end_ani", "", szBuf, MAX_PATH );
	m_JumpSlasher.m_szSecondRunEndAni = szBuf;
	m_JumpSlasher.m_fSecondRunEndAniRate = rkLoader.LoadFloat_e( "jump_second_run_end_ani_rate", FLOAT1 );
}

void ioChargeComboJump2::LoadTopBlInfo( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "jump_normal_move_cnt", 0 );
	for( int i = 0; i < iCnt; ++i )
	{
		StTopMoveInfo info;
		wsprintf( szKey, "jump_top_combo_rotate_x%d", i+1 );
		info.fRotate_x = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "jump_top_combo_rotate_y%d", i+1 );
		info.fRotate_y = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "jump_top_combo_move_length%d", i+1 );
		info.fLength = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf( szKey, "jump_top_combo_create%d", i+1 );
		info.bCreate = rkLoader.LoadBool( szKey, false );

		m_vTopMoveNormalInfo.push_back( info );
	}

	m_TopMoveChargeInfo.fRotate_x = rkLoader.LoadFloat_e( "jump_top_charge_rotate_x", 0.f );
	m_TopMoveChargeInfo.fRotate_y = rkLoader.LoadFloat_e( "jump_top_charge_rotate_y", 0.f );
	m_TopMoveChargeInfo.fLength = rkLoader.LoadFloat_e( "jump_top_charge_length", 0.f );
	m_TopMoveChargeInfo.bCreate = rkLoader.LoadBool_e( "jump_top_charge_create", false );

	rkLoader.LoadString_e( "jump_top_charge_return_attack_ani", "", szBuf, MAX_PATH );
	m_TopBl_ChargeAttack.m_AttackAnimation = szBuf;
	m_TopBl_ChargeAttack.m_fAttackAniRate = rkLoader.LoadFloat_e( "jump_top_charge_return_attack_rate", FLOAT1 );
}

void ioChargeComboJump2::LoadTitanExtendInfo( ioINILoader &rkLoader )
{
	LoadAttackAttribute_e( "extend_jump_titan_move", m_JumpTitanExtend.attribute , rkLoader );
	LoadAttackAttribute_e( "extend_jump_titan_land", m_JumpTitanExtend.attribute_land , rkLoader );

	m_JumpTitanExtend.m_fMoveAngle = rkLoader.LoadFloat_e( "jump_titan_extend_move_fire_angle", 0.f );	
	m_JumpTitanExtend.m_fMoveSpeed = rkLoader.LoadFloat_e( "extend_jump_titan_move_speed", FLOAT1 );
	m_JumpTitanExtend.m_fMoveRate = rkLoader.LoadFloat_e( "extend_jump_titan_move_rate", FLOAT1 );
}

void ioChargeComboJump2::LoadKaelthasExtendInfo( ioINILoader &rkLoader )
{
	LoadAttackAttribute_e( "jump_kaelthas_extend_attack", m_KaelthasExtendAttack , rkLoader );
}

void ioChargeComboJump2::LoadJeonWooChiChargeInfo( ioINILoader &rkLoader )
{
	LoadAttackAttribute_e( "jump_jeonwoochi_extend_attack", m_JeonWooChiExtendAttack , rkLoader );
}

void ioChargeComboJump2::LoadThunderBirdExtendInfo( ioINILoader &rkLoader )
{
	m_fThunderBirdEndJumpPower = rkLoader.LoadFloat_e( "jump_thunder_bird_end_jump_power", 0.0f );
	m_fThunderBirdNormalUseGauge = rkLoader.LoadFloat_e( "jump_thunder_bird_normal_use_gauge", 0.0f );
	m_fThunderBirdChargeUseGauge = rkLoader.LoadFloat_e( "jump_thunder_bird_charge_use_gauge", 0.0f );
}

void ioChargeComboJump2::LoadParkilpyoExtendInfo( ioINILoader &rkLoader )
{
	m_fParkilpyoChargeUseGauge = rkLoader.LoadFloat_e( "jump_parkilpyo_use_gauge", 0.0f );
}

void ioChargeComboJump2::LoadJackExtendInfo( ioINILoader &rkLoader )
{
	m_fJackChargeUseGauge = rkLoader.LoadFloat_e( "jump_jack_charge_use_gauge", 0.0f );
	m_fJackExtraJumpUseGauge = rkLoader.LoadFloat_e( "jump_jack_extra_use_gauge", 0.0f );
	m_fJackExtraJumpUseGaugeDropZone = rkLoader.LoadFloat_e( "jump_jack_extra_use_gauge_drop_zone", 0.0f );
	m_fJackExtraJumpAttackUseGauge = rkLoader.LoadFloat_e( "jump_jack_extra_attack_use_gauge", 0.0f );
	LoadAttackAttribute_e( "extra_jump_attack", m_JackExtraJumpAttack, rkLoader );
	m_fJackExtraJumpAttackGravityAmt = rkLoader.LoadFloat_e( "jump_jack_extra_attack_gravity_amt", 0.0f );
}

void ioChargeComboJump2::LoadMagicSwordExtendInfo( ioINILoader &rkLoader )
{
	LoadAttackAttribute_e( "magic_sword_extend_attack", m_MagicSwordExtendAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "magic_sword_extend_attack_key", m_MagicSwordExtendAttackKeyAttribute, rkLoader );
	LoadAttackAttribute_e( "magic_sword_extend_defense_key", m_MagicSwordExtendDefenseKeyAttribute, rkLoader );
	LoadAttackAttribute_e( "magic_sword_extend_jump_key", m_MagicSwordExtendJumpKeyAttribute, rkLoader );
}

void ioChargeComboJump2::LoadChainMagicianInfo( ioINILoader &rkLoader )
{
	m_fChainMagicianChargeUseGauge = rkLoader.LoadFloat_e( "jump_chain_magician_charge_use_gauge", 0.0f );
	m_fChainMagicianChargeHeight = rkLoader.LoadFloat_e( "jump_chain_magician_charge_enable_height", 0.0f );
}

void ioChargeComboJump2::LoadComboList( ioINILoader &rkLoader )
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

void ioChargeComboJump2::InitJump()
{
	m_bUsedSurferCharge = false;
	m_bUsingExpansionReaperJumpChargeAttack = false;
}

void ioChargeComboJump2::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_ChargeComboState = CCS_NONE;
	m_NewJumpState = NJS_NONE;
	pOwner->SetUsedFlashJump( false );

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedFlash = false;
	m_bChargeAttacked = false;
	m_bJackExtraJumpReserve = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_dwChargingStartTime = 0;
	m_dwChargeAttackEndTime = 0;
	m_dwFireStartTime = 0;

	// double
	m_iCurJumpCnt = 1;

	// flash
	m_dwFlashDelayEndTime = 0;
	m_dwFlashEndTime = 0;

	// dash
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwExtrajumpEnableKeyTime = 0;
	m_bExtraJumpReserveSkill = false;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	if( bFullTime && m_dwJumpChargeTime > 0 )
	{
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

	m_bCanEvySpecial = true;

	m_dwEnableNinjaDashTime = 0;

	m_bUseExtraSpeedRate = false;

	m_dwEnableReaperExpansionTeleportTime = FRAMEGETTIME() + 100;

	m_dwMESpecialMoveMacroTime_S = 0;
	m_dwMESpecialMoveMacroTime_E = 0;
	m_iJumpDashCurCount = 0;
}

void ioChargeComboJump2::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
		return;

	// 체크 순서 중요!!
	DWORD dwCurTime = FRAMEGETTIME();

	// combo
	if( m_bReserveJumpAttack && ( m_ChargeComboState != CCS_DASH || !m_bDashEndImmediateAttack ) )
	{
		SetChargingState( pOwner );
		return;
	}

	// DashCheck
	if( IsCanJumpDash( pOwner, fHeightGap ) )
	{
		m_ChargeComboState = CCS_DASH;
		m_bUseExtendJump = true;

		m_dwActionEndTime = 0;
		m_iJumpDashCurCount++;

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

			if( !m_bUseJumpDashRotate )
				pOwner->SetMoveDirByRotate( qtRotate );
			else
				pOwner->SetTargetRotToRotate( qtRotate, false );
		}

		int iAniID = -1;
		float fTimeRate = FLOAT1;

		bool bDashAttack = SetDashAttack( pOwner, iAniID, fTimeRate );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_JUMP_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetMoveDir();
			kPacket << m_ChargeComboState;
			kPacket << false;					// dash end
			kPacket << pOwner->GetTargetRot();

			if( bDashAttack )
				pOwner->FillAutoTargetInfo( kPacket );

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		if( m_bUseNewJumpTypeDash )
		{
			DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
			SetNewJumpTypeDashAttack( pOwner, m_dwActionEndTime, dwKeyReserveTime );
			return;
		}
	}

	// Double
	if( IsCanDoubleJump( pOwner, fHeightGap ) )
	{
		if( m_fDoubleJumpPower >= 0.0f )
		{
			SetDoubleJump( pOwner );
			return;
		}
	}

	//Jump Combo Dash
	if ( IsCanComboDash( pOwner, fHeightGap ) )
	{
		if ( m_JumpExtendType == JET_BLACK_SPHEAR )
		{
			SetBlackSpearJumpComboDashState( pOwner );
			return;
		}
	}

	ProcessJumpDash( pOwner );

	if ( IsCanExtraJump( pOwner ) )
	{
		pOwner->SetExtraJumpState();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << CCS_EXTRA_JUMP_STATE;
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	//전우치
	if( pOwner->IsNeedProcess() && m_JumpExtendType == JET_JEONWOOCHI && pOwner->IsDefenseKey() )
		CheckJeonWooChiDummy( pOwner );
	else if( pOwner->IsNeedProcess() && m_JumpExtendType == JET_BLACK_SPHEAR && pOwner->IsDefenseKey() )
		CheckBlackSpearBuffCheckState( pOwner );
	else if ( CheckReaperExpansionTeleport( pOwner ) )
	{
		SetReaperExpansionTeleportState( pOwner );
		return;
	}
}

bool ioChargeComboJump2::CheckJeonWooChiDummy( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioJeonWooChiItem *pJeonWooChiItem = ToJeonWooChiWeaponItem( pItem );
	if( pJeonWooChiItem && pJeonWooChiItem->Find_S_MoveTarget( pOwner ) )
	{
		pJeonWooChiItem->Set_S_Move_Step1( pOwner, true );
		return true;
	}

	return false;
}

bool ioChargeComboJump2::SetDashAttack( ioBaseChar *pOwner,  OUT int &iAniID, OUT float &fTimeRate )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_DashJumpAttack.m_AttackAnimation.IsEmpty() )
	{
		iAniID = GetJumpDashAni( pOwner );
		fTimeRate = m_fJumpDashAniRate;

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

			m_dwActionEndTime = dwCurTime;
			m_dwActionEndTime += dwDuration;

			pOwner->SetReservedSliding( m_vForceInfoList, iAniID, fTimeRate, 0 );

			CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
		}

		return false;
	}
	else//홍길동 각성때 추가
	{
		iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
		fTimeRate = m_DashJumpAttack.m_fAttackAniRate;

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

		DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

		m_dwActionEndTime = dwCurTime;
		m_dwActionEndTime = dwCurTime + dwPreDelay + dwEndDelay + dwDuration;

		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

		pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		return true;
	}

	return false;
}

// combo
void ioChargeComboJump2::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_NORMAL:
	case CCS_DASH:
	case CCS_DOUBLE:
	case CCS_BLACK_SPEAR_DASH:
		break;
	default:
		return;
	}

	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
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

void ioChargeComboJump2::SetTopBlNormalAttack( ioBaseChar *pOwner, int nCombo )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDummyCharItem2 *pDummyItem2 = ToDummyCharItem2( pItem );
	if( !pDummyItem2 )
		return;

	if( (int)m_vTopMoveNormalInfo.size() > nCombo && nCombo >= 0)
	{
		if( m_vTopMoveNormalInfo[nCombo].bCreate )
			pDummyItem2->ChangeToDummyChar( pOwner );

		pDummyItem2->SetMoveDummyPos( pOwner, m_vTopMoveNormalInfo[nCombo] );
	}
}

void ioChargeComboJump2::SetTopBlExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDummyCharItem2 *pDummyItem2 = ToDummyCharItem2( pItem );
	if( pDummyItem2 )
	{
		if( m_TopMoveChargeInfo.bCreate )
			pDummyItem2->ChangeToDummyChar( pOwner );

		pDummyItem2->SetMoveDummyPos( pOwner, m_TopMoveChargeInfo );

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( pOwner && !pOwner->IsActionStop() )
			pOwner->InitActionStopDelay( true );
		else
			pOwner->InitActionStopDelay( false );

		int iAniID = pGrp->GetAnimationIdx( m_TopBl_ChargeAttack.m_AttackAnimation );
		DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * m_TopBl_ChargeAttack.m_fAttackAniRate;
		m_dwActionEndTime = dwCurTime + dwDuration;

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_TopBl_ChargeAttack.m_fAttackAniRate, 0.0f, 0, true );

		m_ChargeComboState = CCS_TOP_BL;
	}

	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_TOP_BL;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	m_dwEnableNinjaDashTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_NORMAL;

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

	if( pOwner && !pOwner->IsActionStop() || m_bSetForceActionStopInit )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( m_ComboAttackList[m_iCurComboCnt].m_AttackAnimation );
	float fTimeRate = m_ComboAttackList[m_iCurComboCnt].m_fAttackAniRate;
	DWORD dwPreDelay = m_ComboAttackList[m_iCurComboCnt].m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_ComboAttackList[m_iCurComboCnt].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );

	//진화 닌자
	if( m_JumpExtendType == JET_NINJA_EXPANSION )
	{
		DWORD dwEnableDashTime = 0;
		if( pGrp )
		{
			if( iAniID != -1 ) 
			{
				dwEnableDashTime = pGrp->GetAnimationEventTime_e( iAniID, "ninja_dash" ) * fTimeRate;
				if( dwEnableDashTime > 0)
					m_dwEnableNinjaDashTime = FRAMEGETTIME() + dwEnableDashTime;
			}
		}
	}

	//진화 사신
	if ( m_JumpExtendType == JET_REAPER_EXPANSION )
	{
		if ( m_dwReaperExpansionTeleportTicTime > 0 )
			m_dwEnableReaperExpansionTeleportTime = dwCurTime + dwPreDelay + m_dwReaperExpansionTeleportTicTime;
	}

	if( m_JumpExtendType == JET_MAGICENGINEER )
		SetMacroTime( pGrp, iAniID, fTimeRate, m_ComboAttackList[m_iCurComboCnt].m_dwEndDelay );

	if( m_JumpExtendType == JET_MAGICSTAFF )
	{
		ioMagicStaffItem *pStaff = ToMagicStaffItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pStaff )
			pStaff->SetInputAttackInfo( pOwner, iAniID, fTimeRate );
	}
	
	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );
	
	//탑블레이드
	if(m_JumpExtendType == JET_TOP_BL && CheckIsExtend_Top_Bl_mod( pOwner ) )
		SetTopBlNormalAttack( pOwner, m_iCurComboCnt );

	//썬더버드
	if ( m_JumpExtendType == JET_THUNDER_BIRD )
	{
		ioThunderBirdObjectItem *pThunderBirdObjectItem = ToThunderBirdObjectItem( pOwner->GetObject() );
		if ( pThunderBirdObjectItem )
			pThunderBirdObjectItem->DecreaseGauge( pOwner, m_fThunderBirdNormalUseGauge );
	}

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

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();

	m_bUseExtraSpeedRate = false;
}

// double
void ioChargeComboJump2::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_ChargeComboState = CCS_DOUBLE;

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
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
		kPacket << m_fDoubleJumpPower;
		kPacket << m_fDoubleJumpForce;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump2::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;
	if( !m_bEnableDoubleJump && !m_bEnableDoubleJumpAfterDash ) return false;

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

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_DOUBLE:
		if( m_bEnableDoubleJump )
			break;
		return false;
	case CCS_DASH:
		if( m_bEnableDoubleJumpAfterDash )
			break;
		return false;
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

void ioChargeComboJump2::SetDoubleJumpAni( ioBaseChar *pOwner )
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
bool ioChargeComboJump2::IsCanFlashJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !m_bEnableFlashJump ) return false;

	if( m_ChargeComboState == CCS_DOUBLE )
		return true;

	return false;
}

bool ioChargeComboJump2::SetFlashJumpAttack( ioBaseChar *pOwner )
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
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fFlashDelayRate );

		dwDelayTime = pGrp->GetAnimationFullTime( iAniID ) * m_fFlashDelayRate;

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * m_fFlashDelayRate;

		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_bUseExtendJump = true;
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;

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

bool ioChargeComboJump2::IsCanJumpDashAfterNormalAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !m_bEnableJumpDash ) return false;
	if( m_iJumpDashCurCount >= m_iJumpDashMaxCount ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

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

	if( m_ChargeComboState != CCS_NONE )
	{
		if( m_ChargeComboState == CCS_DOUBLE && m_bUseDoubleJumpNewJumpDash )
		{
			;
		}
		else if( m_ChargeComboState == CCS_NORMAL && m_bUseAttackAfterNewJumpDash && m_iCurComboCnt == 1 )
		{
			;
		}
		else	
			return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

// jump dash
bool ioChargeComboJump2::IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !m_bEnableJumpDash ) return false;
	if( m_iJumpDashCurCount >= m_iJumpDashMaxCount ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->IsChatModeState( true ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if( m_ChargeComboState != CCS_NONE )
	{
		if( m_ChargeComboState == CCS_DOUBLE && m_bUseDoubleJumpNewJumpDash )
		{
			;
		}
		else if( m_ChargeComboState == CCS_NORMAL && m_bUseAttackAfterNewJumpDash && m_iCurComboCnt == 1 )
		{
			;
		}
		else	
			return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

int ioChargeComboJump2::GetJumpDashAni( ioBaseChar *pOwner )
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

	int iCnt = m_vJumpDashAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vJumpDashAniList[iIndex].IsEmpty() && !m_vJumpDashAniList[iIndex2].IsEmpty() )
		{
			if( fRate <= FLOAT05 )
				return pGrp->GetAnimationIdx( m_vJumpDashAniList[iIndex] );
			else
				return pGrp->GetAnimationIdx( m_vJumpDashAniList[iIndex2] );
		}
		else if( !m_vJumpDashAniList[iIndex].IsEmpty() && m_vJumpDashAniList[iIndex2].IsEmpty() )
		{
			return pGrp->GetAnimationIdx( m_vJumpDashAniList[iIndex] );
		}
		else if( !m_vJumpDashAniList[iIndex2].IsEmpty() && m_vJumpDashAniList[iIndex].IsEmpty() )
		{
			return pGrp->GetAnimationIdx( m_vJumpDashAniList[iIndex2] );
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

bool ioChargeComboJump2::IsJumpDashState()
{
	if( m_ChargeComboState == CCS_DASH )
	{
		if( m_dwActionEndTime > 0 && m_dwActionEndTime >= FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioChargeComboJump2::IsJumpAttackReserve()
{
	return m_bJumpAttackReserve;
}

void ioChargeComboJump2::CheckKeyReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;

	if( pOwner->IsAttackKey() && !m_bJumpAttackReserve && !pOwner->IsChatModeState( true ) )
	{
		m_bJumpAttackReserve = true;
	}
}

bool ioChargeComboJump2::CheckNinjaExtendMove( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	//닌자 진화 기능 추가
	if(	m_JumpExtendType == JET_NINJA_EXPANSION && m_dwEnableNinjaDashTime > 0 && 
		dwCurTime > m_dwEnableNinjaDashTime && pOwner->IsDirKeyDoubleClick() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if(	eDir != ioUserKeyInput::DKI_NONE  )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true );
		}

		pOwner->SetNinjaExpansionMove( true );
		return true;;
	}

	return false;
}

void ioChargeComboJump2::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	//진화 닌자 대쉬체크
	if( pOwner->IsNeedProcess() && CheckNinjaExtendMove( pOwner ) )
		return;

	switch( m_ChargeComboState )
	{
	case CCS_NORMAL:
		if( IsCanDoubleJump( pOwner, fHeightGap ) )
		{
			if( m_fDoubleJumpPower >= 0.0f )
			{
				SetDoubleJump( pOwner );
				return;
			}
		}

		ProcessComboJumpAttack( pOwner );
		break;
	case CCS_CHARGE:
		ProcessCharging( pOwner );
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
	case CCS_TITAN:
		ProcessTitanExtendAttack( pOwner );
		break;
	case CCS_THUNDER_BIRD:
		ProcessThunderBirdExtendAttack( pOwner );
		break;
	case CCS_PARKILPYO:
		ProcessParkilpyoExtendAttack( pOwner );
		break;
	case CCS_JACK_CHARGE:
		ProcessJackExtendAttack( pOwner );
		break;
	case CCS_KEY_INPUT_LAST_ATTACK:
		ProcessKeyInputLastAttack( pOwner );
		break;
	case CCS_MAGIC_SWORD_EXTEND_ATTACK:
		ProcessMagicSwordExtendAttack( pOwner );
		break;
	case CCS_MAGIC_SWORD_EXTEND_INPUT_KEY_ATTACK:
		ProcessMagicSwordExtendInputKeyAttack( pOwner );
		break;
	case CCS_BULLFIGHT_CHARGE:
		ProcessBullFightChargeAttack( pOwner );
		break;
	case CCS_BULLFIGHT_CHARGE_EXTEND:
		ProcessBullFightChargeExtendAttack( pOwner );
		break;
	case CCS_JEONWOOCHI_CHARGE:
		ProcessJeonWooChiExtendAttack( pOwner );
		break;
	case CCS_CHAIN_MAGICIAN_CHARGE:
		ProcessChainMagicianAttack( pOwner );
		break;
	case CCS_NEW_JUMP:
		ProcessNewJump( pOwner );
		break;
	}

	//전우치
	if( pOwner->IsNeedProcess() && (m_ChargeComboState == CCS_NORMAL || m_ChargeComboState == CCS_JEONWOOCHI_CHARGE_FAIL )
		&& m_JumpExtendType == JET_JEONWOOCHI && pOwner->IsDefenseKey() )
	{
		if( CheckJeonWooChiDummy( pOwner ) )
			return;
	}
	else if( pOwner->IsNeedProcess() && (m_ChargeComboState == CCS_NORMAL || m_ChargeComboState == CCS_EXTEND_ATTACK )
		&& m_JumpExtendType == JET_BLACK_SPHEAR && pOwner->IsDefenseKey() )
	{
		CheckBlackSpearBuffCheckState( pOwner );
	}

	if( m_JumpExtendType == JET_MAGICENGINEER && pOwner->IsNeedProcess() )
	{
		if( m_dwMESpecialMoveMacroTime_S && m_dwMESpecialMoveMacroTime_S < dwCurTime && m_dwMESpecialMoveMacroTime_E > dwCurTime )
		{
			ioMagicEngineerItem* pMEItem = ToMagicEngineerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pMEItem && pMEItem->IsEnableGauge() )
			{
				if( pOwner->IsDefenseKey() )
				{
					pMEItem->SetSpecailMoveState( pOwner, true, ioMagicEngineerItem::SM_DOWN );
					return;
				}
				else if( pOwner->IsJumpKey() )
				{
					pMEItem->SetSpecailMoveState( pOwner, true, ioMagicEngineerItem::SM_UP );
					return;
				}
			}
		}
	}
	if( pOwner->IsNeedProcess() && JET_MAGICSTAFF == m_JumpExtendType )
	{
		ioMagicStaffItem *pStaff = ToMagicStaffItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pStaff && pStaff->CheckInputAttack() )
		{
			if( pOwner->IsJumpKey() )
			{
				pStaff->SetInputAttackState( pOwner, true, ioMagicStaffItem::IAS_JUMP_A );
				return;
			}
			else if( pOwner->IsDefenseKey() )
			{
				pStaff->SetInputAttackState( pOwner, true, ioMagicStaffItem::IAS_JUMP_S );
				return;
			}
		}
	}
}

void ioChargeComboJump2::ProcessNewJump( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_NewJumpState )
	{
	case NJS_DASH:
		{
			float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
			if( fHeightGap < FLOAT10 && !m_JumpDashLandAni.IsEmpty() )
			{
				SetJumpDashLand( pOwner );
				return;
			}
			if( m_dwNewJumpEndTime > 0 && m_dwNewJumpEndTime < dwCurTime )
			{
				m_dwNewJumpEndTime = 0;
				m_NewJumpState = NJS_END;
			}
			else if( m_dwNewJumpAddAttackReserveTime > 0 && m_dwNewJumpAddAttackReserveTime < dwCurTime && pOwner->IsAttackKey() && 
				m_nNewJumpAttackIndex < (int)m_vNewJumpAddAttackList.size() )
			{
				m_nNewJumpAttackIndex = 0;
				m_NewJumpState = NJS_ADD_ATTACK;
				SetNewJumpDashAddAttack( pOwner, m_nNewJumpAttackIndex );
			}
		}
		break;
	case NJS_ADD_ATTACK:
		if( m_dwNewJumpEndTime > 0 && m_dwNewJumpEndTime < dwCurTime )
		{
			m_dwNewJumpEndTime = 0;
			m_NewJumpState = NJS_END;;
		}
		else if( m_dwNewJumpAddAttackReserveTime > 0 && m_dwNewJumpAddAttackReserveTime < dwCurTime && pOwner->IsAttackKey() &&
			m_nNewJumpAttackIndex < (int)m_vNewJumpAddAttackList.size() )
		{
			m_NewJumpState = NJS_ADD_ATTACK;
			SetNewJumpDashAddAttack( pOwner, m_nNewJumpAttackIndex );
		}
		break;
	case NJS_END:
		break;
	case NJS_DASH_LAND:
		if( m_dwNewJumpEndTime > 0 && m_dwNewJumpEndTime < dwCurTime )
		{
			m_dwNewJumpEndTime = 0;
			m_NewJumpState = NJS_END;
		}
		break;
	}
}

void ioChargeComboJump2::SetJumpDashLand( ioBaseChar* pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	m_NewJumpState = NJS_DASH_LAND;

	int iAniID = pGrp->GetAnimationIdx( m_JumpDashLandAni );
	float fTimeRate = m_fJumpDashLandAniRate;

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwNewJumpEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwNewJumpAddAttackReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetNewJumpAttackStateInfo( m_dwNewJumpEndTime, m_dwNewJumpEndTime, m_fJumpDashGravityAmt );
}

void ioChargeComboJump2::SetNewJumpDashAddAttack( ioBaseChar* pOwner, int nIndex )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	if( (int)m_vNewJumpAddAttackList.size() <= nIndex )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_vNewJumpAddAttackList[nIndex].m_AttackAnimation );
	float fTimeRate = m_vNewJumpAddAttackList[nIndex].m_fAttackAniRate;
	DWORD dwPreDelay = m_vNewJumpAddAttackList[nIndex].m_dwPreDelay;
	DWORD dwEndDelay = m_vNewJumpAddAttackList[nIndex].m_dwEndDelay;

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pOwner->ChangeDirectionByInputDir( false );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_vNewJumpAddAttackList[nIndex].m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	
	m_dwNewJumpEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_vNewJumpAddAttackList[nIndex].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwNewJumpAddAttackReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetComboDashState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_NEW_JUMP;
		kPacket << nIndex;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetNewJumpAttackStateInfo( m_dwNewJumpEndTime, m_dwNewJumpAddAttackReserveTime, m_fJumpDashGravityAmt );
	m_nNewJumpAttackIndex++;	
}

bool ioChargeComboJump2::IsEnableJumpDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( pOwner->GetUsedJumpAttack() && m_bUseAttackAfterNewJumpDash && 
		(dwCurTime >= m_dwEnableComboTime_S+dwLooseTime) )
		return true;

	if( m_JumpExtendType == JET_NINJA_EXPANSION && m_ChargeComboState != CCS_NONE )
		return true;

	if( m_JumpExtendType == JET_BLACK_SPHEAR && m_ChargeComboState != CCS_NONE && m_ChargeComboState != CCS_BLACK_SPEAR_DASH )
		return true;

	return false;
}

void ioChargeComboJump2::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	CheckAirJump( pOwner );

	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_bReserveJumpAttack )
	{
		if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
		{
			SetChargingState( pOwner );
		}
	}
	else if( m_bKeyInputLastAttack && !pOwner->IsHasCrown() )
	{
		int iMaxCnt = m_ComboAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			CheckKeyInputLastAttackState( pOwner );
	}
	//홍길동 진화때 추가
	else if( pOwner->IsNeedProcess () && IsEnableJumpDash(pOwner) && IsCanJumpDashAfterNormalAttack( pOwner, 0.f ) )
	{
		m_ChargeComboState = CCS_DASH;
		m_bUseExtendJump = true;

		m_dwActionEndTime = 0;
		m_iJumpDashCurCount++;

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

			if( !m_bUseJumpDashRotate )
				pOwner->SetMoveDirByRotate( qtRotate );
			else
				pOwner->SetTargetRotToRotate( qtRotate, false );
		}

		int iAniID = -1;
		float fTimeRate = FLOAT1;

		bool bDashAttack = SetDashAttack( pOwner, iAniID, fTimeRate );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_JUMP_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetMoveDir();
			kPacket << m_ChargeComboState;
			kPacket << false;					// dash end
			kPacket << pOwner->GetTargetRot();

			if( bDashAttack )
				pOwner->FillAutoTargetInfo( kPacket );

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		if( m_bUseNewJumpTypeDash )
		{
			DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
			SetNewJumpTypeDashAttack( pOwner, m_dwActionEndTime, dwKeyReserveTime );
			return;
		}
	}
	//Jump Combo Dash
	else if ( IsCanComboDash( pOwner, fHeightGap ) && dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
	{
		if ( m_JumpExtendType == JET_BLACK_SPHEAR )
		{
			SetBlackSpearJumpComboDashState( pOwner );
			return;
		}
	}
	else if ( CheckReaperExpansionTeleport( pOwner ) )
	{
		SetReaperExpansionTeleportState( pOwner );
		return;
	}
}

void ioChargeComboJump2::ProcessJeonWooChiExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime > m_dwEnableComboTime_E )
	{
		JeonWooChiExtendChargeFail( pOwner, true );
		return;
	}

	if( dwCurTime >	m_dwEnableComboTime_S && pOwner->IsAttackKey() )
	{
		m_iCurComboCnt = 0;
		SetExtendComboState( pOwner, true );
	}
}

void ioChargeComboJump2::ProcessTitanExtendAttack(  ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_ExtendAttackList.size() == 0 )
		return;

	const AttackAttribute& pAttr = m_ExtendAttackList[0];
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
	{
		m_dwCreateWeaponIndex = pOwner->IncreaseWeaponIndexBase();

		const WeaponInfo &rkInfo = pAttr.m_vWeaponInfoList.front();
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vDir = CheckTitanExtendDir( pOwner );

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = m_dwCreateWeaponIndex;
		kFireTime.eFireTimeType = FTT_JUMP_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		m_dwFireStartTime = 0;

		return;
	}

	if( m_dwCreateWeaponIndex == 0 )
		return;

	ioUroborusWeapon *pUroborusWeapon = ToUroborusWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCreateWeaponIndex ) );
	if( !pUroborusWeapon )
	{
		// End 처리.
		TitanExtendMoveFail( pOwner, false );
		return;
	}

	if( pUroborusWeapon->IsCanUroborusJump() )
	{
		ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
		ioTitanItem *pTitanItem = ToTitanWeaponItem( pItem );
		if( !pTitanItem )
			return;

		if( pOwner->IsNeedProcess() && pTitanItem->IsEnableGauge() && pOwner->IsJumpKeyDown() )
		{
			pTitanItem->DecreaseBullet();

			D3DXVECTOR3 vTargetPos = pUroborusWeapon->GetPosition();
			pOwner->SetTitanExtendMoveState( m_JumpTitanExtend.attribute.m_AttackAnimation, m_JumpTitanExtend.attribute.m_fAttackAniRate,
				vTargetPos, m_JumpTitanExtend.m_fMoveSpeed, m_JumpTitanExtend.m_fMoveRate, true );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( !pGrp )
				return;

			int iAniID = pGrp->GetAnimationIdx( m_JumpTitanExtend.attribute.m_AttackAnimation );

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetActionAni( m_JumpTitanExtend.attribute.m_AttackAnimation, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_JumpTitanExtend.attribute.m_fAttackAniRate );

			pOwner->RefreshFireTimeList( iAniID, m_JumpTitanExtend.attribute.m_vWeaponInfoList, FTT_JUMP_ATTACK, m_JumpTitanExtend.attribute.m_fAttackAniRate,
				m_JumpTitanExtend.attribute.m_dwPreDelay);

			pOwner->IncreaseWeaponIndexBase();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_EXTEND_JUMP );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetHP().m_fCurValue;
				kPacket << CCS_TITAN_MOVE_STATE;
				kPacket << pOwner->GetTargetRot();
				kPacket << vTargetPos;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			return;
		}
	}
	else if( !pUroborusWeapon->IsUroborusMove() && !pUroborusWeapon->IsUroborusWait() )
	{
		if( pOwner->IsNeedProcess() )
		{
			// End 처리
			TitanExtendMoveFail( pOwner, true );
			return;
		}
	}
}

void ioChargeComboJump2::ProcessDelay( ioBaseChar *pOwner )
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

void ioChargeComboJump2::ProcessFlashMove( ioBaseChar *pOwner )
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

void ioChargeComboJump2::ProcessFlashEndMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;
}

void ioChargeComboJump2::ProcessJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_ChargeComboState != CCS_DASH ) return;
	if( m_bDashEnd ) return;
	if( m_dwActionEndTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwActionEndTime < dwCurTime )
	{
		int iAniID = pOwner->GetJumppingAnimationIdx();
		if( pOwner && iAniID != -1 )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );

				m_bDashEnd = true;
				m_ChargeComboState = CCS_NONE;
				pOwner->ClearDirDoubleClick();

				if( m_fDashEndJumpPower != 0 )
				{
					pOwner->SetCurJumpPowerAmt( 0.0f );
					pOwner->SetForcePowerAmt( 0.0f );
					pOwner->SetGravityAmt( 0.0f );
					pOwner->SetJumpPower( m_fDashEndJumpPower );

					D3DXVECTOR3 vJumpDir = ioMath::UNIT_Y;
					if( m_fDashEndJumpPower > 0.0f )
						pOwner->SetForcePower( vJumpDir, fabs(m_fDashEndJumpPower), FLOAT1, true );
					else
						pOwner->SetForcePower( -vJumpDir, fabs(m_fDashEndJumpPower), FLOAT1, true );
				}

				if( pOwner->IsNeedProcess() )
				{
					pOwner->ChangeDirectionByInputDir( false );

					D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_JUMP_DASH );
						kPacket << pOwner->GetCharName();
						kPacket << (int)GetType();
						kPacket << vMoveDir;
						kPacket << CCS_DASH;
						kPacket << true;					// dash end
						kPacket << pOwner->GetTargetRot();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
		}
	}
	else
	{
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약채크
			CheckKeyReserve( pOwner );
		}
	}
}

void ioChargeComboJump2::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

			if( !COMPARE( m_iCurComboCnt, 0, (int) m_ComboAttackList.size() ) )
				return;

			m_ChargeComboState = CCS_NORMAL;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			if( pOwner && !pOwner->IsActionStop() )
				pOwner->InitActionStopDelay( true );
			else
				pOwner->InitActionStopDelay( false );

			int iAniID = pGrp->GetAnimationIdx( m_ComboAttackList[m_iCurComboCnt].m_AttackAnimation );
			float fTimeRate = m_ComboAttackList[m_iCurComboCnt].m_fAttackAniRate;
			DWORD dwPreDelay = m_ComboAttackList[m_iCurComboCnt].m_dwPreDelay;
			m_dwEnableComboTime_E = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			pOwner->SetAutoTarget( ATT_JUMP );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pOwner->RefreshFireTimeList( iAniID,
				m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
				FTT_JUMP_ATTACK,
				fTimeRate,
				dwPreDelay );

			pOwner->IncreaseWeaponIndexBase();

			DWORD dwStartTime = dwCurTime + dwPreDelay;
			DWORD dwTrackingTime = dwStartTime;
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

			m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

			// For AirJumpPower
			SetAirJump( pOwner, iAniID, fTimeRate );

			//탑블레이드
			if(m_JumpExtendType == JET_TOP_BL && CheckIsExtend_Top_Bl_mod( pOwner ) )
				SetTopBlNormalAttack( pOwner, m_iCurComboCnt );

			//썬더버드
			if ( m_JumpExtendType == JET_THUNDER_BIRD )
			{
				ioThunderBirdObjectItem *pThunderBirdObjectItem = ToThunderBirdObjectItem( pOwner->GetObject() );
				if ( pThunderBirdObjectItem )
					pThunderBirdObjectItem->DecreaseGauge( pOwner, m_fThunderBirdNormalUseGauge );
			}

			pOwner->ApplyAutoTargetInfo( rkPacket );

			m_bReserveJumpAttack = false;
			m_bUseExtendJump = true;
			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			m_iCurComboCnt++;
			pOwner->SetJumpAttackState();
		}
		break;
	case CCS_DOUBLE:
		{
			float fJumpPower, fJumpForce;
			D3DXQUATERNION qtRotate;

			rkPacket >> fJumpPower >> fJumpForce;
			rkPacket >> qtRotate;

			if( fJumpPower >= 0.0f )
			{
				m_ChargeComboState = CCS_DOUBLE;

				m_bUsedDoubleJump = true;
				m_bUseExtendJump = true;

				pOwner->SetCurJumpPowerAmt( 0.0f );
				pOwner->SetForcePowerAmt( 0.0f );
				pOwner->SetGravityAmt( 0.0f );

				pOwner->SetTargetRot( qtRotate );
				pOwner->SetJumpPower( fJumpPower );

				D3DXVECTOR3 vJumpDir = pOwner->GetMoveDir();
				if( m_fDoubleJumpForce > 0.0f )
					pOwner->SetForcePower( vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );
				else
					pOwner->SetForcePower( -vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );

				SetDoubleJumpAni( pOwner );
			}
		}
		break;
	case CCS_DELAY:
		{
			rkPacket >> m_iCurJumpCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			if( !m_FlashDelayAni.IsEmpty() )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );

				pOwner->SetAutoTarget( ATT_JUMP );

				int iAniID = pGrp->GetAnimationIdx( m_FlashDelayAni );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fFlashDelayRate );

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

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_DASH:
		{
			bool bEnd;
			rkPacket >> bEnd;

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			if( !bEnd )
			{
				pOwner->SetCurJumpPowerAmt( 0.0f );
				pOwner->SetForcePowerAmt( 0.0f );
				pOwner->SetGravityAmt( 0.0f );

				pOwner->SetTargetRot( qtRotate );

				int iAniID = -1;
				float fTimeRate = FLOAT1;
				bool bDashAttack = false;
				bDashAttack = SetDashAttack( pOwner, iAniID, fTimeRate );
				m_ChargeComboState = CCS_DASH;
				m_bUseExtendJump = true;

				if( bDashAttack )
				{
					pOwner->ApplyAutoTargetInfo( rkPacket );
				}

				if( m_bUseNewJumpTypeDash )
				{
					DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
					SetNewJumpTypeDashAttack( pOwner, m_dwActionEndTime, dwKeyReserveTime );
				}
			}
			else
			{
				pOwner->SetTargetRot( qtRotate );
				pOwner->SetMoveDirByRotate( qtRotate );

				int iAniID = pOwner->GetJumppingAnimationIdx();
				if( pOwner && iAniID != -1 )
				{
					ioEntityGroup *pGrp = pOwner->GetGroup();
					if( pGrp )
					{
						pGrp->ClearAllActionAni( FLOAT100, true );
						pGrp->ClearAllLoopAni( FLOAT100, true );
						pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );

						m_bDashEnd = true;
						m_ChargeComboState = CCS_NONE;

						pOwner->ClearDirDoubleClick();

						if( m_fDashEndJumpPower != 0 )
						{
							pOwner->SetCurJumpPowerAmt( 0.0f );
							pOwner->SetForcePowerAmt( 0.0f );
							pOwner->SetGravityAmt( 0.0f );
							pOwner->SetJumpPower( m_fDashEndJumpPower );

							D3DXVECTOR3 vJumpDir = ioMath::UNIT_Y;
							if( m_fDashEndJumpPower > 0.0f )
								pOwner->SetForcePower( vJumpDir, fabs(m_fDashEndJumpPower), FLOAT1, true );
							else
								pOwner->SetForcePower( -vJumpDir, fabs(m_fDashEndJumpPower), FLOAT1, true );
						}
					}
				}
			}
		}
		break;
	case CCS_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			bool bException;
			rkPacket >> bException;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChargingState( pOwner, bException );
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
	case CCS_DESTROYER:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetDestroyerFlyState( pOwner );
		}
		break;
	case CCS_MICHAEL_FLY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetMichaelFlyState( pOwner );
		}
		break;
	case CCS_GOOMIHO:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetGoomihoFlyState( pOwner );
		}
		break;
	case CCS_DRACULA:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetDraculaState( pOwner );
		}
		break;
	case CCS_EVY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetEvyBlinkState( pOwner );
		}
		break;
	case CCS_SLASHER:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetSlasherState( pOwner );
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
	case CCS_TOP_BL:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetTopBlExtendAttack( pOwner );
		}
		break;
	case CCS_JEONWOOCHI_CHARGE:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir;
			//rkPacket >> vMoveDir;

			pOwner->SetTargetRotToRotate( qtRot, true );
			pOwner->SetInputDirection( vMoveDir );

			SetJeonWooChiExtendAttack( pOwner );
		}
		break;
	case CCS_JEONWOOCHI_CHARGE_FAIL:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );

			JeonWooChiExtendChargeFail( pOwner, false );
		}
		break;
	case CCS_TITAN:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetTitanExtendAttack( pOwner );
		}
		break;
	case CCS_TITAN_MOVE_STATE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vTargetPos;
			rkPacket >> vTargetPos;
			
			pOwner->SetTitanExtendMoveState( m_JumpTitanExtend.attribute.m_AttackAnimation, m_JumpTitanExtend.attribute.m_fAttackAniRate,
				vTargetPos, m_JumpTitanExtend.m_fMoveSpeed, m_JumpTitanExtend.m_fMoveRate, true );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( !pGrp )
				break;

			int iAniID = pGrp->GetAnimationIdx( m_JumpTitanExtend.attribute.m_AttackAnimation );

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetActionAni( m_JumpTitanExtend.attribute.m_AttackAnimation, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_JumpTitanExtend.attribute.m_fAttackAniRate );

			pOwner->RefreshFireTimeList( iAniID, m_JumpTitanExtend.attribute.m_vWeaponInfoList, FTT_JUMP_ATTACK, m_JumpTitanExtend.attribute.m_fAttackAniRate,
				m_JumpTitanExtend.attribute.m_dwPreDelay);

			pOwner->IncreaseWeaponIndexBase();
		}
		break;
	case CCS_TITAN_MOVE_FAIL:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			TitanExtendMoveFail( pOwner, false );
		}
		break;
	case CCS_KAELTHAS_EXTEND:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			SetKaelthasExtendAttack( pOwner );
		}
		break;
	case CCS_THUNDER_BIRD:
		{
			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;
			pOwner->SetWeaponIndexBase( dwWeaponBaseIndex );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetThunderBirdExtendAttack( pOwner );
		}
		break;
	case CCS_PARKILPYO:
		{
			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;
			pOwner->SetWeaponIndexBase( dwWeaponBaseIndex );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetParkilpyoExtendAttack( pOwner );
		}
		break;
	case CCS_JACK_CHARGE:
		{
			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;
			pOwner->SetWeaponIndexBase( dwWeaponBaseIndex );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetJackExtendAttack( pOwner );
		}
		break;
	case CCS_EXTRA_JUMP:
		{
			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;
			pOwner->SetWeaponIndexBase( dwWeaponBaseIndex );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			SetExtraJump( pOwner );
		}
		break;
	case CCS_JACK_EXTRA_JUMP_ATTACK:
		{
			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;
			pOwner->SetWeaponIndexBase( dwWeaponBaseIndex );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetJackExtraJumpAttack( pOwner );
		}
		break;
	case CCS_EXTRA_JUMP_STATE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			pOwner->SetExtraJumpState();
		}
		break;
	case CCS_KEY_INPUT_LAST_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );
			bool bAttackKey,bDefenseKey,bJumpKey;
			rkPacket >> bAttackKey;
			rkPacket >> bDefenseKey;
			rkPacket >> bJumpKey;
			SetKeyInputLastAttackState( pOwner, bAttackKey, bDefenseKey, bJumpKey );
		}
		break;
	case CCS_MAGIC_SWORD_EXTEND_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			SetMagicSwordExtendAttack( pOwner );
		}
		break;
	case CCS_MAGIC_SWORD_EXTEND_INPUT_KEY_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );
			bool bAttackKey,bDefenseKey,bJumpKey;
			rkPacket >> bAttackKey;
			rkPacket >> bDefenseKey;
			rkPacket >> bJumpKey;
			SetMagicSwordExtendInputKeyAttack( pOwner, bAttackKey, bDefenseKey, bJumpKey );
		}
		break;
	case CCS_BULLFIGHT_CHARGE:
		{
			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;
			pOwner->SetWeaponIndexBase( dwWeaponBaseIndex );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetBullFightChargeAttack( pOwner );
		}
		break;
	case CCS_BULLFIGHT_CHARGE_EXTEND:
		{
			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;
			pOwner->SetWeaponIndexBase( dwWeaponBaseIndex );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetBullFightChargeExtendAttack( pOwner );
		}
		break;
	case CCS_CHAIN_MAGICIAN_CHARGE:
		{
			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;
			pOwner->SetWeaponIndexBase( dwWeaponBaseIndex );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChainMagicianAttack( pOwner );
		}
		break;
	case CCS_NEW_JUMP:
		{
			int nIndex;
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> nIndex;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			SetNewJumpDashAddAttack( pOwner, nIndex );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_BLACK_SPEAR_DASH:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;
			pOwner->SetTargetRotAndMoveDirChange( qtRotate );
			SetBlackSpearJumpComboDashState( pOwner );
		}
		break;
	}
}

void ioChargeComboJump2::SetLandingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_bReserveJumpAttack = false;

	m_ChargeComboState = CCS_NONE;
	m_NewJumpState = NJS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	m_dwEnableNinjaDashTime = 0;
	m_iCurJumpComboDashCount = 0;
	m_bUsingExpansionReaperJumpChargeAttack = false;

	ioMagicEngineerItem	*pMagicEngineerItem = ToMagicEngineerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMagicEngineerItem )
		pMagicEngineerItem->ClearJumpDummy();
}

float ioChargeComboJump2::GetCurFlashForceAmt()
{
	switch( m_ChargeComboState )
	{
	case CCS_FLASH:
		return m_fFlashForce;
	}

	return 0.0f;
}

float ioChargeComboJump2::GetCurFlashGravityAmt()
{
	switch( m_ChargeComboState )
	{
	case CCS_FLASH:
		return m_fFlashGravityAmt;
	}

	return 0.0f;
}

bool ioChargeComboJump2::IsNoProcessGravity()
{
	switch( m_ChargeComboState )
	{
	case CCS_TITAN:
	case CCS_JEONWOOCHI_CHARGE:
		return true;
	}

	return false;
}

float ioChargeComboJump2::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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
	else if( m_bUseExtendJump || m_bUseExtraSpeedRate )
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

bool ioChargeComboJump2::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_DELAY:
	case CCS_FLASH:
	case CCS_FLASH_END:
	case CCS_NEW_JUMP:
		return false;
	case CCS_DASH:
		if( m_bDashEnd )
			return true;

		return false;
	}

	return true;
}

DWORD ioChargeComboJump2::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

void ioChargeComboJump2::SetChargingState( ioBaseChar *pOwner, bool bException )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_ChargingAniList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioHashString szCurChargingAni;

	bool bMaxJumpLimit = pOwner->CheckJumpLimitCnt();

	if( pOwner->IsNeedProcess() && m_bMaxJumpLastChargeAttack && !bMaxJumpLimit )
	{
		m_bUseExtendJump = true;

		pOwner->SetEnableHide( false );
		pOwner->SetNoWoundState( false );
		pOwner->SetEnableChangeTeam( false );

		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
		pOwner->SetJumpAttackState();

		m_ChargeComboState = CCS_CHARGE;
		m_dwChargingStartTime = dwCurTime;
		
		bException = true;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << m_ChargeComboState;
			kPacket << pOwner->GetTargetRot();
			kPacket << bException;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
	else if( !pOwner->IsNeedProcess() && bException )
	{
		m_bUseExtendJump = true;

		pOwner->SetEnableHide( false );
		pOwner->SetNoWoundState( false );
		pOwner->SetEnableChangeTeam( false );

		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
		pOwner->SetJumpAttackState();

		m_ChargeComboState = CCS_CHARGE;
		m_dwChargingStartTime = dwCurTime;

		return;
	}
	else if( m_JumpExtendType == JET_MAGICENGINEER && !CheckMagicEngineerChargeAttack( pOwner, false ) )
	{
		SetComboJumpAttack( pOwner );
		return;
	}
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
		kPacket << bException;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
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
			case JET_MICHAEL_FLY:
				if( CheckEnableMichaelFlyState( pOwner ) )
					SetMichaelFlyState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_GOOMIHO_FLY:
				if( CheckEnableGoomihoFlyState( pOwner ) )
					SetGoomihoFlyState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_SCATHI:
				if( CheckEnableScathiState(pOwner) )
					SetScathiState(pOwner);
				else
					SetComboJumpAttack( pOwner );	
				return;
			case JET_DRACULA:
				if( CheckEnableDraculaState( pOwner ) )
					SetDraculaState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_EVY:
				if( CheckEnableEvyBlinkState( pOwner ) )
					SetEvyBlinkState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_SLASHER:
				if( CheckEnableSlasherState( pOwner ) )
					SetSlasherState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_TOP_BL:
				if( CheckIsExtend_Top_Bl_mod( pOwner ) )
					//회수
					SetTopBlExtendAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_TITAN:
				if( CheckIsEnableTitanExtendAttack( pOwner ) )
					SetTitanExtendAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_KAELTHAS:
				if( CheckKaelthasExtendAttack( pOwner) )
					SetKaelthasExtendAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_JEONWOOCHI:
				if( CheckEnableJeonWooChiChargeAttack( pOwner ) )
					SetJeonWooChiExtendAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_THUNDER_BIRD:
				if( CheckIsEnableThunderBirdExtendAttack( pOwner ) )
					SetThunderBirdExtendAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_PARKILPYO:
				SetParkilpyoExtendAttack( pOwner );
				return;
			case JET_JACK:
				if( CheckEnableJackExtendAttack( pOwner ) )
					SetJackExtendAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_ANGEL_OF_DEATH:
				if( CheckEnableExtendCombo( pOwner ) )
				{
					SetExtendComboState( pOwner, true );
				}
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_MAGIC_SWORD:
				if ( CheckEnableMagicSwordExtendAttack(pOwner) )
					SetMagicSwordExtendAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_RESEARCHER_REAR_FLY:
				if ( CheckEnableResearcherRearFlyState(pOwner) )
					SetResearcherRearFlyState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_BULLFIGHT:
				if( CheckEnableBullFightChargeState( pOwner ) )
					SetBullFightChargeAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_SAWANG_SLIDE:
				if( CheckEnableSawangState(pOwner) )
					SetSawangState(pOwner);
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_CHMISTRY:
				if( CheckEnableChemistryState(pOwner) )
					SetChemistryState(pOwner);
				else
					SetComboJumpAttack( pOwner );	
				return;
			case JET_CHAIN_MAGICIAN:
				if( CheckIsEnableChainMagicianAttack( pOwner ) )
					SetChainMagicianAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_SURFER:
				if( CheckEnableSurferState( pOwner ) )
					SetSurferState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_REAPER_EXPANSION:
				if( CheckExpansionReaperJumpChargeAttackState( pOwner ) )
					ChangeToExpansionReaperJumpChargeAttackState( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_EXPANSION_LANCER:
				if( CheckExpansionLancerChargeAttack( pOwner ) )
					ChangeToExpansionLancerChargeAttack( pOwner );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_MAGICENGINEER:
				if( CheckMagicEngineerChargeAttack( pOwner, true ) )
					ChangeToMagicEngineerChargeAttack( pOwner, false );
				else
					SetComboJumpAttack( pOwner );
				return;
			case JET_SANTA:
				if( CheckSantaChargeAttack( pOwner ) )
					ChangeToSantaChargeAttack( pOwner );
				else
					SetExtendComboState( pOwner, false );
				return;
			}

			ChangeToPowerChargeJumpAttack( pOwner );
			return;
		}
	}
	else
	{
		if( m_dwChargingStartTime+m_dwHalfChargingTime < dwCurTime )
		{
			switch( m_JumpExtendType )
			{
			case JET_MAGICENGINEER:
				if( CheckMagicEngineerChargeAttack( pOwner, true) )
				{
					ChangeToMagicEngineerChargeAttack( pOwner, true );
					return;
				}
				break;
			}
		}
		bool bMaxJumpLimit = pOwner->CheckJumpLimitCnt();
		if( !(m_bMaxJumpLastChargeAttack && !bMaxJumpLimit) )
		{
			if( m_JumpExtendType == JET_SANTA )
				SetExtendComboState( pOwner, false );
			else
				SetComboJumpAttack( pOwner );
		}
		return;
	}
}

int ioChargeComboJump2::GetCurChargeComboState()
{
	return m_ChargeComboState;
}

void ioChargeComboJump2::ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTEND_JUMP_ATTACK );
	if( !pAttr )
		return;

	if( !pOwner->IsEnableExtendJump() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwEnableNinjaDashTime = 0;

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

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	//진화 닌자
	if( m_JumpExtendType == JET_NINJA_EXPANSION )
	{
		DWORD dwEnableDashTime = 0;
		if( pGrp )
		{
			if( iAniID != -1 ) 
			{
				dwEnableDashTime = pGrp->GetAnimationEventTime_e( iAniID, "ninja_dash" ) * fTimeRate;
				if( dwEnableDashTime > 0)
					m_dwEnableNinjaDashTime = FRAMEGETTIME() + dwEnableDashTime;
			}
		}
	}

	if( m_JumpExtendType == JET_MAGICENGINEER )
		SetMacroTime( pGrp, iAniID, fTimeRate, pAttr->m_dwEndDelay );

	if( m_JumpExtendType == JET_MAGICSTAFF )
	{
		ioMagicStaffItem *pStaff = ToMagicStaffItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pStaff )
			pStaff->SetInputAttackInfo( pOwner, iAniID, fTimeRate );
	}
	
	m_ChargeComboState = CCS_EXTEND_ATTACK;
	m_bUsedChargeAttack = true;
	m_bChargeAttacked = true;

	// For AirJumpPower
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	
	m_dwChargeAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	float fCurGravity = pOwner->GetCharGravity();
	float fFlightDuration = GetFlightDuration( fCurGravity );
	if( fFlightDuration > 0.0f )
	{
		m_fCurCharMoveSpeed = m_fChargeBaseRange / (fFlightDuration / FLOAT1000);
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

float ioChargeComboJump2::GetFlightDuration( float fCurGravity )
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

void ioChargeComboJump2::PowerChargeMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_ChargeComboState != CCS_EXTEND_ATTACK )
		return;

	if( !pChar ) return;
	if( !pStage ) return;
	if( m_fCurCharMoveSpeed == 0.0f ) return;

	float fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

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

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pChar )
		dwLooseTime = pChar->GetActionStopTotalDelay();
	//Jump Combo Dash
	if ( IsCanComboDash( pChar, fHeightGap ) && dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
	{
		if ( m_JumpExtendType == JET_BLACK_SPHEAR )
		{
			SetBlackSpearJumpComboDashState( pChar );
			return;
		}
	}
}

bool ioChargeComboJump2::IsChargeAttackState()
{
	return m_bUsedChargeAttack;
}

bool ioChargeComboJump2::CheckEnableBuffAddState( ioBaseChar *pOwner )
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

void ioChargeComboJump2::SetBuffAddState( ioBaseChar *pOwner )
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
	m_NewJumpState = NJS_NONE;
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

bool ioChargeComboJump2::CheckEnableMechanicsFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->IsEnableMechanicsFly() )
		return true;

	return false;
}

void ioChargeComboJump2::SetMechanicsFlyState( ioBaseChar *pOwner )
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

bool ioChargeComboJump2::CheckEnableDestroyerFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->IsEnableDestroyerFly() )
		return true;

	return false;
}

void ioChargeComboJump2::SetDestroyerFlyState( ioBaseChar *pOwner )
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

bool ioChargeComboJump2::CheckEnableExcaliburSpecialAtt( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioExcaliburItem *pExcalibur = ToExcaliburItem(pOwner->GetPriorityItem( SPT_JUMP ));
	if( pExcalibur && pExcalibur->IsEnableSpcialAttack() )
		return true;

	return false;
}

void ioChargeComboJump2::SetExcaliburSpecialAtt( ioBaseChar *pOwner )
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

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

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

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
}

bool ioChargeComboJump2::CheckEnableExtendCombo( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_ExtendAttackList.empty() )
		return false;

	return true;
}

void ioChargeComboJump2::SetExtendComboState( ioBaseChar *pOwner, bool bRefresh )
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

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_bReserveJumpAttack = false;
	m_bUseExtendJump = false;
	m_bUseExtraSpeedRate = true;

	if( m_bExtendChnageNewJumpAttackState )
	{
		pOwner->SetCurJumpPowerAmt( 0.0f );
		if( bRefresh )
		{
			pOwner->SetGravityAmt( 0.0f );		
			pOwner->SetForcePowerAmt( 0.0f );
		}
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

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_ExtendAttackList[m_iCurComboCnt].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( m_bEnableExtendComboColSkip )
		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	if( m_bExtendChnageNewJumpAttackState )
		pOwner->SetNewJumpAttackStateInfo( m_dwEnableComboTime_E, m_dwEnableComboTime_S, fGravityAmt, bRefresh );
	else
		pOwner->SetJumpAttackState();

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

bool ioChargeComboJump2::CheckEnablePhantomFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->IsEnablePhantomFly() )
		return true;

	return false;
}

void ioChargeComboJump2::SetPhantomFlyState( ioBaseChar *pOwner )
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

void ioChargeComboJump2::ProcessExtendComboAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	CheckAirJump( pOwner );

	if( m_JumpExtendType == JET_ANGEL_OF_DEATH )
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

void ioChargeComboJump2::CheckExtendComboAttack( ioBaseChar *pOwner )
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

void ioChargeComboJump2::OnEndExtendJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}
}

bool ioChargeComboJump2::CheckEnableMichaelFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->IsEnableMichaelFly() )
		return true;

	return false;
}

void ioChargeComboJump2::SetMichaelFlyState( ioBaseChar *pOwner )
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

	// 아이템쪽에서 MichaelFlyInfo의 필요정보만 취한다!
	// 방향키 방향으로 회전후 처리
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	pOwner->SetTargetRotToDirKey( eNewDirKey );
	pOwner->SetMichaelFlyStateByJumpDash( m_JumpMichaelFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_MICHAEL_FLY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump2::CheckEnableEvyBlinkState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioEvyItem			*pEvyItem = ToEvyItem( pItem );

	//점프 카운트
	if( !m_bCanEvySpecial )
		return false;

	if( pEvyItem && pEvyItem->IsEnableGauge() && !pOwner->IsHasCrown() )
		return true;

	return false;
}

void ioChargeComboJump2::SetEvyBlinkState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->ClearAttackFireTimeAndSkill();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->SetEvyBlinkStateByJumpDash( m_JumpEvy );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_EVY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump2::CheckEnableSlasherState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;
	
	if ( pOwner->IsEnableSlasherSpecial() )
		return true;

	return false;
}

void ioChargeComboJump2::SetSlasherState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->ClearAttackFireTimeAndSkill();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->SetSlasherSpecialState( m_JumpSlasher );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_SLASHER;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump2::CheckIsExtend_Top_Bl_mod( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioDummyCharItem2 *pDummyItem2 = ToDummyCharItem2( pItem );
	if( pDummyItem2 && pDummyItem2->GetMeshType() == ioDummyCharItem2::IMT_EXTRA )
		return true;

	return false;
}

bool ioChargeComboJump2::CheckKaelthasExtendAttack( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioKaelthasItem* pKaelthasItem = ToKaelthasItem( pItem );
	if( pKaelthasItem && pKaelthasItem->GetCurBullet() >= (int)m_fJumpExtraGauge )
		return true;

	return false;
}

bool ioChargeComboJump2::CheckEnableJeonWooChiChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioJeonWooChiItem *pJeonWooChiItem = ToJeonWooChiWeaponItem( pItem );
	if( pJeonWooChiItem && pJeonWooChiItem->IsEnableGauge() && !pOwner->IsHasCrown() )
		return true;

	return false;
}

void ioChargeComboJump2::SetJeonWooChiExtendAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeComboState = CCS_JEONWOOCHI_CHARGE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.f );

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioJeonWooChiItem *pJeonWooChiItem = ToJeonWooChiWeaponItem( pItem );
	if( pJeonWooChiItem )
		pJeonWooChiItem->DecreaseBullet();

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

			pOwner->SetTargetRotToRotate( qtRotate, true );
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

	int iAniID = pGrp->GetAnimationIdx( m_JeonWooChiExtendAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_JeonWooChiExtendAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_JeonWooChiExtendAttack.m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_JeonWooChiExtendAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

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
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
}

void ioChargeComboJump2::SetKaelthasExtendAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioKaelthasItem* pKaelthasItem = ToKaelthasItem( pItem );
	if( !pKaelthasItem )
		return;

	pKaelthasItem->SetCurBullet( max( 0.f, pKaelthasItem->GetCurBullet() - (int)m_fJumpExtraGauge) );

	m_ChargeComboState = CCS_KAELTHAS_EXTEND;

	if( pOwner->IsNeedProcess() )
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

	int iAniID = pGrp->GetAnimationIdx( m_KaelthasExtendAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_KaelthasExtendAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_KaelthasExtendAttack.m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_KaelthasExtendAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pOwner->SetReservedSliding( m_KaelthasExtendAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

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
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
}

bool ioChargeComboJump2::CheckIsEnableTitanExtendAttack( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioTitanItem *pTitanItem = ToTitanWeaponItem( pItem );
	if( pTitanItem && !pOwner->IsHasCrown() )
		return true;

	return false;
}

void ioChargeComboJump2::SetTitanExtendAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_ExtendAttackList.size() == 0 )
		return;
	
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioTitanItem *pTitanItem = ToTitanWeaponItem( pItem );
	if( !pTitanItem )
		return;

	const AttackAttribute& pAttr = m_ExtendAttackList[0];

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = 0;

	int iAniID = pGrp->GetAnimationIdx( pAttr.m_AttackAnimation );
	float fTimeRate = pAttr.m_fAttackAniRate;
	DWORD dwPreDelay = pAttr.m_dwPreDelay;

	if( pOwner->IsNeedProcess() )
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

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	float fFire = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStartTime = dwPreDelay + dwCurTime + fFire;

	m_ChargeComboState = CCS_TITAN;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

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

bool ioChargeComboJump2::CheckEnableDraculaState(  ioBaseChar *pOwner  )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioDraculaItem		*pDraculaItem = ToDraculaItem( pItem );

	if( pDraculaItem && pOwner->IsEnableDraculaFly() )
	{
		if( !pOwner->IsHasCrown())
			return true;
	}

	return false;
}

void ioChargeComboJump2::SetDraculaState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->ClearAttackFireTimeAndSkill();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	
	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	pOwner->SetDraculaFlyState( m_JumpDracula );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_DRACULA;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump2::CheckEnableChemistryState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioChemistryItem		*pChmistryItem = ToChemistryItem( pItem );

	if( pChmistryItem )
	{
		if( !pOwner->IsHasCrown())
			return true;
	}

	return false;
}

void ioChargeComboJump2::SetChemistryState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioChemistryItem		*pChemistryItem = ToChemistryItem( pItem );
	if( pChemistryItem )
	{
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

		pChemistryItem->SetMakeExtendAttack( true, false );
		pOwner->SetNormalAttack(0);
	}
}

bool ioChargeComboJump2::CheckEnableScathiState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioScathiItem		*pScathiItem = ToScathiItem( pItem );

	if( pScathiItem )
	{
		if( !pOwner->IsHasCrown())
		return true;
	}

	return false;
}

void ioChargeComboJump2::SetScathiState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioAttackableItem	*pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioScathiItem		*pScathiItem = ToScathiItem( pItem );
	if( pScathiItem )
	{
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

		pScathiItem->SetMakeExtendAttack();
		pOwner->SetNormalAttack(0);
	}
}

bool ioChargeComboJump2::CheckEnableGoomihoFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->IsEnableGoomihoFly() )
		return true;

	return false;
}

void ioChargeComboJump2::SetGoomihoFlyState( ioBaseChar *pOwner )
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

	// 아이템쪽에서 MichaelFlyInfo의 필요정보만 취한다!
	// 방향키 방향으로 회전후 처리
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	pOwner->SetTargetRotToDirKey( eNewDirKey );
	pOwner->SetGoomihoFlyStateByJumpDash( m_JumpGoomihoFly );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_GOOMIHO;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump2::IsCanMaxJumpAndChargeAttack()
{
	return m_bMaxJumpLastChargeAttack;
}

void ioChargeComboJump2::SetEvySpecial( bool b )
{
	m_bCanEvySpecial = b;
}

D3DXVECTOR3	ioChargeComboJump2::CheckTitanExtendDir( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	if( !pOwner )
		return vDir;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

	D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vSightDir, &ioMath::UNIT_Y );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_JumpTitanExtend.m_fMoveAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	vDir = qtNewRot * vSightDir;
	D3DXVec3Normalize( &vDir, &vDir );

	return vDir;
}

void ioChargeComboJump2::TitanExtendMoveFail( ioBaseChar *pOwner, bool bSend )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	m_ChargeComboState = CCS_TITAN_MOVE_FAIL;

	int iAniID = pOwner->GetJumppingAnimationIdx();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );

	if( bSend && pOwner->IsNeedSendNetwork() )
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

void ioChargeComboJump2::JeonWooChiExtendChargeFail( ioBaseChar *pOwner, bool bSend )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	m_ChargeComboState = CCS_JEONWOOCHI_CHARGE_FAIL;

	int iAniID = pOwner->GetJumppingAnimationIdx();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );

	if( bSend && pOwner->IsNeedSendNetwork() )
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

void ioChargeComboJump2::SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime )
{
	if( !pOwner ) return;

	m_bUseExtendJump = false;
	m_ChargeComboState = CCS_NEW_JUMP;
	m_NewJumpState = NJS_DASH;

	m_dwNewJumpEndTime = dwEndTime;
	m_dwNewJumpAddAttackReserveTime = dwReserveTime;
	m_nNewJumpAttackIndex = 0;
	if( !m_JumpDashLandAni.IsEmpty() )
		dwEndTime += m_dwJumpDashLandDelayTime;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, m_fJumpDashGravityAmt );
}

//썬더버드
bool ioChargeComboJump2::CheckIsEnableThunderBirdExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( pOwner->IsEnableThunderBirdObjectSpecialState() )
		return true;

	return false;
}

void ioChargeComboJump2::SetThunderBirdExtendAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( m_ExtendAttackList.size() == 0 )
		return;
	
	ioThunderBirdObjectItem *pThunderBirdObjectItem = ToThunderBirdObjectItem( pOwner->GetObject() );
	if( !pThunderBirdObjectItem )
		return;
		
	const AttackAttribute& pAttr = m_ExtendAttackList[0];

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = pOwner->GetWeaponIndexBase();

	int iAniID = pGrp->GetAnimationIdx( pAttr.m_AttackAnimation );
	float fTimeRate = pAttr.m_fAttackAniRate;
	DWORD dwPreDelay = pAttr.m_dwPreDelay;

	if( pOwner->IsNeedProcess() )
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

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeListWithBase( m_dwCreateWeaponIndex + 1, iAniID, pAttr.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay, true );	
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeComboState = CCS_THUNDER_BIRD;

	pThunderBirdObjectItem->DecreaseGauge( pOwner, m_fThunderBirdChargeUseGauge );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwThunderBirdMotionEnd = dwCurTime;
	m_dwThunderBirdMotionEnd += dwPreDelay;
	m_dwThunderBirdMotionEnd += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_dwCreateWeaponIndex;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::ProcessThunderBirdExtendAttack(  ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	CheckAirJump( pOwner );
}

bool ioChargeComboJump2::IsCanJumpAttack( ioBaseChar *pOwner )
{
	if( m_JumpExtendType == JET_THUNDER_BIRD )
	{
		ioThunderBirdObjectItem *pThunderBirdObjectItem = ToThunderBirdObjectItem( pOwner->GetObject() );
		if ( pThunderBirdObjectItem )
		{
			if ( pThunderBirdObjectItem->CheckThunderBirdGauge( pOwner ) )
				return false;
		}
	}

	return true;
}

// 박일표
void ioChargeComboJump2::SetParkilpyoExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( m_ExtendAttackList.size() == 0 )
		return;
	
	ioParkilpyoObjectItem *pParkilpyoObjectItem = ToParkilpyoObjectItem( pOwner->GetObject() );
	if( !pParkilpyoObjectItem )
		return;
		
	const AttackAttribute& pAttr = m_ExtendAttackList[0];

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = pOwner->GetWeaponIndexBase();

	int iAniID = pGrp->GetAnimationIdx( pAttr.m_AttackAnimation );
	float fTimeRate = pAttr.m_fAttackAniRate;
	DWORD dwPreDelay = pAttr.m_dwPreDelay;

	if( pOwner->IsNeedProcess() )
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

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeListWithBase( m_dwCreateWeaponIndex + 1, iAniID, pAttr.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay, true );	
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeComboState = CCS_PARKILPYO;

	pParkilpyoObjectItem->DecreaseGauge( pOwner, m_fParkilpyoChargeUseGauge );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_dwCreateWeaponIndex;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::ProcessParkilpyoExtendAttack(  ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	CheckAirJump( pOwner );
}

bool ioChargeComboJump2::IsChargeAttacked()
{
	return m_bChargeAttacked;
}

bool ioChargeComboJump2::IsChargeEnd()
{
	if ( m_bSetChargeAttackEndWait )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if ( m_dwChargeAttackEndTime < dwCurTime )
			return true;

		m_fCurCharMoveSpeed = 0.0f;
		return false;
	}
	return true;
}

// 잭
bool ioChargeComboJump2::CheckEnableJackExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioJackItem *pJackItem = ToJackItem( pItem );
	if( pJackItem && pJackItem->GetCurBullet() >= m_fJackChargeUseGauge )
		return true;

	return false;
}

void ioChargeComboJump2::SetJackExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( m_ExtendAttackList.size() == 0 )
		return;

	ioJackItem *pJackItem = ToJackItem( pOwner->GetWeapon() );
	if( !pJackItem )
		return;

	const AttackAttribute& pAttr = m_ExtendAttackList[0];

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = pOwner->GetWeaponIndexBase();

	int iAniID = pGrp->GetAnimationIdx( pAttr.m_AttackAnimation );
	float fTimeRate = pAttr.m_fAttackAniRate;
	DWORD dwPreDelay = pAttr.m_dwPreDelay;

	if( pOwner->IsNeedProcess() )
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

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeListWithBase( m_dwCreateWeaponIndex + 1, iAniID, pAttr.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay, true );	
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( pAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeComboState = CCS_JACK_CHARGE;

	pJackItem->SetUseExtraGauge( m_fJackChargeUseGauge );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_dwCreateWeaponIndex;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::ProcessJackExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	CheckAirJump( pOwner );
}

void ioChargeComboJump2::SetJackExtraJumpAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;
	
	ioJackItem *pJackItem = ToJackItem( pOwner->GetWeapon() );
	if( !pJackItem )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = pOwner->GetWeaponIndexBase();

	int iAniID = pGrp->GetAnimationIdx( m_JackExtraJumpAttack.m_AttackAnimation );
	float fTimeRate = m_JackExtraJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_JackExtraJumpAttack.m_dwPreDelay;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			pOwner->SetTargetRotToDirKey( eDir );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeListWithBase( m_dwCreateWeaponIndex + 1, iAniID, m_JackExtraJumpAttack.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay, true );	
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetReservedSliding( m_JackExtraJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	
	pJackItem->SetUseExtraGauge( m_fJackExtraJumpAttackUseGauge );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	
	DWORD dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	DWORD dwExtrajumpEnableKeyTime = dwCurTime += pGrp->GetAnimationEventTime( iAniID, "key_reserve" ) * fTimeRate;

	pOwner->SetNewJumpAttackStateInfo( dwActionEndTime, dwExtrajumpEnableKeyTime, m_fJackExtraJumpAttackGravityAmt );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_JACK_EXTRA_JUMP_ATTACK;
		kPacket << m_dwCreateWeaponIndex;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::CheckExtraJumpReserveKeyJack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioJackItem *pJackItem = ToJackItem( pOwner->GetWeapon() );
	if ( !pJackItem )
		return;

	int nGauge = pJackItem->GetCurBullet();
	if ( nGauge >= m_fJackExtraJumpAttackUseGauge && pOwner->IsAttackKey() )
		SetJackExtraJumpAttack( pOwner );
	else if ( nGauge >= m_fJackExtraJumpUseGauge && pOwner->IsJumpKey() )
		m_bJackExtraJumpReserve = true;
}

bool ioChargeComboJump2::IsCanExtraJump( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;
	
	if( !m_bEnableExtraJump ) 
		return false;

	if ( m_JumpExtendType == JET_JACK )
	{
		ioJackItem *pJackItem = ToJackItem( pOwner->GetWeapon() );
		if( !pJackItem )
			return false;

		if ( pJackItem->GetCurBullet() < m_fJackExtraJumpUseGauge )
			return false;
	}

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
	case CCS_DASH:
		break;
	default:
		return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) 
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwExtraJumpEnableTime )
		return false;
	
	return true;
}

void ioChargeComboJump2::SetExtraJump( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if ( m_JumpExtendType == JET_JACK )
	{
		ioJackItem *pJackItem = ToJackItem( pOwner->GetWeapon() );
		if( !pJackItem )
			return;

		pJackItem->SetUseExtraGauge( m_fJackExtraJumpUseGauge );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = pOwner->GetWeaponIndexBase();
	m_bJackExtraJumpReserve = false;

	int iAniID = pGrp->GetAnimationIdx( m_ExtraJump.m_AttackAnimation );
	float fTimeRate = m_ExtraJump.m_fAttackAniRate;
	DWORD dwPreDelay = m_ExtraJump.m_dwPreDelay;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			pOwner->SetTargetRotToDirKey( eDir );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeListWithBase( m_dwCreateWeaponIndex + 1, iAniID, m_ExtraJump.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay, true );	
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	
	D3DXVECTOR3 vJumpDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if ( !m_bSetExtraJumpForcePower )
		pOwner->SetReservedSliding( m_ExtraJump.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	else
	{
		pOwner->SetJumpPower( m_fExtraJumpPower );

		if( m_fExtraJumpForce > 0.0f )
			pOwner->SetForcePower( vJumpDir, fabs( m_fExtraJumpForce ), FLOAT1, true );
		else
			pOwner->SetForcePower( -vJumpDir, fabs( m_fExtraJumpForce ), FLOAT1, true );
	}

	m_ChargeComboState = CCS_EXTRA_JUMP;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );
		
	m_dwActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwExtrajumpEnableKeyTime = dwCurTime;
	m_dwExtrajumpEnableKeyTime += pGrp->GetAnimationEventTime( iAniID, "key_reserve" ) * fTimeRate;
	
	pOwner->SetExtraJumpState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_dwCreateWeaponIndex;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::ProcessExtraJump( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	CheckAirJump( pOwner );

	if ( CheckExtraJumpReserveSkill( pOwner ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwActionEndTime > 0 && m_dwActionEndTime < dwCurTime )
	{
		if ( m_bJackExtraJumpReserve )
		{
			SetExtraJump( pOwner );
			return;
		}
		else
		{
			pOwner->ClearReservedSliding();
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
			pOwner->SetCurMoveSpeed( 0.0f );
			pOwner->SetSKillEndJumpState( m_fExtrajumpEndPower, false, false, true, true );
		}
	}
	else
	{
		if ( m_dwExtrajumpEnableKeyTime > 0 && m_dwExtrajumpEnableKeyTime < dwCurTime )
			CheckExtraJumpReserveKey( pOwner );
		
		CheckDropZoneDecreaseGauge( pOwner );
	}
}

void ioChargeComboJump2::CheckExtraJumpReserveKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if ( m_JumpExtendType == JET_JACK )
		CheckExtraJumpReserveKeyJack( pOwner );
}

bool ioChargeComboJump2::CheckExtraJumpReserveSkill( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( m_ChargeComboState == CCS_EXTRA_JUMP )
		return false;

	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 && !pOwner->IsBuffLimitSkill() )
	{
		m_bExtraJumpReserveSkill = true;		
		pOwner->CheckJumpingSkill( iSkillNum );
		return true;
	}

	if ( pOwner->IsJumpKeyRelease() && !m_bExtraJumpReserveSkill && m_dwExtrajumpEnableKeyTime == 0 )
		SetExtraJump( pOwner );

	return false;
}

void ioChargeComboJump2::CheckDropZoneDecreaseGauge( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pPlayStage = pOwner->GetCreator();
	if ( !pPlayStage )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pPlayStage->GetMapHeightOnlyTerrain( vCurPos.x, vCurPos.z );
	if ( fMapHeight > 0.0f )
		return;

	if ( m_JumpExtendType == JET_JACK )
	{
		ioJackItem *pJackItem = ToJackItem( pOwner->GetWeapon() );
		if( !pJackItem )
			return;

		float fTimePerSec = g_FrameTimer.GetSecPerFrame();		
		float fGauge = m_fJackExtraJumpUseGaugeDropZone * fTimePerSec;

		pJackItem->SetUseExtraGauge( fGauge );
	}
}

bool ioChargeComboJump2::IsExtraJumpState()
{
	if( m_ChargeComboState == CCS_EXTRA_JUMP )
	{
		if( m_dwActionEndTime > 0 && m_dwActionEndTime >= FRAMEGETTIME() )
			return true;
	}

	return false;
}

void ioChargeComboJump2::CheckKeyInputLastAttackState( ioBaseChar * pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;
	dwLooseTime = pOwner->GetActionStopTotalDelay();
	if ( COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
	{
		bool bAttackKey,bDefenseKey,bJumpKey;
		bAttackKey = bDefenseKey = bJumpKey = false;
		if( pOwner->IsAttackKey() )
		{
			bAttackKey = true;
			SetKeyInputLastAttackState( pOwner, bAttackKey, bDefenseKey, bJumpKey );
		}
		else if( pOwner->IsDefenseKey() )
		{
			bDefenseKey = true;
			SetKeyInputLastAttackState( pOwner, bAttackKey, bDefenseKey, bJumpKey );
		}
		else if( pOwner->IsJumpKey() )
		{
			bJumpKey = true;
			SetKeyInputLastAttackState( pOwner, bAttackKey, bDefenseKey, bJumpKey );
		}
	}
}

void ioChargeComboJump2::SetKeyInputLastAttackState( ioBaseChar * pOwner, bool bAttackKey, bool bDefenseKey, bool bJumpKey )
{
	m_dwEnableNinjaDashTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_KEY_INPUT_LAST_ATTACK;

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

	if( pOwner && !pOwner->IsActionStop() || m_bSetForceActionStopInit )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	AttackAttribute Attr;
	Attr.Init();
	if ( bAttackKey )
		Attr = m_AttackKeyAttribute;
	else if ( bDefenseKey )
		Attr = m_DefenseKeyAttribute;
	else if ( bJumpKey )
		Attr = m_JumpKeyAttribute;

	int iAniID = pGrp->GetAnimationIdx( Attr.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Animation is Not Exist", __FUNCTION__, Attr.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = Attr.m_fAttackAniRate;
	DWORD dwPreDelay = Attr.m_dwPreDelay;
	DWORD dwEndDelay = Attr.m_dwEndDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwEndDelay;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, Attr.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

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
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << bAttackKey;
		kPacket << bDefenseKey;
		kPacket << bJumpKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pOwner->SetJumpAttackState();
	m_bUseExtraSpeedRate = false;
}

void ioChargeComboJump2::ProcessKeyInputLastAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	CheckAirJump( pOwner );
}

bool ioChargeComboJump2::CheckEnableMagicSwordExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_JumpExtendType != JET_MAGIC_SWORD )
		return false;

	ioMagicSwordItem *pMagicSwordItem = ToMagicSwordItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pMagicSwordItem )
		return false;

	if ( !pMagicSwordItem->IsEnableMagicSwordGauge() )
		return false;

	return true;
}

void ioChargeComboJump2::SetMagicSwordExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioMagicSwordItem *pMagicSwordItem = ToMagicSwordItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pMagicSwordItem )
		return;

	m_dwEnableNinjaDashTime = 0;

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

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() || m_bSetForceActionStopInit )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( m_MagicSwordExtendAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Animation is Not Exist", __FUNCTION__, m_MagicSwordExtendAttackAttribute.m_AttackAnimation.c_str() );
		return;
	}

	m_ChargeComboState = CCS_MAGIC_SWORD_EXTEND_ATTACK;

	float fTimeRate = m_MagicSwordExtendAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_MagicSwordExtendAttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_MagicSwordExtendAttackAttribute.m_dwEndDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwEndDelay;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_MagicSwordExtendAttackAttribute.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

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
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( pOwner->IsNeedProcess() )
		pMagicSwordItem->DecreaseGauge();
}

void ioChargeComboJump2::ProcessMagicSwordExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	CheckAirJump( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
		return;

	bool bAttackKey,bDefenseKey,bJumpKey;
	bAttackKey = bDefenseKey = bJumpKey = false;
	if( pOwner->IsAttackKey() )
	{
		bAttackKey = true;
		SetMagicSwordExtendInputKeyAttack( pOwner, bAttackKey, bDefenseKey, bJumpKey );
		return;
	}
	else if( pOwner->IsDefenseKey() )
	{
		bDefenseKey = true;
		SetMagicSwordExtendInputKeyAttack( pOwner, bAttackKey, bDefenseKey, bJumpKey );
		return;
	}
	else if( pOwner->IsJumpKey() )
	{
		bJumpKey = true;
		SetMagicSwordExtendInputKeyAttack( pOwner, bAttackKey, bDefenseKey, bJumpKey );
		return;
	}
}

void ioChargeComboJump2::SetMagicSwordExtendInputKeyAttack( ioBaseChar * pOwner, bool bAttackKey, bool bDefenseKey, bool bJumpKey )
{
	if ( !pOwner )
		return;
	m_dwEnableNinjaDashTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_MAGIC_SWORD_EXTEND_INPUT_KEY_ATTACK;

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

	if( !pOwner->IsActionStop() || m_bSetForceActionStopInit )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	AttackAttribute Attr;
	Attr.Init();
	if ( bAttackKey )
		Attr = m_MagicSwordExtendAttackKeyAttribute;
	else if ( bDefenseKey )
		Attr = m_MagicSwordExtendDefenseKeyAttribute;
	else if ( bJumpKey )
		Attr = m_MagicSwordExtendJumpKeyAttribute;

	int iAniID = pGrp->GetAnimationIdx( Attr.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Animation is Not Exist", __FUNCTION__, Attr.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = Attr.m_fAttackAniRate;
	DWORD dwPreDelay = Attr.m_dwPreDelay;
	DWORD dwEndDelay = Attr.m_dwEndDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwEndDelay;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, Attr.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

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
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << bAttackKey;
		kPacket << bDefenseKey;
		kPacket << bJumpKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::ProcessMagicSwordExtendInputKeyAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	CheckAirJump( pOwner );
}

bool ioChargeComboJump2::CheckEnableResearcherRearFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_JumpExtendType != JET_RESEARCHER_REAR_FLY )
		return false;

	ioResearcher* pResearcher = ToResearcher( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pResearcher )
		return false;

	if( pResearcher->IsOverHeat() )
		return false;

	return true;
}

void ioChargeComboJump2::SetResearcherRearFlyState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_ChargeComboState = CCS_NONE;

	ioResearcher* pResearcher = ToResearcher( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pResearcher )
		pResearcher->ChangeToRearFlySpecialState( pOwner, ioResearcher::RS_JUMP );
}

//투우사
void ioChargeComboJump2::LoadBullFightInfo( ioINILoader &rkLoader )
{
	m_fBullFightChargeHeight = rkLoader.LoadFloat_e( "jump_bullfight_charge_enable_height", 0.0f );
	m_fBullFightUseGauge = rkLoader.LoadFloat_e( "jump_bullfight_use_gauge", 0.0f );
	LoadAttackAttribute_e( "jump_bullfight_charge", m_BullFightChargeAttack, rkLoader );
	LoadAttackAttribute_e( "jump_bullfight_charge_extend", m_BullFightChargeExtendAttack, rkLoader );
}

bool ioChargeComboJump2::CheckEnableBullFightChargeState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_JumpExtendType != JET_BULLFIGHT )
		return false;
	
	ioBullFightItem* pBullFightItem = ToBullFightItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pBullFightItem )
		return false;

	if( pBullFightItem->GetCurBullet() < m_fBullFightUseGauge )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( fabs( fMapHeight - vPos.y ) <= m_fBullFightChargeHeight )
		return false;

	return true;
}

void ioChargeComboJump2::SetBullFightChargeAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioBullFightItem* pBullFightItem = ToBullFightItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pBullFightItem )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;
		
	const AttackAttribute& pAttr = m_BullFightChargeAttack;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = pOwner->GetWeaponIndexBase();

	int iAniID = pGrp->GetAnimationIdx( pAttr.m_AttackAnimation );
	float fTimeRate = pAttr.m_fAttackAniRate;
	DWORD dwPreDelay = pAttr.m_dwPreDelay;

	if( pOwner->IsNeedProcess() )
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

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeListWithBase( m_dwCreateWeaponIndex + 1, iAniID, pAttr.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );

	m_ChargeComboState = CCS_BULLFIGHT_CHARGE;
	m_bUsedChargeAttack = true;
	pBullFightItem->SetUseExtraGauge( m_fBullFightUseGauge );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	m_dwReserveEnableTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_dwCreateWeaponIndex;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::ProcessBullFightChargeAttack(  ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if ( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
	{
		if ( m_dwReserveEnableTime < FRAMEGETTIME() )
		{
			SetBullFightChargeExtendAttack( pOwner );
			return;
		}
	}

	CheckAirJump( pOwner );
}

void ioChargeComboJump2::SetBullFightChargeExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;
	
	const AttackAttribute& pAttr = m_BullFightChargeExtendAttack;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = pOwner->GetWeaponIndexBase();

	int iAniID = pGrp->GetAnimationIdx( pAttr.m_AttackAnimation );
	float fTimeRate = pAttr.m_fAttackAniRate;
	DWORD dwPreDelay = pAttr.m_dwPreDelay;

	if( pOwner->IsNeedProcess() )
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

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeListWithBase( m_dwCreateWeaponIndex + 1, iAniID, pAttr.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );	
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( pAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );

	m_ChargeComboState = CCS_BULLFIGHT_CHARGE_EXTEND;

	m_dwChargeAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_dwCreateWeaponIndex;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::ProcessBullFightChargeExtendAttack(  ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	CheckAirJump( pOwner );
}

//Sawang
bool ioChargeComboJump2::CheckEnableSawangState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_JumpExtendType != JET_SAWANG_SLIDE )
		return false;

	ioSawangItem* pSawangItem = ToSawangItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( pSawangItem && pSawangItem->IsEnableGauge() )
		return true;

	return false;
}

void ioChargeComboJump2::SetSawangState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioSawangItem* pSawangItem = ToSawangItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pSawangItem )
		return;

	pSawangItem->SetSlideState( pOwner, ioSawangItem::SST_JUMP );
}

//체인메지션
bool ioChargeComboJump2::CheckIsEnableChainMagicianAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_JumpExtendType != JET_CHAIN_MAGICIAN )
		return false;

	ioChainMagicianItem *pChainMagicianItem = ToChainMagicianItem( pOwner->GetWeapon() );
	if( !pChainMagicianItem )
		return false;

	if( pChainMagicianItem->GetCurBullet() >= m_fChainMagicianChargeUseGauge )
		return true;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( fabs( fMapHeight - vPos.y ) <= m_fChainMagicianChargeHeight )
		return false;

	return false;
}

void ioChargeComboJump2::SetChainMagicianAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( m_ExtendAttackList.empty() )
		return;
	
	ioChainMagicianItem *pChainMagicianItem = ToChainMagicianItem( pOwner->GetWeapon() );
	if( !pChainMagicianItem )
		return;
		
	const AttackAttribute& pAttr = m_ExtendAttackList[0];

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = pOwner->GetWeaponIndexBase();

	int iAniID = pGrp->GetAnimationIdx( pAttr.m_AttackAnimation );
	float fTimeRate = pAttr.m_fAttackAniRate;
	DWORD dwPreDelay = pAttr.m_dwPreDelay;

	if( pOwner->IsNeedProcess() )
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

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeListWithBase( m_dwCreateWeaponIndex + 1, iAniID, pAttr.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );	
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, false );

	m_ChargeComboState = CCS_CHAIN_MAGICIAN_CHARGE;

	pChainMagicianItem->SetUseExtraGauge( m_fChainMagicianChargeUseGauge );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_dwCreateWeaponIndex;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::ProcessChainMagicianAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	CheckAirJump( pOwner );
}

void ioChargeComboJump2::CheckBlackSpearBuffCheckState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioBlackSpearItem* pBlackSpearItem = ToBlackSpearItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( pBlackSpearItem )
	{
		pBlackSpearItem->CheckBrandBuff( pOwner );
		return;
	}
}

void ioChargeComboJump2::SetBlackSpearJumpComboDashState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	m_ChargeComboState = CCS_BLACK_SPEAR_DASH;
	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
	float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;

	m_bUseExtendJump = true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pOwner->ChangeDirectionByInputDir( false );

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
	pOwner->SetComboDashState();

	m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_iCurJumpComboDashCount++;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_JUMP_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << vMoveDir;
		kPacket << (int)CCS_BLACK_SPEAR_DASH;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump2::GetCurGravityAmt( float& fCurGravity )
{
	if ( m_JumpExtendType == JET_BLACK_SPHEAR && m_ChargeComboState == CCS_BLACK_SPEAR_DASH )
	{
		fCurGravity = 0.0f;
		return;
	}
}

void ioChargeComboJump2::ProcessComboDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if ( m_JumpExtendType != JET_BLACK_SPHEAR ) return;
	if( m_bDashEnd ) return;
	if( m_dwComboDashEndTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwComboDashEndTime < dwCurTime )
	{
		m_dwComboDashEndTime = 0;
		pOwner->SetChangeJumppingState( m_fDashJumpAttackEndJumpPowerRate, false, 0 );
	}
	else
	{
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약채크
			CheckKeyReserve( pOwner );
		}
	}
}

bool ioChargeComboJump2::IsCanComboDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;
	if ( !m_bEnableJumpComboDash )
		return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->IsChatModeState( true ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if( !pOwner->IsCanJumpAttackEnableTime() )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	if ( m_iMaxJumpComboDashCount > 0 && m_iMaxJumpComboDashCount <= m_iCurJumpComboDashCount )
		return false;

	return true;
}

bool ioChargeComboJump2::CheckEnableSurferState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_JumpExtendType != JET_SURFER )
		return false;

	if( m_bUsedSurferCharge )
		return false;

	ioSurferItem* pSurferItem = ToSurferItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( !pSurferItem || !pSurferItem->IsEnableGauge() )
		return false;

	return true;
}

void ioChargeComboJump2::SetSurferState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioSurferItem* pSurferItem = ToSurferItem( pOwner->GetPriorityItem( SPT_JUMP ) );
	if( pSurferItem )
		pSurferItem->SetSurferState( pOwner );

	m_bUsedSurferCharge = true;
}

bool ioChargeComboJump2::IsComboDashState()
{
	switch( m_ChargeComboState )
	{
	case CCS_BLACK_SPEAR_DASH:
		return true;
	}

	return false;
}	

bool ioChargeComboJump2::CheckExpansionReaperJumpChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_JumpExtendType != JET_REAPER_EXPANSION )
		return false;

	if ( m_bUsingExpansionReaperJumpChargeAttack )
		return false;

	ioChargeContactWeaponItem *pChargeContactWeaponItem = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( pChargeContactWeaponItem && pChargeContactWeaponItem->CheckExpansionReaperAttack( pOwner, ioChargeContactWeaponItem::ERCAT_JUMP ) )
		return true;

	return false;
}

void ioChargeComboJump2::ChangeToExpansionReaperJumpChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return;

	ioChargeContactWeaponItem *pChargeContactWeaponItem = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pChargeContactWeaponItem )
		return;

	pChargeContactWeaponItem->ChangeToExpansionReaperAttack( pOwner, ioChargeContactWeaponItem::ERCAT_JUMP );
	m_bUsingExpansionReaperJumpChargeAttack = true;
}

bool ioChargeComboJump2::CheckReaperExpansionTeleport( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( m_JumpExtendType != JET_REAPER_EXPANSION )
		return false;

	ioChargeContactWeaponItem *pChargeContactWeaponItem = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pChargeContactWeaponItem )
		return false;

	if ( pChargeContactWeaponItem->GetCurBullet() < pChargeContactWeaponItem->GetNeedBullet() )
		return false;

	if ( m_dwReaperExpansionTeleportTicTime <= 0 )
		return false;

	if ( m_dwEnableReaperExpansionTeleportTime > FRAMEGETTIME() )
		return false;

	if ( !pOwner->IsDefenseKey() )
		return false;

	return true;
}

void ioChargeComboJump2::SetReaperExpansionTeleportState( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return;

	ioChargeContactWeaponItem *pChargeContactWeaponItem = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pChargeContactWeaponItem )
		return;

	m_dwEnableReaperExpansionTeleportTime = 0;

	pChargeContactWeaponItem->SetReaperExpansionTeleportState( pOwner );
}

bool ioChargeComboJump2::CheckExpansionLancerChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if ( !pOwner->IsNeedProcess() )
		return false;

	if( m_JumpExtendType != JET_EXPANSION_LANCER )
		return false;

	ioFlash_Item *pExpansionLancerItem = ToFlashItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pExpansionLancerItem )
		return false;
 
	if ( !pExpansionLancerItem->IsEnableGauge() )
		return false;

	return true;
}

void ioChargeComboJump2::ChangeToExpansionLancerChargeAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	if( m_JumpExtendType != JET_EXPANSION_LANCER )
		return;

	ioFlash_Item *pExpansionLancerItem = ToFlashItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pExpansionLancerItem )
		return;

	pExpansionLancerItem->SetFlashSpecialState( pOwner, true );
}

bool ioChargeComboJump2::CheckMagicEngineerChargeAttack( ioBaseChar *pOwner, bool bCheckProcess )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if ( !pOwner->IsNeedProcess() && bCheckProcess )
		return false;

	if( m_JumpExtendType != JET_MAGICENGINEER )
		return false;

	ioMagicEngineerItem	*pMagicEngineerItem = ToMagicEngineerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMagicEngineerItem && pMagicEngineerItem->GetDummyCharIndex() == -1 && !pMagicEngineerItem->IsJumpDummy() )
		return true; 

	return false;
}

void ioChargeComboJump2::ChangeToMagicEngineerChargeAttack( ioBaseChar *pOwner, bool bHalf )
{
	if ( !pOwner )
		return;
	if( m_JumpExtendType != JET_MAGICENGINEER )
		return;

	ioMagicEngineerItem	*pMagicEngineerItem = ToMagicEngineerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pMagicEngineerItem )
		return;

	pMagicEngineerItem->SetAttackState( pOwner, true, true, false, bHalf );
}

bool ioChargeComboJump2::CheckSantaChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return false;

	if( m_JumpExtendType != JET_SANTA )
		return false;

	ioSantaItem* pSantaItem = ToSantaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( pSantaItem && pSantaItem->IsEnableGauge() )
		return true;

	return false;
}

void ioChargeComboJump2::ChangeToSantaChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsHasCrown() )
		return;

	ioSantaItem* pSantaItem = ToSantaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( !pSantaItem )
		return;

	pSantaItem->SetFlyState( pOwner, true, true );
}

void ioChargeComboJump2::SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime )
{
	m_dwMESpecialMoveMacroTime_S = 0;
	m_dwMESpecialMoveMacroTime_E = 0;
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwSpecialMoveMacroTime_S = pGrp->GetAnimationEventTime_e( iAniID, "magic_eng_s" ) * fTimeRate;
	DWORD dwSpecialMoveMacroTime_E = pGrp->GetAnimationEventTime_e( iAniID, "magic_eng_e" ) * fTimeRate;
	if( dwSpecialMoveMacroTime_S )
	{
		m_dwMESpecialMoveMacroTime_S = dwCurTime + dwSpecialMoveMacroTime_S;
		m_dwMESpecialMoveMacroTime_E = dwCurTime + dwSpecialMoveMacroTime_E + dwEndTime;
	}
}