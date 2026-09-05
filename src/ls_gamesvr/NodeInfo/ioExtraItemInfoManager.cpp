

#include "stdafx.h"

#include "../Util/IORandom.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "ioUserExtraItem.h"
#include "ioExtraItemInfoManager.h"

#include <strsafe.h>
#include "ioSaleManager.h"
#include "ioEtcItem.h"
#include "ioItemInfoManager.h"

extern CLog EventLOG;
extern CLog RateCheckLOG;


//////////////////////////////////////////////////////////////////////////////////////////

ioRandomMachineGroup::ioRandomMachineGroup()
{
	m_iMachineCode = 0;
	m_iNeedLevel   = 0;
	m_bPackage     = false;
	m_eDefaultType = MT_NONE;

	m_dwTotalItemRate = 0;
	m_dwTotalPeriodRate = 0;
	m_dwTotalReinforceRate = 0;
	m_dwTotalReinforceCashRate = 0;

	m_iNeedCash    = 0;
	m_iBonusPeso   = 0;
	m_iSellPeso    = 0;

	m_iSubscriptionType = 0;
}

ioRandomMachineGroup::~ioRandomMachineGroup()
{
	ClearAll();
}


void ioRandomMachineGroup::ClearAll()
{
	m_iMachineCode = 0;
	m_iNeedCash    = 0;
	m_iBonusPeso   = 0;
	m_iSellPeso    = 0;

	m_iSubscriptionType = 0;
	
	m_vNeedPeso.clear();
	m_vPeriod.clear();

	m_iNeedLevel   = 0;
	m_bPackage     = false;

	m_vRandomItemList.clear();
	m_vPeriodList.clear();
	m_vReinforceList.clear();
	m_vReinforceCashList.clear();
	m_vTradeTypeList.clear();
}

void ioRandomMachineGroup::LoadMachineRandomInfo( IN ioINILoader &rkLoader, IN bool bItemLoadAll )
{
	char szKey1[MAX_PATH] = "";
	char szKey2[MAX_PATH] = "";
	char szKey3[MAX_PATH] = "";

	m_bActive = (rkLoader.LoadInt( "active", 1 ) == 1);

	// Item List
	int iItemCnt = rkLoader.LoadInt( "item_cnt", 0 );
	if( bItemLoadAll )
	{
		m_vRandomItemList.clear();
		m_vRandomItemList.reserve( iItemCnt );
		m_dwTotalItemRate = 0;
	}

	for( int i=0; i < iItemCnt; ++i )
	{
		wsprintf( szKey2, "item%d_code", i+1 );
		int iItemCode = rkLoader.LoadInt( szKey2, 0 );

		wsprintf( szKey2, "item%d_rate", i+1 );
		int iItemRate = rkLoader.LoadInt( szKey2, 0 );

		wsprintf( szKey2, "item%d_trade", i+1 );
		int iList = rkLoader.LoadInt( szKey2, 1 );

		RandomItem kItem;
		kItem.m_iItemCode = iItemCode;
		kItem.m_iRandomRate = iItemRate;
		kItem.m_iTradeTypeList = iList;

		bool bNewItem = false;
		if( !bItemLoadAll )
		{
			bool bExist = false;
			for(RandomItemList::iterator iter = m_vRandomItemList.begin(); iter != m_vRandomItemList.end(); ++iter)
			{
				RandomItem &rkItem = (*iter);
				if( rkItem.m_iItemCode != kItem.m_iItemCode )
					continue;
				rkItem.m_iTradeTypeList = kItem.m_iTradeTypeList;
				int iRateAdjust = kItem.m_iRandomRate - rkItem.m_iRandomRate;
				m_dwTotalItemRate += iRateAdjust;
				rkItem.m_iRandomRate = kItem.m_iRandomRate;
				bExist = true;
				break;
			}

			if( !bExist )
				bNewItem = true;
		}

		if( bItemLoadAll || bNewItem )
		{
			m_dwTotalItemRate += iItemRate;
			m_vRandomItemList.push_back( kItem );
		}
	}

	// Period Time
	int iPeriodTimeCnt = rkLoader.LoadInt( "period_time_cnt", 0 );
	if( iPeriodTimeCnt > 0 )
	{
		m_vPeriodList.clear();
		m_vPeriodList.reserve( iPeriodTimeCnt );
		m_dwTotalPeriodRate = 0;
	}

	for( int j=0; j < iPeriodTimeCnt; ++j )
	{
		RandomPeriodTime kPeriodTime;

		wsprintf( szKey1, "period_time%d_rate", j+1 );
		kPeriodTime.m_iRandomRate = rkLoader.LoadInt( szKey1, 0 );

		wsprintf( szKey2, "period_time%d_value", j+1 );
		kPeriodTime.m_iPeriodTime = rkLoader.LoadInt( szKey2, 0 );

		wsprintf( szKey3, "period_time%d_alarm", j+1 );
		kPeriodTime.m_bAlarm = rkLoader.LoadBool( szKey3, false );

		m_dwTotalPeriodRate += kPeriodTime.m_iRandomRate;

		m_vPeriodList.push_back( kPeriodTime );
	}

	// Reinforce
	int iReinforceCnt = rkLoader.LoadInt( "reinforce_cnt", 0 );
	if( iReinforceCnt > 0 )
	{
		m_vReinforceList.clear();
		m_vReinforceList.reserve( iReinforceCnt );
		m_dwTotalReinforceRate = 0;
	}

	for( int k=0; k < iReinforceCnt; ++k )
	{
		RandomReinforce kReinforce;

		wsprintf( szKey1, "reinforce%d_rate", k+1 );
		kReinforce.m_iRandomRate = rkLoader.LoadInt( szKey1, 0 );

		wsprintf( szKey2, "reinforce%d_value", k+1 );
		kReinforce.m_iReinforce = rkLoader.LoadInt( szKey2, 0 );

		m_dwTotalReinforceRate += kReinforce.m_iRandomRate;

		m_vReinforceList.push_back( kReinforce );
	}

	// Reinforce cash
	int iReinforceCashCnt = rkLoader.LoadInt( "reinforce_cash_cnt", 0 );
	if( iReinforceCashCnt > 0 )
	{
		m_vReinforceCashList.clear();
		m_vReinforceCashList.reserve( iReinforceCashCnt );
		m_dwTotalReinforceCashRate = 0;
	}

	for( int k=0; k < iReinforceCashCnt; ++k )
	{
		RandomReinforce kReinforceCash;

		wsprintf( szKey1, "reinforce_cash%d_rate", k+1 );
		kReinforceCash.m_iRandomRate = rkLoader.LoadInt( szKey1, 0 );

		wsprintf( szKey2, "reinforce_cash%d_value", k+1 );
		kReinforceCash.m_iReinforce = rkLoader.LoadInt( szKey2, 0 );

		m_dwTotalReinforceCashRate += kReinforceCash.m_iRandomRate;

		m_vReinforceCashList.push_back( kReinforceCash );
	}

	// TradeType
	int iTradeTypeCnt = rkLoader.LoadInt( "tradetype_cnt", 0 );
	if( iTradeTypeCnt > 0 )
	{
		m_vTradeTypeList.clear();
		m_vTradeTypeList.reserve( iTradeTypeCnt );
	}

	for( int m=0; m < iTradeTypeCnt; ++m )
	{
		RandomTradeType kTradeType;

		for( int n=0; n < TRADE_TYPE_MAX; ++n )
		{
			wsprintf( szKey1, "tradetype%d_rate%d", m+1, n+1 );
			kTradeType.m_iRandomRate[n] = rkLoader.LoadInt( szKey1, 0 );

			wsprintf( szKey2, "tradetype%d_value%d", m+1, n+1 );
			kTradeType.m_iTradeType[n] = rkLoader.LoadInt( szKey2, 0 );

			kTradeType.m_dwTotalRate += kTradeType.m_iRandomRate[n];
		}

		m_vTradeTypeList.push_back( kTradeType );
	}

	// test
	if( !g_ExtraItemInfoMgr.IsTestLOG() )
		return;

	int iCnt = 0;
	for(RandomItemList::iterator iter = m_vRandomItemList.begin(); iter != m_vRandomItemList.end(); ++iter)
	{
		iCnt++;
		RandomItem &rkItem = (*iter);
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Item%d : %d, %d, %d", iCnt, rkItem.m_iItemCode, rkItem.m_iRandomRate, rkItem.m_iTradeTypeList );
	}
	RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Item TotalRate: %d", m_dwTotalItemRate );

	iCnt = 0;
	for(RandomPeriodTimeList::iterator iter = m_vPeriodList.begin(); iter != m_vPeriodList.end(); ++iter)
	{
		iCnt++;
		RandomPeriodTime &rkPeriodTime = (*iter);
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Period%d : %d, %d, %d", iCnt, rkPeriodTime.m_iPeriodTime, rkPeriodTime.m_iRandomRate, (int) rkPeriodTime.m_bAlarm );
	}
	RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Period TotalRate: %d", m_dwTotalPeriodRate );

	iCnt = 0;
	for(RandomReinforceList::iterator iter = m_vReinforceList.begin(); iter != m_vReinforceList.end(); ++iter)
	{
		iCnt++;
		RandomReinforce &rkReinforce = (*iter);
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Reinforce%d : %d, %d", iCnt, rkReinforce.m_iReinforce, rkReinforce.m_iRandomRate );
	}
	RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Reinforce TotalRate: %d", m_dwTotalReinforceRate );

	iCnt = 0;
	for(RandomReinforceList::iterator iter = m_vReinforceCashList.begin(); iter != m_vReinforceCashList.end(); ++iter)
	{
		iCnt++;
		RandomReinforce &rkReinforceCash = (*iter);
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ReinforceCash%d : %d, %d", iCnt, rkReinforceCash.m_iReinforce, rkReinforceCash.m_iRandomRate );
	}
	RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Reinforce Cash TotalRate: %d", m_dwTotalReinforceCashRate );

	iCnt = 0;
	for(RandomTradeTypeList::iterator iter = m_vTradeTypeList.begin(); iter != m_vTradeTypeList.end(); ++iter)
	{
		iCnt++;
		RandomTradeType &rkTradeType = (*iter);
		for( int n=0; n < TRADE_TYPE_MAX; ++n )
		{
			RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TradeType%d : %d, %d, %d", iCnt, n+1, rkTradeType.m_iTradeType[n], rkTradeType.m_iRandomRate[n] );
		}
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TradeType%d TotalRate: %d", iCnt, rkTradeType.m_dwTotalRate );
	}
}

