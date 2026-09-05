

#include "StdAfx.h"

#include <malloc.h>

#include "ioMesh.h"
#include "ioMeshManager.h"

#include "ioVertexDeclaration.h"
#include "ioVertexBufferBinder.h"
#include "ioVertexBufferInstance.h"
#include "ioVertexBufferManager.h"

#include "ioIndexBufferInstance.h"
#include "ioIndexBufferManager.h"

#include "ioSubMesh.h"
#include "ioCPU.h"

ioMesh::ioMesh( const ioHashString &name ) : ioResource( name )
{
	m_MeshType = MT_STATIC;
	m_pIndexInstance = NULL;

	m_iVertexCount = 0;
	m_iIndexCount  = 0;

	m_pPosNrm = NULL;
	m_pJointWeight = NULL;
	m_pIndexBuf = NULL;

	m_fBoundRadius = 0.0f;
	m_iWeightedBipedCnt = 0;
}

ioMesh::~ioMesh()
{
	SubMeshList::iterator iSubMesh;
	for( iSubMesh=m_SubMeshList.begin() ; iSubMesh!=m_SubMeshList.end() ; ++iSubMesh )
	{
		delete *iSubMesh;
	}
	m_SubMeshList.clear();

	DestroyHardwareBuffers();

	SoftVertexBufferList::iterator iSoft;
	for( iSoft=m_SoftVertexBufferList.begin() ; iSoft!=m_SoftVertexBufferList.end() ; ++iSoft )
	{
		delete []iSoft->second;
	}
	m_SoftVertexBufferList.clear();

	if( m_pPosNrm )
	{
		_aligned_free( m_pPosNrm );
		m_pPosNrm = NULL;
	}

	SAFEDELETEARRAY( m_pJointWeight );
	SAFEDELETEARRAY( m_pIndexBuf );

	ClearAllControlPoints();

	g_MeshMgr.RemoveMem( GetName() );
}

ioSubMesh* ioMesh::CreateSubMesh()
{
	ioSubMesh *pSubMesh = new ioSubMesh( this );
	if( pSubMesh )
	{
		m_SubMeshList.push_back( pSubMesh );
	}

	return pSubMesh;
}

int ioMesh::GetNumSubMesh() const
{
	return m_SubMeshList.size();
}

ioSubMesh* ioMesh::GetSubMesh( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetNumSubMesh() ) )
		return m_SubMeshList[iIndex];

	return NULL;
}

ioMesh::SubMeshIterator ioMesh::GetSubMeshIterator()
{
	return SubMeshIterator( m_SubMeshList.begin(), m_SubMeshList.end() );
}

bool ioMesh::AddControlPoint( ioMeshControlPoint *pPoint )
{
	std::pair< CtrlPointSet::iterator, bool > ret = m_ControlPointSet.insert( pPoint );

	if( !ret.second )
	{
		LOG.PrintTimeAndLog( 0, "ioMesh::AddControlPoint - %s, %d Point Exist",
								pPoint->GetType().c_str(), pPoint->GetIndex() );
		return false;
	}

	return true;
}

const ioMeshControlPoint* ioMesh::GetControlPoint( const ioHashString &szType,
												   int iIndex ) const
{
	ioMeshControlPoint kPoint;
	kPoint.SetTypeIndex( szType.c_str(), iIndex );

	CtrlPointSet::const_iterator iter = m_ControlPointSet.find( &kPoint );
	if( iter != m_ControlPointSet.end() )
		return *iter;

	return NULL;
}

const ioMeshControlPoint* ioMesh::GetControlPoint( int iArray ) const
{
	if( COMPARE( iArray, 0, GetControlPointCnt() ) )
	{
		CtrlPointSet::const_iterator iter = m_ControlPointSet.begin();
		std::advance( iter, iArray );
		return *iter;
	}

	return NULL;
}

ConstCtrlPointList ioMesh::GetControlPointList( const ioHashString &szType )
{
	ConstCtrlPointList kRetList;

	CtrlPointSet::const_iterator iter;
	for( iter=m_ControlPointSet.begin() ; iter!=m_ControlPointSet.end() ; ++iter )
	{
		if( (*iter)->GetType() == szType )
		{
			kRetList.push_back( *iter );
		}
	}

	if( !kRetList.empty() )
	{
		kRetList.sort( CPSort() );
	}

	return kRetList;
}

