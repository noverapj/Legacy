#include "StdAfx.h"
#include "ioUrielJump.h"

#include "ioUrielItem.h"
#include "ioZoneEffectWeapon.h"

ioUrielJump::ioUrielJump(void)
{
	ClearData();
}

ioUrielJump::ioUrielJump( const ioUrielJump &rhs ) :
ioExtendJump( rhs ),
//jump attack
m_ComboAttackList( rhs.m_ComboAttackList ),
//double jump
m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
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
//S_Hold
m_szS_HoldAni( rhs.m_szS_HoldAni ),
m_fS_HoldAniRate( rhs.m_fS_HoldAniRate ),
m_dwS_HoldEnableTime( rhs.m_dwS_HoldEnableTime ),
m_fS_HoldEnableHeight( rhs.m_fS_HoldEnableHeight ),
m_fS_HoldEndJumpPowerRate( rhs.m_fS_HoldEndJumpPowerRate ),
m_dwS_HoldKeyProtectTime( rhs.m_dwS_HoldKeyProtectTime ),
//Dash
m_vJumpDashAttack( rhs.m_vJumpDashAttack ),
m_iMaxJumpDashCount( rhs.m_iMaxJumpDashCount ),
m_fJumpDashEndJumpPowerRate( rhs.m_fJumpDashEndJumpPowerRate ),
//D~
m_szJumpChargeAttackAni( rhs.m_szJumpChargeAttackAni ),
m_fJumpChargeAttackAniRate( rhs.m_fJumpChargeAttackAniRate ),
m_fJumpChargeAttackAngle( rhs.m_fJumpChargeAttackAngle ),
m_fMaxJumpChargeAttackRange( rhs.m_fMaxJumpChargeAttackRange ),
m_iJumpChargeWeaponType( rhs.m_iJumpChargeWeaponType ),
m_fJumpChargeAttackEndJumpPowerRate( rhs.m_fJumpChargeAttackEndJumpPowerRate ),
m_fJumpChargeAttackSpeed( rhs.m_fJumpChargeAttackSpeed ),
m_JumpChargeAddAttack( rhs.m_JumpChargeAddAttack ),
m_dwJumpChargeEndDelay( rhs.m_dwJumpChargeEndDelay )
{
	ClearData();
}

ioUrielJump::~ioUrielJump(void)
{
}

ioExtendJump* ioUrielJump::Clone()
{
	return new ioUrielJump( *this );
}

JumpType ioUrielJump::GetType() const
{
	return JT_URIEL_JUMP;
}

void ioUrielJump::LoadProperty( ioINILoader &rkLoader )
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

	//AttackKey charge
	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );
	int iMaxChargeAni = rkLoader.LoadInt_e( "charging_jump_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "charging_jump_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackKeyChargingAniList.push_back( szBuf );
	}

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

	//S_Hold
	rkLoader.LoadString_e( "s_hold_ani", "", szBuf, MAX_PATH );
	m_szS_HoldAni = szBuf;
	m_fS_HoldAniRate = rkLoader.LoadFloat_e( "s_hold_ani_rate", 0.0f );
	m_fS_HoldEnableHeight = rkLoader.LoadFloat_e( "s_hold_enable_height", 0.0f );
	m_dwS_HoldEnableTime = (DWORD)rkLoader.LoadInt_e( "s_hold_enable_time", 0 );
	m_fS_HoldEndJumpPowerRate = rkLoader.LoadFloat_e( "s_hold_end_jump_power_rate", 0.0f );
	m_dwS_HoldKeyProtectTime = (DWORD)rkLoader.LoadInt_e( "s_hold_key_protect_time", 0 );

	//Dash
	LoadAttackAttribute_e( "jump_dash", m_vJumpDashAttack, rkLoader );
	m_iMaxJumpDashCount = rkLoader.LoadInt_e( "max_jump_dash_count", 0.0f );
	m_fJumpDashEndJumpPowerRate = rkLoader.LoadFloat_e( "jump_dash_end_jump_power_rate", 0.0f );

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
	LoadAttackAttribute_e( "jump_charge_add_attack", m_JumpChargeAddAttack, rkLoader );
}

