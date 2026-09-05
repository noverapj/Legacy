
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioWnd.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOWND_H_
#define _PROPERTYCONTROLLERIOWND_H_

#include "wxioPropertyGrid.h"
#include "../Dialogs/wxRenderElementEditor.h"
#include "../InputProcessor.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRenderImageSummonerPropertyEX : 이미지렌더 편집창을 위한 property
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class WXDLLIMPEXP_PROPGRID wxRenderImageSummonerPropertyEX : public wxLongStringProperty
{
	 WX_PG_DECLARE_PROPERTY_CLASS( wxRenderImageSummonerPropertyEX )

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
    wxRenderImageSummonerPropertyEX( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString )
		: wxLongStringProperty( label, name, value )		
	{}
	virtual ~wxRenderImageSummonerPropertyEX() {}

	virtual bool OnButtonClick( wxPropertyGrid* propgrid, wxString& value );

protected:

	static wxRenderImageEditor*	m_editor;
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRenderFrameSummonerPropertyEX : 이미지렌더 편집창을 위한 property
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class WXDLLIMPEXP_PROPGRID wxRenderFrameSummonerPropertyEX : public wxLongStringProperty
{
	 WX_PG_DECLARE_PROPERTY_CLASS( wxRenderFrameSummonerPropertyEX )

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	wxRenderFrameSummonerPropertyEX( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString )
		: wxLongStringProperty( label, name, value )		
	{}
	virtual ~wxRenderFrameSummonerPropertyEX() {}

	virtual bool OnButtonClick( wxPropertyGrid* propgrid, wxString& value );

protected:

	static wxRenderFrameEditor*	m_editor;
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioWnd
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioWnd;
class PropertyControllerioWnd
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	PropertyControllerioWnd();
	virtual ~PropertyControllerioWnd();

	//----------------------------------------------------------------------------------
	/*
	Setup
	*/
	virtual void		Setup( wxioPropertyGrid* grid );
	virtual void		SetupEventTab( wxioPropertyGrid* grid ) {}

	//----------------------------------------------------------------------------------
	/*
	Sync
	*/
	virtual void		Sync( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		SyncEventTab( wxioPropertyGrid* grid, ioWnd* wnd ) {}

	//----------------------------------------------------------------------------------
	/*
	OnChange
	*/
	virtual void		OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
	virtual void		OnChangeEventTab( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event ) {}

	//----------------------------------------------------------------------------------
	/*
	Util
	*/
	static void			AppendInBoolPropertyCheckbox( wxioPropertyGrid* grid, wxPGProperty* child, const char* str );

public:

	static wxColour		st_textdefcolor;
	static wxColour		st_backdefcolor;
	static wxColour		st_uniquebackcolor;
};

template < typename T >
bool	GetDatasFor( ioWnd* pwnd, T** wnd, wxString& keyname, wxAny& value, wxPropertyGridEvent& event )
{
	*wnd = dynamic_cast<T*>( pwnd );
	if ( !(*wnd) ) {
		return false;
	}

	wxPGProperty* property = event.GetProperty();
	if ( !property ) {
		return false;
	}

	keyname = property->GetName();
	value = property->GetValue();
	return true;
}

#endif // _PROPERTYCONTROLLERIOWND_H_