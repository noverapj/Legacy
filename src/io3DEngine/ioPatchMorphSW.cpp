
#include "stdafx.h"

#include "ioAxisAlignBox.h"
#include "ioMovableObject.h"

#include "ioPatch.h"
#include "ioLandScape.h"
#include "ioPatchMorphSW.h"

#include "ioCPU.h"

ioPatchMorphSW::ioPatchMorphSW( ioLandScape *pLand, int nX, int nY ) : ioPatch( pLand, nX, nY )
{
	m_pVertexLow = NULL;

	m_pYMoveSelf = NULL;
	m_pYMoveLeft   = m_pYMoveLeft2 = NULL;
	m_pYMoveRight  = m_pYMoveRight2 = NULL;
	m_pYMoveBottom = m_pYMoveBottom2 = NULL;
	m_pYMoveTop    = m_pYMoveTop2 = NULL;

	m_bNewFollowsLeft   = m_bFollowsLeft   = false;
	m_bNewFollowsRight  = m_bFollowsRight  = false;
	m_bNewFollowsBottom = m_bFollowsBottom = false;
	m_bNewFollowsTop	= m_bFollowsTop	   = false;

	m_bRecalcBorderLeft	   = false;
	m_bRecalcBorderRight   = false;
	m_bRecalcBorderBottom  = false;
	m_bRecalcBorderTop     = false;
	
	m_bForceReTessellation = false;
}

ioPatchMorphSW::~ioPatchMorphSW()
{
}

void ioPatchMorphSW::Init()
{
	ioPatch::Init();

	m_pVertexLow   = new D3DXVECTOR3[MAX_VERTICES];
	m_pYMoveSelf   = new float[MAX_VERTICES];
	
	m_pYMoveLeft   = new float[MAX_VERTICES];
	m_pYMoveLeft2  = new float[MAX_VERTICES];

	m_pYMoveRight  = new float[MAX_VERTICES];
	m_pYMoveRight2 = new float[MAX_VERTICES];

	m_pYMoveTop    = new float[MAX_VERTICES];
	m_pYMoveTop2   = new float[MAX_VERTICES];

	m_pYMoveBottom = new float[MAX_VERTICES];
	m_pYMoveBottom2 = new float[MAX_VERTICES];

	m_bNewFollowsLeft   = m_bFollowsLeft   = false;
	m_bNewFollowsRight  = m_bFollowsRight  = false;
	m_bNewFollowsBottom = m_bFollowsBottom = false;
	m_bNewFollowsTop	= m_bFollowsTop	   = false;

	m_bRecalcBorderLeft	   = false;
	m_bRecalcBorderRight   = false;
	m_bRecalcBorderBottom  = false;
	m_bRecalcBorderTop     = false;

	m_bForceReTessellation = false;
}

void ioPatchMorphSW::DeInit()
{
	ioPatch::DeInit();

	SAFEDELETEARRAY( m_pVertexLow );
	SAFEDELETEARRAY( m_pYMoveSelf );
	SAFEDELETEARRAY( m_pYMoveLeft );
	SAFEDELETEARRAY( m_pYMoveLeft2 );
	SAFEDELETEARRAY( m_pYMoveRight );
	SAFEDELETEARRAY( m_pYMoveRight2 );
	SAFEDELETEARRAY( m_pYMoveBottom );
	SAFEDELETEARRAY( m_pYMoveBottom2 );
	SAFEDELETEARRAY( m_pYMoveTop );
	SAFEDELETEARRAY( m_pYMoveTop2 );
}

