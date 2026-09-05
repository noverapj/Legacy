

#include "stdafx.h"

#include <malloc.h>

#include "ioSubEntity.h"
#include "ioEntity.h"

#include "ioSubMesh.h"
#include "ioMesh.h"
#include "ioRenderQueue.h"

#include "ioBiped.h"
#include "ioSkeleton.h"
#include "ioEntityParent.h"
#include "ioMaterial.h"

#include "ioVertexBufferBinder.h"
#include "ioVertexBufferInstance.h"
#include "ioIndexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioMeshData.h"

#include "ioMeshManager.h"
#include "ioMaterialManager.h"

#include "ioLight.h"
#include "HelpFunc.h"
#include "ioCPU.h"
#include "ErrorReport.h"

using namespace std;

ioEntity::ioEntity( ioEntityParent *pParent, ioMesh *pMesh )
{
	m_pParent = pParent;

	m_pMesh = pMesh;
	m_pMeshData = NULL;
	m_pExtraBuf = NULL;

	m_aBipedXformList  = NULL;
	m_aBipedIndexList  = NULL;
	m_iApplyBipedCount = 0;

	m_DiffuseRate.r = m_DiffuseRate.g = m_DiffuseRate.b = m_DiffuseRate.a = 1.0f;

	m_MeshType = MT_STATIC;
	m_iAlphaRate = MAX_ALPHA_RATE;

	m_iSoftBufferStartPos = 0;

	m_bVisible = true;
	m_bCulled = false;
	m_bVisibleChangePass = false;

	m_bSoftSkinningMeshData = false;
	m_bDoneEntityBuild = false;
	m_bUpdateBufferOK = false;

	BuildSubEntityList();
}

ioEntity::~ioEntity()
{
	SubEntityList::iterator iter;
	for( iter=m_SubEntityList.begin() ; iter!=m_SubEntityList.end() ; ++iter )
	{
		delete *iter;
	}
	m_SubEntityList.clear();

	SAFEDELETEARRAY( m_aBipedIndexList );

	if( m_aBipedXformList )
	{
		_aligned_free( m_aBipedXformList );
		m_aBipedXformList = NULL;
	}

	DestroyExtraBuffer();
	DestroyMeshData();

	if( m_pMesh )
	{
		ioMeshManager::GetSingleton().Destroy( m_pMesh );
		m_pMesh = NULL;
	}
}

bool ioEntity::BuildEntity( const ioSkeleton *pSkeleton, bool bSoftware )
{
	if( m_bDoneEntityBuild )
		return true;

	if( !m_pMesh->IsLoaded() )
		return false;

	m_MeshType = m_pMesh->GetMeshType();

	if( pSkeleton )
	{
		BuildMeshData( bSoftware );
		BuildBipedIndexList( pSkeleton );
	}
	else
	{
		BuildMeshData( false );
	}

	m_bDoneEntityBuild = true;
	return true;
}

void ioEntity::BuildSubEntityList()
{
	int iNumSubMesh = m_pMesh->GetNumSubMesh();

	m_SubEntityList.clear();
	m_SubEntityList.reserve( iNumSubMesh );

	for( int i=0 ; i<iNumSubMesh; i++ )
	{
		ioSubMesh *pSubMesh = m_pMesh->GetSubMesh(i);
		if( pSubMesh )
		{
			ioSubEntity *pSubEnt = new ioSubEntity( this, pSubMesh );
			if( pSubEnt )
			{
				pSubEnt->SetMaterialName( pSubMesh->GetMaterialName() );
				m_SubEntityList.push_back( pSubEnt );
			}
		}
	}
}

void ioEntity::SetMaterialName( const ioHashString &name )
{
	SubEntityList::iterator iter;
	for( iter=m_SubEntityList.begin() ; iter!=m_SubEntityList.end() ; ++iter )
	{
		(*iter)->SetMaterialName( name );
	}
}

