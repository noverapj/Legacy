
#include "StdAfx.h"

#include "Grid3D.h"
#include "ioRenderSystem.h"

Grid3D::Grid3D()
{
}

Grid3D::~Grid3D()
{
}

// nSLCount  : 스켄라인 타일 카운트 (짝수)
// nLTOffset : 큰 격자 출력 간격
BOOL Grid3D::Create( int nSLCount, int nLTOffset, int iInterval )
{
	m_nSLCount     = ( nSLCount + 1 ) & ~1;// 짝수가 아니면 +1
	m_nHalfSLCount = m_nSLCount / 2;
	m_nLTOffset    = nLTOffset;

	m_NumLargeVertexIndices	 = 0;
	m_NumSmalllVertexIndices = 0;
	m_LargePrimitiveCount	 = 0;
	m_SmallPrimitiveCount	 = 0;

	float fOutPos = (float)(m_nHalfSLCount * iInterval);
	float fGap = 0.0f;
	for( int i = -m_nHalfSLCount; i <= m_nHalfSLCount; ++i ) 
	{
		fGap = (float)(i * iInterval);

		if( 0 == i % m_nLTOffset )	// Large Grid
		{
			m_aLargeIndex[ m_NumLargeVertexIndices + 0 ] = (WORD)m_NumLargeVertexIndices;
			m_aLargeIndex[ m_NumLargeVertexIndices + 1 ] = (WORD)m_NumLargeVertexIndices + 1;
			m_aLargeIndex[ m_NumLargeVertexIndices + 2 ] = (WORD)m_NumLargeVertexIndices + 2;
			m_aLargeIndex[ m_NumLargeVertexIndices + 3 ] = (WORD)m_NumLargeVertexIndices + 3;

			m_aLargeGridVertex[ m_NumLargeVertexIndices ].vPos = D3DXVECTOR3( -fOutPos, 0, fGap );
			m_aLargeGridVertex[ m_NumLargeVertexIndices++ ].dwColor = 0xff505050;
			m_aLargeGridVertex[ m_NumLargeVertexIndices ].vPos = D3DXVECTOR3( fOutPos, 0, fGap );
			m_aLargeGridVertex[ m_NumLargeVertexIndices++ ].dwColor = 0xff505050;
			m_aLargeGridVertex[ m_NumLargeVertexIndices ].vPos = D3DXVECTOR3( fGap, 0, -fOutPos );
			m_aLargeGridVertex[ m_NumLargeVertexIndices++ ].dwColor = 0xff505050;
			m_aLargeGridVertex[ m_NumLargeVertexIndices ].vPos = D3DXVECTOR3( fGap, 0, fOutPos );
			m_aLargeGridVertex[ m_NumLargeVertexIndices++ ].dwColor = 0xff505050;
			
			m_LargePrimitiveCount += 2;
		}
		else // Small Grid
		{
			m_aSmallIndex[ m_NumSmalllVertexIndices + 0 ] = (WORD)m_NumSmalllVertexIndices;
			m_aSmallIndex[ m_NumSmalllVertexIndices + 1 ] = (WORD)m_NumSmalllVertexIndices + 1;
			m_aSmallIndex[ m_NumSmalllVertexIndices + 2 ] = (WORD)m_NumSmalllVertexIndices + 2;
			m_aSmallIndex[ m_NumSmalllVertexIndices + 3 ] = (WORD)m_NumSmalllVertexIndices + 3;

			m_aSmallGridVertex[ m_NumSmalllVertexIndices ].vPos = D3DXVECTOR3( -fOutPos, 0, fGap );
			m_aSmallGridVertex[ m_NumSmalllVertexIndices++ ].dwColor = 0xff707070;
			m_aSmallGridVertex[ m_NumSmalllVertexIndices ].vPos = D3DXVECTOR3( fOutPos, 0, fGap );
			m_aSmallGridVertex[ m_NumSmalllVertexIndices++ ].dwColor = 0xff707070;
			m_aSmallGridVertex[ m_NumSmalllVertexIndices ].vPos = D3DXVECTOR3( fGap, 0, -fOutPos );
			m_aSmallGridVertex[ m_NumSmalllVertexIndices++ ].dwColor = 0xff707070;
			m_aSmallGridVertex[ m_NumSmalllVertexIndices ].vPos = D3DXVECTOR3( fGap, 0, fOutPos );
			m_aSmallGridVertex[ m_NumSmalllVertexIndices++ ].dwColor = 0xff707070;
			
			m_SmallPrimitiveCount += 2;
		}
	}

	// << Base Line
	m_aBaseIndex[0] = 0;
	m_aBaseIndex[1] = 1;
	m_aBaseIndex[2] = 2;
	m_aBaseIndex[3] = 3;

	m_aBaseGridVertex[0].vPos = D3DXVECTOR3( -fOutPos, 0, 0 );
	m_aBaseGridVertex[0].dwColor = 0xff000000;
	m_aBaseGridVertex[1].vPos = D3DXVECTOR3( fOutPos, 0, 0 );
	m_aBaseGridVertex[1].dwColor = 0xff000000;
	m_aBaseGridVertex[2].vPos = D3DXVECTOR3( 0, 0, -fOutPos );
	m_aBaseGridVertex[2].dwColor = 0xff000000;
	m_aBaseGridVertex[3].vPos = D3DXVECTOR3( 0, 0, fOutPos );
	m_aBaseGridVertex[3].dwColor = 0xff000000;
	// >> EOF Base Line

	return TRUE;
}

void Grid3D::DrawGrid()
{
	ioRenderSystem &rkSystem = RenderSystem();
	rkSystem.SetFixedFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	rkSystem.DisableShader();
	rkSystem.SetLightEnable( false );
	rkSystem.SetDepthFunction( D3DCMP_LESSEQUAL, true, true );
	rkSystem.DisableAlphaBlend();
	rkSystem.DisableAlphaTest();

	rkSystem.SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_DIFFUSE, D3DTA_DIFFUSE );
	rkSystem.SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_DIFFUSE, D3DTA_DIFFUSE );
	rkSystem.DisableTextureUnitsFrom( 1 );
	rkSystem.ClearStreamDesc();

	IDirect3DDevice9 *pDevice = rkSystem.GetDevice();

  	// << Draw Small Tile
	pDevice->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 
									 0,
									 m_NumSmalllVertexIndices,
									 m_SmallPrimitiveCount,
									 m_aSmallIndex,
									 D3DFMT_INDEX16,
									 m_aSmallGridVertex,
									 sizeof(ioVtxColor) );
	// >> EOF Small Tile

	// << Draw Large Tile	
	pDevice->DrawIndexedPrimitiveUP( D3DPT_LINELIST,
									   0,
									   m_NumLargeVertexIndices,
									   m_LargePrimitiveCount,
									   m_aLargeIndex,
									   D3DFMT_INDEX16,
									   m_aLargeGridVertex,
									   sizeof(ioVtxColor) );
	// >> EOF Large Tile

	// << Draw Base Line	
	pDevice->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 
	 							     0,
									 4,
									 2,
									 m_aBaseIndex,
									 D3DFMT_INDEX16,
									 m_aBaseGridVertex,
									 sizeof(ioVtxColor) );
	// >> EOF Base Line
}

__EX Grid3D& GetGrid3D()
{
	static Grid3D theGrid3D;
	return theGrid3D;
}