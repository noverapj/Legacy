
#ifndef _BattleModeHelp_h_
#define _BattleModeHelp_h_

#include "User.h"
#include "ModeHelp.h"

struct BattleModeRecord : public ModeRecord
{
	int			m_iUserState;
	int			m_iHP;
	bool		m_bTagWaiting;
	int			m_iEntryActivation;
	DWORD		m_iEntryDurationTime;
	int			m_iEntryActionCount;
	int			m_iBattle_Order;
	DWORD		m_dwTagAcceptTime;
	bool		m_bEntryState;
	int			m_iEntryStateCheckCount;

	BattleModeRecord()
	{
		Init();
	}

	void Init()
	{
		m_iUserState = USER_STATE_WAITING;
		m_iHP	 = 100;
		m_bTagWaiting	= false;	// 테그 요청중 flag
		m_iEntryActivation	= ENTRY_STATE_BEFORE;		// 기회는 한번 // 0 아직 1 난입중 2 난입 끝 
		m_iEntryDurationTime = 0;
		m_iEntryActionCount	 = 0;
		m_iBattle_Order		=	BATTLE_ORDER_RANDOM;
		m_dwTagAcceptTime	= 0;
		m_bEntryState	= false;
		m_iEntryStateCheckCount = 0;
	}
};
typedef std::vector< BattleModeRecord > BattleModeRecordList;
#endif