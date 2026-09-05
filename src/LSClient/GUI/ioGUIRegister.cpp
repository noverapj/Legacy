//----------------------------------------------------------------------------------
/*
Filename	: ioGUIRegister.cpp
Author		: megamegy
Date		: 2013.10.16
Desc		: 
*/
//----------------------------------------------------------------------------------
#include "stdafx.h"
#include "ioGUIRegister.h"
#include "ioSP2GUIManager.h"
#include "QuickLobbyMenu.h"
#include "QuickLobbyWnd.h"
#include "QuickLobbyExitWnd.h"

//----------------------------------------------------------------------------------
void	ioGUIRegister::Register( ioSP2GUIManager* guimgr )
{
	if( !guimgr )
		return;

	guimgr->RegisterWndCreator<QuickLobbyMenu>( "QuickLobbyMenu" );
	guimgr->RegisterWndCreator<QuickLobbyWnd>( "QuickLobbyWnd" );
	guimgr->RegisterWndCreator<QuickLobbyTournament>( "QuickLobbyTournament" );
	guimgr->RegisterWndCreator<QuickLobbyTradeInfoWnd>( "QuickLobbyTradeInfoWnd" );
	guimgr->RegisterWndCreator<ioShuffleButtonEx>( "ioShuffleButtonEx" );
	guimgr->RegisterWndCreator<QuickLobbyExitWnd>( "QuickLobbyExitWnd" );
}