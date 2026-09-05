#pragma once

#include "ioBlock.h" 
#include "BlockDefine.h"

class ioPlayMode;
class ioINILoader;
class ioHouseManager;

class ioTileBlock : public ioBlock
{
friend class ioHouseManager;

public:
	virtual ioBlock::BlockType GetBlockType() const;
	virtual bool IsSetUp() const;

public:
	ioTileBlock( ioEntityGroup *pGrp, ioPlayMode *pMode, ioBlockProperty* pProperty, __int64 nUniqueIndex );
	virtual ~ioTileBlock();
};

inline ioTileBlock* ToTileBlock( ioBlock *pBlock )
{
	if( !pBlock || pBlock->GetBlockType() != ioBlock::BT_TILE )
		return NULL;

	return dynamic_cast< ioTileBlock* >( pBlock );
}

inline const ioTileBlock* ToTileBlockConst( ioBlock *pBlock )
{
	if( !pBlock || pBlock->GetBlockType() != ioBlock::BT_TILE )
		return NULL;

	return dynamic_cast< const ioTileBlock* >( pBlock );
}