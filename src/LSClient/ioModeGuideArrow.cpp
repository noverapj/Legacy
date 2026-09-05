

#include "stdafx.h"

#include "ioPlayMode.h"
#include "ioBaseChar.h"
#include "ioModeGuideArrow.h"

ioModeGuideArrow::ioModeGuideArrow( ioPlayMode *pCreator )
{
	m_pCreator = pCreator;
}

ioModeGuideArrow::~ioModeGuideArrow()
{
	DestroyArrow();
}

void ioModeGuideArrow::DestroyArrow()
{
	ArrowList::iterator iter = m_ArrowList.begin();
	for( ; iter!=m_ArrowList.end() ; ++iter )
	{
		m_pCreator->DestroyMapEffect( *iter );
	}
	m_ArrowList.clear();
}

void ioModeGuideArrow::Initialize( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "mode_guide_red_arrow", "", szBuf, MAX_PATH );
	m_ModeGuideRedArrow = szBuf;

	rkLoader.LoadString_e( "mode_guide_blue_arrow", "", szBuf, MAX_PATH );
	m_ModeGuideBlueArrow = szBuf;

	m_fMaxArrowRange  = rkLoader.LoadFloat_e( "mode_guide_max_range", 300.0f );
	m_fModeGuideScale = rkLoader.LoadFloat_e( "mode_guide_scale", FLOAT1 );
	m_iMaxArrowCount  = rkLoader.LoadInt_e( "mode_guide_max_arrow", 1 );
}

void ioModeGuideArrow::Process( ioBaseChar *pOwner, ioGameEntity *pTarget )
{
	if( m_ModeGuideRedArrow.IsEmpty() || m_ModeGuideBlueArrow.IsEmpty() )
		return;

	if( !CheckArrowCreated( pOwner->GetTeam() ) )
		return;

	if( !pTarget || CheckGuideHide( pOwner ) )
	{
		SetVisible( false );
	}
	else
	{
		ArrangeArrows( pOwner, pTarget->GetWorldPosition() );
		SetVisible( true );
	}
}

void ioModeGuideArrow::Process( ioBaseChar *pOwner, ioGameEntity *pTarget, TeamType eTeamType )
{
	if( m_ModeGuideRedArrow.IsEmpty() || m_ModeGuideBlueArrow.IsEmpty() )
		return;

	if( !CheckArrowCreated( eTeamType ) )
		return;

	if( !pTarget || CheckGuideHide( pOwner ) )
	{
		SetVisible( false );
	}
	else
	{
		ArrangeArrows( pOwner, pTarget->GetWorldPosition() );
		SetVisible( true );
	}
}

void ioModeGuideArrow::Process( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos, TeamType eTeamType )
{
	if( m_ModeGuideRedArrow.IsEmpty() || m_ModeGuideBlueArrow.IsEmpty() )
		return;

	if( !CheckArrowCreated( eTeamType ) )
		return;

	if( CheckGuideHide( pOwner ) )
	{
		SetVisible( false );
	}
	else
	{
		ArrangeArrows( pOwner, vTargetPos );
		SetVisible( true );
	}
}


void ioModeGuideArrow::SetVisible( bool bVisible )
{
	ArrowList::iterator iter = m_ArrowList.begin();
	for( ; iter!=m_ArrowList.end() ; ++iter )
	{
		ioMapEffect *pEffect = GetEffect( *iter );
		if( pEffect )
		{
			pEffect->SetVisible( bVisible );
		}
	}
}

bool ioModeGuideArrow::CheckArrowCreated( TeamType eTeam )
{
	ioHashString szFileName;
	if( eTeam == TEAM_BLUE )
		szFileName = m_ModeGuideBlueArrow;
	else if( eTeam == TEAM_RED )
		szFileName = m_ModeGuideRedArrow;

	if( szFileName.IsEmpty() )
		return false;

	if( !m_ArrowList.empty() )
	{
		if( m_ModeGuideArrow == szFileName )
			return true;
		
		// 이전에 사용하던 Arrow  삭제
		DestroyArrow();
	}

    m_ModeGuideArrow = szFileName;
	D3DXVECTOR3 vScale( m_fModeGuideScale, m_fModeGuideScale, m_fModeGuideScale );

	m_ArrowList.reserve( m_iMaxArrowCount );
	for( int i=0 ; i<m_iMaxArrowCount ; i++ )
	{
		ioMapEffect *pEffect = m_pCreator->CreateMapEffect( szFileName, vScale );
		if( pEffect )
		{
			m_ArrowList.push_back( pEffect->GetUniqueID() );
		}
	}

	return !m_ArrowList.empty();
}

bool ioModeGuideArrow::CheckGuideHide( ioBaseChar *pOwner )
{
	if( !pOwner )
		return true;

	if( m_pCreator->GetModeState() != ioPlayMode::MS_PLAY )
		return true;

	CharState eState = pOwner->GetState();
	if( eState == CS_READY || eState == CS_VIEW || 
		eState == CS_LOADING ||	eState == CS_DIE || eState == CS_OBSERVER )
		return true;

	return false;
}

void ioModeGuideArrow::ArrangeArrows( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos )
{
	if( m_iMaxArrowCount <= 0 || m_ArrowList.empty() )
		return;

	D3DXVECTOR3 vStartPos = pOwner->GetWorldPosition();
	vStartPos.y = pOwner->GetBottomHeight();

	D3DXVECTOR3 vDiff = vTargetPos - pOwner->GetWorldPosition();
	vDiff.y = 0.0f;

	float fTargetLength = D3DXVec3Length( &vDiff );
	D3DXVec3Normalize( &vDiff, &vDiff );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDiff );

	// Update Arrow Pos and Rotation
	D3DXVECTOR3 vCurPos = vStartPos;
	float fGapLength = min( m_fMaxArrowRange, fTargetLength ) / m_iMaxArrowCount;

	ArrowList::iterator iter = m_ArrowList.begin();
	for( ; iter!=m_ArrowList.end() ; ++iter )
	{
		vCurPos += vDiff * fGapLength;

		ioMapEffect *pEff = GetEffect( *iter );
		if( pEff )
		{
			pEff->SetWorldPosition( vCurPos );
			pEff->SetWorldOrientation( qtRot );
		}
	}
}

ioMapEffect* ioModeGuideArrow::GetEffect( UniqueObjID eObjID )
{
	return m_pCreator->GetMapEffect( eObjID );
}