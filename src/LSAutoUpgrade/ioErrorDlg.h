#pragma once
#include "afxwin.h"
#include "resource.h"

// ioErrorDlg 대화 상자입니다.
#include "skin/CDialogSK.h"
#include "skin/HoverButton.h"
#include "afxcmn.h"

class ioErrorDlg : public CDialogSK
{
public:
	ioErrorDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ioErrorDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_IOERRORDLG };

protected:
	bool m_bExitProgram;
	int  m_iErrorNumber;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Edit;
	CBrush       m_EditBGBrush;
	COLORREF     m_EditBGColor;
	COLORREF     m_EditTextColor;

	CHoverButton m_XBtn;
	CHoverButton m_SolutionBtn;
	CHoverButton m_ExitBtn;
	CFont        m_EditFont;
	CLinkCtrl	m_Link;

public:
	void ShowErrorDlg(CString szErrorText, int iErrorNumber, bool bExitProgram);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	afx_msg void OnBnClickedSolution();
	afx_msg void OnBnClickedXBtn();
	afx_msg void OnBnClickedExitBtn();
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();

	afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
};
