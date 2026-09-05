
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioRichLabel.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioRichLabel.h"
#include "../utils/GlobalRef.h"
#include "ioRichLabel.h"
#include "../wxRichTextEditorFrame.h"
#include "../Dialogs/wxRichLabelEditor.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxFrameRendererSummonerPropertyEX : 프레임렌더 편집창을 위한 property
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class WXDLLIMPEXP_PROPGRID wxRichLabelEditorSummonerPropertyEX : public wxLongStringProperty
{
	 WX_PG_DECLARE_PROPERTY_CLASS( wxRichLabelEditorSummonerPropertyEX )

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
    wxRichLabelEditorSummonerPropertyEX( const wxString& label, wxWindow* parent )
		: wxLongStringProperty( label )
		, m_parent( parent )
	{
	}
	virtual ~wxRichLabelEditorSummonerPropertyEX() {}

	virtual bool	OnButtonClick( wxPropertyGrid* propgrid, wxString& value )
	{
		wxRichLabelEditor* editor = new wxRichLabelEditor( m_parent );
		editor->Show( true );
		return true;
	}

protected:

	wxWindow*		m_parent;
};
wxClassInfo*	wxRichLabelEditorSummonerPropertyEX::GetClassInfo() const
{
	return wxLongStringProperty::GetClassInfo();
}
const wxPGEditor*	wxRichLabelEditorSummonerPropertyEX::DoGetEditorClass() const
{
	return wxLongStringProperty::DoGetEditorClass();
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioRichLabel
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioRichLabel::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioWndEX::Setup( grid );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "RichLabel" ) );
	{
		grid->AppendIn( categoryWnd, new wxRichLabelEditorSummonerPropertyEX( "RichText", wxRichTextEditorFrame::GetSingletonPtr() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "TextHorzAlign", *wxPGProperty::sm_wxPG_LABEL, GetHorzAlignString(), GetHorzAlignInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "TextVertAlign", *wxPGProperty::sm_wxPG_LABEL, GetVertAlignString(), GetVertAlignInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "TextMSVertAlign", *wxPGProperty::sm_wxPG_LABEL, GetVertAlignString(), GetVertAlignInt() ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "LineFeedType", *wxPGProperty::sm_wxPG_LABEL, GetLineFeedTypeString(), GetLineFeedTypeInt() ) );
		grid->AppendIn( categoryWnd, new wxFloatProperty( "VertGap" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioRichLabel::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioRichLabel* richlabel = dynamic_cast<ioRichLabel*>( wnd );
	if ( !grid || !richlabel ) {
		return;
	}

	PropertyControllerioWndEX::Sync( grid, richlabel );
	
	//grid->SetValue<const char*>( "RichText", richlabel->() );
	grid->SetValue<int>( "TextHorzAlign", richlabel->GetHorzAlignType() );
	grid->SetValue<int>( "TextVertAlign", richlabel->GetVertAlignType() );
	grid->SetValue<int>( "TextMSVertAlign", richlabel->GetMSVertAlignType() );
	grid->SetValue<int>( "LineFeedType", richlabel->GetLineFeedType() );
	grid->SetValue<float>( "VertGap", richlabel->GetVertGap() );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioRichLabel::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWndEX::OnChange( grid, wnd, event );
	
	ioRichLabel* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioRichLabel>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !guimgr ) {
		return;
	}

	if ( keyname == "TextHorzAlign" ) {
		outwnd->SetHorzAlignType( static_cast<TextAlignType>( event.GetValue().GetInteger() ) );
	} else if ( keyname == "TextVertAlign" ) {
		outwnd->SetVertAlignType( static_cast<TextVertAlign>( event.GetValue().GetInteger() ) );
	} else if ( keyname == "TextMSVertAlign" ) {
		outwnd->SetMSVertAlignType( static_cast<TextVertAlign>( event.GetValue().GetInteger() ) );
	} else if ( keyname == "LineFeedType" ) {
		outwnd->SetLineFeedType( static_cast<ioRichLabel::LINEFEED_TYPE>( event.GetValue().GetInteger() ) );
	} else if ( keyname == "VertGap" ) {
		outwnd->SetVertGap( event.GetValue().GetDouble() );
	}
}