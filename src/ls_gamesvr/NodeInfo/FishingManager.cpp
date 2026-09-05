

#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "FishingManager.h"

extern CLog EventLOG;

template<> FishingManager* Singleton< FishingManager >::ms_Singleton = 0;

FishingManager::FishingManager()
{
	m_RandomSuccess.Randomize();
	m_RandomType.Randomize();
	m_RandomGrade.Randomize();
	m_RandomTime.Randomize();

	ClearAllInfo();
}

FishingManager::~FishingManager()
{
	ClearAllInfo();
}

void FishingManager::CheckNeedReload()
{
	// Base Info
	ioINILoader kLoader;
	kLoader.SetFileName( "config/sp2_fishing_info.ini" );
	if( kLoader.ReadBool( "common", "Change", false ) )
	{
		LoadBaseInfo();
	}

	// Rate Info
	//ioINILoader kLoader2( "config/sp2_fishing_rate.ini", false );
	ioINILoader kLoader2;
	kLoader2.SetFileName( "config/sp2_fishing_rate.ini" );
	if( kLoader2.ReadBool( "common", "Change", false ) )
	{
		LoadRateInfo();
	}
}

void FishingManager::LoadFishingInfo()
{
	ClearAllInfo();

	LoadBaseInfo();
	LoadRateInfo();
}

// 2020-02-17 by bckim, 낚시 시스템 확장
void FishingManager::ClearPrivateBaseInfo()
{
	SetPrivateRewardCount(0);
	m_vPrivateRewardList.clear();
}
void FishingManager::ClearFishingPlaceInfo()
{
	m_vFishingPlaceInfoList.clear();
}

void FishingManager::LoadPrivateFishingInfo()
{
	ClearPrivateFishingAllInfo();

	LoadPrivateBaseInfo();
	LoadFishingPlaceInfo();
}

void FishingManager::ClearPrivateFishingAllInfo()				// load 모체 
{
	ClearPrivateBaseInfo();
	ClearFishingPlaceInfo();		
}

void FishingManager::LoadPrivateBaseInfo()					// 보상 아이템 리스트 
{
	ClearPrivateBaseInfo();

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_private_fishing_info.ini" );

	kLoader.SetTitle( "BaseItemInfo" );
	//kLoader.SaveBool( "Change", false );

	char szKey[MAX_PATH] = "";

	int i = 0;
	int iItemCnt = kLoader.LoadInt( "max_item_cnt", 0 );		// max_item_cnt=6
	
	SetPrivateRewardCount(iItemCnt);

	for(int i=0; i < iItemCnt; ++i )
	{
		PrivateRewardInfo data;
		wsprintf( szKey, "Present%d_Type", i+1 );
		data.iType = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "Present%d_Value1", i+1 );
		data.iValue1 = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "Present%d_Value2", i+1 );
		data.iValue2 = kLoader.LoadInt( szKey, 1 );

		wsprintf( szKey, "Present%d_Period", i+1 );
		data.iPresentPeriod = kLoader.LoadInt( szKey, 7 );

		wsprintf( szKey, "Present%d_Ment", i+1 );
		data.iMent = kLoader.LoadInt( szKey, 0 );
				
		m_vPrivateRewardList.push_back( data );	// 아이템 리스트 
	}

	if( iItemCnt != m_vPrivateRewardList.size() )
	{
		// error 
		return;  
	}
}
void FishingManager::LoadFishingPlaceInfo()				// 낚싯터 정보 
{
	ClearFishingPlaceInfo();

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_private_fishing_info.ini" );

	kLoader.SetTitle( "fishingplace" );

	char szKey[MAX_PATH] = "";
	int iPlaceCnt = kLoader.LoadInt( "max_place_cnt", 0 );		// max_item_cnt=6

	for( int i = 0; i < iPlaceCnt; i++ )
	{
		StringCbPrintf( szKey, sizeof( szKey ), "place%d", i+1 );
		kLoader.SetTitle(szKey);

		int iPlaceCnt = kLoader.LoadInt( "fishingplaceNum", 0 );		

		FishingPlaceInfo	PlaceData;
		PlaceData.iPlaceNum = kLoader.LoadInt( "fishingplaceNum", 0 );		
		PlaceData.dwOwnerRate = (DWORD)kLoader.LoadInt( "private_fishing_owner_rate", 0 );		
		PlaceData.dwNonOwnerRate = (DWORD)kLoader.LoadInt( "private_fishing_non_owner_rate", 0 );			

		int iItemCnt = GetPrivateRewardCount();
		for(int i=0; i < iItemCnt; ++i )
		{

			DWORD dwRate;
			wsprintf( szKey, "item%d_rate_value", i+1 );
			dwRate = (DWORD)kLoader.LoadInt( szKey, 0 );

			PrivateRewardInfo	RewardData;
			GetPrivateRewardInfo( i , RewardData);			// 확률이 있으면 위의 모체에서 아이템 정보 가져옴. 

			if( dwRate == 0 || RewardData.iValue1 == 0)
				continue;
			
			RewardData.dwRate = dwRate;
			PlaceData.vRewardInfo.push_back(RewardData);			
		}

		m_vFishingPlaceInfoList.push_back(PlaceData);
	}

	if( iPlaceCnt != m_vFishingPlaceInfoList.size() )
	{
		return;			// error 
	}
}

