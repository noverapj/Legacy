
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioCheckBoxEX.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOCHECKBOXEX_H_
#define _PROPERTYCONTROLLERIOCHECKBOXEX_H_

#include "PropertyControllerioButtonEX.h"
#include "../Dialogs/wxRenderElementEditor.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioCheckBoxEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioCheckBoxEX : public PropertyControllerioImageButtonEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	PropertyControllerioCheckBoxEX() {}
	virtual ~PropertyControllerioCheckBoxEX() {}

	//----------------------------------------------------------------------------------
	/*
	PropertyControllerioButtonEX override
	*/
	virtual void		SetupPrev( wxioPropertyGrid* grid, wxPGProperty* category );

	//----------------------------------------------------------------------------------
	/*
	Override
	*/
	virtual void		Sync( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
	
	//----------------------------------------------------------------------------------
	/*
	FrameImageSelectorUser override
	*/
	virtual void		OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata );
};


#endif // _PROPERTYCONTROLLERIOCHECKBOXEX_H_