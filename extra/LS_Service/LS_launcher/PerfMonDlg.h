#pragma once


#include <vector>
#include "afxcmn.h"
#include "afxwin.h"
#include "MonitorCtrl.h"

typedef std::vector<int> UNITS;

class CServerDialog;

// CPerfMonDlg 대화 상자입니다.

class CPerfMonDlg : public CDialog
{
	DECLARE_DYNAMIC(CPerfMonDlg)

public:
	CPerfMonDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPerfMonDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PERFMONDLG };

public:
	void SetConfig(CServerDialog* dlg, CString name);
	void ShowResult();

	BOOL IsInited();

	void SetCPU(UNITS& units);
	void SetMemory(UNITS& units);
	void SetNetwork(UNITS& units);
	void SetDisk(UNITS& units);

protected:
	UNITS m_CPUs, m_Memorys, m_Networks, m_Disks;
	BOOL m_init;

	CServerDialog* m_serverDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	CMonitorCtrl m_cpuBar;
	CMonitorCtrl m_MemoryBar;
	CMonitorCtrl m_networkBar;
	afx_msg void OnClose();
	CStatic m_cpuText;
	CStatic m_memText;
	CStatic m_netText;
	virtual BOOL OnInitDialog();
};
