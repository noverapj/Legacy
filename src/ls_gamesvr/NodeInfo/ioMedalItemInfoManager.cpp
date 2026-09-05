#include "stdafx.h"
#include ".\iomedaliteminfomanager.h"
#include <strsafe.h>

extern CLog EventLOG;

template<> ioMedalItemInfoManager* Singleton< ioMedalItemInfoManager >::ms_Singleton = 0;

ioMedalItemInfoManager::ioMedalItemInfoManager(void)
{
	m_bINILoading = false;
}

ioMedalItemInfoManager::~ioMedalItemInfoManager(void)
{
	Clear();
}

ioMedalItemInfoManager& ioMedalItemInfoManager::GetSingleton()
{
	return Singleton< ioMedalItemInfoManager >::GetSingleton();
}


void ioMedalItemInfoManager::LoadINI()
{
	m_bINILoading = true;
	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_medalitem_info.ini" );
	LoadMedalItemInfo( kLoader );
	m_bINILoading = false;
}

void ioMedalItemInfoManager::LoadMedalItemInfo( ioINILoader &rkLoader )
{
	Clear();

	rkLoader.SetTitle( "sell_info" );
	int iDay = rkLoader.LoadInt( "item_sell_devide_day", 30 );
	m_iItemSellDevideMin = iDay * 24 * 60;

	rkLoader.SetTitle( "slot_info" );
	int iMax = rkLoader.LoadInt( "max_info", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		SlotInfo kInfo;

		char szKey[MAX_PATH]="";
		StringCbPrintf( szKey, sizeof( szKey ), "level_over_%d", i+1 );
		kInfo.m_iLevelOver = rkLoader.LoadInt( szKey, -1 );
		StringCbPrintf( szKey, sizeof( szKey ), "slot_num_%d", i+1 );
		kInfo.m_iSlotNum = rkLoader.LoadInt( szKey, -1 );

		m_vSlotInfoVec.push_back( kInfo );
	}

	rkLoader.SetTitle( "item_info_common" );
	iMax = rkLoader.LoadInt( "max_item_info" , 0 );
	for (int i = 0; i < iMax ; i++)
	{
		char szTitle[MAX_PATH]="";
		StringCbPrintf( szTitle, sizeof( szTitle ), "item_info_%d", i+1 );
		rkLoader.SetTitle( szTitle );

		ItemInfo kInfo;
		kInfo.m_iItemType   = rkLoader.LoadInt( "item_type", 0 );
		if( kInfo.m_iItemType <= 0 )
			continue;

		kInfo.m_iLimitLevel = rkLoader.LoadInt( "limit_level", 0 );
		kInfo.m_iSellPeso   = rkLoader.LoadInt( "sell_peso", 0 );

		char szKey[MAX_PATH]="";
		int iMaxClass = rkLoader.LoadInt( "max_class", 0 );
		for (int j = 0; j < iMaxClass ; j++)
		{
			StringCbPrintf( szKey, sizeof( szKey ), "use_class_%d", j+1 );
			DWORD dwClassType = rkLoader.LoadInt( szKey, 0 );
			kInfo.m_vUseClassTypeVec.push_back( dwClassType );
		}

		//HRYOON 20170206 커스튬 메달
		
		kInfo.m_iSubMedalType	= rkLoader.LoadInt( "sub_medal_type", 1 );
		kInfo.m_iTotalPoint		= rkLoader.LoadInt( "total_point", 0 );
		kInfo.m_iEnableSelectStat = rkLoader.LoadInt( "enable_select_stat", 0 );
		kInfo.m_iMinStatPoint	= rkLoader.LoadInt( "min_stat_point", 0 );
		kInfo.m_iMaxStatPoint	= rkLoader.LoadInt( "max_stat_point", 0 );

		kInfo.m_iLimitLevel = rkLoader.LoadInt( "limit_level", 0 );
		
		// 일반 메달인 경우만 세팅
		if( kInfo.m_iSubMedalType == 1 )
		{
			for (int j = 0; j < MAX_CHAR_GROWTH ; j++)
			{
				StringCbPrintf( szKey, sizeof( szKey ), "char_growth_%d", j+1 );
				kInfo.m_iCharGrowth[j] = rkLoader.LoadInt( szKey, 0 );
			}

			for (int j = 0; j < MAX_ITEM_GROWTH ; j++)
			{
				StringCbPrintf( szKey, sizeof( szKey ), "item_growth_%d", j+1 );
				kInfo.m_iItemGrowth[j] = rkLoader.LoadInt( szKey, 0 );
			}
		}
		
		m_vItemInfoVec.push_back( kInfo );
	}

// Test 로그
//	for (int i = 0; i < 100 ; i++)
//	{
//		EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Level : %d , SlotNum : %d", __FUNCTION__, i, GetSlotNum( i ) );
//	}
}

