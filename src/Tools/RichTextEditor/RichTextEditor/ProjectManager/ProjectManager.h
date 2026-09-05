//-------------------------------------------------------------------
//-------------------------------------------------------------------
// 작성자	: cha
// 날짜		: 2013.10.24
// 이름		: ProjectManager
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#ifndef __PROJECTMANAGER_H__
#define __PROJECTMANAGER_H__

#include "wx/wx.h"
#include <vector>
#include "ioSingleton.h"
#include "../wxWidgetControls/wxioPropertyGrid.h"
#include "HeaderFileGenerator.h"

//-------------------------------------------------------------
/*
Util
*/
wxString	GetFileNameOnly( const char* src );

//-------------------------------------------------------------
/*
ProjectManager
*/
class ioWnd;
class PropertyControllerProject;
class ProjectManager : public Singleton< ProjectManager >
{
public:

	//-------------------------------------------------------------
	/*
	생성,파괴
	*/
	ProjectManager();
	~ProjectManager();

	//-------------------------------------------------------------
	/*
	Set / Get
	*/
	void							SetUIFileFullPath( const char* path ) { if ( path ) m_edituipath = path; }
	const char*						GetUIFileFullPath() const { return m_edituipath.c_str(); }
	void							SetUIFilename( const char* name ) { if ( name ) m_edituifilename = name; }
	const char*						GetUIFilename() const { return m_edituifilename.c_str(); }
	void							SetCodeFilename( const char* filename ) { if ( filename ) m_codefilename = filename; }
	const char*						GetCodeFilename() const { return m_codefilename.c_str(); }
	void							SetClassName( const char* name ) { if ( name ) m_classname = name; }
	const char*						GetClassName() const { return m_classname.c_str(); }
	void							SetBaseClassName( const char* name ) { if ( name ) m_baseclassname = name; }
	const char*						GetBaseClassName() const { return m_baseclassname.c_str(); }
	//
	void							SetGUIMgrRegisterFileName( const char* name ) { if ( name ) m_gmregfilename = name; }
	const char*						GetGUIMgrRegisterFileName() const { return m_gmregfilename.c_str(); }
	void							SetGUIMgrRegisterClassName( const char* name ) { if ( name ) m_gmregclassname = name; }
	const char*						GetGUIMgrRegisterClassName() const { return m_gmregclassname.c_str(); }
	void							SetGUIMgrRegisterHeaderFilePath( const char* name ) { if ( name ) m_gmregheaderfilepath = name; }
	const char*						GetGUIMgrRegisterHeaderFilePath() const { return m_gmregheaderfilepath.c_str(); }

	//-------------------------------------------------------------
	/*
	Save / Load Event
	*/
	void							OnNew();
	void							OnSave( bool skipanswer );
	void							OnSaveAs();
	void							OnLoad();
	void							OnLoad( const char* filename );

	//-------------------------------------------------------------
	/*
	Save / Load
	*/
	// Save : 무조건 .xml로 넘어온다는 가정을 한다.
	bool							Save( const char* filename );
	// Load : uiproj or xml 파일이 넘어올수 있다.
	bool							Load( const char* filename, wxString& uifilename );
	bool							LoadFromProjFile( const char* filename, wxString& uifilename );
	bool							LoadFromUIFile( const char* filename, wxString& uifilename );

	//-------------------------------------------------------------
	/*
	GenerateCode
	*/
	bool							GenerateCode();
	bool							EditGUIMgrRegisterCode();

	//----------------------------------------------------------------------------------
	/*
	Control
	*/
	void							Clear();
	bool							IsEmptyEditData();
	int								QuestionSave();

	//----------------------------------------------------------------------------------
	/*
	grid control
	*/
	void							SetupGrid( wxioPropertyGrid* grid );
	void							SetupGridEventTab( wxioPropertyGrid* grid );
	void							SyncGrid( wxioPropertyGrid* grid, ioWnd* wnd );
	void							SyncGridEventTab( wxioPropertyGrid* grid, ioWnd* wnd );
	void							OnChangeGrid( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );
	void							OnChangeGridEventTab( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event );

protected:

	//-------------------------------------------------------------
	/*
	Load
	*/
	bool							LoadProjFile( const char* filename, wxString& uifilename );
	bool							LoadUIFile( const char* filename );

protected:

	wxString						m_edituipath;
	wxString						m_edituifilename;

	wxString						m_codefilename;
	wxString						m_classname;
	wxString						m_baseclassname;

	PropertyControllerProject*		m_gridcontroller;

	// GUIMgr 등록 정보들
	wxString						m_gmregfilename;
	wxString						m_gmregclassname;
	wxString						m_gmregheaderfilepath;
};

#endif //__PROJECTMANAGER_H__
