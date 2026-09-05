//----------------------------------------------------------------------------------
/*
Filename	: ioWndEX.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioWndEX.h"
#include "ioFrameTimer.h"
#include "ioXMLElement.h"
#include "ioUIRenderFrame.h"
#include "ioGUIManager.h"
#include "tchar.h"
#include "ioStringConverter.h"

//------------------------------------------------------------------------------------
/*
String Format
Desc : 주의! 멀티 쓰레드에서 안전하지 않음
*/
const char*	Format( const char* format, ... )
{	
	static const unsigned int FORMAT_BUFF_SIZE = 1024;
	static char buffer[ FORMAT_BUFF_SIZE ];
	va_list marker;
	va_start( marker, format );
	_vsntprintf_s( buffer, FORMAT_BUFF_SIZE, format, marker );
	va_end( marker );
	return buffer;
};


//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioWndEX::ioWndEX()
	: ioWnd()
	, m_extype( IWEXT_WND )
{
}

ioWndEX::ioWndEX( IOWNDEX_TYPE type )
	: ioWnd()
	, m_extype( type )
{
}

ioWndEX::~ioWndEX()
{
	ClearConnectors();	
}

 //----------------------------------------------------------------------------------
/*
ioWnd event override
*/
void	ioWndEX::iwm_create()
{
	ioWnd::iwm_create();

	SendEvent<ioWndEXEvent>( WEVT_CREATE, ioWndEXEvent() );
}

void	ioWndEX::iwm_destroy()
{
	ioWnd::iwm_destroy();

	SendEvent<ioWndEXEvent>( WEVT_DESTROY, ioWndEXEvent() );
}

void	ioWndEX::iwm_lbuttondown( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttondown( mouse );

	SendEvent<ioWndEXMouseEvent>( WEVT_LBUTTONDOWN, ioWndEXMouseEvent( mouse.GetMousePos().x, mouse.GetMousePos().y ) );
}

void	ioWndEX::iwm_lbuttonup( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttonup( mouse );

	SendEvent<ioWndEXMouseEvent>( WEVT_LBUTTONUP, ioWndEXMouseEvent( mouse.GetMousePos().x, mouse.GetMousePos().y ) );
}

void	ioWndEX::iwm_lbuttonDBLCLK( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttonDBLCLK( mouse );

	SendEvent<ioWndEXMouseEvent>( WEVT_LBUTTONDBCLICK, ioWndEXMouseEvent( mouse.GetMousePos().x, mouse.GetMousePos().y ) );
}

void	ioWndEX::iwm_rbuttondown( const ioMouse& mouse )
{
	ioWnd::iwm_rbuttondown( mouse );

	SendEvent<ioWndEXMouseEvent>( WEVT_RBUTTONDOWN, ioWndEXMouseEvent( mouse.GetMousePos().x, mouse.GetMousePos().y ) );
}

void	ioWndEX::iwm_rbuttonup( const ioMouse& mouse )
{
	ioWnd::iwm_rbuttonup( mouse );

	SendEvent<ioWndEXMouseEvent>( WEVT_RBUTTONUP, ioWndEXMouseEvent( mouse.GetMousePos().x, mouse.GetMousePos().y ) );
}

void	ioWndEX::iwm_mouseover( const ioMouse& mouse )
{
	ioWnd::iwm_mouseover( mouse );

	SendEvent<ioWndEXMouseEvent>( WEVT_MOUSEOVER, ioWndEXMouseEvent( mouse.GetMousePos().x, mouse.GetMousePos().y ) );
}

void	ioWndEX::iwm_mousemove( const ioMouse& mouse )
{
	ioWnd::iwm_mousemove( mouse );

	SendEvent<ioWndEXMouseEvent>( WEVT_MOUSEMOVE, ioWndEXMouseEvent( mouse.GetMousePos().x, mouse.GetMousePos().y ) );
}

void	ioWndEX::iwm_mouseleave( const ioMouse& mouse )
{
	ioWnd::iwm_mouseleave( mouse );

	SendEvent<ioWndEXMouseEvent>( WEVT_MOUSELEAVE, ioWndEXMouseEvent( mouse.GetMousePos().x, mouse.GetMousePos().y ) );
}

void	ioWndEX::iwm_wheel( int zDelta )
{
	ioWnd::iwm_wheel( zDelta );

	SendEvent<ioWndEXMouseEvent>( WEVT_WHEEL, ioWndEXMouseEvent( zDelta ) );
}

void	ioWndEX::iwm_show()
{
	ioWnd::iwm_show();

	SendEvent<ioWndEXEvent>( WEVT_SHOW, ioWndEXEvent() );
}

void	ioWndEX::iwm_hide()
{
	ioWnd::iwm_hide();

	SendEvent<ioWndEXEvent>( WEVT_HIDE, ioWndEXEvent() );
}

