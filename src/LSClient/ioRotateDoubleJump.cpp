

#include "stdafx.h"

#include "ioRotateDoubleJump.h"
#include "ioBaseChar.h"

ioRotateDoubleJump::ioRotateDoubleJump()
{
	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;
}

ioRotateDoubleJump::ioRotateDoubleJump( const ioRotateDoubleJump &rhs )
: ioExtendJump( rhs ),
 m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
 m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
 m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
 m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
 m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
 m_DoubleJumpAni( rhs.m_DoubleJumpAni ),
 m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
 m_fDoubleJumpSpeedRate( rhs.m_fDoubleJumpSpeedRate )
{
	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;
}

ioExtendJump* ioRotateDoubleJump::Clone()
{
	return new ioRotateDoubleJump( *this );
}

ioRotateDoubleJump::~ioRotateDoubleJump()
{
}

JumpType ioRotateDoubleJump::GetType() const
{
	return JT_ROTATE_DOUBLE_JUMP;
}

void ioRotateDoubleJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	m_iMaxJumpCnt = rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt = max( 2, m_iMaxJumpCnt );

	m_fDoubleJumpPower = rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce = rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_dwDoubleJumpEnableTime = rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_fDoubleJumpEnableHeight = rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );

	m_fDoubleJumpSpeedRate = rkLoader.LoadFloat_e( "double_jump_speed_rate", FLOAT1 );

	char szBuf[MAX_PATH] = "";
	m_fDoubleJumpAniRate = rkLoader.LoadFloat_e( "double_jump_ani_rate", FLOAT1 );
	
	rkLoader.LoadString_e( "double_jump_animation", "", szBuf, MAX_PATH );
	m_DoubleJumpAni = szBuf;
}

void ioRotateDoubleJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bUsedDoubleJump = false;
	m_bUseExtendJump = false;
	m_iCurJumpCnt = 1;
}

void ioRotateDoubleJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( IsCanDoubleJump( pOwner, fHeightGap ) )
	{
		if( m_fDoubleJumpPower >= 0.0f )
		{
			m_bUsedDoubleJump = true;
			m_bUseExtendJump = true;
			m_iCurJumpCnt++;

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );

			if( pOwner->IsSettedDirection() )
				pOwner->ChangeDirectionByInputDir( false );
			else
			{
				D3DXQUATERNION qtTargetRot = pOwner->GetTargetRot();
				pOwner->SetTargetRotToRotate( qtTargetRot, true );
			}

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

			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );

				int iAniID = pGrp->GetAnimationIdx( m_DoubleJumpAni );
				if( iAniID == -1 )
					iAniID = pOwner->GetJumppingAnimationIdx();

				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DOUBLE_JUMP );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << vJumpDir;
				kPacket << m_fDoubleJumpPower;
				kPacket << m_fDoubleJumpForce;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

bool ioRotateDoubleJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;

	if( !pOwner->IsJumpKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_bUsedDoubleJump )
	{
		if( m_iCurJumpCnt >= m_iMaxJumpCnt )
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

void ioRotateDoubleJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

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

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			int iAniID = pGrp->GetAnimationIdx( m_DoubleJumpAni );
			if( iAniID == -1 )
				iAniID = pOwner->GetJumppingAnimationIdx();

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
	}
}

float ioRotateDoubleJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
{
	if( m_bUsedDoubleJump )
	{
		return m_fDoubleJumpSpeedRate;
	}

	return m_fJumpSpeedRate;
}

void ioRotateDoubleJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUsedDoubleJump = false;
	m_bUseExtendJump = false;
}