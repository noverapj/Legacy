

#ifndef _FootballModeHelp_h_
#define _FootballModeHelp_h_

#include "ModeHelp.h"

struct FootballRecord : public ModeRecord
{
	FootballRecord()
	{
	}
};

typedef std::vector< FootballRecord > FootballRecordList;

#endif