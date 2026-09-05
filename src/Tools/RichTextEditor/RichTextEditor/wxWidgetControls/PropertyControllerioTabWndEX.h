
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioTabWndEX.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOTABWNDEX_H_
#define _PROPERTYCONTROLLERIOTABWNDEX_H_

#include "PropertyControllerioWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioTabWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioTabWndEX : public PropertyControllerioWndEX, public FrameImageSelectorUser
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	PropertyControllerioTabWndEX() {}
	virtual ~PropertyControllerioTabWndEX() {}

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

	//----------------------------------------------------------------------------------
	/*
	FrameImageSelectorUser override
	*/
	virtual void		OnSelectFrameName( const char* name, void* userdata ) {}
	virtual void		OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata );
};

#endif // _PROPERTYCONTROLLERIOTABWNDEX_H_