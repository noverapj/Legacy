
#include "StdAfx.h"

#include "ioNataJump.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioNataJump::ioNataJump(void)
{
	ClearData();
}

ioNataJump::ioNataJump( const ioNataJump &rhs ) 
	: ioExtendJump( rhs ),
	  m_dwFullJumpChargeTime( rhs.m_dwFullJumpChargeTime ),
	  m_ComboAttackList( rhs.m_ComboAttackList ),
	  m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
	  m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
	  m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
	  m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
	  m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
	  m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
	  m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
	  m_dwAttackKeyChargingTime( rhs.m_dwAttackKeyChargingTime ),
	  m_AttackKeyChargingAniList( rhs.m_AttackKeyChargingAniList ),
	  m_dwJumpKeyChargingTime( rhs.m_dwJumpKeyChargingTime ),
	  m_fJumpSpeedRateFR( rhs.m_fJumpSpeedRateFR ),
	  m_fJumpSpeedRateSI( rhs.m_fJumpSpeedRateSI ),
	  m_fJumpSpeedRateBG( rhs.m_fJumpSpeedRateBG ),
	  m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
	  m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
	  m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG )
{
	ClearData();
}

ioNataJump::~ioNataJump(void)
{
}

void ioNataJump::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	ioExtendJump::LoadProperty( rkLoader );
	m_dwFullJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

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
	
	m_dwAttackKeyChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );
	int iMaxChargeAni = rkLoader.LoadInt_e( "charging_jump_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "charging_jump_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackKeyChargingAniList.push_back( szBuf );
	}

	m_dwJumpKeyChargingTime = (DWORD)rkLoader.LoadInt_e( "jump_key_charging_time", 0 );

	int iMaxCombo = rkLoader.LoadInt_e( "jump_max_combo", 0 );
	m_ComboAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboAttackList.push_back( kAttribute );
	}

	m_fDoubleJumpSpeedRateFR = rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI = rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG = rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

	m_fJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );
}

ioExtendJump* ioNataJump::Clone()
{
	return new ioNataJump( *this );
}

JumpType ioNataJump::GetType() const
{
	return JT_NATA_JUMP;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioNataJump::GetChargingTime( ioBaseChar *pChar )
{
	return m_dwFullJumpChargeTime;
}

void ioNataJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_NataJumpState )
	{
	case NJS_NONE:
	case NJS_NORMAL:
	case NJS_DOUBLE_JUMP:
	case NJS_A_CHARGE:
		break;
	default:
		return;
	}

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

void ioNataJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_NataJumpState = NJS_NONE;
	pOwner->SetUsedFlashJump( false );

	m_bUseExtendJump = false;
	m_bUsedDoubleJump = false;
	//m_bUsedChargeAttack = false;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_dwChargingStartTime = 0;
	m_dwReserveEnableTime = 0;
	m_iCurJumpCnt = 1;
}

void ioNataJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
		return;

	if ( m_dwChargingStartTime > 0 && CheckJumpCharging( pOwner ) )
		return;
	
	if( m_bReserveJumpAttack )
	{
		SetAttackChargingState( pOwner );
		return;
	}

	if( IsCanJumpCharge( pOwner, fHeightGap ) )
	{
		SetJumpChargingState( pOwner );
		return;
	}
}

void ioNataJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	switch( m_NataJumpState )
	{
	case NJS_NORMAL:
		ProcessComboJumpAttack( pOwner );
		break;
	case NJS_D_CHARGE:
		ProcessAttackCharging( pOwner );
		break;
	case NJS_A_CHARGE:
		CheckJumpCharging( pOwner );
		break;
	}
}

void ioNataJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case NJS_NORMAL:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			
			SetComboJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case NJS_D_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetAttackChargingState( pOwner );
		}
		break;
	case NJS_A_CHARGE:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetJumpChargingState( pOwner );
		}
		break;
	case NJS_DOUBLE_JUMP:
		{
			float fJumpPower, fJumpForce;
			D3DXQUATERNION qtRotate;

			rkPacket >> fJumpPower >> fJumpForce;
			rkPacket >> qtRotate;

			if( fJumpPower >= 0.0f )
			{
				pOwner->ClearSylphid();
				m_NataJumpState = NJS_DOUBLE_JUMP;

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
	}
}

float ioNataJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

	if( m_NataJumpState == NJS_DOUBLE_JUMP )
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataJump::ClearData()
{
	m_NataJumpState = NJS_NONE;
	m_bReserveJumpAttack = false;

	m_iCurJumpCnt = 1;
	m_bUsedDoubleJump = false;
	m_dwChargingStartTime = 0;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;
}

void ioNataJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	CheckAirJump( pOwner );

	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime >= m_dwEnableComboTime_S + dwLooseTime )
	{
		if ( m_bReserveJumpAttack )
		{	
			SetAttackChargingState( pOwner );
			return;
		}

		int iChargingCnt = m_AttackKeyChargingAniList.size();
		if( COMPARE( m_iCurComboCnt, 0, iChargingCnt ) )
		{
			float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
			if( IsCanJumpCharge( pOwner, fHeightGap ) )
			{
				SetJumpChargingState( pOwner );
				return;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNataJump::SetAttackChargingState( ioBaseChar *pOwner )
{
	if( !pOwner || m_AttackKeyChargingAniList.empty() ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	ioHashString szCurChargingAni;
	int iChargingCnt = m_AttackKeyChargingAniList.size();
	if( COMPARE( m_iCurComboCnt, 0, iChargingCnt ) )
		szCurChargingAni = m_AttackKeyChargingAniList[m_iCurComboCnt];
	else
		szCurChargingAni = m_AttackKeyChargingAniList[0];

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );
	float fTimeRate = (float)m_dwAttackKeyChargingTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	
	pOwner->ClearSylphid();
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwChargingStartTime = dwCurTime;

	m_NataJumpState = NJS_D_CHARGE;
	m_bUseExtendJump = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_NataJumpState;
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataJump::ProcessAttackCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsAttackKeyDown() && pOwner->IsEnableNataSpecialCombo( false, false, true ) )
	{
		if( m_dwChargingStartTime + m_dwAttackKeyChargingTime < dwCurTime )
			ChangeToNataComboSpecialAttack( pOwner );
	}
	else
		SetComboJumpAttack( pOwner );
}

void ioNataJump::ChangeToNataComboSpecialAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	pOwner->SetNataSpecialComboState( false, false, true );

	m_dwChargingStartTime = 0;
}

void ioNataJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;
	
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

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
		
	m_NataJumpState = NJS_NORMAL;
	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;

	pOwner->ClearSylphid();
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
	
	if( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsCanRotateJumpAttack() )
		{
			ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
			if ( pOwner->IsSettedDirection() )
			{
				eNewDirKey = pOwner->CheckCurDirKey();
				pOwner->SetTargetRotToDirKey( eNewDirKey );
				pOwner->SetWorldOrientation( pOwner->GetTargetRot() );
			}
		}
		
		pOwner->SetAutoTarget( ATT_JUMP );

		DWORD dwTrackingTime = dwStartTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << m_NataJumpState;
			kPacket << m_iCurComboCnt;
			kPacket << pOwner->GetWorldOrientation();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	
	m_iCurComboCnt++;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioNataJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	if( //!pOwner->IsJumpKeyPress() || // 이미 점프키를 누른 차징 상태에서의 전환이기 때문에 주석처리
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
	
	switch( m_NataJumpState )
	{
	case NJS_NONE:
	case NJS_DOUBLE_JUMP:
	case NJS_A_CHARGE:
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

void ioNataJump::SetDoubleJump( ioBaseChar *pOwner )
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

	m_NataJumpState = NJS_DOUBLE_JUMP;

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
		kPacket << m_NataJumpState;
		kPacket << m_fDoubleJumpPower;
		kPacket << m_fDoubleJumpForce;
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNataJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioNataJump::IsCanJumpCharge( ioBaseChar *pOwner, float fHeightGap )
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

	switch( m_NataJumpState )
	{
	case NJS_NONE:
	case NJS_DOUBLE_JUMP:
	case NJS_NORMAL:
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

void ioNataJump::SetJumpChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
		
	m_NataJumpState = NJS_A_CHARGE;
	m_bUseExtendJump = false;
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwChargingStartTime = dwCurTime;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_NataJumpState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioNataJump::CheckJumpCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ioNataItem *pNataItem = ToNataItem( pOwner->GetWeapon() );
	if ( pOwner->IsJumpKeyDown() && pOwner->IsEnableNataSpecialFly() )
	{
		if( m_dwChargingStartTime + m_dwJumpKeyChargingTime < dwCurTime )
		{
			pNataItem->SetNataFlySpecialState( pOwner, false );
			m_dwChargingStartTime = 0;
			return true;
		}
	}
	else
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( IsCanDoubleJump( pOwner, fHeightGap ) && m_fDoubleJumpPower > 0.0f )
		{
			SetDoubleJump( pOwner );
			return true;
		}
		else
		{
			m_NataJumpState = NJS_NONE;
			m_dwChargingStartTime = 0;
		}
	}

	return false;
}