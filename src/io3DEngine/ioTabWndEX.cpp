//----------------------------------------------------------------------------------
/*
Filename	: ioTabWndEX.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioTabWndEX.h"
#include "ioXMLElement.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioTabWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

static const int TAB_DOT_DEF_GAP = 1;
static const int TAB_DOT_DEF_HEIGHT = 2;

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioTabWndEX::ioTabWndEX( ioGUIManager* mgr )
	: ioRadioWndEX( IWEXT_TABWND )
	, m_guimgr( mgr )
	, m_dot( 0 )
	, m_dotgap( TAB_DOT_DEF_GAP )
	, m_dotheight( TAB_DOT_DEF_HEIGHT )
{
	m_dot = m_guimgr->CreateImage( "BaseImage001", "tap2_line" );
}

ioTabWndEX::ioTabWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr ) 
	: ioRadioWndEX( type )
	, m_guimgr( mgr )
	, m_dot( 0 )
	, m_dotgap( TAB_DOT_DEF_GAP )
	, m_dotheight( TAB_DOT_DEF_HEIGHT )
{
	m_dot = m_guimgr->CreateImage( "BaseImage001", "tap2_line" );
}

ioTabWndEX::~ioTabWndEX()
{
	SAFEDELETE( m_dot );
}

//----------------------------------------------------------------------------------
/*
ioWnd override
*/
void	ioTabWndEX::AddChild( ioWnd* child )
{
	if ( !child ) {
		return;
	}

	ioRadioWndEX::AddChild( child );

	LayoutTabButton();
}

void	ioTabWndEX::DeleteChild( DWORD id )
{
	ioRadioWndEX::DeleteChild( id );

	LayoutTabButton();
}

void	ioTabWndEX::OnRender()
{
	ioRadioWndEX::OnRender();

	if ( m_dot )
	{
		m_dot->SetAutoSize( true );
		m_dot->SetSize( m_dotgap, m_dotheight );

		int cnt = 0;
		{
			ioWndList::iterator iter = m_ChildList.begin();
			ioWndList::iterator eiter = m_ChildList.end();
			for ( ; iter != eiter; ++iter )
			{
				ioTabFrameButtonEX* exwnd = dynamic_cast<ioTabFrameButtonEX*>( *iter );
				if ( exwnd ) {
					++cnt;
				}
			}
		}

		int x = 0;
		int y = 0;
		ioWndList::iterator iter = m_ChildList.begin();
		ioWndList::iterator eiter = m_ChildList.end();
		for ( ; iter != eiter; ++iter )
		{
			ioTabFrameButtonEX* exwnd = dynamic_cast<ioTabFrameButtonEX*>( *iter );
			if ( exwnd )
			{
				--cnt;

				int ex = exwnd->GetDerivedPosX();
				int ey = exwnd->GetDerivedPosY();

				if ( cnt > 0 ) 
				{
					m_dot->Render( ex + exwnd->GetWidth(), ey + exwnd->GetHeight() - m_dotheight );
					x += ( exwnd->GetWidth() + m_dotgap );
				}
				else 
				{
					x += exwnd->GetWidth();
				}

				y = ey + exwnd->GetHeight() - m_dotheight;
			}
		}

		m_dot->SetSize( GetWidth() - x, m_dotheight );
		m_dot->Render( GetDerivedPosX() + x, y );
	}
}

void	ioTabWndEX::SetWindowAlpha( int alpha )
{
	ioRadioWndEX::SetWindowAlpha( alpha );

	if ( m_dot ) {
		m_dot->SetAlpha( alpha );
	}
}

//----------------------------------------------------------------------------------
/*
callback
*/
void	ioTabWndEX::OnResizeTabBtn( ioButtonWndEX* btn )
{
	LayoutTabButton();
}

void	ioTabWndEX::OnTabClickedActive( ioButtonWndEX* btn, int idx )
{
	if ( btn )
	{
		btn->SetToggle( true );
		btn->ShowChildWnd();

		SendEvent<ioWndEXRadioEvent>( WEVT_TABCHECK, ioWndEXRadioEvent( btn, idx ) );
	}
}

