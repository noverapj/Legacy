
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioButtonEX.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _PROPERTYCONTROLLERIOBUTTONEX_H_
#define _PROPERTYCONTROLLERIOBUTTONEX_H_

#include "PropertyControllerioWndEX.h"
#include "../Dialogs/wxRenderElementEditor.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
편의를 위한 매크로들
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
#define SYNC_IMAGE( grid, wnd, tagname ) \
	grid->SetValue<const char*>( wxString::Format( "%sFileName", #tagname ).c_str(), wnd->GetFileName##tagname() );\
	grid->SetValue<const char*>( wxString::Format( "%sImageSetName", #tagname ).c_str(), wnd->GetImageSetName##tagname() );\
	grid->SetValue<const char*>( wxString::Format( "%sImageName", #tagname ).c_str(), wnd->GetImageName##tagname() );\
	grid->SetValue<int>( wxString::Format( "%sOffsetX", #tagname ).c_str(), wnd->GetOffsetX##tagname() );\
	grid->SetValue<int>( wxString::Format( "%sOffsetY", #tagname ).c_str(), wnd->GetOffsetY##tagname() );\
	grid->SetValue<int>( wxString::Format( "%sRenderType", #tagname ).c_str(), wnd->GetRenderType##tagname() );\
	grid->SetValue<int>( wxString::Format( "%sGrayRenderType", #tagname ).c_str(), wnd->GetGrayRenderType##tagname() );\
	grid->SetValue<int>( wxString::Format( "%sTextureFilterOption", #tagname ).c_str(), wnd->GetTextureFilterOption##tagname() );\
	grid->SetValue<int>( wxString::Format( "%sReverseFormat", #tagname ).c_str(), wnd->GetReverseFormatting##tagname() );\
	grid->SetColorValue( wxString::Format( "%sColor", #tagname ).c_str(), ConvertToToolColor( wnd->GetColor##tagname() ) );

#define CHECK_IMAGE( grid, outwnd, tagname ) \
	{\
		wxString filename = wxString::Format( "%sFileName", #tagname );\
		wxString imagesetname = wxString::Format( "%sImageSetName", #tagname );\
		wxString imagename = wxString::Format( "%sImageName", #tagname );\
		if ( keyname == filename )\
		{\
			wxPGProperty* pt = 0;\
			pt = grid->GetProperty( imagesetname ); pt->SetValue( "" );\
			pt = grid->GetProperty( imagename ); pt->SetValue( "" );\
			outwnd->SetFileName##tagname( event.GetValue().GetString().c_str(), guimgr );\
		}\
		else if ( keyname == imagesetname )\
		{\
			wxPGProperty* pt = grid->GetProperty( imagename );\
			outwnd->SetImageSet##tagname( event.GetValue().GetString().c_str(), pt->GetValue().GetString().c_str(), guimgr );\
			pt = grid->GetProperty( filename );\
			pt->SetValue( "" );\
			grid->AddIgnoreSet( imagesetname );\
			grid->AddIgnoreSet( imagename );\
		}\
		else if ( keyname == imagename )\
		{\
			wxPGProperty* pt = grid->GetProperty( imagesetname );\
			outwnd->SetImageSet##tagname( pt->GetValue().GetString().c_str(), event.GetValue().GetString().c_str(), guimgr );\
			pt = grid->GetProperty( filename );\
			pt->SetValue( "" );\
			grid->AddIgnoreSet( imagesetname );\
			grid->AddIgnoreSet( imagename );\
		}\
		else\
		{\
			if ( keyname == wxString::Format( "%sOffsetX", #tagname ) ) {\
				outwnd->SetOffsetX##tagname( event.GetValue().GetInteger() );\
			} else if ( keyname == wxString::Format( "%sOffsetY", #tagname ) ) {\
				outwnd->SetOffsetY##tagname( event.GetValue().GetInteger() );\
			} else if ( keyname == wxString::Format( "%sRenderType", #tagname ) ) {\
				outwnd->SetRenderType##tagname( static_cast< UIRenderType >( event.GetValue().GetInteger() ) );\
			} else if ( keyname == wxString::Format( "%sGrayRenderType", #tagname ) ) {\
				outwnd->SetGrayRenderType##tagname( static_cast< UIRenderType >( event.GetValue().GetInteger() ) );\
			} else if ( keyname == wxString::Format( "%sTextureFilterOption", #tagname ) ) {\
				outwnd->SetTextureFilterOption##tagname( static_cast< TextureFilterOption >( event.GetValue().GetInteger() ) );\
			} else if ( keyname == wxString::Format( "%sReverseFormat", #tagname ) ) {\
				outwnd->SetReverseFormatting##tagname( static_cast< ioUIRenderImage::ReverseFormatting >( event.GetValue().GetInteger() ) );\
			} else if ( keyname == wxString::Format( "%sColor", #tagname ) ) {\
				outwnd->SetColor##tagname( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );\
			}\
		}\
	}

#define SET_IMAGESETNAME( wnd, tagname ) \
	else if ( strcmp( #tagname, static_cast<char*>(userdata) ) == 0 ) {\
		wnd->SetImageSet##tagname( imagesetname, imagename, guimgr );\
	}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class PropertyControllerioButtonEX : public PropertyControllerioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	PropertyControllerioButtonEX() {}
	virtual ~PropertyControllerioButtonEX() {}

	//----------------------------------------------------------------------------------
	/*
	Override
	*/
	virtual void		Setup( wxioPropertyGrid* grid );
	virtual void		Sync( wxioPropertyGrid* grid, ioWnd* wnd );
	virtual void		OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );

	//----------------------------------------------------------------------------------
	/*
	virtual
	*/
	virtual void		SetupPrev( wxioPropertyGrid* grid, wxPGProperty* category ) {}
	virtual void		SetupPost( wxioPropertyGrid* grid, wxPGProperty* category ) {}
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioFrameButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioFrameButtonWndEX;
class PropertyControllerioFrameButtonEX : public PropertyControllerioButtonEX, public FrameImageSelectorUser
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	PropertyControllerioFrameButtonEX() {}
	virtual ~PropertyControllerioFrameButtonEX() {}

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
	virtual void		OnSelectFrameName( const char* name, void* userdata );
	virtual void		OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata ) {}

	//----------------------------------------------------------------------------------
	/*
	편의
	*/
	static void			SetupFrame( FrameImageSelectorUser* seletor, wxioPropertyGrid* grid, wxPGProperty* parent, char* tagname, bool expand );
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioImageButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioImageButtonWndEX;
class PropertyControllerioImageButtonEX : public PropertyControllerioButtonEX, public FrameImageSelectorUser
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	PropertyControllerioImageButtonEX() {}
	virtual ~PropertyControllerioImageButtonEX() {}

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
	virtual void		OnSelectFrameName( const char* name, void* userdata ) {}
	virtual void		OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata );

	//----------------------------------------------------------------------------------
	/*
	편의
	*/
	static void			SetupImage( FrameImageSelectorUser* seletor, wxioPropertyGrid* grid, wxPGProperty* parent, char* tagname, bool expand );
};


#endif // _PROPERTYCONTROLLERIOBUTTONEX_H_