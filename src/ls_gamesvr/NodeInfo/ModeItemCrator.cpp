#include "stdafx.h"
#include "ModeItemCrator.h"

ModeItem* ModeItemCreator::CreateModeItemTemplate( ModeItemType eType )
{
	ModeItem* pItem = NULL;
	switch( eType )
	{
	case MIT_SHUFFLE_STAR:
	case MIT_HUNTER_COIN:
		pItem = new ModeItem;
		break;
	case MIT_BUFF:
		pItem = new BuffModeItem;
		break;
	default:
		LOG.PrintTimeAndLog( 0, "%s - not unknown mode item-type", __FUNCTION__  );
		break;
	}

	if( pItem )
		pItem->SetType( eType );

	return pItem;
}

//-------------------------------------------------------------------------------

ModeItem::ModeItem()
{
	m_dwModeItemIdx = 0;
	m_eType = MIT_NONE;	
}

ModeItem::~ModeItem()
{
}

bool ModeItem::IsValid() const
{
	if( m_dwModeItemIdx == 0 )
		return false;

	return true;
}

void ModeItem::SetType( ModeItemType eType )
{
	m_eType = eType;
}

ModeItemType ModeItem::GetType() const
{
	return m_eType;
}

//-------------------------------------------------------------------------------

BuffModeItem::BuffModeItem()
{
	m_fXPos  = 0.0f;
	m_fZPos  = 0.0f;
}

BuffModeItem::~BuffModeItem()
{
}