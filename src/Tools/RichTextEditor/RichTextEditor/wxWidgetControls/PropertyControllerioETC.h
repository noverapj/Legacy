
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioETC.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOETC_H_
#define _PROPERTYCONTROLLERIOETC_H_

#include "PropertyControllerioWnd.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioProgressBar
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioProgressBar : public PropertyControllerioWnd
{
public:

	//----------------------------------------------------------------------------------
	/*
	持失 / 社瑚
	*/
	PropertyControllerioProgressBar() {}
	virtual ~PropertyControllerioProgressBar() {}

	//----------------------------------------------------------------------------------
	/*
	Override
	*/
	virtual void		Setup( wxioPropertyGrid* grid );
	virtual void		Sync( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioEdit
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioEdit : public PropertyControllerioWnd
{
public:

	//----------------------------------------------------------------------------------
	/*
	持失 / 社瑚
	*/
	PropertyControllerioEdit() {}
	virtual ~PropertyControllerioEdit() {}
};

#endif // _PROPERTYCONTROLLERIOETC_H_