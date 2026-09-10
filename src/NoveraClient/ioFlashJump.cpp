

#include "stdafx.h"

#include "ioFlashJump.h"
#include "ioBaseChar.h"

ioFlashJump::ioFlashJump()
{
	m_FlashJumpState = FJS_NONE;
	m_dwFlashDelayEndTime = 0;
	m_dwFlashEndTime = 0;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;
	m_iCurChangeJumpCnt = 0;
}

ioFlashJump::ioFlashJump( const ioFlashJump &rhs )
: ioExtendJump( rhs ),
 m_FlashDelayAni( rhs.m_FlashDelayAni ),
 m_fFlashDelayRate( rhs.m_fFlashDelayRate ),
 m_fFlashForce( rhs.m_fFlashForce ),
 m_fFlashGravityAmt( rhs.m_fFlashGravityAmt ),
 m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
 m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
 m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
 m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
 m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
 m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
 m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
 m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
 m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
 m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
 m_FlashJumpAttack( rhs.m_FlashJumpAttack ),
 m_bUseNewJumpTypeFlash( rhs.m_bUseNewJumpTypeFlash )
{
	m_FlashJumpState = FJS_NONE;
	m_dwFlashDelayEndTime = 0;
	m_dwFlashEndTime = 0;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;
	m_iCurChangeJumpCnt = 0;
}

ioExtendJump* ioFlashJump::Clone()
{
	return new ioFlashJump( *this );
}

ioFlashJump::~ioFlashJump()
{
}

JumpType ioFlashJump::GetType() const
{
	return JT_FLASH_JUMP;
}

void ioFlashJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "flash_jump_delay_animation", "", szBuf, MAX_PATH );
	m_FlashDelayAni = szBuf;

	m_fFlashDelayRate = rkLoader.LoadFloat_e( "flash_jump_delay_ani_rate", FLOAT1 );

	m_fFlashForce = rkLoader.LoadFloat_e( "flash_jump_force", 0.0f );
	m_fFlashGravityAmt = rkLoader.LoadFloat_e( "flash_jump_gravity_amt", FLOAT1 );


	// Double
	m_iMaxJumpCnt = rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt = max( 1, m_iMaxJumpCnt );

	m_fDoubleJumpPower = rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce = rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_dwDoubleJumpEnableTime = rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_fDoubleJumpEnableHeight = rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );

	m_fDoubleJumpSpeedRateFR = rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI = rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG = rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

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

	// Flash JumpAttack
	m_FlashJumpAttack.Init();
	LoadAttackAttribute_e( "flash_jump_attack", m_FlashJumpAttack, rkLoader );

	// etc
	m_bUseNewJumpTypeFlash = rkLoader.LoadBool_e( "use_new_jump_type_flash", false );
}

void ioFlashJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_FlashJumpState = FJS_NONE;
	m_dwFlashDelayEndTime = 0;
	m_dwFlashEndTime = 0;

	m_bUseExtendJump = false;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;
}

bool ioFlashJump::SetFlashJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

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

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	if( !m_FlashDelayAni.IsEmpty() )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pOwner->SetAutoTarget( ATT_JUMP );

		int iAniID = pGrp->GetAnimationIdx( m_FlashDelayAni );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fFlashDelayRate );

		dwDelayTime = pGrp->GetAnimationFullTime( iAniID ) * m_fFlashDelayRate;

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * m_fFlashDelayRate;

		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_bUseExtendJump = true;
	pOwner->SetUsedFlashJump( true );

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
	
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );

	m_FlashJumpState = FJS_DELAY;
	m_dwFlashDelayEndTime = dwCurTime + dwDelayTime;

	m_iCurChangeJumpCnt = pOwner->GetCurChangeJumppingCnt();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)m_FlashJumpState;
		kPacket << m_iCurJumpCnt;
		kPacket << m_iCurChangeJumpCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioFlashJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	switch( m_FlashJumpState )
	{
	case FJS_DELAY:
		ProcessDelay( pOwner );
		break;
	case FJS_FLASH:
		ProcessFlashMove( pOwner );
		break;
	case FJS_NORMAL:
		ProcessNormalMove( pOwner );
		break;
	}
}

