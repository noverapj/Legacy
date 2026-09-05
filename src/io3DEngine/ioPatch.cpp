

#include "stdafx.h"

#include "ioRay.h"
#include "ioAxisAlignBox.h"
#include "ioMovableObject.h"
#include "ioMath.h"

#include "ioLandScape.h"
#include "ioPatch.h"

#include "ioCPU.h"

ioPatch::ioPatch( ioLandScape *pLand, int nX, int nY )
{
	m_pLandScape = pLand;
	m_iGridX = nX;
	m_iGridY = nY;

	m_pVertex   = NULL;
	m_pNormal	= NULL;
	m_pIndex    = NULL;
	m_pIndexMap = NULL;

	m_iCurVertex = 0;
	m_iCurIndex  = 0;

	m_dwVisibleLayers = VIS_BASE;

	m_iRealSelfLv = m_iNewRealSelfLv = m_iOldRealSelfLv = EMPTY_LEVEL;
	m_iSelfLv = m_iNewSelfLv = EMPTY_LEVEL;
	m_iLeftLv = m_iRightLv = m_iBottomLv = m_iTopLv = EMPTY_LEVEL;

	m_fCurError = -1.0f;

	m_pLeft = m_pRight = m_pBottom = m_pTop = NULL;
	m_pPrev = m_pNext = NULL;
	m_fCameraDistSq = 0.0f;

	m_bVisible			 = false;
	m_bInited			 = false;
	m_bActiveFlag		 = false;
	m_bForceBufferCreate = true;
	m_bNotLOD			 = false;
}

ioPatch::~ioPatch()
{
}

void ioPatch::Init()
{
	m_iNewSelfLv = m_iSelfLv = EMPTY_LEVEL;
	m_iLeftLv = m_iRightLv = m_iBottomLv = m_iTopLv = EMPTY_LEVEL;
	m_iOldRealSelfLv = EMPTY_LEVEL;

	m_pVertex   = new D3DXVECTOR3[ MAX_VERTICES ];
	m_pNormal	= new D3DXVECTOR3[ MAX_VERTICES ];
	m_pIndex    = new WORD[ MAX_INDICES ];
	m_pIndexMap = new WORD[ MAX_VERTICES ];

	m_iCurVertex = m_iCurIndex = 0;

	m_bForceBufferCreate = true;
	m_bInited = true;

	AddToActiveList();
}

void ioPatch::DeInit()
{
	SAFEDELETEARRAY( m_pVertex );
	SAFEDELETEARRAY( m_pNormal );
	SAFEDELETEARRAY( m_pIndex );
	SAFEDELETEARRAY( m_pIndexMap );

	m_fCurError = -1.0f;
	m_bInited = false;

	ReMoveFromActiveList();
}

void ioPatch::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;

	if( m_bVisible )
	{
		m_bActiveFlag = true;
		if( !m_bInited )
		{
			Init();
		}
	}
	else if( m_bInited )
	{
		DeInit();
	}
}

void ioPatch::UpdateCameraDistanceSq( const D3DXVECTOR3 &vCameraPos )
{
	D3DXVECTOR3 vDistance = m_WorldSphere.GetCenter() - vCameraPos;

	m_fCameraDistSq = D3DXVec3LengthSq( &vDistance );
}

void ioPatch::SetNeighbor( const ioPatch *pLeft,
						   const ioPatch *pRight,
						   const ioPatch *pBottom,
						   const ioPatch *pTop )
{
	m_pLeft		= pLeft;
	m_pRight	= pRight;
	m_pBottom	= pBottom;
	m_pTop		= pTop;	
}

void ioPatch::SetHeightMapPos( int nX, int nY )
{
	m_iHMapX = nX;
	m_iHMapY = nY;
}

void ioPatch::CalculateCollisionObject()
{
	D3DXVECTOR3	vPos;

	float minY = 99999999.0f;
	float maxY = 0.0f;

	for( int x=0 ; x<SOURCE_WIDTH ; x++ )
	{
		for( int y=0 ; y<SOURCE_HEIGHT ; y++ )
		{
			GetVertex( x, y, vPos );

			if( vPos.y < minY )
				minY = vPos.y;
			if( vPos.y > maxY )
				maxY = vPos.y;
		}
	}

	D3DXVECTOR3	vMin, vMax;
	GetVertex( 0, 0, vMin );
	vMin.y = minY;
	GetVertex( SOURCE_WIDTH-1, SOURCE_HEIGHT-1, vMax );
	vMax.y = maxY;

	m_WorldAABB.SetMinMaxPos( vMin, vMax );

	float fRadius = D3DXVec3Length( &(vMax - vMin ) ) / 2.0f;
	m_WorldSphere.SetCenter( m_WorldAABB.GetCenter() );
	m_WorldSphere.SetRadius( fRadius );
}