void ioUrielJump::InitJump()
{
	m_bUsedDoubleJump = false;
	m_bUsedS_HoldAni = false;
	m_iCurJumpDashCount = 0;
	m_iCurJumpCnt = 1;
}

void ioUrielJump::ClearData()
{
	m_UrielJumpState = UJS_NONE;
	m_bReserveJumpAttack = false;

	m_iCurJumpCnt = 1;
	m_bUsedDoubleJump = false;
	m_dwChargingStartTime = 0;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bUsedS_HoldAni = false;
	m_dwMotionEndTime = 0;
	m_iCurJumpDashCount = 0;
	m_dwS_HoldKeyProtectEndTime = 0;
	m_dwAttackFireTime = 0;
	m_dwJumpChargeEndDelayEndTime = 0;
	m_szTargetName.Clear();
	m_fCurJumpChargeAttackRange	= 0.0f;
	m_vJumpChargeAttackMoveDir = ioMath::VEC3_ZERO;
}

void ioUrielJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_UrielJumpState = UJS_NONE;
	pOwner->SetUsedFlashJump( false );

	m_bUseExtendJump = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	// charge
	m_dwChargingStartTime = 0;

	m_dwReserveEnableTime = 0;
	m_dwMotionEndTime = 0;
	m_dwS_HoldKeyProtectEndTime = 0;
	m_dwAttackFireTime = 0;
	m_dwJumpChargeEndDelayEndTime = 0;
	m_fCurJumpChargeAttackRange	= 0.0f;
	m_vJumpChargeAttackMoveDir = ioMath::VEC3_ZERO;
}

float ioUrielJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

	if( m_UrielJumpState == UJS_DOUBLE_JUMP )
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

void ioUrielJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_UrielJumpState )
	{
	case UJS_NONE:
	case UJS_NORMAL_ATTACK:
	case UJS_DOUBLE_JUMP:
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

void ioUrielJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
		return;

	// combo
	if( m_bReserveJumpAttack )
	{
		SetChargingState( pOwner );
		return;
	}
	//S_Hold
	else if ( IsEnableS_HoldState( pOwner, fHeightGap ) )
	{
		SetS_HoldState( pOwner );
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
	switch( m_UrielJumpState )
	{
	case UJS_S_HOLD:
		ProcessS_HoldState( pOwner );
		break;
	case UJS_DASH:
		ProcessJumpDashState( pOwner );
		break;
	}
}

bool ioUrielJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
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

	switch( m_UrielJumpState )
	{
	case UJS_NONE:
	case UJS_DOUBLE_JUMP:
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

void ioUrielJump::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_UrielJumpState = UJS_DOUBLE_JUMP;

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

void ioUrielJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

void ioUrielJump::SetChargingState( ioBaseChar *pOwner )
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

	m_UrielJumpState = UJS_CHARGE;
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

void ioUrielJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	switch( m_UrielJumpState )
	{
	case UJS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case UJS_NORMAL_ATTACK:
		ProcessComboJumpAttack( pOwner, fHeightGap );
		break;
	case UJS_S_HOLD:
		ProcessS_HoldState( pOwner );
		break;
	case UJS_DASH:
		ProcessJumpDashState( pOwner );
		break;
	case UJS_CHARGE_ATTACK:
		ProcessChargeAttackState( pOwner );
		break;
	case UJS_CHARGE_ATTACK_END_DELAY:
		ProcessChargeAttackEndDelayState( pOwner );
		break;
	case UJS_CHARGE_ADD_ATTACK:
		ProcessChargeAddAttackState( pOwner );
		break;
	}
}

void ioUrielJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && IsCanChargeAttackState( pOwner ) )
	{
		if( m_dwChargingStartTime + m_dwChargingTime < dwCurTime )
			SetChargeAttackState( pOwner );
	}
	else
		SetComboJumpAttack( pOwner );
}

void ioUrielJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	pOwner->ClearSylphid();

	DWORD dwCurTime = FRAMEGETTIME();
	m_UrielJumpState = UJS_NORMAL_ATTACK;

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

