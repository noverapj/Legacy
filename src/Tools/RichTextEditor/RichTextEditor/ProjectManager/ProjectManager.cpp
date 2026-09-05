//-------------------------------------------------------------------
//-------------------------------------------------------------------
// 작성자	: cha
// 날짜		: 2013.10.24
// 이름		: ProjectManager
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#include "stdafx.h"
#include "ProjectManager.h"
#include "../wxWidgetControls/PropertyControllerProject.h"
#include "ioXMLDocument.h"
#include "ioXMLElement.h"
#include "../wxRichTextEditorFrame.h"
#include "ioWnd.h"
#include "ioGUIManager.h"
#include "HeaderFileGenerator.h"

template<> ProjectManager* Singleton< ProjectManager >::ms_Singleton;

//-------------------------------------------------------------
/*
생성
*/
ProjectManager::ProjectManager()
	: m_edituipath( wxEmptyString )
	, m_edituifilename( wxEmptyString )
	, m_codefilename( wxEmptyString )
	, m_classname( wxEmptyString )
	, m_baseclassname( wxEmptyString )
	, m_gridcontroller( 0 )
	, m_gmregfilename( wxEmptyString )
	, m_gmregclassname( wxEmptyString )
	, m_gmregheaderfilepath( wxEmptyString )
{
	m_gridcontroller = new PropertyControllerProject();
}

//-------------------------------------------------------------
/*
파괴
*/
ProjectManager::~ProjectManager()
{
	delete m_gridcontroller;
}

//-------------------------------------------------------------
/*
Save / Load Event
*/
void	ProjectManager::OnNew()
{
	if ( QuestionSave() == wxCANCEL ) {
		return;
	}

	Clear();

	wxRichTextEditorFrame::GetSingleton().ClearEditWnds( true );
}

void	CreateXMLChild( ioXMLElement& xml, const char* name, const char* text )
{
	ioXMLElement child = xml.CreateChild( name );
	child.SetText( text );
}

wxString	GetFileNameOnly( const char* src )
{
	wxString tmp = src;
	tmp.Replace( "\\", "/" );
	size_t fpos = tmp.rfind( "/" ) + 1;
	tmp = tmp.substr( 0, tmp.rfind( "." ) );
	tmp = tmp.substr( fpos, tmp.Len() - fpos );
	return tmp;
}

wxString	GetFileNameOnlyWithExtension( const char* src )
{
	wxString tmp = src;
	tmp.Replace( "\\", "/" );
	size_t fpos = tmp.rfind( "/" ) + 1;
	tmp = tmp.substr( fpos, tmp.Len() - fpos );
	return tmp;
}

wxString	CutExtension( const char* src )
{
	wxString tmp = src;
	return tmp.substr( 0, tmp.rfind( "." ) );
}

wxString	GetProjectFilename( const char* filename )
{
	return wxString::Format( "ToolResources/UITool/Projects/%s.uiproj", GetFileNameOnly( filename ).c_str() );
}

wxString	GetLocalPath( const char* path )
{
	wxString rfilename( path );
	rfilename = rfilename.Lower();

	char curbuff[ 512 ] = { 0, };
	GetCurrentDirectory( sizeof( curbuff ), curbuff );
	wxString cpath( curbuff );
	//cpath = cpath.Lower();

	size_t pos = rfilename.find( cpath );
	if ( pos >= rfilename.Len() ) {
		return path;
	}
	pos += cpath.Len();

	rfilename = rfilename.substr( pos, rfilename.Len() - pos );
	while ( rfilename.at( 0 ) == '\\' || rfilename.at( 0 ) == '/' ) {
		rfilename.erase( 0, 1 );
	}
	return rfilename;
}

void	ProjectManager::OnSave( bool skipanswer )
{
	if ( IsEmptyEditData() ) {
		return;
	}

	if ( m_edituipath == wxEmptyString )
	{
		OnSaveAs();
	}
	else 
	{
		if ( skipanswer ) {
			Save( m_edituipath.c_str() );
		} else if ( wxMessageBox( "정말로 저장하실건가요?", "물음", wxYES_NO ) == wxYES ) {
			Save( m_edituipath.c_str() );
		}
	}
}

void	ProjectManager::OnSaveAs()
{
	if ( IsEmptyEditData() ) {
		return;
	}

	wxFileDialog dlg( wxRichTextEditorFrame::GetSingletonPtr(),
					"Save",
					m_edituipath.c_str(),
					"",
					"Xml files (*.xml)|*.xml",
					wxFD_SAVE );
	if ( dlg.ShowModal() == wxID_OK )
	{
		if ( Save( dlg.GetPath().c_str() ) )
		{
			m_edituipath = dlg.GetPath().c_str();
			m_edituifilename = dlg.GetFilename().c_str();
		}
	}
}

