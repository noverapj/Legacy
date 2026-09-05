
//----------------------------------------------------------------------------------
/*
Filename	: wxRenderElementEditor.cpp
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "wxRenderElementEditor.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"
#include "ioGUIManager.h"
#include "ioWnd.h"
#include "ioUIRenderElement.h"
#include "../Utils/GlobalRef.h"
#include "../wxRichTextEditorFrame.h"
#include "wxImageSelector.h"
#include "wxFrameSelector.h"

using namespace GlobalRef;

static const wxColor	g_selectcolor = wxColour( 0xFFF5F0FF );
static const wxColor	g_fixedcolor = wxColour( 200, 250, 250 );
static const wxColor	g_normalcolor = wxColour( 0xFFFFFFFF );

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxImageSetRendererSummonerPropertyEX : 이미지렌더 편집창을 위한 property
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
FrameImageSelectorUser* wxImageSetRendererSummonerPropertyEX::m_parent = 0;
wxImageSelector* wxImageSetRendererSummonerPropertyEX::m_editor = 0;
bool wxImageSetRendererSummonerPropertyEX::OnButtonClick( wxPropertyGrid* propgrid, wxString& value )
{
	m_editor = new wxImageSelector( "ImageSelector", m_parent, m_userdata );
	m_editor->Show( true );

	wxPGProperty* ptis = m_grid->GetProperty( wxString::Format( "%sImageSetName", m_key.c_str() ) );
	wxPGProperty* pti = m_grid->GetProperty( wxString::Format( "%sImageName", m_key.c_str() ) );
	if ( ptis && pti ) {
		m_editor->Select( ptis->GetValue().GetString(), pti->GetValue().GetString() );
	}

	return true;
}
wxClassInfo*	wxImageSetRendererSummonerPropertyEX::GetClassInfo() const
{
	return wxLongStringProperty::GetClassInfo();
}
const wxPGEditor*	wxImageSetRendererSummonerPropertyEX::DoGetEditorClass() const
{
	return wxLongStringProperty::DoGetEditorClass();
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxFrameRendererSummonerPropertyEX : 프레임렌더 편집창을 위한 property
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
FrameImageSelectorUser* wxFrameRendererSummonerPropertyEX::m_parent = 0;
wxFrameSelector* wxFrameRendererSummonerPropertyEX::m_editor = 0;
bool	wxFrameRendererSummonerPropertyEX::OnButtonClick( wxPropertyGrid* propgrid, wxString& value )
{
	m_editor = new wxFrameSelector( "FrameSelector", m_parent, m_userdata );
	m_editor->Show( true );

	wxPGProperty* pt = m_grid->GetProperty( wxString::Format( "%sTemplate", m_key.c_str() ) );
	if ( pt ) {
		m_editor->Select( pt->GetValue().GetString() );
	}

	return true;
}
wxClassInfo*	wxFrameRendererSummonerPropertyEX::GetClassInfo() const
{
	return wxLongStringProperty::GetClassInfo();
}
const wxPGEditor*	wxFrameRendererSummonerPropertyEX::DoGetEditorClass() const
{
	return wxLongStringProperty::DoGetEditorClass();
}


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRenderElementEditor
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
wxRenderElementEditor::wxRenderElementEditor( const wxString& title, const std::string& cname, int renderelemtype )
	: RenderImageEditor( NULL, wxID_ANY, title )
	, m_renderelemtype( renderelemtype )
	, m_catageryname( cname )
{
	Centre( wxBOTH );
}

wxRenderElementEditor::~wxRenderElementEditor()
{
}

//----------------------------------------------------------------------------------
/*
RenderImageEditor override
*/
void	wxRenderElementEditor::OnAdd( wxCommandEvent& event )
{
	AddRenderElementUI( false, m_catageryname );

	event.Skip();
}

void	wxRenderElementEditor::OnRemove( wxCommandEvent& event )
{
	RemoveRenderElementUI( GetSelectRenderElementUI() );

	event.Skip();
}

