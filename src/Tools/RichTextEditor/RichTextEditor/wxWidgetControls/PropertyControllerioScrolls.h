
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioScrolls.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOSCROLLS_H_
#define _PROPERTYCONTROLLERIOSCROLLS_H_

#include "PropertyControllerioWnd.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioScroll
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioScroll : public PropertyControllerioWnd
{
public:

	//----------------------------------------------------------------------------------
	/*
	持失 / 社瑚
	*/
	PropertyControllerioScroll() {}
	virtual ~PropertyControllerioScroll() {}

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
PropertyControllerioSlide
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioSlide : public PropertyControllerioWnd
{
public:

	//----------------------------------------------------------------------------------
	/*
	持失 / 社瑚
	*/
	PropertyControllerioSlide() {}
	virtual ~PropertyControllerioSlide() {}
};

#endif // _PROPERTYCONTROLLERIOSCROLLS_H_