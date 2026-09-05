

#include "stdafx.h"

#include "Channeling/ioChannelingNodeParent.h"
#include "Channeling/ioChannelingNodeManager.h"

#include "WellKnownURL.h"
#include "ioWebBrowser.h"
#include "ioBrowserEvents.h"
#include "ioBrowserManager.h"
#include "BrowserWndProc.h"
#include "resource.h"

CComModule _Module;
BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

//-----------------------------------------------------------------------------

template<> ioBrowserManager* Singleton< ioBrowserManager >::ms_Singleton = 0;

ioBrowserManager::ioBrowserManager()
{
	m_hInstance = NULL;
	m_hClient = NULL;

	m_dwRefreshTime  = 0;

	m_hWebTitleHome  = NULL;
	m_hWebTitleEntry = NULL;
	m_hWebTitleGuild = NULL;
	m_hWebTitleGuildMark = NULL;
	m_hWebTitleCash  = NULL;
	m_hWebOutLine    = NULL;
	m_hWebExit       = NULL;

	m_dwSeeProcess = 0;
	m_dwSendCashInfoTime = 0;
}

ioBrowserManager::~ioBrowserManager()
{
	WebBrowserMap::iterator iter = m_WebBrowserMap.begin();
	for( ; iter!=m_WebBrowserMap.end() ; ++iter )
	{
		delete iter->second;
	}
	m_WebBrowserMap.clear();

	DeleteObject( m_hWebTitleHome );
	DeleteObject( m_hWebTitleEntry );
	DeleteObject( m_hWebTitleGuild );
	DeleteObject( m_hWebTitleGuildMark );
	DeleteObject( m_hWebTitleCash );
	DeleteObject( m_hWebOutLine );
	DeleteObject( m_hWebExit );

	OleUninitialize();
}

ioBrowserManager& ioBrowserManager::GetSingleton()
{
	return Singleton< ioBrowserManager >::GetSingleton();
}

void ioBrowserManager::Initialize( HINSTANCE hInstance, HWND hClient, const ioHashString &szKeyOnCmd )
{
	m_hInstance = hInstance;
	m_hClient = hClient;
	m_szKeyOnCmd = szKeyOnCmd;

	_Module.Init(NULL, hInstance, &LIBID_ATLLib);
	AtlAxWinInit();

	OleInitialize(NULL);

	// Register Web Parent WndClass
	WNDCLASSEX wcChild;
	ZeroMemory( &wcChild, sizeof(wcChild) );

	wcChild.cbSize		  = sizeof(wcChild);
	wcChild.hInstance	  = hInstance;
	wcChild.lpfnWndProc	  = (WNDPROC)WebParentProc;
	wcChild.lpszClassName = ATL_WEB_PARENT;
	RegisterClassEx( &wcChild );

	DWORD dwIDHomeBmp      = IDB_WEB_TITLE_HOME;
	DWORD dwIDEntryBmp     = IDB_WEB_TITLE_ENTRY;
	DWORD dwIDGuildBmp     = IDB_WEB_TITLE_GUILD;
	DWORD dwIDGuildMarkBmp = IDB_WEB_TITLE_GUILD_MARK;
	DWORD dwIDCashBmp      = IDB_WEB_TITLE_CASH;

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		dwIDHomeBmp      = pLocal->GetResourceIDTitleHomeBmp();
		dwIDEntryBmp     = pLocal->GetResourceIDTitleEntryBmp();
		dwIDGuildBmp     = pLocal->GetResourceIDTitleGuildBmp();
		dwIDGuildMarkBmp = pLocal->GetResourceIDTitleGuildMarkBmp();
		dwIDCashBmp      = pLocal->GetResourceIDTitleCashBmp();
	}

	m_hWebTitleHome    = LoadBitmap( hInstance, MAKEINTRESOURCE(dwIDHomeBmp) );
	m_hWebTitleEntry   = LoadBitmap( hInstance, MAKEINTRESOURCE(dwIDEntryBmp) );
	m_hWebTitleGuild   = LoadBitmap( hInstance, MAKEINTRESOURCE(dwIDGuildBmp) );
	m_hWebTitleGuildMark = LoadBitmap( hInstance, MAKEINTRESOURCE(dwIDGuildMarkBmp) );
	m_hWebTitleCash    = LoadBitmap( hInstance, MAKEINTRESOURCE(dwIDCashBmp) );
	m_hWebOutLine      = LoadBitmap( hInstance, MAKEINTRESOURCE(IDB_WEB_OUTLINE) );
	m_hWebExit         = LoadBitmap( hInstance, MAKEINTRESOURCE(IDB_EXIT_BUTTON) );
}

