

#ifndef _HeroMatchModeHelp_h_
#define _HeroMatchModeHelp_h_

#include "ModeHelp.h"

struct HeroMatchRecord : public ModeRecord
{
	HeroMatchRecord()
	{
	}
};

typedef std::vector<HeroMatchRecord> HeroMatchRecordList;

#endif