bool ioPatchMorphSW::UpdateLevel()
{
	ioPatchMorphSW* leftSW	 = (ioPatchMorphSW*)m_pLeft;
	ioPatchMorphSW* rightSW  = (ioPatchMorphSW*)m_pRight;
	ioPatchMorphSW* bottomSW = (ioPatchMorphSW*)m_pBottom;
	ioPatchMorphSW* topSW	 = (ioPatchMorphSW*)m_pTop;

	if( leftSW && 
		leftSW->m_iOldRealSelfLv<EMPTY_LEVEL &&
		leftSW->m_iNewRealSelfLv!=leftSW->m_iOldRealSelfLv )
	{
		m_bRecalcBorderLeft = true;
	}

	if( rightSW &&
		rightSW->m_iOldRealSelfLv<EMPTY_LEVEL &&
		rightSW->m_iNewRealSelfLv!=rightSW->m_iOldRealSelfLv )
	{
		m_bRecalcBorderRight = true;
	}

	if( bottomSW &&
		bottomSW->m_iOldRealSelfLv<EMPTY_LEVEL &&
		bottomSW->m_iNewRealSelfLv!=bottomSW->m_iOldRealSelfLv )
	{
		m_bRecalcBorderBottom = true;
	}

	if( topSW &&
		topSW->m_iOldRealSelfLv<EMPTY_LEVEL &&
		topSW->m_iNewRealSelfLv!=topSW->m_iOldRealSelfLv )
	{
		m_bRecalcBorderTop = true;
	}

	if( ioPatch::UpdateLevel() )
		m_bForceBufferCreate = true;

	return false;
}

bool ioPatchMorphSW::UpdateLevelSecond()
{
	float selfTesEx = GetExactLevel();
	float leftTesEx, rightTesEx, bottomTesEx, topTesEx;

	if( m_pLeft && m_pLeft->IsInited() )	
		leftTesEx = m_pLeft->GetExactLevel();	
	else
		leftTesEx = -1.0f;	

	if( m_pRight && m_pRight->IsInited() )
		rightTesEx = m_pRight->GetExactLevel();
	else
		rightTesEx = -1.0f;

	if( m_pBottom && m_pBottom->IsInited() )
		bottomTesEx = m_pBottom->GetExactLevel();
	else
		bottomTesEx = -1.0f;

	if( m_pTop && m_pTop->IsInited() )
		topTesEx = m_pTop->GetExactLevel();
	else
		topTesEx = -1.0f;

	m_bNewFollowsLeft = (selfTesEx<leftTesEx) || (selfTesEx==leftTesEx&& !((ioPatchMorphSW*)m_pLeft)->m_bNewFollowsRight);
	m_bNewFollowsRight = (selfTesEx<rightTesEx) || (selfTesEx==rightTesEx && !((ioPatchMorphSW*)m_pRight)->m_bNewFollowsLeft);
	m_bNewFollowsBottom = (selfTesEx<bottomTesEx) || (selfTesEx==bottomTesEx && !((ioPatchMorphSW*)m_pBottom)->m_bNewFollowsTop);
	m_bNewFollowsTop = (selfTesEx<topTesEx) || (selfTesEx==topTesEx && !((ioPatchMorphSW*)m_pTop)->m_bNewFollowsBottom);

	if( !m_bForceBufferCreate )
	{
		if( (!m_bNewFollowsLeft && m_bFollowsLeft) || 
			(!m_bNewFollowsRight && m_bFollowsRight) ||
		    (!m_bNewFollowsBottom && m_bFollowsBottom) ||
			(!m_bNewFollowsTop && m_bFollowsTop) )
		{
			assert( m_iSelfLv-m_iLeftLv<=1 );
			assert( m_iSelfLv-m_iRightLv<=1 );
			assert( m_iSelfLv-m_iBottomLv<=1 );
			assert( m_iSelfLv-m_iTopLv<=1 );

			CreateLevel( m_iSelfLv, m_iLeftLv, m_iRightLv, m_iBottomLv, m_iTopLv );
			m_bForceReTessellation = true;
		}
	}

	FillMorphVertex();
	return false;
}

void ioPatchMorphSW::FillMorphVertex()
{
	if( m_bForceBufferCreate || m_bForceReTessellation )
	{
		ioCPU::FastMemcpy(m_pVertexLow, m_pVertex, m_iCurVertex*sizeof(D3DXVECTOR3));

		if( m_iRealSelfLv<MAX_SUBDIV )
		{
			ReduceShapeTo( m_iRealSelfLv+1, m_pVertexLow );
			MakeBordersSimpler( m_pVertexLow );
		}

		for(int i=0; i<m_iCurVertex; i++)
		{
			m_pYMoveSelf[i] = m_pVertex[i].y - m_pVertexLow[i].y;
		}
	}
}

