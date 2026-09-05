
#include "stdafx.h"

#include "ioUserFishItem.h"

#include "ioBaseChar.h"

#include "ioFishingManager.h"

ioUserFishItem::ioUserFishItem()
{
	Initialize();
}

ioUserFishItem::~ioUserFishItem()
{
}

void ioUserFishItem::Initialize()
{
	m_vFishItemList.clear();
	m_vFishingRodItemList.clear();
	m_vFishingBaitItemList.clear();
	m_vFishingGroundItemList.clear();
}

void ioUserFishItem::ApplyFishItemData( SP2Packet &rkPacket )
{
	m_vFishItemList.clear();
	m_vFishingRodItemList.clear();
	m_vFishingBaitItemList.clear();
	m_vFishingGroundItemList.clear();

	int iSize;
	rkPacket >> iSize;

	for( int i=0; i < iSize; ++i )
	{
		FishItem kItem;

		rkPacket >> kItem.m_iType;
		rkPacket >> kItem.m_iArray;
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( pUserEtcItem && pUserEtcItem->CheckDurationOfFishingGear( kItem.m_iType ) )
			kItem.m_bOwn = true;
		else
			kItem.m_bOwn = false;

		m_vFishItemList.push_back( kItem );
	}

	g_FishingMgr.MoveFishingRodData( m_vFishingRodItemList, FGT_ROD );
	g_FishingMgr.MoveFishingRodData( m_vFishingBaitItemList, FGT_BAIT );
	g_FishingMgr.MoveFishingRodData( m_vFishingGroundItemList, FGT_FISHING_GROUND );
	
	std::sort( m_vFishItemList.begin(), m_vFishItemList.end(), FishItemSort() );
	std::sort( m_vFishingRodItemList.begin(), m_vFishingRodItemList.end(), FishItemSort() );
	std::sort( m_vFishingBaitItemList.begin(), m_vFishingBaitItemList.end(), FishItemSort() );
	std::sort( m_vFishingGroundItemList.begin(), m_vFishingGroundItemList.end(), FishItemSort() );
}

void ioUserFishItem::AddFishItemData( BYTE iType, int iIndexArray )
{
	FishItem kItem;
	kItem.m_iType = iType;
	kItem.m_iArray = iIndexArray;

	m_vFishItemList.push_back( kItem );
}

void ioUserFishItem::DeleteFishItemData( int iIndexArray )
{
	int iSize = m_vFishItemList.size();

	for( int i=0; i < iSize; ++i )
	{
		if( m_vFishItemList[i].m_iArray == iIndexArray )
		{
			m_vFishItemList.erase( m_vFishItemList.begin() + i );
			return;
		}
	}
}

int ioUserFishItem::GetFishItemType( int iListArray )
{
	int iSize = m_vFishItemList.size();
	int iType = -1;

	if( COMPARE( iListArray, 0, iSize ) )
	{
		iType = m_vFishItemList[iListArray].m_iType;
	}

	return iType;
}

int ioUserFishItem::GetFishItemArray( int iListArray )
{
	int iSize = m_vFishItemList.size();
	int iIndexArray = 0;

	if( COMPARE( iListArray, 0, iSize ) )
	{
		iIndexArray = m_vFishItemList[iListArray].m_iArray;
	}

	return iIndexArray;
}

int ioUserFishItem::GetFishingRodItemType( int iListArray )
{
	int iSize = m_vFishingRodItemList.size();
	int iType = -1;

	if( COMPARE( iListArray, 0, iSize ) )
	{
		iType = m_vFishingRodItemList[iListArray].m_iType;
	}

	return iType;
}

int ioUserFishItem::GetFishingBaitItemType( int iListArray )
{
	int iSize = m_vFishingBaitItemList.size();
	int iType = -1;

	if( COMPARE( iListArray, 0, iSize ) )
	{
		iType = m_vFishingBaitItemList[iListArray].m_iType;
	}

	return iType;
}

int ioUserFishItem::GetFishingGroundItemType( int iListArray )
{
	int iSize = m_vFishingGroundItemList.size();
	int iType = -1;

	if( COMPARE( iListArray, 0, iSize ) )
	{
		iType = m_vFishingGroundItemList[iListArray].m_iType;
	}

	return iType;
}

