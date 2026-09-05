#pragma once


// CScrollPanel 대화 상자입니다.

class CScrollPanel : public CDialogEx
{
	DECLARE_DYNAMIC(CScrollPanel)

public:
	void EndDrag();
	CScrollPanel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CScrollPanel();
	// dialog size as you see in the resource view (original size)
	CRect	m_rcOriginalRect;

	// dragging
	BOOL	m_bDragging;
	CPoint	m_ptDragPoint;

	// actual scroll position
	int		m_nScrollPos;

	// actual dialog height
	int		m_nCurHeight;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
 
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
public:
 
 
};
