
#ifndef _BossModeHelp_h_
#define _BossModeHelp_h_

#include "User.h"
#include "ModeHelp.h"

struct BossRecord : public ModeRecord
{
	int iBossLevel;
	BossRecord()
	{
		iBossLevel = 1;
	}
};

typedef std::vector< BossRecord > BossRecordList;

#endif