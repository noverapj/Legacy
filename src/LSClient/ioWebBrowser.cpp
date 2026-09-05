

#include "stdafx.h"

#include "Channeling/ioChannelingNodeParent.h"
#include "Channeling/ioChannelingNodeManager.h"

#include "WellKnownURL.h"
#include "ioBrowserEvents.h"
#include "ioWebBrowser.h"
#include "ioBrowserManager.h"
#include "BrowserWndProc.h"
#include "resource.h"
#ifdef XTRAP
#include "Xtrap/ioXtrap.h"
#endif 

#include <comutil.h>

#define BLANK_PAGE	"about:blank"

/*void __stdcall _com_issue_error(long error)
{
	//nop
}*/

ioWebBrowser::ioWebBrowser( ioBrowserManager *pCreator, BrowsePageType ePageType )
{
	m_pCreator = pCreator;
	m_BrowsePageType = ePageType;
	m_BrowsePageSubType = BROWSE_SUB_DEFAULT;

	m_hWebParent = NULL;
	m_hExplorer  = NULL;

	m_pInvokeEvent   = NULL;
	m_dwAdviceCookie = 0;

	m_LoginState = LS_NOT_LOGIN;

	m_bWindowDrag = false;

	m_bCloseReserved  = false;
	m_bShow = false;
	m_bHideProcess = false;
	m_nexonWindowClosed = false;

	m_bNeedExcuteWeb = false;
	m_bNoCloseXTrap = false;
}

ioWebBrowser::~ioWebBrowser()
{
	Release();
}

bool ioWebBrowser::Initialize( HWND hClient, HINSTANCE hInstance )
{
	int iWidth  = 0;
	int iHeight = 0;
	int iXPos   = 0;
	int iYPos   = 0;

	m_nIEhPosX = 0;
	m_nIEhPosY = 0;
	m_nIEhSizeX = 0;
	m_nIEhSizeY = 0;

	GetWindowSizePos(iWidth, iHeight, iXPos, iYPos);

	m_hWebParent = CreateWindow( ATL_WEB_PARENT,
								 NULL,
								 WS_POPUP | WS_CHILD,
								 iXPos, iYPos,
								 iWidth  + 6,
								 iHeight + 32,
								 hClient,
								 (HMENU)0,
								 hInstance,
								 NULL );

	if( !m_hWebParent )	return false;

	// 비주얼 스튜디오 10.0이라 AtlAxWin100
	m_hExplorer = CreateWindowEx( 0,
								  "AtlAxWin100",
								  "Shell.Explorer.2",
								  WS_CHILD | WS_VISIBLE,
								  3, 29, iWidth, iHeight,
								  m_hWebParent,
                                  (HMENU)0,
								  GetModuleHandle(NULL),
								  NULL );

	m_nIEhPosX = 3;
	m_nIEhPosY = 29;
	m_nIEhSizeX = iWidth;
	m_nIEhSizeY = iHeight;

	if( !m_hExplorer )
		return false;

	WebBrowser2ComPtr pBrowser = GetWebBrowser2ComPtr();
	if( !pBrowser )	return false;

	SAFEDELETE( m_pInvokeEvent );
	m_pInvokeEvent = new ioBrowserEvents( this );

	bool bResult = ConnectionAdvise( (LPUNKNOWN)pBrowser,
									 DIID_DWebBrowserEvents2,
									 (IDispatch*)m_pInvokeEvent,
									 FALSE,
									 &m_dwAdviceCookie );

	SetNoScroll();

#ifdef XTRAP
	if( m_BrowsePageType == BROWSE_FILL_CASH )
		g_ioXtrap.StartPayment();
#endif 

	return bResult;
}

