#pragma once

class IBaseData;

enum eDATA
{
	QUEST_INFO_TABLE				= 0,
	EVENT_INFO_TABLE				= 1,
	EVENTSHOP_SLOT_INFO_TABLE		= 2,
	SH_PHASE_DATA_TABLE				= 3,
	SH_PHASE_REWARD_TABLE			= 4,
	SH_RANKING_POINT_TABLE			= 5,
	SH_RANKING_REWARD_TABLE			= 6,
	SH_NPC_INFO_TABLE				= 7,
	SH_REWARD_INFO_TALBE			= 8,
	RANDOMBOX_INFO_TABLE			= 9,
	PIECE_INFO_TABLE				= 10,
	COMPOSE_REWARD_INFO_TABLE		= 11,
	PIECE_CHANGE_REWARD_INFO_TALBE	= 12,
	ITEM_INFO_TABLE					= 13,
	HERO_ITEM_INFO_TABLE			= 14,
	EXTRA_ITEM_INFO_TABLE			= 15,
	ETC_ITEM_INFO_TABLE				= 16,
	DECO_ITEM_INFO_TABLE			= 17,
	MOTION_ITEM_INFO_TABLE			= 18,
	PRESENT_REWARD_INFO_TABLE		= 19,
	TIMEBOX_INFO_TABLE				= 20,
	RANKING_LEVEL_TABLE				= 21,
	RANKING_BASICVALUE_TABLE		= 22,
	RANKING_REWARD_TABLE			= 23,
	HERO_WEAPON_TABLE				= 24,
	RARE_WEAPON_TABLE				= 25,
	MAP_INFO_TABLE					= 26,
	MAP_SET_TABLE					= 27,
	FISH_INFO_TABLE					= 28,
	FISH_GRADE_TABLE				= 29,
	PVE_REWARD_INFO_TABLE			= 30,
	PVE_REWARD_LIST_TABLE			= 31,
	PVP_REWARD_INFO_TABLE			= 32,
	PVP_REWARD_LIST_TABLE			= 33,
	REINFORCE_INFO_TABLE			= 34,
	SHOP_GOODS_TABLE				= 35,
	GENERAL_SHOP_TABLE				= 36,
	VIP_SHOP_TABLE					= 37,
	EVENT_SHOP_TABLE				= 38,
	EVENT_SHOP_SCHEDULE_TABLE		= 39,
	JEWEL_ENERGY_INFO_TABLE			= 40,
	JEWEL_BOX_INFO_TABLE			= 41,
    GUILD_GRADE_INFO_TABLE          = 42,
	ITEM_INIT_CONTROL				= 43,
	LV_REWARD_INFO_TABLE			= 44,
	//LV_REWARD_LIST_TABLE			= 45,
	EXCAVATION_REWARD				= 46,
	EXCAVATION_GRADE				= 47,
	PRACTICE_TABLE					= 48,
	DUMMY_EVENT_COMPOUND			= 49,
	DUMMY_EVENT_USE					= 50,
	DUMMY_EVENT_EXCHANGE			= 51,
	BINGO_REWARD_LINE				= 52,
	BINGO_REWARD_ALL				= 53,
	EVENT_HERO_ITEM_INFO			= 54,
	TREASURE_INFO					= 55,
	TIMEBOX_REWARD_GROUP_TABLE		= 56,
	WANTED_TABLE					= 57,
	TUTORIAL_SELECT_HERO            = 58,
	CUSTOMIZING_INFO_TABLE			= 59,
	CUSTOMIZING_DATA_TABLE			= 60,
	AIUSERDATA_TABLE				= 61,
	PESOEXTRACTOR_TABLE				= 62,
	GROWTHAPPLICATOR_TABLE			= 63,
	GROWTHBOOSTER_TABLE				= 64,
	RANDOMBOX_ITEM_GROUP			= 65,
	COSTUME_ITEM_TABLE				= 66,
	PET_INFO_TABLE					= 67,
	PET_UPGRADE_TABLE				= 68,
	SNAKELADDER_MOVE_INFO			= 69,	// 2018-08-30 by bckim, 주사위 이벤트 추가
	SNAKELADDER_REWARD_INFO			= 70,	// 2018-08-30 by bckim, 주사위 이벤트 추가

	RANDOM_BOX_INFO_TABLE			= 71,	// 2018-08-30 by bckim, 주사위 이벤트 추가

	DATA_TABLE_MAX,
};
typedef std::basic_string<TCHAR> tstring;
typedef boost::unordered_map<tstring, int> DataEumMap;
typedef boost::unordered_map<tstring, tstring> DataSheetMap;

class ioTableDataMgr
{
public:
	ioTableDataMgr();
	virtual ~ioTableDataMgr();

public:
	bool	Init();
	int		GetEnumValue( const char* szEnum );
	void	GetSheetName(int iIdx, tstring& szSheet);
	void	GetFileName( const char* szSheet, tstring& szFile );
	bool	IsReadExcel() { return m_bReadExcel; }

protected:
	bool				m_bReadExcel;
	DataEumMap			m_mapEnum;
	DataSheetMap		m_mapSheetName;
	tstring				m_eSheetName[DATA_TABLE_MAX];
	tstring				m_szExcelFilePath;
};

#define g_TableDataMgr (*cSingleton<ioTableDataMgr>::GetInstance())