void ioBrowserManager::LoadINI( const char *szININame )
{
	ioINILoader kLoader( szININame );

	char szBuf[MAX_PATH];
	kLoader.LoadString( "web", "login", "", szBuf, MAX_PATH );
	m_AutoLoginURL = szBuf;

	kLoader.LoadString( "web", "headquater", "", szBuf, MAX_PATH );
	m_HeadQuaterURL = szBuf;

	kLoader.LoadString( "web", "headquarter_mini", "", szBuf, MAX_PATH );
	m_HeadquartersMiniURL = szBuf;

	kLoader.LoadString( "web", "formality_entry", "", szBuf, MAX_PATH );
	m_FormalityEntryURL = szBuf;

	kLoader.LoadString( "web", "guild_homepage", "", szBuf, MAX_PATH );
	m_GuildHomePageURL = szBuf;

	kLoader.LoadString( "web", "guild_mark_page", "", szBuf, MAX_PATH );
	m_GuildMarkPageURL = szBuf;

	kLoader.LoadString( "web", "channeling_url", "", szBuf, MAX_PATH );
	m_ChannelingURL	= szBuf;

	kLoader.LoadString( "web", "cash", "", szBuf, MAX_PATH );
	m_CashURL = szBuf;

	kLoader.LoadString( "web", "cash_entry", "", szBuf, MAX_PATH );
	m_CashEntryURL = szBuf;
	
	kLoader.LoadString( "web", "probability_page", "", szBuf, MAX_PATH );
	m_ProbabilityURL = szBuf;

	m_nBrowserSingleExtraWidth = kLoader.LoadInt( "web", "browser_single_extra_width", 0 );
	m_nBrowserSingleExtraHeight = kLoader.LoadInt( "web", "browser_single_extra_height", 0 );
}

void ioBrowserManager::Process()
{
	ClearCloseReserved();
	HideBrowserByEscape();
	RefreshLogin();
	ProcessWebParentDrag();

	CheckDeviceModeAndMouse();

	//윤태철
	ProcessSeeIEProcess();
}


void ioBrowserManager::ProcessSeeIEProcess()
{
	if( m_dwSeeProcess )
	{
		if(GetProcessVersion(m_dwSeeProcess) == NULL) 
		{
			m_dwSeeProcess = 0;
			//필요한 일을 하자

			SP2Packet kPacket( CTPK_GET_CASH );
			TCPNetwork::SendToServer( kPacket );
			LOG.PrintTimeAndLog( 0, "~~~~~Send GOLD CASH INFO~~~~~");
			TCPNetwork::MouseBusy( true );
#ifdef XTRAP
			g_ioXtrap.EndPayment();
#endif
		}
	}
}


void ioBrowserManager::SetSeeIEProcessID( DWORD dwIEId)
{
	m_dwSeeProcess = dwIEId;

#ifdef XTRAP
	g_ioXtrap.StartPayment();
#endif
}


void ioBrowserManager::ClearCloseReserved()
{
	WebBrowserMap::iterator iter = m_WebBrowserMap.begin();
	while( iter != m_WebBrowserMap.end() )
	{
		ioWebBrowser *pBrowser = iter->second;

		if( pBrowser->IsCloseReserved() )
		{
			iter = m_WebBrowserMap.erase( iter );
			delete pBrowser;
		}
		else
		{
			++iter;
		}
	}
}