bool	ioWndEX::iwm_esc()
{
	bool ret = ioWnd::iwm_esc();

	SendEvent<ioWndEXEvent>( WEVT_ESC, ioWndEXEvent() );

	return ret;
}

bool	ioWndEX::iwm_spacebar()
{
	bool ret = ioWnd::iwm_spacebar();

	SendEvent<ioWndEXEvent>( WEVT_SPACEBAR, ioWndEXEvent() );

	return ret;
}

//----------------------------------------------------------------------------------
/*
Event
*/
void	ioWndEX::ClearConnectors()
{
	ConnectMap::iterator iter = m_eventconnectors.begin();
	ConnectMap::iterator eiter = m_eventconnectors.end();
	for ( ; iter != eiter; ++iter )
	{
		HandlerVector::iterator iter2 = iter->second.begin();
		HandlerVector::iterator eiter2 = iter->second.end();
		for ( ; iter2 != eiter2; ++iter2 ) {
			delete *iter2;
		}
	}
	m_eventconnectors.clear();
}

void	ioWndEX::Connect( WNDEX_EVENT_TYPE etype, ioWndEXEventHandlerBase* handler )
{
	if ( !handler ) {
		return;
	}

	ConnectMap::iterator iter = m_eventconnectors.find( etype );
	if ( iter == m_eventconnectors.end() )
	{
		HandlerVector newhv;
		newhv.push_back( handler );
		m_eventconnectors.insert( ConnectMap::iterator::value_type( etype, newhv ) );
	}
	else
	{
		// 이미 존재하면 무시
		HandlerVector::iterator iter2 = iter->second.begin();
		HandlerVector::iterator eiter2 = iter->second.end();
		for ( ; iter2 != eiter2; ++iter2 ) {			
			if ( (*iter2)->GetOwnerBase() == handler->GetOwnerBase() ) {
				return;
			}
		}
		iter->second.push_back( handler );
	}
}

void	ioWndEX::Disconnect( WNDEX_EVENT_TYPE etype, ioWndEX* wnd )
{
	if ( !wnd ) {
		return;
	}

	ConnectMap::iterator iter = m_eventconnectors.find( etype );
	if ( iter == m_eventconnectors.end() ) {
		return;
	}

	HandlerVector::iterator iter2 = iter->second.begin();
	HandlerVector::iterator eiter2 = iter->second.end();
	for ( ; iter2 != eiter2; ++iter2 ) 
	{	
		if ( (*iter2)->GetOwnerBase() == wnd ) 
		{
			delete *iter2;
			iter->second.erase( iter2 );
			return;
		}
	}
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioWndEX::SetConnectFunctionName( WNDEX_EVENT_TYPE etype, const char* name )
{
	if ( !name ) {
		return;
	}

	ConnectFunctionNameMap::iterator iter = m_connectfuncnames.find( etype );
	if ( iter == m_connectfuncnames.end() ) 
	{
		m_connectfuncnames.insert( ConnectFunctionNameMap::value_type( etype, name ) );
		return;
	}
	iter->second = name;
}

const char*		ioWndEX::GetConnectFunctionName( WNDEX_EVENT_TYPE etype )
{
	ConnectFunctionNameMap::iterator iter = m_connectfuncnames.find( etype );
	if ( iter == m_connectfuncnames.end() ) {
		return 0;
	}
	return iter->second.c_str();
}

void	ioWndEX::SetSaveData( ioXMLElement& xml )
{
	// set save data
	ioWnd::SetSaveData( xml );

	ioXMLElement extxml = xml.CreateChild( "ExtraInfo" );

	// set ex wnds
	_SetSaveData( extxml );

	// for event
	ioXMLElement toolxml = extxml.CreateChild( "ToolInfo" );
	ConnectFunctionNameMap::iterator iter = m_connectfuncnames.begin();
	ConnectFunctionNameMap::iterator eiter = m_connectfuncnames.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( iter->second == "" ) {
			continue;
		}

		ioXMLElement evtxml = toolxml.CreateChild( "Event" );
		evtxml.SetStringAttribute( "Type", g_wndexeventtype[ iter->first ] );
		evtxml.SetStringAttribute( "Function", iter->second.c_str() );
	}
}

