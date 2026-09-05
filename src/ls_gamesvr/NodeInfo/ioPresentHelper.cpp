#include "stdafx.h"

#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "ioPresentHelper.h"
#include "ioEventUserNode.h"
#include "ioEventUserManager.h"
#include "ioItemInfoManager.h"
#include "ioDecorationPrice.h"
#include "ioEtcItemManager.h"
#include "ioExtraItemInfoManager.h"
#include <strsafe.h>
#include "../MainServerNode/MainServerNode.h"

extern CLog EventLOG;
extern CLog TradeLOG;

template<> ioPresentHelper* Singleton< ioPresentHelper >::ms_Singleton = 0;
ioPresentHelper::ioPresentHelper()
{
	m_dwProcessTime = 0;
	m_dwOneDayGoldItemSeed = 0;
	m_dwSpacialAwardSeed = 0;

	m_iEtcItemPackageAlarm = 0;
	m_iEtcItemPackagePeriod= 0;

	m_iEventAlarm = 0;
	m_iEventPeriod= 0;

	m_iBuyPresentAlarm = 0;
	m_iBuyPresentPeriod= 0;

	m_iCanUserPresentCnt = 0;
	m_iUserPresentAlarm  = 0;
	m_iUserPresentPeriod = 0;
	m_iUserPresentMent   = 0;
	m_iUserPresentBonusPesoMent = 0;
	m_iUserPresentBonusItemMent = 0;

	m_iCanUserSubscriptionCnt = 0;
	m_iUserSubscriptionPeriod = 0;

	m_dwRandomDecoPresentSeedM = 0;
	m_iRandomDecoPresentAlarmM = 0;
	m_iRandomDecoPresentMentM = 0;
	m_iRandomDecoPresentPeriodM = 0;

	m_dwRandomDecoPresentSeedW = 0;
	m_iRandomDecoPresentAlarmW = 0;
	m_iRandomDecoPresentMentW = 0;
	m_iRandomDecoPresentPeriodW = 0;

	m_iExtraItemPackageAlarm  = 0;
	m_iExtraItemPackagePeriod = 0;

	m_iTradePresentAlarm = 0;
	m_iTradeSellPresentAlarm = 0;

	m_iTradePresentPeriod = 0;
	m_iTradePresentBuyMent = 0;
	m_iTradePresentSellMent = 0;
	m_iTradePresentCancelMent = 0;
	m_iTradePresentTimeOutMent = 0;

	m_iAlchemicPresentAlarm = 0;
	m_iAlchemicPresentPeriod = 0;
	m_iAlchemicPresentSoldierMent = 0;
	m_iAlchemicPresentItemMent = 0;
	m_iAlchemicPresentPieceMent = 0;

	m_dwGuildSpecialFishingSeed	= 0;
	m_vGuildSpecialFishingList.clear();

	m_bAbuseMonsterPresent = false;
	
	//popup
	m_iUserPresentPopupNoMent = 9999;	//클라이언트와 정의한값
}

ioPresentHelper::~ioPresentHelper()
{
	m_vOneDayGoldItemDataList.clear();
	m_vDormancyUserDataList.clear();
	m_vSpacialAwardList.clear();
	m_RandTestMap.clear();
	m_MonsterPresentMap.clear();
	for(vGashaponPresentInfo::iterator iter = m_vGashaponPresentInfoList.begin(); iter != m_vGashaponPresentInfoList.end(); ++iter)
	{
		GashaponPresentInfo &rkInfo = (*iter);
		rkInfo.m_vGashaponPresentList.clear();
	}
	m_vGashaponPresentInfoList.clear();
	m_vEtcItemPackagePresent.clear();
	m_vEventPresent.clear();
	m_MonsterAwardMap.clear();
	m_vRandomDecoPresentListM.clear();
	m_vRandomDecoPresentListW.clear();

	for(vEventTypeRand::iterator iter = m_vEventTypeRand.begin(); iter != m_vEventTypeRand.end(); ++iter)
	    delete *iter;
	m_vEventTypeRand.clear();
	m_vBuyPresent.clear();
	m_vExtraItemPackagePresent.clear();
	m_vEventFishingPresent.clear();

	for(HighGradePresentMap::iterator iCreate = m_HighGradePresentMap.begin();iCreate != m_HighGradePresentMap.end();++iCreate)
	{
		HighGradePresentList &kList = iCreate->second;
		kList.m_PresentList.clear();
	}
	m_HighGradePresentMap.clear();
}

ioPresentHelper& ioPresentHelper::GetSingleton()
{
	return Singleton<ioPresentHelper>::GetSingleton();
}

void ioPresentHelper::LoadINI()
{
	ioINILoader kLoader( "config/sp2_present_helper.ini" );
	LoadOneDayEvent( kLoader );
	LoadDormancyUserEvent( kLoader );
	LoadSpacialAward( kLoader );
	LoadSpecialFishing( kLoader );
	LoadEventSpecialFishing( kLoader );
	LoadGuildSpecialFishing( kLoader );
	LoadSpecialFishingPCRoom( kLoader );
	LoadUserPresent( kLoader );
	LoadUserSubscription( kLoader );
	LoadRandomDecoPresentM( kLoader );
	LoadRandomDecoPresentW( kLoader );
	LoadTradePresent( kLoader );
	LoadAlchemicPresent( kLoader );

	ioINILoader kGashaponLoader( "config/sp2_gashapon_present.ini" );
	LoadGashaponPresent( kGashaponLoader );

	ioINILoader kLoader2( "config/sp2_monster_present.ini" );
	LoadMonsterPresent( kLoader2 );

	ioINILoader kEtcLoader( "config/sp2_etcitem_present.ini" );
	LoadEtcItemPackagePresent( kEtcLoader );

	ioINILoader kEventLoader( "config/sp2_event_present.ini" );
	LoadEventPresent( kEventLoader );

	ioINILoader kMonsterAward( "config/sp2_monster_special_award.ini" );
	LoadMonsterSpecialAward( kMonsterAward );

	ioINILoader kBuyLoader( "config/sp2_buy_present.ini" );
	LoadBuyPresent( kBuyLoader );

	ioINILoader kExtraLoader( "config/sp2_extraitem_present.ini" );
	LoadExtraItemPackagePresent( kExtraLoader );

	ioINILoader kEventFishingLoader( "config/sp2_event_fishing_present.ini" );
	LoadEventFishingPresent( kEventFishingLoader );

	ioINILoader kHighGradePresent( "config/sp2_high_grade_present.ini" );
	LoadHighGradePresent( kHighGradePresent );
}

void ioPresentHelper::LoadOneDayEvent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadOneDayEvent INI Start!!!!!!!!!!" );

	m_vOneDayGoldItemDataList.clear();
	m_RandTestMap.clear();
	m_dwOneDayGoldItemSeed = 0;

	// 매일매일 골드 아이템~♬ 이벤트
	rkLoader.SetTitle( "OneDayGoldItem" );
	//rkLoader.SaveBool( "Change", false );	

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		OneDayGoldItemData kData;
		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";
		sprintf_s( szKey, "Event%d_Rand", i + 1 );
		kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Event%d_Alarm", i + 1 );
		kData.m_bAlarm = rkLoader.LoadBool( szKey, false );
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
		m_vOneDayGoldItemDataList.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] (%05d:%05d) = %d : %d : %d", i + 1, m_dwOneDayGoldItemSeed, m_dwOneDayGoldItemSeed + kData.m_dwRand, 
											  		            		 kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2 );
		m_dwOneDayGoldItemSeed += kData.m_dwRand;
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadOneDayEvent INI End Max RandSeed : %d And Max Present : %d", m_dwOneDayGoldItemSeed, (int)m_vOneDayGoldItemDataList.size() );
	m_OneDayGoldItemRandom.Randomize();	
}

void ioPresentHelper::LoadDormancyUserEvent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadDormancyUserEvent INI Start!!!!!!!!!!" );

	m_vDormancyUserDataList.clear();

	// 휴면유저 ComeBack 보너스 이벤트
	rkLoader.SetTitle( "DormancyUserEvent" );
	//rkLoader.SaveBool( "Change", false );	

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		DormancyUserData kData;
		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
		m_vDormancyUserDataList.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] %d : %d : %d", i + 1, kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2 );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadDormancyUserEvent INI End Max Present : %d", (int)m_vDormancyUserDataList.size() );
}

void ioPresentHelper::LoadSpacialAward( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadSpacialAward INI Start!!!!!!!!!!" );

	m_vSpacialAwardList.clear();
	m_dwSpacialAwardSeed = 0;

	rkLoader.SetTitle( "SpacialAward" );
	//rkLoader.SaveBool( "Change", false );	

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		SpacialAward kData;
		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";
		
		sprintf_s( szKey, "Award%d_Rand", i + 1 );
		kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Award%d_Alarm", i + 1 );
		kData.m_bAlarm = rkLoader.LoadBool( szKey, false );		
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Alarm", i + 1);
		kData.m_iPresentState = (short)rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Ment", i + 1 );
		kData.m_iPresentMent = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Period", i + 1 );
		kData.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );		
		m_vSpacialAwardList.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] (%05d:%05d) = %d : %d : %d : %d : %d : %d", i + 1, m_dwSpacialAwardSeed, m_dwSpacialAwardSeed + kData.m_dwRand, 
									kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentState, kData.m_iPresentMent, kData.m_iPresentPeriod );
		m_dwSpacialAwardSeed += kData.m_dwRand;
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadSpacialAward INI End Max RandSeed : %d And Max Present : %d", m_dwSpacialAwardSeed, (int)m_vSpacialAwardList.size() );
	m_SpacialAwardRandom.Randomize();

	if( false )
	{
		for(i = 0;i < (int)m_dwSpacialAwardSeed * 3;i++)
		{
			int iReturnID = 0;
			DWORD dwRand = m_SpacialAwardRandom.Random( m_dwSpacialAwardSeed );
			DWORD dwCurValue = 0;
			vSpacialAward::iterator iter = m_vSpacialAwardList.begin();
			for(iReturnID = 0;iter < m_vSpacialAwardList.end();iter++,iReturnID++)
			{
				SpacialAward &rkData = *iter;
				if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
				{
					RandTestMapInsert( iReturnID );
					break;
				}
				dwCurValue += rkData.m_dwRand;
			}			
		}
		PrintRandTestMap();
	}
}

void ioPresentHelper::LoadMonsterPresent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadMonsterPresent INI Start!!!!!!!!!!" );

	m_MonsterPresentMap.clear();

	rkLoader.SetTitle( "Common" );
	//rkLoader.SaveBool( "Change", false );	

	int i = 0;	
	int iMaxPresent = rkLoader.LoadInt( "MaxPresent", 0 );
	m_bAbuseMonsterPresent = rkLoader.LoadBool( "AbusePresent", false );
	for(i = 0;i < iMaxPresent;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "Present%d", i + 1 );
		rkLoader.SetTitle( szTitle );
		
		DWORD dwPresentCode = i + 1;
		MonsterPresentList kDataList;
		int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
		for(int j = 0;j < iMaxItem;j++)
		{
			MonsterPresent kData;
			char szKey[MAX_PATH] = "";
			char szBuf[MAX_PATH] = "";

			sprintf_s( szKey, "Present%d_Rand", j + 1 );
			kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Present%d_Type", j + 1 );
			kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Present%d_Alarm", j + 1);
			kData.m_iPresentState = (short)rkLoader.LoadInt( szKey, 0 );		
			sprintf_s( szKey, "Present%d_Ment", j + 1 );
			kData.m_iPresentMent = (short)rkLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Present%d_Period", j + 1 );
			kData.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );		
			sprintf_s( szKey, "Present%d_Value1", j + 1 );
			kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );		
			sprintf_s( szKey, "Present%d_Value2", j + 1 );
			kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );		
			sprintf_s( szKey, "Present%d_Send_Cnt", j + 1 );
			kData.m_iPresentSendCnt = rkLoader.LoadInt( szKey, 0 );
			
			kDataList.m_dwRandomSeed += kData.m_dwRand;
			kDataList.m_PresentList.push_back( kData );
			EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] (%05d) = %d : %d : %d : %d : %d : %d : %d", j + 1, kData.m_dwRand, 
										 kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentState, kData.m_iPresentMent, kData.m_iPresentPeriod, kData.m_iPresentSendCnt );
		}
		m_MonsterPresentMap.insert( MonsterPresentMap::value_type( dwPresentCode, kDataList ) );
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadMonsterPresent INI %d] RandSeed : %d And Max Present : %d", i + 1, kDataList.m_dwRandomSeed, (int)kDataList.m_PresentList.size() );
	}	
	m_MonsterPresentRandom.Randomize();
}

void ioPresentHelper::LoadGashaponPresent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadGashaponPresent INI Start!!!!!!!!!!" );

	for(vGashaponPresentInfo::iterator iter = m_vGashaponPresentInfoList.begin(); iter != m_vGashaponPresentInfoList.end(); ++iter)
	{
		GashaponPresentInfo &rkInfo = (*iter);
		rkInfo.m_vGashaponPresentList.clear();
	}
	m_vGashaponPresentInfoList.clear();

	rkLoader.SetTitle( "Common" );
	//rkLoader.SaveBool( "Change", false );
	int iMaxInfo = rkLoader.LoadInt( "MaxInfo", 0 );
	for (int iInfoCnt = 0; iInfoCnt < iMaxInfo ; iInfoCnt++)
	{
		GashaponPresentInfo kInfo;
		kInfo.m_vGashaponPresentList.clear();

		m_RandTestMap.clear();
		kInfo.m_dwGashaponPresentSeed = 0;

		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "GashaponPresent%d", iInfoCnt+1 );
		rkLoader.SetTitle( szName );

		kInfo.m_dwEtcItemType  = rkLoader.LoadInt( "EtcItemType", 0 );
		char szBuf[MAX_PATH]   = "";
		kInfo.m_iGashaponAlarm   = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
		kInfo.m_iGashaponMent    = (short)rkLoader.LoadInt( "PresentMent", 0 );
		kInfo.m_iGashaponPeriod  = rkLoader.LoadInt( "PresentPeriod", 0 );
#ifdef	GASHAPON_PRESENT_LIMITED_QUANTITY_BY_BCKIM			// 2020-03-26 by bckim, 가차 선택 제한
		kInfo.m_iLimited_Quantity  = rkLoader.LoadInt( "PresentLimitedQuantity", 0 );		
#endif 
		int i = 0;	
		int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
		for(i = 0;i < iMaxItem;i++)
		{
			GashaponPresent kData;
			char szKey[MAX_PATH] = "";
			ZeroMemory( szBuf, sizeof( szBuf ) );
			sprintf_s( szKey, "Gashapon%d_Rand", i + 1 );
			kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
			if( kData.m_dwRand == 0 )
			{
				EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] Rand is zero. it is not setting.", i + 1 );
				continue;
			}
			sprintf_s( szKey, "Whole%d_Alarm", i + 1 );
			kData.m_bWholeAlarm = rkLoader.LoadBool( szKey, false );
			sprintf_s( szKey, "Present%d_Type", i + 1 );
			kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Present%d_Value1", i + 1 );
			kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Present%d_Value2", i + 1 );
			kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Present%d_Peso", i + 1 );
			kData.m_iPresentPeso = rkLoader.LoadInt( szKey, 0 );

#ifdef	GASHAPON_PRESENT_LIMITED_QUANTITY_BY_BCKIM			// 2020-03-26 by bckim, 가차 선택 제한
			sprintf_s( szKey, "Present%d_Limited", i + 1 );
			kData.m_bLimited_Flag = rkLoader.LoadBool( szKey, false );
