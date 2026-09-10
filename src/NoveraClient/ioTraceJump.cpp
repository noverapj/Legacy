

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ItemDefine.h"

ioTraceJump::ioTraceJump()
{
	m_ChargeComboState = CCS_NONE;

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedFlash = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_dwChargingStartTime = 0;
	m_dwFireStartTime = 0;

	// double
	m_iCurJumpCnt = 1;

	// dash
	m_dwJumpDashStartTime = 0;
	m_dwJumpDashEndTime = 0;

	m_GuidEffectID = -1;

	m_bUsedChargeAttack = false;

	m_dwAllFireAttackStartTime = 0;
	m_dwAllFireAttackEndTime = 0;
}

ioTraceJump::ioTraceJump( const ioTraceJump &rhs )
: ioExtendJump( rhs ),
m_fJumpSpeedRateFR( rhs.m_fJumpSpeedRateFR ),
m_fJumpSpeedRateSI( rhs.m_fJumpSpeedRateSI ),
m_fJumpSpeedRateBG( rhs.m_fJumpSpeedRateBG ),
m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
m_ChargeComboAttackList( rhs.m_ChargeComboAttackList ),
m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
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
m_ChargingAniList( rhs.m_ChargingAniList ),
m_fChargeJumpPower( rhs.m_fChargeJumpPower ),
m_fChargeGravityRate( rhs.m_fChargeGravityRate ),
m_fChargeBaseRange( rhs.m_fChargeBaseRange ),
m_ChargeAttackLandAni( rhs.m_ChargeAttackLandAni ),
m_fChargeAttackLandAniRate( rhs.m_fChargeAttackLandAniRate ),
m_ExtendAttackList( rhs.m_ExtendAttackList ),
m_fDashEndJumpPower( rhs.m_fDashEndJumpPower ),
m_TraceComboAttackList( rhs.m_TraceComboAttackList ),
m_AllFireAttack( rhs.m_AllFireAttack ),
m_vJumpDashAniList( rhs.m_vJumpDashAniList ),
m_bMaxJumpLastChargeAttack( rhs.m_bMaxJumpLastChargeAttack ),
m_fNeedDashGauge( rhs.m_fNeedDashGauge ),
m_fNeedJumpGauge( rhs.m_fNeedJumpGauge ),
m_fNeedChagingGauge( rhs.m_fNeedChagingGauge ),
m_fNeedAllFireGauge( rhs.m_fNeedAllFireGauge )
{
	m_ChargeComboState = CCS_NONE;

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedFlash = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_dwChargingStartTime = 0;	
	m_dwFireStartTime = 0;

	// double
	m_iCurJumpCnt = 1;

	// dash
	m_dwJumpDashStartTime = 0;
	m_dwJumpDashEndTime = 0;

	m_GuidEffectID = -1;

	m_bUsedChargeAttack = false;

	m_bLanded = true;
	m_bFirstJump = true;
	m_nDecGaugeCnt = 0;
}

ioExtendJump* ioTraceJump::Clone()
{
	return new ioTraceJump( *this );
}

ioTraceJump::~ioTraceJump()
{
}

JumpType ioTraceJump::GetType() const
{
	return JT_TRACE_JUMP;
}

void ioTraceJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//
	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	m_bEnableDoubleJump = rkLoader.LoadBool_e( "enable_double_jump", false );
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );

	// combo
	m_fJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	LoadTraceComboList( rkLoader );
	LoadChargeComboList( rkLoader );
	LoadAllFireComboList( rkLoader );

	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );

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


	// dash
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_ComboDashType = (ComboDashType)rkLoader.LoadInt_e( "jump_dash_type", CDT_GRAVITY );

	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "dash_jump_attack", m_DashJumpAttack, rkLoader );

	rkLoader.LoadString_e( "jump_dash_land_ani", "", szBuf, MAX_PATH );
	m_JumpDashLandAni = szBuf;
	m_fJumpDashLandAniRate = rkLoader.LoadFloat_e( "jump_dash_land_ani_rate", FLOAT1 );

	m_fJumpDashGravityAmt = rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );

	m_bUseNewJumpTypeDash = rkLoader.LoadBool_e( "use_new_jump_type_dash", false );

	//
	m_fChargeJumpPower = rkLoader.LoadFloat_e( "jump_charge_jump_power", 0.0f );
	m_fChargeGravityRate = rkLoader.LoadFloat_e( "jump_charge_gravity_rate", FLOAT1 );
	m_fChargeBaseRange = rkLoader.LoadFloat_e( "jump_charge_base_range", 0.0f );

	rkLoader.LoadString_e( "jump_charge_land", "", szBuf, MAX_PATH );
	m_ChargeAttackLandAni = szBuf;
	m_fChargeAttackLandAniRate = rkLoader.LoadFloat_e( "jump_charge_land_time_rate", FLOAT1 );

	// extend combo
	int iMax = rkLoader.LoadInt_e( "extend_jump_max_cnt", 0 );
	for( int i=0; i < iMax; ++i )
	{
		AttackAttribute eAttribute;

		wsprintf_e( szKey, "extend_jump_attack%d", i+1);
		LoadAttackAttribute( szKey, eAttribute, rkLoader );

		m_ExtendAttackList.push_back( eAttribute );
	}

	//////////////////////////////////////////////////////////////////////////
	m_fDashEndJumpPower = rkLoader.LoadFloat_e( "jump_dash_end_jump_power", 0.f );

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

	//메이드때 추가
	m_bMaxJumpLastChargeAttack = rkLoader.LoadBool_e( "max_jump_and_last_charge_attack", false );

	m_fNeedDashGauge = rkLoader.LoadFloat_e( "extra_jump_dash_need_gauge", FLOAT10 );
	m_fNeedJumpGauge = rkLoader.LoadFloat_e( "extra_jump_double_need_gauge", FLOAT10 );
	m_fNeedChagingGauge = rkLoader.LoadFloat_e( "extra_jump_charging_need_gauge", FLOAT10 );
	m_fNeedAllFireGauge = rkLoader.LoadFloat_e( "extra_jump_allfire_need_gauge", FLOAT10 );
}


void ioTraceJump::LoadTraceComboList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	int iMaxCombo = rkLoader.LoadInt_e( "jump_max_trace_combo", 0 );

	m_TraceComboAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_trace_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_TraceComboAttackList.push_back( kAttribute );
	}
}


void ioTraceJump::LoadChargeComboList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	int iMaxChargeCombo = rkLoader.LoadInt_e( "jump_max_trace_charge_combo", 0 );

	m_ChargeComboAttackList.clear();
	for( int i = 0; i < iMaxChargeCombo; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "jump_charge_trace_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ChargeComboAttackList.push_back( kAttribute );
	}
}


void ioTraceJump::LoadAllFireComboList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	int iMaxAllFireCombo = rkLoader.LoadInt_e( "jump_max_trace_all_fire_combo", 0 );

	m_AllFireAttack.clear();
	for( int i = 0; i < iMaxAllFireCombo; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "jump_all_fire_trace_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_AllFireAttack.push_back( kAttribute );
	}
}


void ioTraceJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedFlash = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_dwChargingStartTime = 0;
	m_dwFireStartTime = 0;

	// double
	m_iCurJumpCnt = 1;

	// dash
	m_dwJumpDashStartTime = 0;
	m_dwJumpDashEndTime = 0;

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

	//
	m_bUsedChargeAttack = false;

	if( m_bFirstJump )
	{
		m_bFirstJump = false;
	}
}


void ioTraceJump::ProcessLanding( ioBaseChar *pOwner )
{
}

void ioTraceJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
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

	if( IsCanJumpDash( pOwner, fHeightGap ) )
		SetJumpDash( pOwner, fHeightGap );

	// Double
	if( IsCanDoubleJump( pOwner, fHeightGap ) )
	{
		if( m_fDoubleJumpPower >= 0.0f )
		{
			SetDoubleJump( pOwner );
			return;
		}
	}
	
	if( IsCanAllFireAttack( pOwner) )
	{
		SetAllFireAttack( pOwner );
		return;
	}

	ProcessJumpDash( pOwner );
}


