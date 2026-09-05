

#include "stdafx.h"

#include "ioMesh.h"
#include "ioEntity.h"
#include "ioSubEntity.h"

#include "ioMaterial.h"
#include "ioTechnique.h"
#include "ioPass.h"
#include "ioTextureUnitState.h"

#include "ioRenderQueue.h"
#include "ioModelParticle.h"
#include "ioCamera.h"
#include "ioMeshManager.h"

#include "ioModelParticleSystem.h"
#include "HelpFunc.h"

ioModelParticle::ioModelParticle()
: m_qtOrientation(0.0f,0.0f,0.0f,1.0f),
  m_vPosition(0.0f,0.0f,0.0f),
  m_vScale(1.0f,1.0f,1.0f),
  m_vDir(0.0f,1.0f,0.0f)
{
	m_fCurTime   = 0.0f;
	m_fLiveTime  = 0.0f;
	m_fMoveSpeed = 0.0f;
	m_fRotSpeed  = 0.0f;

	m_pParent = NULL;
	m_pEntity = NULL;
}

ioModelParticle::~ioModelParticle()
{
	SAFEDELETE( m_pEntity );
}

void ioModelParticle::Rotate( const D3DXVECTOR3 &vAxis, float fAngle )
{
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fAngle ) );
	Rotate( qtRot );
}

void ioModelParticle::Rotate( const D3DXQUATERNION &qtRot )
{
	D3DXQuaternionMultiply( &m_qtOrientation, &m_qtOrientation, &qtRot );
}
	
void ioModelParticle::SetParent( ioModelParticleSystem *pParent )
{
	m_pParent = pParent;
}

void ioModelParticle::SetEntity( const ioHashString &kMeshName )
{
	SAFEDELETE( m_pEntity );

	ioMesh *pMesh = ioMeshManager::GetSingleton().CreateMesh( kMeshName, true );
	if( pMesh )
	{
		m_pEntity = new ioEntity( this, pMesh );
		m_pEntity->BuildEntity( NULL, false );
	}
}

void ioModelParticle::SetRotateSpeed( float fAngle )
{
	m_fRotSpeed = fAngle;
}

void ioModelParticle::SetAlphaRate( int iAlphaRate )
{
	if( m_pEntity )
	{
		m_pEntity->SetAlphaRate( iAlphaRate );
	}
}

void ioModelParticle::UpdateRenderQueue( ioRenderQueue &queue, RenderGroupID eGroupID )
{
	if( m_pEntity )
	{
		if( m_pEntity->IsLoadDone() )
		{
			m_pEntity->UpdateRenderQueue( queue, eGroupID );
		}
	}
}

void ioModelParticle::BuildResource()
{
	if( m_pEntity )
	{
		m_pEntity->BuildEntity( NULL, false );
	}
}

bool ioModelParticle::GetWorldBoundPos( D3DXVECTOR3 &vMin, D3DXVECTOR3 &vMax ) const
{
	if( !m_pEntity || !m_pEntity->IsBuildDone() )
		return false;

	D3DXVECTOR3 vLocalMin = m_pEntity->GetBoundBox().GetMinPos();
	D3DXVECTOR3 vLocalMax = m_pEntity->GetBoundBox().GetMaxPos();

	D3DXVec3TransformCoord( &vLocalMin, &vLocalMin, &m_WorldTransform );
	D3DXVec3TransformCoord( &vLocalMax, &vLocalMax, &m_WorldTransform );

	vMin.x = min( vLocalMin.x, vLocalMax.x );
	vMin.y = min( vLocalMin.y, vLocalMax.y );
	vMin.z = min( vLocalMin.z, vLocalMax.z );

	vMax.x = max( vLocalMin.x, vLocalMax.x );
	vMax.y = max( vLocalMin.y, vLocalMax.y );
	vMax.z = max( vLocalMin.z, vLocalMax.z );

	return true;
}

ioAxisAlignBox ioModelParticle::GetWorldBoundBox() const
{
	ioAxisAlignBox kWorldBox;
	if( m_pEntity )
	{
		D3DXMATRIX matWorld;
		GetEntityTransform( &matWorld );

		kWorldBox = m_pEntity->GetBoundBox();
		kWorldBox.Transform( &matWorld );
	}

	return kWorldBox;
}

DWORD ioModelParticle::GetParticleColor() const
{
	if( m_pEntity )
	{
		DWORD dwColor = (DWORD)m_pEntity->GetAlphaRate();
		dwColor <<= 24;

		return ( dwColor | 0xffffff );
	}

	return 0xffffffff;
}

bool ioModelParticle::GetTextureMatrix( int iSubIndex, D3DXMATRIX *pMatTex )
{
	if( m_pEntity )
	{
		ioSubEntity *pSubEntity = m_pEntity->GetSubEntity( iSubIndex );
		if( pSubEntity )
		{
			ioMaterial *pMtl = pSubEntity->GetMaterial();
			if( pMtl )
			{
				ioTechnique *pTech = pMtl->GetBestTechnique();
				if( pTech )
				{
					ioPass *pPass = pTech->GetPass(0);
					if( pPass )
					{
						ioTextureUnitState *pState = pPass->GetTextureUnitState(0);
						if( pState )
						{
							*pMatTex = pState->GetTextureTransform();
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

void ioModelParticle::UpdateWorldTransform()
{
	D3DXMATRIX matScale, matRotate, matMove;

	ioModelParticleSystem::FollowParentType eFollowType = m_pParent->GetFollowParentMode();
	if( eFollowType == ioModelParticleSystem::FPT_NONE ||
		eFollowType == ioModelParticleSystem::FPT_ROT_ONLY )
	{
		D3DXVECTOR3 vScale = m_vScale * m_pParent->GetParentScale();
		D3DXMatrixScaling( &matScale, vScale.x, vScale.y, vScale.z );
	}
	else
	{
		D3DXMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	}

	D3DXQUATERNION qtRotate = m_qtOrientation;
	if( eFollowType == ioModelParticleSystem::FPT_ROT_ONLY )
	{
		D3DXQuaternionMultiply( &qtRotate, &qtRotate, &m_qtStartParentOrientation );
	}

	D3DXMatrixRotationQuaternion( &matRotate, &qtRotate );
	D3DXMatrixTranslation( &matMove, m_vPosition.x, m_vPosition.y, m_vPosition.z );

	D3DXMatrixMultiply( &m_WorldTransform, &matScale, &matRotate );
	D3DXMatrixMultiply( &m_WorldTransform, &m_WorldTransform, &matMove );
	D3DXMatrixMultiply( &m_WorldTransform, &m_WorldTransform, m_pParent->GetTransform() );
}

void ioModelParticle::GetEntityTransform( D3DXMATRIX *pMatWorld ) const
{
	*pMatWorld = m_WorldTransform;
}

float ioModelParticle::GetSquaredViewDepth( const ioCamera *pCamera )
{
	D3DXVECTOR3 vDiff(m_WorldTransform._41, m_WorldTransform._42, m_WorldTransform._43 );
	vDiff -= pCamera->GetPosition();

	return D3DXVec3LengthSq( &vDiff );
}

