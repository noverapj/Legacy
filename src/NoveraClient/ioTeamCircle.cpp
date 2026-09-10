

#include "stdafx.h"

#include "ioTeamCircle.h"
#include "ioPlayStage.h"

ioHashString ioTeamCircle::m_RedTeamCircle;
ioHashString ioTeamCircle::m_BlueTeamCircle;
ioHashString ioTeamCircle::m_DefaultShadow;

ioTeamCircle::ioTeamCircle()
{
	m_EffectID = -1;
	m_ShadowEffectID = -1;

	m_bMyCircle  = false;
}

ioTeamCircle::~ioTeamCircle()
{
}

void ioTeamCircle::SetTeam( ioPlayStage *pStage, TeamType eTeam, const D3DXVECTOR3 &vPos, bool bMyCircle )
{
	if( !pStage ) return;

	DestroyEffect( pStage );

	m_bMyCircle = bMyCircle;

	ioHashString szEffectName;
	switch( eTeam )
	{
	case TEAM_RED:
		szEffectName = m_RedTeamCircle;
		break;
	case TEAM_BLUE:
		szEffectName = m_BlueTeamCircle;
		break;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

	if( !m_DefaultShadow.IsEmpty() )
	{
		ioMapEffect *pShadowEffect = pStage->CreateMapEffect( m_DefaultShadow, vPos, vScale );
		if( pShadowEffect )
		{
			m_ShadowEffectID = pShadowEffect->GetUniqueID();
		}
	}

	if( !szEffectName.IsEmpty() )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pEffect = pStage->CreateMapEffect( szEffectName, vPos, vScale );
		if( pEffect )
		{
			m_EffectID = pEffect->GetUniqueID();
		}
	}
}

void ioTeamCircle::UpdatePos( ioPlayStage *pStage, const D3DXVECTOR3 &vPos, bool bCircleVisible, bool bShadowVisible )
{
	if( !pStage ) return;

	if( m_ShadowEffectID != -1 )
	{
		ioMapEffect *pShadowEffect = pStage->FindMapEffect( m_ShadowEffectID );
		if( pShadowEffect )
		{
			pShadowEffect->SetWorldPosition( vPos );

			if( !Setting::UseShadow() )
				pShadowEffect->SetVisible( bShadowVisible );
			else
				pShadowEffect->SetVisible( false );
		}
	}
	
	if( m_EffectID != -1 )
	{
		ioMapEffect *pEffect = pStage->FindMapEffect( m_EffectID );
		if( pEffect )
		{
			pEffect->SetWorldPosition( vPos );
			pEffect->SetVisible( bCircleVisible );
		}
	}
}

void ioTeamCircle::DestroyEffect( ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( m_EffectID == -1 ) return;
	if( m_ShadowEffectID == -1 ) return;

	pStage->DestroyMapEffect( m_ShadowEffectID );
	pStage->DestroyMapEffect( m_EffectID );

	m_ShadowEffectID = -1;
	m_EffectID = -1;
}