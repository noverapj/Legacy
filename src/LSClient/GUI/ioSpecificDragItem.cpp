

#include "StdAfx.h"


#include "../ioMyInfo.h"
#include "ioSpecificDragItem.h"

ioInvenDragItem::ioInvenDragItem( DWORD dwType ) : m_dwType( dwType )
{
	m_iItemIdx = 0;
	m_dwItemCode = 0;

	m_ptIconOffset.x = 0;
	m_ptIconOffset.y = 0;
}

ioInvenDragItem::~ioInvenDragItem()
{
}

void ioInvenDragItem::SetItemData( int iItemDBIndex, DWORD dwItemCode )
{
	m_iItemIdx = iItemDBIndex;
	m_dwItemCode = dwItemCode;
}

void ioInvenDragItem::SetIconOffset( int iXPos, int iYPos )
{
	m_ptIconOffset.x = iXPos;
	m_ptIconOffset.y = iYPos;
}

int ioInvenDragItem::GetTypeID() const
{
	return m_dwType;
}

const ioHashString& ioInvenDragItem::GetTypeName() const
{
	static ioHashString sName( "InvenDragItem" );

	return sName;
}
//////////////////////////////////////////////////////////////////////////
ioSoldierDragItem::ioSoldierDragItem( DWORD dwType ) : m_dwType( dwType )
{
	m_dwIndex = 0;
}

ioSoldierDragItem::~ioSoldierDragItem()
{
}

void ioSoldierDragItem::SetSoldierInfo( DWORD dwIndex )
{
	int iCharArray = g_MyInfo.GetCharArray( dwIndex );
	if( iCharArray == -1 )
		return;

	m_dwIndex = dwIndex;
	m_charInfo = g_MyInfo.GetCharacter( iCharArray );
	for(int i = 0;i < MAX_INVENTORY;i++)
		m_equipInfo[i] = g_MyInfo.GetCharItem( iCharArray, i );
}

const CHARACTER &ioSoldierDragItem::GetCharInfo()
{
	return m_charInfo;
}

const ITEM_DATA &ioSoldierDragItem::GetEquipInfo( int iArray )
{
	return m_equipInfo[iArray];
}

int ioSoldierDragItem::GetTypeID() const
{
	return m_dwType;
}

const ioHashString &ioSoldierDragItem::GetTypeName() const
{
	static ioHashString sName( "SoldierDragItem" );

	return sName;
}
//////////////////////////////////////////////////////////////////////////
ioShopSoldierDragItem::ioShopSoldierDragItem( DWORD dwType ) : m_dwType( dwType )
{
	m_iSoldierArray = -1;
	m_ptIconOffset.x = 0;
	m_ptIconOffset.y = 0;
}

ioShopSoldierDragItem::~ioShopSoldierDragItem()
{
}

void ioShopSoldierDragItem::SetSoldierArray( int iSoldierArray )
{
	m_iSoldierArray = iSoldierArray;
}

int ioShopSoldierDragItem::GetTypeID() const
{
	return m_dwType;
}

const ioHashString & ioShopSoldierDragItem::GetTypeName() const
{
	static ioHashString sName( "ShopSoldierDragItem" );
	return sName;
}

void ioShopSoldierDragItem::SetIconOffset( int iXPos, int iYPos )
{
	m_ptIconOffset.x = iXPos;
	m_ptIconOffset.y = iYPos;
}
//////////////////////////////////////////////////////////////////////////
ioUnAllocateButtonDragItem::ioUnAllocateButtonDragItem( DWORD dwType ) : m_dwType( dwType )
{
	m_pRenderFrame    = NULL;
	m_pCustomTeamData = NULL;
}

ioUnAllocateButtonDragItem::~ioUnAllocateButtonDragItem()
{
}

int ioUnAllocateButtonDragItem::GetTypeID() const
{
	return m_dwType;
}

const ioHashString & ioUnAllocateButtonDragItem::GetTypeName() const
{
	static ioHashString sName( "ioUnAllocateButtonDragItem" );
	return sName;
}
//////////////////////////////////////////////////////////////////////////
ioAllocatedButtonDragItem::ioAllocatedButtonDragItem( DWORD dwType ) : m_dwType( dwType )
{
	m_pRenderFrame    = NULL;
	m_pCustomTeamData = NULL;

	m_bClickRemove = false;
}

ioAllocatedButtonDragItem::~ioAllocatedButtonDragItem()
{
}

int ioAllocatedButtonDragItem::GetTypeID() const
{
	return m_dwType;
}

const ioHashString & ioAllocatedButtonDragItem::GetTypeName() const
{
	static ioHashString sName( "ioAllocatedButtonDragItem" );
	return sName;
}