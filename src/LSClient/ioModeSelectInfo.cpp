

#include "stdafx.h"

#include "ioModeSelectInfo.h"

#include "GUI/ModeMapSelectList.h"
#include "GUI/PartyMenuWnd.h"
#include "GUI/PlazaMainWnd.h"

template<> ioModeSelectInfo* Singleton< ioModeSelectInfo >::ms_Singleton = 0;

ioModeSelectInfo::ioModeSelectInfo()
{
}

ioModeSelectInfo::~ioModeSelectInfo()
{	
	ClearAllResource();
}

ioModeSelectInfo& ioModeSelectInfo::GetSingleton()
{
	return Singleton< ioModeSelectInfo >::GetSingleton();
}

void ioModeSelectInfo::ClearAllResource()
{
	{
		ModeAndMapUIResource::iterator iCreate;
		for( iCreate = m_ModeUIResource.begin() ; iCreate != m_ModeUIResource.end() ; ++iCreate )
		{
			SAFEDELETE( iCreate->second );
		}
		m_ModeUIResource.clear();

		for( iCreate = m_MapUIResource.begin() ; iCreate != m_MapUIResource.end() ; ++iCreate )
		{
			SAFEDELETE( iCreate->second );
		}
		m_MapUIResource.clear();

		for( iCreate = m_ListModeUIResource.begin() ; iCreate != m_ListModeUIResource.end() ; ++iCreate )
		{
			SAFEDELETE( iCreate->second );
		}
		m_ListModeUIResource.clear();

		for( iCreate = m_ListMapUIResource.begin() ; iCreate != m_ListMapUIResource.end() ; ++iCreate )
		{
			SAFEDELETE( iCreate->second );
		}
		m_ListMapUIResource.clear();
	}

	{
		OrderMarkUIResource::iterator iCreate2;
		for( iCreate2 = m_OrderMarkResource.begin() ; iCreate2 != m_OrderMarkResource.end() ; ++iCreate2 )
		{
			SAFEDELETE( iCreate2->second );
		}
		m_OrderMarkResource.clear();
	}

	vModeIcon::iterator iModeIconIter = m_ModeIconList.begin();
	for(;iModeIconIter != m_ModeIconList.end();iModeIconIter++)
	{
		ModeIcon *pItem = *iModeIconIter;
		if( !pItem ) continue;
		SAFEDELETE( pItem->m_pDefaultIcon );
		SAFEDELETE( pItem->m_pBlueTeamIcon );
		SAFEDELETE( pItem->m_pRedTeamIcon );
		SAFEDELETE( pItem );
	}
	m_ModeIconList.clear();

	vMapList::iterator iMapIter = m_MapList.begin();
	for(;iMapIter != m_MapList.end();iMapIter++)
	{
		MapList &rkItem = *iMapIter;
		rkItem.m_MapInfo.clear();
	}
	m_MapList.clear();
	m_QuickStartList.clear();
	m_PlazaModeList.clear();
	m_HeadquartersModeList.clear();
	m_HouseModeList.clear();
	m_BattleModeList.clear();
	m_BroadcastBattleModeList.clear();
	m_LadderTeamModeList.clear();
	m_BattleRoomListTitle.clear();
	m_QuickStartHelpMap.clear();
	m_szCustomModeTitle.Clear();
}

