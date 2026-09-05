// Item.cpp: implementation of the ioItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "Item.h"
#include "ioItemInfoManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ioItem::ioItem( ioItemMaker *pCreator )
{
	m_iGameIndex = 0;
	m_dwDropTime = 0;
	m_fItemCurGauge = 0.0f;
	m_bNotDeleteItem = false;

	m_iCrownItemType = 0;
	m_iItemTeamType	 = 0;

	m_pCreator = pCreator;
}

ioItem::~ioItem()
{
	m_pCreator->NotifyItemDestroyed( GetItemCode() );
}

void ioItem::SetItemData( const ITEM_DATA &rkData )
{
	m_ItemData = rkData;
}

void ioItem::SetItemData( int iCode, int iReinforce, DWORD dwMaleCustom, DWORD dwFemaleCustom )
{
	m_ItemData.m_item_code   = iCode;
	m_ItemData.m_item_reinforce = iReinforce;	
	m_ItemData.m_item_male_custom = dwMaleCustom;
	m_ItemData.m_item_female_custom = dwFemaleCustom;
}

void ioItem::SetItemCode( int iItemCode )
{
	m_ItemData.m_item_code = iItemCode;
}

void ioItem::SetGameIndex( int iIndex )
{
	m_iGameIndex = iIndex;
}

void ioItem::SetItemName( const ioHashString &rkName )
{
	m_ItemName = rkName;
}

void ioItem::SetOwnerName( const ioHashString &rkName )
{
	m_OwnerName = rkName;
}

void ioItem::SetItemPos( const Vector3 &vPos )
{
	m_ItemPos = vPos;
}

bool ioItem::HasOwner() const
{
	return !m_OwnerName.IsEmpty();
}

void ioItem::ClearOwnerNameIf( const ioHashString &rkName )
{
	CRASH_GUARD();
	if( m_OwnerName == rkName )
	{
		m_OwnerName.Clear();
	}
}

bool ioItem::IsRightItem( int iGameIndex, int iItemCode ) const
{
	if( iGameIndex == MAX_INT_VALUE )
	{
		// 아직 동기화되지 않은 아이템은 아이템 코드만 확인
		if( m_ItemData.m_item_code == iItemCode )
			return true;
	}
	else if( m_iGameIndex == iGameIndex && m_ItemData.m_item_code == iItemCode )
	{
		return true;
	}

	return false;
}

void ioItem::FillFieldItemInfo( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write(m_ItemData.m_item_code) );
	PACKET_GUARD_VOID( rkPacket.Write(m_ItemData.m_item_reinforce) );
	PACKET_GUARD_VOID( rkPacket.Write(m_ItemData.m_item_male_custom) );
	PACKET_GUARD_VOID( rkPacket.Write(m_ItemData.m_item_female_custom) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iGameIndex) );
	PACKET_GUARD_VOID( rkPacket.Write(m_OwnerName) );
	PACKET_GUARD_VOID( rkPacket.Write(m_ItemPos) );
	PACKET_GUARD_VOID( rkPacket.Write(m_fItemCurGauge) );

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// FillFieldItemInfo
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:FillFieldItemInfo] m_ItemData.m_item_code[%d]m_iGameIndex[%d]m_OwnerName[%S]m_ItemPos[%f,%f,%f]"\
		,m_ItemData.m_item_code,	m_iGameIndex,m_OwnerName.c_str(),m_ItemPos.x,m_ItemPos.y,m_ItemPos.z);
#endif // FLAG_MODE_BY_BCKIM_DEBUG


}

void ioItem::SetDropTime( DWORD dwDropTime )
{
	m_dwDropTime = dwDropTime;
}

void ioItem::SetCurItemGauge( float fGauge )
{
	m_fItemCurGauge = fGauge;
}

void ioItem::SetCrown( int iCrownItemType, int iItemTeamType )
{
	m_iCrownItemType = iCrownItemType;
	m_iItemTeamType  = iItemTeamType;
}