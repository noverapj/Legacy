

#ifndef _SymbolModeHelp_h_
#define _SymbolModeHelp_h_

#include "ModeHelp.h"

struct SymbolStruct
{
	TeamType m_Team;
	TeamType m_OrgTeam;
	int m_iRevivalCnt;

	float m_fMaxHP;
	float m_fCurHP;

	bool m_bActive;

	SymbolStruct()
	{
		m_Team = m_OrgTeam = TEAM_NONE;
		m_iRevivalCnt = 0;

		m_fMaxHP = 0.0f;
		m_fCurHP = 0.0f;

		m_bActive = false;
	}

	void RestoreTeam()
	{
		m_Team = m_OrgTeam;
		m_fMaxHP = 0.0f;
		m_fCurHP = 0.0f;
	}
	
	void SetActive( bool bActive )
	{
		m_bActive = bActive;
		m_Team = TEAM_NONE;

		if( bActive )
			m_Team = m_OrgTeam;
	}
};

typedef std::vector< SymbolStruct > SymbolList;

struct SymbolRecord : public ModeRecord
{
	SymbolRecord()
	{
	}
};

typedef std::vector< SymbolRecord > SymbolRecordList;

#endif