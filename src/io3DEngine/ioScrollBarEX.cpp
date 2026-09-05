//----------------------------------------------------------------------------------
/*
Filename	: ioScrollBarEX.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioScrollBarEX.h"
#include "ioImageWndEX.h"
#include "ioButtonWndEX.h"
#include "ioXMLElement.h"

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioScrollBarEX::ioScrollBarEX( ioGUIManager* mgr )
	: ioFrameWndEX( IWEXT_SCROLLBAR, mgr )
	, m_scrolltype( ST_VERT )
	, m_scrollpos( 0 )
	, m_scrollrange( 100 )
	, m_scrolling( false )
	, m_scroll( 0 )
	, m_decreasebtn( 0 )
	, m_increasebtn( 0 )
{
}

ioScrollBarEX::ioScrollBarEX( IOWNDEX_TYPE type, ioGUIManager* mgr )
	: ioFrameWndEX( type, mgr )
	, m_scrolltype( ST_VERT )
	, m_scrollpos( 0 )
	, m_scrollrange( 100 )
	, m_scrolling( false )
	, m_scroll( 0 )
	, m_decreasebtn( 0 )
	, m_increasebtn( 0 )
{
}

ioScrollBarEX::~ioScrollBarEX()
{
}

//----------------------------------------------------------------------------------
/*
ioWnd Override
*/
void	ioScrollBarEX::OnRender()
{
	ioFrameWndEX::OnRender();

	switch ( m_scrolltype )
	{
	case ST_VERT:
		{
			if ( m_decreasebtn )
			{
				m_decreasebtn->SetPosX( GetWidth() * FLOAT05 - m_decreasebtn->GetWidth() * FLOAT05 );
				m_decreasebtn->SetPosY( -m_decreasebtn->GetHeight() );
			}

			if ( m_increasebtn )
			{
				m_increasebtn->SetPosX( GetWidth() * FLOAT05 - m_increasebtn->GetWidth() * FLOAT05 );
				m_increasebtn->SetPosY( GetHeight() );
			}

			if ( m_scroll )
			{
				m_scroll->SetPosX( GetWidth() * FLOAT05 - m_scroll->GetWidth() * FLOAT05 );

				if ( m_scrollrange == 0 ) 
				{
					m_scroll->SetPosY( 0 );
				}
				else
				{
					float rate = m_scrollpos / static_cast<float>( m_scrollrange );

					m_scroll->SetPosY( rate * ( GetHeight() - m_scroll->GetHeight() ) );
				}
			}
		}	
		break;

	case ST_HORZ:
		{
			if ( m_decreasebtn )
			{
				m_decreasebtn->SetPosX( -m_decreasebtn->GetWidth() );
				m_decreasebtn->SetPosY( GetHeight() * FLOAT05 - m_decreasebtn->GetHeight() * FLOAT05 );
			}

			if ( m_increasebtn )
			{
				m_increasebtn->SetPosX( GetWidth() );
				m_increasebtn->SetPosY( GetHeight() * FLOAT05 - m_increasebtn->GetHeight() * FLOAT05 );
			}

			if ( m_scroll )
			{
				m_scroll->SetPosY( GetHeight() * FLOAT05 - m_scroll->GetHeight() * FLOAT05 );

				if ( m_scrollrange == 0 ) 
				{
					m_scroll->SetPosX( 0 );
				}
				else
				{
					float rate = m_scrollpos / static_cast<float>( m_scrollrange );

					m_scroll->SetPosX( rate * ( GetWidth() - m_scroll->GetWidth() ) );
				}
			}
		}
		break;
	}
}

void	ioScrollBarEX::OnProcess( float fTimePerSec )
{
	ioFrameWndEX::OnProcess( fTimePerSec );

	if ( m_scrolling && m_guimgr )
	{
		if ( m_guimgr->IsLastLButtonDown() )
		{
			POINT curcursor = m_guimgr->GetLastMousePos();

			switch ( m_scrolltype )
			{
			case ST_VERT:
				{
					int deltay = curcursor.y - m_scrolledcursorpos.y;

					float rate = static_cast<float>( deltay ) / static_cast<float>( GetHeight() );

					SetScrollPos( GetScrollPos() + rate * GetScrollRange() );

					SendEvent<ioWndEXScrollBarEvent>( WEVT_SB_SCROLL, ioWndEXScrollBarEvent( GetScrollPos() ) );
				}
				break;

			case ST_HORZ:
				{
					int deltax = curcursor.x - m_scrolledcursorpos.x;

					float rate = static_cast<float>( deltax ) / static_cast<float>( GetWidth() );

					SetScrollPos( GetScrollPos() + rate * GetScrollRange() );

					SendEvent<ioWndEXScrollBarEvent>( WEVT_SB_SCROLL, ioWndEXScrollBarEvent( GetScrollPos() ) );
				}
				break;
			}

			m_scrolledcursorpos = curcursor;
		}
		else
		{
			m_scrolling = false;
		}		
	}
}

