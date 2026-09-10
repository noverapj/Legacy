
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTrinityJump.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioTrinityJump::ioTrinityJump()
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

ioTrinityJump::ioTrinityJump( const ioTrinityJump &rhs )
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
	m_AttackInfo( rhs.m_AttackInfo ),
	m_nMaxWeaponCnt( rhs.m_nMaxWeaponCnt ),
	m_dwMaxSwallowTime( rhs.m_dwMaxSwallowTime ),
	m_stSwallowAni( rhs.m_stSwallowAni ),
	m_fSwallowEndJumpPower( rhs.m_fSwallowEndJumpPower )
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
	m_dwMotionEndTime = 0;
	m_dwSwallowEndTime = 0;
	m_bSwallowModePlayed = false;

	//트리니티
	m_bDoubleJumped = false;

	m_bCanSwallowJump = false;
}

ioExtendJump* ioTrinityJump::Clone()
{
	return new ioTrinityJump( *this );
}

ioTrinityJump::~ioTrinityJump()
{
}

JumpType ioTrinityJump::GetType() const
{
	return JT_TRINITY_JUMP;
}

void ioTrinityJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	//////////////////////////////////////////////////////////////////////////
	LoadTrinityInfo(rkLoader);
	//////////////////////////////////////////////////////////////////////////

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
	LoadAttackAttribute_e( "extend_jump_max_attack", m_ExtendMaxAttack, rkLoader );

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
}

void ioTrinityJump::LoadComboList( ioINILoader &rkLoader )
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

void ioTrinityJump::LoadTrinityInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	m_AttackInfo.Init();

	m_nMaxWeaponCnt = rkLoader.LoadInt_e( "max_weapon_cnt", 0 );
	m_nMaxWeaponCnt = max( 1, m_nMaxWeaponCnt );
	for( int i = 0; i < m_nMaxWeaponCnt; i++ )
	{
		//D~
		wsprintf_e( szBuf, "jump_attack_atribute_full_%d", i+1 );
		LoadAttackAttribute( szBuf, m_AttackInfo.m_FullAttack[i], rkLoader );

		//~D
		wsprintf_e( szBuf, "jump_attack_atribute_none_%d", i+1 );
		LoadAttackAttribute( szBuf, m_AttackInfo.m_NoneFullAttack[i], rkLoader );

		//랜드어택
		wsprintf_e( szKey, "jump_attack_use_landattack_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackInfo.m_bUseGravityForJumpAttack[i] = rkLoader.LoadBool( szKey, false );
	}

	rkLoader.LoadString_e( "jump_reload_animation", "", szBuf, MAX_PATH );
	m_AttackInfo.m_ReloadAni = szBuf;
	m_AttackInfo.m_fReloadAniRate = rkLoader.LoadFloat_e( "jump_reload_time_rate", FLOAT1 );

	m_dwMaxSwallowTime = (DWORD)rkLoader.LoadInt_e( "max_swallow_time", 0 );

	rkLoader.LoadString_e( "swallow_ani", "", szBuf, MAX_PATH );
	m_stSwallowAni = szBuf;

	m_fSwallowEndJumpPower = rkLoader.LoadFloat_e( "swallow_end_power", 0.f );
}

void ioTrinityJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
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

void ioTrinityJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
		return;

	// 체크 순서 중요!!
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsDefenseKeyPress() 
		&& !pOwner->IsAttackKeyDown() && !pOwner->IsJumpKeyDown() && !m_bSwallowModePlayed && m_bCanSwallowJump )
	{
		SetSwallowMode( pOwner );
		return;
	}

	// combo
	if( m_bReserveJumpAttack )
	{
		SetChargingState( pOwner );
		return;
	}

	// DashCheck
	if( IsCanJumpDash( pOwner, fHeightGap ) )
	{
		pOwner->ClearSylphid();

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

			pOwner->SetMoveDirByRotate( qtRotate );
		}
		//

		ioEntityGroup *pGrp = pOwner->GetGroup();

		int iAniID = GetJumpDashAni( pOwner );
		float fTimeRate = m_fJumpDashAniRate;
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
	if( IsCanDoubleJump( pOwner, fHeightGap ) && !m_bDoubleJumped )
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
void ioTrinityJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_NORMAL:
	case CCS_DASH:
	case CCS_SWALLOW_MOON:
	case CCS_SWALLOW_MOON_END:
	case CCS_DOUBLE:
	case CCS_SPECIAL_FIRED:
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
	m_bCanSwallowJump = false;
}

void ioTrinityJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	//pOwner->ClearSylphid();

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

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

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
void ioTrinityJump::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	//pOwner->ClearSylphid();

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

	m_bDoubleJumped = true;
	m_bCanSwallowJump = false;

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

bool ioTrinityJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
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

void ioTrinityJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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
bool ioTrinityJump::IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;

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

	if( m_ChargeComboState != CCS_NONE )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

int ioTrinityJump::GetJumpDashAni( ioBaseChar *pOwner )
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

bool ioTrinityJump::IsJumpDashState()
{
	if( m_ChargeComboState == CCS_DASH )
	{
		if( m_dwActionEndTime > 0 && m_dwActionEndTime >= FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioTrinityJump::IsJumpAttackReserve()
{
	return m_bJumpAttackReserve;
}

void ioTrinityJump::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;

	if( pOwner->IsAttackKey() && !m_bJumpAttackReserve && !pOwner->IsChatModeState( true ) )
	{
		m_bJumpAttackReserve = true;
	}
}

void ioTrinityJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeComboState )
	{
	case CCS_NORMAL:
		ProcessComboJumpAttack( pOwner );

		if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			m_ChargeComboState = CCS_NONE;

		if( pOwner->IsNeedProcess() && pOwner->IsDefenseKeyPress() 
			&& !pOwner->IsAttackKeyDown() && !pOwner->IsJumpKeyDown() && !m_bSwallowModePlayed && m_bCanSwallowJump )
		{
			SetSwallowMode( pOwner );
			break;
		}

		CheckAirJump( pOwner );

		break;
	case CCS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case CCS_GATHER:
		ProcessGather( pOwner );
		break;
	case CCS_RELOAD:
		if( pOwner->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
			SetReload(pOwner);

		CheckAirJump( pOwner );
		
		break;
	case CCS_SPECIAL_FIRE:
		if( pOwner->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
			SetSpecialFired(pOwner);

		if( pOwner->IsNeedProcess() && pOwner->IsDefenseKeyPress() 
			&& !pOwner->IsAttackKeyDown() && !pOwner->IsJumpKeyDown() && !m_bSwallowModePlayed && m_bCanSwallowJump )
			SetSwallowMode( pOwner );

		CheckAirJump( pOwner );
		
		break;
	default:
		if( pOwner->IsNeedProcess() && pOwner->IsDefenseKeyPress() 
			&& !pOwner->IsAttackKeyDown() && !pOwner->IsJumpKeyDown() && !m_bSwallowModePlayed && m_bCanSwallowJump )
			SetSwallowMode( pOwner );

		break;
	}
}

void ioTrinityJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
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

void ioTrinityJump::ProcessJumpDash( ioBaseChar *pOwner )
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
			CheckReserve( pOwner );
		}
	}
}

void ioTrinityJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
				pOwner->ClearSylphid();
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
			bool bEnd;
			rkPacket >> bEnd;

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			if( !bEnd )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				int iAniID = GetJumpDashAni( pOwner );
				float fTimeRate = m_fJumpDashAniRate;
				if( fTimeRate <= 0.0f )
					fTimeRate = FLOAT1;

				if( pOwner && iAniID != -1 )
				{
					pOwner->ClearSylphid();

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

					pOwner->SetReservedSliding( m_vForceInfoList, iAniID, fTimeRate, 0 );
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

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			SetChargingState( pOwner );
		}
		break;
	case CCS_GATHER:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ApplySpecialFireState( pOwner );
		}
		break;
	case CCS_RELOAD:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ApplySpecialFireState( pOwner );
			ChangeReload(pOwner);
		}
		break;
	case CCS_RELOADED:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			int nCurWeapon;
			rkPacket >> nCurWeapon;

			SetReload(pOwner, nCurWeapon);
		}
		break;
	case CCS_SPECIAL_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ApplySpecialFireState( pOwner );

			int nWeapon;
			rkPacket >> nWeapon;

			ChangeSpecialFire(pOwner, nWeapon );
		}
		break;
	case CCS_LAND_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ApplySpecialFireState( pOwner );

			int nCurWeapon;
			rkPacket >> nCurWeapon;
			ChangeSpecialLandAttack( pOwner, nCurWeapon );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_SWALLOW_MOON:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			SetSwallowMode( pOwner );
		}
		break;
	case CCS_SWALLOW_MOON_END:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			SetSwallowEndJumpState( pOwner );
		}
		break;
	}
}

void ioTrinityJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_ChargeComboState = CCS_NONE;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_bDoubleJumped = false;

	pOwner->SetUsedFlashJump( false );
}

bool ioTrinityJump::IsNoProcessGravity()
{
	switch( m_ChargeComboState )
	{
	case CCS_GATHER:
		return true;
	}

	return false;
}

float ioTrinityJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

bool ioTrinityJump::IsCanJumpingSkill( ioBaseChar *pOwner )
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

DWORD ioTrinityJump::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

void ioTrinityJump::SetChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	//pOwner->ClearSylphid();

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
	m_bCanSwallowJump = false;

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

void ioTrinityJump::SetSpecialFireState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioTrinityItem *pTrinityItem = ToTrinityItem( pItem );
	if( !pTrinityItem )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_GATHER;

	// 기본처리
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

	m_bCanSwallowJump = false;

	// 장전 처리
	int nWeaponIndex = pTrinityItem->GetCurWeapon();
	float fCurBullet = pTrinityItem->GetCurBullet();
	if( fCurBullet < FLOAT1 )
	{
		ChangeReload( pOwner );
		return;
	}

	// 특별공격
	ChangeSpecialFire(pOwner, nWeaponIndex);

	/*
	if( pOwner->IsNeedProcess() )
	{
		//패킷 보내고
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	*/
}

void ioTrinityJump::ApplySpecialFireState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	// 기본처리
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

	m_bCanSwallowJump = false;
}

void ioTrinityJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			SetSpecialFireState( pOwner );
			return;
		}
	}
	else
	{
		SetComboJumpAttack( pOwner );
		return;
	}
}

void ioTrinityJump::ProcessGather( ioBaseChar *pOwner )
{
}

void ioTrinityJump::ProcessGatherAttack( ioBaseChar *pOwner )
{
	ProcessAirJump( pOwner );
}

int ioTrinityJump::GetCurChargeComboState()
{
	return m_ChargeComboState;
}

void ioTrinityJump::ProcessAirJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();

	CheckAirJump( pOwner );
}

