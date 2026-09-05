
/////////////////////////////////////////////////////////////////////////////
// File:	URLLinkButton.cpp
// By:		Nguyen Duc Thanh, Vegasoft, JSC.
//			Copyright 2004, Nguyen Duc Thanh.
// Email:	thanhnd@vegasoft.ws			
// Website: http:\\www.vegasoft.ws
// Date:	July 15, 2004
/////////////////////////////////////////////////////////////////////////////
//
// The CURLLinkButton class extends the functionality of CButton 
// by providing support for URL links
// It displays the URL link and invokes the shell when clicked.
// Tooltip supported, full color customization
//
/////////////////////////////////////////////////////////////////////////////
//
// This code is free for non-commercial use, providing this notice remains 
// intact in the source files and all eventual changes are clearly marked 
// with comments. Do not misrepresent the origin of this code.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk
// 
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "URLLinkButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HCURSOR	 CURLLinkButton::m_hCursorHand = NULL;

/////////////////////////////////////////////////////////////////////////////
// CURLLinkButton

CURLLinkButton::CURLLinkButton()
{
	m_bHover = FALSE;
	m_bTracking = FALSE;
	m_bVisited = FALSE;
	m_bKeyDown = FALSE;

	m_clrVisited = RGB( 0x80, 0x00, 0x80 );
	m_clrRegular = ::GetSysColor(COLOR_HIGHLIGHT);
	m_clrDisabled = ::GetSysColor(COLOR_GRAYTEXT);
	
	#if(WINVER >= 0x0500)
		m_clrHover = ::GetSysColor (COLOR_HOTLIGHT);		
	#else
		m_clrHover = RGB (0x0, 0x0, 0xFF);
	#endif 

	m_pVisitedFont = NULL;

	// Initilize static member once
	if (m_hCursorHand==NULL)
	{		
	#if(WINVER >= 0x0500)
		//Load system hand cursor
		m_hCursorHand = AfxGetApp()->LoadCursor (IDC_HAND);	
	#else
		// Use a custom Hand cursor
		// Must add a cursor resourse in the project with ID: IDC_CURSOR_HAND
		//m_hCursorHand = AfxGetApp()->LoadCursor (IDC_CURSOR_HAND);
		
		// If you haven't the cursor resourse in your project
		// load it from WinHlp32 module 
		//**********************************
		TCHAR szWindowsDir[MAX_PATH];
		GetWindowsDirectory(szWindowsDir ,MAX_PATH);
		strcat(szWindowsDir,"\\Winhlp32.exe");
		HMODULE hModule = LoadLibrary(szWindowsDir);		
		if (hModule)
			m_hCursorHand = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
		//**********************************
		
	#endif 
		
	}
}

CURLLinkButton::~CURLLinkButton()
{
	if ( m_pVisitedFont && m_pVisitedFont->GetSafeHandle())
	{
		m_pVisitedFont->DeleteObject();
		delete m_pVisitedFont;
	}
}


BEGIN_MESSAGE_MAP(CURLLinkButton, CButton)
	//{{AFX_MSG_MAP(CURLLinkButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CURLLinkButton message handlers

void CURLLinkButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);		
	}
	
	CButton::OnMouseMove(nFlags, point);
}

LRESULT CURLLinkButton::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	m_bHover=TRUE;
	Invalidate();

	if (m_ToolTip.m_hWnd)
		if (m_ToolTip.GetToolCount() != 0) 
			m_ToolTip.Activate(TRUE);

	return 0;
}


LRESULT CURLLinkButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bHover=FALSE;
	Invalidate();

	if (m_ToolTip.m_hWnd)
		if (m_ToolTip.GetToolCount() != 0) 
			m_ToolTip.Activate(FALSE);
		
	return 0;
}

void CURLLinkButton::OnClicked() 
{
	if (!IsWindowEnabled ())
	{
		return;
	}

	CWaitCursor wait;

	CString strURL = m_sURL;
	if (strURL.IsEmpty ())
	{
		GetWindowText (strURL);
	}

	if (::ShellExecute (NULL, _T("open"), m_sPrefix + strURL, NULL, NULL, NULL) < (HINSTANCE) 32)
	{
		//Can't open URL, send WM_LINK_CLICKED message to parent window
		CWnd *pParent = GetParent();
		if( pParent )											
		{
			UINT	uiCtrlID = GetDlgCtrlID();		
			pParent -> SendMessage(WM_LINK_CLICKED, (WPARAM)uiCtrlID,(LPARAM)NULL);		
		}
	}
	
	m_bVisited = TRUE;
	m_bHover = FALSE;

	Invalidate ();
	
}

BOOL CURLLinkButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( nHitTest == HTCLIENT )								
	{
		if( m_hCursorHand )									
		{
			::SetCursor( m_hCursorHand );						
			return( TRUE );									
		}
	}

	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void CURLLinkButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	Invalidate();	
}

void CURLLinkButton::SetURL (LPCTSTR lpszURL)
{
	if (lpszURL == NULL)
	{
		m_sURL.Empty ();
	}
	else
	{
		m_sURL = lpszURL;
	}
}