const ioMedalItemInfoManager::ItemInfo *ioMedalItemInfoManager::GetItemInfo( int iItemType )
{
	if ( m_bINILoading == true )
		return NULL;

	for(vItemInfoVec::iterator iter = m_vItemInfoVec.begin(); iter != m_vItemInfoVec.end(); ++iter)
	{
	    ItemInfo &rkInfo = (*iter);
		if( rkInfo.m_iItemType != iItemType )
			continue;

		return &rkInfo;
	}

	return NULL;
}

int ioMedalItemInfoManager::GetSlotNum( int iCurLevel )
{
	if ( m_bINILoading == true )
		return -1;

	int iStartLevel    = 0;
	int iReturnSlotNum = -1;
	for(vSlotInfoVec::iterator iter = m_vSlotInfoVec.begin(); iter != m_vSlotInfoVec.end(); ++iter)
	{
	    SlotInfo &rkInfo = (*iter);
		if( COMPARE( iCurLevel, iStartLevel , rkInfo.m_iLevelOver ) )
			return iReturnSlotNum;

		iStartLevel    = rkInfo.m_iLevelOver;
		iReturnSlotNum = rkInfo.m_iSlotNum;
	}

	if( iCurLevel >= iStartLevel )
		return iReturnSlotNum;

	return -1;
}

int ioMedalItemInfoManager::GetSubMedalType( int iItemType )
{
	if ( m_bINILoading == true )
		return -1;

	const ItemInfo *pInfo = GetItemInfo( iItemType );
	if( !pInfo )
		return -1;

	return pInfo->m_iSubMedalType;
}

int ioMedalItemInfoManager::GetMinStatPoint( int iItemType ) 
{
	if ( m_bINILoading == true )
		return -1;

	const ItemInfo *pInfo = GetItemInfo( iItemType );
	if( !pInfo )
		return -1;

	return pInfo->m_iMinStatPoint;
}

int ioMedalItemInfoManager::GetTotalPoint( int iItemType ) 
{
	if ( m_bINILoading == true )
		return -1;

	const ItemInfo *pInfo = GetItemInfo( iItemType );
	if( !pInfo )
		return -1;

	return pInfo->m_iTotalPoint;
}

int ioMedalItemInfoManager::GetMaxStatPoint( int iItemType )
{
	if ( m_bINILoading == true )
		return -1;

	const ItemInfo *pInfo = GetItemInfo( iItemType );
	if( !pInfo )
		return -1;

	return pInfo->m_iMaxStatPoint;
}

int ioMedalItemInfoManager::GetSelectStat( int iItemType )
{
	if ( m_bINILoading == true )
		return -1;

	const ItemInfo *pInfo = GetItemInfo( iItemType );
	if( !pInfo )
		return -1;

	return pInfo->m_iEnableSelectStat;
}

DWORD ioMedalItemInfoManager::GetLevelLimit( int iItemType )
{
	if ( m_bINILoading == true )
		return -1;

	const ItemInfo *pInfo = GetItemInfo( iItemType );
	if( !pInfo )
		return -1;

	return pInfo->m_iLimitLevel;
}

bool ioMedalItemInfoManager::IsRight( int iItemType, int iClassType )
{
	if ( m_bINILoading == true )
		return false;

	const ItemInfo *pInfo = GetItemInfo( iItemType );
	if( !pInfo )
		return false;

	if( pInfo->m_vUseClassTypeVec.empty() ) // class 설정이 없다면 모든 캐릭터에게 장착 가능
		return true;

	int iSize = pInfo->m_vUseClassTypeVec.size();
	for (int i = 0; i < iSize ; i++)
	{
		if( pInfo->m_vUseClassTypeVec[i] == iClassType )
			return true;
	}

	return false;
}

void ioMedalItemInfoManager::Clear()
{
	m_vSlotInfoVec.clear();
	for(vItemInfoVec::iterator iter = m_vItemInfoVec.begin(); iter != m_vItemInfoVec.end(); ++iter)
	{
		ItemInfo &rkInfo = (*iter );
		rkInfo.m_vUseClassTypeVec.clear();
	}
	m_vItemInfoVec.clear();
}

int ioMedalItemInfoManager::GetMedalItemGrowth( int iItemType, bool bCharGrowth, int iArray )
{
	if ( m_bINILoading == true )
		return 0;

	const ItemInfo *pItemInfo = GetItemInfo( iItemType );
	if( !pItemInfo )
		return 0;

	if( bCharGrowth )
	{
		if( COMPARE( iArray, 0, MAX_CHAR_GROWTH ) )
			return pItemInfo->m_iCharGrowth[iArray];
	}
	else
	{
		if( COMPARE( iArray, 0, MAX_ITEM_GROWTH ) )
			return pItemInfo->m_iItemGrowth[iArray];
	}

	return 0;
}

int ioMedalItemInfoManager::GetSellPeso( int iItemType )
{
	if ( m_bINILoading == true )
		return 0;

	const ItemInfo *pInfo = GetItemInfo( iItemType );
	if( !pInfo )
		return 0;

	return pInfo->m_iSellPeso;
}