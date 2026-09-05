

#include "stdafx.h"

#include "ioPolygonMesh.h"

ioPolygonMesh::ioPolygonMesh()
{
}

ioPolygonMesh::ioPolygonMesh( const ioPolygonMesh &kPolyMesh )
 : m_vPolygonList( kPolyMesh.m_vPolygonList )
{
}

ioPolygonMesh::~ioPolygonMesh()
{
}

void ioPolygonMesh::SwapPolygonWithLast( int iPoly )
{
	int iNumPoly = GetNumPolygon();
	if( iPoly == iNumPoly-1 )	return;

	if( !COMPARE( iPoly, 0, iNumPoly ) )
	{
		LOG.PrintTimeAndLog( 0, "ioPolygonMesh::SwapPolygonWithLast - over flow(%d)", iPoly );
		return;
	}

	PolygonList::iterator iter = m_vPolygonList.begin();
	std::advance( iter, iPoly );

	PolygonList::iterator iLast = m_vPolygonList.end();
	--iLast;

	ioPolygon kTempPoly = *iter;
	*iter = *iLast;
	*iLast = kTempPoly;
}

const ioPolygon& ioPolygonMesh::GetPolygon( int iIndex ) const
{
	PolygonList::const_iterator iter = m_vPolygonList.begin();
	std::advance( iter, iIndex );

	return *iter;
}

void ioPolygonMesh::GetAllVtxList( Vector3Vec &vVtxList )
{
	int iTotal = 0;

	PolygonList::iterator iter;
	for( iter=m_vPolygonList.begin() ; iter!=m_vPolygonList.end() ; ++iter )
	{
		iTotal += iter->Size();
	}

	vVtxList.erase( vVtxList.begin(), vVtxList.end() );
	vVtxList.reserve( iTotal );

	for( iter=m_vPolygonList.begin() ; iter!=m_vPolygonList.end() ; ++iter )
	{
		vVtxList.insert( vVtxList.end(), iter->vPoint.begin(), iter->vPoint.end() );
	}
}