void	ioWndEX::ParseExtraInfo( ioXMLElement& xml )
{
	ioWnd::ParseExtraInfo( xml );

	_ParseExtraInfo( xml );

	ioXMLElement elem = xml.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if ( strcmp( elem.GetTagName(), "ToolInfo" ) == 0 )
		{
			ioXMLElement elem2 = elem.FirstChild();
			while ( !elem2.IsEmpty() )
			{
				if ( strcmp( elem2.GetTagName(), "Event" ) == 0 )
				{
					const char* type = elem2.GetStringAttribute( "Type" );
					const char* function = elem2.GetStringAttribute( "Function" );
					for ( int i = 0; i < WEVT_MAX; ++i )
					{
						if ( strcmp( g_wndexeventtype[ i ], type ) == 0 ) 
						{
							SetConnectFunctionName( static_cast<WNDEX_EVENT_TYPE>( i ), function );
							break;
						}
					}
				}
				elem2 = elem2.NextSibling();
			}			
			break;
		}
		elem = elem.NextSibling();
	}
}

//----------------------------------------------------------------------------------
/*
편의 함수들
*/
bool	ioWndEX::IsInRect( ioWnd** childs, int cnum, int widthweight, int heightweight, const POINT& pos )
{
	RECT rc = { INT_MAX, INT_MAX, 0, 0 };
	for ( int i = 0; i < cnum; ++i )
	{
		if ( childs[ i ] )
		{
			rc.left = std::min<int>( rc.left, childs[ i ]->GetDerivedPosX() );
			rc.top = std::min<int>( rc.top, childs[ i ]->GetDerivedPosY() );

			rc.right = std::max<int>( rc.right, childs[ i ]->GetDerivedPosX() + childs[ i ]->GetWidth() );			
			rc.bottom = std::max<int>( rc.bottom, childs[ i ]->GetDerivedPosY() + childs[ i ]->GetHeight() );			
		}
	}

	rc.left -= widthweight;
	rc.top -= heightweight;
	rc.right += widthweight;
	rc.bottom += heightweight;

	return ( PtInRect( &rc, pos ) == FALSE ? false : true );
}

void	ioWndEX::SetRenderElementSize( ioUIRenderElement* elem, int width, int height )
{
	if ( elem )
	{
		RECT rt = elem->GetRenderRect();
		rt.right = rt.left + width;
		rt.bottom = rt.top + height;
		elem->SetRenderRect( rt );
	}
}

void	ioWndEX::ChangeFrame( ioUIRenderFrame** frame, const char* templatename, ioGUIManager* mgr )
{
	if ( !frame || !templatename || !mgr ) {
		return;
	}

	DWORD color = 0xFFFFFFFF;
	RECT rt = { 0,0,0,0 };
	if ( *frame )
	{
		color = (*frame)->GetColor();
		rt = (*frame)->GetRenderRect();
	}
	SAFEDELETE( *frame );

	*frame = mgr->CloneFrame( templatename );

	if ( *frame ) 
	{
		(*frame)->SetRenderRect( rt );
		(*frame)->SetColor( color );
	}
}

void	ioWndEX::ChangeImageByFileName( ioUIRenderImage** image, const char* filename, ioGUIManager* mgr )
{
	if ( !image || !filename || !mgr ) {
		return;
	}

	DWORD color = 0xFFFFFFFF;
	RECT rt = { 0,0,0,0 };
	if ( *image )
	{
		color = (*image)->GetColor();
		rt = (*image)->GetRenderRect();
	}
	SAFEDELETE( *image );

	const char* setimagename = NULL;
	const char* imagename = NULL;
	StringVector vparam = ioStringConverter::Split( std::string(filename), "#", 2 );
	if( vparam.size() == 2 ){
		if( strcmp( vparam[0].c_str(), "" ) != 0 ){
			setimagename = vparam[0].c_str();
		}
		if( strcmp( vparam[1].c_str(), "" ) != 0 ){
			imagename = vparam[1].c_str();
		}
	}

	*image = mgr->CreateImage( setimagename, imagename );
	if ( *image ) 
	{
		(*image)->SetRenderRect( rt );
		(*image)->SetColor( color );
		(*image)->SetImageSetName( setimagename );
		(*image)->SetImageName( imagename );
		(*image)->SetImageFileName( filename );
	}
}

void	ioWndEX::ChangeImageByImageSet( ioUIRenderImage** image, const char* imagesetname, const char* imagename, ioGUIManager* mgr )
{
	if ( !image || !imagesetname || !imagename || !mgr ) {
		return;
	}

	DWORD color = 0xFFFFFFFF;
	RECT rt = { 0,0,0,0 };
	if ( *image )
	{
		color = (*image)->GetColor();
		rt = (*image)->GetRenderRect();
	}
	SAFEDELETE( *image );

	*image = mgr->CreateImage( imagesetname, imagename );

	if ( *image ) 
	{
		(*image)->SetRenderRect( rt );
		(*image)->SetColor( color );
		(*image)->SetImageSetName( imagesetname );
		(*image)->SetImageName( imagename );
		char szbuf[MAX_PATH];
		sprintf_s( szbuf, "%s#%s", imagesetname, imagename );
		(*image)->SetImageFileName( szbuf );
	}
}