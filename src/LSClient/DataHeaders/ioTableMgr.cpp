#include <stdafx.h>

#include "ioTableMgr.h"
#include "ioExcelReader.h"
//#include "../IoMapManager.h"
//#include "../ioShopItemManager.h"
ioTableDataMgr* ioTableDataMgr::ms_Singleton = NULL;

ioTableDataMgr::ioTableDataMgr() : m_bReadExcel(false)
{
	for( int i = 0; i < DATA_TABLE_MAX; i++ )
		m_pDatas[i] = NULL;

    char key[MAX_PATH];

	// 헤더에 선언된 이넘값과 1:1 매칭이 되어야 함	
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_quest_info"				); m_eSheetName[QUEST_INFO_TABLE         ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_event_info"				); m_eSheetName[EVENT_INFO_TABLE         ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_SH_Phase_Data"			); m_eSheetName[SH_PHASE_DATA_TABLE      ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_SH_Phase_Reward"		); m_eSheetName[SH_PHASE_REWARD_TABLE    ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_SH_Ranking_Point"		); m_eSheetName[SH_RANKING_POINT_TABLE   ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_SH_Ranking_Reward"		); m_eSheetName[SH_RANKING_REWARD_TABLE  ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_SH_NPC_Info"			); m_eSheetName[SH_NPC_INFO_TABLE        ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_PieceInfo"				); m_eSheetName[PIECE_INFO_TABLE         ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_ComposeRewardInfo"		); m_eSheetName[COMPOSE_REWARD_INFO_TABLE] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_heroitem_info"			); m_eSheetName[HERO_ITEM_INFO_TABLE     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_etcitem_info"			); m_eSheetName[ETC_ITEM_INFO_TABLE      ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_decoitem_info"			); m_eSheetName[DECO_ITEM_INFO_TABLE     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_motionitem_info"		); m_eSheetName[MOTION_ITEM_INFO_TABLE   ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_motionoption_info"		); m_eSheetName[MOTION_OPTION_INFO_TABLE ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_MsgInfo"				); m_eSheetName[MSG_INFO_TABLE           ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_PresentReward_info"		); m_eSheetName[PRESENT_REWARD_INFO_TABLE] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_timebox_info"			); m_eSheetName[TIMEBOX_INFO_TABLE       ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_RankingLevel_info"		); m_eSheetName[RANKING_LEVEL_TABLE      ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Ranking_BasicValue"		); m_eSheetName[RANKING_BASICVALUE_TABLE ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Ranking_Reward"			); m_eSheetName[RANKING_REWARD_TABLE     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_MapGroup"				); m_eSheetName[MAP_GROUP_TABLE          ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_MapInfo"				); m_eSheetName[MAP_INFO_TABLE           ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_MapSet"					); m_eSheetName[MAP_SET_TABLE            ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Fish_Info"				); m_eSheetName[FISH_INFO_TABLE          ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Fish_Grade"				); m_eSheetName[FISH_GRADE_TABLE         ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_RandomboxInfo"			); m_eSheetName[RANDOMBOX_INFO_TABLE     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Reinforce_info"			); m_eSheetName[REINFORCE_INFO_TABLE     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_extraitem_info"			); m_eSheetName[EXTRA_ITEM_INFO_TABLE    ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_GeneralShop_info"		); m_eSheetName[GENERALSHOP_INFO_TABLE   ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_VipShop_info"			); m_eSheetName[VIPSHOP_INFO_TABLE       ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_ShopGoods_info"			); m_eSheetName[SHOPGOODS_INFO_TABLE     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_guild_grade_info"		); m_eSheetName[GUILD_GRADE_INFO_TABLE   ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_guild_mark_info"		); m_eSheetName[GUILD_MARK_INFO_TABLE    ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Jewel_present"			); m_eSheetName[JEWEL_PRESENT_INFO_TABLE ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Jewel_energy"			); m_eSheetName[JEWEL_ENERGY_INFO_TABLE  ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Excavation_info"		); m_eSheetName[EXCAVATION_INFO_TABLE    ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Excavation_grade"		); m_eSheetName[EXCAVATION_GRADE_TABLE   ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Practice"				); m_eSheetName[PRACTICE_TABLE			 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Bingo_reward_line"		); m_eSheetName[BINGO_REWARD_LINE_TABLE	 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Bingo_reward_all"		); m_eSheetName[BINGO_REWARD_ALL_TABLE	 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_dummy_use_event"		); m_eSheetName[DUMMY_EVENT_TABLE        ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_exchange_dummy"			); m_eSheetName[DUMMY_EXCHANGE_TABLE     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_compound_dummy"			); m_eSheetName[DUMMY_COMPOUND_TABLE     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_GameManual"				); m_eSheetName[GAME_MANUAL				 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Event_list"				); m_eSheetName[EVENT_LIST_TABLE		 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_treasure_info"			); m_eSheetName[TREASURE_MAP_TABLE		 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_SelectHero_info"		); m_eSheetName[TUTORIAL_SELECT_HERO	 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Wanted"					); m_eSheetName[WANTED_TABLE			 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Customizing_info"		); m_eSheetName[CUSTOMIZING_INFO_TABLE	 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Customizing_data"		); m_eSheetName[CUSTOMIZING_DATA_TABLE	 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Peso_extractor_info"	); m_eSheetName[PESOEXTRACTOR_TABLE		 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Growthapplicator_info"	); m_eSheetName[GROWTHAPPLICATOR_TABLE	 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_Growthbooster_info"		); m_eSheetName[GROWTHBOOSTER_TABLE		 ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_RB_item_info"			); m_eSheetName[RANDOMBOX_ITEM_GROUP     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_costum_item_info"		); m_eSheetName[COSTUME_INFO_TABLE	     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_CloverExchange_info"	); m_eSheetName[CLOVER_EXCHANGE_TABLE    ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_LV_Reward_info"			); m_eSheetName[LEVEL_REWARD_INFO_TABLE  ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_pet_info"				); m_eSheetName[PET_INFO_TABLE		     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_pet_upgrade"			); m_eSheetName[PET_UPGRAGE_TABLE	     ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_SnakeLadders_Move"		); m_eSheetName[SNAKELADDER_MOVE_TABLE   ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_SnakeLadders_Reward"	); m_eSheetName[SNAKELADDER_REWARD_TABLE ] = key;
	memset(key, 0, MAX_PATH); sprintf_s_e(key, "LSC_New_Gashapon_Info"		); m_eSheetName[NEW_GASHAPON_INFO_TABLE	 ] = key;
}