void ioUrielJump::ProcessComboJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	CheckAirJump( pOwner );

	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( FRAMEGETTIME() >= m_dwEnableComboTime_S + dwLooseTime )
	{
		if ( m_bReserveJumpAttack )
		{	
			SetChargingState( pOwner );
			return;
		}
		//S_Hold 체크
		else if ( IsEnableS_HoldState( pOwner, fHeightGap ) )
		{
			SetS_HoldState( pOwner );
			return;
		}
		//Jump Dash 체크
		else if ( IsCanJumpDash( pOwner, fHeightGap ) )
		{
			SetJumpDashState( pOwner );
			return;
		}
	}
}

bool ioUrielJump::IsEnableS_HoldState( ioBaseChar *pOwner, float fHeightGap )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( m_bUsedS_HoldAni )
		return false;

	if( !pOwner->IsDefenseKey() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() )
		return false;

	if ( pOwner->GetUsedBuffJumpAttack() )
		return false;

	switch( m_UrielJumpState )
	{
	case UJS_NONE:
	case UJS_NORMAL_ATTACK:
	case UJS_DOUBLE_JUMP:
	case UJS_CHARGE_ATTACK:
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

void ioUrielJump::SetS_HoldState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szS_HoldAni );
	if( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_UrielJumpState = UJS_S_HOLD;
	m_bUsedS_HoldAni = true;
	//공격 초기화
	m_iCurComboCnt = 0;
	//대쉬 초기화
	m_iCurJumpDashCount = 0;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	m_dwS_HoldKeyProtectEndTime = 0;
	if ( m_dwS_HoldKeyProtectTime > 0 )
		m_dwS_HoldKeyProtectEndTime = dwCurTime + m_dwS_HoldKeyProtectTime;

	float fRate = 1.0f;
	if ( m_fS_HoldAniRate > 0.0f )
		fRate = m_fS_HoldAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;;
		kPacket << ST_S_HOLD;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUrielJump::ProcessS_HoldState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwS_HoldKeyProtectEndTime && m_dwS_HoldKeyProtectEndTime < FRAMEGETTIME() 
		&& !pOwner->IsDefenseKeyDown() )
	{
		SetS_HoldEnd( pOwner );
		return;
	}

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		SetS_HoldEnd( pOwner );
		return;
	}
}

void ioUrielJump::SetS_HoldEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	m_dwS_HoldKeyProtectEndTime = 0;
	m_dwMotionEndTime = 0;
	m_UrielJumpState = UJS_NONE;
	pOwner->SetChangeJumppingState( m_fS_HoldEndJumpPowerRate, false, 0 );
}

float ioUrielJump::GetCurGravityAmt( int iJumpState )
{
	switch( m_UrielJumpState )
	{
	case UJS_S_HOLD:
	case UJS_DASH:
	case UJS_CHARGE_ATTACK:
	case UJS_CHARGE_ADD_ATTACK:
	case UJS_CHARGE_ATTACK_END_DELAY:
		return -FLOAT1;
	}
	return 0.0f;
}

bool ioUrielJump::IsCanMoveState( int iJumpState )
{
	if ( m_UrielJumpState == UJS_S_HOLD )
	if ( m_UrielJumpState == UJS_CHARGE_ATTACK )
		return false;

	return true;
}

void ioUrielJump::ClearState( ioBaseChar *pOwner, int eNewState )
{
	if ( !pOwner )
		return;
	if ( pOwner->GetState() == CS_URIEL_STATE && eNewState == CS_JUMP )
	{
		m_bUsedDoubleJump = false;
		m_bUsedS_HoldAni = false;
		m_iCurJumpDashCount = 0;
		m_iCurJumpCnt = 1;
	}
	else if ( eNewState != CS_JUMP && eNewState != CS_USING_SKILL )
	{
		m_bUsedDoubleJump = false;
		m_bUsedS_HoldAni = false;
		m_iCurJumpDashCount = 0;
		m_iCurJumpCnt = 1;
	}
}

