
//----------------------------------------------------------------------------------
/*
Filename	: InputProcessor.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "InputProcessor.h"
#include <wx/statbmp.h>
#include "wxRichTextEditorFrame.h"
#include "ioWnd.h"
#include "ioMouse.h"
#include "ioGUIManager.h"
#include "ProjectManager/ProjectManager.h"
#include "Utils/Visualizer2D.h"
#include "wxWidgetControls/PropertyControllerioWnd.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioToolMouse : 마우스
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioToolMouse : public ioMouse
{
public:
	
	ioToolMouse()
		: ioMouse()
	{}
	virtual ~ioToolMouse() {}
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
EditData : 편집관련 데이터들
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
EditData::EditData()
{
	Clear(); 
}

EditData::~EditData()
{
}
	
//----------------------------------------------------------------------------------
/*
Clear
*/
void	EditData::Clear()
{
	m_rootwnd = 0;
	m_selectwnd = 0;
	m_treeitemid.Unset();

	m_matchingWT.clear();
	m_matchingTW.clear();

	m_subselectwnds.clear();
}

//----------------------------------------------------------------------------------
/*
Get
*/
ioWnd*	EditData::GetWnd( int index )
{
	if ( index < 0 || index >= GetNum() ) {
		return 0;
	}
	ioWndTreeItemId::iterator iter = m_matchingWT.begin();
	ioWndTreeItemId::iterator eiter = m_matchingWT.end();
	for ( int cnt = 0; iter != eiter; ++iter, ++cnt ) {
		if ( cnt == index ) {
			return iter->first;
		}
	}
	return 0;
}