ioTableDataMgr::~ioTableDataMgr()
{
	m_mapEnum.clear();
	m_mapSheetName.clear();
}

bool ioTableDataMgr::Init()
{
	m_bReadExcel = false;

#ifndef SHIPPING
	ioINILoader kLoader( "../global_loadtable.ini" );
	int iReadType = kLoader.LoadInt_e( "LoadTableType", "Client", 0 );
	if( iReadType == 1 )
    {
		m_bReadExcel = true;
    }
	else
    {
		return true;
    }
#else
	return true;
#endif

	//ioAdoAutoInit autoinit;
	ioExcelReader excel;

    _ENCSTR("../../Tool/ioDataMaker/Config.xlsx", szConfigPath);
    _ENCSTR("enum", szEnum);
    _ENCSTR("config", szConfig);

	if( excel.LoadEnum(szConfigPath, szEnum, m_mapEnum) == false )
    {
		return false;
    }

	if( excel.LoadSheet(szConfigPath, szConfig, m_mapSheetName) == false )
    {
		return false;
    }

	return true;
}

int ioTableDataMgr::GetEnumValue( const char* szEnum )
{
	DataEumMap::iterator it = m_mapEnum.find(szEnum);
	if( it == m_mapEnum.end() )
		return 0;

	return it->second;
}

void ioTableDataMgr::GetSheetName(int iIdx, ioHashString& szSheet)
{
	if( COMPARE( iIdx, 0, DATA_TABLE_MAX ) )
		szSheet = m_eSheetName[iIdx];
}

void ioTableDataMgr::GetFileName( const char* szSheet, ioHashString& szFile )
{
	DataSheetMap::iterator it = m_mapSheetName.find(szSheet);
	if( it != m_mapSheetName.end() )
		szFile = it->second;
}

void ioTableDataMgr::SetDataMgr( int iIdx, IBaseData* pData )
{
	if( COMPARE( iIdx, 0, DATA_TABLE_MAX ) )
		m_pDatas[iIdx] = pData;
}

void ioTableDataMgr::ReloadAll()
{
/*
	if( !m_bReadExcel )
		return;

	for( int i = 0; i < DATA_TABLE_MAX; i++ )
	{
		if( i == QUEST_INFO_TABLE )
			continue;
		
		if( m_pDatas[i] )
			m_pDatas[i]->Reload();
	}

	g_EtcItemMgr.Refresh();
	//g_ShopItemManager.Refresh();*/
	//g_MapManager.CreateMapInfos();
}

void ioTableDataMgr::ReleaseInstance()
{
	SAFEDELETE( ms_Singleton );
}