void ioWebBrowser::Release()
{
	if( m_pInvokeEvent && m_dwAdviceCookie )
	{
		WebBrowser2ComPtr pBrowser = GetWebBrowser2ComPtr();
		if( pBrowser )
		{
			ConnectionUnadvise( (LPUNKNOWN)pBrowser,
								DIID_DWebBrowserEvents2,
								(IDispatch*)m_pInvokeEvent,
								FALSE,
								m_dwAdviceCookie );

			m_dwAdviceCookie = 0;
		}

		SAFEDELETE( m_pInvokeEvent );
	}

	if( m_hExplorer )
	{
		::DestroyWindow( m_hExplorer );
	}

	if( m_hWebParent )
	{
		::DestroyWindow( m_hWebParent );
	}
	HideProcess();
}

void ioWebBrowser::WebParentDragStart()
{
	 ::GetCursorPos( &m_ptPreCursor );
	 m_bWindowDrag = true;
}

void ioWebBrowser::ProcessDrag()
{
	//by 윤태철
	//길드홈피를 열면 IE 사이즈가 어느순간 작아 져서 오른쪽 아래에 붙는다.
	//왜 그런지는 모르겠다.
	//우선 임시 방편으로 생성시 위치와 사이즈를 기억해서 
	//리사이징 한다.
	if( m_hWebParent && m_hExplorer )
		MoveWindow( m_hExplorer, m_nIEhPosX, m_nIEhPosY, m_nIEhSizeX, m_nIEhSizeY, FALSE);

	if( !m_bWindowDrag )	return;

	POINT ptCurPos;
	::GetCursorPos( &ptCurPos );

	if( m_ptPreCursor.x == ptCurPos.x && m_ptPreCursor.y == ptCurPos.y )
		return;

	RECT rcRect;
	::GetWindowRect( m_hWebParent, &rcRect );

	rcRect.left += ptCurPos.x - m_ptPreCursor.x;
	rcRect.top  += ptCurPos.y - m_ptPreCursor.y;
	::SetWindowPos( m_hWebParent, NULL, rcRect.left, rcRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER );

	m_ptPreCursor = ptCurPos;
}

void ioWebBrowser::WebParentDragEnd()
{
	ProcessDrag();

	m_bWindowDrag = false;
}

void ioWebBrowser::ShowBrowser( bool bShow )
{
	if( m_bShow == bShow )
		return;

	// SetDialogMode가 창이 열리기전에 먼저 세팅이 되어야 창에 뒷배경이 묻어나지 않는다.
	// 그래서 먼저 m_bShow를 세팅하고 CheckDeviceModeAndMouse 를 호출함
	m_bShow = bShow;

	if( bShow )
	{
		m_pCreator->CheckDeviceModeAndMouse();

		::SetFocus( m_hWebParent );
		ShowWindow( m_hWebParent, SW_SHOW );

		m_bHideProcess = true;
	}
	else
	{
		ShowWindow( m_hWebParent, SW_HIDE );
		Navigate( BLANK_PAGE );

		HideProcess();
	}
}

void ioWebBrowser::HideProcess()
{
	if( !m_bHideProcess ) return;
	if( !m_pCreator ) return;
	
	m_pCreator->NotifyHideProcess( m_BrowsePageType );

	if( m_BrowsePageType == BROWSE_FILL_CASH )
	{
		
#ifdef XTRAP
			if( !m_bNoCloseXTrap )
				g_ioXtrap.EndPayment();
#endif 
		
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
			pLocal->OnCashPageHide();
	}

	m_bHideProcess = false;
}

void ioWebBrowser::ReserveClose()
{
	ShowBrowser( false );
	m_bCloseReserved = true;
}


void ioWebBrowser::SetNeedExcuteWeb(bool bNeed)
{
	m_bNeedExcuteWeb = bNeed;
}


