#pragma once
#include "resource.h"
#include "afxwin.h"

// CGetTextDlg 대화 상자입니다.

class CGetTextDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGetTextDlg)

public:
	CGetTextDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGetTextDlg();
	void GetText(CString* str) { m_pStr = str; };
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
// 대화 상자 데이터입니다.
	enum { IDD = IDD_GETTEXTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	DECLARE_MESSAGE_MAP()
public:
	CString m_strString;
	afx_msg void OnBnClickedOk();
private:
	CString* m_pStr;
	bool	m_bStartSet;
public:
	CEdit m_strCtr;
};
