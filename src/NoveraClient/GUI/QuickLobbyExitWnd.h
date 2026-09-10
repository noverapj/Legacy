#pragma once
#include "QuickLobbyExitWndBase.h"

class QuickLobbyExitWnd : public QuickLobbyExitWndBase
{
public:
	virtual void iwm_create();
	virtual void iwm_show();

public:
	virtual void OnExitButtonDown( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual void OnGameExitBtnUp( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual void OnOptionBtnUp( ioWndEX* wnd, ioWndEXMouseEvent& event );
	virtual void OnPlazaBackBtnUp( ioWndEX* wnd, ioWndEXMouseEvent& event );

public:
	QuickLobbyExitWnd();
	~QuickLobbyExitWnd();
};