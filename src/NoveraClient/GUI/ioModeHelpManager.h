#pragma once


#include "../ioComplexStringPrinter.h"
#include "FSMStateMgr.h"

struct HelpText
{
	DWORD m_dwColor;
	char  m_szHelp[MAX_PATH];

	//
	DWORD m_dwSwapCurTIme;
	bool  m_bSwapHelp;
	DWORD m_dwSwapTime;
	char  m_szSwapHelp[MAX_PATH];
	HelpText()
	{
		m_dwColor	= 0;
		m_dwSwapTime= 0;
		m_dwSwapCurTIme = 0;
		m_bSwapHelp = false;
		strcpy( m_szHelp, "" );
		strcpy( m_szSwapHelp, "" );
	}

	void SwapProcess()
	{
		if( strcmp( m_szSwapHelp, "" ) == 0 ) return;

		if( FRAMEGETTIME() - m_dwSwapCurTIme > m_dwSwapTime )
		{
			m_dwSwapCurTIme = FRAMEGETTIME();
			m_bSwapHelp = !m_bSwapHelp;
		}
	}
};
typedef std::vector< HelpText > vHelpText;

struct ModeHelp
{
	DWORD m_dwID;
	int m_iType;
	int m_iDuration;
	ioHashString m_szIconName;

	vHelpText m_vText;

	ModeHelp()
	{
		m_dwID = 0;
		m_iType = 0;
		m_iDuration  = 0;

		m_vText.clear();
	}
};
typedef std::vector< ModeHelp > vModeHelpList;

class ioPlayMode;
class ioBaseChar;

class ioModeHelpManager : public Singleton< ioModeHelpManager >
{	
public:
	enum HelpType
	{
		HT_NONE					= 0,
		HT_VIEW					= 1,
		HT_REVIVING				= 2,
		HT_PICK_ITEM			= 3,
		HT_ICE_BLOCK			= 4,
		HT_SYMBOL_ATTACK		= 5,
		HT_START_BLOCK			= 6,
		HT_TEAM_SYMBOL_ATTACK	= 7,
		HT_NEED_GAUGE_W			= 8,
		HT_NEED_GAUGE_A			= 9,
		HT_NEED_GAUGE_H			= 10,
		HT_NEED_GAUGE_C			= 11,
		HT_FAST_WAKEUP			= 12,
		HT_DROP_DAMAGE			= 13,
		HT_SKILL				= 14,
		HT_FIRE_RES				= 15,
		HT_ICE_RES				= 16,
		HT_STOP_GAUGE1			= 17,
		HT_DIE					= 18,
		HT_DEATH_TIME			= 19,
		HT_BUFF					= 20,
		HT_STOP_GAUGE2			= 21,
		HT_CHAT_MODE			= 22,
		HT_CHANGE_CHAT			= 23,
		HT_CHANGE_NORMAL		= 24,
		HT_CONTRIBUTE_PRAISE	= 25,
		HT_SKILL_BLOCK			= 26,
		HT_MINER_BLOCK			= 27,
		HT_BUFF_BLOCK			= 28,
		HT_GETUP_BLOCK			= 29,
		HT_REVIVAL_BLOCK		= 30,
		HT_DEFENSE_BREAK		= 31,
		HT_NETWORK_BAD			= 32,
		HT_NETWORK_DISCONNECT	= 33,
		HT_EXPERIENCE_MODE		= 34,
		HT_CHANGE_EXPERIENCE	= 35,
		HT_NO_INPUT				= 36,
		HT_ROULETTE_START		= 37,
		HT_BLOCK_INTERPLAY		= 38,
		HT_MAX,
	};

protected:
	vModeHelpList m_vModeHelpList;
	HelpType m_eCurHelpType;	
	DWORD m_dwHideHelpTime;
	DWORD m_dwReserveRevivalEndTime;

protected:
	void ProcessHelpText();
	HelpType GetCurHelpType() const;
	const vHelpText *GetCurHelpText() const;	
	const ioHashString *GetCurIconName( ) const;
	int GetDuration( HelpType eHelpType );
	DWORD GetHideHelpTime() const { return m_dwHideHelpTime; }

	void SetHelpList( HelpType eHelpType,const ioHashString &rszText1,const ioHashString &rszText2,const ioHashString &rszIconName);
	void SetHelpList( HelpType eHelpType, const ioHashString &rszText2 );

	void ShowHelpWnd();
	void HideHelpWnd();
	void SetHidingAni();
	bool IsHidedAni();
	bool IsHidingAni();
	bool ShowHelpWhenHided( DWORD dwCurTime, const ioPlayMode *pPlayMode );

public:
	const vHelpText *GetHelpText( HelpType eHelpTye ) const;
	const ioHashString *GetIconName( HelpType eHelpType ) const;

public:
	void LoadModeHelpList();
	void InitModeHelp();
	void Process( const ioPlayMode *pPlayMode );
	void ShowHelp( HelpType eHelpType, DWORD dwHideHelpTime = 0 );
	void ShowContributePraise( int iContribute );
	void ShowSkillHelp( const ioHashString &rszDescName, const ioHashString &rszDesc, const ioHashString &rszIconName );
	void HideHelp( HelpType eHelpType );
	void HideHelpDirectly();

public:
	static ioModeHelpManager &GetSingleton();
	static ioModeHelpManager *GetSingletonPtr();

public:
	ioModeHelpManager();
	virtual ~ioModeHelpManager();
};

#define g_ModeHelpMgr ioModeHelpManager::GetSingleton()

