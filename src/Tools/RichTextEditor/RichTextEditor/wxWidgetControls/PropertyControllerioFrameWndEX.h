
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioFrameWndEX.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOFRAMEWNDEX_H_
#define _PROPERTYCONTROLLERIOFRAMEWNDEX_H_

#include "PropertyControllerioWndEX.h"
#include "../Dialogs/wxRenderElementEditor.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioFrameWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioFrameWndEX : public PropertyControllerioWndEX, public FrameImageSelectorUser
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	PropertyControllerioFrameWndEX() {}
	virtual ~PropertyControllerioFrameWndEX() {}

	//----------------------------------------------------------------------------------
	/*
	Override
	*/
	virtual void		Setup( wxioPropertyGrid* grid );
	virtual void		Sync( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
	
	//----------------------------------------------------------------------------------
	/*
	FrameImageSelectorUser override
	*/
	virtual void		OnSelectFrameName( const char* name, void* userdata );
	virtual void		OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata ) {}
};

#endif // _PROPERTYCONTROLLERIOFRAMEWNDEX_H_