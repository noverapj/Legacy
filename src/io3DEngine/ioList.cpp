

#include "stdafx.h"

#include "ioWndType.h"
#include "ioWnd.h"
#include "ioScroll.h"
#include "ioList.h"
#include "ioFontManager.h"

ioListItem::ioListItem()
{
}

ioListItem::~ioListItem()
{

}

ioTextItem::ioTextItem()
{
}

ioTextItem::~ioTextItem()
{
}

void ioTextItem::SetText( const char *szText )
{
	m_szText = szText;
}

void ioTextItem::SetText( const ioHashString &szText )
{
	m_szText = szText;
}

void ioTextItem::Render( int iXPos, int iYPos )
{
}


//-----------------------------------------------------------------------


ioList::ioList()
{
}

ioList::~ioList()
{
	ClearAllItems();
}

void ioList::AddItem( ioListItem *pItem )
{
	if( pItem )
	{
		m_ItemList.push_back( pItem );
	}
}

void ioList::DeleteItem( ioListItem *pItem )
{
	if( pItem )
	{
		m_ItemList.remove( pItem );
		SAFEDELETE( pItem );
	}
}

void ioList::DeleteItem( int iIdx )
{
	ioListItem *pItem = GetItem( iIdx );
	if( pItem )
	{
		DeleteItem( pItem );
	}
}

void ioList::ClearAllItems()
{
	ioItemList::iterator iter;
	for( iter = m_ItemList.begin() ; iter!=m_ItemList.end() ; ++iter )
	{
		SAFEDELETE( *iter );
	}

	m_ItemList.clear();
}

int ioList::GetItemCount() const
{
	return m_ItemList.size();
}

ioListItem* ioList::GetItem( int iIdx )
{
	if( COMPARE( iIdx, 0, GetItemCount() ) )
	{
		ioItemList::iterator iter = m_ItemList.begin();
		std::advance( iter, iIdx );
		return *iter;
	}

	return NULL;
}

void ioList::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioItemList::iterator iter;
	for( iter=m_ItemList.begin() ; iter!=m_ItemList.end() ; ++iter )
	{
		(*iter)->Render( iXPos, iYPos );
	}
}

DWORD ioList::GetWndType() const
{
	return IWT_LIST;
}
