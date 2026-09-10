

#include "stdafx.h"

#include "ioDoubleJump.h"
#include "ioBaseChar.h"

ioDoubleJump::ioDoubleJump()
{
	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_CurJumpState = DJS_NONE;
}

ioDoubleJump::ioDoubleJump( const ioDoubleJump &rhs )
: ioExtendJump( rhs ),
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
 m_fGravityAmt( rhs.m_fGravityAmt ),
 m_bUseLandJumpAttack( rhs.m_bUseLandJumpAttack ),
 m_DashJumpAttack( rhs.m_DashJumpAttack ),
 m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
 m_bUseJumpElementAtt( rhs.m_bUseJumpElementAtt ),
 m_MagicElementAtt( rhs.m_MagicElementAtt ),
 m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
 m_dwJumpChargeTime( rhs.m_dwJumpChargeTime )
{
	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_CurJumpState = DJS_NONE;
	m_bUsedHighJump = false;
}

ioExtendJump* ioDoubleJump::Clone()
{
	return new ioDoubleJump( *this );
}

ioDoubleJump::~ioDoubleJump()
{
}

JumpType ioDoubleJump::GetType() const
{
	return JT_DOUBLE_JUMP;
}

void ioDoubleJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_iMaxJumpCnt = rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt = max( 1, m_iMaxJumpCnt );

	m_fDoubleJumpPower = rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce = rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_dwDoubleJumpEnableTime = rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_fDoubleJumpEnableHeight = rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );

	m_fDoubleJumpSpeedRateFR = rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI = rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG = rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

	m_bUseLandJumpAttack = rkLoader.LoadBool_e( "use_land_jump_attack", false );
	m_fGravityAmt = rkLoader.LoadFloat_e( "land_jump_attack_gravity", 0.0f );

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

	// Dash JumpAttack
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );

	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "dash_jump_attack", m_DashJumpAttack, rkLoader );

	// elelment attack
	m_bUseJumpElementAtt = rkLoader.LoadBool_e( "use_jump_element_att", false );
	m_MagicElementAtt.Init();

	LoadAttackAttribute_e( "jump_fire_element_attack", m_MagicElementAtt.m_FireAttack, rkLoader );
	LoadAttackAttribute_e( "jump_ice_element_attack", m_MagicElementAtt.m_IceAttack, rkLoader );
	LoadAttackAttribute_e( "jump_light_element_attack", m_MagicElementAtt.m_LightAttack, rkLoader );

	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );
}

DWORD ioDoubleJump::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

void ioDoubleJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bUsedDoubleJump = false;
	m_bUseExtendJump = false;
	m_bUsedHighJump = false;
	m_iCurJumpCnt = 1;

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_CurJumpState = DJS_NONE;

	if( bFullTime && m_dwJumpChargeTime > 0 )
	{
		m_bUsedHighJump = true;
		m_bUseExtendJump = true;
		float fJumpPower = pOwner->GetJumpPower();
		fJumpPower *= m_fExtraJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	}
}

void ioDoubleJump::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwReserveEnableTime = 0;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
}

void ioDoubleJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	// DashCheck
	if( IsCanJumpDash( pOwner, fHeightGap ) )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();

		DWORD dwCurTime = FRAMEGETTIME();
		D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

		int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
		float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
		DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;

		m_bUseExtendJump = true;
		m_bUsedDoubleJump = false;

		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );

		pOwner->SetEnableHide( false );
		pOwner->SetNoWoundState( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

		pOwner->ChangeDirectionByInputDir( false );
		pOwner->ClearDirDoubleClick();

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

		m_dwActionEndTime = dwCurTime + dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

		m_dwReserveEnableTime = 0;
		CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

		pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

		m_CurJumpState = DJS_DASH;;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_JUMP_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << vMoveDir;
			kPacket << (int)m_CurJumpState;
			kPacket << false;					// dash end
			kPacket << pOwner->GetTargetRot();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	if( IsCanDoubleJump( pOwner, fHeightGap ) )
	{
		if( m_fDoubleJumpPower >= 0.0f )
		{
			m_CurJumpState = DJS_DOUBLE;

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
				kPacket << (int)m_CurJumpState;
				kPacket << m_fDoubleJumpPower;
				kPacket << m_fDoubleJumpForce;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	ProcessJumpDash( pOwner );
}

void ioDoubleJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_CurJumpState == DJS_ELEMENT_ATTACK )
	{
		CheckAirJump( pOwner );
	}
	else if( m_bUseJumpElementAtt && pOwner->IsNeedProcess() )
	{
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime < dwCurTime )
		{
			if( pOwner->IsAttackKey() )
			{
				ChangeToElementAttack( pOwner, ioAttackableItem::MET_FIRE );
				return;
			}
			else if( pOwner->IsDefenseKey() )
			{
				ChangeToElementAttack( pOwner, ioAttackableItem::MET_ICE );
				return;
			}
			else if( pOwner->IsJumpKey() )
			{
				ChangeToElementAttack( pOwner, ioAttackableItem::MET_LIGHT );
				return;
			}
		}
	}
}

bool ioDoubleJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;
	if( m_bUsedHighJump ) return false;

	if( !pOwner->IsJumpKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	|| 
		pOwner->GetUsedBuffJumpAttack() )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	// HARDCODE :: 더블점프 관련하여 max를 1로 하면 1단 점프만 넘어가게 처리. 문제생길경우 되돌릴것
	// 2012.10.29.
	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	/*
	if( m_bUsedDoubleJump )
	{
		if( m_iCurJumpCnt >= m_iMaxJumpCnt )
			return false;
	}
	*/
	//

	if( m_CurJumpState != DJS_NONE && m_CurJumpState != DJS_DOUBLE )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

void ioDoubleJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case DJS_DOUBLE:
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
		break;
	case DJS_DASH:
		{
			bool bEnd;
			rkPacket >> bEnd;

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			if( !bEnd )
			{
				pOwner->SetTargetRotAndMoveDirChange( qtRotate );

				ioEntityGroup *pGrp = pOwner->GetGroup();
				DWORD dwCurTime = FRAMEGETTIME();

				int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
				float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
				DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;

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

				m_dwActionEndTime = dwCurTime + dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

				DWORD dwTrackingTime = dwCurTime;
				dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
				pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

				pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

				pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

				m_CurJumpState = DJS_DASH;

				m_bUseExtendJump = true;
				m_bUsedDoubleJump = false;

				pOwner->SetEnableHide( false );
				pOwner->SetNoWoundState( false );
				pOwner->SetEnableChangeTeam( false );

				pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

				pOwner->SetGravityAmt( 0.0f );
				pOwner->SetForcePowerAmt( 0.0f );
				pOwner->SetCurJumpPowerAmt( 0.0f );

				pOwner->ApplyAutoTargetInfo( rkPacket );
			}
			else
			{
				pOwner->SetTargetRotAndMoveDirChange( qtRotate );
			}
		}
		break;
	case DJS_ELEMENT_ATTACK:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			int iSubType;
			rkPacket >> iSubType;

			pOwner->SetTargetRotAndMoveDirChange( qtRotate );

			ChangeToElementAttack( pOwner, iSubType );
		}
		break;
	}
}

void ioDoubleJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

float ioDoubleJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
{
	if( m_bUsedDoubleJump )
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

	return m_fJumpSpeedRate;
}

void ioDoubleJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUsedDoubleJump = false;
	m_bUseExtendJump = false;
}

bool ioDoubleJump::IsUseLandJumpAttack()
{
	if( m_bUseLandJumpAttack && m_bUsedDoubleJump )
		return true;

	return false;
}

float ioDoubleJump::GetCurGravityAmt()
{
	if( m_bUseLandJumpAttack && m_bUsedDoubleJump )
	{
		return m_fGravityAmt;
	}

	return 0.0f;
}

bool ioDoubleJump::IsJumpDashState()
{
	if( m_CurJumpState == DJS_DASH )
	{
		if( m_dwActionEndTime > 0 && m_dwActionEndTime >= FRAMEGETTIME() )
		{
			return true;
		}
	}

	return false;
}

bool ioDoubleJump::IsJumpAttackReserve()
{
	return m_bJumpAttackReserve;
}

void ioDoubleJump::CheckKeyReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;

	if( pOwner->IsAttackKey() && !m_bJumpAttackReserve && !pOwner->IsChatModeState( true ) )
	{
		m_bJumpAttackReserve = true;
	}
}

bool ioDoubleJump::IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap )
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

	if( pOwner->IsChatModeState( true ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if( m_CurJumpState != DJS_NONE && m_CurJumpState != DJS_DOUBLE )
		return false;

	if( !pOwner->IsCanJumpAttackEnableTime() )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

void ioDoubleJump::ProcessJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_CurJumpState != DJS_DASH ) return;
	if( m_bDashEnd ) return;
	if( m_dwActionEndTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwActionEndTime < dwCurTime )
	{
		m_dwActionEndTime = 0;

		int iAniID = pOwner->GetJumppingAnimationIdx();
		if( pOwner && iAniID != -1 )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				m_bDashEnd = true;

				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
			}
		}
	}
	else
	{
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			CheckKeyReserve( pOwner );
		}
	}
}

bool ioDoubleJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !m_bEnableJumpDash )
		return false;

	if( pOwner->GetJumpState() == JS_JUMPPING )
		return true;

	return false;
}

int ioDoubleJump::GetCurJumpState()
{
	return (int)m_CurJumpState;
}

bool ioDoubleJump::IsCanJumpAttack( ioBaseChar *pOwner )
{
	if( m_CurJumpState == DJS_DASH )
	{
		if( m_dwActionEndTime > 0 && m_dwActionEndTime >= FRAMEGETTIME() )
			return false;
	}

	return true;
}

void ioDoubleJump::ChangeToElementAttack( ioBaseChar *pOwner, int iSubType )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	AttackAttribute kAttackAttribute;

	switch( iSubType )
	{
	case ioAttackableItem::MET_FIRE:
		kAttackAttribute = m_MagicElementAtt.m_FireAttack;
		break;
	case ioAttackableItem::MET_ICE:
		kAttackAttribute = m_MagicElementAtt.m_IceAttack;
		break;
	case ioAttackableItem::MET_LIGHT:
		kAttackAttribute = m_MagicElementAtt.m_LightAttack;
		break;
	}

	int iAniID = pGrp->GetAnimationIdx( kAttackAttribute.m_AttackAnimation );
	float fTimeRate = kAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = kAttackAttribute.m_dwPreDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	m_CurJumpState = DJS_ELEMENT_ATTACK;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, kAttackAttribute.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_dwReserveEnableTime = 0;
	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_CurJumpState;
		kPacket << pOwner->GetTargetRot();
		kPacket << iSubType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
