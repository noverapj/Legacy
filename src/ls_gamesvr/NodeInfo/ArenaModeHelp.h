

#ifndef _ArenaModeHelp_h_
#define _ArenaModeHelp_h_

#include "ModeHelp.h"

struct ArenaRecord : public ModeRecord
{
	DWORD dwCurPrisonerTime;
	bool  bFirstPrisoner;
	bool  bPrisoner;
	DWORD dwRunningManDeco;
	ioHashString szRunningManName;

	ArenaRecord()
	{
		dwCurPrisonerTime = 0;
		dwRunningManDeco  = 0;
		bFirstPrisoner = false;
		bPrisoner = false;
	}
};

typedef std::vector<ArenaRecord> ArenaRecordList;

#endif