void ioTrinityJump::ChangeReload( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeComboState = CCS_RELOAD;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx(m_AttackInfo.m_ReloadAni );
	float fTimeRate  = m_AttackInfo.m_fReloadAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_bCanSwallowJump = false;

	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedProcess() )
	{
		//패킷 보내고
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioTrinityJump::ChangeSpecialFire( ioBaseChar *pOwner, int nCurWeapon )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeComboState = CCS_SPECIAL_FIRE;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioTrinityItem *pTrinityItem = ToTrinityItem( pItem );
	if( !pTrinityItem )
		return false;

	if( m_AttackInfo.m_bUseGravityForJumpAttack[nCurWeapon] )
	{
		return ChangeSpecialLandAttack( pOwner, nCurWeapon );
	}

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pTrinityItem->DecreaseGauge();

	int iAniID	     = pGrp->GetAnimationIdx(m_AttackInfo.m_NoneFullAttack[nCurWeapon].m_AttackAnimation);
	float fTimeRate  = m_AttackInfo.m_NoneFullAttack[nCurWeapon].m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackInfo.m_NoneFullAttack[nCurWeapon].m_dwPreDelay;
	DWORD dwEndDelay = m_AttackInfo.m_NoneFullAttack[nCurWeapon].m_dwEndDelay;

	pOwner->SetNormalAttackByAttribute( m_AttackInfo.m_NoneFullAttack[nCurWeapon] );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	m_bCanSwallowJump = false;

	SetAirJump( pOwner, iAniID, fTimeRate );

	pTrinityItem->EraseEntityAndEffect(pOwner);
	pTrinityItem->AddEntityAndEffect( pOwner, true );

	if( pOwner->IsNeedProcess() )
	{
		//패킷 보내고
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		kPacket << nCurWeapon;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

bool ioTrinityJump::ChangeSpecialLandAttack( ioBaseChar *pOwner, int nCurWeapon )
{
	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;


	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioTrinityItem *pTrinityItem = ToTrinityItem( pItem );
	if( !pTrinityItem )
		return false;

	float fGravityAmt = pItem->GetNewJumpAttackGravity();

	pTrinityItem->DecreaseGauge();

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

	int iAniID = pGrp->GetAnimationIdx( m_AttackInfo.m_NoneFullAttack[nCurWeapon].m_AttackAnimation );
	float fTimeRate = m_AttackInfo.m_NoneFullAttack[nCurWeapon].m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackInfo.m_NoneFullAttack[nCurWeapon].m_dwPreDelay;
	DWORD dwEndDelay = m_AttackInfo.m_NoneFullAttack[nCurWeapon].m_dwEndDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_AttackInfo.m_NoneFullAttack[nCurWeapon].m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_AttackInfo.m_NoneFullAttack[nCurWeapon].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	DWORD dwEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	DWORD dwReserveTime = 0;

	float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveTime > 0.0f )
		dwReserveTime = dwCurTime + fReserveTime;

	m_ChargeComboState = CCS_LAND_ATTACK;
	m_bCanSwallowJump = false;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, fGravityAmt, true );

	pTrinityItem->EraseEntityAndEffect(pOwner);
	pTrinityItem->AddEntityAndEffect( pOwner, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		kPacket << nCurWeapon;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioTrinityJump::ClearEntityAndEffect( ioBaseChar *pOwner )
{
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioTrinityItem *pTrinityItem = ToTrinityItem( pItem );
	if( !pTrinityItem )
		return;
}

void ioTrinityJump::SetReload( ioBaseChar *pOwner, int nCurWeapon )
{
	m_ChargeComboState = CCS_RELOADED;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioTrinityItem *pTrinityItem = ToTrinityItem( pItem );
	if( !pTrinityItem )
		return;

	pTrinityItem->EraseEntityAndEffect(pOwner);
	if( nCurWeapon == -1 )
		pTrinityItem->NextWeapon();
	else
		pTrinityItem->SetCurWeapon( nCurWeapon );

	pTrinityItem->AddNormalEntityAndEffect(pOwner);
	int nCurWeaponIndex = pTrinityItem->GetCurWeapon();

	if( pOwner->IsNeedProcess() )
	{
		//패킷 보내고
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		kPacket << nCurWeaponIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTrinityJump::SetSpecialFired( ioBaseChar *pOwner )
{
	m_ChargeComboState = CCS_SPECIAL_FIRED;

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioTrinityItem *pTrinityItem = ToTrinityItem( pItem );
	if( !pTrinityItem )
		return;

	pTrinityItem->EraseEntityAndEffect(pOwner);
	pTrinityItem->AddNormalEntityAndEffect(pOwner);

	m_bCanSwallowJump = false;

	if( pOwner->IsNeedProcess() )
	{

	}
}

bool ioTrinityJump::IsSwallowMoonState()
{
	if( m_ChargeComboState == CCS_SWALLOW_MOON )
		return true;

	return false;
}

void ioTrinityJump::ClearState( ioBaseChar *pOwner, int eState  )
{
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioTrinityItem *pTrinityItem = ToTrinityItem( pItem );
	if( !pTrinityItem )
		return;

	m_bCanSwallowJump = false;
	if( eState == CS_TRINITY_SWALLOW )
	{
		m_bSwallowModePlayed = true;
	}
	else if( eState == CS_JUMP )
	{
		;
	}
	else if( eState == CS_USING_SKILL )
	{
		;
	}
	else if( eState == CS_NEW_JUMP_ATTACK )
	{
		;
	}
	else 
	{
		m_bDoubleJumped = false;
		m_bSwallowModePlayed = false;

		pTrinityItem->EraseEntityAndEffect(pOwner);
		pTrinityItem->AddNormalEntityAndEffect(pOwner);
	}
}

void ioTrinityJump::ProcessSwallowMoonState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetCurMoveSpeed( 0.0f );

	if( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsDefenseKeyDown() )
			SetSwallowEndJumpState( pOwner );

		if( dwCurTime > m_dwSwallowEndTime )
			SetSwallowEndJumpState( pOwner );
	}
}

void ioTrinityJump::SetSwallowMode( ioBaseChar *pOwner )
{
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioTrinityItem *pTrinityItem = ToTrinityItem( pItem );
	if( !pTrinityItem )
		return;

	pTrinityItem->EraseEntityAndEffect(pOwner);
	pTrinityItem->AddNormalEntityAndEffect(pOwner);

	m_bCanSwallowJump = false;
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwSwallowEndTime = dwCurTime + m_dwMaxSwallowTime;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_stSwallowAni );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_ChargeComboState = CCS_SWALLOW_MOON;
	m_bCanSwallowJump = false;

	if( pOwner->IsNeedProcess() )
	{
		//패킷 보내고
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	

	pOwner->SetState( CS_TRINITY_SWALLOW );
}

void ioTrinityJump::SetSwallowEndJumpState( ioBaseChar *pOwner )
{
	m_bCanSwallowJump = false;
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetSKillEndJumpState( m_fSwallowEndJumpPower, false, false, false, true );

	m_ChargeComboState = CCS_SWALLOW_MOON_END;
	m_bCanSwallowJump = false;

	if( pOwner->IsNeedProcess() )
	{
		//패킷 보내고
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTrinityJump::SetSwallowInput(int iType, ioBaseChar *pOwner)
{
	m_bCanSwallowJump = true;
}

/*void ioTrinityJump::SetShowNormalMesh( ioBaseChar *pOwner )
{
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioTrinityItem *pTrinityItem = ToTrinityItem( pItem );
	if( !pTrinityItem )
		return;

	pTrinityItem->EraseEntityAndEffect(pOwner);
	pTrinityItem->AddNormalEntityAndEffect(pOwner);
}


void ioTrinityJump::SetHideMesh( ioBaseChar *pOwner )
{
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	ioTrinityItem *pTrinityItem = ToTrinityItem( pItem );
	if( !pTrinityItem )
		return;

	pTrinityItem->EraseEntityAndEffect(pOwner);
}*/

bool ioTrinityJump::CheckCanUseBuffAirDash()
{
	if( m_ChargeComboState == CCS_SWALLOW_MOON_END )
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioTrinitySwallowMoonSpecialState::ioTrinitySwallowMoonSpecialState()
{
}

ioTrinitySwallowMoonSpecialState::~ioTrinitySwallowMoonSpecialState()
{
}

void ioTrinitySwallowMoonSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioTrinitySwallowMoonSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioTrinitySwallowMoonSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioAttackableItem *pAttackable = pOwner->GetPriorityItem( SPT_ATTACK );
	if ( !pAttackable )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	ioTrinityJump *pTrinityJump = ToTrinityJump( pAttackable->GetCurExtendJump() );
	if( pTrinityJump )
		pTrinityJump->ProcessSwallowMoonState( pOwner );
	else
		pOwner->SetState( CS_DELAY );
}

void ioTrinitySwallowMoonSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioTrinitySwallowMoonSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}