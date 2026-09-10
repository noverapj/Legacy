#pragma once

#include "ioBlock.h" 
#include "BlockDefine.h"

class ioPlayMode;
class ioINILoader;
class ioHouseManager;

class ioFishBlockProperty : public ioBlockProperty
{

public:
	virtual bool LoadBlockProperty( ioINILoader &rkLoader );

public:
	virtual ioBlockProperty* Clone();

public:
	ioFishBlockProperty();
	ioFishBlockProperty( const ioFishBlockProperty& rhs );
	~ioFishBlockProperty();
};

class ioFishingBlock : public ioBlock
{
friend class ioHouseManager;

public:
	virtual ioBlock::BlockType GetBlockType() const;

public:
	bool IsFishing( const TilePos& TargetPos );

public:
	ioFishingBlock( ioEntityGroup *pGrp, ioPlayMode *pMode, ioBlockProperty* pProperty, __int64 nUniqueIndex );
	virtual ~ioFishingBlock();
};

inline ioFishingBlock* ToFishingBlock( ioBlock *pBlock )
{
	if( !pBlock || pBlock->GetBlockType() != ioBlock::BT_FISHING )
		return NULL;

	return dynamic_cast< ioFishingBlock* >( pBlock );
}

inline const ioFishingBlock* ToFishingBlockConst( ioBlock *pBlock )
{
	if( !pBlock || pBlock->GetBlockType() != ioBlock::BT_FISHING )
		return NULL;

	return dynamic_cast< const ioFishingBlock* >( pBlock );
}