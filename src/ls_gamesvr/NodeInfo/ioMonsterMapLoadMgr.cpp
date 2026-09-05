#include "stdafx.h"
#include "../EtcHelpFunc.h"
#include "ioMonsterMapLoadMgr.h"
#include <strsafe.h>
#include "../MainServerNode/MainServerNode.h"


template<> ioMonsterMapLoadMgr* Singleton< ioMonsterMapLoadMgr >::ms_Singleton = 0;

ioMonsterMapLoadMgr::ioMonsterMapLoadMgr()
{
}

ioMonsterMapLoadMgr::~ioMonsterMapLoadMgr()
{
	ClearData();
}

ioMonsterMapLoadMgr& ioMonsterMapLoadMgr::GetSingleton()
{
	return Singleton<ioMonsterMapLoadMgr>::GetSingleton();
}

void ioMonsterMapLoadMgr::ClearData()
{
	TurnDataMap::iterator iCreate;
	for(iCreate = m_TurnDataMap.begin();iCreate != m_TurnDataMap.end();iCreate++)
	{
		TurnData &rkList = iCreate->second;
		rkList.m_vMonsterList.clear();
	}
	m_TurnDataMap.clear();

	MonsterDropItemMap::iterator iCreate2;
	for(iCreate2 = m_DropItemTable.begin();iCreate2 != m_DropItemTable.end();iCreate2++)
	{
		MonsterDropItemList &rkList = iCreate2->second;
		rkList.m_ItemList.clear();
	}
	m_DropItemTable.clear();

	MonsterCreateMap::iterator iCreate3;
	for(iCreate3 = m_MonsterCreateTable.begin();iCreate3 != m_MonsterCreateTable.end();iCreate3++)
	{
		MonsterCreateTable &rkList = iCreate3->second;
		rkList.m_vNormalMonster.clear();
	}
	m_MonsterCreateTable.clear();

	MonsterRandCreateMap::iterator iCreate4;
	for(iCreate4 = m_MonsterRandCreateTable.begin();iCreate4 != m_MonsterRandCreateTable.end();iCreate4++)
	{
		MonsterRandCreateTable &rkList = iCreate4->second;
		rkList.m_vMonster.clear();
	}
	m_MonsterRandCreateTable.clear();

	MonsterDiceTableMap::iterator iCreate5;
	for(iCreate5 = m_MonsterDiceTableMap.begin();iCreate5 != m_MonsterDiceTableMap.end();iCreate5++)
	{
		MonsterDiceTable &rkList = iCreate5->second;
		rkList.m_PresentList.clear();
	}
	m_MonsterDiceTableMap.clear();
	m_vDamageDiceValue.clear();

	MonsterDropRewardItemMap::iterator iCreate6;
	for(iCreate6 = m_DropRewardItemTable.begin();iCreate6 != m_DropRewardItemTable.end();iCreate6++)
	{
		MonsterDropRewardItemList &rkList = iCreate6->second;
		rkList.m_ItemList.clear();
	}
	m_DropRewardItemTable.clear();

	MonsterTreasureCardMap::iterator iCreate7;
	for(iCreate7 = m_MonsterTreasureCardMap.begin();iCreate7 != m_MonsterTreasureCardMap.end();iCreate7++)
	{
		MonsterTreasureCardTable &rkList = iCreate7->second;
		rkList.m_CardList.clear();
	}
	m_MonsterTreasureCardMap.clear();
	
	TreasureCardExtraItemRandMap::iterator iCreate8;
	for(iCreate8 = m_TreasureCardExtraItemRandMap.begin();iCreate8 != m_TreasureCardExtraItemRandMap.end();iCreate8++)
	{
		TreasureCardExtraItemRand &rkList = iCreate8->second;
		rkList.m_LimitDateList.clear();
		rkList.m_ReinforceList.clear();
	}
    m_TreasureCardExtraItemRandMap.clear();
}

void ioMonsterMapLoadMgr::LoadMapData()
{
	ClearData();
	LoadTurnData();
	LoadMonsterTable();
	LoadRandMonsterTable();
	LoadDropItemData();	
	LoadDropRewardItemData();
	LoadMonsterDiceTable();
	LoadMonsterTreasureCardTable();
}

void ioMonsterMapLoadMgr::LoadTurnData()
{
	ioINILoader kLoader( "config/sp2_monster_turn_list.ini" );
	kLoader.SetTitle( "info" );

	int iMaxLoadList = kLoader.LoadInt( "max_load_list", 0 );
	for(int iLoadCnt = 0;iLoadCnt < iMaxLoadList;iLoadCnt++)
	{
		kLoader.SetTitle( "info" );
		char szBuf[MAX_PATH] = "";
		sprintf_s( szBuf, "list%d_seq_start", iLoadCnt + 1 );
		int iTurnStart = kLoader.LoadInt( szBuf, 0 );	

		sprintf_s( szBuf, "list%d_seq_end", iLoadCnt + 1 );
		int iTurnEnd = kLoader.LoadInt( szBuf, iTurnStart );

		// LowTurn Load
		for(;iTurnStart <= iTurnEnd;iTurnStart++)
		{
			sprintf_s( szBuf, "turn%d", iTurnStart );
			kLoader.SetTitle( szBuf );

			int iTurnHelpIndex = kLoader.LoadInt( "turn_help_idx", -1 );
			if( iTurnHelpIndex == -1 ) continue;     //

			TurnData kTurnData;
			kTurnData.m_fTurnPoint			  = kLoader.LoadFloat( "turn_point", 10.0f );
			kTurnData.m_bClearAlarm			  = kLoader.LoadBool( "turn_clear_alarm", false );
			kTurnData.m_iAlarmFloor			  = kLoader.LoadInt( "turn_alarm_floor", 0 );
			kTurnData.m_dwReduceNpcCreateTime = kLoader.LoadInt( "turn_reduce_npc_time", 5000 );
			kTurnData.m_dwHelpIndex           = iTurnHelpIndex;
			kTurnData.m_dwCreateMonsterCode   = kLoader.LoadInt( "monster_code", 0 );
			kTurnData.m_dwCreateBossMonsterCode = kLoader.LoadInt( "monster_boss_code", 0 );
			kTurnData.m_dwCreateMonsterTable  = kLoader.LoadInt( "monster_table", 0 );
			m_TurnDataMap.insert( TurnDataMap::value_type( iTurnStart, kTurnData ) );
		}
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][monster]Load turn data : [%d]", m_TurnDataMap.size() );	
}

