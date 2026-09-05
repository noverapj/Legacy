

#ifndef _SuccessionModeHelp_h_
#define _SuccessionModeHelp_h_

#include "ModeHelp.h"

struct SuccessionRecord : public ModeRecord
{
	DWORD dwCurPrisonerTime;
	bool  bFirstPrisoner;
	bool  bPrisoner;
	DWORD dwRunningManDeco;
	ioHashString szRunningManName;

	float m_fWinRate;

	SuccessionRecord()
	{
		dwCurPrisonerTime = 0;
		dwRunningManDeco  = 0;
		bFirstPrisoner = false;
		bPrisoner = false;
		m_fWinRate = 0.0f;
	}
};

typedef std::vector<SuccessionRecord> SuccessionRecordList;

#endif