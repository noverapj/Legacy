#include "StdAfx.h"
#include "ioSkyWalkerJump.h"

#include "ioSkyWalkerItem.h"
#include "ioZoneEffectWeapon.h"

ioSkyWalkerJump::ioSkyWalkerJump(void)
{
	ClearData();
}

ioSkyWalkerJump::ioSkyWalkerJump( const ioSkyWalkerJump &rhs ) :
ioExtendJump( rhs ),
	//jump attack
	m_ComboAttackList( rhs.m_ComboAttackList ),
	//double jump
	m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
	m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
	m_iMaxChargeAttAfterJumpCnt( rhs.m_iMaxChargeAttAfterJumpCnt ),
	m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
	m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
	m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
	m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
	m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
	m_fJumpSpeedRateFR( rhs.m_fJumpSpeedRateFR ),
	m_fJumpSpeedRateSI( rhs.m_fJumpSpeedRateSI ),
	m_fJumpSpeedRateBG( rhs.m_fJumpSpeedRateBG ),
	m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
	m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
	m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
	//charge
	m_dwChargingTime( rhs.m_dwChargingTime ),
	m_AttackKeyChargingAniList( rhs.m_AttackKeyChargingAniList ),
	m_fJumpChargeAttackUseGauge( rhs.m_fJumpChargeAttackUseGauge ),
	//Dash
	m_vJumpStraightDashAttack( rhs.m_vJumpStraightDashAttack ),
	m_vJumpTopDashAttack( rhs.m_vJumpTopDashAttack ),
	m_fJumpAttackTopDashAngle( rhs.m_fJumpAttackTopDashAngle ),
	m_fJumpAttackDiagonalDashAngle( rhs.m_fJumpAttackDiagonalDashAngle ),
	m_vJumpTopDiagonalDashAttack( rhs.m_vJumpTopDiagonalDashAttack ),
	m_vJumpBottomDashAttack( rhs.m_vJumpBottomDashAttack ),
	m_fJumpAttackBottomDashAngle( rhs.m_fJumpAttackBottomDashAngle ),
	m_vJumpBottomDiagonalDashAttack( rhs.m_vJumpBottomDiagonalDashAttack ),
	m_fJumpDashEndJumpPowerRate( rhs.m_fJumpDashEndJumpPowerRate ),
	m_fJumpDashUseGauge( rhs.m_fJumpDashUseGauge ),
	m_fInactiveEndJumpDistance( rhs.m_fInactiveEndJumpDistance ),
	m_iSameJumpDashLimitCount( rhs.m_iSameJumpDashLimitCount ),
	//D~
	m_szJumpChargeAttackAni( rhs.m_szJumpChargeAttackAni ),
	m_fJumpChargeAttackAniRate( rhs.m_fJumpChargeAttackAniRate ),
	m_fJumpChargeAttackAngle( rhs.m_fJumpChargeAttackAngle ),
	m_fMaxJumpChargeAttackRange( rhs.m_fMaxJumpChargeAttackRange ),
	m_iJumpChargeWeaponType( rhs.m_iJumpChargeWeaponType ),
	m_fJumpChargeAttackEndJumpPowerRate( rhs.m_fJumpChargeAttackEndJumpPowerRate ),
	m_fJumpChargeAttackSpeed( rhs.m_fJumpChargeAttackSpeed ),
	m_dwJumpChargeEndDelay( rhs.m_dwJumpChargeEndDelay )
{
	ClearData();
}

ioSkyWalkerJump::~ioSkyWalkerJump(void)
{
}

ioExtendJump* ioSkyWalkerJump::Clone()
{
	return new ioSkyWalkerJump( *this );
}

JumpType ioSkyWalkerJump::GetType() const
{
	return JT_SKY_WALKER_JUMP;
}

void ioSkyWalkerJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//double jump
	m_fDoubleJumpPower = rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce = rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_fDoubleJumpEnableHeight = rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );
	m_dwDoubleJumpEnableTime = rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_vDoubleJumpAniList.clear();
	m_vDoubleJumpAniList.reserve( 4 );
	rkLoader.LoadString_e( "double_jump_animation_front", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( szBuf );
	rkLoader.LoadString_e( "double_jump_animation_right", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( szBuf );
	rkLoader.LoadString_e( "double_jump_animation_back", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( szBuf );
	rkLoader.LoadString_e( "double_jump_animation_left", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( szBuf );
	m_fDoubleJumpAniRate = rkLoader.LoadFloat_e( "double_jump_ani_rate", FLOAT1 );

	m_fDoubleJumpSpeedRateFR = rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI = rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG = rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

	m_fJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	m_iMaxJumpCnt = max( 1, rkLoader.LoadInt_e( "max_jump_cnt", 1 ) );
	m_iMaxChargeAttAfterJumpCnt = max( 1, rkLoader.LoadInt_e( "max_charge_attack_after_jump_cnt", 1 ) );

	//AttackKey charge
	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );
	int iMaxChargeAni = rkLoader.LoadInt_e( "charging_jump_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "charging_jump_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackKeyChargingAniList.push_back( szBuf );
	}

	m_fJumpChargeAttackUseGauge = rkLoader.LoadFloat_e( "jump_charge_use_gauage", FLOAT1 );

	//jump attack
	int iMaxCombo = rkLoader.LoadInt_e( "jump_max_combo", 0 );
	m_ComboAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboAttackList.push_back( kAttribute );
	}
	
	//Dash
	LoadAttackAttribute_e( "jump_straight_dash", m_vJumpStraightDashAttack, rkLoader );
	LoadAttackAttribute_e( "jump_top_diagonal_dash", m_vJumpTopDiagonalDashAttack, rkLoader );
	LoadAttackAttribute_e( "jump_top_dash", m_vJumpTopDashAttack, rkLoader );
	m_fJumpAttackTopDashAngle	= rkLoader.LoadFloat_e( "jump_attack_top_dash_angle", 0.0f );
	m_fJumpAttackDiagonalDashAngle = rkLoader.LoadFloat_e( "jump_attack_diagonal_dash_angle", 0.0f );
	LoadAttackAttribute_e( "jump_bottom_diagonal_dash", m_vJumpBottomDiagonalDashAttack, rkLoader );
	LoadAttackAttribute_e( "jump_bottom_dash", m_vJumpBottomDashAttack, rkLoader );
	m_fJumpAttackBottomDashAngle	= rkLoader.LoadFloat_e( "jump_attack_bottom_dash_angle", 0.0f );
	m_fJumpDashEndJumpPowerRate = rkLoader.LoadFloat_e( "jump_dash_end_jump_power_rate", 0.0f );
	m_fInactiveEndJumpDistance = rkLoader.LoadFloat_e( "inactive_end_jump_distance", 0.0f );
	m_iSameJumpDashLimitCount = rkLoader.LoadInt_e( "same_jump_dash_limit_cnt", 0 );

	m_fJumpDashUseGauge = rkLoader.LoadFloat_e( "jump_dash_use_gauage", 0.0f );

	//D~
	rkLoader.LoadString_e( "jump_charge_attack_ani", "", szBuf, MAX_PATH );
	m_szJumpChargeAttackAni = szBuf;
	m_fJumpChargeAttackAniRate = rkLoader.LoadFloat_e( "jump_charge_attack_ani_rate", 1.0f );
	m_fJumpChargeAttackAngle = rkLoader.LoadFloat_e( "jump_charge_attack_angle", 0.0f );
	m_fMaxJumpChargeAttackRange = rkLoader.LoadFloat_e( "jump_charge_attack_max_range", 0.0f );
	m_iJumpChargeWeaponType = rkLoader.LoadInt_e( "jump_charge_attack_weapon_type", 0.0f );
	m_fJumpChargeAttackEndJumpPowerRate = rkLoader.LoadFloat_e( "jump_charge_attack_end_jump_power_rate", 0.0f );
	m_fJumpChargeAttackSpeed = rkLoader.LoadFloat_e( "jump_charge_attack_speed", 0.0f );
	m_dwJumpChargeEndDelay = rkLoader.LoadInt_e( "jump_charge_attack_end_delay", 0 );
}

void ioSkyWalkerJump::InitJump()
{
	m_bUsedDoubleJump = false;
	m_iCurJumpDashCount = 0;
	m_iCurJumpCnt = 1;
}

void ioSkyWalkerJump::ClearData()
{
	m_SkyWalkerJumpState = SWJS_NONE;
	m_bReserveJumpAttack = false;

	m_iCurJumpCnt = 1;
	m_bUsedDoubleJump = false;
	m_dwChargingStartTime = 0;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;
	m_dwEnableChargeAttack = 0;

	m_dwMotionEndTime = 0;
	m_iCurJumpDashCount = 0;
	m_dwAttackFireTime = 0;
	m_dwJumpChargeEndTime = 0;
	m_dwJumpChargeEndDelayEndTime = 0;
	m_szTargetName.Clear();
	m_fCurJumpChargeAttackRange	= 0.0f;
	m_vJumpChargeAttackMoveDir = ioMath::VEC3_ZERO;

	m_ePreInputDirKey = ioUserKeyInput::DKI_NONE;
	m_eCurInputDirKey = ioUserKeyInput::DKI_NONE;
	m_iSameJumpDashDirCount = 0;

	m_bEnableChargeAttAfterSeveralJump = false;



	// 방향에 따른 대시 방향 타입을 정하기 위한 방향 정보 셋팅
	m_vDirInfo.push_back( DirectionInfo( DDT_FIRST_TYPE, D3DXVECTOR3( -1.0f, 0.0f, 0.0f ) ) );		// 왼
	m_vDirInfo.push_back( DirectionInfo( DDT_FIRST_TYPE, D3DXVECTOR3( 1.0f, 0.0f, 0.0f ) ) );		// 오른
	m_vDirInfo.push_back( DirectionInfo( DDT_FIRST_TYPE, D3DXVECTOR3( -0.5f, 0.0f, 0.5f ) ) );		// 왼 위
	m_vDirInfo.push_back( DirectionInfo( DDT_FIRST_TYPE, D3DXVECTOR3( 0.5f, 0.0f, 0.5f ) ) );		// 오른 위

	m_vDirInfo.push_back( DirectionInfo( DDT_SECOND_TYPE, D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) ) );		// 위
	m_vDirInfo.push_back( DirectionInfo( DDT_SECOND_TYPE, D3DXVECTOR3( 0.0f, 0.0f, -1.0f ) ) );		// 아래
	m_vDirInfo.push_back( DirectionInfo( DDT_SECOND_TYPE, D3DXVECTOR3( -0.5f, 0.0f, -0.5f ) ) );	// 왼 아래
	m_vDirInfo.push_back( DirectionInfo( DDT_SECOND_TYPE, D3DXVECTOR3( 0.5f, 0.0f, -0.5f ) ) );		// 오른 아래
}