void ioMesh::ClearAllControlPoints()
{
	CtrlPointSet::iterator iter;
	for( iter=m_ControlPointSet.begin() ; iter!=m_ControlPointSet.end() ; ++iter )
	{
		delete *iter;
	}
	m_ControlPointSet.clear();
}

void ioMesh::DestroyHardwareBuffers()
{
	VertexBufferList::iterator iter;
	for( iter=m_VertexBufferList.begin() ; iter!=m_VertexBufferList.end() ; ++iter )
	{
		g_VtxBufMgr.DestroyInstance( *iter );
	}
	m_VertexBufferList.clear();

	if( m_pIndexInstance )
	{
		g_IdxBufMgr.DestroyInstance( m_pIndexInstance );
		m_pIndexInstance = NULL;
	}
}

void ioMesh::AddVertexBuffer( ioVertexBufferInstance *pVtxBuf )
{
	m_VertexBufferList.push_back( pVtxBuf );
}

void ioMesh::SetIndexBufferInstance( ioIndexBufferInstance *pInstance )
{
	m_pIndexInstance = pInstance;
}

void ioMesh::SetBipedNameList( const ioHashStringVec &rkNameList )
{
	m_BipedNameList = rkNameList;
}

void ioMesh::AddSoftVertexBuffer( VertexComponent eComponent, void *pBuffer )
{
	SoftVertexBufferList::iterator iter = m_SoftVertexBufferList.find( eComponent );
	if( iter != m_SoftVertexBufferList.end() )
	{
		delete []iter->second;
		iter->second = pBuffer;
	}
	else
	{
		m_SoftVertexBufferList.insert( SoftVertexBufferList::value_type( eComponent, pBuffer ) );
	}
}

void ioMesh::SetSoftIndexBuffer( WORD *pBuf )
{
	SAFEDELETEARRAY( m_pIndexBuf );
	m_pIndexBuf = pBuf;
}

void ioMesh::SetSoftBlendWeight( const BlendWeight *pWeights, int iVtxCount, int iJointCount )
{
	SAFEDELETEARRAY( m_pJointWeight );
	m_pJointWeight = new JointWeight[iJointCount];

	JointWeight *pCurJoint = m_pJointWeight;
	JointWeight *pPreJoint = m_pJointWeight;

	const BlendWeight *pCurWeight = pWeights;
	for( int i=0 ; i<iVtxCount ; i++ )
	{
		for( int j=0 ; j<MAX_BLEND_BIPED ; j++ )
		{
			if( pCurWeight->fWeight[j] > 0.0f )
			{
				pCurJoint->fWeight = pCurWeight->fWeight[j];
				pCurJoint->iJointMatOffset = (int)pCurWeight->fBipedID[j] * 3 * sizeof(D3DXVECTOR4);
				pCurJoint->iNextVertexOffset = 0;

				pPreJoint = pCurJoint;
				pCurJoint++;
			}
			else
			{
				pPreJoint->iNextVertexOffset = sizeof(JointWeight);
				break;
			}
		}

		pCurWeight++;
	}
}

void ioMesh::BuildSoftSkinningBuffers( int iVertexCount )
{
	const D3DXVECTOR3 *pPos = (const D3DXVECTOR3 *)GetSoftVertexBuffer( IOFVF_POSITION );
	const D3DXVECTOR3 *pNrm = (const D3DXVECTOR3 *)GetSoftVertexBuffer( IOFVF_NORMAL );
	if( !pPos || !pNrm )
	{
		LOG.PrintTimeAndLog( 0, "ioMesh::BuildSoftSkinningBuffers - %s Failed(%x, %x)",
								GetName().c_str(), pPos, pNrm );
		return;
	}

	if( m_pPosNrm )
	{
		_aligned_free( m_pPosNrm );
		m_pPosNrm = NULL;
	}

	m_pPosNrm = (ioVtxNrm4*)_aligned_malloc( iVertexCount * sizeof(ioVtxNrm4), 16 );
	if( m_pPosNrm )
	{
		ioVtxNrm4 *pDest = m_pPosNrm;
		for( int i=0 ; i<iVertexCount ; i++ )
		{
			pDest->vPos.x = pPos->x;
			pDest->vPos.y = pPos->y;
			pDest->vPos.z = pPos->z;
			pDest->vPos.w = 1.0f;

			pDest->vNrm.x = pNrm->x;
			pDest->vNrm.y = pNrm->y;
			pDest->vNrm.z = pNrm->z;
			pDest->vNrm.w = 0.0f;

			pDest++;
			pPos++;
			pNrm++;
		}
	}
}

