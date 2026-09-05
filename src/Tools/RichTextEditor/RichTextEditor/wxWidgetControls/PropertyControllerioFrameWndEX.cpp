
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioFrameWndEX.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioFrameWndEX.h"
#include "../utils/GlobalRef.h"
#include "ioFrameWndEX.h"
#include "../wxRichTextEditorFrame.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioFrameWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioFrameWndEX::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioWndEX::Setup( grid );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "FrameWnd" ) );
	{
		grid->AppendIn( categoryWnd, new wxFrameRendererSummonerPropertyEX( this, 0, "Template", "", grid ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "RenderType", *wxPGProperty::sm_wxPG_LABEL, GetRenderTypeString(), GetRenderTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "GrayRenderType", *wxPGProperty::sm_wxPG_LABEL, GetRenderTypeString(), GetRenderTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "TextureFilterOption", *wxPGProperty::sm_wxPG_LABEL, GetTextureFilterOptionTypeString(), GetTextureFilterOptionTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxBoolProperty( "NoMagicAlign" ) );
		grid->AppendIn( categoryWnd, new wxColourProperty( "Color", wxPG_LABEL, *wxWHITE ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioFrameWndEX::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioFrameWndEX* framewnd = dynamic_cast<ioFrameWndEX*>( wnd );
	if ( !grid || !framewnd ) {
		return;
	}

	PropertyControllerioWndEX::Sync( grid, framewnd );

	grid->SetValue<const char*>( "Template", framewnd->GetFrameTemplateName() );
	grid->SetValue<int>( "RenderType", framewnd->GetRenderType() );
	grid->SetValue<int>( "GrayRenderType", framewnd->GetGrayRenderType() );
	grid->SetValue<int>( "TextureFilterOption", framewnd->GetTextureFilterOption() );
	grid->SetValue<int>( "NoMagicAlign", framewnd->GetNoMagicAlign() );
	grid->SetColorValue( "Color", ConvertToToolColor( framewnd->GetColor() ) );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioFrameWndEX::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWndEX::OnChange( grid, wnd, event );
	
	ioFrameWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioFrameWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !guimgr ) {
		return;
	}

	if ( keyname == "Template" ) {
		outwnd->SetFrameTemplateName( event.GetValue().GetString().c_str(), guimgr );
	} else if ( keyname == "RenderType" ) {
		outwnd->SetRenderType( static_cast< UIRenderType >( event.GetValue().GetInteger() ) );
	} else if ( keyname == "GrayRenderType" ) {
		outwnd->SetGrayRenderType( static_cast< UIRenderType >( event.GetValue().GetInteger() ) );
	} else if ( keyname == "TextureFilterOption" ) {
		outwnd->SetTextureFilterOption( static_cast< TextureFilterOption >( event.GetValue().GetInteger() ) );
	} else if ( keyname == "NoMagicAlign" ) {
		outwnd->SetNoMagicAlign( event.GetValue().GetBool() );
	} else if ( keyname == "Color" ) {
		outwnd->SetColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	}
}

//----------------------------------------------------------------------------------
/*
FrameImageSelectorUser override
*/
void	PropertyControllerioFrameWndEX::OnSelectFrameName( const char* name, void* userdata )
{
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	ioFrameWndEX* wnd = dynamic_cast<ioFrameWndEX*>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd() );
	if ( !wnd || !guimgr ) {
		return;
	}

	wnd->SetFrameTemplateName( name, guimgr );	
	wxRichTextEditorFrame::GetSingleton().SyncProperty();
}