void ioBrowserManager::HideBrowserByEscape()
{
/*
	ESC키를 처리하는 곳이 여러곳이다.
	우선 포커스가 있을수 있는 상황들은

	1. 클라이언트가 소유
	2. 웹페이지의 부모가 소유
	3. 웹페이지내의 컨트롤중에서 소유 or 다른 프로그램이 소유

	1번 상황은 DirectInput이 키입력을 받는 상황이고,
	게임내의 다른 UI의 창닫기와 함께 조율하기위해 g_Input을 사용한 ioSPGUIManager::ProcessGUI에서 처리된다.

	2번 상황은 WebParentProc에서 WM_KEYDOWN으로 처리되고
	3번이 문제인데 3번은 여기서 처리한다.
*/

	// 3번상황에서 무조건 WebParent에게 포커스를 줘버리면 웹컨트롤이 제대로 작동을 안한다
	if( GetAsyncKeyState( VK_ESCAPE ) >= 0 )
		return;

	HWND hFocus = ::GetFocus();

	if( hFocus != m_hClient )
	{
		WebBrowserMap::iterator iter=m_WebBrowserMap.begin();
		for( ; iter!=m_WebBrowserMap.end() ; ++iter )
		{
			ioWebBrowser *pBrowser = iter->second;
			if( !pBrowser->IsShow() )
				continue;

			if( pBrowser->IsFocusInBrowser( hFocus ) )
			{
				//UJ 130722, fix for cursor disappearing on javascript alert
				if (IsWindowEnabled(pBrowser->GetParentHWND()) == FALSE)
					continue;

				pBrowser->ShowBrowser( false );
				::SetFocus( m_hClient );
				break;
			}
		}
	}
}

ioWebBrowser* ioBrowserManager::OpenBrowser( const char *szURL,
											 BrowsePageType ePageType,
											 BrowsePageSubType ePageSubType,
											 bool bNeedLogin, bool bNeedExcuteWeb )
{
	// 임시 렉권 by Minyeong
	if( !g_App.IsOpenWebPageState() )
	{
		g_ChatMgr.SetSystemMsg( STR(1) );
		return NULL;
	}
	//
	ioWebBrowser *pOpenBrowser = NULL;

	if( ePageType == BROWSE_SINGLE )
	{
		pOpenBrowser = FindBrowserByPageType( ePageType );

		if( !pOpenBrowser )
		{
			pOpenBrowser = OpenNewBrowser( ePageType );
		}
	}
	else if( ePageType == BROWSE_GUILD_MARK )
	{
		pOpenBrowser = FindBrowserByPageType( ePageType );

		if( !pOpenBrowser )
		{
			pOpenBrowser = OpenNewBrowser( ePageType );
		}
	}
	else
	{
		pOpenBrowser = OpenNewBrowser( ePageType );
		// 윤태철
		if( pOpenBrowser && ePageType == BROWSE_FILL_CASH )
			pOpenBrowser->SetNeedExcuteWeb( bNeedExcuteWeb );
	}

	if( pOpenBrowser )
	{
		pOpenBrowser->SetBrowsePageSubType( ePageSubType );

		if( bNeedLogin )
		{
			ioHashString sFullLoginURL;
			ioChannelingNodeParent *pChannelingNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
			if( pChannelingNode )
			{
				pChannelingNode->GetLoginURL( sFullLoginURL, m_AutoLoginURL, m_szKeyOnCmd );
			}

			pOpenBrowser->NavigateWithLogin( sFullLoginURL.c_str(), szURL );

			if( m_dwRefreshTime == 0 )	// First Call NavigateWithLogin()
			{
				m_dwRefreshTime = REALGETTIME();
			}
		}
		else
		{
			pOpenBrowser->Navigate( szURL );
		}
	}

	return pOpenBrowser;
}


ioHashString ioBrowserManager::GetAutoLoginUrl()
{
	return m_AutoLoginURL;
}


ioWebBrowser* ioBrowserManager::OpenNewBrowser( BrowsePageType ePageType )
{
	ioWebBrowser *pNewBrowser = new ioWebBrowser( this, ePageType );

	if( pNewBrowser->Initialize( m_hClient, m_hInstance ) )
	{
		HWND hWebParent = pNewBrowser->GetParentHWND();
		m_WebBrowserMap.insert( WebBrowserMap::value_type( hWebParent, pNewBrowser ) );
		return pNewBrowser;
	}

	SAFEDELETE( pNewBrowser );
	return NULL;
}

ioWebBrowser* ioBrowserManager::FindBrowser( HWND hParent )
{
	WebBrowserMap::iterator iter = m_WebBrowserMap.find( hParent );
	if( iter != m_WebBrowserMap.end() )
		return iter->second;

	return NULL;
}

