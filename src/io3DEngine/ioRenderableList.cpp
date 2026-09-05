

#include "stdafx.h"

#include "ioCPU.h"
#include "HelpFunc.h"
#include "ioRenderableList.h"


ioRenderableList::ioRenderableList()
{
	m_pArray = NULL;
	m_iMaxSize = 0;
	m_iCurSize = 0;
	m_iGrowBy  = 0;
}

ioRenderableList::ioRenderableList( int iMaxSize, int iGrowBy )
{
	m_pArray = NULL;

	Initialize( iMaxSize, iGrowBy );
}

ioRenderableList::~ioRenderableList()
{
	SAFEDELETEARRAY( m_pArray );
}

void ioRenderableList::Initialize( int iMaxSize, int iGrowBy )
{
	SAFEDELETEARRAY( m_pArray );

	m_pArray = new RenderablePass[iMaxSize];

	m_iMaxSize = iMaxSize;
	m_iCurSize = 0;
	m_iGrowBy  = iGrowBy;
}

void ioRenderableList::Add( const RenderablePass &rkPass )
{
	if( m_iCurSize < m_iMaxSize )
	{
		m_pArray[m_iCurSize++] = rkPass;
	}
	else
	{
		RenderablePass *pNewBuf = new RenderablePass[m_iMaxSize + m_iGrowBy];
		ioCPU::FastMemcpy( pNewBuf, m_pArray, sizeof(RenderablePass) * m_iCurSize );
		m_iMaxSize += m_iGrowBy;

		SAFEDELETEARRAY( m_pArray );
		m_pArray = pNewBuf;

		m_pArray[m_iCurSize++] = rkPass;
	}
}
