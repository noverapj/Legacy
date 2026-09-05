
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioRadioWndEX.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIORADIOWNDEX_H_
#define _PROPERTYCONTROLLERIORADIOWNDEX_H_

#include "PropertyControllerioWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioRadioWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioRadioWndEX : public PropertyControllerioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	PropertyControllerioRadioWndEX() {}
	virtual ~PropertyControllerioRadioWndEX() {}

	//----------------------------------------------------------------------------------
	/*
	Override
	*/
	virtual void		SetupEventTab( wxioPropertyGrid* grid );
	virtual void		SyncEventTab( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		OnChangeEventTab( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
};

#endif // _PROPERTYCONTROLLERIORADIOWNDEX_H_