#endif 
			kInfo.m_vGashaponPresentList.push_back( kData );

			EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] (%05d:%05d) = %d : %d : %d", i + 1, kInfo.m_dwGashaponPresentSeed, kInfo.m_dwGashaponPresentSeed + kData.m_dwRand, kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2 );
			kInfo.m_dwGashaponPresentSeed += kData.m_dwRand;
		}
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadGashaponPresent INI [%d] End Max RandSeed : %d And Max Present : %d", kInfo.m_dwEtcItemType, kInfo.m_dwGashaponPresentSeed, (int)kInfo.m_vGashaponPresentList.size() );
		kInfo.m_GashaponPresentRandom.Randomize();

		std::sort( kInfo.m_vGashaponPresentList.begin(), kInfo.m_vGashaponPresentList.end(), GashaponPresentSort() );

		m_vGashaponPresentInfoList.push_back( kInfo );

		// Test
		if( false )
		{
			for(i = 0;i < (int)kInfo.m_dwGashaponPresentSeed * 10000;i++)
			{
				int iReturnID = 0;
				DWORD dwRand = kInfo.m_GashaponPresentRandom.Random( kInfo.m_dwGashaponPresentSeed );
				DWORD dwCurValue = 0;
				vGashaponPresent::iterator iter = kInfo.m_vGashaponPresentList.begin();
				for(iReturnID = 0;iter < kInfo.m_vGashaponPresentList.end();iter++,iReturnID++)
				{
					GashaponPresent &rkData = *iter;
					if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
					{
						RandTestMapInsert( iReturnID );
						break;
					}
					dwCurValue += rkData.m_dwRand;
				}			
			}
			PrintRandTestMap();
		}
		//	
	}
}

void ioPresentHelper::LoadRandomDecoPresentM( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadRandomDecoPresentM INI Start!!!!!!!!!!" );

	m_vRandomDecoPresentListM.clear();
	m_dwRandomDecoPresentSeedM = 0;

	rkLoader.SetTitle( "RandomDecoPresentM" );
	//rkLoader.SaveBool( "Change", false );	

	char szBuf[MAX_PATH] = "";
	m_iRandomDecoPresentAlarmM  = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
	m_iRandomDecoPresentMentM   = (short)rkLoader.LoadInt( "PresentMent", 0 );
	m_iRandomDecoPresentPeriodM = rkLoader.LoadInt( "PresentPeriod", 0 );

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		RandomDecoPresent kData;
		char szKey[MAX_PATH] = "";
		ZeroMemory( szBuf, sizeof( szBuf ) );
		sprintf_s( szKey, "Deco%d_Rand", i + 1 );
		kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Deco%d_Alarm", i + 1 );
		kData.m_bAlarm = rkLoader.LoadBool( szKey, false );
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Peso", i + 1 );
		kData.m_iPresentPeso = rkLoader.LoadInt( szKey, 0 );
		m_vRandomDecoPresentListM.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] (%05d:%05d) = %d : %d : %d", i + 1, m_dwRandomDecoPresentSeedM, m_dwRandomDecoPresentSeedM + kData.m_dwRand, kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2 );
		m_dwRandomDecoPresentSeedM += kData.m_dwRand;
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadRandomDecoPresentM INI End Max RandSeed : %d And Max Present : %d", m_dwRandomDecoPresentSeedM, (int)m_vRandomDecoPresentListM.size() );
	m_RandomDecoPresentRandomM.Randomize();
	//
}

void ioPresentHelper::LoadRandomDecoPresentW( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadRandomDecoPresentW INI Start!!!!!!!!!!" );

	m_vRandomDecoPresentListW.clear();
	m_dwRandomDecoPresentSeedW = 0;

	rkLoader.SetTitle( "RandomDecoPresentW" );
	//rkLoader.SaveBool( "Change", false );	

	char szBuf[MAX_PATH] = "";
	m_iRandomDecoPresentAlarmW  = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
	m_iRandomDecoPresentMentW   = (short)rkLoader.LoadInt( "PresentMent", 0 );
	m_iRandomDecoPresentPeriodW = rkLoader.LoadInt( "PresentPeriod", 0 );

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		RandomDecoPresent kData;
		char szKey[MAX_PATH] = "";
		ZeroMemory( szBuf, sizeof( szBuf ) );
		sprintf_s( szKey, "Deco%d_Rand", i + 1 );
		kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Deco%d_Alarm", i + 1 );
		kData.m_bAlarm = rkLoader.LoadBool( szKey, false );
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Peso", i + 1 );
		kData.m_iPresentPeso = rkLoader.LoadInt( szKey, 0 );
		m_vRandomDecoPresentListW.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] (%05d:%05d) = %d : %d : %d", i + 1, m_dwRandomDecoPresentSeedW, m_dwRandomDecoPresentSeedW + kData.m_dwRand, kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2 );
		m_dwRandomDecoPresentSeedW += kData.m_dwRand;
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadRandomDecoPresentW INI End Max RandSeed : %d And Max Present : %d", m_dwRandomDecoPresentSeedW, (int)m_vRandomDecoPresentListW.size() );
	m_RandomDecoPresentRandomW.Randomize();
	//
}

void ioPresentHelper::LoadEtcItemPackagePresent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadEtcItemPackagePresent INI Start!!!!!!!!!!" );

	m_vEtcItemPackagePresent.clear();

	rkLoader.SetTitle( "EtcitemPresent" );
	//rkLoader.SaveBool( "Change", false );	

	char szBuf[MAX_PATH] = "";
	m_iEtcItemPackageAlarm   = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
	m_iEtcItemPackagePeriod  = rkLoader.LoadInt( "PresentPeriod", 0 );

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		EtcItemPackagePresent kData;
		char szKey[MAX_PATH] = "";
		ZeroMemory( szBuf, sizeof( szBuf ) );
		sprintf_s( szKey, "EtcItem%d_Type", i + 1 );
		kData.m_dwEtcItemType  = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType   = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Ment", i + 1 );
		kData.m_iPresentMent   = rkLoader.LoadInt( szKey, 0 );
		m_vEtcItemPackagePresent.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] %d : %d : %d : %d : %d", i + 1, kData.m_dwEtcItemType, kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentMent );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadEtcItemPackagePresent INI End Max Present : %d", (int)m_vEtcItemPackagePresent.size() );
}

void ioPresentHelper::LoadEventPresent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadEventPresent INI Start!!!!!!!!!!" );

	m_vEventPresent.clear();
	m_RandTestMap.clear();

	for(vEventTypeRand::iterator iter = m_vEventTypeRand.begin(); iter != m_vEventTypeRand.end(); ++iter)
		delete *iter;
	m_vEventTypeRand.clear();

	rkLoader.SetTitle( "EventPresent" );
	//rkLoader.SaveBool( "Change", false );	

	char szBuf[MAX_PATH] = "";
	m_iEventAlarm  = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
	m_iEventPeriod = rkLoader.LoadInt( "PresentPeriod", 0 );

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		EventPresent kData;
		char szKey[MAX_PATH] = "";
		ZeroMemory( szBuf, sizeof( szBuf ) );
		sprintf_s( szKey, "Event%d_Type", i + 1 );
		kData.m_dwEventType = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Event%d_Rand", i + 1 );
		kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Ment", i + 1 );
		kData.m_iPresentMent   = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Always", i + 1 );
		kData.m_bAlwaysPresent   = rkLoader.LoadBool( szKey, false );
		sprintf_s( szKey, "Present%d_Receive_Type", i + 1 );
		kData.m_iPresentReceiveType = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Send_Cnt", i + 1 );
		kData.m_iPresentSendCnt = rkLoader.LoadInt( szKey, 0 );

		m_vEventPresent.push_back( kData );

		EventTypeRand *pRand = GetEventTypeRand( kData.m_dwEventType );

		if(  pRand )
		{
			pRand->m_dwEventPresentSeed += kData.m_dwRand;
		}
		else
		{
			pRand = new EventTypeRand;
			if( pRand )
			{
				pRand->m_dwEventType = kData.m_dwEventType;
				pRand->m_EventPresentRandom.Randomize();
				pRand->m_dwEventPresentSeed += kData.m_dwRand;

				m_vEventTypeRand.push_back( pRand );
			}
		}

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] %d] (%05d) = %d : %d : %d : %d : %d :%d", i + 1, kData.m_dwEventType, kData.m_dwRand,  kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_bAlwaysPresent, kData.m_iPresentReceiveType, kData.m_iPresentSendCnt );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadEventPresent INI End Max Present : %d", (int)m_vEventPresent.size() );

	// Test
	if( false )
	{
		for(vEventTypeRand::iterator iter = m_vEventTypeRand.begin(); iter != m_vEventTypeRand.end(); ++iter)
		{
			EventTypeRand *pRand = (*iter);
			if( !pRand )
				continue;

			for(i = 0;i < 10000;i++)
			{
				int iReturnID = 0;
				DWORD dwRand = pRand->m_EventPresentRandom.Random( pRand->m_dwEventPresentSeed );
				DWORD dwCurValue = 0;
				vEventPresent::iterator iter2 = m_vEventPresent.begin();
				for(iReturnID = 0;iter2 < m_vEventPresent.end();iter2++,iReturnID++)
				{
					EventPresent &rkData = *iter2;
					if( pRand->m_dwEventType != rkData.m_dwEventType )
						continue;

					if( rkData.m_bAlwaysPresent || COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
					{
						RandTestMapInsert( iReturnID );
						if( !rkData.m_bAlwaysPresent )
							break;
					}

					dwCurValue += rkData.m_dwRand;
				}
			}
		}
		PrintRandTestMap();
	}
	//
}

void ioPresentHelper::LoadSpecialFishing( ioINILoader &rkLoader )
{
	// Nomal
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadSpecialFishing INI Start!!!!!!!!!!" );

	m_vSpecialFishingList.clear();
	m_dwSpecialFishingSeed = 0;

	rkLoader.SetTitle( "SpecialFishing" );
	//rkLoader.SaveBool( "Change", false );

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		SpecialFishing kData;
		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";

		sprintf_s( szKey, "Fishing%d_Rand", i + 1 );
		kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Fishing%d_Alarm", i + 1 );
		kData.m_bAlarm = rkLoader.LoadBool( szKey, false );		
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Alarm", i + 1);
		kData.m_iPresentState = (short)rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Ment", i + 1 );
		kData.m_iPresentMent = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Period", i + 1 );
		kData.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );		
		m_vSpecialFishingList.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] - (%05d:%05d) = %d : %d : %d : %d : %d : %d", i + 1, m_dwSpecialFishingSeed, m_dwSpecialFishingSeed + kData.m_dwRand, 
			kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentState, kData.m_iPresentMent, kData.m_iPresentPeriod );
		m_dwSpecialFishingSeed += kData.m_dwRand;
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadSpecialFishing INI End Max RandSeed : %d And Max Present : %d", m_dwSpecialFishingSeed, (int)m_vSpecialFishingList.size() );
	m_SpecialFishingRandom.Randomize();
}

void ioPresentHelper::LoadEventSpecialFishing( ioINILoader &rkLoader )
{
	// Event
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadEventSpecialFishing INI Start!!!!!!!!!!" );

	m_vEventSpecialFishingList.clear();
	m_dwEventSpecialFishingSeed = 0;

	rkLoader.SetTitle( "EventSpecialFishing" );
	//rkLoader.SaveBool( "Change", false );

	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for( int i = 0;i < iMaxItem;i++)
	{
		SpecialFishing kData;
		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";

		sprintf_s( szKey, "Fishing%d_Rand", i + 1 );
		kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
		if( kData.m_dwRand == 0 )
		{
			EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] Rand is zero. it is not setting.", i + 1 );
			continue;
		}
		sprintf_s( szKey, "Fishing%d_Alarm", i + 1 );
		kData.m_bAlarm = rkLoader.LoadBool( szKey, false );		
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Alarm", i + 1);
		kData.m_iPresentState = (short)rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Ment", i + 1 );
		kData.m_iPresentMent = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Period", i + 1 );
		kData.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );		
		m_vEventSpecialFishingList.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] - (%05d:%05d) = %d : %d : %d : %d : %d : %d", i + 1, m_dwEventSpecialFishingSeed, m_dwEventSpecialFishingSeed + kData.m_dwRand, 
			kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentState, kData.m_iPresentMent, kData.m_iPresentPeriod );
		m_dwEventSpecialFishingSeed += kData.m_dwRand;
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadEventSpecialFishing INI End Max RandSeed : %d And Max Present : %d", m_dwEventSpecialFishingSeed, (int)m_vEventSpecialFishingList.size() );
	m_EventSpecialFishingRandom.Randomize();
}

void ioPresentHelper::LoadGuildSpecialFishing( ioINILoader &rkLoader )
{
	m_vGuildSpecialFishingList.clear();
	m_dwEventSpecialFishingSeed	= 0;

	rkLoader.SetTitle( "GuildSpecialFishing" );

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		SpecialFishing kData;
		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";

		sprintf_s( szKey, "Fishing%d_Rand", i + 1 );
		kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Fishing%d_Alarm", i + 1 );
		kData.m_bAlarm = rkLoader.LoadBool( szKey, false );		
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Alarm", i + 1);
		kData.m_iPresentState = (short)rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Ment", i + 1 );
		kData.m_iPresentMent = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Period", i + 1 );
		kData.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );	

		m_vGuildSpecialFishingList.push_back( kData );
		m_dwGuildSpecialFishingSeed += kData.m_dwRand;
	}

	m_GuildSpecialFishingRandom.Randomize();
}

void ioPresentHelper::LoadSpecialFishingPCRoom( ioINILoader &rkLoader )
{
	// Nomal
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadSpecialFishingPCRoom INI Start!!!!!!!!!!" );

	m_vSpecialFishingPCRoomList.clear();
	m_dwSpecialFishingPCRoomSeed = 0;

	rkLoader.SetTitle( "SpecialFishingPCRoom" );
	//rkLoader.SaveBool( "Change", false );

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		SpecialFishing kData;
		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";

		sprintf_s( szKey, "Fishing%d_Rand", i + 1 );
		kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Fishing%d_Alarm", i + 1 );
		kData.m_bAlarm = rkLoader.LoadBool( szKey, false );		
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Alarm", i + 1);
		kData.m_iPresentState = (short)rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Ment", i + 1 );
		kData.m_iPresentMent = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Period", i + 1 );
		kData.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );		
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );		
		m_vSpecialFishingPCRoomList.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] - (%05d:%05d) = %d : %d : %d : %d : %d : %d", i + 1, m_dwSpecialFishingPCRoomSeed, m_dwSpecialFishingPCRoomSeed + kData.m_dwRand, 
									kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentState, kData.m_iPresentMent, kData.m_iPresentPeriod );
		m_dwSpecialFishingPCRoomSeed += kData.m_dwRand;
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadSpecialFishingPCRoom INI End Max RandSeed : %d And Max Present : %d", m_dwSpecialFishingPCRoomSeed, (int)m_vSpecialFishingPCRoomList.size() );
	m_SpecialFishingPCRoomRandom.Randomize();
}