//----------------------------------------------------------------------------------
/*
이벤트
*/
void	wxRenderElementEditor::Select( wxObject* obj )
{
	RenderElementUIList::iterator iter = m_renderelementUIs.begin();
	RenderElementUIList::iterator eiter = m_renderelementUIs.end();
	for ( ; iter != eiter; ++iter ) 
	{
		if ( obj != iter->m_check ) 
		{
			iter->m_check->SetValue( false );
			if ( iter->m_fixed ) {
				iter->m_grid->SetPropertyBackgroundColour( iter->m_catagerypg, g_fixedcolor );
			} else {
				iter->m_grid->SetPropertyBackgroundColour( iter->m_catagerypg, g_normalcolor );
			}
		}
		else
		{
			if ( iter->m_check->GetValue() ) {
				iter->m_grid->SetPropertyBackgroundColour( iter->m_catagerypg, g_selectcolor );
			} else {
				if ( iter->m_fixed ) {
					iter->m_grid->SetPropertyBackgroundColour( iter->m_catagerypg, g_fixedcolor );
				} else {
					iter->m_grid->SetPropertyBackgroundColour( iter->m_catagerypg, g_normalcolor );
				}
			}
		}
	}
}

void	wxRenderElementEditor::OnSelect( wxCommandEvent& event )
{
	Select( event.GetEventObject() );

	event.Skip();
}

void	wxRenderElementEditor::OnCopy( wxCommandEvent& event )
{	
	RenderElementUIList::iterator iter = m_renderelementUIs.begin();
	RenderElementUIList::iterator eiter = m_renderelementUIs.end();
	for ( int cnt = 0; iter != eiter; ++iter, ++cnt )
	{
		if ( iter->m_copybtn == event.GetEventObject() )
		{
			Copy( iter->m_grid );
			break;
		}
	}

	event.Skip();
}

void	wxRenderElementEditor::OnPaste( wxCommandEvent& event )
{
	RenderElementUIList::iterator iter = m_renderelementUIs.begin();
	RenderElementUIList::iterator eiter = m_renderelementUIs.end();
	for ( int cnt = 0; iter != eiter; ++iter, ++cnt ) 
	{
		if ( iter->m_pastebtn == event.GetEventObject() )
		{
			Paste( iter->m_grid );
			SetToEngine( iter->m_grid, cnt, iter->m_type );
			break;
		}
	}
	SyncFromEngine();

	event.Skip();
}

void	wxRenderElementEditor::OnPropertyChanged( wxPropertyGridEvent& event )
{
	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	if ( !wnd ) {
		return;
	}

	RenderElementUIList::iterator iter = m_renderelementUIs.begin();
	RenderElementUIList::iterator eiter = m_renderelementUIs.end();
	for ( int cnt = 0; iter != eiter; ++iter, ++cnt ) 
	{
		if ( iter->m_grid == event.GetEventObject() ) 
		{
			SetToEngine( iter->m_grid, event, cnt, iter->m_type );
			break;
		}
	}
	SyncFromEngine();

	event.Skip();
}

void	wxRenderElementEditor::OnPropertySelect( wxPropertyGridEvent& event )
{	
	RenderElementUIList::iterator iter = m_renderelementUIs.begin();
	RenderElementUIList::iterator eiter = m_renderelementUIs.end();
	for ( ; iter != eiter; ++iter ) {
		if ( iter->m_grid != event.GetProperty()->GetGrid() ) {
			iter->m_grid->ClearSelection();
		}
	}
	for ( iter = m_renderelementUIs.begin(); iter != eiter; ++iter ) 
	{
		if ( iter->m_grid == event.GetProperty()->GetGrid() )
		{
			iter->m_check->SetValue( true );
			Select( iter->m_check );
			break;
		}
	}

	event.Skip();
}

//----------------------------------------------------------------------------------
/*
RenderImageUIList 컨트롤
*/
void	wxRenderElementEditor::SyncFromEngine()
{
	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	if ( !wnd ) {
		return;
	}

	// 우선 ui와 갯수를 맞춘다.
	int numelem = wnd->GetNumRenderElement( m_renderelemtype );
	int numfixed = 0;
	if ( m_renderelemtype == ioUIRenderElement::UIE_FRAME ) {
		numfixed = wnd->GetNumFixedRenderFrame();
	} else {
		numfixed = wnd->GetNumFixedRenderImage();
	}
	int numtotal = numelem + numfixed;
	int numui = static_cast<int>( m_renderelementUIs.size() );
	if ( numui < numtotal ) {
		for ( int i = 0; i < numtotal - numui; ++i ) {
			AddRenderElementUI( false, m_catageryname.c_str() );
		}
	} else if ( numtotal < numui ) {
		for ( int i = 0; i < numui - numtotal; ++i ) {
			RemoveRenderElementUI( i );
		}
	}

	// ui를 type에 맞게 변환한다.
	for ( int i = 0; i < numfixed; ++i )
	{
		std::string type;
		if ( m_renderelemtype == ioUIRenderElement::UIE_FRAME ) {
			type = wnd->GetFixedRenderFrameType( i );
		} else {
			type = wnd->GetFixedRenderImageType( i );
		}
		ConvertUI( i, true, type );
	}
	for ( int i = numfixed; i < numelem; ++i ) {
		ConvertUI( i, false, m_catageryname.c_str() );
	}
	
	assert( numtotal == static_cast<int>( m_renderelementUIs.size() ) );

	// data들을 동기화한다.
	for ( int i = 0; i < numtotal; ++i )
	{
		RenderElementInfo* uiinfo = GetRenderElementUI( i );
		_SyncFromEngine( uiinfo, wnd, i - numfixed );
		uiinfo->m_grid->ClearIgnoreSet();
	}	
}

