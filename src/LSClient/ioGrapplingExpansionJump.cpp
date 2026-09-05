

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ItemDefine.h"

ioGrapplingExpansionJump::ioGrapplingExpansionJump()
{
	Init();

	m_bUsedChargeAttack			= false;
	m_bUsedStrikerChargeAttack	= false;
}

ioGrapplingExpansionJump::ioGrapplingExpansionJump( const ioGrapplingExpansionJump &rhs )
: ioExtendJump( rhs ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_fChargeJumpPower( rhs.m_fChargeJumpPower ),
m_fChargeGravityRate( rhs.m_fChargeGravityRate ),
m_fChargeBaseRange( rhs.m_fChargeBaseRange ),
m_ChargeAttackLandAni( rhs.m_ChargeAttackLandAni ),
m_fChargeAttackLandAniRate( rhs.m_fChargeAttackLandAniRate ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_bEnableHighJump( rhs.m_bEnableHighJump ),
m_stChargingAni( rhs.m_stChargingAni ),
m_AttackAttribute( rhs.m_AttackAttribute ),
m_ChargeAttackAttribute( rhs.m_ChargeAttackAttribute ),
m_ItemLandAttackInfo( rhs.m_ItemLandAttackInfo )
{
	Init();

	m_bUsedChargeAttack			= false;
	m_bUsedStrikerChargeAttack	= false;
	m_bUseHeighJump = false;
}

ioExtendJump* ioGrapplingExpansionJump::Clone()
{
	return new ioGrapplingExpansionJump( *this );
}

ioGrapplingExpansionJump::~ioGrapplingExpansionJump()
{
}

JumpType ioGrapplingExpansionJump::GetType() const
{
	return JT_GRAPPLING_EXPANSION_JUMP;
}

void ioGrapplingExpansionJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	LoadAttack( rkLoader );
	LoadChargeAttack( rkLoader );

	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "jump_charging_time", 0 );

	//
	m_fChargeJumpPower = rkLoader.LoadFloat_e( "jump_charge_jump_power", 0.0f );
	m_fChargeGravityRate = rkLoader.LoadFloat_e( "jump_charge_gravity_rate", FLOAT1 );
	m_fChargeBaseRange = rkLoader.LoadFloat_e( "jump_charge_base_range", 0.0f );

	rkLoader.LoadString_e( "jump_charge_land", "", szBuf, MAX_PATH );
	m_ChargeAttackLandAni = szBuf;
	m_fChargeAttackLandAniRate = rkLoader.LoadFloat_e( "jump_charge_land_time_rate", FLOAT1 );

	m_bEnableHighJump = rkLoader.LoadBool_e( "enable_high_jump", false );
	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );

	rkLoader.LoadString( "charging_ani", "", szBuf, MAX_PATH );
	m_stChargingAni = szBuf;

	//jumpAttack
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
}


void ioGrapplingExpansionJump::LoadAttack( ioINILoader &rkLoader )
{
	//char szBuf[MAX_PATH];
	/*int iMaxCombo = rkLoader.LoadInt_e( "jump_max_combo", 0 );

	m_ComboAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboAttackList.push_back( kAttribute );
	}*/

	LoadAttackAttribute( "jump_attack", m_AttackAttribute, rkLoader );
}


void ioGrapplingExpansionJump::Init()
{
	m_bUseExtendJump			= false;
	m_bUsedComboAttack			= false;
	m_bReserveJumpAttack		= false;
	m_bUseHeighJump				= false;
	m_bUseNormalJumpAttack		= false;

	m_dwChargingStartTime		= 0;
}

void ioGrapplingExpansionJump::LoadChargeAttack( ioINILoader &rkLoader )
{
	LoadAttackAttribute( "jump_charge_attack", m_ChargeAttackAttribute, rkLoader );
}

void ioGrapplingExpansionJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	Init();
	m_bUseExtendJump = false;
	m_bUseHeighJump  = false;

	if( m_bEnableHighJump && bFullTime )
	{
		m_bUseExtendJump = true;
		float fJumpPower = pOwner->GetJumpPower();
		fJumpPower *= m_fExtraJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

		m_bUseHeighJump = true;
	}

	m_charge_state = CES_NONE;
	//
	m_bUsedChargeAttack = false;
}