bool FishingManager::GetPrivateRewardInfo( int orderNum, PrivateRewardInfo &RewardInfo )		// 초기 ini 파일 로딩할때만 사용하는게 좋다. 	
{
	if ( m_vPrivateRewardList.empty() )
		return false;

	if( COMPARE(orderNum,0,m_vPrivateRewardList.size()) )
	{
		RewardInfo = m_vPrivateRewardList[orderNum];
		return true;
	}
	return false;		
}

bool FishingManager::GetFishingPlaceInfo( int iPlaceNum, FishingPlaceInfo &PlaceInfo )		
{
	int iSize = m_vFishingPlaceInfoList.size();
	if ( iSize == 0)
		return false;	

	for(int i=0; i<iSize; i++ )
	{
		FishingPlaceInfo tempdata = m_vFishingPlaceInfoList[i];
		if( tempdata.iPlaceNum == iPlaceNum  )
		{
			PlaceInfo = m_vFishingPlaceInfoList[i];
			return true;
		}
	}
	return false;
}
bool FishingManager::IsPrivateFishingSuccess( bool IsOwner, int iPlaceNum, PrivateRewardInfo &RewardInfo)
{
	// RND 
	DWORD dwRandValue = (DWORD)(rand()%1000);			// 기획팀 요청으로 기존 100 에서 1000 변경 

#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// 2020-02-17 by bckim, 낚시 시스템 확장
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[PRIVATE_FISHING] RESULT!!(1) SUCCESS_RND(%lu)",dwRandValue);
#endif	// FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// End. 2020-02-17 by bckim, 낚시 시스템 	

	FishingPlaceInfo PlaceInfo;
	GetFishingPlaceInfo( iPlaceNum, PlaceInfo);

	DWORD dwToRange = 0;
	if( IsOwner )
		dwToRange = PlaceInfo.GetOwnerRate();		// 주인 확률
	else
		dwToRange = PlaceInfo.GetNonOwnerRate();		// 주인 아님. 
	
#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// 2020-02-17 by bckim, 낚시 시스템 확장
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[PRIVATE_FISHING] RESULT!!(2) IsOwner[%d] SUCCESS_RND(0::%lu::%lu)",IsOwner,dwRandValue,dwToRange);
#endif	// FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// End. 2020-02-17 by bckim, 낚시 시스템 	

	
	if(  COMPARE(dwRandValue,0,dwToRange) )			// 
	{
		DWORD dwTotalRange = 0;
		int iSize = PlaceInfo.vRewardInfo.size();
		for( int i=0; i<iSize ; i++)
		{
			dwTotalRange = dwTotalRange + PlaceInfo.vRewardInfo[i].dwRate;
		}

		if ( dwTotalRange == 0 )
			return false;

		DWORD GetRND = 	(DWORD)(rand()%dwTotalRange);	
		DWORD dwEndOfRange = 0;

#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// 2020-02-17 by bckim, 낚시 시스템 확장
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[PRIVATE_FISHING] RESULT!!(3) ITEM_RND(0<%lu<%lu)",GetRND,dwTotalRange);
#endif	// FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// End. 2020-02-17 by bckim, 낚시 시스템 	

		for( int j=0; j<iSize ; j++)
		{
			PrivateRewardInfo tempdata = PlaceInfo.vRewardInfo[j];
			dwEndOfRange = dwEndOfRange + tempdata.dwRate;
			if (  COMPARE(GetRND,0,dwEndOfRange) )
			{
				RewardInfo = tempdata;
				return true;
			}
		}		
	}
	return false;
}

// End. 2020-02-17 by bckim, 낚시 시스템 확장

