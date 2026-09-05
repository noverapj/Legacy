
//----------------------------------------------------------------------------------
/*
Filename	: wxRichTextEditorFrame.cpp
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "wxRichTextEditorFrame.h"
#include "ioRenderSystem.h"
#include "ioVertexBufferManager.h"
#include "ioIndexBufferManager.h"
#include "ioTextureManager.h"
#include "ioShaderManager.h"
#include "ioFontManager.h"
#include "ioGUIManager.h"
#include "ioCamera.h"
#include "ioFrameTimer.h"
#include "ioResourceLoader.h"
#include "ioCameraController.h"
#include "ioLookAtCameraController.h"
#include "InputBox.h"
#include "ioWnd.h"
#include "ioButton.h"
#include "ioEdit.h"
#include "ioProgressBar.h"
#include "ioScroll.h"
#include "wx/imaglist.h"
#include "Utils/GlobalRef.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"
#include "Utils/Visualizer2D.h"
#include "Dialogs/wxRenderElementEditor.h"
#include "ioMouse.h"
#include "wxWidgetControls/PropertyControllerioWnd.h"
#include "wxWidgetControls/PropertyControllerioScrolls.h"
#include "wxWidgetControls/PropertyControllerioETC.h"
#include "wxWidgetControls/PropertyControllerioButtons.h"
#include "wx/filedlg.h"
#include "ProjectManager/ProjectManager.h"
#include "ioSoundManager.h"
#include "ioWndEX.h"
#include "ioFrameWndEX.h"
#include "ioImageWndEX.h"
#include "ioLabelWndEX.h"
#include "ioButtonWndEX.h"
#include "ioCheckBoxEX.h"
#include "ioFlashPlayer.h"
#include "ioTabWndEX.h"
#include "ioScrollBarEX.h"
#include "ioRichLabel.h"
#include "wxWidgetControls/PropertyControllerioWndEX.h"
#include "wxWidgetControls/PropertyControllerioFrameWndEX.h"
#include "wxWidgetControls/PropertyControllerioImageWndEX.h"
#include "wxWidgetControls/PropertyControllerioLabelWndEX.h"
#include "wxWidgetControls/PropertyControllerioButtonEX.h"
#include "wxWidgetControls/PropertyControllerioCheckBoxEX.h"
#include "wxWidgetControls/PropertyControllerioFlashPlayer.h"
#include "wxWidgetControls/PropertyControllerioTabWndEX.h"
#include "wxWidgetControls/PropertyControllerioRadioWndEX.h"
#include "wxWidgetControls/PropertyControllerioScrollBarEX.h"
#include "wxWidgetControls/PropertyControllerioRichLabel.h"
#include "Dialogs/wxIDSettingBox.h"
#include "ioVertexFormat.h"
#include "ioUIRenderer.h"

using namespace GlobalRef;

static const int TOOLTIP_SURPPORT_NUM = 1;

template<> wxRichTextEditorFrame* Singleton< wxRichTextEditorFrame >::ms_Singleton = 0;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioUIToolGUIManager
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioUIToolGUIManager : public ioGUIManager
{
public:

	ioUIToolGUIManager() 
		: ioGUIManager()
	{}

	~ioUIToolGUIManager() 
	{}

	virtual void	InitManager()
	{
		ioGUIManager::InitManager();

		LoadImageSetFile( "../XML/UIImageSet.xml" );
		LoadFrameFile( "../XML/DefaultUIFrame.xml" );
	}

	virtual ioWnd*	CreateNewWindow( const ioHashString& szClassName )
	{
		return ioGUIManager::CreateNewWindow( szClassName );
	}
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
EventHooker
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class EventHooker : public wxEvtHandler
{
public:

    EventHooker()
	{
		this->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( EventHooker::OnLButtonUp ) );
	}

	~EventHooker()
	{
		this->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( EventHooker::OnLButtonUp ) );
	}

    static void		RegisterFor( wxWindow* win )
	{
		wxWindowListNode* childnode = win->GetChildren().GetFirst();
		while ( childnode )
		{
			childnode->GetData()->PushEventHandler( new EventHooker() );
			childnode = childnode->GetNext();
		} 
	}

	static void		UnregisterFor( wxWindow* win )
	{
		wxWindowListNode* childnode = win->GetChildren().GetFirst();
		while ( childnode )
		{
			childnode->GetData()->PopEventHandler( true );
			childnode = childnode->GetNext();
		} 
	}
 
private:
 
	void			OnLButtonUp( wxMouseEvent& event )
	{
		event.ResumePropagation(1);
		event.Skip();
	} 
};
 



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRichTextEditorFrame
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

enum TIMERID
{
	TIMERID_RENDER,
};

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
wxRichTextEditorFrame::wxRichTextEditorFrame( const wxString& title )
	: wxMainFrame( NULL, wxID_ANY, title )
	, m_uniqueid( 0 )
	, m_visualizer( 0 )
	, m_inputprocessor( this )
{
	//
	Centre();
		
	// set event hooker
	{
		EventHooker::RegisterFor( this );

		this->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( wxRichTextEditorFrame::OnHookLButtonUp ) );
	}

	// visualizer
	m_visualizer = new Visualizer2D();

	// init engine
	{
		g_ResourceLoader.Initialize( false );
		g_ResourceLoader.StopLoadingThread();

		HWND hwnd = reinterpret_cast<HWND>( GetDisplayWnd()->GetHWND() );
		wxSize size = GetDisplayWnd()->GetSize();
		m_renderer = &RenderSystem();
		m_renderer->SetMinDisplayMode( size.GetWidth(), size.GetHeight() );
		m_renderer->Create( hwnd, size.GetWidth(), size.GetHeight(), false );

		timeBeginPeriod( 1 );

		m_vxbuffermgr = new ioVertexBufferManager( m_renderer->GetDevice() );
		m_ixbuffermgr = new ioIndexBufferManager( m_renderer->GetDevice() );

		m_texturemgr = new ioTextureManager( m_renderer->GetDevice() );
		m_texturemgr->SetStartDir( "../resource/Texture" );
		m_texturemgr->ChangeTextureQuality( TQ_DEFAULT );
		m_texturemgr->SetGrayScaleRate( 1 );
		m_texturemgr->SetGrayLightScaleRate( 1 );
		
		m_shadermgr = new ioShaderManager( m_renderer->GetDevice() );	
		m_shadermgr->SetStartDir( "../Shader" );
		
		m_uirenderer = new ioUIRenderer();
		m_uirenderer->Initialize();

		m_fontmgr	= new ioFontManager();
		m_fontmgr->Initialize( m_uirenderer );
		m_fontmgr->SetStartDir( "../resource/Font" );
		m_fontmgr->SetFont( "lostsaga.ttf" );
		m_fontmgr->SetFontSize( 24, 9 );

		m_guimgr = new ioUIToolGUIManager();
		m_guimgr->InitManager();
		ClearEditWnds( true );		
		
		m_frametimer = new ioFrameTimer();
		m_frametimer->Start( 30.0f );

		m_camera = new ioCamera();
		ioLookAtCameraController* camcontrol = new ioLookAtCameraController();
		camcontrol->SetPosition( D3DXVECTOR3( 0.0f, 120.0f, -300.0f ) );
		camcontrol->SetLookAt( D3DXVECTOR3( 0.0f, 120.0f, 0.0f ) );
		m_camera->SetController( camcontrol );
		m_camera->SetProjection( 4.0f, 5000.0f, 60.0f, static_cast<float>( size.GetWidth() ) / static_cast<float>( size.GetHeight() ) );
		
		m_inputbox = new ioInputBox( reinterpret_cast<HWND>( GetHWND() ), 0 );

		m_soundmgr = new ioSoundManager();
	}

	// init controls
	{
		// tree
		wxImageList* imagelist = new wxImageList( 30, 24, true, GlobalRef::TBT_MAX );
		for ( int i = 0; i < GlobalRef::TBT_MAX; ++i ) {
			imagelist->Add( wxBitmap( GlobalRef::g_toolboxbitmapfilenames[ i ], wxBITMAP_TYPE_ANY ) );
		}
		m_InheritTree->DeleteAllItems();
		m_InheritTree->AssignImageList( imagelist );

		// property grid
		m_propertygrid->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( wxRichTextEditorFrame::OnPropertyGridChanged ), NULL, this );
		m_propertygridevent->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( wxRichTextEditorFrame::OnEventPropertyGridChanged ), NULL, this );
		
		// toolbar
		m_toolbar_wndcontorl->SetToolDisabledBitmap( wxID_TOOl_ALIGN_LEFT, wxBitmap( wxT("ToolResources/UITool/Images/alignleftd.bmp"), wxBITMAP_TYPE_ANY ) );
		m_toolbar_wndcontorl->SetToolDisabledBitmap( wxID_TOOl_ALIGN_RIGHT, wxBitmap( wxT("ToolResources/UITool/Images/alignrightd.bmp"), wxBITMAP_TYPE_ANY ) );
		m_toolbar_wndcontorl->SetToolDisabledBitmap( wxID_TOOl_ALIGN_TOP, wxBitmap( wxT("ToolResources/UITool/Images/aligntopd.bmp"), wxBITMAP_TYPE_ANY ) );
		m_toolbar_wndcontorl->SetToolDisabledBitmap( wxID_TOOl_ALIGN_BOTTOM, wxBitmap( wxT("ToolResources/UITool/Images/alignbottomd.bmp"), wxBITMAP_TYPE_ANY ) );
		m_toolbar_wndcontorl->SetToolDisabledBitmap( wxID_TOOl_CORRECTGAP_LR, wxBitmap( wxT("ToolResources/UITool/Images/correctgaplrd.bmp"), wxBITMAP_TYPE_ANY ) );
		m_toolbar_wndcontorl->SetToolDisabledBitmap( wxID_TOOl_CORRECTGAP_TB, wxBitmap( wxT("ToolResources/UITool/Images/correctgaptbd.bmp"), wxBITMAP_TYPE_ANY ) );
		m_toolbar_wndcontorl->SetToolDisabledBitmap( wxID_TOOl_CORRECTSIZE_LR, wxBitmap( wxT("ToolResources/UITool/Images/correctsizelrd.bmp"), wxBITMAP_TYPE_ANY ) );
		m_toolbar_wndcontorl->SetToolDisabledBitmap( wxID_TOOl_CORRECTSIZE_TB, wxBitmap( wxT("ToolResources/UITool/Images/correctsizetbd.bmp"), wxBITMAP_TYPE_ANY ) );
		m_toolbar_wndcontorl->SetToolDisabledBitmap( wxID_TOOl_CORRECTSIZE_ALL, wxBitmap( wxT("ToolResources/UITool/Images/correctsizealld.bmp"), wxBITMAP_TYPE_ANY ) );
	}

	// pg controller
	{
		// TBT_WND 에 따라 순서대로 셋팅한다
		m_pgcontrollers.push_back( new PropertyControllerioWnd() );
		m_pgcontrollers.push_back( new PropertyControllerioButton() );
		m_pgcontrollers.push_back( new PropertyControllerioRadioButton() );
		m_pgcontrollers.push_back( new PropertyControllerioCheckBox() );
		m_pgcontrollers.push_back( new PropertyControllerioEdit() );
		m_pgcontrollers.push_back( new PropertyControllerioProgressBar() );
		m_pgcontrollers.push_back( new PropertyControllerioSlide() );
		m_pgcontrollers.push_back( new PropertyControllerioActiveScreenButton() );
		m_pgcontrollers.push_back( new PropertyControllerioScroll() );
		// EX
		m_pgcontrollers.push_back( new PropertyControllerioWndEX() );
		m_pgcontrollers.push_back( new PropertyControllerioFrameWndEX() );
		m_pgcontrollers.push_back( new PropertyControllerioImageWndEX() );
		m_pgcontrollers.push_back( new PropertyControllerioLabelWndEX() );
		m_pgcontrollers.push_back( new PropertyControllerioFrameButtonEX() );
		m_pgcontrollers.push_back( new PropertyControllerioImageButtonEX() );
		m_pgcontrollers.push_back( new PropertyControllerioCheckBoxEX() );
		m_pgcontrollers.push_back( new PropertyControllerioFlashPlayer() );
		m_pgcontrollers.push_back( new PropertyControllerioRadioWndEX() ); // radio
		m_pgcontrollers.push_back( new PropertyControllerioFrameButtonEX() ); // radio frame btn
		m_pgcontrollers.push_back( new PropertyControllerioImageButtonEX() ); // radio image btn
		m_pgcontrollers.push_back( new PropertyControllerioFrameButtonEX() ); // tab frame
		m_pgcontrollers.push_back( new PropertyControllerioTabWndEX() ); // tabwnd
		m_pgcontrollers.push_back( new PropertyControllerioScrollBarEX() ); // scrollbar
		m_pgcontrollers.push_back( new PropertyControllerioRichLabel() );
	}

	// init timer
	{
		Connect( TIMERID_RENDER, wxEVT_TIMER, wxTimerEventHandler( wxRichTextEditorFrame::OnTimer ) );	
		m_timer = new wxTimer();
		m_timer->SetOwner( this, TIMERID_RENDER );
		m_timer->Start( 1, wxTIMER_CONTINUOUS );
	}

	// init file drag event
	{
		DragAcceptFiles( true );
		Connect( wxEVT_DROP_FILES, wxDropFilesEventHandler( wxRichTextEditorFrame::OnDropFiles ) );
	}
	
	// unselect
	m_inputprocessor.UnSelectAll();

	//
	SyncAll();
}

wxRichTextEditorFrame::~wxRichTextEditorFrame()
{
	// disconnect
	Disconnect( TIMERID_RENDER, wxEVT_TIMER, wxTimerEventHandler( wxRichTextEditorFrame::OnTimer ) );	
	Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler( wxRichTextEditorFrame::OnDropFiles ) );
	m_propertygridevent->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( wxRichTextEditorFrame::OnEventPropertyGridChanged ), NULL, this );
	m_propertygrid->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( wxRichTextEditorFrame::OnPropertyGridChanged ), NULL, this );

	EventHooker::UnregisterFor( this );
	this->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( wxRichTextEditorFrame::OnHookLButtonUp ) );

	int num = static_cast<int>( m_pgcontrollers.size() );
	for ( int i = 0; i < num; ++i ) {
		delete m_pgcontrollers[ i ];
	}
	m_pgcontrollers.clear();
		
	DEL( m_visualizer );

	DEL( m_soundmgr );	
	DEL( m_shadermgr );	
	DEL( m_fontmgr );
	DEL( m_guimgr );
	DEL( m_camera );
	DEL( m_inputbox );	
	DEL( m_texturemgr );
	DEL( m_uirenderer );
	DEL( m_vxbuffermgr );
	DEL( m_ixbuffermgr );
	
	ioResourceLoader::ReleaseInstance();
	ReleaseRenderSystem();

	DEL( m_frametimer );

	m_timer->Stop();
	DEL( m_timer );	
}

//----------------------------------------------------------------------------------
/*
wxWindow / wxFrame Override
*/

