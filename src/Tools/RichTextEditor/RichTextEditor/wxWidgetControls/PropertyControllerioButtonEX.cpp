
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioButtonEX.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioButtonEX.h"
#include "../utils/GlobalRef.h"
#include "ioButtonWndEX.h"
#include "../wxRichTextEditorFrame.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
enum { BUTTONEX_TAGNAME_NUM = 8 };
static char* g_tagnames[ BUTTONEX_TAGNAME_NUM ] =
{
	"Normal",
	"Over",
	"OverAdd",
	"Push",
	"Back",
	"Disable",
	"ExDisableNormal",
	"ExDisablePush"
};
void	PropertyControllerioButtonEX::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioWndEX::Setup( grid );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "ButtonEX" ) );
	
	SetupPrev( grid, categoryWnd );

	wxPGProperty* ftcategory = grid->AppendIn( categoryWnd, new wxPropertyCategory( "ButtonText" ) );
	{
		grid->AppendIn( ftcategory, new wxStringProperty( "Label_Text" ) );
		grid->AppendIn( ftcategory, new wxIntProperty( "Label_Size" ) );
		grid->AppendIn( ftcategory, new wxIntProperty( "Label_Gap" ) );
		grid->AppendIn( ftcategory, new wxEnumProperty( "Label_Style", *wxPGProperty::sm_wxPG_LABEL, GetTextStyleString(), GetTextStyleInt() ) );
		grid->AppendIn( ftcategory, new wxEnumProperty( "Label_DisableStyle", *wxPGProperty::sm_wxPG_LABEL, GetTextStyleString(), GetTextStyleInt() ) );		
		grid->AppendIn( ftcategory, new wxEnumProperty( "Label_VertAlign", *wxPGProperty::sm_wxPG_LABEL, GetVertAlignString(), GetVertAlignInt() ) );
		grid->AppendIn( ftcategory, new wxEnumProperty( "Label_HorzAlign", *wxPGProperty::sm_wxPG_LABEL, GetHorzAlignString(), GetHorzAlignInt() ) );		
		grid->AppendIn( ftcategory, new wxBoolProperty( "Label_IsCutPrint" ) );
		grid->AppendIn( ftcategory, new wxFloatProperty( "Label_PrintWidthCutSize" ) );
		grid->AppendIn( ftcategory, new wxIntProperty( "Label_Alpha" ) );
		// state color
		grid->AppendIn( ftcategory, new wxColourProperty( "Label_NormalColor", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( ftcategory, new wxColourProperty( "Label_NormalBKColor", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( ftcategory, new wxColourProperty( "Label_OverColor", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( ftcategory, new wxColourProperty( "Label_OverBKColor", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( ftcategory, new wxColourProperty( "Label_PushColor", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( ftcategory, new wxColourProperty( "Label_PushBKColor", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( ftcategory, new wxColourProperty( "Label_DisableColor", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( ftcategory, new wxColourProperty( "Label_DisableBKColor", wxPG_LABEL, *wxWHITE ) );
	}	
	grid->SetPropertyBackgroundColour( ftcategory, PropertyControllerioWnd::st_backdefcolor );

	wxPGProperty* fecategory = grid->AppendIn( categoryWnd, new wxPropertyCategory( "ButtonETC" ) );
	{
		grid->AppendIn( fecategory, new wxEnumProperty( "ButtonType", *wxPGProperty::sm_wxPG_LABEL, GetButtonTypeString(), GetButtonTypeInt() ) );
		grid->AppendIn( fecategory, new wxStringProperty( "SoundOver" ) );
		grid->AppendIn( fecategory, new wxStringProperty( "SoundClick" ) );
	}	
	grid->SetPropertyBackgroundColour( fecategory, PropertyControllerioWnd::st_backdefcolor );
	grid->Collapse( fecategory );

	SetupPost( grid, categoryWnd );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioButtonEX::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioButtonWndEX* framebtn = dynamic_cast<ioButtonWndEX*>( wnd );
	if ( !grid || !framebtn ) {
		return;
	}

	PropertyControllerioWndEX::Sync( grid, framebtn );
	
	grid->SetValue<const char*>( "Label_Text", framebtn->GetLabelText() );
	grid->SetValue<int>( "Label_Size", framebtn->GetLabelTextSize() );
	grid->SetValue<int>( "Label_Gap", framebtn->GetLabelTextGap() );
	grid->SetValue<int>( "Label_Style", framebtn->GetLabelTextStyle() );
	grid->SetValue<int>( "Label_DisableStyle", framebtn->GetLabelTextDisableStyle() );
	grid->SetValue<int>( "Label_VertAlign", framebtn->GetLabelVertAlign() );
	grid->SetValue<int>( "Label_HorzAlign", framebtn->GetLabelHorzAlign() );	
	grid->SetValue<bool>( "Label_IsCutPrint", ( framebtn->GetLabelPrintTextStyle() == ioUITitle::PTS_WIDTHCUT ) );
	grid->SetValue<float>( "Label_PrintWidthCutSize", framebtn->GetLabelPrintWidthCutSize() );
	grid->SetValue<int>( "Label_Alpha", framebtn->GetLabelTextAlpha() );
	grid->SetColorValue( "Label_NormalColor", ConvertToToolColor( framebtn->GetLabelNormalTextColor() ) );
	grid->SetColorValue( "Label_NormalBKColor", ConvertToToolColor( framebtn->GetLabelNormalTextBKColor() ) );
	grid->SetColorValue( "Label_OverColor", ConvertToToolColor( framebtn->GetLabelOverTextColor() ) );
	grid->SetColorValue( "Label_OverBKColor", ConvertToToolColor( framebtn->GetLabelOverTextBKColor() ) );
	grid->SetColorValue( "Label_PushColor", ConvertToToolColor( framebtn->GetLabelPushTextColor() ) );
	grid->SetColorValue( "Label_PushBKColor", ConvertToToolColor( framebtn->GetLabelPushTextBKColor() ) );
	grid->SetColorValue( "Label_DisableColor", ConvertToToolColor( framebtn->GetLabelDisableTextColor() ) );
	grid->SetColorValue( "Label_DisableBKColor", ConvertToToolColor( framebtn->GetLabelDisableTextBKColor() ) );
	grid->SetValue<int>( "ButtonType", framebtn->GetButtonEXType() );
	grid->SetValue<const char*>( "SoundOver", framebtn->GetSoundOver() );
	grid->SetValue<const char*>( "SoundClick", framebtn->GetSoundClick() );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioButtonEX::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWndEX::OnChange( grid, wnd, event );
	
	ioButtonWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioButtonWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !guimgr ) {
		return;
	}
		
	if ( keyname == "Label_Text" ) {
		outwnd->SetLabelText( event.GetValue().GetString().c_str(), "" );
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
	} else if ( keyname == "Label_IsCutPrint" ) {
		outwnd->SetLabelPrintTextStyle( ( event.GetValue().GetBool() ? ioUITitle::PTS_WIDTHCUT : ioUITitle::PTS_NORMAL ) );
	} else if ( keyname == "Label_PrintWidthCutSize" ) {
		outwnd->SetLabelPrintWidthCutSize( event.GetValue().GetInteger() );
	} else if ( keyname == "Label_Alpha" ) {
		outwnd->SetLabelTextAlpha( event.GetValue().GetInteger() );
	} else if ( keyname == "Label_NormalColor" ) {
		outwnd->SetLabelNormalTextColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "Label_NormalBKColor" ) {
		outwnd->SetLabelNormalTextBKColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "Label_OverColor" ) {
		outwnd->SetLabelOverTextColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "Label_OverBKColor" ) {
		outwnd->SetLabelOverTextBKColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "Label_PushColor" ) {
		outwnd->SetLabelPushTextColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "Label_PushBKColor" ) {
		outwnd->SetLabelPushTextBKColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "Label_DisableColor" ) {
		outwnd->SetLabelDisableTextColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "Label_DisableBKColor" ) {
		outwnd->SetLabelDisableTextBKColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "ButtonType" ) {
		outwnd->SetButtonEXType( static_cast< BUTTONEX_TYPE >( event.GetValue().GetInteger() ) );
	} else if ( keyname == "SoundOver" ) {
		outwnd->SetSoundOver( event.GetValue().GetString().c_str() );
	} else if ( keyname == "SoundClick" ) {
		outwnd->SetSoundClick( event.GetValue().GetString().c_str() );
	}
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioFrameButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
PropertyControllerioButtonEX override
*/
void	PropertyControllerioFrameButtonEX::SetupPrev( wxioPropertyGrid* grid, wxPGProperty* category )
{
	for ( int i = 0; i < BUTTONEX_TAGNAME_NUM; ++i ) {
		SetupFrame( this, grid, category, g_tagnames[ i ], i < 4 );
	}
}

void	PropertyControllerioFrameButtonEX::SetupFrame( FrameImageSelectorUser* seletor, wxioPropertyGrid* grid, wxPGProperty* parent, char* tagname, bool expand )
{
	if ( !grid || !parent || !tagname ) {
		return;
	}

	wxPGProperty* categoryWnd = grid->AppendIn( parent, new wxPropertyCategory( tagname ) );
	{
		grid->AppendIn( categoryWnd, new wxFrameRendererSummonerPropertyEX( seletor, static_cast<void*>(tagname), wxString::Format( "%sTemplate", tagname ), tagname, grid ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( wxString::Format( "%sRenderType", tagname ), *wxPGProperty::sm_wxPG_LABEL, GetRenderTypeString(), GetRenderTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( wxString::Format( "%sGrayRenderType", tagname ), *wxPGProperty::sm_wxPG_LABEL, GetRenderTypeString(), GetRenderTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( wxString::Format( "%sTextureFilterOption", tagname ), *wxPGProperty::sm_wxPG_LABEL, GetTextureFilterOptionTypeString(), GetTextureFilterOptionTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxColourProperty( wxString::Format( "%sColor", tagname ), wxPG_LABEL, *wxWHITE ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );

	if ( !expand ) {
		grid->Collapse( categoryWnd );
	}
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioFrameButtonEX::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioFrameButtonWndEX* framebtn = dynamic_cast<ioFrameButtonWndEX*>( wnd );
	if ( !grid || !framebtn ) {
		return;
	}

	PropertyControllerioButtonEX::Sync( grid, wnd );

#define SYNC_FRAME( tagname ) \
	grid->SetValue<const char*>( wxString::Format( "%sTemplate", #tagname ).c_str(), framebtn->GetFrameTemplateName##tagname() );\
	grid->SetValue<int>( wxString::Format( "%sRenderType", #tagname ).c_str(), framebtn->GetRenderType##tagname() );\
	grid->SetValue<int>( wxString::Format( "%sGrayRenderType", #tagname ).c_str(), framebtn->GetGrayRenderType##tagname() );\
	grid->SetValue<int>( wxString::Format( "%sTextureFilterOption", #tagname ).c_str(), framebtn->GetTextureFilterOption##tagname() );\
	grid->SetColorValue( wxString::Format( "%sColor", #tagname ).c_str(), ConvertToToolColor( framebtn->GetColor##tagname() ) );

	SYNC_FRAME( Normal )
	SYNC_FRAME( Over )
	SYNC_FRAME( OverAdd )
	SYNC_FRAME( Push )
	SYNC_FRAME( Back )
	SYNC_FRAME( Disable )
	SYNC_FRAME( ExDisableNormal )
	SYNC_FRAME( ExDisablePush )
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioFrameButtonEX::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioButtonEX::OnChange( grid, wnd, event );
	
	ioFrameButtonWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioFrameButtonWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !guimgr ) {
		return;
	}

#define CHECK_FRAME( tagname ) \
	if ( keyname == wxString::Format( "%sTemplate", #tagname ) ) {\
		outwnd->SetFrameTemplateName##tagname( event.GetValue().GetString().c_str(), guimgr );\
	} else if ( keyname == wxString::Format( "%sRenderType", #tagname ) ) {\
		outwnd->SetRenderType##tagname( static_cast< UIRenderType >( event.GetValue().GetInteger() ) );\
	} else if ( keyname == wxString::Format( "%sGrayRenderType", #tagname ) ) {\
		outwnd->SetGrayRenderType##tagname( static_cast< UIRenderType >( event.GetValue().GetInteger() ) );\
	} else if ( keyname == wxString::Format( "%sTextureFilterOption", #tagname ) ) {\
		outwnd->SetTextureFilterOption##tagname( static_cast< TextureFilterOption >( event.GetValue().GetInteger() ) );\
	} else if ( keyname == wxString::Format( "%sColor", #tagname ) ) {\
		outwnd->SetColor##tagname( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );\
	}

	CHECK_FRAME( Normal )
	CHECK_FRAME( Over )
	CHECK_FRAME( OverAdd )
	CHECK_FRAME( Push )
	CHECK_FRAME( Back )
	CHECK_FRAME( Disable )
	CHECK_FRAME( ExDisableNormal )
	CHECK_FRAME( ExDisablePush )
}

//----------------------------------------------------------------------------------
/*
FrameImageSelectorUser override
*/
void	PropertyControllerioFrameButtonEX::OnSelectFrameName( const char* name, void* userdata )
{
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	ioFrameButtonWndEX* wnd = dynamic_cast<ioFrameButtonWndEX*>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd() );
	if ( !wnd || !guimgr ) {
		return;
	}

#define SET_TEMPLATENAME( tagname ) \
	else if ( strcmp( #tagname, static_cast<char*>(userdata) ) == 0 ) {\
		wnd->SetFrameTemplateName##tagname( name, guimgr );\
	}

	if ( false ) {}
	SET_TEMPLATENAME( Normal )
	SET_TEMPLATENAME( Over )
	SET_TEMPLATENAME( OverAdd )
	SET_TEMPLATENAME( Push )
	SET_TEMPLATENAME( Back )
	SET_TEMPLATENAME( Disable )
	SET_TEMPLATENAME( ExDisableNormal )
	SET_TEMPLATENAME( ExDisablePush )
	else {
		return;
	}

	wxRichTextEditorFrame::GetSingleton().SyncProperty();
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioImageButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
PropertyControllerioButtonEX override
*/
void	PropertyControllerioImageButtonEX::SetupPrev( wxioPropertyGrid* grid, wxPGProperty* category )
{
	for ( int i = 0; i < BUTTONEX_TAGNAME_NUM; ++i ) {
		SetupImage( this, grid, category, g_tagnames[ i ], i < 4 );
	}
}

void	PropertyControllerioImageButtonEX::SetupImage( FrameImageSelectorUser* seletor, wxioPropertyGrid* grid, wxPGProperty* parent, char* tagname, bool expand )
{
	if ( !grid || !parent || !tagname ) {
		return;
	}

	wxPGProperty* categoryWnd = grid->AppendIn( parent, new wxPropertyCategory( tagname ) );
	{
		grid->AppendIn( categoryWnd, new wxStringProperty( wxString::Format( "%sFileName", tagname ) ) );
		grid->AppendIn( categoryWnd, new wxImageSetRendererSummonerPropertyEX( seletor, static_cast<void*>(tagname), wxString::Format( "%sImageSetName", tagname ), tagname, grid ) );
		grid->AppendIn( categoryWnd, new wxImageSetRendererSummonerPropertyEX( seletor, static_cast<void*>(tagname), wxString::Format( "%sImageName", tagname ), tagname, grid ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( wxString::Format( "%sOffsetX", tagname ) ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( wxString::Format( "%sOffsetY", tagname ) ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( wxString::Format( "%sRenderType", tagname ), *wxPGProperty::sm_wxPG_LABEL, GetRenderTypeString(), GetRenderTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( wxString::Format( "%sGrayRenderType", tagname ), *wxPGProperty::sm_wxPG_LABEL, GetRenderTypeString(), GetRenderTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( wxString::Format( "%sTextureFilterOption", tagname ), *wxPGProperty::sm_wxPG_LABEL, GetTextureFilterOptionTypeString(), GetTextureFilterOptionTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( wxString::Format( "%sReverseFormat", tagname ), *wxPGProperty::sm_wxPG_LABEL, GetReserveFormatString(), GetReserveFormatInt() ) );
		grid->AppendIn( categoryWnd, new wxColourProperty( wxString::Format( "%sColor", tagname ), wxPG_LABEL, *wxWHITE ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );

	if ( !expand ) {
		grid->Collapse( categoryWnd );
	}
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioImageButtonEX::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioImageButtonWndEX* imagebtn = dynamic_cast<ioImageButtonWndEX*>( wnd );
	if ( !grid || !imagebtn ) {
		return;
	}

	PropertyControllerioButtonEX::Sync( grid, wnd );

	SYNC_IMAGE( grid, imagebtn, Normal )
	SYNC_IMAGE( grid, imagebtn, Over )
	SYNC_IMAGE( grid, imagebtn, OverAdd )
	SYNC_IMAGE( grid, imagebtn, Push )
	SYNC_IMAGE( grid, imagebtn, Back )
	SYNC_IMAGE( grid, imagebtn, Disable )
	SYNC_IMAGE( grid, imagebtn, ExDisableNormal )
	SYNC_IMAGE( grid, imagebtn, ExDisablePush )
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioImageButtonEX::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioButtonEX::OnChange( grid, wnd, event );
	
	ioImageButtonWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioImageButtonWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !guimgr ) {
		return;
	}

	CHECK_IMAGE( grid, outwnd, Normal )
	CHECK_IMAGE( grid, outwnd, Over )
	CHECK_IMAGE( grid, outwnd, OverAdd )
	CHECK_IMAGE( grid, outwnd, Push )
	CHECK_IMAGE( grid, outwnd, Back )
	CHECK_IMAGE( grid, outwnd, Disable )
	CHECK_IMAGE( grid, outwnd, ExDisableNormal )
	CHECK_IMAGE( grid, outwnd, ExDisablePush )
}

//----------------------------------------------------------------------------------
/*
FrameImageSelectorUser override
*/
void	PropertyControllerioImageButtonEX::OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata )
{
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	ioImageButtonWndEX* wnd = dynamic_cast<ioImageButtonWndEX*>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd() );
	if ( !wnd || !guimgr ) {
		return;
	}

	if ( false ) {}
	SET_IMAGESETNAME( wnd, Normal )
	SET_IMAGESETNAME( wnd, Over )
	SET_IMAGESETNAME( wnd, OverAdd )
	SET_IMAGESETNAME( wnd, Push )
	SET_IMAGESETNAME( wnd, Back )
	SET_IMAGESETNAME( wnd, Disable )
	SET_IMAGESETNAME( wnd, ExDisableNormal )
	SET_IMAGESETNAME( wnd, ExDisablePush )
	else {
		return;
	}

	wxRichTextEditorFrame::GetSingleton().SyncProperty();
}