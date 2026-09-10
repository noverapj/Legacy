//----------------------------------------------------------------------------------
/*
Filename	: D:\LostSaga_Dev2\Work\QuickLobbyTournamentBase.h
Author		: UITool
Date		: 114.1.7_9:42:16

*/
//----------------------------------------------------------------------------------

#ifndef _QUICKLOBBYTOURNAMENTBASE_
#define _QUICKLOBBYTOURNAMENTBASE_

#include "ioSP2GUIManager.h"
#include "ioImageWndEX.h"



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
QuickLobbyTournamentBase
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class QuickLobbyTournamentBase : public ioImageWndEX
{
protected:

	enum
	{
		ID_CAMP_BTN       = 2,
		ID_TOURNAMENT_BTN = 3,
		CAMP_PROGRESS     = 4,
		CAMP_UNPROGRESS   = 5,
		TOUR_REST         = 6,
		TOUR_APP          = 7,
		TOUR_CHEER        = 8,
		TOUR_BATTLE       = 9,
	};

public:

	//----------------------------------------------------------------------------------
	/*
	积己 / 家戈
	*/
	QuickLobbyTournamentBase() : ioImageWndEX( g_GUIMgr.GetSingletonPtr() )
	{
	}

	virtual ~QuickLobbyTournamentBase()
	{
	}

	//----------------------------------------------------------------------------------
	/*
	楷搬 / 楷搬 产扁
	*/
	virtual void	iwm_create()
	{
		ioWndEX::iwm_create();



		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(2) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyTournamentBase >( this, &QuickLobbyTournamentBase::OnCampMainLButtonUp ) );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(3) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyTournamentBase >( this, &QuickLobbyTournamentBase::OnTournamentLButtonUp ) );
				}
		}

	}

	virtual void	iwm_destroy()
	{
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(2) );
				if ( child ) {
					child->Disconnect( static_cast<WNDEX_EVENT_TYPE>(7), this );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(3) );
				if ( child ) {
					child->Disconnect( static_cast<WNDEX_EVENT_TYPE>(7), this );
				}
		}


		ioWndEX::iwm_destroy();
	}

	//----------------------------------------------------------------------------------
	/*
	Event Virtual Functions
	*/
	virtual void OnCampMainLButtonUp( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}
	virtual void OnTournamentLButtonUp( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}


protected:


};


#endif // _QUICKLOBBYTOURNAMENTBASE_


