//----------------------------------------------------------------------------------
/*
Filename	: D:\LostSaga_Alpha1\Work\quicklobbywndBase.h
Author		: UITool
Date		: 114.1.25_15:3:3

*/
//----------------------------------------------------------------------------------

#ifndef _QUICKLOBBYWNDBASE_

#define _QUICKLOBBYWNDBASE_


#include "ioMovingWnd.h"



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
QuickLobbyWndBase
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class QuickLobbyWndBase : public ioMovingWndEX
{
protected:

	enum
	{
		EXIT_BTN = 1,
		ID_MODE_HELP_TOOLTIP = 500,
		ID_QUICKJOIN_TOOLTIP = 501,

	};

public:

	//----------------------------------------------------------------------------------
	/*
	积己 / 家戈
	*/
	QuickLobbyWndBase() : ioMovingWndEX() 
	{

	}

	virtual ~QuickLobbyWndBase() 
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
					child->Connect( static_cast<WNDEX_EVENT_TYPE>(7), new ioWndEXMouseEventHandlerEX< QuickLobbyWndBase >( this, &QuickLobbyWndBase::OnExitButtonDown ) );
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


		ioWndEX::iwm_destroy();
	}

	//----------------------------------------------------------------------------------
	/*
	Event Virtual Functions
	*/
	virtual void OnExitButtonDown( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}


protected:


};


#endif // _QUICKLOBBYWNDBASE_


