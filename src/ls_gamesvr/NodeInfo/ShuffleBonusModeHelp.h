
#ifndef _ShuffleBonusModeHelp_h_
#define _ShuffleBonusModeHelp_h_

#include "User.h"
#include "ModeHelp.h"

struct ShuffleBonusRecord : public ModeRecord
{
	int m_iStarCount;
	int m_iStarCountByCalcBonus;

	float m_fBonusPercent;

	int m_iMonsterSyncCount;

	ShuffleBonusRecord()
	{
		m_iStarCount            = 0;
		m_iStarCountByCalcBonus	= 0;

		m_fBonusPercent		= 0.0f;
		m_iMonsterSyncCount = 0;
	}
};

typedef std::vector< ShuffleBonusRecord > ShuffleBonusRecordList;
typedef std::vector< ShuffleBonusRecord* > ShuffleBonusPtrRecordList;

#endif