void ioModeSelectInfo::LoadModeSelectInfo()
{
	ClearAllResource();

	ioINILoader_e kLoader( "config/sp2_mode_select.ini" );

	// 모드 & 맵 리소스
	kLoader.SetTitle_e( "Resource" );
	int i = 0;
	int iMaxRes = kLoader.LoadInt_e( "max_mode", 0 );
	char szKey[MAX_PATH], szTitle[MAX_PATH], szBuf[MAX_PATH];
	for(i = 0;i < iMaxRes;i++)
	{
		sprintf_e( szKey, "mode_%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szTitle, MAX_PATH );
		sprintf_e( szKey, "mode_%d_image", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_ModeUIResource.insert( ModeAndMapUIResource::value_type( szTitle, pImg ) );		
	}	

	iMaxRes = kLoader.LoadInt_e( "max_map", 0 );
	for(i = 0;i < iMaxRes;i++)
	{
		sprintf_e( szKey, "map_%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szTitle, MAX_PATH );
		sprintf_e( szKey, "map_%d_image", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_MapUIResource.insert( ModeAndMapUIResource::value_type( szTitle, pImg ) );		
	}	

	// 빠른전투 모드 리스트
	kLoader.SetTitle_e( "QuickStartMenu" );
	int iMaxList = kLoader.LoadInt_e( "max_list", 0 );
	for(i = 0;i < iMaxList;i++)
	{
		sprintf_e( szKey, "mode_%d_active", i + 1 );
		if( !kLoader.LoadBool( szKey, true ) )
			continue;

		QuickStartList kList;
		sprintf_e( szKey, "mode_%d_index", i + 1 );
		kList.m_iIndex = kLoader.LoadInt( szKey, 0 );
		
		sprintf_e( szKey, "mode_%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szTitle, MAX_PATH );
		kList.m_Title = szTitle;

		sprintf_e( szKey, "mode_%d_limit_grade", i + 1 );
		kList.m_iLimitGrade = kLoader.LoadInt( szKey, 0 );
		
		sprintf_e( szKey, "mode_%d_use_coin_cnt", i + 1 );
		kList.m_iUseCoinCnt = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode_%d_help_index", i + 1 );
		kList.m_iHelpIndex = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode_%d_manual_index", i + 1 );
		kList.m_iManualIndex = kLoader.LoadInt( szKey, -1 );

		sprintf_e( szKey, "mode_%d_limit_max_user", i + 1 );
		kList.m_iLimitMaxUser = kLoader.LoadInt( szKey, MAX_PLAYER );

		sprintf_e( szKey, "mode_%d_def_min_user", i + 1 );
		kList.m_iDefMinUser = kLoader.LoadInt( szKey, 1 );

		sprintf_e( szKey, "mode_%d_def_max_user", i + 1 );
		kList.m_iDefMaxUser = kLoader.LoadInt( szKey, MAX_PLAYER );

		sprintf_e( szKey, "mode_%d_use_same_user", i + 1 );
		kList.m_bUseSameUser = kLoader.LoadBool( szKey, true );

		sprintf_e( szKey, "mode_%d_def_same_user", i + 1 );
		kList.m_bDefSameUser = kLoader.LoadBool( szKey, true );

		sprintf_e( szKey, "mode_%d_add_create_ui", i + 1 );
		kList.m_bAddCreateUI = kLoader.LoadBool( szKey, false );
		
		sprintf_e( szKey, "mode_%d_add_create_order", i + 1 );
		kList.m_iCreateModeOrder = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode_%d_add_create_mark", i + 1 );
		kList.m_iCreateMarkRes = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode_%d_add_quick_mark", i + 1 );
		kList.m_nQuickNewHot = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode_%d_sub_title", i + 1 );
		kLoader.LoadString( szKey, "", szTitle, MAX_PATH );
		kList.m_SubTitle = szTitle;

		m_QuickStartList.push_back( kList );
	}	
	std::sort( m_QuickStartList.begin(), m_QuickStartList.end(), QuickBattleInfoSort() );

	// 전투방 모드 타이틀
	m_BattleRoomListTitle.clear();
	kLoader.SetTitle_e( "BattleRoomListTitle" );
	iMaxList = kLoader.LoadInt_e( "max_list", 0 );
	for(i = 0;i < iMaxList;i++)
	{
		sprintf_e( szKey, "mode_%d_active", i + 1 );
		if( !kLoader.LoadBool( szKey, true ) )
			continue;

		BattleRoomListTitle kList;
		sprintf_e( szKey, "mode_%d_index", i + 1 );
		kList.m_iIndex = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode_%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szTitle, MAX_PATH );
		kList.m_Title = szTitle;

		m_BattleRoomListTitle.push_back( kList );
	}	

	// 커스텀 모드 타이틀(전투방 모드 타이틀과 같은 분류에...)
	kLoader.LoadString_e( "custom_mode_title", "", szTitle, MAX_PATH );
	m_szCustomModeTitle = szTitle;

	// 맵 리스트
	kLoader.SetTitle_e( "MapInfo" );
	int iMaxMap = kLoader.LoadInt_e( "max_list", 0 );
	for(i = 0;i < iMaxMap;i++)
	{
		MapList kMapList;
		sprintf_e( szTitle, "MapList%d", i + 1 );
		kLoader.SetTitle( szTitle );
		int iMaxList = kLoader.LoadInt_e( "max_map", 0 );
		for(int j = 0;j < iMaxList;j++)
		{
			sprintf_e( szKey, "map%d_active", j + 1 );
			if( !kLoader.LoadBool( szKey, true ) )
				continue;

			MapInfo kMapInfo;
			sprintf_e( szKey, "map%d_index", j + 1 );
			kMapInfo.m_MapIndex = kLoader.LoadInt( szKey, -1 );

			sprintf_e( szKey, "map%d_limit_grade", j + 1 );
			kMapInfo.m_GradeLimitCheck = kLoader.LoadInt( szKey, -1 );
			
			sprintf_e( szKey, "map%d_limit_max_player", j + 1 );
			kMapInfo.m_LimitMaxPlayer = kLoader.LoadInt( szKey, MAX_PLAYER );
			
			sprintf_e( szKey, "map%d_default_max_player", j + 1 );
			kMapInfo.m_DefaultMaxPlayer = kLoader.LoadInt( szKey, MAX_PLAYER / 2 );

			sprintf_e( szKey, "map%d_start_set_limit_player", j + 1 );
			kMapInfo.m_bStartSetLimitPlayer = kLoader.LoadBool( szKey, true );
			
			sprintf_e( szKey, "map%d_use_coin_cnt", j + 1 );
			kMapInfo.m_iUseCoinCnt = kLoader.LoadInt( szKey, 0 );
			
			sprintf_e( szKey, "map%d_order", j + 1 );
			kMapInfo.m_iMapOrder = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "map%d_mark_b", j + 1 );
			kMapInfo.m_iMarkResBig = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "map%d_mark_s", j + 1 );
			kMapInfo.m_iMarkResSmall = kLoader.LoadInt( szKey, 0 );
			
			sprintf_e( szKey, "map%d_title", j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kMapInfo.m_MapTitle = szBuf;
			
			kMapList.m_MapInfo.push_back( kMapInfo );
		}
		std::sort( kMapList.m_MapInfo.begin(), kMapList.m_MapInfo.end(), MapInfoSort() );
		m_MapList.push_back( kMapList );
	}

	// 광장 리스트
	kLoader.SetTitle_e( "PlazaMenu" );
	int iMaxMode = kLoader.LoadInt_e( "max_mode", 0 );
	for(i = 0;i < iMaxMode;i++)
	{
		PlazaModeList kModeList;
		sprintf_e( szKey, "mode%d_index", i + 1 );
		kModeList.m_ModeIndex = kLoader.LoadInt( szKey, -1 );
		sprintf_e( szKey, "mode%d_plaza_type", i + 1 );
		kModeList.m_PlazaType = kLoader.LoadInt( szKey, 1 );
		sprintf_e( szKey, "mode%d_map_list", i + 1 );
		kModeList.m_MapListIndex = kLoader.LoadInt( szKey, 1 );
		sprintf_e( szKey, "mode%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kModeList.m_ModeTitle = szBuf;

		m_PlazaModeList.push_back( kModeList );
	}

	// 훈련소 리스트
	kLoader.SetTitle_e( "HeadquartersMenu" );
	iMaxMode = kLoader.LoadInt_e( "max_mode", 0 );
	for(i = 0;i < iMaxMode;i++)
	{
		HeadquartersModeList kModeList;
		sprintf_e( szKey, "mode%d_index", i + 1 );
		kModeList.m_ModeIndex = kLoader.LoadInt( szKey, -1 );
		sprintf_e( szKey, "mode%d_map_list", i + 1 );
		kModeList.m_MapListIndex = kLoader.LoadInt( szKey, 1 );
		sprintf_e( szKey, "mode%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kModeList.m_ModeTitle = szBuf;
		m_HeadquartersModeList.push_back( kModeList );
	}

	//개인본부 리스트
	kLoader.SetTitle_e( "HouseMenu" );
	iMaxMode = kLoader.LoadInt_e( "max_mode", 0 );
	for(i = 0;i < iMaxMode;i++)
	{
		HouseModeList kModeList;
		sprintf_e( szKey, "mode%d_index", i + 1 );
		kModeList.m_ModeIndex = kLoader.LoadInt( szKey, -1 );
		sprintf_e( szKey, "mode%d_map_list", i + 1 );
		kModeList.m_MapListIndex = kLoader.LoadInt( szKey, 1 );
		sprintf_e( szKey, "mode%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kModeList.m_ModeTitle = szBuf;
		m_HouseModeList.push_back( kModeList );
	}

	// 전투방 리스트
	kLoader.SetTitle_e( "BattleMenu" );
	iMaxMode = kLoader.LoadInt_e( "max_mode", 0 );
	for(i = 0;i < iMaxMode;i++)
	{
		sprintf_e( szKey, "mode%d_active", i + 1 );
		if( !kLoader.LoadBool( szKey, true ) )
			continue;

		BattleModeList kModeList;
		sprintf_e( szKey, "mode%d_index", i + 1 );
		kModeList.m_ModeIndex = kLoader.LoadInt( szKey, -1 );

		sprintf_e( szKey, "mode%d_help_index", i + 1 );
		kModeList.m_ModeHelpIndex = kLoader.LoadInt( szKey, -1 );

		sprintf_e( szKey, "mode%d_manual_index", i + 1 );
		kModeList.m_ModeManualIndex = kLoader.LoadInt( szKey, -1 );

		sprintf_e( szKey, "mode%d_safety", i + 1 );
		kModeList.m_bSafetyMode = kLoader.LoadBool( szKey, false );

		sprintf_e( szKey, "mode%d_freeforall", i + 1 );
		kModeList.m_bFreeForAll = kLoader.LoadBool( szKey, false );

		sprintf_e( szKey, "mode%d_minuser", i + 1 );
		kModeList.m_StartMinUser = kLoader.LoadInt( szKey, 1 );

		sprintf_e( szKey, "mode%d_none_play_join", i + 1 );
		kModeList.m_bStartEnterX = kLoader.LoadBool( szKey, false ); 

		sprintf_e( szKey, "mode%d_coin_visible", i + 1 );
		kModeList.m_bCoinVisible = kLoader.LoadBool( szKey, false );

		sprintf_e( szKey, "mode%d_map_list", i + 1 );
		kModeList.m_MapListIndex = kLoader.LoadInt( szKey, 1 );

		sprintf_e( szKey, "mode%d_order", i + 1 );
		kModeList.m_iModeOrder = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode%d_mark_b", i + 1 );
		kModeList.m_iMarkResBig = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode%d_mark_s", i + 1 );
		kModeList.m_iMarkResSmall = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kModeList.m_ModeTitle = szBuf;
		sprintf_e( szKey, "mode%d_AIMode", i + 1 );
		kModeList.m_bAIMode = kLoader.LoadBool( szKey, false );

		sprintf_e( szKey, "mode%d_auto_start", i + 1 );
		kModeList.m_bAutoStart = kLoader.LoadBool( szKey, true );
		m_BattleModeList.push_back( kModeList );
	}
	std::sort( m_BattleModeList.begin(), m_BattleModeList.end(), BattleModeInfoSort() );

	// 방송용 전투방 리스트
	kLoader.SetTitle_e( "BroadcastBattleMenu" );
	iMaxMode = kLoader.LoadInt_e( "max_mode", 0 );
	for(i = 0;i < iMaxMode;i++)
	{
		BattleModeList kModeList;
		sprintf_e( szKey, "mode%d_index", i + 1 );
		kModeList.m_ModeIndex = kLoader.LoadInt( szKey, -1 );

		sprintf_e( szKey, "mode%d_help_index", i + 1 );
		kModeList.m_ModeHelpIndex = kLoader.LoadInt( szKey, -1 );

		sprintf_e( szKey, "mode%d_manual_index", i + 1 );
		kModeList.m_ModeManualIndex = kLoader.LoadInt( szKey, -1 );
		
		sprintf_e( szKey, "mode%d_safety", i + 1 );
		kModeList.m_bSafetyMode = kLoader.LoadBool( szKey, false );
		
		sprintf_e( szKey, "mode%d_freeforall", i + 1 );
		kModeList.m_bFreeForAll = kLoader.LoadBool( szKey, false );
		
		sprintf_e( szKey, "mode%d_minuser", i + 1 );
		kModeList.m_StartMinUser = kLoader.LoadInt( szKey, 1 );
		
		sprintf_e( szKey, "mode%d_none_play_join", i + 1 );
		kModeList.m_bStartEnterX = kLoader.LoadBool( szKey, false );
		
		sprintf_e( szKey, "mode%d_coin_visible", i + 1 );
		kModeList.m_bCoinVisible = kLoader.LoadBool( szKey, false );
		
		sprintf_e( szKey, "mode%d_map_list", i + 1 );
		kModeList.m_MapListIndex = kLoader.LoadInt( szKey, 1 );

		sprintf_e( szKey, "mode%d_order", i + 1 );
		kModeList.m_iModeOrder = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode%d_mark_b", i + 1 );
		kModeList.m_iMarkResBig = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode%d_mark_s", i + 1 );
		kModeList.m_iMarkResSmall = kLoader.LoadInt( szKey, 0 );
		
		sprintf_e( szKey, "mode%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kModeList.m_ModeTitle = szBuf;

		m_BroadcastBattleModeList.push_back( kModeList );
	}
	std::sort( m_BroadcastBattleModeList.begin(), m_BroadcastBattleModeList.end(), BattleModeInfoSort() );

	// 진영팀 리스트
	kLoader.SetTitle_e( "LadderTeamMenu" );
	iMaxMode = kLoader.LoadInt_e( "max_mode", 0 );
	for(i = 0;i < iMaxMode;i++)
	{
		sprintf_e( szKey, "mode%d_active", i + 1 );
		if( !kLoader.LoadBool( szKey, true ) )
			continue;

		LadderTeamModeList kModeList;
		sprintf_e( szKey, "mode%d_index", i + 1 );
		kModeList.m_ModeIndex = kLoader.LoadInt( szKey, -1 );

		sprintf_e( szKey, "mode%d_help_index", i + 1 );
		kModeList.m_ModeHelpIndex = kLoader.LoadInt( szKey, -1 );

		sprintf_e( szKey, "mode%d_manual_index", i + 1 );
		kModeList.m_ModeManualIndex = kLoader.LoadInt( szKey, -1 );
		
		sprintf_e( szKey, "mode%d_map_list", i + 1 );
		kModeList.m_MapListIndex = kLoader.LoadInt( szKey, 1 );
		
		sprintf_e( szKey, "mode%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kModeList.m_ModeTitle = szBuf;

		sprintf_e( szKey, "mode%d_hero_match", i + 1 );
		kModeList.m_bHeroMatch = kLoader.LoadBool( szKey, false );

		sprintf_e( szKey, "mode%d_minimum_player", i + 1 );
		kModeList.m_iMinimumPlayer = kLoader.LoadInt( szKey, 2 );

		sprintf_e( szKey, "mode%d_default_player", i + 1 );
		kModeList.m_iDefaultPlayer = kLoader.LoadInt( szKey, 4 );

		sprintf_e( szKey, "mode%d_order", i + 1 );
		kModeList.m_iModeOrder = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode%d_mark_b", i + 1 );
		kModeList.m_iMarkResBig = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mode%d_mark_s", i + 1 );
		kModeList.m_iMarkResSmall = kLoader.LoadInt( szKey, 0 );

		m_LadderTeamModeList.push_back( kModeList );
	}
	std::sort( m_LadderTeamModeList.begin(), m_LadderTeamModeList.end(), LadderTeamModeInfoSort() );

	// 모드 & 맵 리스트 리소스
	kLoader.SetTitle_e( "ListResource" );
	iMaxRes = kLoader.LoadInt_e( "max_mode", 0 );
	for(i = 0;i < iMaxRes;i++)
	{
		sprintf_e( szKey, "mode_%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szTitle, MAX_PATH );
		sprintf_e( szKey, "mode_%d_image", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_ListModeUIResource.insert( ModeAndMapUIResource::value_type( szTitle, pImg ) );		
	}	

	iMaxRes = kLoader.LoadInt_e( "max_map", 0 );
	for(i = 0;i < iMaxRes;i++)
	{
		sprintf_e( szKey, "map_%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szTitle, MAX_PATH );
		sprintf_e( szKey, "map_%d_image", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_ListMapUIResource.insert( ModeAndMapUIResource::value_type( szTitle, pImg ) );		
	}	

	// 모드 아이콘 리소스
	kLoader.SetTitle_e( "ModeIcon" );
	int iMaxModeIcon = kLoader.LoadInt_e( "max_mode", 0 );
	for(i = 0;i < iMaxModeIcon;i++)
	{
		sprintf_e( szTitle, "ModeIcon%d", i + 1 );
		kLoader.SetTitle( szTitle );

		//
		ModeType eModeType = (ModeType)kLoader.LoadInt_e( "mode_type", 0 );
		if( eModeType == MT_NONE ) continue;

		ModeIcon *pModeIcon = new ModeIcon;
		if( !pModeIcon ) continue;
		pModeIcon->m_eModeType = eModeType;
		pModeIcon->m_bUserMode = kLoader.LoadBool_e( "user_mode", false );
		
		kLoader.LoadString_e( "def_icon", "", szBuf, MAX_PATH );
		pModeIcon->m_pDefaultIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
		kLoader.LoadString_e( "blue_icon", "", szBuf, MAX_PATH );
		pModeIcon->m_pBlueTeamIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
		kLoader.LoadString_e( "red_icon", "", szBuf, MAX_PATH );
		pModeIcon->m_pRedTeamIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		m_ModeIconList.push_back( pModeIcon );
	}

	// 모드 & 맵 순서 마크
	kLoader.SetTitle_e( "OrderMark" );
	int iMaxMark = kLoader.LoadInt_e( "max_mark", 0 );
	for(i = 0;i < iMaxModeIcon;i++)
	{
		sprintf_e( szKey, "MarkNum%d", i + 1 );
		int iMarkNum = kLoader.LoadInt( szKey, 0 );
	
		sprintf_e( szKey, "MarkRes%d", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		if( !pImg ) continue;

		m_OrderMarkResource.insert( OrderMarkUIResource::value_type( iMarkNum, pImg ) );		
	}
	LoadQuickStartHelp();

	kLoader.SetTitle_e( "ModeType" );

	for( i = MT_SYMBOL; i < MAX_MODE_TYPE; i++ )
	{
		ModeTypeInfo Info;		
		Info.eModeType = i;
		sprintf_e( szKey, "mode%d_index", i );
		Info.iModeIdex = kLoader.LoadInt( szKey, 0 );

		m_ModeTypeInfoList.push_back( Info );
	}
}

void ioModeSelectInfo::LoadQuickStartHelp()
{
	ioINILoader_e kLoader( "config/sp2_quickstart_mode_help.ini" );
	kLoader.SetTitle_e( "info" );

	m_QuickStartHelpMap.clear();
	int iMaxHelp = kLoader.LoadInt_e( "max_help", 0 );
	for(int i = 0;i < iMaxHelp;i++)
	{
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		sprintf_e( szKey, "help%d", i + 1 );
		kLoader.SetTitle( szKey );

		QuickStartHelp kQuickHelp;
		kLoader.LoadString_e( "state", "", szBuf, MAX_PATH );
		kQuickHelp.m_szState = szBuf;

		int iMaxLine = kLoader.LoadInt_e( "max_line", 0 );
		for(int j = 0;j < iMaxLine;j++)
		{
			QuickStartHelpData kData;
			sprintf_e( szKey, "line_%d", j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kData.m_szHelp = szBuf;
			
			sprintf_e( szKey, "tooltip_pass_%d", j + 1 );
			kData.m_bModeTooltipHelpPass = kLoader.LoadBool( szKey, false );
			kQuickHelp.m_vHelpList.push_back( kData );
		}
		m_QuickStartHelpMap.insert( QuickStartHelpMap::value_type( i + 1, kQuickHelp ) );
	}
}

ioUIRenderImage *ioModeSelectInfo::GetModeUIResource( const ioHashString &szTitle )
{
	ModeAndMapUIResource::iterator iter = m_ModeUIResource.find( szTitle );

	if( iter != m_ModeUIResource.end() )
	{
		return iter->second;		
	}
	return NULL;
}

ioUIRenderImage *ioModeSelectInfo::GetMapUIResource( const ioHashString &szTitle )
{
	ModeAndMapUIResource::iterator iter = m_MapUIResource.find( szTitle );

	if( iter != m_MapUIResource.end() )
	{
		return iter->second;		
	}
	return NULL;
}

ioUIRenderImage *ioModeSelectInfo::GetListModeUIResource( const ioHashString &szTitle )
{
	ModeAndMapUIResource::iterator iter = m_ListModeUIResource.find( szTitle );

	if( iter != m_ListModeUIResource.end() )
	{
		return iter->second;		
	}
	return NULL;
}

ioUIRenderImage *ioModeSelectInfo::GetListMapUIResource( const ioHashString &szTitle )
{
	ModeAndMapUIResource::iterator iter = m_ListMapUIResource.find( szTitle );

	if( iter != m_ListMapUIResource.end() )
	{
		return iter->second;		
	}
	return NULL;
}

ioUIRenderImage *ioModeSelectInfo::GetOrderMarkUIResource( DWORD dwMarkNum )
{
	OrderMarkUIResource::iterator iter = m_OrderMarkResource.find( dwMarkNum );

	if( iter != m_OrderMarkResource.end() )
	{
		return iter->second;		
	}
	return NULL;
}

int ioModeSelectInfo::GetQuickStartModeIndexCheck( int iModeIndex )
{
	// 보스를 일반으로 변경
	switch( iModeIndex )
	{
	case BMT_CATCH_BOSS:
		iModeIndex = BMT_CATCH;
		break;
	case BMT_CATCH_RUNNINGMAN_BOSS:
		iModeIndex = BMT_CATCH_RUNNINGMAN;
		break;
	case BMT_STONE_BOSS:
		iModeIndex = BMT_STONE;
		break;
	case BMT_KING_BOSS:
		iModeIndex = BMT_KING;
		break;
	case BMT_TEAM_SURVIVAL_BOSS:
		iModeIndex = BMT_TEAM_SURVIVAL;
		break;
	case BMT_TEAM_SURVIVAL_FIRST_BOSS:
		iModeIndex = BMT_TEAM_SURVIVAL_FIRST;
		break;
	case BMT_RANDOM_BOSS:
		iModeIndex = BMT_RANDOM;
		break;
	case BMT_FOOTBALL_BOSS:
		iModeIndex = BMT_FOOTBALL;
		break;
	case BMT_DOUBLE_CROWN_BOSS:
		iModeIndex = MT_DOUBLE_CROWN;
		break;
	}
	return iModeIndex;
}

ioHashString ioModeSelectInfo::GetSelectQuickStartModeTitle( int iModeIndex )
{	
	iModeIndex = GetQuickStartModeIndexCheck( iModeIndex );

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return kList.m_Title;
		}
	}
	return STR(1);
}

ioHashString ioModeSelectInfo::GetSelectQuickStartModeSubTitle( int iModeIndex )
{
	iModeIndex = GetQuickStartModeIndexCheck( iModeIndex );

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return kList.m_SubTitle;
		}
	}
	return STR(1);
}

int ioModeSelectInfo::GetSelectQuickStartModeLimitGrade( int iModeIndex )
{
	iModeIndex = GetQuickStartModeIndexCheck( iModeIndex );

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return kList.m_iLimitGrade;
		}
	}
	return 0;
}

int ioModeSelectInfo::GetSelectQuickStartModeUseCoinCnt( int iModeIndex )
{
	iModeIndex = GetQuickStartModeIndexCheck( iModeIndex );

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return kList.m_iUseCoinCnt;
		}
	}
	return 0;
}

void ioModeSelectInfo::GetQuickModeList( ioWnd *pParseWnd )
{
	BattleQuickStartOption *pModeList = dynamic_cast<BattleQuickStartOption *>(pParseWnd);
	if( !pModeList )
		return;

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( g_MyInfo.GetGradeLevel() < kList.m_iLimitGrade ) continue;

		if( kList.m_iIndex == BMT_TEAM_SURVIVAL_FIRST )
		{
			if( !g_MyInfo.IsSafetyLevel() ) continue;
		}

		pModeList->AddList( kList.m_iIndex, kList.m_Title, kList.m_iHelpIndex, kList.m_iLimitMaxUser, 
							kList.m_iDefMinUser, kList.m_iDefMaxUser, kList.m_bUseSameUser,
							GetOrderMarkUIResource( kList.m_nQuickNewHot ) );
	}
}

int ioModeSelectInfo::GetSelectQuickStartModeDefMaxPlayer( int iModeIndex )
{
	iModeIndex = GetQuickStartModeIndexCheck( iModeIndex );

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return kList.m_iDefMaxUser;
		}
	}
	return MAX_PLAYER;
}

int ioModeSelectInfo::GetSelectQuickStartModeDefMinPlayer( int iModeIndex )
{
	iModeIndex = GetQuickStartModeIndexCheck( iModeIndex );

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return kList.m_iDefMinUser;
		}
	}
	return 0;
}

bool ioModeSelectInfo::GetSelectQuickStartModeDefSameUser( int iModeIndex )
{
	iModeIndex = GetQuickStartModeIndexCheck( iModeIndex );

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return kList.m_bDefSameUser;
		}
	}
	return false;
}