void ioMonsterMapLoadMgr::LoadMonsterTable()
{
	ioINILoader kLoader( "config/sp2_monster_create_table.ini" );
	
	kLoader.SetTitle( "info" );
	int iMaxTable = kLoader.LoadInt( "max_table", 0 );
	for(int i = 0;i < iMaxTable;i++)
	{
		char szBuf[MAX_PATH] = "";
		sprintf_s( szBuf, "table%d", i + 1 );
		kLoader.SetTitle( szBuf );

		MonsterCreateTable kMonsterTable;

		// 랜덤 몬스터 코드
		int iMaxRandMonsterCode = kLoader.LoadInt( "monster_rand_max", 0 );
		for(int r = 0;r < iMaxRandMonsterCode;r++)
		{
			sprintf_s( szBuf, "monster_rand_code%d", r + 1 );
			kMonsterTable.m_MonsterRandomCode.push_back( kLoader.LoadInt( szBuf, 0 ) );
		}

		// 일반 몬스터 로드 
		int iMaxMonster = kLoader.LoadInt( "max_monster", 0 );
		for(int k = 0;k < iMaxMonster;k++)
		{
			MonsterRecord kMonster;

			// NPC 코드 = 일반
			sprintf_s( szBuf, "monster%d_code", k + 1 );
			kMonster.dwCode = kLoader.LoadInt( szBuf, 0 );			

			// NPC 출현 턴
			sprintf_s( szBuf, "monster%d_turn", k + 1 );
			kMonster.iCreateLimitTurn = kLoader.LoadInt( szBuf, 0 );			

			// NPC 생성 시간
			sprintf_s( szBuf, "monster%d_start_time", k + 1 );
			kMonster.dwStartTime = kLoader.LoadInt( szBuf, 0 );

			// 보스 턴 NPC 생성 시간
			sprintf_s( szBuf, "monster%d_boss_start_time", k + 1 );
			kMonster.dwBossStartTime = kLoader.LoadInt( szBuf, 0 );

			// NPC 죽음 타입
			sprintf_s( szBuf, "monster%d_die_type", k + 1 );
			kMonster.dwDieType = kLoader.LoadInt( szBuf, 0 );

			// NPC 보스 턴 몬스터 출격 여부
			sprintf_s( szBuf, "monster%d_boss_turn_show", k + 1 );
			kMonster.bBossTurnShow = kLoader.LoadBool( szBuf, true );

			// NPC 생성 좌표
			sprintf_s( szBuf, "monster%d_startx", k + 1 );
			kMonster.fStartXPos = kLoader.LoadFloat( szBuf, 0.0f );
			sprintf_s( szBuf, "monster%d_startz", k + 1 );
			kMonster.fStartZPos = kLoader.LoadFloat( szBuf, 0.0f );				

			// NPC team
			sprintf_s( szBuf, "monster%d_team", k + 1 );
			kMonster.eTeam = (TeamType)((kLoader.LoadInt( szBuf, 1 )));

			// NPC : make group 
			sprintf_s( szBuf, "monster%d_group_idx", k + 1 );
			kMonster.nGroupIdx = kLoader.LoadInt( szBuf, 0 );

			sprintf_s( szBuf, "monster%d_type", k + 1 );
			kMonster.nNpcType = kLoader.LoadInt( szBuf, 0 );
			
			// NPC : set group boss 그룹의 보스가 죽을때 같은 그룹의 npc 와 자신을 부활시킨다.
			sprintf_s(szBuf, "monster%d_group_boss", k+1);
			kMonster.bGroupBoss = kLoader.LoadBool( szBuf, false );

			sprintf_s(szBuf, "monster%d_end_game", k+1);
			kMonster.bEndBoss = kLoader.LoadBool( szBuf, false );

			// NPC 드랍 아이템 리스트.
			int d = 0;
			sprintf_s( szBuf, "monster%d_max_drop_list", k + 1 );
			int iMaxDropList = kLoader.LoadInt( szBuf, 0 );
			for(d = 0;d < iMaxDropList;d++)
			{
				sprintf_s( szBuf, "monster%d_drop_list%d", k + 1, d + 1 );
				kMonster.vDropItemList.push_back( kLoader.LoadInt( szBuf, 0 ) );
			}

			// NPC 드랍 보상 아이템 리스트.
			sprintf_s( szBuf, "monster%d_max_drop_reward_list", k + 1 );
			iMaxDropList = kLoader.LoadInt( szBuf, 0 );
			for(d = 0;d < iMaxDropList;d++)
			{
				sprintf_s( szBuf, "monster%d_drop_reward_list%d", k + 1, d + 1 );
				kMonster.vDropRewardItemList.push_back( kLoader.LoadInt( szBuf, 0 ) );
			}

			// NPC가 지급할 선물 리스트 코드
			sprintf_s( szBuf, "monster%d_present_code", k + 1 );
			kMonster.dwPresentCode = kLoader.LoadInt( szBuf, 0 );

			// NPC가 지급할 경험치 / 페소
			sprintf_s( szBuf, "monster%d_reward_exp", k + 1 );
			kMonster.iExpReward = kLoader.LoadInt( szBuf, 0 );
			sprintf_s( szBuf, "monster%d_reward_peso", k + 1 );
			kMonster.iPesoReward = kLoader.LoadInt( szBuf, 0 );

			// NPC가 지급할 주사위 선물
			sprintf_s( szBuf, "monster%d_dice_table", k + 1 );
			kMonster.dwDiceTable = kLoader.LoadInt( szBuf, 0 );

			kMonsterTable.m_vNormalMonster.push_back( kMonster );
		}	

		// 보스 몬스터 로드 
		kMonsterTable.m_BossMonster.dwCode = kLoader.LoadInt( "monster_boss_code", 0 );			
		kMonsterTable.m_BossMonster.iCreateLimitTurn = kLoader.LoadInt( "monster_boss_turn", 0 );	
		kMonsterTable.m_BossMonster.dwStartTime = kLoader.LoadInt( "monster_boss_start_time", 0 );
		kMonsterTable.m_BossMonster.dwDieType = kLoader.LoadInt( "monster_boss_die_type", 0 );
		kMonsterTable.m_BossMonster.fStartXPos = kLoader.LoadFloat( "monster_boss_startx", 0.0f );
		kMonsterTable.m_BossMonster.fStartZPos = kLoader.LoadFloat( "monster_boss_startz", 0.0f );				
		kMonsterTable.m_BossMonster.dwPresentCode = kLoader.LoadInt( "monster_boss_present_code", 0 );
		kMonsterTable.m_BossMonster.iExpReward = kLoader.LoadInt( "monster_boss_reward_exp", 0 );
		kMonsterTable.m_BossMonster.iPesoReward = kLoader.LoadInt( "monster_boss_reward_peso", 0 );
		kMonsterTable.m_BossMonster.dwDiceTable = kLoader.LoadInt( "monster_boss_dice_table", 0 );

		int d = 0;
		int iMaxDropList = kLoader.LoadInt( "monster_boss_max_drop_list", 0 );
		for(d = 0;d < iMaxDropList;d++)
		{
			sprintf_s( szBuf, "monster_boss_drop_list%d", d + 1 );
			kMonsterTable.m_BossMonster.vDropItemList.push_back( kLoader.LoadInt( szBuf, 0 ) );
		}

		iMaxDropList = kLoader.LoadInt( "monster_boss_max_drop_reward_list", 0 );
		for(d = 0;d < iMaxDropList;d++)
		{
			sprintf_s( szBuf, "monster_boss_drop_reward_list%d", d + 1 );
			kMonsterTable.m_BossMonster.vDropRewardItemList.push_back( kLoader.LoadInt( szBuf, 0 ) );
		}

		//
		m_MonsterCreateTable.insert( MonsterCreateMap::value_type( i + 1, kMonsterTable ) );
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][monster]Load monster table : [%d]", m_MonsterCreateTable.size() );	
}