void ioUserFishItem::ChangeFishingGroundItemOwn( int iListArray, bool bOwn )
{
	int iSize = m_vFishingGroundItemList.size();
	int iType = -1;

	if( COMPARE( iListArray, 0, iSize ) )
	{
		m_vFishingGroundItemList[iListArray].m_bOwn = bOwn;
	}
}

void ioUserFishItem::AlignFishingGearByOwn()
{
	bool bPeriodExpired = false;
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
		return;

	// 소유 여부 체크
	for( int i = 0; i < (int)m_vFishingRodItemList.size(); i++ )
	{
		if( g_MyInfo.HasEtcItem( m_vFishingRodItemList[ i ].m_dwEtcItemCode ) && pUserEtcItem->CheckDurationOfFishingGear( m_vFishingRodItemList[ i ].m_dwEtcItemCode ) )
			m_vFishingRodItemList[ i ].m_bOwn = true;
		else
			m_vFishingRodItemList[ i ].m_bOwn = false;
	}

	for( int j = 0; j < (int)m_vFishingBaitItemList.size(); j++ )
	{
		if( g_MyInfo.HasEtcItem( m_vFishingBaitItemList[ j ].m_dwEtcItemCode ) && pUserEtcItem->CheckDurationOfFishingGear( m_vFishingBaitItemList[ j ].m_dwEtcItemCode ) )
			m_vFishingBaitItemList[ j ].m_bOwn = true;
		else
			m_vFishingBaitItemList[ j ].m_bOwn = false;
	}

	for( int k = 0; k < (int)m_vFishingGroundItemList.size(); k++ )
	{
		if( g_MyInfo.HasEtcItem( m_vFishingGroundItemList[ k ].m_dwEtcItemCode ) && pUserEtcItem->CheckDurationOfFishingGear( m_vFishingGroundItemList[ k ].m_dwEtcItemCode ) )
			m_vFishingGroundItemList[ k ].m_bOwn = true;
		else
			m_vFishingGroundItemList[ k ].m_bOwn = false;
	}

	// 정렬
	std::sort( m_vFishingRodItemList.begin(), m_vFishingRodItemList.end(), FishingGearItemSort() );
	std::sort( m_vFishingBaitItemList.begin(), m_vFishingBaitItemList.end(), FishingGearItemSort() );
	std::sort( m_vFishingGroundItemList.begin(), m_vFishingGroundItemList.end(), FishingGearItemSort() );
}

bool ioUserFishItem::GetFishingGearOwnByTabType( int iListArray, FishingGearType eFishingGearType )
{
	int iSize = 0;
	bool bOwn = false;

	if( eFishingGearType == FGT_NONE )
		return true;
	else if( eFishingGearType == FGT_ROD )
	{
		iSize = (int)m_vFishingRodItemList.size();

		if( COMPARE( iListArray, 0, iSize ) )
		{
			return m_vFishingRodItemList[iListArray].m_bOwn;
		}
	}
	else if( eFishingGearType == FGT_BAIT )
	{
		iSize = (int)m_vFishingBaitItemList.size();

		if( COMPARE( iListArray, 0, iSize ) )
		{
			return m_vFishingBaitItemList[iListArray].m_bOwn;
		}
	}
	else if( eFishingGearType == FGT_FISHING_GROUND )
	{
		iSize = (int)m_vFishingGroundItemList.size();

		if( COMPARE( iListArray, 0, iSize ) )
		{
			return m_vFishingGroundItemList[iListArray].m_bOwn;
		}
	}

	return false;
}

bool ioUserFishItem::GetFishingGearOwnByEtcCode( int iEtcCode, FishingGearType eFishingGearType )
{
	int iSize = 0;
	bool bOwn = false;

	if( eFishingGearType == FGT_NONE )
		return true;
	else if( eFishingGearType == FGT_ROD )
	{
		iSize = (int)m_vFishingRodItemList.size();

		for( int i = 0; i < iSize; i++ )
		{
			if( m_vFishingRodItemList[ i ].m_dwEtcItemCode == iEtcCode )
				return m_vFishingRodItemList[ i ].m_bOwn;
		}
	}
	else if( eFishingGearType == FGT_BAIT )
	{
		iSize = (int)m_vFishingBaitItemList.size();

		for( int j = 0; j < iSize; j++ )
		{
			if( m_vFishingBaitItemList[ j ].m_dwEtcItemCode == iEtcCode )
				return m_vFishingBaitItemList[ j ].m_bOwn;
		}
	}

	return false;
}

