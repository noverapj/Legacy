
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioImageWndEX.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOIMAGEWNDEX_H_
#define _PROPERTYCONTROLLERIOIMAGEWNDEX_H_

#include "PropertyControllerioWndEX.h"
#include "../Dialogs/wxRenderElementEditor.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioImageWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioImageWndEX : public PropertyControllerioWndEX, public FrameImageSelectorUser
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	PropertyControllerioImageWndEX() {}
	virtual ~PropertyControllerioImageWndEX() {}

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
	virtual void		OnSelectFrameName( const char* name, void* userdata ) {}
	virtual void		OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata );
};

#endif // _PROPERTYCONTROLLERIOIMAGEWNDEX_H_