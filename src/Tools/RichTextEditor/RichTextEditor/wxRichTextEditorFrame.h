
//----------------------------------------------------------------------------------
/*
Filename	: wxRichTextEditorFrame.h
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _WXRICHTEXTEDITORFRAME_H_
#define _WXRICHTEXTEDITORFRAME_H_

#include "wxMainFrame.h"
#include "ioSingleton.h"
#include <wx/timer.h>
#include <map>
#include <vector>
#include "InputProcessor.h"
#include "ProjectManager/HeaderFileGenerator.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
가상 선언들
*/
class ioRenderSystem;
class ioFontManager;
class ioVertexBufferManager;
class ioIndexBufferManager;
class ioTextureManager;
class ioShaderManager;
class ioGUIManager;
class ioCamera;
class ioFrameTimer;
class ioInputBox;
class ioWnd;
class Visualizer2D;
class ioMouse;
class ioSoundManager;
class PropertyControllerioWnd;
class ioUIRenderer;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRichTextEditorFrame
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class wxRichTextEditorFrame : public wxMainFrame, public Singleton< wxRichTextEditorFrame >
{
	friend class InputProcessor;

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	wxRichTextEditorFrame( const wxString& title );
	virtual ~wxRichTextEditorFrame();
	
	//----------------------------------------------------------------------------------
	/*
	wxMainFrame Override
	*/	
	// Menu
	virtual void				OnNew( wxCommandEvent& event );
	virtual void				OnSave( wxCommandEvent& event );
	virtual void				OnSaveAs( wxCommandEvent& event );
	virtual void				OnLoad( wxCommandEvent& event );
	virtual void				OnSummonIDSettingBox( wxCommandEvent& event );
	// Toolbar
	virtual void				OnClickToolAlignLeft( wxCommandEvent& event );
	virtual void				OnClickToolAlignRight( wxCommandEvent& event );
	virtual void				OnClickToolAlignTop( wxCommandEvent& event );
	virtual void				OnClickToolAlignBottom( wxCommandEvent& event );
	virtual void				OnClickToolCorrectGapLeftRight( wxCommandEvent& event );
	virtual void				OnClickToolCorrectGapTopBottom( wxCommandEvent& event );
	virtual void				OnClickToolCorrectSizeLeftRight( wxCommandEvent& event );
	virtual void				OnClickToolCorrectSizeTopBottom( wxCommandEvent& event );
	virtual void				OnClickToolCorrectSizeAll( wxCommandEvent& event );
	// Frame
	// InheritTree
	virtual void				OnInheritTreeKeyDown( wxKeyEvent& event );
	virtual void				OnInheritTreeKeyUp( wxKeyEvent& event );
	virtual void				OnInheritTreeBoxSelect( wxTreeEvent& event );
	// ToolBox
	virtual void				OnToolBoxControlDown( wxMouseEvent& event );	
	// Displayer
	virtual void				OnDisplayerKeyDown( wxKeyEvent& event );
	virtual void				OnDisplayerKeyUp( wxKeyEvent& event );
	virtual void				OnDisplayerLButtonDown( wxMouseEvent& event );
	virtual void				OnDisplayerLButtonUp( wxMouseEvent& event );
	virtual void				OnDisplayerMouseMove( wxMouseEvent& event );
	virtual void				OnDisplayerPaint( wxPaintEvent& event );
	// PropertyTab
	virtual void				OnChangePropertyTab( wxNotebookEvent& event );
	//ETC
	virtual void				OnCmd( wxCommandEvent& event );
	virtual void				OnCmd2( wxCommandEvent& event );

	//----------------------------------------------------------------------------------
	/*
	PropertyGrid Event
	*/
	void						OnPropertyGridChanged( wxPropertyGridEvent& event );
	void						OnEventPropertyGridChanged( wxPropertyGridEvent& event );

	//----------------------------------------------------------------------------------
	/*
	Hooking Event
	*/
	void						OnHookLButtonUp( wxMouseEvent& event );

	//----------------------------------------------------------------------------------
	/*
	Get
	*/
	void						GetioWndIDInfos( std::map< wxString, int >& out );
	void						GetEventFunctionInfos( HeaderFileGenerator::EventFuncInfoSet& out, HeaderFileGenerator::EventFuncConnectInfoSet& out2 );
	void						GetMemberInfos( HeaderFileGenerator::MemeberInfoSet& out );
	wxPanel*					GetDisplayWnd() { return m_displayer; }
	ioWnd*						GetSelectioWnd();	
	ioGUIManager*				GetGUIManager() { return m_guimgr; }
	ioRenderSystem*				GetRenderSystem() { return m_renderer; }
	ioTextureManager*			GetTextureManager() { return m_texturemgr; }
	ioUIRenderer*				GetUIRenderer() { return m_uirenderer; }
	IDirect3DDevice9*			GetD3DDevice();
	InputProcessor&				GetInputProcessor() { return m_inputprocessor; }
	Visualizer2D*				GetVisualizer() { return m_visualizer; }
	bool						IsPropertyTab() const;

	//----------------------------------------------------------------------------------
	/*
	Event
	*/
	void						OnTimer( wxTimerEvent& evt );
	void						OnDropFiles( wxDropFilesEvent& event );

	//----------------------------------------------------------------------------------
	/*
	Render
	*/
	void						Render();

	//----------------------------------------------------------------------------------
	/*
	Sync Engine To Tool
	*/
	void						SyncAll();
	void						SyncAll( ioWnd* root, DWORD selid );
	void						SyncAllRecursive( const wxTreeItemId& parentitem, ioWnd* parentwnd, DWORD selid );
	void						Sync( const wxTreeItemId& parentitem, ioWnd* parentwnd );
	void						SyncProperty();

	//----------------------------------------------------------------------------------
	/*
	ETC
	*/	
	void						CorrectWndID(); // 윈도우 아이디를 유니크하도록 조절해준다.
	void						CorrectUniqueid(); // 편집시 필요한 유니크 id변수를 현재 편집상태에 맞도록 조절해준다.
	void						ClearEditWnds( bool addroot );

protected:

	//----------------------------------------------------------------------------------
	/*
	Render
	*/
	void						RenderEngine();
	void						RenderUIRect();

	//----------------------------------------------------------------------------------
	/*
	ETC
	*/
	void						BuildWndID( ioWnd* wnd ); // 윈도우 아이디를 유니크아이디를 이용해 재설정 해준다.
	void						CorrectWndID( ioWnd* wnd, std::set<int>& ids, int& maxid, bool ignoreSetid );
	void						CorrectUniqueid( ioWnd* wnd, bool ignoreSetid );
	ioWnd*						AddWnd( const wxWindowID& id );
	ioWnd*						AddWndToEngine( ioWnd* parent, const wxWindowID& id );
	ioWnd*						CreateWnd( int TBTType );
	void						SetDefaultSetting( int TBTType, ioWnd* wnd );

protected:

	wxTimer*					m_timer;
		
	// 편집 관련	
	DWORD						m_uniqueid;
	
	// 입력 관련 처리를 담당하는 넘
	InputProcessor				m_inputprocessor;

	// propertygrid 컨트롤 객체
	typedef std::vector< PropertyControllerioWnd* > PGControllerVec;
	PGControllerVec				m_pgcontrollers;

	// 엔진쪽 멤버변수
	ioRenderSystem*				m_renderer;	
	ioVertexBufferManager*		m_vxbuffermgr;
	ioIndexBufferManager*		m_ixbuffermgr;
	ioTextureManager*			m_texturemgr;
	ioShaderManager*			m_shadermgr;
	ioFontManager*				m_fontmgr;
	ioGUIManager*				m_guimgr;
	ioCamera*					m_camera;
	ioFrameTimer*				m_frametimer;
	ioInputBox*					m_inputbox;
	ioSoundManager*				m_soundmgr;
	ioUIRenderer*				m_uirenderer;

	// Client쪽에서 DX로 무언가 그리기 위한 모듈
	Visualizer2D*				m_visualizer;
};


#endif // _WXRICHTEXTEDITORFRAME_H_