//----------------------------------------------------------------------------------
/*
wxMainFrame Override
*/

// ToolBox
void	wxRichTextEditorFrame::OnToolBoxControlDown( wxMouseEvent& event )
{
	m_inputprocessor.OnToolBoxControlDown( event );
}

// Displayer
void	wxRichTextEditorFrame::OnDisplayerKeyDown( wxKeyEvent& event )
{
	m_inputprocessor.OnDisplayerKeyDown( event );
}

void	wxRichTextEditorFrame::OnDisplayerKeyUp( wxKeyEvent& event )
{
	m_inputprocessor.OnDisplayerKeyUp( event );

	event.Skip();
}

void	wxRichTextEditorFrame::OnDisplayerLButtonDown( wxMouseEvent& event )
{
	m_inputprocessor.OnDisplayerLButtonDown( event );

	m_displayer->CaptureMouse();
	m_displayer->SetFocus();

	event.Skip();
}

void	wxRichTextEditorFrame::OnDisplayerLButtonUp( wxMouseEvent& event )
{
	if ( m_inputprocessor.IsLButtonDown() ) {
		m_displayer->ReleaseMouse();
	}
	m_displayer->SetFocus();

	m_inputprocessor.OnDisplayerLButtonUp( event );

	event.Skip();
}

void	wxRichTextEditorFrame::OnDisplayerMouseMove( wxMouseEvent& event )
{
	m_inputprocessor.OnDisplayerMouseMove( event );

	if ( event.LeftIsDown() ) {
		m_displayer->SetFocus();
	}

	event.Skip();
}