int		wxRenderElementEditor::GetSelectRenderElementUI()
{
	int cnt = 0;
	RenderElementUIList::iterator iter = m_renderelementUIs.begin();
	RenderElementUIList::iterator eiter = m_renderelementUIs.end();
	for ( ; iter != eiter; ++iter, ++cnt ) {
		if ( iter->m_check->GetValue() ) {
			return cnt;
		}
	}
	return -1;
}

wxRenderElementEditor::RenderElementInfo*	wxRenderElementEditor::GetRenderElementUI( int index )
{
	int cnt = 0;
	RenderElementUIList::iterator iter = m_renderelementUIs.begin();
	RenderElementUIList::iterator eiter = m_renderelementUIs.end();
	for ( ; iter != eiter; ++iter, ++cnt ) {
		if ( cnt == index ) {
			return &(*iter);
		}
	}
	return 0;
}

void	wxRenderElementEditor::ConvertUI( int index, bool fixed, const std::string& type )
{
	RenderElementInfo* ui = GetRenderElementUI( index );
	if ( !ui ) {
		return;
	}
	
	if ( ui->m_check->GetValue() ) {
		ui->m_grid->SetPropertyBackgroundColour( ui->m_catagerypg, g_selectcolor );
	} else {
		if ( fixed ) {
			ui->m_grid->SetPropertyBackgroundColour( ui->m_catagerypg, g_fixedcolor );
		} else {
			ui->m_grid->SetPropertyBackgroundColour( ui->m_catagerypg, g_normalcolor );
		}
	}

	if ( ui->m_catagerypg ) {
		ui->m_catagerypg->SetLabel( type.c_str() );
	}
	ui->m_type = type;
	ui->m_fixed = fixed;
}

wxRenderElementEditor::RenderElementInfo*	wxRenderElementEditor::AddRenderElementUI( bool fixed, const std::string& type )
{
	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	if ( !wnd ) {
		return 0;
	}
	
	// 생성
	RenderElementInfo info( 0, 0, 0, 0, 0, fixed, type );
	
	info.m_sizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* subsizer = new wxBoxSizer( wxVERTICAL );	

	info.m_check = new wxCheckBox( m_AddPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50, -1 ), 0 );
	subsizer->Add( info.m_check, 0, wxALL, 5 );

	info.m_copybtn = new wxButton( m_AddPanel, wxID_ANY, wxT("Copy"), wxDefaultPosition, wxDefaultSize, 0 );
	subsizer->Add( info.m_copybtn, 0, wxALL, 5 );

	info.m_pastebtn = new wxButton( m_AddPanel, wxID_ANY, wxT("Paste"), wxDefaultPosition, wxDefaultSize, 0 );
	subsizer->Add( info.m_pastebtn, 0, wxALL, 5 );

	info.m_sizer->Add( subsizer, 1, wxEXPAND, 5 );
	
	info.m_grid = new wxioPropertyGrid( m_AddPanel, wxID_ANY, wxDefaultPosition, wxSize( 465, 200 ), 0  );
	info.m_sizer->Add( info.m_grid, 0, wxALL|wxEXPAND, 5 );
		
	m_AddSizer->Add( info.m_sizer, 0, wxEXPAND, 5 );
	
	// 초기화
	info.m_catagerypg = info.m_grid->Append( new wxPropertyCategory( type.c_str() ) );
	_AddRenderElementUI( info.m_grid, info.m_catagerypg );

	// 이벤트 연결
	info.m_check->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxRenderElementEditor::OnSelect ), NULL, this );
	info.m_copybtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxRenderElementEditor::OnCopy ), NULL, this );
	info.m_pastebtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxRenderElementEditor::OnPaste ), NULL, this );
	info.m_grid->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( wxRenderElementEditor::OnPropertyChanged ), NULL, this );
	info.m_grid->Connect( wxEVT_PG_SELECTED, wxPropertyGridEventHandler( wxRenderElementEditor::OnPropertySelect ), NULL, this );
	
	m_renderelementUIs.push_back( info );

	m_AddPanel->Layout();
	m_AddSizer->Fit( m_AddPanel );

	Layout();

	return &(m_renderelementUIs.back());
}