void FishingManager::LoadBaseInfo()
{
	ClearBaseInfo();

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_fishing_info.ini" );

	kLoader.SetTitle( "common" );
	//kLoader.SaveBool( "Change", false );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	int i = 0;
	int iTypeCnt = kLoader.LoadInt( "type_cnt", 0 );

	for(int i=0; i < iTypeCnt; ++i )
	{
		// SuccessRate
		DWORD dwRate;
		wsprintf( szKey, "fishing_success_rate%d", i+1 );
		dwRate = (DWORD)kLoader.LoadInt( szKey, 0 );

		m_vSuccessRateList.push_back( dwRate );

		// Time
		FishingTime kTime;
		wsprintf( szKey, "fishing_min_time%d", i+1 );
		kTime.m_dwMinTime = (DWORD)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "fishing_max_time%d", i+1 );
		kTime.m_dwMaxTime = (DWORD)kLoader.LoadInt( szKey, 0 );

		m_vFishingTimeList.push_back( kTime );
	}

	// PCRoom
	m_iPCRoomSuccesPlusRate		= kLoader.LoadInt( "pcroom_succes_plus_rate", 0 );
	m_iPCRoomFishTimeMinusMin   = kLoader.LoadInt( "pcroom_fish_time_minus_min", 0 );
	m_iPCRoomFishTimeMinusMax   = kLoader.LoadInt( "pcroom_fish_time_minus_max", 0 );

	// BaseItemInfo
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadFishing - BaseInfo INI Start!!!!!!!!!!" );

	kLoader.SetTitle( "BaseItemInfo" );

	int iBaseItemCnt = kLoader.LoadInt( "max_item_cnt", 0 );
	for(int i=0; i < iBaseItemCnt; ++i )
	{
		FishingItemInfo kItem;

		wsprintf( szKey, "item%d_num", i+1 );
		kItem.m_iItemNum = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_peso", i+1 );
		kItem.m_iPeso = (__int64)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_room_alarm", i+1 );
		kItem.m_bRoomAlarm = kLoader.LoadBool( szKey, 0 );

		wsprintf( szKey, "item%d_all_alarm", i+1 );
		kItem.m_bAllAlarm = kLoader.LoadBool( szKey, 0 );

		wsprintf( szKey, "item%d_sell_alarm", i+1 );
		kItem.m_bSellAlarm = kLoader.LoadBool( szKey, 0 );

		wsprintf( szKey, "item%d_special", i+1 );
		kItem.m_bSpecial = kLoader.LoadBool( szKey, 0 );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] - Num: %d, Special: %d", i + 1, kItem.m_iItemNum, kItem.m_bSpecial );

		m_vFishingItemList.push_back( kItem );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadFishing - BaseInfo INI End - InfoCnt: %d", iBaseItemCnt );

	//길드 낚시터에서 나올 수 있는 아이템 정의.
	m_vGuildFisheryItemList.clear();
	kLoader.SetTitle( "GuildFisheryItem" );

	for(int i=0; i < 500; ++i )
	{
		FishingItemInfo kItem;

		wsprintf( szKey, "item%d_num", i+1 );
		kItem.m_iItemNum = kLoader.LoadInt( szKey, 0 );

		if( 0 == kItem.m_iItemNum )
			break;

		wsprintf( szKey, "item%d_peso", i+1 );
		kItem.m_iPeso = (__int64)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_room_alarm", i+1 );
		kItem.m_bRoomAlarm = kLoader.LoadBool( szKey, 0 );

		wsprintf( szKey, "item%d_all_alarm", i+1 );
		kItem.m_bAllAlarm = kLoader.LoadBool( szKey, 0 );

		wsprintf( szKey, "item%d_sell_alarm", i+1 );
		kItem.m_bSellAlarm = kLoader.LoadBool( szKey, 0 );

		wsprintf( szKey, "item%d_special", i+1 );
		kItem.m_bSpecial = kLoader.LoadBool( szKey, 0 );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%02d] - Num: %d, Special: %d", i + 1, kItem.m_iItemNum, kItem.m_bSpecial );

		m_vGuildFisheryItemList.push_back( kItem );
	}

	// ItemGradeList
	kLoader.SetTitle( "GradeInfo" );

	int iGradeCnt = kLoader.LoadInt( "max_grade_cnt", 0 );
	for( int j=0; j < iGradeCnt; ++j )
	{
		FishingItemGradeInfo kGrade;

		wsprintf( szKey, "grade%d_num", j+1 );
		kGrade.m_iGradeNum = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "grade%d_value", j+1 );
		kGrade.m_fValue = kLoader.LoadFloat( szKey, 1.0f );

		wsprintf( szKey, "grade%d_alarm", j+1 );
		kGrade.m_bAlarm = kLoader.LoadBool( szKey, false );

		m_vFishingItemGradeList.push_back( kGrade );
	}
}