void	wxRichTextEditorFrame::OnDisplayerPaint( wxPaintEvent& event )
{
	Render();

	event.Skip();
}

// PropertyTab
void	wxRichTextEditorFrame::OnChangePropertyTab( wxNotebookEvent& event )
{
	m_inputprocessor.SetupProperty();

	event.Skip();
}

// Menu
void	wxRichTextEditorFrame::OnNew( wxCommandEvent& event )
{
	ProjectManager::GetSingleton().OnNew();

	event.Skip();
}

void	wxRichTextEditorFrame::OnSave( wxCommandEvent& event )
{
	ProjectManager::GetSingleton().OnSave( false );

	event.Skip();
}

void	wxRichTextEditorFrame::OnSaveAs( wxCommandEvent& event )
{
	ProjectManager::GetSingleton().OnSaveAs();

	event.Skip();
}

void	wxRichTextEditorFrame::OnLoad( wxCommandEvent& event )
{
	ProjectManager::GetSingleton().OnLoad();

	event.Skip();
}

void	wxRichTextEditorFrame::OnSummonIDSettingBox( wxCommandEvent& event )
{
	wxIDSettingBox::WndIDVec wndids;
	ioWnd* selwnd = GetInputProcessor().GetEditData().GetSelectWnd();
	if ( selwnd ) 
	{
		wndids.push_back( selwnd->GetID() );
		int subnum = GetInputProcessor().GetEditData().GetNumSubSelect();
		for ( int i = 0; i < subnum; ++i )
		{
			ioWnd* subwnd = GetInputProcessor().GetEditData().GetSubSelectWnd( i );
			if ( subwnd ) {
				wndids.push_back( subwnd->GetID() );
			}
		}
	}
	wxIDSettingBox* idsettingbox = new wxIDSettingBox( this, wndids );
	idsettingbox->Show();

	event.Skip();
}

// Toolbar
void	wxRichTextEditorFrame::OnClickToolAlignLeft( wxCommandEvent& event )
{
	m_inputprocessor.OnClickToolAlignLeft();

	event.Skip();
}

void	wxRichTextEditorFrame::OnClickToolAlignRight( wxCommandEvent& event )
{
	m_inputprocessor.OnClickToolAlignRight();

	event.Skip();
}

void	wxRichTextEditorFrame::OnClickToolAlignTop( wxCommandEvent& event )
{
	m_inputprocessor.OnClickToolAlignTop();

	event.Skip();
}

void	wxRichTextEditorFrame::OnClickToolAlignBottom( wxCommandEvent& event )
{
	m_inputprocessor.OnClickToolAlignBottom();

	event.Skip();
}

void	wxRichTextEditorFrame::OnClickToolCorrectGapLeftRight( wxCommandEvent& event )
{
	m_inputprocessor.OnClickToolCorrectGapLeftRight();

	event.Skip();
}

void	wxRichTextEditorFrame::OnClickToolCorrectGapTopBottom( wxCommandEvent& event )
{
	m_inputprocessor.OnClickToolCorrectGapTopBottom();

	event.Skip();
}

void	wxRichTextEditorFrame::OnClickToolCorrectSizeLeftRight( wxCommandEvent& event )
{
	m_inputprocessor.OnClickToolCorrectSizeLeftRight();

	event.Skip();
}

void	wxRichTextEditorFrame::OnClickToolCorrectSizeTopBottom( wxCommandEvent& event )
{
	m_inputprocessor.OnClickToolCorrectSizeTopBottom();

	event.Skip();
}

void	wxRichTextEditorFrame::OnClickToolCorrectSizeAll( wxCommandEvent& event )
{
	m_inputprocessor.OnClickToolCorrectSizeAll();

	event.Skip();
}

// InheritTree
void	wxRichTextEditorFrame::OnInheritTreeKeyDown( wxKeyEvent& event )
{
	m_inputprocessor.OnInheritTreeKeyDown( event );

	event.Skip();
}

void	wxRichTextEditorFrame::OnInheritTreeKeyUp( wxKeyEvent& event )
{
	m_inputprocessor.OnInheritTreeKeyUp( event );

	event.Skip();
}

void	wxRichTextEditorFrame::OnInheritTreeBoxSelect( wxTreeEvent& event )
{	
	m_inputprocessor.OnInheritTreeBoxSelect( event );	
	
	event.Skip();
}

// ETC
void	wxRichTextEditorFrame::OnCmd( wxCommandEvent& event )
{
	event.Skip();
}

void	wxRichTextEditorFrame::OnCmd2( wxCommandEvent& event )
{
	event.Skip();
}

