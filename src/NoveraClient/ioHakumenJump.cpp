

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioHakumenJump.h"

ioHakumenJump::ioHakumenJump()
{
	m_ChargeComboState = CCS_NONE;

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedGatherAttack = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	// gather
	m_dwChargingStartTime = 0;
	m_dwGatherStartTime = 0;

	// double
	m_iCurJumpCnt = 1;

	// dash
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
}

ioHakumenJump::ioHakumenJump( const ioHakumenJump &rhs )
	: ioExtendJump( rhs ),
	m_fJumpSpeedRateFR( rhs.m_fJumpSpeedRateFR ),
	m_fJumpSpeedRateSI( rhs.m_fJumpSpeedRateSI ),
	m_fJumpSpeedRateBG( rhs.m_fJumpSpeedRateBG ),
	m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
	m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
	m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
	m_ComboAttackList( rhs.m_ComboAttackList ),
	m_ExtendMaxAttack( rhs.m_ExtendMaxAttack ),
	m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
	m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
	m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
	m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
	m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
	m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
	m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
	m_fJumpDashAniRate( rhs.m_fJumpDashAniRate ),
	m_vJumpDashAniList( rhs.m_vJumpDashAniList ),
	m_vForceInfoList( rhs.m_vForceInfoList ),
	m_bEnableGatherJump( rhs.m_bEnableGatherJump ),
	m_bEnableDoubleJump( rhs.m_bEnableDoubleJump ),
	m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
	m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
	m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
	m_dwGatherTime( rhs.m_dwGatherTime ),
	m_szGatherAni( rhs.m_szGatherAni ),
	m_dwChargingTime( rhs.m_dwChargingTime ),
	m_szChargingAni( rhs.m_szChargingAni ),
	m_DoubleJumpAttack( rhs.m_DoubleJumpAttack ),
	m_HakumenDefenseInfo( rhs.m_HakumenDefenseInfo ),
	m_DashJumpAttack( rhs.m_DashJumpAttack ),
	m_nMaxJumpDashCnt( rhs.m_nMaxJumpDashCnt )
{
	m_ChargeComboState = CCS_NONE;

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedGatherAttack = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	// gather
	m_dwChargingStartTime = 0;
	m_dwGatherStartTime = 0;

	// double
	m_iCurJumpCnt = 1;

	// dash
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_nCurJumpDashCnt = 0;

	m_bNeedDashWoundedJump = false;
	m_fJumpPowerRate = FLOAT1;
}

ioExtendJump* ioHakumenJump::Clone()
{
	return new ioHakumenJump( *this );
}

ioHakumenJump::~ioHakumenJump()
{
}

JumpType ioHakumenJump::GetType() const
{
	return JT_HAKUMEN_JUMP;
}

void ioHakumenJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//
	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	m_bEnableGatherJump = rkLoader.LoadBool_e( "enable_gather_jump", false );
	m_bEnableDoubleJump = rkLoader.LoadBool_e( "enable_double_jump", false );
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );

	// combo
	m_fJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	LoadComboList( rkLoader );

	// gather
	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );
	rkLoader.LoadString_e( "charging_ani", "", szBuf, MAX_PATH );
	m_szChargingAni = szBuf;

	m_dwGatherTime = (DWORD)rkLoader.LoadInt_e( "gather_time", 0 );
	rkLoader.LoadString_e( "gather_ani", "", szBuf, MAX_PATH );
	m_szGatherAni = szBuf;

	m_ExtendMaxAttack.Init();
	LoadAttackAttribute( "extend_jump_max_attack", m_ExtendMaxAttack, rkLoader );

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

	m_DoubleJumpAttack.Init();
	LoadAttackAttribute_e( "double_jump_attack", m_DoubleJumpAttack, rkLoader );

	// dash
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

	//////////////////////////////////////////////////////////////////////////
	m_HakumenDefenseInfo.Init();

	rkLoader.LoadString_e( "jump_defense_ready_motion", "", szBuf, MAX_PATH );
	m_HakumenDefenseInfo.m_stDefenseReadyMotion = szBuf;
	m_HakumenDefenseInfo.m_fDefenseReadyMotionRate = rkLoader.LoadFloat_e( "jump_defense_ready_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_defense_wait_motion", "", szBuf, MAX_PATH );
	m_HakumenDefenseInfo.m_stDefenseMotion = szBuf;
	m_HakumenDefenseInfo.m_fDefenseMotionRate = rkLoader.LoadFloat_e( "jump_defense_wait_motion_rate", FLOAT1 );

	wsprintf_e( szKey, "jump_defense_attack" );
	LoadAttackAttribute( szKey, m_HakumenDefenseInfo.m_DefenseAttack, rkLoader );

	m_HakumenDefenseInfo.m_fEndJumpAmt = rkLoader.LoadFloat_e( "jump_defense_jump_amt", 0.0f );

	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "dash_jump_attack", m_DashJumpAttack, rkLoader );

	m_nMaxJumpDashCnt = rkLoader.LoadInt_e( "max_jump_dash_cnt", 1 );
}

