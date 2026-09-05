
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioLabelWndEX.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOLABELWNDEX_H_
#define _PROPERTYCONTROLLERIOLABELWNDEX_H_

#include "PropertyControllerioWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioLabelWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioLabelWndEX : public PropertyControllerioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	PropertyControllerioLabelWndEX() {}
	virtual ~PropertyControllerioLabelWndEX() {}

	//----------------------------------------------------------------------------------
	/*
	Override
	*/
	virtual void		Setup( wxioPropertyGrid* grid );
	virtual void		Sync( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
};

#endif // _PROPERTYCONTROLLERIOLABELWNDEX_H_