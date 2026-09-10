#include "StdAfx.h"
#include "ioStriderJump.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioStriderItem.h"

ioStriderJump::ioStriderJump(void)
{
	ClearData();
}

ioStriderJump::ioStriderJump( const ioStriderJump &rhs ):
ioExtendJump( rhs ),
m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),

m_ChargingAniList( rhs.m_ChargingAniList ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_ComboAttackList( rhs.m_ComboAttackList ),

//Dash
m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
m_DashJumpAttack( rhs.m_DashJumpAttack ),
m_fDashJumpAttackGravity( rhs.m_fDashJumpAttackGravity )

{
	ClearData();
}

ioStriderJump::~ioStriderJump(void)
{
}

ioExtendJump* ioStriderJump::Clone()
{
	return new ioStriderJump( *this );
}

JumpType ioStriderJump::GetType() const
{
	return JT_STRIDER_JUMP;
}

void ioStriderJump::LoadProperty( ioINILoader &rkLoader )
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

	m_iMaxJumpCnt = max( 1, rkLoader.LoadInt_e( "max_jump_cnt", 1 ) );

	//Charging
	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );
	int iMaxChargeAni = rkLoader.LoadInt_e( "charging_jump_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "charging_jump_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ChargingAniList.push_back( szBuf );
	}

	//Combo
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
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "jump_dash_attack", m_DashJumpAttack, rkLoader );
	m_fDashJumpAttackGravity = rkLoader.LoadFloat_e( "jump_dash_attack_gravity_amt", 0.0f );
}

void ioStriderJump::ClearData()
{
	m_StriderJumpState = SJS_NONE;
	m_bReserveJumpAttack = false;

	m_iCurJumpCnt = 1;
	m_bUsedDoubleJump = false;
	m_dwChargingStartTime = 0;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_dwInputSpecialDashTime = 0;

}

void ioStriderJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	ClearData();
	pOwner->SetUsedFlashJump( false );
	m_bUseExtendJump = false;
	m_dwReserveEnableTime = 0;
}

void ioStriderJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
		return;

	if( m_bEnableJumpDash && IsNewJumpDashAttack( pOwner, fHeightGap ) )
	{
		SetNewJumpDashAttack( pOwner );
		return;
	}

	if( IsCanDoubleJump( pOwner, fHeightGap ) && m_fDoubleJumpPower > 0.0f )
	{
		SetDoubleJump( pOwner );
		return;
	}

	if( m_bReserveJumpAttack )
	{
		SetChargingState( pOwner );
		return;
	}
}

bool ioStriderJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
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

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	switch( m_StriderJumpState )
	{
	case SJS_NONE:
	case SJS_DOUBLE_JUMP:
		break;
	default:
		return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) 
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

void ioStriderJump::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	SetDoubleJumpAni( pOwner );

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

	pOwner->ClearSylphid();

	m_StriderJumpState = SJS_DOUBLE_JUMP;

	m_bUsedDoubleJump = true;
	m_bUseExtendJump = true;
	m_iCurJumpCnt++;

	g_TutorialMgr.ActionDoubleJump();

	m_dwChargingStartTime = 0;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DOUBLE_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << vJumpDir;
		kPacket << static_cast<BYTE>(m_StriderJumpState);
		kPacket << m_fDoubleJumpPower;
		kPacket << m_fDoubleJumpForce;
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

void ioStriderJump::SetDoubleJumpAni( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

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

void ioStriderJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	if( m_bReserveJumpAttack )
		return;

	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
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

void ioStriderJump::SetChargingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;


	if( m_ChargingAniList.empty() )
		return;

	pOwner->ClearSylphid();

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	ioHashString szCurChargingAni;
	int iChargingCnt = m_ChargingAniList.size();
	if( COMPARE( m_iCurComboCnt, 0, iChargingCnt ) )
		szCurChargingAni = m_ChargingAniList[m_iCurComboCnt];
	else
		szCurChargingAni = m_ChargingAniList[0];

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );
	float fTimeRate = (float)m_dwChargingTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_StriderJumpState = SJS_CHARGE;
	m_dwChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<BYTE>(m_StriderJumpState);
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStriderJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	switch( m_StriderJumpState )
	{
	case SJS_NORMAL_ATTACK:
		ProcessComboJumpAttack( pOwner, fHeightGap );
		break;
	case SJS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case SJS_DASH_ATTACK:
		ProcessNewJumpDashAttack( pOwner );
		break;
	}
}

void ioStriderJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioStriderItem *pStriderItem = ToStriderItem( pOwner->GetWeapon() );
	if ( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() &&
		pStriderItem && pStriderItem->IsStriderChargeAttackGauge() )
	{
		if( m_dwChargingStartTime + m_dwChargingTime < dwCurTime )
			SetStriderChargeAttack( pOwner );
	}
	else
		SetComboJumpAttack( pOwner );
}