void	ioTabWndEX::OnTabClickedUnActive( ioButtonWndEX* btn )
{
	if ( btn )
	{
		btn->SetToggle( false );
		btn->HideChildWnd();
	}
}

//----------------------------------------------------------------------------------
/*
control
*/
void	ioTabWndEX::LayoutTabButton()
{
	int x = 0;
	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	for ( ; iter != eiter; ++iter )
	{
		ioTabFrameButtonEX* exwnd = dynamic_cast<ioTabFrameButtonEX*>( *iter );
		if ( exwnd )
		{
			exwnd->SetWndPos( x, 0 );
			x += ( exwnd->GetWidth() + m_dotgap );
		}
	}
}

bool	ioTabWndEX::IsControlType( IOWNDEX_TYPE type ) const
{
	switch ( type )
	{
	case IWEXT_TABFRAMEBUTTON:
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioTabWndEX::Copy( ioWnd* wnd )
{
	ioRadioWndEX::Copy( wnd );

	ioTabWndEX* tabwnd = dynamic_cast<ioTabWndEX*>(wnd);
	if ( !tabwnd ) {
		return;
	}

	CLONEIMAGE( tabwnd, m_dot );

	tabwnd->m_dotgap = m_dotgap;
	tabwnd->m_dotheight = m_dotheight;
}

void	ioTabWndEX::_SetSaveData( ioXMLElement& extxml )
{
	ioRadioWndEX::_SetSaveData( extxml );

	ioImageButtonWndEX::SetImageDataToXML( m_dot, extxml, "DotImage" );

	extxml.SetIntAttribute( "DotGap", m_dotgap );
	extxml.SetIntAttribute( "DotHeight", m_dotheight );
}

void	ioTabWndEX::_ParseExtraInfo( ioXMLElement& extxml )
{
	if ( !m_guimgr ) {
		return;
	}

	ioRadioWndEX::_ParseExtraInfo( extxml );

	SAFEDELETE( m_dot );

	ioImageButtonWndEX::SetImageDataFromXML( reinterpret_cast<ioUIRenderElement**>(&m_dot), extxml, "DotImage", m_guimgr );

	SetDotGap( extxml.GetIntAttribute( "DotGap" ) );
	SetDotHeight( extxml.GetIntAttribute( "DotHeight" ) );
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioTabFrameButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioTabFrameButtonEX::ioTabFrameButtonEX( ioGUIManager* mgr )
	: ioRadioFrameButtonEX( IWEXT_TABFRAMEBUTTON, mgr ) 
{
}

ioTabFrameButtonEX::ioTabFrameButtonEX( IOWNDEX_TYPE type, ioGUIManager* mgr ) 
	: ioRadioFrameButtonEX( type, mgr )
{
}


//----------------------------------------------------------------------------------
/*
ioWnd Override
*/
void	ioTabFrameButtonEX::SetPosX( int x )
{
	ioRadioFrameButtonEX::SetPosX( x );

	ioTabWndEX* tabwnd = dynamic_cast<ioTabWndEX*>( GetParent() );
	if ( tabwnd ) {
		tabwnd->OnResizeTabBtn( this );
	}
}

void	ioTabFrameButtonEX::SetPosY( int y )
{
	ioRadioFrameButtonEX::SetPosY( y );

	ioTabWndEX* tabwnd = dynamic_cast<ioTabWndEX*>( GetParent() );
	if ( tabwnd ) {
		tabwnd->OnResizeTabBtn( this );
	}
}

void	ioTabFrameButtonEX::SetWidth( int w )
{
	ioRadioFrameButtonEX::SetWidth( w );

	ioTabWndEX* tabwnd = dynamic_cast<ioTabWndEX*>( GetParent() );
	if ( tabwnd ) {
		tabwnd->OnResizeTabBtn( this );
	}
}

void	ioTabFrameButtonEX::SetHeight( int h )
{
	ioRadioFrameButtonEX::SetHeight( h );

	ioTabWndEX* tabwnd = dynamic_cast<ioTabWndEX*>( GetParent() );
	if ( tabwnd ) {
		tabwnd->OnResizeTabBtn( this );
	}
}