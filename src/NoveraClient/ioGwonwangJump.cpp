

#include "stdafx.h"

#include "ioGwonwangJump.h"
#include "ItemDefine.h"
#include "FindPredicateImpl.h"

ioGwonwangJump::ioGwonwangJump()
{
	m_bUsedDoubleJump					= false;
	m_iCurJumpCnt						= 1;

	m_iCurComboCnt						= 0;
	m_dwEnableComboTime_S				= 0;
	m_dwEnableComboTime_E				= 0;	

	m_bReserveJumpAttack				= false;

	m_ComboState						= CST_NONE;
	m_JumpState							= PJS_NONE;

	m_dwComboDashStartTime				= 0;
	m_dwComboDashEndTime				= 0;

	m_bUsedComboDash					= false;
	m_bUsedCancelComboDash				= false;	// 2020-06-17

	m_dwChargingStartTime				= 0;

	m_dwMotionEndTime					= 0;
	
	m_bUsedChargeAttack					= false;
	m_dwChargeAttackEndTime				= 0;

	m_dwJumpDashAddAttackReserveTime	= 0;
	m_iJumpDashAddAttackIndex			= 0;
	m_bUseHeighJump						= false;
	m_bUseNormalJumpAttack				= false;
}

ioGwonwangJump::ioGwonwangJump( const ioGwonwangJump &rhs )
: ioExtendJump( rhs ),
m_ComboAttackList( rhs.m_ComboAttackList ),
m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
m_fExtendJumpSpeedRateFR( rhs.m_fExtendJumpSpeedRateFR ),
m_fExtendJumpSpeedRateSI( rhs.m_fExtendJumpSpeedRateSI ),
m_fExtendJumpSpeedRateBG( rhs.m_fExtendJumpSpeedRateBG ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
m_ComboDashLandAni( rhs.m_ComboDashLandAni ),
m_fComboDashLandAniRate( rhs.m_fComboDashLandAniRate ),
m_bComboDashLandEnable( rhs.m_bComboDashLandEnable ),
m_fComboDashGravityAmt( rhs.m_fComboDashGravityAmt ),
m_ComboDashType( rhs.m_ComboDashType ),
m_dwComboDashEndJumpPower( rhs.m_dwComboDashEndJumpPower ),
m_bUseNewJumpTypeDash( rhs.m_bUseNewJumpTypeDash ),
m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
m_fMoveRange( rhs.m_fMoveRange ),
m_fMoveHeightRange( rhs.m_fMoveHeightRange ),
m_fChargeJumpPower( rhs.m_fChargeJumpPower ),
m_fChargeGravityRate( rhs.m_fChargeGravityRate ),
m_ExtendLandAttack( rhs.m_ExtendLandAttack ),
m_vJumpDashAddAttack( rhs.m_vJumpDashAddAttack ),
m_bJumpChargeEnable( rhs.m_bJumpChargeEnable ),
m_dwJumpKeyChargingTime( rhs.m_dwJumpKeyChargingTime ),
m_DashJumpAttack( rhs.m_DashJumpAttack ),				// 점프 대시공격
m_CancelDashJumpAttack( rhs.m_CancelDashJumpAttack ),	// 점프 기본공격 캔슬 대시
m_ItemLandAttackInfo( rhs.m_ItemLandAttackInfo ),		// 공중 잡기
m_bEnableHighJump( rhs.m_bEnableHighJump ),
m_JumpChargeAttribute( rhs.m_JumpChargeAttribute ),
m_stChargingAni( rhs.m_stChargingAni )
{
	m_iCurComboCnt					= 0;
	m_dwEnableComboTime_S			= 0;
	m_dwEnableComboTime_E			= 0;	
	m_bReserveJumpAttack			= false;
	m_JumpState						= PJS_NONE;
	m_ComboState					= CST_NONE;
	m_dwComboDashStartTime			= 0;
	m_dwComboDashEndTime			= 0;
	m_bUsedComboDash				= false;
	m_bUsedCancelComboDash			= false;	// 2020-06-17
	m_bUsedDoubleJump				= false;
	m_iCurJumpCnt					= 1;
	m_dwChargingStartTime			= 0;
	m_dwMotionEndTime				= 0;
	m_bUsedChargeAttack				= false;
	m_dwChargeAttackEndTime			= 0;
	m_dwJumpDashAddAttackReserveTime= 0;
	m_iJumpDashAddAttackIndex		= 0;
	m_bUseHeighJump					= false;
	m_bUseNormalJumpAttack			= false;
}

ioExtendJump* ioGwonwangJump::Clone()
{
	return new ioGwonwangJump( *this );
}

ioGwonwangJump::~ioGwonwangJump()
{
}

JumpType ioGwonwangJump::GetType() const
{
	return JT_GWONWANG_JUMP;
}

void ioGwonwangJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	
	m_fDoubleJumpSpeedRateFR = rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI = rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG = rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

	m_fExtendJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fExtendJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fExtendJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	LoadComboList( rkLoader );

	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );


	rkLoader.LoadString_e( "jump_dash_land_ani", "", szBuf, MAX_PATH );
	m_ComboDashLandAni = szBuf;
	m_fComboDashLandAniRate = rkLoader.LoadFloat_e( "jump_dash_land_ani_rate", FLOAT1 );
	m_bComboDashLandEnable = rkLoader.LoadBool_e( "jump_dash_land_enable", false );

	m_fComboDashGravityAmt = rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );

	m_bUseNewJumpTypeDash = rkLoader.LoadBool_e( "use_new_jump_type_dash", false );

	// Double
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

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_target_wound_type", TWT_ALL );

	m_fMoveRange = rkLoader.LoadFloat_e( "jump_teleport_move_range", 0.0f );
	m_fMoveHeightRange = rkLoader.LoadFloat_e( "jump_teleport_move_height_range", 0.0f );

	m_fChargeJumpPower = rkLoader.LoadFloat_e( "jump_charge_jump_power", 0.0f );
	m_fChargeGravityRate = rkLoader.LoadFloat_e( "jump_charge_gravity_rate", FLOAT1 );

	m_ExtendLandAttack.Init();
	LoadAttackAttribute_e( "jump_charge_land", m_ExtendLandAttack, rkLoader );

	char szKey[MAX_PATH] = "";
	int JumpDashAddAttackCnt = rkLoader.LoadInt_e( "dash_jump_attack_add_attack_cnt", 0 );
	for( int i=0; i<JumpDashAddAttackCnt; ++i )
	{
		AttackAttribute kAttack;
		kAttack.Init();

		wsprintf_e( szKey, "dash_jump_attack_add_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttack, rkLoader );

		m_vJumpDashAddAttack.push_back( kAttack );
	}
	

	m_bJumpChargeEnable = rkLoader.LoadBool_e( "jump_charge_enable", false );
	m_dwJumpKeyChargingTime = (DWORD)rkLoader.LoadInt_e( "jump_key_charging_time", 0 );

	// Dash JumpAttack
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_ComboDashType = (ComboDashType)rkLoader.LoadInt_e( "jump_dash_type", CDT_GRAVITY );
	m_dwComboDashEndJumpPower = rkLoader.LoadFloat( "combo_dash_end_jump_power", 0.0f );

	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "dash_jump_attack", m_DashJumpAttack, rkLoader );

	m_CancelDashJumpAttack.Init();
	LoadAttackAttribute_e( "cancel_dash_jump_attack", m_CancelDashJumpAttack, rkLoader );

	// 점프 차지
	rkLoader.LoadString( "charging_jump_ani", "", szBuf, MAX_PATH );
	m_stChargingAni = szBuf;

	// 점프 차지 공격
	LoadAttackAttribute_e( "charge_jump_attack", m_JumpChargeAttribute, rkLoader );

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

	// 사용 할까?
	m_bEnableHighJump = rkLoader.LoadBool_e( "enable_high_jump", false );
	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
}

