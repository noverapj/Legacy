

#include "stdafx.h"

#include "ioTargetMarker.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioTargetMarker::ioTargetMarker( ioBaseChar *pOwner )
{
	m_pOwner = pOwner;
	m_MarkerState = MS_NONE;
	m_fMarkerRange = FLOAT1;
	m_fJumpMarkerAngle = 0.0f;
	m_fMarkerYOffset = 0.0f;

	m_bBall = false;
	m_bUseEnable = true;
	m_bPreUseEnable = true;

	m_ReadyID = -1;
	m_AimingID = -1;
	m_AimedID = -1;
	m_ZeroAimedID = -1;
	m_JumpAimID = -1;
	m_HeadAimedID = -1;
}

ioTargetMarker::~ioTargetMarker()
{
}

void ioTargetMarker::SetMarkerEffect( const ioHashString &szReady, 
									  const ioHashString &szAiming,
									  const ioHashString &szAimed,
									  const ioHashString &szZeroAimed,
									  const ioHashString &szHeadAimed )
{
	m_szReadyEffect = szReady;
	m_szAimingEffect = szAiming;
	m_szAimedEffect = szAimed;
	m_szZeroAimedEffect = szZeroAimed;
	m_szHeadAimedEffect = szHeadAimed;
}

void ioTargetMarker::SetSpecialMarker( const ioHashString &szSpecial )
{
	m_szSpecailEffect = szSpecial;
}

void ioTargetMarker::SetUseEnable( bool bEnable )
{
	m_bPreUseEnable = m_bUseEnable;
	m_bUseEnable = bEnable;
}

void ioTargetMarker::SetMarkerState( ioPlayStage *pStage, MarkerState eState )
{
	m_MarkerState = eState;
	DestroyMarker( pStage );

	switch( m_MarkerState )
	{
	case MS_NONE:
		m_Target.Clear();
		break;
	case MS_READY:
		m_Target.Clear();
		m_ReadyID = CreateEffect( pStage, m_szReadyEffect );
		break;
	case MS_AIMING:
		m_Target.Clear();
		m_AimingID = CreateEffect( pStage, m_szAimingEffect );
		break;
	case MS_AIMED:
		if( m_bUseEnable )
			m_AimedID = CreateEffect( pStage, m_szAimedEffect );
		else
			m_AimedID = CreateEffect( pStage, m_szReadyEffect );
		break;
	case MS_ZERO_AIMED:
		m_ZeroAimedID = CreateEffect( pStage, m_szZeroAimedEffect );
		break;
	case MS_JUMP_AIM:
		m_JumpAimID = CreateEffect( pStage, m_szReadyEffect );
		break;
	case MS_HEAD_AIMED: 
		m_HeadAimedID = CreateEffect( pStage, m_szHeadAimedEffect );
		break;
	case MS_SPECIAL_AIMED: 
		m_SpecailID = CreateEffect( pStage, m_szSpecailEffect );
		break;
	}
}

void ioTargetMarker::SetTarget( const ioHashString &rkTarget, bool bBall )
{
	m_Target = rkTarget;
	m_bBall = bBall;
}

ioBaseChar* ioTargetMarker::GetTargetChar() const
{
	return m_pOwner->GetBaseChar( m_Target );
}

bool ioTargetMarker::IsTarget()
{
	if( !m_Target.IsEmpty() )
		return true;

	return false;
}

ioTargetMarker::MarkerState ioTargetMarker::GetMarkerState() const
{
	return m_MarkerState;
}

void ioTargetMarker::DestroyMarker( ioPlayStage *pStage )
{
	if( !pStage ) return;

	if( m_ReadyID != -1 )
	{
		DestroyMarkerEffect( pStage, m_ReadyID );
		m_ReadyID = -1;
	}

	if( m_AimingID != -1 )
	{
		DestroyMarkerEffect( pStage, m_AimingID );
		m_AimingID = -1;
	}

	if( m_AimedID != -1 )
	{
		DestroyMarkerEffect( pStage, m_AimedID );
		m_AimedID = -1;
	}

	if( m_ZeroAimedID != -1 )
	{
		DestroyMarkerEffect( pStage, m_ZeroAimedID );
		m_ZeroAimedID = -1;
	}

	if( m_JumpAimID != -1 )
	{
		DestroyMarkerEffect( pStage, m_JumpAimID );
		m_JumpAimID = -1;
	}

	if( m_HeadAimedID != -1 )
	{
		DestroyMarkerEffect( pStage, m_HeadAimedID );
		m_HeadAimedID = -1;
	}

	if( m_SpecailID != -1 )
	{
		DestroyMarkerEffect( pStage, m_SpecailID );
		m_SpecailID = -1;
	}
}

void ioTargetMarker::DestroyMarkerEffect( ioPlayStage *pStage, UniqueObjID dwEffectID )
{
	if( !pStage ) return;

	pStage->DestroyMapEffect( dwEffectID );
}

ioMapEffect* ioTargetMarker::GetCurMarker( ioPlayStage *pStage )
{
	if( !pStage )
		return NULL;

	DWORD dwID = -1;

	switch( m_MarkerState )
	{
	case MS_READY:
		dwID = m_ReadyID;
		break;
	case MS_AIMING:
		dwID = m_AimingID;
		break;
	case MS_AIMED:
		dwID = m_AimedID;
		break;
	case MS_ZERO_AIMED:
		dwID = m_ZeroAimedID;
		break;
	case MS_JUMP_AIM:
		dwID = m_JumpAimID;
		break;
	case MS_HEAD_AIMED: 
		dwID = m_HeadAimedID;
		break;
	}

	return pStage->FindMapEffect( dwID );
}