void ioPresentHelper::LoadMonsterSpecialAward( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadMonsterSpecialAward INI Start!!!!!!!!!!" );

	rkLoader.SetTitle( "Common" );
	//rkLoader.SaveBool( "Change", false );

	int i = 0;

	// AwardTimeInfo
	m_vAwardTimeInfoList.clear();

	rkLoader.SetTitle( "AwardTimeInfo" );
	int iMaxInfoList = rkLoader.LoadInt( "Max_Award_Time_Info", 0 );

	for(int i=0; i < iMaxInfoList; ++i )
	{
		char szKey[MAX_PATH] = "";
		AwardTimeInfo kInfo;

		wsprintf( szKey, "Award_Time_info%d_min", i+1 );
		kInfo.m_fMinRate = rkLoader.LoadFloat( szKey, 0 );

		wsprintf( szKey, "Award_Time_info%d_max", i+1 );
		kInfo.m_fMaxRate = rkLoader.LoadFloat( szKey, 1 );

		wsprintf( szKey, "Award_Time_info%d_code", i+1 );
		kInfo.m_dwAwardCode = rkLoader.LoadInt( szKey, 0 );

		m_vAwardTimeInfoList.push_back( kInfo );
	}


	// AwardList
	m_MonsterAwardMap.clear();

	rkLoader.SetTitle( "MonsterSpecialAward" );
	int iMaxAwardList = rkLoader.LoadInt( "MaxAwardList", 0 );
	for(i = 0;i < iMaxAwardList;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "MonsterAwardList%d", i + 1 );
		rkLoader.SetTitle( szTitle );

		DWORD dwAwardCode = i + 1;
		MonsterAwardList kDataList;
		int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
		for(int j = 0;j < iMaxItem;j++)
		{
			SpacialAward kData;
			char szKey[MAX_PATH] = "";
			char szBuf[MAX_PATH] = "";

			sprintf_s( szKey, "Award%d_Rand", j + 1 );
			kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Award%d_Alarm", j + 1 );
			kData.m_bAlarm = rkLoader.LoadBool( szKey, false );		
			sprintf_s( szKey, "Present%d_Type", j + 1 );
			kData.m_iPresentType = (short)rkLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Present%d_Alarm", j + 1);
			kData.m_iPresentState = (short)rkLoader.LoadInt( szKey, 0 );		
			sprintf_s( szKey, "Present%d_Ment", j + 1 );
			kData.m_iPresentMent = (short)rkLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Present%d_Period", j + 1 );
			kData.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );		
			sprintf_s( szKey, "Present%d_Value1", j + 1 );
			kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );		
			sprintf_s( szKey, "Present%d_Value2", j + 1 );
			kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );

			kDataList.m_dwRandomSeed += kData.m_dwRand;
			kDataList.m_AwardList.push_back( kData );

			EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] (%05d) = %d : %d : %d : %d : %d : %d", j + 1, kData.m_dwRand, 
				kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentState, kData.m_iPresentMent, kData.m_iPresentPeriod );
		}
		m_MonsterAwardMap.insert( MonsterAwardMap::value_type( dwAwardCode, kDataList ) );
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadMonsterAward INI %d] RandSeed : %d And Max Present : %d", i + 1, kDataList.m_dwRandomSeed, (int)kDataList.m_AwardList.size() );
	}	
	m_MonsterAwardRandom.Randomize();
}

void ioPresentHelper::LoadBuyPresent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadBuyPresent INI Start!!!!!!!!!!" );

	m_vBuyPresent.clear();

	rkLoader.SetTitle( "BuyPresent" );
	//rkLoader.SaveBool( "Change", false );	

	char szBuf[MAX_PATH] = "";
	m_iBuyPresentAlarm       = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
	m_iBuyPresentPeriod      = rkLoader.LoadInt( "PresentPeriod", 0 );

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		BuyPresent kData;
		char szKey[MAX_PATH] = "";
		ZeroMemory( szBuf, sizeof( szBuf ) );
		sprintf_s( szKey, "Event%d_Type", i + 1 );
		kData.m_dwEventType  = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Buy%d_Type", i + 1 );
		kData.m_iBuyType  = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Buy%d_Value1", i + 1 );
		kData.m_iBuyValue1  = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Buy%d_Value2", i + 1 );
		kData.m_iBuyValue2  = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Pass_Buy%d_Value1", i + 1 );
		kData.m_bPassBuyValue1  = rkLoader.LoadBool( szKey, false );
		sprintf_s( szKey, "Pass_Buy%d_Value2", i + 1 );
		kData.m_bPassBuyValue2  = rkLoader.LoadBool( szKey, false );
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType   = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Ment", i + 1 );
		kData.m_iPresentMent = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Pass", i + 1 );
		kData.m_iPresentPass = rkLoader.LoadInt( szKey, 0 );
		m_vBuyPresent.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, " %02d] [%d: %d : %d : %d : %d : %d ] %d : %d : %d : %d", i + 1, kData.m_dwEventType, kData.m_iBuyType, kData.m_iBuyValue1, kData.m_iBuyValue2, kData.m_bPassBuyValue1, kData.m_bPassBuyValue2, kData.m_iPresentType, kData.m_iPresentMent, kData.m_iPresentValue1, kData.m_iPresentValue2 );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadBuyPresent INI End Max Present : %d", (int)m_vBuyPresent.size() );
}

void ioPresentHelper::LoadUserPresent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadUserPresent INI Start!!!!!!!!!!" );

	rkLoader.SetTitle( "UserPresent" );
	//rkLoader.SaveBool( "Change", false );	

	m_iUserPresentAlarm      = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
	m_iUserPresentPeriod     = rkLoader.LoadInt( "PresentPeriod", 0 );
	m_iCanUserPresentCnt     = rkLoader.LoadInt( "CanPresentCnt", 0 );
	m_iUserPresentMent       = rkLoader.LoadInt( "PresentMent", 0 );
	m_iUserPresentBonusPesoMent = rkLoader.LoadInt( "BonusPesoMent", 0 );
	m_iUserPresentBonusItemMent = rkLoader.LoadInt( "BonusItemMent", 0 );

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Alarm:%d, Period:%d, Cnt:%d [Ment:%d:%d:%d]", m_iUserPresentAlarm, m_iUserPresentPeriod, m_iCanUserPresentCnt, m_iUserPresentMent, m_iUserPresentBonusPesoMent, m_iUserPresentBonusItemMent );
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadUserPresent INI End" );
}

void ioPresentHelper::LoadUserSubscription( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadUserSubscription INI Start!!!!!!!!!!" );

	rkLoader.SetTitle( "UserSubscription" );
	//rkLoader.SaveBool( "Change", false );	

	char szBuf[MAX_PATH] = "";
	m_iCanUserSubscriptionCnt = rkLoader.LoadInt( "CanSubscriptionCnt", 0 );

	m_iUserSubscriptionPeriod = rkLoader.LoadInt( "SubscriptionPeriod", 0 );

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadUserSubscription INI End" );
}

void ioPresentHelper::LoadExtraItemPackagePresent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadExtraItemPackagePresent INI Start!!!!!!!!!!" );

	m_vExtraItemPackagePresent.clear();

	rkLoader.SetTitle( "ExtraItemPresent" );
	//rkLoader.SaveBool( "Change", false );	

	char szBuf[MAX_PATH] = "";
	m_iExtraItemPackageAlarm   = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
	m_iExtraItemPackagePeriod  = rkLoader.LoadInt( "PresentPeriod", 0 );

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		ExtraItemPackagePresent kData;
		char szKey[MAX_PATH] = "";
		ZeroMemory( szBuf, sizeof( szBuf ) );
		sprintf_s( szKey, "Machine%d_Code", i + 1 );
		kData.m_iMachineCode   = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType   = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Ment", i + 1 );
		kData.m_iPresentMent   = rkLoader.LoadInt( szKey, 0 );
		m_vExtraItemPackagePresent.push_back( kData );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] %d : %d : %d : %d : %d", i + 1, kData.m_iMachineCode, kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentMent );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadExtraItemPackagePresent INI End Max Present : %d", (int)m_vExtraItemPackagePresent.size() );
}

void ioPresentHelper::LoadEventFishingPresent( ioINILoader &rkLoader )
{
	m_vEventFishingPresent.clear();

	rkLoader.SetTitle( "EventFishingPresent" );
	//rkLoader.SaveBool( "Change", false );

	short iPresentState = 0;
	int iPresentPeriod = 0;

	char szBuf[MAX_PATH] = "";

	iPresentState = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
	iPresentPeriod = rkLoader.LoadInt( "PresentPeriod", 0 );

	int i = 0;	
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		EventFishing kData;
		char szKey[MAX_PATH] = "";
		ZeroMemory( szBuf, sizeof( szBuf ) );

		kData.m_iPresentState = iPresentState;
		kData.m_iPresentPeriod = iPresentPeriod;

		sprintf_s( szKey, "Present%d_num", i + 1 );
		kData.m_iPresentNum = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Type", i + 1 );
		kData.m_iPresentType   = (short)rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value1", i + 1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Present%d_Value2", i + 1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "Present%d_Ment", i + 1 );
		kData.m_iPresentMent   = rkLoader.LoadInt( szKey, 0 );
		m_vEventFishingPresent.push_back( kData );
	}

	m_EventFishingRandom.Randomize();
}

void ioPresentHelper::LoadTradePresent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadTradePresent INI Start!!!!!!!!!!" );

	rkLoader.SetTitle( "TradePresent" );
	//rkLoader.SaveBool( "Change", false );	

	m_iTradePresentAlarm      = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
	m_iTradeSellPresentAlarm = (short)rkLoader.LoadInt( "SellPresentAlarm", 0 );
	m_iTradePresentPeriod     = rkLoader.LoadInt( "PresentPeriod", 0 );

	m_iTradePresentBuyMent       = rkLoader.LoadInt( "TradeBuyMent", 0 );
	m_iTradePresentSellMent = rkLoader.LoadInt( "TradeSellMent", 0 );
	m_iTradePresentCancelMent = rkLoader.LoadInt( "TradeCancelMent", 0 );
	m_iTradePresentTimeOutMent = rkLoader.LoadInt( "TradeTimeOutMent", 0 );

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Alarm:%d, %d, Period:%d, Cnt:%d [Ment:%d:%d:%d]", m_iTradePresentAlarm,
																				m_iTradeSellPresentAlarm,
																				m_iTradePresentPeriod,
																				m_iTradePresentBuyMent,
																				m_iTradePresentSellMent,
																				m_iTradePresentCancelMent,
																				m_iTradePresentTimeOutMent );
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadTradePresent INI End" );
}

void ioPresentHelper::LoadAlchemicPresent( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "AlchemicPresent" );
	//rkLoader.SaveBool( "Change", false );	

	m_iAlchemicPresentAlarm       = (short)rkLoader.LoadInt( "PresentAlarm", 0 );
	m_iAlchemicPresentPeriod      = rkLoader.LoadInt( "PresentPeriod", 0 );
	m_iAlchemicPresentSoldierMent = rkLoader.LoadInt( "AlchemicSoldierMent", 0 );
	m_iAlchemicPresentItemMent    = rkLoader.LoadInt( "AlchemicItemMent", 0 );
	m_iAlchemicPresentPieceMent	  = rkLoader.LoadInt( "AlchemicPieceMent", 0 );
}

void ioPresentHelper::LoadHighGradePresent( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadHighGradePresent INI Start!!!!!" );

	for(HighGradePresentMap::iterator iCreate = m_HighGradePresentMap.begin();iCreate != m_HighGradePresentMap.end();++iCreate)
	{
		HighGradePresentList &kList = iCreate->second;
		kList.m_PresentList.clear();
	}
	m_HighGradePresentMap.clear();

	rkLoader.SetTitle( "Common" );
	//rkLoader.SaveBool( "Change", false );

	int iMaxGrade = rkLoader.LoadInt( "MaxGrade", 0 );
	for(int i = 0;i < iMaxGrade;i++)
	{	
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "Grade%d", i + 1 );
		rkLoader.SetTitle( szKey );
		
		HighGradePresentList kPresentList;

		DWORD dwGrade = rkLoader.LoadInt( "Grade", 0 );
		
		int k = 0;
		DWORDVec kPresent;
		int iMaxPresent = rkLoader.LoadInt( "MaxPresent", 0 );
		for(k = 0;k < iMaxPresent;k++)
		{
			sprintf_s( szKey, "Present%d", k + 1 );			
			kPresent.push_back( rkLoader.LoadInt( szKey, 0 ) );
		}
		for(k = 0;k < (int)kPresent.size();k++)
		{
			sprintf_s( szKey, "Present%d", kPresent[k] );
			rkLoader.SetTitle( szKey );
			
			HighGradePresent kPresentData;
			kPresentData.m_dwRand			= rkLoader.LoadInt( "RandValue", 0 );
			kPresentData.m_iPresentType		= rkLoader.LoadInt( "PresentType", 0 );
			kPresentData.m_iPresentValue1	= rkLoader.LoadInt( "PresentValue1", 0 );
			kPresentData.m_iPresentValue2	= rkLoader.LoadInt( "PresentValue2", 0 );
			kPresentData.m_iPresentValue3	= rkLoader.LoadInt( "PresentValue3", 0 );
			kPresentData.m_iPresentValue4	= rkLoader.LoadInt( "PresentValue4", 0 );
			kPresentData.m_iPresentMent		= rkLoader.LoadInt( "PresentMent", 0 );
			kPresentData.m_iPresentState	= rkLoader.LoadInt( "PresentState", 0 );
			kPresentData.m_iPresentPeriod   = rkLoader.LoadInt( "PresentPeriod", 0 );

			kPresentList.m_dwRandSeed += kPresentData.m_dwRand;
			kPresentList.m_PresentList.push_back( kPresentData );

			EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Grade : %d - %d.%d.%d.%d.%d.%d.%d.%d.%d", dwGrade,
										 kPresentData.m_dwRand, kPresentData.m_iPresentType, kPresentData.m_iPresentValue1, 
										 kPresentData.m_iPresentValue2, kPresentData.m_iPresentValue3, kPresentData.m_iPresentValue4, 
										 kPresentData.m_iPresentMent, kPresentData.m_iPresentState, kPresentData.m_iPresentPeriod);
		}
		m_HighGradePresentMap.insert( HighGradePresentMap::value_type( dwGrade, kPresentList ) );
	}
	m_HighGradePresentRandom.Randomize();

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadHighGradePresent INI End Max:%d", (int)m_HighGradePresentMap.size() );
}

void ioPresentHelper::RandTestMapInsert( DWORD dwCheckID )
{
	vRandTestMap::iterator iter = m_RandTestMap.find( dwCheckID );
	if( iter != m_RandTestMap.end() )
	{
		int &kCount = iter->second;
		kCount++;
	}
	else 
	{
		m_RandTestMap.insert( vRandTestMap::value_type( dwCheckID, 1 ) );
	}
}

void ioPresentHelper::PrintRandTestMap()
{
	EventLOG.PrintTimeAndLog( 0, "ioPresentHelper::PrintRandTestMap Max Index : %d", (int)m_RandTestMap.size() );
	vRandTestMap::iterator iCreate;
	for( iCreate = m_RandTestMap.begin() ; iCreate != m_RandTestMap.end() ; ++iCreate )
	{
		EventLOG.PrintTimeAndLog( 0, "PrintRandTestMap : ID:%d - %d회 당첨", iCreate->first, iCreate->second );
	}
}