void ioGwonwangJump::LoadComboList( ioINILoader &rkLoader )
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

void ioGwonwangJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;	

	m_bReserveJumpAttack = false;
	m_bUsedChargeAttack = false;
	m_dwChargeAttackEndTime = 0;

	
	m_bUseHeighJump  = false;

	m_ComboState = CST_NONE;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;
	m_iJumpDashAddAttackIndex = 0;

	m_dwChargingStartTime = 0;
	m_dwMotionEndTime = 0;
}

void ioGwonwangJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if ( m_dwChargingStartTime > 0 && CheckJumpCharging( pOwner ) )
		return;

	if( m_bEnableJumpDash && IsCanComboDash( pOwner, fHeightGap ) )
	{
		SetComboDash( pOwner, fHeightGap );
		return;
	}

	if( IsCanJumpCharge( pOwner, fHeightGap ) )
	{
		SetJumpChargingState( pOwner );
	}
	else if( IsCanDoubleJump( pOwner, fHeightGap, false ) && m_fDoubleJumpPower >= 0.0f )	// 2020-06-19
	{
		SetDoubleJump( pOwner );
	}

	if( m_bReserveJumpAttack )
	{
		ChangeToJumpAttack( pOwner );
	}
}

// 2020-06-19
bool ioGwonwangJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap, bool bCharge )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( ( !bCharge && !pOwner->IsJumpKeyPress() ) ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

// 2020-06-19
void ioGwonwangJump::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	m_bUseExtendJump = true;
	m_bUsedDoubleJump = true;
	m_dwChargingStartTime = 0;
	m_iCurJumpCnt++;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurGravityGapAmt( 0.0f );	// 2020-06-19
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
		kPacket << SSC_DOUBLE_JUMP;
		kPacket << m_fDoubleJumpPower;
		kPacket << m_fDoubleJumpForce;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// 2020-06-19
void ioGwonwangJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

void ioGwonwangJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	if( m_bUseHeighJump )
		return;

	if( m_ComboState == CST_NEW_JUMP )
	{
		switch( m_JumpState )
		{
		case PJS_DASH_ATTACK:
			if( m_dwJumpDashAddAttackReserveTime > 0 && m_dwJumpDashAddAttackReserveTime < dwCurTime && pOwner->IsAttackKey() && 
				m_iJumpDashAddAttackIndex < (int)m_vJumpDashAddAttack.size() )
			{
				m_iJumpDashAddAttackIndex = 0;
				m_JumpState = PJS_DASH_ADD_ATTACK;
				SetJumpDashAddAttack( pOwner, m_iJumpDashAddAttackIndex );
			}
			break;
		case PJS_DASH_ADD_ATTACK:
			if( m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
			{
				m_dwComboDashEndTime = 0;
			}
			else if( m_dwJumpDashAddAttackReserveTime > 0 && m_dwJumpDashAddAttackReserveTime < dwCurTime && pOwner->IsAttackKey() && 
					 m_iJumpDashAddAttackIndex < (int)m_vJumpDashAddAttack.size() )
			{
				m_JumpState = PJS_DASH_ADD_ATTACK;
				SetJumpDashAddAttack( pOwner, m_iJumpDashAddAttackIndex );
			}
			break;
		}

		return;
	}

	switch( m_JumpState )
	{
	case PJS_NONE:
		if( m_bReserveJumpAttack )
		{
			
			ChangeToJumpAttack( pOwner );
		}
		break;
	case PJS_CHARGING_JUMP_ATTACK:	// 점프 차지 공격
		{
			SetChargeJumpAttack( pOwner );
		}
		break;
	case PJS_NORMAL_ATTACK:
		{
			ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
			ioGwonwangItem* pGwonwangItem = ToGwonwangItem( pItem );

			int iMaxCnt = m_ComboAttackList.size();
			if( pGwonwangItem && pGwonwangItem->IsEnoughGaugeJumpCharge() && pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt + 1 ) ) // 2020-06-03
			{
				CheckAirJump( pOwner );	// 2020-06-23
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
				{
					SetChargingJumpState( pOwner );
				}
			}
			else
			{
				m_dwChargingStartTime = FRAMEGETTIME();				
				DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();

				if( m_bEnableJumpDash && IsCanComboDash( pOwner, fHeightGap ) )
				{
					if( pGwonwangItem )
					{
						SetComboDash( pOwner, fHeightGap );
						return;
					}
					else if( m_dwEnableComboTime_E > 0 && m_dwEnableComboTime_E + dwLooseTime < dwCurTime )
					{
						if( CheckAliceGun( pOwner ) )
						{
							SetComboDash( pOwner, fHeightGap );
						}
						return;
					}
				}

				CheckAirJump( pOwner );				

				if( m_bReserveJumpAttack )
				{
					if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
					{
						if( CheckAliceGun( pOwner ) )
						{
							SetComboJumpAttack( pOwner );
						}						
					}
				}
				float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
				if( IsCanJumpCharge( pOwner, fHeightGap ) )
				{
					SetJumpChargingState( pOwner );
					return;
				}
			}
		}
		break;
	case PJS_EXTEND_ATTACK:
		CheckAirJump( pOwner );
		break;
	case PJS_END:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime <= FRAMEGETTIME() )
		{
			pOwner->CheckExtendAttackEndJump();
		}
		break;
	}
}