void ioMonsterMapLoadMgr::LoadRandMonsterTable()
{
	ioINILoader kLoader( "config/sp2_monster_create_rand_table.ini" );

	kLoader.SetTitle( "info" );
	int iMaxTable = kLoader.LoadInt( "max_table", 0 );
	for(int i = 0;i < iMaxTable;i++)
	{
		char szBuf[MAX_PATH] = "";
		sprintf_s( szBuf, "table%d", i + 1 );
		kLoader.SetTitle( szBuf );

		MonsterRandCreateTable kMonsterList;
		// 일반 몬스터 로드 
		int iMaxMonster = kLoader.LoadInt( "max_monster", 0 );
		for(int k = 0;k < iMaxMonster;k++)
		{
			MonsterRecord kMonster;

			// NPC 코드 = 일반
			sprintf_s( szBuf, "monster%d_code", k + 1 );
			kMonster.dwCode = kLoader.LoadInt( szBuf, 0 );			

			// NPC 출현 턴
			sprintf_s( szBuf, "monster%d_turn", k + 1 );
			kMonster.iCreateLimitTurn = kLoader.LoadInt( szBuf, 0 );			

			// NPC 생성 시간
			sprintf_s( szBuf, "monster%d_start_time", k + 1 );
			kMonster.dwStartTime = kLoader.LoadInt( szBuf, 0 );

			// 보스 턴 NPC 생성 시간
			sprintf_s( szBuf, "monster%d_boss_start_time", k + 1 );
			kMonster.dwBossStartTime = kLoader.LoadInt( szBuf, 0 );

			// NPC 죽음 타입
			sprintf_s( szBuf, "monster%d_die_type", k + 1 );
			kMonster.dwDieType = kLoader.LoadInt( szBuf, 0 );

			// NPC 보스 턴 몬스터 출격 여부
			sprintf_s( szBuf, "monster%d_boss_turn_show", k + 1 );
			kMonster.bBossTurnShow = kLoader.LoadBool( szBuf, true );

			// NPC 생성 좌표
			sprintf_s( szBuf, "monster%d_startx", k + 1 );
			kMonster.fStartXPos = kLoader.LoadFloat( szBuf, 0.0f );
			sprintf_s( szBuf, "monster%d_startz", k + 1 );
			kMonster.fStartZPos = kLoader.LoadFloat( szBuf, 0.0f );

			// NPC team
			sprintf_s( szBuf, "monster%d_team", k + 1 );
			kMonster.eTeam = (TeamType)((kLoader.LoadInt( szBuf, 1 )));

			// NPC : make group 
			sprintf_s( szBuf, "monster%d_group_idx", k + 1 );
			kMonster.nGroupIdx = kLoader.LoadInt( szBuf, 0 );

			sprintf_s( szBuf, "monster%d_type", k + 1 );
			kMonster.nNpcType = kLoader.LoadInt( szBuf, 0 );

			// NPC : set group boss 그룹의 보스가 죽을때 같은 그룹의 npc 와 자신을 부활시킨다.
			sprintf_s(szBuf, "monster%d_group_boss", k+1);
			kMonster.bGroupBoss = kLoader.LoadBool( szBuf, false );

			// NPC 생성 랜덤값
			sprintf_s( szBuf, "monster%d_rand", k + 1 );
			kMonster.dwCreateRandValue = kLoader.LoadInt( szBuf, 0 );		

			// NPC 생성시 오직 1마리만 존재하는지 체크
			sprintf_s( szBuf, "monster%d_unique_type", k + 1 );
			kMonster.bCreateUniqueType = kLoader.LoadBool( szBuf, false );

			// NPC 드랍 아이템 리스트.
			sprintf_s( szBuf, "monster%d_max_drop_list", k + 1 );
			int iMaxDropList = kLoader.LoadInt( szBuf, 0 );
			for(int d = 0;d < iMaxDropList;d++)
			{
				sprintf_s( szBuf, "monster%d_drop_list%d", k + 1, d + 1 );
				kMonster.vDropItemList.push_back( kLoader.LoadInt( szBuf, 0 ) );
			}

			// NPC가 지급할 선물 리스트 코드
			sprintf_s( szBuf, "monster%d_present_code", k + 1 );
			kMonster.dwPresentCode = kLoader.LoadInt( szBuf, 0 );

			// NPC가 지급할 경험치 / 페소
			sprintf_s( szBuf, "monster%d_reward_exp", k + 1 );
			kMonster.iExpReward = kLoader.LoadInt( szBuf, 0 );
			sprintf_s( szBuf, "monster%d_reward_peso", k + 1 );
			kMonster.iPesoReward = kLoader.LoadInt( szBuf, 0 );

			// NPC가 지급할 주사위 선물
			sprintf_s( szBuf, "monster%d_dice_table", k + 1 );
			kMonster.dwDiceTable = kLoader.LoadInt( szBuf, 0 );
			
			kMonsterList.m_dwRondomSeed += kMonster.dwCreateRandValue;
			kMonsterList.m_vMonster.push_back( kMonster );
		}	
		if(iMaxMonster >= 1)
			m_MonsterRandCreateTable.insert( MonsterRandCreateMap::value_type( i + 1, kMonsterList ) );
	}
	m_MonsterRandCreateRandom.SetRandomSeed( timeGetTime() + ( rand()%11111 + 1 ) );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][monster]Load rand monster table : [%d]", m_MonsterRandCreateTable.size() );	
}