void ioRandomMachineGroup::LoadMachineBasicInfo( ioINILoader &rkLoader )
{
	int iMachineCode = rkLoader.LoadInt( "machine_code", -1 );
	if( iMachineCode != -1 )
		m_iMachineCode = iMachineCode;

	int iNeedLevel   = rkLoader.LoadInt( "need_level", -1 );
	if( iNeedLevel != -1 )
		m_iNeedLevel = iNeedLevel;

	int iPackage     = rkLoader.LoadInt( "package", -1 );
	if( iPackage != -1 )
	{
		if( iPackage == 0 )
			m_bPackage = false;
		else
			m_bPackage = true;
	}

	int iNeedCash    = rkLoader.LoadInt( "need_cash", -1 );
	if( iNeedCash != -1 )
		m_iNeedCash = iNeedCash;

	int iBonusPeso	 = rkLoader.LoadInt( "bonus_peso", -1 );
	if( iBonusPeso != -1 )
		m_iBonusPeso = iBonusPeso;

	int iSellPeso    = rkLoader.LoadInt( "sell_peso", -1 );	
	if( iSellPeso != -1 )
		m_iSellPeso = iSellPeso;

	m_iSubscriptionType = rkLoader.LoadInt( "subscription_type", SUBSCRIPTION_NONE );

	enum 
	{
		MAX_LOOP = 100,
	};

	char szName[MAX_PATH]="";
	int iNeedPeso  = 0; 
	int iPeriod    = 0;

	for (int i = 0; i < MAX_LOOP ; i++)
	{
		g_SaleMgr.LoadINI( true, rkLoader, ioSaleManager::IT_EXTRA_BOX, m_iMachineCode, i, i );

		StringCbPrintf( szName, sizeof( szName ), "need_peso%d", i+1 );
		iNeedPeso = rkLoader.LoadInt( szName, -1 );

		StringCbPrintf( szName, sizeof( szName ), "period%d", i+1 );
		iPeriod      = rkLoader.LoadInt( szName, -1 );	

		if( iNeedPeso == -1 && iPeriod == -1 )
			break;

		if( i == 0 )
		{
			m_vNeedPeso.clear();
			m_vPeriod.clear();
		}

		m_vNeedPeso.push_back( iNeedPeso );
		m_vPeriod.push_back( iPeriod );
	}

	// test
	if( !g_ExtraItemInfoMgr.IsTestLOG() )
		return;
	RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "MachineCode: %d", m_iMachineCode );
	RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "NeedLevel: %d", m_iNeedLevel );
	RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "bPackage: %d", (int)m_bPackage );
	RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "NeedCash: %d", m_iNeedCash );
	RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BonusPeso: %d", m_iBonusPeso );
	RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SellPeso: %d", m_iSellPeso );

	int iCnt = 0;
	for(IntVec::iterator iter = m_vNeedPeso.begin(); iter != m_vNeedPeso.end(); ++iter)
	{
		iCnt++;
	    int &riValue = (*iter);
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "NeedPeso%d: %d", iCnt, riValue );
	}

	iCnt = 0;
	for(IntVec::iterator iter = m_vPeriod.begin(); iter != m_vPeriod.end(); ++iter)
	{
		iCnt++;
		int &riValue = (*iter);
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Period%d: %d", iCnt, riValue );
	}
}

