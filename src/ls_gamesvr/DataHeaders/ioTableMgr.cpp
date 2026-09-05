#include <stdafx.h>
#include "ioTableMgr.h"
#include "ioExcelReader.h"
#include "BaseDataManager.h"

ioTableDataMgr::ioTableDataMgr() : m_bReadExcel(false)
{
	// 헤더에 선언된 이넘값과 1:1 매칭이 되어야 함
	m_eSheetName[QUEST_INFO_TABLE]					= "LSC_quest_info";
	m_eSheetName[EVENT_INFO_TABLE]					= "LSC_event_info";
	m_eSheetName[EVENTSHOP_SLOT_INFO_TABLE]			= "LSC_EventShop_Slot_info";
	m_eSheetName[SH_PHASE_DATA_TABLE]				= "LSC_SH_Phase_Data";
	m_eSheetName[SH_PHASE_REWARD_TABLE]				= "LSC_SH_Phase_Reward";
	m_eSheetName[SH_RANKING_POINT_TABLE]			= "LSC_SH_Ranking_Point";
	m_eSheetName[SH_RANKING_REWARD_TABLE]			= "LSC_SH_Ranking_Reward";
	m_eSheetName[SH_NPC_INFO_TABLE]					= "LSC_SH_NPC_Info";
	m_eSheetName[SH_REWARD_INFO_TALBE]				= "LSC_SH_Reward_Info";
	m_eSheetName[RANDOMBOX_INFO_TABLE]				= "LSC_RandomboxInfo";
	m_eSheetName[PIECE_INFO_TABLE]					= "LSC_PieceInfo";
	m_eSheetName[COMPOSE_REWARD_INFO_TABLE]			= "LSC_ComposeRewardInfo";
	m_eSheetName[PIECE_CHANGE_REWARD_INFO_TALBE]	= "LSC_PiecechangeRewardInfo";
	m_eSheetName[ITEM_INFO_TABLE]					= "LSC_item_info";
	m_eSheetName[HERO_ITEM_INFO_TABLE]				= "LSC_heroitem_info";
	m_eSheetName[EXTRA_ITEM_INFO_TABLE]				= "LSC_extraitem_info";
	m_eSheetName[ETC_ITEM_INFO_TABLE]				= "LSC_etcitem_info";
	m_eSheetName[DECO_ITEM_INFO_TABLE]				= "LSC_decoitem_info";
	m_eSheetName[MOTION_ITEM_INFO_TABLE]			= "LSC_motionitem_info";
	m_eSheetName[PRESENT_REWARD_INFO_TABLE]			= "LSC_PresentReward_info";
	m_eSheetName[TIMEBOX_INFO_TABLE]				= "LSC_timebox_info";
	m_eSheetName[RANKING_LEVEL_TABLE]				= "LSC_RankingLevel_info";
	m_eSheetName[RANKING_BASICVALUE_TABLE]			= "LSC_Ranking_BasicValue";
	m_eSheetName[RANKING_REWARD_TABLE]				= "LSC_Ranking_Reward";
	m_eSheetName[HERO_WEAPON_TABLE]					= "LSC_heroweapon";
	m_eSheetName[RARE_WEAPON_TABLE]					= "LSC_rareweapon";
	m_eSheetName[MAP_INFO_TABLE]					= "LSC_MapInfo";
	m_eSheetName[MAP_SET_TABLE]						= "LSC_MapSet";
	m_eSheetName[FISH_INFO_TABLE]					= "LSC_Fish_Info";
	m_eSheetName[FISH_GRADE_TABLE]					= "LSC_Fish_Grade";
	m_eSheetName[PVE_REWARD_INFO_TABLE]				= "LSC_PVE_Reward_info";
	m_eSheetName[PVE_REWARD_LIST_TABLE]				= "LSC_PVE_Reward_list";
	m_eSheetName[PVP_REWARD_INFO_TABLE]				= "LSC_PVP_Reward_info";
	m_eSheetName[PVP_REWARD_LIST_TABLE]				= "LSC_PVP_Reward_list";
	m_eSheetName[REINFORCE_INFO_TABLE]				= "LSC_Reinforce_info";
	m_eSheetName[SHOP_GOODS_TABLE]					= "LSC_ShopGoods_info";
	m_eSheetName[GENERAL_SHOP_TABLE]				= "LSC_GeneralShop_info";
	m_eSheetName[VIP_SHOP_TABLE]					= "LSC_VipShop_info";
	m_eSheetName[EVENT_SHOP_TABLE]					= "LSC_EventShop_info";
	m_eSheetName[EVENT_SHOP_SCHEDULE_TABLE]			= "LSC_EventShopSchedule_info";
	m_eSheetName[JEWEL_ENERGY_INFO_TABLE]			= "LSC_Jewel_energy";
	m_eSheetName[JEWEL_BOX_INFO_TABLE]				= "LSC_Jewelbox_info";
	m_eSheetName[GUILD_GRADE_INFO_TABLE]			= "LSC_guild_grade_info";
	m_eSheetName[ITEM_INIT_CONTROL]					= "LSC_item_init_control";
	//m_eSheetName[LV_REWARD_LIST_TABLE]				= "LSC_LV_Reward_list";
	m_eSheetName[LV_REWARD_INFO_TABLE]				= "LSC_LV_Reward_info";
	m_eSheetName[EXCAVATION_REWARD]					= "LSC_Excavation_info";
	m_eSheetName[EXCAVATION_GRADE]					= "LSC_Excavation_grade";
	m_eSheetName[PRACTICE_TABLE]					= "LSC_Practice";
	m_eSheetName[DUMMY_EVENT_COMPOUND]				= "LSC_compound_dummy";
	m_eSheetName[DUMMY_EVENT_USE]					= "LSC_dummy_use_event";
	m_eSheetName[DUMMY_EVENT_EXCHANGE]				= "LSC_exchange_dummy";
	m_eSheetName[BINGO_REWARD_LINE]					= "LSC_Bingo_reward_line";
	m_eSheetName[BINGO_REWARD_ALL]					= "LSC_Bingo_reward_all";
	m_eSheetName[EVENT_HERO_ITEM_INFO]				= "LSC_Event_heroitem_info";
	m_eSheetName[TREASURE_INFO]						= "LSC_treasure_info";
	m_eSheetName[TIMEBOX_REWARD_GROUP_TABLE]		= "LSC_timebox_reward_group";
	m_eSheetName[WANTED_TABLE]						= "LSC_Wanted";
	m_eSheetName[TUTORIAL_SELECT_HERO]				= "LSC_SelectHero_info";	
	m_eSheetName[CUSTOMIZING_INFO_TABLE]			= "LSC_Customizing_info";
	m_eSheetName[CUSTOMIZING_DATA_TABLE]			= "LSC_Customizing_data";
	m_eSheetName[AIUSERDATA_TABLE]					= "LSC_AI_userlist_info";
	m_eSheetName[PESOEXTRACTOR_TABLE]				= "LSC_Peso_extractor_info";
	m_eSheetName[GROWTHAPPLICATOR_TABLE]			= "LSC_Growthapplicator_info";
	m_eSheetName[GROWTHBOOSTER_TABLE]				= "LSC_Growthbooster_info";
	m_eSheetName[RANDOMBOX_ITEM_GROUP]				= "LSC_RB_item_info";
	m_eSheetName[COSTUME_ITEM_TABLE]				= "LSC_costum_item_info";
	m_eSheetName[PET_INFO_TABLE]					= "LSC_pet_info";
	m_eSheetName[PET_UPGRADE_TABLE]					= "LSC_pet_upgrade";
	
	m_eSheetName[SNAKELADDER_MOVE_INFO]				= "LSC_SnakeLadders_Move";			// 2018-08-30 by bckim, 주사위 이벤트 추가
	m_eSheetName[SNAKELADDER_REWARD_INFO]			= "LSC_SnakeLadders_Reward";		// 2018-08-30 by bckim, 주사위 이벤트 추가		

#ifdef OHTG_NEW_RANROM_BOX_20201109
	m_eSheetName[RANDOM_BOX_INFO_TABLE]				= "LSC_New_Gashapon_info";		// 2018-08-30 by bckim, 주사위 이벤트 추가		
#endif //OHTG_NEW_RANROM_BOX_20201109
}