bool ioUrielJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	switch( m_UrielJumpState )
	{
	case UJS_NONE:
	case UJS_DOUBLE_JUMP:
	case UJS_NORMAL_ATTACK:
	case UJS_CHARGE_ATTACK:
	case UJS_CHARGE:
		return true;
	default:
		return false;
	}
}

bool ioUrielJump::IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( m_iCurJumpDashCount >= m_iMaxJumpDashCount)
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

	switch( m_UrielJumpState )
	{
	case UJS_NONE:
	case UJS_DOUBLE_JUMP:
	case UJS_NORMAL_ATTACK:
	case UJS_CHARGE_ATTACK:
		break;
	default:
		return false;
	}

	return true;
}

void ioUrielJump::SetJumpDashState( ioBaseChar *pOwner )
{
	pOwner->ClearSylphid();

	m_UrielJumpState = UJS_DASH;
	m_bUseExtendJump = true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	//
	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eCurrDir = pOwner->CheckCurDirKey();
		if( eCurrDir != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eCurrDir );
	}
	//

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_vJumpDashAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_vJumpDashAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += dwDuration;

	DWORD dwPreDelay = m_vJumpDashAttack.m_dwPreDelay;
	pOwner->SetReservedSliding( m_vJumpDashAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );
	
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_vJumpDashAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;;
		kPacket << ST_DASH;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iCurJumpDashCount++;
}

void ioUrielJump::ProcessJumpDashState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if ( !pGrp )
			return;

		m_dwMotionEndTime = 0;
		m_UrielJumpState = UJS_NONE;
		pOwner->SetChangeJumppingState( m_fJumpDashEndJumpPowerRate, false, 0 );
	}
}

bool ioUrielJump::IsCanChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;
	
	ioUrielItem *pUrielItem = ToUrielItem( pOwner->GetWeapon() );
	if ( !pUrielItem || !pUrielItem->CheckChargeAttackGauge() )
		return false;

	return true;
}

void ioUrielJump::SetChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	int iAniID = pGrp->GetAnimationIdx( m_szJumpChargeAttackAni );
	if ( iAniID == -1 )
		return;

	ioUrielItem* pUrielItem = ToUrielItem( pOwner->GetWeapon() );
	if ( !pUrielItem )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	m_UrielJumpState = UJS_CHARGE_ATTACK;

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

	pUrielItem->DecreaseChargeAttackGauge();
}

void ioUrielJump::ProcessChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

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

		if ( IsEnableJumpChargeAddAttack( pOwner ) )
		{
			SetJumpChargeAddAttack( pOwner );
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
}

bool ioUrielJump::IsChargeAttacked()
{
	switch( m_UrielJumpState )
	{
	case UJS_CHARGE_ATTACK:
	case UJS_CHARGE_ATTACK_END_DELAY:
	case UJS_CHARGE_ADD_ATTACK:
		return true;
	}

	return false;
}

bool ioUrielJump::CheckChargeWeaponLive( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;
	ioZoneEffectWeapon* pZoneWeapon = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwJumpChargeWeaponIndex ) );
	if( pZoneWeapon && pZoneWeapon->IsLive() )
		return true;

	return false;
}

void ioUrielJump::ClearChargeWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioZoneEffectWeapon* pZone = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwJumpChargeWeaponIndex ) );
	if( pZone )
		pZone->SetWeaponDead();
	m_dwJumpChargeWeaponIndex = 0;
}

bool ioUrielJump::IsEnableJumpChargeAddAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( m_dwEnableComboTime_S <= 0 || m_dwEnableComboTime_S > FRAMEGETTIME() )
		return false;

	if ( !pOwner->IsAttackKey() )
		return false;

	if ( m_szTargetName.IsEmpty() )
		return false;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( !pTarget )
	{
		m_szTargetName.Clear();
		return false;
	}

	return true;
}

