
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioButtons.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOBUTTONS_H_
#define _PROPERTYCONTROLLERIOBUTTONS_H_

#include "PropertyControllerioWnd.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioButton
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioButton : public PropertyControllerioWnd
{
public:

	//----------------------------------------------------------------------------------
	/*
	持失 / 社瑚
	*/
	PropertyControllerioButton() {}
	virtual ~PropertyControllerioButton() {}

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
PropertyControllerioRadioButton
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioRadioButton : public PropertyControllerioButton
{
public:

	//----------------------------------------------------------------------------------
	/*
	持失 / 社瑚
	*/
	PropertyControllerioRadioButton() {}
	virtual ~PropertyControllerioRadioButton() {}
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioCheckBox
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioCheckBox : public PropertyControllerioButton
{
public:

	//----------------------------------------------------------------------------------
	/*
	持失 / 社瑚
	*/
	PropertyControllerioCheckBox() {}
	virtual ~PropertyControllerioCheckBox() {}

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
PropertyControllerioActiveScreenButton
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioActiveScreenButton : public PropertyControllerioButton
{
public:

	//----------------------------------------------------------------------------------
	/*
	持失 / 社瑚
	*/
	PropertyControllerioActiveScreenButton() {}
	virtual ~PropertyControllerioActiveScreenButton() {}
};

#endif // _PROPERTYCONTROLLERIOBUTTONS_H_