void	wxRenderElementEditor::RemoveRenderElementUI( int index )
{
	if ( index < 0 || index >= static_cast<int>( m_renderelementUIs.size() ) ) {
		return;
	}

	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	if ( !wnd ) {
		return;
	}

	RenderElementUIList::iterator iter = m_renderelementUIs.begin();
	std::advance( iter, index );

	if ( iter == m_renderelementUIs.end() ) {
		return;
	}

	// 고정은 삭제 못함
	if ( iter->m_fixed )
	{
		wxMessageBox( "고정 데이터는 삭제할 수 없습니다.", "알림" );
		return;
	}

	iter->m_grid->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( wxRenderElementEditor::OnPropertyChanged ), NULL, this );
	iter->m_grid->Disconnect( wxEVT_PG_SELECTED, wxPropertyGridEventHandler( wxRenderElementEditor::OnPropertySelect ), NULL, this );
	iter->m_copybtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxRenderElementEditor::OnCopy ), NULL, this );
	iter->m_pastebtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxRenderElementEditor::OnPaste ), NULL, this );
	iter->m_check->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxRenderElementEditor::OnSelect ), NULL, this );	

	m_AddSizer->Remove( iter->m_sizer );
	
	m_AddPanel->RemoveChild( iter->m_grid );
	m_AddPanel->RemoveChild( iter->m_check );

	delete iter->m_grid;
	delete iter->m_check;

	m_AddPanel->Layout();
	m_AddSizer->Fit( m_AddPanel );

	Layout();

	m_renderelementUIs.erase( iter );
}

