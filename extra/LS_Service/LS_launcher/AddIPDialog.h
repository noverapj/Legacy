#pragma once


// CAddIPDialog 대화 상자입니다.

class CAddIPDialog : public CDialog
{
	DECLARE_DYNAMIC(CAddIPDialog)

public:
	CAddIPDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAddIPDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ADD_IP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddIpOkButton();
	afx_msg void OnBnClickedAddIpCancelButton();

	CString GetTapName() { return m_tapName; }
	UINT GetPort()		 { return m_port; }
	CString GetIP()		 { return m_ip; }

protected:
	CString m_tapName;
	UINT m_port;
	CString m_ip;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
