#include "StdAfx.h"

#include "ioSaberItem.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ioDrinkItem.h"
ioSaberItem::ioSaberItem(void)
{
	ClearData();

	m_fCurBullet = 0.0f;
}

ioSaberItem::ioSaberItem( const ioSaberItem &rhs ):
ioWeaponItem( rhs ),
m_SaberChargeAttack( rhs.m_SaberChargeAttack ),
m_JumpSaberChargeAttack( rhs.m_JumpSaberChargeAttack ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_fMaxBullet( rhs.m_fMaxBullet ),
m_fChargingAttackNeedBullet( rhs.m_fChargingAttackNeedBullet ),
m_fIncreseGaugeTic( rhs.m_fIncreseGaugeTic ),
m_fSaberFlyNeedBullet( rhs.m_fSaberFlyNeedBullet ),
m_fDecreaseSaberFlyTic( rhs.m_fDecreaseSaberFlyTic ),
m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
m_fHighDecreaseSaberFlyTic( rhs.m_fHighDecreaseSaberFlyTic ),
m_SaberFlyInfo( rhs.m_SaberFlyInfo ),
m_JumpSaberFlyInfo( rhs.m_JumpSaberFlyInfo ),
m_fEnableMoveMaxLength( rhs.m_fEnableMoveMaxLength ),
m_fTargetOffset( rhs.m_fTargetOffset ),

m_fSaberExtendDashNeedBullet( rhs.m_fSaberExtendDashNeedBullet ),
m_fDecreaseSaberExtendDashTic( rhs.m_fDecreaseSaberExtendDashTic ),
m_ExtendDashWaitAni( rhs.m_ExtendDashWaitAni ),
m_fExtendDashWaitAniRate( rhs.m_fExtendDashWaitAniRate ),
m_ExtendDashRunningAni( rhs.m_ExtendDashRunningAni ),
m_fExtendDashRunningAniRate( rhs.m_fExtendDashRunningAniRate ),
m_fExtendDashRunSpeed( rhs.m_fExtendDashRunSpeed ),
m_fExtendDashTargetRange( rhs.m_fExtendDashTargetRange ),
m_fExtendDashTargetAngle( rhs.m_fExtendDashTargetAngle ),
m_fExtendDashTargetUpHeight( rhs.m_fExtendDashTargetUpHeight ),
m_fExtendDashTargetDownHeight( rhs.m_fExtendDashTargetDownHeight ),
m_ExtendDashTargetWoundType( rhs.m_ExtendDashTargetWoundType ),
m_fExtendDashTargetMarkerRange( rhs.m_fExtendDashTargetMarkerRange ),
m_ExtendDashAttackAttribute( rhs.m_ExtendDashAttackAttribute ),
m_ExtendDashEndAni( rhs.m_ExtendDashEndAni ),
m_fExtendDashEndAniRate( rhs.m_fExtendDashEndAniRate ),
m_fSaberLandingRate( rhs.m_fSaberLandingRate ),
m_fEnableEndJumpHeight( rhs.m_fEnableEndJumpHeight )
{
	ClearData();

	m_fCurBullet = 0.0f;
}

ioSaberItem::~ioSaberItem(void)
{
	ClearData();
}

void ioSaberItem::ClearData()
{
	m_ChargeState = CS_NONE;
	
	m_iCurCombo = 0;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	ClearSaberChargeAttackData();
	ClearSaberFlyData();
	ClearSaberExtendDashData();
}

void ioSaberItem::ClearSaberChargeAttackData( ioBaseChar *pOwner /*= NULL */ )
{
	m_ChargeAttackState = CAS_NONE;
	m_CurSaberChargeAttack.Init();
	m_szTargetName.Clear();
	m_fCurRange = 0.0f;
	m_iCurChargeCombo = 0;
	m_vChargeMoveDir = ioMath::VEC3_ZERO;
	m_fCurMaxRange = 0.0f;
	m_fCurMoveSpeed = 0.0f;
	m_dwAniEndTime = 0;
	if ( pOwner )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
}

void ioSaberItem::ClearSaberFlyData( ioBaseChar *pOwner /*=NULL */ )
{
	m_SaberFlyState = SFS_NONE;
	m_CurSaberFlyInfo.Init();
	m_fSaberFlyCurRange = 0.0f;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;
	m_vFlyForceDir = ioMath::VEC3_ZERO;
	m_dwFlyForceTime = 0;
	m_bCharge = false;
	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;
	m_iCurFlyChargeCombo = 0;
	m_fCurTargetRange = 0.0f;
	m_fCurTargetAngle = 0.0f;
	m_fCurTargetUpHeight = 0.0f;
	m_fCurTargetDownHeight = 0.0f;
	m_CurTargetWoundType = TWT_DOWN_WOUND_EXCEPT;
	m_szTargetName.Clear();
	m_bInputJumpKey = false;
	if ( pOwner )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
}

void ioSaberItem::ClearSaberExtendDashData( ioBaseChar *pOwner /*=NULL */ )
{
	m_ExtendDashState = EDS_NONE;
	m_szTargetName.Clear();
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_fCurTargetRange = 0.0f;
	m_fCurTargetAngle = 0.0f;
	m_fCurTargetUpHeight = 0.0f;
	m_fCurTargetDownHeight = 0.0f;
	m_CurTargetWoundType = TWT_DOWN_WOUND_EXCEPT;
	if ( pOwner )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
}

ioItem* ioSaberItem::Clone()
{
	return new ioSaberItem( *this );
}

ioWeaponItem::WeaponSubType ioSaberItem::GetSubType() const
{
	return WST_SABER_ITEM;
}

void ioSaberItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	ClearData();
}

void ioSaberItem::LoadProperty( ioINILoader &rkLoader )
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

	//gauge
	m_fMaxBullet = rkLoader.LoadFloat_e( "max_gauge", 0 );
	m_fIncreseGaugeTic = rkLoader.LoadFloat_e( "increase_gauge_tic", 0.0f );
	m_fChargingAttackNeedBullet = rkLoader.LoadFloat_e( "need_charging_attack_gauge", 0.0f );
	m_fSaberFlyNeedBullet = rkLoader.LoadFloat_e( "need_saber_fly_gauge", 0.0f );
	m_fDecreaseSaberFlyTic = rkLoader.LoadFloat_e( "decrease_saber_fly_tic", 0.0f );
	m_fHighDecreaseSaberFlyTic = rkLoader.LoadFloat_e( "high_decrease_saber_fly_tic", 0.0f );
	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", 0.0f );
	m_fSaberExtendDashNeedBullet = rkLoader.LoadFloat_e( "need_dash_attack_gauge", 0.0f );
	m_fDecreaseSaberExtendDashTic = rkLoader.LoadFloat_e( "decrease_saber_dash_attack_tic", 0.0f );

	m_fSaberLandingRate = rkLoader.LoadFloat_e( "saber_landing_rate", FLOAT1 );
	m_fEnableEndJumpHeight = rkLoader.LoadFloat_e( "enable_end_jump_height", 0.0f );
	
	LoadSaberChargingAttackInfo( rkLoader );
	LoadJumpSaberChargingAttackInfo( rkLoader );
	
	LoadSaberFlyInfo( rkLoader );
	LoadJumpSaberFlyInfo( rkLoader );

	LoadSaberExtendDashInfo( rkLoader );
}

void ioSaberItem::LoadSaberChargingAttackInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_fEnableMoveMaxLength = rkLoader.LoadFloat_e( "charge_attack_move_enable_max_length", 0.0f );
	m_fTargetOffset = rkLoader.LoadFloat_e( "charge_attack_move_target_offset", 0.0f );

	m_SaberChargeAttack.Init();

	LoadAttackAttribute_e( "charge_attack", m_SaberChargeAttack.m_Attack, rkLoader );

	// 타겟 대상 검색용
	m_SaberChargeAttack.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "charge_target_wound_type", TWT_ALL );
	m_SaberChargeAttack.m_fTargetAngle = rkLoader.LoadFloat_e( "charge_target_angle", 0.0f );
	m_SaberChargeAttack.m_fTargetRange = rkLoader.LoadFloat_e( "charge_target_range", 0.0f );
	m_SaberChargeAttack.m_fTargetUpHeight = rkLoader.LoadFloat_e( "charge_target_up_height", 0.0f );
	m_SaberChargeAttack.m_fTargetDownHeight = rkLoader.LoadFloat_e( "charge_target_down_height", 0.0f );

	rkLoader.LoadString_e( "charge_attack_move_ani_fr", "", szBuf, MAX_PATH );
	m_SaberChargeAttack.m_MoveAniFr = szBuf;
	m_SaberChargeAttack.m_fMoveAniFrRate= rkLoader.LoadFloat_e( "charge_attack_move_ani_rate_fr", FLOAT1 );
	rkLoader.LoadString_e( "charge_attack_move_ani_bk", "", szBuf, MAX_PATH );
	m_SaberChargeAttack.m_MoveAniBk = szBuf;
	m_SaberChargeAttack.m_fMoveAniBkRate= rkLoader.LoadFloat_e( "charge_attack_move_ani_rate_bk", FLOAT1 );
	rkLoader.LoadString_e( "charge_attack_move_ani_rt", "", szBuf, MAX_PATH );
	m_SaberChargeAttack.m_MoveAniRt = szBuf;
	m_SaberChargeAttack.m_fMoveAniRtRate= rkLoader.LoadFloat_e( "charge_attack_move_ani_rate_rt", FLOAT1 );
	rkLoader.LoadString_e( "charge_attack_move_ani_lt", "", szBuf, MAX_PATH );
	m_SaberChargeAttack.m_MoveAniLt = szBuf;
	m_SaberChargeAttack.m_fMoveAniLtRate= rkLoader.LoadFloat_e( "charge_attack_move_ani_rate_lt", FLOAT1 );

	m_SaberChargeAttack.m_fMoveMaxRange = rkLoader.LoadFloat_e( "charge_attack_move_max_range", FLOAT1 );
	m_SaberChargeAttack.m_fMoveSpeed = rkLoader.LoadFloat_e( "charge_attack_move_speed", FLOAT1 );
	m_SaberChargeAttack.m_fMoveAngleGap  = rkLoader.LoadFloat_e( "charge_attack_move_angle_gap", 0.0f );
	
	m_SaberChargeAttack.m_AttackCombo.clear();
	int nMaxChargeComboCnt = rkLoader.LoadInt_e( "charge_attack_combo_cnt", 0 );
	for ( int i=0; i<nMaxChargeComboCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "charge_attack_combo%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_SaberChargeAttack.m_AttackCombo.push_back( sAttackAttribute );

	}
	// 점프 전환용
	m_SaberChargeAttack.m_fEndJumpAmt = rkLoader.LoadFloat_e( "charge_attack_end_jump_amt", 0.0f );
}