void	wxRenderElementEditor::ClearRenderElementUI()
{
	while ( !m_renderelementUIs.empty() ) {
		RemoveRenderElementUI( m_renderelementUIs.size() - 1 );
	}
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRenderImageEditor
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
wxRenderImageEditor::wxRenderImageEditor( const wxString& title )
	: wxRenderElementEditor( title, "RenderImage", ioUIRenderElement::UIE_IMAGE )
{
}

wxRenderImageEditor::~wxRenderImageEditor()
{
}

//----------------------------------------------------------------------------------
/*
RenderImageEditor override
*/
void	wxRenderImageEditor::OnAdd( wxCommandEvent& event )
{
	AddRenderElementUI( false, m_catageryname );

	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	if ( wnd )
	{
		wnd->GenerateImageList( ( m_renderelementUIs.size() - 1 ) - wnd->GetNumFixedRenderImage() );
		SyncFromEngine();
	}

	event.Skip();
}

void	wxRenderImageEditor::OnRemove( wxCommandEvent& event )
{
	int index = GetSelectRenderElementUI();
	RemoveRenderElementUI( index );

	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	if ( wnd )
	{
		wnd->RemoveRenderElementList( index - wnd->GetNumFixedRenderImage(), ioUIRenderElement::UIE_IMAGE );
		SyncFromEngine();
	}

	event.Skip();
}

void	wxRenderImageEditor::SetToEngine( wxioPropertyGrid* grid, wxPropertyGridEvent& event, int index, const std::string& type )
{	
	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !wnd || !guimgr ) {
		return;
	}

	wxPGProperty* property = event.GetProperty();
	if ( !property ) {
		return;
	}

	wxString keyname = property->GetName();
	wxAny value = property->GetValue();
	index = index - wnd->GetNumFixedRenderImage();

	if ( keyname == "FileName" )
	{
		wnd->SetRenderImageFilename( type.c_str(), index, event.GetValue().GetString().c_str(), guimgr );

		wxPGProperty* pt = 0;
		pt = grid->GetProperty( "ImageSetName" );
		pt->SetValue( "" );
		pt = grid->GetProperty( "ImageName" );
		pt->SetValue( "" );
		return;
	}
	
	if ( keyname == "ImageSetName" )
	{
		wxPGProperty* pt = grid->GetProperty( "ImageName" );
		wnd->SetRenderImageImageName( type.c_str(), index, event.GetValue().GetString().c_str(), pt->GetValue().GetString().c_str(), guimgr );

		pt = grid->GetProperty( "FileName" );
		pt->SetValue( "" );

		grid->AddIgnoreSet( "ImageSetName" );
		grid->AddIgnoreSet( "ImageName" );				
		return;
	}

	if ( keyname == "ImageName" )
	{
		wxPGProperty* pt = grid->GetProperty( "ImageSetName" );
		wnd->SetRenderImageImageName( type.c_str(), index, pt->GetValue().GetString().c_str(), event.GetValue().GetString().c_str(), guimgr );

		pt = grid->GetProperty( "FileName" );
		pt->SetValue( "" );

		grid->AddIgnoreSet( "ImageSetName" );
		grid->AddIgnoreSet( "ImageName" );
		return;
	}
	
	if (  keyname == "X" ) {
		wnd->SetRenderImageRectX( type.c_str(), index, event.GetValue().GetInteger() );
	} else if (  keyname == "Y" ) {
		wnd->SetRenderImageRectY( type.c_str(), index, event.GetValue().GetInteger() );
	} else if (  keyname == "Width" ) {
		wnd->SetRenderImageRectWidth( type.c_str(), index, event.GetValue().GetInteger() );
	} else if (  keyname == "Height" ) {
		wnd->SetRenderImageRectHeight( type.c_str(), index, event.GetValue().GetInteger() );
	} else if (  keyname == "Color" ) {
		wnd->SetRenderImageColor( type.c_str(), index, ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	}
}

void	wxRenderImageEditor::SetToEngine( wxioPropertyGrid* grid, int index, const std::string& type )
{
	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !wnd || !guimgr ) {
		return;
	}

	index = index - wnd->GetNumFixedRenderImage();

	//! 주의 : 반드시 Filename or ImageSetName / ImageName 먼저 셋팅해줘야 생성되서 뒤에 인자들이 셋팅된다.
	wxString filename = grid->GetProperty( "FileName" )->GetValueAsString();
	if ( filename != wxEmptyString )
	{
		wnd->SetRenderImageFilename( type.c_str(), index, filename.c_str(), guimgr );

		wxPGProperty* pt = 0;
		pt = grid->GetProperty( "ImageSetName" );
		pt->SetValue( "" );
		pt = grid->GetProperty( "ImageName" );
		pt->SetValue( "" );
	}
	else
	{
		wnd->SetRenderImageImageName( type.c_str(), index, grid->GetProperty( "ImageSetName" )->GetValueAsString().c_str(), grid->GetProperty( "ImageName" )->GetValueAsString().c_str(), guimgr );

		wxPGProperty* pt = grid->GetProperty( "FileName" );
		pt->SetValue( "" );
	}
	
	wnd->SetRenderImageRectX( type.c_str(), index, grid->GetProperty( "X" )->GetValue().GetInteger() );
	wnd->SetRenderImageRectY( type.c_str(), index, grid->GetProperty( "Y" )->GetValue().GetInteger() );
	wnd->SetRenderImageRectWidth( type.c_str(), index, grid->GetProperty( "Width" )->GetValue().GetInteger() );
	wnd->SetRenderImageRectHeight( type.c_str(), index, grid->GetProperty( "Height" )->GetValue().GetInteger() );
	wxAny var = grid->GetProperty( "Color" )->GetValue();
	wnd->SetRenderImageColor( type.c_str(), index, ConvertToEngineColor( var.As<wxColour>().m_pixel ) );
}

//----------------------------------------------------------------------------------
/*
RenderImageUIList 컨트롤
*/
void	wxRenderImageEditor::_SyncFromEngine( RenderElementInfo* info, ioWnd* wnd, int index )
{
	const char* filename = wnd->GetRenderImageFilename( info->m_type.c_str() ,index );
	const char* imageset = wnd->GetRenderImageImageSetName( info->m_type.c_str(), index );
	const char* image    = wnd->GetRenderImageImageName( info->m_type.c_str(), index );

	info->m_grid->SetValue<const char*>( "FileName", wnd->GetRenderImageFilename( info->m_type.c_str(), index ) );
	info->m_grid->SetValue<const char*>( "ImageSetName", wnd->GetRenderImageImageSetName( info->m_type.c_str(), index ) );
	info->m_grid->SetValue<const char*>( "ImageName", wnd->GetRenderImageImageName( info->m_type.c_str(), index ) );	

	info->m_grid->SetValue<int>( "X", wnd->GetRenderImageRectX( info->m_type.c_str(), index ) );
	info->m_grid->SetValue<int>( "Y", wnd->GetRenderImageRectY( info->m_type.c_str(), index ) );
	info->m_grid->SetValue<int>( "Width", wnd->GetRenderImageRectWidth( info->m_type.c_str(), index ) );
	info->m_grid->SetValue<int>( "Height", wnd->GetRenderImageRectHeight( info->m_type.c_str(), index ) );
	info->m_grid->SetColorValue( "Color", ConvertToToolColor( wnd->GetRenderImageColor( info->m_type.c_str(), index ) ) );
}