void ioEntity::SetCustomParameter( const ioShaderCustomArg &rkArg )
{
	SubEntityList::iterator iter;
	for( iter=m_SubEntityList.begin() ; iter!=m_SubEntityList.end() ; ++iter )
	{
		(*iter)->SetCustomParameter( rkArg );
	}
}

void ioEntity::ConvertToVertexColor()
{
	if( m_MeshType != MT_STATIC )	return;

	ioVertexDeclaration *pDecl = m_pMeshData->m_pVertexDeclaration;
	pDecl->InsertStreamSource( 2, IOFVF_COLOR0 );
	pDecl->ModifyStreamSource( 2, IOFVF_UV0, 3, IOFVF_UV0 );

	ioVertexBufferInstance *pExtra = CreateExtraBuffer( BT_MANAGED, IOFVF_COLOR0 );
	m_pMeshData->SetBinding( 2, pExtra );

	ioVertexBufferInstance *pUVBuf = m_pMesh->GetVertexBufferInstanceByMask( IOFVF_UV0 );
	m_pMeshData->SetBinding( 3, pUVBuf );

	m_MeshType = MT_STATIC_VERTEX_COLOR;

	SubEntityList::iterator iter = m_SubEntityList.begin();
	for( ; iter!=m_SubEntityList.end() ; ++iter )
	{
		(*iter)->ConvertShaderStaticToVertexColor();
	}
}

bool ioEntity::HasSkeleton() const
{
	if( m_pParent )
		return m_pParent->HasSkeleton();

	return false;
}

bool ioEntity::IsShadowRecvRender() const
{
	if( m_pParent )
		return m_pParent->IsShadowRecvRender();

	return false;
}

bool ioEntity::IsLoadDone()
{
	if( !m_bDoneEntityBuild )
		return false;

	SubEntityList::iterator iter;
	for( iter=m_SubEntityList.begin() ; iter!=m_SubEntityList.end() ; ++iter )
	{
		if( !(*iter)->IsMaterialLoadDone() )
			return false;
	}

	return true;
}

ioSubEntity* ioEntity::GetSubEntity( int iIndex )
{
	if( COMPARE( iIndex, 0, GetNumSubEntity() ) )
		return m_SubEntityList[iIndex];

	LOG.PrintTimeAndLog( 0, "ioEntity::GetSubEntity - Overflow(%d/%d)", iIndex, GetNumSubEntity() );

	return NULL;
}

const ioHashString& ioEntity::GetMeshName() const
{
	return m_pMesh->GetName();
}

int ioEntity::GetMeshVertexCount() const
{
	return m_pMesh->GetVertexCount();
}

int ioEntity::GetMeshFaceCount() const
{
	return m_pMesh->GetFaceCount();
}

ioVertexBufferInstance* ioEntity::CreateExtraBuffer( BufferType bType, DWORD dwMask )
{
	DestroyExtraBuffer();

	m_pExtraBuf = g_VtxBufMgr.CreateInstance( bType, dwMask, m_pMeshData->m_iVertexCount );

	return m_pExtraBuf;
}

void ioEntity::DestroyExtraBuffer()
{
	if( m_pExtraBuf )
	{
		g_VtxBufMgr.DestroyInstance( m_pExtraBuf );
		m_pExtraBuf = NULL;
	}
}

void ioEntity::BuildBipedIndexList( const ioSkeleton *pSkeleton )
{
	const ioHashStringVec &rkBipedNameList = m_pMesh->GetBipedNameList();
	if( rkBipedNameList.empty() )
		return;

	SAFEDELETEARRAY( m_aBipedIndexList );
	
	m_iApplyBipedCount = (int)rkBipedNameList.size();
	m_aBipedIndexList  = new int[m_iApplyBipedCount];

	for( int i=0 ; i<m_iApplyBipedCount; i++ )
	{
		m_aBipedIndexList[i] = pSkeleton->GetBipedIndex( rkBipedNameList[i] );
	}

	if( m_aBipedXformList )
	{
		_aligned_free( m_aBipedXformList );
		m_aBipedXformList = NULL;
	}

	m_aBipedXformList = (D3DXVECTOR4*)_aligned_malloc( m_iApplyBipedCount * 3 * sizeof(D3DXVECTOR4), 16 ); 
}