void FishingManager::LoadRateInfo()
{
	ClearRateInfo();

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_fishing_rate.ini" );

	kLoader.SetTitle( "common" );
	//kLoader.SaveBool( "Change", false );

	m_bTestFishing = kLoader.LoadBool( "test_fishing", false );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	int i = 0;
	int j = 0;

	// Normal Rate
	kLoader.SetTitle( "NormalItemRate" );	

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadFishing - Normal Rate INI Start!!!!!!!!!!" );

	// Item Rate
	int iItemCnt = kLoader.LoadInt( "max_item_cnt", 0 );
	for(int i=0; i < iItemCnt; ++i )
	{
		FishingItemRate kItem;

		wsprintf( szKey, "item%d_num", i+1 );
		kItem.m_iItemNum = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_event_present", i+1 );
		kItem.m_iEventPresent = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_rate", i+1 );
		kItem.m_dwRate = (DWORD)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_sell_alarm", i+1 );
		kItem.m_bSellAlarm = kLoader.LoadBool( szKey, 0 );

		m_dwItemListTotal += kItem.m_dwRate;
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItem%02d - Num: %d, Rate: %d, Event: %d", i + 1, kItem.m_iItemNum, kItem.m_dwRate, kItem.m_iEventPresent );

		m_vNormalRateList.push_back( kItem );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItem TotalRate: %d", m_dwItemListTotal );

	// Grade Rate
	int iGradeCnt = kLoader.LoadInt( "max_grade_cnt", 0 );
	for( j=0; j < iGradeCnt; ++j )
	{
		FishingItemGradeRate kGrade;

		wsprintf( szKey, "grade%d_num", j+1 );
		kGrade.m_iGradeNum = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "grade%d_value", j+1 );
		kGrade.m_fValue = kLoader.LoadFloat( szKey, 1.0f );

		wsprintf( szKey, "grade%d_rate", j+1 );
		kGrade.m_dwRate = (DWORD)kLoader.LoadInt( szKey, 0 );

		m_dwGradeTotal += kGrade.m_dwRate;
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItemGrade%02d - Num: %d, Rate: %d", j + 1, kGrade.m_iGradeNum, kGrade.m_dwRate );

		m_vNormalGradeRateList.push_back( kGrade );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItemGrade TotalRate: %d", m_dwGradeTotal );

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadFishing - Normal Rate INI End !!!!!!!!!" );



	// Event Item Rate
	kLoader.SetTitle( "EventItemRate" );

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadFishing - Event Rate INI Start!!!!!!!!!!" );

	iItemCnt = kLoader.LoadInt( "max_item_cnt", 0 );
	for(int i=0; i < iItemCnt; ++i )
	{
		FishingItemRate kItem;

		wsprintf( szKey, "item%d_num", i+1 );
		kItem.m_iItemNum = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_event_present", i+1 );
		kItem.m_iEventPresent = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_rate", i+1 );
		kItem.m_dwRate = (DWORD)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_sell_alarm", i+1 );
		kItem.m_bSellAlarm = kLoader.LoadBool( szKey, 0 );

		m_dwEventItemListTotal += kItem.m_dwRate;
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItem%02d - Num: %d, Rate: %d, Event: %d", i + 1, kItem.m_iItemNum, kItem.m_dwRate, kItem.m_iEventPresent );

		m_vEventRateList.push_back( kItem );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItem TotalRate: %d", m_dwItemListTotal );

	// Event Grade Rate
	iGradeCnt = kLoader.LoadInt( "max_grade_cnt", 0 );
	for( j=0; j < iGradeCnt; ++j )
	{
		FishingItemGradeRate kGrade;

		wsprintf( szKey, "grade%d_num", j+1 );
		kGrade.m_iGradeNum = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "grade%d_value", j+1 );
		kGrade.m_fValue = kLoader.LoadFloat( szKey, 1.0f );

		wsprintf( szKey, "grade%d_rate", j+1 );
		kGrade.m_dwRate = (DWORD)kLoader.LoadInt( szKey, 0 );

		m_dwEventGradeTotal += kGrade.m_dwRate;
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItemGrade%02d - Num: %d, Rate: %d", j + 1, kGrade.m_iGradeNum, kGrade.m_dwRate );

		m_vEventGradeRateList.push_back( kGrade );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItemGrade TotalRate: %d", m_dwEventGradeTotal );

	//
	// PCRoom Item Rate
	kLoader.SetTitle( "PCRoomItemRate" );

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadFishing - PCRoom Rate INI Start!!!!!!!!!!" );

	iItemCnt = kLoader.LoadInt( "max_item_cnt", 0 );
	for(int i=0; i < iItemCnt; ++i )
	{
		FishingItemRate kItem;

		wsprintf( szKey, "item%d_num", i+1 );
		kItem.m_iItemNum = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_event_present", i+1 );
		kItem.m_iEventPresent = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_rate", i+1 );
		kItem.m_dwRate = (DWORD)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_sell_alarm", i+1 );
		kItem.m_bSellAlarm = kLoader.LoadBool( szKey, 0 );

		m_dwPCRoomItemListTotal += kItem.m_dwRate;
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItem%02d - Num: %d, Rate: %d, Event: %d", i + 1, kItem.m_iItemNum, kItem.m_dwRate, kItem.m_iEventPresent );

		m_vPCRoomRateList.push_back( kItem );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItem TotalRate: %d", m_dwPCRoomItemListTotal );

	// PCRoom Grade Rate
	iGradeCnt = kLoader.LoadInt( "max_grade_cnt", 0 );
	for( j=0; j < iGradeCnt; ++j )
	{
		FishingItemGradeRate kGrade;

		wsprintf( szKey, "grade%d_num", j+1 );
		kGrade.m_iGradeNum = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "grade%d_value", j+1 );
		kGrade.m_fValue = kLoader.LoadFloat( szKey, 1.0f );

		wsprintf( szKey, "grade%d_rate", j+1 );
		kGrade.m_dwRate = (DWORD)kLoader.LoadInt( szKey, 0 );

		m_dwPCRoomGradeTotal += kGrade.m_dwRate;
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItemGrade%02d - Num: %d, Rate: %d", j + 1, kGrade.m_iGradeNum, kGrade.m_dwRate );

		m_vPCRoomGradeRateList.push_back( kGrade );
	}
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItemGrade TotalRate: %d", m_dwPCRoomGradeTotal );


	// Guild
	kLoader.SetTitle( "GuildFisheryItemRate" );

	for(int i=0; i < 500; ++i )
	{
		FishingItemRate kItem;

		wsprintf( szKey, "item%d_num", i+1 );
		kItem.m_iItemNum = kLoader.LoadInt( szKey, 0 );

		if( 0 == kItem.m_iItemNum )
			break;

		wsprintf( szKey, "item%d_event_present", i+1 );
		kItem.m_iEventPresent = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_rate", i+1 );
		kItem.m_dwRate = (DWORD)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "item%d_sell_alarm", i+1 );
		kItem.m_bSellAlarm = kLoader.LoadBool( szKey, 0 );

		m_dwGuildFisheryItemListTotal += kItem.m_dwRate;
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingItem%02d - Num: %d, Rate: %d, Event: %d", i + 1, kItem.m_iItemNum, kItem.m_dwRate, kItem.m_iEventPresent );

		m_vGuildFisheryRateList.push_back( kItem );
	}

	// Guild
	for( j=0; j < 100; ++j )
	{
		FishingItemGradeRate kGrade;

		wsprintf( szKey, "grade%d_num", j+1 );
		kGrade.m_iGradeNum = kLoader.LoadInt( szKey, 0 );

		if( 0 == kGrade.m_iGradeNum )
			break;

		wsprintf( szKey, "grade%d_value", j+1 );
		kGrade.m_fValue = kLoader.LoadFloat( szKey, 1.0f );

		wsprintf( szKey, "grade%d_rate", j+1 );
		kGrade.m_dwRate = (DWORD)kLoader.LoadInt( szKey, 0 );

		m_dwGuildFisheryGradeTotal += kGrade.m_dwRate;

		m_vGuildFisheryGradeRateList.push_back( kGrade );
	}
	//
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LoadFishing - Event Rate INI End !!!!!!!!!" );
}

void FishingManager::ClearAllInfo()
{
	m_vFishingTimeList.clear();
	m_vSuccessRateList.clear();

	m_vFishingItemList.clear();
	m_vFishingItemGradeList.clear();

	m_vNormalRateList.clear();
	m_vEventRateList.clear();
	m_vPCRoomRateList.clear();

	m_dwItemListTotal = 0;
	m_dwEventItemListTotal = 0;
	m_dwPCRoomItemListTotal = 0;

	m_vNormalGradeRateList.clear();
	m_vEventGradeRateList.clear();
	m_vPCRoomGradeRateList.clear();

	m_dwGradeTotal = 0;
	m_dwEventGradeTotal = 0;
	m_dwPCRoomGradeTotal = 0;

	m_iPCRoomSuccesPlusRate = 0;
	m_iPCRoomFishTimeMinusMin = 0;
	m_iPCRoomFishTimeMinusMax = 0;

	m_dwGuildFisheryItemListTotal	= 0;
	m_dwGuildFisheryGradeTotal		= 0;

	m_vGuildFisheryItemList.clear();
	m_vGuildFisheryRateList.clear();
	m_vGuildFisheryGradeRateList.clear();
}

void FishingManager::ClearBaseInfo()
{
	m_vFishingTimeList.clear();
	m_vSuccessRateList.clear();

	m_vFishingItemList.clear();
	m_vFishingItemGradeList.clear();
}

void FishingManager::ClearRateInfo()
{
	m_vNormalRateList.clear();
	m_vEventRateList.clear();
	m_vPCRoomRateList.clear();

	m_dwItemListTotal = 0;
	m_dwEventItemListTotal = 0;
	m_dwPCRoomItemListTotal = 0;

	m_vNormalGradeRateList.clear();
	m_vEventGradeRateList.clear();
	m_vPCRoomGradeRateList.clear();

	m_dwGradeTotal = 0;
	m_dwEventGradeTotal = 0;
	m_dwPCRoomGradeTotal = 0;
}

FishingManager& FishingManager::GetSingleton()
{
	return Singleton< FishingManager >::GetSingleton();
}

bool FishingManager::IsFishingSuccess( int iType, bool bPCRoom )
{
	int iNewType = iType - 1;
	DWORD dwCurRate = 0;
	if( COMPARE( iNewType, 0, (int)m_vSuccessRateList.size() ) )
	{
		dwCurRate = m_vSuccessRateList[iNewType];
	}

	if( bPCRoom )
	{
		dwCurRate = min( 100, dwCurRate + (DWORD)m_iPCRoomSuccesPlusRate );
	}

	if( dwCurRate == 0 )
		return false;

	DWORD dwRand = m_RandomSuccess.Random( 100 );

	if( COMPARE( dwRand, 0, dwCurRate ) )
		return true;
	
	return false;
}

int FishingManager::GetFishingItemNum( bool bEvent, bool bPCRoom, bool bGuildFishery )
{
	DWORD dwRand = 0;
	DWORD dwCurValue = 0;

	if( bGuildFishery )
	{
		dwRand = m_RandomType.Random( m_dwGuildFisheryItemListTotal );
		int iSize = m_vGuildFisheryRateList.size();
		dwCurValue = 0;

		for( int i=0; i < iSize; ++i )
		{
			DWORD dwValue = m_vGuildFisheryRateList[i].m_dwRate;

			if( COMPARE( dwRand, dwCurValue, dwCurValue+dwValue ) )
			{
				return m_vGuildFisheryRateList[i].m_iItemNum;
			}

			dwCurValue += dwValue;
		}
	}
	else if( bEvent )
	{
		dwRand = m_RandomType.Random( m_dwEventItemListTotal );
		int iSize = m_vEventRateList.size();
		dwCurValue = 0;

		for( int i=0; i < iSize; ++i )
		{
			DWORD dwValue = m_vEventRateList[i].m_dwRate;

			if( COMPARE( dwRand, dwCurValue, dwCurValue+dwValue ) )
			{
				return m_vEventRateList[i].m_iItemNum;
			}

			dwCurValue += dwValue;
		}
	}
	else if( bPCRoom )
	{
		dwRand = m_RandomType.Random( m_dwPCRoomItemListTotal );
		int iSize = m_vPCRoomRateList.size();
		dwCurValue = 0;

		for( int i=0; i < iSize; ++i )
		{
			DWORD dwValue = m_vPCRoomRateList[i].m_dwRate;

			if( COMPARE( dwRand, dwCurValue, dwCurValue+dwValue ) )
			{
				return m_vPCRoomRateList[i].m_iItemNum;
			}

			dwCurValue += dwValue;
		}
	}
	else 
	{
		dwRand = m_RandomType.Random( m_dwItemListTotal );
		int iSize = m_vNormalRateList.size();
		dwCurValue = 0;

		for( int i=0; i < iSize; ++i )
		{
			DWORD dwValue = m_vNormalRateList[i].m_dwRate;

			if( COMPARE( dwRand, dwCurValue, dwCurValue+dwValue ) )
			{
				return m_vNormalRateList[i].m_iItemNum;
			}

			dwCurValue += dwValue;
		}
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FishingManager::GetFishingItemNum - Out of Bound: %d, %d", dwRand, dwCurValue );
	return -1;
}

__int64 FishingManager::GetFishingItemSellPeso( int iItemType, int iItemGrade, bool bEvent, bool bPCRoom, bool bGuildFishery )
{
	__int64 iResultPeso = 0;
	float fGradeRate = 1.0f;

	if( bGuildFishery )
	{
		int iSize = m_vGuildFisheryGradeRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			if( m_vGuildFisheryGradeRateList[i].m_iGradeNum == iItemGrade )
			{
				fGradeRate = m_vGuildFisheryGradeRateList[i].m_fValue;
				break;
			}
		}
	}
	else if( bEvent )
	{
		int iSize = m_vEventGradeRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			if( m_vEventGradeRateList[i].m_iGradeNum == iItemGrade )
			{
				fGradeRate = m_vEventGradeRateList[i].m_fValue;
				break;
			}
		}
	}
	else if( bPCRoom )
	{
		int iSize = m_vPCRoomGradeRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			if( m_vPCRoomGradeRateList[i].m_iGradeNum == iItemGrade )
			{
				fGradeRate = m_vPCRoomGradeRateList[i].m_fValue;
				break;
			}
		}
	}
	else
	{
		int iSize = m_vNormalGradeRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			if( m_vNormalGradeRateList[i].m_iGradeNum == iItemGrade )
			{
				fGradeRate = m_vNormalGradeRateList[i].m_fValue;
				break;
			}
		}
	}

	if( bGuildFishery )
	{
		BOOL bFind	= FALSE;

		LOOP_GUARD();
		FishingItemInfoList::iterator iter = m_vGuildFisheryItemList.begin();
		while( iter != m_vGuildFisheryItemList.end() )
		{
			if( (*iter).m_iItemNum == iItemType )
			{
				iResultPeso = (*iter).m_iPeso;
				iResultPeso *= fGradeRate;
				bFind		= TRUE;
				break;
			}

			++iter;
		}
		LOOP_GUARD_CLEAR();

		if( !bFind )
		{
			LOOP_GUARD();
			FishingItemInfoList::iterator iter = m_vFishingItemList.begin();
			while( iter != m_vFishingItemList.end() )
			{
				if( (*iter).m_iItemNum == iItemType )
				{
					iResultPeso = (*iter).m_iPeso;
					iResultPeso *= fGradeRate;
					break;
				}

				++iter;
			}
			LOOP_GUARD_CLEAR();
		}
	}
	else
	{
		LOOP_GUARD();
		FishingItemInfoList::iterator iter = m_vFishingItemList.begin();
		while( iter != m_vFishingItemList.end() )
		{
			if( (*iter).m_iItemNum == iItemType )
			{
				iResultPeso = (*iter).m_iPeso;
				iResultPeso *= fGradeRate;
				break;
			}

			++iter;
		}
		LOOP_GUARD_CLEAR();
	}
	

	return iResultPeso;
}

