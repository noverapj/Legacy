

#include "stdafx.h"

#include "ioDecal.h"

#include "ioEntityGroup.h"
#include "ioRenderOperation.h"

#include "ioVertexBufferInstance.h"
#include "ioIndexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioIndexBufferManager.h"
#include "ioMeshData.h"

#include "ioMaterial.h"
#include "ioPass.h"
#include "ioTechnique.h"
#include "ioTextureUnitState.h"
#include "ioMath.h"
#include "ioMaterialManager.h"
#include "HelpFunc.h"

#include "ioLight.h"
#include "ioCPU.h"

#include "ioCamera.h"

#define DECAL_EPSILON 0.25f

ioDecal::ioDecal( const ioHashString &rkName )
: m_Name( rkName )
{
	m_pParent = NULL;
	m_pMeshData = NULL;
	m_pMaterial = NULL;

	m_bVisible = true;
}

ioDecal::~ioDecal()
{
	DestroyResource();
}

bool ioDecal::Build( const Vector3Vec &rkVtxList, const Vector2Vec &rkUVList, const WordVec &rkIdxList )
{
	DestroyResource();

	int iVertexCount = rkVtxList.size();

	m_pMeshData = new ioMeshData;
	m_pMeshData->InsertStreamSource( 0, IOFVF_POSITION | IOFVF_UV0 );	

	ioVertexBufferInstance *pVtxInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_MANAGED,
																				IOFVF_POSITION | IOFVF_UV0,
																				iVertexCount );

	if( pVtxInstance && pVtxInstance->Lock() )
	{
		ioVtxTex *pBuf = (ioVtxTex*)pVtxInstance->GetBuffer();
		for( int i=0 ; i<iVertexCount ; i++ )
		{
			pBuf[i].vPos = rkVtxList[i];
			pBuf[i].u = rkUVList[i].x;
			pBuf[i].v = rkUVList[i].y;
		}

		pVtxInstance->UnLock();
	}
	else
	{
		DestroyResource();
		return false;
	}

	m_pMeshData->SetBinding( 0, pVtxInstance );
	m_pMeshData->m_iVertexCount = iVertexCount;

	int iIndexCount = rkIdxList.size();

	ioIndexBufferInstance *pIdxInstance = g_IdxBufMgr.CreateExclusiveInstance( BT_MANAGED, iIndexCount );
	if( pIdxInstance && pIdxInstance->Lock() )
	{
		ioCPU::FastMemcpy( pIdxInstance->GetBuffer(), &rkIdxList[0], sizeof(WORD) * iIndexCount );
		pIdxInstance->UnLock();
	}
	else
	{
		DestroyResource();
		return false;
	}

	m_pMeshData->m_pIndexBufferInstance = pIdxInstance;
	m_pMeshData->m_iIndexCount = iIndexCount;

	BuildBoundBox( rkVtxList );
	BuildMaterial();

	return true;
}

void ioDecal::BuildBoundBox( const Vector3Vec &rkVtxList )
{
	D3DXVECTOR3 vMin, vMax;
	ioMath::CalcMinMaxPoint( vMin, vMax, rkVtxList );
	m_BoundBox.SetMinMaxPos( vMin, vMax );
}

void ioDecal::DestroyResource()
{
	if( m_pMeshData )
	{
		m_pMeshData->DeleteAllBuffers();
		SAFEDELETE( m_pMeshData );
	}

	SAFEDELETE( m_pMaterial );
}

void ioDecal::BuildMaterial()
{
	SAFEDELETE( m_pMaterial );

	m_pMaterial = g_MaterialMgr.CreateMaterial();
	m_pMaterial->SetAmbient( 0.0f, 0.0f, 0.0f );
	m_pMaterial->SetEmissive( 0.0f, 0.0f, 0.0f );
}

void ioDecal::NotifyAttached( ioEntityGroup *pParent )
{
	m_pParent = pParent;
}

void ioDecal::NotifyCurrentCamera( ioCamera *pCamera )
{
	D3DXMATRIX matWorld;
	GetWorldTransform( &matWorld );

	ioAxisAlignBox kWorldBox = m_BoundBox;
	kWorldBox.Transform( &matWorld );

	m_bVisible = pCamera->IsVisible( kWorldBox );
}

//-----------------------------------------------

ioMaterial* ioDecal::GetMaterial() const
{
	return m_pMaterial;
}

float ioDecal::GetSquaredViewDepth( const ioCamera *pCamera )
{
	if( m_pParent )
		return m_pParent->GetSquaredViewDepth( pCamera );

	return 0.0f;
}

void ioDecal::GetRenderOperation( ioRenderOperation &rkRenderOp )
{
	rkRenderOp.m_pMeshData = m_pMeshData;
	rkRenderOp.m_PrimitiveType = D3DPT_TRIANGLELIST;

	rkRenderOp.m_iMinIndex    = 0;
	rkRenderOp.m_iVertexCount = m_pMeshData->m_iVertexCount;
	rkRenderOp.m_iIndexStart = 0;
	rkRenderOp.m_iFaceCount  = m_pMeshData->m_iIndexCount / 3;
	rkRenderOp.m_bUseIndex = true;
}

void ioDecal::GetWorldTransform( D3DXMATRIX *pMatWorld ) const
{
	if( m_pParent )
	{
		m_pParent->GetEntityTransform( pMatWorld );
	}
}