bool ioPatchMorphSW::UpdateLevelThird()
{
	CheckBorderLeft();
	CheckBorderRight();
	CheckBorderBottom();
	CheckBorderTop();

	m_bForceBufferCreate = false;

	m_bFollowsLeft   = m_bNewFollowsLeft;
	m_bFollowsRight  = m_bNewFollowsRight;
	m_bFollowsBottom = m_bNewFollowsBottom;
	m_bFollowsTop	 = m_bNewFollowsTop;

	m_bRecalcBorderLeft	   = false;
	m_bRecalcBorderRight   = false;
	m_bRecalcBorderBottom  = false;
	m_bRecalcBorderTop     = false;

	m_bForceReTessellation = false;

	return false;
}

void ioPatchMorphSW::CheckBorderLeft()
{
	if( m_bForceBufferCreate ||
		m_bForceReTessellation ||
		m_bFollowsLeft != m_bNewFollowsLeft )
	{
		memset( m_pYMoveLeft, 0, sizeof(float)*m_iCurVertex );
		memset( m_pYMoveLeft2, 0, sizeof(float)*m_iCurVertex );
	}

	if( m_bNewFollowsLeft &&
		(!m_bFollowsLeft || m_bForceBufferCreate || m_bForceReTessellation || m_bRecalcBorderLeft) )
	{
		const ioPatchMorphSW* leftSW = reinterpret_cast<const ioPatchMorphSW*>(m_pLeft);

		int y,yc, pow;

		// copy shared border column
		//
		assert( m_iSelfLv==leftSW->m_iRightLv );
		assert( m_iLeftLv==leftSW->m_iSelfLv );
		assert( leftSW->m_pIndexMap );

		pow = GetPowerTwo( min( m_iSelfLv, m_iLeftLv ) );

		float yLT, yRB;
		int idx, idxL, idxLT, idxRB;

		for(y=pow; y<SOURCE_HEIGHT-1; y+=pow)
		{
			idx = m_pIndexMap[ y * SOURCE_WIDTH ];
			idxL = leftSW->m_pIndexMap[SOURCE_WIDTH-1 + y * SOURCE_WIDTH];

			assert(idx!=0xffff);
			assert(idxL!=0xffff);

			m_pVertexLow[idx].y = leftSW->m_pVertexLow[idxL].y;
			m_pVertex[idx].y = leftSW->m_pVertex[idxL].y;
			m_pYMoveLeft[idx] = leftSW->m_pYMoveSelf[idxL];
			m_pYMoveSelf[idx] = 0.0f;
		}

		if( m_iSelfLv>=3 )
			return;

		// correct border neighbors
		//
		pow = GetPowerTwo( m_iSelfLv );

		float moveLT;

		for(y=pow,yc=1; y<SOURCE_HEIGHT-1; y+=pow,yc++)
		{
			yLT = 0.0f;
			yRB = 0.0f;
			idxLT = 0;
			idxRB = 0;

			idx = CalculateDiagonalVertices(pow, y, 1, yc, pow, m_pVertexLow, yLT,idxLT, yRB,idxRB);
			assert(idx>=0 && idx!=0xffff);

			moveLT = m_pVertex[idxLT].y - m_pVertexLow[idxLT].y;

			m_pVertexLow[idx].y = (yLT + yRB) / 2.0f;

			// our vertex is moved 50% by the left (-top) neighbor
			// this garanties that this vertex is on the diagonal when currentError is 0.0f
			m_pYMoveLeft2[idx] = moveLT * FLOAT05;
			m_pYMoveSelf[idx]  = m_pVertex[idx].y - m_pVertexLow[idx].y;
		}


		// correct neighbors to lie on a diagonal
		//
		if( m_iSelfLv!=m_iRealSelfLv )
		{
			pow = GetPowerTwo( m_iSelfLv );

			for(y=pow,yc=1; y<SOURCE_HEIGHT-1; y+=pow,yc++)
			{
				yLT = 0.0f;
				yRB = 0.0f;
				idxLT = 0;
				idxRB = 0;

				idx = CalculateDiagonalVertices(pow, y, 1, yc, pow, m_pVertexLow, yLT,idxLT, yRB,idxRB);
				assert(idx>=0 && idx!=0xffff);

				m_pVertexLow[idx].y = (yLT + yRB) * FLOAT05;
				m_pVertex[idx].y = (m_pVertex[idxLT].y + m_pVertex[idxRB].y) * FLOAT05;

				// the real new part: this vertex always STAYS on the diagonal
				//
				m_pYMoveLeft2[idx] = 0.0f;
				m_pYMoveLeft[idx] = (m_pVertex[idxLT].y - m_pVertexLow[idxLT].y) * FLOAT05;
				m_pYMoveSelf[idx] = (m_pVertex[idxRB].y - m_pVertexLow[idxRB].y) * FLOAT05;
			}

			// necessary to recorrect the shared right-top border neighbor
			//
			if( m_bFollowsBottom )
			{
				m_bRecalcBorderBottom = true;
			}
		}
	}	
}