void ioStriderJump::SetStriderChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	ioStriderItem *pStriderItem = ToStriderItem( pOwner->GetWeapon() );
	if ( pStriderItem )
		pStriderItem->SetStriderChargeAttackState( pOwner, true );

	m_dwChargingStartTime = 0;
}

void ioStriderJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	m_StriderJumpState = SJS_NORMAL_ATTACK;
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
		kPacket << static_cast<BYTE>(m_StriderJumpState);
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

void ioStriderJump::ProcessComboJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	CheckAirJump( pOwner );

	DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_bEnableJumpDash && IsNewJumpDashAttack( pOwner, fHeightGap ) )
	{
		SetNewJumpDashAttack( pOwner );
		return;
	}
	else if( dwCurTime >= m_dwEnableComboTime_S + dwLooseTime )
	{
		if ( m_bReserveJumpAttack )
		{
			SetChargingState( pOwner );
			return;
		}
	}
}


bool ioStriderJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return m_bEnableJumpDash;
}

bool ioStriderJump::IsNewJumpDashAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
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
	if( dwMoveStartTime == 0 )
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	ioStriderItem *pStriderItem = ToStriderItem( pOwner->GetWeapon() );
	if ( !pStriderItem )
		return false;

	if ( !pStriderItem->IsStriderJumpDashAttackGauge() )
		return false;

	return true;
}

void ioStriderJump::SetNewJumpDashAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioStriderItem *pStriderItem = ToStriderItem( pOwner->GetWeapon() );
	if ( !pStriderItem )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	if ( m_DashJumpAttack.m_AttackAnimation.IsEmpty() )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	m_StriderJumpState = SJS_DASH_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimeRate = FLOAT1;
	if( m_DashJumpAttack.m_fAttackAniRate > 0.0f )
		fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	m_bUseExtendJump = false;

	pOwner->InitActionStopDelay( true );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	if ( pOwner->IsNeedProcess() )
		pOwner->ChangeDirectionByInputDir( false, false );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_DashJumpAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
	m_dwEnableComboTime_E = dwStartTime + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwInputSpecialDashTime = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if ( m_dwInputSpecialDashTime > 0 )
		m_dwInputSpecialDashTime += dwStartTime;
	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	
	pOwner->SetNewJumpAttackStateInfo( m_dwEnableComboTime_E, m_dwEnableComboTime_S, m_fDashJumpAttackGravity );
	pStriderItem->DecreaseJumpDashAttGauge();
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<BYTE>(m_StriderJumpState);
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStriderJump::ProcessNewJumpDashAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	CheckAirJump( pOwner );

	

	if ( m_dwInputSpecialDashTime && m_dwInputSpecialDashTime < dwCurTime )
	{
		ioStriderItem *pStriderItem = ToStriderItem( pOwner->GetWeapon() );
		if( pStriderItem && pStriderItem->IsConditionalSpecialDashReserve( pOwner ) && pOwner->IsDirKeyDoubleClick() 
			&& pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE)
		{
			pStriderItem->SetChangeConditionalSpecialDash( pOwner, pOwner->CheckCurDirKey(), ioAttackableItem::CCSD_JUMP );
			return;
		}
	}
	
	if ( IsCanNewJumpDashAddAttack( pOwner ) )
	{
		SetNewJumpDashAddAttack( pOwner );
		return;
	}
}

void ioStriderJump::SetNewJumpDashAddAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	ioStriderItem *pStriderItem = ToStriderItem( pOwner->GetWeapon() );
	if ( pStriderItem )
		pStriderItem->SetJumpDashAddAttackState( pOwner );
}

void ioStriderJump::SetJumpComboAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsBuffLimitAttack() )
		return;

	SetChargingState( pOwner );
}

void ioStriderJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	BYTE btType;
	rkPacket >> btType;

	if ( btType == SJS_CHARGE )
		SetChargingState( pOwner );
	else if( btType == SJS_DOUBLE_JUMP )
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
	else if ( btType == SJS_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurComboCnt;

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;

		D3DXVECTOR3 vMoveDir, vInputDir;
		rkPacket >> vMoveDir >> vInputDir;

		pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );
		pOwner->ApplyAutoTargetInfo( rkPacket );

		SetComboJumpAttack( pOwner );
	}
	else if ( btType == SJS_DASH_ATTACK )
	{
		D3DXVECTOR3 vDir;
		rkPacket >> vDir;

		pOwner->SetMoveDirection( vDir );

		D3DXQUATERNION qtRotate;
		rkPacket >> qtRotate;

		pOwner->SetTargetRotAndMoveDirChange( qtRotate );
		SetNewJumpDashAttack( pOwner );
	}
}

bool ioStriderJump::IsCanNewJumpDashAddAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();
	if( FRAMEGETTIME() < m_dwEnableComboTime_S + dwLooseTime )
		return false;

	ioStriderItem *pStriderItem = ToStriderItem( pOwner->GetWeapon() );
	if ( !pStriderItem )
		return false;

	if( !pOwner->IsAttackKey() )
		return false;

	if ( !pStriderItem->IsStriderJumpDashAddAttackGauge() )
		return false;

	return true;
}