ioWebBrowser* ioBrowserManager::FindBrowserByPageType( BrowsePageType ePageType )
{
	ioWebBrowser *pBrowser = NULL;

	WebBrowserMap::iterator iter = m_WebBrowserMap.begin();
	for( ; iter!=m_WebBrowserMap.end() ; ++iter )
	{
		pBrowser = iter->second;

		if(pBrowser->GetBrowsePageType() == ePageType )
			return pBrowser;
	}

	return NULL;
}

bool ioBrowserManager::CloseBrowser( HWND hWnd )
{
	WebBrowserMap::iterator iter = m_WebBrowserMap.find( hWnd );
	if( iter == m_WebBrowserMap.end() )
		return true;

	ioWebBrowser *pBrowser = iter->second;
	if( pBrowser->GetBrowsePageType() == BROWSE_SINGLE )
	{
		//UJ 130722, fix for cursor disappearing on javascript alert
		if (IsWindowEnabled(pBrowser->GetParentHWND()) == FALSE)
			return true;

		pBrowser->ShowBrowser( false );
		return false;
	}

	m_WebBrowserMap.erase( iter );
	SAFEDELETE( pBrowser );

	return true;
}

bool ioBrowserManager::CloseBrowserByType( BrowsePageType eType )
{
	WebBrowserMap::iterator iter = m_WebBrowserMap.begin();
	for( ; iter!=m_WebBrowserMap.end() ; ++iter )
	{
		ioWebBrowser *pBrowser = iter->second;
		if( !pBrowser->IsShow() )
			continue;

		if( pBrowser->GetBrowsePageType() == eType )
		{
			//UJ 130722, fix for cursor disappearing on javascript alert
			if (IsWindowEnabled(pBrowser->GetParentHWND()) == FALSE)
				continue;

			pBrowser->ShowBrowser( false );
			return true;
		}
	}

	return false;
}

// 임시 렉권 by Minyeong
void ioBrowserManager::CloseAllBrowser()
{
	WebBrowserMap::iterator iter = m_WebBrowserMap.begin();
	for( ; iter != m_WebBrowserMap.end(); iter++ )
	{
		ioWebBrowser *pBrowser = iter->second;
		pBrowser->ShowBrowser( false );
		m_WebBrowserMap.erase( iter );
		SAFEDELETE( pBrowser );
	}
}
//

bool ioBrowserManager::HasFocusInBrowser( HWND hFocus )
{
	WebBrowserMap::iterator iter = m_WebBrowserMap.begin();
	for( ; iter!=m_WebBrowserMap.end() ; ++iter )
	{
		ioWebBrowser *pBrowser = iter->second;
		if( !pBrowser->IsShow() )
			continue;

		if( pBrowser->IsFocusInBrowser( hFocus ) )
			return true;
	}

	return false;
}

void ioBrowserManager::NotifyDeleteFriendByWeb( const char *szFriendNick )
{
	g_App.NotifyDeleteFriendByWeb( szFriendNick );
}

void ioBrowserManager::NotifyUserEntryRefresh( const char *szNick )
{
	g_App.NotifyUserEntryRefresh( szNick );
}

void ioBrowserManager::NotifyGuildMarkChangeComplete()
{
	g_App.NotifyGuildMarkChangeComplete();
}

void ioBrowserManager::NotifyHideProcess( BrowsePageType eType )
{
	g_App.NotifyHideProcess( eType );
}

void ioBrowserManager::RefreshLogin()
{
	if( m_dwRefreshTime == 0 )	return;

	DWORD dwPastTime = REALGETTIME() - m_dwRefreshTime;
	dwPastTime /= 1000;

	if( dwPastTime > 3600 )	// 1시간
	{
		ioWebBrowser *pBrowser = FindBrowserByPageType( BROWSE_SINGLE );
		if( pBrowser )
		{
			pBrowser->Refresh();
			m_dwRefreshTime = REALGETTIME();
		}
	}
}

void ioBrowserManager::ProcessWebParentDrag()
{
	WebBrowserMap::iterator iter = m_WebBrowserMap.begin();
	for( ; iter!=m_WebBrowserMap.end() ; ++iter )
	{
		ioWebBrowser *pBrowser = iter->second;
		if( !pBrowser->IsCloseReserved() && pBrowser->IsShow() )
		{
			pBrowser->ProcessDrag();
		}
	}
}

