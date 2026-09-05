

#include "stdafx.h"

#include "ioMovableObject.h"
#include "ioRenderOperation.h"
#include "ioRenderQueue.h"
#include "ioMeshData.h"

#include "ioBiped.h"
#include "ioSkeleton.h"

#include "ioResource.h"
#include "ioRcAnimateFX.h"
#include "ioAnimateFX.h"
#include "ioAnimateFXManager.h"

#include "ioNode.h"
#include "ioSceneNode.h"

ioAnimatePlane::ioAnimatePlane( ioAnimateFX *pParent, ioRcAnimatePlane *pRcPlane )
{
	m_pParent = pParent;
	m_pRcPlane = pRcPlane;

	D3DXMatrixIdentity( &m_FullLocalTransform );
	D3DXMatrixIdentity( &m_ObjectOffsetTM );
	
	m_fLinkOffsetRate = 1.0f;
	m_iCurAlphaRate = 0;

	D3DXMatrixIdentity( &m_OwnerTransform );
	m_bSetLinked = false;
	m_bSetOwnerTransform = false;
}

ioAnimatePlane::~ioAnimatePlane()
{
}

void ioAnimatePlane::SetLinkInfo( const D3DXMATRIX &rkObjectOffsetTM, float fLinkOffsetRate )
{
	m_ObjectOffsetTM = rkObjectOffsetTM;
	m_fLinkOffsetRate = fLinkOffsetRate;
	m_bSetLinked = true;
}

bool ioAnimatePlane::UpdateFrame( float fCurTimePast )
{
	float fStartTime = m_pRcPlane->GetStartTime();
	float fEndTime = m_pRcPlane->GetEndTime();

	if( fCurTimePast < fStartTime )	// Ready
	{
		m_iCurAlphaRate = 0;
		return true;
	}
	else if( COMPARE( fCurTimePast, fStartTime, fEndTime ) ) // Play
	{
		float fTimeRate = fCurTimePast - m_pRcPlane->GetStartTime();
		fTimeRate /= m_pRcPlane->GetDuration();
		m_iCurAlphaRate = (int)( m_pRcPlane->GetPlaneAlphaRate( fTimeRate ) * 255.0f );

		SetOwnerTransform();
	}
	else	// End
	{
		m_iCurAlphaRate = 0;
		return false;
	}

	if( m_pRcPlane->IsAnimate() )
	{
		D3DXVECTOR3 vTrans;
		D3DXQUATERNION qtRot;
		if( m_pRcPlane->GetCurFrame( fCurTimePast, vTrans, qtRot ) )
		{
			if( m_bSetLinked )
			{
				vTrans *= m_fLinkOffsetRate;
			}

			D3DXMatrixRotationQuaternion( &m_FullLocalTransform, &qtRot );
			m_FullLocalTransform._41 = vTrans.x;
			m_FullLocalTransform._42 = vTrans.y;
			m_FullLocalTransform._43 = vTrans.z;

			D3DXMatrixMultiply( &m_FullLocalTransform,
								&m_pRcPlane->GetWorldTMInverse(),
								&m_FullLocalTransform );
		}
		else
		{
			D3DXMatrixIdentity( &m_FullLocalTransform );
		}
	}

	if( m_bSetLinked )
	{
		D3DXMatrixMultiply( &m_FullLocalTransform,
							&m_ObjectOffsetTM,
							&m_FullLocalTransform );
	}

	return true;
}

void ioAnimatePlane::SetOwnerTransform()
{
	if( !m_bSetOwnerTransform && GetTransformType() == FXT_OWNER )
	{
		if( m_pParent->IsAttached() )
		{
			m_OwnerTransform = *m_pParent->GetTransform();
			m_bSetOwnerTransform = true;
		}
	}
}

const ioHashString& ioAnimatePlane::GetLinkBipedName() const
{
	return m_pRcPlane->GetLinkBipedName();
}

FXTransformType ioAnimatePlane::GetTransformType() const
{
	return m_pRcPlane->GetTransformType();
}

ioMaterial* ioAnimatePlane::GetMaterial() const
{
	return m_pRcPlane->GetMaterial();
}

float ioAnimatePlane::GetSquaredViewDepth( const ioCamera *pCamera )
{
	if( m_pParent->IsAttached() )
	{
		ioNode *pNode = m_pParent->GetParentNode();
		return pNode->GetSquaredViewDepth( pCamera );
	}

	return 0.0f;
}

void ioAnimatePlane::GetRenderOperation( ioRenderOperation &rkRenderOp )
{
	ioMeshData *pMeshData = m_pRcPlane->GetMeshData();

	rkRenderOp.m_pMeshData	   = pMeshData;
	rkRenderOp.m_PrimitiveType = D3DPT_TRIANGLELIST;

	rkRenderOp.m_iMinIndex	  = 0;
	rkRenderOp.m_iVertexCount = pMeshData->m_iVertexCount;
	rkRenderOp.m_iIndexStart  = 0;
	rkRenderOp.m_iFaceCount   = pMeshData->m_iIndexCount / 3;
	rkRenderOp.m_bUseIndex	  = true;
}