void ioPresentHelper::CheckNeedReload()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	ioINILoader kLoader;
	kLoader.SetFileName( "config/sp2_present_helper.ini" );
	if( kLoader.ReadBool( "OneDayGoldItem", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "OneDayGoldItem" );
		LoadOneDayEvent( kLoader );
	}
	if( kLoader.ReadBool( "DormancyUserEvent", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "DormancyUserEvent" );
		LoadDormancyUserEvent( kLoader );
	}
	if( kLoader.ReadBool( "SpacialAward", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "SpacialAward" );
		LoadSpacialAward( kLoader );
	}
	if( kLoader.ReadBool( "SpecialFishing", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "SpecialFishing" );
		LoadSpecialFishing( kLoader );
	}
	if( kLoader.ReadBool( "EventSpecialFishing", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "EventSpecialFishing" );
		LoadEventSpecialFishing( kLoader );
	}
	if( kLoader.ReadBool( "SpecialFishingPCRoom", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "SpecialFishingPCRoom" );
		LoadSpecialFishingPCRoom( kLoader );
	}
	if( kLoader.ReadBool( "UserPresent", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "UserPresent" );
		LoadUserPresent( kLoader );
	}
	if( kLoader.ReadBool( "UserSubscription", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "UserSubscription" );
		LoadUserSubscription( kLoader );
	}
	if( kLoader.ReadBool( "GuildSpecialFishing", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "GuildSpecialFishing" );
		LoadGuildSpecialFishing( kLoader );
	}
	if( kLoader.ReadBool( "RandomDecoPresentM", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "RandomDecoPresentM" );
		LoadRandomDecoPresentM( kLoader );
	}
	if( kLoader.ReadBool( "RandomDecoPresentW", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "RandomDecoPresentW" );
		LoadRandomDecoPresentW( kLoader );
	}
	if( kLoader.ReadBool( "TradePresent", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "TradePresent" );
		LoadTradePresent( kLoader );
	}
	if( kLoader.ReadBool( "AlchemicPresent", "Change", false ) )
	{
		kLoader.ReloadFile( "config/sp2_present_helper.ini", "AlchemicPresent" );
		LoadAlchemicPresent( kLoader );
	}


	
	ioINILoader kGashponLoader;
	kGashponLoader.SetFileName( "config/sp2_gashapon_present.ini" );
	if( kGashponLoader.ReadBool( "Common", "Change", false ) )
	{
		kGashponLoader.ReloadFile( "config/sp2_gashapon_present.ini" );
		LoadGashaponPresent( kGashponLoader );
	}

	// PvE
	ioINILoader kLoader2;
	kLoader2.SetFileName( "config/sp2_monster_present.ini" );
	if( kLoader2.ReadBool( "Common", "Change", false ) )
	{
		kLoader2.ReloadFile( "config/sp2_monster_present.ini" );
		LoadMonsterPresent( kLoader2 );
	}	

	ioINILoader kEtcLoader;
	kEtcLoader.SetFileName( "config/sp2_etcitem_present.ini" );
	if( kEtcLoader.ReadBool( "EtcitemPresent", "Change", false ) )
	{
		kEtcLoader.ReloadFile( "config/sp2_etcitem_present.ini" );
		LoadEtcItemPackagePresent( kEtcLoader );
	}	

	ioINILoader kEventLoader;
	kEventLoader.SetFileName( "config/sp2_event_present.ini" );
	if( kEventLoader.ReadBool( "EventPresent", "Change", false ) )
	{
		kEventLoader.ReloadFile( "config/sp2_event_present.ini" );
		LoadEventPresent( kEventLoader );
	}

	ioINILoader kMonsterAward;
	kMonsterAward.SetFileName( "config/sp2_monster_special_award.ini" );
	if( kMonsterAward.ReadBool( "Common", "Change", false ) )
	{
		kMonsterAward.ReloadFile( "config/sp2_monster_special_award.ini" );
		LoadMonsterSpecialAward( kMonsterAward );
	}

	ioINILoader kBuyLoader;
	kBuyLoader.SetFileName( "config/sp2_buy_present.ini" );
	if( kBuyLoader.ReadBool( "BuyPresent", "Change", false ) )
	{
		kBuyLoader.ReloadFile( "config/sp2_buy_present.ini" );
		LoadBuyPresent( kBuyLoader );
	}

	ioINILoader kExtraLoader;
	kExtraLoader.SetFileName( "config/sp2_extraitem_present.ini" );
	if( kExtraLoader.ReadBool( "ExtraItemPresent", "Change", false ) )
	{
		kExtraLoader.ReloadFile( "config/sp2_extraitem_present.ini" );
		LoadExtraItemPackagePresent( kExtraLoader );
	}	

	ioINILoader kEventFishingLoader;
	kEventFishingLoader.SetFileName( "config/sp2_event_fishing_present.ini" );
	if( kEventFishingLoader.ReadBool( "EventFishingPresent", "Change", false ) )
	{
		kEventFishingLoader.ReloadFile( "config/sp2_event_fishing_present.ini" );
		LoadEventFishingPresent( kEventFishingLoader );
	}

	ioINILoader kHighGradePresent;
	kHighGradePresent.SetFileName( "config/sp2_high_grade_present.ini" );
	if( kHighGradePresent.ReadBool( "Common", "Change", false ) )
	{
		kHighGradePresent.ReloadFile( "config/sp2_high_grade_present.ini" );
		LoadHighGradePresent( kHighGradePresent );
	}
}

int ioPresentHelper::GetOneDayEventPresent( ioHashString &rSendID, short &rPresentType, int &rPresentValue1, int &rPresentValue2, int &rPresentValue3, int &rPresentValue4, bool &rAlarm )
{
	int iReturnID = 0;
	DWORD dwRand = m_OneDayGoldItemRandom.Random( m_dwOneDayGoldItemSeed );
	DWORD dwCurValue = 0;
	vOneDayGoldItemData::iterator iter = m_vOneDayGoldItemDataList.begin();
	for(iReturnID = 0;iter < m_vOneDayGoldItemDataList.end();iter++,iReturnID++)
	{
		OneDayGoldItemData &rkData = *iter;
		if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
		{
			rSendID       = g_MainServer.GetSendID();
			rPresentType  = rkData.m_iPresentType;
			rPresentValue1= rkData.m_iPresentValue1;
			rPresentValue2= rkData.m_iPresentValue2;
			rPresentValue3= rkData.m_iPresentValue3;
			rPresentValue4= rkData.m_iPresentValue4;
			rAlarm        = rkData.m_bAlarm;
			return iReturnID;
		}
		dwCurValue += rkData.m_dwRand;
	}
	EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioPresentHelper::GetOneDayEventPresent None Rand Value[%d]", dwRand );
	return -1;
}

int ioPresentHelper::GetMaxDormancyUserPresent()
{
	return m_vDormancyUserDataList.size();
}

int ioPresentHelper::GetDormancyUserPresent( int iArray, ioHashString &rSendID, short &rPresentType, int &rPresentValue1, int &rPresentValue2, int &rPresentValue3, int &rPresentValue4 )
{
	if( !COMPARE( iArray, 0, (int)m_vDormancyUserDataList.size() ) )
		return -1;

	DormancyUserData &rkData = m_vDormancyUserDataList[iArray];
	rSendID       = g_MainServer.GetSendID();
	rPresentType  = rkData.m_iPresentType;
	rPresentValue1= rkData.m_iPresentValue1;
	rPresentValue2 = rkData.m_iPresentValue2;
	rPresentValue3= rkData.m_iPresentValue3;
	rPresentValue4 = rkData.m_iPresentValue4;

	return iArray;
}

bool ioPresentHelper::SendSpacialAwardPresent( User *pSendUser, SP2Packet &rkPacket, OUT int & iPresentType, OUT int & iPresentValue1, OUT int & iPresentValue2, OUT bool & bAlarm)
{
	// 서버 알림에 사용하기 위해서 변수를 가져온다. 
	iPresentType = iPresentValue1 = iPresentValue2 = 0;
	bAlarm = false;

	if( pSendUser )
	{
		int iReturnID = 0;
		DWORD dwRand = m_SpacialAwardRandom.Random( m_dwSpacialAwardSeed );
		DWORD dwCurValue = 0;
		vSpacialAward::iterator iter = m_vSpacialAwardList.begin();
		for(iReturnID = 0;iter < m_vSpacialAwardList.end();iter++,iReturnID++)
		{
			SpacialAward &rkData = *iter;
			if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
			{
				// 선물 Insert
				CTimeSpan cPresentGapTime( rkData.m_iPresentPeriod, 0, 0, 0 );
				CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
				g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4,
												rkData.m_iPresentMent, kPresentTime, rkData.m_iPresentState );
				
				g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, "SpacialAward" );

				pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
				rkPacket << rkData.m_iPresentType << rkData.m_iPresentValue1 << rkData.m_iPresentValue2 << rkData.m_iPresentValue3 << rkData.m_iPresentValue4 << rkData.m_bAlarm;

				iPresentType = rkData.m_iPresentType;
				iPresentValue1 = rkData.m_iPresentValue1;
				iPresentValue2 = rkData.m_iPresentValue2;
				bAlarm = true;
                return true;
			}
			dwCurValue += rkData.m_dwRand;
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioPresentHelper::GetSpacialAwardPresent None Rand Value[%d]", dwRand );
	}
	rkPacket << 0 << 0 << 0 << false;
	return false;
}

bool ioPresentHelper::SendSpecialFishingPresent( User *pSendUser, SP2Packet &rkPacket,
												OUT int & iPresentType, OUT int & iPresentValue1, OUT int & iPresentValue2 )
{
	// 서버 알림에 사용하기 위해서 변수를 가져온다. 
	iPresentType = iPresentValue1 = iPresentValue2 = 0;

	if( pSendUser )
	{
		bool bEvent = false;
		FishingEventUserNode *pEventNode = static_cast<FishingEventUserNode*> ( pSendUser->GetEventUserMgr().GetEventUserNode( EVT_FISHING ) );
		if( pEventNode )
		{
			if( pEventNode->IsEventTime( pSendUser ) )
				bEvent = true;
		}
		
		int iReturnID = 0;
		DWORD dwRand = 0;
		DWORD dwCurValue = 0;
		vSpecialFishing::iterator iter, iter_e;

		if( pSendUser->GetGuildFisheryCode() != 0 ) //길드 낚시가 최우선
		{
			dwRand	= m_GuildSpecialFishingRandom.Random(m_dwGuildSpecialFishingSeed);
			iter = m_vGuildSpecialFishingList.begin();
			iter_e = m_vGuildSpecialFishingList.end();
		}
		else if( bEvent )   // 이벤트 먼저 판단.
		{
			dwRand = m_EventSpecialFishingRandom.Random( m_dwEventSpecialFishingSeed );
			iter = m_vEventSpecialFishingList.begin();
			iter_e = m_vEventSpecialFishingList.end();
		}
		else if( pSendUser->IsPCRoomAuthority() )
		{
			dwRand = m_SpecialFishingPCRoomRandom.Random( m_dwSpecialFishingPCRoomSeed );
			iter = m_vSpecialFishingPCRoomList.begin();
			iter_e = m_vSpecialFishingPCRoomList.end();
		}
		else 
		{
			dwRand = m_SpecialFishingRandom.Random( m_dwSpecialFishingSeed );
			iter = m_vSpecialFishingList.begin();
			iter_e = m_vSpecialFishingList.end();
		}

		for(iReturnID = 0;iter < iter_e;iter++,iReturnID++)
		{
			SpecialFishing &rkData = *iter;
			if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
			{
				// 선물 Insert
				CTimeSpan cPresentGapTime( rkData.m_iPresentPeriod, 0, 0, 0 );
				CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
				
				g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4,
												rkData.m_iPresentMent, kPresentTime, rkData.m_iPresentState );
				
				g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, "SpecialFishing" );

				pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청

				rkPacket << g_MainServer.GetSendID()
						 << rkData.m_iPresentType
						 << rkData.m_iPresentMent
						 << rkData.m_iPresentValue1
						 << rkData.m_iPresentValue2
						 << rkData.m_iPresentValue3
						 << rkData.m_iPresentValue4
						 << rkData.m_bAlarm;


				iPresentType = rkData.m_iPresentType;
				iPresentValue1 = rkData.m_iPresentValue1;
				iPresentValue2 = rkData.m_iPresentValue2;
				
				return true;
			}

			dwCurValue += rkData.m_dwRand;
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioPresentHelper::GetSpecialFishingPresent None Rand Value[%d]", dwRand );
	}

	rkPacket << ioHashString() << 0 << 0 << 0 << 0 << false;

	return false;
}

bool ioPresentHelper::SendMonsterPresent( User *pSendUser, DWORD dwPresentCode, bool bAbuseTime )
{
	if( pSendUser )
	{
		MonsterPresentMap::iterator iter = m_MonsterPresentMap.find( dwPresentCode );
		if( iter == m_MonsterPresentMap.end() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioPresentHelper::SendMonsterPresent None Present Code[%d]", dwPresentCode );
			return false;
		}
		MonsterPresentList &rkMonsterPresentList = iter->second;
		int iReturnID = 0;
		DWORD dwRand = m_MonsterPresentRandom.Random( rkMonsterPresentList.m_dwRandomSeed );
		DWORD dwCurValue = 0;

		if( bAbuseTime )
		{
			if( m_bAbuseMonsterPresent )
				dwRand = 0;      // 어뷰즈라면 처음 선물을 지급한다.
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioPresentHelper::SendMonsterPresent %s is not present on Abuse.", pSendUser->GetPublicID().c_str() );
				return false;
			}
		}
		
		vMonsterPresent::iterator iCurrent = rkMonsterPresentList.m_PresentList.begin();
		for(iReturnID = 0;iCurrent < rkMonsterPresentList.m_PresentList.end();iCurrent++,iReturnID++)
		{
			MonsterPresent &rkData = *iCurrent;
			if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
			{
				int iSize = 1;
				if( rkData.m_iPresentSendCnt > 1 )
					iSize = rkData.m_iPresentSendCnt;
				for (int i = 0; i < iSize ; i++)
				{
					// 선물 Insert
					CTimeSpan cPresentGapTime( rkData.m_iPresentPeriod, 0, 0, 0 );
					CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
					g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4,
						rkData.m_iPresentMent, kPresentTime, rkData.m_iPresentState );

					g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, "Monster" );
				}
				pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
				return true;
			}
			dwCurValue += rkData.m_dwRand;
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioPresentHelper::SendMonsterPresent None Rand Value[%d]", dwRand );
	}
	return false;
}

ioPresentHelper::GashaponPresentInfo *ioPresentHelper::GetGashaponPresentInfo( DWORD dwEtcItemType )
{
	for(vGashaponPresentInfo::iterator iter = m_vGashaponPresentInfoList.begin(); iter != m_vGashaponPresentInfoList.end(); ++iter)
	{
		GashaponPresentInfo &kInfo = (*iter);
		if( kInfo.m_dwEtcItemType == dwEtcItemType )
			return &kInfo;
	}

	return NULL;
}

bool ioPresentHelper::SendGashaponPresent( User *pSendUser, DWORD dwEtcItemType, short &riPresentType, int &riPresentValue1, int &riPresentValue2, int &riPresentValue3, int &riPresentValue4, bool &rbWholeAlarm, int &riPresentPeso )
{
	if( !pSendUser )
		return false;

	GashaponPresentInfo *pInfo = GetGashaponPresentInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return false;
	}

	DWORD dwRand = pInfo->m_GashaponPresentRandom.Random( pInfo->m_dwGashaponPresentSeed );
	DWORD dwCurValue = 0;
	for( vGashaponPresent::iterator iter = pInfo->m_vGashaponPresentList.begin(); iter < pInfo->m_vGashaponPresentList.end(); iter++ )
	{
		GashaponPresent &rkData = *iter;
		if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
		{
			// 선물 Insert
			CTimeSpan cPresentGapTime( pInfo->m_iGashaponPeriod, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
//			g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pInfo->m_szGashaponSendID, pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, pInfo->m_iGashaponMent, kPresentTime, pInfo->m_iGashaponAlarm );

			pSendUser->AddPresentMemory( g_MainServer.GetSendID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, pInfo->m_iGashaponMent, kPresentTime, pInfo->m_iGashaponAlarm );

			char szNote[MAX_PATH]="";
			StringCbPrintf( szNote, sizeof( szNote ) , "Gashapon:%d", dwEtcItemType );
			g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, szNote );
			// 페소도 동시 지급
			if( rkData.m_iPresentPeso != 0 )
			{
				//g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pInfo->m_szGashaponSendID, pSendUser->GetPublicID(), PRESENT_PESO , rkData.m_iPresentPeso, 0, 0, 0, pInfo->m_iGashaponMent, kPresentTime, pInfo->m_iGashaponAlarm );
				pSendUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_PESO , rkData.m_iPresentPeso, 0, 0, 0, pInfo->m_iGashaponMent, kPresentTime, pInfo->m_iGashaponAlarm );
				
				g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), PRESENT_PESO, rkData.m_iPresentPeso, 0, 0, 0, LogDBClient::PST_RECIEVE, "Gashapon" );
			}

//			pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청

			riPresentType   = rkData.m_iPresentType; 
			riPresentValue1 = rkData.m_iPresentValue1;
			riPresentValue2 = rkData.m_iPresentValue2;
			riPresentValue3 = rkData.m_iPresentValue3;
			riPresentValue4 = rkData.m_iPresentValue4;
			rbWholeAlarm    = rkData.m_bWholeAlarm;
			riPresentPeso   = rkData.m_iPresentPeso;
			return true;
		}
		dwCurValue += rkData.m_dwRand;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None Rand Value[%d][%d]", __FUNCTION__, dwEtcItemType, dwRand );

	return false;
}

