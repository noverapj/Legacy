

#include "stdafx.h"

#include "ModeCreator.h"

#include "Mode.h"
#include "SymbolMode.h"
#include "HiddenkingMode.h"
#include "TrainingMode.h"
#include "SurvivalMode.h"
#include "HeroMatchMode.h"
#include "TeamSurvivalMode.h"
#include "BossMode.h"
#include "FootballMode.h"
#include "MonsterSurvivalMode.h"
#include "GangsiMode.h"
#include "DungeonAMode.h"
#include "HeadquartersMode.h"
#include "CatchRunningManMode.h"
#include "FightClubMode.h"
#include "TowerDefMode.h"
#include "DoubleCrownMode.h"
#include "ShuffleBonusMode.h"
#include "TeamSurvivalAIMode.h"
#include "HouseMode.h"
#include "UnderwearMode.h"
#include "CBTMode.h"
#include "RaidMode.h"
#include "SuccessionMode.h"
#include "PracticeMode.h"
#include "FlagMode.h"	// 2018-03-15 by bckim, 깃발모드 추가
#include "ArenaMode.h"	// 2018-07-25 by bckim, 아레나 모드 추가
#include "BattleMode.h"	// 2019-02-14 by bckim, 배틀 모드 추가
#include "FarmingMode.h"	// 2019-07-03 by bckim, 파밍모드 추가


Mode* ModeCreator::CreateMode( Room *pCreator, ModeType eMode )
{
	Mode *pMode = NULL;
	switch( eMode )
	{
	case MT_SYMBOL:
		pMode = new SymbolMode( pCreator );
		break;
	case MT_UNDERWEAR:
		pMode = new UnderwearMode( pCreator );
		break;
	case MT_CBT:
		pMode = new CBTMode( pCreator );
		break;
	case MT_CATCH:
		pMode = new CatchMode( pCreator );
		break;
	case MT_KING:
		pMode = new HiddenkingMode( pCreator );
		break;
	case MT_TRAINING:
		pMode = new TrainingMode( pCreator );
		break;
	case MT_SURVIVAL:
		pMode = new SurvivalMode( pCreator );
		break;
	case MT_TEAM_SURVIVAL:
		pMode = new TeamSurvivalMode( pCreator );
		break;
	case MT_TEAM_SURVIVAL_AI:
		pMode = new TeamSurvivalAIMode( pCreator );
		break;
	case MT_BOSS: 
		pMode = new BossMode( pCreator );
		break;
	case MT_MONSTER_SURVIVAL: 
		pMode = new MonsterSurvivalMode( pCreator );
		break;
	case MT_FOOTBALL:
		pMode = new FootballMode( pCreator );
		break;
	case MT_HEROMATCH:
		pMode = new HeroMatchMode( pCreator );
		break;
	case MT_GANGSI:
		pMode = new GangsiMode( pCreator );
		break;
	case MT_DUNGEON_A:
		pMode = new DungeonAMode( pCreator );
		break;
	case MT_HEADQUARTERS:
		pMode = new HeadquartersMode( pCreator );
		break;
	case MT_CATCH_RUNNINGMAN:
		pMode = new CatchRunningManMode( pCreator );
		break;
	case MT_FIGHT_CLUB:
		pMode = new FightClubMode( pCreator );
		break;
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_FACTORY:
		pMode = new CTowerDefMode( pCreator, eMode );
		break;
	case MT_DOBULE_CROWN:
		pMode = new DoubleCrownMode( pCreator );
		break;
	case MT_SHUFFLE_BONUS:
		pMode = new ShuffleBonusMode( pCreator );
		break;
	case MT_HOUSE:
		pMode = new HouseMode( pCreator );
		break;
	case MT_RAID: 
		pMode = new RaidMode( pCreator, eMode );
		break;
	case MT_SUCCESSION:
		pMode = new SuccessionMode( pCreator );
		break;
	case MT_PRACTICE:
		pMode = new PracticeMode( pCreator );
		break;

#ifdef FLAG_MODE_BY_BCKIM			// 2018-03-15 by bckim, 깃발모드 추가	//	ModeCreator::CreateMode
	case MT_FLAG:
		pMode = new FlagMode( pCreator );
#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// 모드 생성 ModeCreator::CreateMode
		LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:ModeCreator::CreateMode================] eMode[%d]",eMode);
#endif // FLAG_MODE_BY_BCKIM_DEBUG
		break;
#endif
#ifdef ARENA_MODE_BY_BCKIM		// 2018-07-25 by bckim, 아레나 모드 추가	ModeCreator::CreateMode( Room *pCreator, ModeType eMode )
	case MT_ARENA:
		pMode = new ArenaMode( pCreator );
		LOG.PrintTimeAndLog(0,"[ARENA_MODE_BY_BCKIM:ModeCreator::CreateMode================] eMode[%d]",eMode);
		break;
#endif 	// ARENA_MODE_BY_BCKIM	// End. 2018-07-25 by bckim, 아레나 모드 추가

#ifdef BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가	ModeCreator::CreateMode( Room *pCreator, ModeType eMode )
	case MT_BATTLE:		
		pMode = new BattleMode( pCreator );
		LOG.PrintTimeAndLog(0,"[BATTLE_MODE_BY_BCKIM:ModeCreator::CreateMode================] eMode[%d]",eMode);
		break;
#endif 	// BATTLE_MODE_BY_BCKIM	// End. 2019-02-14 by bckim, 배틀 모드 추가

#ifdef FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가 ModeCreator::CreateMode( Room *pCreator, ModeType eMode )
	case MT_FARMING:		
		pMode = new FarmingMode( pCreator );
		LOG.PrintTimeAndLog(0,"[FARMING_MODE_BY_BCKIM:ModeCreator::CreateMode================] eMode[%d]",eMode);
		break;
#endif 	// FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가

	}	

	if( !pMode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ModeCreator::CreateMode - %d Unknown ModeType", eMode );
		pMode = new SymbolMode( pCreator );	// 임시
	}

	return pMode;
}
