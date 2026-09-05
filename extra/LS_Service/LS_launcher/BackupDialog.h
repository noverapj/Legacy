#pragma once
#include "afxwin.h"


// CBackupDialog 대화 상자입니다.

class CBackupDialog : public CDialog
{
	DECLARE_DYNAMIC(CBackupDialog)

public:
	CBackupDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CBackupDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_BACKUP_DIALOG };

public:
	BOOL GetBackupPath(CString& source, CString& target);

protected:
	CString m_source;
	CString m_target;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSource();
	afx_msg void OnBnClickedButtonTarget();
	CStatic m_sourcePath;
	CStatic m_targetPath;
	afx_msg void OnBnClickedButtonBackupOK();
	afx_msg void OnBnClickedButtonBackupCancel();
};
