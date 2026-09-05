#if !defined(AFX_MSGBOX_H__3CCD5D0F_7FE0_44E6_A952_34D6446D9295__INCLUDED_)
#define AFX_MSGBOX_H__3CCD5D0F_7FE0_44E6_A952_34D6446D9295__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgBox window

class CMsgBox : public CWnd
{
	DECLARE_DYNAMIC(CMsgBox)

// Construction
public:
	CMsgBox(CWnd* pParent);
	CMsgBox();
	virtual ~CMsgBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgBox)
	//}}AFX_VIRTUAL

private:
	CString m_Caption;
	BOOL m_bChildCreated;

// Implementation
public:
	void SetParent(CWnd* pParent);
	void MessageBox(CString sMsg, CString sCaption, UINT nSleep, 
				UINT nFlags, bool bAutoClose = true /*Default*/);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgBox)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGBOX_H__3CCD5D0F_7FE0_44E6_A952_34D6446D9295__INCLUDED_)