int FishingManager::GetGrade( bool bEvent, bool bPCRoom, bool bGuildFishery )
{
	DWORD dwRand = 0;

	if( bGuildFishery )
	{
		dwRand = m_RandomGrade.Random( m_dwGuildFisheryGradeTotal );

		DWORD dwCurValue = 0;
		int iSize = m_vGuildFisheryGradeRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			DWORD dwValue = m_vGuildFisheryGradeRateList[i].m_dwRate;
			if( COMPARE( dwRand, dwCurValue, dwCurValue+dwValue ) )
				return m_vGuildFisheryGradeRateList[i].m_iGradeNum;

			dwCurValue += dwValue;
		}
	}
	else if( bEvent )
	{
		dwRand = m_RandomGrade.Random( m_dwEventGradeTotal );

		DWORD dwCurValue = 0;
		int iSize = m_vEventGradeRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			DWORD dwValue = m_vEventGradeRateList[i].m_dwRate;
			if( COMPARE( dwRand, dwCurValue, dwCurValue+dwValue ) )
				return m_vEventGradeRateList[i].m_iGradeNum;

			dwCurValue += dwValue;
		}
	}
	else if( bPCRoom )
	{
		dwRand = m_RandomGrade.Random( m_dwPCRoomGradeTotal );

		DWORD dwCurValue = 0;
		int iSize = m_vPCRoomGradeRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			DWORD dwValue = m_vPCRoomGradeRateList[i].m_dwRate;
			if( COMPARE( dwRand, dwCurValue, dwCurValue+dwValue ) )
				return m_vPCRoomGradeRateList[i].m_iGradeNum;

			dwCurValue += dwValue;
		}
	}
	else 
	{
		dwRand = m_RandomGrade.Random( m_dwGradeTotal );

		DWORD dwCurValue = 0;
		int iSize = m_vNormalGradeRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			DWORD dwValue = m_vNormalGradeRateList[i].m_dwRate;
			if( COMPARE( dwRand, dwCurValue, dwCurValue+dwValue ) )
				return m_vNormalGradeRateList[i].m_iGradeNum;

			dwCurValue += dwValue;
		}
	}
	
	return -1;
}

