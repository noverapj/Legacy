#pragma once
#include "ioBlock.h"
#include "ioTileBlock.h"
#include "ioPosMapContainer.h"

class ioPosMapQuadTree;

typedef ioPosMapContainer<ioBlock*> PosMapContainer;
typedef ioPosMapContainer<ioTileBlock*> TileMapContainer;

struct BlockMapBuffer
{
	ioBlock*	m_pBuffer[BUFFER_MAX];
	int			m_nPos;

	BlockMapBuffer()
	{
		ZeroMemory( m_pBuffer, sizeof( ioBlock* ) * BUFFER_MAX );
		m_nPos = 0;
	}

	void push_back( ioBlock* pBlock )
	{
		m_pBuffer[m_nPos] = pBlock;
		m_nPos++;
	}

	int size()
	{
		return m_nPos;
	}
};

class ioBlockMapBase
{
protected:
	ioPosMapQuadTree*		m_pBlockQuadTree;
	PosMapContainer*		m_PosLevelMap[HEIGHT_MAX];
	TileMapContainer*		m_pTileContainer;
	int						m_nHeightMax;
	int						m_nTileMax;

public:
	void Initialize( BlockModeType eType );

protected:
	void BuildQuadTree( BlockModeType eType );
	void DestroyQuadTree();

	void CreateLevelMapContainer( BlockModeType eType );
	void DestroyLevelMapContainer();

	void CreateTileMapContainer( BlockModeType eType );
	void DestroyTileMapContainer();

	void DestroyTileMap();

public:
	TileMapContainer* GetTileMapContainer();
	PosMapContainer* GetPosMapContainer( int nLevel );

	ioBlock* GetBlockByWorldPosition( IN const D3DXVECTOR3& vPos );
	ioBlock* GetBlockByTilePos( IN const TilePos& Pos );

	void GetBlockByTop( IN const D3DXVECTOR3& vPos, OUT ioBlockList& List );	
	void WorldCoordToTileCoord( const D3DXVECTOR3& vPos, TilePos& Pos );
	void GetAdjacentBlockList( IN const TilePos& Pos, IN float fRange, OUT BlockMapBuffer& List );
	void GetAdjacentBlockList( IN const D3DXVECTOR3& vPos, IN float fRange, OUT BlockMapBuffer& List );
	void GetAdjacentBlockList( IN int nMin, IN int nMax, IN const TilePos& Pos, IN float fRange, OUT BlockMapBuffer& List );
	void GetAdjacentBlockListByTile( IN const TilePos& Pos, IN float fRange, OUT BlockMapBuffer& List );
	void FindBlockIndexList( const ioRay& kYRay, OUT ioBlockList& List );	

public:
	void SetShadowMapEnable( bool bEnable );

public:
	void RenderPosQuadTree( ioPlayStage* pStage, ioCamera* pCamera );	
	void RenderBlockAABB( CollRenderType eType, ioPlayStage* pStage, ioCamera* pCamera );

public:
	ioBlockMapBase();
	virtual ~ioBlockMapBase();
};