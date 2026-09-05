#pragma once
#include "afxwin.h"


// CClientVersionDlg 대화 상자입니다.

class CClientVersionDlg : public CDialog
{
	DECLARE_DYNAMIC(CClientVersionDlg)

public:
	CClientVersionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CClientVersionDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CLIENT_VERSION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CheckButtonUse;
	CEdit m_EditVersion;
	CEdit m_EditPassWord;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedSend();
	afx_msg void OnClose();
	afx_msg void OnBnClickedDefaultCancel();
};
