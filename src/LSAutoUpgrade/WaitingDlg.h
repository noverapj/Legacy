#pragma once


#include "ioBalancer.h"

// CWaitingDlg 대화 상자입니다.

class CWaitingDlg : public CDialog, public ioBalancer
{
	DECLARE_DYNAMIC(CWaitingDlg)

public:
	CWaitingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWaitingDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_WAITING };

protected:
	HWND GetHandle();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	afx_msg LONG OnReceive(WPARAM wParam, LPARAM lParam);
};