void CURLLinkButton::SetURLPrefix (LPCTSTR lpszPrefix)
{
	ASSERT (lpszPrefix != NULL);
	m_sPrefix = lpszPrefix;
}

void CURLLinkButton::SetDisplayText (LPCTSTR lpszDisplayText)
{
	ASSERT (lpszDisplayText != NULL);
	SetWindowText(lpszDisplayText);
	SizeToContent();
	Invalidate();
}

BOOL CURLLinkButton::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
		{
			m_bKeyDown = TRUE;
			return TRUE;
		}
		break;

	case WM_KEYUP:
		if (pMsg->wParam == VK_SPACE)
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_RETURN)
		{
			if (m_bKeyDown)
			{
				m_bKeyDown = FALSE;
				OnClicked ();
				return TRUE;
			}
		}
		break;
	}
	
	if (m_ToolTip.m_hWnd)
		m_ToolTip.RelayEvent(pMsg);
	
	m_bKeyDown = FALSE;
	
	return CButton::PreTranslateMessage(pMsg);
}

void CURLLinkButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle (lpDrawItemStruct->hDC);
	
	CRect rect = lpDrawItemStruct->rcItem;

	
	if( GetFocus() == this ) 
	{
		//Draw focus rectangle
		pDC->DrawFocusRect(rect);
	}
	
	//Create UL font
	if ( !m_pVisitedFont )
	{
		LOGFONT	lfFont;
		GetFont() -> GetLogFont( &lfFont );			// Normal font

		//////////////////////////////////////////////////////////////////////////
		//Customize the visited font here
		lfFont.lfUnderline = TRUE;		// Set UnderLine flag
		//lfFont.lfItalic = TRUE;		// Set Italic flag

		m_pVisitedFont = new CFont;
		if( m_pVisitedFont )
			m_pVisitedFont->CreateFontIndirect( &lfFont );	// Create UL font
	}
	
	CFont* pOldFont;
	
	// Set text parameters:
	if (!IsWindowEnabled ())
		pDC->SetTextColor(m_clrDisabled);
	else
	{
		// Set visited font if hover
		if (m_bHover)
			pOldFont = pDC->SelectObject (m_pVisitedFont);

		pDC->SetTextColor(m_bHover ? m_clrHover : 
						 (m_bVisited?m_clrVisited:m_clrRegular));
	}

	pDC->SetBkMode (TRANSPARENT);
	
	
		
	// Obtain label:
	CString strLabel;
	GetWindowText (strLabel);

	CRect rectText = rect;
	pDC->DrawText (strLabel, rectText, DT_SINGLELINE);
	 
	if (m_bHover)
		pDC->SelectObject (pOldFont);
}


void CURLLinkButton::SizeToContent()
{
	CClientDC dc (this);

	// Obtain label:
	CString strLabel;
	GetWindowText (strLabel);
	
	//Get the displayed text size
	CSize sizeText = dc.GetTextExtent(strLabel);
	
	//Resize the control
	SetWindowPos (NULL, -1, -1, sizeText.cx, sizeText.cy,
	  			  SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	
}

CString CURLLinkButton::GetDefaultTipText()
{
	CString sTip = m_sURL;
	if (sTip.IsEmpty())
		GetWindowText (sTip);
	
	return m_sPrefix + sTip;
}

void CURLLinkButton::SetToolTipText(CString sTip)
{
	//Use default tooltip if sTip is empty
	if (sTip.IsEmpty())
		sTip = GetDefaultTipText();

	if (m_ToolTip.m_hWnd == NULL)
	{
		m_ToolTip.Create(this);
		m_ToolTip.Activate(FALSE);
	}
	
	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, sTip, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip.UpdateTipText(sTip, this, 1);
}

void CURLLinkButton::SetToolTipTextColor(COLORREF clrTextColor)
{
	m_ToolTip.SetTipTextColor(clrTextColor);
}

void CURLLinkButton::SetToolTipBkColor(COLORREF clrBkColor)
{
	m_ToolTip.SetTipBkColor(clrBkColor);
}

void CURLLinkButton::SetToolTipColor(COLORREF clrTextColor, COLORREF clrBkColor)
{
	SetToolTipTextColor(clrTextColor);
	SetToolTipBkColor(clrBkColor);
}


void CURLLinkButton::SetRegularColor(COLORREF clrRegular)
{
	m_clrRegular = clrRegular;
}

void CURLLinkButton::SetHoverColor(COLORREF clrHover)
{
	m_clrHover = clrHover;
}

void CURLLinkButton::SetVisitedColor(COLORREF clrVisited)
{
	m_clrVisited = clrVisited;
}

void CURLLinkButton::SetLinkColor(COLORREF clrRegular, COLORREF clrHover, COLORREF clrVisited)
{
	SetRegularColor(clrRegular);
	SetHoverColor(clrHover);
	SetVisitedColor(clrVisited);
}


void CURLLinkButton::PreSubclassWindow() 
{
	ModifyStyle (BS_DEFPUSHBUTTON, BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}
