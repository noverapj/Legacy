//----------------------------------------------------------------------------------
/*
Filename	: D:\LostSaga_Dev2\xml\quicklobbymenuBase.h
Author		: UITool
Date		: 114.3.2_19:27:1

*/
//----------------------------------------------------------------------------------

#ifndef _QUICKLOBBYMENUBASE_

#define _QUICKLOBBYMENUBASE_


#include "ioMovingWnd.h"



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
QuickLobbyMenuBase
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class QuickLobbyMenuBase : public ioMovingWndEX
{
protected:

	enum
	{
		TOURNAMENT_MENU_WND	= 1,
		SHUFFLE_BTN			= 2,
		BATTLE_BTN			= 3,
		TOURNAMENT_BTN		= 4,
		PLAZA_BTN			= 5,
		TRADE_BTN			= 6,
		HEAD_QUATER_BTN		= 7,
		PLAZA_GO_BTN		= 8,
	};

public:

	//----------------------------------------------------------------------------------
	/*
	积己 / 家戈
	*/
	QuickLobbyMenuBase() : ioMovingWndEX() 
	{

	}

	virtual ~QuickLobbyMenuBase() 
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
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyMenuBase >( this, &QuickLobbyMenuBase::OnShuffleLButtonUP ) );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(3) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyMenuBase >( this, &QuickLobbyMenuBase::OnBattleLButtonUP ) );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(4) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyMenuBase >( this, &QuickLobbyMenuBase::OnTournamentLButtonUP ) );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(5) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyMenuBase >( this, &QuickLobbyMenuBase::OnPlazaLButtonUP ) );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(6) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyMenuBase >( this, &QuickLobbyMenuBase::OnTradeLButtonUP ) );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(7) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyMenuBase >( this, &QuickLobbyMenuBase::OnHeadQuartersLButtonUP ) );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(8) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyMenuBase >( this, &QuickLobbyMenuBase::OnPlazaGOLButtonUP ) );
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
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(4) );
				if ( child ) {
					child->Disconnect( static_cast<WNDEX_EVENT_TYPE>(7), this );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(5) );
				if ( child ) {
					child->Disconnect( static_cast<WNDEX_EVENT_TYPE>(7), this );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(6) );
				if ( child ) {
					child->Disconnect( static_cast<WNDEX_EVENT_TYPE>(7), this );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(7) );
				if ( child ) {
					child->Disconnect( static_cast<WNDEX_EVENT_TYPE>(7), this );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(8) );
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
	virtual void OnBattleLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}
	virtual void OnHeadQuartersLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}
	virtual void OnPlazaGOLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}
	virtual void OnPlazaLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}
	virtual void OnShuffleLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}
	virtual void OnTournamentLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}
	virtual void OnTradeLButtonUP( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}


protected:


};


#endif // _QUICKLOBBYMENUBASE_