int ioRandomMachineGroup::GetRandomItemCode( IN int iItemRand, OUT int &iList , IN int iMachineCode)
{
	int iItemIndex = 0;
	int iCurPosition = 0;

	int iItemCnt = m_vRandomItemList.size();
	for( int i=0; i < iItemCnt; ++i )
	{
		int iCurRate = m_vRandomItemList[i].m_iRandomRate;
		if( COMPARE( iItemRand, iCurPosition, iCurPosition+iCurRate ) )
		{
			iList = m_vRandomItemList[i].m_iTradeTypeList;
			//유니크 장비보급 관련 임시 하드 코딩
			if( iMachineCode == 20153 ||  iMachineCode == 20190 || iMachineCode == 20191 || iMachineCode == 20192 )
			{
				// 위 머신코드는 정상 처리.
			}
			else
			{
				const ItemInfo* pItemInfo = g_ItemInfoMgr.GetItemInfo( m_vRandomItemList[i].m_iItemCode );
				if ( !pItemInfo || pItemInfo->m_iGradeType == 4)
				{
					i = i / 2;
					iItemRand = iItemRand / 2;
					iCurPosition = iCurPosition / 2;
					continue;
				}
			}
			return m_vRandomItemList[i].m_iItemCode;
		}
		iCurPosition += iCurRate;
	}
	return 0;
}

int ioRandomMachineGroup::GetRandomPeriod( int iPeriodRand )
{
	int iCurPosition = 0;

	int iGroupCnt = m_vPeriodList.size();
	for( int i=0; i < iGroupCnt; ++i )
	{
		int iCurRate = m_vPeriodList[i].m_iRandomRate;
		if( COMPARE( iPeriodRand, iCurPosition, iCurPosition+iCurRate ) )
		{
			return m_vPeriodList[i].m_iPeriodTime;
		}

		iCurPosition += iCurRate;
	}

	return -1;
}

int ioRandomMachineGroup::GetRandomReinforce( int iReinforceRand )
{
	int iCurPosition = 0;

	int iGroupCnt = m_vReinforceList.size();
	for( int i=0; i < iGroupCnt; ++i )
	{
		int iCurRate = m_vReinforceList[i].m_iRandomRate;
		if( COMPARE( iReinforceRand, iCurPosition, iCurPosition+iCurRate ) )
		{
			return m_vReinforceList[i].m_iReinforce;
		}

		iCurPosition += iCurRate;
	}

	return 0;
}

int ioRandomMachineGroup::GetRandomReinforceCash( int iReinforceCashRand )
{
	int iCurPosition = 0;

	int iGroupCnt = m_vReinforceCashList.size();
	for( int i=0; i < iGroupCnt; ++i )
	{
		int iCurRate = m_vReinforceCashList[i].m_iRandomRate;
		if( COMPARE( iReinforceCashRand, iCurPosition, iCurPosition+iCurRate ) )
		{
			return m_vReinforceCashList[i].m_iReinforce;
		}

		iCurPosition += iCurRate;
	}

	return 0;
}

int ioRandomMachineGroup::GetRandomTradeType( int iList, int iTradeTypeRand )
{
	int iIndex = iList - 1;
	if( !COMPARE( iIndex, 0, (int)m_vTradeTypeList.size() ) )
		return 0;

	int iCurPosition = 0;

	for( int i=0; i < TRADE_TYPE_MAX; ++i )
	{
		int iCurRate = m_vTradeTypeList[iIndex].m_iRandomRate[i];
		if( iCurRate > 0 && COMPARE( iTradeTypeRand, iCurPosition, iCurPosition+iCurRate ) )
		{
			return m_vTradeTypeList[iIndex].m_iTradeType[i];
		}

		iCurPosition += iCurRate;
	}

	return 0;
}

bool ioRandomMachineGroup::IsAlarm( int iPeriodTime )
{
	int iGroupCnt = m_vPeriodList.size();
	for( int i=0; i < iGroupCnt; ++i )
	{
		RandomPeriodTime &kInfo = m_vPeriodList[i];
		if( kInfo.m_iPeriodTime == iPeriodTime )
		{
			return kInfo.m_bAlarm;
		}
	}

	return false;
}


bool ioRandomMachineGroup::IsReinforceCash()
{
	if( !m_vReinforceCashList.empty() )
		return true;

	return false;
}

DWORD ioRandomMachineGroup::GetTotalTradeTypeRate( int iList )
{
	int iIndex = iList - 1;
	if( COMPARE( iIndex, 0, (int)m_vTradeTypeList.size() ) )
	{
		return m_vTradeTypeList[iIndex].m_dwTotalRate;
	}

	return 0;
}

int ioRandomMachineGroup::GetNeedPeso( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int)m_vNeedPeso.size() ) )
		return 0;

	return m_vNeedPeso[iArray];
}

int ioRandomMachineGroup::GetPeriod( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int)m_vPeriod.size() ) )
		return -1;

	return m_vPeriod[iArray];
}

void ioRandomMachineGroup::SetNeedPeso( int iNeedPeso, int iArray )
{
	if( !COMPARE( iArray, 0, (int)m_vNeedPeso.size() ) )
		return;

	m_vNeedPeso[iArray] = iNeedPeso;
}

void ioRandomMachineGroup::SetNeedCash( int iNeedCash )
{
	m_iNeedCash = iNeedCash;
}

