

#ifndef _Grid3D_h_
#define _Grid3D_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#define MAX_INDEX_BUF	(4 * 100)

class __EX Grid3D
{
private:
	WORD m_aLargeIndex[ MAX_INDEX_BUF ];
	WORD m_aSmallIndex[ MAX_INDEX_BUF ];
	WORD m_aBaseIndex[ 4 ];

	ioVtxColor	m_aLargeGridVertex[ MAX_INDEX_BUF ];
	ioVtxColor	m_aSmallGridVertex[ MAX_INDEX_BUF ];
	ioVtxColor	m_aBaseGridVertex[ 4 ];

	UINT m_NumLargeVertexIndices;
	UINT m_NumSmalllVertexIndices;
	UINT m_LargePrimitiveCount;
	UINT m_SmallPrimitiveCount;

private:
	int m_nLTOffset;		// Large Line АЃАн
	int	m_nSLCount;
	int	m_nHalfSLCount;

public:
	BOOL Create( int nSLCount, int nLTOffset, int iInterval );
	void DrawGrid();

public:
	Grid3D();
	~Grid3D();
};

__EX Grid3D& GetGrid3D();

#endif