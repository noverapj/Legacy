#include "stdafx.h"

#include "ioTileBlock.h"
#include "../ioPlayStage.h"

ioTileBlock::ioTileBlock( ioEntityGroup *pGrp, ioPlayMode *pMode, ioBlockProperty* pProperty, __int64 nUniqueIndex ) :
	ioBlock( pGrp, pMode, pProperty, nUniqueIndex )
{
}

ioTileBlock::~ioTileBlock()
{
}

ioBlock::BlockType ioTileBlock::GetBlockType() const
{
	return ioBlock::BT_TILE; 
}

bool ioTileBlock::IsSetUp() const
{	
	//타일 블럭은 드랍존에 설치 불가
	if( m_pModeParent && m_pModeParent->GetCreator() )
	{
		D3DXVECTOR3 vPos;
		TileCoordToWorldCoord( m_Pos.x, m_Pos.y, m_Pos.z, vPos.x, vPos.y, vPos.z );
		float fHeight = m_pModeParent->GetCreator()->GetMapHeight( vPos.x, vPos.z, NULL, true, 0.0f );
		if( fHeight <= 0.0f )
			return false;
	}

	return ioBlock::IsSetUp();
}