void ioPresentHelper::SendGashponPresentList( User *pSendUser, DWORD dwEtcItemType )
{
	if( !pSendUser )
		return;

	GashaponPresentInfo *pInfo = GetGashaponPresentInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return;
	}

	SP2Packet kPacket( STPK_GASHAPON_LIST );
	kPacket << (int) pInfo->m_vGashaponPresentList.size();

	for( vGashaponPresent::iterator iter = pInfo->m_vGashaponPresentList.begin(); iter < pInfo->m_vGashaponPresentList.end(); iter++ )
	{
		GashaponPresent &rkData = *iter;
		kPacket << rkData.m_iPresentType; 
		kPacket << rkData.m_iPresentValue1;
		kPacket << rkData.m_iPresentValue2;
		kPacket << rkData.m_iPresentValue3;
		kPacket << rkData.m_iPresentValue4;
	}

	pSendUser->SendMessage( kPacket );
}


#if defined( SRC_OVERSEAS )
// 일반 가챠 패키지 전부 보내기 (매크로)		JCLEE 140718
void ioPresentHelper::SendAllGashaponPresent( User *pSendUser, DWORD dwEtcItemType )
{
	if( !pSendUser )
		return;

	GashaponPresentInfo *pInfo = GetGashaponPresentInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return;
	}

	for( vGashaponPresent::iterator iter = pInfo->m_vGashaponPresentList.begin(); iter < pInfo->m_vGashaponPresentList.end(); iter++ )
	{
		SP2Packet kPacket( STPK_ETCITEM_USE );
		GashaponPresent &rkData = *iter;		

		CTimeSpan cPresentGapTime( pInfo->m_iGashaponPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

		pSendUser->AddPresentMemory( pInfo->m_szGashaponSendID, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, pInfo->m_iGashaponMent, kPresentTime, pInfo->m_iGashaponAlarm );

		char szNote[MAX_PATH]="";
		StringCbPrintf( szNote, sizeof( szNote ) , "Gashapon:%d", dwEtcItemType );
		g_LogDBClient.OnInsertPresent( 0, pInfo->m_szGashaponSendID, g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, szNote );

		kPacket << ETCITEM_USE_OK;
		kPacket << dwEtcItemType;
		kPacket << 0;
		kPacket << 0;
		kPacket << rkData.m_iPresentType; 
		kPacket << rkData.m_iPresentValue1;
		kPacket << rkData.m_iPresentValue2;
		kPacket << rkData.m_iPresentValue3;
		kPacket << rkData.m_iPresentValue4;
		kPacket << rkData.m_bWholeAlarm;
		kPacket << rkData.m_iPresentPeso;

		pSendUser->SendMessage( kPacket );
		pSendUser->SendPresentMemory();  // 메모리 선물 전송
	}
}
#endif


bool ioPresentHelper::SendEtcItemPackagePresent( User *pSendUser, DWORD dwEtcItemType )
{
	if( !pSendUser )
		return false;

	bool bInsert = false;
	for( vEtcItemPackagePresent::iterator iter = m_vEtcItemPackagePresent.begin(); iter < m_vEtcItemPackagePresent.end(); iter++ )
	{
		EtcItemPackagePresent &rkData = *iter;
		if( rkData.m_dwEtcItemType == dwEtcItemType )
		{
			// 선물 Insert
			bInsert = true;
			CTimeSpan cPresentGapTime( m_iEtcItemPackagePeriod, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
			g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, rkData.m_iPresentMent, kPresentTime, m_iEtcItemPackageAlarm );
			char szNote[MAX_PATH]="";
			StringCbPrintf( szNote, sizeof( szNote ), "EtcItem:%d", dwEtcItemType );
			g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, szNote );
		}
	}

	if( bInsert )
	{
		pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
		return true;
	}
	
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None Etc Type[%d]", __FUNCTION__, dwEtcItemType );
	return false;
}

bool ioPresentHelper::SendEventPresent( User *pSendUser, DWORD dwEventType, int iPresentReceiveType /*= 0 */ )
{
	if( !pSendUser )
		return false;

	EventTypeRand *pRand = GetEventTypeRand( dwEventType );
	if( !pRand )
		return false;

	DWORD dwRand      = pRand->m_EventPresentRandom.Random( pRand->m_dwEventPresentSeed );
	DWORD dwCurValue  = 0;
	bool  bGetPresent = false;
	for( vEventPresent::iterator iter = m_vEventPresent.begin(); iter < m_vEventPresent.end(); iter++ )
	{
		EventPresent &rkData = *iter;
		if( rkData.m_dwEventType != pRand->m_dwEventType )
			continue;

		bool bGet = false;
		if( rkData.m_iPresentReceiveType != 0 )
		{
			if( rkData.m_iPresentReceiveType == iPresentReceiveType )
				bGet = true;
		}
		else if( rkData.m_bAlwaysPresent )
			bGet = true;
		else if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
			bGet = true;

		if( bGet )
		{
			int iSize = 1;
			if( rkData.m_iPresentSendCnt > 1 )
				iSize = rkData.m_iPresentSendCnt;

			for (int i = 0; i < iSize ; i++)
			{
				// 선물 Insert
				bGetPresent = true;
				CTimeSpan cPresentGapTime( m_iEventPeriod, 0, 0, 0 );
				CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
				g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, rkData.m_iPresentMent, kPresentTime, m_iEventAlarm );
				char szNote[MAX_PATH]="";
				StringCbPrintf( szNote, sizeof( szNote ), "Event:%d", dwEventType );
				g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, szNote );
			}
		}
		dwCurValue += rkData.m_dwRand;
	}

	if( bGetPresent )
	{
		pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
		return true;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None Rand Value[%d]", __FUNCTION__, dwRand );
	return false;
}

bool ioPresentHelper::SendRandomDecoPresent( User *pSendUser, short &riPresentType, int &riPresentValue1, int &riPresentValue2, int &riPresentValue3, int &riPresentValue4, bool &rbWholeAlarm, int &riPresentPeso, bool bMan )
{
	if( !pSendUser )
		return false;

	DWORD dwCurValue = 0;
	DWORD dwRand = m_RandomDecoPresentRandomM.Random( m_dwRandomDecoPresentSeedM );
	vRandomDecoPresent::iterator iter = m_vRandomDecoPresentListM.begin();
	vRandomDecoPresent::iterator iter_e = m_vRandomDecoPresentListM.end();

	char szNote[MAX_PATH]="";
	
	if( bMan )
		StringCbPrintf( szNote, sizeof( szNote ) , "RandomDecoM" );
	else
		StringCbPrintf( szNote, sizeof( szNote ) , "RandomDecoW" );

	ioHashString szSendID = g_MainServer.GetSendID();
	
	int iPresentState = m_iRandomDecoPresentAlarmM;
	int iPeriod = m_iRandomDecoPresentPeriodM;
	int iMent = m_iRandomDecoPresentMentM;


	if( !bMan )
	{
		dwRand = m_RandomDecoPresentRandomW.Random( m_dwRandomDecoPresentSeedW );
		iter = m_vRandomDecoPresentListW.begin();
		iter_e = m_vRandomDecoPresentListW.end();

		szSendID = g_MainServer.GetSendID();

		iPresentState = m_iRandomDecoPresentAlarmW;
		iPeriod = m_iRandomDecoPresentPeriodW;
		iMent = m_iRandomDecoPresentMentW;
	}

	for( ; iter < iter_e; iter++ )
	{
		RandomDecoPresent &rkData = *iter;
		if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
		{
			// 선물 Insert
			CTimeSpan cPresentGapTime( iPeriod, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
			g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), szSendID, pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, iMent, kPresentTime, iPresentState );

			g_LogDBClient.OnInsertPresent( 0, szSendID, g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, szNote );
			// 페소도 동시 지급
			if( rkData.m_iPresentPeso != 0 )
			{
				g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), szSendID, pSendUser->GetPublicID(), PRESENT_PESO , rkData.m_iPresentPeso, 0, 0, 0, iMent, kPresentTime, iPresentState );
				g_LogDBClient.OnInsertPresent( 0, szSendID, g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), PRESENT_PESO, rkData.m_iPresentPeso, 0, 0, 0, LogDBClient::PST_RECIEVE, szNote );
			}

			pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청

			riPresentType   = rkData.m_iPresentType; 
			riPresentValue1 = rkData.m_iPresentValue1;
			riPresentValue2 = rkData.m_iPresentValue2;
			riPresentValue3 = rkData.m_iPresentValue3;
			riPresentValue4 = rkData.m_iPresentValue4;
			rbWholeAlarm    = rkData.m_bAlarm;
			riPresentPeso   = rkData.m_iPresentPeso;
			return true;
		}
		dwCurValue += rkData.m_dwRand;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None Rand Value[%d]", __FUNCTION__, dwRand );

	return false;
}

ioPresentHelper::EventTypeRand * ioPresentHelper::GetEventTypeRand( DWORD dwEventType )
{
	for(vEventTypeRand::iterator iter = m_vEventTypeRand.begin(); iter != m_vEventTypeRand.end(); ++iter)
	{
		EventTypeRand *pRand = (*iter);
		if( !pRand )
			continue;
		if( pRand->m_dwEventType == dwEventType )
		{
			return pRand;
		}
	}

	return NULL;
}

bool ioPresentHelper::SendMonsterSpacialAwardPresent( User *pSendUser, float fCurRate, SP2Packet &rkPacket )
{
	if( pSendUser )
	{
		DWORD dwAwardCode = GetAwardCode( fCurRate );
		MonsterAwardMap::iterator iter = m_MonsterAwardMap.find( dwAwardCode );
		if( iter == m_MonsterAwardMap.end() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioPresentHelper::SendMonsterSpacialAwardPresent None Present Code[%d]", dwAwardCode );
			rkPacket << 0 << 0 << 0 << false;
			return false;
		}

		MonsterAwardList &rkMonsterAwardList = iter->second;
		DWORD dwRand = m_MonsterAwardRandom.Random( rkMonsterAwardList.m_dwRandomSeed );
		DWORD dwCurValue = 0;
		
		int iReturnID = 0;
		vSpacialAward::iterator iCurrent = rkMonsterAwardList.m_AwardList.begin();
		for(iReturnID = 0;iCurrent < rkMonsterAwardList.m_AwardList.end();iCurrent++,iReturnID++)
		{
			SpacialAward &rkData = *iCurrent;
			if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
			{
				// 선물 Insert
				CTimeSpan cPresentGapTime( rkData.m_iPresentPeriod, 0, 0, 0 );
				CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
				g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4,
												rkData.m_iPresentMent, kPresentTime, rkData.m_iPresentState );
				g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, "MonsterSpacialAward" );

				pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
				rkPacket << rkData.m_iPresentType << rkData.m_iPresentValue1 << rkData.m_iPresentValue2 << rkData.m_iPresentValue3 << rkData.m_iPresentValue4 << rkData.m_bAlarm;
				return true;
			}
			dwCurValue += rkData.m_dwRand;
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioPresentHelper::SendMonsterSpacialAwardPresent None Rand Value[%d]", dwRand );
	}

	rkPacket << 0 << 0 << 0 << false;
	return false;
}

DWORD ioPresentHelper::GetAwardCode( float fCurRate )
{
	int iInfoCnt = m_vAwardTimeInfoList.size();
	for( int i=0; i < iInfoCnt; ++i )
	{
		float fMinRate = m_vAwardTimeInfoList[i].m_fMinRate;
		float fMaxRate = m_vAwardTimeInfoList[i].m_fMaxRate;

		if( fMaxRate == 0.0f )
			return m_vAwardTimeInfoList[i].m_dwAwardCode;

		if( COMPARE( fCurRate, fMinRate, fMaxRate ) )
			return m_vAwardTimeInfoList[i].m_dwAwardCode;;
	}

	return 0;
}

bool ioPresentHelper::SendBuyPresent( User *pSendUser, DWORD dwEventType, short iBuyType, int iBuyValue1, int iBuyValue2, bool bCash, int iItemPrice, int iRealCash )
{
	if( !pSendUser )
		return false;

	bool bInsert = false;
	for( vBuyPresent::iterator iter = m_vBuyPresent.begin(); iter < m_vBuyPresent.end(); iter++ )
	{
		BuyPresent &rkData = *iter;
		if( rkData.m_dwEventType != dwEventType )
			continue;
		if( rkData.m_iBuyType != iBuyType )
			continue;
		if( rkData.m_iBuyValue1 != iBuyValue1 && !rkData.m_bPassBuyValue1 )
			continue;
		if( rkData.m_iBuyValue2 != iBuyValue2 && !rkData.m_bPassBuyValue2 )
			continue;

		// 선물 Insert
		bInsert = true;
		CTimeSpan cPresentGapTime( m_iBuyPresentPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

		int iCashBackRatio	= 0;
		int iCashBack		= 0;

		if( rkData.m_iPresentValue1 == ioEtcItem::EIT_ETC_MILEAGE_COIN && g_EventMgr.isMileageShopOpen() && bCash )
		{
			int iMileage	= 0;
			
			// 20150716 마일리지 비율 수정 z3
			CTime cCurTime = CTime::GetCurrentTime();
			if ( Help::ConvertCTimeToDate( cCurTime ) >= Help::ConvertCTimeToDate( g_EventMgr.GetLosaStartDate() ) && 
				( Help::ConvertCTimeToDate( cCurTime ) < Help::ConvertCTimeToDate( g_EventMgr.GetLosaEndDate() ) ) )
			{
				int iMilegeRatio	= 0;
				if( pSendUser->IsPCRoomAuthority() )
					iMilegeRatio	= g_EventMgr.GetPcRoomMileageRatio();
			
				iMilegeRatio	+= g_EventMgr.GetLosaMileageRatio();

				if( iItemPrice > 0 )
					iMileage = (iItemPrice * iMilegeRatio ) /100;
			}
			else
			{
				
				if( pSendUser->IsPCRoomAuthority() ) 
				{
					if( iItemPrice > 0 )
						iMileage = (iItemPrice * g_EventMgr.GetPcRoomMileageRatio() ) /100;
				}
				else
				{
					if( iItemPrice > 0 )
						iMileage = (iItemPrice * g_EventMgr.GetMileageRatio() ) / 100;
				}

			}

			//캐시백 이벤트 
			if( g_EventMgr.isChannelingCashBackOpen() )
			{
				if ( Help::ConvertCTimeToDate( cCurTime ) >= Help::ConvertCTimeToDate( g_EventMgr.GetCashBackStartDate() ) && 
					( Help::ConvertCTimeToDate( cCurTime ) < Help::ConvertCTimeToDate( g_EventMgr.GetCashBackEndDate() ) ) )
				{
					
					iCashBackRatio = g_EventMgr.GetLosaCashBackRatio();
					if( iRealCash > 0 )
						iCashBack = (iRealCash * iCashBackRatio ) / 100;

					if( iCashBack > 0 )
					{
						g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), PRESENT_BONUS_CASH, iCashBack,
						g_EventMgr.GetLosaCashBackPeriod(), 0, 0, 17, kPresentTime, 1 );

						char szNote[MAX_PATH]="";
						StringCbPrintf( szNote, sizeof( szNote ), "CashBackEvent:%d:%d:%d", PRESENT_BONUS_CASH
							, iCashBack,g_EventMgr.GetLosaCashBackPeriod() );
						g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), 
							PRESENT_BONUS_CASH, iCashBack,
							g_EventMgr.GetLosaCashBackPeriod(),0, 0, LogDBClient::PST_RECIEVE, szNote );
					}
					
				}
			}

			// 마일리지가 0이면 선물보내지 않음
			if( iMileage <= 0 )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Mileage 0 : userIndex:%d,userID:%s,mileage:%d", __FUNCTION__, pSendUser->GetUserIndex(), pSendUser->GetPublicID().c_str(), iMileage );
				return true;
			}

			g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, iMileage, rkData.m_iPresentValue3, rkData.m_iPresentValue4, rkData.m_iPresentMent, kPresentTime, m_iBuyPresentAlarm );
			char szNote[MAX_PATH]="";
			StringCbPrintf( szNote, sizeof( szNote ), "Buy:%d:%d:%d,%d", iBuyType, iBuyValue1, iBuyValue2, iMileage );
			g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, iMileage, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, szNote );
		}
		else if( rkData.m_iPresentValue1 != ioEtcItem::EIT_ETC_MILEAGE_COIN )
		{
			g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, rkData.m_iPresentMent, kPresentTime, m_iBuyPresentAlarm );
			char szNote[MAX_PATH]="";
			StringCbPrintf( szNote, sizeof( szNote ), "Buy:%d:%d:%d", iBuyType, iBuyValue1, iBuyValue2 );
			g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, szNote );
		}
		
	}

	if( bInsert )
	{
		pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
		return true;
	}

	// 없을 수도 있다.
	return false;
}