float ioEntity::GetSquaredViewDepth( const ioCamera *pCamera )
{
	return m_pParent->GetSquaredViewDepth( pCamera );
}

void ioEntity::GetWorldTransform( D3DXMATRIX *pMatWorld ) const
{
	m_pParent->GetEntityTransform( pMatWorld );
}

const D3DXVECTOR4* ioEntity::GetBipedList( int &iBipedListCnt ) const
{
	if( HasSkeleton() )
	{
		iBipedListCnt = m_iApplyBipedCount * 3;
		return m_aBipedXformList;
	}

	iBipedListCnt = 0;
	return NULL;
}

const ioAxisAlignBox& ioEntity::GetBoundBox() const
{
	return m_pMesh->GetBoundBox();
}

float ioEntity::GetBoundRadius() const
{
	return m_pMesh->GetBoundRadius();
}

void ioEntity::UpdateLightList( const ioLightList &rkList,
								const D3DXQUATERNION &qtParentRot,
								const D3DXVECTOR3 &vParentPos,
								const D3DXVECTOR3 &vParentScale )
{
	if( !m_bDoneEntityBuild )
		return;

	D3DXVECTOR3 vLocalPos = m_pMesh->GetBoundBox().GetCenter();
	D3DXVECTOR3 vDerivedPos = qtParentRot * ( vLocalPos * vParentScale ) + vParentPos;

	float fRadius = m_pMesh->GetBoundRadius();
	fRadius *= max( vParentScale.x, max( vParentScale.y, vParentScale.z ) );

	float fRadiusSq = fRadius * fRadius;
	m_ApplyLightList.clear();

	ioLightList::const_iterator iter;
	for( iter=rkList.begin() ; iter!=rkList.end() ; ++iter )
	{
		ioLight *pLight = *iter;
		if( !pLight ) continue;

		if( pLight->GetType() == LT_DIRECTIONAL )
		{
			pLight->m_fTempSquareDist = 0.0f;
			m_ApplyLightList.push_back( pLight );
		}
		else
		{
			D3DXVECTOR3 vDiff = pLight->GetDerivedPosition() - vDerivedPos;
			pLight->m_fTempSquareDist = D3DXVec3LengthSq( &vDiff );

			if( pLight->m_fTempSquareDist - fRadiusSq < pLight->GetSquareRange() )
			{
				m_ApplyLightList.push_back( pLight );
			}
		}
	}

	if( m_ApplyLightList.size() > 1 )
	{
		sort( m_ApplyLightList.begin(), m_ApplyLightList.end(), LightLess() );
	}
}

void ioEntity::UpdateRenderQueue( ioRenderQueue &queue, RenderGroupID eGroupID )
{
	if( !m_bDoneEntityBuild )
		return;

	// 소프트스키닝인데 버퍼가 제대로 업데이트 되지 못했다.
	if( m_bSoftSkinningMeshData && !m_bUpdateBufferOK )
		return;

	SubEntityList::iterator iter;
	for( iter=m_SubEntityList.begin(); iter!=m_SubEntityList.end(); ++iter )
	{
		ioSubEntity *pSubEnt = *iter;
		if( pSubEnt && pSubEnt->GetFaceCount() > 0 )
		{
			queue.AddRenderable( pSubEnt, eGroupID );
		}
	}
}

void ioEntity::UpdateGenShadowMapToQueue( ioRenderQueue &queue, RenderGroupID eGroupID )
{
	if( !m_bDoneEntityBuild )	return;

	// 소프트스키닝인데 버퍼가 제대로 업데이트 되지 못했다.
	if( m_bSoftSkinningMeshData && !m_bUpdateBufferOK )
		return;

	SubEntityList::iterator iter;
	for( iter=m_SubEntityList.begin() ; iter!=m_SubEntityList.end() ; ++iter )
	{
		ioSubEntity *pSubEnt = *iter;
		if( pSubEnt && pSubEnt->GetFaceCount() > 0 )
		{
			queue.AddGenerateShadowMapRenderable( pSubEnt );
		}
	}
}