void ioSkyWalkerJump::SetJumpState()
{
	m_bUseExtendJump = false;

	m_bEnableChargeAttAfterSeveralJump = false;

	m_ePreInputDirKey = ioUserKeyInput::DKI_NONE;
	m_eCurInputDirKey = ioUserKeyInput::DKI_NONE;
}

void ioSkyWalkerJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_SkyWalkerJumpState = SWJS_NONE;
	pOwner->SetUsedFlashJump( false );

	m_bUseExtendJump = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;
	m_dwEnableChargeAttack = 0;

	m_bReserveJumpAttack = false;

	// charge
	m_dwChargingStartTime = 0;

	m_dwReserveEnableTime = 0;
	m_dwMotionEndTime = 0;
	m_dwAttackFireTime = 0;
	m_dwJumpChargeEndTime = 0;
	m_dwJumpChargeEndDelayEndTime = 0;
	m_fCurJumpChargeAttackRange	= 0.0f;
	m_vJumpChargeAttackMoveDir = ioMath::VEC3_ZERO;
}

float ioSkyWalkerJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

	if( m_SkyWalkerJumpState == SWJS_DOUBLE_JUMP )
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

void ioSkyWalkerJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_SkyWalkerJumpState )
	{
	case SWJS_NONE:
	case SWJS_NORMAL_ATTACK:
	case SWJS_DOUBLE_JUMP:
	case SWJS_DASH:
		break;
	default:
		return;
	}

	if( m_bReserveJumpAttack )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;
	int iMaxCnt = m_ComboAttackList.size();

	if( COMPARE( dwCurTime, m_dwEnableChargeAttack, m_dwEnableComboTime_E+dwLooseTime ) && m_iCurComboCnt == iMaxCnt )
		m_bReserveJumpAttack = true;

	
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		return;
	}

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_iCurComboCnt > 0 && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
		return;

	if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
		return;
	
	m_bReserveJumpAttack = true;
}

void ioSkyWalkerJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
		return;

	pOwner->CheckExtraAniJump();

	// combo
	if( m_bReserveJumpAttack )
	{
		SetChargingState( pOwner );
		return;
	}
	// Double Jump
	else if( m_fDoubleJumpPower >= 0.0f && IsCanDoubleJump( pOwner, fHeightGap ) )
	{
		SetDoubleJump( pOwner );
		return;
	}
	// jump Dash check
	else if( IsCanJumpDash( pOwner, fHeightGap ) )
	{
		SetJumpDashState( pOwner );
		return;
	}

	//Check State
	switch( m_SkyWalkerJumpState )
	{
	case SWJS_DASH:
		ProcessJumpDashState( pOwner );
		break;
	}
}

bool ioSkyWalkerJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( !pOwner->IsJumpKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() )
		return false;

	if( pOwner->GetUsedBuffJump() != BJUT_NONE ) 
		return false;
	if( pOwner->GetUsedBuffFlyJump() != BJUT_NONE ) 
		return false;
	if( pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	switch( m_SkyWalkerJumpState )
	{
	case SWJS_NONE:
	case SWJS_DOUBLE_JUMP:
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

void ioSkyWalkerJump::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_SkyWalkerJumpState = SWJS_DOUBLE_JUMP;

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
		kPacket << ST_DOUBLE_JUMP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSkyWalkerJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

void ioSkyWalkerJump::SetChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_AttackKeyChargingAniList.empty() )
		return;

	pOwner->ClearSylphid();

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	ioHashString szCurChargingAni;
	int iChargingCnt = m_AttackKeyChargingAniList.size();
	if( COMPARE( m_iCurComboCnt, 0, iChargingCnt ) )
		szCurChargingAni = m_AttackKeyChargingAniList[m_iCurComboCnt];
	else
		szCurChargingAni = m_AttackKeyChargingAniList[0];
	
	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );
	float fTimeRate = (float)m_dwChargingTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_SkyWalkerJumpState = SWJS_CHARGE;
	m_dwChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << ST_CHARGE;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSkyWalkerJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	switch( m_SkyWalkerJumpState )
	{
	case SWJS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case SWJS_NORMAL_ATTACK:
		ProcessComboJumpAttack( pOwner, fHeightGap );
		break;
	case SWJS_DASH:
		ProcessJumpDashState( pOwner );
		break;
	case SWJS_CHARGE_ATTACK:
		ProcessChargeAttackState( pOwner );
		break;
	case SWJS_CHARGE_ATTACK_END_DELAY:
		ProcessChargeAttackEndDelayState( pOwner );
		break;
	}
}

void ioSkyWalkerJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && IsCanChargeAttackState( pOwner ) )
	{
		if( m_dwChargingStartTime > 0 && m_dwChargingStartTime + m_dwChargingTime < dwCurTime )
		{
			SetChargeAttackState( pOwner );
			m_dwChargingStartTime = 0;
			m_dwEnableChargeAttack = 0;
		}
	}
	else
		SetComboJumpAttack( pOwner );
}

void ioSkyWalkerJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		if( m_iCurComboCnt == iMaxCnt )
		{
			m_SkyWalkerJumpState = SWJS_NORMAL_ATTACK;
			m_bReserveJumpAttack = false;
		}
		return;
	}

	pOwner->ClearSylphid();

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkyWalkerJumpState = SWJS_NORMAL_ATTACK;

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
	pOwner->RefreshFireTimeList( iAniID, m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwEnableChargeAttack = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "input_charge_s" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << ST_NORMAL_ATTACK;
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

void ioSkyWalkerJump::ProcessComboJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	CheckAirJump( pOwner );

	DWORD dwLooseTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( dwCurTime >= m_dwEnableComboTime_S + dwLooseTime )
	{
		if ( m_bReserveJumpAttack )
		{	
			SetChargingState( pOwner );
			return;
		}
		////Jump Dash 체크
		else if ( IsCanJumpDash( pOwner, fHeightGap ) )
		{
			SetJumpDashState( pOwner );
			return;
		}
	}

	if( FRAMEGETTIME() >= m_dwEnableChargeAttack  + dwLooseTime )
	{
		if ( m_bReserveJumpAttack )
		{
			SetChargingState( pOwner );
			return;
		}
	}
}

float ioSkyWalkerJump::GetCurGravityAmt( int iJumpState )
{
	switch( m_SkyWalkerJumpState )
	{
	case SWJS_DASH:
	case SWJS_CHARGE_ATTACK:
	case SWJS_CHARGE_ATTACK_END_DELAY:
		return -FLOAT1;
	}
	return 0.0f;
}

bool ioSkyWalkerJump::IsCanMoveState( int iJumpState )
{
	if ( m_SkyWalkerJumpState == SWJS_CHARGE_ATTACK )
		return false;

	return true;
}

void ioSkyWalkerJump::ClearState( ioBaseChar *pOwner, int eNewState )
{
	if ( !pOwner )
		return;
	if ( pOwner->GetState() == CS_URIEL_STATE && eNewState == CS_JUMP )
	{
		m_bUsedDoubleJump = false;
		m_iCurJumpDashCount = 0;
		m_iCurJumpCnt = 1;
	}
	else if ( eNewState != CS_JUMP && eNewState != CS_USING_SKILL )
	{
		m_bUsedDoubleJump = false;
		m_iCurJumpDashCount = 0;
		m_iCurJumpCnt = 1;
	}
}

bool ioSkyWalkerJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	switch( m_SkyWalkerJumpState )
	{
	case SWJS_NONE:
	case SWJS_DOUBLE_JUMP:
	case SWJS_NORMAL_ATTACK:
	case SWJS_CHARGE_ATTACK:
	case SWJS_CHARGE:
		return true;
	default:
		return false;
	}
}

bool ioSkyWalkerJump::IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;
	
	ioSkyWalkerItem *pSkyWalkerItem = ToSkyWalkerItem( pOwner->GetWeapon() );
	if ( !pSkyWalkerItem )
		return false;

	if( !pSkyWalkerItem->IsEnableGauge( m_fJumpDashUseGauge ) )
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

	switch( m_SkyWalkerJumpState )
	{
	case SWJS_NONE:
	case SWJS_DOUBLE_JUMP:
	case SWJS_NORMAL_ATTACK:
	case SWJS_CHARGE_ATTACK:
		break;
	default:
		return false;
	}

	return true;
}