void ioAnimatePlane::GetWorldTransform( D3DXMATRIX *pMatWorld ) const
{
	if( GetTransformType() == FXT_OWNER )
	{
		D3DXMatrixMultiply( pMatWorld, &m_FullLocalTransform, &m_OwnerTransform );
	}
	else
	{
		D3DXMatrixMultiply( pMatWorld, &m_FullLocalTransform, m_pParent->GetTransform() );
	}
}

int ioAnimatePlane::GetAlphaRate() const
{
	return m_iCurAlphaRate;
}

ioAnimateFX::ioAnimateFX( ioRcAnimateFX *pRcFX )
{
	m_pRcFX = pRcFX;

	m_fCurTimePast = 0.0f;
	m_fTimeFactor  = 1.0f;
	m_bPlaneGenerated = false;
}

ioAnimateFX::~ioAnimateFX()
{
	DestroyAllPlanes();

	if( m_pRcFX )
	{
		ioAnimateFXManager::GetSingleton().Destroy( m_pRcFX );
		m_pRcFX = NULL;
	}
}

void ioAnimateFX::DestroyAllPlanes()
{
	AnimatePlaneList::iterator iter;
	for( iter=m_AnimatePlaneList.begin() ; iter!=m_AnimatePlaneList.end() ; ++iter )
	{
		delete *iter;
	}

	m_AnimatePlaneList.clear();
}

void ioAnimateFX::Initialize( float fTimeFactor, ioSkeleton *pOwnerSkel )
{
	m_fCurTimePast = 0.0f;
	m_fTimeFactor  = fTimeFactor;

	if( m_pRcFX->IsLoaded() )
	{
		GeneratePlanes( pOwnerSkel );
	}
}

bool ioAnimateFX::GeneratePlanes( ioSkeleton *pOwnerSkel )
{
	if( m_bPlaneGenerated )
		return true;

	DestroyAllPlanes();

	int iPlaneCnt = m_pRcFX->GetPlaneCnt();
	m_AnimatePlaneList.reserve( iPlaneCnt );

	ioAnimatePlane *pPlane;
	for( int i=0 ; i<iPlaneCnt ; i++ )
	{
		pPlane = new ioAnimatePlane( this, m_pRcFX->GetPlane(i) );
		m_AnimatePlaneList.push_back( pPlane );
	}

	if( pOwnerSkel && pOwnerSkel->IsLinkedSkeleton() )
	{
		AnimatePlaneList::iterator iter;
		for( iter=m_AnimatePlaneList.begin() ; iter!=m_AnimatePlaneList.end() ; ++iter )
		{
			ioAnimatePlane *pPlane = *iter;
			if( !pPlane->GetLinkBipedName().IsEmpty() )
			{
				ioBiped *pBiped = pOwnerSkel->GetBiped( pPlane->GetLinkBipedName() );
				if( pBiped )
				{
					pPlane->SetLinkInfo( pBiped->GetObjectOffsetTM(),
										 pBiped->GetLinkOffsetRate() );
				}
			}
		}
	}

	m_bPlaneGenerated = true;
	return true;
}

bool ioAnimateFX::UpdateFX( float fTime, ioSkeleton *pOwnerSkel )
{
	if( m_pRcFX->GetLoadState() == RLS_FAILED )
		return false;

	m_fCurTimePast += fTime * m_fTimeFactor;

	if( m_pRcFX->IsLoaded() )
	{
		GeneratePlanes( pOwnerSkel );
	}

	bool bLive = false;

	AnimatePlaneList::iterator iter;
	for( iter=m_AnimatePlaneList.begin() ; iter!=m_AnimatePlaneList.end() ; ++iter )
	{
		if( (*iter)->UpdateFrame( m_fCurTimePast ) )
		{
			bLive = true;
		}
	}

	return bLive;
}

void ioAnimateFX::NotifyCurrentCamera( ioCamera *pCamera )
{
}

void ioAnimateFX::UpdateRenderQueue( ioRenderQueue &queue )
{
	AnimatePlaneList::iterator iter;
	for( iter=m_AnimatePlaneList.begin() ; iter!=m_AnimatePlaneList.end() ; ++iter )
	{
		ioAnimatePlane *pPlane = *iter;
		if( pPlane->GetAlphaRate() > 0 )
		{
			queue.AddRenderable( pPlane, m_RenderQueueID );
		}
	}
}

const ioAxisAlignBox& ioAnimateFX::GetBoundBox() const
{
	return m_pRcFX->GetBoundBox();
}

float ioAnimateFX::GetBoundRadius() const
{
	return m_pRcFX->GetBoundRadius();
}