void ioPatchMorphSW::CheckBorderRight()
{
	if( m_bForceBufferCreate ||
		m_bForceReTessellation ||
		m_bFollowsRight!=m_bNewFollowsRight )
	{
		memset( m_pYMoveRight, 0, sizeof(float)*m_iCurVertex );
		memset( m_pYMoveRight2, 0, sizeof(float)*m_iCurVertex );
	}

	if( m_bNewFollowsRight &&
	    (!m_bFollowsRight || m_bForceBufferCreate || m_bForceReTessellation || m_bRecalcBorderRight) )
	{
		const ioPatchMorphSW* rightSW = reinterpret_cast<const ioPatchMorphSW*>(m_pRight);

		int y, yc, pow, xcMax;

		// copy shared border column
		//
		assert( m_iSelfLv==rightSW->m_iLeftLv );
		assert( m_iRightLv==rightSW->m_iSelfLv );
		assert( rightSW->m_pIndexMap );

		pow = GetPowerTwo( min(m_iSelfLv,m_iRightLv) );

		float yLT, yRB;
		int idx, idxR, idxLT, idxRB;

		for(y=pow; y<SOURCE_HEIGHT-1; y+=pow)
		{
			idx = m_pIndexMap[ SOURCE_WIDTH-1 + y * SOURCE_WIDTH ];
			idxR = rightSW->m_pIndexMap[ y * SOURCE_WIDTH ];

			assert(idx!=0xffff);
			assert(idxR!=0xffff);

			m_pVertexLow[idx].y = rightSW->m_pVertexLow[idxR].y;
			m_pVertex[idx].y = rightSW->m_pVertex[idxR].y;
			m_pYMoveRight[idx] = rightSW->m_pYMoveSelf[idxR];
			m_pYMoveSelf[idx] = 0.0f;
		}

		// correct border neighbors
		//
		if( m_iRealSelfLv<3 )
		{
			pow = GetPowerTwo( m_iRealSelfLv );
			xcMax = (SOURCE_WIDTH-1-pow)/pow;

			for(y=pow,yc=1; y<SOURCE_HEIGHT-1; y+=pow,yc++)
			{
				yLT = 0.0f;
				yRB = 0.0f;
				idxLT = 0;
				idxRB = 0;

				idx = CalculateDiagonalVertices( SOURCE_WIDTH-1-pow, y, xcMax, yc, pow, m_pVertexLow, yLT,idxLT, yRB,idxRB );
				assert(idx>=0 && idx!=0xffff);

				m_pVertexLow[idx].y = (yLT + yRB) * FLOAT05;

				// our vertex is moved 50% by the right (-bottom) neighbor
				// this garanties that this vertex is on the diagonal when currentError is 0.0f
				m_pYMoveRight2[idx] = (m_pVertex[idxRB].y - m_pVertexLow[idxRB].y) * FLOAT05;
				m_pYMoveSelf[idx] = m_pVertex[idx].y - m_pVertexLow[idx].y;
			}
		}

		// correct neighbors to lie on a diagonal
		//
		if( m_iSelfLv!=m_iRealSelfLv )
		{
			pow = GetPowerTwo( m_iSelfLv );
			xcMax = (SOURCE_WIDTH-1-pow)/pow;

			for(y=pow,yc=1; y<SOURCE_HEIGHT-1; y+=pow,yc++)
			{
				yLT = 0.0f;
				yRB = 0.0f;
				idxLT = 0;
				idxRB = 0;

				idx = CalculateDiagonalVertices(SOURCE_WIDTH-1-pow, y, xcMax, yc, pow, m_pVertexLow, yLT,idxLT, yRB,idxRB);
				assert(idx>=0 && idx!=0xffff);

				m_pVertexLow[idx].y = (yLT + yRB) * FLOAT05;
				m_pVertex[idx].y = (m_pVertex[idxLT].y + m_pVertex[idxRB].y) * FLOAT05;

				// the real new part: this vertex always STAYS on the diagonal
				//
				m_pYMoveRight2[idx] = 0.0f;
				m_pYMoveRight[idx] = (m_pVertex[idxRB].y - m_pVertexLow[idxRB].y) * FLOAT05;
				m_pYMoveSelf[idx] = (m_pVertex[idxLT].y - m_pVertexLow[idxLT].y) * FLOAT05;
			}

			// necessary to recorrect the shared right-top border neighbor
			//
			if( m_bFollowsTop )
			{
				m_bRecalcBorderTop = true;
			}
		}
	}
}

