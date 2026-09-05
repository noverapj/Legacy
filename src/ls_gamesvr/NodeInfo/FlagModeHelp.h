#ifndef _FlagModeHelp_h_
#define _FlagModeHelp_h_

#include "User.h"
#include "ModeHelp.h"

struct FlagRecord : public ModeRecord
{
	FlagRecord()
	{
	}
};

typedef std::vector< FlagRecord > FlagRecordList;

#endif