void ioRandomMachineGroup::SetActive( bool bActive )
{
	m_bActive = bActive;
}
//////////////////////////////////////////////////////////////////////////////////////////
template<> ioExtraItemInfoManager* Singleton< ioExtraItemInfoManager >::ms_Singleton = 0;

ioExtraItemInfoManager::ioExtraItemInfoManager()
{
	m_TotalRandom.SetRandomSeed( timeGetTime() );

	m_ItemGroupRandom.SetRandomSeed( timeGetTime()+1 );
	m_ItemListRandom.SetRandomSeed( timeGetTime()+2 );
	m_PeriodTimeRandom.SetRandomSeed( timeGetTime()+3 );
	m_ReinforceRandom.SetRandomSeed( timeGetTime()+4 );
	m_TradeTypeRandom.SetRandomSeed( timeGetTime()+5 );

	m_bTestRandomItem = false;
	
	m_fItemSellConstRate = 0.0f;
}

ioExtraItemInfoManager::~ioExtraItemInfoManager()
{
	ClearAllInfo();
}

void ioExtraItemInfoManager::CheckNeedReload()
{
	ioINILoader kLoader;
	kLoader.SetFileName( "config/sp2_extraitem_info.ini" );
	if( kLoader.ReadBool( "common_info", "Change", false ) )
	{
		if( kLoader.ReloadFile( "config/sp2_extraitem_info.ini" ) == false )
		{
			LOG.PrintTimeAndLog( 0, "ERROR] config/sp2_extraitem_info.ini ReLoad Fail..." );
			return;
		}
		LoadRandomExtraItemInfo( kLoader );
	}
}

void ioExtraItemInfoManager::LoadAllExtraItemInfo()
{
	ioINILoader kLoader( "config/sp2_extraitem_info.ini" );
	LoadRandomExtraItemInfo( kLoader );
}

void ioExtraItemInfoManager::LoadRandomExtraItemInfo( ioINILoader &rkLoader )
{
	ClearAllInfo();

	
	rkLoader.SetTitle( "common_info" );

	// 임시 주석 Change값 변경 못하게.. 13.01.30. kjh
	//rkLoader.SaveBool( "Change", false );

	m_iLevelLimitConst = rkLoader.LoadInt( "item_limit_const", 0);
	m_fItemSellConstRate = rkLoader.LoadFloat( "mortmain_item_sell_rate", 0.0f );
	m_fMortmainItemSell = rkLoader.LoadFloat( "mortmain_item_sell", 0.0f );
	m_fTimeItemSell = rkLoader.LoadFloat( "time_item_sell", 0.0f );
	m_bTestRandomItem = rkLoader.LoadBool( "test_random_item", false );

	m_iDefaultExtraItemCount = rkLoader.LoadInt( "default_limited_cnt", 500);

	if( m_bTestRandomItem )
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LoadRandomExtraItemInfo INI Start!!!!!!!!!!" );

	int iCnt = rkLoader.LoadInt( "random_machine_cnt", 0 );

	enum 
	{
		MAX_LOOP = 100,
	};
	char szTitle[MAX_PATH]="";
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		StringCbPrintf( szTitle, sizeof( szTitle ), "default%d", i+1 );
		rkLoader.SetTitle(szTitle);
		int iDefaultType = rkLoader.LoadInt( "type", 0 );
		if( iDefaultType == 0 )
			break;

		ioRandomMachineGroup *pGroup = new ioRandomMachineGroup;
		if( !pGroup )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - %s pRandomMachine == NULL.", __FUNCTION__, rkLoader.GetTitle() );
			break;
		}
		if( m_bTestRandomItem )
			RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Default %d Info Start>>>>>>>>>>>>>>>>", iDefaultType );
		pGroup->LoadMachineBasicInfo( rkLoader );
		pGroup->LoadMachineRandomInfo( rkLoader, true );	
		pGroup->m_eDefaultType = (ioRandomMachineGroup::MachineType) iDefaultType;
		m_vDefaultMachineGroupList.push_back( pGroup );
		if( m_bTestRandomItem )
			RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Default %d Info End<<<<<<<<<<<<<<<<<<", iDefaultType );
	}

	LOOP_GUARD();
	m_vMachineGroupList.reserve( iCnt );
	for( int i=0; i < iCnt; i++ )
	{
		wsprintf( szTitle, "random_machine%d", i+1 );
		rkLoader.SetTitle( szTitle );
		int iMachineCode = rkLoader.LoadInt( "machine_code", 0 );
		if( iMachineCode == 0 )
		{
			continue;
		}

		ioRandomMachineGroup *pGroup = new ioRandomMachineGroup;
		if( !pGroup )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - %s pRandomMachine == NULL.", __FUNCTION__, rkLoader.GetTitle() );
			break;
		}
		if( m_bTestRandomItem )
			RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Machine %d Info Start>>>>>>>>>>>>>>>>", iMachineCode );

		CopyDefaultMachineBasicInfo( rkLoader, pGroup );
		pGroup->LoadMachineBasicInfo( rkLoader );
		bool bLoaded = CopyDefaultMachineRandomInfo( rkLoader, pGroup );
		pGroup->LoadMachineRandomInfo( rkLoader, !bLoaded );
		m_MachineGroupMap.insert( RandomMachineGroupMap::value_type(iMachineCode, pGroup) );
		m_vMachineGroupList.push_back( pGroup );

		if( m_bTestRandomItem )
			RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Machine %d Info End<<<<<<<<<<<<<<<<<<", iMachineCode );
	}
	LOOP_GUARD_CLEAR();

	if( m_bTestRandomItem )
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LoadRandomExtraItemInfo INI End!!!!!!!!!!" );

	//
	if( m_bTestRandomItem )
	{
		CheckTestRandomItem();
	}
	//
}

