
#ifndef _TrainingModeHelp_h_
#define _TrainingModeHelp_h_

#include "User.h"

struct TrainingRecord : public ModeRecord
{
	int  iMonsterSyncCount;                               // 자신이 동기화를 맡고 있는 몬스터 갯수
	DWORD dwRunningManDeco;
	ioHashString szRunningManName;

	TrainingRecord()
	{
		dwRunningManDeco = 0;
		iMonsterSyncCount = 0;
	}
};

typedef std::vector< TrainingRecord > TrainingRecordList;

#endif