void ioMonsterMapLoadMgr::LoadDropItemData()
{
	ioINILoader kLoader( "config/monstersurvival_dorpitem.ini" );
	kLoader.SetTitle( "info" );

	int iMaxList = kLoader.LoadInt( "max_list", 0 );
	for(int i = 0;i < iMaxList;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "list%d", i + 1 );
		kLoader.SetTitle( szKey );

		MonsterDropItemList kDropItemList;
		kDropItemList.m_dwRandSeed = kLoader.LoadInt( "max_rand_seed", 0 );

		int iMaxItem = kLoader.LoadInt( "max_item", 0 );
		for(int j = 0;j < iMaxItem;j++)
		{
			MonsterDropItem kItemData;
			sprintf_s( szKey, "item%d_code", j + 1 );
			kItemData.m_item_data.m_item_code = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "item%d_reinforce", j + 1 );
			kItemData.m_item_data.m_item_reinforce = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "item%d_rate", j + 1 );
			kItemData.m_dwRateValue = kLoader.LoadInt( szKey, 0 );

			kDropItemList.m_ItemList.push_back( kItemData );
		}
		m_DropItemTable.insert( MonsterDropItemMap::value_type( i + 1, kDropItemList ) );
	}
	m_DropItemRandom.SetRandomSeed( timeGetTime() + ( rand()%10000 + 1 ) );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][monster]Load drop Item data : [%d]", m_DropItemTable.size() );	
}

void ioMonsterMapLoadMgr::LoadDropRewardItemData()
{
	ioINILoader kLoader( "config/monstersurvival_drop_reward_item.ini" );
	kLoader.SetTitle( "info" );

	int iMaxList = kLoader.LoadInt( "max_list", 0 );
	for(int i = 0;i < iMaxList;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "list%d", i + 1 );
		kLoader.SetTitle( szKey );

		MonsterDropRewardItemList kDropRewardItemList;
		kDropRewardItemList.m_dwRandSeed = kLoader.LoadInt( "max_rand_seed", 0 );

		int iMaxItem = kLoader.LoadInt( "max_item", 0 );
		for(int j = 0;j < iMaxItem;j++)
		{
			MonsterDropRewardItem kItemData;

			sprintf_s( szKey, "item%d_rate", j + 1 );
			kItemData.m_dwRateValue = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "item%d_resource", j + 1 );
			kItemData.m_dwResourceValue = kLoader.LoadInt( szKey, 0 );
			
			sprintf_s( szKey, "item%d_all_user_give", j + 1 );
			kItemData.m_bAllUserGive = kLoader.LoadBool( szKey, false );		

			sprintf_s( szKey, "item%d_acquire_right_time", j + 1 );
			kItemData.m_dwAcquireRightTime = kLoader.LoadInt( szKey, 0 ); 

			sprintf_s( szKey, "item%d_release_time", j + 1 );
			kItemData.m_dwReleaseTime = kLoader.LoadInt( szKey, 0 );      

			sprintf_s( szKey, "item%d_present_type", j + 1 );
			kItemData.m_iPresentType = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "item%d_present_state", j + 1 );
			kItemData.m_iPresentState = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "item%d_present_ment", j + 1 );
			kItemData.m_iPresentMent = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "item%d_present_period", j + 1 );
			kItemData.m_iPresentPeriod = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "item%d_present_value1", j + 1 );
			kItemData.m_iPresentValue1 = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "item%d_present_value2", j + 1 );
			kItemData.m_iPresentValue2 = kLoader.LoadInt( szKey, 0 );

			kDropRewardItemList.m_ItemList.push_back( kItemData );
		}
		m_DropRewardItemTable.insert( MonsterDropRewardItemMap::value_type( i + 1, kDropRewardItemList ) );
	}
	m_DropRewardItemRandom.SetRandomSeed( timeGetTime() + ( rand()%10000 + 1 ) );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][monster]Load drop reward Item data : [%d]", m_DropRewardItemTable.size() );	
}