bool ioTraceJump::IsCanAllFireAttack( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() ) return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) 
		return false;

	if( m_ChargeComboState != CCS_NONE && m_ChargeComboState != CCS_DOUBLE && m_ChargeComboState != CCS_DASH_END )
		return false;

	if( !pOwner->IsDefenseKey() )
		return false;

	//게이지 체크
	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioTraceItem *pTraceItem = ToTraceItem( pWeapon );
	if( !pTraceItem || !pTraceItem->IsEnableGauge( m_fNeedAllFireGauge ) )
		return false;

	return true;
}


void ioTraceJump::SetAllFireAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioTraceItem *pTraceItem = ToTraceItem( pWeapon );
	if( pTraceItem )
		pTraceItem->DecreaseAllBullet();
	else
		return;

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
	m_ChargeComboState = CCS_ALL_FIRE;

	pOwner->ClearReservedSliding();

	int iAllFireCnt = m_AllFireAttack.size();

	if( !COMPARE( m_nDecGaugeCnt, 0, iAllFireCnt ) )
		return;

	const AttackAttribute *pAttr = &m_AllFireAttack[m_nDecGaugeCnt];
	if( !pAttr ) 
		return;

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate  = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;
	DWORD dwEndDelay = pAttr->m_dwEndDelay;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );
	//
	m_dwAllFireAttackStartTime = dwCurTime;
	m_dwAllFireAttackEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	pOwner->SetJumpAttackState();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.f );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_JUMP_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetMoveDir();
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_nDecGaugeCnt;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


// combo
void ioTraceJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_NORMAL:
	case CCS_DOUBLE:
	case CCS_DASH_END:
		break;
	default:
		return;
	}

	int iMaxCnt = m_TraceComboAttackList.size();
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

void ioTraceJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_TraceComboAttackList.size();
	if( !COMPARE( m_nDecGaugeCnt, 0, iMaxCnt ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_NORMAL;

	//pOwner->ClearAttackFireTimeAndSkill();

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

	int iAniID = pGrp->GetAnimationIdx( m_TraceComboAttackList[m_nDecGaugeCnt].m_AttackAnimation );
	float fTimeRate = m_TraceComboAttackList[m_nDecGaugeCnt].m_fAttackAniRate;
	DWORD dwPreDelay = m_TraceComboAttackList[m_nDecGaugeCnt].m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_TraceComboAttackList[m_nDecGaugeCnt].m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

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
		kPacket << m_nDecGaugeCnt;
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


void ioTraceJump::SetComboChargeJumpAttack( ioBaseChar *pOwner )
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

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
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
void ioTraceJump::SetDoubleJump( ioBaseChar *pOwner )
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

	//게이지 체크
	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioTraceItem *pTraceItem = ToTraceItem( pWeapon );
	if( pTraceItem )
		pTraceItem->DecreaseBullet( m_fNeedJumpGauge );

	m_nDecGaugeCnt = m_nDecGaugeCnt + (int)m_fNeedJumpGauge;

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
		kPacket << m_nDecGaugeCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

bool ioTraceJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;
	if( !m_bEnableDoubleJump ) return false;

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

	if( pOwner->IsAttackKeyDown() || pOwner->IsDefenseKeyDown() )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_DOUBLE:
	case CCS_DASH_END:
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

	//게이지 체크
	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioTraceItem *pTraceItem = ToTraceItem( pWeapon );
	if( !pTraceItem || !pTraceItem->IsEnableGauge( m_fNeedJumpGauge ) )
		return false;

	return true;
}

void ioTraceJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

// jump dash
bool ioTraceJump::IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !m_bEnableJumpDash ) return false;

	if( !pOwner->IsNeedProcess() ) return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	if( m_ChargeComboState != CCS_NONE && m_ChargeComboState != CCS_DOUBLE && m_ChargeComboState != CCS_DASH_END )
		return false;

	//게이지 체크
	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioTraceItem *pTraceItem = ToTraceItem( pWeapon );
	if( !pTraceItem || !pTraceItem->IsEnableGauge( m_fNeedDashGauge ) )
		return false;

	return true;
}