bool ioModeSelectInfo::IsNoneQuickStartMode( int iModeIndex )
{
	iModeIndex = GetQuickStartModeIndexCheck( iModeIndex );

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return false;
		}
	}
	return true;
}

int ioModeSelectInfo::GetQuickStartIndexToHelpIndex( int iModeIndex )
{
	iModeIndex = GetQuickStartModeIndexCheck( iModeIndex );

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return kList.m_iHelpIndex;
		}
	}
	return 0;
}

int ioModeSelectInfo::GetQuickStartIndexToManualIndex( int iModeIndex )
{
	iModeIndex = GetQuickStartModeIndexCheck( iModeIndex );

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return kList.m_iManualIndex;
		}
	}
	return -1;
}

ioHashString ioModeSelectInfo::GetQuickStartHelpState( int iHelpIndex )
{
	QuickStartHelpMap::iterator iter = m_QuickStartHelpMap.find( iHelpIndex );
	if( iter != m_QuickStartHelpMap.end() )
	{
		QuickStartHelp &rkHelp = iter->second;
		return rkHelp.m_szState;
	}
	return "";
}

ioHashString ioModeSelectInfo::GetQuickStartHelpMent( int iHelpIndex, int iLine )
{
	QuickStartHelpMap::iterator iter = m_QuickStartHelpMap.find( iHelpIndex );
	if( iter != m_QuickStartHelpMap.end() )
	{
		QuickStartHelp &rkHelp = iter->second;
		if( !COMPARE( iLine, 0, (int)rkHelp.m_vHelpList.size() ) )
			return "";
		return rkHelp.m_vHelpList[iLine].m_szHelp;
	}
	return "";
}