void ioExtraItemInfoManager::CopyDefaultMachineBasicInfo( IN ioINILoader &rkLoader, OUT ioRandomMachineGroup *pGroup )
{
	if( !pGroup )
		return;

	ioRandomMachineGroup::MachineType eType = (ioRandomMachineGroup::MachineType)rkLoader.LoadInt( "type", 0 );
	if( eType == MT_NONE )
		return;

	if( m_vDefaultMachineGroupList.empty() )
		return;

	ioRandomMachineGroup::MachineType eCheckType = ioRandomMachineGroup::MT_NONE;
	if( COMPARE( (int) eType, ioRandomMachineGroup::MT_WEAPON, ioRandomMachineGroup::MT_RARE_ALL+1 ) )
	{
		eCheckType = ioRandomMachineGroup::MT_ALL;
	}
	else if( eType == ioRandomMachineGroup::MT_HIGH_EXTRA )
	{
		eCheckType = ioRandomMachineGroup::MT_HIGH_EXTRA;
	}
	else if( eType == ioRandomMachineGroup::MT_LIMITED_EXTRA )
	{
		eCheckType = ioRandomMachineGroup::MT_LIMITED_EXTRA;
	}

	for(RandomMachineGroupList::iterator iter = m_vDefaultMachineGroupList.begin(); iter != m_vDefaultMachineGroupList.end(); ++iter)
	{
		ioRandomMachineGroup *pDefaultGroup = (*iter);
		if( !pDefaultGroup )
			continue;
		if( pDefaultGroup->GetDefaultType() != eCheckType )
			continue;

		pGroup->m_iMachineCode = pDefaultGroup->m_iMachineCode;
		pGroup->m_iNeedLevel   = pDefaultGroup->m_iNeedLevel;
		pGroup->m_bPackage     = pDefaultGroup->m_bPackage;
		pGroup->m_iNeedCash    = pDefaultGroup->m_iNeedCash;
		pGroup->m_iBonusPeso   = pDefaultGroup->m_iBonusPeso;
		pGroup->m_iSellPeso    = pDefaultGroup->m_iSellPeso;


		if( !pDefaultGroup->m_vNeedPeso.empty() )
			pGroup->m_vNeedPeso = pDefaultGroup->m_vNeedPeso;

		if( !pDefaultGroup->m_vPeriod.empty() )
			pGroup->m_vPeriod = pDefaultGroup->m_vPeriod;

		return; //////////////////////////////////////////////////////
	}
}

bool ioExtraItemInfoManager::CopyDefaultMachineRandomInfo( IN ioINILoader &rkLoader, OUT ioRandomMachineGroup *pGroup )
{
	if( !pGroup )
		return false;

	ioRandomMachineGroup::MachineType eType = (ioRandomMachineGroup::MachineType)rkLoader.LoadInt( "type", 0 );
	if( eType == MT_NONE )
		return false;

	if( m_vDefaultMachineGroupList.empty() )
		return false;

	ioRandomMachineGroup::MachineType eCheckType = ioRandomMachineGroup::MT_NONE;
	if( COMPARE( (int) eType, ioRandomMachineGroup::MT_WEAPON, ioRandomMachineGroup::MT_RARE_ALL+1 ) )
	{
		eCheckType = ioRandomMachineGroup::MT_ALL;
	}
	else if( eType == ioRandomMachineGroup::MT_HIGH_EXTRA )
	{
		eCheckType = ioRandomMachineGroup::MT_HIGH_EXTRA;
	}
	else if( eType == ioRandomMachineGroup::MT_LIMITED_EXTRA )
	{
		eCheckType = ioRandomMachineGroup::MT_LIMITED_EXTRA;
	}

	for(RandomMachineGroupList::iterator iter = m_vDefaultMachineGroupList.begin(); iter != m_vDefaultMachineGroupList.end(); ++iter)
	{
		ioRandomMachineGroup *pDefaultGroup = (*iter);
		if( !pDefaultGroup )
			continue;
		if( pDefaultGroup->GetDefaultType() != eCheckType )
			continue;

		// item
		enum { MIN_ITEM_SIZE = 100, };
		pGroup->m_vRandomItemList.clear();
		pGroup->m_vRandomItemList.reserve( MIN_ITEM_SIZE );
		pGroup->m_dwTotalItemRate = 0;

		for(RandomItemList::iterator iter2 = pDefaultGroup->m_vRandomItemList.begin(); iter2 != pDefaultGroup->m_vRandomItemList.end(); ++iter2)
		{
			RandomItem kItem = (*iter2);
			if( !IsRightExtraItem( eType, kItem.m_iItemCode ) )
				continue;
			pGroup->m_dwTotalItemRate += kItem.m_iRandomRate;
			pGroup->m_vRandomItemList.push_back( kItem );
		}

		// period
		pGroup->m_vPeriodList.clear();
		pGroup->m_vPeriodList.reserve( pDefaultGroup->m_vPeriodList.size() );
		pGroup->m_dwTotalPeriodRate = 0;
		for(RandomPeriodTimeList::iterator iter2 = pDefaultGroup->m_vPeriodList.begin(); iter2 != pDefaultGroup->m_vPeriodList.end(); ++iter2)
		{
			RandomPeriodTime kPeriodTime = (*iter2);
			pGroup->m_dwTotalPeriodRate += kPeriodTime.m_iRandomRate;
			pGroup->m_vPeriodList.push_back( kPeriodTime );
		}
		
		// reinforce
		pGroup->m_vReinforceList.clear();
		pGroup->m_vReinforceList.reserve( pDefaultGroup->m_vReinforceList.size() );
		pGroup->m_dwTotalReinforceRate = 0;
		for(RandomReinforceList::iterator iter2 = pDefaultGroup->m_vReinforceList.begin(); iter2 != pDefaultGroup->m_vReinforceList.end(); ++iter2)
		{
			RandomReinforce kReinforce = (*iter2);
			pGroup->m_dwTotalReinforceRate += kReinforce.m_iRandomRate;
			pGroup->m_vReinforceList.push_back( kReinforce );
		}

		// reinforce cash
		pGroup->m_vReinforceCashList.clear();
		pGroup->m_vReinforceCashList.reserve( pDefaultGroup->m_vReinforceCashList.size() );
		pGroup->m_dwTotalReinforceCashRate = 0;
		for(RandomReinforceList::iterator iter2 = pDefaultGroup->m_vReinforceCashList.begin(); iter2 != pDefaultGroup->m_vReinforceCashList.end(); ++iter2)
		{
			RandomReinforce kReinforceCash = (*iter2);
			pGroup->m_dwTotalReinforceCashRate += kReinforceCash.m_iRandomRate;
			pGroup->m_vReinforceCashList.push_back( kReinforceCash );
		}

		// TradeType
		pGroup->m_vTradeTypeList.clear();
		pGroup->m_vTradeTypeList.reserve( pDefaultGroup->m_vTradeTypeList.size() );
		for(RandomTradeTypeList::iterator iter2 = pDefaultGroup->m_vTradeTypeList.begin(); iter2 != pDefaultGroup->m_vTradeTypeList.end(); ++iter2)
		{
			RandomTradeType kTradeType = (*iter2);
			pGroup->m_vTradeTypeList.push_back( kTradeType );
		}

		return true; //////////////////////////////////////////////////////
	}

	return false;
}