void ioGwonwangJump::ProcessComboDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_ComboDashType != CDT_NO_GRAVITY )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ComboState )
	{
	case CST_DASH:
		if( m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
		{
			m_dwComboDashEndTime = 0;
			m_ComboState = CST_DASH_END;

			if( m_dwComboDashEndJumpPower > 0.0f )
			{
				pOwner->SetSKillEndJumpState( m_dwComboDashEndJumpPower );
			}
			else
			{
				int iAniID = pOwner->GetJumppingAnimationIdx();
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
				}
			}
		}
		break;
	case CST_DASH_END:
		break;
	}
}

void ioGwonwangJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		return;
	}

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

void ioGwonwangJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
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

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
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
	pOwner->RefreshFireTimeList( iAniID,
								 m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
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

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_ATTACK;
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

bool ioGwonwangJump::CheckAliceGun( ioBaseChar *pOwner )
{
	ioAliceObjectItem *pAliceGun = ToAliceObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAliceGun && pAliceGun->IsReleaseItem( pOwner ) )
	{
		ioAliceItem* pAlice = ToAliceItem( pOwner->GetWeapon() );
		if( pAlice )
		{
			return false;
		}
	}

	return true;
}

void ioGwonwangJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_ATTACK:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			if( !COMPARE( m_iCurComboCnt, 0, (int) m_ComboAttackList.size() ) )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			if( pOwner && !pOwner->IsActionStop() )
				pOwner->InitActionStopDelay( true );
			else
				pOwner->InitActionStopDelay( false );

			DWORD dwCurTime = FRAMEGETTIME();

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
			m_JumpState = PJS_NORMAL_ATTACK;
			pOwner->SetJumpAttackState();
		}
		break;
	case SSC_CANCEL_DASH:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			pOwner->SetMoveDirection( vDir );

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRotAndMoveDirChange( qtRotate );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			DWORD dwCurTime = FRAMEGETTIME();

			int iAniID = pGrp->GetAnimationIdx( m_CancelDashJumpAttack.m_AttackAnimation );
			float fTimeRate = m_CancelDashJumpAttack.m_fAttackAniRate;
			DWORD dwPreDelay = m_CancelDashJumpAttack.m_dwPreDelay;
			DWORD dwEndDelay = m_CancelDashJumpAttack.m_dwEndDelay;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pOwner->SetAutoTarget( ATT_JUMP );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		

			pOwner->RefreshFireTimeList( iAniID,
										 m_CancelDashJumpAttack.m_vWeaponInfoList,
										 FTT_JUMP_ATTACK,
										 fTimeRate,
										 dwPreDelay );

			pOwner->IncreaseWeaponIndexBase();

			m_dwComboDashStartTime = dwCurTime;
			m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetReservedSliding( m_CancelDashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

			DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

			DWORD dwTrackingTime = dwCurTime;
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

			m_bUseExtendJump = true;

			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );
			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
			pOwner->SetGravityAmt( 0.0f );
			pOwner->SetCurGravityGapAmt( 0.0f );	// 2020-06-19
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );

			m_ComboState = CST_DASH;
			pOwner->SetComboDashState();
			pOwner->ApplyAutoTargetInfo( rkPacket );

			if( m_bUseNewJumpTypeDash )
			{
				SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
			}
		}
		break;
	case SSC_DASH:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			pOwner->SetMoveDirection( vDir );

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRotAndMoveDirChange( qtRotate );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			DWORD dwCurTime = FRAMEGETTIME();

			int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
			float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
			DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
			DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;

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

			m_dwComboDashStartTime = dwCurTime;
			m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

			DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

			DWORD dwTrackingTime = dwCurTime;
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

			m_bUseExtendJump = true;

			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );
			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
			pOwner->SetGravityAmt( 0.0f );
			pOwner->SetCurGravityGapAmt( 0.0f );	// 2020-06-19
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );

			m_ComboState = CST_DASH;
			pOwner->SetComboDashState();
			pOwner->ApplyAutoTargetInfo( rkPacket );

			if( m_bUseNewJumpTypeDash )
			{
				SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
			}
		}
		break;

	case SSC_DOUBLE_JUMP:	// 2020-06-19
		{
			float fJumpPower, fJumpForce;
			D3DXQUATERNION qtRotate;

			rkPacket >> fJumpPower >> fJumpForce;
			rkPacket >> qtRotate;

			if( fJumpPower >= 0.0f )
			{
				m_bUsedDoubleJump = true;
				m_bUseExtendJump = true;

				pOwner->SetCurJumpPowerAmt( 0.0f );
				pOwner->SetForcePowerAmt( 0.0f );
				pOwner->SetGravityAmt( 0.0f );
				pOwner->SetCurGravityGapAmt( 0.0f );	// 2020-06-19
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

	case SS_CHARGING_JUMP:	// 점프차지
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChargingJumpState( pOwner );
		}
		break;
	case SS_CHARGING_JUMP_ATTACK:	// 점프 차지 공격
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );
			SetChargeJumpAttack( pOwner );
		}
		break;
	case SSC_EXTEND_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeToJumpAttack( pOwner );
			ChangeToPowerChargeJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case SSC_DASH_ADD_ATTACK:
		{
			int iAttackIndex;
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> iAttackIndex;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			SetJumpDashAddAttack( pOwner, iAttackIndex );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case SSC_CHANGE_ON:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChangeOnGriffinState( pOwner );
		}
		break;
	}
}

float ioGwonwangJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

	if( m_bUsedDoubleJump )
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
			return m_fExtendJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fExtendJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fExtendJumpSpeedRateBG;
		}
	}

	return m_fJumpSpeedRate;
}

bool ioGwonwangJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return true;
	return false;
}

DWORD ioGwonwangJump::GetChargingTime( ioBaseChar *pChar )
{
	if( m_bJumpChargeEnable )
		return m_dwJumpKeyChargingTime;

	return 0;
}

bool ioGwonwangJump::IsComboDashState()
{
	switch( m_ComboState )
	{
	case CST_DASH:
	case CST_DASH_END:
		return true;
	}

	return false;
}

bool ioGwonwangJump::IsCanComboDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	// 2020-06-17
	if( m_bUsedComboDash && m_bUsedCancelComboDash && ToGwonwangItem( pOwner->GetEquipedItem(ES_WEAPON) ) )
	{
		return false;
	}

	if( pOwner->IsHasCrown() ||	pOwner->IsCatchMode()	||	pOwner->IsPrisonerMode() ||	(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE)||	(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
	{
		return false;
	}

	if( pOwner->IsChatModeState( true ) )
	{
		return false;
	}

	if( pOwner->IsBuffLimitExtendJump() )
	{
		return false;
	}

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
	{
		return false;
	}

	if( m_ComboState != CST_NONE )
	{	
		return false;
	}

	if( !pOwner->IsCanJumpAttackEnableTime() )
	{
		return false;
	}

	if( !CheckAliceGun( pOwner ) )
	{
		return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
	{
		return false;
	}

	if( !pOwner->IsDirKeyDoubleClick() )
	{
		return false;
	}

	return true;
}

void ioGwonwangJump::SetComboDash( ioBaseChar *pOwner, float fHeightGap )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	AttackAttribute	cancelDashJumpAttack = m_DashJumpAttack;
	JumpState jumpstate = pOwner->GetJumpState();

	// 점프 중 대시 했을 때 중복 처리 2020-06-17
	if( m_bUsedComboDash && (JS_JUMPPING == jumpstate) )
	{
		return;
	}

	// 점프 중 캔슬대시 했을 때 중복 처리 2020-06-17
	if( m_bUsedCancelComboDash && (JS_JUMP_ATTACK == jumpstate) )
	{
		return;
	}

	if( JS_JUMPPING == jumpstate )
	{
		m_bUsedComboDash = true;		
	}

	if( JS_JUMP_ATTACK == jumpstate )
	{
		cancelDashJumpAttack	= m_CancelDashJumpAttack;
		m_bUsedCancelComboDash	= true;	// 2020-06-17
	}

	int iAniID = pGrp->GetAnimationIdx( cancelDashJumpAttack.m_AttackAnimation );
	float fTimeRate = cancelDashJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = cancelDashJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = cancelDashJumpAttack.m_dwEndDelay;

	m_bUseExtendJump = true;	
	m_bUsedComboDashEnd = false;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurGravityGapAmt( 0.0f );	// 2020-06-19

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pOwner->ChangeDirectionByInputDir( false );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
								 cancelDashJumpAttack.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwComboDashStartTime = dwCurTime;
	m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( cancelDashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_ComboState = CST_DASH;
	m_JumpState = PJS_DASH_ATTACK;
	m_iJumpDashAddAttackIndex = 0;

	pOwner->SetComboDashState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		if( JS_JUMP_ATTACK == jumpstate )
		{
			kPacket << SSC_CANCEL_DASH;
		}
		else
		{
			kPacket << SSC_DASH;
		}
		kPacket << vMoveDir;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( m_bUseNewJumpTypeDash )
	{
		SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
		return;
	}
}

void ioGwonwangJump::SetComboDashLand( ioBaseChar *pOwner, float fHeightGap )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	int iAniID = pGrp->GetAnimationIdx( m_ComboDashLandAni );
	float fTimeRate = m_fComboDashLandAniRate;

	m_bUseExtendJump = true;
	m_bUsedComboDash = true;
	m_bUsedCancelComboDash	=  false;	// 2020-06-17
	m_bUsedComboDashEnd = true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurGravityGapAmt( 0.0f );	// 2020-06-19
	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwComboDashStartTime = dwCurTime;
	m_dwComboDashEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_ComboState = CST_DASH;
	m_JumpState = PJS_DASH_ATTACK;
	m_iJumpDashAddAttackIndex = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_DASH_END;
		kPacket << vMoveDir;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( m_bUseNewJumpTypeDash )
	{
		SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
		return;
	}
}

float ioGwonwangJump::GetCurGravityAmt()
{
	if( m_ComboState == CST_DASH || m_ComboState == CST_NEW_JUMP )
	{
		if( m_ComboDashType == CDT_GRAVITY )
			return m_fComboDashGravityAmt;
		else if( m_ComboDashType == CDT_NO_GRAVITY )
			return -FLOAT1;
	}

	return 0.0f;
}

bool ioGwonwangJump::IsEnableJumpLandEndDash()
{
	if( m_ComboState == CST_DASH || m_ComboState == CST_DASH_END )
		return true;

	return false;
}

bool ioGwonwangJump::IsComboDashLandEnable()
{
	if( m_bComboDashLandEnable && !m_bUsedComboDashEnd )
		return true;

	return false;
}

bool ioGwonwangJump::IsComboDashLanded()
{
	if( m_bComboDashLandEnable && m_bUsedComboDashEnd )
		return true;

	return false;
}

bool ioGwonwangJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return m_bEnableJumpDash;
}

