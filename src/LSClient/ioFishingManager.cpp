

#include "stdafx.h"

#include "ioFishingManager.h"

#include "ioDecorationPrice.h"
#include "ioEtcItem.h"

template<> ioFishingManager* Singleton< ioFishingManager >::ms_Singleton = 0;

ioFishingManager::ioFishingManager()
{
	ClearAllInfo();
}

ioFishingManager::~ioFishingManager()
{
	ClearAllInfo();
}

void ioFishingManager::LoadFishingInfo()
{
	ClearAllInfo();

	ioINILoader_e kLoader( "config/sp2_fishing_info.ini" );

	kLoader.SetTitle_e( "common" );

	LoadFishItemList( kLoader );
	LoadFishGradeList( kLoader );
	LoadFishTypeList( kLoader ); 
	LoadFishingRodItemList( kLoader );
	LoadFishingBaitItemList( kLoader );
	LoadFishingGroundItemList( kLoader );
}

void ioFishingManager::LoadFishItemList( ioINILoader& rkLoader )
{
	// ItemList
	int iItemCnt = rkLoader.LoadInt_e( "max_item_cnt", 0 );
	
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	for( int i=0; i < iItemCnt; ++i )
	{
		FishingItem kItem;

		wsprintf_e( szKey, "item%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_Name = szBuf;

		wsprintf_e( szKey, "item%d_icon_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_IconName = szBuf;

		wsprintf_e( szKey, "item%d_desc", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_Desc = szBuf;

		wsprintf_e( szKey, "item%d_num", i+1 );
		kItem.m_iItemNum = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "item%d_peso", i+1 );
		kItem.m_iPeso = (__int64)rkLoader.LoadInt( szKey, 0 );

		m_vFishingItemList.push_back( kItem );
	}
}

void ioFishingManager::LoadFishGradeList( ioINILoader& rkLoader )
{
	// ItemGradeList
	int iGradeCnt = rkLoader.LoadInt_e( "max_grade_cnt", 0 );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	for( int j=0; j < iGradeCnt; ++j )
	{
		FishingItemGrade kGrade;

		wsprintf_e( szKey, "grade%d_name", j+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kGrade.m_Name = szBuf;

		sprintf_e( szKey, "grade%d_desc", j+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kGrade.m_Desc  = szBuf;

		wsprintf_e( szKey, "grade%d_num", j+1 );
		kGrade.m_iGradeNum = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "grade%d_value", j+1 );
		kGrade.m_fValue = rkLoader.LoadFloat( szKey, FLOAT1 );

		m_vFishingItemGradeList.push_back( kGrade );
	}
}

void ioFishingManager::LoadFishTypeList( ioINILoader& rkLoader )
{
	// type list
	int iTypeCnt = rkLoader.LoadInt_e( "type_cnt", 0 );
	
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	for( int k=0; k < iTypeCnt; ++k )
	{
		FishingTypeInfo kInfo;

		wsprintf_e( szKey, "fishing_rod%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FishingRod = szBuf;

		wsprintf_e( szKey, "fishing_ready%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FishingReadyEffect = szBuf;

		wsprintf_e( szKey, "fishing_wait%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FishingWaitEffect = szBuf;

		wsprintf_e( szKey, "fishing_pull%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FishingPullEffect = szBuf;

		m_vFishingTypeInfoList.push_back( kInfo );
	}
}

void ioFishingManager::LoadFishingRodItemList( ioINILoader& rkLoader )
{
	// ItemList
	int iItemCnt = rkLoader.LoadInt_e( "max_rod_cnt", 0 );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	for( int i=0; i < iItemCnt; ++i )
	{
		FishingItem kItem;

		wsprintf_e( szKey, "rod%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_Name = szBuf;

		wsprintf_e( szKey, "rod%d_icon_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_IconName = szBuf;

		wsprintf_e( szKey, "rod%d_desc", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_Desc = szBuf;

		wsprintf_e( szKey, "rod%d_num", i+1 );
		kItem.m_iItemNum = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "rod%d_item_code", i+1 );
		kItem.m_dwEtcItemCode = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "rod%d_order", i+1 );
		kItem.m_iOrder = rkLoader.LoadInt( szKey, 0 );

		kItem.m_GearType = FGT_ROD;

		m_vFishingGearItemList.push_back( kItem );
	}
}

void ioFishingManager::LoadFishingBaitItemList( ioINILoader& rkLoader )
{
	// ItemList
	int iItemCnt = rkLoader.LoadInt_e( "max_bait_cnt", 0 );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	for( int i=0; i < iItemCnt; ++i )
	{
		FishingItem kItem;

		wsprintf_e( szKey, "bait%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_Name = szBuf;

		wsprintf_e( szKey, "bait%d_icon_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_IconName = szBuf;

		wsprintf_e( szKey, "bait%d_desc", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_Desc = szBuf;

		wsprintf_e( szKey, "bait%d_num", i+1 );
		kItem.m_iItemNum = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "bait%d_item_code", i+1 );
		kItem.m_dwEtcItemCode = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "bait%d_order", i+1 );
		kItem.m_iOrder = rkLoader.LoadInt( szKey, 0 );

		kItem.m_GearType = FGT_BAIT;

		m_vFishingGearItemList.push_back( kItem );
	}
}

void ioFishingManager::LoadFishingGroundItemList( ioINILoader& rkLoader )
{
	// ItemList
	int iItemCnt = rkLoader.LoadInt_e( "max_fishing_ground_cnt", 0 );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	for( int i=0; i < iItemCnt; ++i )
	{
		FishingItem kItem;

		wsprintf_e( szKey, "fishing_ground%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_Name = szBuf;

		wsprintf_e( szKey, "fishing_ground%d_icon_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_IconName = szBuf;

		wsprintf_e( szKey, "fishing_ground%d_desc", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kItem.m_Desc = szBuf;

		wsprintf_e( szKey, "fishing_ground%d_num", i+1 );
		kItem.m_iItemNum = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "fishing_ground%d_item_code", i+1 );
		kItem.m_dwEtcItemCode = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "fishing_ground%d_order", i+1 );
		kItem.m_iOrder = rkLoader.LoadInt( szKey, 0 );

		kItem.m_GearType = FGT_FISHING_GROUND;

		m_vFishingGearItemList.push_back( kItem );
	}
}

void ioFishingManager::LoadGuildFishingInfo()
{
	ioINILoader_e kLoader( "config/sp2_fishing_info.ini" );
	kLoader.SetTitle_e( "GuildFisheryItem" );
	LoadFishItemList( kLoader );
	LoadFishGradeList( kLoader );
}

void ioFishingManager::ClearAllInfo()
{
	m_vFishingItemList.clear();
	m_vFishingGearItemList.clear();
	m_vFishingItemGradeList.clear();
	m_vFishingTypeInfoList.clear();
}

ioFishingManager& ioFishingManager::GetSingleton()
{
	return Singleton< ioFishingManager >::GetSingleton();
}

ioHashString ioFishingManager::GetFishingItemName( int iItemType, FishingGearType GearType )
{
	if( GearType == FGT_NONE )
	{
		FishingItemList::iterator iter = m_vFishingItemList.begin();
		while( iter != m_vFishingItemList.end() )
		{
			if( (*iter).m_iItemNum == iItemType )
				return (*iter).m_Name;

			++iter;
		}
	}
	else
	{
		FishingItemList::iterator iter = m_vFishingGearItemList.begin();
		while( iter != m_vFishingGearItemList.end() )
		{
			if( (*iter).m_iItemNum == iItemType && (*iter).m_GearType == GearType )
				return (*iter).m_Name;

			++iter;
		}
	}

	return ioHashString();
}

ioHashString ioFishingManager::GetFishingItemDesc( int iItemType, FishingGearType GearType )
{
	if( GearType == FGT_NONE )
	{
		FishingItemList::iterator iter = m_vFishingItemList.begin();
		while( iter != m_vFishingItemList.end() )
		{
			if( (*iter).m_iItemNum == iItemType )
				return (*iter).m_Desc;

			++iter;
		}
	}
	else
	{
		FishingItemList::iterator iter = m_vFishingGearItemList.begin();
		while( iter != m_vFishingGearItemList.end() )
		{
			if( (*iter).m_iItemNum == iItemType )
				return (*iter).m_Desc;

			++iter;
		}
	}

	return ioHashString();
}

ioHashString ioFishingManager::GetFishingItemIconName( int iItemType, FishingGearType GearType )
{
	if( GearType == FGT_NONE )
	{
		FishingItemList::iterator iter = m_vFishingItemList.begin();
		while( iter != m_vFishingItemList.end() )
		{
			if( (*iter).m_iItemNum == iItemType )
				return (*iter).m_IconName;

			++iter;
		}
	}
	else
	{
		FishingItemList::iterator iter = m_vFishingGearItemList.begin();
		while( iter != m_vFishingGearItemList.end() )
		{
			if( (*iter).m_iItemNum == iItemType && (*iter).m_GearType == GearType )
				return (*iter).m_IconName;

			++iter;
		}
	}

	return ioHashString();
}

ioHashString ioFishingManager::GetFishingItemGradeName( int iItemGrade )
{
	int iGradeCnt = m_vFishingItemGradeList.size();
	for( int i=0; i < iGradeCnt; ++i )
	{
		if( m_vFishingItemGradeList[i].m_iGradeNum == iItemGrade )
		{
			return m_vFishingItemGradeList[i].m_Name;
		}
	}

	return ioHashString();
}

ioHashString ioFishingManager::GetFishingItemGradeDesc( int iItemGrade )
{
	int iGradeCnt = m_vFishingItemGradeList.size();
	for( int i=0; i < iGradeCnt; ++i )
	{
		if( m_vFishingItemGradeList[i].m_iGradeNum == iItemGrade )
		{
			return m_vFishingItemGradeList[i].m_Desc;
		}
	}

	return ioHashString();
}

ioHashString ioFishingManager::GetFishingRod( int iType )
{
	int iExtraType = -1;
	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( iType );
	if( pItem )
	{
		iExtraType = pItem->GetExtraType() - 1;
	}

	if( COMPARE( iExtraType, 0, (int)m_vFishingTypeInfoList.size() ) )
	{
		return m_vFishingTypeInfoList[iExtraType].m_FishingRod;
	}

	return ioHashString();
}

ioHashString ioFishingManager::GetFishingReadyEffect( int iType )
{
	int iExtraType = -1;
	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( iType );
	if( pItem )
	{
		iExtraType = pItem->GetExtraType() - 1;
	}

	if( COMPARE( iExtraType, 0, (int)m_vFishingTypeInfoList.size() ) )
	{
		return m_vFishingTypeInfoList[iExtraType].m_FishingReadyEffect;
	}

	return ioHashString();
}

ioHashString ioFishingManager::GetFishingWaitEffect( int iType )
{
	int iExtraType = -1;
	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( iType );
	if( pItem )
	{
		iExtraType = pItem->GetExtraType() - 1;
	}

	if( COMPARE( iExtraType, 0, (int)m_vFishingTypeInfoList.size() ) )
	{
		return m_vFishingTypeInfoList[iExtraType].m_FishingWaitEffect;
	}

	return ioHashString();
}

ioHashString ioFishingManager::GetFishingPullEffect( int iType )
{
	int iExtraType = -1;
	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( iType );
	if( pItem )
	{
		iExtraType = pItem->GetExtraType() - 1;
	}

	if( COMPARE( iExtraType, 0, (int)m_vFishingTypeInfoList.size() ) )
	{
		return m_vFishingTypeInfoList[iExtraType].m_FishingPullEffect;
	}

	return ioHashString();
}

ioHashString ioFishingManager::GetFishingPresentItemName( short iPresentType, int iValue1, int iValue2 )
{
	static ioHashString szReturn;

	ioHashString szText1 = g_PresentMgr.GetPresentValue1Text( iPresentType, iValue1, iValue2 );
	ioHashString szText2 = g_PresentMgr.GetPresentValue2Text( iPresentType, iValue1, iValue2 );

	char szBuf[MAX_PATH] = "";
	wsprintf_e( szBuf, "%s %s", szText1.c_str(), szText2.c_str() );
	szReturn = szBuf;

	switch( iPresentType )
	{
	case PRESENT_ETC_ITEM:
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iValue1 );
			if( !pEtcItem )
				szReturn = "None";
		}
		break;
	case PRESENT_EXTRAITEM:
	case PRESENT_ACCESSORY:
		{
			const ioItem *pItem = g_ItemMaker.GetItemConst( iValue1, __FUNCTION__ );
			if( !pItem )
				szReturn = "None";
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		{
			RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iValue1 );
			if( !pInfo )
				szReturn = "None";
		}
		break;
	case PRESENT_MEDALITEM:
		{
			const ioMedalItemInfoManager::ItemInfo *pItem = g_MedalItemMgr.GetItemInfo( iValue1 );
			if( !pItem )	
				szReturn = "None";
		}
		break;
	case PRESENT_COSTUME:
		{
			CostumeInfo sCostumeInfo;
			if ( !g_CostumeInfoMgr.GetCostumeInfo( iValue1, sCostumeInfo ) )
				szReturn = "None";
		}
		break;
	case PRESENT_COSTUME_BOX:
		{
			CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( iValue1 );
			if( !pInfo )
				szReturn = "None";
		}
		break;
	}

	return szReturn;
}

void ioFishingManager::MoveFishingRodData( FishItemList& rkFishItemList, FishingGearType GearType )
{
	int iCount = 0;
	for( int i = 0; i < (int)m_vFishingGearItemList.size(); i++ )
	{
		if( m_vFishingGearItemList[ i ].m_GearType == GearType )
		{
			FishItem kItem;
			kItem.m_iType = m_vFishingGearItemList[ i ].m_iItemNum;
			kItem.m_dwEtcItemCode = m_vFishingGearItemList[ i ].m_dwEtcItemCode;
			kItem.m_iArray = m_vFishingGearItemList[ i ].m_iOrder;

			rkFishItemList.push_back( kItem );
		}
	}
}

int ioFishingManager::GetFishingGearNumByItemCode( DWORD dwItemCode )
{
	for( int i = 0; i < (int)m_vFishingGearItemList.size(); i++ )
	{
		if( m_vFishingGearItemList[ i ].m_dwEtcItemCode == dwItemCode )
			return m_vFishingGearItemList[ i ].m_iItemNum;
	}

	return 0;
}

DWORD ioFishingManager::GetFishingGearCodeByTypeNum( FishingGearType GearType, int iItemNum )
{
	for( int i = 0; i < (int)m_vFishingGearItemList.size(); i++ )
	{
		if( m_vFishingGearItemList[ i ].m_GearType == GearType )
		{
			if( m_vFishingGearItemList[ i ].m_iItemNum == iItemNum )
				return m_vFishingGearItemList[ i ].m_dwEtcItemCode;
		}
	}

	return 0;
}