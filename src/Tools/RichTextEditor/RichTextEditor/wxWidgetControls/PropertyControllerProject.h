
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerProject.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERPROJECT_H_
#define _PROPERTYCONTROLLERPROJECT_H_

#include "PropertyControllerioWnd.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerProject
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerProject : public PropertyControllerioWnd
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	PropertyControllerProject() {}
	virtual ~PropertyControllerProject() {}

	//----------------------------------------------------------------------------------
	/*
	Override
	*/
	virtual void		Setup( wxioPropertyGrid* grid );
	virtual void		Sync( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
};

#endif // _PROPERTYCONTROLLERPROJECT_H_