bool ioMesh::CheckSoftNormalBuffer()
{
	if( GetSoftVertexBuffer( IOFVF_NORMAL ) )
		return true;

	if( m_iVertexCount == 0 || m_iIndexCount == 0 )
		return false;

	if( !m_pIndexBuf )
		return false;

	D3DXVECTOR3 *pPosBuf = (D3DXVECTOR3*)GetSoftVertexBuffer( IOFVF_POSITION );
	if( !pPosBuf )
		return false;

	D3DXVECTOR3 *pNormal = new D3DXVECTOR3[m_iVertexCount];
	ZeroMemory( pNormal, sizeof(D3DXVECTOR3) * m_iVertexCount );

	int *pNormalCount = new int[m_iVertexCount];
	ZeroMemory( pNormalCount, sizeof(int)*m_iVertexCount );

	int i=0;

	WORD wIdx0, wIdx1, wIdx2;
	D3DXVECTOR3 vNormal, vEdge10, vEdge20;

	int iIndexStart = 0;
	int iFaceCount  = GetFaceCount();
	for( i=0 ; i<iFaceCount ; i++ )
	{
		wIdx0 = m_pIndexBuf[ iIndexStart+0 ];
		wIdx1 = m_pIndexBuf[ iIndexStart+1 ];
		wIdx2 = m_pIndexBuf[ iIndexStart+2 ];

		vEdge10 = pPosBuf[wIdx1] - pPosBuf[wIdx0];
		vEdge20 = pPosBuf[wIdx2] - pPosBuf[wIdx0];

		D3DXVec3Normalize( &vEdge10, &vEdge10 );
		D3DXVec3Normalize( &vEdge20, &vEdge20 );

		D3DXVec3Cross( &vNormal, &vEdge10, &vEdge20 );

		pNormal[wIdx0] += vNormal;
		pNormal[wIdx1] += vNormal;
		pNormal[wIdx2] += vNormal;

		pNormalCount[wIdx0]++;
		pNormalCount[wIdx1]++;
		pNormalCount[wIdx2]++;

		iIndexStart += 3;
	}

	for( i=0 ; i<m_iVertexCount ; i++ )
	{
		pNormal[i] /= pNormalCount[i];
		D3DXVec3Normalize( &pNormal[i], &pNormal[i] );
	}

	SAFEDELETEARRAY( pNormalCount );

	AddSoftVertexBuffer( IOFVF_NORMAL, pNormal );
	return true;
}

const void* ioMesh::GetSoftVertexBuffer( VertexComponent eComponent ) const
{
	SoftVertexBufferList::const_iterator iter = m_SoftVertexBufferList.find( eComponent );
	if( iter != m_SoftVertexBufferList.end() )
		return iter->second;

	return NULL;
}

ioVertexBufferInstance* ioMesh::GetVertexBufferInstance( int iIndex )
{
	if( COMPARE( iIndex, 0, GetVertexBufferInstanceCnt() ) )
		return m_VertexBufferList[iIndex];

	LOG.PrintTimeAndLog( 0, "ioMesh::GetVertexBufferInstance - overflow (%d/%d)",
							iIndex, GetVertexBufferInstanceCnt() );

	return NULL;
}

ioVertexBufferInstance* ioMesh::GetVertexBufferInstanceByMask( DWORD dwMask )
{
	VertexBufferList::iterator iter;
	for( iter=m_VertexBufferList.begin() ; iter!=m_VertexBufferList.end() ; ++iter )
	{
		if( (*iter)->GetVertexMask() == dwMask )
			return *iter;
	}

	return NULL;
}