void ioSaberItem::LoadJumpSaberChargingAttackInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_JumpSaberChargeAttack.Init();

	LoadAttackAttribute_e( "jump_charge_attack", m_JumpSaberChargeAttack.m_Attack, rkLoader );

	// 타겟 대상 검색용
	m_JumpSaberChargeAttack.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_charge_target_wound_type", TWT_ALL );
	m_JumpSaberChargeAttack.m_fTargetAngle = rkLoader.LoadFloat_e( "jump_charge_target_angle", 0.0f );
	m_JumpSaberChargeAttack.m_fTargetRange = rkLoader.LoadFloat_e( "jump_charge_target_range", 0.0f );
	m_JumpSaberChargeAttack.m_fTargetUpHeight = rkLoader.LoadFloat_e( "jump_charge_target_up_height", 0.0f );
	m_JumpSaberChargeAttack.m_fTargetDownHeight = rkLoader.LoadFloat_e( "jump_charge_target_down_height", 0.0f );

	rkLoader.LoadString_e( "jump_charge_attack_move_ani_fr", "", szBuf, MAX_PATH );
	m_JumpSaberChargeAttack.m_MoveAniFr = szBuf;
	m_JumpSaberChargeAttack.m_fMoveAniFrRate= rkLoader.LoadFloat_e( "jump_charge_attack_move_ani_rate_fr", FLOAT1 );
	rkLoader.LoadString_e( "jump_charge_attack_move_ani_bk", "", szBuf, MAX_PATH );
	m_JumpSaberChargeAttack.m_MoveAniBk = szBuf;
	m_JumpSaberChargeAttack.m_fMoveAniBkRate= rkLoader.LoadFloat_e( "jump_charge_attack_move_ani_rate_bk", FLOAT1 );
	rkLoader.LoadString_e( "jump_charge_attack_move_ani_rt", "", szBuf, MAX_PATH );
	m_JumpSaberChargeAttack.m_MoveAniRt = szBuf;
	m_JumpSaberChargeAttack.m_fMoveAniRtRate= rkLoader.LoadFloat_e( "jump_charge_attack_move_ani_rate_rt", FLOAT1 );
	rkLoader.LoadString_e( "jump_charge_attack_move_ani_lt", "", szBuf, MAX_PATH );
	m_JumpSaberChargeAttack.m_MoveAniLt = szBuf;
	m_JumpSaberChargeAttack.m_fMoveAniLtRate= rkLoader.LoadFloat_e( "jump_charge_attack_move_ani_rate_lt", FLOAT1 );

	m_JumpSaberChargeAttack.m_fMoveMaxRange = rkLoader.LoadFloat_e( "jump_charge_attack_move_max_range", FLOAT1 );
	m_JumpSaberChargeAttack.m_fMoveSpeed = rkLoader.LoadFloat_e( "jump_charge_attack_move_speed", FLOAT1 );
	m_JumpSaberChargeAttack.m_fMoveAngleGap  = rkLoader.LoadFloat_e( "jump_charge_attack_move_angle_gap", 0.0f );

	m_JumpSaberChargeAttack.m_AttackCombo.clear();
	int nMaxChargeComboCnt = rkLoader.LoadInt_e( "jump_charge_attack_combo_cnt", 0 );
	for ( int i=0; i<nMaxChargeComboCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "jump_charge_attack_combo%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_JumpSaberChargeAttack.m_AttackCombo.push_back( sAttackAttribute );

	}
	// 점프 전환용
	m_JumpSaberChargeAttack.m_fEndJumpAmt = rkLoader.LoadFloat_e( "jump_charge_attack_end_jump_amt", 0.0f );
}

void ioSaberItem::LoadSaberFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	m_SaberFlyInfo.Init();

	//Fly Start
	rkLoader.LoadString_e( "saber_fly_start_ani", "", szBuf, MAX_PATH );
	m_SaberFlyInfo.m_StartAni = szBuf;
	m_SaberFlyInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "saber_fly_start_ani_rate", FLOAT1 );

	m_SaberFlyInfo.m_fStartAngle = rkLoader.LoadFloat_e( "saber_fly_start_angle", 0.0f );
	m_SaberFlyInfo.m_fStartMaxRange = rkLoader.LoadFloat_e( "saber_fly_start_max_range", 0.0f );
	m_SaberFlyInfo.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "saber_fly_start_duration", 0 );

	//Delay
	rkLoader.LoadString_e( "saber_fly_delay_ani", "", szBuf, MAX_PATH );
	m_SaberFlyInfo.m_DelayAni = szBuf;
	m_SaberFlyInfo.m_fDelayAniRate = rkLoader.LoadFloat_e( "saber_fly_delay_ani_rate", FLOAT1 );

	//Move
	rkLoader.LoadString_e( "saber_fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_SaberFlyInfo.m_MoveAniFr = szBuf;
	m_SaberFlyInfo.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "saber_fly_move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "saber_fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_SaberFlyInfo.m_MoveAniBk = szBuf;
	m_SaberFlyInfo.m_fMoveAniBkRate = rkLoader.LoadFloat_e( "saber_fly_move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "saber_fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_SaberFlyInfo.m_MoveAniRt = szBuf;
	m_SaberFlyInfo.m_fMoveAniRtRate = rkLoader.LoadFloat_e( "saber_fly_move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "saber_fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_SaberFlyInfo.m_MoveAniLt = szBuf;
	m_SaberFlyInfo.m_fMoveAniLtRate = rkLoader.LoadFloat_e( "saber_fly_move_ani_rate_lt", FLOAT1 );

	rkLoader.LoadString_e( "saber_fly_move_ani_up", "", szBuf, MAX_PATH );
	m_SaberFlyInfo.m_MoveAniUp = szBuf;
	m_SaberFlyInfo.m_fMoveAniUpRate = rkLoader.LoadFloat_e( "saber_fly_move_ani_rate_up", FLOAT1 );

	rkLoader.LoadString_e( "saber_fly_move_ani_down", "", szBuf, MAX_PATH );
	m_SaberFlyInfo.m_MoveAniDown = szBuf;
	m_SaberFlyInfo.m_fMoveAniDownRate = rkLoader.LoadFloat_e( "saber_fly_move_ani_rate_down", FLOAT1 );

	m_SaberFlyInfo.m_fMoveForceAmt = rkLoader.LoadFloat_e( "fly_force_move", 0.0f );
	m_SaberFlyInfo.m_fMoveForceFric = rkLoader.LoadFloat_e( "fly_force_friction", FLOAT1 );
	m_SaberFlyInfo.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	//Target
	m_SaberFlyInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_SaberFlyInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_SaberFlyInfo.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_SaberFlyInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_SaberFlyInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );

	m_SaberFlyInfo.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );

	//Charge Ani
	rkLoader.LoadString_e( "saber_fly_charge_ani", "", szBuf, MAX_PATH );
	m_SaberFlyInfo.m_ChargeAni = szBuf;
	m_SaberFlyInfo.m_dwChargeTime = (DWORD)rkLoader.LoadInt( "saber_fly_charge_time", 0 );
	
	//Attack Move
	rkLoader.LoadString_e( "saber_fly_attack_move_ani", "", szBuf, MAX_PATH );
	m_SaberFlyInfo.m_AttackMoveAni = szBuf;
	m_SaberFlyInfo.m_fAttackMoveAniRate = rkLoader.LoadFloat_e( "saber_fly_attack_move_ani_rate", FLOAT1 );
	m_SaberFlyInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "saber_fly_attack_move_speed", 0.0f );
	m_SaberFlyInfo.m_fAttackMoveRange = rkLoader.LoadFloat_e( "saber_fly_attack_move_range", FLOAT100 );
	m_SaberFlyInfo.m_fAttackMoveRate = rkLoader.LoadFloat_e( "saber_fly_attack_move_rate", FLOAT1 );

	m_SaberFlyInfo.m_fSaberFlyJumpAmt = rkLoader.LoadFloat_e( "saber_fly_jump_amt", 0.0f );

	//Attack Combo
	m_SaberFlyInfo.m_bComboRotByDirKey = rkLoader.LoadBool_e( "saber_fly_combo_rot_by_dir_key", false );
	m_SaberFlyInfo.m_AttackCombo.clear();
	int nMaxChargeComboCnt = rkLoader.LoadInt_e( "saber_fly_attack_combo_cnt", 0 );
	for ( int i=0; i<nMaxChargeComboCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "saber_fly_attack_combo%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_SaberFlyInfo.m_AttackCombo.push_back( sAttackAttribute );
	}
}

