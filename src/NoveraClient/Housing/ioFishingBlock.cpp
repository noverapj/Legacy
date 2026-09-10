#include "stdafx.h"

#include "ioFishingBlock.h"

ioFishBlockProperty::ioFishBlockProperty() : ioBlockProperty()
{

}

ioFishBlockProperty::ioFishBlockProperty( const ioFishBlockProperty& rhs ) :
	ioBlockProperty( rhs )
{

}

ioFishBlockProperty::~ioFishBlockProperty()
{

}

bool ioFishBlockProperty::LoadBlockProperty( ioINILoader &rkLoader )
{
	if( !ioBlockProperty::LoadBlockProperty( rkLoader ) )
		return false;

	return true;
}

ioBlockProperty* ioFishBlockProperty::Clone()
{
	return new ioFishBlockProperty( *this );
}

//----------------------------------------------------------------------------------------------------------------------------------

ioFishingBlock::ioFishingBlock( ioEntityGroup *pGrp, ioPlayMode *pMode, ioBlockProperty* pProperty, __int64 nUniqueIndex ) :
ioBlock( pGrp, pMode, pProperty, nUniqueIndex )
{
}

ioFishingBlock::~ioFishingBlock()
{
}

ioBlock::BlockType ioFishingBlock::GetBlockType() const
{
	return ioBlock::BT_FISHING; 
}

bool ioFishingBlock::IsFishing( const TilePos& TargetPos )
{
	if( !m_Property )
		return false;

	TilePosList List;
	GetPosList( List );	

	for( int i = 0; i < (int)List.size(); ++i )
	{
		const TilePos& Pos = List[i];
		if( Pos.event == TilePos::TPE_FISH && Pos == TargetPos )
		{
			return true;
		}
	}

	return false;
}