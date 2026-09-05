#include "StdAfx.h"
#include "ioTargetTrace6Dummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioTargetTrace6Dummy::ioTargetTrace6Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
}


ioTargetTrace6Dummy::~ioTargetTrace6Dummy(void)
{
}

void ioTargetTrace6Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	LoadInitProperty( rkLoader );
}

void ioTargetTrace6Dummy::LoadInitProperty( ioINILoader &rkLoader )
{
	m_bCheckDropZoneDie = rkLoader.LoadBool("check_dropzone_die", false );
}

void ioTargetTrace6Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_TRACE:
		ProcessTraceState( fTimePerSec, dwPreTime );
		break;
	}

	ioBaseChar *pOwner = GetOwner();

	if( m_bCheckDropZoneDie && pOwner && pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vPos = this->GetWorldPosition();
		ioPlayStage *pStage = GetOwner()->GetCreator();

		if( pStage )
		{
			float fNewMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );

			if ( fNewMapHeight <= 0)
			{
				SendDieState( true );
				return;
			}
		}
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
	{
		SendDieState( true );
		return;
	}



	ProcessPos( fTimePerSec );
	ProcessEffectUpdate( fTimePerSec );
	CheckCurHp();
}

void ioTargetTrace6Dummy::ProcessStartState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar *pOwner = GetOwner();
	if ( pOwner )
	{
		if ( !m_vTargetedChar.empty() )
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_vTargetedChar[0] );
			if ( pTarget )
			{
				StatusValue TargetValue = pTarget->GetHP();
				SetHP( TargetValue.m_fCurValue, TargetValue.m_fMaxValue );
				pTarget->SetWorldPosition( GetWorldPosition() );
			}		
		}		
	}
	

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioTargetTrace6Dummy::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwDelayDuration < dwCurTime )
	{
		m_dwDelayStartTime = 0;

		if( m_nCanEatCnt && m_nCanEatCnt <= (int)m_vTargetedChar.size() )
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner && pOwner->IsNeedProcess() )
			{
				SendDieState( true );
				return;
			}
		}
		SetTraceState( DCTS_SENSING );
	}
}

void ioTargetTrace6Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	/*ioBaseChar *pOwner = GetOwner();
	if ( pOwner )
	{
	if ( !m_vTargetedChar.empty() )
	{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_vTargetedChar[0] );
	if ( pTarget )
	{
	pTarget->SetCurHP( GetHP().m_fCurValue );
	}
	}
	}*/

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

void ioTargetTrace6Dummy::CheckCurHp()
{
	ioBaseChar *pOwner = GetOwner();
	if ( pOwner )
	{
		if ( !m_vTargetedChar.empty() )
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_vTargetedChar[0] );
			if ( pTarget )
			{
				if( pTarget->IsZeroHPByMode() )
					pTarget->SetCurHP( 0.f );		
				else
					pTarget->SetCurHP( GetHP().m_fCurValue );		
			}
		}
	}
}
