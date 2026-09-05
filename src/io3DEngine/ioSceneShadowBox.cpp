

#include "stdafx.h"

#include "ioMath.h"
#include "ioRay.h"
#include "ioCylinder.h"
#include "ErrorReport.h"

#include "ioCamera.h"
#include "ioMovableObject.h"
#include "ioSceneShadowBox.h"

ioSceneShadowBox::ioSceneShadowBox( ioCamera *pCamera, const D3DXVECTOR3 &vLightDir )
: m_pCamera( pCamera ),
  m_vLightDir( vLightDir )
{
	m_fClosingOffset = 1.0f;
	m_fExtendDiagonalRate = 3.0f;

	m_aOutSidePolygon = NULL;
	m_iCurOutSideArraySize = 0;
}

ioSceneShadowBox::~ioSceneShadowBox()
{
	SAFEDELETEARRAY( m_aOutSidePolygon );
}

bool ioSceneShadowBox::AddShadowCaster( ioMovableObject *pObj, float fScale )
{
	const ioAxisAlignBox &rkWorldBox = pObj->GetWorldBoundBox();

	if( m_pCamera->IsVisible( rkWorldBox ) )
	{
		AddShadowReceiveBox( rkWorldBox, fScale );
		return true;
	}
	else if( m_fExtendDiagonalRate > 0.0f )
	{
		D3DXVECTOR3 vEndPoint = rkWorldBox.GetCenter();
		vEndPoint += m_vLightDir * ( rkWorldBox.GetHalfDiagonal() * m_fExtendDiagonalRate );

		ioCylinder kCylinder( rkWorldBox.GetCenter(),
							  vEndPoint,
							  rkWorldBox.GetHalfDiagonal() );

		if( m_pCamera->IsVisible( kCylinder ) )
		{
			AddShadowReceiveBox( rkWorldBox, fScale );
			return true;
		}
	}

	return false;
}

void ioSceneShadowBox::AddShadowReceiveBox( const ioAxisAlignBox &kAxisBox, float fScale )
{
	if( fScale == 1.0f )
	{
		m_SceneBox.Merge( kAxisBox );
	}
	else
	{
		float fNewRange = kAxisBox.GetHalfDiagonal() * fScale;
		D3DXVECTOR3 vRange( fNewRange, fNewRange, fNewRange );
	
		m_SceneBox.Merge( kAxisBox.GetCenter() - vRange, 
						  kAxisBox.GetCenter() + vRange );
	}
}

void ioSceneShadowBox::SetMakeClosingPolygonOffset( float fOffset )
{
	m_fClosingOffset = fOffset;
}

void ioSceneShadowBox::SetExtendDiagonalRate( float fRate )
{
	m_fExtendDiagonalRate = fRate;
}

void ioSceneShadowBox::CreateShadowFrustum( Vector3Vec &kVolumnVtxList )
{
	if( !kVolumnVtxList.empty() )
	{
		kVolumnVtxList.erase( kVolumnVtxList.begin(), kVolumnVtxList.end() );
	}

	ioPolygonMesh kFrustum;
	MakeViewFrustumPolygon( kFrustum );

	PlaneList vScenePlaneList;
	CalcAABoxPlaneList( m_SceneBox, vScenePlaneList );

	ClipFrustomByPlaneList( vScenePlaneList, kFrustum );

	kFrustum.GetAllVtxList( kVolumnVtxList );
}

void ioSceneShadowBox::MakeViewFrustumPolygon( ioPolygonMesh &kFrustum )
{
	Vector3Vec kVolumnVtxList;
	kVolumnVtxList.reserve( 8 );
	m_pCamera->GetViewVolumnWorldVtx( kVolumnVtxList );

	ioPolygon kPolygon;
	kPolygon.Reserve(4);

	// Near Poly CCW
	kPolygon.AddPoint( kVolumnVtxList[0] );
	kPolygon.AddPoint( kVolumnVtxList[1] );
	kPolygon.AddPoint( kVolumnVtxList[2] );
	kPolygon.AddPoint( kVolumnVtxList[3] );
	kFrustum.AddPolygon( kPolygon );

	// Far Poly CCW
	kPolygon.Clear();
	kPolygon.AddPoint( kVolumnVtxList[7] );
	kPolygon.AddPoint( kVolumnVtxList[6] );
	kPolygon.AddPoint( kVolumnVtxList[5] );
	kPolygon.AddPoint( kVolumnVtxList[4] );
	kFrustum.AddPolygon( kPolygon );

	// Left Poly CCW
	kPolygon.Clear();
	kPolygon.AddPoint( kVolumnVtxList[0] );
	kPolygon.AddPoint( kVolumnVtxList[3] );
	kPolygon.AddPoint( kVolumnVtxList[7] );
	kPolygon.AddPoint( kVolumnVtxList[4] );
	kFrustum.AddPolygon( kPolygon );

	// Right Poly CCW
	kPolygon.Clear();
	kPolygon.AddPoint( kVolumnVtxList[1] );
	kPolygon.AddPoint( kVolumnVtxList[5] );
	kPolygon.AddPoint( kVolumnVtxList[6] );
	kPolygon.AddPoint( kVolumnVtxList[2] );
	kFrustum.AddPolygon( kPolygon );

	// Bottom Poly CCW
	kPolygon.Clear();
	kPolygon.AddPoint( kVolumnVtxList[4] );
	kPolygon.AddPoint( kVolumnVtxList[5] );
	kPolygon.AddPoint( kVolumnVtxList[1] );
	kPolygon.AddPoint( kVolumnVtxList[0] );
	kFrustum.AddPolygon( kPolygon );

	// Top Poly CCW
	kPolygon.Clear();
	kPolygon.AddPoint( kVolumnVtxList[6] );
	kPolygon.AddPoint( kVolumnVtxList[7] );
	kPolygon.AddPoint( kVolumnVtxList[3] );
	kPolygon.AddPoint( kVolumnVtxList[2] );
	kFrustum.AddPolygon( kPolygon );
}