DWORD FishingManager::GetCurFishingTime( int iType, bool bPCRoom )
{
	int iNewType = iType - 1;
	DWORD dwCurMin = 0;
	DWORD dwCurMax = 0;

	if( COMPARE( iNewType, 0, (int)m_vFishingTimeList.size() ) )
	{
		dwCurMin = m_vFishingTimeList[iNewType].m_dwMinTime;
		dwCurMax = m_vFishingTimeList[iNewType].m_dwMaxTime;

		if( bPCRoom )
		{
			dwCurMin = max( 0, (int)dwCurMin - m_iPCRoomFishTimeMinusMin );
			dwCurMax = max( 0, (int)dwCurMax - m_iPCRoomFishTimeMinusMax );
		}
	}

	DWORD dwCurTime = 0;
	DWORD dwGapTime = 0;
	if( dwCurMax > dwCurMin )
	{
		dwGapTime = dwCurMax - dwCurMin;
	}

	dwCurTime = dwCurMax;
	if( dwGapTime > 0 )
	{
		DWORD dwRandTime = m_RandomTime.Random( dwGapTime );
		dwCurTime = dwCurMin + dwRandTime;
	}

	return dwCurTime;
}

bool FishingManager::IsRoomAlarm( int iItemType )
{
	LOOP_GUARD();
	FishingItemInfoList::iterator iter = m_vFishingItemList.begin();
	while( iter != m_vFishingItemList.end() )
	{
		if( (*iter).m_iItemNum == iItemType )
			return (*iter).m_bRoomAlarm;

		++iter;
	}
	LOOP_GUARD_CLEAR();
	return false;
}