//----------------------------------------------------------------------------------
/*
PropertyGrid Event
*/
void	wxRichTextEditorFrame::OnPropertyGridChanged( wxPropertyGridEvent& event )
{
	int type = m_inputprocessor.GetEditData().GetSelectType();
	if ( type >= 0 && type < static_cast<int>( m_pgcontrollers.size() ) ) {
		m_pgcontrollers[ type ]->OnChange( m_propertygrid, m_inputprocessor.GetSelectWnd(), event );
	} else {
		ProjectManager::GetSingleton().OnChangeGrid( m_propertygrid, 0, event );
	}

	SyncProperty();

	event.Skip();
}

void	wxRichTextEditorFrame::OnEventPropertyGridChanged( wxPropertyGridEvent& event )
{
	int type = m_inputprocessor.GetEditData().GetSelectType();
	if ( type >= 0 && type < static_cast<int>( m_pgcontrollers.size() ) ) {
		m_pgcontrollers[ type ]->OnChangeEventTab( m_propertygridevent, m_inputprocessor.GetSelectWnd(), event );
	} else {
		ProjectManager::GetSingleton().OnChangeGridEventTab( m_propertygridevent, 0, event );
	}

	SyncProperty();

	event.Skip();
}

//----------------------------------------------------------------------------------
/*
Hooking Event
*/
void	wxRichTextEditorFrame::OnHookLButtonUp( wxMouseEvent& event )
{
	m_inputprocessor.OnHookLButtonUp( event );

	event.Skip();
}


//----------------------------------------------------------------------------------
/*
Get
*/
void	wxRichTextEditorFrame::GetioWndIDInfos( std::map< wxString, int >& out )
{
	out.clear();
	
	int num = m_inputprocessor.GetEditData().GetNum();
	for ( int i = 0; i < num; ++i )
	{
		ioWnd* wnd = m_inputprocessor.GetEditData().GetWnd( i );
		out.insert( std::map< wxString, int >::value_type( wnd->GetIDName(), wnd->GetID() ) );
	}
}

void	wxRichTextEditorFrame::GetEventFunctionInfos( HeaderFileGenerator::EventFuncInfoSet& out, HeaderFileGenerator::EventFuncConnectInfoSet& out2 )
{
	out.clear();

	int num = m_inputprocessor.GetEditData().GetNum();
	for ( int i = 0; i < num; ++i )
	{
		ioWndEX* wnd = dynamic_cast<ioWndEX*>( m_inputprocessor.GetEditData().GetWnd( i ) );
		if ( wnd )
		{
			for ( int i = 0; i < WEVT_MAX; ++i )
			{
				const char* name = wnd->GetConnectFunctionName( static_cast<WNDEX_EVENT_TYPE>( i ) );
				if ( name )
				{
					out.insert( HeaderFileGenerator::EventFuncInfo( name, i ) );
					out2.insert( HeaderFileGenerator::EventFuncConnectInfo( name, i, wnd->GetID() ) );
				}
			}
		}
	}
}

void	wxRichTextEditorFrame::GetMemberInfos( HeaderFileGenerator::MemeberInfoSet& out )
{
	out.clear();

	int num = m_inputprocessor.GetEditData().GetNum();
	for ( int i = 0; i < num; ++i )
	{
		ioWndEX* wnd = dynamic_cast<ioWndEX*>( m_inputprocessor.GetEditData().GetWnd( i ) );
		if ( wnd )
		{
			const char* headerfilename = wnd->GetHeaderFileName();
			const char* classname = wnd->GetDefClassName();
			const char* membername = wnd->GetMemberName();
			int id = wnd->GetID();
			if ( classname && membername && headerfilename ) {
				out.insert( HeaderFileGenerator::MemeberInfo( id, classname, membername, headerfilename ) );
			}
		}
	}
}

ioWnd*	wxRichTextEditorFrame::GetSelectioWnd()
{
	return m_inputprocessor.GetSelectWnd();
}

IDirect3DDevice9*	wxRichTextEditorFrame::GetD3DDevice()
{
	return m_renderer->GetDevice();
}

bool	wxRichTextEditorFrame::IsPropertyTab() const
{
	return ( m_notebook2->GetCurrentPage() == m_panel141 );
}

//----------------------------------------------------------------------------------
/*
Event
*/
void	wxRichTextEditorFrame::OnTimer( wxTimerEvent& evt )
{
	switch ( evt.GetId() )
	{
	case TIMERID_RENDER:
		Render();
		break;
	}
}

void	wxRichTextEditorFrame::OnDropFiles( wxDropFilesEvent& event )
{
	if ( event.GetNumberOfFiles() <= 0 ) {
		return;
	}

	wxString* filename = event.GetFiles();

	ProjectManager::GetSingleton().OnLoad( filename[ 0 ].c_str() );

	event.Skip();
}

//----------------------------------------------------------------------------------
/*
sync
*/
void	wxRichTextEditorFrame::SyncAll()
{
	DWORD selid = -1;
	ioWnd* selwnd = m_inputprocessor.GetSelectWnd();
	if ( selwnd ) {
		selid = selwnd->GetID();
	}
	ioWnd* rootwnd = m_inputprocessor.GetEditData().GetRootWnd();

	SyncAll( rootwnd, selid );
}

void	wxRichTextEditorFrame::SyncAll( ioWnd* root, DWORD selid )
{
	m_inputprocessor.GetEditData().Clear();
	m_InheritTree->DeleteAllItems();

	if ( !root ) {
		return;
	}	

	int type = GlobalRef::ConvertToTBT( root->GetTemplateName(), -1 );
	m_inputprocessor.GetEditData().SetRootWnd( root );
	wxTreeItemId rootitem = m_InheritTree->AddRoot( root->GetTemplateName(), type );
	m_inputprocessor.GetEditData().InsertTW( rootitem, root, type );

	SyncAllRecursive( rootitem, root, selid );

	SyncProperty();

	m_InheritTree->ExpandAll();

	if ( root->GetID() == selid ) {
		m_inputprocessor.MainSelect( root );
	}

	CorrectWndID();
}

void	wxRichTextEditorFrame::SyncAllRecursive( const wxTreeItemId& parentitem, ioWnd* parentwnd, DWORD selid )
{
	int numchild = parentwnd->GetNumChildren();
	for ( int i = 0; i < numchild; ++i )
	{
		ioWnd* wnd = parentwnd->GetChild( i );
		if ( !wnd ) {
			continue;
		}

		int type = GlobalRef::ConvertToTBT( wnd->GetTemplateName(), -1 );
		wxTreeItemId newid = m_InheritTree->AppendItem( parentitem, wnd->GetTemplateName(), type );
		m_inputprocessor.GetEditData().InsertTW( newid, wnd, type );

		if ( wnd->GetID() == selid ) {
			m_inputprocessor.MainSelect( wnd );
		}

		SyncAllRecursive( newid, wnd, selid );
	}
}

void	wxRichTextEditorFrame::Sync( const wxTreeItemId& parentitem, ioWnd* parentwnd )
{
	if ( parentwnd ) 
	{
		SyncAllRecursive( parentitem, parentwnd, -1 );
		m_InheritTree->Expand( parentitem );
	}
}