bool ioTraceJump::IsJumpDashState()
{
	switch( m_ChargeComboState )
	{
	case CCS_DASH:
		return true;
	}

	return false;
}

void ioTraceJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

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
	case CCS_EXTEND_COMBO:
		ProcessExtendComboAttack( pOwner );
		break;
	default:
		CheckAirJump( pOwner );
		break;
	}

}

void ioTraceJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	CheckAirJump( pOwner );

	if( m_bReserveJumpAttack )
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


void ioTraceJump::ProcessJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	switch( m_ChargeComboState )
	{
	case CCS_DASH:
		pOwner->ClearDirDoubleClick();
		break;
	default:
		return;
	}

	if( m_ComboDashType != CDT_NO_GRAVITY )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if(m_dwDashKeyReserveTime && m_dwDashKeyReserveTime < dwCurTime && !m_bDefenseKey && !m_bAttackKey )
	{
		if( pOwner->IsAttackKey() )
			m_bAttackKey = true;
		else if( pOwner->IsDefenseKey() )
			m_bDefenseKey = true;
	}

	switch( m_ChargeComboState )
	{
	case CCS_DASH:
		if( m_dwJumpDashEndTime > 0 && m_dwJumpDashEndTime < dwCurTime )
		{
			if( m_bAttackKey )
			{
				SetComboJumpAttack( pOwner );
				return;
			}
			
			if( m_bDefenseKey )
			{
				SetAllFireAttack( pOwner );
				return;
			}

			m_dwJumpDashEndTime = 0;
			m_ChargeComboState = CCS_DASH_END;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_JUMP_DASH );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetMoveDir();
				kPacket << m_ChargeComboState;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetMoveDir();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );

			pOwner->SetJumpPower( m_fDashEndJumpPower );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			SetDoubleJumpAni( pOwner );
		}
		break;
	case CCS_DASH_END:
		break;
	}
}

void ioTraceJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
			rkPacket >> m_nDecGaugeCnt;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			if( !COMPARE( m_nDecGaugeCnt, 0, (int) m_TraceComboAttackList.size() ) )
				return;

			m_ChargeComboState = CCS_NORMAL;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			if( pOwner && !pOwner->IsActionStop() )
				pOwner->InitActionStopDelay( true );
			else
				pOwner->InitActionStopDelay( false );

			int iAniID = pGrp->GetAnimationIdx( m_TraceComboAttackList[m_nDecGaugeCnt].m_AttackAnimation );
			float fTimeRate = m_TraceComboAttackList[m_nDecGaugeCnt].m_fAttackAniRate;
			DWORD dwPreDelay = m_TraceComboAttackList[m_nDecGaugeCnt].m_dwPreDelay;
			DWORD dwStartTime = dwCurTime + dwPreDelay;
			m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			pOwner->SetAutoTarget( ATT_JUMP );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pOwner->RefreshFireTimeList( iAniID,
				m_TraceComboAttackList[m_nDecGaugeCnt].m_vWeaponInfoList,
				FTT_JUMP_ATTACK,
				fTimeRate,
				dwPreDelay );

			DWORD dwTrackingTime = dwStartTime;
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

			m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

			// For AirJumpPower
			SetAirJump( pOwner, iAniID, fTimeRate );

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
			rkPacket >> m_nDecGaugeCnt;

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
	case CCS_DASH:
		{
			D3DXQUATERNION qtRotate;
			D3DXVECTOR3 vMoveDir;
			rkPacket >> qtRotate;
			rkPacket >> vMoveDir;
			rkPacket >> m_nDecGaugeCnt;

			pOwner->SetTargetRotToRotate( qtRotate, false );
			pOwner->SetMoveDirection( vMoveDir );

			SetJumpDash( pOwner, 0.f  );

			pOwner->ClearDirDoubleClick();
		}
		break;
	case CCS_DASH_END:
		{
			//D3DXQUATERNION qtRotate;
			D3DXVECTOR3 vWorldPos;
			D3DXVECTOR3 vMoveDir;
			rkPacket >> vWorldPos;
			rkPacket >> vMoveDir;

			pOwner->SetWorldPosition( vWorldPos );
			pOwner->SetMoveDirection( vMoveDir );

			m_dwJumpDashEndTime = 0;
			m_ChargeComboState = CCS_DASH_END;

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );

			pOwner->SetJumpPower( m_fDashEndJumpPower );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			SetDoubleJumpAni( pOwner );
		}
		break;
	case CCS_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			rkPacket >> m_nDecGaugeCnt;
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

			rkPacket >> m_nDecGaugeCnt;

			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			dwWeaponBaseIndex = dwWeaponBaseIndex - 100;

			pOwner->SetWeaponIndexBase(dwWeaponBaseIndex);
			ChangeChargeJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
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
	case CCS_ALL_FIRE :
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			rkPacket >> m_nDecGaugeCnt;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetAllFireAttack( pOwner );
		}
		break;
	}
}

void ioTraceJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_bReserveJumpAttack = false;

	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	m_bLanded = true;
	m_bFirstJump = true;
	m_nDecGaugeCnt = 0;
}

float ioTraceJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

bool ioTraceJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_DASH:
	case CCS_NEW_JUMP:
		return false;
	}

	return true;
}

DWORD ioTraceJump::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

void ioTraceJump::SetChargingState( ioBaseChar *pOwner, bool bException )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_ChargingAniList.empty() )
		return;

	//pOwner->ClearAttackFireTimeAndSkill();
	DWORD dwCurTime = FRAMEGETTIME();


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
			kPacket << m_nDecGaugeCnt;
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

	ioHashString szCurChargingAni;

	int iChargingCnt = m_ChargingAniList.size();
	if( COMPARE( m_nDecGaugeCnt, 0, iChargingCnt ) )
		szCurChargingAni = m_ChargingAniList[m_nDecGaugeCnt];
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
		kPacket << m_nDecGaugeCnt;
		kPacket << bException;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTraceJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	//게이지 체크
	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioTraceItem *pTraceItem = ToTraceItem( pWeapon );
	if( !pTraceItem )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && pTraceItem->IsEnableGauge( m_fNeedChagingGauge ) )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			ChangeChargeJumpAttack( pOwner );
			return;
		}
	}
	else
	{
		bool bMaxJumpLimit = pOwner->CheckJumpLimitCnt();

		if( !(m_bMaxJumpLastChargeAttack && !bMaxJumpLimit) )
			SetComboJumpAttack( pOwner );

		return;
	}
}

int ioTraceJump::GetCurChargeComboState()
{
	return m_ChargeComboState;
}

void ioTraceJump::ChangeChargeJumpAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_ChargeComboAttackList.empty() )
		return;

	int iChargingCnt = m_ChargeComboAttackList.size();

	//pOwner->ClearAttackFireTimeAndSkill();

	if( !COMPARE( m_nDecGaugeCnt, 0, iChargingCnt ) )
		return;

	const AttackAttribute *pAttr = &m_ChargeComboAttackList[m_nDecGaugeCnt];
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

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay, true );

	m_ChargeComboState = CCS_EXTEND_ATTACK;
	m_bUsedChargeAttack = true;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	//
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

	//게이지 체크
	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioTraceItem *pTraceItem = ToTraceItem( pWeapon );
	if( pTraceItem )	
		pTraceItem->DecreaseBullet( m_fNeedChagingGauge );

	//승환씨 요청 사항
	//pOwner->SetJumppingLimitCnt( 0 );
	DWORD dwWeaponBaseIndex = pOwner->GetWeaponIndexBase();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_nDecGaugeCnt;
		kPacket << dwWeaponBaseIndex;
		
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_nDecGaugeCnt = m_nDecGaugeCnt + (int)m_fNeedChagingGauge;
}