void ioUrielJump::SetJumpChargeAddAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_JumpChargeAddAttack.m_AttackAnimation.IsEmpty() )
		return;

	ClearChargeWeapon( pOwner );

	m_UrielJumpState = UJS_CHARGE_ADD_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_JumpChargeAddAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_JumpChargeAddAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_JumpChargeAddAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_JumpChargeAddAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();


	m_dwEnableComboTime_E = dwStartTime;
	m_dwEnableComboTime_E += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwEnableComboTime_E += m_JumpChargeAddAttack.m_dwEndDelay;

	m_dwAttackFireTime = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwEnableComboTime_S = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwEnableComboTime_S > 0 )
		m_dwEnableComboTime_S += dwStartTime;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;;
		kPacket << ST_CHARGE_ADD_ATTACK;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUrielJump::ProcessChargeAddAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwAttackFireTime && m_dwAttackFireTime < FRAMEGETTIME() )
	{
		m_dwAttackFireTime = 0;
		JumpChargeAddAttackFire( pOwner );
	}

	if ( m_dwEnableComboTime_E && m_dwEnableComboTime_E < FRAMEGETTIME() )
	{
		m_dwEnableComboTime_E = 0;
		ClearChargeWeapon( pOwner );
		pOwner->SetChangeJumppingState( m_fJumpChargeAttackEndJumpPowerRate, false, 0 );
	}
}

void ioUrielJump::JumpChargeAddAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_JumpChargeAddAttack.m_vWeaponInfoList.empty() )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
	if ( !pTarget )
		return;

	D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetMidPositionByRate() - vPos;
	vDir.y = 0.0f;
	D3DXVec3Normalize( &vDir, &vDir );

	DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_JumpChargeAddAttack.m_vWeaponInfoList.front().m_iWeaponIdx;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = dwWeaponIndex;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;;
		kPacket << ST_CHARGE_ADD_ATTACK_FIRE;
		kPacket << vPos;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUrielJump::SetNextJumpItemState( ioBaseChar *pOwner, ioBaseChar *pWounder, bool bDefense, ChangeNextItemType type, ioWeapon* pWeapon, float fDamage )
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

void ioUrielJump::SetJumpChargeEndDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_UrielJumpState = UJS_CHARGE_ATTACK_END_DELAY;
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

void ioUrielJump::ProcessChargeAttackEndDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( IsEnableJumpChargeAddAttack( pOwner ) )
	{
		SetJumpChargeAddAttack( pOwner );
		return;
	}

	if ( m_dwJumpChargeEndDelayEndTime && m_dwJumpChargeEndDelayEndTime < FRAMEGETTIME() )
	{
		m_dwJumpChargeEndDelayEndTime = 0;
		pOwner->SetChangeJumppingState( m_fJumpChargeAttackEndJumpPowerRate, false, 0 );
		return;
	}
}

void ioUrielJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case ST_S_HOLD:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;
			pOwner->SetTargetRotToRotate( qtRotate, false );

			SetS_HoldState( pOwner );
		}
		break;
	case ST_DASH:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;
			pOwner->SetTargetRotToRotate( qtRotate, false );

			SetJumpDashState( pOwner );
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
	case ST_CHARGE_ADD_ATTACK:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;
			pOwner->SetTargetRotToRotate( qtRotate, false );

			SetJumpChargeAddAttack( pOwner );
		}
		break;
	case ST_SET_TARGET:
		rkPacket >> m_szTargetName;
		break;
	case ST_CHARGE_ADD_ATTACK_FIRE:
		{
			if ( m_JumpChargeAddAttack.m_vWeaponInfoList.empty() )
				return;
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
			if ( !pTarget )
				return;
			D3DXVECTOR3 vPos,vDir;
			rkPacket >> vPos;
			rkPacket >> vDir;

			DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();

			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();
			kFireTime.iAttributeIdx = m_JumpChargeAddAttack.m_vWeaponInfoList.front().m_iWeaponIdx;
			kFireTime.iResistanceIdx = 0;
			kFireTime.szWoundedAni  = "";
			kFireTime.dwWoundedAniDuration = 0;
			kFireTime.bLoopWoundedAni = false;
			kFireTime.dwWeaponIdx = dwWeaponIndex;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

			pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		}
		break;
	}
}

void ioUrielJump::SetLandingState( ioBaseChar *pOwner )
{
	ioExtendJump::SetLandingState( pOwner );
	m_UrielJumpState = UJS_NONE;
}