void ioMonsterMapLoadMgr::LoadMonsterDiceTable()
{
	ioINILoader kLoader( "config/sp2_monster_dice.ini" );
	
	int i = 0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	kLoader.SetTitle( "Common" );
	int iMaxDamageList = kLoader.LoadInt( "MaxDamageList", 0 );
	for(i = 0;i < iMaxDamageList;i++)
	{
		sprintf_s( szKey, "DamageRank%dDice", i + 1 );
		m_vDamageDiceValue.push_back( kLoader.LoadInt( szKey, 0 ) );
	}

	int iMaxTableList = kLoader.LoadInt( "MaxPresentTable", 0 );
	for(i = 0;i < iMaxTableList;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "PresentTable%d", i + 1 );
		kLoader.LoadString( szTitle, "Present", "", szBuf, MAX_PATH );

		MonsterDiceTable kDiceTbale;

		IntVec kPresent;
		Help::SplitString( szBuf, kPresent, '.' );
		for(int k = 0;k < (int)kPresent.size();k++)
		{
			if( kPresent[k] == 0 ) continue;

			sprintf_s( szTitle, "Present%d", kPresent[k] );
			kLoader.SetTitle( szTitle );
			
			MonsterDicePresent kDicePresent;
			kDicePresent.m_dwRandValue		= kLoader.LoadInt( "Present_Rand", 0 );
			kDicePresent.m_iPresentType		= kLoader.LoadInt( "Present_Type", 0 );
			kDicePresent.m_iPresentState	= kLoader.LoadInt( "Present_Alarm", 0 );
			kDicePresent.m_iPresentMent		= kLoader.LoadInt( "Present_Ment", 0 );
			kDicePresent.m_iPresentPeriod	= kLoader.LoadInt( "Present_Period", 0 );
			kDicePresent.m_iPresentValue1	= kLoader.LoadInt( "Present_Value1", 0 );
			kDicePresent.m_iPresentValue2	= kLoader.LoadInt( "Present_Value2", 0 );
			
			kDiceTbale.m_dwAllRandSeed += kDicePresent.m_dwRandValue;
			kDiceTbale.m_PresentList.push_back( kDicePresent );
		}
		m_MonsterDiceTableMap.insert( MonsterDiceTableMap::value_type( i + 1, kDiceTbale ) );
	}
	m_MonsterDiceRandom.SetRandomSeed( timeGetTime() + ( rand()%1000 + 1 ) );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][monster]Load monster dice table : [%d] [%d]", m_vDamageDiceValue.size(), m_MonsterDiceTableMap.size() );	
}

void ioMonsterMapLoadMgr::LoadMonsterTreasureCardTable()
{
	ioINILoader kLoader( "config/sp2_monster_treasure_card.ini" );

	int i = 0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	kLoader.SetTitle( "common" );

	// 선물 리스트
	int iMaxTable = kLoader.LoadInt( "max_table", 0 );
	for(i = 0;i < iMaxTable;i++)
	{
		sprintf_s( szKey, "table%d", i + 1 );
		kLoader.SetTitle( szKey );

		MonsterTreasureCardTable kCardTable;

		kCardTable.m_iPresentState = kLoader.LoadInt( "present_state", 0 );
		kCardTable.m_iPresentMent  = kLoader.LoadInt( "present_ment", 0 );
		kCardTable.m_iPresentPeriod = kLoader.LoadInt( "present_period", 0 );	

		int iMaxPresent = kLoader.LoadInt( "max_present", 0 );
		for(int k = 0;k < iMaxPresent;k++)
		{
			MonsterTreasureCard kCard;

			sprintf_s( szKey, "present%d_rand", k + 1 );
			kCard.m_dwRandValue = kLoader.LoadInt( szKey, 0 );			

			sprintf_s( szKey, "present%d_type", k + 1 );
			kCard.m_iPresentType = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "present%d_value1", k + 1 );
			kCard.m_iPresentValue1 = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "present%d_value2", k + 1 );
			kCard.m_iPresentValue2 = kLoader.LoadInt( szKey, 0 );

			kCardTable.m_dwAllRandSeed += kCard.m_dwRandValue;
			kCardTable.m_CardList.push_back( kCard );
		}
		m_MonsterTreasureCardMap.insert( MonsterTreasureCardMap::value_type( i + 1, kCardTable ) );
	}
	m_MonsterTreasureCardRandom.SetRandomSeed( timeGetTime() + ( rand()%1000 + 1 ) );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][monster]Load monster treasure card table : [%d]", m_MonsterTreasureCardMap.size() );	

	// 장비 선물의 기간+강화값 랜덤 리스트
	kLoader.SetTitle( "common" );
	iMaxTable = kLoader.LoadInt( "max_extraitem_rand_table", 0 );
	for(i = 0;i < iMaxTable;i++)
	{
		sprintf_s( szKey, "extraitem_rand_table%d", i + 1 );
		kLoader.SetTitle( szKey );

		int k = 0;
		TreasureCardExtraItemRand kExtraItemRandTable;
		int iMaxLimitDate = kLoader.LoadInt( "max_limit_date", 0 );
		for(k = 0;k < iMaxLimitDate;k++)
		{
			RandValueData kRandData;
			sprintf_s( szKey, "limit_date%d_rand", k + 1 );
			kRandData.m_dwRandValue = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "limit_date%d_value", k + 1 );
			kRandData.m_iRandData = kLoader.LoadInt( szKey, 0 );
			kExtraItemRandTable.m_dwAllLimitDateSeed += kRandData.m_dwRandValue;
			kExtraItemRandTable.m_LimitDateList.push_back( kRandData );
		}

		int iMaxReinforceDate = kLoader.LoadInt( "max_reinforce", 0 );
		for(k = 0;k < iMaxReinforceDate;k++)
		{
			RandValueData kRandData;
			sprintf_s( szKey, "reinforce%d_rand", k + 1 );
			kRandData.m_dwRandValue = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "reinforce%d_value", k + 1 );
			kRandData.m_iRandData = kLoader.LoadInt( szKey, 0 );
			kExtraItemRandTable.m_dwAllReinforceSeed += kRandData.m_dwRandValue;
			kExtraItemRandTable.m_ReinforceList.push_back( kRandData );
		}
		m_TreasureCardExtraItemRandMap.insert( TreasureCardExtraItemRandMap::value_type( i + 1, kExtraItemRandTable ) );
	}
	m_TreasureCardExtraItemRandom.SetRandomSeed( timeGetTime() + ( rand()%1000 + 1 ) );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][monster]Load monster treasure card extraItem rand Table : [%d]", m_TreasureCardExtraItemRandMap.size() );	
}

