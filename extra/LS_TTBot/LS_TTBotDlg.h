// LS_TTBotDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "ioTestCase.h"
#include "ioTestConnection.h"
#include "ioTestLogin.h"
#include "ioTestEcho.h"
#include "ioTestSend.h"
#include "ioTestQuerySend1.h"
#include "ioTestQuerySend2.h"
#include <vector>
#include <ppl.h>
#include <WS2tcpip.h>

// CLS_TTBotDlg 대화 상자
class CLS_TTBotDlg : public CDialog
{
// 생성
public:
	CLS_TTBotDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = 102 };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

public:
	void LOG(const TCHAR *format, ...);
	LRESULT LOG(WPARAM wParam, LPARAM lParam);

// 구현
protected:
	void SetDefault();

	void TimerBegin();
	void TimerEnd();

	void DestroyTestCases();

protected:
	typedef std::vector<ioTestCase*> TESTCASES;

	uint32 m_sequence, m_max;
	TESTCASES m_cases;

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	CEdit m_interval;
	CEdit m_count;
	CEdit m_IP;
	CEdit m_port;
public:
	CComboBox m_testCases;
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnTimer(UINT nIDEvent);
	CListBox m_logs;
	CEdit m_concurrent;
	Concurrency::critical_section m_lock;
};

extern CLS_TTBotDlg* g_Log;