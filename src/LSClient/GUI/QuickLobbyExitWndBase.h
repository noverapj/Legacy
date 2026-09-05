//----------------------------------------------------------------------------------
/*
Filename	: D:\LostSaga_Alpha1\Work\QuickLobbyExitWndBase.h
Author		: UITool
Date		: 114.3.2_18:33:42

*/
//----------------------------------------------------------------------------------

#ifndef _QUICKLOBBYEXITWNDBASE_

#define _QUICKLOBBYEXITWNDBASE_


#include "ioWndEX.h"



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
QuickLobbyExitWndBase
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class QuickLobbyExitWndBase : public ioWndEX
{
protected:

	enum
	{
		ALTF4_BTN = 5,
		EXIT_BTN = 1,
		OPTION_BTN = 3,
		PLAZA_BACK_BTN = 4,

	};

public:

	//----------------------------------------------------------------------------------
	/*
	积己 / 家戈
	*/
	QuickLobbyExitWndBase() : ioWndEX() 
	{

	}

	virtual ~QuickLobbyExitWndBase() 
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
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(1) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyExitWndBase >( this, &QuickLobbyExitWndBase::OnExitButtonDown ) );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(3) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyExitWndBase >( this, &QuickLobbyExitWndBase::OnOptionBtnUp ) );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(4) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyExitWndBase >( this, &QuickLobbyExitWndBase::OnPlazaBackBtnUp ) );
				}
		}
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(5) );
				if ( child ) {
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyExitWndBase >( this, &QuickLobbyExitWndBase::OnGameExitBtnUp ) );
				}
		}

	}

	virtual void	iwm_destroy()
	{
		{
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(1) );
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


		ioWndEX::iwm_destroy();
	}

	//----------------------------------------------------------------------------------
	/*
	Event Virtual Functions
	*/
	virtual void OnExitButtonDown( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}
	virtual void OnGameExitBtnUp( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}
	virtual void OnOptionBtnUp( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}
	virtual void OnPlazaBackBtnUp( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}


protected:


};


#endif // _QUICKLOBBYEXITWNDBASE_


