
// LSControllerDlg.h : header file
//

#pragma once

#include "ServerEntry.h"
#include "LogTailer.h"
#include <string>
#include <thread>
#include <vector>


// CLSControllerDlg dialog
class CLSControllerDlg : public CDialogEx
{
// Construction
public:
	CLSControllerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LSCONTROLLER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	std::vector<ServerEntry> m_arrServers;

	CListCtrl     m_list;
	CComboBox     m_cboLog;
	CComboBox     m_cboFile;
	CRichEditCtrl m_edLog;
	CLogTailer    m_tailer;

	// Start All sequencer (staggered, timer driven: one server per tick)
	std::vector<int> m_arrPending;
	size_t m_nPendingPos = 0;

	// Asynchronous stop sequencer (worker thread; the UI stays responsive)
	std::thread m_threadStop;
	bool m_bStopping = false;
	std::vector<CString> m_arrStopResults;   // written by the stop thread

	enum
	{
		IDT_STATUS   = 1,   // 1s status polling
		IDT_STARTALL = 2,   // Start All stagger
	};

	static const UINT WM_APP_LOGDATA;   // posted by the log tailer thread
	static const UINT WM_APP_STOPDONE;  // posted by the stop thread

	int  GetSelectedEntryIndex() const;
	bool AnyRunning() const;

	void LoadConfig();
	void SetupList();
	void RefreshStatus();
	void UpdateButtons();
	void StartAllBegin();
	void StartAllNext();
	void StopAll();

	// Runs the stop sequence on a worker thread; UI stays responsive.
	// arrIndices: entries to stop (in order). nRestartIdx: when >= 0, that
	// entry is started again after it stopped.
	void BeginStopThread(std::vector<int> arrIndices, int nRestartIdx = -1);

	void PopulateLogCombo();
	void PopulateFileCombo(int nServerIndex);
	void StartTailer(int nServerIndex, const CString& strFileName);
	void AppendLogText(const std::string& strAnsi);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedRestart();
	afx_msg void OnBnClickedStartAll();
	afx_msg void OnBnClickedStopAll();
	afx_msg void OnCbnSelchangeLog();
	afx_msg void OnCbnSelchangeFile();
	afx_msg LRESULT OnLogData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStopDone(WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};