bool FishingManager::IsAllAlarm( int iItemType )
{
	LOOP_GUARD();
	FishingItemInfoList::iterator iter = m_vFishingItemList.begin();
	while( iter != m_vFishingItemList.end() )
	{
		if( (*iter).m_iItemNum == iItemType )
			return (*iter).m_bAllAlarm;

		++iter;
	}
	LOOP_GUARD_CLEAR();
	return false;
}

bool FishingManager::IsSellAlarm( int iItemType, int iGrade, bool bEvent, bool bPCRoom )
{
	bool bGradeAlarm = false;
	int iGradeCnt = m_vFishingItemGradeList.size();
	for( int i=0; i < iGradeCnt; ++i )
	{
		if( m_vFishingItemGradeList[i].m_iGradeNum == iGrade )
		{
			bGradeAlarm = m_vFishingItemGradeList[i].m_bAlarm;
			break;
		}
	}

	if( bEvent )
	{
		LOOP_GUARD();
		FishingItemRateList::iterator iter = m_vEventRateList.begin();
		while( iter != m_vEventRateList.end() )
		{
			if( (*iter).m_iItemNum == iItemType )
			{
				if( (*iter).m_bSellAlarm && bGradeAlarm )
					return true;

				return false;
			}

			++iter;
		}
		LOOP_GUARD_CLEAR();
	}
	else if( bPCRoom )
	{
		LOOP_GUARD();
		FishingItemRateList::iterator iter = m_vPCRoomRateList.begin();
		while( iter != m_vPCRoomRateList.end() )
		{
			if( (*iter).m_iItemNum == iItemType )
			{
				if( (*iter).m_bSellAlarm && bGradeAlarm )
					return true;

				return false;
			}

			++iter;
		}
		LOOP_GUARD_CLEAR();
	}
	else 
	{
		LOOP_GUARD();
		FishingItemRateList::iterator iter = m_vNormalRateList.begin();
		while( iter != m_vNormalRateList.end() )
		{
			if( (*iter).m_iItemNum == iItemType )
			{
				if( (*iter).m_bSellAlarm && bGradeAlarm )
					return true;

				return false;
			}

			++iter;
		}
		LOOP_GUARD_CLEAR();
	}
	return false;
}