void ioSaberItem::LoadJumpSaberFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	m_JumpSaberFlyInfo.Init();

	//Fly Start
	rkLoader.LoadString_e( "jump_saber_fly_start_ani", "", szBuf, MAX_PATH );
	m_JumpSaberFlyInfo.m_StartAni = szBuf;
	m_JumpSaberFlyInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "jump_saber_fly_start_ani_rate", FLOAT1 );

	m_JumpSaberFlyInfo.m_fStartAngle = rkLoader.LoadFloat_e( "jump_saber_fly_start_angle", 0.0f );
	m_JumpSaberFlyInfo.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_saber_fly_start_max_range", 0.0f );
	m_JumpSaberFlyInfo.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "jump_saber_fly_start_duration", 0 );

	//Delay
	rkLoader.LoadString_e( "jump_saber_fly_delay_ani", "", szBuf, MAX_PATH );
	m_JumpSaberFlyInfo.m_DelayAni = szBuf;
	m_JumpSaberFlyInfo.m_fDelayAniRate = rkLoader.LoadFloat_e( "jump_saber_fly_delay_ani_rate", FLOAT1 );

	//Move
	rkLoader.LoadString_e( "jump_saber_fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_JumpSaberFlyInfo.m_MoveAniFr = szBuf;
	m_JumpSaberFlyInfo.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "jump_saber_fly_move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "jump_saber_fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_JumpSaberFlyInfo.m_MoveAniBk = szBuf;
	m_JumpSaberFlyInfo.m_fMoveAniBkRate = rkLoader.LoadFloat_e( "jump_saber_fly_move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "jump_saber_fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_JumpSaberFlyInfo.m_MoveAniRt = szBuf;
	m_JumpSaberFlyInfo.m_fMoveAniRtRate = rkLoader.LoadFloat_e( "jump_saber_fly_move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "jump_saber_fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_JumpSaberFlyInfo.m_MoveAniLt = szBuf;
	m_JumpSaberFlyInfo.m_fMoveAniLtRate = rkLoader.LoadFloat_e( "jump_saber_fly_move_ani_rate_lt", FLOAT1 );

	rkLoader.LoadString_e( "jump_saber_fly_move_ani_up", "", szBuf, MAX_PATH );
	m_JumpSaberFlyInfo.m_MoveAniUp = szBuf;
	m_JumpSaberFlyInfo.m_fMoveAniUpRate = rkLoader.LoadFloat_e( "jump_saber_fly_move_ani_rate_up", FLOAT1 );

	rkLoader.LoadString_e( "jump_saber_fly_move_ani_down", "", szBuf, MAX_PATH );
	m_JumpSaberFlyInfo.m_MoveAniDown = szBuf;
	m_JumpSaberFlyInfo.m_fMoveAniDownRate = rkLoader.LoadFloat_e( "jump_saber_fly_move_ani_rate_down", FLOAT1 );

	m_JumpSaberFlyInfo.m_fMoveForceAmt = rkLoader.LoadFloat_e( "jump_fly_force_move", 0.0f );
	m_JumpSaberFlyInfo.m_fMoveForceFric = rkLoader.LoadFloat_e( "jump_fly_force_friction", FLOAT1 );
	m_JumpSaberFlyInfo.m_fUpDownSpeed = rkLoader.LoadFloat_e( "jump_fly_updown_speed", 0.0f );

	//Target
	m_JumpSaberFlyInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_fly_target_wound_type", TWT_ALL );
	m_JumpSaberFlyInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "jump_fly_target_angle", 45.0f );
	m_JumpSaberFlyInfo.m_fTargetRange = rkLoader.LoadFloat_e( "jump_fly_target_range", 45.0f );
	m_JumpSaberFlyInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "jump_fly_target_up_height", 45.0f );
	m_JumpSaberFlyInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "jump_fly_target_down_height", 45.0f );

	m_JumpSaberFlyInfo.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "jump_fly_target_marker_range", 45.0f );

	//Charge Ani
	rkLoader.LoadString_e( "jump_saber_fly_charge_ani", "", szBuf, MAX_PATH );
	m_JumpSaberFlyInfo.m_ChargeAni = szBuf;
	m_JumpSaberFlyInfo.m_dwChargeTime = (DWORD)rkLoader.LoadInt( "jump_saber_fly_charge_time", 0 );

	//Attack Move
	rkLoader.LoadString_e( "jump_saber_fly_attack_move_ani", "", szBuf, MAX_PATH );
	m_JumpSaberFlyInfo.m_AttackMoveAni = szBuf;
	m_JumpSaberFlyInfo.m_fAttackMoveAniRate = rkLoader.LoadFloat_e( "jump_saber_fly_attack_move_ani_rate", FLOAT1 );
	m_JumpSaberFlyInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "jump_saber_fly_attack_move_speed", 0.0f );
	m_JumpSaberFlyInfo.m_fAttackMoveRange = rkLoader.LoadFloat_e( "jump_saber_fly_attack_move_range", FLOAT100 );
	m_JumpSaberFlyInfo.m_fAttackMoveRate = rkLoader.LoadFloat_e( "jump_saber_fly_attack_move_rate", FLOAT1 );

	m_JumpSaberFlyInfo.m_fSaberFlyJumpAmt = rkLoader.LoadFloat_e( "jump_saber_fly_jump_amt", 0.0f );

	//Attack Combo
	m_JumpSaberFlyInfo.m_bComboRotByDirKey = rkLoader.LoadBool_e( "jump_saber_fly_combo_rot_by_dir_key", false );
	m_JumpSaberFlyInfo.m_AttackCombo.clear();
	int nMaxChargeComboCnt = rkLoader.LoadInt_e( "jump_saber_fly_attack_combo_cnt", 0 );
	for ( int i=0; i<nMaxChargeComboCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "jump_saber_fly_attack_combo%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_JumpSaberFlyInfo.m_AttackCombo.push_back( sAttackAttribute );
	}
}

void ioSaberItem::LoadSaberExtendDashInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	//wait
	rkLoader.LoadString_e( "saber_extend_dash_wait_ani", "", szBuf, MAX_PATH );
	m_ExtendDashWaitAni = szBuf;
	m_fExtendDashWaitAniRate = rkLoader.LoadFloat_e( "saber_extend_dash_wait_ani_rate", FLOAT1 );

	//run
	rkLoader.LoadString_e( "saber_extend_dash_running_ani", "", szBuf, MAX_PATH );
	m_ExtendDashRunningAni = szBuf;
	m_fExtendDashRunningAniRate = rkLoader.LoadFloat_e( "saber_extend_dash_running_ani_rate", FLOAT1 );
	m_fExtendDashRunSpeed = rkLoader.LoadFloat_e( "saber_extend_dash_run_speed", FLOAT1 );

	//target
	m_fExtendDashTargetRange = rkLoader.LoadFloat_e( "saber_extend_dash_target_range", FLOAT1 );
	m_fExtendDashTargetAngle = rkLoader.LoadFloat_e( "saber_extend_dash_target_angle", FLOAT1 );
	m_fExtendDashTargetUpHeight = rkLoader.LoadFloat_e( "saber_extend_dash_target_up_height", FLOAT1 );
	m_fExtendDashTargetDownHeight = rkLoader.LoadFloat_e( "saber_extend_dash_target_down_height", FLOAT1 );
	m_ExtendDashTargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "saber_extend_dash_target_wound_type", TWT_ALL );
	m_fExtendDashTargetMarkerRange = rkLoader.LoadFloat_e( "saber_extend_dash_target_marker_range", FLOAT1 );

	//attack
	LoadAttackAttribute_e( "saber_extend_dash_attack", m_ExtendDashAttackAttribute, rkLoader );

	//end
	rkLoader.LoadString_e( "saber_extend_dash_end_ani", "", szBuf, MAX_PATH );
	m_ExtendDashEndAni = szBuf;
	m_fExtendDashEndAniRate = rkLoader.LoadFloat_e( "saber_extend_dash_end_ani_rate", FLOAT1 );
}

//Normal Attack
void ioSaberItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_szTargetName.Clear();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_ChargeAttackState = CAS_NONE;
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
		m_dwMotionStartTime = FRAMEGETTIME();
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

bool ioSaberItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioSaberItem::GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType )
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

void ioSaberItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
			{
				ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
				if( eNewDirKey != ioUserKeyInput::DKI_NONE )
				{
					pOwner->SetTargetRotToDirKey( eNewDirKey );
					if ( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << static_cast<byte>( ST_SET_DIR );
						kPacket << (int)eNewDirKey;
						kPacket << pOwner->GetWorldPosition();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
				return;
			}
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioSaberItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	if( IsEnableChargingAttackGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetSaberChargeAttackState( pOwner, false );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioSaberItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( m_ChargeState );
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//Set D~
void ioSaberItem::SetSaberChargeAttackState( ioBaseChar *pOwner, bool bJumpState )
{
	if( !pOwner )
		return;

	ClearSaberChargeAttackData();

	pOwner->SetState( CS_SABER_ATTACK );

	m_CurSaberChargeAttack.Init();
	if ( bJumpState )
		m_CurSaberChargeAttack = m_JumpSaberChargeAttack;
	else 
		m_CurSaberChargeAttack = m_SaberChargeAttack;

	ChangeSaberChargeAttackState( pOwner );

	if ( pOwner->IsNeedProcess() )
		pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_CHARGE_ATTACK );
		kPacket << static_cast<byte>( m_ChargeAttackState );		//CAS_ATTACK;
		kPacket << bJumpState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fChargingAttackNeedBullet );
}

void ioSaberItem::ChangeSaberChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurSaberChargeAttack.m_Attack.m_AttackAnimation.IsEmpty() )
		return;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	ClearKeyReserve();

	m_ChargeAttackState = CAS_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_CurSaberChargeAttack.m_Attack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_CurSaberChargeAttack.m_Attack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_CurSaberChargeAttack.m_Attack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CurSaberChargeAttack.m_Attack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurSaberChargeAttack.m_Attack.m_vWeaponInfoList,
		FTT_EXTEND_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_CurSaberChargeAttack.m_Attack.m_dwEndDelay;

	m_dwAniEndTime = dwStartTime;
	m_dwAniEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_CurSaberChargeAttack.m_Attack.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
}

//CS_SABER_ATTACK~ Process
void ioSaberItem::ProcessSaberChargeAttackState( ioBaseChar *pOwner )
{
	switch( m_ChargeAttackState )
	{
	case CAS_ATTACK:
		ProcessSaberChargeAttack( pOwner );
		break;
	case CAS_MOVE:
		ProcessSaberChargeMove( pOwner );
		break;
	case CAS_COMBO_ATTACK:
		ProcessSaberChargeCombo( pOwner );
		break;
	case CAS_END:
		break;
	}

	ProcessSaberChargeAttackStateMove( pOwner );
}

void ioSaberItem::ProcessSaberChargeAttackStateMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vMove( 0.0f, 0.0f, 0.0f );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurTimeGap = fTimePerSec * FLOAT1000;

	bool bMove = false;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fCurMoveSpeed > 0.0f && m_vChargeMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fCurMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vChargeMoveDir * fFinalMoveAmt;
				vMove += vMoveAmt;
				bMove = true;
			}
		}

		if( pOwner->ProcessForceMove( fTimePerSec, vMoveAmt ) )
		{
			vMove += vMoveAmt;
			bMove = true;
		}
	}


	if( bMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			if ( !bCol )
			{
				pOwner->Translate( vMove );

				if ( m_ChargeAttackState == CAS_MOVE )
				{
					D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;
					vDiff = vMove - vDiff;
					float fMoveDist = D3DXVec3Length( &vDiff );
					m_fCurRange += fMoveDist;
				}
			}
			

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

//D~
void ioSaberItem::ProcessSaberChargeAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
		if ( m_dwAniEndTime < dwCurTime )
		{
			if( pOwner->GetWorldPosition().y <= pOwner->GetBottomHeight() )
			{
				if( ChangeStateByReserveKeyInput( pOwner ) )
					return;
			}
		}
	}

	if ( m_dwMotionEndTime < dwCurTime )
	{
		ChangeSaberChargeAttackEnd( pOwner );
		return;
	}

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;

		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			//Dash가 우선순위
			CheckCancelReserveByConditionalSpecialDash( pOwner );
			if ( CheckEnableMoveState( pOwner ) )
			{
				ChangeSaberChargeMoveState( pOwner );
				return;
			}
			else
				CheckReserve( pOwner );
		}
	}
}

bool ioSaberItem::CheckEnableMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;
	if ( m_CurSaberChargeAttack.m_fMoveMaxRange <= 0.0f )
		return false;
	if ( m_CurSaberChargeAttack.m_fMoveSpeed <= 0.0f )
		return false;
	if ( !m_bEnableDashState )
		return false;

	return true;
}

//D~ -> Dash
void ioSaberItem::ChangeSaberChargeMoveState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_fCurMoveSpeed = m_CurSaberChargeAttack.m_fMoveSpeed;

	D3DXVECTOR3 vMoveDir = ioMath::VEC3_ZERO;
	
	ioBaseChar* pTarget = pStage->GetBaseChar( m_szTargetName );
	if ( pTarget )
	{
		D3DXVECTOR3 vTargetMoveDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fLength = D3DXVec3Length( &vTargetMoveDir );

		if ( fLength <= m_fEnableMoveMaxLength )
		{
			float fYawD = 180.0f + 45.0f * ( m_ConditionalSpecialDashKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
			D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vKeyMoveDir( vSightDir.x, 0.0f, vSightDir.z );
			D3DXVec3Normalize( &vKeyMoveDir, &vKeyMoveDir );

			D3DXQUATERNION qtDashKeyRot;
			ioMath::CalcDirectionQuaternion( qtDashKeyRot, -vKeyMoveDir );

			D3DXVECTOR3 vSightTargetMoveDir = vTargetMoveDir;
			vSightTargetMoveDir.y = 0;
			D3DXVec3Normalize( &vSightTargetMoveDir, &vSightTargetMoveDir );
			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vSightTargetMoveDir );


			float fDashKeyYaw, fTargetYaw;
			fDashKeyYaw = RADtoDEG( ioMath::QuaterToYaw( qtDashKeyRot ) );
			fDashKeyYaw = ioMath::ArrangeHead( fDashKeyYaw );
			fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
			fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
			
			float fAngleGap = fabs( ioMath::AngleGap( fDashKeyYaw, fTargetYaw ) );
			if ( fAngleGap < m_CurSaberChargeAttack.m_fMoveAngleGap )
			{
				if ( fLength > m_CurSaberChargeAttack.m_fMoveMaxRange )
					m_fCurMaxRange = m_CurSaberChargeAttack.m_fMoveMaxRange;
				else
				{
					if ( fLength < m_fTargetOffset )
						m_fCurMaxRange = fLength;
					else
					{
						float fOffset = fLength - m_fTargetOffset;
						m_fCurMaxRange = max( 0, fOffset );
					}
				}

				D3DXVec3Normalize( &vTargetMoveDir, &vTargetMoveDir );
				vMoveDir = vTargetMoveDir;
			}
		}
	}

	if ( vMoveDir == ioMath::VEC3_ZERO )
	{
		m_fCurMaxRange = m_CurSaberChargeAttack.m_fMoveMaxRange;
		CheckMoveAni( pOwner, m_ConditionalSpecialDashKey );
	}
	else
		SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniFr, m_CurSaberChargeAttack.m_fMoveAniFrRate, vMoveDir );
}

void ioSaberItem::CheckMoveAni( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDirKey )
{
	DWORD dwCurTime = FRAMEGETTIME();

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = eDirKey;
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	switch( iCnt )
	{
	case 0:
	case 7:
		if( !m_CurSaberChargeAttack.m_MoveAniFr.IsEmpty() )
			SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniFr, m_CurSaberChargeAttack.m_fMoveAniFrRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_CurSaberChargeAttack.m_MoveAniRt.IsEmpty() )
				SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniRt, m_CurSaberChargeAttack.m_fMoveAniRtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_CurSaberChargeAttack.m_MoveAniFr.IsEmpty() )
				SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniFr, m_CurSaberChargeAttack.m_fMoveAniFrRate, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_CurSaberChargeAttack.m_MoveAniRt.IsEmpty() )
				SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniRt, m_CurSaberChargeAttack.m_fMoveAniRtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_CurSaberChargeAttack.m_MoveAniBk.IsEmpty() )
				SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniBk, m_CurSaberChargeAttack.m_fMoveAniBkRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_CurSaberChargeAttack.m_MoveAniBk.IsEmpty() )
			SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniBk, m_CurSaberChargeAttack.m_fMoveAniBkRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_CurSaberChargeAttack.m_MoveAniLt.IsEmpty() )
				SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniLt, m_CurSaberChargeAttack.m_fMoveAniLtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_CurSaberChargeAttack.m_MoveAniBk.IsEmpty() )
				SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniBk, m_CurSaberChargeAttack.m_fMoveAniBkRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_CurSaberChargeAttack.m_MoveAniLt.IsEmpty() )
				SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniLt, m_CurSaberChargeAttack.m_fMoveAniLtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_CurSaberChargeAttack.m_MoveAniFr.IsEmpty() )
				SetMoveAni( pOwner, m_CurSaberChargeAttack.m_MoveAniFr, m_CurSaberChargeAttack.m_fMoveAniFrRate, vMoveDir );
		}
		break;
	}
}

void ioSaberItem::SetMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	ClearCancelInfo();

	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	if ( iAniID == - 1)
		return;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );


	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += FRAMEGETTIME();

	m_ChargeAttackState = CAS_MOVE;

	SetCurAttackAniForInputJump( szAni, fAniRate );

	m_vChargeMoveDir = vDir;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_CHARGE_ATTACK );
		kPacket << static_cast<byte>( m_ChargeAttackState );	//CAS_MOVE;
		kPacket << m_fCurMoveSpeed;
		kPacket << m_fCurMaxRange;
		kPacket << m_vChargeMoveDir;
		kPacket << szAni;
		kPacket << fTimeRate;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSaberItem::ProcessSaberChargeMove( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_fCurRange >= m_fCurMaxRange && m_fCurMoveSpeed > 0.0f )
		m_fCurMoveSpeed = 0.0f;

	if( m_dwMotionEndTime < dwCurTime )
	{
		ChangeSaberChargeAttackEnd( pOwner );
		return;
	}

	if ( pOwner->IsNeedProcess() )
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			if ( pOwner->IsAttackKey() )
			{
				ChangeSaberChargeComboAttackState( pOwner );
				return;
			}
		}
	}
}