void	ioScrollBarEX::AddChild( ioWnd* wnd )
{
	ioFrameWndEX::AddChild( wnd );

	if ( wnd )
	{
		if ( wnd->GetID() == GetID() + SCI_DECBTN )
		{
			m_decreasebtn = dynamic_cast<ioFrameButtonWndEX*>( wnd );
			m_decreasebtn->Connect( WEVT_LBUTTONUP, new ioWndEXMouseEventHandler< ioScrollBarEX >( this, &ioScrollBarEX::OnClickDecrease ) );
		}

		if ( wnd->GetID() == GetID() + SCI_INCBTN ) 
		{
			m_increasebtn = dynamic_cast<ioFrameButtonWndEX*>( wnd );
			m_increasebtn->Connect( WEVT_LBUTTONUP, new ioWndEXMouseEventHandler< ioScrollBarEX >( this, &ioScrollBarEX::OnClickIncrease ) );
		}

		if ( wnd->GetID() == GetID() + SCI_SCROLL )
		{
			m_scroll = dynamic_cast<ioFrameWndEX*>( wnd );
			m_scroll->Connect( WEVT_LBUTTONDOWN, new ioWndEXMouseEventHandler< ioScrollBarEX >( this, &ioScrollBarEX::OnLDownScrollBtn ) );
		}
	}
}

void	ioScrollBarEX::SetIDEX( DWORD& dwID, bool onlychild ) // 확장 ID 셋팅 함수 : 자식까지 전부 unique하게 아이디를 지정해주고... 그 최대값을 저장해준다.
{
	if ( !onlychild ) {
		m_dwID = dwID++;
	}

	if ( m_decreasebtn ) {
		m_decreasebtn->SetID( GetID() + SCI_DECBTN );
	}
	if ( m_increasebtn ) {
		m_increasebtn->SetID( GetID() + SCI_INCBTN );
	}
	if ( m_scroll ) {
		m_scroll->SetID( GetID() + SCI_SCROLL );
	}

	dwID = GetID() + SCI_SCROLL + 1;

	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	for ( ; iter != eiter; ++iter ) 
	{
		if ( m_decreasebtn && m_decreasebtn == *iter ) {
			m_decreasebtn->SetIDEX( dwID, true );
		} else if ( m_increasebtn && m_increasebtn == *iter ) {
			m_increasebtn->SetIDEX( dwID, true );
		} else if ( m_scroll && m_scroll == *iter ) {
			m_scroll->SetIDEX( dwID, true );
		} else {
			(*iter)->SetIDEX( dwID, false );
		}
	}
}

//----------------------------------------------------------------------------------
/*
ioWnd Event Override
*/
void	ioScrollBarEX::iwm_create()
{
	ioFrameWndEX::iwm_create();

	SetWndStyle( GetWndStyle() | IWS_PROCESS );

	m_decreasebtn = dynamic_cast<ioFrameButtonWndEX*>( FindChildWnd( GetID() + SCI_DECBTN ) );
	if ( !m_decreasebtn )
	{
		m_decreasebtn = new ioFrameButtonWndEX( m_guimgr );
		m_decreasebtn->iwm_create();	
		m_decreasebtn->SetID( GetID() + SCI_DECBTN );
		m_decreasebtn->ShowWnd();
		AddChild( m_decreasebtn );
	}
	else
	{
		m_decreasebtn->Connect( WEVT_LBUTTONUP, new ioWndEXMouseEventHandler< ioScrollBarEX >( this, &ioScrollBarEX::OnClickDecrease ) );
	}

	m_increasebtn = dynamic_cast<ioFrameButtonWndEX*>( FindChildWnd( GetID() + SCI_INCBTN ) );
	if ( !m_increasebtn )
	{
		m_increasebtn = new ioFrameButtonWndEX( m_guimgr );
		m_increasebtn->iwm_create();	
		m_increasebtn->SetID( GetID() + SCI_INCBTN );
		m_increasebtn->ShowWnd();
		AddChild( m_increasebtn );
	}
	else
	{
		m_increasebtn->Connect( WEVT_LBUTTONUP, new ioWndEXMouseEventHandler< ioScrollBarEX >( this, &ioScrollBarEX::OnClickIncrease ) );
	}

	m_scroll = dynamic_cast<ioFrameWndEX*>( FindChildWnd( GetID() + SCI_SCROLL ) );
	if ( !m_scroll )
	{
		m_scroll = new ioFrameWndEX( m_guimgr );
		m_scroll->iwm_create();	
		m_scroll->SetID( GetID() + SCI_SCROLL );
		m_scroll->ShowWnd();	
		AddChild( m_scroll );
		m_scroll->SetCheckParentOutSide( true );
	}
	else
	{
		m_scroll->Connect( WEVT_LBUTTONDOWN, new ioWndEXMouseEventHandler< ioScrollBarEX >( this, &ioScrollBarEX::OnLDownScrollBtn ) );
	}
}

