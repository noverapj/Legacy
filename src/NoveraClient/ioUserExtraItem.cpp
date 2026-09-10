
#include "stdafx.h"

#include "ioUserExtraItem.h"

#include "ioBaseChar.h"
#include "ioPowerUpManager.h"

ioUserExtraItem::ioUserExtraItem()
{
	Initialize();
}

ioUserExtraItem::~ioUserExtraItem()
{
}

void ioUserExtraItem::Initialize()
{
	m_vExtraItemList.clear();
}

void ioUserExtraItem::ApplyExtraItemData( SP2Packet &rkPacket )
{
	//m_vExtraItemList.clear();

	int iSize;
	rkPacket >> iSize;

	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kItem;

		rkPacket >> kItem.m_iItemCode
				 >> kItem.m_iReinforce
				 >> kItem.m_iIndex
				 >> kItem.m_iTradeState
				 >> kItem.m_PeriodType
				 >> kItem.m_iValue1
				 >> kItem.m_iValue2
			     >> kItem.m_dwMaleCustom
				 >> kItem.m_dwFemaleCustom
				 >> kItem.m_failExp;

		const ioItem *pItem = g_ItemMaker.GetItemConst( kItem.m_iItemCode, __FUNCTION__ );
		if( pItem )
			kItem.m_Name = pItem->GetName();

		kItem.m_IconName = g_ItemMaker.GetItemLargeIcon( kItem.m_iItemCode, 0 );

		m_vExtraItemList.push_back( kItem );
	}

	std::sort( m_vExtraItemList.begin(), m_vExtraItemList.end(), ExtraItemSort() );
}

void ioUserExtraItem::AddExtraItemData( const EXTRAITEMSLOT &rkNewSlot )
{
	EXTRAITEMSLOT kItem;

	kItem = rkNewSlot;

	const ioItem *pItem = g_ItemMaker.GetItemConst( kItem.m_iItemCode, __FUNCTION__ );
	if( pItem )
		kItem.m_Name = pItem->GetName();

	kItem.m_IconName = g_ItemMaker.GetItemLargeIcon( kItem.m_iItemCode, 0 );

	m_vExtraItemList.push_back( kItem );
}

bool ioUserExtraItem::DeleteExtraItemData( int iSlotIndex )
{
	int iSize = m_vExtraItemList.size();

	for( int i=0; i < iSize; ++i )
	{
		if( m_vExtraItemList[i].m_iIndex == iSlotIndex )
		{
			m_vExtraItemList.erase( m_vExtraItemList.begin() + i );
			return true;
		}
	}

	return false;
}

int ioUserExtraItem::GetExtraItemCnt()
{
	int iSize = m_vExtraItemList.size();
	return iSize;
}

int ioUserExtraItem::GetExtraItemMaxSlotCnt()
{
	int nMaxSlotCnt = MAX_SLOT_CNT;

	ioUserEtcItem *pUserEtc = g_MyInfo.GetUserEtcItem();
	if ( pUserEtc )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtc->GetEtcItem( ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND, kSlot ) )
			nMaxSlotCnt += kSlot.GetUse();
	}

	return nMaxSlotCnt;
}

bool ioUserExtraItem::IsSlotFull()
{	
	if( GetExtraItemCnt() > GetExtraItemMaxSlotCnt() )
		return true;
	return false;
}

void ioUserExtraItem::SetExtraItem( const EXTRAITEMSLOT &rkExtraItem )
{
	int iSize = m_vExtraItemList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_vExtraItemList[i].m_iIndex == rkExtraItem.m_iIndex )
		{
			m_vExtraItemList[i] = rkExtraItem;
			return;
		}
	}
}

bool ioUserExtraItem::GetExtraItem( IN int iSlotIndex, OUT EXTRAITEMSLOT &rkExtraItem )
{
	//초기화
	rkExtraItem.Init();

	int iSize = m_vExtraItemList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_vExtraItemList[i].m_iIndex == iSlotIndex )
		{
			rkExtraItem = m_vExtraItemList[i];
			return true;
		}
	}

	return false;
}

bool ioUserExtraItem::GetExtraItemArray( IN int iArray, OUT EXTRAITEMSLOT &rkExtraItem )
{
	//초기화
	rkExtraItem.Init();

	if( COMPARE( iArray, 0, (int)m_vExtraItemList.size() ) )
	{
		rkExtraItem = m_vExtraItemList[iArray];
		return true;
	}

	return false;
}

bool ioUserExtraItem::SetCharEquip( int iSlotIndex, bool bEquip )
{
	int iSize = m_vExtraItemList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_vExtraItemList[i].m_iIndex == iSlotIndex )
		{
			m_vExtraItemList[i].m_bCharEquip = bEquip;
			return true;
		}
	}

	return false;
}

void ioUserExtraItem::ClearCharEquip()
{
	int iSize = m_vExtraItemList.size();
	for( int i=0; i < iSize; ++i )
	{
		m_vExtraItemList[i].m_bCharEquip = false;
	}
}

