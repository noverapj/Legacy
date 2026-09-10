#pragma once
#include "../../io3DEngine/ioSingleton.h"
class IBaseData;

enum eDATA
{
	QUEST_INFO_TABLE			= 0,
	EVENT_INFO_TABLE			= 1,
	SH_PHASE_DATA_TABLE			= 2,
	SH_PHASE_REWARD_TABLE		= 3,
	SH_RANKING_POINT_TABLE		= 4,
	SH_RANKING_REWARD_TABLE		= 5,
	SH_NPC_INFO_TABLE			= 6,
	PIECE_INFO_TABLE			= 7,
	COMPOSE_REWARD_INFO_TABLE	= 8,
	HERO_ITEM_INFO_TABLE		= 9,
	ETC_ITEM_INFO_TABLE			= 10,
	DECO_ITEM_INFO_TABLE		= 11,
	MOTION_ITEM_INFO_TABLE		= 12,
	MOTION_OPTION_INFO_TABLE	= 13,
	MSG_INFO_TABLE				= 14,
	PRESENT_REWARD_INFO_TABLE	= 15,
	TIMEBOX_INFO_TABLE			= 16,
	RANKING_LEVEL_TABLE			= 17,
	RANKING_BASICVALUE_TABLE	= 18,
	RANKING_REWARD_TABLE		= 19,
	MAP_GROUP_TABLE				= 20,
	MAP_INFO_TABLE				= 21,
	MAP_SET_TABLE				= 22,
	FISH_INFO_TABLE				= 23,
	FISH_GRADE_TABLE			= 24,
	RANDOMBOX_INFO_TABLE		= 25,
	REINFORCE_INFO_TABLE		= 26,
	EXTRA_ITEM_INFO_TABLE		= 27,
	GENERALSHOP_INFO_TABLE		= 28,
	VIPSHOP_INFO_TABLE			= 29,
	SHOPGOODS_INFO_TABLE		= 30,
    GUILD_GRADE_INFO_TABLE      = 31,
    GUILD_MARK_INFO_TABLE       = 32,
	JEWEL_PRESENT_INFO_TABLE	= 33,
	JEWEL_ENERGY_INFO_TABLE		= 34,
	EXCAVATION_INFO_TABLE		= 35,
	EXCAVATION_GRADE_TABLE		= 36,
	PRACTICE_TABLE				= 37,
	BINGO_REWARD_LINE_TABLE		= 38,
	BINGO_REWARD_ALL_TABLE		= 39,
	DUMMY_EVENT_TABLE			= 40,
	DUMMY_EXCHANGE_TABLE		= 41,
	DUMMY_COMPOUND_TABLE		= 42,
	GAME_MANUAL					= 43,
	EVENT_LIST_TABLE			= 44,
	TREASURE_MAP_TABLE			= 45,
	TUTORIAL_SELECT_HERO		= 46,
	WANTED_TABLE				= 47,
	CUSTOMIZING_INFO_TABLE		= 48,
	CUSTOMIZING_DATA_TABLE		= 49,
	PESOEXTRACTOR_TABLE			= 50,
	GROWTHAPPLICATOR_TABLE		= 51,
	GROWTHBOOSTER_TABLE			= 52,
	RANDOMBOX_ITEM_GROUP		= 53,
	COSTUME_INFO_TABLE			= 54,
	CLOVER_EXCHANGE_TABLE		= 55,
	LEVEL_REWARD_INFO_TABLE		= 56,
	PET_INFO_TABLE				= 57,
	PET_UPGRAGE_TABLE			= 58,
	SNAKELADDER_MOVE_TABLE		= 59,
	SNAKELADDER_REWARD_TABLE	= 60,
	NEW_GASHAPON_INFO_TABLE		= 61,

	DATA_TABLE_MAX,
};

typedef std::map< ioHashString, int > DataEumMap;
typedef std::map< ioHashString, ioHashString > DataSheetMap;

class ioTableDataMgr
{
public:
	ioTableDataMgr();
	virtual ~ioTableDataMgr();

public:
	bool	Init();
	int		GetEnumValue( const char* szEnum );
	void	GetSheetName(int iIdx, ioHashString& szSheet);
	void	GetFileName( const char* szSheet, ioHashString& szFile );
	void	SetDataMgr( int iIdx, IBaseData* pData );
	void	ReloadAll();
	void	ReleaseInstance();
	bool	IsReadExcel() { return m_bReadExcel; }

	static ioTableDataMgr& GetSingleton()
	{
		if(!ms_Singleton)
		{
			ioTableDataMgr*	pTemp = new ioTableDataMgr();
			ms_Singleton = pTemp;
		}
		return (*ms_Singleton);
	}



protected:
	static ioTableDataMgr* ms_Singleton;
	bool				m_bReadExcel;
	DataEumMap			m_mapEnum;
	DataSheetMap		m_mapSheetName;
	IBaseData*			m_pDatas[DATA_TABLE_MAX];
	ioHashString		m_eSheetName[DATA_TABLE_MAX];
	ioHashString		m_szExcelFilePath;
};

#define g_TableDataMgr ioTableDataMgr::GetSingleton()