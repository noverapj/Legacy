

#include "stdafx.h"
#include "ioFlyFollowStateBuff2.h"
#include "ioBaseChar.h"
#include "ioEntityGroup.h"
#include "ioPlayStage.h"

ioFlyFollowStateBuff2::ioFlyFollowStateBuff2()
{
	m_BuffState = BS_NONE;
	m_dwMovingEndTime = 0;
	m_dwWaitEndTime = 0;
	m_vNorLookAt = ioMath::VEC3_ZERO;
	m_vBeforeScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
}

ioFlyFollowStateBuff2::ioFlyFollowStateBuff2( const ioFlyFollowStateBuff2 &rhs )
: ioBuff( rhs ),
m_MoveAni( rhs.m_MoveAni ),
m_fMoveAniRate( rhs.m_fMoveAniRate ),
m_fFollowSpeed( rhs.m_fFollowSpeed ),
m_vLookAt( rhs.m_vLookAt ),
m_dwMoveDuration( rhs.m_dwMoveDuration ),
m_dwWaitDuraion( rhs.m_dwWaitDuraion ),
m_vAxisScale( rhs.m_vAxisScale ),
m_bSetEndJump( rhs.m_bSetEndJump ),
m_fEndJumpPower( rhs.m_fEndJumpPower ),
m_WaitAni( rhs.m_WaitAni ),
m_fWaitAniRate( rhs.m_fWaitAniRate ),
m_bCharCol( rhs.m_bCharCol )
{
	m_BuffState = BS_NONE;
	m_dwMovingEndTime = 0;
	m_dwWaitEndTime = 0;
	m_vNorLookAt = ioMath::VEC3_ZERO;
	m_vBeforeScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
}

ioFlyFollowStateBuff2::~ioFlyFollowStateBuff2()
{
}

void ioFlyFollowStateBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_MoveAni = szBuf;
	m_fMoveAniRate = rkLoader.LoadFloat( "move_animation_rate", FLOAT1 );
	m_fFollowSpeed = rkLoader.LoadFloat_e( "move_speed", 400.0f );
	m_vLookAt.x = rkLoader.LoadFloat_e( "lookat_x", 0.0f );
	m_vLookAt.y = rkLoader.LoadFloat_e( "lookat_y", 0.0f );
	m_vLookAt.z = rkLoader.LoadFloat_e( "lookat_z", 0.0f );

	m_dwMoveDuration = (DWORD)rkLoader.LoadInt_e( "move_duration", 0 );
	m_dwWaitDuraion = (DWORD)rkLoader.LoadInt_e( "wait_duration", 0 );

	m_vAxisScale.x = rkLoader.LoadFloat( "scale_rate_x", FLOAT1 );
	m_vAxisScale.y = rkLoader.LoadFloat( "scale_rate_y", FLOAT1 );
	m_vAxisScale.z = rkLoader.LoadFloat( "scale_rate_z", FLOAT1 );

	m_bSetEndJump = rkLoader.LoadBool_e( "set_end_jump", false );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	rkLoader.LoadString_e( "wait_animation", "", szBuf, MAX_PATH );
	m_WaitAni = szBuf;
	m_fWaitAniRate = rkLoader.LoadFloat( "wait_animation_rate", FLOAT1 );

	m_bCharCol = rkLoader.LoadBool_e( "char_col", false );
}

ioBuff* ioFlyFollowStateBuff2::Clone()
{
	return new ioFlyFollowStateBuff2( *this );
}

void ioFlyFollowStateBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	SetBuffMoveState( pOwner );
}

bool ioFlyFollowStateBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	SetBuffMoveState( pOwner );

	return true;
}

void ioFlyFollowStateBuff2::SetBuffMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetState( CS_BUFF_ACTION );

	D3DXVec3Normalize( &m_vNorLookAt, &m_vLookAt );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_MoveAni );
	if ( iAniID != -1 )
	{
		float fAniRate = m_fMoveAniRate;
		if ( fAniRate <= 0.0f )
			fAniRate = FLOAT1;
		pGrp->ClearAllActionAni( 0.0f, true );
		pGrp->ClearAllLoopAni( 0.0f, true );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	}

	pOwner->SetTargetRotToDir( m_vNorLookAt, true );

	m_dwMovingEndTime = m_dwBuffStartTime + m_dwMoveDuration;

	m_BuffState = BS_MOVE;
}

void ioFlyFollowStateBuff2::ProcessBuff( float fTimePerSec )
{
	if ( !CheckLive() )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_BuffState )
	{
	case BS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	case BS_WAIT:
		ProcessWaitState( fTimePerSec );
		break;
	}
}

void ioFlyFollowStateBuff2::EndBuff()
{
	ClearBuffState();
	ioBuff::EndBuff();
}

bool ioFlyFollowStateBuff2::CheckLive()
{
	if( !GetCreator() || !GetOwner() )
		return false;

	if( CheckEnableDelayState() )
		return false;

	if( m_dwBuffEndTime <= FRAMEGETTIME() )
		return false;

	return true;
}

void ioFlyFollowStateBuff2::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	if ( m_dwMovingEndTime > 0 && m_dwMovingEndTime < FRAMEGETTIME() )
	{
		m_dwMovingEndTime = 0;
		SetBuffWaitState( pOwner );
		return;
	}

	ProcessMove( pOwner, fTimePerSec );
}

void ioFlyFollowStateBuff2::ProcessMove( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	float fCurMoveSpeed = m_fFollowSpeed * fTimePerSec;
	D3DXVECTOR3 vMoveAmt = m_vNorLookAt * fCurMoveSpeed;
	pOwner->Translate( vMoveAmt );

}

void ioFlyFollowStateBuff2::SetBuffWaitState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	m_BuffState = BS_WAIT;

	int iAniID = pGrp->GetAnimationIdx( m_WaitAni );
	if ( iAniID != -1 )
	{
		float fAniRate = m_fWaitAniRate;
		if ( fAniRate <= 0.0f )
			fAniRate = FLOAT1;
		pGrp->ClearAllActionAni( 0.0f, true );
		pGrp->ClearAllLoopAni( 0.0f, true );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	}

	D3DXVECTOR3 vCurScale = pOwner->GetWorldScale();
	m_vBeforeScale = vCurScale;
	
	if ( m_vAxisScale.x > 0.0f )
		vCurScale.x = m_vAxisScale.x;
	if ( m_vAxisScale.y > 0.0f )
		vCurScale.y = m_vAxisScale.y;
	if ( m_vAxisScale.z > 0.0f )
		vCurScale.z = m_vAxisScale.z;

	pOwner->SetWorldScale( vCurScale );
	m_dwWaitEndTime = FRAMEGETTIME() + m_dwWaitDuraion;
}

void ioFlyFollowStateBuff2::ProcessWaitState( float fTimePerSec )
{
	if ( m_dwWaitEndTime > 0 && m_dwWaitEndTime < FRAMEGETTIME() )
	{
		m_dwWaitEndTime = 0;
		SetReserveEndBuff();
		return;
	}
}

bool ioFlyFollowStateBuff2::IsCharCollisionSkipState()
{
	return m_bCharCol;
}

void ioFlyFollowStateBuff2::ClearBuffState()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	pOwner->SetWorldScale( m_vBeforeScale );
	if( pOwner->GetState() == CS_BUFF_ACTION )
	{
		if( m_bSetEndJump )
			pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, true, false, true );
		else
		{
			BlowWoundAniInfo kInfo;
			pOwner->SetBlowWoundedState( true, kInfo );
		}
	}
}