void ioWebBrowser::NavigateWithLogin( const char *szLoginURL, const char *szURL )
{
	switch( m_LoginState )
	{
	case LS_NOT_LOGIN:
		m_ReservedURL = szURL;
		m_LoginState  = LS_LOGINNING;

		Navigate( szLoginURL );
		break;
	case LS_LOGINNING:
		m_ReservedURL = szURL;
		break;
	case LS_LOGIN_DONE:

		if( m_bNeedExcuteWeb )
		{
			m_bNoCloseXTrap = true;
		
			PROCESS_INFORMATION IEProcessInfo;
			IELAUNCHURLINFO LunchInfo;

			Help::ExcuteIE( IEProcessInfo, LunchInfo, szURL );
			g_BrowserMgr.SetSeeIEProcessID( IEProcessInfo.dwProcessId );
			::SleepEx( 1000, FALSE );
			//LOG.PrintTimeAndLog( 0, "ioWebBrowser::NavigateWithLogin Done!!" );

			ReserveClose();
		}
		else
		{
			Navigate( szURL );
		}

		break;
	}
}

void ioWebBrowser::Navigate( const char *szURL )
{
	WebBrowser2ComPtr pBrowser = GetWebBrowser2ComPtr();
	if( !pBrowser )	return;

	VARIANT myURL;
	VariantInit(&myURL);
	myURL.vt = VT_BSTR;

	DWORD dwSize = MultiByteToWideChar( CP_ACP, 0, szURL, -1, 0, 0 );
	wchar_t *pBuffer = (wchar_t*)GlobalAlloc( GMEM_FIXED, sizeof(wchar_t) * dwSize );
	if( pBuffer )
	{
		MultiByteToWideChar(CP_ACP, 0, szURL, -1, pBuffer, dwSize);
		myURL.bstrVal = SysAllocString(pBuffer);
		GlobalFree( pBuffer );

		//UJ 131007, 렉권방지: 브라우져에서 alert 띄워서 렉을 유발하는 행위를 방지함
		pBrowser->put_Silent( VARIANT_TRUE );

		pBrowser->Navigate2( &myURL, 0, 0, 0, 0 );
	}

	VariantClear(&myURL);
}

void ioWebBrowser::Refresh()
{
	WebBrowser2ComPtr pBrowser = GetWebBrowser2ComPtr();
	if( pBrowser )
	{
		pBrowser->Refresh();
	}
}

//UJ 131007, 렉권방지: alert()류 윈도우를 띄워 렉권 유발하는 방법을 차단한다
void ioWebBrowser::SuppressAlerts()
{
	WebBrowser2ComPtr pBrowser = GetWebBrowser2ComPtr();
	if( !pBrowser )	return;

	CComPtr<IDispatch> pDocDisp = NULL;
	pBrowser->get_Document( &pDocDisp );
	if( pDocDisp )
	{
		CComPtr<IHTMLDocument2> pDoc = NULL;
		CComPtr<IHTMLElementCollection> pElements = NULL;
		pDocDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pDoc );
		if( !pDoc ) return;

		pDoc->get_scripts( &pElements );
		if( !pElements ) return;

		IDispatch *pDisp = NULL;
		pElements->item( CComVariant(0, VT_I4), CComVariant(0, VT_I4), &pDisp );
		if( !pDisp ) return;

		IHTMLScriptElement *pScript = NULL;
		pDisp->QueryInterface( IID_IHTMLScriptElement, (void**)&pScript );
		SAFERELEASE( pDisp );
		if( !pScript ) return;

		BSTR bstrOldScript;
		if( SUCCEEDED( pScript->get_text(&bstrOldScript) ))
		{
			_bstr_t bstrNewScript = L"window.alert=function(){};";
			bstrNewScript += bstrOldScript;
			pScript->put_text( bstrNewScript );
			::SysFreeString( bstrOldScript );
		}
		SAFERELEASE( pScript );
	}
}

