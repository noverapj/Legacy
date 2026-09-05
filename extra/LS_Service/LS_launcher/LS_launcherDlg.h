// LS_launcherDlg.h : header file
//

#pragma once

#include <list>
#include <vector>
#include "afxcmn.h"
#include "LauncherTabCtrl.h"
#include "Server.h"
#include "afxwin.h"
#include "Utility.h"
#include "Batch/BatchManager.h"

class CServerDialog;
// CLS_launcherDlg dialog
class CLS_launcherDlg : public CDialog
{
// Construction
public:
	CLS_launcherDlg(const TCHAR* parameter, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SERVICELAUNCHER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public:
	UINT GetTabCurFocus()				{ return m_tabCurFocus; }
	void SetTabCurFocus( UINT focus)	{ m_tabCurFocus = focus; }
	void SetLoaded(BOOL b)				{ m_loaded = b;}
	void DeleteServerInfo(CString name);
	CString GetServerInfo();
	CBatchManager* GetBatchManager() { return &m_batchManager;}

	int GetTabDialogCount();
	CServerDialog* GetTabDialog(int index);

protected:
	BOOL Load(const TCHAR* filePath);
	BOOL LoadOption();

	BOOL ReadFile(const TCHAR* filePath);
	BOOL SaveFile();

	BOOL ParseServer();
	BOOL ParseToken(TOKENS &tokens);

	BOOL InitTab();
	void AddServerInfo(CString title, CString IP, UINT port);

	void ShowMainDlg(int nCmdShow);
	void PostMessageToAll(WPARAM wParam, LPARAM lParam);

protected:
	void BeginBatches();
	void EndBatches();

	void BatchStartCheck();
	void BatchContinueCheck();

public:
	void BatchWait();
	void BatchProgress(int progress);

	void ConsoleStart();
	void ConsoleStop();

	void Quit();
	
protected:
	BOOL m_loaded;
	CString m_buffer;
	UINT m_tabCurFocus;

	CString m_parameter;
	CString m_fileName;
	CString m_batchName;

	typedef std::list<CServerInfo*> SERVERINFO_LIST;
	SERVERINFO_LIST m_serverInfos;

	CBatchManager	m_batchManager;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CLauncherTabCtrl m_serverTabs;
	CButton m_chk_watchdog;
	CButton m_chk_applyall;
	CButton m_chk_monitor;
	CButton m_chk_recusive;
	CButton m_chk_console;

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnTcnSelchangeTabServers(NMHDR *pNMHDR, LRESULT *pResult);
 	afx_msg void OnBnClickedCheckWatchdog();
	afx_msg void OnBnClickedCheckApplyall();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
 	afx_msg void OnBnClickedCheckRecursive();
	afx_msg void OnBnClickedCheckConsole();

	afx_msg LRESULT OnAddIP(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteIP(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSpread(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedCheckMonitor();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
