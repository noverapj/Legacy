

#ifndef _ioEquipSlot_h_
#define _ioEquipSlot_h_

class SP2Packet;
class ioItem;

class ioEquipSlot
{
protected:
	ioItem* m_EquipSlot[MAX_EQUIP_SLOT];

public:
	void ClearSlot();

public:
	ioItem* EquipItem( int iSlot, ioItem *pItem );
	ioItem* EquipItem( ioItem *pItem );
	
	ioItem* ReleaseItem( int iSlot );
	ioItem* ReleaseItem( int iGameIndex, int iItemCode );

	const ioItem* GetItem( int iSlot ) const;
	const ioItem* GetItemByGameIndex( int iGameIndex ) const;
	
public:
	void ClearOwnerName( const ioHashString &rkOwnerName );

public:
	bool IsSlotEquiped( int iSlot ) const;
	bool IsEquipedItem( int iGameIndex ) const;

	void FillEquipItemInfo( SP2Packet &rkPacket );

public:
	ioEquipSlot();
	virtual ~ioEquipSlot();
};

#endif
