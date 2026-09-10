
#include "stdafx.h"

#include "ioInventory.h"

#include "ioDecorationPrice.h"

ioInventory::ioInventory()
{
	Initialize();
}

ioInventory::~ioInventory()
{
	
}

void ioInventory::Initialize()
{
	m_vItemList.clear();
}

void ioInventory::ApplyInventorySlotData( SP2Packet &rkPacket )
{
	int iSize;
	rkPacket >> iSize;
	for(int i = 0;i < iSize;i++)
	{
		ITEMDB kItemDB;
		for(int j = 0;j < MAX_SLOT;j++)
		{
			rkPacket >> kItemDB.m_Slot[j].m_item_type >> kItemDB.m_Slot[j].m_item_code >> kItemDB.m_Slot[j].m_item_equip;
		}
		m_vItemList.push_back( kItemDB );
	}
}

bool ioInventory::IsSlotItem( int iType, int iCode )
{
	if( m_vItemList.empty() ) 
		return false;

	
	if( iType%1000 == UID_KINDRED )
	{
		// 종족 치장
		// 성별을 구분하지 않는다.
		if( iCode == RDT_HUMAN_MAN ) return true;        //인간 남자는 기본

		vITEMDB::iterator iter;
		for(iter = m_vItemList.begin();iter != m_vItemList.end();iter++)
		{
			ITEMDB &kItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( ( kItemDB.m_Slot[i].m_item_type / 100000 ) != ( iType / 100000 ) ) continue;
				if( kItemDB.m_Slot[i].m_item_type % 1000 != UID_KINDRED ) continue;

				if( kItemDB.m_Slot[i].m_item_code == iCode )
					return true;
			}		
		}
	}
	else
	{
		// 일반 치장은 클래스 + 성별 + 치장타입이 동일해야함
		vITEMDB::iterator iter;
		for(iter = m_vItemList.begin();iter != m_vItemList.end();iter++)
		{
			ITEMDB &kItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kItemDB.m_Slot[i].m_item_type == iType &&
					kItemDB.m_Slot[i].m_item_code == iCode )
				{
					return true;
				}
			}		
		}
	}
	return false;
}

void ioInventory::AddSlotItem( int iType, int iCode )
{
	vITEMDB::iterator iter;
	for(iter = m_vItemList.begin();iter != m_vItemList.end();iter++)
	{
		ITEMDB &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Slot[i].m_item_type == 0 &&
				kItemDB.m_Slot[i].m_item_code == 0 )
			{
				kItemDB.m_Slot[i].m_item_type = iType;
				kItemDB.m_Slot[i].m_item_code = iCode;
				return;
			}
		}		
	}

	// New
	ITEMDB kItemDB;
	kItemDB.m_Slot[0].m_item_type = iType;
	kItemDB.m_Slot[0].m_item_code = iCode;
	m_vItemList.push_back( kItemDB );
}

bool ioInventory::RemoveSlotItem( int iType, int iCode )
{
	if( iType%1000 == UID_KINDRED )
		return false;

	vITEMDB::iterator iter;
	for(iter = m_vItemList.begin();iter != m_vItemList.end();iter++)
	{
		ITEMDB &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Slot[i].m_item_type == iType &&
				kItemDB.m_Slot[i].m_item_code == iCode )
			{
				kItemDB.m_Slot[i].m_item_type = 0;
				kItemDB.m_Slot[i].m_item_code = 0;
				return true;
			}
		}		
	}
	return false;
}

void ioInventory::SetEquipItem( const int iItemType, const int iItemCode )
{
	vITEMDB::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		ITEMDB &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			// 종족 치장 
			if( iItemType % 1000 == UID_KINDRED )
			{
				if( iItemType / 100000 != kItemDB.m_Slot[i].m_item_type / 100000 ) continue;
				if( UID_KINDRED != kItemDB.m_Slot[i].m_item_type % 1000 ) continue;

				if( kItemDB.m_Slot[i].m_item_code == iItemCode)
				{
					kItemDB.m_Slot[i].m_item_equip = 1;
				}
				else
				{
					kItemDB.m_Slot[i].m_item_equip = 0;
				}
			}
			else      //일반 치장
			{
				if( kItemDB.m_Slot[i].m_item_type == iItemType && kItemDB.m_Slot[i].m_item_code == iItemCode)
				{
					kItemDB.m_Slot[i].m_item_equip = 1;
				}
				else if( kItemDB.m_Slot[i].m_item_type == iItemType )
				{
					kItemDB.m_Slot[i].m_item_equip = 0;
				}
			}
		}		
	}
}