ioTableDataMgr::~ioTableDataMgr()
{
	m_mapEnum.clear();
	m_mapSheetName.clear();
}

bool ioTableDataMgr::Init()
{
	ioINILoader kLoader;
	bool bResult = kLoader.ReloadFile( "../global_loadtable.ini" );
	if( !bResult ) return false;

	int iReadType = kLoader.LoadInt( "LoadTableType", "GameSvr", 0 );
	if( iReadType == 1 )
		m_bReadExcel = true;
	else
		return true;

	ioAdoAutoInit autoinit;
	
	ioExcelReader excel;
	if( excel.LoadEnum("../../Tool/ioDataMaker/Config.xlsx", "enum", m_mapEnum) == false )
		return false;

	if( excel.LoadSheet("../../Tool/ioDataMaker/Config.xlsx", "config", m_mapSheetName) == false )
		return false;

	return true;
}

int ioTableDataMgr::GetEnumValue( const char* szEnum )
{
	DataEumMap::iterator it = m_mapEnum.find(szEnum);
	if( it == m_mapEnum.end() )
		return 0;

	return it->second;
}

void ioTableDataMgr::GetSheetName(int iIdx, tstring& szSheet)
{
	if( COMPARE( iIdx, 0, DATA_TABLE_MAX ) )
		szSheet = m_eSheetName[iIdx];
}

void ioTableDataMgr::GetFileName( const char* szSheet, tstring& szFile )
{
	DataSheetMap::iterator it = m_mapSheetName.find(szSheet);
	if( it != m_mapSheetName.end() )
		szFile = it->second;
}