int ioModeSelectInfo::GetQuickStartMaxHelpMent( int iHelpIndex )
{
	QuickStartHelpMap::iterator iter = m_QuickStartHelpMap.find( iHelpIndex );
	if( iter != m_QuickStartHelpMap.end() )
	{
		QuickStartHelp &rkHelp = iter->second;
		return rkHelp.m_vHelpList.size();
	}
	return 0;
}

int ioModeSelectInfo::GetQuickStartMaxPassHelpMent( int iHelpIndex )
{
	QuickStartHelpMap::iterator iter = m_QuickStartHelpMap.find( iHelpIndex );
	if( iter != m_QuickStartHelpMap.end() )
	{
		QuickStartHelp &rkHelp = iter->second;
		int iPassCount = 0;
		for(int i = 0;i < (int)rkHelp.m_vHelpList.size();i++)
		{
			QuickStartHelpData &rkData = rkHelp.m_vHelpList[i];
			if( rkData.m_bModeTooltipHelpPass )
				iPassCount++;
		}
		return iPassCount;
	}
	return 0;
}

QuickStartHelp &ioModeSelectInfo::GetQuickStartHelpData( int iHelpIndex )
{
	QuickStartHelpMap::iterator iter = m_QuickStartHelpMap.find( iHelpIndex );
	if( iter != m_QuickStartHelpMap.end() )
	{
		return iter->second;
	}
	
	static QuickStartHelp kReturn;
	return kReturn;
}