void ioExtraItemInfoManager::CopyDefaultMachineAll( IN ioRandomMachineGroup::MachineType eType, OUT RandomInfo &rkInfo, OUT DWORD &rdwTotalItemRate, OUT DWORD &rdwTotalPeriodRate, OUT DWORD &rdwTotalReinforceRate, IN DWORD dwItemCode )
{
	if( m_vDefaultMachineGroupList.empty() )
		return;

	for(RandomMachineGroupList::iterator iter = m_vDefaultMachineGroupList.begin(); iter != m_vDefaultMachineGroupList.end(); ++iter)
	{
		ioRandomMachineGroup *pDefaultGroup = (*iter);
		if( !pDefaultGroup )
			continue;
		if( pDefaultGroup->GetDefaultType() != ioRandomMachineGroup::MT_ALL )
			continue;

		// item
		enum { MIN_ITEM_SIZE = 100, };
		rkInfo.m_vRandomItemList.clear();
		rkInfo.m_vRandomItemList.reserve( MIN_ITEM_SIZE );

		for(RandomItemList::iterator iter2 = pDefaultGroup->m_vRandomItemList.begin(); iter2 != pDefaultGroup->m_vRandomItemList.end(); ++iter2)
		{
			RandomItem kItem = (*iter2);
			if( !IsRightExtraItem( eType, kItem.m_iItemCode ) )
				continue;

			if( dwItemCode == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND4 ) //유니크 합성기 일 경우, 랜덤 아이템에 유니크 장비만 넣자.
			{
				const ItemInfo* pItemInfo = g_ItemInfoMgr.GetItemInfo( kItem.m_iItemCode );
				if ( !pItemInfo || pItemInfo->m_iGradeType != 4)
				{
					continue;
				}
			}
			else if( dwItemCode ==  ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 || //상급 합성기
				dwItemCode == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND5 ) //부위별 상급 합성기
			{	
			}
			else
			{
				const ItemInfo* pItemInfo = g_ItemInfoMgr.GetItemInfo( kItem.m_iItemCode );
				if ( !pItemInfo || pItemInfo->m_iGradeType == 4)
				{
					continue;
				}
			}
			rdwTotalItemRate += kItem.m_iRandomRate;
			rkInfo.m_vRandomItemList.push_back( kItem );
		}

		// period
		rkInfo.m_vPeriodList.clear();
		rkInfo.m_vPeriodList.reserve( pDefaultGroup->m_vPeriodList.size() );
		for(RandomPeriodTimeList::iterator iter2 = pDefaultGroup->m_vPeriodList.begin(); iter2 != pDefaultGroup->m_vPeriodList.end(); ++iter2)
		{
			RandomPeriodTime kPeriodTime = (*iter2);
			rdwTotalPeriodRate += kPeriodTime.m_iRandomRate;
			rkInfo.m_vPeriodList.push_back( kPeriodTime );
		}

		// reinforce
		rkInfo.m_vReinforceList.clear();
		rkInfo.m_vReinforceList.reserve( pDefaultGroup->m_vReinforceList.size() );
		for(RandomReinforceList::iterator iter2 = pDefaultGroup->m_vReinforceList.begin(); iter2 != pDefaultGroup->m_vReinforceList.end(); ++iter2)
		{
			RandomReinforce kReinforce = (*iter2);
			rdwTotalReinforceRate += kReinforce.m_iRandomRate;
			rkInfo.m_vReinforceList.push_back( kReinforce );
		}

		return; ///////////////////////////////////////////////////////////
	}
}

bool ioExtraItemInfoManager::IsRightExtraItem( ioRandomMachineGroup::MachineType eType, int iItemCode )
{
	if( iItemCode <= 0 )
		return false;

	if( eType == ioRandomMachineGroup::MT_ALL )
		return true;
	else if( eType == ioRandomMachineGroup::MT_WEAPON )
	{
		if( (iItemCode/100000) == 0 )
			return true;
	}
	else if( eType == ioRandomMachineGroup::MT_ARMOR )
	{
		if( (iItemCode/100000) == 1 )
			return true;
	}
	else if( eType == ioRandomMachineGroup::MT_HELMET )
	{
		if( (iItemCode/100000) == 2 )
			return true;
	}
	else if( eType == ioRandomMachineGroup::MT_CLOAK )
	{
		if( (iItemCode/100000) == 3 )
			return true;
	}
	else if( eType == ioRandomMachineGroup::MT_RARE_ALL )
	{
		if( (iItemCode/1000)%10 == 1 )
			return true;
	}
	else if( eType == ioRandomMachineGroup::MT_RARE_WEAPON )
	{
		if( (iItemCode/1000)%10 == 1 )
		{
			if( (iItemCode/100000) == 0 )
				return true;
		}
	}
	else if( eType == ioRandomMachineGroup::MT_RARE_ARMOR )
	{
		if( (iItemCode/1000)%10 == 1 )
		{
			if( (iItemCode/100000) == 1 )
				return true;
		}
	}
	else if( eType == ioRandomMachineGroup::MT_RARE_HELMET )
	{
		if( (iItemCode/1000)%10 == 1 )
		{
			if( (iItemCode/100000) == 2 )
				return true;
		}
	}
	else if( eType == ioRandomMachineGroup::MT_RARE_CLOAK )
	{
		if( (iItemCode/1000)%10 == 1 )
		{
			if( (iItemCode/100000) == 3 )
				return true;
		}
	}
	else if( eType == ioRandomMachineGroup::MT_HIGH_EXTRA )
		return false;

	return false;
}

void ioExtraItemInfoManager::ClearAllInfo()
{
	m_MachineGroupMap.clear();

	//
	RandomMachineGroupList::iterator iter, iEnd;
	iEnd = m_vMachineGroupList.end();
	for( iter=m_vMachineGroupList.begin() ; iter!=iEnd ; ++iter )
	{
		delete *iter;
	}
	
	m_vMachineGroupList.clear();

	//
	iEnd = m_vDefaultMachineGroupList.end();
	for( iter=m_vDefaultMachineGroupList.begin() ; iter!=iEnd ; ++iter )
	{
		delete *iter;
	}

	m_vDefaultMachineGroupList.clear();
}

