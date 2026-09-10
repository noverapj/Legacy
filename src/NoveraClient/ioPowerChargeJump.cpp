

#include "stdafx.h"

#include "ioPowerChargeJump.h"
#include "ioBaseChar.h"

ioPowerChargeJump::ioPowerChargeJump()
{
	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;
	m_bUseExtendJumpAttack = false;

	m_dwNormalAttackEndTime = 0;

	m_dwDashStartTime = 0;
	m_dwDashEndTime = 0;
}

ioPowerChargeJump::ioPowerChargeJump( const ioPowerChargeJump &rhs )
: ioExtendJump( rhs ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
 m_dwChargingTime( rhs.m_dwChargingTime ),
 m_szChargingAni( rhs.m_szChargingAni ),
 m_szGahteringAni( rhs.m_szGahteringAni ),
 m_JumpAttack( rhs.m_JumpAttack ),
 m_ExtendJumpAttack( rhs.m_ExtendJumpAttack ),
 m_bEnableReservedSliding( rhs.m_bEnableReservedSliding ),
 m_bUseLandJumpAttack( rhs.m_bUseLandJumpAttack ),
 m_fGravityAmt( rhs.m_fGravityAmt ),
  m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
 m_DashJumpAttack( rhs.m_DashJumpAttack ),
 m_DashLandAni( rhs.m_DashLandAni ),
 m_fDashLandAniRate( rhs.m_fDashLandAniRate ),
 m_fDashGravityAmt( rhs.m_fDashGravityAmt ),
 m_DashType( rhs.m_DashType ),
 m_bUseNewJumpTypeDash( rhs.m_bUseNewJumpTypeDash )
{
	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;
	m_bUseExtendJumpAttack = false;

	m_dwNormalAttackEndTime = 0;

	m_dwDashStartTime = 0;
	m_dwDashEndTime = 0;
}

ioExtendJump* ioPowerChargeJump::Clone()
{
	return new ioPowerChargeJump( *this );
}

ioPowerChargeJump::~ioPowerChargeJump()
{
}

JumpType ioPowerChargeJump::GetType() const
{
	return JT_POWER_CHARGE_JUMP;
}

void ioPowerChargeJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//
	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	// charging
	m_dwChargingTime = rkLoader.LoadInt_e( "jump_charging_time", 0 );
	rkLoader.LoadString_e( "jump_attack_ready_animation", "", szBuf, MAX_PATH );
	m_szChargingAni = szBuf;

	// gathering
	rkLoader.LoadString_e( "jump_gathering_animation", "", szBuf, MAX_PATH );
	m_szGahteringAni = szBuf;

	// JumpAttack
	m_bUseLandJumpAttack = rkLoader.LoadBool_e( "use_land_jump_attack", false );

	m_fGravityAmt = rkLoader.LoadFloat_e( "land_jump_attack_gravity", 0.0f );

	m_JumpAttack.Init();
	LoadAttackAttribute_e( "jump_attack", m_JumpAttack, rkLoader );

	m_ExtendJumpAttack.Init();
	LoadAttackAttribute_e( "extend_jump_attack", m_ExtendJumpAttack, rkLoader );
	m_bEnableReservedSliding = rkLoader.LoadBool_e( "enable_reserved_sliding", false );

	// Dash JumpAttack
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_DashType = (LocalDashType)rkLoader.LoadInt_e( "jump_dash_type", LDT_GRAVITY );

	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "dash_jump_attack", m_DashJumpAttack, rkLoader );

	rkLoader.LoadString_e( "jump_dash_land_ani", "", szBuf, MAX_PATH );
	m_DashLandAni = szBuf;
	m_fDashLandAniRate = rkLoader.LoadFloat_e( "jump_dash_land_ani_rate", FLOAT1 );

	m_fDashGravityAmt = rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );

	m_bUseNewJumpTypeDash = rkLoader.LoadBool_e( "use_new_jump_type_dash", false );
}

void ioPowerChargeJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_JumpState = PJS_NONE;
	m_dwChargingStartTime = 0;

	m_bUseExtendJump = false;
	m_bUseExtendJumpAttack = false;

	m_dwNormalAttackEndTime = 0;

	m_dwDashStartTime = 0;
	m_dwDashEndTime = 0;

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

bool ioPowerChargeJump::SetPowerChargeJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );

	int iAniID = pGrp->GetAnimationIdx( m_szChargingAni );
	float fTimeRate = (float)m_dwChargingTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->SetAutoTargetTrackingEndTime( dwCurTime );

	m_bUseExtendJump = true;
	m_bUseExtendJumpAttack = false;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_JumpState = PJS_CHARGING;
	m_dwChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)m_JumpState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioPowerChargeJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_JumpState )
	{
	case PJS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case PJS_GATHERING:
		ProcessGathering( pOwner );
		break;
	case PJS_NORMAL_ATTACK:
	case PJS_EXTEND_ATTACK:
		{
			CheckAirJump( pOwner );
		}
		break;
	}
}

void ioPowerChargeJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			//ChangeToGathering( pOwner );
			ChangeToPowerChargeJumpAttack( pOwner );
			return;
		}
	}
	else
	{
		ChangeToNormalJumpAttack( pOwner );
		return;
	}
}

void ioPowerChargeJump::ProcessGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToPowerChargeJumpAttack( pOwner );
	}
}

