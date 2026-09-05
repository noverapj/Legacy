
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioRichLabel.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIORICHLABEL_H_
#define _PROPERTYCONTROLLERIORICHLABEL_H_

#include "PropertyControllerioWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioRichLabel
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioRichLabel : public PropertyControllerioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	PropertyControllerioRichLabel() {}
	virtual ~PropertyControllerioRichLabel() {}

	//----------------------------------------------------------------------------------
	/*
	Override
	*/
	virtual void		Setup( wxioPropertyGrid* grid );
	virtual void		Sync( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
};

#endif // _PROPERTYCONTROLLERIORICHLABEL_H_