void ioPatchMorphSW::CheckBorderBottom()
{
	if( m_bForceBufferCreate ||
		m_bForceReTessellation ||
		m_bFollowsBottom!=m_bNewFollowsBottom )
	{
		memset( m_pYMoveBottom, 0, sizeof(float) * m_iCurVertex );
		memset( m_pYMoveBottom2, 0, sizeof(float) * m_iCurVertex );
	}

	if( m_bNewFollowsBottom && 
		(!m_bFollowsBottom || m_bForceBufferCreate || m_bForceReTessellation || m_bRecalcBorderBottom) )
	{
		const ioPatchMorphSW* bottomSW = reinterpret_cast<const ioPatchMorphSW*>(m_pBottom);

		int x, xc, pow;

		// copy shared border column
		//
		assert( m_iSelfLv==bottomSW->m_iTopLv );
		assert( m_iBottomLv==bottomSW->m_iSelfLv );
		assert( bottomSW->m_pIndexMap );

		pow = GetPowerTwo( min( m_iSelfLv, m_iBottomLv ) );

		int idx, idxB, idxLT, idxRB;

		for(x=pow; x<SOURCE_WIDTH-1; x+=pow)
		{
			idx = m_pIndexMap[x];
			idxB = bottomSW->m_pIndexMap[x + (SOURCE_WIDTH-1) * SOURCE_WIDTH];

			assert(idx!=0xffff);
			assert(idxB!=0xffff);

			m_pVertexLow[idx].y = bottomSW->m_pVertexLow[idxB].y;
			m_pVertex[idx].y = bottomSW->m_pVertex[idxB].y;
			m_pYMoveBottom[idx] = bottomSW->m_pYMoveSelf[idxB];
			m_pYMoveSelf[idx] = 0.0f;
		}

		if( m_iSelfLv>=3 )
			return;

		// correct border neighbors
		//
		pow = GetPowerTwo( m_iSelfLv );

		float yLT, yRB, moveRB;

		for(x=pow,xc=1; x<SOURCE_WIDTH-1; x+=pow,xc++)
		{
			yLT = 0.0f;
			yRB = 0.0f;
			idxLT = 0;
			idxRB = 0;

			idx = CalculateDiagonalVertices(x, pow, xc, 1, pow, m_pVertexLow, yLT,idxLT, yRB,idxRB);
			assert(idx>=0 && idx!=0xffff);

			moveRB = m_pVertex[idxRB].y - m_pVertexLow[idxRB].y;

			m_pVertexLow[idx].y = (yLT + yRB) / 2.0f;

			// our vertex is moved 50% by the right (-bottom) neighbor
			// this garanties that this vertex is on the diagonal when currentError is 0.0f
			m_pYMoveBottom2[idx] = moveRB * FLOAT05;
			m_pYMoveSelf[idx] = m_pVertex[idx].y - m_pVertexLow[idx].y;
		}

		// correct neighbors to lie on a diagonal
		//
		if( m_iSelfLv!=m_iRealSelfLv )
		{
			pow = GetPowerTwo( m_iSelfLv );

			for(x=pow,xc=1; x<SOURCE_WIDTH-1; x+=pow,xc++)
			{
				yLT = 0.0f;
				yRB = 0.0f;
				idxLT = 0;
				idxRB = 0;

				idx = CalculateDiagonalVertices(x, pow, xc, 1, pow, m_pVertexLow, yLT,idxLT, yRB,idxRB);
				assert(idx>=0 && idx!=0xffff);

				m_pVertexLow[idx].y = (yLT + yRB) * FLOAT05;
				m_pVertex[idx].y = (m_pVertex[idxLT].y + m_pVertex[idxRB].y) * FLOAT05;

				// the real new part: this vertex always STAYS on the diagonal
				//
				m_pYMoveBottom2[idx] = 0.0f;
				m_pYMoveBottom[idx] = (m_pVertex[idxRB].y - m_pVertexLow[idxRB].y) * FLOAT05;

				if( m_pYMoveLeft[idx]==0.0f )
				{
					m_pYMoveSelf[idx] = (m_pVertex[idxLT].y - m_pVertexLow[idxLT].y) * FLOAT05;
				}
				else
				{
					m_pYMoveSelf[idx] = 0.0f;
				}
			}
		}
	}
}

