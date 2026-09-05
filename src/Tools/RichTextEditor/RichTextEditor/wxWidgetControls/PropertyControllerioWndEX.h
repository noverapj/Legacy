
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioWndEX.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOWNDEX_H_
#define _PROPERTYCONTROLLERIOWNDEX_H_

#include "PropertyControllerioWnd.h"

#define SET_CONNECTFUNCTION_NAME( comp, evt ) else if ( keyname == comp ) { outwnd->SetConnectFunctionName( evt, event.GetValue().GetString().c_str() ); }

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioWndEX : public PropertyControllerioWnd
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	PropertyControllerioWndEX() {}
	virtual ~PropertyControllerioWndEX() {}

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
};

#endif // _PROPERTYCONTROLLERIOWNDEX_H_