float ioTraceJump::GetFlightDuration( float fCurGravity )
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

void ioTraceJump::PowerChargeMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_ChargeComboState != CCS_EXTEND_ATTACK )
		return;

	if( !pChar ) return;
	if( !pStage ) return;
	if( m_fCurCharMoveSpeed == 0.0f ) return;

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
}

bool ioTraceJump::IsChargeAttackState()
{
	return m_bUsedChargeAttack;
}


bool ioTraceJump::CheckEnableExtendCombo( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_ExtendAttackList.empty() )
		return false;

	return true;
}

void ioTraceJump::SetExtendComboState( ioBaseChar *pOwner, bool bRefresh )
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

void ioTraceJump::ProcessExtendComboAttack( ioBaseChar *pOwner )
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

void ioTraceJump::CheckExtendComboAttack( ioBaseChar *pOwner )
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

void ioTraceJump::OnEndExtendJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}
}

void ioTraceJump::SetJumpDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();

	if( pOwner->IsNeedProcess() && eDir != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetMoveDirByRotate( qtRotate );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	m_ChargeComboState = CCS_DASH;
	m_bUseExtendJump = true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	int iAniID = GetJumpDashAni( pOwner );
	float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	//pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_dwJumpDashStartTime = dwCurTime;
	m_dwJumpDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	/*DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );*/


	//대시를 하면 점프 카운트가 초기화됨
	m_iCurJumpCnt = 0;
	m_dwDashKeyReserveTime = 0;
	DWORD dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserveTime )
		m_dwDashKeyReserveTime = dwCurTime +  dwKeyReserveTime;

	m_bDefenseKey = false;
	m_bAttackKey = false;

	//게이지 체크
	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioTraceItem *pTraceItem = ToTraceItem( pWeapon );
	if( pTraceItem )
		pTraceItem->DecreaseBullet( m_fNeedDashGauge );
	//m_nDecGaugeCnt++;

	//승환씨 요청 사항
	pOwner->SetJumppingLimitCnt( 0 );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_JUMP_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetMoveDir();
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << m_nDecGaugeCnt;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_nDecGaugeCnt = m_nDecGaugeCnt + (int)m_fNeedDashGauge;

	if( m_bUseNewJumpTypeDash )
	{
		DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
		SetNewJumpTypeDashAttack( pOwner, m_dwJumpDashEndTime, dwKeyReserveTime );
		return;
	}
}

void ioTraceJump::SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime )
{
	if( !pOwner ) return;

	m_bUseExtendJump = false;
	m_ChargeComboState = CCS_NEW_JUMP;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, m_fJumpDashGravityAmt );
}

float ioTraceJump::GetCurGravityAmt()
{
	if( m_ChargeComboState == CCS_DASH )
	{
		if( m_ComboDashType == CDT_GRAVITY )
			return m_fJumpDashGravityAmt;
		else if( m_ComboDashType == CDT_NO_GRAVITY )
			return -FLOAT1;
	}

	return 0.0f;
}

bool ioTraceJump::IsEnableJumpLandEndDash()
{
	if( m_ChargeComboState == CCS_DASH )//|| m_ChargeComboState == CCS_DASH_END )
		return true;

	return false;
}

bool ioTraceJump::IsJumpAttackReserve()
{
	return false;
}

bool ioTraceJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return true;
}


int ioTraceJump::GetJumpDashAni( ioBaseChar *pOwner )
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


bool ioTraceJump::IsCanMaxJumpAndChargeAttack( ioBaseChar *pOwner )
{
	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioTraceItem *pTraceItem = ToTraceItem( pWeapon );
	if( !pTraceItem || !pTraceItem->IsEnableGauge( m_fNeedChagingGauge ) )
		return false;

	return m_bMaxJumpLastChargeAttack;
}