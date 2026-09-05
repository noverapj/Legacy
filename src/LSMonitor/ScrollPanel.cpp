// ScrollPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "ScrollPanel.h"
#include "afxdialogex.h"


// CScrollPanel 대화 상자입니다.

IMPLEMENT_DYNAMIC(CScrollPanel, CDialogEx)

CScrollPanel::CScrollPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScrollPanel::IDD, pParent)
{

}

CScrollPanel::~CScrollPanel()
{
}

void CScrollPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScrollPanel, CDialogEx)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

BOOL CScrollPanel::OnInitDialog()
{
	CDialog::OnInitDialog();

 

	//SetClassLong(m_hWnd,GCL_HCURSOR,(long)m_hCursor1);

	// save the original size
	GetWindowRect(m_rcOriginalRect);

	// initial scroll position
	m_nScrollPos = 0; 

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE

}

void CScrollPanel::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	int nDelta;
	int nMaxPos = m_rcOriginalRect.Height() - m_nCurHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;

		nDelta = min(max(nMaxPos/20,5),nMaxPos-m_nScrollPos);
		break;

	case SB_LINEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(max(nMaxPos/20,5),m_nScrollPos);
		break;
	case SB_PAGEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(max(nMaxPos/10,5),nMaxPos-m_nScrollPos);
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;

	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(max(nMaxPos/10,5),m_nScrollPos);
		break;

	default:
		return;
	}
	m_nScrollPos += nDelta;
	SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
	ScrollWindow(0,-nDelta);
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

}

void CScrollPanel::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	m_nCurHeight = cy;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL; 
	si.nMin = 0;
	si.nMax = m_rcOriginalRect.Height();
	si.nPage = cy;
	si.nPos = 0;
	SetScrollInfo(SB_VERT, &si, TRUE); 	
}

BOOL CScrollPanel::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int nMaxPos = m_rcOriginalRect.Height() - m_nCurHeight;

	if (zDelta<0)
	{
		if (m_nScrollPos < nMaxPos)
		{
			zDelta = min(max(nMaxPos/20,5),nMaxPos-m_nScrollPos);

			m_nScrollPos += zDelta;
			SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
			ScrollWindow(0,-zDelta);
		}
	}
	else
	{
		if (m_nScrollPos > 0)
		{
			zDelta = -min(max(nMaxPos/20,5),m_nScrollPos);

			m_nScrollPos += zDelta;
			SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
			ScrollWindow(0,-zDelta);
		}
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CScrollPanel::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_bDragging=TRUE;
	SetCapture();

	m_ptDragPoint=point;

 

	CDialog::OnLButtonDown(nFlags, point);
}

void CScrollPanel::OnLButtonUp( UINT nFlags, CPoint point )
{
	EndDrag();

	CDialog::OnLButtonUp(nFlags, point);
}

void CScrollPanel::OnMouseMove( UINT nFlags, CPoint point )
{
	if (m_bDragging)
	{
		int nDelta=m_ptDragPoint.y-point.y;
		m_ptDragPoint=point;

		int nNewPos=m_nScrollPos+nDelta;

		if (nNewPos<0)
			nNewPos=0;
		else if (nNewPos>m_rcOriginalRect.Height() - m_nCurHeight)
			nNewPos=m_rcOriginalRect.Height() - m_nCurHeight;

		nDelta=nNewPos-m_nScrollPos;
		m_nScrollPos=nNewPos;

		SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
		ScrollWindow(0,-nDelta);
	}

	CDialog::OnMouseMove(nFlags, point);

}

void CScrollPanel::OnKillFocus( CWnd* pNewWnd )
{
	CDialog::OnKillFocus(pNewWnd);

	EndDrag();
}

 
void CScrollPanel::EndDrag()
{

	m_bDragging=FALSE;
	ReleaseCapture();
	 

}

 

// CScrollPanel 메시지 처리기입니다.