void	ioScrollBarEX::iwm_destroy()
{
	//메세지 해제
	if ( m_decreasebtn ) {
		m_decreasebtn->Disconnect( WEVT_LBUTTONUP, this );
	}
	if ( m_increasebtn ) {
		m_increasebtn->Disconnect( WEVT_LBUTTONUP, this );
	}
	if ( m_scroll ) {
		m_scroll->Disconnect( WEVT_LBUTTONDOWN, this );
	}

	ioFrameWndEX::iwm_destroy();
}

//----------------------------------------------------------------------------------
/*
event func
*/
void	ioScrollBarEX::OnClickDecrease( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	SetScrollPos( GetScrollPos() - 1 );

	SendEvent<ioWndEXScrollBarEvent>( WEVT_SB_CLICK_DECREASE, ioWndEXScrollBarEvent( GetScrollPos() ) );	
}

void	ioScrollBarEX::OnClickIncrease( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	SetScrollPos( GetScrollPos() + 1 );

	SendEvent<ioWndEXScrollBarEvent>( WEVT_SB_CLICK_INCREASE, ioWndEXScrollBarEvent( GetScrollPos() ) );
}

void	ioScrollBarEX::OnLDownScrollBtn( ioWndEX* wnd, ioWndEXMouseEvent& event )
{
	m_scrolling = true;

	m_scrolledcursorpos.x = event.m_x;
	m_scrolledcursorpos.y = event.m_y;
}

//----------------------------------------------------------------------------------
/*
Control
*/
void	ioScrollBarEX::SetScrollPos( float pos )
{
	m_scrollpos = std::min<float>( std::max<float>( 0.0f, pos ), m_scrollrange );
}

void	ioScrollBarEX::SetScrollRange( int range )
{
	m_scrollrange = std::max<int>( 0, range );

	SetScrollPos( m_scrollpos );
}

void	ioScrollBarEX::SetScroll( const char* templatename )
{
	if ( m_scroll && templatename ) {
		m_scroll->SetFrameTemplateName( templatename, m_guimgr );
	}
}

void	ioScrollBarEX::SetScrollWidth( int w )
{
	if ( m_scroll ) {
		m_scroll->SetWidth( w );
	}
}

void	ioScrollBarEX::SetScrollHeight( int h )
{
	if ( m_scroll ) {
		m_scroll->SetHeight( h );
	}
}

// dec btn
void	ioScrollBarEX::SetDecBtnTemplateNameNormal( const char* templatename )
{
	if ( m_decreasebtn ) {
		m_decreasebtn->SetFrameTemplateNameNormal( templatename, m_guimgr );
	}
}

void	ioScrollBarEX::SetDecBtnTemplateNameOver( const char* templatename )
{
	if ( m_decreasebtn ) {
		m_decreasebtn->SetFrameTemplateNameOver( templatename, m_guimgr );
	}
}

void	ioScrollBarEX::SetDecBtnTemplateNameOverAdd( const char* templatename )
{
	if ( m_decreasebtn ) {
		m_decreasebtn->SetFrameTemplateNameOverAdd( templatename, m_guimgr );
	}
}

void	ioScrollBarEX::SetDecBtnTemplateNamePush( const char* templatename )
{
	if ( m_decreasebtn ) {
		m_decreasebtn->SetFrameTemplateNamePush( templatename, m_guimgr );
	}
}

void	ioScrollBarEX::SetDecBtnWidth( int w )
{
	if ( m_decreasebtn ) {
		m_decreasebtn->SetWidth( w );
	}
}