bool ioMonsterMapLoadMgr::GetTurnData( DWORD dwIndex, TurnData &rkTurnData, int iHighTurn, int iLowTurn, int &rNameCount, bool bAutoDropItem )
{
	TurnDataMap::const_iterator iter = m_TurnDataMap.find( dwIndex );
	if( iter == m_TurnDataMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetTurnData - %d TurnData Not Exist", dwIndex );		
		return false;
	}

	const TurnData &rOriginal = iter->second;
	rkTurnData.m_fTurnPoint = rOriginal.m_fTurnPoint;
	rkTurnData.m_dwReduceNpcCreateTime = rOriginal.m_dwReduceNpcCreateTime;
	rkTurnData.m_dwHelpIndex = rOriginal.m_dwHelpIndex;
	rkTurnData.m_bClearAlarm = rOriginal.m_bClearAlarm;
	rkTurnData.m_iAlarmFloor = rOriginal.m_iAlarmFloor;

	MonsterCreateMap::const_iterator iTable = m_MonsterCreateTable.find( rOriginal.m_dwCreateMonsterTable );
	if( iTable == m_MonsterCreateTable.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetTurnData - %d MonsterTable Not Exist", rOriginal.m_dwCreateMonsterTable );		
		return false;
	}
	
	const MonsterCreateTable &rMonsterTable = iTable->second;
	
	// 랜덤 테이블이 있으면 한번 섞는다.
	DWORDVec vRandomCode;
	if( !rMonsterTable.m_MonsterRandomCode.empty() )
	{
		vRandomCode.resize( rMonsterTable.m_MonsterRandomCode.size() );
		std::copy( rMonsterTable.m_MonsterRandomCode.begin(), rMonsterTable.m_MonsterRandomCode.end(), vRandomCode.begin() );
		io_random_shuffle( vRandomCode.begin(), vRandomCode.end() ); 
	}
	
	// 일반 몬스터 로드
	int iLimitTurn  = iLowTurn + 1;
	int iMaxMonster = rMonsterTable.m_vNormalMonster.size();
	for(int i = 0;i < iMaxMonster;i++)
	{
		MonsterRecord kMonster = rMonsterTable.m_vNormalMonster[i];					
		if( kMonster.iCreateLimitTurn > iLimitTurn ) continue;		
		if( rkTurnData.m_bBossTurn && !kMonster.bBossTurnShow ) continue;
		
		// 몬스터 타입
		if( kMonster.dwCode == 0 )
		{
			if( i < (int)vRandomCode.size() )
				kMonster.dwCode = vRandomCode[i];   // 랜덤으로 추출된 값
			else
				kMonster.dwCode = rOriginal.m_dwCreateMonsterCode;        // 턴에서 지정한 값
		}

		// 보스 턴.
		if( rkTurnData.m_bBossTurn )
		{
			kMonster.dwStartTime = kMonster.dwBossStartTime;
		}		

		// NPC 드랍 아이템
		if( bAutoDropItem )
		{
			kMonster.vDropItemList.push_back( kMonster.dwCode );
		}

		// NPC 이름은 서버가 자동으로 만든다.
		char szNpcName[MAX_PATH] = "";
		sprintf_s( szNpcName, " -N%d- ", ++rNameCount );        // 좌우에 공백을 넣어서 유저 닉네임과 중복되지 않도록 한다.
		kMonster.szName = szNpcName;		

		// NPC가 소속한 턴 적용
		kMonster.iHighTurnIndex = iHighTurn;
		kMonster.iLowTurnIndex  = iLowTurn;

		rkTurnData.m_vMonsterList.push_back( kMonster );
	}

	// 보스 몬스터 로드
	if( rkTurnData.m_bBossTurn )
	{
		MonsterRecord kMonster = rMonsterTable.m_BossMonster;
		// 몬스터 타입
		if( kMonster.dwCode == 0 )
			kMonster.dwCode = rOriginal.m_dwCreateBossMonsterCode;

		// NPC 드랍 아이템
		if( bAutoDropItem )
		{
    		kMonster.vDropItemList.push_back( kMonster.dwCode );
		}

		// NPC 이름은 서버가 자동으로 만든다.
		char szNpcName[MAX_PATH] = "";
		sprintf_s( szNpcName, " -N%d- ", ++rNameCount );        // 좌우에 공백을 넣어서 유저 닉네임과 중복되지 않도록 한다.
		kMonster.szName = szNpcName;		

		// NPC가 소속한 턴 적용
		kMonster.iHighTurnIndex = iHighTurn;
		kMonster.iLowTurnIndex  = iLowTurn;

		rkTurnData.m_vMonsterList.push_back( kMonster );
	}

	if( rkTurnData.m_vMonsterList.empty() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetTurnData - %d - %d - Monster None", iHighTurn, iLowTurn );	
		return false;
	}
	return true;
}

bool ioMonsterMapLoadMgr::GetRandMonster( DWORD dwTable, MonsterRecordList &rkMonster, int &rNameCount, int iMonsterCount, bool bFieldUniqueLive, bool bAutoDropItem )
{
	MonsterRandCreateMap::const_iterator iter = m_MonsterRandCreateTable.find( dwTable );
	if( iter == m_MonsterRandCreateTable.end() )
	{
//		LOG.PrintTimeAndLog( 0, "ioMonsterMapLoadMgr::GetRandMonster - %d TurnData Not Exist", dwTable );		
		return false;
	}
	
	const MonsterRandCreateTable &rOriginal = iter->second;
	for(int i = 0;i < iMonsterCount;i++)
	{
		MonsterRecord kSelectMonster;

		DWORD dwRand	 = m_MonsterRandCreateRandom.Random( rOriginal.m_dwRondomSeed );
		DWORD dwCurValue = 0;
		for(int k = 0;k < (int)rOriginal.m_vMonster.size();k++)
		{
			MonsterRecord kMonster = rOriginal.m_vMonster[k];
			if( bFieldUniqueLive )
			{
				if( kMonster.bCreateUniqueType ) continue;
			}

			DWORD dwValue = kMonster.dwCreateRandValue;
			if( COMPARE( dwRand, dwCurValue, dwCurValue + dwValue ) )
			{
				kSelectMonster = kMonster;
				break;
			}
			dwCurValue += dwValue;
		}
		// NPC 유니크 타입이 선택되었다
		if( kSelectMonster.bCreateUniqueType )
			bFieldUniqueLive = true;

		// NPC 드랍 아이템
		if( bAutoDropItem )
		{
			kSelectMonster.vDropItemList.push_back( kSelectMonster.dwCode );
		}

		// NPC 이름은 서버가 자동으로 만든다.
		char szNpcName[MAX_PATH] = "";
		sprintf_s( szNpcName, " -N%d- ", ++rNameCount );        // 좌우에 공백을 넣어서 유저 닉네임과 중복되지 않도록 한다.
		kSelectMonster.szName = szNpcName;		

		rkMonster.push_back( kSelectMonster );
	}

	return true;
}