void ioEntity::DestroyMeshData()
{
	if( !m_pMeshData )	return;

	if( m_bSoftSkinningMeshData )
	{
		ioVertexBufferInstance *pInstance = m_pMeshData->m_pVertexBufferBinder->UnSetBinding( 0 );
		if( pInstance )
		{
			g_VtxBufMgr.DestroyInstance( pInstance );
		}
	}

	delete m_pMeshData;
	m_pMeshData = NULL;
}

void ioEntity::BuildMeshData( bool bSoftSkinning )
{
	if( !m_pMesh->IsLoaded() )
		return;

	if( m_pMesh->GetMeshType() != MT_ANIMATION )
	{
		BuildHardwareMeshData();
	}
	else
	{
		if( !bSoftSkinning )
		{
			BuildHardwareMeshData();

			// 중간에 MT_ANIMATION -> MT_STATIC으로 교체 된적이 있다.
			if( m_MeshType == MT_STATIC )
			{
				ConvertMaterialShaderStaticToAnimate();
				m_MeshType = MT_ANIMATION;
			}
		}
		else
		{
			BuildSoftSkinningMeshData();
			ConvertMaterialShaderAnimateToStatic();
			m_MeshType = MT_STATIC;
		}
	}
}

void ioEntity::BuildHardwareMeshData()
{
	DestroyMeshData();

	m_pMeshData = new ioMeshData;

	int iVtxBufCnt = m_pMesh->GetVertexBufferInstanceCnt();
	for( int i=0 ; i<iVtxBufCnt ; i++ )
	{
		ioVertexBufferInstance *pVtxInstance = m_pMesh->GetVertexBufferInstance( i );
		m_pMeshData->InsertStreamSource( i, pVtxInstance->GetVertexMask() );
		m_pMeshData->SetBinding( i, pVtxInstance );
	}

	m_pMeshData->m_pIndexBufferInstance = m_pMesh->GetIndexBufferInstance();
	m_pMeshData->m_iVertexCount = m_pMesh->GetVertexCount();
	m_pMeshData->m_iIndexCount  = m_pMesh->GetIndexCount();

	m_bSoftSkinningMeshData = false;
}

#define MAX_SOFT_BUFFER_COUNT 2

// 소프트스키닝은 현재 전형적인 ( pos + normal + uv ) 포멧에 대해서만 작동한다.
// 즉 tangent + binormal이 들어가는 노말맵은 지원되지 않는다.
void ioEntity::BuildSoftSkinningMeshData()
{
	DestroyMeshData();

	m_pMeshData = new ioMeshData;

	int iVertexCount = GetMeshVertexCount();

	ioVertexBufferInstance *pPosNrm = NULL;
	pPosNrm = g_VtxBufMgr.CreateExclusiveInstance( BT_DYNAMIC,
												   IOFVF_POSITION | IOFVF_NORMAL,
												   iVertexCount * MAX_SOFT_BUFFER_COUNT );

	m_pMeshData->InsertStreamSource( 0, IOFVF_POSITION | IOFVF_NORMAL );
	m_pMeshData->SetBinding( 0, pPosNrm );

	ioVertexBufferInstance *pHardUV = m_pMesh->GetVertexBufferInstanceByMask( IOFVF_UV0 );
	m_pMeshData->InsertStreamSource( 1, IOFVF_UV0 );
	m_pMeshData->SetBinding( 1, pHardUV );

	m_pMeshData->m_pIndexBufferInstance = m_pMesh->GetIndexBufferInstance();
	m_pMeshData->m_iVertexCount = m_pMesh->GetVertexCount();
	m_pMeshData->m_iIndexCount  = m_pMesh->GetIndexCount();

	m_iSoftBufferStartPos = 1;	// 처음부터 DISCARD안하게 하기 위함
	m_bSoftSkinningMeshData = true;
}