void	wxRichTextEditorFrame::SyncProperty()
{
	int type = m_inputprocessor.GetEditData().GetSelectType();
	if ( type >= 0 && type < static_cast<int>( m_pgcontrollers.size() ) ) {
		if ( IsPropertyTab() ) {
			m_pgcontrollers[ type ]->Sync( m_propertygrid, m_inputprocessor.GetSelectWnd() );
		} else {
			m_pgcontrollers[ type ]->SyncEventTab( m_propertygridevent, m_inputprocessor.GetSelectWnd() );
		}
	} else {
		if ( IsPropertyTab() ) {
			ProjectManager::GetSingleton().SyncGrid( m_propertygrid, 0 );
		} else {
			ProjectManager::GetSingleton().SyncGridEventTab( m_propertygridevent, 0 );
		}
	}

	m_propertygrid->ClearIgnoreSet();
	m_propertygridevent->ClearIgnoreSet();
}

//----------------------------------------------------------------------------------
/*
ETC
*/
void	wxRichTextEditorFrame::ClearEditWnds( bool addroot )
{	
	if ( m_guimgr )
	{
		m_inputprocessor.UnSelectAll();
		m_inputprocessor.GetEditData().Clear();

		wxSize size = GetDisplayWnd()->GetSize();
		m_guimgr->ClearAllGUI();
		m_guimgr->InitDeskTop( size.GetWidth(), size.GetHeight() );
		m_uniqueid = 0;

		if ( addroot ) {
			ioWnd* pWnd = AddWnd( -100 );
			if( pWnd )
				pWnd->SetSize( size.GetWidth(), size.GetHeight() );
		}
		SyncAll();
	}
}

//----------------------------------------------------------------------------------
/*
Render
*/
void	wxRichTextEditorFrame::Render()
{
	RenderEngine();	

	m_inputprocessor.Render();
}

void	wxRichTextEditorFrame::RenderEngine()
{
	if ( !m_renderer || !m_guimgr ) {
		return;
	}

	m_renderer->CheckLostDevice( 100 );
	m_renderer->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF7758A5 );
	{
		m_renderer->SetFogEnable( false );
		m_renderer->SetLightEnable( true );
		m_renderer->SetCullingMode( CM_CCW );
		m_renderer->SetTextureFiltering( 0, TFO_BILINEAR );
		m_renderer->SetTextureFiltering( 1, TFO_BILINEAR );
		
		ioLookAtCameraInput caminput;
		m_camera->Update( &caminput );

		if ( m_inputprocessor.GetMouse() ) {
			m_guimgr->UpdateGUI( *( m_inputprocessor.GetMouse() ) );
		}

		m_guimgr->RenderGUI();
		
		m_uirenderer->Render( m_renderer );

		RenderUIRect();
	}
	m_renderer->Present();
}

void	wxRichTextEditorFrame::RenderUIRect()
{
	if ( !m_renderer || !m_visualizer ) {
		return;
	}

	ioWnd* wnd = m_inputprocessor.GetSelectWnd();
	if ( wnd ) 
	{
		// select render
		m_visualizer->RenderRect( wnd, 0xFF00FF00, 1.0f, false );

		// sub select render
		int numsub = m_inputprocessor.GetEditData().GetNumSubSelect();
		for ( int i = 0; i < numsub; ++i )
		{
			ioWnd* subwnd = m_inputprocessor.GetEditData().GetSubSelectWnd( i );
			if ( subwnd ) {
				m_visualizer->RenderRect( subwnd, 0x9FFFFF00, 0.5f, true );
			}
		}
	}

	// drawprimitives
	m_visualizer->Render( m_renderer );
}

//----------------------------------------------------------------------------------
/*
ETC
*/
void	wxRichTextEditorFrame::BuildWndID( ioWnd* wnd )
{
	if ( !wnd ) {
		return;
	}

	wnd->SetIDEX( m_uniqueid, false );
}

void	wxRichTextEditorFrame::CorrectWndID()
{
	ioWnd* wnd = m_guimgr->GetDeskTop();
	if ( !m_guimgr || !wnd ) {
		return;
	}

	int maxid = 0;
	std::set<int> ids;	
	int num = wnd->GetNumChildren();
	for ( int i = 0; i < num; ++i ) {
		CorrectWndID( wnd->GetChild( i ), ids, maxid, true ); // 최상위 윈도우는 ID시스템에서 열외한다.
	}

	CorrectUniqueid();
}

void	wxRichTextEditorFrame::CorrectWndID( ioWnd* wnd, std::set<int>& ids, int& maxid, bool ignoreSetid )
{
	if ( !wnd ) {
		return;
	}

	if ( !ignoreSetid )
	{
		maxid = std::max<int>( maxid, wnd->GetID() );

		std::set<int>::iterator iter = ids.find( wnd->GetID() );
		if ( iter == ids.end() )
		{
			ids.insert( wnd->GetID() );		
		}
		else
		{
			wnd->SetID( ++maxid );
			ids.insert( maxid );
		}
	}

	int num = wnd->GetNumChildren();
	for ( int i = 0; i < num; ++i )
	{
		ioWnd* child = wnd->GetChild( i );
		if ( child ) {
			CorrectWndID( child, ids, maxid, false );
		}
	}
}

void	wxRichTextEditorFrame::CorrectUniqueid()
{
	ioWnd* wnd = m_guimgr->GetDeskTop();
	if ( !m_guimgr || !wnd )
	{
		m_uniqueid = 0;
		return;
	}

	m_uniqueid = 0;	
	int num = wnd->GetNumChildren();
	for ( int i = 0; i < num; ++i ) {
		CorrectUniqueid( wnd->GetChild( i ), true );
	}
	++m_uniqueid;
}

void	wxRichTextEditorFrame::CorrectUniqueid( ioWnd* wnd, bool ignoreSetid )
{
	if ( !wnd ) {
		return;
	}

	if ( !ignoreSetid ) {
		m_uniqueid = std::max<int>( wnd->GetID(), m_uniqueid );
	}

	int num = wnd->GetNumChildren();
	for ( int i = 0; i < num; ++i )
	{
		ioWnd* child = wnd->GetChild( i );
		if ( child ) {
			CorrectUniqueid( child, false );
		}
	}
}

ioWnd*	wxRichTextEditorFrame::AddWnd( const wxWindowID& id )
{
	if ( id == -1 ) {
		return 0;
	}

	ioWnd* newwnd = 0;
	wxTreeItemId newid;

	int treeselimage = GlobalRef::ConvertToTBT( id, -1 );
	wxString treetext = GlobalRef::ConvertToString( treeselimage, "None" ).c_str();	

	if ( !m_inputprocessor.GetEditData().GetRootWnd() ) 
	{
		m_inputprocessor.GetEditData().Clear();
		m_InheritTree->DeleteAllItems();

		newwnd = AddWndToEngine( m_guimgr->GetDeskTop(), id );
		newid = m_InheritTree->AddRoot( treetext, treeselimage );

		m_inputprocessor.GetEditData().SetRootWnd( newwnd );

		m_InheritTree->Expand( newid );
	}
	else 
	{
		ioWnd* selwnd = m_inputprocessor.GetSelectWnd();
		wxTreeItemId selid = m_inputprocessor.GetEditData().GetSelectTreeItem();
		if ( !selwnd ) 
		{
			selwnd = m_inputprocessor.GetEditData().GetRootWnd();
			selid = m_inputprocessor.GetEditData().Get( selwnd );
		}

		//ioWndEX* exwnd = dynamic_cast<ioWndEX*>( selwnd );
		//if ( exwnd && exwnd->GetEXType() == IWEXT_SCROLLBAR ) {// 스크롤바는 자식생성 금지
		//	return 0;
		//}

		newwnd = AddWndToEngine( selwnd, id );
		newid = m_InheritTree->AppendItem( selid, treetext, treeselimage );

		m_InheritTree->Expand( selid );
	}

	m_inputprocessor.GetEditData().InsertTW( newid, newwnd, treeselimage );
	return newwnd;
}

