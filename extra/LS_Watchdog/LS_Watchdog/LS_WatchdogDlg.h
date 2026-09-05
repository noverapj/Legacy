
// LS_WatchdogDlg.h : 헤더 파일
//

#pragma once


#include <string>
#include <map>
#include "afxwin.h"


// CLS_WatchdogDlg 대화 상자
class CLS_WatchdogDlg : public CDialogEx
{
// 생성입니다.
public:
	CLS_WatchdogDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LS_WATCHDOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	void CollectProcess();
	BOOL FindProcess(const int PID);
	void WatchProcess();

protected:
	typedef std::map<int,std::string> MODULES;

	MODULES m_modules;

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_moduleName;
	afx_msg void OnBnClickedButtonWatch();
};