bool ioModeSelectInfo::IsQuickStartFreeForAllMode( int iModeIndex )
{
	switch( iModeIndex )
	{
	case BMT_ALL_MODE:
	case BMT_RANDOM:
	case BMT_CATCH:
	case BMT_CATCH_BOSS:
	case BMT_CATCH_RUNNINGMAN:
	case BMT_CATCH_RUNNINGMAN_BOSS:
	case BMT_STONE:
	case BMT_KING:
	case BMT_TEAM_SURVIVAL:
	case BMT_TEAM_SURVIVAL_FIRST:
	case BMT_STONE_BOSS:
	case BMT_KING_BOSS:
	case BMT_TEAM_SURVIVAL_BOSS:
	case BMT_TEAM_SURVIVAL_FIRST_BOSS:
	case BMT_RANDOM_BOSS:
	case BMT_USER_CUSTOM:
	case BMT_FOOTBALL:
	case BMT_FOOTBALL_BOSS:
	case BMT_DOUBLE_CROWN:
	case BMT_DOUBLE_CROWN_BOSS:
	case BMT_UNDERWEAR:
	case BMT_CBT:
	case BMT_FARMING:
		return false;
	}
	return true;
}

int ioModeSelectInfo::CheckPlazaModeIndex( int iPlazaType )
{
	int iSize = m_PlazaModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		PlazaModeList &kList = m_PlazaModeList[i];
		if( kList.m_PlazaType == iPlazaType )
			return iPlazaType;
	}
	return -1;
}