void ioPatch::AddToActiveList()
{
	assert( m_pLandScape );
	assert( m_pPrev == NULL );
	assert( m_pNext == NULL );

	m_pPrev = NULL;
	m_pNext = m_pLandScape->GetActiveListHead();
	if( m_pNext )
	{
		m_pNext->m_pPrev = this;
	}

	m_pLandScape->GetActiveListHead() = this;
}

void ioPatch::ReMoveFromActiveList()
{
	assert( m_pLandScape );

	if( m_pPrev )
	{
		m_pPrev->m_pNext = m_pNext;
	}

	if( m_pNext )
	{
		m_pNext->m_pPrev = m_pPrev;
	}

	if( m_pLandScape->GetActiveListHead() == this )
	{
		assert( m_pPrev == NULL );
		m_pLandScape->GetActiveListHead() = m_pNext;
	}

	m_pPrev = m_pNext = NULL;
}

void ioPatch::UpdateProjectionError( const D3DXMATRIX *pMat )
{
	D3DXVECTOR3	vCrt1, vReal, vDiff;

	D3DXVec3TransformCoord( &vCrt1, &m_Error[1].vCorrect, pMat );

	if( vCrt1.z < 0.0f )
		vCrt1.z = 0.0f;

	for( int i=1 ; i<=MAX_SUBDIV ; i++ )
	{
		D3DXVec3TransformCoord( &vReal, &m_Error[i].vReal, pMat );
		if( vReal.z < 0.0f )
			vReal.z = 0.0f;

		vDiff = vCrt1 - vReal;
		m_Error[i].fDiff = D3DXVec3Length( &vDiff );

		if( m_Error[i].fDiff < m_Error[i-1].fDiff * 1.1f )
		{
			m_Error[i].fDiff = m_Error[i-1].fDiff * 1.1f;
		}
	}
}

void ioPatch::SetRealLevel( int iLevel )
{
	m_iOldRealSelfLv = m_iRealSelfLv;
	m_iNewRealSelfLv = iLevel;
}

int ioPatch::GetNewLevelSafe() const
{
	if( NULL == this )
		return MAX_SUBDIV;

	if( !IsVisible() )
		return  m_iSelfLv; // MAX_SUBDIV;

	return GetNewLevel();
}

float ioPatch::GetExactLevel() const
{
	if( m_iRealSelfLv < 0 )
		return -1.0f;

	if( m_fCurError <= 0.0f )
		return (float)m_iRealSelfLv;
	else
		return (float)m_iRealSelfLv + m_fCurError;
}

bool ioPatch::UpdateLevel()
{
	bool bCreate = false;

	int iLLv = m_pLeft->GetNewLevelSafe();
	int iRLv = m_pRight->GetNewLevelSafe();
	int iBLv = m_pBottom->GetNewLevelSafe();
	int iTLv = m_pTop->GetNewLevelSafe();

	if( m_bForceBufferCreate ||
		m_iRealSelfLv != m_iNewRealSelfLv ||
		m_iSelfLv != m_iNewSelfLv ||
		m_iLeftLv != iLLv ||
		m_iRightLv != iRLv ||
		m_iBottomLv != iBLv ||
		m_iTopLv != iTLv )
	{
		m_iRealSelfLv	= m_iNewRealSelfLv;
		m_iSelfLv		= m_iNewSelfLv;
		m_iLeftLv		= iLLv;
		m_iRightLv		= iRLv;
		m_iBottomLv		= iBLv;
		m_iTopLv		= iTLv;

		assert( m_iSelfLv - iLLv <= 1 );
		assert( m_iSelfLv - iRLv <= 1 );
		assert( m_iSelfLv - iBLv <= 1 );
		assert( m_iSelfLv - iTLv <= 1 );

		CreateLevel( m_iSelfLv, m_iLeftLv, m_iRightLv, m_iBottomLv, m_iTopLv );

		m_bForceBufferCreate = false;
		bCreate = true;
	}

	if( !m_bNotLOD )
	{
		float fError0 = m_Error[ m_iRealSelfLv ].fDiff;
		float fError1 = m_Error[ m_iRealSelfLv + 1 ].fDiff;
		float fMaxErr = m_pLandScape->GetMaxError();

		if( m_iRealSelfLv >= MAX_SUBDIV )
		{
			fError1 = 2.0f * fError0;
		}

		m_fCurError = ( fMaxErr - fError0 ) / ( fError1 - fError0 );
		
		if( m_iRealSelfLv>=MAX_SUBDIV && m_fCurError > 1.0f )
			m_fCurError = 1.0f;
	}
	else
	{
		m_fCurError = 0.0f;
	}

//	assert( m_fCurError <= 1.0f );	

	return bCreate;
}