void ioSkyWalkerJump::SetJumpDashState( ioBaseChar *pOwner, bool bSend, D3DXVECTOR3 vRecvCharDir )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_eCurInputDirKey = pOwner->CheckCurDirKey();
	if( bSend )
	{
		// 이전과 같은 방향 키 연속 입력 시 return되어 나감
		ioUserKeyInput::DirKeyInput eCurrDir = pOwner->CheckCurDirKey();
		if( m_ePreInputDirKey == eCurrDir )
		{
			if( m_iSameJumpDashDirCount < m_iSameJumpDashLimitCount-1 )
				m_iSameJumpDashDirCount++;
			else
				return;
		}
		else
			m_iSameJumpDashDirCount = 0;

		ioSkyWalkerItem *pSkyWalkerItem = ToSkyWalkerItem( pOwner->GetWeapon() );
		if( pSkyWalkerItem )
			pSkyWalkerItem->DecreaseGauge( m_fJumpDashUseGauge );
	}
	
	pOwner->ClearSylphid();

	m_SkyWalkerJumpState = SWJS_DASH;
	m_bUseExtendJump = true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	D3DXVECTOR3 vCharDir = ioMath::VEC3_ZERO;
	if( bSend )
		vCharDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	else
		vCharDir = vRecvCharDir;
	
	DashDirType eAttDirType = CheckAttackDirByAngle( pOwner, vCharDir );

	float fDashAngle = 0.0;
	AttackAttribute AttAttr = m_vJumpStraightDashAttack;
	if( eAttDirType == DDT_FIRST_TYPE )
	{
		if ( m_eCurInputDirKey == ioUserKeyInput::DKI_LEFT )
		{
			pOwner->SetTargetRotToDirKey( ioUserKeyInput::DKI_LEFT );
			pOwner->SetTargetRotToDir( D3DXVECTOR3( -1.0f, 0.0f, 0.0f ), true );
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHT )
		{
			pOwner->SetTargetRotToDirKey( ioUserKeyInput::DKI_RIGHT );
			pOwner->SetTargetRotToDir( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ), true );
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_UP )
		{
			AttAttr = m_vJumpTopDashAttack;
			fDashAngle = m_fJumpAttackTopDashAngle;
		}
		else if( m_eCurInputDirKey == ioUserKeyInput::DKI_DOWN )
		{
			AttAttr = m_vJumpBottomDashAttack;
			fDashAngle = m_fJumpAttackBottomDashAngle;
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_LEFTUP )
		{
			AttAttr = m_vJumpTopDiagonalDashAttack;
			fDashAngle = m_fJumpAttackDiagonalDashAngle;
			pOwner->SetTargetRotToDir( D3DXVECTOR3( -1.0f, 0.0f, 0.0f ), true );
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHTUP )
		{
			AttAttr = m_vJumpTopDiagonalDashAttack;
			fDashAngle = m_fJumpAttackDiagonalDashAngle;
			pOwner->SetTargetRotToDir( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ), true );
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_LEFTDOWN )
		{
			AttAttr = m_vJumpBottomDiagonalDashAttack;
			fDashAngle = -m_fJumpAttackDiagonalDashAngle;
			pOwner->SetTargetRotToDir( D3DXVECTOR3( -1.0f, 0.0f, 0.0f ), true );
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHTDOWN )
		{
			AttAttr = m_vJumpBottomDiagonalDashAttack;
			fDashAngle = -m_fJumpAttackDiagonalDashAngle;
			pOwner->SetTargetRotToDir( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ), true );
		}
	}
	else if( eAttDirType == DDT_SECOND_TYPE )
	{
		if ( m_eCurInputDirKey == ioUserKeyInput::DKI_LEFT )
		{
			pOwner->SetTargetRotToDirKey( ioUserKeyInput::DKI_DOWN );
			pOwner->SetTargetRotToDir( D3DXVECTOR3( 0.0f, 0.0f, -1.0f ), true );
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHT )
		{
			pOwner->SetTargetRotToDirKey( ioUserKeyInput::DKI_UP );
			pOwner->SetTargetRotToDir( D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), true );
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_UP )
		{
			AttAttr = m_vJumpTopDashAttack;
			fDashAngle = m_fJumpAttackTopDashAngle;
		}
		else if( m_eCurInputDirKey == ioUserKeyInput::DKI_DOWN )
		{
			AttAttr = m_vJumpBottomDashAttack;
			fDashAngle = m_fJumpAttackBottomDashAngle;
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_LEFTUP )
		{
			AttAttr = m_vJumpTopDiagonalDashAttack;
			fDashAngle = m_fJumpAttackDiagonalDashAngle;
			pOwner->SetTargetRotToDir( D3DXVECTOR3( 0.0f, 0.0f, -1.0f ), true );
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHTUP )
		{
			AttAttr = m_vJumpTopDiagonalDashAttack;
			fDashAngle = m_fJumpAttackDiagonalDashAngle;
			pOwner->SetTargetRotToDir( D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), true );
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_LEFTDOWN )
		{
			AttAttr = m_vJumpBottomDiagonalDashAttack;
			fDashAngle = -m_fJumpAttackDiagonalDashAngle;
			pOwner->SetTargetRotToDir( D3DXVECTOR3( 0.0f, 0.0f, -1.0f ), true );
		}
		else if ( m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHTDOWN )
		{
			AttAttr = m_vJumpBottomDiagonalDashAttack;
			fDashAngle = -m_fJumpAttackDiagonalDashAngle;
			pOwner->SetTargetRotToDir( D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), true );
		}
	}

	int iAniID = pGrp->GetAnimationIdx( AttAttr.m_AttackAnimation );
	float fTimeRate = AttAttr.m_fAttackAniRate;
	DWORD dwPreDelay = AttAttr.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );	
	pOwner->RefreshFireTimeList( iAniID,
		AttAttr.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += dwDuration;

	m_dwEnableComboTime_S = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwEnableComboTime_S > 0 )
		m_dwEnableComboTime_S += dwCurTime;

	if( m_eCurInputDirKey == ioUserKeyInput::DKI_UP || m_eCurInputDirKey == ioUserKeyInput::DKI_DOWN
		|| m_eCurInputDirKey == ioUserKeyInput::DKI_LEFTUP || m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHTUP || m_eCurInputDirKey == ioUserKeyInput::DKI_LEFTDOWN || m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHTDOWN )
	{
		if( pOwner && !pOwner->IsActionStop() )
			pOwner->InitActionStopDelay( true );
		else
			pOwner->InitActionStopDelay( false );

		DWORD dwSlidingTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
		D3DXVECTOR3 vMoveDirrr;
		if ( !AttAttr.m_vForceInfoList.empty() && dwSlidingTime != 0 )
		{
			D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vTargetDir, &vTargetDir );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
			D3DXVec3Normalize( &vAxis, &vAxis );

			D3DXQUATERNION qtNewRot;
			float fCurAngle = ioMath::ArrangeHead( fDashAngle );
			D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

			D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
			D3DXVec3Normalize( &vMoveDir, &vMoveDir );
			vMoveDirrr = vMoveDir;
			ForceInfo Info = AttAttr.m_vForceInfoList.front();
			dwSlidingTime *= fTimeRate;
			dwSlidingTime *= dwPreDelay;

			if( m_eCurInputDirKey == ioUserKeyInput::DKI_LEFTUP || m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHTUP || m_eCurInputDirKey == ioUserKeyInput::DKI_LEFTDOWN || m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHTDOWN )
				pOwner->AddReservedSliding3( dwSlidingTime, Info.m_fForceMovePower, Info.m_fForceMoveFriction, Info.m_fForceAngle, vMoveDir );
			else
				pOwner->AddReservedSliding2( dwSlidingTime, Info.m_fForceMovePower, Info.m_fForceMoveFriction, vMoveDir );
		}
	}
	else// if ( m_eCurInputDirKey == ioUserKeyInput::DKI_LEFT || m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHT )
	{
		pOwner->ChangeDirectionByInputDir( true );

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

		pOwner->ClearJumpAimState();

		pOwner->SetReservedSliding( AttAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, 1.0f, false );

		DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	}

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;;
		kPacket << ST_DASH;
		kPacket << pOwner->GetTargetRot();
		kPacket << vCharDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_ePreInputDirKey = m_eCurInputDirKey;

	m_iCurJumpDashCount++;
}