DWORD ioUserFishItem::GetFishingGearEtcCodeByTabType( int iListArray, FishingGearType eFishingGearType )
{
	int iSize = 0;

	if( eFishingGearType == FGT_NONE )
		return 0;
	else if( eFishingGearType == FGT_ROD )
	{
		iSize = (int)m_vFishingRodItemList.size();

		if( COMPARE( iListArray, 0, iSize ) )
		{
			return m_vFishingRodItemList[iListArray].m_dwEtcItemCode;
		}
	}
	else if( eFishingGearType == FGT_BAIT )
	{
		iSize = (int)m_vFishingBaitItemList.size();

		if( COMPARE( iListArray, 0, iSize ) )
		{
			return m_vFishingBaitItemList[iListArray].m_dwEtcItemCode;
		}
	}
	else if( eFishingGearType == FGT_FISHING_GROUND )
	{
		iSize = (int)m_vFishingGroundItemList.size();

		if( COMPARE( iListArray, 0, iSize ) )
		{
			return m_vFishingGroundItemList[iListArray].m_dwEtcItemCode;
		}
	}

	return 0;
}

DWORD ioUserFishItem::GetAscendingOrderFishingBaitEtcCodeByOwn()
{
	for( int i = 0; i < (int)m_vFishingBaitItemList.size(); i++ )
	{
		if( m_vFishingBaitItemList[ i ].m_bOwn )
			return m_vFishingBaitItemList[ i ].m_dwEtcItemCode;
	}
	return 0;
}

int ioUserFishItem::GetFishingGearCountByTabType( FishingGearType eFishingGearType )
{
	if( eFishingGearType == FGT_ROD )
		return (int)m_vFishingRodItemList.size();
	else if( eFishingGearType == FGT_BAIT )
		return (int)m_vFishingBaitItemList.size();
	else if( eFishingGearType == FGT_FISHING_GROUND )
		return (int)m_vFishingGroundItemList.size();

	return 0;
}

int ioUserFishItem::GetCurFishItemCnt()
{
	int iSize = m_vFishItemList.size();
	
	return iSize;
}

DWORD ioUserFishItem::GetFishingRodByFirstOrder()
{
	DWORD dwEtcCode = 0;
	int iArray = MAX_INT_VALUE;
	for( int i = 0; i < (int)m_vFishingRodItemList.size(); i++ )
	{
		if( iArray > m_vFishingRodItemList[ i ].m_iArray && m_vFishingRodItemList[ i ].m_bOwn )
		{
			iArray = m_vFishingRodItemList[ i ].m_iArray;
			dwEtcCode = m_vFishingRodItemList[ i ].m_dwEtcItemCode;
		}
	}

	return dwEtcCode;
}

DWORD ioUserFishItem::GetFishingBaitByFirstOrder()
{
	DWORD dwEtcCode = 0;
	int iArray = MAX_INT_VALUE;
	for( int i = 0; i < (int)m_vFishingBaitItemList.size(); i++ )
	{
		if( iArray > m_vFishingBaitItemList[ i ].m_iArray && m_vFishingBaitItemList[ i ].m_bOwn )
		{
			iArray = m_vFishingBaitItemList[ i ].m_iArray;
			dwEtcCode = m_vFishingBaitItemList[ i ].m_dwEtcItemCode;
		}
	}

	return dwEtcCode;
}

void ioUserFishItem::CheckCurMaxInventory()
{
	m_iCurMaxInventory = DEFAULT_FISH_INVEN;

	// 특별아이템 체크
	int iExtendItem = g_MyInfo.GetFishingSlotExtendItem();
	m_iCurMaxInventory += iExtendItem;

	// 낚시 레벨 체크
	int iLevelAdd = g_MyInfo.GetFishingLevel() / 10;
	m_iCurMaxInventory += iLevelAdd;
}

int ioUserFishItem::GetCurMaxInventory()
{
	CheckCurMaxInventory();

	return m_iCurMaxInventory;
}