void	wxRenderImageEditor::_AddRenderElementUI( wxioPropertyGrid* grid, wxPGProperty* categoryWnd )
{
	grid->AppendIn( categoryWnd, new wxFileProperty( "FileName" ) );
	grid->AppendIn( categoryWnd, new wxImageSetRendererSummonerPropertyEX( this, 0, "ImageSetName", "", grid ) );
	grid->AppendIn( categoryWnd, new wxImageSetRendererSummonerPropertyEX( this, 0, "ImageName", "", grid ) );	
	grid->AppendIn( categoryWnd, new wxIntProperty( "X" ) );
	grid->AppendIn( categoryWnd, new wxIntProperty( "Y" ) );
	grid->AppendIn( categoryWnd, new wxIntProperty( "Width" ) );
	grid->AppendIn( categoryWnd, new wxIntProperty( "Height" ) );
	grid->AppendIn( categoryWnd, new wxColourProperty( "Color", wxPG_LABEL, *wxWHITE ) );
}

//----------------------------------------------------------------------------------
/*
FrameImageSelectorUser override
*/
void	wxRenderImageEditor::OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata )
{
	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !wnd || !guimgr ) {
		return;
	}

	RenderElementUIList::iterator iter = m_renderelementUIs.begin();
	RenderElementUIList::iterator eiter = m_renderelementUIs.end();
	for ( int cnt = 0; iter != eiter; ++iter, ++cnt ) 
	{
		if ( iter->m_check->IsChecked() )
		{
			wxPGProperty* pt = 0;
			pt = iter->m_grid->GetProperty( "ImageSetName" );
			pt->SetValue( imagesetname );
			pt = iter->m_grid->GetProperty( "ImageName" );
			pt->SetValue( imagename );
			pt = iter->m_grid->GetProperty( "FileName" );
			pt->SetValueFromString( "" );

			wnd->SetRenderImageImageName( iter->m_type.c_str(), cnt - wnd->GetNumFixedRenderImage(), imagesetname, imagename, guimgr );
			SyncFromEngine();
			break;
		}
	}
}

//----------------------------------------------------------------------------------
/*
Copy / Paste
*/
void	wxRenderImageEditor::Copy( wxioPropertyGrid* grid )
{
	m_copydata.m_filename = grid->GetProperty( "FileName" )->GetValue().GetString().c_str();
	m_copydata.m_imagesetname = grid->GetProperty( "ImageSetName" )->GetValue().GetString().c_str();
	m_copydata.m_imagename = grid->GetProperty( "ImageName" )->GetValue().GetString().c_str();
	m_copydata.m_x = grid->GetProperty( "X" )->GetValue().GetInteger();
	m_copydata.m_y = grid->GetProperty( "Y" )->GetValue().GetInteger();
	m_copydata.m_width = grid->GetProperty( "Width" )->GetValue().GetInteger();
	m_copydata.m_height = grid->GetProperty( "Height" )->GetValue().GetInteger();

	wxAny value = grid->GetProperty( "Color" )->GetValue();
	m_copydata.m_color = value.As<wxColour>().m_pixel;
}

void	wxRenderImageEditor::Paste( wxioPropertyGrid* grid )
{
	grid->GetProperty( "FileName" )->SetValueFromString( m_copydata.m_filename.c_str() );
	grid->GetProperty( "ImageSetName" )->SetValueFromString( m_copydata.m_imagesetname.c_str() );
	grid->GetProperty( "ImageName" )->SetValueFromString( m_copydata.m_imagename.c_str() );
	grid->GetProperty( "X" )->SetValueFromInt( m_copydata.m_x );
	grid->GetProperty( "Y" )->SetValueFromInt( m_copydata.m_y );
	grid->GetProperty( "Width" )->SetValueFromInt( m_copydata.m_width );
	grid->GetProperty( "Height" )->SetValueFromInt( m_copydata.m_height );

	wxVariant variant;
	variant << wxColour( m_copydata.m_color );;
	grid->GetProperty( "Color" )->SetValue( variant );
}


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRenderFrameEditor
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
wxRenderFrameEditor::wxRenderFrameEditor( const wxString& title )
	: wxRenderElementEditor( title, "RenderFrame", ioUIRenderElement::UIE_FRAME )
{
}

wxRenderFrameEditor::~wxRenderFrameEditor()
{
}