bool ioPatch::UpdateLevelSecond()
{
	return false;
}

bool ioPatch::UpdateLevelThird()
{
	return false;
}

int ioPatch::FillVertices( D3DXVECTOR3 *pBuf )
{
	ioCPU::FastMemcpy( pBuf, m_pVertex, m_iCurVertex * sizeof(D3DXVECTOR3) );

	return m_iCurVertex;
}

void ioPatch::CreateLevel( int iCenterLv,
						   int iLeftLv,
						   int iRightLv,
						   int iBottomLv,
						   int iTopLv )
{
	assert( m_pVertex );
	assert( m_pIndex );
	assert( m_pIndexMap );

	memset( m_pIndexMap, 0xff, MAX_VERTICES * sizeof(WORD) );
	m_iCurVertex = m_iCurIndex = 0;

	int pow = GetPowerTwo( iCenterLv );

	assert( iCenterLv - iLeftLv <= 1 );
	assert( iCenterLv - iRightLv <= 1 );
	assert( iCenterLv - iBottomLv <= 1 );
	assert( iCenterLv - iTopLv <= 1 );	

	bool bLeft( iLeftLv < iCenterLv );
	bool bRight( iRightLv < iCenterLv );
	bool bBottom( iBottomLv < iCenterLv );
	bool bTop( iTopLv < iCenterLv );	

	if( pow == 16 )
	{
		AddLevel4PatchTriangles( bLeft, bRight, bBottom, bTop );
	}
	else
	{
		for( int y=0 ; y < SOURCE_HEIGHT - pow ; y+=pow )
		{
			AddTriangleRow( y, pow, bLeft, bRight, bBottom, bTop );
		}
	}

	assert( (m_iCurIndex & 1 ) == 0 );	// Indices must alwayw be an even number

	if( m_iRealSelfLv > m_iSelfLv )
	{
		ReduceShapeTo( m_iRealSelfLv, m_pVertex );
	}

	MakeBordersSimpler( m_pVertex );
}