int ioModeSelectInfo::CheckPlazaMapIndex( int iPlazaType, int iMapIndex )
{
	int iMapListIndex = GetPlazaMapIndex( iPlazaType ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return -1;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return iMapIndex;
	}
	return -1;
}

int ioModeSelectInfo::GetPlazaMapIndex( int iPlazaType )
{
	int iSize = m_PlazaModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		PlazaModeList &kList = m_PlazaModeList[i];
		if( kList.m_PlazaType == iPlazaType )
			return kList.m_MapListIndex;
	}
	return -1;
}

ioHashString ioModeSelectInfo::GetPlazaModeTitle( int iPlazaType )
{
	int iSize = m_PlazaModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		PlazaModeList &kList = m_PlazaModeList[i];
		if( kList.m_PlazaType == iPlazaType )
			return kList.m_ModeTitle;
	}

	return STR(1);
}

ioHashString ioModeSelectInfo::GetPlazaMapTitle( int iPlazaType, int iMapIndex )
{
	int iMapListIndex = GetPlazaMapIndex( iPlazaType ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return STR(1);

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return kList.m_MapTitle;
	}
	return STR(2);
}

void ioModeSelectInfo::GetPlazaModeUIList( ioWnd *pParseWnd )
{
	PlazaModeNMapListWnd *pModeList = dynamic_cast<PlazaModeNMapListWnd*>(pParseWnd);
	if( !pModeList )
		return;
	
	int iSize = m_PlazaModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		PlazaModeList &kList = m_PlazaModeList[i];
		pModeList->AddList( kList.m_PlazaType, kList.m_ModeTitle, NULL );
	}
}

void ioModeSelectInfo::GetPlazaMapUIList( ioWnd *pParseWnd, int iPlazaType )
{
	PlazaModeNMapListWnd *pMapList = dynamic_cast<PlazaModeNMapListWnd*>(pParseWnd);	
	int iMapIndex = GetPlazaMapIndex( iPlazaType ) - 1;
	if( !COMPARE( iMapIndex, 0, (int)m_MapList.size() ) ) return;

	int iMapSize = m_MapList[iMapIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapIndex].m_MapInfo[i];
		pMapList->AddList( kList.m_MapIndex, kList.m_MapTitle, GetOrderMarkUIResource( kList.m_iMarkResBig ) );
	}
}

int ioModeSelectInfo::GetHeadquartersModeIndex()
{
	int iSize = m_HeadquartersModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		HeadquartersModeList &kList = m_HeadquartersModeList[i];

		return kList.m_ModeIndex;
	}
	return -1;
}

int ioModeSelectInfo::GetHeadquartersMapIndex( int iModeIndex )
{
	int iSize = m_HeadquartersModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		HeadquartersModeList &kList = m_HeadquartersModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_MapListIndex;
	}
	return -1;
}

int ioModeSelectInfo::CheckHeadquartersMapIndex( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetHeadquartersMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return -1;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return iMapIndex;
	}
	return -1;
}

int ioModeSelectInfo::GetHouseModeIndex()
{
	int iSize = m_HouseModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		HouseModeList &kList = m_HouseModeList[i];

		return kList.m_ModeIndex;
	}
	return -1;
}

int ioModeSelectInfo::GetHouseMapIndex( int iModeIndex )
{
	int iSize = m_HouseModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		HouseModeList &kList = m_HouseModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_MapListIndex;
	}
	return -1;
}

int ioModeSelectInfo::CheckHouseMapIndex( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetHouseMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return -1;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return iMapIndex;
	}
	return -1;
}

void ioModeSelectInfo::GetBattleCreateModeList( ioWnd *pParseWnd )
{
	CreateModeSelectList *pModeList = dynamic_cast<CreateModeSelectList*>(pParseWnd);
	if( !pModeList )
		return;

	int iSize = m_QuickStartList.size();
	for(int i = 0;i < iSize;i++)
	{
		QuickStartList &kList = m_QuickStartList[i];
		if( !kList.m_bAddCreateUI ) continue;
		if( g_MyInfo.GetGradeLevel() < kList.m_iLimitGrade ) continue;

		if( kList.m_iIndex == BMT_TEAM_SURVIVAL_FIRST )
		{
			if( !g_MyInfo.IsSafetyLevel() ) continue;
		}		
		pModeList->AddList( kList.m_iIndex, kList.m_Title, kList.m_iHelpIndex, kList.m_iManualIndex, GetOrderMarkUIResource( kList.m_iCreateMarkRes ) );
	}
}

int ioModeSelectInfo::CheckBattleModeIndex( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return iModeIndex;
	}

	// 방송용 전투방 리스트
	iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BroadcastBattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return iModeIndex;
	}
	return -1;
}

int ioModeSelectInfo::CheckBattleMapIndex( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return -1;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return iMapIndex;
	}
	return -1;
}

int ioModeSelectInfo::CheckBattleFirstMapIndex( int iModeIndex )
{
	int iMapListIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return -1;
	if( m_MapList[iMapListIndex].m_MapInfo.empty() ) return -1;

	MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[0];
	return kList.m_MapIndex;
}

int ioModeSelectInfo::GetBattleMapIndex( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_MapListIndex;
	}

	// 방송용 전투방 리스트
	iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BroadcastBattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_MapListIndex;
	}
	return -1;
}

int ioModeSelectInfo::GetBattleModeHelpIndex( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_ModeHelpIndex;
	}

	// 방송용 전투방 헬프
	iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BroadcastBattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_ModeHelpIndex;
	}
	return -1;
}

int ioModeSelectInfo::GetBattleModeManualIndex( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_ModeManualIndex;
	}

	// 방송용 전투방 헬프
	iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BroadcastBattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_ModeManualIndex;
	}
	return -1;
}

ioHashString ioModeSelectInfo::GetBattleModeTitle( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_ModeTitle;
	}

	// 방송용 전투방 리스트
	iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BroadcastBattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_ModeTitle;
	}
	return STR(1);
}

ioHashString ioModeSelectInfo::GetBattleMapTitle( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return STR(1);

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return kList.m_MapTitle;
	}
	return STR(2);
}

