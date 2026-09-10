#include "stdafx.h"

#include "ioBlockMapBase.h"
#include "ioPosMapQuadTree.h"
#include "ioBlockManager.h"

#include "../ioPlayStage.h"

ioBlockMapBase::ioBlockMapBase()
{
	m_nHeightMax		= 0;
	m_nTileMax			= 0;

	m_pBlockQuadTree	= NULL;
	m_pTileContainer	= NULL;	

	for( int i = 0; i < HEIGHT_MAX; ++i )
		m_PosLevelMap[i] = NULL;
}

ioBlockMapBase::~ioBlockMapBase()
{
	DestroyQuadTree();
	DestroyLevelMapContainer();
	DestroyTileMapContainer();
}

void ioBlockMapBase::Initialize( BlockModeType eType )
{
	BuildQuadTree( eType );

	CreateLevelMapContainer( eType );
	CreateTileMapContainer( eType );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 타일 좌표 맵 생성
//

void ioBlockMapBase::BuildQuadTree( BlockModeType eType )
{
	int nHeight		= g_BlockManager.GetBlockStartHeight();
	int nTileMax	= g_BlockManager.GetTileMax();
	int nTileMiddle	= (nTileMax-1) * UD_SIZE / 2;

	m_pBlockQuadTree = new ioPosMapQuadTree;
	m_pBlockQuadTree->Build( D3DXVECTOR3( nTileMiddle, nHeight, -nTileMiddle ), ioPosMapQuadTree::FT_NONE, nTileMax, 1, nTileMax );
	m_pBlockQuadTree->SetCollBoxRotaion( g_BlockManager.GetBlockRotation() );
}

void ioBlockMapBase::DestroyQuadTree()
{
	SAFEDELETE( m_pBlockQuadTree );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 블럭 엔티티 컨테이너 생성/파괴
//

void ioBlockMapBase::CreateLevelMapContainer( BlockModeType eType )
{
	DestroyLevelMapContainer();

	m_nHeightMax	= g_BlockManager.GetHeightMax();
	m_nTileMax		= g_BlockManager.GetTileMax();
	for( int i = 0; i < HEIGHT_MAX; ++i )
	{
		switch( eType )
		{
		case BMT_GUILD:
			m_PosLevelMap[i] = new ioPos64MapContainer<ioBlock*>;
			break;
		case BMT_PERSONAL:
			m_PosLevelMap[i] = new ioPos32MapContainer<ioBlock*>;
			break;
		}
	}
}

void ioBlockMapBase::DestroyLevelMapContainer()
{
	for( int i = 0; i < HEIGHT_MAX; ++i )
	{
		SAFEDELETE( m_PosLevelMap[i] );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 타일 블럭 엔티티 컨테이너 생성/파괴
//

void ioBlockMapBase::CreateTileMapContainer( BlockModeType eType )
{
	DestroyTileMapContainer();
	switch( eType )
	{
	case BMT_GUILD:
		m_pTileContainer = new ioPos64MapContainer<ioTileBlock*>;
		break;
	case BMT_PERSONAL:
		m_pTileContainer = new ioPos32MapContainer<ioTileBlock*>;
		break;
	}
}

void ioBlockMapBase::DestroyTileMapContainer()
{
	SAFEDELETE( m_pTileContainer );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 월드 타일 위치
//

void ioBlockMapBase::WorldCoordToTileCoord( const D3DXVECTOR3& vPos, TilePos& Pos )
{
	D3DXVECTOR3 vDir = -ioMath::UNIT_Y;
	if( vPos.y < g_BlockManager.GetBlockStartHeight() )
		vDir = ioMath::UNIT_Y;

	ioRay kYRay( vPos, vDir );
	if( m_pBlockQuadTree )
	{
		int nXPos = 0;
		int nZPos = 0;
		if( !m_pBlockQuadTree->FindBlockIndex( kYRay, nXPos, nZPos ) )
			return;

		Pos.x	= nXPos;
		Pos.y	= ( vPos.y - g_BlockManager.GetBlockStartHeight() ) / UD_SIZE;
		Pos.z	= nZPos;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 월드 엔티티 GET/SET
//

PosMapContainer* ioBlockMapBase::GetPosMapContainer( int nLevel )
{	
	if( COMPARE( nLevel, 0, m_nHeightMax ) )
	{
		return m_PosLevelMap[nLevel];
	}

	return NULL;
}

TileMapContainer* ioBlockMapBase::GetTileMapContainer()
{
	return m_pTileContainer;
}

ioBlock* ioBlockMapBase::GetBlockByWorldPosition( IN const D3DXVECTOR3& vPos )
{
	TilePos Pos;
	WorldCoordToTileCoord( vPos, Pos );

	ioBlock* pBlock = NULL;
	PosMapContainer* pContainer = GetPosMapContainer( Pos.y );
	if( pContainer )
		pBlock = pContainer->GetElement( Pos );

	if( pBlock == NULL )
	{
		TileMapContainer* pTileContainer = GetTileMapContainer();
		if( pTileContainer )
			pBlock = pTileContainer->GetElement( Pos );
	}

	return pBlock;
}

ioBlock* ioBlockMapBase::GetBlockByTilePos( IN const TilePos& Pos )
{
	ioBlock* pBlock = NULL;
	PosMapContainer* pContainer = GetPosMapContainer( Pos.y );
	if( pContainer )
		pBlock = pContainer->GetElement( Pos );
	
	if( pBlock == NULL && Pos.y == 1 )
	{
		TileMapContainer* pTileContainer = GetTileMapContainer();
		if( pTileContainer )
			pBlock = pTileContainer->GetElement( Pos );
	}

	return pBlock;
}

void ioBlockMapBase::GetBlockByTop( IN const D3DXVECTOR3& vPos, OUT ioBlockList& List )
{
	TilePos Pos;
	WorldCoordToTileCoord( vPos, Pos );

	for( int i = m_nHeightMax; i >= 0 ; --i )
	{
		PosMapContainer* pContainer = GetPosMapContainer( i );
		if( !pContainer )
			continue;

		ioBlock* pBlock = pContainer->GetElement( Pos );
		if( !pBlock )
			continue;

		List.push_back( pBlock );
	}
}

void ioBlockMapBase::GetAdjacentBlockList( IN const D3DXVECTOR3& vPos, IN float fRange, OUT BlockMapBuffer& List )
{
	TilePos Pos;
	WorldCoordToTileCoord( vPos, Pos );

	if( fRange < UD_SIZE )
		fRange = UD_SIZE;

	//상
	int nMax = m_nHeightMax;
	if( nMax > Pos.y )
	{
		int nHeightMax = min( nMax, Pos.y + fRange/UD_SIZE + 1 );
		GetAdjacentBlockList( Pos.y, nHeightMax, Pos, fRange, List );
	}

	//하
	if( 0 <= Pos.y )
	{
		GetAdjacentBlockList( max( 0, Pos.y - fRange/UD_SIZE ), Pos.y, Pos, fRange, List );
	}

	//150911 JSM 최적화, CPU 점유율이 생각보다 높아서 제거하고 로직을 변경
	/*
	std::stable_sort( List.begin(), List.end(), ioBlockListSort() );
	ioBlockList::iterator iter = std::unique( List.begin(), List.end() );
	List.erase( iter, List.end() );
	*/
}

void ioBlockMapBase::GetAdjacentBlockList( IN int nMin, IN int nMax, IN const TilePos& Pos, IN float fRange, OUT BlockMapBuffer& List )
{
	int Range = fRange/UD_SIZE;	
	int nIndex		= 0;
	
	for( int i = nMin; i < nMax; ++i )
	{
		PosMapContainer* pContainer = GetPosMapContainer( i );
		if( pContainer )
		{
			int nZStart = max( 0, Pos.z - Range );
			int nZEnd	= min( m_nTileMax-1, Pos.z + Range );
			int nXStart	= max( 0, Pos.x - Range );
			int nXEnd	= min( m_nTileMax-1, Pos.x + Range );

			for( int nZ = nZStart; nZ <= nZEnd; ++nZ )
			{
				for( int nX = nXStart; nX <= nXEnd; ++nX )
				{					
					ioBlock* pBlock = pContainer->GetElement( nX + nZ * m_nTileMax );
					if( pBlock )
						List.push_back( pBlock );
				}
			}
			
			//150911. JSM 최적화 : 인덱스 리스트를 계산하여 GetAdjacentIndexList 내부에서 
			//매 프레임마다 수십 수백개의 push_back()이 일어나면서 프레임 저하의 원인(메모리 재 할당 및 속도 문제)이 되어 함수를 제거
			/* 
			IntVec vVec;
			GetAdjacentIndexList( Pos.x, Pos.z, , vVec )
			for( int nPos = 0; nPos < (int)vVec.size(); ++nPos )
			{
				ioBlock* pBlock = pContainer->GetElement( vVec[nPos] );
				if( pBlock )
					List.push_back( pBlock );
			}
			*/
		}
	}
}

void ioBlockMapBase::GetAdjacentBlockListByTile( IN const TilePos& Pos, IN float fRange, OUT BlockMapBuffer& List )
{
	if( m_pTileContainer )
	{
		int Range = fRange/UD_SIZE;
		int nZStart = max( 0, Pos.z - Range );
		int nZEnd	= min( m_nTileMax-1, Pos.z + Range );
		int nXStart	= max( 0, Pos.x - Range );
		int nXEnd	= min( m_nTileMax-1, Pos.x + Range );

		for( int nZ = nZStart; nZ <= nZEnd; ++nZ )
		{
			for( int nX = nXStart; nX <= nXEnd; ++nX )
			{
				ioBlock* pBlock = m_pTileContainer->GetElement( nX + nZ * m_nTileMax );
				if( pBlock )
					List.push_back( pBlock );
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------

void ioBlockMapBase::FindBlockIndexList( const ioRay& kYRay, OUT ioBlockList& List )
{
	if( m_pBlockQuadTree )
	{
		IntVec IndexList;
		m_pBlockQuadTree->FindBlockIndexList( kYRay, IndexList );

		for( int i = 0; i < m_nHeightMax; ++i )
		{
			PosMapContainer* pContainer = GetPosMapContainer( i );
			if( !pContainer )
				continue;

			for( int nPos = 0; nPos < (int)IndexList.size(); ++nPos )
			{
				ioBlock* pBlock = pContainer->GetElement( IndexList[nPos] );
				if( !pBlock )
					continue;

				List.push_back( pBlock );
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// 오브젝트 렌더
//

void ioBlockMapBase::RenderPosQuadTree( ioPlayStage* pStage, ioCamera* pCamera )
{
	if( m_pBlockQuadTree )
		m_pBlockQuadTree->RenderCollision( pStage, pCamera );
}

void ioBlockMapBase::RenderBlockAABB( CollRenderType eType, ioPlayStage* pStage, ioCamera* pCamera )
{
	switch( eType )
	{
	case RENDER_BOX:
	case RENDER_SPHERE:
	case RENDER_INTERPLAY_BOX:
		{
			for( int i = 0; i < m_nHeightMax; ++i )
			{
				PosMapContainer* pContainer = GetPosMapContainer( i );
				if( !pContainer )
					continue;

				for( int nPos = 0; nPos < pContainer->GetMax(); ++nPos )
				{
					ioBlock* pBlock = pContainer->GetElement( nPos );
					if( !pBlock )
						continue;

					pBlock->RenderBlockAABB( eType, pStage, pCamera );
				}
			}
		}
		break;
	}
}