void ioMonsterMapLoadMgr::GetMonsterDropItem( DWORD dwTableIndex, ITEM_DATA &rkItem )
{
	MonsterDropItemMap::const_iterator iter = m_DropItemTable.find( dwTableIndex );
	if( iter == m_DropItemTable.end() )
	{
		rkItem.Initialize();
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetMonsterDropItem - %d ItemTable Not Exist", dwTableIndex );		
	}
	else
	{
		const MonsterDropItemList &rkDropItemList = iter->second;

		DWORD dwRandSeed = rkDropItemList.m_dwRandSeed;
		DWORD dwRand = m_DropItemRandom.Random( dwRandSeed );

		int iSize = rkDropItemList.m_ItemList.size();
		DWORD dwCurValue = 0;
		for(int i = 0;i < iSize;i++)
		{
			const MonsterDropItem &rkDropItem = rkDropItemList.m_ItemList[i];
			DWORD dwValue = rkDropItem.m_dwRateValue;
			if( COMPARE( dwRand, dwCurValue, dwCurValue+dwValue ) )
			{
				rkItem = rkDropItem.m_item_data;
				break;
			}
			dwCurValue += dwValue;
		}
	}	
}

void ioMonsterMapLoadMgr::GetMonsterDropRewardItem( DWORD dwTableIndex, MonsterDropRewardItem &rkReturnItem )
{
	MonsterDropRewardItemMap::const_iterator iter = m_DropRewardItemTable.find( dwTableIndex );
	if( iter == m_DropRewardItemTable.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetMonsterDropRewardItem - %d ItemTable Not Exist", dwTableIndex );	
	}
	else
	{
		const MonsterDropRewardItemList &rkDropRewardItemList = iter->second;

		DWORD dwRandSeed = rkDropRewardItemList.m_dwRandSeed;
		DWORD dwRand     = m_DropRewardItemRandom.Random( dwRandSeed );

		int iSize = rkDropRewardItemList.m_ItemList.size();
		DWORD dwCurValue = 0;
		for(int i = 0;i < iSize;i++)
		{
			const MonsterDropRewardItem &rkDropRewardItem = rkDropRewardItemList.m_ItemList[i];
			DWORD dwValue = rkDropRewardItem.m_dwRateValue;
			if( COMPARE( dwRand, dwCurValue, dwCurValue + dwValue ) )
			{
				rkReturnItem = rkDropRewardItem;
				break;
			}
			dwCurValue += dwValue;
		}
	}
}

void ioMonsterMapLoadMgr::GetMonsterDicePresent( DWORD dwTableIndex, int nSubIdx, ioHashString &rkSendID, short &rPresentType, short &rPresentState, 
	short &rPresentMent, int &rPresentPeriod, int &rPresentValue1, int &rPresentValue2 )
{
	if( dwTableIndex == 0 ) return;

	MonsterDiceTableMap::const_iterator iter = m_MonsterDiceTableMap.find( dwTableIndex );
	if( iter == m_MonsterDiceTableMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetMonsterDicePresent - %d Not Exist", dwTableIndex );		
	}
	else
	{
		const MonsterDiceTable &rkDiceTable = iter->second;

		if( rkDiceTable.m_PresentList.empty() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetMonsterDicePresent - %d Not Exist", dwTableIndex );		
			return;
		}

		if( (int)rkDiceTable.m_PresentList.size() <= nSubIdx )
			nSubIdx = rkDiceTable.m_PresentList.size() - 1;

		const MonsterDicePresent &rkPresent = rkDiceTable.m_PresentList[nSubIdx];

		rkSendID		= g_MainServer.GetSendID();
		rPresentType	= rkPresent.m_iPresentType;
		rPresentState   = rkPresent.m_iPresentState;
		rPresentMent    = rkPresent.m_iPresentMent;
		rPresentPeriod  = rkPresent.m_iPresentPeriod;
		rPresentValue1  = rkPresent.m_iPresentValue1;
		rPresentValue2  = rkPresent.m_iPresentValue2;
	}
}

void ioMonsterMapLoadMgr::GetMonsterDicePresent( DWORD dwTableIndex, ioHashString &rkSendID, short &rPresentType, short &rPresentState, 
												 short &rPresentMent, int &rPresentPeriod, int &rPresentValue1, int &rPresentValue2 )
{
	if( dwTableIndex == 0 ) return;

	MonsterDiceTableMap::const_iterator iter = m_MonsterDiceTableMap.find( dwTableIndex );
	if( iter == m_MonsterDiceTableMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetMonsterDicePresent - %d Not Exist", dwTableIndex );		
	}
	else
	{
		const MonsterDiceTable &rkDiceTable = iter->second;
		
		DWORD dwRandValue = 0;
		DWORD dwRand = m_MonsterDiceRandom.Random( rkDiceTable.m_dwAllRandSeed );
		for(int i = 0;i < (int)rkDiceTable.m_PresentList.size();i++)
		{
			const MonsterDicePresent &rkPresent = rkDiceTable.m_PresentList[i];

			if( COMPARE( dwRand, dwRandValue, dwRandValue + rkPresent.m_dwRandValue ) )
			{
				rkSendID		= g_MainServer.GetSendID();
				rPresentType	= rkPresent.m_iPresentType;
				rPresentState   = rkPresent.m_iPresentState;
				rPresentMent    = rkPresent.m_iPresentMent;
				rPresentPeriod  = rkPresent.m_iPresentPeriod;
				rPresentValue1  = rkPresent.m_iPresentValue1;
				rPresentValue2  = rkPresent.m_iPresentValue2;
				break;
			}
			dwRandValue += rkPresent.m_dwRandValue;
		}
	}
}