ioUIRenderImage *ioModeSelectInfo::GetBattleModeOrderMarkSmall( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return GetOrderMarkUIResource( kList.m_iMarkResSmall );
	}

	// 방송용 전투방 리스트
	iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BroadcastBattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return GetOrderMarkUIResource( kList.m_iMarkResSmall );
	}
	return NULL;
}

ioUIRenderImage *ioModeSelectInfo::GetBattleMapOrderMarkSmall( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return NULL;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return GetOrderMarkUIResource( kList.m_iMarkResSmall );
	}
	return NULL;
}

int ioModeSelectInfo::GetBattleMapLimitGrade( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return -1;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return kList.m_GradeLimitCheck;
	}
	return -1;
}

int ioModeSelectInfo::GetBattleMapLimitMaxPlayer( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return MAX_PLAYER;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return kList.m_LimitMaxPlayer;
	}
	return MAX_PLAYER;
}

bool ioModeSelectInfo::IsBattleStartSetMapLimitMaxPlayer( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return true;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return kList.m_bStartSetLimitPlayer;
	}
	return true;
}

int ioModeSelectInfo::GetBattleMapDefaultMaxPlayer( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return MAX_PLAYER / 2;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return kList.m_DefaultMaxPlayer;
	}
	return MAX_PLAYER / 2;
}

int ioModeSelectInfo::GetBattleMapCoinUseCnt( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return 0;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return kList.m_iUseCoinCnt;
	}
	return 0;
}

void ioModeSelectInfo::GetBattleModeUIList( ioWnd *pParseWnd )
{
	ModeMapSelectList *pModeList = dynamic_cast<ModeMapSelectList*>(pParseWnd);
	if( !pModeList )
		return;

	int iModeCount = 0;
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kMode = m_BattleModeList[i];
		if( kMode.m_bSafetyMode && !g_BattleRoomMgr.IsAllUserSafetyLevel() ) continue;

        pModeList->AddList( kMode.m_ModeIndex, kMode.m_bSafetyMode, kMode.m_ModeTitle, kMode.m_ModeHelpIndex, kMode.m_ModeManualIndex,
							GetOrderMarkUIResource( kMode.m_iMarkResBig ) );
		iModeCount++;
	}

	// 한개밖에 없으면 리스트에 표시하지 않는다.
	if( iModeCount <= 1 )
	{
		pModeList->HideWnd();
	}
}

void ioModeSelectInfo::GetBattleMapUIList( ioWnd *pParseWnd, int iModeIndex )
{
	ModeMapSelectList *pMapList = dynamic_cast<ModeMapSelectList*>(pParseWnd);

	int iMapCount = 0;
	int iMapIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapIndex, 0, (int)m_MapList.size() ) ) return;

	int iMapSize = m_MapList[iMapIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapIndex].m_MapInfo[i];
		pMapList->AddList( kList.m_MapIndex, false, kList.m_MapTitle, -1, -1, GetOrderMarkUIResource( kList.m_iMarkResBig ) );
		iMapCount++;
	}

	// 한개밖에 없으면 리스트에 표시하지 않는다.
	if( iMapCount <= 1 )
	{
		pMapList->HideWnd();
	}
}

bool ioModeSelectInfo::IsFreeForAllMode( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_bFreeForAll;
	}
	iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BroadcastBattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_bFreeForAll;
	}
	return false;
}

bool ioModeSelectInfo::IsStartEnterX( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_bStartEnterX;
	}
	iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BroadcastBattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_bStartEnterX;
	}
	return false;
}

bool ioModeSelectInfo::IsCoinVisible( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_bCoinVisible;
	}
	iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BroadcastBattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_bCoinVisible;
	}
	return false;
}

int ioModeSelectInfo::GetBattleStartMinUser( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_StartMinUser;
	}
	iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kList = m_BroadcastBattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_StartMinUser;
	}
	return 0;
}

ioHashString ioModeSelectInfo::GetBattleRoomListTitle( int iModeIndex )
{
	int i = 0;
	int iSize = m_BattleRoomListTitle.size();
	for(i = 0;i < iSize;i++)
	{
		BattleRoomListTitle &kList = m_BattleRoomListTitle[i];
		if( kList.m_iIndex == iModeIndex )
		{
			return kList.m_Title;
		}
	}
	return STR(1);
}

void ioModeSelectInfo::GetBroadcastBattleModeUIList( ioWnd *pParseWnd )
{
	ModeMapSelectList *pModeList = dynamic_cast<ModeMapSelectList*>(pParseWnd);
	if( !pModeList )
		return;

	int iModeCount = 0;
	int i = 0;
	int iSize = m_BroadcastBattleModeList.size();
	for(i = 0;i < iSize;i++)
	{
		BattleModeList &kMode = m_BroadcastBattleModeList[i];
		if( kMode.m_bSafetyMode && !g_BattleRoomMgr.IsAllUserSafetyLevel() ) continue;

		pModeList->AddList( kMode.m_ModeIndex, kMode.m_bSafetyMode, kMode.m_ModeTitle, kMode.m_ModeHelpIndex, kMode.m_ModeManualIndex,
							GetOrderMarkUIResource( kMode.m_iMarkResBig ) );
		iModeCount++;
	}

	// 한개밖에 없으면 리스트에 표시하지 않는다.
	if( iModeCount <= 1 )
	{
		pModeList->HideWnd();
	}
}

int ioModeSelectInfo::GetBattleModeIndex( int eModeType )
{
	for( int i = 0; i < (int)m_ModeTypeInfoList.size(); ++i )
	{
		const ModeTypeInfo& rkInfo = m_ModeTypeInfoList[i];
		if( rkInfo.eModeType == eModeType )
			return rkInfo.iModeIdex;
	}

	return -1;
}

int ioModeSelectInfo::GetBattleMode( int nIndex )
{
	for( int i = 0; i < (int)m_ModeTypeInfoList.size(); ++i )
	{
		const ModeTypeInfo& rkInfo = m_ModeTypeInfoList[i];
		if( rkInfo.iModeIdex == nIndex )
			return rkInfo.eModeType;
	}

	return MT_NONE;
}

void ioModeSelectInfo::GetBattleModeStringList( StringScrollListWnd *pParseWnd, int iModeIndex, DWORD WndID )
{
	if( pParseWnd == NULL )
		return;

	int iMapIndex = GetBattleMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapIndex, 0, (int)m_MapList.size() ) )
		return;

	int iMapSize = m_MapList[iMapIndex].m_MapInfo.size();
	for(int i = 0; i < iMapSize; i++ )
	{
		MapInfo &kList = m_MapList[iMapIndex].m_MapInfo[i];	
		pParseWnd->AddList( WndID, kList.m_MapIndex, kList.m_MapTitle, NULL );
	}
}