void	ProjectManager::OnLoad()
{
	if ( QuestionSave() == wxCANCEL ) {
		return;
	}

	wxFileDialog dlg( wxRichTextEditorFrame::GetSingletonPtr(),
					"Choose a file",
					m_edituipath.c_str(),
					m_edituifilename.c_str(),
					"Xml files (*.xml)|*.xml|Project files (*.uiproj)|*.uiproj",
					wxFD_OPEN );
	if ( dlg.ShowModal() == wxID_OK ) {
		OnLoad( dlg.GetPath().c_str() );
	}
}

void	ProjectManager::OnLoad( const char* filename )
{
	if ( !filename ) {
		return;
	}

	wxString uifilename( wxEmptyString );
	if ( Load( filename, uifilename ) )
	{
		m_edituipath = uifilename;
		m_edituifilename = GetFileNameOnlyWithExtension( uifilename.c_str() );
	}
}

//-------------------------------------------------------------
/*
Save / Load
*/
bool	ProjectManager::Save( const char* filename )
{
	ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetInputProcessor().GetEditData().GetRootWnd();
	if ( !filename || !wnd ) 
	{
		wxMessageBox( "편집할 윈도우가 없거나 파일명이 올바르지 않습니다.", "Error" );
		return false;
	}

	wxString rfilename( GetLocalPath( filename ) );

	if ( m_classname == wxEmptyString ) {
		m_classname = wxString::Format( "%sBase", wnd->GetClassNameEX() );//GetFileNameOnly( rfilename.c_str() );
	}
	if ( m_baseclassname == wxEmptyString ) {
		m_baseclassname = "ioWndEX";
	}
	if ( m_codefilename == wxEmptyString ) {
		m_codefilename = wxString::Format( "%sBase.h", CutExtension( rfilename.c_str() ).c_str() );
	}

	// project file save
	ioXMLDocument doc;
	doc.CreateDocument( "Project", "1.0", "euc-kr", "" );

	ioXMLElement root = doc.GetRootElement();
	
	CreateXMLChild( root, "UIFileName", rfilename.c_str() );
	CreateXMLChild( root, "CodeFileName", m_codefilename.c_str() );
	CreateXMLChild( root, "ClassName", m_classname.c_str() );
	CreateXMLChild( root, "BaseClassName", m_baseclassname.c_str() );
	
	CreateXMLChild( root, "GUIRegisterFileName", m_gmregfilename.c_str() );
	CreateXMLChild( root, "GUIRegisterIncludeInfo", m_gmregheaderfilepath.c_str() );
	CreateXMLChild( root, "GUIRegisterClassName", m_gmregclassname.c_str() );

	if ( !doc.SaveXML( GetProjectFilename( rfilename.c_str() ).c_str() ) )
	{
		wxMessageBox( "프로젝트 파일 저장에 실패하였습니다.프로젝트 설정을 확인하세요.", "Error" );
		return false;
	}
		
	// ui file save
	if ( !wnd->Save( rfilename.c_str() ) ) 
	{
		wxMessageBox( "ui파일 저장에 실패하였습니다.", "Error" );
		return false;
	}

	// header file save
	if ( !GenerateCode() )
	{
		wxMessageBox( "Class Header 파일 저장에 실패하였습니다.프로젝트 설정을 확인하세요.", "Error" );
		return false;
	}

	//// guimgr 등록정보 갱신
	//if ( !EditGUIMgrRegisterCode() )
	//{
	//	wxMessageBox( "GUIManager 등록 정보를 갱신하는데 실패하였습니다.프로젝트 설정을 확인하세요.", "Error" );
	//	return false;
	//}

	return true;
}

bool	ProjectManager::LoadProjFile( const char* filename, wxString& uifilename )
{
	if ( !filename ) {
		return false;
	}

	ioXMLDocument doc;
	if ( !doc.LoadFromFile( filename ) ) {
		return false;
	}

	uifilename = wxEmptyString;
	ioXMLElement root = doc.GetRootElement();	
	ioXMLElement child = root.FirstChild();
	while ( !child.IsEmpty() )
	{
		const char* tag = child.GetTagName();
		if ( strcmp( tag, "UIFileName" ) == 0 ) {
			uifilename = child.GetText();
		} else if ( strcmp( tag, "CodeFileName" ) == 0 ) {
			m_codefilename = child.GetText();
		} else if ( strcmp( tag, "ClassName" ) == 0 ) {
			m_classname = child.GetText();
		} else if ( strcmp( tag, "BaseClassName" ) == 0 ) {
			m_baseclassname = child.GetText();
		} else if ( strcmp( tag, "GUIRegisterFileName" ) == 0 ) {
			m_gmregfilename = child.GetText();
		} else if ( strcmp( tag, "GUIRegisterIncludeInfo" ) == 0 ) {
			m_gmregheaderfilepath = child.GetText();
		} else if ( strcmp( tag, "GUIRegisterClassName" ) == 0 ) {
			m_gmregclassname = child.GetText();
		}
		child = child.NextSibling();
	}
	return true;
}

