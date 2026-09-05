

#ifndef _CatchModeHelp_h_
#define _CatchModeHelp_h_

#include "ModeHelp.h"

struct CatchRecord : public ModeRecord
{
	DWORD dwCurPrisonerTime;
	bool  bFirstPrisoner;
	bool  bPrisoner;
	DWORD dwRunningManDeco;
	ioHashString szRunningManName;

	CatchRecord()
	{
		dwCurPrisonerTime = 0;
		dwRunningManDeco  = 0;
		bFirstPrisoner = false;
		bPrisoner = false;
	}
};

typedef std::vector<CatchRecord> CatchRecordList;

#endif