void ioSceneShadowBox::CalcAABoxPlaneList( const ioAxisAlignBox &kSceneBox,
										   std::vector< ioPlane > &kPlaneList )
{
	kPlaneList.reserve( 6 );

	D3DXVECTOR3 vMin, vMax;
	vMin = kSceneBox.GetMinPos();
	vMax = kSceneBox.GetMaxPos();

	ioPlane kPlane;

	// bottom plane
	kPlane.SetPlane( -ioMath::UNIT_Y, vMin );
	kPlaneList.push_back( kPlane );

	// top plane
	kPlane.SetPlane( ioMath::UNIT_Y, vMax );
	kPlaneList.push_back( kPlane );

	// left plane
	kPlane.SetPlane( -ioMath::UNIT_X, vMin );
	kPlaneList.push_back( kPlane );

	// right plane
	kPlane.SetPlane( ioMath::UNIT_X, vMax );
	kPlaneList.push_back( kPlane );

	// back plane
	kPlane.SetPlane( ioMath::UNIT_Z, vMax );
	kPlaneList.push_back( kPlane );

	// front plane
	kPlane.SetPlane( -ioMath::UNIT_Z, vMin );
	kPlaneList.push_back( kPlane );
}

void ioSceneShadowBox::ClipFrustomByPlaneList( const PlaneList &rkPlaneList, ioPolygonMesh &kFrustum )
{
	ioPolygon kOutPoly, kInterPoly;

	int iPlaneCnt = rkPlaneList.size();
	for( int i=0 ; i<iPlaneCnt ; i++ )
	{
		int iNumPolygon = kFrustum.GetNumPolygon();
		if( iNumPolygon == 0 )
			break;

		ioPolygonMesh kIntersectMesh;
		ioPolygonMesh kCopyMesh( kFrustum );
		kFrustum.ClearPolygonList();

		const ioPlane &rkPlane = rkPlaneList[i];
		for( int j=0 ; j<iNumPolygon ; j++ )
		{
			kOutPoly.Clear();
			kInterPoly.Clear();

			ClipPolygonByPlane( kCopyMesh.GetPolygon(j), rkPlane, kOutPoly, kInterPoly );

			if( !kOutPoly.IsEmpty() )
			{
				kFrustum.AddPolygon( kOutPoly );
			}

			if( !kInterPoly.IsEmpty() )
			{
				if( kInterPoly.Size() == 1 )
				{
					kInterPoly.AddPoint( kInterPoly.vPoint[0] );
				}

				kIntersectMesh.AddPolygon( kInterPoly );
			}
		}

		MakeClosingPolygon( kIntersectMesh, kFrustum, rkPlane.GetNormal() );
	}
}