int ioExtraItemInfoManager::GetNeedPeso( int iMachineCode, int iArray )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		return iter->second->GetNeedPeso( iArray );
	}

	return -1;
}

int ioExtraItemInfoManager::GetNeedCash( int iMachineCode )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		return iter->second->GetNeedCash();
	}

	return -1;
}

int ioExtraItemInfoManager::GetSubscriptionType( int iMachineCode )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		return iter->second->GetSubscriptionType();
	}

	return SUBSCRIPTION_NONE;
}

int ioExtraItemInfoManager::GetBonusPeso( int iMachineCode )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		return iter->second->GetBonusPeso();
	}

	return -1;
}

int ioExtraItemInfoManager::GetNeedLevel( int iCode )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iCode );
	if( iter != m_MachineGroupMap.end() )
	{
		return iter->second->GetNeedLevel();
	}

	return -1;
}

int ioExtraItemInfoManager::GetSellPeso( int iMachineCode )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		return iter->second->GetSellPeso();
	}

	return -1;
}

bool ioExtraItemInfoManager::GetActive( int iMachineCode )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		return iter->second->GetActive();
	}

	return false;
}

int ioExtraItemInfoManager::GetPeriod( int iMachineCode, int iArray )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		return iter->second->GetPeriod( iArray );
	}

	return -1;
}

int ioExtraItemInfoManager::GetRandomItemCode( IN int iCode, OUT int &iList )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iCode );
	if( iter != m_MachineGroupMap.end() )
	{
		DWORD dwTotalRate = iter->second->GetTotalItemRate();
		int iItemRand = m_ItemListRandom.Random( dwTotalRate );

		int iItemCode = iter->second->GetRandomItemCode( iItemRand, iList , iCode);
		return iItemCode;
	}

	return 0;
}

int ioExtraItemInfoManager::GetRandomPeriodTime( int iCode )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iCode );
	if( iter != m_MachineGroupMap.end() )
	{
		DWORD dwTotalRate = iter->second->GetTotalPeriodRate();
		int iPeriodRand = m_PeriodTimeRandom.Random( dwTotalRate );
		
		return iter->second->GetRandomPeriod( iPeriodRand );
	}

	return -1;
}

int ioExtraItemInfoManager::GetRandomReinforce( int iCode, bool bCash )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iCode );
	if( iter != m_MachineGroupMap.end() )
	{
		if( bCash && iter->second->IsReinforceCash() )
		{
			DWORD dwTotalCashRate = iter->second->GetTotalReinforceCashRate();
			int iReinforceCashRand = m_ReinforceRandom.Random( dwTotalCashRate );
			return iter->second->GetRandomReinforceCash( iReinforceCashRand );
		}
		else
		{
			DWORD dwTotalRate = iter->second->GetTotalReinforceRate();
			int iReinforceRand = m_ReinforceRandom.Random( dwTotalRate );
			return iter->second->GetRandomReinforce( iReinforceRand );
		}
	}

	return 0;
}

int ioExtraItemInfoManager::GetRandomTradeType( int iMachineCode, int iList )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		DWORD dwTotalRate = iter->second->GetTotalTradeTypeRate( iList );
		int iTradeTypeRand = m_TradeTypeRandom.Random( dwTotalRate );

		return iter->second->GetRandomTradeType( iList, iTradeTypeRand );
	}

	return 0;
}