bool ioPresentHelper::SendExtraItemPackagePresent( User *pSendUser, int iMachineCode )
{
	if( !pSendUser )
		return false;

	bool bInsert = false;
	for( vExtraItemPackagePresent::iterator iter = m_vExtraItemPackagePresent.begin(); iter < m_vExtraItemPackagePresent.end(); iter++ )
	{
		ExtraItemPackagePresent &rkData = *iter;
		if( rkData.m_iMachineCode == iMachineCode )
		{
			// 선물 Insert
			bInsert = true;
			CTimeSpan cPresentGapTime( m_iExtraItemPackagePeriod, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
			g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, rkData.m_iPresentMent, kPresentTime, m_iExtraItemPackageAlarm );
			char szNote[MAX_PATH]="";
			StringCbPrintf( szNote, sizeof( szNote ), "ExtraItemPackage:%d", iMachineCode );
			g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, rkData.m_iPresentValue3, rkData.m_iPresentValue4, LogDBClient::PST_RECIEVE, szNote );
		}
	}

	if( bInsert )
	{
		pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
		return true;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None ExtraItemPackage[%d]", __FUNCTION__, iMachineCode );
	return false;
}


bool ioPresentHelper::SendEventFishingPresent( User *pSendUser, int iPresentNum, SP2Packet &rkPacket,
												OUT int & iEventFishingPresentType, OUT int & iEventFishingPresentValue1, OUT int & iEventFishingPresentValue2 )
{
	// 서버 알림에 사용하기 위해서 변수를 가져온다. 
	iEventFishingPresentType = iEventFishingPresentValue1 = iEventFishingPresentValue2 = 0;

	if( pSendUser && iPresentNum > 0 )
	{
		DWORDVec vClassList;
		for( int i=0; i < g_ItemPriceMgr.GetMaxClassInfo(); i++ )
		{
			if( !g_ItemPriceMgr.GetArrayClassActive( i ) ) continue;
			vClassList.push_back( g_ItemPriceMgr.GetArrayClassCode( i ) );
		}

		int iClassSize = vClassList.size();
		if( iClassSize <= 0 )
		{
			rkPacket << ioHashString() << 0 << 0 << 0  << 0 << 0 << 0 << false;
			return false;
		}

		DWORD dwRand = m_EventFishingRandom.Random( iClassSize );

		vEventFishing::iterator iter, iter_e;
		iter = m_vEventFishingPresent.begin();
		iter_e = m_vEventFishingPresent.end();

		for( ; iter < iter_e; iter++ )
		{
			EventFishing kData = *iter;
			
			if( kData.m_iPresentNum != iPresentNum )
				continue;

			if( kData.m_iPresentType == 1 && kData.m_iPresentValue1 == 0 )	// 렌덤용병 체크
			{
				kData.m_iPresentValue1 = vClassList[dwRand];
			}

			// 선물 Insert
			CTimeSpan cPresentGapTime( kData.m_iPresentPeriod, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

			g_DBClient.OnInsertPresentData( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(), g_MainServer.GetSendID(), pSendUser->GetPublicID(), kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentValue3, kData.m_iPresentValue4,
				kData.m_iPresentMent, kPresentTime, kData.m_iPresentState );

			g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentValue3, kData.m_iPresentValue4, LogDBClient::PST_RECIEVE, "EventFishing" );

			pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
			rkPacket << g_MainServer.GetSendID() << kData.m_iPresentType << kData.m_iPresentMent
	 				 << kData.m_iPresentValue1 << kData.m_iPresentValue2 << kData.m_iPresentValue3 << kData.m_iPresentValue4 << false;

			iEventFishingPresentType = kData.m_iPresentType;
			iEventFishingPresentValue1 = kData.m_iPresentValue1;
			iEventFishingPresentValue2 = kData.m_iPresentValue2;

			return true;
		}
	}

	rkPacket << ioHashString() << 0 << 0 << 0 << 0 << 0 << 0 << false;

	return false;
}


int ioPresentHelper::CheckSpecialFishingPresent( User *pUser )
{
	if( pUser )
	{
		bool bEvent = false;
		FishingEventUserNode *pEventNode = static_cast<FishingEventUserNode*> ( pUser->GetEventUserMgr().GetEventUserNode( EVT_FISHING ) );
		if( pEventNode )
		{
			if( pEventNode->IsEventTime( pUser ) )
				bEvent = true;
		}

		DWORD dwRand = 0;
		DWORD dwCurValue = 0;
		vSpecialFishing::iterator iter, iter_e;

		if( bEvent )
		{
			dwRand = m_EventSpecialFishingRandom.Random( m_dwEventSpecialFishingSeed );
			iter = m_vEventSpecialFishingList.begin();
			iter_e = m_vEventSpecialFishingList.end();
		}
		else if( pUser->IsPCRoomAuthority() )
		{
			dwRand = m_SpecialFishingPCRoomRandom.Random( m_dwSpecialFishingPCRoomSeed );
			iter = m_vSpecialFishingPCRoomList.begin();
			iter_e = m_vSpecialFishingPCRoomList.end();
		}
		else 
		{
			dwRand = m_SpecialFishingRandom.Random( m_dwSpecialFishingSeed );
			iter = m_vSpecialFishingList.begin();
			iter_e = m_vSpecialFishingList.end();
		}

		for( int iCnt=0; iter < iter_e; iter++, iCnt++ )
		{
			SpecialFishing &rkData = *iter;
			if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
			{
				return iCnt+1;
			}

			dwCurValue += rkData.m_dwRand;
		}
	}

	return -1;
}

int ioPresentHelper::GetSubscriptionType( short iPresentType, int iBuyValue1, int iBuyValue2 )
{
	int iSubscriptionType = SUBSCRIPTION_NONE;

	if( iPresentType == PRESENT_SOLDIER )
	{
		iSubscriptionType = g_ItemPriceMgr.GetSubscriptionType( iBuyValue1 );
	}
	else if( iPresentType == PRESENT_DECORATION )
	{
		iSubscriptionType = g_DecorationPrice.GetSubscriptionType( iBuyValue1, iBuyValue2 );
	}
	else if( iPresentType == PRESENT_ETC_ITEM )
	{
		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iBuyValue1 );
		if( !pEtcItem )
			iSubscriptionType = SUBSCRIPTION_NONE;
		else
			iSubscriptionType = pEtcItem->GetSubscriptionType( iBuyValue2 );
	}
	else if( iPresentType == PRESENT_EXTRAITEM_BOX )
	{
		iSubscriptionType = g_ExtraItemInfoMgr.GetSubscriptionType( iBuyValue1 );
	}

	return iSubscriptionType;
}

int ioPresentHelper::GetCash( short iPresentType, int iBuyValue1, int iBuyValue2 )
{
	int iBuyCash = 0;
	if( iPresentType == PRESENT_SOLDIER )
	{
		if( iBuyValue2 != 0 )
			iBuyCash = g_ItemPriceMgr.GetClassBuyCash( iBuyValue1, iBuyValue2 );
		else
			iBuyCash = g_ItemPriceMgr.GetMortmainCharCash( iBuyValue1 );
	}
	else if( iPresentType == PRESENT_DECORATION )
	{
		iBuyCash = g_DecorationPrice.GetDecoCash( iBuyValue1, iBuyValue2 );
	}
	else if( iPresentType == PRESENT_ETC_ITEM )
	{
		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iBuyValue1 );
		if( !pEtcItem || pEtcItem->GetActive( iBuyValue2 ) == 0 )
			iBuyCash = 0;
		else
			iBuyCash = pEtcItem->GetCash( iBuyValue2 );
	}
	else if( iPresentType == PRESENT_EXTRAITEM_BOX )
	{
		if( g_ExtraItemInfoMgr.GetActive( iBuyValue1 ) == 0 )
			iBuyCash = 0;
		else
			iBuyCash = g_ExtraItemInfoMgr.GetNeedCash( iBuyValue1 );
	}

	return iBuyCash;
}

int ioPresentHelper::GetBonusPeso( short iPresentType, int iBuyValue1, int iBuyValue2 )
{
	int iBonusPeso = 0;
	if( iPresentType == PRESENT_SOLDIER )
	{
		bool bMortmain = false;
		if( iBuyValue2 == 0 )
			bMortmain = true;
		iBonusPeso = g_ItemPriceMgr.GetBonusPeso( iBuyValue1, bMortmain );
	}
	else if( iPresentType == PRESENT_DECORATION )
	{
		iBonusPeso = g_DecorationPrice.GetBonusPeso( iBuyValue1, iBuyValue2 );
	}
	else if( iPresentType == PRESENT_ETC_ITEM )
	{
		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iBuyValue1 );
		if( !pEtcItem )
			iBonusPeso = 0;
		else
			iBonusPeso = pEtcItem->GetBonusPeso( iBuyValue2 );
	}
	else if( iPresentType == PRESENT_EXTRAITEM_BOX )
	{
		iBonusPeso = g_ExtraItemInfoMgr.GetBonusPeso( iBuyValue1 );
	}

	return iBonusPeso;
}

bool ioPresentHelper::InsertUserPresent( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwSendUserIndex, const ioHashString &rszPublicID, const char *szPublicIP, DWORD dwRecvUserIndex, short iPresentType, int iBuyValue1, int iBuyValue2, bool bBonusMent, bool bBuyToPresent, bool bPopup /*= false */, bool bPresentEvent /*= false */ )
{
	if( dwSendUserIndex == 0 )
		return false;

	if( dwRecvUserIndex == 0 )
		return false;
	
	if( !COMPARE( iPresentType, PRESENT_SOLDIER, MAX_PRESENT_TYPE ) )
	{
		return false;
	}

	// 일반 치장은 용병을 선택할 수 없어 랜덤치장으로 속성을 변경한다.
	if( iPresentType == PRESENT_DECORATION )
		iPresentType = PRESENT_RANDOM_DECO;

	int iPresentValue1 = iBuyValue1;
	int iPresentValue2 = iBuyValue2;
	int iPresentValue3 = 0;             //유저간 선물에는 3,4를 사용안함
	int iPresentValue4 = 0;

	// buy type을 present type으로 가공
	if( bBuyToPresent )
	{
		if( iPresentType == PRESENT_ETC_ITEM )
		{
			ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iBuyValue1 );
			if( !pEtcItem )
				return false;

			iPresentValue2 = pEtcItem->GetValue( iBuyValue2 );
		}
	}

	int iPresentMent = m_iUserPresentMent;
	if( bBonusMent )
	{
		if( iPresentType == PRESENT_PESO )
			iPresentMent = m_iUserPresentBonusPesoMent;
		else
			iPresentMent = m_iUserPresentBonusItemMent;
	}

	if( iPresentValue1 == ioEtcItem::EIT_ETC_MILEAGE_COIN)
	{
		iPresentType = PRESENT_ETC_ITEM;
		if( iBuyValue2 <= 0)
			return false;

		iPresentValue2 = iBuyValue2;
		
		                                             
	}

	CTimeSpan cPresentGapTime( m_iUserPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	if( iPresentValue1 == ioEtcItem::EIT_ETC_MILEAGE_COIN)
	{
		if( bPresentEvent )
			return true;

		//선물 보낸 사람과 받는 사람이 같은 경우 , 내가 선물하고 내가 마일리지 받을 때
		if( dwSendUserIndex == dwRecvUserIndex )
		{
			g_DBClient.OnInsertPresentData( dwAgentID, dwThreadID, g_MainServer.GetSendID(), rszPublicID, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, m_iUserPresentAlarm );
		}
		else
			return true;
	}
	else
	{
		g_DBClient.OnInsertPresentDataByUserIndex( dwAgentID, dwThreadID, dwSendUserIndex, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, m_iUserPresentAlarm );
	}
	char szNote[MAX_PATH]="";
	if( bBonusMent )
		StringCbPrintf( szNote, sizeof( szNote ) , "User(Bonus)" );
	else
		StringCbPrintf( szNote, sizeof( szNote ) , "User" );

	if( bPopup )
		StringCbPrintf( szNote, sizeof( szNote ) , "User(Popup)" );

	if( iPresentValue1 == ioEtcItem::EIT_ETC_MILEAGE_COIN)
		g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), szPublicIP, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, LogDBClient::PST_RECIEVE, szNote );
	else
		g_LogDBClient.OnInsertPresent( dwSendUserIndex, rszPublicID, szPublicIP, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, LogDBClient::PST_RECIEVE, szNote );
	return true;
}

bool ioPresentHelper::InsertUserPresentByBuyPresent( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwEventType, DWORD dwSendUserIndex, const ioHashString &rszPublicID, const char *szPublicIP, DWORD dwRecvUserIndex, short iPresentType, int iBuyValue1, int iBuyValue2, bool bPresentEvent )
{
	int iBuyType = BT_NONE;
	if( iPresentType == PRESENT_SOLDIER )
		iBuyType = BT_SOLDIER;
	else if( iPresentType == PRESENT_DECORATION )
		iBuyType = BT_DECO;
	else if( iPresentType == PRESENT_ETC_ITEM )
		iBuyType = BT_ETC;
	else if( iPresentType == PRESENT_PESO )
		iBuyType = BT_PESO;
	else if( iPresentType == PRESENT_EXTRAITEM )
		iBuyType = BT_EXTRAITEM;
	else if( iPresentType == PRESENT_EXTRAITEM_BOX )
		iBuyType = BT_EXTRA_BOX;
	else if( iPresentType == PRESENT_RANDOM_DECO )
		iBuyType = BT_RANDOM_DECO;
	else if( iPresentType == PRESENT_GRADE_EXP )
		iBuyType = BT_GRADE_EXP;
	else if( iPresentType == PRESENT_MEDALITEM )
		iBuyType = BT_MEDALITEM;
	else
		return true; // 그외 항목은 없다.

	int iPresentValue1 = iBuyValue1;
	int iPresentValue2 = iBuyValue2;
	int iPresentValue3 = 0;             //유저간 선물에는 3,4를 사용안함
	int iPresentValue4 = 0;

	if( iPresentType == PRESENT_ETC_ITEM )
	{
		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iBuyValue1 );
		if( !pEtcItem )
			return false;

		iPresentValue2 = pEtcItem->GetValue( iBuyValue2 );
	}

	for( vBuyPresent::iterator iter = m_vBuyPresent.begin(); iter < m_vBuyPresent.end(); iter++ )
	{
		BuyPresent &rkData = *iter;
		if( rkData.m_dwEventType != dwEventType )
			continue;
		if( rkData.m_iBuyType != iBuyType )
			continue;
		if( rkData.m_iBuyValue1 != iPresentValue1 && !rkData.m_bPassBuyValue1 )
			continue;
		if( rkData.m_iBuyValue2 != iPresentValue2 && !rkData.m_bPassBuyValue2 )
			continue;

		if( rkData.m_iPresentPass == 1 && dwSendUserIndex == dwRecvUserIndex)
		{
			continue;
		}
		else if( rkData.m_iPresentPass == 2 && dwSendUserIndex != dwRecvUserIndex)
		{
			continue;
		}

		// 선물 Insert
		if( !InsertUserPresent( dwAgentID, dwThreadID, dwSendUserIndex, rszPublicID, szPublicIP, dwRecvUserIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, true, false, false, bPresentEvent ) )
			return false;
	}

	return true;
}