void ioSceneShadowBox::ClipPolygonByPlane( const ioPolygon &rkSrcPoly,
										   const ioPlane &kPlane,
										   ioPolygon &kOutPoly,
										   ioPolygon &kInterPoly )
{
	int iPolyVtxCnt = rkSrcPoly.Size();
	if( iPolyVtxCnt < 3 )
		return;

	CheckOutSidePolygonArray( iPolyVtxCnt );

	for( int i=0 ; i<iPolyVtxCnt ; i++ )
	{
		m_aOutSidePolygon[i] = kPlane.GetSide( rkSrcPoly.vPoint[i] );
	}

	D3DXVECTOR3 vInterPt;
	ioPlane::Side eCurSide, eNextSide;

	kOutPoly.Reserve( iPolyVtxCnt );
	kInterPoly.Reserve( iPolyVtxCnt );

	for( i=0 ; i<iPolyVtxCnt ; i++ )
	{
		int iNext = ( i+1 ) % iPolyVtxCnt;

		eCurSide  = m_aOutSidePolygon[i];
		eNextSide = m_aOutSidePolygon[iNext];

		if( eCurSide == ioPlane::POSITIVE_SIDE &&
			eNextSide == ioPlane::POSITIVE_SIDE )
			continue;

		if( eCurSide == ioPlane::POSITIVE_SIDE )	// Outside -> Inside ( Save inter, iNext )
		{
			ioSegment kSegment( rkSrcPoly.vPoint[i], rkSrcPoly.vPoint[iNext] );

			if( ioMath::FindIntersection( kSegment, kPlane, vInterPt ) )
			{
				kOutPoly.AddPoint( vInterPt );
				kInterPoly.AddPoint( vInterPt );
			}

			kOutPoly.AddPoint( rkSrcPoly.vPoint[iNext] );
		}
		else if( eNextSide == ioPlane::POSITIVE_SIDE )	// Inside -> Outside ( Save Inter )
		{
			ioSegment kSegment( rkSrcPoly.vPoint[i], rkSrcPoly.vPoint[iNext] );

			if( ioMath::FindIntersection( kSegment, kPlane, vInterPt ) )
			{
				kOutPoly.AddPoint( vInterPt );
				kInterPoly.AddPoint( vInterPt );
			}
		}
		else	// Both Inside Save iNext
		{
			kOutPoly.AddPoint( rkSrcPoly.vPoint[iNext] );
		}
	}
}

void ioSceneShadowBox::CheckOutSidePolygonArray( int INeedArray )
{
	if( m_iCurOutSideArraySize < INeedArray )
	{
		SAFEDELETEARRAY( m_aOutSidePolygon );
		m_aOutSidePolygon = new ioPlane::Side[INeedArray];
		m_iCurOutSideArraySize = INeedArray;
	}
}

void ioSceneShadowBox::MakeClosingPolygon( ioPolygonMesh &kSrcMesh,
										   ioPolygonMesh &kOutMesh,
										   const D3DXVECTOR3 &rkNrm )
{
	if( kSrcMesh.GetNumPolygon() < 3 )	return;

	const ioPolygon &rkPolyIn = kSrcMesh.GetBack();
	
	ioPolygon kPolyOut;
	kPolyOut.Reserve( kSrcMesh.GetNumPolygon() + 1 );
	kPolyOut.AddPoint( rkPolyIn.vPoint[0] );
	kPolyOut.AddPoint( rkPolyIn.vPoint[1] );

	kSrcMesh.PopBack();

	while( kSrcMesh.GetNumPolygon() > 0 )
	{
		int nr = FindSamePointInMeshAndSwapWithLast( kSrcMesh, kPolyOut.vPoint.back() );
		if( nr >= 0 )
		{
			const ioPolygon &rkPolyIn = kSrcMesh.GetBack();
			kPolyOut.AddPoint( rkPolyIn.vPoint[ (nr+1)%2 ] );
		}

		kSrcMesh.PopBack();
	}

	kPolyOut.Resize( kPolyOut.vPoint.size() - 1 );
	if( kPolyOut.Size() >= 3 )
	{
		ioPlane kPlane( kPolyOut.vPoint[0], kPolyOut.vPoint[1], kPolyOut.vPoint[2] );
		if( !ioMath::IsEqual( kPlane.GetNormal(), rkNrm, 0.01f ) )
		{
			std::reverse( kPolyOut.vPoint.begin(), kPolyOut.vPoint.end() );
		}
	}

	kOutMesh.AddPolygon( kPolyOut );
}

int ioSceneShadowBox::FindSamePointInMeshAndSwapWithLast( ioPolygonMesh &kPolyMesh,
														  const D3DXVECTOR3 &vLastPt )
{
	if( kPolyMesh.GetNumPolygon() < 1 )
		return -1;

	for( int i= kPolyMesh.GetNumPolygon() ; i>0 ; i-- )
	{
		const ioPolygon &rkPoly = kPolyMesh.GetPolygon( i-1 );
		if( rkPoly.Size() == 2 )
		{
			int nr = FindSamePointInPolygon( rkPoly, vLastPt );
			if( 0 <= nr )
			{
				kPolyMesh.SwapPolygonWithLast( i-1 );
				return nr;
			}
		}
	}

	return -1;
}

int ioSceneShadowBox::FindSamePointInPolygon( const ioPolygon &rkPoly, const D3DXVECTOR3 &vLastPt )
{
	int iPointSize = rkPoly.Size();
	for( int i=0 ; i<iPointSize ; i++ )
	{
		if( ioMath::IsEqual( rkPoly.vPoint[i], vLastPt, m_fClosingOffset ) )
			return i;
	}

	return -1;
}

bool ioSceneShadowBox::IsShadowBoxEmpty() const
{
	return m_SceneBox.IsNull();
}
