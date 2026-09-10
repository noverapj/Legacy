#include "stdafx.h"
#include "ItemDefine.h"
#include "ioGenisysarmyItem.h"

ioGenisysarmyJump::ioGenisysarmyJump()
{	
	m_iCurComboCnt			= 0;
	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;
	m_dwEnableChargeAttack	= 0;
	m_ChargeJumpState		= CJS_NONE;
	m_dwChargingStartTime	= 0;
	m_bReserveJumpAttack	= false;
}

ioGenisysarmyJump::ioGenisysarmyJump( const ioGenisysarmyJump &rhs )
	: ioExtendJump( rhs ),
	m_dwChargingTime( rhs.m_dwChargingTime ),
	m_ChargingAni( rhs.m_ChargingAni ),
	m_AttackAttribute( rhs.m_AttackAttribute ),
	m_iMaxComboCnt( rhs.m_iMaxComboCnt),
	m_fJumpSpeedRateFR( rhs.m_fJumpSpeedRateFR ),
	m_fJumpSpeedRateSI( rhs.m_fJumpSpeedRateSI ),
	m_fJumpSpeedRateBG( rhs.m_fJumpSpeedRateBG ),
	m_bJumpChargeEnable( rhs.m_bJumpChargeEnable ),			//	점프 차지 사용여부 2020-07-08
	m_dwJumpKeyChargingTime( rhs.m_dwJumpKeyChargingTime )	//	점프 차지 시간 2020-07-08
{
	m_iCurComboCnt			= 0;
	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;
	m_dwEnableChargeAttack	= 0;
	m_ChargeJumpState		= CJS_NONE;
	m_dwChargingStartTime	= 0;
	m_bReserveJumpAttack	= false;
}

ioExtendJump* ioGenisysarmyJump::Clone()
{
	return new ioGenisysarmyJump( *this );
}

ioGenisysarmyJump::~ioGenisysarmyJump()
{
}

JumpType ioGenisysarmyJump::GetType() const
{
	return JT_GENISYSARMY_JUMP;
}

void ioGenisysarmyJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// 기본 점프 차징
	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );

	// 기본 점프 차징 준비 애니
	m_ChargingAni.Init();
	LoadAttackAttribute( "charging_jump_ani", m_ChargingAni, rkLoader );

	// 기본 점프 공격
	m_iMaxComboCnt = rkLoader.LoadInt_e( "jump_max_combo", 0 );
	LoadAttackAttribute( "jump_attack", m_AttackAttribute, rkLoader );

	// 점프 이동, 타켓
	m_fJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	// 점프 차지 사용여부 와 시간 2020-07-08
	m_bJumpChargeEnable		= rkLoader.LoadBool_e( "jump_charge_enable", false );
	m_dwJumpKeyChargingTime = (DWORD)rkLoader.LoadInt_e( "jump_key_charging_time", 0 );
}

void ioGenisysarmyJump::InitJump()
{

}

void ioGenisysarmyJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	pOwner->SetUsedFlashJump( false );

	m_bUseExtendJump		= false;
	m_iCurComboCnt			= 0;
	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;
	m_dwEnableChargeAttack	= 0;
	m_ChargeJumpState		= CJS_NONE;
	m_dwChargingStartTime	= 0;
	m_bReserveJumpAttack	= false;
}

void ioGenisysarmyJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
	{
		return;
	}

	
	DWORD dwCurTime = FRAMEGETTIME();

	// 기본 공격 
	if( m_bReserveJumpAttack )	// <---- if( m_bReserveJumpAttack ) 사용하지 않음 
	{
		SetChargingState( pOwner );
		return;
	}
	else if( pOwner->IsAttackKeyDown() )
	{		
		SetChargingState( pOwner );
		return;
	}
}

void ioGenisysarmyJump::ProcessLanding( ioBaseChar *pOwner )
{
}

void ioGenisysarmyJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return;

	switch( m_ChargeJumpState )
	{
		case CJS_NONE:
		{
			break;
		}
		case CJS_CHARGE:
		{
			ProcessCharging( pOwner );
			break;
		}
		case CJS_NORMAL_ATTACK:
		{
			ProcessComboJumpAttack( pOwner );
			break;
		}
		case CJS_CHARGING_ATTACK:
		{
			ioGenisysarmyItem *pGenisysarmyItem = ToGenisysarmyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( !pGenisysarmyItem )	{	return;		}

			DWORD dwInput_Dash_S_Star = 0;
			pGenisysarmyItem->ProcessComboChargingJumpAttack( pOwner, dwInput_Dash_S_Star );

			break;
		}
	}
}

void ioGenisysarmyJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )	{	return;	}
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
		case CJS_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChargingState( pOwner );
			break;
		}
		
		case CJS_NORMAL_ATTACK:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			SetComboJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
			break;
		}

		case CJS_CHARGING_ATTACK:
		{
			ioGenisysarmyItem *pGenisysarmyItem = ToGenisysarmyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( !pGenisysarmyItem )
				return;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ioHashString szTarget;
			bool bAimedBall;
			rkPacket >> szTarget;
			rkPacket >> bAimedBall;

			m_ChargeJumpState = CJS_CHARGING_ATTACK;
			m_bUseExtendJump = true;
			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );
			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
			m_bReserveJumpAttack = false;
			pOwner->SetJumpAttackState();
			DWORD dwInput_Dash_S_StartTime = 0;
			pGenisysarmyItem->ApplyJComboChargingJumpAttack( pOwner, dwInput_Dash_S_StartTime, szTarget, bAimedBall );
			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	}
}

void ioGenisysarmyJump::OnEndExtendJump( ioBaseChar *pOwner )
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 기본 함수 정의
void ioGenisysarmyJump::SetChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) {	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) {	return;	}

	pOwner->ClearAttackFireTimeAndSkill();

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_ChargingAni.m_AttackAnimation );
	float fTimeRate = (float)m_dwChargingTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_bUseExtendJump		= true;
	m_ChargeJumpState		= CJS_CHARGE;
	m_dwChargingStartTime	= dwCurTime;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeJumpState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGenisysarmyJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
	{
		return;
	}	

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown()  )
	{
		if( m_dwChargingStartTime > 0 && m_dwChargingStartTime + m_dwChargingTime < dwCurTime )
		{
			SetComboChargingJumpAttack( pOwner );
			return;
		}
	}
	else
	{
		SetComboJumpAttack( pOwner );
	}
}

void ioGenisysarmyJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	CheckAirJump( pOwner );

	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
	{
		if ( m_bReserveJumpAttack )
		{	
			SetChargingState( pOwner );
			return;
		}
	}
}

void ioGenisysarmyJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeJumpState )
	{
		case CJS_NONE:
		case CJS_NORMAL_ATTACK:
		{
			break;
		}
		
		default:
			return;
	}

	if( m_bReserveJumpAttack )
	{
		return;
	}


	if( !COMPARE( m_iCurComboCnt, 0, m_iMaxComboCnt ) )
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

void ioGenisysarmyJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	if( !COMPARE( m_iCurComboCnt, 0, m_iMaxComboCnt ) )
	{
		return;
	}

	DWORD dwCurTime		= FRAMEGETTIME();
	m_ChargeJumpState	= CJS_NORMAL_ATTACK;

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
	{
		pOwner->InitActionStopDelay( true );
	}
	else
	{
		pOwner->InitActionStopDelay( false );
	}

	int iAniID			= pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	float fTimeRate		= m_AttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay	= m_AttackAttribute.m_dwPreDelay;

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
		kPacket << m_ChargeJumpState;
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

// 점프 D~
void ioGenisysarmyJump::SetComboChargingJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	ioGenisysarmyItem *pGenisysarmyItem = ToGenisysarmyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pGenisysarmyItem )	{	return;		}

	m_ChargeJumpState		= CJS_CHARGING_ATTACK;
	m_bUseExtendJump		= true;
	m_bReserveJumpAttack	= false;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	// 상대방 
	ioHashString szTarget;
	bool bAimedBall;	
	DWORD dwInput_Dash_S_StartTime = 0;
	pGenisysarmyItem->SetComboChargingJumpAttack( pOwner, dwInput_Dash_S_StartTime, szTarget, bAimedBall );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeJumpState;
		kPacket << pOwner->GetTargetRot();
		kPacket << szTarget;
		kPacket << bAimedBall;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
// GetMoveDir(), GetTargetRot()
float ioGenisysarmyJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

	if( m_bUseExtendJump )
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

// 점프차징 시간 2020-07-08
DWORD ioGenisysarmyJump::GetChargingTime( ioBaseChar *pChar )
{
	if( m_bJumpChargeEnable )
	{
		return m_dwJumpKeyChargingTime;
	}

	return 0;
}