ioWnd*	wxRichTextEditorFrame::AddWndToEngine( ioWnd* parent, const wxWindowID& id )
{
	int TBTType = GlobalRef::ConvertToTBT( id, -1 );

	ioWnd* wnd = CreateWnd( TBTType );
	parent->AddChild( wnd );
	wnd->SetID( m_uniqueid++ );
	wnd->iwm_create();
	wnd->ShowWnd();

	SetDefaultSetting( TBTType, wnd );

	return wnd;
}

ioWnd*	wxRichTextEditorFrame::CreateWnd( int TBTType )
{
	ioWnd* ret = 0;
	switch ( TBTType )
	{
	case TBT_BUTTON:
		ret = new ioButton();
		break;

	case TBT_RADIO:
		ret = new ioRadioButton();
		break;

	case TBT_CHECK:
		ret = new ioCheckButton();
		break;

	case TBT_EDIT:
		ret = new ioEdit();
		break;

	case TBT_PROGRESSBAR:
		ret = new ioProgressBar();
		break;
		
	case TBT_SLIDE:
		ret = new ioSlide();
		break;

	case TBT_ACTIVESCREENBTN:
		ret = new ioActiveScreenBtn();
		break;
		
	case TBT_SCROLL:
		ret = new ioScroll();
		break;

	case TBT_WNDEX:
		ret = new ioWndEX();
		break;

	case TBT_FRAMEWND:
		ret = new ioFrameWndEX( m_guimgr );
		break;

	case TBT_IMAGEWND:
		ret = new ioImageWndEX( m_guimgr );
		break;

	case TBT_LABELWND:
		ret = new ioLabelWndEX();
		break;

	case TBT_FRAMEBUTTON:
		ret = new ioFrameButtonWndEX( m_guimgr );
		break;

	case TBT_IMAGEBUTTON:
		ret = new ioImageButtonWndEX( m_guimgr );
		break;

	case TBT_CHECKBUTTON:
		ret = new ioCheckBoxEX( m_guimgr );
		break;

	case TBT_FLASHPLAYER:
		ret = new ioFlashPlayer();
		break;

	case TBT_RADIOWND:
		ret = new ioRadioWndEX();
		break;

	case TBT_RADIOFRAMEBUTTON:
		ret = new ioRadioFrameButtonEX( m_guimgr );
		break;

	case TBT_RADIOIMAGEBUTTON:
		ret = new ioRadioImageButtonEX( m_guimgr );
		break;

	case TBT_TABFRAMEBUTTON:
		ret = new ioTabFrameButtonEX( m_guimgr );
		break;

	case TBT_TABWND:
		ret = new ioTabWndEX( m_guimgr );
		break;

	case TBT_SCROLLBAR:
		ret = new ioScrollBarEX( m_guimgr );
		break;

	case TBT_RICHLABEL:
		ret = new ioRichLabel();
		break;

	case TBT_WND:
	default:
		ret = new ioWnd();
		break;
	}

	return ret;
}