void ioUserExtraItem::GetUseInfo( IN int iSlotIndex, OUT ioHashString &rsInfo )
{
	EXTRAITEMSLOT kSlot;
	if( !GetExtraItem( iSlotIndex, kSlot ) )
		return;

	char szText[MAX_PATH]="";
	if( kSlot.m_PeriodType == EPT_MORTMAIN )
	{
		int iEquipSlotIndex = kSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
		if ( iEquipSlotIndex == ES_WEAPON )
		{
			int iPowerUpType = ConvertPowerUpItemToItemGrade( kSlot.m_iItemCode );
			if ( IsPowerUpItemGrade( iPowerUpType ) )
			{
				ioHashString szPowerUpName;
				szPowerUpName = g_PowerUpManager.GetItemGradeTitle( kSlot.m_iItemCode, iPowerUpType );
				SafeSprintf( szText, sizeof( szText ), STR(2), szPowerUpName.c_str() );
			}
			else
				SafeSprintf( szText, sizeof( szText ), STR(1) );	
		}
		else
		{
			int iPowerUpType = ConvertPowerUpRareItemToRareItemGrade( kSlot.m_iItemCode );
			if ( IsPowerUpRareItemGrade( iPowerUpType ) )
			{
				ioHashString szPowerUpName;
				szPowerUpName = g_PowerUpManager.GetExtraItemGradeTitle( kSlot.m_iItemCode, iPowerUpType );
				SafeSprintf( szText, sizeof( szText ), STR(2), szPowerUpName.c_str() );
			}
			else
				SafeSprintf( szText, sizeof( szText ), STR(1) );	
		}
	}
	else 
	{
		Help::GetRemainTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), szText, sizeof( szText ) );
	}

	rsInfo = szText;
}

bool ioUserExtraItem::HasTradeWaitItem()
{
	int iSize = m_vExtraItemList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_vExtraItemList[i].m_iTradeState == EET_NORMAL )
		{
			return true;
		}
	}

	return false;
}

void ioUserExtraItem::EquipItem( CHARACTERDATA &rkCharData, bool bSetEquip, bool bBulletInit )
{	
	for(int i = 0; i < MAX_INVENTORY; i++ )
	{
		//기본
		int iItemCode = 0;
		if( rkCharData.m_data.m_eCharPowerUpType != PUGT_NONE )
		{
			iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + ConvertPowerUpCharToItem( rkCharData.m_data.m_eCharPowerUpType ) + rkCharData.m_data.m_class_type;

			//아이템 생성시 검증
			if( !g_PowerUpManager.IsPowerUpGradeItem( rkCharData.m_data.m_class_type, iItemCode ) )
			{
				iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + rkCharData.m_data.m_class_type;
			}
		}
		else
		{
			iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + rkCharData.m_data.m_class_type;
		}
		rkCharData.m_equip_item[i].m_item_code = iItemCode;

		if( bBulletInit )
			rkCharData.m_equip_item[i].m_item_bullet	= -1;

		rkCharData.m_equip_item[i].m_item_male_custom	= 0;
		rkCharData.m_equip_item[i].m_item_female_custom	= 0;

		if( rkCharData.m_data.m_extra_item[i] > 0 )	// 장비아이템
		{
			EXTRAITEMSLOT kSlot;
			if( GetExtraItem( rkCharData.m_data.m_extra_item[i], kSlot ) )
			{
				rkCharData.m_equip_item[i].m_item_code = kSlot.m_iItemCode;
				rkCharData.m_equip_item[i].m_item_reinforce = kSlot.m_iReinforce;
				if( bBulletInit )
					rkCharData.m_equip_item[i].m_item_bullet = -1;
				rkCharData.m_equip_item[i].m_item_male_custom = kSlot.m_dwMaleCustom;
				rkCharData.m_equip_item[i].m_item_female_custom = kSlot.m_dwFemaleCustom;

				if( bSetEquip )
					SetCharEquip( rkCharData.m_data.m_extra_item[i], true );
			}
		}
	}
}

void ioUserExtraItem::FindAllItem( IN int iItemCode, OUT ExtraItemList& rkExtraItemList )
{
	rkExtraItemList.clear();
	for( int i = 0; i < (int)m_vExtraItemList.size(); ++i )
	{
		if( m_vExtraItemList[i].m_iItemCode == iItemCode )
		{
			rkExtraItemList.push_back( m_vExtraItemList[i] );
		}
	}
}

bool ioUserExtraItem::GetExtraItemByItemCode( IN int ItemCode, OUT EXTRAITEMSLOT &rkExtraItem )
{
	//초기화
	rkExtraItem.Init();

	int iSize = m_vExtraItemList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_vExtraItemList[i].m_iItemCode == ItemCode )
		{
			rkExtraItem = m_vExtraItemList[i];
			return true;
		}
	}

	return false;
}