
#ifndef _SurvivalModeHelp_h_
#define _SurvivalModeHelp_h_

#include "User.h"
#include "ModeHelp.h"

struct SurvivalRecord : public ModeRecord
{
	SurvivalRecord()
	{
	}
};

typedef std::vector< SurvivalRecord > SurvivalRecordList;

#endif