void ioBrowserManager::CheckDeviceModeAndMouse()
{
	RECT rcWindow;
	bool bCurserInWindow = false;

	POINT ptPoint;
	GetCursorPos( &ptPoint );

	int nWebBrowserCount = 0;
	WebBrowserMap::iterator iter = m_WebBrowserMap.begin();
	for( ; iter!=m_WebBrowserMap.end() ; ++iter )
	{
		ioWebBrowser *pBrowser = iter->second;

		if( pBrowser->IsCloseReserved() || !pBrowser->IsShow() )
			continue;

		nWebBrowserCount++;
		pBrowser->GetWebWindowRect( &rcWindow );

		if( PtInRect( &rcWindow, ptPoint ) )
		{
			bCurserInWindow = true;
			break;
		}
	}

	if( bCurserInWindow )
	{
		g_App.ShowWindowCursor( SWCT_WEB, true );
	}
	else
	{
		if( nWebBrowserCount )
		{
			//UJ 130722, fix for cursor disappearing on javascript alert
			HWND hWindowOnCursor = WindowFromPoint(ptPoint);
			if (hWindowOnCursor && HasFocusInBrowser(hWindowOnCursor))
			{
				g_App.ShowWindowCursor( SWCT_WEB, true );
				g_App.SetDialogBoxMode( IsWebOpened() );
				return;
			}
		}
		
		g_App.ShowWindowCursor( SWCT_WEB, false );
	}

	g_App.SetDialogBoxMode( IsWebOpened() );
}

void ioBrowserManager::WebParentClicked( HWND hWnd )
{
	ioWebBrowser *pBrowser = FindBrowser( hWnd );
	if( pBrowser )
	{
		pBrowser->WebParentDragStart();
	}
}

void ioBrowserManager::WebParentClickOff( HWND hWnd )
{
	ioWebBrowser *pBrowser = FindBrowser( hWnd );
	if( pBrowser )
	{
		pBrowser->WebParentDragEnd();
	}
}

int ioBrowserManager::GetBrowserCount() const
{
	return m_WebBrowserMap.size();
}

bool ioBrowserManager::IsWebOpened() const
{
	WebBrowserMap::const_iterator iter = m_WebBrowserMap.begin();
	for( ; iter!=m_WebBrowserMap.end() ; ++iter )
	{
		if( iter->second->IsShow() )
			return true;
	}

	return false;
}

