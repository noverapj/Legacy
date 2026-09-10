#pragma once

#include "AccessoryEnumType.h"

class ioUserAccessory
{
public:
	enum
	{
		MAX_SLOT_CNT = 500,
	};
protected:
	AccessorySlotList m_AccessorySlotList;
public:
	//µ¿±âÈ­
	void ApplyAccessorySlotData( SP2Packet &rkPacket );

public:
	//½½·Ô Ãß°¡
	void AddAccessorySlotData( const AccessorySlot &sNewSlot );
	//½½·Ô »èÁ¦
	bool DeleteAccessorySlotData( int iSlotIndex );
	//½½·Ô º¯°æ
	void SetAccessorySlot( const AccessorySlot &sAccessorySlot );
	void SetAccessoryCompoundInfo( int iSlotIndex, int iCompoundCode, int iCompoundValue );
	void SetAccessoryReinforceInfo( int iAccessoryIndex, int iRandomValue, int iValue1, int iValue2 );

public:
	//ÀåÂø ¼³Á¤
	void SetAccessoryEquip( int iSlotIndex, bool bEquip );

public:
	//½½·Ô ¾ò±â(½½·Ô ÀÎµ¦½º)
	bool GetAccessorySlot( int iSlotIndex, AccessorySlot &sAccessorySlot );
	//½½·Ô ¾ò±â(½½·Ô ¹è¿­)
	bool GetAccessorySlotArray( int iArray, AccessorySlot &sAccessorySlot );
	//½½·Ô °³¼ö ¾ò±â
	int GetAccessorySlotCnt();
	//½½·Ô »óÅÂ È®ÀÎ
	bool IsSlotFull();

	int GetAccessoryCode( int iSlotIndex );
	bool GetUseInfo( int iSlotIndex, ioHashString &rsInfo );
public:
	ioUserAccessory(void);
	~ioUserAccessory(void);
};