void ioEntity::ConvertMaterialShaderStaticToAnimate()
{
	SubEntityList::iterator iter = m_SubEntityList.begin();
	for( ; iter!=m_SubEntityList.end() ; ++iter )
	{
		(*iter)->ConvertShaderStaticToAnimate();
	}
}

void ioEntity::ConvertMaterialShaderAnimateToStatic()
{
	SubEntityList::iterator iter = m_SubEntityList.begin();
	for( ; iter!=m_SubEntityList.end() ; ++iter )
	{
		(*iter)->ConvertShaderAnimateToStatic();
	}
}

void ioEntity::CacheBipedMatrices( const ioSkeleton *pSkeleton )
{
	if( !pSkeleton || !m_aBipedXformList || !m_aBipedIndexList )
		return;

	D3DXVECTOR4 *pOffset = m_aBipedXformList;

	for( int i=0 ; i<m_iApplyBipedCount ; i++ )
	{
		ioBiped *pBiped = pSkeleton->GetBiped( m_aBipedIndexList[i] );

		if( !pBiped )
		{
			ErrorReport::SetFormat( "%s:%s:%d", GetMeshName().c_str(),
												pSkeleton->GetName().c_str(),
												m_aBipedIndexList[i] );
		}

		memcpy( pOffset,
				pBiped->GetTransposeBipedSpaceFullXform(),
				sizeof(D3DXVECTOR4) * 3 );

		pOffset += 3;
	}
}

void ioEntity::SoftwareVertexBlending()
{
	m_bUpdateBufferOK = true;

	if( !m_bDoneEntityBuild || m_pMesh->GetMeshType() != MT_ANIMATION )
	{
		m_bUpdateBufferOK = false;
		return;
	}

	ioVertexBufferInstance *pInstance = m_pMeshData->GetVtxInstance( 0 );
	if( !pInstance )
	{
		m_bUpdateBufferOK = false;
		return;
	}

	int iLockSizeBytes  = sizeof(ioVtxNrm) * m_pMeshData->m_iVertexCount;
	int iLockStartBytes = iLockSizeBytes * m_iSoftBufferStartPos;

	DWORD dwLockFlags = D3DLOCK_NOOVERWRITE;
	if( m_iSoftBufferStartPos == 0 )
	{
		dwLockFlags = D3DLOCK_DISCARD;
	}

	if( !pInstance->Lock( dwLockFlags, iLockStartBytes, iLockSizeBytes ) )
	{
		m_bUpdateBufferOK = false;
		return;
	}

	__try
	{
		ioCPU::TransformVertsAndNormals( pInstance->GetBuffer(),
										 m_pMesh->GetVertexCount(),
										 m_aBipedXformList,
										 m_pMesh->GetSoftPosNrm(),
										 m_pMesh->GetSoftJointWeights() );
	}
	__except( ErrorReport::DynamicBufferFilter( GetExceptionCode() ) )
	{
		m_bUpdateBufferOK = false;
	}

	pInstance->UnLock();
	pInstance->SetExtraOffset( iLockStartBytes );

	m_iSoftBufferStartPos = ( m_iSoftBufferStartPos + 1 ) % MAX_SOFT_BUFFER_COUNT;
}

int ioEntity::GetControlPointCnt() const
{
	if( m_pMesh )
		return m_pMesh->GetControlPointCnt();

	return 0;
}

const ioMeshControlPoint* ioEntity::GetControlPoint( const ioHashString &szType, int iIndex ) const
{
	if( m_pMesh )
		return m_pMesh->GetControlPoint( szType, iIndex );

	return NULL;
}

const ioMeshControlPoint* ioEntity::GetControlPoint( int iArray ) const
{
	if( m_pMesh )
		return m_pMesh->GetControlPoint( iArray );

	return NULL;
}

ConstCtrlPointList ioEntity::GetControlPointList( const ioHashString &szType )
{
	if( m_pMesh )
		return m_pMesh->GetControlPointList( szType );

	return ConstCtrlPointList();
}