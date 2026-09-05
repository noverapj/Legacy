
//----------------------------------------------------------------------------------
/*
Filename	: InputProcessor.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _INPUTPROCESSOR_H_
#define _INPUTPROCESSOR_H_

#include <map>
#include <vector>
#include <wx/treectrl.h>

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
EditData : 편집관련 데이터들
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioWnd;
class EditData
{
	//----------------------------------------------------------------------------------
	/*
	저장 데이터 Type
	*/
	struct WTInfo {
		WTInfo( const wxTreeItemId& id, int type ) : m_id( id ), m_type( type ) {}
		wxTreeItemId			m_id;
		int						m_type;
	};
	struct TWInfo {
		TWInfo( ioWnd* wnd, int type ) : m_wnd( wnd ), m_type( type ) {}
		ioWnd*					m_wnd;
		int						m_type;
	};
	typedef std::map< ioWnd*, WTInfo >			ioWndTreeItemId;
	typedef std::map< wxTreeItemId, TWInfo >	TreeItemIdioWnd;
	typedef std::vector< ioWnd* >				ioWndVector;

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	EditData();
	~EditData();
	
	//----------------------------------------------------------------------------------
	/*
	Clear
	*/
	void						Clear();

	//----------------------------------------------------------------------------------
	/*
	Get
	*/
	int							GetNum() const { return static_cast<int>( m_matchingWT.size() ); }
	ioWnd*						GetWnd( int index );
	ioWnd*						GetWndFromID( DWORD id );

	//----------------------------------------------------------------------------------
	/*
	Root
	*/
	ioWnd*						GetRootWnd() { return m_rootwnd; }
	void						SetRootWnd( ioWnd* wnd ) { m_rootwnd = wnd; }

	//----------------------------------------------------------------------------------
	/*
	Select
	*/
	ioWnd*						GetSelectWnd() { return m_selectwnd; }
	const wxTreeItemId&			GetSelectTreeItem() const { return m_treeitemid; }
	int							GetSelectType() { return GetType( GetSelectWnd() ); }
	void						SetSelect( ioWnd* wnd, const wxTreeItemId& id ) { m_selectwnd = wnd; m_treeitemid = id; }
	void						UnSelect() {  m_selectwnd = 0; m_treeitemid.Unset(); }

	//----------------------------------------------------------------------------------
	/*
	Data Info Get
	*/
	wxTreeItemId				Get( ioWnd* wnd );
	ioWnd*						Get( const wxTreeItemId& id );
	int							GetType( const wxTreeItemId& id );
	int							GetType( ioWnd* wnd );

	//----------------------------------------------------------------------------------
	/*
	Data Insert / Remove
	*/
	void						InsertWT( ioWnd* wnd, const wxTreeItemId& id, int type );
	void						InsertTW( const wxTreeItemId& id, ioWnd* wnd, int type );
	void						RemoveChild( ioWnd* wnd );
	void						Remove( ioWnd* wnd, bool removechild );
	void						Remove( const wxTreeItemId& id, bool removechild );
	ioWnd*						Remove( DWORD id, bool removechild );	
	void						Swap( const wxTreeItemId& id1, const wxTreeItemId& id2 );

	//----------------------------------------------------------------------------------
	/*
	Sub Select Wnd
	*/
	void						AddSubSelect( ioWnd* wnd );
	void						RemoveSubSelect( ioWnd* wnd );
	void						ClearSubSelect() { m_subselectwnds.clear(); }
	int							GetNumSubSelect() const { return static_cast<int>( m_subselectwnds.size() ); }
	ioWnd*						GetSubSelectWnd( int index );

private:

	ioWnd*						m_rootwnd;
	ioWnd*						m_selectwnd;
	wxTreeItemId				m_treeitemid;

	ioWndTreeItemId				m_matchingWT;
	TreeItemIdioWnd				m_matchingTW;

	ioWndVector					m_subselectwnds;
};	


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
InputProcessor
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioMouse;
class wxRichTextEditorFrame;
class wxStaticBitmap;
class InputProcessor
{
	enum MOUSE_LBUTTON_EDIT_STATE
	{
		MLES_NONE,
		MLES_DRAG_SELECT,	// 마우스 드래그 선택
		MLES_MOVING,		// 이동
		MLES_SIZING,		// 싸이즈 조절
	};

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	InputProcessor( wxRichTextEditorFrame* parent );
	virtual ~InputProcessor();

