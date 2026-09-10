#include "StdAfx.h"
#include "ioOwnerTraceDummy2.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"

ioOwnerTraceDummy2::ioOwnerTraceDummy2( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
}


ioOwnerTraceDummy2::~ioOwnerTraceDummy2(void)
{
}

void ioOwnerTraceDummy2::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	m_vFollowOffset.x	= rkLoader.LoadFloat_e( "follow_offset_x", 0.0f );
	m_vFollowOffset.y	= rkLoader.LoadFloat_e( "follow_offset_y", 0.0f );
	m_vFollowOffset.z	= rkLoader.LoadFloat_e( "follow_offset_z", 0.0f );
	m_fFollowLength		= rkLoader.LoadFloat_e( "follow_length", 0.0f );
}


void ioOwnerTraceDummy2::InitDummyCharInfo( int iIndex, DWORD dwCreateGapTime, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale, float fStartAngle, bool bNetWork )
{
	__super::InitDummyCharInfo( iIndex, dwCreateGapTime, vPos, vScale, fStartAngle, bNetWork );
}

void ioOwnerTraceDummy2::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
	{
		SetDieState( false );
		return;
	}

	switch( m_State )
	{
	case DCS_START:
		OnProcessStartState();
		break;
	case DCS_DELAY:
		CheckAttackKeyControl();
		break;
	case DCS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimerPerSec, dwPreTime );
		break;
	case DCS_CONTROL_ATTACK:
		OnProcessChargeAttack( fTimerPerSec );
		break;
	}


	if( pOwner->IsNeedProcess() && m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
	{
		SendDieState( true );
		return;
	}

	ProcessEffectUpdate( fTimerPerSec );
	ProcessPos( pOwner, fTimerPerSec );
}

void ioOwnerTraceDummy2::OnProcessChargeAttack( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			SetDelayState( true );
		}
	}
}

void ioOwnerTraceDummy2::ProcessPos( ioBaseChar *pOwner, float fTimePerSec )
{
	if( !pOwner )
		return;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( pOwner->GetWorldOrientation() * m_vFollowOffset );
	SetWorldPosition( vPos );
	SetWorldOrientation( pOwner->GetWorldOrientation() );
}

void ioOwnerTraceDummy2::OnProcessStartState()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );

		ioBaseChar *pOwner = GetOwner();
		if ( !pOwner )
			return;
	}
}

void ioOwnerTraceDummy2::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	pStage->RemoveSummonDummyInfo( m_iCreateIndex, false );

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioOwnerTraceDummy2::IsMountingAvailable() const
{
	return false;
}

bool ioOwnerTraceDummy2::IsMountAvailable() const
{
	return false;
}

bool ioOwnerTraceDummy2::IsNowMountEnable() const
{
	return false;
}