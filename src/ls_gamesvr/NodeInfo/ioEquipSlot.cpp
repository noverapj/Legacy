

#include "stdafx.h"

#include "Item.h"
#include "ioEquipSlot.h"
#include "../EtcHelpFunc.h"

ioEquipSlot::ioEquipSlot()
{
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		m_EquipSlot[i] = NULL;
	}
}

ioEquipSlot::~ioEquipSlot()
{
	ClearSlot();
}

void ioEquipSlot::ClearSlot()
{
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		SAFEDELETE( m_EquipSlot[i] );
	}
}

ioItem* ioEquipSlot::EquipItem( int iSlot, ioItem *pItem )
{
	ioItem *pPreItem = NULL;
	if( COMPARE( iSlot, 0, MAX_EQUIP_SLOT ) )
	{
		pPreItem = m_EquipSlot[iSlot];
		m_EquipSlot[iSlot] = pItem;
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioEquipSlot::EquipItem - %d Slot Overflow", iSlot );
		SAFEDELETE( pItem );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::EquipItem( ioItem *pItem )
{
	int iEquipSlot = Help::GetEquipSlot( pItem->GetItemCode() );

	return EquipItem( iEquipSlot, pItem );
}

ioItem* ioEquipSlot::ReleaseItem( int iSlot )
{
	ioItem *pPreItem = NULL;
	if( COMPARE( iSlot, 0, MAX_EQUIP_SLOT ) )
	{
		pPreItem = m_EquipSlot[iSlot];
		m_EquipSlot[iSlot] = NULL;
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioEquipSlot::ReleaseItem - %d Slot Overflow", iSlot );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseItem( int iGameIndex, int iItemCode )
{
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		ioItem *pItem = m_EquipSlot[i];
		if( pItem && pItem->IsRightItem( iGameIndex, iItemCode ) )
		{
			m_EquipSlot[i] = NULL;
			return pItem;
		}
	}

	return NULL;
}

void ioEquipSlot::ClearOwnerName( const ioHashString &rkOwnerName )
{
	ioItem *pItem = NULL;
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		pItem = m_EquipSlot[i];
		if( pItem )
		{
			pItem->ClearOwnerNameIf( rkOwnerName );
		}
	}
}

const ioItem* ioEquipSlot::GetItem( int iSlot ) const
{
	if( COMPARE( iSlot, 0, MAX_EQUIP_SLOT ) )
		return m_EquipSlot[iSlot];

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioEquipSlot::GetItem - %d Slot Overflow", iSlot );
	return NULL;
}

const ioItem* ioEquipSlot::GetItemByGameIndex( int iGameIndex ) const
{
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		if( m_EquipSlot[i] )
		{
			if( m_EquipSlot[i]->GetGameIndex() == iGameIndex )
				return m_EquipSlot[i];
		}
	}

	return NULL;
}

bool ioEquipSlot::IsSlotEquiped( int iSlot ) const
{
	if( COMPARE( iSlot, 0, MAX_EQUIP_SLOT ) )
	{
		if( m_EquipSlot[iSlot] )
			return true;

		return false;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioEquipSlot::IsSlotEquiped - %d Slot Overflow", iSlot );
	return false;
}

bool ioEquipSlot::IsEquipedItem( int iGameIndex ) const
{
	if( GetItemByGameIndex( iGameIndex ) )
		return true;

	return false;
}

void ioEquipSlot::FillEquipItemInfo( SP2Packet &rkPacket )
{
	ioItem *pCurItem = NULL;
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		pCurItem = m_EquipSlot[i];
		if( pCurItem )
		{
			PACKET_GUARD_VOID( rkPacket.Write(pCurItem->GetItemCode()) );
			PACKET_GUARD_VOID( rkPacket.Write(pCurItem->GetItemReinforce()) );
			PACKET_GUARD_VOID( rkPacket.Write(pCurItem->GetItemMaleCustom()) );
			PACKET_GUARD_VOID( rkPacket.Write(pCurItem->GetItemFemaleCustom()) );
			PACKET_GUARD_VOID( rkPacket.Write(pCurItem->GetGameIndex()) );
			PACKET_GUARD_VOID( rkPacket.Write(pCurItem->GetOwnerName()) );
		}
		else
		{
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			PACKET_GUARD_VOID( rkPacket.Write("") );
		}
	}
}
