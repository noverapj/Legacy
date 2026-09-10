
#include "stdafx.h"

#include "ioDruidEagleDummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioDruidEagleDummy::ioDruidEagleDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
}

ioDruidEagleDummy::~ioDruidEagleDummy()
{
}

void ioDruidEagleDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "up_fly_animation", "", szBuf, MAX_PATH );
	m_UpFlyAnimation			= szBuf;
	m_UpFlyAnimationRate		= rkLoader.LoadFloat_e( "up_fly_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "down_fly_animation", "", szBuf, MAX_PATH );
	m_DownFlyAnimation			= szBuf;
	m_DownFlyAnimationRate		= rkLoader.LoadFloat_e( "down_fly_animation_rate", FLOAT1 );

	m_dwSpecialStateCheckTime	= rkLoader.LoadInt( "special_check_time", 500 );
}

void ioDruidEagleDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	}

	CheckCratorState( GetOwner() ); 
	ProcessPos( fTimePerSec );
	ProcessEffectUpdate( fTimePerSec );
}

void ioDruidEagleDummy::ProcessPos( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
	}

	SetWorldPosition( pOwner->GetMidPositionByRate() );
	SetWorldOrientation( pOwner->GetTargetRot() );

	m_dwProcessTime = dwCurTime;
}

void ioDruidEagleDummy::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwDelayDuration < dwCurTime )
	{
		SetMoveState();
	}
}

void ioDruidEagleDummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;
}

void ioDruidEagleDummy::CheckCratorState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( 0 < m_dwSpecialStateCheckTime )
	{
		if( pOwner->GetState() == CS_DRUID_EAGLE_FLY || pOwner->GetState() == CS_CHANGE_ON_GRIFFIN_FLY )
		{
			m_dwCurrCheckTime = 0;
		}
		else if( pOwner->GetState() != CS_DRUID_EAGLE_FLY && pOwner->GetState() != CS_CHANGE_ON_GRIFFIN_FLY && m_dwCurrCheckTime == 0 )
		{
			m_dwCurrCheckTime = FRAMEGETTIME();
		}
		else if( 0 < m_dwCurrCheckTime && m_dwCurrCheckTime + m_dwSpecialStateCheckTime < FRAMEGETTIME() )
		{
			SetDieState( true );
		}
	}
}

void ioDruidEagleDummy::SetMoveState()
{
	m_State = DCS_MOVE;
	SetFly();

	if( !m_DummyEffect.IsEmpty() )
	{
		m_pGroup->AttachEffect( m_DummyEffect, NULL );
	}
}

void ioDruidEagleDummy::SetMoveStateMid()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_State != DCS_MOVE )
		return;

	if( !m_MoveAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = m_pGroup->GetLoopInPlay( iAniID );
			if( pAni ) 
				dwPlayTime = pAni->GetCurTime();

			m_pGroup->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, pOwner->GetRunDashAniRate() );
		}
	}
}

void ioDruidEagleDummy::SetFly()
{
	if( !m_MoveAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );
			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_fMoveAniRate );
		}
	}
}

void ioDruidEagleDummy::SetDownFly()
{
	if( !m_DownFlyAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DownFlyAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );
			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_DownFlyAnimationRate );
		}
	}
}

void ioDruidEagleDummy::SetUpFly()
{
	if( !m_UpFlyAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_UpFlyAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );
			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_UpFlyAnimationRate );
		}
	}
}