bool	ProjectManager::LoadUIFile( const char* filename )
{
	wxRichTextEditorFrame::GetSingleton().ClearEditWnds( false );

	// 로드할때는 미리 root를 생성해 놓으면 안된다!!
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !guimgr ) {
		return false;
	}	

	ioWnd* wnd = guimgr->AddWnd( filename );
	if ( !wnd ) {		
		return false;
	}

	wnd->ShowWnd();
	wxRichTextEditorFrame::GetSingleton().SyncAll( wnd, -1 );
	return true;
}

bool	ProjectManager::Load( const char* filename, wxString& uifilename )
{
	if ( !filename )
	{
		wxMessageBox( "파일명이 올바르지 않습니다.", "Error" );
		return false;
	}

	wxString rfilename( GetLocalPath( filename ) );

	if ( rfilename.find( ".xml" ) < rfilename.Len() ) {
		return LoadFromUIFile( rfilename.c_str(), uifilename );
	} else if ( rfilename.find( ".uiproj" ) < rfilename.Len() ) {
		return LoadFromProjFile( rfilename.c_str(), uifilename );
	}

	wxMessageBox( "지원하지 않는 포맷입니다.", "Error" );
	return false;
}

bool	ProjectManager::LoadFromProjFile( const char* filename, wxString& uifilename )
{
	if ( !filename ) {
		return false;
	}

	Clear();

	// project file load
	uifilename = wxEmptyString;
	if ( !LoadProjFile( filename, uifilename ) )
	{
		wxMessageBox( "프로젝트 파일 불러오기가 실패하였습니다.", "Error" );
		return false;
	}

	// ui file load
	if ( !LoadUIFile( uifilename.c_str() ) )
	{
		wxMessageBox( "ui 파일 불러오기가 실패하였습니다.", "Error" );
		return false;
	}

	return true;
}

bool	ProjectManager::LoadFromUIFile( const char* filename, wxString& uifilename )
{
	if ( !filename ) {
		return false;
	}

	Clear();

	// project file load ( proj 파일이 없을수 있다 )
	uifilename = wxEmptyString;
	LoadProjFile( GetProjectFilename( filename ).c_str(), uifilename );
	
	// ui file load
	if ( !LoadUIFile( filename ) )
	{
		wxMessageBox( "ui 파일 불러오기가 실패하였습니다.", "Error" );
		return false;
	}

	uifilename = filename;
	return true;
}

//-------------------------------------------------------------
/*
GenerateCode
*/
bool	ProjectManager::GenerateCode()
{
	if ( m_codefilename != wxEmptyString )
	{
		HeaderFileGenerator hfgen;

		if ( hfgen.Load( "ToolResources/UITool/ClassHeaderTemplateFile.h" ) )
		{
			time_t ltime;
			struct tm* today;
			time( &ltime );
			today = localtime( &ltime );

			hfgen.SetClassName( m_classname.c_str() );
			hfgen.SetBaseClassName( m_baseclassname.c_str() );
			hfgen.SetDate( wxString::Format( "%d.%d.%d_%d:%d:%d\n", today->tm_year, today->tm_mon, today->tm_mday, today->tm_hour, today->tm_min, today->tm_sec ).c_str() );
			hfgen.SetDefine( wxString::Format( "_%s_\n", m_classname.Upper().c_str() ).c_str() );
			hfgen.SetFilename( m_codefilename.c_str() );

			std::map< wxString, int > idinfos;
			wxRichTextEditorFrame::GetSingleton().GetioWndIDInfos( idinfos );
			hfgen.SetWndIDInfo( idinfos );

			HeaderFileGenerator::EventFuncInfoSet funcinfo;
			HeaderFileGenerator::EventFuncConnectInfoSet funcconnectinfo;
			wxRichTextEditorFrame::GetSingleton().GetEventFunctionInfos( funcinfo, funcconnectinfo );
			hfgen.SetEventFunctionInfo( funcinfo );
			hfgen.SetEventFunctionConnectInfo( funcconnectinfo );

			HeaderFileGenerator::MemeberInfoSet memberinfo;
			wxRichTextEditorFrame::GetSingleton().GetMemberInfos( memberinfo );
			hfgen.SetMemberInfo( memberinfo );
			
			return hfgen.Save( m_codefilename.c_str() );
		}
	}

	return false;
}