void ioPatch::AddLevel4PatchTriangles( bool bLeft, bool bRight, bool bBottom, bool bTop )
{
	const int powH = 8, pow = 16;
	int code = ( bLeft ? 1 : 0 ) | ( bRight ? 2 : 0 ) | ( bBottom ? 4 : 0 ) | (bTop ? 8 : 0 );

	switch( code )
	{
	case 0:
		AddIndex( GetIndex( 0, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( pow, pow ) );
		break;
	case 1:
		AddIndex( GetIndex( 0,0 ) );
		AddIndex( GetIndex( 0,powH ) );
		AddIndex( GetIndex( pow,0 ) );
		AddIndex( GetIndex( 0,pow ) );
		AddIndex( GetIndex( pow,pow ) );
		AddLastIndexAgain();
		break;
	case 2:
		AddIndex( GetIndex( pow, pow ) );
		AddIndex( GetIndex( pow, powH ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( 0, 0 ) );
		AddLastIndexAgain();
		break;
	case 3:
		AddIndex( GetIndex( 0, 0 ) );
		AddIndex( GetIndex( 0, powH ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, powH ) );
		AddIndex( GetIndex( pow, pow ) );
		break;
	case 4:
		AddIndex( GetIndex( 0, 0 ) );
		AddLastIndexAgain();
		AddIndex( GetIndex( powH, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( pow, pow ) );
		break;
	case 5:
		AddIndex( GetIndex( 0, 0 ) );
		AddIndex( GetIndex( 0, powH ) );
		AddIndex( GetIndex( powH, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( pow, pow ) );
		break;
	case 6:
		AddIndex( GetIndex( 0, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( powH, 0 ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddLastIndexAgain();
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, powH ) );
		AddIndex( GetIndex( pow, pow ) );
		break;
	case 7:
		AddIndex( GetIndex( pow, pow ) );
		AddIndex( GetIndex( pow, powH ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( 0, powH ) );
		AddIndex( GetIndex( powH, 0 ) );
		AddIndex( GetIndex( 0, 0 ) );
		AddLastIndexAgain();
		break;
	case 8:
		AddIndex( GetIndex( pow, pow ) );
		AddLastIndexAgain();
		AddIndex( GetIndex( powH, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( 0, 0 ) );
		break;
	case 9:
		AddIndex( GetIndex( pow, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( powH, pow ) );
		AddIndex( GetIndex( 0, pow ) );
		AddLastIndexAgain();
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( 0, powH ) );
		AddIndex( GetIndex( 0, 0 ) );
		break;
	case 10:
		AddIndex( GetIndex( pow, pow ) );
		AddIndex( GetIndex( pow, powH ) );
		AddIndex( GetIndex( powH, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( 0, 0 ) );
		break;
	case 11:
		AddIndex( GetIndex( 0, 0 ) );
		AddIndex( GetIndex( 0, powH ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, powH ) );
		AddIndex( GetIndex( powH, pow ) );
		AddIndex( GetIndex( pow, pow ) );
		AddLastIndexAgain();
		break;
	case 12:
		AddIndex( GetIndex( 0, 0 ) );
		AddLastIndexAgain();
		AddIndex( GetIndex( powH, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( powH, pow ) );
		AddIndex( GetIndex( pow, pow ) );
		AddLastIndexAgain();
		break;
	case 13:
		AddIndex( GetIndex( 0, 0 ) );
		AddIndex( GetIndex( 0, powH ) );
		AddIndex( GetIndex( powH, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( powH, pow ) );
		AddIndex( GetIndex( pow, pow ) );
		AddLastIndexAgain();
		break;
	case 14:
		AddIndex( GetIndex( pow, pow ) );
		AddIndex( GetIndex( pow, powH ) );
		AddIndex( GetIndex( powH, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( powH, 0 ) );
		AddIndex( GetIndex( 0, 0 ) );
		AddLastIndexAgain();
		break;
	case 15:
		AddIndex( GetIndex( 0, 0 ) );
		AddIndex( GetIndex( 0, powH ) );
		AddIndex( GetIndex( powH, 0 ) );
		AddIndex( GetIndex( 0, pow ) );
		AddIndex( GetIndex( pow, 0 ) );
		AddIndex( GetIndex( powH, pow ) );
		AddIndex( GetIndex( pow, powH ) );
		AddIndex( GetIndex( pow, pow ) );
		break;
	}

	// indices must always be an even number (we store quads as two triangles)
	assert((m_iCurIndex&1) == 0);
}

void ioPatch::AddTriangleRow( int nY, int nPow,
							  bool bLeft, bool bRight, bool bBottom, bool bTop )
{
	int powH = nPow/2;

	// special case
	if( nY == 0 && bBottom )
	{
		AddTriangleBottomRow( nPow, bLeft, bRight );
		return;
	}

	if( nY != 0 )
	{
		AddLastIndexAgain();
		AddIndex( GetIndex( 0, nY ) );
	}

	// special case
	if( nY >= SOURCE_HEIGHT - 2*nPow && bTop )
	{
		AddTriangleTopRow( nPow, bLeft, bRight );
		return;
	}

	// all other case

	int x, x0 = 0, x1 = SOURCE_WIDTH;

	if( bLeft )
	{
		assert( powH >= 1 );

		AddIndex( GetIndex( 0, nY ) );
		AddIndex( GetIndex( 0, nY+powH ) );
		AddIndex( GetIndex( nPow, nY ) );
		AddIndex( GetIndex( 0, nY+nPow ) );
		AddIndex( GetIndex( nPow, nY+nPow ) );
		AddLastIndexAgain();	// finalize sub-strip
		x0 = nPow;
	}

	if( bRight )
		x1 -= nPow;
	
	for( x=x0 ; x<x1 ; x+= nPow )
	{
		AddIndex( GetIndex( x, nY ) );
		AddIndex( GetIndex( x, nY + nPow ) );
	}

	if( bRight )
	{
		assert( powH >= 1 );

		AddIndex( GetIndex( SOURCE_WIDTH - 1 - nPow, nY ) );
		AddLastIndexAgain();
		AddIndex( GetIndex( SOURCE_WIDTH - 1, nY ) );
		AddIndex( GetIndex( SOURCE_WIDTH - 1-nPow, nY+nPow ) );
		AddIndex( GetIndex( SOURCE_WIDTH - 1, nY + powH ) );
		AddIndex( GetIndex( SOURCE_WIDTH - 1, nY + nPow ) );
	}
}

void ioPatch::AddTriangleTopRow( int nPow, bool bLeft, bool bRight )
{
	int powH = nPow/2;
	int x0=0, x1=SOURCE_WIDTH-1;

	assert( powH >= 1 );

	if( bLeft )
	{
		AddIndex( GetIndex( 0, SOURCE_HEIGHT-1-nPow ) );
		AddIndex( GetIndex( 0, SOURCE_HEIGHT-1-powH ) );
		AddIndex( GetIndex( nPow, SOURCE_HEIGHT-1-nPow ) );
		AddIndex( GetIndex( 0, SOURCE_HEIGHT-1 ) );
		AddIndex( GetIndex( powH, SOURCE_HEIGHT-1 ) );
		AddLastIndexAgain();						// reverse oriented triangles
		AddIndex( GetIndex( nPow, SOURCE_HEIGHT-1-nPow ) );
		AddIndex( GetIndex( nPow, SOURCE_HEIGHT-1 ) );
		x0 = nPow;
	}

	assert( (m_iCurIndex&1) == 0 );

	if( bRight )
	{
		x1 -= nPow;
	}

	for( int x=x0 ; x<x1 ; x+=nPow )
	{
		AddIndex( GetIndex( x, SOURCE_HEIGHT-1-nPow ) );
		AddIndex( GetIndex( x, SOURCE_HEIGHT-1 ) );
		AddIndex( GetIndex( x+nPow, SOURCE_HEIGHT-1-nPow ) );
		AddIndex( GetIndex( x+powH, SOURCE_HEIGHT-1 ) );
		AddIndex( GetIndex( x+nPow, SOURCE_HEIGHT-1 ) );
		AddLastIndexAgain();		// reverse oriented triangles
	}

	if(bRight)
	{
		AddIndex( GetIndex( x, SOURCE_HEIGHT-1-nPow ) );
		AddIndex( GetIndex( x, SOURCE_HEIGHT-1 ) );
		AddIndex( GetIndex( x+nPow, SOURCE_HEIGHT-1-nPow ) );
		AddIndex( GetIndex( x+powH, SOURCE_HEIGHT-1 ) );
		AddIndex( GetIndex( x+nPow, SOURCE_HEIGHT-1-powH ) );
		AddIndex( GetIndex( x+nPow, SOURCE_HEIGHT-1 ) );
	}
}

void ioPatch::AddTriangleBottomRow( int nPow, bool bLeft, bool bRight )
{
	int powH = nPow/2;
	int x0=0, x1=SOURCE_WIDTH-1;

	assert( powH >= 1 );

	if( bLeft )
	{
		AddIndex( GetIndex( 0, 0 ) );
		AddIndex( GetIndex( 0, powH ) );
		AddIndex( GetIndex( powH, 0 ) );
		AddIndex( GetIndex( 0, nPow ) );
		AddIndex( GetIndex( nPow, 0 ) );
		AddIndex( GetIndex( nPow, nPow ) );
		x0 = nPow;
	}

	assert( (m_iCurIndex&1) == 0 );

	if( bRight )
	{
		x1 -= nPow;
	}

	for( int x=x0 ; x<x1 ; x+= nPow )
	{
		AddIndex( GetIndex( x, 0 ) );
		AddLastIndexAgain();						// reverse oriented triangles
		AddIndex( GetIndex( x+powH, 0 ) );
		AddIndex( GetIndex( x, nPow ) );
		AddIndex( GetIndex( x+nPow, 0 ) );
		AddIndex( GetIndex( x+nPow, nPow ) );
	}

	assert( (m_iCurIndex&1) == 0 );

	if( bRight )
	{
		AddIndex( GetIndex( SOURCE_WIDTH-1-nPow, 0 ) );
		AddIndex( GetIndex( SOURCE_WIDTH-1-nPow, nPow ) );
		AddIndex( GetIndex( SOURCE_WIDTH-1-powH, 0 ) );
		AddIndex( GetIndex( SOURCE_WIDTH-1, 0 ) );
		AddLastIndexAgain();							// finalize sub-strip
		AddIndex( GetIndex( SOURCE_WIDTH-1-nPow, nPow ) );
		AddIndex( GetIndex( SOURCE_WIDTH-1, powH ) );
		AddIndex( GetIndex( SOURCE_WIDTH-1, nPow ) );
	}

	assert( (m_iCurIndex&1) == 0 );
}

void ioPatch::ReduceShapeTo( int iLevel, D3DXVECTOR3 *pVtx )
{
	for( int i=iLevel ; i>0 ; i-- )
	{
		if( m_iSelfLv < i )
		{
			MakeSimpler( i, pVtx );
		}
	}
}

void ioPatch::MakeBordersSimpler( D3DXVECTOR3 *pVtx )
{
	int x, y, pow;
	int iIdx0, iIdx1, iIdx2;
	int iLDiff = m_iSelfLv - m_iLeftLv;
	int iRDiff = m_iSelfLv - m_iRightLv;
	int iBDiff = m_iSelfLv - m_iBottomLv;
	int iTDiff = m_iSelfLv - m_iTopLv;

	if( (iLDiff == 1 || iLDiff == -1) && m_pLeft )
	{
		pow = GetPowerTwo( min(m_iLeftLv, m_iSelfLv ) );

		for( y=pow ; y<SOURCE_HEIGHT-1 ; y+= 2*pow )
		{
			iIdx0 = m_pIndexMap[ y*SOURCE_WIDTH ];
			iIdx1 = m_pIndexMap[ (y-pow)*SOURCE_WIDTH ];
			iIdx2 = m_pIndexMap[ (y+pow)*SOURCE_WIDTH ];

			assert( iIdx0 != 0xffff );
			assert( iIdx1 != 0xffff );
			assert( iIdx2 != 0xffff );

			pVtx[iIdx0].y = ( pVtx[iIdx1].y + pVtx[iIdx2].y ) / 2.0f;
		}
	}

	if( (iRDiff == 1 || iRDiff == -1) && m_pRight )
	{
		pow = GetPowerTwo( min(m_iRightLv, m_iSelfLv ) );

		for( y=pow ; y<SOURCE_HEIGHT-1 ; y+= 2*pow )
		{
			iIdx0 = m_pIndexMap[SOURCE_WIDTH-1 + y*SOURCE_WIDTH];
			iIdx1 = m_pIndexMap[SOURCE_WIDTH-1 + (y-pow)*SOURCE_WIDTH];
			iIdx2 = m_pIndexMap[SOURCE_WIDTH-1 + (y+pow)*SOURCE_WIDTH];

			assert( iIdx0 != 0xffff );
			assert( iIdx1 != 0xffff );
			assert( iIdx2 != 0xffff );

			pVtx[iIdx0].y = ( pVtx[iIdx1].y + pVtx[iIdx2].y ) / 2.0f;
		}
	}

	if( (iBDiff == 1 || iBDiff == -1) && m_pBottom )
	{
		pow = GetPowerTwo( min( m_iBottomLv, m_iSelfLv ) );

		for( x=pow; x<SOURCE_WIDTH-1 ; x+= 2*pow )
		{
			iIdx0 = m_pIndexMap[x];
			iIdx1 = m_pIndexMap[x-pow];
			iIdx2 = m_pIndexMap[x+pow];

			assert( iIdx0 != 0xffff );
			assert( iIdx1 != 0xffff );
			assert( iIdx2 != 0xffff );

			pVtx[iIdx0].y = ( pVtx[iIdx1].y + pVtx[iIdx2].y ) / 2.0f;
		}
	}

	if( (iTDiff == 1 || iTDiff == -1) && m_pTop )
	{
		pow = GetPowerTwo( min( m_iTopLv, m_iSelfLv ) );

		for( x=pow; x<SOURCE_WIDTH-1 ; x+= 2*pow )
		{
			iIdx0 = m_pIndexMap[ x + (SOURCE_HEIGHT-1)*SOURCE_WIDTH ];
			iIdx1 = m_pIndexMap[ x-pow + (SOURCE_HEIGHT-1)*SOURCE_WIDTH ];
			iIdx2 = m_pIndexMap[ x+pow + (SOURCE_HEIGHT-1)*SOURCE_WIDTH ];

			assert( iIdx0 != 0xffff );
			assert( iIdx1 != 0xffff );
			assert( iIdx2 != 0xffff );

			pVtx[iIdx0].y = ( pVtx[iIdx1].y + pVtx[iIdx2].y ) / 2.0f;
		}
	}
}

void ioPatch::MakeSimpler( int iLevel, D3DXVECTOR3 *pVtx )
{
	int pow = GetPowerTwo( iLevel - 1 );
	int x, y, xc, yc;

	for( yc=y=0 ; y<SOURCE_HEIGHT ; y+=pow, yc++ )
	{
		for( xc=x=0 ; x<SOURCE_WIDTH ; x+=pow, xc++ )
		{
			CalculateInbetweenVertex( x, y, xc, yc, pow, pVtx );
		}
	}
}

void ioPatch::CalculateInbetweenVertex( int x, int y, int xc, int yc, int pow, D3DXVECTOR3 *pVtx )
{
	float fHeightLeftTop=0.0f, fHeightRightBottom=0.0f;
	int iIdx, iIdxLT, iIdxRB;

	iIdx = CalculateDiagonalVertices( x, y, xc, yc, pow, pVtx,
									  fHeightLeftTop, iIdxLT, fHeightRightBottom, iIdxRB );

	if( iIdx >= 0 )
	{
		pVtx[ iIdx ].y = ( fHeightLeftTop + fHeightRightBottom ) / 2.0f;
	}
}

int ioPatch::CalculateDiagonalVertices( int x, int y, int xc, int yc,
									    int pow,
										D3DXVECTOR3 *pVtx,
									    float &fHeightLeftTop, int &idxLT,
									    float &fHeightRightBottom, int &idxRB )
{
	int code = ( xc&1 ) + ( yc&1 ) * 2;
	int iIdx = m_pIndexMap[ x + y*SOURCE_WIDTH ];

	assert( iIdx != 0xffff );

	switch( code )
	{
	case 0:			// even-x & even-y
		// nothing to do
		return -1;
	case 1:			// odd-x & even-y
		assert( x>0 && x<SOURCE_WIDTH-1 );

		idxLT = m_pIndexMap[ x-pow + y*SOURCE_WIDTH ];
		idxRB = m_pIndexMap[ x+pow + y*SOURCE_WIDTH ];

		assert( idxLT != 0xffff );
		assert( idxRB != 0xffff );

		fHeightLeftTop = pVtx[idxLT].y;
		fHeightRightBottom = pVtx[idxRB].y;
		return iIdx;
	case 2:			// even-x & odd-y
		assert( y>0 && y<SOURCE_HEIGHT-1 );

		idxLT = m_pIndexMap[x + (y+pow)*SOURCE_WIDTH];
		idxRB = m_pIndexMap[x + (y-pow)*SOURCE_WIDTH];

		assert(idxLT!=0xffff);
		assert(idxRB!=0xffff);

		fHeightLeftTop = pVtx[idxLT].y;
		fHeightRightBottom = pVtx[idxRB].y;
		return iIdx;
	case 3:			// odd-x & odd-y
		assert( x>0 && x<SOURCE_WIDTH-1 );
		assert( y>0 && y<SOURCE_HEIGHT-1 );

		idxLT = m_pIndexMap[x-pow + (y+pow)*SOURCE_WIDTH];
		idxRB = m_pIndexMap[x+pow + (y-pow)*SOURCE_WIDTH];

		assert(idxLT!=0xffff);
		assert(idxRB!=0xffff);

		fHeightLeftTop = pVtx[idxLT].y;
		fHeightRightBottom = pVtx[idxRB].y;
		return iIdx;
	}

	assert( false );
	return -1;
}

inline void ioPatch::AddIndex( WORD wIdx )
{
	m_pIndex[ m_iCurIndex++ ] = wIdx;
}

inline void ioPatch::AddLastIndexAgain()
{
	if( m_iCurIndex > 0 )
	{
		m_pIndex[ m_iCurIndex++ ] = m_pIndex[ m_iCurIndex - 1 ];
	}
}

inline WORD ioPatch::GetIndex( int nX, int nY )
{
	int idx = nX + nY * SOURCE_WIDTH;

	assert( nX>=0 && nX<SOURCE_WIDTH );
	assert( nY>=0 && nY<SOURCE_HEIGHT );
	assert( idx<MAX_VERTICES );

	if( m_pIndexMap[idx] == 0xffff )	// empty
	{
		m_pIndexMap[idx] = (WORD)m_iCurVertex;

		GetVertex( nX,nY, m_pVertex[m_iCurVertex] );
		GetNormal( nX,nY, m_pNormal[m_iCurVertex] );

		m_iCurVertex++;
	}

	return m_pIndexMap[idx];
}

inline float ioPatch::GetHeight( int nX, int nY ) const
{
	assert( nX>=0 && nX<SOURCE_WIDTH );
	assert( nY>=0 && nY<SOURCE_HEIGHT );
	assert( m_pLandScape );

	return m_vPosition.y + m_pLandScape->GetHeight( m_iHMapX+nX, m_iHMapY+nY );
}

inline void ioPatch::GetVertex( int nX, int nY, D3DXVECTOR3& vPos ) const
{
	assert( m_pLandScape );
	assert( nX>=0 && nX<SOURCE_WIDTH );
	assert( nY>=0 && nY<SOURCE_HEIGHT );

	vPos.x = m_vPosition.x + m_vScale.x * nX / (float)(SOURCE_WIDTH-1);
	vPos.y = GetHeight( nX,nY );
	vPos.z = m_vPosition.z + m_vScale.z * nY / (float)(SOURCE_HEIGHT-1);
}

inline void ioPatch::GetNormal( int nX, int nY, D3DXVECTOR3 &vNrm ) const
{
	assert( m_pLandScape );
	assert(nX>=0 && nX<SOURCE_WIDTH);
	assert(nY>=0 && nY<SOURCE_HEIGHT);

	vNrm = m_pLandScape->GetNormal( m_iHMapX + nX, m_iHMapY + nY );
}

void ioPatch::CalErrors()
{
	for( int i=0 ;i<=MAX_SUBDIV ; i++ )
	{
		CalError(i);
	}
}

void ioPatch::CalError( int iLevel )
{
	float	fSumError = 0.0f;
	int		iNumError = 0;

	if( iLevel == 0 )	return;

	int pow = GetPowerTwo( iLevel );
	int x0, y0, xi, yi;

	float fX0, fY0, fX1, fY1;
	float height00, height10, height01, height11;
	float fPaintHeight, fCorrectHeight;
	float fError;

	for( y0=0 ; y0<SOURCE_HEIGHT-pow ; y0 += pow )
	{
		for( x0=0 ; x0<SOURCE_WIDTH-pow ; x0 += pow )
		{
			for( yi=1 ; yi<pow ; yi++ )
			{
				for( xi=1 ; xi<pow ; xi++ )
				{
					fX0 = (float)xi/(float)pow;
					fX1 = 1.0f - fX0;

					fY0 = (float)yi/(float)pow;
					fY1 = 1.0f - fY0;

					height00 = GetHeight( x0,y0 ),
					height10 = GetHeight( x0+pow,y0 ),
					height01 = GetHeight( x0,y0+pow ),
					height11 = GetHeight( x0+pow,y0+pow );

					fPaintHeight = fX1 * fY1 * height00 +
								   fX0 * fY1 * height10 +
								   fX1 * fY0 * height01 +
								   fX0 * fY0 * height11;

					fCorrectHeight =  GetHeight( x0 + xi, y0 + yi );

					fSumError += (float)fabs(fPaintHeight - fCorrectHeight);
					iNumError++;
				}
			}
		}

		fError = fSumError / iNumError;
		GetVertex(SOURCE_WIDTH/2, SOURCE_HEIGHT/2, m_Error[ iLevel ].vCorrect );
		m_Error[ iLevel ].vReal = m_Error[ iLevel ].vCorrect;
		m_Error[ iLevel ].vReal.y += fError;
		m_Error[ iLevel ].fDiff = fError;
	}
}

int ioPatch::FillNormalVertices( D3DXVECTOR3 *pBuf )
{
	ioCPU::FastMemcpy( pBuf, m_pNormal, m_iCurVertex * sizeof(D3DXVECTOR3) );

	return m_iCurVertex;
}