bool ioPresentHelper::SendPresentByTradeItemBuy( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwSendUserIndex, DWORD dwRecvUserIndex,
												 DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, ioHashString &szRegisterUserNick )
{
	if( dwSendUserIndex == 0 )
	{
		return false;
	}

	if( dwRecvUserIndex == 0 )
	{
		return false;
	}

	if( dwItemType != PRESENT_EXTRAITEM )
	{
		return false;
	}


	short iPresentType = PRESENT_EXTRAITEM;
	int iPresentValue1 = dwItemMagicCode;
	int iPresentValue2 = (dwItemValue * PRESENT_EXTRAITEM_DIVISION_2);
	int iPresentValue3 = dwItemMaleCustom;
	int iPresentValue4 = dwItemFemaleCustom;
	int iPresentMent = m_iTradePresentBuyMent;

	CTimeSpan cPresentGapTime( m_iTradePresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	g_DBClient.OnInsertPresentDataByUserIndex( dwAgentID, dwThreadID, dwSendUserIndex, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, m_iTradePresentAlarm );

	TradeLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SendPresentByTradeItemBuy : [Send:%d] [Recv:%d] [%d:%d:%d:%d:%d:%d:%d:%d]", dwSendUserIndex, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, m_iTradePresentPeriod, m_iTradePresentAlarm );		

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "TradeBuy" );

	g_LogDBClient.OnInsertPresent( dwSendUserIndex, szRegisterUserNick, g_App.GetPublicIP().c_str(), dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, LogDBClient::PST_TRADE, szNote );
	return true;
}

bool ioPresentHelper::SendPresentByTradeItemSell( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwSendUserIndex, DWORD dwRecvUserIndex,
												  int iItemPrice, ioHashString &szRegisterUserNick )
{
	if( dwSendUserIndex == 0 )
		return false;

	if( dwRecvUserIndex == 0 )
		return false;

	short iPresentType = PRESENT_PESO;
	int iPresentValue1 = iItemPrice;
	int iPresentValue2 = 0;
	int iPresentValue3 = 0;
	int iPresentValue4 = 0;
	int iPresentMent = m_iTradePresentSellMent;

	CTimeSpan cPresentGapTime( m_iTradePresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	g_DBClient.OnInsertPresentDataByUserIndex( dwAgentID, dwThreadID, dwSendUserIndex, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, m_iTradeSellPresentAlarm );

	TradeLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SendPresentByTradeItemSell : [Send:%d] [Recv:%d] [%d:%d:%d:%d:%d:%d:%d:%d]", dwSendUserIndex, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, m_iTradePresentPeriod, m_iTradeSellPresentAlarm );

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "TradeSell" );

	g_LogDBClient.OnInsertPresent( dwSendUserIndex, szRegisterUserNick, g_App.GetPublicIP().c_str(), dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, LogDBClient::PST_TRADE, szNote );
	return true;
}

bool ioPresentHelper::SendPresentByTradeCancel( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwSendUserIndex, DWORD dwRecvUserIndex,
												DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, ioHashString &szRegisterUserNick )
{
	if( dwSendUserIndex == 0 )
		return false;

	if( dwRecvUserIndex == 0 )
		return false;

	if( dwItemType != PRESENT_EXTRAITEM )
		return false;


	short iPresentType = PRESENT_EXTRAITEM;
	int iPresentValue1 = dwItemMagicCode;
	int iPresentValue2 = (dwItemValue * PRESENT_EXTRAITEM_DIVISION_2);
	int iPresentValue3 = dwItemMaleCustom;
	int iPresentValue4 = dwItemFemaleCustom;
	int iPresentMent = m_iTradePresentCancelMent;

	CTimeSpan cPresentGapTime( m_iTradePresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	g_DBClient.OnInsertPresentDataByUserIndex( dwAgentID, dwThreadID, dwSendUserIndex, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, m_iTradePresentAlarm );

	TradeLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SendPresentByTradeItemCancel : [Send:%d] [Recv:%d] [%d:%d:%d:%d:%d:%d:%d:%d]", dwSendUserIndex, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, m_iTradePresentPeriod, m_iTradePresentAlarm );

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "TradeCancel" );

	g_LogDBClient.OnInsertPresent( dwSendUserIndex, szRegisterUserNick, g_App.GetPublicIP().c_str(), dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, LogDBClient::PST_TRADE, szNote );
	return true;
}

bool ioPresentHelper::SendPresentByTradeTimeOut( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwSendUserIndex, DWORD dwRecvUserIndex,
												 DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, ioHashString &szRegisterUserNick )
{
	if( dwSendUserIndex == 0 )
		return false;

	if( dwRecvUserIndex == 0 )
		return false;

	if( dwItemType != PRESENT_EXTRAITEM )
		return false;


	short iPresentType = PRESENT_EXTRAITEM;
	int iPresentValue1 = dwItemMagicCode;
	int iPresentValue2 = (dwItemValue * PRESENT_EXTRAITEM_DIVISION_2);
	int iPresentValue3 = dwItemMaleCustom;
	int iPresentValue4 = dwItemFemaleCustom;
	int iPresentMent = m_iTradePresentTimeOutMent;

	CTimeSpan cPresentGapTime( m_iTradePresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	g_DBClient.OnInsertPresentDataByUserIndex( dwAgentID, dwThreadID, dwSendUserIndex, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, m_iTradeSellPresentAlarm );

	TradeLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SendPresentByTradeTimeOut : [Send:%d] [Recv:%d] [%d:%d:%d:%d:%d:%d:%d:%d]", dwSendUserIndex, dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, m_iTradePresentPeriod, m_iTradeSellPresentAlarm );

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "TradeTimeOut" );

	g_LogDBClient.OnInsertPresent( dwSendUserIndex, szRegisterUserNick, g_App.GetPublicIP().c_str(), dwRecvUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, LogDBClient::PST_TRADE, szNote );
	return true;
}

bool ioPresentHelper::SendPresentByAlchemicSoldier( User *pSendUser, DWORD dwClassType, DWORD dwPeriodValue )
{
	if( !pSendUser )
		return false;

	short iPresentType = PRESENT_SOLDIER;
	int iPresentValue1 = dwClassType;
	int iPresentValue2 = dwPeriodValue;
	int iPresentValue3 = 0;
	int iPresentValue4 = 0;
	int iPresentMent = m_iAlchemicPresentSoldierMent;

	DWORD dwUserIndex = pSendUser->GetUserIndex();

	CTimeSpan cPresentGapTime( m_iAlchemicPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	g_DBClient.OnInsertPresentDataByUserIndex( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(),
											   dwUserIndex, dwUserIndex,
											   iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, m_iAlchemicPresentAlarm );

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "AlchemicSoldier" );

	g_LogDBClient.OnInsertPresent( dwUserIndex, pSendUser->GetPublicID(), g_App.GetPublicIP().c_str(), dwUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, LogDBClient::PST_ALCHEMIC, szNote );

	pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
	return true;
}

bool ioPresentHelper::SendPresentByAlchemicItem( User *pSendUser, DWORD dwItemCode, int iLimitTime, int iReinforce )
{
	if( !pSendUser )
		return false;

	short iPresentType = PRESENT_EXTRAITEM;
	int iPresentValue1 = dwItemCode;
	int iPresentValue2 = (iReinforce * PRESENT_EXTRAITEM_DIVISION_2) + iLimitTime;
	int iPresentValue3 = 0;
	int iPresentValue4 = 0;
	int iPresentMent = m_iAlchemicPresentItemMent;

	DWORD dwUserIndex = pSendUser->GetUserIndex();

	CTimeSpan cPresentGapTime( m_iAlchemicPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	g_DBClient.OnInsertPresentDataByUserIndex( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(),
											   dwUserIndex, dwUserIndex,
											   iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, m_iAlchemicPresentAlarm );

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "AlchemicItem" );

	g_LogDBClient.OnInsertPresent( dwUserIndex, pSendUser->GetPublicID(), g_App.GetPublicIP().c_str(), dwUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, LogDBClient::PST_ALCHEMIC, szNote );

	pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
	return true;
}

bool ioPresentHelper::SendPresentByAlchemicItem( User *pSendUser, int iCode, int iCnt )
{
	if( !pSendUser )
		return false;

	short iPresentType = PRESENT_ALCHEMIC_ITEM;
	int iPresentValue1 = iCode;
	int iPresentValue2 = iCnt;
	int iPresentValue3 = 0;
	int iPresentValue4 = 0;
	int iPresentMent = m_iAlchemicPresentPieceMent;

	DWORD dwUserIndex = pSendUser->GetUserIndex();

	CTimeSpan cPresentGapTime( m_iAlchemicPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	g_DBClient.OnInsertPresentDataByUserIndex( pSendUser->GetUserDBAgentID(), pSendUser->GetAgentThreadID(),
											   dwUserIndex, dwUserIndex,
											   iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, m_iAlchemicPresentAlarm );

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "AlchemicItem" );

	g_LogDBClient.OnInsertPresent( dwUserIndex, pSendUser->GetPublicID(), g_App.GetPublicIP().c_str(), dwUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, LogDBClient::PST_ALCHEMIC, szNote );

	pSendUser->_OnSelectPresent( 30 );     // 받지 않은 선물 최대 30개 요청
	return true;
}

bool ioPresentHelper::GetHighGradePresent( DWORD dwGrade, ioHashString &rkSendID, short &rPresentType, int &rPresentValue1, int &rPresentValue2,
										   int &rPresentValue3, int &rPresentValue4, short &rPresentMent, short &rPresentState, int &rPresentPeriod )
{
	HighGradePresentMap::iterator iter = m_HighGradePresentMap.find( dwGrade );
	if( iter == m_HighGradePresentMap.end() )
		return false;

	HighGradePresentList &rkPresentList = iter->second;

	DWORD dwRand = m_HighGradePresentRandom.Random( rkPresentList.m_dwRandSeed );
	DWORD dwCurValue = 0;
	HighGradePresentVec::iterator PresentIter = rkPresentList.m_PresentList.begin();
	for(;PresentIter < rkPresentList.m_PresentList.end();PresentIter++)
	{
		HighGradePresent &rkData = *PresentIter;
		if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
		{
			rkSendID		= g_MainServer.GetSendID();
			rPresentType	= rkData.m_iPresentType;
			rPresentValue1  = rkData.m_iPresentValue1;
			rPresentValue2  = rkData.m_iPresentValue2;
			rPresentValue3  = rkData.m_iPresentValue3;
			rPresentValue4  = rkData.m_iPresentValue4;
			rPresentMent    = rkData.m_iPresentMent;
			rPresentState   = rkData.m_iPresentState;
			rPresentPeriod  = rkData.m_iPresentPeriod;
			return true;
		}
		dwCurValue += rkData.m_dwRand;
	}
	return false;
}

void ioPresentHelper::SendAwardEtcItemBonus( User *pSendUser, DWORD dwEtcItemType, int iEtcItemCount )
{
	if( !pSendUser ) return;
	
	if( dwEtcItemType == 0 || iEtcItemCount == 0 )
		return;

	CTimeSpan cPresentGapTime( 7, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	pSendUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_ETC_ITEM, dwEtcItemType, iEtcItemCount, 0, 0, Help::GetAwardEtcItemBonusMent(),
								 kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
	pSendUser->SendPresentMemory();
}

bool ioPresentHelper::InsertUserSubscription( User *pUser, const ioHashString& szSubscriptionID, int iSubscriptionGold,
											  int iBonusCash, short iPresentType, int iBuyValue1, int iBuyValue2 )
{
	if( !pUser )
		return false;

	// 선물 Insert
	if( iPresentType == PRESENT_DECORATION )
		iPresentType = PRESENT_RANDOM_DECO;

	int iPresentValue1 = iBuyValue1;
	int iPresentValue2 = iBuyValue2;

	if( iPresentType == PRESENT_ETC_ITEM )
	{
		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iBuyValue1 );
		if( !pEtcItem )
			return false;

		iPresentValue2 = pEtcItem->GetValue( iBuyValue2 );
	}

	CTimeSpan cPresentGapTime( m_iUserSubscriptionPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	short iSubscriptionState = ioUserSubscription::SUBSCRIPTION_STATE_NORMAL;

	g_DBClient.OnInsertSubscriptionData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(),
										 szSubscriptionID, iSubscriptionGold, iBonusCash,
										 iPresentType, iPresentValue1, iPresentValue2,
										 iSubscriptionState, kPresentTime );
	
	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "Subscription" );
	
	pUser->_OnSelectSubscription( 30 );
	
	return true;
}

bool ioPresentHelper::SendRandPresentBySelectGashapon( User* pUser, DWORD dwEtcItemType, IN const SelectGashaponValueVec& vSelect, DWORD dwErrorPacketID, DWORD dwErrorType, short& m_iType, int& m_iValue1, int& m_iValue2 )
{
	GashaponPresentInfo* pInfo = g_PresentHelper.GetGashaponPresentInfo( dwEtcItemType );
	if( !pInfo )
	{
		SP2Packet kPacket( dwErrorPacketID );
		PACKET_GUARD_bool(kPacket.Write(dwErrorType));
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s gashapon info empty.(%d:%s,%d)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), dwEtcItemType );

		return false;
	}

	static vGashaponPresent vPresentList;
	vPresentList.clear();
	vPresentList = pInfo->m_vGashaponPresentList;

	if( vPresentList.empty() )
	{
		SP2Packet kPacket( dwErrorPacketID );
		PACKET_GUARD_bool(kPacket.Write(dwErrorType));
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s gashapon present list empty.(%d:%s,%d,%d)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), dwEtcItemType, pInfo->m_vGashaponPresentList.size() );

		return false;
	}
	
#ifdef GASHAPON_PRESENT_LIMITED_QUANTITY_BY_BCKIM			// 2020-03-26 by bckim, 가차 선택 제한

	if( vPresentList.size() < MAX_GASHAPON_SELECT_SLOT )
	{
		SP2Packet kPacket( dwErrorPacketID );
		PACKET_GUARD_bool(kPacket.Write(dwErrorType));
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s gashapon present list under MAX_GASHAPON_SELECT_SLOT (%d:%s,%d,%d)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), dwEtcItemType, pInfo->m_vGashaponPresentList.size() );
		return false;
	}

	// 선택한 리스트 제한 카운트 체크 
	int iCount = 0;
	for( unsigned int ii = 0; ii < vSelect.size(); ++ii )
	{
		const SelectGashaponValue& rkSelect = vSelect[ii];			// 선택된 아이템 리스트 	
		for( unsigned int jj = 0; jj < vPresentList.size(); jj++ )	
		{
			const GashaponPresent& rkPresent = vPresentList[jj];
			if( rkSelect.m_iType == rkPresent.m_iPresentType && rkSelect.m_iValue1 == rkPresent.m_iPresentValue1 && rkSelect.m_iValue2 == rkPresent.m_iPresentValue2 )
			{
				if( rkPresent.m_bLimited_Flag )
					iCount = iCount + 1;

				if ( iCount > pInfo->m_iLimited_Quantity )
				{
					SP2Packet kPacket( dwErrorPacketID );
					PACKET_GUARD_bool(kPacket.Write(BUY_SELECT_GASHAPON_LIMITED_COUNT_OVER));
					pUser->SendMessage( kPacket );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s gashapon present BUY_SELECT_GASHAPON_LIMITED_COUNT_OVER.(%d:%s,%d,%d)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), dwEtcItemType, pInfo->m_vGashaponPresentList.size() );
					return false;
				}
			}
		}
	}
