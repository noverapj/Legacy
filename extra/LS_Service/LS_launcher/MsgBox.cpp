// MsgBox.cpp : implementation file
//

/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h" //Replace with your PCH file
/////////////////////////////////////////////////////////////////////////////
#include "MsgBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgBox

IMPLEMENT_DYNAMIC(CMsgBox, CWnd)
CMsgBox::CMsgBox(CWnd* pParent)
{
	// Create a dummpy child window. It gets attached to this CWnd Object
	Create(NULL, 
		   _T("{62BAB41D-B6BB-402C-89EF-5B86830DF68C}"), 
		   WS_OVERLAPPED, CRect(0,0,0,0),
		   pParent,
		   1000);
	m_bChildCreated = TRUE;
	m_Caption = _T("");
}

CMsgBox::CMsgBox()
{
	m_bChildCreated = FALSE;
	m_Caption = _T("");
}


CMsgBox::~CMsgBox()
{
}


BEGIN_MESSAGE_MAP(CMsgBox, CWnd)
	//{{AFX_MSG_MAP(CMsgBox)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMsgBox message handlers

void CMsgBox::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	BOOL bRetVal = false;
	
	// Find the message box window
	CWnd* pWnd = FindWindow(NULL, m_Caption);
	if(pWnd != NULL)
	{
		// Send close command to the message box window
		::PostMessage(pWnd->m_hWnd, WM_CLOSE, 0, 0);
	}

	// Kill the timer
	KillTimer(100);

	CWnd::OnTimer(nIDEvent);
}

void CMsgBox::MessageBox(CString sMsg, CString sCaption, UINT nSleep, 
						 UINT nFlags, bool bAutoClose)
{
	// Save the caption, for finding this 
	// message box window later
	m_Caption = sCaption;
	
	// If auto close selected then, start the timer.
	if(bAutoClose)
		SetTimer(100, nSleep, NULL);
	
	// Show the message box
	CWnd::MessageBox(sMsg, sCaption, nFlags);
}

// This method called only once
void CMsgBox::SetParent(CWnd* pParent)
{
	// Create a dummpy child window. It gets attached to this CWnd Object
	if(!m_bChildCreated)
	{
		Create(NULL, 
		   _T("{62BAB41D-B6BB-402C-89EF-5B86830DF68C}"), 
		   WS_OVERLAPPED, CRect(0,0,0,0),
		   pParent,
		   1000);
		m_bChildCreated = TRUE;
	}
}