void ioPatchMorphSW::CheckBorderTop()
{
	if( m_bForceBufferCreate || m_bForceReTessellation || m_bFollowsTop!=m_bNewFollowsTop )
	{
		memset( m_pYMoveTop, 0, sizeof(float) * m_iCurVertex );
		memset( m_pYMoveTop2, 0, sizeof(float) * m_iCurVertex );
	}

	if( m_bNewFollowsTop &&
		(!m_bFollowsTop || m_bForceBufferCreate || m_bForceReTessellation || m_bRecalcBorderTop) )
	{
		const ioPatchMorphSW* topSW = reinterpret_cast<const ioPatchMorphSW*>(m_pTop);

		int x, xc, pow, ycMax;

		// copy shared border column
		//
		assert( m_iSelfLv==topSW->m_iBottomLv );
		assert( m_iTopLv==topSW->m_iSelfLv );
		assert( topSW->m_pIndexMap );

		pow = GetPowerTwo( min(m_iSelfLv, m_iTopLv) );

		int idx, idxT;

		for( x=pow; x<SOURCE_WIDTH-1; x+=pow )
		{
			idx = m_pIndexMap[ x + (SOURCE_HEIGHT-1) * SOURCE_WIDTH ];
			idxT = topSW->m_pIndexMap[ x ];

			assert(idx!=0xffff);
			assert(idxT!=0xffff);

			m_pVertexLow[idx].y = topSW->m_pVertexLow[idxT].y;
			m_pVertex[idx].y = topSW->m_pVertex[idxT].y;
			m_pYMoveTop[idx] = topSW->m_pYMoveSelf[idxT];
			m_pYMoveSelf[idx] = 0.0f;
		}

		if( m_iSelfLv>=3 )
			return;

		// correct border neighbors
		//
		pow = GetPowerTwo( m_iSelfLv );
		ycMax = (SOURCE_WIDTH-1-pow)/pow;

		int idxLT, idxRB;
		float yLT, yRB, moveLT;

		for(x=pow,xc=1; x<SOURCE_WIDTH-1; x+=pow,xc++)
		{
			yLT = 0.0f;
			yRB = 0.0f;
			idxLT = 0;
			idxRB = 0;
			
			idx = CalculateDiagonalVertices(x, SOURCE_HEIGHT-1-pow, xc, ycMax, pow, m_pVertexLow, yLT,idxLT, yRB,idxRB);
			assert(idx>=0 && idx!=0xffff);

			moveLT = m_pVertex[idxLT].y - m_pVertexLow[idxLT].y;

			m_pVertexLow[idx].y = (yLT + yRB) / 2.0f;

			// our vertex is moved 50% by the left (-top) neighbor
			// this garanties that this vertex is on the diagonal when currentError is 0.0f
			m_pYMoveTop2[idx] = moveLT * FLOAT05;

			m_pYMoveSelf[idx] = m_pVertex[idx].y - m_pVertexLow[idx].y;
		}

		// correct neighbors to lie on a diagonal
		//
		if( m_iSelfLv!=m_iRealSelfLv )
		{
			pow = GetPowerTwo( m_iSelfLv );
			ycMax = (SOURCE_WIDTH-1-pow)/pow;

			for(x=pow,xc=1; x<SOURCE_WIDTH-1; x+=pow,xc++)
			{
				yLT = 0.0f;
				yRB = 0.0f;
				idxLT = 0;
				idxRB = 0;

				idx = CalculateDiagonalVertices(x, SOURCE_HEIGHT-1-pow, xc, ycMax, pow, m_pVertexLow, yLT,idxLT, yRB,idxRB);
				assert(idx>=0 && idx!=0xffff);

				m_pVertexLow[idx].y = (yLT + yRB) * FLOAT05;
				m_pVertex[idx].y = (m_pVertex[idxLT].y + m_pVertex[idxRB].y) * FLOAT05;

				// the real new part: this vertex always STAYS on the diagonal
				//
				m_pYMoveTop2[idx] = 0.0f;
				m_pYMoveTop[idx] = (m_pVertex[idxLT].y - m_pVertexLow[idxLT].y) * FLOAT05;

				if(m_pYMoveRight[idx]==0.0f)
					m_pYMoveSelf[idx] = (m_pVertex[idxRB].y - m_pVertexLow[idxRB].y) * FLOAT05;
				else
					m_pYMoveSelf[idx] = 0.0f;
			}
		}
	}
}

