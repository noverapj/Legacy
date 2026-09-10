#pragma once
#include "QuickLobbyWndBase.h"

class QuickLobbyWnd : public QuickLobbyWndBase
{
public:
	enum
	{		
		NONE	        = 0,
		TOUR_MENU		= 1,
		//기존 로비 윈도우들을 활용
		//인포 윈도우
		CAMP_INFO       = 3,
		TOUR_INFO		= 7,
		TRADE_INFO	    = 8,

		//매인 윈도우
		BATTLE          = 10,
		CAMP            = 11,
		PLAZA           = 12,
		TRADE           = 15,
		TOURNAMENT      = 16,
	};

	enum
	{
		DEFALUT_X = 442,
	};

protected:
	DWORD	m_dwCurrMenu;

	ioWnd*	m_pModeHelpTooltip;
	ioWnd*	m_pQuickJoinTooltip;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual bool IsEXType() const { return false; }
	__forceinline DWORD GetCurrMenu(){ return m_dwCurrMenu; }
	__forceinline void  ResetCurrMenu(){ m_dwCurrMenu = NONE; }

public:
	virtual void OnExitButtonDown( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual bool iwm_esc();

public:
	void SetModeHelpTooltip( ioWnd *pCallWnd, const ioHashString &szTitle, int iQuickstartModeHelpIndex );
	void SetQuickJoinTooltip( ioWnd *pCallWnd, const int nMode );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	void ShowMenu( DWORD dwWndID );
	void ShowInfo( DWORD dwWndID, bool bReCalcPos = false );
	void HideInfo( DWORD dwWndID );
	void CalcSize( DWORD dwWndID );
	void SeTitle( DWORD dwWndID );
	void OnCreateHeadquarters();
	void Layout( int iWidth, int iHeight );
	
public:	
	void ApplyTournamentList( SP2Packet &rkPacket );
	void ApplyTournamentConfirmCheck( SP2Packet &rkPacket );
	void ApplyTournamentConfirmRequest( SP2Packet &rkPacket );
	void ApplyNormalTeamList( SP2Packet &rkPacket );
	void RefreshBattleList( DWORD dwIndex = 0 );
	void ApplyLadderTeamList( SP2Packet &rkPacket );
	void RefreshTradeItemList();	
	void ApplyPlazaList( SP2Packet &rkPacket );
	void SetTournamentLink( DWORD dwTourIndex );

	bool QuestGuide( int iModeSelectIndex, float &rfXPos, float &rfYPos );

	void RefreshCampInfo( int iPrevMyCampPos );

public:
	QuickLobbyWnd();
	~QuickLobbyWnd();
};

//----------------------------------------------------------------------------------------------------------
class LobbyOwnerTradeListWnd;
class QuickLobbyTradeInfoWnd : public ioWnd
{
protected:
	LobbyOwnerTradeListWnd* m_pInfoWnd;

public:
	virtual void iwm_create();	
	virtual void iwm_hide();
	virtual void iwm_show();
	
public:
	void OnRefreshTradeItemList();

public:
	QuickLobbyTradeInfoWnd();
	~QuickLobbyTradeInfoWnd();
};