void ioPowerChargeJump::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_szGahteringAni );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_JumpState = PJS_GATHERING;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)m_JumpState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeJump::ChangeToNormalJumpAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_JumpAttack.m_AttackAnimation );
	float fTimeRate = m_JumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_JumpAttack.m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

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

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_JumpAttack.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	m_JumpState = PJS_NORMAL_ATTACK;
	m_bUseExtendJumpAttack = false;

	// For AirJumpPower
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;
	m_dwNormalAttackEndTime = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)m_JumpState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeJump::ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_ExtendJumpAttack.m_AttackAnimation );
	float fTimeRate = m_ExtendJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_ExtendJumpAttack.m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

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

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	if( !m_bUseLandJumpAttack )
	{
		pOwner->RefreshFireTimeList( iAniID, m_ExtendJumpAttack.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
	}

	if( m_bEnableReservedSliding )
	{
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );		
		pOwner->SetForcePowerAmt( 0.0f );

		pOwner->SetReservedSliding( m_ExtendJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	}

	m_JumpState = PJS_EXTEND_ATTACK;
	m_bUseExtendJumpAttack = true;

	// For AirJumpPower
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)m_JumpState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case PJS_CHARGING:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			pOwner->SetAutoTarget( ATT_JUMP );

			int iAniID = pGrp->GetAnimationIdx( m_szChargingAni );
			float fTimeRate = (float)m_dwChargingTime / pGrp->GetAnimationFullTime( iAniID );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

			pOwner->SetAutoTargetTrackingEndTime( dwCurTime );

			m_bUseExtendJump = true;
			m_bUseExtendJumpAttack = false;

			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
			pOwner->SetJumpAttackState();

			m_JumpState = PJS_CHARGING;
			m_dwChargingStartTime = dwCurTime;

			pOwner->ApplyAutoTargetInfo( rkPacket );

		}
		break;
	case PJS_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case PJS_NORMAL_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			ChangeToNormalJumpAttack( pOwner );
		}
		break;
	case PJS_EXTEND_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			ChangeToPowerChargeJumpAttack( pOwner );
		}
		break;
	case PJS_DASH:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			pOwner->SetMoveDirection( vDir );

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRotAndMoveDirChange( qtRotate );

			ioEntityGroup *pGrp = pOwner->GetGroup();
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
			//
			m_dwDashStartTime = dwCurTime;
			m_dwDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

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
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );

			m_JumpState = PJS_DASH;
			pOwner->SetComboDashState();

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if( m_bUseNewJumpTypeDash )
			{
				SetNewJumpTypeDashAttack( pOwner, m_dwDashEndTime, dwKeyReserveTime );
			}
		}
		break;
	}
}

void ioPowerChargeJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_bUseExtendJumpAttack = false;

	m_dwDashStartTime = 0;
	m_dwDashEndTime = 0;

	m_dwNormalAttackEndTime = 0;

	m_JumpState = PJS_NONE;
}

bool ioPowerChargeJump::IsUseLandJumpAttack()
{
	if( m_bUseLandJumpAttack && m_bUseExtendJumpAttack )
		return true;

	return false;
}

bool ioPowerChargeJump::IsUseExtendJumpAttack()
{
	return m_bUseExtendJumpAttack;
}

float ioPowerChargeJump::GetCurGravityAmt()
{
	if( m_JumpState == PJS_EXTEND_ATTACK )
	{
		if( m_bUseLandJumpAttack )
			return m_fGravityAmt;
	}
	else if( m_JumpState == PJS_DASH )
	{
		if( m_DashType == LDT_GRAVITY )
			return m_fDashGravityAmt;
		else if( m_DashType == LDT_NO_GRAVITY )
			return -FLOAT1;
	}

	return 0.0f;
}

bool ioPowerChargeJump::IsCanDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

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

	if( m_JumpState != PJS_NONE )
		return false;

	if( !pOwner->IsCanJumpAttackEnableTime() )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

void ioPowerChargeJump::SetDash( ioBaseChar *pOwner, float fHeightGap )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

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
	//
	m_dwDashStartTime = dwCurTime;
	m_dwDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_JumpState = PJS_DASH;
	pOwner->SetComboDashState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_JumpState;

		kPacket << vMoveDir;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( m_bUseNewJumpTypeDash )
	{
		SetNewJumpTypeDashAttack( pOwner, m_dwDashEndTime, dwKeyReserveTime );
		return;
	}
}

void ioPowerChargeJump::SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime )
{
	if( !pOwner ) return;

	m_bUseExtendJump = false;
	m_JumpState = PJS_NEW_JUMP;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, m_fDashGravityAmt );
}

bool ioPowerChargeJump::IsEnableJumpLandEndDash()
{
	if( m_JumpState == PJS_DASH || m_JumpState == PJS_DASH_END )
		return true;

	return false;
}

bool ioPowerChargeJump::IsDashState()
{
	switch( m_JumpState )
	{
	case PJS_DASH:
	case PJS_DASH_END:
		return true;
	}

	return false;
}

void ioPowerChargeJump::ProcessDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_DashType != LDT_NO_GRAVITY )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_JumpState )
	{
	case PJS_DASH:
		if( m_dwDashEndTime > 0 && m_dwDashEndTime < dwCurTime )
		{
			m_dwDashEndTime = 0;
			m_JumpState = PJS_DASH_END;

			int iAniID = pOwner->GetJumppingAnimationIdx();
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
			}
		}
		break;
	case PJS_DASH_END:
		break;
	}
}

void ioPowerChargeJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( m_bEnableJumpDash && IsCanDash( pOwner, fHeightGap ) )
	{
		SetDash( pOwner, fHeightGap );
		return;
	}
}

bool ioPowerChargeJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return m_bEnableJumpDash;
}

DWORD ioPowerChargeJump::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}


