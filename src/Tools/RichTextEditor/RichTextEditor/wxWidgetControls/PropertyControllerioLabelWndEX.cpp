
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioLabelWndEX.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioLabelWndEX.h"
#include "../utils/GlobalRef.h"
#include "ioLabelWndEX.h"
#include "../wxRichTextEditorFrame.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioLabelWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioLabelWndEX::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioWndEX::Setup( grid );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "LabelWnd" ) );
	{
		grid->AppendIn( categoryWnd, new wxStringProperty( "Label_Text" ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "Label_OffsetX" ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "Label_OffsetY" ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "Label_AlignStyle", *wxPGProperty::sm_wxPG_LABEL, GetAlignStyleString(), GetAlignStyleInt() ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "Label_Size" ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "Label_Gap" ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "Label_Style", *wxPGProperty::sm_wxPG_LABEL, GetTextStyleString(), GetTextStyleInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "Label_DisableStyle", *wxPGProperty::sm_wxPG_LABEL, GetTextStyleString(), GetTextStyleInt() ) );		
		grid->AppendIn( categoryWnd, new wxEnumProperty( "Label_VertAlign", *wxPGProperty::sm_wxPG_LABEL, GetVertAlignString(), GetVertAlignInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "Label_HorzAlign", *wxPGProperty::sm_wxPG_LABEL, GetHorzAlignString(), GetHorzAlignInt() ) );		
		grid->AppendIn( categoryWnd, new wxColourProperty( "Label_Color", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( categoryWnd, new wxColourProperty( "Label_BKColor", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( categoryWnd, new wxBoolProperty( "Label_IsCutPrint" ) );
		grid->AppendIn( categoryWnd, new wxFloatProperty( "Label_PrintWidthCutSize" ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "Label_Alpha" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioLabelWndEX::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioLabelWndEX* labelwnd = dynamic_cast<ioLabelWndEX*>( wnd );
	if ( !grid || !labelwnd ) {
		return;
	}

	PropertyControllerioWndEX::Sync( grid, labelwnd );

	grid->SetValue<const char*>( "Label_Text", labelwnd->GetLabelText() );
	grid->SetValue<int>( "Label_OffsetX", labelwnd->GetLabelOffsetX() );
	grid->SetValue<int>( "Label_OffsetY", labelwnd->GetLabelOffsetY() );
	grid->SetValue<int>( "Label_AlignStyle", labelwnd->GetAlignStyle() );
	grid->SetValue<int>( "Label_Size", labelwnd->GetLabelTextSize() );
	grid->SetValue<int>( "Label_Gap", labelwnd->GetLabelTextGap() );
	grid->SetValue<int>( "Label_Style", labelwnd->GetLabelTextStyle() );
	grid->SetValue<int>( "Label_DisableStyle", labelwnd->GetLabelTextDisableStyle() );
	grid->SetValue<int>( "Label_VertAlign", labelwnd->GetLabelVertAlign() );
	grid->SetValue<int>( "Label_HorzAlign", labelwnd->GetLabelHorzAlign() );
	grid->SetColorValue( "Label_Color", ConvertToToolColor( labelwnd->GetLabelTextColor() ) );
	grid->SetColorValue( "Label_BKColor", ConvertToToolColor( labelwnd->GetLabelTextBKColor() ) );
	grid->SetValue<bool>( "Label_IsCutPrint", ( labelwnd->GetLabelPrintTextStyle() == ioUITitle::PTS_WIDTHCUT ) );
	grid->SetValue<float>( "Label_PrintWidthCutSize", labelwnd->GetLabelPrintWidthCutSize() );
	grid->SetValue<int>( "Label_Alpha", labelwnd->GetLabelTextAlpha() );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioLabelWndEX::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWndEX::OnChange( grid, wnd, event );
	
	ioLabelWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioLabelWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	if ( keyname == "Label_Text" ) {
		outwnd->SetLabelText( event.GetValue().GetString().c_str(), "" );
	} else if ( keyname == "Label_OffsetX" ) {
		outwnd->SetLabelOffsetX( event.GetValue().GetInteger() );
	} else if ( keyname == "Label_OffsetY" ) {
		outwnd->SetLabelOffsetY( event.GetValue().GetInteger() );
	} else if ( keyname == "Label_AlignStyle" ) {
		outwnd->SetAlignStyle( static_cast< ALIGN_STYLE_EX >( event.GetValue().GetInteger() ) );
	} else if ( keyname == "Label_Size" ) {
		outwnd->SetLabelTextSize( event.GetValue().GetInteger() );
	} else if ( keyname == "Label_Gap" ) {
		outwnd->SetLabelTextGap( event.GetValue().GetInteger() );
	} else if ( keyname == "Label_Style" ) {
		outwnd->SetLabelTextStyle( static_cast< TextStyle >( event.GetValue().GetInteger() ) );
	} else if ( keyname == "Label_DisableStyle" ) {
		outwnd->SetLabelTextDisableStyle( static_cast< TextStyle >( event.GetValue().GetInteger() ) );
	} else if ( keyname == "Label_VertAlign" ) {
		outwnd->SetLabelVertAlign( static_cast< TextVertAlign >( event.GetValue().GetInteger() ) );
	} else if ( keyname == "Label_HorzAlign" ) {
		outwnd->SetLabelHorzAlign( static_cast< TextAlignType >( event.GetValue().GetInteger() ) );
	} else if ( keyname == "Label_Color" ) {
		outwnd->SetLabelTextColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "Label_BKColor" ) {
		outwnd->SetLabelTextBKColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "Label_IsCutPrint" ) {
		outwnd->SetLabelPrintTextStyle( ( event.GetValue().GetBool() ? ioUITitle::PTS_WIDTHCUT : ioUITitle::PTS_NORMAL ) );
	} else if ( keyname == "Label_PrintWidthCutSize" ) {
		outwnd->SetLabelPrintWidthCutSize( event.GetValue().GetInteger() );
	} else if ( keyname == "Label_Alpha" ) {
		outwnd->SetLabelTextAlpha( event.GetValue().GetInteger() );
	}
}