void ioGrapplingExpansionJump::ProcessLanding( ioBaseChar *pOwner )
{
}

void ioGrapplingExpansionJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
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

}

// combo
void ioGrapplingExpansionJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	if( m_bReserveJumpAttack )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();


	m_bReserveJumpAttack = true;
}

void ioGrapplingExpansionJump::SetNormalJumpAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearAttackFireTimeAndSkill();

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

	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	float fTimeRate = m_AttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackAttribute.m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_AttackAttribute.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );
	m_charge_state = CES_NORMAL;
	m_bUseNormalJumpAttack = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CES_NORMAL;
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

	pOwner->SetJumpAttackState();
}


void ioGrapplingExpansionJump::SetChargeJumpAttack( ioBaseChar *pOwner )
{
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
	ioGrapplingExpansionItem *pGrapplingExpansionItem = ToGrapplingExpansionItem( pWeapon );
	if( pGrapplingExpansionItem )
		pGrapplingExpansionItem->SetJumpLandAttackAttribute( m_ItemLandAttackInfo );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( m_ChargeAttackAttribute.m_AttackAnimation );
	float fTimeRate = m_ChargeAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargeAttackAttribute.m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_ChargeAttackAttribute.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	m_charge_state = CES_CHARGE_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CES_CHARGE_ATTACK;
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
	pOwner->SetJumpAttackState();
}


void ioGrapplingExpansionJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_bUseHeighJump )
		return;

	//DWORD dwCurTime = FRAMEGETTIME();

	switch( m_charge_state )
	{
	case CES_NORMAL:
		break;
	case CES_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CES_CHARGE_ATTACK:
		break;
	}
	
}



void ioGrapplingExpansionJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
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

		//if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
		{
			SetChargingState( pOwner );
		}
	}
}

void ioGrapplingExpansionJump::ProcessDelay( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

}


void ioGrapplingExpansionJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vMoveDir, vInputDir;

	int iType;
	rkPacket >> iType;
	switch( iType )
	{
	case CES_NORMAL:
		rkPacket >> qtRot;
		rkPacket >> vMoveDir >> vInputDir;

		pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

		SetNormalJumpAttack( pOwner );
		break;
	case CES_CHARGING:
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		SetChargingState( pOwner );
		break;
	case CES_CHARGE_ATTACK:
		rkPacket >> qtRot;
		rkPacket >> vMoveDir >> vInputDir;

		pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

		SetChargeJumpAttack( pOwner );
		break;
	}

}

void ioGrapplingExpansionJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_bUsedChargeAttack = false;
	m_bReserveJumpAttack = false;
	m_bUsedStrikerChargeAttack = false;
}


DWORD ioGrapplingExpansionJump::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

void ioGrapplingExpansionJump::SetChargingState( ioBaseChar *pOwner )
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
	float fTimeRate = (float)m_dwChargingTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_bUseExtendJump = true;

	m_charge_state = CES_CHARGING;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_dwChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CES_CHARGING;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingExpansionJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			SetChargeJumpAttack( pOwner );
		}
	}
	else
	{
		SetNormalJumpAttack( pOwner );
		return;
	}
}


bool ioGrapplingExpansionJump::IsChargeAttackState()
{
	return m_bUsedChargeAttack;
}


void ioGrapplingExpansionJump::SetReserveJumpAttack( ioBaseChar *pOwner )
{
	m_bReserveJumpAttack = true;
}

bool ioGrapplingExpansionJump::IsUsedHighJump()
{
	if( m_bEnableHighJump && m_bUseHeighJump )
		return true;

	return false;
}


bool ioGrapplingExpansionJump::IsJumpAttackReserve()
{
	return false;
}


bool ioGrapplingExpansionJump::IsEnableLandAttack()
{
	if( m_bUseHeighJump )
		return true;

	if( m_bUseNormalJumpAttack )
		return true;

	return false;
}