//----------------------------------------------------------------------------------
/*
RenderImageEditor override
*/
void	wxRenderFrameEditor::OnAdd( wxCommandEvent& event )
{
	AddRenderElementUI( false, m_catageryname );

	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	if ( wnd )
	{
		wnd->GenerateFrameList( ( m_renderelementUIs.size() - 1 ) - wnd->GetNumFixedRenderFrame() );
		SyncFromEngine();
	}

	event.Skip();
}

void	wxRenderFrameEditor::OnRemove( wxCommandEvent& event )
{
	int index = GetSelectRenderElementUI();
	RemoveRenderElementUI( index );

	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	if ( wnd )
	{
		wnd->RemoveRenderElementList( index - wnd->GetNumFixedRenderFrame(), ioUIRenderElement::UIE_FRAME );
		SyncFromEngine();
	}

	event.Skip();
}

void	wxRenderFrameEditor::SetToEngine( wxioPropertyGrid* grid, wxPropertyGridEvent& event, int index, const std::string& type )
{
	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !wnd || !guimgr ) {
		return;
	}

	wxPGProperty* property = event.GetProperty();
	if ( !property ) {
		return;
	}

	wxString keyname = property->GetName();
	wxAny value = property->GetValue();
	index = index - wnd->GetNumFixedRenderFrame();

	if (  keyname == "X" ) {
		wnd->SetFrameRenderRectX( type.c_str(), index, event.GetValue().GetInteger() );
	} else if (  keyname == "Y" ) {
		wnd->SetFrameRenderRectY( type.c_str(), index, event.GetValue().GetInteger() );
	} else if (  keyname == "Width" ) {
		wnd->SetFrameRenderRectWidth( type.c_str(), index, event.GetValue().GetInteger() );
	} else if (  keyname == "Height" ) {
		wnd->SetFrameRenderRectHeight( type.c_str(), index, event.GetValue().GetInteger() );
	} else if (  keyname == "Color" ) {
		wnd->SetFrameRenderColor( type.c_str(), index, ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if (  keyname == "AutoResize" ) {
		wnd->SetFrameRenderAutoResize( type.c_str(), index, event.GetValue().GetBool() );
	} else if (  keyname == "Template" ) {
		//wnd->SetFrameRenderTemplete( type.c_str(), index, property->ValueToString( event.GetValue() ).c_str(), guimgr );
		wnd->SetFrameRenderTemplete( type.c_str(), index, event.GetValue().GetString().c_str(), guimgr );
	}

	SyncFromEngine();
}

void	wxRenderFrameEditor::SetToEngine( wxioPropertyGrid* grid, int index, const std::string& type )
{
	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !wnd || !guimgr ) {
		return;
	}

	index = index - wnd->GetNumFixedRenderFrame();

	//! 주의 : 반드시 Template먼저 셋팅해줘야 생성되서 뒤에 인자들이 셋팅된다.
	wnd->SetFrameRenderTemplete( type.c_str(), index, grid->GetProperty( "Template" )->GetValueAsString().c_str(), guimgr );
	wnd->SetFrameRenderRectX( type.c_str(), index, grid->GetProperty( "X" )->GetValue().GetInteger() );
	wnd->SetFrameRenderRectY( type.c_str(), index, grid->GetProperty( "Y" )->GetValue().GetInteger() );
	wnd->SetFrameRenderRectWidth( type.c_str(), index, grid->GetProperty( "Width" )->GetValue().GetInteger() );
	wnd->SetFrameRenderRectHeight( type.c_str(), index, grid->GetProperty( "Height" )->GetValue().GetInteger() );	
	wnd->SetFrameRenderAutoResize( type.c_str(), index, grid->GetProperty( "AutoResize" )->GetValue().GetBool() );	

	wxAny var = grid->GetProperty( "Color" )->GetValue();
	wnd->SetFrameRenderColor( type.c_str(), index, ConvertToEngineColor( var.As<wxColour>().m_pixel ) );
}

//----------------------------------------------------------------------------------
/*
RenderImageUIList 컨트롤
*/
void	wxRenderFrameEditor::_SyncFromEngine( RenderElementInfo* info, ioWnd* wnd, int index )
{
	info->m_grid->SetValue<int>( "X", wnd->GetFrameRenderRectX( info->m_type.c_str(), index ) );
	info->m_grid->SetValue<int>( "Y", wnd->GetFrameRenderRectY( info->m_type.c_str(), index ) );
	info->m_grid->SetValue<int>( "Width", wnd->GetFrameRenderRectWidth( info->m_type.c_str(), index ) );
	info->m_grid->SetValue<int>( "Height", wnd->GetFrameRenderRectHeight( info->m_type.c_str(), index ) );
	info->m_grid->SetValue<int>( "AutoResize", wnd->GetFrameRenderAutoResize( info->m_type.c_str(), index ) );
	info->m_grid->SetColorValue( "Color", ConvertToToolColor( wnd->GetFrameRenderColor( info->m_type.c_str(), index ) ) );
	info->m_grid->SetValue<const char*>( "Template", wnd->GetFrameRenderTemplete( info->m_type.c_str(), index ) );
}