bool ioGwonwangJump::CheckJumpCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;
	if( !m_bJumpChargeEnable )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsJumpKeyDown() && !pOwner->IsHasCrown() && pOwner->IsEnableAzazelSpecialAttack() )
	{
		if( m_dwChargingStartTime + m_dwJumpKeyChargingTime < dwCurTime )
		{
			m_dwChargingStartTime = 0;
			return true;
		}
	}
	else
	{
		// 2020-06-18
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( IsCanDoubleJump( pOwner, fHeightGap, true ) && m_fDoubleJumpPower > 0.0f )
		{
			SetDoubleJump( pOwner );
			return true;
		}
		else
		{
			m_JumpState = PJS_NONE;
			m_dwChargingStartTime = 0;
		}
	}

	return false;
}

void ioGwonwangJump::SetJumpChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_bUseExtendJump = true;
	m_dwChargingStartTime = dwCurTime;
}

bool ioGwonwangJump::IsCanJumpCharge( ioBaseChar *pOwner, float fHeightGap )
{
	if( !m_bJumpChargeEnable )
		return false;
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

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) 
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

void ioGwonwangJump::SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime )
{
	if( !pOwner ) return;

	m_bUseExtendJump = false;
	m_ComboState = CST_NEW_JUMP;
	m_dwJumpDashAddAttackReserveTime = dwReserveTime;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, m_fComboDashGravityAmt );

	ioAliceObjectItem *pAliceGun = ToAliceObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAliceGun )
	{		
		pAliceGun->NotifyDashJumpAttack( pOwner, 0 );
	}
}