//D~ -> Dash -> DDD( Comobo Attack )
void ioSaberItem::ChangeSaberChargeComboAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( !COMPARE( m_iCurChargeCombo, 0, (int)m_CurSaberChargeAttack.m_AttackCombo.size() ) )
		return;

	m_vChargeMoveDir = ioMath::VEC3_ZERO;
	m_fCurMoveSpeed = 0.0f;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	ClearKeyReserve();

	const AttackAttribute &sChargeCombo = m_CurSaberChargeAttack.m_AttackCombo[m_iCurChargeCombo];

	int iAniID = pGrp->GetAnimationIdx( sChargeCombo.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate  = sChargeCombo.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	DWORD dwPreDelay = sChargeCombo.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->SetReservedSliding( sChargeCombo.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeList( iAniID, sChargeCombo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += sChargeCombo.m_dwEndDelay;


	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	m_ChargeAttackState = CAS_COMBO_ATTACK;

	SetCurAttackAniForInputJump( sChargeCombo.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedProcess() )
	{
		//ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		//if( eNewDirKey != ioUserKeyInput::DKI_NONE )
//			pOwner->SetTargetRotToDirKey( eNewDirKey );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_SABER_CHARGE_ATTACK );
			kPacket << static_cast<byte>( m_ChargeAttackState );		//CAS_COMBO_ATTACK
			//kPacket << vMoveDir;
			//kPacket << (int)eNewDirKey;
			kPacket << m_iCurChargeCombo;
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_iCurChargeCombo++;
}

void ioSaberItem::ProcessSaberChargeCombo( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		if( pOwner->IsNeedProcess() )
		{
			float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
			if( m_fEnableEndJumpHeight < fHeightGap )
				ChangeSaberChargeAttackEnd( pOwner );
			else
			{
				if ( !ChangeStateByReserveKeyInput( pOwner ) )
				{
					ChangeSaberChargeAttackEnd( pOwner );
					return;
				}
			}
		}
	}

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;

		if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			if ( COMPARE( m_iCurChargeCombo, 0, (int)m_CurSaberChargeAttack.m_AttackCombo.size() ) )
			{
				if ( pOwner->IsAttackKey() )
				{
					ChangeSaberChargeComboAttackState( pOwner );
					return;
				}
			}
			// if Last Combo
			else if( m_iCurChargeCombo >= (int)m_CurSaberChargeAttack.m_AttackCombo.size() )
			{
				CheckReserveAttackKey( pOwner );
				return;
			}
		}
	}
}

//D~ End 
void ioSaberItem::ChangeSaberChargeAttackEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_ChargeAttackState = CAS_END;
	m_vChargeMoveDir = ioMath::VEC3_ZERO;
	m_fCurMoveSpeed = 0.0f;

	if ( pOwner->IsNeedProcess() )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( m_fEnableEndJumpHeight < fHeightGap )
			pOwner->SetExtendAttackEndJump( m_CurSaberChargeAttack.m_fEndJumpAmt, FLOAT1 );
		else
			pOwner->SetExtendAttackEndJump( 0.0f, m_fSaberLandingRate );
	}
}

/*
void ioSaberItem::SetChargeTarget( ioBaseChar *pTarget, ioBaseChar *pAttacker )
{
	if ( !m_szTargetName.IsEmpty() )
		return;

	m_szTargetName = pTarget->GetCharName();

	if( pTarget->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pAttacker, kPacket );
		kPacket << static_cast<byte>( ST_SET_TARGET );
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
*/

bool ioSaberItem::ChangeStateByReserveKeyInput( ioBaseChar *pOwner )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return true;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return true;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return true;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
			return true;
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
				return true;
			}
		}
	}

	return false;
}

bool ioSaberItem::IsEnableChargingAttackGauge()
{
	if( m_fCurBullet >= m_fChargingAttackNeedBullet )
		return true;

	return false;
}

void ioSaberItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurBullet -= fNeedGauge;
	m_fCurBullet = (float)max( 0, min( m_fCurBullet, m_fMaxBullet ) );
}

void ioSaberItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		{
			if( m_fCurBullet < m_fMaxBullet )
			{
				float fGauge = m_fIncreseGaugeTic * fTimePerSec;

				m_fCurBullet += fGauge;
				m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
			}
		}
		return;
	case CS_SABER_FLY:
		{
			float fGauge = m_fDecreaseSaberFlyTic * fTimePerSec;
			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreaseSaberFlyTic * fTimePerSec;
			}

			m_fCurBullet -= fGauge;
			m_fCurBullet = max( 0.0f, m_fCurBullet );
		}
		return;
	case CS_SABER_DASH_ATTACK:
		{
			if ( m_ExtendDashState == EDS_RUN )
			{
				float fGauge = m_fDecreaseSaberExtendDashTic * fTimePerSec;
				m_fCurBullet -= fGauge;
				m_fCurBullet = max( 0.0f, m_fCurBullet );
			}
		}
		return;
	}
}

void ioSaberItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if ( !bFullTime )
	{
		float fJumpPower = pOwner->GetJumpPower() * m_fJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		if( m_pExtendJump )
		{
			m_pExtendJump->SetJumpping( pOwner, bFullTime );
		}
	}
	else
	{
		SetSaberFlyState( pOwner, false );
	}
}

void ioSaberItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

bool ioSaberItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
		return false;

	fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

//Set CS_SABER_FLY(A~) State
bool ioSaberItem::IsEnableSaberFlyGauge()
{
	if( m_fCurBullet >= m_fSaberFlyNeedBullet )
		return true;

	return false;
}

void ioSaberItem::SetSaberFlyState( ioBaseChar *pOwner, bool bJumpState )
{
	if( !pOwner )
		return;

	ClearSaberFlyData();
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetState( CS_SABER_FLY );

	m_CurSaberFlyInfo.Init();
	if ( bJumpState )
		m_CurSaberFlyInfo = m_JumpSaberFlyInfo;
	else
		m_CurSaberFlyInfo = m_SaberFlyInfo;

	m_fCurTargetRange = m_CurSaberFlyInfo.m_fTargetRange;
	m_fCurTargetAngle = m_CurSaberFlyInfo.m_fTargetAngle;
	m_fCurTargetUpHeight = m_CurSaberFlyInfo.m_fTargetUpHeight;
	m_fCurTargetDownHeight = m_CurSaberFlyInfo.m_fTargetDownHeight;
	m_CurTargetWoundType = m_CurSaberFlyInfo.m_TargetWoundType;

	ChangeSaberFlyStartState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_FLY );
		kPacket << static_cast<byte>( m_SaberFlyState );		//SFS_START;
		kPacket << bJumpState;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fSaberFlyNeedBullet );
}

//CS_SABER_FLY(A~) State
void ioSaberItem::ChangeSaberFlyStartState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurSaberFlyInfo.m_StartAni );
	float fTimeRate  = m_CurSaberFlyInfo.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_SaberFlyState = SFS_START;

	m_dwMotionStartTime = FRAMEGETTIME(); 
	pOwner->SetTargetMarkerRange( m_CurSaberFlyInfo.m_fTargetMarkerRange );
}

//CS_SABER_FLY(A~) State
void ioSaberItem::ProcessSaberFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( m_SaberFlyState )
	{
	case SFS_START:
		ProcessSaberFlyStartState( pOwner );
		break;
	case SFS_DELAY:
		ProcessSaberFlyDelayState( pOwner );
		break;
	case SFS_MOVE:
		ProcessSaberFlyMoveState( pOwner );
		break;
	case SFS_CHARGING:
		ProcessSaberFlyCharging( pOwner );
		break;
	case SFS_ATTACK_MOVE:
		{
			DWORD dwCurTime = FRAMEGETTIME();
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_bCharge )
				{
					SetSaberChargeAttackState( pOwner, true );
					return;
				}
				else
				{
					ChangeSaberFlyComboAttackState( pOwner );
					return;
				}
			}
		}
		break;
	case SFS_COMBO_ATTACK:
		ProcessSaberFlyComboAttackState( pOwner );
		break;
	}
	ProcessSaberFlyMoving( pOwner );
}

//Fly Start State
void ioSaberItem::ProcessSaberFlyStartState( ioBaseChar *pOwner )
{
	if( m_dwMotionStartTime + m_CurSaberFlyInfo.m_dwStartDuration < FRAMEGETTIME() )
	{
		ChangeSaberFlyDelay( pOwner );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		return;
	}
	else if( !ProcessSaberFlyStartMoving( pOwner ) )
	{
		ChangeSaberFlyDelay( pOwner );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		return;
	}
}

bool ioSaberItem::ProcessSaberFlyStartMoving( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	float fJumpPoweru = pOwner->GetCurJumpPowerAmt();

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_CurSaberFlyInfo.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurSaberFlyInfo.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurSaberFlyInfo.m_fStartMaxRange / (float)m_CurSaberFlyInfo.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fSaberFlyCurRange+fDistance < m_CurSaberFlyInfo.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fSaberFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurSaberFlyInfo.m_fStartMaxRange - m_fSaberFlyCurRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	return bRangeLimit;
}

//Fly Delay State
void ioSaberItem::ChangeSaberFlyDelay( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurSaberFlyInfo.m_DelayAni );
	float fTimeRate  = m_CurSaberFlyInfo.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_SaberFlyState = SFS_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwMotionStartTime = 0;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_FLY );
		kPacket << static_cast<byte>( m_SaberFlyState );		//SFS_DELAY;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

