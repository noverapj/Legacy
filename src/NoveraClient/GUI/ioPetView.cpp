

#include "stdafx.h"

#include "ioPetView.h"

ioPetView::ioPetView( ioEntityGroup *pGrp )
{
	m_pGrp = pGrp;
	
	if( m_pGrp )
		m_pGrp->SetEventParent( this );
}

ioPetView::~ioPetView()
{
	if( m_pGrp )
	{
		ioSceneNode *pSceneNode = m_pGrp->GetParentSceneNode();
		if( pSceneNode )
		{
			pSceneNode->RemoveAndDestroyAllChildren();

			ioSceneManager *pSceneMgr = pSceneNode->GetSceneMgr();
			if( pSceneMgr )
			{
				pSceneMgr->DestroySceneNode( pSceneNode->GetUniqueID() );
				pSceneMgr->DestroyEntityGrp( m_pGrp );
				m_pGrp = NULL;
			}
		}
	}
}

void ioPetView::Update( float fTimePerSec )
{
	if( !m_pGrp )	return;

	m_pGrp->Update( fTimePerSec * FLOAT1000 );
	m_pGrp->UpdateAttachedObjects( fTimePerSec * FLOAT1000 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPetView::SetPosition( float fXPos, float fYPos, float fZPos )
{
	SetPosition( D3DXVECTOR3( fXPos, fYPos, fZPos ) );
}

void ioPetView::SetPosition( const D3DXVECTOR3 &vPos )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		pSNode->SetPosition( vPos );
	}
}

void ioPetView::SetPitch( float fPitch )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		D3DXQUATERNION qtOrient;
		D3DXQuaternionRotationYawPitchRoll( &qtOrient, 0.0f, DEGtoRAD(fPitch), 0.0f );
		pSNode->SetOrientation( qtOrient );
	}
}

void ioPetView::SetYaw( float fYaw )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		D3DXQUATERNION qtOrient;
		D3DXQuaternionRotationYawPitchRoll( &qtOrient, DEGtoRAD( fYaw ), 0.0f, 0.0f );
		pSNode->SetOrientation( qtOrient );
	}
}

void ioPetView::RotateX( float fPitch )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		pSNode->PitchX( fPitch );
	}
}

void ioPetView::RotateY( float fYaw )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		pSNode->YawY( fYaw );
	}
}

void ioPetView::SetScale( float fScale )
{
	SetScale( D3DXVECTOR3( fScale, fScale, fScale ) );
}

void ioPetView::SetScale( const D3DXVECTOR3 &vScale )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		pSNode->SetScale( vScale );
	}
}

void ioPetView::SetScale( float fXScale, float fYScale, float fZScale )
{
	SetScale( D3DXVECTOR3( fXScale, fYScale, fZScale ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPetView::SetActionAni( const ioHashString &szAniName, float fFadeIn, float fFadeOut )
{
	if( m_pGrp )
	{
		m_pGrp->ClearAllActionAni( 0.0f );
		m_pGrp->ClearAllLoopAni( 0.0f );

		m_pGrp->SetActionAni( szAniName, fFadeIn, fFadeOut );
	}
}

void ioPetView::SetLoopAni( const ioHashString &szAniName, float fFadeIn )
{
	if( m_pGrp )
	{
		m_pGrp->ClearAllActionAni( 0.0f );
		m_pGrp->ClearAllLoopAni( 0.0f );

		m_pGrp->SetLoopAni( szAniName, fFadeIn );
		m_pGrp->Update( 0.001f );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 ioPetView::GetWorldPosition() const
{
	return m_pGrp->GetParentNode()->GetDerivedPosition();
}

float ioPetView::GetYaw()
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		return RADtoDEG( ioMath::QuaterToYaw( pSNode->GetOrientation() ) );
	}

	return 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEffect* ioPetView::AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner )
{
	if( !m_pGrp ) 
		return NULL;

	if( !rkEffect.IsEmpty() )
		return m_pGrp->AttachEffect( rkEffect, pEntityEventOwner );

	return NULL;
}

void ioPetView::EndEffect( const ioHashString &rkEffect, bool bGrace )
{
	if( !m_pGrp ) 
		return;

	m_pGrp->EndEffect( rkEffect, bGrace );
}