void ioHakumenJump::LoadComboList( ioINILoader &rkLoader )
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

void ioHakumenJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedGatherAttack = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	// gather
	m_dwChargingStartTime = 0;
	m_dwGatherStartTime = 0;

	// double
	m_iCurJumpCnt = 1;

	// dash
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

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
}

void ioHakumenJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
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

	// DashCheck
	if( IsCanJumpDash( pOwner, fHeightGap ) )
	{
		m_ChargeComboState = CCS_DASH;
		m_bUseExtendJump = true;

		m_dwActionEndTime = 0;

		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );

		//
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
		//

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( !pGrp )
			return;

		int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );

		//int iAniID = GetJumpDashAni( pOwner );
		float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;;
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

			pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

			CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

			pOwner->RefreshFireTimeList( iAniID,
				m_DashJumpAttack.m_vWeaponInfoList,
				FTT_JUMP_ATTACK,
				fTimeRate,
				m_DashJumpAttack.m_dwPreDelay );

			//pOwner->SetComboDashState();

			m_nCurJumpDashCnt++;
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_JUMP_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetMoveDir();
			kPacket << m_ChargeComboState;
			kPacket << false;					// dash end
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
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

	ProcessJumpDash( pOwner );
}

// combo
void ioHakumenJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_NORMAL:
	case CCS_DASH:
		break;
	default:
		return;
	}

	if( m_bReserveJumpAttack )
		return;

	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		return;
	}

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

void ioHakumenJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

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

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();
}

// double
void ioHakumenJump::SetDoubleJump( ioBaseChar *pOwner )
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

bool ioHakumenJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

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

void ioHakumenJump::SetDoubleJumpAni( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

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
bool ioHakumenJump::IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;

	if( !pOwner->IsNeedProcess() ) return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if( m_ChargeComboState != CCS_NONE )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	if( m_nMaxJumpDashCnt <= m_nCurJumpDashCnt )
		return false;

	return true;
}

int ioHakumenJump::GetJumpDashAni( ioBaseChar *pOwner )
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

bool ioHakumenJump::IsJumpDashState()
{
	if( m_ChargeComboState == CCS_DASH )
	{
		if( m_dwActionEndTime > 0 && m_dwActionEndTime >= FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioHakumenJump::IsJumpAttackReserve()
{
	return m_bJumpAttackReserve;
}

void ioHakumenJump::CheckKeyReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;

	if( pOwner->IsAttackKey() && !m_bJumpAttackReserve && !pOwner->IsChatModeState( true ) )
	{
		m_bJumpAttackReserve = true;
	}
}

void ioHakumenJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	switch( m_ChargeComboState )
	{
	case CCS_NORMAL:
		ProcessComboJumpAttack( pOwner );
		break;
	case CCS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case CCS_GATHER:
		ProcessGather( pOwner );
		break;
	case CCS_GATHER_ATTACK:
		ProcessGatherAttack( pOwner );
		break;
	case CCS_GATHER_MAX:
		ProcessGatherMaxAttack( pOwner );
		break;
	}
}

void ioHakumenJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ProcessAirJump( pOwner );

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

void ioHakumenJump::ProcessJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_ChargeComboState != CCS_DASH ) return;
	if( m_bDashEnd ) return;
	if( m_dwActionEndTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwActionEndTime < dwCurTime )
	{
		if( m_bNeedDashWoundedJump )
		{
			m_bNeedDashWoundedJump = false;
			float fJumpPower = pOwner->GetJumpPower();
			fJumpPower *= m_fJumpPowerRate;
			//pOwner->SetJumpPower( fJumpPower );
			m_ChargeComboState = CCS_NONE;
			pOwner->SetExtendAttackEndJump( fJumpPower, FLOAT1 );
			return;
		}
		
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
				pOwner->ClearDirDoubleClick();
				//pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );

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
						kPacket << m_ChargeComboState;
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

void ioHakumenJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

			SetComboJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
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
	case CCS_DOUBLE_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetDoubleJumpAttack( pOwner );

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
				ioEntityGroup *pGrp = pOwner->GetGroup();
				//int iAniID = GetJumpDashAni( pOwner );
				int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
				float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;;
				//float fTimeRate = m_fJumpDashAniRate;
				if( fTimeRate <= 0.0f )
					fTimeRate = FLOAT1;

				if( pOwner && iAniID != -1 )
				{
					m_ChargeComboState = CCS_DASH;
					m_bUseExtendJump = true;

					m_dwActionEndTime = 0;

					pOwner->SetCurJumpPowerAmt( 0.0f );
					pOwner->SetForcePowerAmt( 0.0f );
					pOwner->SetGravityAmt( 0.0f );

					pOwner->SetTargetRot( qtRotate );

					pGrp->ClearAllActionAni( FLOAT100, true );
					pGrp->ClearAllLoopAni( FLOAT100, true );
					pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, fTimeRate, 0.0f, 0, true );

					DWORD dwCurTime = FRAMEGETTIME();
					DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

					m_dwActionEndTime = dwCurTime;
					m_dwActionEndTime += dwDuration;

					pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

					pOwner->RefreshFireTimeList( iAniID,
						m_DashJumpAttack.m_vWeaponInfoList,
						FTT_JUMP_ATTACK,
						fTimeRate,
						m_DashJumpAttack.m_dwPreDelay );
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

						pOwner->ClearDirDoubleClick();
					}
				}
			}
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
	case CCS_GATHER:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			SetGatherJump( pOwner );
		}
		break;
	case CCS_GATHER_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeToGatherJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_GATHER_MAX:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetGatherMax( pOwner );
		}
		break;
	}
}

void ioHakumenJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_ChargeComboState = CCS_NONE;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_nCurJumpDashCnt = 0;

	m_bNeedDashWoundedJump = false;

	pOwner->SetUsedFlashJump( false );
}

bool ioHakumenJump::IsNoProcessGravity()
{
	switch( m_ChargeComboState )
	{
	case CCS_GATHER:
		return true;
	}

	return false;
}

float ioHakumenJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

bool ioHakumenJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_DASH:
		if( m_bDashEnd )
			return true;

		return false;
	}

	return true;
}

DWORD ioHakumenJump::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

void ioHakumenJump::SetChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szChargingAni );
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

void ioHakumenJump::SetGatherJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

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

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szGatherAni );
	float fTimeRate = FLOAT1;
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_ChargeComboState = CCS_GATHER;
	m_dwGatherStartTime = dwCurTime;

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

void ioHakumenJump::ChangeToGatherJumpAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTEND_JUMP_ATTACK );
	if( !pAttr ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeComboState = CCS_GATHER_ATTACK;

	// For AirJumpPower
	DWORD dwStartTime = dwCurTime + dwPreDelay;

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

void ioHakumenJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			SetGatherJump( pOwner );
			return;
		}
	}
	else
	{
		SetComboJumpAttack( pOwner );
		return;
	}
}

void ioHakumenJump::ProcessGather( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( pOwner->IsEnableHakumenDefense() )
		{
			if( m_dwGatherStartTime+m_dwGatherTime < dwCurTime )
			{
				SetGatherMax( pOwner );
				return;
			}
		}
		else
			ChangeToGatherJumpAttack( pOwner );
	}
	else
		ChangeToGatherJumpAttack( pOwner );
}

void ioHakumenJump::ProcessGatherAttack( ioBaseChar *pOwner )
{
	ProcessAirJump( pOwner );
}

void ioHakumenJump::ProcessGatherMaxAttack( ioBaseChar *pOwner )
{
	ProcessAirJump( pOwner );
}

void ioHakumenJump::SetGatherMax( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetHakumenDefenseState( m_HakumenDefenseInfo );

	m_ChargeComboState = CCS_GATHER_MAX;

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

int ioHakumenJump::GetCurChargeComboState()
{
	return m_ChargeComboState;
}

void ioHakumenJump::SetDoubleJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	if( pItem && pItem->IsEnableNewJumpAttack() )
	{
		SetNewDoubleJumpAttack( pOwner, pItem->GetNewJumpAttackGravity() );
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_DoubleJumpAttack.m_AttackAnimation );
	float fTimeRate = m_DoubleJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DoubleJumpAttack.m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_DoubleJumpAttack.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_DoubleJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	m_ChargeComboState = CCS_DOUBLE_ATTACK;

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

void ioHakumenJump::ProcessAirJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();

	CheckAirJump( pOwner );
}

bool ioHakumenJump::IsCanDoubleJumpAttack()
{
	return m_bUsedDoubleJump;
}


void ioHakumenJump::ReserveDashWoundedJump( float fJumpPowerRate )
{
	m_bNeedDashWoundedJump = true;
	m_fJumpPowerRate = fJumpPowerRate;
}

void ioHakumenJump::SetNewDoubleJumpAttack( ioBaseChar *pOwner, float fGravityAmt )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_bReserveJumpAttack = false;
	m_bUseExtendJump = false;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );

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

	int iAniID = pGrp->GetAnimationIdx( m_DoubleJumpAttack.m_AttackAnimation );
	float fTimeRate = m_DoubleJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DoubleJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DoubleJumpAttack.m_dwEndDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_DoubleJumpAttack.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_DoubleJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	DWORD dwEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	DWORD dwReserveTime = 0;

	float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveTime > 0.0f )
		dwReserveTime = dwCurTime + fReserveTime;

	m_ChargeComboState = CCS_DOUBLE_ATTACK;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, fGravityAmt, true );

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