void ioGwonwangJump::SetJumpDashAddAttack( ioBaseChar *pOwner, int iAttackIndex )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	if( (int)m_vJumpDashAddAttack.size() <= iAttackIndex )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_vJumpDashAddAttack[iAttackIndex].m_AttackAnimation );
	float fTimeRate = m_vJumpDashAddAttack[iAttackIndex].m_fAttackAniRate;
	DWORD dwPreDelay = m_vJumpDashAddAttack[iAttackIndex].m_dwPreDelay;
	DWORD dwEndDelay = m_vJumpDashAddAttack[iAttackIndex].m_dwEndDelay;

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pOwner->ChangeDirectionByInputDir( false );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_vJumpDashAddAttack[iAttackIndex].m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_vJumpDashAddAttack[iAttackIndex].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwJumpDashAddAttackReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

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
		kPacket << SSC_DASH_ADD_ATTACK;
		kPacket << iAttackIndex;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetNewJumpAttackStateInfo( m_dwComboDashEndTime, m_dwJumpDashAddAttackReserveTime, m_fComboDashGravityAmt );
	m_iJumpDashAddAttackIndex++;
}

void ioGwonwangJump::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	m_JumpState = PJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime = FRAMEGETTIME();	
}

void ioGwonwangJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump		= false;
	m_bReserveJumpAttack	= false;
	m_bUsedComboDash		= false;
	m_bUsedCancelComboDash	= false;
	m_JumpState				= PJS_NONE;
	
	pOwner->SetForcePowerAmt( 0.0f );
    pOwner->SetCurJumpPowerAmt( 0.0f );
}