RaceDetailType ioInventory::GetEquipRaceType( const int iClassType )
{
	vITEMDB::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		ITEMDB &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Slot[i].m_item_equip == 0 ) continue;
			if( kItemDB.m_Slot[i].m_item_type / 100000 != iClassType ) continue;
			if( kItemDB.m_Slot[i].m_item_type % 1000 != UID_KINDRED ) continue;

			return (RaceDetailType)kItemDB.m_Slot[i].m_item_code;
		}		
	}
	return RDT_HUMAN_MAN;        // 인간 남자는 인벤토리에 없으므로 기본값이다.
}

void ioInventory::GetEquipItemCode( CHARACTER &charInfo )
{
	RaceDetailType eRaceType = GetEquipRaceType( charInfo.m_class_type );
	switch( eRaceType )
	{
	case RDT_HUMAN_MAN:
		charInfo.m_kindred = 1;
		charInfo.m_sex = 1;
		break;
	case RDT_HUMAN_WOMAN:
		charInfo.m_kindred = 1;
		charInfo.m_sex = 2;
		break;
	case RDT_ELF_MAN:
		charInfo.m_kindred = 2;
		charInfo.m_sex = 1;
		break;
	case RDT_ELF_WOMAN:
		charInfo.m_kindred = 2;
		charInfo.m_sex = 2;
		break;
	case RDT_DWARF_MAN:
		charInfo.m_kindred = 3;
		charInfo.m_sex = 1;
		break;
	case RDT_DWARF_WOMAN:
		charInfo.m_kindred = 3;
		charInfo.m_sex = 2;
		break;
	}

	// 얼굴       UID_FACE
	charInfo.m_face = GetEquipItemCode( ( charInfo.m_class_type * 100000 ) + ( ( charInfo.m_sex - 1) * 1000 ) + UID_FACE );
	// 머리       UID_HAIR  
	charInfo.m_hair = GetEquipItemCode( ( charInfo.m_class_type * 100000 ) + ( ( charInfo.m_sex - 1) * 1000 ) + UID_HAIR );
	// 피부색     UID_SKIN_COLOR
	charInfo.m_skin_color = GetEquipItemCode( ( charInfo.m_class_type * 100000 ) + ( ( charInfo.m_sex - 1) * 1000 ) + UID_SKIN_COLOR );
	// 머리색     UID_HAIR_COLOR
	charInfo.m_hair_color = GetEquipItemCode( ( charInfo.m_class_type * 100000 ) + ( ( charInfo.m_sex - 1) * 1000 ) + UID_HAIR_COLOR );
	// 속옷       UID_UNDERWEAR
	charInfo.m_underwear = GetEquipItemCode( ( charInfo.m_class_type * 100000 ) + ( ( charInfo.m_sex - 1) * 1000 ) + UID_UNDERWEAR );
}

int ioInventory::GetEquipItemCode( const int iItemType )
{
	vITEMDB::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		ITEMDB &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Slot[i].m_item_type  == iItemType &&
				kItemDB.m_Slot[i].m_item_equip ==  1 ) // 장착 
			{
				return ( kItemDB.m_Slot[i].m_item_code );				
			}
		}		
	}

	return -1;
}

bool ioInventory::IsClassTypeInventory( int iClassType )
{
	// 해당 용병의 치장 아이템이 있으면 true. : 이것은 남자 치장이 슬롯에 없으므로 최초 1회라도 구입하지 않은 용병을 알수있다.
	vITEMDB::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		ITEMDB &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			int iItemClass = kItemDB.m_Slot[i].m_item_type / 100000;
			if( iItemClass == iClassType )
				return true;
		}		
	}
	return false;
}

bool ioInventory::IsClassTypeInventory( int iClassType, int iSexType, int iDecoType )
{
	// 해당 용병의 치장 아이템이 있으면 true. : 이것은 남자 치장이 슬롯에 없으므로 최초 1회라도 구입하지 않은 용병을 알수있다.
	int iItemType = ( iClassType * 100000 ) + ( iSexType * 1000 ) + iDecoType;

	vITEMDB::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		ITEMDB &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Slot[i].m_item_type == iItemType )
				return true;
		}		
	}
	return false;
}

int  ioInventory::GetInventoryItemCnt()
{
	int iItemCnt = 0;
	vITEMDB::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		ITEMDB &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Slot[i].m_item_type == 0 ) continue;

			iItemCnt++;
		}		
	}

	return iItemCnt;
}

bool ioInventory::IsInventoryItemFull()
{
	if( GetInventoryItemCnt() > MAX_SLOT_CNT )
		return true;
	return false;
}