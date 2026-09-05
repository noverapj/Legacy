#pragma once
#include "afxwin.h"


// CInstallDialog 대화 상자입니다.

class CInstallDialog : public CDialog
{
	DECLARE_DYNAMIC(CInstallDialog)

public:
	CInstallDialog(const CString folder, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInstallDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSTALL_DIALOG };

public:
	CString GetService()	{ return m_service; }
	CString GetDemon()		{ return m_demon; }
	CString GetScript()		{ return m_script; }

protected:
	CString m_folder, m_demon, m_script;
	CString m_service;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSelectDemon();
	afx_msg void OnBnClickedButtonSelectScript();
	CStatic m_selectedDemon;
	CStatic m_selectedScript;
	afx_msg void OnBnClickedButtonInstallCancel();
	afx_msg void OnBnClickedButtonInstallOK();
	CEdit m_serviceName;
};