void ioGwonwangJump::ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner )
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
	
	m_JumpState = PJS_EXTEND_ATTACK;
	m_bUsedChargeAttack = true;
	m_dwChargingStartTime = 0;

	// For AirJumpPower
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwChargeAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir.y = 0.0f;
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
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
		kPacket << SSC_EXTEND_ATTACK;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGwonwangJump::SetAutomatonState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioAutomatonItem *pAutomaton = ToAutomatonItem( pItem );
	if( !pAutomaton )
		return;

	pAutomaton->SetJumpSpecialState( pOwner, true );
}

void ioGwonwangJump::SetChangeOnGriffinState( ioBaseChar *pOwner )
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
						kPacket << SSC_CHANGE_ON;
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
		kPacket << SSC_CHANGE_ON;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGwonwangJump::SetHanzoState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioHanzoItem* pHanzo = ToHanzoItem( pItem );
	if( !pHanzo )
		return;

	pHanzo->ChangeToJumpGatheringState( pOwner );
}

void ioGwonwangJump::SetMidnightState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;

	ioAttackableItem* pItem = pOwner->GetPriorityItem( SPT_ATTACK );
	ioMidnightItem* pMidnight = ToMidnightItem( pItem );
	if( !pMidnight )
		return;

	pMidnight->ChangeToJumpDashState( pOwner, pOwner->CheckCurDirKey() );
}