void ioBrowserManager::DrawWebParent( HWND hWnd, HDC hDC, RECT rcClient )
{
	int iClientWidth  = rcClient.right - rcClient.left;
	int iClientHeight = rcClient.bottom - rcClient.top;
	if( iClientWidth <= 0 || iClientHeight <= 0 )
		return;

	ioWebBrowser *pBrowser = FindBrowser( hWnd );
	if( !pBrowser )
		return;

	HDC hBitmapDC = CreateCompatibleDC( hDC );
	
	HBITMAP hOldBitmap = NULL;
	switch( pBrowser->GetBrowsePageSubType() )
	{
	case BROWSE_SUB_QUEATER:
		hOldBitmap = (HBITMAP)SelectObject( hBitmapDC, m_hWebTitleHome );
		break;
	case BROWSE_SUB_FORMALITY_ENTRY:
	case BROWSE_SUB_CASH_ENTRY:
		hOldBitmap = (HBITMAP)SelectObject( hBitmapDC, m_hWebTitleEntry );
		break;
	case BROWSE_SUB_GUILD:
		hOldBitmap = (HBITMAP)SelectObject( hBitmapDC, m_hWebTitleGuild );
		break;
	case BROWSE_SUB_GUILD_MARK:
		hOldBitmap = (HBITMAP)SelectObject( hBitmapDC, m_hWebTitleGuildMark );
		break;
	case BROWSE_SUB_FILL_CASH:
		hOldBitmap = (HBITMAP)SelectObject( hBitmapDC, m_hWebTitleCash );
		break;
	default:
		hOldBitmap = (HBITMAP)SelectObject( hBitmapDC, m_hWebTitleHome );
		break;
	}

	// top ( 이미지 하나를 삼등분해서 사용한다 )

	enum { FRONT_WIDTH = 76, HEIGHT = 29, };
	StretchBlt( hDC, 0, 0, FRONT_WIDTH, HEIGHT, hBitmapDC, 0, 0, FRONT_WIDTH, HEIGHT, SRCCOPY ); // title front

	enum { X_START = 76, CENTER_WIDTH = 8, END_WIDTH = 11 };
	int iTopCenterWidth = iClientWidth - FRONT_WIDTH + END_WIDTH;  // title center
	StretchBlt( hDC, FRONT_WIDTH, 0, iTopCenterWidth, HEIGHT, hBitmapDC, X_START, 0, CENTER_WIDTH, HEIGHT, SRCCOPY );

	int iTopRightStart = iClientWidth - END_WIDTH;
	StretchBlt( hDC, iTopRightStart, 0, END_WIDTH, HEIGHT, hBitmapDC, X_START, 0, END_WIDTH, HEIGHT, SRCCOPY ); // title end

	// left & right
	SelectObject( hBitmapDC, m_hWebOutLine );

	int iSideHeight = iClientHeight - 29 - 3;
	StretchBlt( hDC, 0, 29, 3, iSideHeight, hBitmapDC, 0, 0, 3, 3, SRCCOPY );

	int iSideRightStart = iClientWidth - 3;
	StretchBlt( hDC, iSideRightStart, 29, 3, iSideHeight, hBitmapDC, 3 * 2, 0, 3, 3, SRCCOPY );

	// bottom
	int iBottomStart = iClientHeight - 3;
	StretchBlt( hDC, 0, iBottomStart, 3, 3, hBitmapDC, 0, 3, 3, 3, SRCCOPY );

	int iBottomCenterWidth = iClientWidth - 3 * 2;
	StretchBlt( hDC, 3, iBottomStart, iBottomCenterWidth, 3, hBitmapDC, 3, 3, 3, 3, SRCCOPY );

	int iBottomRightStart = iClientWidth - 3;
	StretchBlt( hDC, iBottomRightStart, iBottomStart, 3, 3, hBitmapDC, 3 * 2, 3, 3, 3, SRCCOPY );

	SelectObject( hBitmapDC, hOldBitmap );
	DeleteDC( hBitmapDC );
}

void ioBrowserManager::DrawExitButton( HDC hButtonDC, int iItemState )
{
	HDC hDC = CreateCompatibleDC( hButtonDC );
	HBITMAP hOldBitmap = (HBITMAP)SelectObject( hDC, m_hWebExit );

	StretchBlt( hButtonDC, 0, 0, 22, 22, hDC, iItemState * 22, 0, 22, 22, SRCCOPY );

	SelectObject( hDC, hOldBitmap );
	DeleteDC( hDC );
}

bool ioBrowserManager::OnPreMessage( MSG &msg )
{
	if( msg.message != WM_KEYDOWN )
		return false;
	if( msg.wParam != VK_TAB )
		return false;

	WebBrowserMap::iterator iter = m_WebBrowserMap.begin();
	for( ; iter!=m_WebBrowserMap.end() ; ++iter )
	{
		ioWebBrowser *pBrowser = iter->second;
		if( !pBrowser )
			continue;
		if( !pBrowser->GetWebBrowser2ComPtr() )
			continue;

		if( !pBrowser->IsShow() )
			continue;

		HWND hFocus = ::GetFocus();
		if( !pBrowser->IsFocusInBrowser( hFocus ) )
			continue;

		// IWebBrowser2 버그로 탭키등 키 값을 처리 못하므로 강제로 넣어주어야 함.
		IOleInPlaceActiveObject* pIOIPAO = NULL;
		HRESULT hr = pBrowser->GetWebBrowser2ComPtr()->QueryInterface( IID_IOleInPlaceActiveObject, (void**)&pIOIPAO );
		if (SUCCEEDED(hr))
			hr = pIOIPAO->TranslateAccelerator(&msg);   
		pIOIPAO->Release();

		if( SUCCEEDED( hr ) )
			return true;
		else
			return false;
	}

	return false;
}

int ioBrowserManager::GetBrowserSingleExtraWidth()
{
	return m_nBrowserSingleExtraWidth;
}

int ioBrowserManager::GetBrowserSingleExtraHeight()
{
	return m_nBrowserSingleExtraHeight;
}