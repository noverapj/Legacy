#pragma once

#include "sms/SMSRuleManager.h"

// CSMSDlg 대화 상자입니다.

class CSMSDlg : public CDialog
{
	DECLARE_DYNAMIC(CSMSDlg)

protected:
	CSMSRuleManager m_SMSRuleManager;
	CEdit           m_EditCustomStopMin;
	CComboBox       m_ComboBoxSelectServer;
	int             m_iZoneArray;

public:
	SMSRULESTATE GetState(int iSMSArray);
	DWORD GetRemainWaitTime(int iSMSArray);
	void SetFirstError(int iSMSArray);
	void SetErrorToNormal(int iSMSArray);
	void SetErrorInfo(CString &szErrorInfo);

public:
	void InitServerInfo( vZONEINFO &rvZoneInfo );
	void InitSystemInfo( vZONEINFO &rvZoneInfo );

public:
	void _OnTimerSMS();
public:
	CSMSDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSMSDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	afx_msg LONG OnSMSNormal(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnSMSStop(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedOnehourstop();
	afx_msg void OnBnClickedCustomstop();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnBnClickedDefaultCancel();
};
