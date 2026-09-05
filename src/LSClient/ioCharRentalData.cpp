
#include "stdafx.h"

#include "ioCharRentalData.h"
#include "ioPowerUpManager.h"

ioCharRentalData::ioCharRentalData()
{
}

ioCharRentalData::~ioCharRentalData()
{
}

void ioCharRentalData::Initialize()
{
	m_RentalDataList.clear();
}

const ioHashString &ioCharRentalData::GetOwnerName( const DWORD dwCharIndex )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == dwCharIndex )
		{
			return rkRentalData.m_szOwnerID;
		}
	}

	static ioHashString kReturn;
	return kReturn;
}

int ioCharRentalData::GetClassLevel( const DWORD dwCharIndex )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == dwCharIndex )
		{
			return rkRentalData.m_iClassLevel;
		}
	}
	return 0;
}

void ioCharRentalData::GetEquipItem( const DWORD dwCharIndex, ITEM_DATA &rkEquipItem, int iSlot )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == dwCharIndex )
		{
			if( COMPARE( iSlot, 0, MAX_INVENTORY ) )
				rkEquipItem = rkRentalData.m_EquipItem[iSlot];
			break;
		}
	}
}

bool ioCharRentalData::GetEquipExtraItem( const DWORD dwCharIndex, EXTRAITEMSLOT &rkExtraItem, int iSlot )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == dwCharIndex )
		{
			if( COMPARE( iSlot, 0, MAX_INVENTORY ) )
			{
				if( rkRentalData.m_EquipItem[iSlot].m_item_code > 0 )
				{
					rkExtraItem.m_iIndex		 = iSlot;
					rkExtraItem.m_iItemCode		 = rkRentalData.m_EquipItem[iSlot].m_item_code;
					rkExtraItem.m_iReinforce	 = rkRentalData.m_EquipItem[iSlot].m_item_reinforce;
					rkExtraItem.m_dwMaleCustom	 = rkRentalData.m_EquipItem[iSlot].m_item_male_custom;
					rkExtraItem.m_dwFemaleCustom = rkRentalData.m_EquipItem[iSlot].m_item_female_custom;
					rkExtraItem.m_PeriodType	 = EPT_MORTMAIN;

					const ioItem *pItem = g_ItemMaker.GetItemConst( rkExtraItem.m_iItemCode, __FUNCTION__ );
					if( pItem )
						rkExtraItem.m_Name = pItem->GetName();
					rkExtraItem.m_IconName = g_ItemMaker.GetItemLargeIcon( rkExtraItem.m_iItemCode, 0 );
					return true;
				}
			}
			break;
		}
	}
	return false;
}

void ioCharRentalData::GetEquipMedal( const DWORD dwCharIndex, MedalSyncInfoVec &rkEquipMedal )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == dwCharIndex )
		{
			rkEquipMedal = rkRentalData.m_EquipMedal;
			break;
		}
	}
}

void ioCharRentalData::GetCharGrowth( const DWORD dwCharIndex, BYTE &rkCharGrowth, int iSlot )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == dwCharIndex )
		{
			if( COMPARE( iSlot, 0, MAX_CHAR_GROWTH ) )
				rkCharGrowth = rkRentalData.m_CharGrowth[iSlot];
			break;
		}
	}
}

void ioCharRentalData::GetItemGrowth( const DWORD dwCharIndex, BYTE &rkItemGrowth, int iSlot )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == dwCharIndex )
		{
			if( COMPARE( iSlot, 0, MAX_ITEM_GROWTH ) )
				rkItemGrowth = rkRentalData.m_ItemGrowth[iSlot];
			break;
		}
	}
}

void ioCharRentalData::EquipItem( CHARACTERDATA &rkCharData, bool bBulletInit )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == rkCharData.m_dwIndex )
		{			
			for(int i = 0;i < MAX_INVENTORY;i++)
			{
				rkCharData.m_data.m_extra_item[i] = 0;

				// 기본
				if( rkCharData.m_data.m_eCharPowerUpType != PUGT_NONE )
				{
					rkCharData.m_equip_item[i].m_item_code = (DEFAULT_BASIC_ITEM_CODE * i)  + ConvertPowerUpCharToItem( rkCharData.m_data.m_eCharPowerUpType ) + rkCharData.m_data.m_class_type;						
					if( !g_PowerUpManager.IsPowerUpGradeItem( rkCharData.m_data.m_class_type, rkCharData.m_equip_item[i].m_item_code ) || !g_ItemMaker.IsItem( rkCharData.m_equip_item[i].m_item_code ) )
					{
						rkCharData.m_equip_item[i].m_item_code	= (DEFAULT_BASIC_ITEM_CODE * i) + rkCharData.m_data.m_class_type;
					}
				}
				else
				{
					rkCharData.m_equip_item[i].m_item_code	= (DEFAULT_BASIC_ITEM_CODE * i) + rkCharData.m_data.m_class_type;
				}

				if( bBulletInit )
					rkCharData.m_equip_item[i].m_item_bullet	= -1;
				rkCharData.m_equip_item[i].m_item_male_custom	= 0;
				rkCharData.m_equip_item[i].m_item_female_custom	= 0;

				if( rkRentalData.m_EquipItem[i].m_item_code > 0 )	// 장비 아이템
				{
					rkCharData.m_equip_item[i].m_item_code			= rkRentalData.m_EquipItem[i].m_item_code;
					rkCharData.m_equip_item[i].m_item_reinforce		= rkRentalData.m_EquipItem[i].m_item_reinforce;
					if( bBulletInit )
						rkCharData.m_equip_item[i].m_item_bullet		= -1;
					rkCharData.m_equip_item[i].m_item_male_custom	= rkRentalData.m_EquipItem[i].m_item_male_custom;
					rkCharData.m_equip_item[i].m_item_female_custom = rkRentalData.m_EquipItem[i].m_item_female_custom;
				}
			}
			break;
		}
	}
}