void ioSkyWalkerJump::ProcessJumpDashState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->CheckExtraAniJump();

	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if ( !pGrp )
			return;

		m_dwMotionEndTime = 0;
		m_SkyWalkerJumpState = SWJS_NONE;

		// 용병 Y값과 맵 지형 높이 값을 비교해 end jump 동작할 것인지 판단
		D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
		ioPlayStage* pStage = pOwner->GetCreator();
		float fCheckMapHeight = 0.0f;
		if( pStage 
			&& ( m_eCurInputDirKey == ioUserKeyInput::DKI_LEFTDOWN || m_eCurInputDirKey == ioUserKeyInput::DKI_RIGHTDOWN || m_eCurInputDirKey == ioUserKeyInput::DKI_DOWN ) )
		{
			fCheckMapHeight = pStage->GetMapHeight( vOwnerPos.x, vOwnerPos.z );
			float fValue = vOwnerPos.y - fCheckMapHeight;
			if( m_fInactiveEndJumpDistance > fValue )
			{
				m_SkyWalkerJumpState = SWJS_LOW_JUMP_HEIGHT;

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_EXTEND_JUMP );
					kPacket << pOwner->GetCharName();
					kPacket << (int)GetType();
					kPacket << pOwner->GetHP().m_fCurValue;;
					kPacket << ST_DASH_LOW_HEIGHT_END;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}

		pOwner->SetChangeJumppingState( m_fJumpDashEndJumpPowerRate, false, 0 );
		return;
	}

	if( FRAMEGETTIME() >= m_dwEnableComboTime_S + dwLooseTime )
	{
		if ( m_bReserveJumpAttack )
		{	
			SetChargingState( pOwner );
			return;
		}
	}
}

bool ioSkyWalkerJump::IsCanChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	ioSkyWalkerItem *pSkyWalkerItem = ToSkyWalkerItem( pOwner->GetWeapon() );
	if ( !pSkyWalkerItem )
		return false;

	if( !pSkyWalkerItem->IsEnableGauge( m_fJumpChargeAttackUseGauge ) )
		return false;

	return true;
}

