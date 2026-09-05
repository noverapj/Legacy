
// LS_MonitorClientDlg.h : 헤더 파일
//

#pragma once

#include "afxcmn.h"
#include "MonitorTabCtrl.h"

// CLS_MonitorClientDlg 대화 상자
class CLS_MonitorClientDlg : public CDialogEx
{
	DECLARE_EASYSIZE

// 생성입니다.
public:
	CLS_MonitorClientDlg(const TCHAR* parameter, CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LS_MONITORCLIENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	CString GetIP()	{ return m_IP; }
	void UpdateTitle(const CString strIP, const int currentUser, const int maxUser);
	
protected:
	BOOL LoadBillingFromINI(const TCHAR* file, CString section, BILLINGSERVERTABLE &tempBillingServerTable);
	void LoadINI(const TCHAR* file);
	void LoadIP();

	void AddTab(TCHAR* name, TCHAR* IP, const int port);

protected:
	CString m_IP;
	CRect m_clientRect;
	CString m_parameter;
	
// 구현입니다.
protected:
	HICON m_hIcon;
	
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

public:
	CMonitorTabCtrl m_zoneTabs;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
};