void ioWebBrowser::SetBodyNoScroll()
{
	WebBrowser2ComPtr pBrowser = GetWebBrowser2ComPtr();
	if( !pBrowser )	return;

	CComPtr<IDispatch> pDocDisp = NULL;
	pBrowser->get_Document( &pDocDisp );
	if( pDocDisp )
	{
		CComPtr<IHTMLDocument2> pDoc = NULL;
		pDocDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pDoc );
		if( pDoc )
		{
			CComPtr<IHTMLElement> pElement = NULL;
			pDoc->get_body( &pElement );

			if( pElement )
			{
				CComPtr<IHTMLBodyElement> pBodyElement = NULL;
				pElement->QueryInterface( IID_IHTMLBodyElement, (void**)&pBodyElement );
				if( pBodyElement )
				{
					pBodyElement->put_scroll( SysAllocString(L"no") );
				}
			}
		}
	}
}

void ioWebBrowser::GetWebWindowRect( RECT *pRect )
{
	::GetWindowRect( m_hWebParent, pRect );
}

bool ioWebBrowser::IsAutoLoginTime() const
{
	if( m_LoginState == LS_LOGINNING )
		return true;

	return false;
}

bool ioWebBrowser::IsFocusInBrowser( HWND hFocus ) const
{
	while( hFocus != NULL )
	{
		if( m_hWebParent == hFocus )
			return true;

		hFocus = ::GetParent( hFocus );
	}

	return false;
}

void ioWebBrowser::OnBeforeNavigate2( IDispatch *pDisp,
									  VARIANT *url,
									  VARIANT *Flags,
									  VARIANT *TargetFrameName,
									  VARIANT *PostData,
									  VARIANT *Headers,
									  VARIANT_BOOL *Cancel )
{
	char szNewURL[MAX_PATH*2];
	ZeroMemory( szNewURL, sizeof( szNewURL ) );
	int iBSTRLen = SysStringLen( url->bstrVal );
	int iStrLen  = WideCharToMultiByte(CP_ACP, 0,  url->bstrVal, iBSTRLen, szNewURL , MAX_PATH*2 , NULL, NULL);
	if(COMPARE(iStrLen, 0, MAX_PATH*2))
		szNewURL[iStrLen]=NULL;
	else
		szNewURL[(MAX_PATH*2)-1] = NULL;

	bool bBlankPage = false;
	if( !strcmp( szNewURL, BLANK_PAGE ) ) 
	{
		bBlankPage = true;
	}

	char szNewTitle[MAX_PATH];
	ZeroMemory( szNewTitle, sizeof( szNewTitle ) );
	iBSTRLen = SysStringLen( TargetFrameName->bstrVal );
	iStrLen  = WideCharToMultiByte(CP_ACP, 0,  TargetFrameName->bstrVal, iBSTRLen, szNewTitle , MAX_PATH , NULL, NULL);
	if(COMPARE(iStrLen, 0, MAX_PATH))
		szNewTitle[iStrLen]=NULL;
	else
		szNewTitle[MAX_PATH-1] = NULL;

	if( !strcmp( szNewTitle, "miniHome" ) || !strcmp( szNewTitle, "avamall" ) )
	{
		ioWebBrowser *pHeadBrowser = m_pCreator->FindBrowserByPageType( BROWSE_SINGLE );
		if( pHeadBrowser )
		{
			pHeadBrowser->Navigate( szNewURL );
			pHeadBrowser->ShowBrowser( true );

			*Cancel = VARIANT_TRUE;

			if( this != pHeadBrowser )
			{
				ReserveClose();
			}
		}
	}
	else
	{
		if( !IsAutoLoginTime() && !bBlankPage )
		{
			CComPtr<IWebBrowser2> spBrowser;
			pDisp->QueryInterface( IID_IWebBrowser2, (void**)&spBrowser );

			// Top Level Frame...
			if( GetWebBrowser2ComPtr() == spBrowser )
			{
				//UJ 130722, fix Nexon's payment page not closing
				if (g_MyInfo.GetChannelingType() == CNT_NEXON && m_nexonWindowClosed)
				{
					if (strnicmp( szNewURL, "https://nxpay.nexon.com", 23) == 0)
					{
						m_nexonWindowClosed = false;
						return;
					}
				}

				ShowBrowser( true );
			}
		}
	}
}