	//----------------------------------------------------------------------------------
	/*
	Event
	*/
	// Toolbar
	void						OnClickToolAlignLeft();
	void						OnClickToolAlignRight();
	void						OnClickToolAlignTop();
	void						OnClickToolAlignBottom();
	void						OnClickToolCorrectGapLeftRight();
	void						OnClickToolCorrectGapTopBottom();
	void						OnClickToolCorrectSizeLeftRight();
	void						OnClickToolCorrectSizeTopBottom();
	void						OnClickToolCorrectSizeAll();
	// Frame
	// InheritTree
	void						OnInheritTreeKeyDown( wxKeyEvent& event );
	void						OnInheritTreeKeyUp( wxKeyEvent& event );
	void						OnInheritTreeBoxSelect( wxTreeEvent& event );
	// ToolBox
	void						OnToolBoxControlDown( wxMouseEvent& event );	
	// Displayer
	void						OnDisplayerKeyDown( wxKeyEvent& event );
	void						OnDisplayerKeyUp( wxKeyEvent& event );
	void						OnDisplayerLButtonDown( wxMouseEvent& event );
	bool						OnDisplayerLButtonDownSelectWnd( wxMouseEvent& event );
	void						OnDisplayerLButtonUp( wxMouseEvent& event );
	void						OnDisplayerLButtonUpSelectWnd( wxMouseEvent& event );
	void						OnDisplayerMouseMove( wxMouseEvent& event );
	// Hooking Event
	void						OnHookLButtonUp( wxMouseEvent& event );

	//----------------------------------------------------------------------------------
	/*
	Render
	*/
	void						Render();
	void						UpdateCursorStatus();	
	wxStockCursor				IntersectCursor( ioWnd* wnd, int weighthalf );

	//----------------------------------------------------------------------------------
	/*
	DragCreate
	*/
	void						CreateCurDragControl( wxStaticBitmap* sbitmap );
	void						DeleteCurDragControl();
	void						RenderCurDragControl();	

	//----------------------------------------------------------------------------------
	/*
	Set / Get
	*/
	EditData&					GetEditData() { return m_editdata; }
	ioWnd*						GetSelectWnd() { return m_editdata.GetSelectWnd(); }
	ioMouse*					GetMouse() { return m_mouse; }
	bool						IsLButtonDown() const { return m_lbuttondown; }

	//----------------------------------------------------------------------------------
	/*
	ETC
	*/
	void						UnSelectAll();
	void						MainSelect( ioWnd* wnd, bool clearsub = true );
	void						SubSelect( ioWnd* wnd );
	void						SetupProperty();
	void						SetupProperty( ioWnd* wnd );

protected:
	
	//----------------------------------------------------------------------------------
	/*
	OnEvent Common
	*/
	void						OnKeyDownCommon( wxKeyEvent& event );
	void						OnKeyUpCommon( wxKeyEvent& event );
	void						OnKeyDownTree( wxKeyEvent& event );
	void						OnKeyUpTree( wxKeyEvent& event );
	void						OnKeyDownDisplayer( wxKeyEvent& event );
	void						OnKeyUpDisplayer( wxKeyEvent& event );

	//----------------------------------------------------------------------------------
	/*
	GetMostParentWnd : 주어진 윈도우중 최상위 윈도우만을 골라 리턴해준다.
	*/
	std::vector<ioWnd*>			GetMostParentWnd( std::vector<ioWnd*>& wnds );
	std::vector<ioWnd*>			GetMostParentWnd();
	void						GetSelectWndAll( std::vector< ioWnd* >& wnds );

	//----------------------------------------------------------------------------------
	/*
	ETC
	*/	
	void						SetupProjectProperty();
	void						SwapWnd( ioWnd* wnd, bool up );
	ioWnd*						GetSelWnd( wxMouseEvent& event );
	ioWnd*						GetSelWnd( int x, int y );
	void						ProcessWndAlign( void (*func)( ioWnd* wnd, int param1, int param2 ), int param1, int param2 );	

protected:

	wxRichTextEditorFrame*		m_parent;

	EditData					m_editdata;

	ioWnd*						m_copyedwnd;

	// cursor 상태
	bool						m_isupdatecursor;
	wxStockCursor				m_cursortype;

	// 선택 윈도우 타입 관련 멤버 변수들
	wxStaticBitmap*				m_curdragcontrol;
	wxWindowID					m_curdragwndid;

	// 마우스 왼쪽버튼 편집 상태
	MOUSE_LBUTTON_EDIT_STATE	m_lbutton_edit_state;
	bool						m_lbuttondown;
	bool						m_draged;
	POINT						m_prevmousepos;
	POINT						m_lbuttondownmousepos;
	RECT						m_dragselectrect;

	// 엔진쪽 멤버변수
	ioMouse*					m_mouse;
};


#endif // _INPUTPROCESSOR_H_