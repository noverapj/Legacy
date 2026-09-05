
#ifndef _DoubleCrownlModeHelp_h_
#define _DoubleCrownlModeHelp_h_

#include "User.h"
#include "ModeHelp.h"

struct DoubleCrownRecord : public ModeRecord
{
	DoubleCrownRecord()
	{
	}
};

typedef std::vector< DoubleCrownRecord > DoubleCrownRecordList;

#endif