void	wxRichTextEditorFrame::SetDefaultSetting( int TBTType, ioWnd* wnd )
{
	if ( !wnd ) {
		return;
	}

	// 기본 셋으로 생성되도록 한다.	
	if ( !m_guimgr ) {
		return;
	}
	
	// 각 컨트롤들 기본정보를 셋팅한다.
	switch ( TBTType )
	{
	case TBT_WND:
		{
			static const int defwndwidth = 300;
			static const int defwndheight = 200;

			// 반드시 윈도우의 싸이즈를 먼저 셋팅해줘야... 아래것들이 셋팅이 된다.m_InheritTree
			wnd->SetWidth( defwndwidth );
			wnd->SetHeight( defwndheight );

			//! 주의 : 반드시 Template먼저 셋팅해줘야 생성되서 뒤에 인자들이 셋팅된다.
			wnd->SetFrameRenderTemplete( "None", 0, "win2_frame90", m_guimgr );
			wnd->SetFrameRenderRectWidth( "None", 0, defwndwidth );
			wnd->SetFrameRenderRectHeight( "None", 0, defwndheight );
		}
		break;

	case TBT_BUTTON:
	case TBT_ACTIVESCREENBTN:
		{
			static const int defbtnwidth = 100;
			static const int defbtnheight = 31;

			// 반드시 윈도우의 싸이즈를 먼저 셋팅해줘야... 아래것들이 셋팅이 된다.m_InheritTree
			wnd->SetWidth( defbtnwidth );
			wnd->SetHeight( defbtnheight );

			//! 주의 : 반드시 Template먼저 셋팅해줘야 생성되서 뒤에 인자들이 셋팅된다.
			wnd->SetFrameRenderTemplete( "Normal", -1, "bt006_red_nor", m_guimgr );
			wnd->SetFrameRenderRectWidth( "Normal", -1, defbtnwidth );
			wnd->SetFrameRenderRectHeight( "Normal", -1, defbtnheight );
			wnd->SetFrameRenderTemplete( "Over", -1, "bt006_red_nor", m_guimgr );
			wnd->SetFrameRenderRectWidth( "Over", -1, defbtnwidth );
			wnd->SetFrameRenderRectHeight( "Over", -1, defbtnheight );
			wnd->SetFrameRenderTemplete( "OverAdd", -1, "bt_overadd", m_guimgr );
			wnd->SetFrameRenderRectWidth( "OverAdd", -1, defbtnwidth );
			wnd->SetFrameRenderRectHeight( "OverAdd", -1, defbtnheight );
			wnd->SetFrameRenderTemplete( "Push", -1, "bt006_red_down", m_guimgr );
			wnd->SetFrameRenderRectWidth( "Push", -1, defbtnwidth );
			wnd->SetFrameRenderRectHeight( "Push", -1, defbtnheight );
		}
		break;

	case TBT_RADIO:
		{
			static const int defradiowidth = 89;
			static const int defradioheight = 23;

			wnd->SetWidth( defradiowidth );
			wnd->SetHeight( defradioheight );

			//! 주의 : 반드시 Template먼저 셋팅해줘야 생성되서 뒤에 인자들이 셋팅된다.
			wnd->SetFrameRenderTemplete( "Normal", -1, "tap3_off", m_guimgr );
			wnd->SetFrameRenderRectWidth( "Normal", -1, defradiowidth );
			wnd->SetFrameRenderRectHeight( "Normal", -1, defradioheight );
			wnd->SetFrameRenderTemplete( "Over", -1, "tap3_off", m_guimgr );
			wnd->SetFrameRenderRectWidth( "Over", -1, defradiowidth );
			wnd->SetFrameRenderRectHeight( "Over", -1, defradioheight );
			wnd->SetFrameRenderTemplete( "OverAdd", -1, "tap3_overadd", m_guimgr );
			wnd->SetFrameRenderRectWidth( "OverAdd", -1, defradiowidth );
			wnd->SetFrameRenderRectHeight( "OverAdd", -1, defradioheight );
			wnd->SetFrameRenderTemplete( "Push", -1, "tap3_on1", m_guimgr );
			wnd->SetFrameRenderRectWidth( "Push", -1, defradiowidth );
			wnd->SetFrameRenderRectHeight( "Push", -1, defradioheight );
			wnd->SetTitleText( "RadioButton" );
			wnd->SetTitleOffsetX( 44 );
			wnd->SetTitleOffsetY( 5 );
			wnd->SetTitleTextSize( 12 );
		}
		break;

	case TBT_CHECK:
		{
			static const int defcheckwidth = 16;
			static const int defcheckheight = 16;

			wnd->SetWidth( defcheckwidth );
			wnd->SetHeight( defcheckheight );

			//! 주의 : 반드시 ImageSet먼저 셋팅해줘야 생성되서 뒤에 인자들이 셋팅된다.
			wnd->SetRenderImageImageName( "Normal", -1, "BaseImage001", "check_box_off", m_guimgr );
			wnd->SetRenderImageImageName( "Over", -1, "BaseImage001", "check_box_off", m_guimgr );
			wnd->SetRenderImageImageName( "Push", -1, "BaseImage001", "check_box_off", m_guimgr );
			wnd->SetRenderImageImageName( "Check", -1, "BaseImage001", "check_box_on", m_guimgr );
			wnd->SetRenderImageImageName( "Disable", -1, "BaseImage001", "check_box_no", m_guimgr );
			wnd->SetTitleText( "CheckButton" );
			wnd->SetTitleOffsetX( 20 );
			wnd->SetTitleOffsetY( 2 );
			wnd->SetTitleTextSize( 14 );
		}
		break;

	case TBT_EDIT:
		{
			static const int defeditwidth = 200;
			static const int defeditheight = 30;

			// 반드시 윈도우의 싸이즈를 먼저 셋팅해줘야... 아래것들이 셋팅이 된다.m_InheritTree
			wnd->SetWidth( defeditwidth );
			wnd->SetHeight( defeditheight );
		}
		break;

	case TBT_PROGRESSBAR:
		{
			static const int defpbwidth = 200;
			static const int defpbheight = 30;

			// 반드시 윈도우의 싸이즈를 먼저 셋팅해줘야... 아래것들이 셋팅이 된다.m_InheritTree
			wnd->SetWidth( defpbwidth );
			wnd->SetHeight( defpbheight );
		}
		break;
		
	case TBT_SLIDE:
		{
			static const int defslidewidth = 15;
			static const int defslideheight = 30;

			wnd->SetWidth( defslidewidth );
			wnd->SetHeight( defslideheight );

			//! 주의 : 반드시 ImageSet먼저 셋팅해줘야 생성되서 뒤에 인자들이 셋팅된다.
			wnd->SetFrameRenderTemplete( "None", 0, "scrollbar_drag", m_guimgr );
			wnd->SetFrameRenderRectWidth( "None", 0, defslidewidth );
			wnd->SetFrameRenderRectHeight( "None", 0, defslideheight );
			wnd->SetRenderImageImageName( "None", 0, "BaseImage001", "scrollbar_drag_icon", m_guimgr );
			RECT rt = { 0, 0, 15, 30 };
			wnd->SetDragRect( rt );
			wnd->AddExStyle( IWS_EX_NOTIFY_MOVED, true );
			wnd->AddStyle( IWS_CANMOVE, true );
		}
		break;
		
	case TBT_SCROLL:
		{
			static const int defscrollwidth = 15;
			static const int defscrollheight = 378;

			wnd->SetWidth( defscrollwidth );
			wnd->SetHeight( defscrollheight );

			//! 주의 : 반드시 ImageSet먼저 셋팅해줘야 생성되서 뒤에 인자들이 셋팅된다.
			wnd->SetFrameRenderTemplete( "None", 0, "scrollbar_back", m_guimgr );
			wnd->SetFrameRenderRectX( "None", 0, 5 );
			wnd->SetFrameRenderRectY( "None", 0, 10 );
			wnd->SetFrameRenderRectWidth( "None", 0, 5 );
			wnd->SetFrameRenderRectHeight( "None", 0, 358 );
			wnd->AddExStyle( IWS_EX_VSCROLL, true );
			wnd->AddStyle( IWS_PROCESS, true );
		}
		break;

	case TBT_WNDEX:
		{
			ioWndEX* wndex = dynamic_cast<ioWndEX*>( wnd );
			if ( wndex )
			{
				static const int defwndexwidth = 800;
				static const int defwndexheight = 600;

				wndex->SetSize( defwndexwidth, defwndexheight );
			}
		}		
		break;

	case TBT_FRAMEWND:
		{
			ioFrameWndEX* wndex = dynamic_cast<ioFrameWndEX*>( wnd );
			if ( wndex )
			{
				static const int defframewndexwidth = 300;
				static const int defframewndexheight = 200;

				wndex->SetFrameTemplateName( "win1_frame90", m_guimgr );
				wndex->SetSize( defframewndexwidth, defframewndexheight );				
			}
		}
		break;

	case TBT_IMAGEWND:
		{
			ioImageWndEX* wndex = dynamic_cast<ioImageWndEX*>( wnd );
			if ( wndex )
			{
				static const int defframewndexwidth = 180;
				static const int defframewndexheight = 100;

				wndex->SetImageSet( "BaseImage000", "Loading_info", m_guimgr );
				wndex->SetSize( defframewndexwidth, defframewndexheight );				
			}
		}
		break;

	case TBT_LABELWND:
		{
			ioLabelWndEX* wndex = dynamic_cast<ioLabelWndEX*>( wnd );
			if ( wndex )
			{
				static const int defframewndexwidth = 60;
				static const int defframewndexheight = 27;

				wndex->SetSize( defframewndexwidth, defframewndexheight );
				wndex->SetLabelText( "Label", "" );
				wndex->SetLabelTextGap( 0 );
				wndex->SetLabelTextSize( 16 );
			}
		}
		break;

	case TBT_FRAMEBUTTON:
		{
			ioFrameButtonWndEX* wndex = dynamic_cast<ioFrameButtonWndEX*>( wnd );
			if ( wndex )
			{
				static const int defbtnwidth = 100;
				static const int defbtnheight = 31;

				wndex->SetFrameTemplateNameNormal( "bt006_blue_nor", m_guimgr );
				wndex->SetFrameTemplateNameOver( "bt006_blue_nor", m_guimgr );
				wndex->SetFrameTemplateNameOverAdd( "bt_overadd", m_guimgr );
				wndex->SetFrameTemplateNamePush( "bt006_blue_down", m_guimgr );
				wndex->SetLabelTextGap( 0 );
				wndex->SetLabelTextSize( 16 );
				wndex->SetSize( defbtnwidth, defbtnheight );
			}
		}
		break;

	case TBT_IMAGEBUTTON:
		{
			ioImageButtonWndEX* wndex = dynamic_cast<ioImageButtonWndEX*>( wnd );
			if ( wndex )
			{
				static const int defbtnwidth = 53;
				static const int defbtnheight = 53;

				wndex->SetImageSetNormal( "BaseImage008", "bt015_blue_nor", m_guimgr );
				wndex->SetImageSetOver( "BaseImage008", "bt015_blue_nor", m_guimgr );
				wndex->SetImageSetOverAdd( "BaseImage008", "bt015_blue_nor", m_guimgr );
				wndex->SetImageSetPush( "BaseImage008", "bt015_blue_down", m_guimgr );
				wndex->SetLabelTextGap( 0 );
				wndex->SetLabelTextSize( 16 );
				wndex->SetSize( defbtnwidth, defbtnheight );
			}
		}
		break;

	case TBT_CHECKBUTTON:
		{
			ioCheckBoxEX* wndex = dynamic_cast<ioCheckBoxEX*>( wnd );
			if ( wndex )
			{
				static const int defcheckwidth = 16;
				static const int defcheckheight = 16;

				wndex->SetImageSetNormal( "BaseImage001", "check_box_off", m_guimgr );
				wndex->SetImageSetOver( "BaseImage001", "check_box_off", m_guimgr );
				wndex->SetImageSetPush( "BaseImage001", "check_box_off", m_guimgr );
				wndex->SetImageSetCheck( "BaseImage001", "check_box_on", m_guimgr );
				wndex->SetImageSetDisable( "BaseImage001", "check_box_no", m_guimgr );
				wndex->SetLabelText( "check me!", "" );
				wndex->SetLabelTextSize( 12 );
				wndex->SetLabelTextGap( 0 );
				wndex->SetLabelOffsetX( 20 );
				wndex->SetSize( defcheckwidth, defcheckheight );
			}
		}
		break;

	case TBT_FLASHPLAYER:
		{
			ioFlashPlayer* wndex = dynamic_cast<ioFlashPlayer*>( wnd );
			if ( wndex )
			{
				static const int defwidth = 300;
				static const int defheight = 200;
								
				wndex->SetSize( defwidth, defheight );
				wndex->Load( "bana.swf" );
			}
		}
		break;

	case TBT_RADIOWND:
		{
			ioRadioWndEX* wndex = dynamic_cast<ioRadioWndEX*>( wnd );
			if ( wndex )
			{
				static const int defwidth = 100;
				static const int defheight = 70;

				wndex->SetSize( defwidth, defheight );
			}
		}
		break;

	case TBT_RADIOIMAGEBUTTON:
		{
			ioImageButtonWndEX* wndex = dynamic_cast<ioImageButtonWndEX*>( wnd );
			if ( wndex )
			{
				static const int defbtnwidth = 16;
				static const int defbtnheight = 16;

				wndex->SetImageSetNormal( "BaseImage001", "check_radio_off", m_guimgr );
				wndex->SetImageSetOver( "BaseImage001", "check_radio_off", m_guimgr );
				wndex->SetImageSetPush( "BaseImage001", "check_radio_on", m_guimgr );
				wndex->SetImageSetDisable( "BaseImage001", "check_radio_no", m_guimgr );
				wndex->SetSize( defbtnwidth, defbtnheight );
			}
		}
		break;

	case TBT_RADIOFRAMEBUTTON:
	case TBT_TABFRAMEBUTTON:
		{
			ioFrameButtonWndEX* wndex = dynamic_cast<ioFrameButtonWndEX*>( wnd );
			if ( wndex )
			{
				static const int defbtnwidth = 100;
				static const int defbtnheight = 22;

				wndex->SetFrameTemplateNameNormal( "tap4_off", m_guimgr );
				wndex->SetFrameTemplateNameOver( "tap4_off", m_guimgr );
				wndex->SetFrameTemplateNameOverAdd( "tap4_overadd", m_guimgr );
				wndex->SetFrameTemplateNamePush( "tap4_on", m_guimgr );
				wndex->SetLabelText( "Tab", "" );
				wndex->SetLabelTextGap( 0 );
				wndex->SetLabelTextSize( 16 );
				wndex->SetSize( defbtnwidth, defbtnheight );
			}
		}
		break;

	case TBT_TABWND:
		{
			ioTabWndEX* wndex = dynamic_cast<ioTabWndEX*>( wnd );
			if ( wndex )
			{
				static const int defbtnwidth = 600;
				static const int defbtnheight = 400;

				wndex->SetSize( defbtnwidth, defbtnheight );
			}
		}
		break;

	case TBT_SCROLLBAR:
		{
			ioScrollBarEX* wndex = dynamic_cast<ioScrollBarEX*>( wnd );
			if ( wndex )
			{
				static const int defbtnwidth = 5;
				static const int defbtnheight = 200;

				wndex->SetFrameTemplateName( "scrollbar6_back", m_guimgr );
				wndex->SetSize( defbtnwidth, defbtnheight );

				wndex->SetScroll( "scrollbar6_drag" );
				wndex->SetScrollWidth( 15 );
				wndex->SetScrollHeight( 35 );

				wndex->SetDecBtnTemplateNameNormal( "slot_empty_white" );
				wndex->SetDecBtnTemplateNameOver( "slot_empty_white" );
				wndex->SetDecBtnTemplateNameOverAdd( "slot_empty_white" );
				wndex->SetDecBtnTemplateNamePush( "slot_empty_white" );
				wndex->SetDecBtnWidth( 20 );
				wndex->SetDecBtnHeight( 20 );
				wndex->SetDecBtnLabelText( "▲" );
				wndex->SetDecBtnLabelGap( 0 );
				wndex->SetDecBtnLabelVertAlign( TVA_CENTER );
				wndex->SetDecBtnLabelHorzAlign( TAT_CENTER );

				wndex->SetIncBtnTemplateNameNormal( "slot_empty_white" );
				wndex->SetIncBtnTemplateNameOver( "slot_empty_white" );
				wndex->SetIncBtnTemplateNameOverAdd( "slot_empty_white" );
				wndex->SetIncBtnTemplateNamePush( "slot_empty_white" );
				wndex->SetIncBtnWidth( 20 );
				wndex->SetIncBtnHeight( 20 );
				wndex->SetIncBtnLabelText( "▼" );
				wndex->SetIncBtnLabelGap( 0 );
				wndex->SetIncBtnLabelVertAlign( TVA_CENTER );
				wndex->SetIncBtnLabelHorzAlign( TAT_CENTER );
			}
		}
		break;

	case TBT_RICHLABEL:
		{
			ioRichLabel* wndex = dynamic_cast<ioRichLabel*>( wnd );
			if ( wndex )
			{
				static const int defwidth = 110;
				static const int defheight = 20;

				wndex->SetSize( defwidth, defheight );
				wndex->AddText( 20, TS_NORMAL, 0xFF000000, "RichLabel" );
			}
		}
		break;
	}
}