void ioSkyWalkerJump::SetChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	int iAniID = pGrp->GetAnimationIdx( m_szJumpChargeAttackAni );
	if ( iAniID == -1 )
		return;

	ioSkyWalkerItem *pSkyWalkerItem = ToSkyWalkerItem( pOwner->GetWeapon() );
	if( pSkyWalkerItem )
		pSkyWalkerItem->DecreaseGauge( m_fJumpChargeAttackUseGauge );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	m_SkyWalkerJumpState = SWJS_CHARGE_ATTACK;
	m_eCurInputDirKey = ioUserKeyInput::DKI_NONE;

	//Clear Weapon
	ClearChargeWeapon( pOwner );

	//Clear Data
	m_fCurJumpChargeAttackRange	= 0.0f;
	m_szTargetName.Clear();

	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eCurrDir = pOwner->CheckCurDirKey();
		if( eCurrDir != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eCurrDir );

		m_vJumpChargeAttackMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &m_vJumpChargeAttackMoveDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vAxis, &vAxis );

		D3DXQUATERNION qtNewRot;
		float fCurAngle = ioMath::ArrangeHead( m_fJumpChargeAttackAngle );
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

		m_vJumpChargeAttackMoveDir = qtNewRot * m_vJumpChargeAttackMoveDir;
		D3DXVec3Normalize( &m_vJumpChargeAttackMoveDir, &m_vJumpChargeAttackMoveDir );
	}

	float fRate = 1.0f;
	if ( m_fJumpChargeAttackAniRate > 0.0f )
		fRate = m_fJumpChargeAttackAniRate;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );

	pOwner->CheckCharColSkipTime( iAniID, fRate, 0 );

	pOwner->IncreaseWeaponIndexBase();
	m_dwJumpChargeWeaponIndex = pOwner->GetWeaponIndexBase();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwEnableComboTime_S = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fRate;
	if( m_dwEnableComboTime_S > 0 )
		m_dwEnableComboTime_S += dwCurTime;

	FireTime kFireTime;
	kFireTime.dwStart				= dwCurTime;
	kFireTime.iAttributeIdx			= m_iJumpChargeWeaponType;
	kFireTime.iResistanceIdx		= 0;
	kFireTime.szWoundedAni			= "";
	kFireTime.dwWoundedAniDuration	= 0;
	kFireTime.bLoopWoundedAni		= false;
	kFireTime.dwWeaponIdx			= m_dwJumpChargeWeaponIndex;
	kFireTime.eFireTimeType			= FTT_EXTEND_ATTACK;

	m_dwJumpChargeEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fRate;

	pOwner->ExtendFireExplicit( kFireTime,  pOwner->GetMidPositionByRate(), m_vJumpChargeAttackMoveDir, "" );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;;
		kPacket << ST_CHARGE_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vJumpChargeAttackMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSkyWalkerJump::ProcessChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	D3DXVECTOR3 vComparePos = pOwner->GetWorldPosition();
	if ( pOwner->IsNeedProcess() )
	{
		if ( !CheckChargeWeaponLive( pOwner ) )
		{
			SetJumpChargeEndDelayState( pOwner );
			return;
		}

		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( !pOwner->IsCharDropZonePos() && fHeightGap < 5.0f )
		{
			SetJumpChargeEndDelayState( pOwner );
			return;
		}

		if ( m_fCurJumpChargeAttackRange > m_fMaxJumpChargeAttackRange )
		{
			SetJumpChargeEndDelayState( pOwner );
			return;
		}
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fDistance = m_fJumpChargeAttackSpeed * fTimePerSec;
	D3DXVECTOR3 vMoveAmt = ioMath::VEC3_ZERO;
	vMoveAmt = m_vJumpChargeAttackMoveDir * fDistance;
	m_fCurJumpChargeAttackRange += fDistance;

	if( vMoveAmt == ioMath::VEC3_ZERO )
		return;

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{		
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
	{
		pOwner->SetWorldPosition( vPos );
	}

	if ( pOwner->IsNeedProcess() )
	{
		if( vPos == vComparePos )
		{
			if( m_dwJumpChargeEndTime < FRAMEGETTIME() )
			{
				SetJumpChargeEndDelayState( pOwner );
				return;
			}
		}
	}
}

bool ioSkyWalkerJump::IsChargeAttacked()
{
	switch( m_SkyWalkerJumpState )
	{
	case SWJS_CHARGE_ATTACK:
	case SWJS_CHARGE_ATTACK_END_DELAY:
		return true;
	}

	return false;
}

bool ioSkyWalkerJump::IsDashAttacked()
{
	switch( m_SkyWalkerJumpState )
	{
	case SWJS_DASH:
		return true;
	}

	return false;
}

bool ioSkyWalkerJump::CheckChargeWeaponLive( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;
	ioZoneEffectWeapon* pZoneWeapon = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwJumpChargeWeaponIndex ) );
	if( pZoneWeapon && pZoneWeapon->IsLive() )
		return true;

	return false;
}

void ioSkyWalkerJump::ClearChargeWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioZoneEffectWeapon* pZone = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwJumpChargeWeaponIndex ) );
	if( pZone )
		pZone->SetWeaponDead();
	m_dwJumpChargeWeaponIndex = 0;
}

