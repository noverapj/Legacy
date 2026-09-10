#include "stdafx.h"

#include "ioMedalItemInfoManager.h"

template<> ioMedalItemInfoManager* Singleton< ioMedalItemInfoManager >::ms_Singleton = 0;

ioMedalItemInfoManager::ioMedalItemInfoManager(void)
{
	m_iMaxItemArray  = 0;
	m_pItemInfoArray = NULL;
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
	Clear();

	ioINILoader_e kLoader( "config/sp2_medalitem_info.ini" );

	kLoader.SetTitle_e( "sell_info" );
	int iDay = kLoader.LoadInt_e( "item_sell_devide_day", 30 );
	m_iItemSellDevideMin = iDay * 24 * 60;

	kLoader.SetTitle_e( "slot_info" );
	int iMax = kLoader.LoadInt_e( "max_info", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		SlotInfo kInfo;

		char szKey[MAX_PATH]="";
		StringCbPrintf_e( szKey, sizeof( szKey ), "level_over_%d", i+1 );
		kInfo.m_iLevelOver = kLoader.LoadInt( szKey, -1 );
		StringCbPrintf_e( szKey, sizeof( szKey ), "slot_num_%d", i+1 );
		kInfo.m_iSlotNum = kLoader.LoadInt( szKey, -1 );

		m_vSlotInfoVec.push_back( kInfo );
	}

	kLoader.SetTitle_e( "item_info_common" );
	int iMaxItemInfo = kLoader.LoadInt_e( "max_item_info" , 0 );
	if( iMaxItemInfo == 0 )
		return;

	// size
	m_iMaxItemArray = 0;
	for (int i = 0; i < iMaxItemInfo; i++)
	{
		char szTitle[MAX_PATH]="";
		StringCbPrintf_e( szTitle, sizeof( szTitle ), "item_info_%d", i+1 );
		kLoader.SetTitle( szTitle );

		ItemInfo kInfo;
		kInfo.m_iItemType   = kLoader.LoadInt_e( "item_type", 0 );
		if( kInfo.m_iItemType <= 0 )
			continue;
		m_iMaxItemArray++;
	}

	int iCnt = 0;
	m_pItemInfoArray = new ItemInfo[m_iMaxItemArray];
	for (int i = 0; i < iMaxItemInfo ; i++)
	{
		char szTitle[MAX_PATH]="";
		StringCbPrintf_e( szTitle, sizeof( szTitle ), "item_info_%d", i+1 );
		kLoader.SetTitle( szTitle );

		ItemInfo kInfo;
		kInfo.m_iItemType   = kLoader.LoadInt_e( "item_type", 0 );
		if( kInfo.m_iItemType <= 0 )
			continue;

		kInfo.m_iLimitLevel = kLoader.LoadInt_e( "limit_level", 0 );
		kInfo.m_iSubMedalType = kLoader.LoadInt_e( "sub_medal_type", 1 );
		kInfo.m_iSelectStat = kLoader.LoadInt_e( "enable_select_stat", 0 );
		kInfo.m_iMinStatPoint = kLoader.LoadInt_e( "min_stat_point", 0 );
		kInfo.m_iMaxStatPoint = kLoader.LoadInt_e( "max_stat_point", 0 );

		char szKey[MAX_PATH]="";
		int iMaxClass = kLoader.LoadInt_e( "max_class", 0 );
		for (int j = 0; j < iMaxClass ; j++)
		{
			StringCbPrintf_e( szKey, sizeof( szKey ), "use_class_%d", j+1 );
			DWORD dwClassType = kLoader.LoadInt( szKey, 0 );
			kInfo.m_vUseClassTypeVec.push_back( dwClassType );
		}

		for (int j = 0; j < MAX_CHAR_GROWTH ; j++)
		{
			StringCbPrintf_e( szKey, sizeof( szKey ), "char_growth_%d", j+1 );
			kInfo.m_iCharGrowth[j] = kLoader.LoadInt( szKey, 0 );
		}

		for (int j = 0; j < MAX_ITEM_GROWTH ; j++)
		{
			StringCbPrintf_e( szKey, sizeof( szKey ), "item_growth_%d", j+1 );
			kInfo.m_iItemGrowth[j] = kLoader.LoadInt( szKey, 0 );
		}
		
		char szBuf[MAX_PATH] = "";
		kLoader.LoadString_e( "name", "", szBuf, MAX_PATH );
		kInfo.m_sName = szBuf;

		kLoader.LoadString_e( "icon", "", szBuf, MAX_PATH );
		kInfo.m_sIcon = szBuf;
		kLoader.LoadString_e( "subicon", "", szBuf, MAX_PATH );
		kInfo.m_sSubIcon = szBuf;

		kInfo.m_iManual   = kLoader.LoadInt_e( "manual", 0 );
		kInfo.m_iSellPeso = kLoader.LoadInt_e( "sell_peso", 0 );

		if( COMPARE( iCnt , 0, m_iMaxItemArray ) )
		{
			m_pItemInfoArray[iCnt] = kInfo;
			iCnt++;
		}
	}
}

const ioMedalItemInfoManager::ItemInfo *ioMedalItemInfoManager::GetItemInfo( int iItemType )
{
	for (int i = 0; i < m_iMaxItemArray ; i++)
	{
		if( m_pItemInfoArray[i].m_iItemType != iItemType )
			continue;

		return &m_pItemInfoArray[i];

	}

	return NULL;
}

void ioMedalItemInfoManager::Clear()
{
	m_vSlotInfoVec.clear();

	for (int i = 0; i < m_iMaxItemArray ; i++)
	{
		m_pItemInfoArray[i].m_vUseClassTypeVec.clear();
	}

	SAFEDELETEARRAY( m_pItemInfoArray );
}

int ioMedalItemInfoManager::GetSlotNum( int iCurLevel )
{
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
	{
		return iReturnSlotNum;
	}

	return -1;
}

int ioMedalItemInfoManager::GetLevelOver( int iSlotNum )
{
	for(vSlotInfoVec::iterator iter = m_vSlotInfoVec.begin(); iter != m_vSlotInfoVec.end(); ++iter)
	{
		SlotInfo &rkInfo = (*iter);
		if( rkInfo.m_iSlotNum == iSlotNum )
			return rkInfo.m_iLevelOver;
	}

	return -1;
}

DWORD ioMedalItemInfoManager::GetLevelLimit( int iItemType )
{
	const ItemInfo *pInfo = GetItemInfo( iItemType );
	if( !pInfo )
		return -1;

	return pInfo->m_iLimitLevel;
}

bool ioMedalItemInfoManager::IsRight( int iItemType, int iClassType )
{
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

int ioMedalItemInfoManager::GetMedalItemGrowth( int iItemType, bool bCharGrowth, int iArray )
{
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
	const ItemInfo *pInfo = GetItemInfo( iItemType );
	if( !pInfo )
		return 0;

	return pInfo->m_iSellPeso;
}