void ioSaberItem::ProcessSaberFlyDelayState( ioBaseChar *pOwner )
{
	if( m_fCurBullet <= 0.0f )
	{
		ChangeSaberFlyEnd( pOwner );
		return;
	}

	CheckAttTarget( pOwner );

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsAttackKey() )
		{
			ChangeSaberFlyChargeState( pOwner );
			return;
		}
	}
	// CheckKeyInput 내부에서 상태가 바뀔수 있다
	CheckSaberFlyKeyInput( pOwner );
}

//Fly Move State
void ioSaberItem::CheckSaberFlyKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	if( m_SaberFlyState == SFS_DELAY )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			if( eNewDirKey != ioUserKeyInput::DKI_NONE )
				bChangeDir = true;

			m_CurDirKey = eNewDirKey;
		}
	}

	// Check UpDown
	if( m_bSetUpKey )
	{
		if( !pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = false;

			if( pOwner->IsDefenseKeyDown() )
				m_bSetDownKey = true;
		}
	}
	else if( m_bSetDownKey )
	{
		if( !pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = false;

			if( pOwner->IsJumpKeyDown() )
				m_bSetUpKey = true;
		}
	}
	else
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = true;
		}

		//최초에 점프키를 한번 더 눌러야지만 
		if ( !m_bInputJumpKey )
		{
			if( pOwner->IsJumpKey() )
				m_bInputJumpKey = true;
		}
		else
		{
			if( pOwner->IsJumpKeyDown() )
			{
				bChangeUpDown = true;
				m_bSetUpKey = true;
				m_bSetDownKey = false;
			}
		}
		
	}
	
	if( bChangeDir || bChangeUpDown )
	{
		if( bChangeDir )
			ChangeSaberFlyMove( pOwner );
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_SABER_FLY );
			kPacket << static_cast<byte>( SFS_MOVE );
			kPacket << bChangeDir;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			kPacket << bChangeUpDown;
			kPacket << m_bSetUpKey;
			kPacket << m_bSetDownKey;
			kPacket << pOwner->GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioSaberItem::ChangeSaberFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = m_CurDirKey;
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	switch( iCnt )
	{
	case 0:
	case 7:
		if( !m_CurSaberFlyInfo.m_MoveAniFr.IsEmpty() )
			CheckSaberFlyMoveAni( pOwner, m_CurSaberFlyInfo.m_MoveAniFr, m_CurSaberFlyInfo.m_fMoveAniFrRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_CurSaberFlyInfo.m_MoveAniRt.IsEmpty() )
				CheckSaberFlyMoveAni( pOwner, m_CurSaberFlyInfo.m_MoveAniRt, m_CurSaberFlyInfo.m_fMoveAniRtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_CurSaberFlyInfo.m_MoveAniFr.IsEmpty() )
				CheckSaberFlyMoveAni( pOwner, m_CurSaberFlyInfo.m_MoveAniFr, m_CurSaberFlyInfo.m_fMoveAniFrRate, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_CurSaberFlyInfo.m_MoveAniRt.IsEmpty() )
				CheckSaberFlyMoveAni( pOwner, m_CurSaberFlyInfo.m_MoveAniRt, m_CurSaberFlyInfo.m_fMoveAniRtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_CurSaberFlyInfo.m_MoveAniBk.IsEmpty() )
				CheckSaberFlyMoveAni( pOwner, m_CurSaberFlyInfo.m_MoveAniBk, m_CurSaberFlyInfo.m_fMoveAniBkRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_CurSaberFlyInfo.m_MoveAniBk.IsEmpty() )
			CheckSaberFlyMoveAni( pOwner, m_CurSaberFlyInfo.m_MoveAniBk, m_CurSaberFlyInfo.m_fMoveAniBkRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_CurSaberFlyInfo.m_MoveAniLt.IsEmpty() )
				CheckSaberFlyMoveAni( pOwner, m_CurSaberFlyInfo.m_MoveAniLt, m_CurSaberFlyInfo.m_fMoveAniLtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_CurSaberFlyInfo.m_MoveAniBk.IsEmpty() )
				CheckSaberFlyMoveAni( pOwner, m_CurSaberFlyInfo.m_MoveAniBk, m_CurSaberFlyInfo.m_fMoveAniBkRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_CurSaberFlyInfo.m_MoveAniLt.IsEmpty() )
				CheckSaberFlyMoveAni( pOwner, m_CurSaberFlyInfo.m_MoveAniLt, m_CurSaberFlyInfo.m_fMoveAniLtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_CurSaberFlyInfo.m_MoveAniFr.IsEmpty() )
				CheckSaberFlyMoveAni( pOwner, m_CurSaberFlyInfo.m_MoveAniFr, m_CurSaberFlyInfo.m_fMoveAniFrRate, vMoveDir );
		}
		break;
	}

	m_SaberFlyState = SFS_MOVE;

	m_dwMotionStartTime = 0;
}

void ioSaberItem::CheckSaberFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_dwKeyReserveTime = 0;
	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwFlyForceTime = dwCurTime + dwTime;

	m_vFlyForceDir = vDir;
}

void ioSaberItem::ProcessSaberFlyMoveState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	CheckAttTarget( pOwner );

	if( m_dwMotionEndTime < dwCurTime )
	{
		ChangeSaberFlyDelay( pOwner );
		return;
	}
	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsAttackKey() )
		{
			ChangeSaberFlyChargeState( pOwner );
			return;
		}
	}
	CheckSaberFlyKeyInput( pOwner );
}

//Charge State
void ioSaberItem::ChangeSaberFlyChargeState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_CurSaberFlyInfo.m_ChargeAni );
	if ( iAniID == -1 )
		return;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate = (float)m_CurSaberFlyInfo.m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = FRAMEGETTIME();

	m_SaberFlyState = SFS_CHARGING;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_FLY );
		kPacket << static_cast<byte>( m_SaberFlyState );		//SFS_CHARGING;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSaberItem::ProcessSaberFlyCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	if( IsEnableChargingAttackGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_CurSaberFlyInfo.m_dwChargeTime <= FRAMEGETTIME() )
		{
			m_bCharge = true;
			ChangeSaberFlyAttackMove( pOwner );
			return;
		}
	}
	else
	{
		m_bCharge = false;
		ChangeSaberFlyAttackMove( pOwner );
		return;
	}
}

// Attack Dash
void ioSaberItem::ChangeSaberFlyAttackMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurSaberFlyInfo.m_AttackMoveAni.IsEmpty() )
		return;

	if( m_CurSaberFlyInfo.m_fAttackMoveSpeed <= 0.0f )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_CurSaberFlyInfo.m_AttackMoveAni );
	float fTimeRate = m_CurSaberFlyInfo.m_fAttackMoveAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwMoveDuration = 0;
	if( pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vTargetPos;
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );

		bool bTargetOk = false;
		// Target 유효성 체크.
		if( pTarget && CheckTargetValidate(pOwner) )
		{
			bTargetOk = true;
		}

		m_dwMotionEndTime = dwCurTime;

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			vTargetPos += (m_CurSaberFlyInfo.m_fAttackMoveRange*m_CurSaberFlyInfo.m_fAttackMoveRate) * vDir;

			float fRange = m_CurSaberFlyInfo.m_fAttackMoveRange*m_CurSaberFlyInfo.m_fAttackMoveRate;

			dwMoveDuration = (DWORD)(fRange / m_CurSaberFlyInfo.m_fAttackMoveSpeed * FLOAT1000);
			m_dwMotionEndTime += dwMoveDuration;

			m_vFlyMoveDir = vDir;
			m_fFlyMoveSpeed = m_CurSaberFlyInfo.m_fAttackMoveSpeed;
		}
		else
		{
			D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();

			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			fRange *= m_CurSaberFlyInfo.m_fAttackMoveRate;
			dwMoveDuration = (DWORD)(fRange / m_CurSaberFlyInfo.m_fAttackMoveSpeed * FLOAT1000);
			m_dwMotionEndTime += dwMoveDuration;

			m_vFlyMoveDir = vDir;
			m_fFlyMoveSpeed = m_CurSaberFlyInfo.m_fAttackMoveSpeed;
		}
	}

	m_SaberFlyState = SFS_ATTACK_MOVE;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_FLY );
		kPacket << static_cast<byte>( m_SaberFlyState );		//SFS_ATTACK_MOVE;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vFlyMoveDir;
		kPacket << m_fFlyMoveSpeed;
		kPacket << dwMoveDuration;
		kPacket << m_bCharge;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//Fly Combo Attack State
void ioSaberItem::ChangeSaberFlyComboAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if ( !COMPARE( m_iCurFlyChargeCombo, 0, (int)m_CurSaberFlyInfo.m_AttackCombo.size() ) )
		return;

	m_fFlyMoveSpeed = 0.0f;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	ClearKeyReserve();

	const AttackAttribute &sChargeCombo = m_CurSaberFlyInfo.m_AttackCombo[m_iCurFlyChargeCombo];

	int iAniID = pGrp->GetAnimationIdx( sChargeCombo.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate  = sChargeCombo.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	DWORD dwPreDelay = sChargeCombo.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->SetReservedSliding( sChargeCombo.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeList( iAniID, sChargeCombo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += sChargeCombo.m_dwEndDelay;


	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	m_SaberFlyState = SFS_COMBO_ATTACK;

	SetCurAttackAniForInputJump( sChargeCombo.m_AttackAnimation, fTimeRate );

	ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
	if( m_CurSaberFlyInfo.m_bComboRotByDirKey )
	{
		if( pOwner->IsNeedProcess() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			if( eNewDirKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( eNewDirKey );
		}
	}
	
	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_FLY );
		kPacket << static_cast<byte>( m_SaberFlyState );		//SFS_COMBO_ATTACK;
		kPacket << (int)eNewDirKey;
		kPacket << m_iCurFlyChargeCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurFlyChargeCombo++;
}

void ioSaberItem::ProcessSaberFlyComboAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}

	if( m_dwMotionEndTime < dwCurTime )
	{
		ChangeSaberFlyEnd( pOwner );
		return;
	}

	if( pOwner->IsNeedProcess() )
	{
		if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			if ( pOwner->IsAttackKey() )
			{
				ChangeSaberFlyComboAttackState( pOwner );
				return;
			}
		}
	}
}