int ioPatchMorphSW::FillVertices( D3DXVECTOR3 *pBuf )
{
	if( m_iCurVertex == 0 || m_iCurIndex == 0 )	return 0;

	if( m_iSelfLv>0 || m_iSelfLv<MAX_SUBDIV )
	{
		float fFactorSelf	 = 1.0f - m_fCurError;
		float fFactorLeft	 = m_pLeft	 ? 1.0f - m_pLeft->GetCurError()   : 0.0f;
		float fFactorRight   = m_pRight  ? 1.0f - m_pRight->GetCurError()  : 0.0f;
		float fFactorBottom  = m_pBottom ? 1.0f - m_pBottom->GetCurError() : 0.0f;
		float fFactorTop	 = m_pTop	 ? 1.0f - m_pTop->GetCurError()	   : 0.0f;

		assert( fFactorSelf>=0.0f );
		assert( fFactorLeft>=0.0f );
		assert( fFactorRight>=0.0f );
		assert( fFactorBottom>=0.0f );
		assert( fFactorTop>=0.0f );

		// clamping for patches which use selfTes!=realSelfTes
		//
		if( fFactorSelf>1.0f )
			fFactorSelf = 1.0f;
		if( fFactorLeft>1.0f )
			fFactorLeft = 1.0f;
		if( fFactorRight>1.0f )
			fFactorRight = 1.0f;
		if( fFactorBottom>1.0f )
			fFactorBottom = 1.0f;
		if( fFactorTop>1.0f )
			fFactorTop = 1.0f;

		float	fFactorLeft2   = (1.0f-fFactorSelf) * fFactorLeft;
		float	fFactorRight2  = (1.0f-fFactorSelf) * fFactorRight;
		float	fFactorBottom2 = (1.0f-fFactorSelf) * fFactorBottom;
		float	fFactorTop2	   = (1.0f-fFactorSelf) * fFactorTop;
		
		D3DXVECTOR3 vTempVtx;

		for(int i=0; i<m_iCurVertex; i++)
		{
			vTempVtx.x = m_pVertexLow[i].x;			

			vTempVtx.y = m_pVertexLow[i].y + fFactorSelf * m_pYMoveSelf[i]
										   + fFactorLeft * m_pYMoveLeft[i]
										   + fFactorLeft2 * m_pYMoveLeft2[i]
										   + fFactorRight * m_pYMoveRight[i]
										   + fFactorRight2 * m_pYMoveRight2[i]
										   + fFactorBottom * m_pYMoveBottom[i]
										   + fFactorBottom2 * m_pYMoveBottom2[i]
										   + fFactorTop * m_pYMoveTop[i]
										   + fFactorTop2 * m_pYMoveTop2[i];

			vTempVtx.z = m_pVertexLow[i].z;

			pBuf[i] = vTempVtx;
		}
	}
	else
	{
		ioCPU::FastMemcpy( pBuf, m_pVertex, m_iCurVertex*sizeof(D3DXVECTOR3) );
	}

	return m_iCurVertex;
}
