

#include "stdafx.h"

#include "ioAxisAlignBox.h"
#include "ioMovableObject.h"

#include "ioPatch.h"
#include "ioPatchDefault.h"

ioPatchDefault::ioPatchDefault( ioLandScape *pLand, int nX, int nY ) : ioPatch( pLand, nX, nY )
{	
}

ioPatchDefault::~ioPatchDefault()
{
}

bool ioPatchDefault::UpdateLevel()
{
	if( ioPatch::UpdateLevel() )
	{
		m_bForceBufferCreate = true;
	}

	return true;
}

bool ioPatchDefault::UpdateLevelSecond()
{
	ioPatchDefault *pLeft   = (ioPatchDefault*)this->m_pLeft;
	ioPatchDefault *pRight  = (ioPatchDefault*)this->m_pRight;
	ioPatchDefault *pBottom = (ioPatchDefault*)this->m_pBottom;
	ioPatchDefault *pTop	= (ioPatchDefault*)this->m_pTop;

	if( m_bForceBufferCreate ||
		( pLeft && pLeft->m_bForceBufferCreate ) ||
		( pRight && pRight->m_bForceBufferCreate ) ||
		( pBottom && pBottom->m_bForceBufferCreate ) ||
		( pTop && pTop->m_bForceBufferCreate ) )
	{
		CheckBorders();
	}

	return true;
}

bool ioPatchDefault::UpdateLevelThird()
{
	m_bForceBufferCreate = false;

	return true;
}

bool ioPatchDefault::CheckBorders()
{
	ioPatchDefault *pLeft   = (ioPatchDefault*)this->m_pLeft;
	ioPatchDefault *pRight  = (ioPatchDefault*)this->m_pRight;
	ioPatchDefault *pBottom = (ioPatchDefault*)this->m_pBottom;
	ioPatchDefault *pTop	= (ioPatchDefault*)this->m_pTop;

	bool bUpdated = false;

	int iIdx, iIdxL, iIdxR, iIdxB, iIdxT;
	int iPow;

	if( pLeft && pLeft->IsInited() &&
		( pLeft->m_iSelfLv != pLeft->m_iRealSelfLv || pLeft->m_iRealSelfLv != pLeft->m_iOldRealSelfLv ) )
	{
		iPow = GetPowerTwo( min( m_iLeftLv, m_iSelfLv ) );

		for( int y=iPow ; y<SOURCE_HEIGHT-1 ; y+=iPow )
		{
			iIdx = m_pIndexMap[ y*SOURCE_WIDTH ];
			iIdxL = pLeft->m_pIndexMap[ SOURCE_WIDTH-1 + y*SOURCE_WIDTH ];

			assert( iIdx != 0xffff );
			assert( iIdxL != 0xffff );

			m_pVertex[iIdx].y = pLeft->m_pVertex[iIdxL].y;
		}

		bUpdated = true;
	}

	if( pRight && pRight->IsInited() && 
		( pRight->m_iSelfLv != pRight->m_iRealSelfLv || pRight->m_iRealSelfLv != pRight->m_iOldRealSelfLv ) )
	{
		iPow = GetPowerTwo( min( m_iRightLv, m_iSelfLv ) );

		for( int y=iPow; y<SOURCE_HEIGHT-1 ; y+=iPow )
		{
			iIdx = m_pIndexMap[SOURCE_WIDTH-1 + y*SOURCE_WIDTH];
			iIdxR = pRight->m_pIndexMap[ y*SOURCE_WIDTH ];

			assert( iIdx != 0xffff );
			assert( iIdxR != 0xffff );

			m_pVertex[iIdx].y = pRight->m_pVertex[iIdxR].y;
		}

		bUpdated = true;
	}

	if( pBottom && pBottom->IsInited() &&
		( pBottom->m_iSelfLv != pBottom->m_iRealSelfLv || pBottom->m_iRealSelfLv != pBottom->m_iOldRealSelfLv ) )
	{
		iPow = GetPowerTwo( min( m_iBottomLv , m_iSelfLv ) );

		for( int x = iPow ; x<SOURCE_WIDTH-1 ; x+= iPow )
		{
			iIdx = m_pIndexMap[ x ];
			iIdxB = pBottom->m_pIndexMap[ x + (SOURCE_HEIGHT-1)*SOURCE_WIDTH ];

			assert( iIdx != 0xffff );
			assert( iIdxB != 0xffff );

			m_pVertex[iIdx].y = pBottom->m_pVertex[iIdxB].y;
		}

		bUpdated = true;
	}

	if( pTop && pTop->IsInited() &&
		(pTop->m_iSelfLv != pTop->m_iRealSelfLv || pTop->m_iRealSelfLv != pTop->m_iOldRealSelfLv ) )
	{
		iPow = GetPowerTwo( min( m_iTopLv, m_iSelfLv ) );

		for( int x=iPow ; x<SOURCE_HEIGHT-1 ; x+=iPow )
		{
			iIdx = m_pIndexMap[ x + (SOURCE_HEIGHT-1)*SOURCE_WIDTH ];
			iIdxT = pTop->m_pIndexMap[ x ];

			assert( iIdx != 0xffff );
			assert( iIdxT != 0xffff );

			m_pVertex[iIdx].y = pTop->m_pVertex[iIdxT].y;
		}

		bUpdated = true;
	}

	return bUpdated;
}
