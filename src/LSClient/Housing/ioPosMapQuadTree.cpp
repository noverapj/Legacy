#include "stdafx.h"

#include "BlockDefine.h"
#include "ioPosMapQuadTree.h"

#include "../ioPlayStage.h"

ioPosMapQuadTree::ioPosMapQuadTree()
{
	for( int i = 0; i < FT_MAX; ++i )
		m_pChild[i] = NULL;
		
	m_nSquared	= 0;
	m_nTileMax	= 0;

	m_CollRot	= ioMath::QUAT_IDENTITY;
	m_vWorldPos	= ioMath::VEC3_ZERO;
}

ioPosMapQuadTree::~ioPosMapQuadTree()
{
	for( int i = 0; i < FT_MAX; ++i )
		SAFEDELETE( m_pChild[i] );
}

void ioPosMapQuadTree::Build( const D3DXVECTOR3& vPos, int nFaceType, int nFaceMax, int nSquared, int nTileMax )
{
	m_nTileMax = nTileMax;

	switch( nFaceType )
	{
	case FT_1_4:
		m_FacePos = FacePos( 0, 0 );
		break;
	case FT_2_4:
		m_FacePos = FacePos( nFaceMax, 0 );
		break;
	case FT_3_4:
		m_FacePos = FacePos( 0, nFaceMax );
		break;
	case FT_4_4:
		m_FacePos = FacePos( nFaceMax, nFaceMax );
		break;
	default:
		m_FacePos = FacePos( 0, 0 );
		break;
	}

	m_vWorldPos			= vPos;
	m_nSquared			= nSquared;
	
	UpdateCollBox( nSquared );

	nSquared = nSquared * 2;

	AddChild( FT_1_4, vPos, nFaceMax/2, this, nSquared );
	AddChild( FT_2_4, vPos, nFaceMax/2, this, nSquared );
	AddChild( FT_3_4, vPos, nFaceMax/2, this, nSquared );
	AddChild( FT_4_4, vPos, nFaceMax/2, this, nSquared );
}

void ioPosMapQuadTree::AddChild( FaceType eType, const D3DXVECTOR3& vPos, int nFaceMax, ioPosMapQuadTree* pParents, int nSquared )
{
	if( nSquared > m_nTileMax )
		return;

	int  nUnit = (UD_SIZE * m_nTileMax) / nSquared / 2;
	D3DXVECTOR3 vChildPos = vPos;
	switch( eType )
	{
	case FT_1_4:
		{
			vChildPos.x -= nUnit;
			vChildPos.z += nUnit;
		}
		break;
	case FT_2_4:
		{
			vChildPos.x += nUnit;
			vChildPos.z += nUnit;
		}
		break;
	case FT_3_4:
		{
			vChildPos.x -= nUnit;
			vChildPos.z -= nUnit;
		}
		break;
	case FT_4_4:
		{
			vChildPos.x += nUnit;
			vChildPos.z -= nUnit;			
		}
		break;
	}

	m_pChild[eType] = new ioPosMapQuadTree();
	m_pChild[eType]->Build( vChildPos, eType, nFaceMax, nSquared, m_nTileMax );
}

bool ioPosMapQuadTree::IsLeaf()
{
	if( m_nSquared == m_nTileMax )
		return true;

	return false;
}

bool ioPosMapQuadTree::IsRoot()
{
	if( m_nSquared == 1 )
		return true;

	return false;
}

void ioPosMapQuadTree::SetCollBoxRotaion( const D3DXQUATERNION& qtRot )
{
	m_CollRot	= qtRot;
	m_vWorldPos = qtRot * m_vWorldPos;

	UpdateCollBox( m_nSquared );

	for( int i = 0; i < FT_MAX; ++i )
	{
		if( m_pChild[i] )
			m_pChild[i]->SetCollBoxRotaion( qtRot );
	}	
}

void ioPosMapQuadTree::UpdateCollBox( int nSquared )
{
	D3DXVECTOR3 vPos = m_vWorldPos;	
	
	D3DXVECTOR3 vMin, vMax;
	vMax = vMin = vPos;

	vMin.x -= ( (UD_SIZE * m_nTileMax) / nSquared ) / 2;
	vMax.x += ( (UD_SIZE * m_nTileMax) / nSquared ) / 2;

	vMin.y -= 2.5f;	
	vMax.y += 2.5f;

	vMin.z -= ( (UD_SIZE * m_nTileMax) / nSquared ) / 2;
	vMax.z += ( (UD_SIZE * m_nTileMax) / nSquared ) / 2;

	ioOrientBox kBox;
	kBox.SetBoxByMinMax( vMin, vMax );
	kBox.Transform( vPos, ioMath::UNIT_ALL, m_CollRot );
	kBox.SetCenter( vPos );
	m_CollBox = kBox;
}

bool ioPosMapQuadTree::TestIntersection( const ioRay& kYRay )
{	
	if( ioMath::TestIntersection( kYRay, m_CollBox ) )
		return true;

	return false;
}

bool ioPosMapQuadTree::FindBlockIndex( ioRay& kYRay, int& XPos, int& ZPos )
{
	if( TestIntersection( kYRay ) )
	{
		if( IsLeaf() )
		{
			XPos = m_FacePos.m_XPos;
			ZPos = m_FacePos.m_ZPos;
			return true;
		}
		else
		{
			for( int i = 0; i < FT_MAX; ++i )
			{
				if( m_pChild[i]->FindBlockIndex( kYRay, XPos, ZPos ) )
				{
					XPos += m_FacePos.m_XPos; 
					ZPos += m_FacePos.m_ZPos;
					return true;
				}
			}
		}
	}

	return false;
}

bool ioPosMapQuadTree::FindBlockIndexList( const ioRay& kYRay, IntVec& vList )
{
	if( TestIntersection( kYRay ) )
	{
		if( IsLeaf() )
		{
			vList.push_back( m_FacePos.m_XPos + m_FacePos.m_ZPos * m_nTileMax );
		}
		else
		{
			for( int i = 0; i < FT_MAX; ++i )
			{
				if( m_pChild[i]->FindBlockIndexList( kYRay, vList ) )
				{					
					return true;
				}
			}
		}
	}

	return false;
}

void ioPosMapQuadTree::RenderCollision( ioPlayStage* pStage, ioCamera* pCamera )
{
	ioSphere kSphere( m_CollBox.GetCenter(), m_CollBox.GetBoxRadius() );
	if( !pCamera || !pCamera->IsVisible( kSphere ) )
		return;

	D3DXVECTOR3 vVtxArray[8];
	m_CollBox.GetVertices( vVtxArray );
	pStage->RenderBox( vVtxArray, 0xff0000ff );

	for( int i = 0; i < FT_MAX; ++i )
	{
		if( m_pChild[i] )
			m_pChild[i]->RenderCollision( pStage, pCamera );
	}
}