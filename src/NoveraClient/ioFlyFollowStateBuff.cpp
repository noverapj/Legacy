

#include "stdafx.h"
#include "ioFlyFollowStateBuff.h"
#include "ioBaseChar.h"
#include "ioEntityGroup.h"
#include "ioPlayStage.h"

ioFlyFollowStateBuff::ioFlyFollowStateBuff()
{
}

ioFlyFollowStateBuff::ioFlyFollowStateBuff( const ioFlyFollowStateBuff &rhs )
	: ioBuff( rhs ),
	m_MoveAni( rhs.m_MoveAni ),
	m_fAniRate( rhs.m_fAniRate ),
	m_fFollowSpeed( rhs.m_fFollowSpeed ),
	m_fEndJumpPower( rhs.m_fEndJumpPower ),
	m_fCreatorCheckLength( rhs.m_fCreatorCheckLength )
{
	m_bNotJump = false;
	m_bSendJumpNet = false;
}

ioFlyFollowStateBuff::~ioFlyFollowStateBuff()
{
}

void ioFlyFollowStateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_MoveAni = szBuf;
	m_fAniRate = rkLoader.LoadFloat( "move_animation_rate", FLOAT1 );
	m_fFollowSpeed = rkLoader.LoadFloat_e( "move_speed", 400.0f );

	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", FLOAT100 );

	m_fCreatorCheckLength = rkLoader.LoadFloat_e( "creator_check_length", FLOAT0001 );
}

ioBuff* ioFlyFollowStateBuff::Clone()
{
	return new ioFlyFollowStateBuff( *this );
}

void ioFlyFollowStateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetState( CS_FLY_FOLLOW_STATE );

	if( GetCreator() )
	{
		m_vOffSet = pOwner->GetWorldPosition() - GetCreator()->GetWorldPosition();
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_MoveAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );
}

bool ioFlyFollowStateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->SetState( CS_FLY_FOLLOW_STATE );

	if( GetCreator() )
	{
		m_vOffSet = pOwner->GetWorldPosition() - GetCreator()->GetWorldPosition();
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_MoveAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	return true;
}


void ioFlyFollowStateBuff::SetEndJump( bool b )
{
	m_bNotJump = !b;
}

void ioFlyFollowStateBuff::ProcessBuff( float fTimePerSec )
{
	ioBuff::ProcessBuff( fTimePerSec );

	if( GetCreator() == NULL )
		return;

	if( GetOwner() == NULL )
		return;

	ioPlayStage *pStage = GetOwner()->GetCreator();
	if( !pStage )
		return;

	//당사자가 거리체크(네트웍으로 점프 상황을 보냄)
	if( GetOwner()->IsNeedProcess() )
	{
		D3DXVECTOR3 vOffSet = GetCreator()->GetWorldPosition() - GetOwner()->GetWorldPosition();
		float fLength = D3DXVec3Length( &vOffSet );
		if( fLength > m_fCreatorCheckLength )
		{
			m_bSendJumpNet = true;
			SetReserveEndBuff();
			return;
		}
	}

	if( GetOwner()->GetState() != CS_FLY_FOLLOW_STATE )
	{
		m_bNotJump = true;
		SetReserveEndBuff();
		return;
	}

	if( GetCreator()->GetState() != CS_DARKSEAS_STATE )
	{
		SetReserveEndBuff();
		return;
	}

	//이동
	D3DXVECTOR3 vGoalPos = GetCreator()->GetWorldPosition() + m_vOffSet;
	D3DXVECTOR3 vLookDir = vGoalPos - GetOwner()->GetWorldPosition();
	float fLength = D3DXVec3Length( &vLookDir );
	
	if( fLength < 20.f )
		return;

	D3DXVec3Normalize( &vLookDir, &vLookDir );
	float fCurMoveSpeed = m_fFollowSpeed * fTimePerSec;
	fCurMoveSpeed = min( fCurMoveSpeed, fLength );

	D3DXVECTOR3 vMoveAmt = vLookDir * fCurMoveSpeed;

	bool bCol = false;
	if( pStage->ReCalculateMoveVectorByTerrain( GetOwner(), &vMoveAmt, bCol ) )
		GetOwner()->Translate( vMoveAmt );

	D3DXVECTOR3 vCurPos = GetOwner()->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, GetOwner(), false, GetOwner()->GetMidHeightByRate(1.0f) );
	float fMapNoneUnit = pStage->GetMapHeightOnlyTerrain( vCurPos.x, vCurPos.z );

	vCurPos.y = max( vCurPos.y, fMapNoneUnit );

	//if( m_FlyState == LFS_FLY_S_ATTACK && fMapNoneUnit > FLOAT10 )
	/*{
		if( vCurPos.y < (fMapNoneUnit + m_f_S_Min_Land_Height) )
			vCurPos.y = fMapNoneUnit + m_f_S_Min_Land_Height;
	}*/

	//pStage->CheckFlyMapLimit( &vCurPos );
	//pStage->CheckAxisLimit( &vCurPos );
	GetOwner()->SetWorldPosition( vCurPos );

}

void ioFlyFollowStateBuff::EndBuff()
{
	ioBuff::EndBuff();

	CheckExceptCallingBuff();

	if( m_pOwner->GetState() == CS_FLY_FOLLOW_STATE && !m_bNotJump )
	{
		m_pOwner->SetCurMoveSpeed( 0.0f );
		m_pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, m_bSendJumpNet, true );
	}
}



