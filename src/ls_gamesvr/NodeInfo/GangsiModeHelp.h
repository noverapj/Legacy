
#ifndef _GangsiModeHelp_h_
#define _GangsiModeHelp_h_

#include "User.h"
#include "ModeHelp.h"

struct GangsiRecord : public ModeRecord
{
	bool bHostGangsi;
	bool bInfectionGangsi;
	GangsiRecord()
	{		
		bHostGangsi = bInfectionGangsi = false;
	}
};

typedef std::vector< GangsiRecord > GangsiRecordList;

#endif