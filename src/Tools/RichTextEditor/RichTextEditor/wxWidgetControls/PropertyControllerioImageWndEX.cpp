
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioImageWndEX.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioImageWndEX.h"
#include "../utils/GlobalRef.h"
#include "ioImageWndEX.h"
#include "../wxRichTextEditorFrame.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioImageWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioImageWndEX::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioWndEX::Setup( grid );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "ImageWnd" ) );
	{
		grid->AppendIn( categoryWnd, new wxStringProperty( "FileName" ) );
		grid->AppendIn( categoryWnd, new wxImageSetRendererSummonerPropertyEX( this, 0, "ImageSetName", "", grid ) );
		grid->AppendIn( categoryWnd, new wxImageSetRendererSummonerPropertyEX( this, 0, "ImageName", "", grid ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "OffsetX" ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "OffsetY" ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "RenderType", *wxPGProperty::sm_wxPG_LABEL, GetRenderTypeString(), GetRenderTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "GrayRenderType", *wxPGProperty::sm_wxPG_LABEL, GetRenderTypeString(), GetRenderTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "TextureFilterOption", *wxPGProperty::sm_wxPG_LABEL, GetTextureFilterOptionTypeString(), GetTextureFilterOptionTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "ReverseFormat", *wxPGProperty::sm_wxPG_LABEL, GetReserveFormatString(), GetReserveFormatInt() ) );
		grid->AppendIn( categoryWnd, new wxBoolProperty( "NoMagicAlign" ) );
		grid->AppendIn( categoryWnd, new wxColourProperty( "Color", wxPG_LABEL, *wxWHITE ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioImageWndEX::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioImageWndEX* imagewnd = dynamic_cast<ioImageWndEX*>( wnd );
	if ( !grid || !imagewnd ) {
		return;
	}

	PropertyControllerioWndEX::Sync( grid, imagewnd );
		
	if ( imagewnd->IsCreationFromFileImage() ) 
	{
		grid->SetValue<const char*>( "FileName", imagewnd->GetFileName() );
		grid->SetValue<const char*>( "ImageSetName", imagewnd->GetImageSetName() );
		grid->SetValue<const char*>( "ImageName", imagewnd->GetImageName() );
	}
	else
	{
		grid->SetValue<const char*>( "FileName", imagewnd->GetFileName() );
		grid->SetValue<const char*>( "ImageSetName", imagewnd->GetImageSetName() );
		grid->SetValue<const char*>( "ImageName", imagewnd->GetImageName() );
	}

	grid->SetValue<int>( "OffsetX", imagewnd->GetOffsetX() );
	grid->SetValue<int>( "OffsetY", imagewnd->GetOffsetY() );
	grid->SetValue<int>( "RenderType", imagewnd->GetRenderType() );
	grid->SetValue<int>( "GrayRenderType", imagewnd->GetGrayRenderType() );
	grid->SetValue<int>( "TextureFilterOption", imagewnd->GetTextureFilterOption() );
	grid->SetValue<int>( "ReverseFormat", imagewnd->GetReverseFormatting() );
	grid->SetValue<int>( "NoMagicAlign", imagewnd->GetNoMagicAlign() );
	grid->SetColorValue( "Color", ConvertToToolColor( imagewnd->GetColor() ) );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioImageWndEX::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWndEX::OnChange( grid, wnd, event );
	
	ioImageWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioImageWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !guimgr ) {
		return;
	}

	if ( keyname == "FileName" )
	{
		wxPGProperty* pt = 0;
		pt = grid->GetProperty( "ImageSetName" );
		pt->SetValue( "" );
		pt = grid->GetProperty( "ImageName" );
		pt->SetValue( "" );

		outwnd->SetFileName( event.GetValue().GetString().c_str(), guimgr );
	}
	else if ( keyname == "ImageSetName" ) 
	{
		wxPGProperty* pt = grid->GetProperty( "ImageName" );
		outwnd->SetImageSet( event.GetValue().GetString().c_str(), pt->GetValue().GetString().c_str(), guimgr );

		pt = grid->GetProperty( "FileName" );
		pt->SetValue( "" );

		grid->AddIgnoreSet( "ImageSetName" );
		grid->AddIgnoreSet( "ImageName" );
	}
	else if ( keyname == "ImageName" )
	{
		wxPGProperty* pt = grid->GetProperty( "ImageSetName" );
		outwnd->SetImageSet( pt->GetValue().GetString().c_str(), event.GetValue().GetString().c_str(), guimgr );

		pt = grid->GetProperty( "FileName" );
		pt->SetValue( "" );

		grid->AddIgnoreSet( "ImageSetName" );
		grid->AddIgnoreSet( "ImageName" );
	}
	else		
	{
		if ( keyname == "OffsetX" ) {
			outwnd->SetOffsetX( event.GetValue().GetInteger() );
		} else if ( keyname == "OffsetY" ) {
			outwnd->SetOffsetY( event.GetValue().GetInteger() );
		} else if ( keyname == "RenderType" ) {
			outwnd->SetRenderType( static_cast< UIRenderType >( event.GetValue().GetInteger() ) );
		} else if ( keyname == "GrayRenderType" ) {
			outwnd->SetGrayRenderType( static_cast< UIRenderType >( event.GetValue().GetInteger() ) );
		} else if ( keyname == "TextureFilterOption" ) {
			outwnd->SetTextureFilterOption( static_cast< TextureFilterOption >( event.GetValue().GetInteger() ) );
		} else if ( keyname == "ReverseFormat" ) {
			outwnd->SetReverseFormatting( static_cast< ioUIRenderImage::ReverseFormatting >( event.GetValue().GetInteger() ) );
		} else if ( keyname == "NoMagicAlign" ) {
			outwnd->SetNoMagicAlign( event.GetValue().GetBool() );
		} else if ( keyname == "Color" ) {
			outwnd->SetColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
		}
	}
}

//----------------------------------------------------------------------------------
/*
FrameImageSelectorUser override
*/
void	PropertyControllerioImageWndEX::OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata )
{
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	ioImageWndEX* wnd = dynamic_cast<ioImageWndEX*>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd() );
	if ( !wnd || !guimgr ) {
		return;
	}

	wnd->SetImageSet( imagesetname, imagename, guimgr );
	wxRichTextEditorFrame::GetSingleton().SyncProperty();
}