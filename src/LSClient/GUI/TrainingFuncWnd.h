#pragma once

class TrainingFuncWnd : public ioMovingWnd
{
public:
	enum
	{		
		NONE		        = 0,
		ID_EXIT			    = 1,
		ID_TRADE_INFO	    = 2,

		//매인 윈도우
		ID_BATTLE          = 10,
		ID_PLAZA           = 12,
		ID_TRADE           = 15,

		ID_MODE_HELP_TOOLTIP = 500,
		ID_QUICKJOIN_TOOLTIP = 501,
	};

	enum
	{
		DEFALUT_X = 442,
	};

	enum FuncMsgType
	{
		FMT_NONE,
		FMT_HQ,
		FMT_HOME,
		FMT_GUILD_HQ,
	};

protected:
	DWORD		m_dwCurrMenu;

	ioWnd*		m_pModeHelpTooltip;
	ioWnd*		m_pQuickJoinTooltip;
	
	FuncMsgType	m_FuncMsgType;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetModeHelpTooltip( ioWnd *pCallWnd, const ioHashString &szTitle, int iQuickstartModeHelpIndex );
	void SetQuickJoinTooltip( ioWnd *pCallWnd, const int nMode );

	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	DWORD GetCurrMenu(){ return m_dwCurrMenu; }
	void ResetCurrMenu(){ m_dwCurrMenu = NONE; }

	void ShowMenu( DWORD dwWndID );
	void ShowInfo( DWORD dwWndID, bool bReCalcPos = false );
	void SetTitle( DWORD dwWndID );
	void CalcSize( DWORD dwWndID );
	void Layout( int iWidth, int iHeight );

public:
	virtual void OnProcess( float fTimePerSec );

public:	
	void RefreshBattleList( DWORD dwIndex = 0 );
	void ApplyNormalTeamList( SP2Packet &rkPacket );

	void ApplyPlazaList( SP2Packet &rkPacket );

	void RefreshTradeItemList();

	void ShowHeadquartersMsg();
	void ShowHomeMsg();
	void ShowGuildHQMsg();

	void OnCreateHeadquarters();
	void OnCreateHouse();
	void OnCreateGuildHeadquarters();

	bool QuestGuide( int iModeSelectIndex, float &rfXPos, float &rfYPos );

public:
	TrainingFuncWnd();
	~TrainingFuncWnd();
};