void ioCharRentalData::InsertRentalData( RentalData &rkRentalData )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkData = *iter;
		if( rkData.m_dwCharIndex == rkRentalData.m_dwCharIndex )
		{
			m_RentalDataList.erase( iter );
			break;
		}
	}

	m_RentalDataList.push_back( rkRentalData );
}

void ioCharRentalData::DeleteRentalData( const DWORD dwCharIndex )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkData = *iter;
		if( rkData.m_dwCharIndex == dwCharIndex )
		{
			m_RentalDataList.erase( iter );
			return;
		}
	}
}

bool ioCharRentalData::GetEquipCostume( const DWORD dwCharIndex, CostumeSlot &sCostumeSlot, int nSlot )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == dwCharIndex )
		{
			if( COMPARE( nSlot, 0, MAX_INVENTORY ) )
			{
				if( rkRentalData.m_CostumeEquipInfo[nSlot].m_CostumeCode > 0 )
				{
					sCostumeSlot.m_nCostumeCode		= rkRentalData.m_CostumeEquipInfo[nSlot].m_CostumeCode;
					sCostumeSlot.m_dwMaleCustom		= rkRentalData.m_CostumeEquipInfo[nSlot].m_Costume_Male_Custom;
					sCostumeSlot.m_dwFemaleCustom	= rkRentalData.m_CostumeEquipInfo[nSlot].m_Costume_Female_Custom;
					sCostumeSlot.m_PeriodType		= EPT_MORTMAIN;

					sCostumeSlot.m_Name = g_CostumeInfoMgr.GetCostumeName( sCostumeSlot.m_nCostumeCode );
					sCostumeSlot.m_IconName = g_CostumeInfoMgr.GetCostumeIcon( sCostumeSlot.m_nCostumeCode );
					return true;
				}
			}
			break;
		}
	}
	return false;
}

void ioCharRentalData::EquipCostume( CHARACTERDATA &rkCharData )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == rkCharData.m_dwIndex )
		{			
			for(int i = 0;i < MAX_INVENTORY;i++)
			{
				rkCharData.m_data.m_EquipCostume[i].Init();

				// 코스튬 관련 (코스튬 적용)
				if( rkRentalData.m_CostumeEquipInfo[i].m_CostumeCode > 0 )	
				{
					rkCharData.m_data.m_EquipCostume[i].m_CostumeCode			= rkRentalData.m_CostumeEquipInfo[i].m_CostumeCode;
					rkCharData.m_data.m_EquipCostume[i].m_Costume_Male_Custom	= rkRentalData.m_CostumeEquipInfo[i].m_Costume_Male_Custom;
					rkCharData.m_data.m_EquipCostume[i].m_Costume_Female_Custom = rkRentalData.m_CostumeEquipInfo[i].m_Costume_Female_Custom;
				}
			}
			break;
		}
	}
}

bool ioCharRentalData::GetEquipAccessory( const DWORD dwCharIndex, AccessorySlot &sAccessorySlot, int iSlot )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == dwCharIndex )
		{
			if( COMPARE( iSlot, 0, MAX_ACC_INVENTORY ) )
			{
				if( rkRentalData.m_AccessoryEquipInfo[iSlot].m_iAccessoryCode> 0 )
				{
					sAccessorySlot.m_iAccessoryCode		= rkRentalData.m_AccessoryEquipInfo[iSlot].m_iAccessoryCode;
					const ioItem *pItem = g_ItemMaker.GetItemConst( sAccessorySlot.m_iAccessoryCode, __FUNCTION__ );
					if( pItem )
						sAccessorySlot.m_Name = pItem->GetName();

					sAccessorySlot.m_IconName = g_ItemMaker.GetItemLargeIcon( sAccessorySlot.m_iAccessoryCode, 0 );
					sAccessorySlot.m_iRandomNumber = rkRentalData.m_AccessoryEquipInfo[iSlot].m_iRandomNumber;
					sAccessorySlot.m_iCompoundCode = rkRentalData.m_AccessoryEquipInfo[iSlot].m_iCompoundCode;
					sAccessorySlot.m_iCompoundValue = rkRentalData.m_AccessoryEquipInfo[iSlot].m_iCompoundValue;
					return true;
				}
			}
			break;
		}
	}
	return false;
}

void ioCharRentalData::EquipAccessory( CHARACTERDATA &rkCharData )
{
	RentalDataList::iterator iter = m_RentalDataList.begin();
	for(;iter != m_RentalDataList.end();++iter)
	{
		RentalData &rkRentalData = *iter;
		if( rkRentalData.m_dwCharIndex == rkCharData.m_dwIndex )
		{			
			for(int i = 0;i < MAX_ACC_INVENTORY;i++)
			{
				rkCharData.m_data.m_EquipAccessory[i].Init();
				if( rkRentalData.m_AccessoryEquipInfo[i].m_iAccessoryCode > 0 )	
					rkCharData.m_data.m_EquipAccessory[i].m_iAccessoryCode			= rkRentalData.m_AccessoryEquipInfo[i].m_iAccessoryCode;
			}
			break;
		}
	}
}