ioWnd*	EditData::GetWndFromID( DWORD id )
{
	ioWndTreeItemId::iterator iter = m_matchingWT.begin();
	ioWndTreeItemId::iterator eiter = m_matchingWT.end();
	for ( ; iter != eiter; ++iter ) {
		if ( iter->first->GetID() == id ) {
			return iter->first;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------
/*
Data Info Get
*/
wxTreeItemId	EditData::Get( ioWnd* wnd )
{
	ioWndTreeItemId::iterator iter = m_matchingWT.find( wnd );
	if ( iter == m_matchingWT.end() ) {
		return wxTreeItemId();
	}
	return iter->second.m_id;
}

ioWnd*	EditData::Get( const wxTreeItemId& id )
{
	TreeItemIdioWnd::iterator iter = m_matchingTW.find( id );
	if ( iter == m_matchingTW.end() ) {
		return 0;
	}
	return iter->second.m_wnd;
}

int		EditData::GetType( const wxTreeItemId& id )
{
	TreeItemIdioWnd::iterator iter = m_matchingTW.find( id );
	if ( iter == m_matchingTW.end() ) {
		return -1;
	}
	return iter->second.m_type;
}

int		EditData::GetType( ioWnd* wnd )
{
	ioWndTreeItemId::iterator iter = m_matchingWT.find( wnd );
	if ( iter == m_matchingWT.end() ) {
		return -1;
	}
	return iter->second.m_type;
}

//----------------------------------------------------------------------------------
/*
Data Insert / Remove
*/
void	EditData::InsertWT( ioWnd* wnd, const wxTreeItemId& id, int type )
{
	m_matchingWT.insert( ioWndTreeItemId::value_type( wnd, WTInfo( id, type ) ) );
	m_matchingTW.insert( TreeItemIdioWnd::value_type( id, TWInfo( wnd, type ) ) );
}

void	EditData::InsertTW( const wxTreeItemId& id, ioWnd* wnd, int type )
{
	m_matchingTW.insert( TreeItemIdioWnd::value_type( id, TWInfo( wnd, type ) ) );
	m_matchingWT.insert( ioWndTreeItemId::value_type( wnd, WTInfo( id, type ) ) );
}

void	EditData::RemoveChild( ioWnd* wnd )
{
	if ( !wnd ) return;

	int num = wnd->GetNumChildren();
	for ( int i = 0; i < num; ++i ) {
		Remove( wnd->GetChild( i ), true );
	}
}

void	EditData::Remove( ioWnd* wnd, bool removechild )
{
	if ( !wnd ) return;

	if ( removechild ) {
		RemoveChild( wnd );
	}

	ioWndTreeItemId::iterator wtiter = m_matchingWT.find( wnd );
	if ( wtiter == m_matchingWT.end() ) {
		return;
	}
	TreeItemIdioWnd::iterator twiter = m_matchingTW.find( wtiter->second.m_id );
	if ( twiter != m_matchingTW.end() ) {
		m_matchingTW.erase( twiter );
	}
	m_matchingWT.erase( wtiter );	
}

void	EditData::Remove( const wxTreeItemId& id, bool removechild )
{
	if ( removechild ) {
		RemoveChild( Get( id ) );
	}

	TreeItemIdioWnd::iterator twiter = m_matchingTW.find( id );
	if ( twiter == m_matchingTW.end() ) {
		return;
	}
	ioWndTreeItemId::iterator wtiter = m_matchingWT.find( twiter->second.m_wnd );
	if ( wtiter != m_matchingWT.end() ) {
		m_matchingWT.erase( wtiter );
	}
	m_matchingTW.erase( twiter );
}

ioWnd*	EditData::Remove( DWORD id, bool removechild )
{
	ioWndTreeItemId::iterator iter = m_matchingWT.begin();
	ioWndTreeItemId::iterator eiter = m_matchingWT.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( iter->first->GetID() == id )
		{
			ioWnd* wnd = iter->first;
			Remove( wnd, removechild );
			return wnd;
		}
	}
	return 0;
}

void	EditData::Swap( const wxTreeItemId& id1, const wxTreeItemId& id2 )
{
	ioWnd* wnd1 = Get( id1 );
	ioWnd* wnd2 = Get( id2 );
	
	if ( !wnd1 || !wnd2 ) {
		return;
	}

	int type1 = GetType( id1 );
	int type2 = GetType( id2 );

	Remove( id1, false );
	Remove( id2, false );

	InsertWT( wnd1, id2, type1 );
	InsertWT( wnd2, id1, type2 );
}

//----------------------------------------------------------------------------------
/*
Sub Select Wnd
*/
void	EditData::AddSubSelect( ioWnd* wnd )
{
	if ( !wnd ) {
		return;
	}
	
	ioWndVector::iterator iter = m_subselectwnds.begin();
	ioWndVector::iterator eiter = m_subselectwnds.end();
	for ( ; iter != eiter; ++iter ) {
		if ( *iter == wnd ) {
			return;
		}
	}
	m_subselectwnds.push_back( wnd );
}

void	EditData::RemoveSubSelect( ioWnd* wnd )
{
	if ( !wnd ) {
		return;
	}
	
	ioWndVector::iterator iter = m_subselectwnds.begin();
	ioWndVector::iterator eiter = m_subselectwnds.end();
	for ( ; iter != eiter; ++iter ) 
	{
		if ( *iter == wnd ) 
		{
			m_subselectwnds.erase( iter );
			return;
		}
	}
}

ioWnd*	EditData::GetSubSelectWnd( int index ) 
{
	if ( index < 0 || index >= GetNumSubSelect() ) {
		return 0;
	}
	return m_subselectwnds[ index ];
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
InputProcessor
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
InputProcessor::InputProcessor( wxRichTextEditorFrame* parent )
	: m_parent( parent )
	, m_isupdatecursor( true )
	, m_cursortype( wxCURSOR_ARROW )
	, m_lbuttondown( false )
	, m_draged( false )
	, m_copyedwnd( 0 )
	, m_curdragcontrol( 0 )
	, m_curdragwndid( -1 )
	, m_lbutton_edit_state( MLES_NONE )
{
	assert( m_parent );

	m_mouse = new ioToolMouse();
}

InputProcessor::~InputProcessor()
{
	DeleteCurDragControl();

	DEL( m_copyedwnd );

	DEL( m_mouse );
}

//----------------------------------------------------------------------------------
/*
GetMostParentWnd : 주어진 윈도우중 최상위 윈도우만을 골라 리턴해준다.
*/
std::vector<ioWnd*>		InputProcessor::GetMostParentWnd( std::vector<ioWnd*>& wnds )
{
	std::vector< ioWnd* > live;
	std::vector< ioWnd* >::iterator iter = wnds.begin();
	std::vector< ioWnd* >::iterator eiter = wnds.end();
	for ( ; iter != eiter; ++iter )
	{
		bool islive = true;
		std::vector< ioWnd* >::iterator subiter = wnds.begin();
		std::vector< ioWnd* >::iterator subeiter = wnds.end();
		for ( ; subiter != subeiter; ++subiter )
		{
			if ( *iter == *subiter ) {
				continue;
			}
			if ( (*iter)->IsParent( (*subiter)->GetID() ) ) 
			{
				islive = false;
				break;
			}
		}
		if ( islive ) {
			live.push_back( *iter );
		}
	}
	return live;
}

std::vector<ioWnd*>		InputProcessor::GetMostParentWnd()
{
	std::vector< ioWnd* > wnds;
	GetSelectWndAll( wnds );
	wnds = GetMostParentWnd( wnds );
	return wnds;
}

void	InputProcessor::GetSelectWndAll( std::vector< ioWnd* >& wnds )
{
	wnds.clear();
	wnds.push_back( m_editdata.GetSelectWnd() );
	for ( int i = 0; i < m_editdata.GetNumSubSelect(); ++i ) {
		wnds.push_back( m_editdata.GetSubSelectWnd( i ) );
	}
}

//----------------------------------------------------------------------------------
/*
Event
*/

// Common
void	InputProcessor::OnKeyDownCommon( wxKeyEvent& event )
{
	static bool copyedchilds = false;

	switch ( event.GetKeyCode() )
	{
	case WXK_DELETE:
		{
			bool isdel = false;
			std::vector< ioWnd* > wnds = GetMostParentWnd();
			std::vector< ioWnd* >::iterator iter = wnds.begin();
			std::vector< ioWnd* >::iterator eiter = wnds.end();
			for ( ; iter != eiter; ++iter )
			{
				ioWnd* wnd = *iter;//m_editdata.GetSelectWnd();	
				if ( !wnd  ) {
					continue;
				}
				ioWnd* parent = wnd->GetParent();
				if ( !parent  ) {
					continue;
				}
				isdel = true;
				bool isroot = ( wnd == m_editdata.GetRootWnd() );
	
				// Tree에서 지운다.
				m_parent->m_InheritTree->Delete( m_editdata.GetSelectTreeItem() );				

				// edit 데이터에서 지운다.
				if ( isroot ) {
					m_editdata.SetRootWnd( 0 );
				}
				m_editdata.Remove( wnd, true );

				// 엔진에서 지운다.				
				parent->DeleteChild( wnd->GetID() );
				wxRichTextEditorFrame::GetSingleton().GetGUIManager()->ClearPreState();
			}

			if ( isdel ) {
				UnSelectAll();
			}
		}
		break;

	case 'c':
	case 'C':
		{
			if ( event.ControlDown() )
			{
				ioWnd* wnd = m_editdata.GetSelectWnd();	
				if ( !wnd  ) {
					break;
				}

				DEL( m_copyedwnd );
				m_copyedwnd = wnd->Clone( !event.ShiftDown() );				
				copyedchilds = !event.ShiftDown();
			}
		}
		break;

	case 'v':
	case 'V':
		{
			if ( event.ControlDown() )
			{
				ioWnd* wnd = m_editdata.GetSelectWnd();	
				if ( !wnd  ) {
					break;
				}
				
				if ( m_copyedwnd )
				{					
					wnd->AddChild( m_copyedwnd );
					m_copyedwnd->iwm_create();
					m_parent->BuildWndID( m_copyedwnd );
					m_copyedwnd->SetPosX( m_copyedwnd->GetPosX() + 5 );
					m_copyedwnd->SetPosY( m_copyedwnd->GetPosY() + 5 );
					m_copyedwnd->RecalcSize();
					m_copyedwnd = m_copyedwnd->Clone( copyedchilds );					

					m_parent->SyncAll();
				}
			}
		}
		break;
	}
}

void	InputProcessor::OnKeyUpCommon( wxKeyEvent& event )
{
}

void	InputProcessor::OnKeyDownTree( wxKeyEvent& event )
{
	switch ( event.GetKeyCode() )
	{
	case WXK_UP:
		{
			ioWnd* wnd = m_editdata.GetSelectWnd();				
			if ( wnd && wnd->GetParent() && event.ControlDown() ) {
				SwapWnd( wnd, true );
			}
		}
		break;

	case WXK_DOWN:
		{
			ioWnd* wnd = m_editdata.GetSelectWnd();				
			if ( wnd && wnd->GetParent() && event.ControlDown() ) {
				SwapWnd( wnd, false );				
			}
		}
		break;
	}
}

void	InputProcessor::OnKeyUpTree( wxKeyEvent& event )
{
}

void	InputProcessor::OnKeyDownDisplayer( wxKeyEvent& event )
{
}

void	InputProcessor::OnKeyUpDisplayer( wxKeyEvent& event )
{
	if ( m_editdata.GetSelectWnd() )
	{
		std::vector< ioWnd* > wnds = GetMostParentWnd();
		std::vector< ioWnd* >::iterator iter = wnds.begin();
		std::vector< ioWnd* >::iterator eiter = wnds.end();
		for ( ; iter != eiter; ++iter )
		{
			ioWnd* wnd = *iter;

			int x = 0;
			int y = 0;
			if ( !event.ControlDown() ) // 위치 조절
			{
				x = wnd->GetPosX();
				y = wnd->GetPosY();
			}

			switch ( event.GetKeyCode() )
			{
			case WXK_LEFT:
				x -= 1;
				break;

			case WXK_RIGHT:
				x += 1;
				break;

			case WXK_UP:
				y -= 1;
				break;

			case WXK_DOWN:
				y += 1;
				break;
			}

			if ( event.ControlDown() ) // size조절
			{
				wnd->SetWidth( wnd->GetWidth() + x );
				wnd->SetHeight( wnd->GetHeight() + y );
			}
			else // 위치 조절
			{
				wnd->SetPosX( x );
				wnd->SetPosY( y );
			}
		}

		wxioPropertyGrid* grid = m_parent->m_propertygrid;
		if ( event.ControlDown() ) // size 표시 변경
		{
			grid->SetValue<int>( "Width", m_editdata.GetSelectWnd()->GetWidth() );
			grid->SetValue<int>( "Height", m_editdata.GetSelectWnd()->GetHeight() );
		}
		else // 위치 표시 변경
		{			
			grid->SetValue<int>( "X", m_editdata.GetSelectWnd()->GetPosX() );
			grid->SetValue<int>( "Y", m_editdata.GetSelectWnd()->GetPosY() );
		}
	}
}

// Toolbar
void	InputProcessor::ProcessWndAlign( void (*func)( ioWnd* wnd, int param1, int param2 ), int param1, int param2 )
{
	std::vector< ioWnd* > wnds;
	GetSelectWndAll( wnds );

	//while ( !wnds.empty() )
	{
		std::vector< ioWnd* > mostwnds = GetMostParentWnd( wnds );
		std::vector< ioWnd* >::iterator iter = mostwnds.begin();
		std::vector< ioWnd* >::iterator eiter = mostwnds.end();
		for ( ; iter != eiter; ++iter ) 
		{
			func( *iter, param1, param2 );

			//std::vector< ioWnd* >::iterator witer = wnds.begin();
			//std::vector< ioWnd* >::iterator weiter = wnds.end();
			//for ( ; witer != weiter; ++witer )
			//{
			//	if ( *witer == *iter ) 
			//	{
			//		wnds.erase( witer );
			//		break;
			//	}
			//}
		}
	}
}

void	AlignLeft( ioWnd* wnd, int param1, int param2 ) {
	wnd->SetPosX( wnd->GetPosX() + ( param1 - wnd->GetDerivedPosX() ) );
}
void	AlignRight( ioWnd* wnd, int param1, int param2 ) {	
	wnd->SetPosX( wnd->GetPosX() + ( param1 - ( wnd->GetDerivedPosX() + wnd->GetWidth() ) ) );
}
void	AlignTop( ioWnd* wnd, int param1, int param2 ) {
	wnd->SetPosY( wnd->GetPosY() + ( param1 - wnd->GetDerivedPosY() ) );
}
void	AlignBottom( ioWnd* wnd, int param1, int param2 ) {
	wnd->SetPosY( wnd->GetPosY() + ( param1 - ( wnd->GetDerivedPosY() + wnd->GetHeight() ) ) );
}

void	InputProcessor::OnClickToolAlignLeft()
{
	ioWnd* wnd = m_editdata.GetSelectWnd();
	if ( wnd ) {
		ProcessWndAlign( AlignLeft, wnd->GetDerivedPosX(), 0 );
	}
}

void	InputProcessor::OnClickToolAlignRight()
{	
	ioWnd* wnd = m_editdata.GetSelectWnd();
	if ( wnd ) {
		ProcessWndAlign( AlignRight, wnd->GetDerivedPosX() + wnd->GetWidth(), 0 );
	}
}

void	InputProcessor::OnClickToolAlignTop()
{
	ioWnd* wnd = m_editdata.GetSelectWnd();
	if ( wnd ) {
		ProcessWndAlign( AlignTop, wnd->GetDerivedPosY(), 0 );
	}
}

void	InputProcessor::OnClickToolAlignBottom()
{
	ioWnd* wnd = m_editdata.GetSelectWnd();
	if ( wnd ) {
		ProcessWndAlign( AlignBottom, wnd->GetDerivedPosY() + wnd->GetHeight(), 0 );
	}
}

class PosXSort {
public:
	bool operator()( const ioWnd* lhs, const ioWnd* rhs ) const {
		return ( lhs->GetDerivedPosX() < rhs->GetDerivedPosX() );
	}
};

class PosYSort {
public:
	bool operator()( const ioWnd* lhs, const ioWnd* rhs ) const {
		return ( lhs->GetDerivedPosY() < rhs->GetDerivedPosY() );
	}
};

void	InputProcessor::OnClickToolCorrectGapLeftRight()
{
	//std::vector< ioWnd* > wnds;
	//GetSelectWndAll( wnds );	
	std::vector< ioWnd* > wnds = GetMostParentWnd();
	if ( wnds.size() < 3 ) {
		return;
	}

	std::sort( wnds.begin(), wnds.end(), PosXSort() );

	int beginx = wnds[ 0 ]->GetDerivedPosX();
	int length = wnds[ wnds.size() - 1 ]->GetDerivedPosX() - beginx;
	int unit = length / ( wnds.size() - 1 );

	int numwnd = static_cast<int>( wnds.size() );
	for ( int i = 1; i < numwnd; ++i ) {
		wnds[ i ]->SetPosX( wnds[ i ]->GetPosX() + ( ( beginx + ( unit * i ) ) - wnds[ i ]->GetDerivedPosX() ) );
	}
}

void	InputProcessor::OnClickToolCorrectGapTopBottom()
{
	//std::vector< ioWnd* > wnds;
	//GetSelectWndAll( wnds );	
	std::vector< ioWnd* > wnds = GetMostParentWnd();
	if ( wnds.size() < 3 ) {
		return;
	}

	std::sort( wnds.begin(), wnds.end(), PosYSort() );

	int beginy = wnds[ 0 ]->GetDerivedPosY();
	int length = wnds[ wnds.size() - 1 ]->GetDerivedPosY() - beginy;
	int unit = length / ( wnds.size() - 1 );

	int numwnd = static_cast<int>( wnds.size() );
	for ( int i = 1; i < numwnd; ++i ) {
		wnds[ i ]->SetPosY( wnds[ i ]->GetPosY() + ( ( beginy + ( unit * i ) ) - wnds[ i ]->GetDerivedPosY() ) );
	}
}

void	InputProcessor::OnClickToolCorrectSizeLeftRight()
{
	ioWnd* wnd = m_editdata.GetSelectWnd();
	if ( wnd )
	{
		int numsub = m_editdata.GetNumSubSelect();
		for ( int i = 0; i < numsub; ++i ) 
		{
			ioWnd* wndsub = m_editdata.GetSubSelectWnd( i );
			if ( wndsub ) {
				wndsub->SetWidth( wnd->GetWidth() );
			}
		}
	}
}

void	InputProcessor::OnClickToolCorrectSizeTopBottom()
{
	ioWnd* wnd = m_editdata.GetSelectWnd();
	if ( wnd )
	{
		int numsub = m_editdata.GetNumSubSelect();
		for ( int i = 0; i < numsub; ++i ) 
		{
			ioWnd* wndsub = m_editdata.GetSubSelectWnd( i );
			if ( wndsub ) {
				wndsub->SetHeight( wnd->GetHeight() );
			}
		}
	}
}

void	InputProcessor::OnClickToolCorrectSizeAll()
{
	ioWnd* wnd = m_editdata.GetSelectWnd();
	if ( wnd )
	{
		int numsub = m_editdata.GetNumSubSelect();
		for ( int i = 0; i < numsub; ++i ) 
		{
			ioWnd* wndsub = m_editdata.GetSubSelectWnd( i );
			if ( wndsub ) {
				wndsub->SetSize( wnd->GetWidth(), wnd->GetHeight() );
			}
		}
	}
}

// ToolBox
void	InputProcessor::OnToolBoxControlDown( wxMouseEvent& event )
{
	wxStaticBitmap* cbitmap = dynamic_cast<wxStaticBitmap*>( event.GetEventObject() );
	if ( cbitmap ) {
		CreateCurDragControl( cbitmap );
	}	

	event.Skip();
}

// Displayer
void	InputProcessor::OnDisplayerKeyDown( wxKeyEvent& event )
{
	OnKeyDownCommon( event );

	OnKeyDownDisplayer( event );

	event.Skip();
}

void	InputProcessor::OnDisplayerKeyUp( wxKeyEvent& event )
{
	OnKeyUpCommon( event );

	OnKeyUpDisplayer( event );

	event.Skip();
}

bool	InputProcessor::OnDisplayerLButtonDownSelectWnd( wxMouseEvent& event )
{
	// 선택을 한다.
	ioWnd* selwnd = GetSelWnd( event );
	if ( m_cursortype == wxCURSOR_ARROW )
	{
		if ( !selwnd )
		{
			UnSelectAll();
			SetupProjectProperty();
			return false;
		}
		else
		{
			ioWnd* selectedwnd = m_editdata.GetSelectWnd();
				
			if ( event.ControlDown() ) 
			{
				MainSelect( selwnd, false );

				if ( selectedwnd && selectedwnd != selwnd ) {
					SubSelect( selectedwnd );
				}
			}
			else 
			{
				MainSelect( selwnd, true );
			}
			return true;
		}
	}
	else if ( m_cursortype == wxCURSOR_SIZING )
	{
		if ( selwnd )
		{
			ioWnd* selectedwnd = m_editdata.GetSelectWnd();

			if ( event.ControlDown() ) 
			{
				MainSelect( selwnd, false );

				if ( selectedwnd && selectedwnd != selwnd ) {
					SubSelect( selectedwnd );
				}
				return true;
			}
		}
	}

	return false;
}

void	InputProcessor::OnDisplayerLButtonDown( wxMouseEvent& event )
{
	if ( m_mouse ) {
		m_mouse->SetLBtnDown( event.GetX(), event.GetY() );
	}
		
	// 드래그 상태가 아니라고 셋팅
	m_lbuttondown = true;
	m_draged = false;

	// 현재 마우스 위치를 기억한다.
	m_lbuttondownmousepos.x = m_prevmousepos.x = event.GetX();
	m_lbuttondownmousepos.y = m_prevmousepos.y = event.GetY();

	// 선택을 한다.
	OnDisplayerLButtonDownSelectWnd( event );
	
	// 커서상태를 한번 업데이트 해준다.
	UpdateCursorStatus();

	// cursor상태 업데이트를 중지한다.
	m_isupdatecursor = false;

	// 마우스가 어떤 편집을 시작하는지 감지한다.
	if ( m_cursortype == wxCURSOR_SIZING ) {
		m_lbutton_edit_state = MLES_MOVING;
	} else if ( m_cursortype != wxCURSOR_ARROW ) {
		m_lbutton_edit_state = MLES_SIZING;
	} else if ( !GetSelWnd( event ) ) {
		m_lbutton_edit_state = MLES_DRAG_SELECT;
	} else {
		m_lbutton_edit_state = MLES_MOVING;
	}

	event.Skip();
}

void	InputProcessor::OnDisplayerLButtonUpSelectWnd( wxMouseEvent& event )
{
	if ( m_lbutton_edit_state != MLES_MOVING || m_draged ) {
		return;
	}

	if ( !event.ControlDown() ) {
		MainSelect( GetSelWnd( event ), true );
	}
}

void	InputProcessor::OnDisplayerLButtonUp( wxMouseEvent& event )
{
	if ( m_mouse ) {
		m_mouse->SetLBtnUp( event.GetX(), event.GetY() );
	}

	OnDisplayerLButtonUpSelectWnd( event );

	switch ( m_lbutton_edit_state )
	{
	case MLES_NONE:	
	case MLES_SIZING:
	case MLES_MOVING:
		break;

	case MLES_DRAG_SELECT:
		{
			// 드래그 선택인데 드래그를 안했다면 아무것도 선택하지 않는다. 프로젝트설정을 띄운다.
			if ( !m_draged )
			{
				UnSelectAll();
				SetupProjectProperty();
			}
			else
			{
				// rect를 조절한다.
				SetRect( &m_dragselectrect, 
					std::min<int>( m_dragselectrect.left, m_dragselectrect.right ),
					std::min<int>( m_dragselectrect.top, m_dragselectrect.bottom ),
					std::max<int>( m_dragselectrect.left, m_dragselectrect.right ),
					std::max<int>( m_dragselectrect.top, m_dragselectrect.bottom ) );

				int index = 0;
				std::vector< ioWnd* > sels( 100, 0 );
				m_parent->m_guimgr->CheckMouse( m_parent->m_guimgr->GetDeskTop(), m_dragselectrect, &(sels[0]), index );
				if ( index <= 0 )
				{
					UnSelectAll();
					SetupProjectProperty();
				}
				else
				{
					MainSelect( sels[ index - 1 ] );
					for ( int i = index - 2; i >= 0; --i ) {
						SubSelect( sels[ i ] );
					}
				}
			}

			SetRect( &m_dragselectrect, 0, 0, 0, 0 );
		}
		break;
	}
	
	ioWnd* newwnd = m_parent->AddWnd( m_curdragwndid );
	m_parent->Sync( m_editdata.Get( newwnd ), newwnd );
	m_parent->CorrectUniqueid();

	m_isupdatecursor = true;

	m_lbutton_edit_state = MLES_NONE;
	m_lbuttondown = false;
	m_draged = false;	

	event.Skip();
}

void	InputProcessor::OnDisplayerMouseMove( wxMouseEvent& event )
{
	if ( m_mouse ) {
		m_mouse->SetMouseMove( event.GetX(), event.GetY() );
	}

	if ( m_lbuttondown )
	{
		m_draged = true;

		switch ( m_lbutton_edit_state )
		{
		case MLES_NONE:
			break;

		case MLES_DRAG_SELECT:
			{
				m_dragselectrect.left = m_lbuttondownmousepos.x;
				m_dragselectrect.right = event.GetX();
				m_dragselectrect.top = m_lbuttondownmousepos.y;
				m_dragselectrect.bottom = event.GetY();
			}
			break;

		case MLES_MOVING:
			{
				std::vector< ioWnd* > wnds = GetMostParentWnd();
				std::vector< ioWnd* >::iterator iter = wnds.begin();
				std::vector< ioWnd* >::iterator eiter = wnds.end();
				for ( ; iter != eiter; ++iter )
				{
					ioWnd* wnd = *iter;
		
					int x = wnd->GetPosX() + ( event.GetX() - m_prevmousepos.x );
					int y = wnd->GetPosY() + ( event.GetY() - m_prevmousepos.y );

					// 엔진쪽을 바꿔주고
					wnd->SetPosX( x );
					wnd->SetPosY( y );			
				}

				// 속성창도 바꿔준다.
				wxioPropertyGrid* grid = m_parent->m_propertygrid;
				grid->SetValue<int>( "X", m_editdata.GetSelectWnd()->GetPosX() );
				grid->SetValue<int>( "Y", m_editdata.GetSelectWnd()->GetPosY() );
			}
			break;

		case MLES_SIZING:
			{
				ioWnd* wnd = m_editdata.GetSelectWnd();
			
				int x = event.GetX() - m_prevmousepos.x;
				int y = event.GetY() - m_prevmousepos.y;

				switch ( m_cursortype )
				{
				case wxCURSOR_SIZENESW:
				case wxCURSOR_SIZENWSE:
					break;

				case wxCURSOR_SIZENS:
					x = 0;
					break;
			
				case wxCURSOR_SIZEWE:
					y = 0;
					break;
				}

				wnd->SetWidth( wnd->GetWidth() + x );
				wnd->SetHeight( wnd->GetHeight() + y );

				// 속성창도 바꿔준다.
				wxioPropertyGrid* grid = m_parent->m_propertygrid;
				grid->SetValue<int>( "Width", wnd->GetDefWidth() );
				grid->SetValue<int>( "Height", wnd->GetDefHeight() );
			}
			break;
		}

		m_prevmousepos.x = event.GetX();
		m_prevmousepos.y = event.GetY();
	}

	event.Skip();
}

// InheritTree
void	InputProcessor::OnInheritTreeKeyDown( wxKeyEvent& event )
{
	OnKeyDownCommon( event );

	OnKeyDownTree( event );

	event.Skip();
}

void	InputProcessor::OnInheritTreeKeyUp( wxKeyEvent& event )
{
	OnKeyUpCommon( event );

	OnKeyUpTree( event );

	event.Skip();
}

void	InputProcessor::OnInheritTreeBoxSelect( wxTreeEvent& event )
{
	MainSelect( m_editdata.Get( event.GetItem() ), true );
	
	event.Skip();
}

//----------------------------------------------------------------------------------
/*
Hooking Event
*/
void	InputProcessor::OnHookLButtonUp( wxMouseEvent& event )
{
	DeleteCurDragControl();

	event.Skip();
}

//----------------------------------------------------------------------------------
/*
Render
*/
void	InputProcessor::Render()
{
	RenderCurDragControl();

	UpdateCursorStatus();

	// 마우스 드래그 선택 영역을 그린다.
	if ( m_lbutton_edit_state == MLES_DRAG_SELECT )
	{
		m_parent->GetVisualizer()->RenderRectangle
			(
				Vector2D( m_dragselectrect.left, m_dragselectrect.top ),
				Vector2D( m_dragselectrect.right, m_dragselectrect.top ),
				Vector2D( m_dragselectrect.left, m_dragselectrect.bottom ),
				Vector2D( m_dragselectrect.right, m_dragselectrect.bottom ),
				0.5f, 0xffff0000
			);
	}
}

void	InputProcessor::UpdateCursorStatus()
{
	if ( m_isupdatecursor )
	{
		wxStockCursor cur = IntersectCursor( m_editdata.GetSelectWnd(), 3 );
		if ( cur != m_cursortype ) 
		{
			m_cursortype = cur;
			m_parent->SetCursor( wxCursor( m_cursortype ) );
		}
	}

	wxPoint mpos = m_parent->m_displayer->ScreenToClient( wxGetMousePosition() );
	m_parent->m_statusbar->SetLabelText( wxString::Format( "x:%d   y:%d", mpos.x, mpos.y ) );
}

bool	IntersectOutlineRect( const wxPoint& pos, const wxPoint& outpos, const wxSize& outsize, const wxPoint& inpos, const wxSize& insize )
{
	wxRect rtout( outpos, outsize );
	if ( rtout.Contains( pos ) )
	{					
		wxRect rtin( inpos, insize );
		if ( !rtin.Contains( pos ) ) {
			return true;
		}
	}
	return false;
}
bool	CheckLeft( int x, int y, int width, int height, int wh, const wxPoint& pos ) {
	return IntersectOutlineRect( pos, wxPoint( x - wh, y ), wxSize( width + wh, height ), wxPoint( x + wh, y ), wxSize( width - wh, height ) );
}
bool	CheckRight( int x, int y, int width, int height, int wh, const wxPoint& pos ) {
	return IntersectOutlineRect( pos, wxPoint( x, y ), wxSize( width + wh, height ), wxPoint( x, y ), wxSize( width - wh, height ) );
}
bool	CheckUp( int x, int y, int width, int height, int wh, const wxPoint& pos ) {
	return IntersectOutlineRect( pos, wxPoint( x, y - wh ), wxSize( width, height + wh ), wxPoint( x, y + wh ), wxSize( width, height - wh ) );
}
bool	CheckDown( int x, int y, int width, int height, int wh, const wxPoint& pos ) {
	return IntersectOutlineRect( pos, wxPoint( x, y ), wxSize( width, height + wh ), wxPoint( x, y ), wxSize( width, height - wh ) );
}
wxStockCursor	InputProcessor::IntersectCursor( ioWnd* wnd, int weighthalf )
{
	wxStockCursor ret = wxCURSOR_ARROW;
	if ( !wnd ) {
		return ret;
	}

	wxPoint mpos = m_parent->m_displayer->ScreenToClient( wxGetMousePosition() );
	if ( m_parent->m_displayer->HitTest( mpos ) == wxHT_WINDOW_INSIDE )
	{		
		int x = wnd->GetDerivedPosX();
		int y = wnd->GetDerivedPosY();
		int width = wnd->GetWidth();
		int height = wnd->GetHeight();

		if ( CheckLeft( x, y, width, height, weighthalf, mpos ) ) {
			if ( CheckDown( x, y, width, height, weighthalf, mpos ) ) {
				ret = wxCURSOR_SIZENESW;
			} else if ( CheckUp( x, y, width, height, weighthalf, mpos ) ) {
				ret = wxCURSOR_ARROW;//wxCURSOR_SIZENWSE;
			} else {
				ret = wxCURSOR_ARROW;//wxCURSOR_SIZEWE;
			}
		} else if ( CheckRight( x, y, width, height, weighthalf, mpos ) ) {
			if ( CheckDown( x, y, width, height, weighthalf, mpos ) ) {
				ret = wxCURSOR_SIZENWSE;
			} else if ( CheckUp( x, y, width, height, weighthalf, mpos ) ) {
				ret = wxCURSOR_ARROW;//wxCURSOR_SIZENESW;
			} else {
				ret = wxCURSOR_SIZEWE;
			}
		} else if ( CheckUp( x, y, width, height, weighthalf, mpos ) ) {
			ret = wxCURSOR_ARROW;//wxCURSOR_SIZENS;
		} else if ( CheckDown( x, y, width, height, weighthalf, mpos ) ) {
			ret = wxCURSOR_SIZENS;
		}

		if ( ret == wxCURSOR_ARROW )
		{
			ioWnd* overwnd = GetSelWnd( mpos.x, mpos.y );
			std::vector< ioWnd* > wnds;
			GetSelectWndAll( wnds );

			std::vector< ioWnd* >::iterator iter = wnds.begin();
			std::vector< ioWnd* >::iterator eiter = wnds.end();
			for ( ; iter != eiter; ++iter ) 
			{
				if ( overwnd == *iter )
				{
					ret = wxCURSOR_SIZING;
					break;
				}
			}
		}
	}

	return ret;
}

//----------------------------------------------------------------------------------
/*
DragCreate
*/
void	InputProcessor::CreateCurDragControl( wxStaticBitmap* sbitmap )
{
	DeleteCurDragControl();

	m_curdragwndid = sbitmap->GetId();
	m_curdragcontrol = new wxStaticBitmap( m_parent, wxID_ANY, sbitmap->GetBitmap(), wxDefaultPosition, wxDefaultSize, wxPOPUP_WINDOW );
}

void	InputProcessor::DeleteCurDragControl()
{
	if ( m_curdragcontrol )
	{
		m_parent->RemoveChild( m_curdragcontrol );
		m_curdragcontrol->Destroy();
		m_curdragcontrol = 0;
		m_curdragwndid = -1;
		m_parent->Refresh();
	}
}

void	InputProcessor::RenderCurDragControl()
{
	if ( m_curdragcontrol )
	{
		wxPoint pos = m_parent->ScreenToClient( wxGetMousePosition() );
		pos.x -= m_curdragcontrol->GetSize().GetWidth() * 0.5f;
		pos.y -= m_curdragcontrol->GetSize().GetHeight() * 0.5f;
		m_curdragcontrol->SetPosition( pos );
		m_curdragcontrol->Refresh();
	}
}

//----------------------------------------------------------------------------------
/*
ETC
*/
void	InputProcessor::SetupProjectProperty()
{
	if ( m_parent->IsPropertyTab() ) {
		ProjectManager::GetSingleton().SetupGrid( m_parent->m_propertygrid );
	} else {
		ProjectManager::GetSingleton().SetupGridEventTab( m_parent->m_propertygridevent );
	}
	m_parent->SyncProperty();
}

void	InputProcessor::SwapWnd( ioWnd* wnd, bool up )
{
	if ( wnd && wnd->GetParent() ) 
	{
		wxTreeItemId myitem = m_editdata.Get( wnd );
		int myimage = m_parent->m_InheritTree->GetItemImage( myitem );
		wxString mytext = m_parent->m_InheritTree->GetItemText( myitem );
				
		int trgid = -1;
		if ( up ) {
			trgid = wnd->GetParent()->UpChild( wnd->GetID() );
		} else {
			trgid = wnd->GetParent()->DownChild( wnd->GetID() );
		}
		if ( trgid == -1 ) {
			return;
		}
		ioWnd* twnd = m_editdata.GetWndFromID( trgid );
		wxTreeItemId trgitem = m_editdata.Get( twnd );
		int trgimage = m_parent->m_InheritTree->GetItemImage( trgitem );
		wxString trgtext = m_parent->m_InheritTree->GetItemText( trgitem );

		m_parent->m_InheritTree->SetItemImage( trgitem, myimage );
		m_parent->m_InheritTree->SetItemImage( myitem, trgimage );

		m_parent->m_InheritTree->SetItemText( trgitem, mytext );
		m_parent->m_InheritTree->SetItemText( myitem, trgtext );

		m_editdata.Swap( myitem, trgitem );

		MainSelect( m_editdata.Get( trgitem ) );

		m_parent->SyncAll();
	}
}

static const long g_toolalignids[ 4 ] = 
{
	wxID_TOOl_ALIGN_LEFT,
	wxID_TOOl_ALIGN_RIGHT,
	wxID_TOOl_ALIGN_TOP,
	wxID_TOOl_ALIGN_BOTTOM
};
static const long g_toolcorrectgapids[ 2 ] = 
{
	wxID_TOOl_CORRECTGAP_LR,
	wxID_TOOl_CORRECTGAP_TB
};
static const long g_toolcorrectsizeids[ 3 ] = 
{
	wxID_TOOl_CORRECTSIZE_LR,
	wxID_TOOl_CORRECTSIZE_TB,
	wxID_TOOl_CORRECTSIZE_ALL
};
void	EnableTools( const long* ids, int num, bool enable )
{
	for ( int i = 0; i < num; ++i ) {
		wxRichTextEditorFrame::GetSingleton().m_toolbar_wndcontorl->EnableTool( ids[ i ], enable );
	}
}

void	InputProcessor::UnSelectAll()
{
	m_parent->m_InheritTree->UnselectAll();
	m_parent->m_propertygrid->Clear();
	m_parent->m_propertygridevent->Clear();
	m_editdata.UnSelect();
	m_editdata.ClearSubSelect();
	
	EnableTools( g_toolalignids, 4, false );
	EnableTools( g_toolcorrectgapids, 2, false );
	EnableTools( g_toolcorrectsizeids, 3, false );
}

void	InputProcessor::MainSelect( ioWnd* wnd, bool clearsub )
{
	if ( !wnd ) {
		return;
	}

	m_editdata.UnSelect();
	if ( clearsub ) {
		m_editdata.ClearSubSelect();
	} else {
		m_editdata.RemoveSubSelect( wnd );
	}

	wxTreeCtrl* tree = m_parent->m_InheritTree;
	tree->SetEvtHandlerEnabled( false );
	tree->SelectItem( m_editdata.Get( wnd ) );
	tree->SetEvtHandlerEnabled( true );

	m_editdata.SetSelect( wnd, m_editdata.Get( wnd ) );
	SetupProperty( wnd );

	EnableTools( g_toolalignids, 4, false );
	EnableTools( g_toolcorrectgapids, 2, false );
	EnableTools( g_toolcorrectsizeids, 3, false );
}

void	InputProcessor::SubSelect( ioWnd* wnd )
{
	if ( !wnd ) {
		return;
	}

	if ( wnd != m_editdata.GetSelectWnd() ) 
	{
		m_editdata.AddSubSelect( wnd );

		EnableTools( g_toolalignids, 4, true );		
		EnableTools( g_toolcorrectgapids, 2, ( m_editdata.GetNumSubSelect() > 1 ) );
		EnableTools( g_toolcorrectsizeids, 3, true );		
	}
}

void	InputProcessor::SetupProperty()
{
	SetupProperty( m_editdata.GetSelectWnd() );
}

void	InputProcessor::SetupProperty( ioWnd* wnd )
{
	m_parent->m_propertygrid->Clear();
	m_parent->m_propertygridevent->Clear();

	if ( !wnd )
	{
		SetupProjectProperty();
		return;
	}

	int type = m_editdata.GetType( m_editdata.Get( wnd ) );
	if ( type >= 0 && type < static_cast<int>( m_parent->m_pgcontrollers.size() ) )
	{
		if ( m_parent->IsPropertyTab() ) {
			m_parent->m_pgcontrollers[ type ]->Setup( m_parent->m_propertygrid );
		} else {
			m_parent->m_pgcontrollers[ type ]->SetupEventTab( m_parent->m_propertygridevent );
		}
		m_parent->SyncProperty();
	}
}

ioWnd*	InputProcessor::GetSelWnd( wxMouseEvent& event )
{
	return GetSelWnd( event.GetX(), event.GetY() );
}

ioWnd*	InputProcessor::GetSelWnd( int x, int y )
{
	ioGUIManager* guimgr = m_parent->m_guimgr;		
	POINT pos = { x, y };
	return guimgr->CheckMouse( guimgr->GetDeskTop(), pos );
}