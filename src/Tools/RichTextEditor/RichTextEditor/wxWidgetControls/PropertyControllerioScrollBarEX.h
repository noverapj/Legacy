
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioScrollBarEX.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOSCROLLBAREX_H_
#define _PROPERTYCONTROLLERIOSCROLLBAREX_H_

#include "PropertyControllerioFrameWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioScrollBarEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioScrollBarEX : public PropertyControllerioFrameWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	PropertyControllerioScrollBarEX() {}
	virtual ~PropertyControllerioScrollBarEX() {}

	//----------------------------------------------------------------------------------
	/*
	Override
	*/
	virtual void		Setup( wxioPropertyGrid* grid );
	virtual void		SetupEventTab( wxioPropertyGrid* grid );
	virtual void		Sync( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		SyncEventTab( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
	virtual void		OnChangeEventTab( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
};

#endif // _PROPERTYCONTROLLERIOSCROLLBAREX_H_