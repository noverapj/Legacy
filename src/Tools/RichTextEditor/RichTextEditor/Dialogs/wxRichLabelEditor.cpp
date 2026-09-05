
//----------------------------------------------------------------------------------
/*
Filename	: wxRichLabelEditor.cpp
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "wxRichLabelEditor.h"
#include "ioRichLabel.h"
#include "../Utils/GlobalRef.h"
#include "../wxRichTextEditorFrame.h"
#include "../wxWidgetControls/PropertyControllerioWnd.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRichLabelEditor
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
wxRichLabelEditor::wxRichLabelEditor( wxWindow* parent )
	: RichLabelEditor( parent )
{
	Centre( wxBOTH );

	m_propertygrid->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( wxRichLabelEditor::OnPropertyGridChanged ), NULL, this );

	SyncTextBlockList();
}

wxRichLabelEditor::~wxRichLabelEditor()
{
	m_propertygrid->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( wxRichLabelEditor::OnPropertyGridChanged ), NULL, this );
}

//----------------------------------------------------------------------------------
/*
RichLabelEditor override
*/
void	wxRichLabelEditor::OnClickBlock( wxCommandEvent& event )
{
	event.Skip();

	SetupGrid();

	SyncTextBlock();
}

void	wxRichLabelEditor::OnAdd( wxCommandEvent& event )
{
	event.Skip();

	ioRichLabel* rich = dynamic_cast<ioRichLabel*>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd() );
	if ( rich )
	{
		wxString txt = GetNewText();

		m_blocklistbox->AppendString( txt );

		rich->AddText( 13, TS_NORMAL, 0xFFFFFFFF, txt.c_str(), true );
	}
}

void	wxRichLabelEditor::OnRemove( wxCommandEvent& event )
{
	event.Skip();

	ioRichLabel* rich = dynamic_cast<ioRichLabel*>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd() );
	if ( rich )
	{
		int sel = m_blocklistbox->GetSelection();

		if( sel >= 0 )
		{
			rich->RemoveText( sel );

			m_blocklistbox->Delete( sel );
			m_propertygrid->Clear();

			SyncTextBlockList();
		}
	}
}

//----------------------------------------------------------------------------------
/*
Callback
*/
void	wxRichLabelEditor::OnPropertyGridChanged( wxPropertyGridEvent& event )
{
	event.Skip();

	int sel = m_blocklistbox->GetSelection();
	if ( sel < 0 ) return;

	ioRichLabel* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioRichLabel>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd(), &outwnd, keyname, value, event ) ) {
		return;
	}

	if ( keyname == "Text" ) 
	{
		outwnd->SetInputInfoText( sel, event.GetValue().GetString().c_str() );
		m_blocklistbox->SetString( sel, event.GetValue().GetString() );
	}
	else if ( keyname == "Size" ) {
		outwnd->SetInputInfoSize( sel, event.GetValue().GetInteger() );
	} else if ( keyname == "Style" ) {
		outwnd->SetInputInfoStyle( sel, static_cast<TextStyle>( event.GetValue().GetInteger() ) );
	} else if ( keyname == "Color" ) {
		outwnd->SetInputInfoColor( sel, ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	}

	outwnd->RecalcTextLine();
}

//----------------------------------------------------------------------------------
/*
Control
*/
void	wxRichLabelEditor::SetupGrid()
{
	m_propertygrid->Clear();

	wxPGProperty* categoryWnd = m_propertygrid->Append( new wxPropertyCategory( "TextBlock" ) );
	{
		m_propertygrid->AppendIn( categoryWnd, new wxStringProperty( "Text" ) );
		m_propertygrid->AppendIn( categoryWnd, new wxIntProperty( "Size" ) );
		m_propertygrid->AppendIn( categoryWnd, new wxEnumProperty( "Style", *wxPGProperty::sm_wxPG_LABEL, GetTextStyleString(), GetTextStyleInt() ) );
		m_propertygrid->AppendIn( categoryWnd, new wxColourProperty( "Color", wxPG_LABEL, *wxWHITE ) );
	}
	m_propertygrid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );
}

void	wxRichLabelEditor::SyncTextBlockList()
{
	ioRichLabel* rich = dynamic_cast<ioRichLabel*>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd() );
	if ( rich )
	{
		m_propertygrid->Clear();
		m_blocklistbox->Clear();

		int num = rich->GetNumInputInfo();
		for ( int i = 0; i < num; ++i )
		{
			const char* txt = rich->GetInputInfoText( i );
			if ( txt ) {
				m_blocklistbox->Append( txt );
			} else {
				m_blocklistbox->Append( "" );
			}
		}
	}
}

void	wxRichLabelEditor::SyncTextBlock()
{
	int sel = m_blocklistbox->GetSelection();
	if ( sel < 0 )
	{
		m_propertygrid->SetValue<const char*>( "Text", "" );
		m_propertygrid->SetValue<int>( "Size", 0 );
		m_propertygrid->SetValue<int>( "Style", 0 );
		m_propertygrid->SetColorValue( "Color", 0xFFFFFFFF );
	}
	else
	{
		ioRichLabel* rich = dynamic_cast<ioRichLabel*>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd() );
		if ( rich )
		{
			m_blocklistbox->SetString( sel, rich->GetInputInfoText( sel ) );
			m_propertygrid->SetValue<const char*>( "Text", rich->GetInputInfoText( sel ) );
			m_propertygrid->SetValue<int>( "Size", rich->GetInputInfoSize( sel ) );
			m_propertygrid->SetValue<int>( "Style", rich->GetInputInfoStyle( sel ) );
			m_propertygrid->SetColorValue( "Color", ConvertToToolColor( rich->GetInputInfoColor( sel ) ) );
		}
	}
}

//----------------------------------------------------------------------------------
/*
기타
*/
wxString	wxRichLabelEditor::GetNewText()
{
	static wxString pattern = "sumin_";
	wxString ret = pattern;
	int num = m_blocklistbox->GetCount();
	for ( int i = 0; i < num; ++i ) 
	{
		if ( ret == m_blocklistbox->GetString( i ) )
		{
			ret += pattern;
			i = 0;
		}
	}
	return ret;
}