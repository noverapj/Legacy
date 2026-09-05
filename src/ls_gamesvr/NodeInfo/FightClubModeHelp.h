
#ifndef _FightClubModeHelp_h_
#define _FightClubModeHelp_h_

#include "User.h"
#include "ModeHelp.h"

enum
{
	FIGHTCLUB_WAITING,
	FIGHTCLUB_CHAMPION,
	FIGHTCLUB_CHALLENGER,
	FIGHTCLUB_OBSERVER,
};

enum FightClubResults
{
	FIGHTCLUB_RESULT_NONE = 0,
	FIGHTCLUB_RESULT_CHANPIONWIN,
	FIGHTCLUB_RESULT_CHALLENGERWIN
};

enum FightClubTargets
{
	FIGHTCLUB_TARGET_NONE = 0,
	FIGHTCLUB_TARGET_CHANPION,
	FIGHTCLUB_TARGET_CHALLENGER,
	FIGHTCLUB_TARGET_WAITER,
	FIGHTCLUB_TARGET_OBSERVER
};

struct FightClubRecord : public ModeRecord
{
	int  iFightState;
	int  iFightSeq;
	int  iFightVictories;
	int  iFightCheer;
	bool bFightWin;
	FightClubRecord()
	{
		bFightWin = false;
		iFightSeq = iFightVictories = iFightCheer = 0;
		iFightState = FIGHTCLUB_WAITING;
	}
};

typedef std::vector< FightClubRecord > FightClubRecordList;

struct FightNPCRecord
{
	DWORD        dwCode; 
	ioHashString szName;
	ioHashString szSyncUser;

	RecordState  eState;
	TeamType     eTeam;

	DWORD        dwCurDieTime;

	FightNPCRecord()
	{
		dwCode		= 0;

		eState      = RS_LOADING;
		eTeam       = TEAM_RED;

		dwCurDieTime= 0;
	}
};

typedef std::vector< FightNPCRecord > FightNPCRecordList;

#endif