void ioModeSelectInfo::GetLadderCreateModeList( ioWnd *pParseWnd )
{
	CreateModeSelectList *pModeList = dynamic_cast<CreateModeSelectList*>(pParseWnd);
	if( !pModeList )
		return;

	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_bHeroMatch ) continue;

		pModeList->AddList( kList.m_ModeIndex, kList.m_ModeTitle, kList.m_ModeHelpIndex, kList.m_ModeManualIndex,
							GetOrderMarkUIResource( kList.m_iMarkResBig ) );
	}
}

int ioModeSelectInfo::CheckLadderTeamModeIndex( int iModeIndex )
{
	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return iModeIndex;
	}
	return -1;
}

int ioModeSelectInfo::CheckLadderTeamMapIndex( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetLadderTeamMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return -1;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return iMapIndex;
	}
	return -1;
}

int ioModeSelectInfo::GetLadderTeamMapIndex( int iModeIndex )
{
	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_MapListIndex;
	}
	return -1;
}

int ioModeSelectInfo::GetLadderTeamModeHelpIndex( int iModeIndex )
{
	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_ModeHelpIndex;
	}
	return -1;
}

int ioModeSelectInfo::GetLadderTeamModeManualIndex( int iModeIndex )
{
	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_ModeManualIndex;
	}
	return -1;
}

int ioModeSelectInfo::GetLadderTeamMinimumPlayer( int iModeIndex )
{
	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_iMinimumPlayer;
	}
	return 2;
}

int ioModeSelectInfo::GetLadderTeamDefaultPlayer( int iModeIndex )
{
	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_iDefaultPlayer;
	}
	return 4;
}

int ioModeSelectInfo::GetLadderTeamHeroMatchIndex()
{
	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_bHeroMatch )
			return kList.m_ModeIndex;
	}

	return -1;
}

bool ioModeSelectInfo::IsLadderTeamHeroMatchMode( int iModeIndex )
{
	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_bHeroMatch;
	}
	return false;
}

ioHashString ioModeSelectInfo::GetLadderTeamModeTitle( int iModeIndex )
{
	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_ModeTitle;
	}
	return STR(1);
}

ioHashString ioModeSelectInfo::GetLadderTeamMapTitle( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetLadderTeamMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return STR(1);

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return kList.m_MapTitle;
	}
	return STR(2);
}

ioUIRenderImage *ioModeSelectInfo::GetLadderTeamModeOrderMarkSmall( int iModeIndex )
{
	int iSize = m_LadderTeamModeList.size();
	for(int i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kList = m_LadderTeamModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return GetOrderMarkUIResource( kList.m_iMarkResSmall );
	}
	return NULL;
}

ioUIRenderImage *ioModeSelectInfo::GetLadderTeamMapOrderMarkSmall( int iModeIndex, int iMapIndex )
{
	int iMapListIndex = GetLadderTeamMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapListIndex, 0, (int)m_MapList.size() ) ) return NULL;

	int iMapSize = m_MapList[iMapListIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapListIndex].m_MapInfo[i];
		if( kList.m_MapIndex == iMapIndex )
			return GetOrderMarkUIResource( kList.m_iMarkResSmall );
	}
	return NULL;
}

void ioModeSelectInfo::GetLadderTeamModeUIList( ioWnd *pParseWnd, bool bHeroMatch )
{
	ModeMapSelectList *pModeList = dynamic_cast<ModeMapSelectList*>(pParseWnd);
	if( !pModeList )
		return;

	int i = 0;
	int iSize = m_LadderTeamModeList.size();
	for(i = 0;i < iSize;i++)
	{
		LadderTeamModeList &kMode = m_LadderTeamModeList[i];
		if( kMode.m_bHeroMatch != bHeroMatch ) continue;
		
		pModeList->AddList( kMode.m_ModeIndex, false, kMode.m_ModeTitle, kMode.m_ModeHelpIndex, kMode.m_ModeManualIndex,
							GetOrderMarkUIResource( kMode.m_iMarkResBig ) );
	}
}

void ioModeSelectInfo::GetLadderTeamMapUIList( ioWnd *pParseWnd, int iModeIndex )
{
	ModeMapSelectList *pMapList = dynamic_cast<ModeMapSelectList*>(pParseWnd);

	int iMapIndex = GetLadderTeamMapIndex( iModeIndex ) - 1;
	if( !COMPARE( iMapIndex, 0, (int)m_MapList.size() ) ) return;

	int iMapSize = m_MapList[iMapIndex].m_MapInfo.size();
	for(int i = 0;i < iMapSize;i++)
	{
		MapInfo &kList = m_MapList[iMapIndex].m_MapInfo[i];
		pMapList->AddList( kList.m_MapIndex, false, kList.m_MapTitle, -1, -1, GetOrderMarkUIResource( kList.m_iMarkResBig ) );
	}
}

ioUIRenderImage *ioModeSelectInfo::GetModeIcon( ModeType eModeType, TeamType eTeamType, bool bUserMode )
{
	if( eModeType == MT_UNDERWEAR || eModeType == MT_CBT || eModeType == MT_FARMING )
		bUserMode = false;

	vModeIcon::iterator iModeIconIter = m_ModeIconList.begin();
	for(;iModeIconIter != m_ModeIconList.end();iModeIconIter++)
	{
		ModeIcon *pItem = *iModeIconIter;
		if( !pItem ) continue;
		if( pItem->m_eModeType != eModeType ) continue;
		if( pItem->m_bUserMode != bUserMode ) continue;

		if( eTeamType == TEAM_BLUE && pItem->m_pBlueTeamIcon )
			return pItem->m_pBlueTeamIcon;
		else if( eTeamType == TEAM_RED && pItem->m_pRedTeamIcon )
			return pItem->m_pRedTeamIcon;
		else 
			return pItem->m_pDefaultIcon;
	}

	return NULL;
}
bool ioModeSelectInfo::IsAIMode( int iModeIndex )
{
	int iSize = m_BattleModeList.size();
	for(int i = 0; i < iSize; ++i )
	{
		BattleModeList& kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_bAIMode;
	}

	return false;
}

bool ioModeSelectInfo::IsAutoStart( int iModeIndex )
{
	int iSize = m_BattleModeList.size();
	for(int i = 0; i < iSize; ++i )
	{
		BattleModeList& kList = m_BattleModeList[i];
		if( kList.m_ModeIndex == iModeIndex )
			return kList.m_bAutoStart;
	}

	return false;
}

void ioModeSelectInfo::GetBattleAIUIList( ioWnd *pParseWnd )
{
	ModeMapSelectList *pMapList = dynamic_cast<ModeMapSelectList*>(pParseWnd);

	MapInfo kList;
	kList.m_MapTitle = "쉬움";
	kList.m_MapIndex = 0;
	pMapList->AddList( kList.m_MapIndex, false, kList.m_MapTitle, -1, -1, NULL );

	kList.m_MapTitle = "보통";
	kList.m_MapIndex = 1;
	pMapList->AddList( kList.m_MapIndex, false, kList.m_MapTitle, -1, -1, NULL );	
}