void ioSkyWalkerJump::SetNextJumpItemState( ioBaseChar *pOwner, ioBaseChar *pWounder, bool bDefense, ChangeNextItemType type, ioWeapon* pWeapon, float fDamage )
{
	if ( pWounder->IsNeedProcess() && fDamage > 0.0f )
	{
		if ( !m_szTargetName.IsEmpty() )
			return;

		m_szTargetName = pWounder->GetCharName();

		if( pWounder->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << ST_SET_TARGET;
			kPacket << m_szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioSkyWalkerJump::SetJumpChargeEndDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SkyWalkerJumpState = SWJS_CHARGE_ATTACK_END_DELAY;
	ClearChargeWeapon( pOwner );
	m_dwJumpChargeEndDelayEndTime = FRAMEGETTIME() + m_dwJumpChargeEndDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;;
		kPacket << ST_CHARGE_ATTACK_END_DELAY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSkyWalkerJump::ProcessChargeAttackEndDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if ( m_dwJumpChargeEndDelayEndTime && m_dwJumpChargeEndDelayEndTime < FRAMEGETTIME() )
	{
		m_dwJumpChargeEndDelayEndTime = 0;
		m_dwJumpChargeEndTime = 0;
		m_iCurComboCnt = 0;
		pOwner->SetChangeJumppingState( m_fJumpChargeAttackEndJumpPowerRate, false, 0 );
		m_bEnableChargeAttAfterSeveralJump = true;
		return;
	}
}

void ioSkyWalkerJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case ST_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChargingState( pOwner );
		}
		break;
	case ST_NORMAL_ATTACK:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			SetComboJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case ST_DOUBLE_JUMP:
		SetDoubleJump( pOwner );
		break;
	case ST_DASH:
		{
			D3DXQUATERNION qtRotate;
			D3DXVECTOR3 vCharDir = ioMath::VEC3_ZERO;
			rkPacket >> qtRotate;
			rkPacket >> vCharDir;
			pOwner->SetTargetRotToRotate( qtRotate, false );


			SetJumpDashState( pOwner, false, vCharDir );
		}
		break;
	case ST_DASH_LOW_HEIGHT_END:
		{
			m_SkyWalkerJumpState = SWJS_LOW_JUMP_HEIGHT;
		}
		break;
	case ST_CHARGE_ATTACK:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;
			pOwner->SetTargetRotToRotate( qtRotate, false );

			rkPacket >> m_vJumpChargeAttackMoveDir;

			SetChargeAttackState( pOwner );
		}
		break;
	case ST_CHARGE_ATTACK_END_DELAY:
		SetJumpChargeEndDelayState( pOwner );
		break;
	case ST_SET_TARGET:
		rkPacket >> m_szTargetName;
		break;
	}
}

void ioSkyWalkerJump::SetLandingState( ioBaseChar *pOwner )
{
	ioExtendJump::SetLandingState( pOwner );
	m_SkyWalkerJumpState = SWJS_NONE;
}

float ioSkyWalkerJump::CalculateAngle( D3DXVECTOR3 v1, D3DXVECTOR3 v2 )
{
	D3DXVec3Normalize( &v1, &v1 );
	D3DXVec3Normalize( &v2, &v2 );

	v1 *= 100.0f;
	v2 *= 100.0f;

	float fDot		= ( v1.x * v2.x ) + ( v1.y * v2.y ) + ( v1.z * v2.z );
	float fCosth	= fDot / (D3DXVec3Length( &v1 ) * D3DXVec3Length( &v2 ));
	float fRadian	= acosf(fCosth);
	float fDegree	= fRadian * ( 180.0f / 3.14159265f );

	return fDegree;
}

ioSkyWalkerJump::DashDirType ioSkyWalkerJump::CheckAttackDirByAngle( ioBaseChar *pOwner, D3DXVECTOR3 &vDir )
{
	ioUserKeyInput::DirKeyInput eCurrDir = pOwner->CheckCurDirKey();
	float fAngle = 360.0f;
	DashDirType eDashDirType = DDT_FIRST_TYPE;
	D3DXVECTOR3 vCompare = ioMath::VEC3_ZERO;
	for( int i = 0; i < (int)m_vDirInfo.size(); i++ )
	{
		m_vDirInfo[i].m_vDir.y = vDir.y;
		float fCompareAngle = CalculateAngle( vDir, m_vDirInfo[i].m_vDir );
		if( fAngle > fCompareAngle )
		{
			fAngle			= fCompareAngle;
			eDashDirType	= m_vDirInfo[i].m_eDashDirType;
			vCompare		= vDir - m_vDirInfo[i].m_vDir;
			vCompare.x		= abs( vCompare.x );
			vCompare.z		= abs( vCompare.z );
		}
		else if( fAngle == fCompareAngle && vDir != m_vDirInfo[i].m_vDir )
		{
			D3DXVECTOR3 vCompare2 = vDir - m_vDirInfo[i].m_vDir;
			vCompare2.x		= abs( vCompare2.x );
			vCompare2.z		= abs( vCompare2.z );
			if( vCompare > vCompare2 )
			{
				vCompare = vCompare2;
				fAngle			= fCompareAngle;
				eDashDirType	= m_vDirInfo[i].m_eDashDirType;
			}
		}
	}

	return eDashDirType;
}