void	wxRenderFrameEditor::_AddRenderElementUI( wxioPropertyGrid* grid, wxPGProperty* categoryWnd )
{
	grid->AppendIn( categoryWnd, new wxIntProperty( "X" ) );
	grid->AppendIn( categoryWnd, new wxIntProperty( "Y" ) );
	grid->AppendIn( categoryWnd, new wxIntProperty( "Width" ) );
	grid->AppendIn( categoryWnd, new wxIntProperty( "Height" ) );
	grid->AppendIn( categoryWnd, new wxBoolProperty( "AutoResize" ) );
	grid->AppendIn( categoryWnd, new wxColourProperty( "Color", wxPG_LABEL, *wxWHITE ) );
	grid->AppendIn( categoryWnd, new wxFrameRendererSummonerPropertyEX( this, 0, "Template", "", grid ) );

	//wxArrayString stritem;
	//int numframe = wxRichTextEditorFrame::GetSingleton().GetGUIManager()->GetNumFrame();	
	//for ( int i = 0; i < numframe; ++i ) {
	//	stritem.Add( wxRichTextEditorFrame::GetSingleton().GetGUIManager()->GetFrameName( i ) );
	//}
	//stritem.Sort();
	//grid->AppendIn( categoryWnd, new wxEditEnumProperty( "Template", *wxPGProperty::sm_wxPG_LABEL, stritem ) );
}

//----------------------------------------------------------------------------------
/*
기타
*/
void	wxRenderFrameEditor::OnSelectFrameName( const char* name, void* userdata )
{
	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetSelectioWnd();
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !wnd || !guimgr ) {
		return;
	}

	RenderElementUIList::iterator iter = m_renderelementUIs.begin();
	RenderElementUIList::iterator eiter = m_renderelementUIs.end();
	for ( int cnt = 0; iter != eiter; ++iter, ++cnt ) 
	{
		if ( iter->m_check->IsChecked() )
		{
			wxPGProperty* pt = 0;
			pt = iter->m_grid->GetProperty( "Template" );
			pt->SetValue( name );

			wnd->SetFrameRenderTemplete( iter->m_type.c_str(), cnt - wnd->GetNumFixedRenderFrame(), name, guimgr );
			SyncFromEngine();
			break;
		}
	}
}

//----------------------------------------------------------------------------------
/*
Copy / Paste
*/
void	wxRenderFrameEditor::Copy( wxioPropertyGrid* grid )
{
	m_copydata.m_templete = grid->GetProperty( "Template" )->GetValue().GetString().c_str();
	m_copydata.m_x = grid->GetProperty( "X" )->GetValue().GetInteger();
	m_copydata.m_y = grid->GetProperty( "Y" )->GetValue().GetInteger();
	m_copydata.m_width = grid->GetProperty( "Width" )->GetValue().GetInteger();
	m_copydata.m_height = grid->GetProperty( "Height" )->GetValue().GetInteger();
	m_copydata.m_autoresize = grid->GetProperty( "AutoResize" )->GetValue().GetBool();

	wxAny value = grid->GetProperty( "Color" )->GetValue();
	m_copydata.m_color = value.As<wxColour>().m_pixel;	
}

void	wxRenderFrameEditor::Paste( wxioPropertyGrid* grid )
{
	grid->GetProperty( "Template" )->SetValueFromString( m_copydata.m_templete.c_str() );
	grid->GetProperty( "X" )->SetValueFromInt( m_copydata.m_x );
	grid->GetProperty( "Y" )->SetValueFromInt( m_copydata.m_y );
	grid->GetProperty( "Width" )->SetValueFromInt( m_copydata.m_width );
	grid->GetProperty( "Height" )->SetValueFromInt( m_copydata.m_height );
	grid->GetProperty( "AutoResize" )->SetValueFromInt( m_copydata.m_autoresize );
		
	wxVariant vc;
	vc << wxColour( m_copydata.m_color );
	grid->GetProperty( "Color" )->SetValue( vc );
}