//Fly End State
void ioSaberItem::ChangeSaberFlyEnd( ioBaseChar *pOwner )
{
	m_SaberFlyState = SFS_END;

	if ( pOwner->IsNeedProcess() )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( m_fEnableEndJumpHeight < fHeightGap )
			pOwner->SetExtendAttackEndJump( m_CurSaberFlyInfo.m_fSaberFlyJumpAmt, FLOAT1 );
		else
			pOwner->SetExtendAttackEndJump( 0.0f, m_fSaberLandingRate );
	}
}

//Fly State Move
void ioSaberItem::ProcessSaberFlyMoving( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	// Dir Move
	if( m_dwFlyForceTime > 0 && m_dwFlyForceTime < dwCurTime )
	{
		m_dwFlyForceTime = 0;
		pOwner->SetForcePower( m_vFlyForceDir, m_CurSaberFlyInfo.m_fMoveForceAmt, m_CurSaberFlyInfo.m_fMoveForceFric );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );

	// UpDown Move
	if( m_bSetUpKey || m_bSetDownKey )
	{
		float fCurHeightAmt = 0.0f;
		float fCurUpDownSpeed = m_CurSaberFlyInfo.m_fUpDownSpeed * fTimePerSec;

		if( m_bSetUpKey )
			fCurHeightAmt += fCurUpDownSpeed;
		else if( m_bSetDownKey )
			fCurHeightAmt -= fCurUpDownSpeed;

		vCurPos.y += fCurHeightAmt;
	}

	vCurPos.y = max( vCurPos.y, fMapHeight );

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );


	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fFlyMoveSpeed > 0.0f && m_vFlyMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fFlyMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vFlyMoveDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
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
	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}

//Extend Dash
bool ioSaberItem::IsEnableSaberExtendDashAttackGauge()
{
	if( m_fCurBullet >= m_fSaberExtendDashNeedBullet )
		return true;

	return false;
}

void ioSaberItem::SetSaberExtendDashState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ClearSaberExtendDashData();
	pOwner->SetState( CS_SABER_DASH_ATTACK );
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_fCurTargetRange = m_fExtendDashTargetRange;
	m_fCurTargetAngle = m_fExtendDashTargetAngle;
	m_fCurTargetUpHeight = m_fExtendDashTargetUpHeight;
	m_fCurTargetDownHeight = m_fExtendDashTargetDownHeight;
	m_CurTargetWoundType = m_ExtendDashTargetWoundType;
	ChangeExtendDashWaitState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_EXTEND_DASH );
		kPacket << static_cast<byte>( m_ExtendDashState );		//EDS_WAIT;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSaberItem::ProcessSaberExtendDashState( ioBaseChar *pOwner )
{
	switch( m_ExtendDashState )
	{
	case EDS_WAIT:
		ProcessSaberExtendDashWaitState( pOwner );
		break;
	case EDS_RUN:
		ProcessSaberExtendDashRunState( pOwner );
		break;
	case EDS_ATTACK:
		ProcessSaberExtendDashAttackState( pOwner );
		break;
	case EDS_END:
		ProcessSaberExtendDashEndState( pOwner );
		break;
	}
}

//Extend Dash Wait
void ioSaberItem::ChangeExtendDashWaitState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashWaitAni );
	if ( iAniID == - 1)
		return;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni(  FLOAT100 );

	float fCurRate = FLOAT1;
	if( m_fExtendDashWaitAniRate > 0.0f )
		fCurRate = m_fExtendDashWaitAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	m_ExtendDashState = EDS_WAIT;
}

void ioSaberItem::ProcessSaberExtendDashWaitState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		ChangeSaberExtendDashRunningState( pOwner );
}

//Extend Dash Run
void ioSaberItem::ChangeSaberExtendDashRunningState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashRunningAni );
	if ( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	float fCurRate = FLOAT1;
	if( m_fExtendDashRunningAniRate > 0.0f )
		fCurRate = m_fExtendDashRunningAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

	m_ExtendDashState = EDS_RUN;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
	pOwner->SetTargetMarkerRange( m_fExtendDashTargetMarkerRange );
	
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_EXTEND_DASH );
		kPacket << static_cast<byte>( m_ExtendDashState );		//EDS_RUN;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSaberItem::ProcessSaberExtendDashRunState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if ( pOwner->IsNeedProcess() )
	{
		if ( m_fCurBullet <= 0.0f )
		{
			ChangeSaberExtendDashEndState( pOwner );
			return;
		}

		if ( !pOwner->IsAttackKeyDown() )
		{
			if ( IsEnableSaberExtendDashAttackGauge() )
			{
				ChangeSaberExtendDashAttackState( pOwner );
				return;
			}
			ChangeSaberExtendDashEndState( pOwner );
			return;
		}

		float fHeight = pOwner->GetBottomHeight();
		if ( fHeight <= 0.0f )
		{
			pOwner->SetFallState( true );
			return;
		}
		CheckSaberExtendDashKeyInput( pOwner );
		CheckAttTarget( pOwner );
	}
	pOwner->SetCurMoveSpeed( m_fExtendDashRunSpeed );
}

void ioSaberItem::CheckSaberExtendDashKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) return;

	bool bChangeDir = false;
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			bChangeDir = true;

		m_CurDirKey = eNewDirKey;
	}

	if ( bChangeDir )
	{
		pOwner->SetTargetRotToDirKey( m_CurDirKey );
		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_SET_DIR );
			kPacket << (int)m_CurDirKey;
			kPacket << pOwner->GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

//Extend Dash Attack
void ioSaberItem::ChangeSaberExtendDashAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_ExtendDashAttackAttribute.m_AttackAnimation.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	ClearKeyReserve();

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashAttackAttribute.m_AttackAnimation );
	if ( iAniID  == - 1 )
		return;

	float fTimeRate = m_ExtendDashAttackAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_ExtendDashAttackAttribute.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_ExtendDashAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_ExtendDashAttackAttribute.m_vWeaponInfoList,
		FTT_EXTEND_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_ExtendDashAttackAttribute.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_ExtendDashAttackAttribute.m_AttackAnimation, fTimeRate );

	DWORD dwMoveDuration = 0;
	D3DXQUATERNION qtNewRot = pOwner->GetTargetRot();
	if( pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vTargetPos;
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );

		bool bTargetOk = false;
		// Target 유효성 체크.
		if( pTarget && CheckTargetValidate(pOwner) )
		{
			bTargetOk = true;
		}
		
		if( bTargetOk )
		{
			D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();

			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );
		}
	}

	m_ExtendDashState = EDS_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_EXTEND_DASH );
		kPacket << static_cast<byte>( m_ExtendDashState );		//EDS_ATTACK;
		kPacket << qtNewRot;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}


	DecreaseGauge( m_fSaberExtendDashNeedBullet );
}

void ioSaberItem::ProcessSaberExtendDashAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}

	if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
	{
		if ( !ChangeStateByReserveKeyInput( pOwner ) )
			pOwner->SetState( CS_DELAY );

		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	}

	if( pOwner->IsNeedProcess() )
	{
		if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			CheckReserve( pOwner );
	}
}

//Extend Dash End
void ioSaberItem::ChangeSaberExtendDashEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashEndAni );
	if ( iAniID == - 1)
		return;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni(  FLOAT100 );

	float fCurRate = FLOAT1;
	if( m_fExtendDashEndAniRate > 0.0f )
		fCurRate = m_fExtendDashEndAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	m_ExtendDashState = EDS_END;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_SABER_EXTEND_DASH );
		kPacket << static_cast<byte>( m_ExtendDashState );		//EDS_END;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioSaberItem::ProcessSaberExtendDashEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

//Search Target
void ioSaberItem::CheckAttTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( m_szTargetName.IsEmpty() && !m_bAimedBall )
	{
		FindAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_szTargetName.Clear();
			m_bAimedBall = true;
		}

		if( !m_szTargetName.IsEmpty() || m_bAimedBall )
		{
			pOwner->SetTargetMarkerTarget( m_szTargetName, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			
		}
	}
	else if( !CheckTargetValidate( pOwner ) )
	{
		FindAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_szTargetName.Clear();
			m_bAimedBall = true;
		}

		if( !m_szTargetName.IsEmpty() || m_bAimedBall )
		{
			pOwner->SetTargetMarkerTarget( m_szTargetName, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}
		else
		{
			pOwner->SetTargetMarkerTarget( m_szTargetName, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}
	}
}

void ioSaberItem::FindAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_szTargetName.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_fCurTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fCurTargetRange * m_fCurTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_fCurTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fCurTargetDownHeight )
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
			m_szTargetName = szTarget;
		}
	}
}

bool ioSaberItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget( false ) )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurTargetWoundType ) )
		return false;

	return true;
}

