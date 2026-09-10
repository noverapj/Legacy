
#include "stdafx.h"

#include "ioVehicleDummy.h"

#include "ioVoiceChat/Fourier.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "SkillDefine.h"

#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

ioVehicleDummy::ioVehicleDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_eDriverState = ioVehicleDriverObjectItem::VDS_NONE;
}

ioVehicleDummy::~ioVehicleDummy()
{
}

void ioVehicleDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "jump_ready_animation", "", szBuf, MAX_PATH );
	m_JumpReadyAnimation = szBuf;
	m_fJumpReadyAniRate  = rkLoader.LoadFloat_e( "jump_ready_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "jumpping_animation", "", szBuf, MAX_PATH );
	m_JumppingAnimation = szBuf;
	m_fJumppingAniRate  = rkLoader.LoadFloat_e( "jumpping_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_landing_animation", "", szBuf, MAX_PATH );
	m_JumpLandingAnimation = szBuf;
	m_fJumpLandingAniRate  = rkLoader.LoadFloat_e( "jump_landing_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "drift_left_animation", "", szBuf, MAX_PATH );
	m_DriftLeftAnimation = szBuf;
	m_fDriftLeftAniRate  = rkLoader.LoadFloat_e( "drift_left_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "drift_right_animation", "", szBuf, MAX_PATH );
	m_DrifRightAnimation = szBuf;
	m_fDrifRightAniRate  = rkLoader.LoadFloat_e( "drift_right_animation_rate", FLOAT1 );

	m_dwSpecialStateCheckTime = rkLoader.LoadInt( "special_check_time", 500 );
}

void ioVehicleDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
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

void ioVehicleDummy::ProcessPos( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
	}

	SetWorldPosition( pOwner->GetWorldPosition() );
	SetWorldOrientation( pOwner->GetTargetRot() );

	m_dwProcessTime = dwCurTime;
}

void ioVehicleDummy::ProcessDelayState( float fTimePerSec )
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

void ioVehicleDummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;
}

void ioVehicleDummy::CheckCratorState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( 0 < m_dwSpecialStateCheckTime )
	{
		if( pOwner->GetState() == CS_VEHICLE_DRIVER )
		{
			m_dwCurrCheckTime = 0;
		}
		else if( pOwner->GetState() != CS_VEHICLE_DRIVER && m_dwCurrCheckTime == 0 )
		{
			m_dwCurrCheckTime = FRAMEGETTIME();
		}
		else if( 0 < m_dwCurrCheckTime && m_dwCurrCheckTime + m_dwSpecialStateCheckTime < FRAMEGETTIME() )
		{
			SetDieState( true );
		}
	}

	ioVehicleDirverState* pState = ToVehicleDirverState( pOwner->GetSpecialState() );
	if( pState && pState->GetDriverState() != m_eDriverState )
	{
		m_eDriverState = pState->GetDriverState();
		switch( m_eDriverState )
		{
		case ioVehicleDriverObjectItem::VDS_RUN:			
			SetRunState();
			break;			
		case ioVehicleDriverObjectItem::VDS_JUMP_READY:
			SetJumpReadyState();
			break;
		case ioVehicleDriverObjectItem::VDS_JUMPPING:
			SetJummping();
			break;
		case ioVehicleDriverObjectItem::VDS_LADING:
			SetLanding();
			break;
		case ioVehicleDriverObjectItem::VDS_DRIFT:
			SetDrift( pState->GetDriftLeft() );
			break;
		}
	}
}

void ioVehicleDummy::SetMoveState()
{
	m_State = DCS_MOVE;
	SetRunState();
}

void ioVehicleDummy::SetMoveStateMid()
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

void ioVehicleDummy::SetRunState()
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

void ioVehicleDummy::SetJumpReadyState()
{
	if( !m_JumpReadyAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_JumpReadyAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );
			m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fJumpReadyAniRate );
		}
	}
}
void ioVehicleDummy::SetJummping()
{
	if( !m_JumppingAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_JumppingAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );
			m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fJumppingAniRate );
		}
	}
}

void ioVehicleDummy::SetLanding()
{
	if( !m_JumpLandingAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_JumpLandingAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );
			m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fJumpLandingAniRate );
		}
	}
}

void ioVehicleDummy::SetDrift( bool bLeft )
{
	ioHashString szDriftAnimation;
	float fDriftRate = FLOAT1;

	if( bLeft )
	{
		szDriftAnimation	= m_DriftLeftAnimation;
		fDriftRate			= m_fDriftLeftAniRate;
	}
	else
	{
		szDriftAnimation	= m_DrifRightAnimation;
		fDriftRate			= m_fDrifRightAniRate;
	}

	if( !szDriftAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( szDriftAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );
			m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fDriftRate );
		}
	}
}