void ioExtraItemInfoManager::CheckTestRandomItem()
{
	if( !m_bTestRandomItem ) return;

	LOOP_GUARD();
	RandomMachineGroupList::iterator iter = m_vMachineGroupList.begin();
	while( iter != m_vMachineGroupList.end() )
	{
		ioRandomMachineGroup *pGroup = *iter;
		if( pGroup )
		{
			int iMachineCode = pGroup->GetMachineCode();

			typedef std::map< int, int > InfoMap;
			InfoMap eItemMap, ePeriodMap, eReinforceMap, eReinforceCashMap, eTradeTypeMap;

			typedef std::map< int, ItemTestInfo > TradeInfoMap;
			TradeInfoMap eTradeInfoMap;

			for( int i=0; i < 1000000; ++i )
			{
				int iTradeTypeList = 0;
				int iItemCode = GetRandomItemCode( iMachineCode, iTradeTypeList );
				InfoMap::iterator iter_i = eItemMap.find( iItemCode );
				if( iter_i != eItemMap.end() )
					iter_i->second += 1;
				else
					eItemMap.insert( InfoMap::value_type(iItemCode, 1) );

				int iPeriodTime = GetRandomPeriodTime( iMachineCode );
				InfoMap::iterator iter_p = ePeriodMap.find( iPeriodTime );
				if( iter_p != ePeriodMap.end() )
					iter_p->second += 1;
				else
					ePeriodMap.insert( InfoMap::value_type(iPeriodTime, 1) );

				int iReinforce = GetRandomReinforce( iMachineCode, false );
				InfoMap::iterator iter_r = eReinforceMap.find( iReinforce );
				if( iter_r != eReinforceMap.end() )
					iter_r->second += 1;
				else
					eReinforceMap.insert( InfoMap::value_type(iReinforce, 1) );

				int iReinforceCash = GetRandomReinforce( iMachineCode, true );
				InfoMap::iterator iter_rc = eReinforceCashMap.find( iReinforceCash );
				if( iter_rc != eReinforceCashMap.end() )
					iter_rc->second += 1;
				else
					eReinforceCashMap.insert( InfoMap::value_type(iReinforceCash, 1) );

				int iTradeType = GetRandomTradeType( iMachineCode, iTradeTypeList );
				InfoMap::iterator iter_t = eTradeTypeMap.find( iTradeType );
				if( iter_t != eTradeTypeMap.end() )
					iter_t->second += 1;
				else
					eTradeTypeMap.insert( InfoMap::value_type(iTradeType, 1) );


				TradeInfoMap::iterator iter_ti = eTradeInfoMap.find( iItemCode );
				if( iter_ti != eTradeInfoMap.end() )
				{
					iter_ti->second.m_iCnt += 1;
					iter_ti->second.m_iTypeList = iTradeTypeList;

					switch(iTradeType)
					{
					case ioUserExtraItem::EET_DISABLE:
						iter_ti->second.m_iTradeDisable += 1;
						break;
					case ioUserExtraItem::EET_NORMAL:
						iter_ti->second.m_iTradeNormal += 1;
						break;
					case ioUserExtraItem::EET_ENABLE:
						iter_ti->second.m_iTradeEnable += 1;
						break;
					}
				}
				else
				{
					ItemTestInfo kInfo;
					kInfo.m_iCnt = 1;
					kInfo.m_iTypeList = iTradeTypeList;

					switch(iTradeType)
					{
					case ioUserExtraItem::EET_DISABLE:
						kInfo.m_iTradeDisable = 1;
						break;
					case ioUserExtraItem::EET_NORMAL:
						kInfo.m_iTradeNormal = 1;
						break;
					case ioUserExtraItem::EET_ENABLE:
						kInfo.m_iTradeEnable = 1;
						break;
					}

					eTradeInfoMap.insert( TradeInfoMap::value_type(iItemCode, kInfo) );
				}
			}

			InfoMap::iterator iter_i = eItemMap.begin();
			for( ; iter_i != eItemMap.end(); ++iter_i )
			{
				int iItemCode = iter_i->first;
				float fRate = (float) iter_i->second / 1000000 * 100.0f;
				RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TestRandomItem(%d) - %d : %f", iMachineCode, iItemCode, fRate );
			}

			InfoMap::iterator iter_p = ePeriodMap.begin();
			for( ; iter_p != ePeriodMap.end(); ++iter_p )
			{
				int iPeriod = iter_p->first;
				float fRate = (float) iter_p->second / 1000000 * 100.0f;
				RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TestRandomPeriod(%d) - %d : %f", iMachineCode, iPeriod, fRate );
			}

			InfoMap::iterator iter_r = eReinforceMap.begin();
			for( ; iter_r != eReinforceMap.end(); ++iter_r )
			{
				int iReinforce = iter_r->first;
				float fRate = (float) iter_r->second / 1000000 * 100.0f;
				RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TestRandomReinforce(%d) - %d : %f", iMachineCode, iReinforce, fRate );
			}

			InfoMap::iterator iter_rc = eReinforceCashMap.begin();
			for( ; iter_rc != eReinforceCashMap.end(); ++iter_rc )
			{
				int iReinforceCash = iter_rc->first;
				float fRateCash = (float) iter_rc->second / 1000000 * 100.0f;
				RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TestRandomReinforceCash(%d) - %d : %f", iMachineCode, iReinforceCash, fRateCash );
			}

			InfoMap::iterator iter_t = eTradeTypeMap.begin();
			for( ; iter_t != eTradeTypeMap.end(); ++iter_t )
			{
				int iTradeType = iter_t->first;
				float fRate = (float) iter_t->second / 1000000 * 100.0f;
				RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TestRandomTradeTypeTotal(%d) - %d : %f", iMachineCode, iTradeType, fRate );
			}


			TradeInfoMap::iterator iter_ti = eTradeInfoMap.begin();
			for( ; iter_ti != eTradeInfoMap.end(); ++iter_ti )
			{
				int iItemCode = iter_ti->first;
				int iCnt = iter_ti->second.m_iCnt;
				float fDisable, fNormal, fEnable;
				fDisable = fNormal = fEnable = 0.0f;

				int iDisable = iter_ti->second.m_iTradeDisable;
				int iNormal = iter_ti->second.m_iTradeNormal;
				int iEnable = iter_ti->second.m_iTradeEnable;
				int iTypeList = iter_ti->second.m_iTypeList;

				if( iCnt > 0 )
				{
					fDisable = (float)iDisable / iCnt * 100.0f;
					fNormal = (float)iNormal / iCnt * 100.0f;
					fEnable = (float)iEnable / iCnt * 100.0f;
				}

				RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TestRandomTradeTypeItem(%d) - %d(%d) : [%f/%f/%f]", iMachineCode,
																									  iItemCode,
																									  iTypeList,
																									  fDisable,
																									  fNormal,
																									  fEnable );
			}
		}

		++iter;
	}
	LOOP_GUARD_CLEAR();
}

bool ioExtraItemInfoManager::IsAlarm( int iCode, int iPeriodTime )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iCode );
	if( iter != m_MachineGroupMap.end() )
	{
		ioRandomMachineGroup *pGroup = iter->second;
		if( !pGroup )
			return false;

		return pGroup->IsAlarm( iPeriodTime );
	}

	return false;
}

bool ioExtraItemInfoManager::IsPackage( int iCode )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iCode );
	if( iter != m_MachineGroupMap.end() )
	{
		return iter->second->IsPackage();
	}

	return false;
}

void ioExtraItemInfoManager::SetNeedCash( int iMachineCode, int iNeedCash )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		iter->second->SetNeedCash( iNeedCash );
	}
}

void ioExtraItemInfoManager::SetNeedPeso( int iMachineCode, int iNeedPeso, int iArray )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		iter->second->SetNeedPeso( iNeedPeso, iArray );
	}
}

void ioExtraItemInfoManager::SetActive( int iMachineCode, bool bActive )
{
	RandomMachineGroupMap::iterator iter = m_MachineGroupMap.find( iMachineCode );
	if( iter != m_MachineGroupMap.end() )
	{
		iter->second->SetActive( bActive );
	}
}

ioExtraItemInfoManager& ioExtraItemInfoManager::GetSingleton()
{
	return Singleton< ioExtraItemInfoManager >::GetSingleton();
}

int ioExtraItemInfoManager::GetExtraItemExtendType(int iCode)
{
	int iExtendType = ( iCode % 100000 ) / 10000;
	int iRare = ( iCode % 100000 ) % 10000 / 1000;

	//강화된 장비 등급
	if(  COMPARE( iExtendType, 5, 10 ) && 0 == iRare )
		return EIET_DEFAULT_POWERUP;
	
	if(  COMPARE( iExtendType, 5, 10 ) && 0 < iRare )
		return EIET_EXTREA_POWERUP;
	
	if( COMPARE( iExtendType, 2, 5 ) )
		return EIET_SPECIAL_EXTRA;
	
	if( iExtendType == 1 && COMPARE( iRare, 5, 10) )
		return EIET_RARE_POWERUP;

	if( iExtendType == 1 && 0 < iRare )
		return EIET_RARE;

	if( iExtendType == 1 && 0 == iRare )
		return EIET_EXTRA;

	return EIET_DEFAULT;
}