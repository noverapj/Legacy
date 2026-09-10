

#include "stdafx.h"

#include "ioBadGuyJump.h"
#include "ioBaseChar.h"

ioBadGuyJump::ioBadGuyJump()
{
	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_CurJumpState = BJS_NONE;
}

ioBadGuyJump::ioBadGuyJump( const ioBadGuyJump &rhs )
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
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
m_DashAction( rhs.m_DashAction ),
m_fActionTimeRate( rhs.m_fActionTimeRate ),
m_vForceInfoList( rhs.m_vForceInfoList )
{
	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_CurJumpState = BJS_NONE;
}

ioExtendJump* ioBadGuyJump::Clone()
{
	return new ioBadGuyJump( *this );
}

ioBadGuyJump::~ioBadGuyJump()
{
}

JumpType ioBadGuyJump::GetType() const
{
	return JT_BADGUY_JUMP;
}

void ioBadGuyJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	// Double
	m_iMaxJumpCnt = rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt = max( 2, m_iMaxJumpCnt );

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


	// High
	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );


	// Dash
	rkLoader.LoadString_e( "dash_jump_action", "", szBuf, MAX_PATH );
	m_DashAction = szBuf;

	m_fActionTimeRate = rkLoader.LoadFloat_e( "dash_jump_action_time_rate", FLOAT1 );

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

void ioBadGuyJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bUsedDoubleJump = false;
	m_bUseExtendJump = false;
	m_iCurJumpCnt = 1;

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_CurJumpState = BJS_NONE;
}

void ioBadGuyJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	// DashCheck
	if( IsCanJumpDash( pOwner, fHeightGap ) )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID = pGrp->GetAnimationIdx( m_DashAction );
		if( pOwner && iAniID != -1 )
		{
			m_CurJumpState = BJS_DASH;
			m_bUseExtendJump = true;

			m_dwActionEndTime = 0;

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );

			pOwner->ChangeDirectionByInputDir( false );

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fActionTimeRate, 0.0f, 0, true );

			pOwner->SetEnableHide( false );
			pOwner->SetEnableChangeTeam( false );
			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * m_fActionTimeRate;

			m_dwActionEndTime = dwCurTime;
			m_dwActionEndTime += dwDuration;

			pOwner->SetReservedSliding( m_vForceInfoList, iAniID, m_fActionTimeRate, 0 );

			CheckKeyReserveTime( pOwner, iAniID, m_fActionTimeRate );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_JUMP_DASH );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << vMoveDir;
				kPacket << (int)m_CurJumpState;
				kPacket << false;					// dash end
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	// DoubleJumpCheck
	if( IsCanDoubleJump( pOwner, fHeightGap ) )
	{
		if( m_fDoubleJumpPower >= 0.0f )
		{
			m_CurJumpState = BJS_DOUBLE;

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

void ioBadGuyJump::ProcessJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_CurJumpState != BJS_DASH ) return;
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

				if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
				{
					pOwner->ChangeDirectionByInputDir( false );

					D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_JUMP_DASH );
						kPacket << pOwner->GetCharName();
						kPacket << (int)GetType();
						kPacket << vMoveDir;
						kPacket << (int)m_CurJumpState;
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
			// 예약체크
			CheckKeyReserve( pOwner );
		}
	}
}

bool ioBadGuyJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
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

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_bUsedDoubleJump )
	{
		if( m_iCurJumpCnt >= m_iMaxJumpCnt )
			return false;
	}

	if( m_CurJumpState != BJS_NONE && m_CurJumpState != BJS_DOUBLE )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

bool ioBadGuyJump::IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap )
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

	if( m_CurJumpState != BJS_NONE )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

void ioBadGuyJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case BJS_DASH:
		{
			bool bEnd;
			rkPacket >> bEnd;

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			if( !bEnd )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				int iAniID = pGrp->GetAnimationIdx( m_DashAction );
				if( pOwner && iAniID != -1 )
				{
					m_CurJumpState = BJS_DASH;
					m_bUseExtendJump = true;

					m_dwActionEndTime = 0;

					pOwner->SetCurJumpPowerAmt( 0.0f );
					pOwner->SetForcePowerAmt( 0.0f );
					pOwner->SetGravityAmt( 0.0f );

					pOwner->SetTargetRot( qtRotate );

					pGrp->ClearAllActionAni( FLOAT100, true );
					pGrp->ClearAllLoopAni( FLOAT100, true );
					pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fActionTimeRate, 0.0f, 0, true );

					DWORD dwCurTime = FRAMEGETTIME();
					DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * m_fActionTimeRate;

					m_dwActionEndTime = dwCurTime;
					m_dwActionEndTime += dwDuration;

					pOwner->SetReservedSliding( m_vForceInfoList, iAniID, m_fActionTimeRate, 0 );
				}
			}
			else
			{
				pOwner->SetTargetRot( qtRotate );
			}
		}
		break;
	case BJS_DOUBLE:
		{
			float fJumpPower, fJumpForce;
			D3DXQUATERNION qtRotate;

			rkPacket >> fJumpPower >> fJumpForce;
			rkPacket >> qtRotate;

			if( fJumpPower >= 0.0f )
			{
				m_CurJumpState = BJS_DOUBLE;

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

void ioBadGuyJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

float ioBadGuyJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

void ioBadGuyJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUsedDoubleJump = false;
	m_bUseExtendJump = false;
	m_bDashEnd = false;

	m_CurJumpState = BJS_NONE;
}

DWORD ioBadGuyJump::GetChargingTime( ioBaseChar *pChar )
{
	return 0;
	//return m_dwJumpChargeTime;
}

bool ioBadGuyJump::IsCanJumpAttack( ioBaseChar *pOwner )
{
	if( m_CurJumpState == BJS_DASH )
	{
		if( m_dwActionEndTime > 0 && m_dwActionEndTime >= FRAMEGETTIME() )
			return false;
	}

	return true;
}

bool ioBadGuyJump::IsJumpDashState()
{
	if( m_CurJumpState == BJS_DASH )
	{
		if( m_dwActionEndTime > 0 && m_dwActionEndTime >= FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioBadGuyJump::IsJumpAttackReserve()
{
	return m_bJumpAttackReserve;
}

void ioBadGuyJump::CheckKeyReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	
	if( pOwner->IsAttackKey() && !m_bJumpAttackReserve && !pOwner->IsChatModeState( true ) )
	{
		m_bJumpAttackReserve = true;
	}
}