#endif // GASHAPON_PRESENT_LIMITED_QUANTITY_BY_BCKIM		// End. 2020-03-26 by bckim, 가차 선택 제한

	static vGashaponPresent vRandPresent;
	vRandPresent.clear();

	for( unsigned int i = 0; i < vSelect.size(); ++i )
	{
		const SelectGashaponValue& rkSelect = vSelect[i];
		for( unsigned int j = 0; j < vPresentList.size(); )
		{
			//선택한 가챠가 선물목록에 존재하는지 확인
			const GashaponPresent& rkPresent = vPresentList[j];
			
			if( rkSelect.m_iType == rkPresent.m_iPresentType && rkSelect.m_iValue1 == rkPresent.m_iPresentValue1 && rkSelect.m_iValue2 == rkPresent.m_iPresentValue2 )
			{
				//선택 가챠는 랜덤 목록에 담고 선물 목록에서 제거한다.
				vRandPresent.push_back( rkPresent );
				vPresentList.erase( vPresentList.begin() + j );
			}
			else
			{
				++j;
			}
		}
	}

	//선택한 가챠가 랜덤 목록에 하나라도 없는 경우
	if( vSelect.size() != vRandPresent.size() )
	{
		SP2Packet kPacket( dwErrorPacketID );
		PACKET_GUARD_bool(kPacket.Write(dwErrorType));
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s rand present list worng.(%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return false;
	}

	io_random_shuffle( vPresentList.begin(), vPresentList.end() );

	int iRandSize = MAX_GASHAPON_SELECT_SLOT - (int)vSelect.size();
#ifdef GASHAPON_PRESENT_LIMITED_QUANTITY_BY_BCKIM			// 2020-03-26 by bckim, 가차 선택 제한			
	for( int k = 0; k < iRandSize;)
	{
		const GashaponPresent& rkPresent = vPresentList[k];
		if ( rkPresent.m_bLimited_Flag )				// 플레그 ON 
		{
			if( pInfo->m_iLimited_Quantity > iCount )
			{
				vRandPresent.push_back( vPresentList[k] );
				iCount = iCount + 1;			
				++k;
			}
			else
			{
				++k;
			}
		}
		else				// 플레그 OFF
		{
			vRandPresent.push_back( vPresentList[k] );
			++k;
		}
	}
#else
	for( int i = 0; i < iRandSize; i++ )
	{
		vRandPresent.push_back( vPresentList[i] );
	}
#endif // GASHAPON_PRESENT_LIMITED_QUANTITY_BY_BCKIM		// End. 2020-03-26 by bckim, 가차 선택 제한
	

	DWORD dwTotalRand = 0;
	for( int i = 0; i < (int)vRandPresent.size(); i++ )
	{
		dwTotalRand += vRandPresent[i].m_dwRand;
	}

	IORandom mRand;
	mRand.SetRandomSeed( timeGetTime() );
	DWORD dwRand = mRand.Random( 0, dwTotalRand );	

	DWORD dwCurValue = 0;	
	for( int i = 0; i < (int)vRandPresent.size(); i++ )
	{
		if( COMPARE( dwRand, dwCurValue, dwCurValue + vRandPresent[i].m_dwRand ) )
		{
			//기간 설정
			CTimeSpan cPresentGapTime( pInfo->m_iGashaponPeriod, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

			//선물 지급
			pUser->AddPresentMemory( g_MainServer.GetSendID(),
				vRandPresent[i].m_iPresentType,
				vRandPresent[i].m_iPresentValue1,
				vRandPresent[i].m_iPresentValue2,
				vRandPresent[i].m_iPresentValue3,
				vRandPresent[i].m_iPresentValue4,
				pInfo->m_iGashaponMent, 
				kPresentTime,
				pInfo->m_iGashaponAlarm );

			//로그
			char szNote[MAX_PATH]="";
			StringCbPrintf( szNote, sizeof( szNote ) , "SelectGashapon:%d", dwEtcItemType );
			g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(),
				g_App.GetPublicIP().c_str(),
				pUser->GetUserIndex(),
				vRandPresent[i].m_iPresentType,
				vRandPresent[i].m_iPresentValue1,
				vRandPresent[i].m_iPresentValue2,
				vRandPresent[i].m_iPresentValue3,
				vRandPresent[i].m_iPresentValue4,
				LogDBClient::PST_RECIEVE,
				szNote );

			// 페소도 동시 지급
			if( vRandPresent[i].m_iPresentPeso != 0 )
			{
				pUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_PESO , vRandPresent[i].m_iPresentPeso, 0, 0, 0, pInfo->m_iGashaponMent, kPresentTime, pInfo->m_iGashaponAlarm );
				g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), PRESENT_PESO, vRandPresent[i].m_iPresentPeso, 0, 0, 0, LogDBClient::PST_RECIEVE, "SelectGashapon" );
			}	

			//결과
			m_iType   = vRandPresent[i].m_iPresentType;
			m_iValue1 = vRandPresent[i].m_iPresentValue1;
			m_iValue2 = vRandPresent[i].m_iPresentValue2;
			
			return true;
		}
		else
		{
			dwCurValue += vRandPresent[i].m_dwRand;
		}
	}
	
	SP2Packet kPacket( dwErrorPacketID );
	PACKET_GUARD_bool(kPacket.Write(dwErrorType));
	pUser->SendMessage( kPacket );	
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s rand present select failed.(%d:%s) - %d, %d", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), dwRand, dwCurValue );

	return false;
}

bool ioPresentHelper::InsertUserPopupItem( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwSendUserIndex, const ioHashString &rszPublicID, const char *szPublicIP, int iPresentType, int iBuyValue1, int iBuyValue2, bool bBonus )
{
	if( dwSendUserIndex == 0 )
		return false;

	if( !COMPARE( iPresentType, PRESENT_SOLDIER, MAX_PRESENT_TYPE ) )
	{
		return false;
	}

	// 일반 치장은 용병을 선택할 수 없어 랜덤치장으로 속성을 변경한다.
	if( iPresentType == PRESENT_DECORATION )
		iPresentType = PRESENT_RANDOM_DECO;

	int iPresentValue1 = iBuyValue1;
	int iPresentValue2 = iBuyValue2;
	int iPresentValue3 = 0;
	int iPresentValue4 = 0;

	int iPresentMent = m_iUserPresentMent;
	if( bBonus )
	{
		if( iPresentType == PRESENT_PESO )
			iPresentMent = m_iUserPresentBonusPesoMent;
		else
			iPresentMent = m_iUserPresentBonusItemMent;
	}

	//팝업용인거 표시!
	iPresentMent = m_iUserPresentPopupNoMent;

	//HRYOON 20141222 마일리지 선물함으로 쏨, PRESENT TYPE 을 ETC 3 번으로, 마일리지는 VALUE2 에 재 설정해줌, 확인차!
	if( iPresentValue1 == ioEtcItem::EIT_ETC_MILEAGE_COIN && g_EventMgr.isMileageShopOpen() )
	{
		iPresentType = PRESENT_ETC_ITEM;
		iPresentValue2 = iBuyValue2;		                                             
	}

	CTimeSpan cPresentGapTime( m_iUserPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	g_DBClient.OnInsertPresentDataByUserIndex( dwAgentID, dwThreadID, dwSendUserIndex, /*dwRecvUserIndex*/ dwSendUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iPresentMent, kPresentTime, m_iUserPresentAlarm );

	//szNode를 이요해서 popup 아이템인지 구분..
	char szNote[MAX_PATH]="";
	if( bBonus )
		StringCbPrintf( szNote, sizeof( szNote ) , "User(Bonus)" );
	else
		StringCbPrintf( szNote, sizeof( szNote ) , "User" );

	if( iPresentValue1 == ioEtcItem::EIT_ETC_MILEAGE_COIN && g_EventMgr.isMileageShopOpen() )
		g_LogDBClient.OnInsertPresent( 0, "DeveloperK", szPublicIP, dwSendUserIndex, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, LogDBClient::PST_RECIEVE, szNote );

	return true;
}

bool ioPresentHelper::SendRandPresentByRisingGashapon( User* pUser, DWORD dwEtcItemType, DWORD dwErrorPacketID, DWORD dwErrorType, short& m_iType, int& m_iValue1, int& m_iValue2, int& m_iGashaponIndex )
{
	GashaponPresentInfo* pInfo = g_PresentHelper.GetGashaponPresentInfo( dwEtcItemType );
	if( !pInfo )
	{
		SP2Packet kPacket( dwErrorPacketID );
		PACKET_GUARD_bool(kPacket.Write(dwErrorType));
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s gashapon info empty.(%d:%s,%d)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), dwEtcItemType );

		return false;
	}

	static vGashaponPresent vPresentList;
	vPresentList.clear();
	vPresentList = pInfo->m_vGashaponPresentList;

	if( vPresentList.empty() )
	{
		SP2Packet kPacket( dwErrorPacketID );
		PACKET_GUARD_bool(kPacket.Write(dwErrorType));
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s gashapon present list empty.(%d:%s,%d,%d)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), dwEtcItemType, pInfo->m_vGashaponPresentList.size() );

		return false;
	}


	// 획득한 아이템 리스트
	std::vector<int> vGettedIndex = pUser->GetRisingGetIndex();

	// 이미 획득한 아이템의 수가 가샤폰 수보다 많다.
	if(vGettedIndex.size() > vPresentList.size())
	{
		SP2Packet kReturn( STPK_BUY_RISING_GASHAPON_RESULT );
		PACKET_GUARD_bool(kReturn.Write(BUY_RISING_GASHAPON_EXCEPTION));
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - Getted Item size is bigger than gashapon -%s : %d, %d", __FUNCTION__ , pUser->GetPublicID().c_str(), dwEtcItemType, vGettedIndex.size() );
		return false;
	}


	ioRisingGashapon * pItem = dynamic_cast<ioRisingGashapon *>(g_EtcItemMgr.FindEtcItem( dwEtcItemType ));
	if( !pItem )
	{
		SP2Packet kReturn( STPK_BUY_RISING_GASHAPON_RESULT );
		PACKET_GUARD_bool(kReturn.Write(BUY_RISING_GASHAPON_EXCEPTION));
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - EtcItem NULL -%s : %d", __FUNCTION__ , pUser->GetPublicID().c_str(), dwEtcItemType );
		return false;
	}

	// 미끼아이템을 뽑을 순서 인가?
	bool bFocusItem = false;

	// 미끼아이템이 이미 뽑혀있는가?
	int iFocusIndex = pItem->GetFocusIndex() - 1;
	if(pUser->GetRisingBuyCount() == ( pItem->GetFocusCount() - 1 ))
	{
		bFocusItem = true;
		for(int getted_count = 0; getted_count < vGettedIndex.size() ; ++getted_count)
		{
			if(vGettedIndex[getted_count] == iFocusIndex)
			{
				bFocusItem = false;
				break;
			}
		}
	}


	DWORD dwRand = 0;
	DWORD dwCurValue = 0;	
	if(bFocusItem)
	{

		SendRisingPresentProcess(pUser, pInfo, vPresentList[iFocusIndex]);

		//결과
		m_iType   = vPresentList[iFocusIndex].m_iPresentType;
		m_iValue1 = vPresentList[iFocusIndex].m_iPresentValue1;
		m_iValue2 = vPresentList[iFocusIndex].m_iPresentValue2;
		m_iGashaponIndex = iFocusIndex;

		return true;

	}
	else
	{
		static vGashaponPresent vRandPresent;
		vRandPresent.clear();

		// 획득한 아이템 제외한 리스트 뽑기.
		bool bMatch = false;
		for(int present_count = 0; present_count < vPresentList.size() ; ++present_count)
		{
			bMatch = false;
			for(int getted_count = 0; getted_count < vGettedIndex.size() ; ++getted_count)
			{
				if(vGettedIndex[getted_count] == present_count)
				{
					bMatch = true;
					break;
				}
			}
			if(!bMatch)
			{
				vRandPresent.push_back(vPresentList[present_count]);
			}
		}

		if(vRandPresent.empty())
		{
			SP2Packet kReturn( STPK_BUY_RISING_GASHAPON_RESULT );
			PACKET_GUARD_bool(kReturn.Write(BUY_RISING_GASHAPON_EXCEPTION));
			pUser->SendMessage( kReturn );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - Random Item empty -%s : %d, %d", __FUNCTION__ , pUser->GetPublicID().c_str(), dwEtcItemType, vGettedIndex.size() );
			return false;

		}

		io_random_shuffle( vRandPresent.begin(), vRandPresent.end() );


		DWORD dwTotalRand = 0;
		for( int i = 0; i < (int)vRandPresent.size(); i++ )
		{
			dwTotalRand += vRandPresent[i].m_dwRand;
		}

		IORandom mRand;
		mRand.SetRandomSeed( timeGetTime() );
		dwRand = mRand.Random( 0, dwTotalRand );	

		for( int i = 0; i < (int)vRandPresent.size(); i++ )
		{
			if( COMPARE( dwRand, dwCurValue, dwCurValue + vRandPresent[i].m_dwRand ) )
			{

				SendRisingPresentProcess(pUser, pInfo, vRandPresent[i]);
				//결과
				m_iType   = vRandPresent[i].m_iPresentType;
				m_iValue1 = vRandPresent[i].m_iPresentValue1;
				m_iValue2 = vRandPresent[i].m_iPresentValue2;
				// 가샤폰 인덱스 구하기
				for(int present_count = 0; present_count < vPresentList.size() ; ++present_count)
				{
					if(vPresentList[present_count].m_iPresentType == vRandPresent[i].m_iPresentType &&
						vPresentList[present_count].m_iPresentValue1 == vRandPresent[i].m_iPresentValue1 &&
						vPresentList[present_count].m_iPresentValue2 == vRandPresent[i].m_iPresentValue2 &&
						vPresentList[present_count].m_iPresentPeso == vRandPresent[i].m_iPresentPeso)
					{
						m_iGashaponIndex = present_count;
						break;
					}
				}

				return true;
			}
			else
			{
				dwCurValue += vRandPresent[i].m_dwRand;
			}
		}
	}

	SP2Packet kPacket( dwErrorPacketID );
	PACKET_GUARD_bool(kPacket.Write(dwErrorType));
	pUser->SendMessage( kPacket );	
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s rand present rising failed.(%d:%s) - %d, %d", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), dwRand, dwCurValue );

	return false;

}

void ioPresentHelper::SendRisingPresentProcess( User* pUser, GashaponPresentInfo * pInfo, GashaponPresent & rkPresent )
{
	//기간 설정
	CTimeSpan cPresentGapTime( pInfo->m_iGashaponPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

	//선물 지급
	pUser->AddPresentMemory( g_MainServer.GetSendID(),
		rkPresent.m_iPresentType,
		rkPresent.m_iPresentValue1,
		rkPresent.m_iPresentValue2,
		rkPresent.m_iPresentValue3,
		rkPresent.m_iPresentValue4,
		pInfo->m_iGashaponMent, 
		kPresentTime,
		pInfo->m_iGashaponAlarm );

	//로그
	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "RisingGashapon:%d", pInfo->m_dwEtcItemType );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(),
		g_App.GetPublicIP().c_str(),
		pUser->GetUserIndex(),
		rkPresent.m_iPresentType,
		rkPresent.m_iPresentValue1,
		rkPresent.m_iPresentValue2,
		rkPresent.m_iPresentValue3,
		rkPresent.m_iPresentValue4,
		LogDBClient::PST_RECIEVE,
		szNote );

	// 페소도 동시 지급
	if( rkPresent.m_iPresentPeso != 0 )
	{
		pUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_PESO , rkPresent.m_iPresentPeso, 0, 0, 0, pInfo->m_iGashaponMent, kPresentTime, pInfo->m_iGashaponAlarm );
		g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), PRESENT_PESO, rkPresent.m_iPresentPeso, 0, 0, 0, LogDBClient::PST_RECIEVE, "RisingGashapon" );
	}


}