void	ioScrollBarEX::SetDecBtnHeight( int h )
{
	if ( m_decreasebtn ) {
		m_decreasebtn->SetHeight( h );
	}
}

void	ioScrollBarEX::SetDecBtnLabelText( const char* txt )
{
	if ( m_decreasebtn ) {
		m_decreasebtn->SetLabelText( txt );
	}
}

void	ioScrollBarEX::SetDecBtnLabelGap( int gap )
{
	if ( m_decreasebtn ) {
		m_decreasebtn->SetLabelTextGap( gap );
	}
}

void	ioScrollBarEX::SetDecBtnLabelVertAlign( TextVertAlign align )
{
	if ( m_decreasebtn ) {
		m_decreasebtn->SetLabelVertAlign( align );
	}
}

void	ioScrollBarEX::SetDecBtnLabelHorzAlign( TextAlignType align )
{
	if ( m_decreasebtn ) {
		m_decreasebtn->SetLabelHorzAlign( align );
	}
}

void	ioScrollBarEX::SetIncBtnTemplateNameNormal( const char* templatename )
{
	if ( m_increasebtn ) {
		m_increasebtn->SetFrameTemplateNameNormal( templatename, m_guimgr );
	}
}

void	ioScrollBarEX::SetIncBtnTemplateNameOver( const char* templatename )
{
	if ( m_increasebtn ) {
		m_increasebtn->SetFrameTemplateNameOver( templatename, m_guimgr );
	}
}

void	ioScrollBarEX::SetIncBtnTemplateNameOverAdd( const char* templatename )
{
	if ( m_increasebtn ) {
		m_increasebtn->SetFrameTemplateNameOverAdd( templatename, m_guimgr );
	}
}

void	ioScrollBarEX::SetIncBtnTemplateNamePush( const char* templatename )
{
	if ( m_increasebtn ) {
		m_increasebtn->SetFrameTemplateNamePush( templatename, m_guimgr );
	}
}

void	ioScrollBarEX::SetIncBtnWidth( int w )
{
	if ( m_increasebtn ) {
		m_increasebtn->SetWidth( w );
	}
}

void	ioScrollBarEX::SetIncBtnHeight( int h )
{
	if ( m_increasebtn ) {
		m_increasebtn->SetHeight( h );
	}
}

void	ioScrollBarEX::SetIncBtnLabelText( const char* txt )
{
	if ( m_increasebtn ) {
		m_increasebtn->SetLabelText( txt );
	}
}

void	ioScrollBarEX::SetIncBtnLabelGap( int gap )
{
	if ( m_increasebtn ) {
		m_increasebtn->SetLabelTextGap( gap );
	}
}

void	ioScrollBarEX::SetIncBtnLabelVertAlign( TextVertAlign align )
{
	if ( m_increasebtn ) {
		m_increasebtn->SetLabelVertAlign( align );
	}
}

void	ioScrollBarEX::SetIncBtnLabelHorzAlign( TextAlignType align )
{
	if ( m_increasebtn ) {
		m_increasebtn->SetLabelHorzAlign( align );
	}
}

//----------------------------------------------------------------------------------
/*
ioWnd 재정의 ParseExtraInfo
*/
void	ioScrollBarEX::ParseExtraInfo( ioXMLElement& extxml )
{
	ioFrameWndEX::ParseExtraInfo( extxml );

	m_scrolltype = static_cast<SCROLL_TYPE>( extxml.GetIntAttribute( "ScrollType" ) );
	m_scrollrange = extxml.GetIntAttribute( "ScrollRange" );

	m_scrollpos = 0;
	m_scrolling = false;
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioScrollBarEX::Copy( ioWnd* wnd )
{
	ioFrameWndEX::Copy( wnd );

	ioScrollBarEX* scrollwnd = dynamic_cast<ioScrollBarEX*>(wnd);
	if ( !scrollwnd ) {
		return;
	}

	scrollwnd->m_scrolltype = m_scrolltype;
	scrollwnd->m_scrollrange = m_scrollrange;

	scrollwnd->m_scrollpos = 0;
	scrollwnd->m_scrolling = false;
}

void	ioScrollBarEX::_SetSaveData( ioXMLElement& extxml )
{
	ioFrameWndEX::_SetSaveData( extxml );

	extxml.SetIntAttribute( "ScrollType", m_scrolltype );
	extxml.SetIntAttribute( "ScrollRange", m_scrollrange );
}