void ioWebBrowser::OnNavigateComplete2(IDispatch *pDisp, VARIANT *URL)
{
	//UJ 131007, 렉권방지: 게임중일때는 웹브라우져에서 팝업 띄우지 않는다
	if( g_App.IsPlaying() )
		SuppressAlerts();
	SetBodyNoScroll();
}

void ioWebBrowser::OnTitleChange( BSTR szTitle )
{
	char szCharTitle[MAX_PATH];
	ZeroMemory( szCharTitle, sizeof( szCharTitle ) );
	int iBSTRLen = SysStringLen( szTitle );
	int iStrLen  = WideCharToMultiByte(CP_ACP, 0, szTitle, iBSTRLen, szCharTitle , MAX_PATH , NULL, NULL);
	if(COMPARE(iStrLen, 0, MAX_PATH))
		szCharTitle[iStrLen]=NULL;
	else
		szCharTitle[MAX_PATH-1] = NULL;

	::SetWindowText( m_hWebParent, szCharTitle );
	InvalidateRect( m_hWebParent, NULL, FALSE );

	SetBodyNoScroll();

	// HARDCODE : 같은 타이틀이 한번 이상오는 경우가 있다.
	//            새로 기능을 추가한다면 중복 실행을 막을 방안을 마련해야함
	if( strstr( szCharTitle, STR(1) ) )
	{
		StringVector vParam = ioStringConverter::Split( std::string(szCharTitle), " " );
		if( vParam.size() == 2 )
		{
			if( vParam[1] == STR(2) )
				m_pCreator->NotifyDeleteFriendByWeb( vParam[0].c_str() );
		}
	}
	else if( strstr( szCharTitle , STR(3)) )
	{
		StringVector vParam = ioStringConverter::Split( std::string(szCharTitle), " " );
		m_pCreator->NotifyUserEntryRefresh( vParam[0].c_str() );
	}
	else if( strstr( szCharTitle, STR(4) ) )
	{
		if( GetBrowsePageType() == BROWSE_GUILD_MARK )
			m_pCreator->NotifyGuildMarkChangeComplete();
	}
}

void ioWebBrowser::OnClientToHostWindow( long *CX,long *CY )
{
	int iWidth  = *CX;
	int iHeight = *CY;

	RECT rcRect;
	GetWindowRect( m_hWebParent, &rcRect );
	MoveWindow( m_hWebParent, rcRect.left, rcRect.top, iWidth + 3*2, iHeight + 29 + 3, FALSE );
	SetWindowPos( m_hExplorer, 0, 0, 0, iWidth, iHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER );
}

void ioWebBrowser::OnNewWindow( IDispatch **ppDisp, VARIANT_BOOL *pCancel )
{
	ioWebBrowser *pNewBrowser = m_pCreator->OpenBrowser( "",
														 BROWSE_DEFAULT,
														 BROWSE_SUB_DEFAULT,
														 false );
	if( pNewBrowser )
	{
		WebBrowser2ComPtr pBrowser2 = pNewBrowser->GetWebBrowser2ComPtr();
		if( pBrowser2 )
		{
			pBrowser2->QueryInterface( IID_IDispatch, (void**)ppDisp );
		}
	}
}

void ioWebBrowser::OnDocumentComplete(IDispatch *pDisp, VARIANT *URL)
{
	CComPtr<IWebBrowser2> spBrowser;
	pDisp->QueryInterface( IID_IWebBrowser2, (void**)&spBrowser );

	// Top Level Frame
	if( GetWebBrowser2ComPtr() == spBrowser )
	{
		if( m_LoginState == LS_LOGINNING )
		{
			m_LoginState = LS_LOGIN_DONE;
			
			//윤태철
			if( m_bNeedExcuteWeb )
			{
				m_bNoCloseXTrap = true;

				PROCESS_INFORMATION IEProcessInfo;
				IELAUNCHURLINFO LunchInfo;

				g_BrowserMgr.SetSeeIEProcessID( 0 );
				Help::ExcuteIE( IEProcessInfo, LunchInfo, m_ReservedURL.c_str() );
				g_BrowserMgr.SetSeeIEProcessID( IEProcessInfo.dwProcessId );
				::SleepEx( 1000, FALSE );
				//LOG.PrintTimeAndLog( 0, "ioWebBrowser::OnDocumentComplete" );

				ReserveClose();
			}
			else
				Navigate( m_ReservedURL.c_str() );
		}
	}
}

