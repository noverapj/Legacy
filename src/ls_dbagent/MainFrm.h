// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__B3D9E302_4CAD_43B5_A5E3_C1A6924FCD79__INCLUDED_)
#define AFX_MAINFRM_H__B3D9E302_4CAD_43B5_A5E3_C1A6924FCD79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	APPNAME                 "_LOSTSAGA_DBAGENT_"
#define WM_PROGRAM_EXIT		    (WM_USER + 9999)

class ioDBServer;
class LogicThread;

class CMainFrame : public CFrameWnd
{
private:
	ioDBServer *m_pDBServer;
	LogicThread *m_logicThread;

	char m_dbip[MAX_PATH];
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
private:
	ioHashString m_szPrivateIP;
	ioHashString m_szPublicIP;

	bool GetLocalIpAddressList( OUT ioHashStringVec &rvIPList );
	bool SetLocalIP( int iPrivateIPFirstByte );

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
//	CStatusBar  m_wndStatusBar;
//	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnProgramExit(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	bool m_bOnProgramExit;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__B3D9E302_4CAD_43B5_A5E3_C1A6924FCD79__INCLUDED_)