// 점프 차지
void ioGwonwangJump::SetChargingJumpState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	pOwner->ClearAttackFireTimeAndSkill();

	DWORD dwCurTime = FRAMEGETTIME();
	ioHashString szCurChargingAni;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_stChargingAni );
	float fTimeRate = (float)m_dwJumpChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	// 2020-06-23
	//m_dwChargingStartTime = dwCurTime;
	//bool bIsNeedProcess = pOwner->IsNeedProcess();
	//if( bIsNeedProcess )
	//{
		
		m_JumpState = PJS_CHARGING_JUMP_ATTACK;
	//}


	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SS_CHARGING_JUMP;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// 점프 차지 공격
void ioGwonwangJump::SetChargeJumpAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
	{
		return;
	}

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

	ioWeaponItem *pWeapon = pOwner->GetWeapon();
	ioGwonwangItem *pGwonwangItem = ToGwonwangItem( pWeapon );
	if( pGwonwangItem && !pGwonwangItem->IsBUsedWereWolf() )
	{
		pGwonwangItem->SetJumpLandAttackAttribute( m_ItemLandAttackInfo );
	}
	else
	{
		ioObjectBankruptcyItem* pObjectBankruptcyItem = ToObjectBankruptcyItem( pOwner->GetObject() );
		if( pObjectBankruptcyItem )
		{
			pObjectBankruptcyItem->SetJumpLandAttackAttribute( m_ItemLandAttackInfo );
		}
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )	{	pOwner->InitActionStopDelay( true );	}
	else	{	pOwner->InitActionStopDelay( false );	}

	int iAniID = pGrp->GetAnimationIdx( m_JumpChargeAttribute.m_AttackAnimation );
	float fTimeRate = m_JumpChargeAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_JumpChargeAttribute.m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_JumpChargeAttribute.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	m_JumpState = PJS_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SS_CHARGING_JUMP_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
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

bool ioGwonwangJump::IsChargeAttacked()
{
	return m_bUsedChargeAttack;
}

bool ioGwonwangJump::IsChargeEnd()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwChargeAttackEndTime < dwCurTime )
		return true;

	m_fCurCharMoveSpeed = 0.0f;
	return false;
}

bool ioGwonwangJump::IsCanMoveState()
{
	return true;
}

void ioGwonwangJump::ResetJumpState()
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_bUsedChargeAttack = false;
	m_dwChargeAttackEndTime = 0;

	m_ComboState = CST_NONE;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;
	m_iJumpDashAddAttackIndex = 0;

	m_dwChargingStartTime = 0;

	m_dwMotionEndTime = 0;
}

bool ioGwonwangJump::IsUsedHighJump()
{
	if( m_bEnableHighJump && m_bUseHeighJump )
		return true;

	return false;
}

bool ioGwonwangJump::IsEnableLandAttack()
{
	if( m_bUseHeighJump )
		return true;

	if( m_bUseNormalJumpAttack )
		return true;

	return false;
}

bool ioGwonwangJump::IsJumpAttackReserve()
{
	return false;
}