bool FishingManager::IsSpecial( int iItemType, bool bGuildFishery )
{
	if( bGuildFishery )
	{
		LOOP_GUARD();
		for( int i = 0; i < (int)m_vGuildFisheryItemList.size(); i++ )
		{
			if( m_vGuildFisheryItemList[i].m_iItemNum == iItemType )
				return m_vGuildFisheryItemList[i].m_bSpecial;
		}
		LOOP_GUARD_CLEAR();
	}
	else
	{
		LOOP_GUARD();
		FishingItemInfoList::iterator iter = m_vFishingItemList.begin();
		while( iter != m_vFishingItemList.end() )
		{
			if( (*iter).m_iItemNum == iItemType )
				return (*iter).m_bSpecial;

			++iter;
		}
		LOOP_GUARD_CLEAR();
	}
	
	return false;
}

int FishingManager::GetEventFishingPresentNum( int iItemNum, bool bEvent, bool bPCRoom )
{
	if( bEvent )
	{
		int iSize = m_vEventRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			if( m_vEventRateList[i].m_iItemNum == iItemNum )
				return m_vEventRateList[i].m_iEventPresent;
		}
	}
	else if( bPCRoom )
	{
		int iSize = m_vPCRoomRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			if( m_vPCRoomRateList[i].m_iItemNum == iItemNum )
				return m_vPCRoomRateList[i].m_iEventPresent;
		}
	}
	else
	{
		int iSize = m_vNormalRateList.size();
		for( int i=0; i < iSize; ++i )
		{
			if( m_vNormalRateList[i].m_iItemNum == iItemNum )
				return m_vNormalRateList[i].m_iEventPresent;
		}
	}

	return -1;
}