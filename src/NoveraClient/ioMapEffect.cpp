

#include "stdafx.h"

#include "ioMapEffect.h"

ioMapEffect::ioMapEffect( ioEffect *pEffect )
{
	m_pEffect = pEffect;
}

ioMapEffect::~ioMapEffect()
{
	if( m_pEffect )
	{
		ioSceneNode *pScene = m_pEffect->GetParentSceneNode();
		if( pScene )
		{
			pScene->RemoveAndDestroyAllChildren();

			ioSceneManager *pSceneMgr = pScene->GetSceneMgr();
			if( pSceneMgr )
			{
				pSceneMgr->DestroySceneNode( pScene->GetUniqueID() );
			}
		}

		g_EffectFactory.DestroyEffect( m_pEffect );
		m_pEffect = NULL;
	}
}

void ioMapEffect::SetWorldPosition( const D3DXVECTOR3 &vPos )
{
	if( m_pEffect )
	{
		m_pEffect->GetParentNode()->SetPosition( vPos );
	}
}

void ioMapEffect::SetWorldOrientation( const D3DXQUATERNION &qtRot )
{
	if( m_pEffect )
	{
		m_pEffect->GetParentNode()->SetOrientation( qtRot );
	}
}

void ioMapEffect::Rotate( const D3DXVECTOR3 &vAxis, float fDegrees )
{
	if( m_pEffect )
	{
		m_pEffect->GetParentNode()->Rotate( vAxis, fDegrees );
	}
}

void ioMapEffect::SetEffectDirection( const D3DXVECTOR3 &vDir )
{
	if( m_pEffect )
	{
		m_pEffect->ChangeEffectDirection( vDir, false );
	}
}

void ioMapEffect::SetVisible( bool bVisible )
{
	if( m_pEffect )
	{
		m_pEffect->SetVisible( bVisible );
	}
}

void ioMapEffect::SetAlphaRate( int iAlpha )
{
	if( m_pEffect )
	{
		m_pEffect->SetExtendColorRate( (float)iAlpha / 255.0f );
	}
}

void ioMapEffect::SetRenderQueueGroup( RenderGroupID eGroup )
{
	if( m_pEffect )
	{
		m_pEffect->SetRenderQueueGroup( eGroup );
	}
}

void ioMapEffect::ReserveEffectStop( float fStopDuration, float fStopLooseRate )
{
	if( m_pEffect )
	{
		m_pEffect->ReserveEffectStop( 0.0f, fStopDuration, fStopLooseRate );
	}
}

void ioMapEffect::EndEffectForce()
{
	if( m_pEffect )
	{
		m_pEffect->EndEffectForce();
	}
}

void ioMapEffect::EndEffectGrace()
{
	if( m_pEffect )
	{
		m_pEffect->EndEffectGrace();
	}
}

UniqueObjID ioMapEffect::GetUniqueID() const
{
	if( m_pEffect )
		return m_pEffect->GetUniqueID();

	return -1;
}

const D3DXVECTOR3& ioMapEffect::GetWorldPosition() const
{
	return m_pEffect->GetParentNode()->GetDerivedPosition();
}

const D3DXQUATERNION& ioMapEffect::GetWorldOrientation() const
{
	return m_pEffect->GetParentNode()->GetDerivedOrientation();
}

void ioMapEffect::Translate( const D3DXVECTOR3 &vMove )
{
	if( m_pEffect )
		m_pEffect->GetParentNode()->Translate( vMove );
}

void ioMapEffect::Update( float fTimeGap )
{
	if( m_pEffect )
	{
		m_pEffect->Update( fTimeGap );
	}
}

bool ioMapEffect::IsLive() const
{
	if( m_pEffect )
		return m_pEffect->IsLive();

	return false;
}