void ioFlashJump::ProcessDelay( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFlashDelayEndTime > 0 && m_dwFlashDelayEndTime < dwCurTime )
	{
		m_FlashJumpState = FJS_FLASH;

		int iAniID = pGrp->GetAnimationIdx( m_FlashJumpAttack.m_AttackAnimation );
		float fTimeRate = m_FlashJumpAttack.m_fAttackAniRate;
		DWORD dwPreDelay = m_FlashJumpAttack.m_dwPreDelay;
		DWORD dwEndDelay = m_FlashJumpAttack.m_dwEndDelay;

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		

		pOwner->RefreshFireTimeList( iAniID,
			m_FlashJumpAttack.m_vWeaponInfoList,
			FTT_JUMP_ATTACK,
			fTimeRate,
			dwPreDelay );

		pOwner->IncreaseWeaponIndexBase();
		//

		m_dwFlashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

		DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

		if( m_bUseNewJumpTypeFlash )
		{
			SetNewJumpTypeFlashAttack( pOwner, m_dwFlashEndTime, dwKeyReserveTime );
			return;
		}
	}
}

void ioFlashJump::ProcessFlashMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFlashEndTime > 0 && m_dwFlashEndTime < dwCurTime )
	{
		m_FlashJumpState = FJS_NORMAL;

		pOwner->SetUsedFlashJump( false );
		pOwner->SetGravityAmt( 0.0f );

		pOwner->SetCharCollisionSkipTime( 0, 0 );
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}
}

void ioFlashJump::ProcessNormalMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;
}

void ioFlashJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case FJS_DOUBLE:
		{
			float fJumpPower, fJumpForce;
			D3DXQUATERNION qtRotate;

			rkPacket >> fJumpPower >> fJumpForce;
			rkPacket >> qtRotate;

			if( fJumpPower >= 0.0f )
			{
				m_FlashJumpState = FJS_DOUBLE;

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
	case FJS_DELAY:
		{
			rkPacket >> m_iCurJumpCnt;
			rkPacket >> m_iCurChangeJumpCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			if( !m_FlashDelayAni.IsEmpty() )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );

				pOwner->SetAutoTarget( ATT_JUMP );

				int iAniID = pGrp->GetAnimationIdx( m_FlashDelayAni );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fFlashDelayRate );

				dwDelayTime = pGrp->GetAnimationFullTime( iAniID ) * m_fFlashDelayRate;

				DWORD dwTrackingTime = dwCurTime;
				dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * m_fFlashDelayRate;

				pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
			}

			m_bUseExtendJump = true;
			pOwner->SetUsedFlashJump( true );

			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
			pOwner->SetJumpAttackState();

			pOwner->SetGravityAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );

			m_FlashJumpState = FJS_DELAY;
			m_dwFlashDelayEndTime = dwCurTime + dwDelayTime;

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	}
}

void ioFlashJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_FlashJumpState = FJS_NONE;
	pOwner->SetUsedFlashJump( false );
}

float ioFlashJump::GetCurFlashForceAmt()
{
	switch( m_FlashJumpState )
	{
	case FJS_FLASH:
		return m_fFlashForce;
	}

	return 0.0f;
}

float ioFlashJump::GetCurFlashGravityAmt()
{
	switch( m_FlashJumpState )
	{
	case FJS_FLASH:
		return m_fFlashGravityAmt;
	}

	return 0.0f;
}

bool ioFlashJump::IsNoFlashProcessMove()
{
	switch( m_FlashJumpState )
	{
	case FJS_DELAY:
		return true;
	}

	return false;
}

bool ioFlashJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( !pOwner->IsJumpKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->HasBuff( BT_BUFF_MULTI_FLY_JUMP ) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	if( m_FlashJumpState != FJS_NONE && m_FlashJumpState != FJS_DOUBLE )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

void ioFlashJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	// DoubleJumpCheck
	if( IsCanDoubleJump( pOwner, fHeightGap ) )
	{
		if( m_fDoubleJumpPower >= 0.0f )
		{
			m_FlashJumpState = FJS_DOUBLE;

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
				kPacket << (int)m_FlashJumpState;
				kPacket << m_fDoubleJumpPower;
				kPacket << m_fDoubleJumpForce;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioFlashJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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
	//
}

bool ioFlashJump::IsCanFlashJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	int iCurCnt = pOwner->GetCurChangeJumppingCnt();
	if( iCurCnt > 0 || m_iCurJumpCnt > 1 )
	{
		return true;
	}

	return false;
}

void ioFlashJump::SetNewJumpTypeFlashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime )
{
	if( !pOwner ) return;

	m_bUseExtendJump = false;
	m_FlashJumpState = FJS_NONE;
	pOwner->SetUsedFlashJump( false );

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, m_fFlashGravityAmt );

	D3DXVECTOR3 vForceDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vForceDir, &vForceDir );

	pOwner->SetForcePower( vForceDir, m_fFlashForce, FLOAT1, true );
}

bool ioFlashJump::IsFlashJumpState()
{
	switch( m_FlashJumpState )
	{
	case FJS_DELAY:
	case FJS_FLASH:
	case FJS_NORMAL:
		return true;
	}

	return false;
}