void ioWebBrowser::OnWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel)
{
	*Cancel = TRUE;
	ShowBrowser( false );

	//UJ 130722, fix Nexon's payment page not closing
	if (g_MyInfo.GetChannelingType() == CNT_NEXON)
	{
		if (IsChildWindow != VARIANT_FALSE)
			m_nexonWindowClosed = true;
	}
}

bool ioWebBrowser::ConnectionAdvise( LPUNKNOWN pUnkSrc, REFIID iid,LPUNKNOWN pUnkSink, BOOL bRefCount, DWORD* pdwCookie )
{
	HRESULT hr;
	bool bSuccess = false;

	LPCONNECTIONPOINTCONTAINER pCPC;
	hr = pUnkSrc->QueryInterface( IID_IConnectionPointContainer, (LPVOID*)&pCPC );
	if( SUCCEEDED(hr) )
	{
		LPCONNECTIONPOINT pCP;
		hr = pCPC->FindConnectionPoint(iid, &pCP);
		if( SUCCEEDED(hr) )
		{
			hr = pCP->Advise(pUnkSink, pdwCookie);
			if( SUCCEEDED(hr) )
				bSuccess = true;

			pCP->Release();

			if (bSuccess && !bRefCount)
				pUnkSink->Release();
		}

		pCPC->Release();
	}

	return bSuccess;
}

bool ioWebBrowser::ConnectionUnadvise(LPUNKNOWN pUnkSrc, REFIID iid,LPUNKNOWN pUnkSink, BOOL bRefCount, DWORD dwCookie)
{
	if (!bRefCount)
		pUnkSink->AddRef();

	HRESULT hr;
	bool bSuccess = false;

	LPCONNECTIONPOINTCONTAINER pCPC;
	hr = pUnkSrc->QueryInterface( IID_IConnectionPointContainer, (LPVOID*)&pCPC );
	if( SUCCEEDED(hr) )
	{
		LPCONNECTIONPOINT pCP;
		hr = pCPC->FindConnectionPoint(iid, &pCP);
		if( SUCCEEDED(hr) )
		{
			hr = pCP->Unadvise(dwCookie);
			if( SUCCEEDED( hr ) )
				bSuccess = true;

			pCP->Release();
		}

		pCPC->Release();
	}

	if (!bRefCount && !bSuccess)
		pUnkSink->Release();

	return bSuccess;
}

ioWebBrowser::WebBrowser2ComPtr ioWebBrowser::GetWebBrowser2ComPtr()
{
	WebBrowser2ComPtr pBrowser;

	UnknownComPtr pUnknown; 
	if( AtlAxGetControl( m_hExplorer, &pUnknown ) == S_OK )
	{
		pBrowser = pUnknown;
	}

	return pBrowser;
}

bool ioWebBrowser::IsDownLoadComplete()
{
	WebBrowser2ComPtr pBrowser = GetWebBrowser2ComPtr();
	if( !pBrowser )	return false;

	READYSTATE kState;
	if( pBrowser->get_ReadyState( &kState ) == S_OK )
	{
		if( kState == READYSTATE_COMPLETE )
		{
			return true;
		}
		else
		{
			// 0 == FALSE, -1 == TRUE 
			VARIANT_BOOL bBusy;
			if( pBrowser->get_Busy( &bBusy ) == S_OK )
			{
				if( bBusy == 0 )
					return true;
				else
					return false;
			}
		}
	}

	return true;
}

