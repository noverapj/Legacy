#pragma once
#include "QuickLobbyMenuBase.h"
#include "QuickLobbyTournamentBase.h"

#include "../io3DEngine/ioButtonWndEX.h"


class ioShuffleButtonEx : public ioImageButtonWndEX
{
public:
	ioShuffleButtonEx();
	virtual ~ioShuffleButtonEx();

public:
	virtual void OnRender();
};

//----------------------------------------------------------------------------------------------------------

class QuickLobbyMenu : public QuickLobbyMenuBase
{
protected:
	DWORD m_dwCurrMenu;
	DWORD m_dwStartTime;
	int m_iMyCampPos;

	bool m_bCurrDisableTournament;

	int m_iDefaultX;
	int m_iDefaultY;

public:	
	virtual void iwm_create();
	virtual void iwm_show();
	virtual bool iwm_esc();

public:
	virtual void OnBattleLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual void OnHeadQuartersLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual void OnPlazaGOLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual void OnPlazaLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual void OnShuffleLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual void OnTournamentLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual void OnTradeLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event );

public:
	void OnCampLButtonUP( bool breShow = false );
	void OnTournamentMainLButtonUP();
	void SetTournamentLink( DWORD dwTourIndex );
	void SetDisableTournament( bool bDisable );
	void RefreshTournamentTab();

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void OnProcess( float fTimePerSec );

public:
	QuickLobbyMenu();
	~QuickLobbyMenu();
};

//----------------------------------------------------------------------------------------------------------

class QuickLobbyTournament : public QuickLobbyTournamentBase
{
protected:
	DWORD m_dwCurrActive;
	std::list< ioWnd* > m_ExceptionList;

public:
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_show();
	virtual void OnProcess( float fTimePerSec );

public:
	void AddExceptionList( ioWnd* pWnd );

public:
	virtual void OnCampMainLButtonUp( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual void OnTournamentLButtonUp( ioWndEX* wnd, ioWndEXMouseEvent& event );

public:
	void SetDisableTournament( bool bDisable );

public:
	QuickLobbyTournament();
	~QuickLobbyTournament();
};

//----------------------------------------------------------------------------------------------------------