bool	ProjectManager::EditGUIMgrRegisterCode()
{
	if ( m_gmregfilename != wxEmptyString )
	{
		wxString buffer;
		if ( !HeaderFileGenerator::LoadFrom( m_gmregfilename.c_str(), buffer ) ) {
			return false;
		}

		std::ofstream stream( static_cast<const char*>( m_gmregfilename.c_str() ) );
		if ( !stream ) {
			return false;
		}		
		
		static const char* mkheader = "//#$%HEADERADDMARK%$#";
		if ( m_gmregheaderfilepath != wxEmptyString )
		{
			wxString include = wxString::Format( "#include \"%s\"", m_gmregheaderfilepath.c_str() );			
			if ( buffer.find( include.c_str() ) >= buffer.size() ) {
				buffer.Replace( mkheader, wxString::Format( "%s\n%s", include.c_str(), mkheader ) );
			}
		}

		static const char* mkadd = "//#$%REGADDMARK%$#";
		if ( m_gmregheaderfilepath != wxEmptyString ) 
		{
			wxString reg = wxString::Format( "\tguimgr->RegisterWndCreator<%s>( \"%s\" );", m_gmregclassname.c_str(), m_gmregclassname.c_str() );
			if ( buffer.find( reg.c_str() ) >= buffer.size() ) {
				buffer.Replace( mkadd, wxString::Format( "%s\n%s", reg.c_str(), mkadd ) );
			}
		}

		stream << buffer.c_str() << std::endl;
		stream.close();
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------
/*
Control
*/
void	ProjectManager::Clear()
{
	m_edituipath = wxEmptyString;
	m_edituifilename = wxEmptyString;
	m_codefilename = wxEmptyString;
	m_classname = wxEmptyString;
	m_baseclassname = wxEmptyString;
	
	//m_gmregfilename;
	m_gmregclassname = wxEmptyString;
	m_gmregheaderfilepath = wxEmptyString;
}

bool	ProjectManager::IsEmptyEditData()
{
	ioWnd* desktopwnd = wxRichTextEditorFrame::GetSingleton().GetGUIManager()->GetDeskTop();
	if ( !desktopwnd ) {
		return true;
	}
	
	if ( !desktopwnd->HasChild() ) {
		return true;
	}

	int a = desktopwnd->GetNumChildren();

	ioWnd* root = desktopwnd->GetChild( 0 );
	if ( !root ) {
		return true;
	}

	return !( root->HasChild() );
}

int		ProjectManager::QuestionSave()
{
	if ( !IsEmptyEditData() ) 
	{
		int res = wxMessageBox( "현재 작업을 저장할까요?", "물음", wxYES_NO | wxCANCEL );
		switch ( res )
		{
		case wxYES:		OnSave( true ); return wxYES;
		case wxNO:		return wxNO;
		case wxCANCEL:	return wxCANCEL;
		}
	}

	return wxNO;
}

//----------------------------------------------------------------------------------
/*
grid control
*/
void	ProjectManager::SetupGrid( wxioPropertyGrid* grid )
{
	if ( m_gridcontroller ) {
		m_gridcontroller->Setup( grid );
	}
}

void	ProjectManager::SetupGridEventTab( wxioPropertyGrid* grid )
{
	if ( m_gridcontroller ) {
		m_gridcontroller->SetupEventTab( grid );
	}
}

void	ProjectManager::SyncGrid( wxioPropertyGrid* grid, ioWnd* wnd )
{
	if ( m_gridcontroller ) {
		m_gridcontroller->Sync( grid, wnd );
	}
}

void	ProjectManager::SyncGridEventTab( wxioPropertyGrid* grid, ioWnd* wnd )
{
	if ( m_gridcontroller ) {
		m_gridcontroller->SyncEventTab( grid, wnd );
	}
}

void	ProjectManager::OnChangeGrid( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	if ( m_gridcontroller ) {
		m_gridcontroller->OnChange( grid, wnd, event );
	}
}

void	ProjectManager::OnChangeGridEventTab( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	if ( m_gridcontroller ) {
		m_gridcontroller->OnChangeEventTab( grid, wnd, event );
	}
}