bool ioSaberItem::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fCurTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fCurTargetRange * m_fCurTargetRange;

	if( m_bAimedBall )
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
		if( fHeightGap > 0.0f && fHeightGap > m_fCurTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fCurTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}
	else
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
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
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fCurTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fCurTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}

	return true;
}

bool ioSaberItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fCurTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fCurTargetRange * m_fCurTargetRange;

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
	if( fHeightGap > 0.0f && fHeightGap > m_fCurTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fCurTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

int ioSaberItem::GetNeedBullet()
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioBaseChar *pOwner = NULL;
	if( pStage )
		pOwner = pStage->GetBaseChar( m_OwnerName );

	if( !pOwner )
		return 0;

	switch( pOwner->GetState() )
	{
	case CS_SABER_ATTACK:
		return (int)m_fChargingAttackNeedBullet;
	case CS_SABER_FLY:
		return 0;
	case CS_SABER_DASH_ATTACK:
		return (int)m_fSaberExtendDashNeedBullet;
	}
	return (int)m_fChargingAttackNeedBullet;
}

void ioSaberItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxBullet ) );

	m_fCurBullet = fCurBullet;
}

bool ioSaberItem::IsSaberCollisionSkipState()
{
	switch( m_SaberFlyState )
	{
	case SFS_START:
	case SFS_MOVE:
	case SFS_ATTACK_MOVE:
		return true;
	}

	return false;
}

void ioSaberItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker,"" );
}

void ioSaberItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );

	ioAttackableItem::OnReleased( pOwner );
}

void ioSaberItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btState;
	rkPacket >> btState;
	if ( btState == CS_NONE )
	{
		bool bDelayState;
		rkPacket >> bDelayState;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;

		if( bDelayState )
			pOwner->SetState( CS_DELAY );
	}
	else if ( btState == CS_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;

		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );

		ChangeToNormalAttack( pOwner );
	}
}

void ioSaberItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btSyncType;
	rkPacket >> btSyncType;
	switch( btSyncType )
	{
	case ST_NONE:
		break;
	case ST_SET_TARGET:
		rkPacket >> m_szTargetName;
		break;
	case ST_SET_DIR:
		{
			int iDirKey;
			rkPacket >> iDirKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDirKey;
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToDirKey( m_CurDirKey );
		}
		break;
	case ST_SABER_CHARGE_ATTACK:
		ApplySaberChargeAttack( pOwner, rkPacket );
		break;
	case ST_SABER_FLY:
		ApplySaberFly( pOwner, rkPacket );
		break;
	case ST_SABER_EXTEND_DASH:
		ApplySaberExtendDash( pOwner, rkPacket );
		break;
	}
}

void ioSaberItem::ApplySaberChargeAttack( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btChargeAttackType;
	rkPacket >> btChargeAttackType;
	if ( btChargeAttackType == CAS_ATTACK )
	{
		bool bJumpState;
		rkPacket >> bJumpState;
		ClearSaberChargeAttackData();
		pOwner->SetState( CS_SABER_ATTACK );
		m_CurSaberChargeAttack.Init();
		if ( bJumpState )
			m_CurSaberChargeAttack = m_JumpSaberChargeAttack;
		else 
			m_CurSaberChargeAttack = m_SaberChargeAttack;
		ChangeSaberChargeAttackState( pOwner );

		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vPos;
		rkPacket >> qtRot;
		rkPacket >> vPos;

		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		pOwner->SetWorldPosition( vPos );
	}
	else if ( btChargeAttackType == CAS_MOVE )
	{
		m_dwMotionStartTime = 0;
		m_dwMotionEndTime = 0;
		rkPacket >> m_fCurMoveSpeed;
		rkPacket >> m_fCurMaxRange;
		rkPacket >> m_vChargeMoveDir;
		ioHashString szAni;
		float fRate;
		rkPacket >> szAni;
		rkPacket >> fRate;

		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		SetMoveAni( pOwner, szAni, fRate, m_vChargeMoveDir );
	}
	else if ( btChargeAttackType == CAS_COMBO_ATTACK )
	{
		//int iDirKey;
		//rkPacket >> iDirKey;
		//if ( iDirKey != ioUserKeyInput::DKI_NONE )
//			pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iDirKey );

		rkPacket >> m_iCurChargeCombo;

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotToRotate( qtRot, true );

		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		ChangeSaberChargeComboAttackState( pOwner );
	}
}

void ioSaberItem::ApplySaberFly( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btFlyType;
	rkPacket >> btFlyType;
	if ( btFlyType == SFS_START )
	{
		bool bJumpState;
		rkPacket >> bJumpState;

		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		SetSaberFlyState( pOwner, bJumpState );
	}
	else if ( btFlyType == SFS_DELAY )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		ChangeSaberFlyDelay( pOwner );
	}
	else if ( btFlyType == SFS_MOVE )
	{
		bool bChangeDir;
		int iDir;
		D3DXQUATERNION qtRot;
		rkPacket >> bChangeDir;
		rkPacket >> qtRot;
		rkPacket >> iDir;		
		
		bool bChangeUpDown, bSetUpKey, bSetDownKey;
		rkPacket >> bChangeUpDown >> bSetUpKey >> bSetDownKey;

		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		if( bChangeDir )
		{
			pOwner->SetTargetRot( qtRot );
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			ChangeSaberFlyMove( pOwner );
		}

		if( bChangeUpDown )
		{
			m_bSetUpKey = bSetUpKey;
			m_bSetDownKey = bSetDownKey;
		}
	}
	else if ( btFlyType == SFS_CHARGING )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		ChangeSaberFlyChargeState( pOwner );
	}
	else if ( btFlyType == SFS_ATTACK_MOVE )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;

		pOwner->SetTargetRotToRotate( qtRot, true );

		DWORD dwMoveDuration;
		rkPacket >> m_vFlyMoveDir ;
		rkPacket >> m_fFlyMoveSpeed ;
		rkPacket >> dwMoveDuration;

		m_dwMotionEndTime = FRAMEGETTIME() + dwMoveDuration;

		rkPacket >> m_bCharge;

		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		ChangeSaberFlyAttackMove( pOwner );
	}
	else if ( btFlyType == SFS_COMBO_ATTACK )
	{
		int iDir;
		rkPacket >> iDir;
		if ( iDir != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iDir );

		rkPacket >> m_iCurFlyChargeCombo;
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotToRotate( qtRot, true );
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		ChangeSaberFlyComboAttackState( pOwner );
	}
}

void ioSaberItem::ApplySaberExtendDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE btExtendDashType;
	rkPacket >> btExtendDashType;
	if ( btExtendDashType == EDS_WAIT )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		SetSaberExtendDashState( pOwner );
	}
	else if ( btExtendDashType == EDS_RUN )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		ChangeSaberExtendDashRunningState( pOwner );
	}
	else if ( btExtendDashType == EDS_ATTACK )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;

		pOwner->SetTargetRotToRotate( qtRot, true );
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		ChangeSaberExtendDashAttackState( pOwner );

	}
	else if ( btExtendDashType == EDS_END )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		ChangeSaberExtendDashEndState( pOwner );
	}
}

void ioSaberItem::CheckReserveAttackKey( ioBaseChar *pOwner )
{
	if( pOwner->IsAttackKey() && !m_bEnableAttackState )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
}

void ioSaberItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if ( pWoundChar->IsNeedProcess() && fDamage > 0.0f )
	{
		if ( !m_szTargetName.IsEmpty() )
			return;

		m_szTargetName = pWoundChar->GetCharName();

		if( pWoundChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << static_cast<byte>( ST_SET_TARGET );
			kPacket << m_szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioSaberAttackSpecialState::ioSaberAttackSpecialState()
{
}

ioSaberAttackSpecialState::~ioSaberAttackSpecialState()
{
}

void ioSaberAttackSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSaberAttackSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioSaberItem *pSaberItem = ToSaberItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSaberItem )
		pSaberItem->ClearSaberChargeAttackData( pOwner );
}

bool ioSaberAttackSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioSaberItem *pSaberItem = ToSaberItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pSaberItem && pSaberItem->IsSaberCollisionSkipState( ) )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioSaberAttackSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSaberItem *pSaber = ToSaberItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSaber )
		pSaber->ProcessSaberChargeAttackState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSaberAttackSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSaberAttackSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioSaberFlySpecialState::ioSaberFlySpecialState()
{
}

ioSaberFlySpecialState::~ioSaberFlySpecialState()
{
}

void ioSaberFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSaberFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioSaberItem *pSaberItem = ToSaberItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSaberItem )
		pSaberItem->ClearSaberFlyData( pOwner );
}

bool ioSaberFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioSaberItem *pSaberItem = ToSaberItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pSaberItem && pSaberItem->IsSaberCollisionSkipState( ) )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioSaberFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSaberItem *pSaber = ToSaberItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSaber )
		pSaber->ProcessSaberFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSaberFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSaberFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioSaberDashAttackSpecialState::ioSaberDashAttackSpecialState()
{
}

ioSaberDashAttackSpecialState::~ioSaberDashAttackSpecialState()
{
}

void ioSaberDashAttackSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSaberDashAttackSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioSaberItem *pSaberItem = ToSaberItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSaberItem )
		pSaberItem->ClearSaberExtendDashData( pOwner );
}

void ioSaberDashAttackSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSaberItem *pSaber = ToSaberItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSaber )
		pSaber->ProcessSaberExtendDashState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}