void ioWebBrowser::GetWindowSizePos( OUT int &riWidth,OUT int &riHeight,OUT int &riXPos,OUT int &riYPos )
{
	enum { DEF_WIDTH = 792, DEF_HEIGHT = 570, SINGLE_WIDTH = 792, SINGLE_HEIGHT = 570, ENTRY_WIDTH = 750, ENTRY_HIGHT = 555, GUILD_MARK_WIDTH = 515, GUILD_MARK_HEIGHT = 460,
	       CASH_DEFAULT_WIDTH= 450, CASH_DEFAULT_HEIGHT= 310, CASH_ENTRY_WIDTH = 750, CASH_ENTRY_HEIGHT = 555, };

	if( m_BrowsePageType == BROWSE_DEFAULT )
	{
		riWidth  = DEF_WIDTH;
		riHeight = DEF_HEIGHT;

		riXPos = 0;
		riYPos = 0;
	}
	else if( m_BrowsePageType == BROWSE_SINGLE )
	{
		//14.12.09 홈페이지 리뉴얼 관련 (미니홈피, 길드홈피 사이즈 추가)
		if ( m_pCreator )
		{
			riWidth  = SINGLE_WIDTH + m_pCreator->GetBrowserSingleExtraWidth();
			riHeight = SINGLE_HEIGHT + m_pCreator->GetBrowserSingleExtraHeight();
		}
		else
		{
			riWidth  = SINGLE_WIDTH;
			riHeight = SINGLE_HEIGHT;
		}

		riXPos = 0;
		riYPos = 0;
	}
	else if( m_BrowsePageType == BROWSE_FORMALITY_ENTRY )
	{
		riWidth  = ENTRY_WIDTH;
		riHeight = ENTRY_HIGHT;

		riXPos = (Setting::Width()  - riWidth ) / 2;
		riYPos = (Setting::Height() - riHeight ) / 2;
	}	
	else if( m_BrowsePageType == BROWSE_GUILD_MARK )
	{
		riWidth  = GUILD_MARK_WIDTH;
		riHeight = GUILD_MARK_HEIGHT;

		riXPos = (Setting::Width()  - riWidth ) / 2;
		riYPos = (Setting::Height() - riHeight ) / 2;
	}
	else if( m_BrowsePageType == BROWSE_FILL_CASH )
	{
		riWidth  = CASH_DEFAULT_WIDTH;
		riHeight = CASH_DEFAULT_HEIGHT;

		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			riWidth = pLocal->GetCashPageWidth();
			riHeight= pLocal->GetCashPageHeight();
		}

		riXPos = (Setting::Width()  - riWidth ) / 2;
		riYPos = (Setting::Height() - riHeight ) / 2;
	}
	else if( m_BrowsePageType == BROWSE_CASH_ENTRY )
	{
		riWidth  = CASH_ENTRY_WIDTH;
		riHeight = CASH_ENTRY_HEIGHT;

		riXPos = (Setting::Width()  - riWidth ) / 2;
		riYPos = (Setting::Height() - riHeight ) / 2;
	}
}

void ioWebBrowser::SetNoScroll()
{
	if( m_BrowsePageType == BROWSE_FORMALITY_ENTRY || 
		m_BrowsePageType == BROWSE_GUILD_MARK      || 
		m_BrowsePageType == BROWSE_FILL_CASH       ||
		m_BrowsePageType == BROWSE_CASH_ENTRY      )
	{
		UnknownComPtr spUnk;
		if(  AtlAxGetHost( m_hExplorer, &spUnk ) == S_OK )
		{
			CComQIPtr< IAxWinAmbientDispatch> spWinAmb( spUnk );
			spWinAmb->put_DocHostFlags(DOCHOSTUIFLAG_SCROLL_NO|DOCHOSTUIFLAG_NO3DBORDER );
		}
	}
}