int ioMonsterMapLoadMgr::GetMonsterDiceRate( int iDamageRank )
{
	if( !COMPARE( iDamageRank, 0, (int)m_vDamageDiceValue.size() ) ) 
		return 0;
	return m_vDamageDiceValue[iDamageRank];
}

// int ioMonsterMapLoadMgr::GetTreasureCardMVPCount( DWORD dwTableIndex )
// {
// 	MonsterTreasureCardMap::const_iterator iter = m_MonsterTreasureCardMap.find( dwTableIndex );
// 	if( iter == m_MonsterTreasureCardMap.end() )
// 	{
// 		LOG.PrintTimeAndLog( 0, "ioMonsterMapLoadMgr::GetTreasureCardMVPCount - %d Not Exist", dwTableIndex );		
// 		return 0;
// 	}
// 
// 	const MonsterTreasureCardTable &rkTreasureTable = iter->second;
// 	return rkTreasureTable.m_iClearMVPCardCount;
// }
// 
// 
// int ioMonsterMapLoadMgr::GetTreasureCardClearCount( DWORD dwTableIndex )
// {
// 	MonsterTreasureCardMap::const_iterator iter = m_MonsterTreasureCardMap.find( dwTableIndex );
// 	if( iter == m_MonsterTreasureCardMap.end() )
// 	{
// 		LOG.PrintTimeAndLog( 0, "ioMonsterMapLoadMgr::GetTreasureCardClearCount - %d Not Exist", dwTableIndex );		
// 		return 0;
// 	}
// 
// 	const MonsterTreasureCardTable &rkTreasureTable = iter->second;
// 	return rkTreasureTable.m_iClearCardCount;
// }

void ioMonsterMapLoadMgr::GetTreasureCardPresent( DWORD dwTableIndex, ioHashString &rkSendID, short &rPresentType, short &rPresentState, 
												  short &rPresentMent, int &rPresentPeriod, int &rPresentValue1, int &rPresentValue2 )
{
	MonsterTreasureCardMap::const_iterator iter = m_MonsterTreasureCardMap.find( dwTableIndex );
	if( iter == m_MonsterTreasureCardMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetTreasureCardPresent - %d Not Exist", dwTableIndex );		
	}
	else
	{
		const MonsterTreasureCardTable &rkTreasureTable = iter->second;
		rkSendID		= g_MainServer.GetSendID();
		rPresentState   = rkTreasureTable.m_iPresentState;
		rPresentMent    = rkTreasureTable.m_iPresentMent;
		rPresentPeriod  = rkTreasureTable.m_iPresentPeriod;

		DWORD dwRandValue = 0;
		DWORD dwRand = m_MonsterTreasureCardRandom.Random( rkTreasureTable.m_dwAllRandSeed );
		for(int i = 0;i < (int)rkTreasureTable.m_CardList.size();i++)
		{
			const MonsterTreasureCard &rkPresent = rkTreasureTable.m_CardList[i];

			if( COMPARE( dwRand, dwRandValue, dwRandValue + rkPresent.m_dwRandValue ) )
			{				
				rPresentType	= rkPresent.m_iPresentType;
				rPresentValue1  = rkPresent.m_iPresentValue1;
				rPresentValue2  = GetTreasureCardPresentValue2( rPresentType, rkPresent.m_iPresentValue2 );
				break;
			}
			dwRandValue += rkPresent.m_dwRandValue;
		}
	}
}


int ioMonsterMapLoadMgr::GetTreasureCardPresentValue2( short iPresentType, int iPresentValue2 )
{
	if( iPresentType == PRESENT_EXTRAITEM )
	{
		TreasureCardExtraItemRandMap::const_iterator iter = m_TreasureCardExtraItemRandMap.find( iPresentValue2 );
		if( iter == m_TreasureCardExtraItemRandMap.end() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetTreasureCardPresentValue2 - %d Not Exist", iPresentValue2 );	
		}
		else
		{
			int i = 0;
			const TreasureCardExtraItemRand &rkExtraItemRand = iter->second;
			
			// 기간
			int iLimitDate = -1;
			DWORD dwRandValue = 0;
			DWORD dwRand = m_TreasureCardExtraItemRandom.Random( rkExtraItemRand.m_dwAllLimitDateSeed );
			for(i = 0;i < (int)rkExtraItemRand.m_LimitDateList.size();i++)
			{
				const RandValueData &rkRandValue = rkExtraItemRand.m_LimitDateList[i];
				if( COMPARE( dwRand, dwRandValue, dwRandValue + rkRandValue.m_dwRandValue ) )
				{
					iLimitDate = rkRandValue.m_iRandData;
					break;
				}
				dwRandValue += rkRandValue.m_dwRandValue;
			}

			// 강화
			int iReinforce = -1;
			dwRand = m_TreasureCardExtraItemRandom.Random( rkExtraItemRand.m_dwAllReinforceSeed );
            for(i = 0,dwRandValue = 0;i < (int)rkExtraItemRand.m_ReinforceList.size();i++)
			{
				const RandValueData &rkRandValue = rkExtraItemRand.m_ReinforceList[i];
				if( COMPARE( dwRand, dwRandValue, dwRandValue + rkRandValue.m_dwRandValue ) )
				{
					iReinforce = rkRandValue.m_iRandData;
					break;
				}
				dwRandValue += rkRandValue.m_dwRandValue;
			}

			if( iLimitDate == -1 )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioMonsterMapLoadMgr::GetTreasureCardPresentValue2 - %d Rand Failed : %d - %d", iPresentValue2, iLimitDate, iReinforce );	
				iPresentValue2 = ( 0 * PRESENT_EXTRAITEM_DIVISION_2 ) + 1;
			}
			else
			{
				int iValue = abs(iReinforce) * PRESENT_EXTRAITEM_DIVISION_2 + iLimitDate;
				if( iReinforce < 0 )
					iValue = -iValue;

				iPresentValue2 = iValue;
			}
		}
	}
    return iPresentValue2;
}