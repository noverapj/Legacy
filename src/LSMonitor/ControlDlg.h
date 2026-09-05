#pragma once
#include "afxwin.h"


// CControlDlg 대화 상자입니다.

class CControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CControlDlg)
public:
	CControlDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CControlDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONTROL };

	enum 
	{
		CHECK_TIMER = 2,
	};

	enum Actiontype
	{
		AT_NONE        = 0,
		AT_SEND_COPY   = 1,
		AT_SEND_DELETE = 2,
		AT_SEND_LOAD   = 3,
		AT_SEND_INFO   = 4,
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	typedef struct tagCheckInfo
	{
		CString m_szSystemIP;
		CString m_szServerIP;
		int     m_iPort;

		tagCheckInfo()
		{
			m_iPort = 0;
		}
	}CheckInfo;

	typedef std::vector<CheckInfo> vCheckInfo;

	CEdit    m_EditVersion;
	CEdit    m_EditIP;
	CEdit    m_EditChange;
	CButton  m_ButtonSendCopy;
	CButton  m_ButtonSendLoad;
	CButton  m_ButtonSendInfo;
	CListBox m_ListBoxInfo;
	CButton  m_CheckBtnServerIP;
	CButton  m_CheckBtnChange;

	vCheckInfo m_vCheckInfo;
	DWORD    m_dwSendTime;
	Actiontype m_eActionType;

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSendCopy();
	afx_msg void OnBnClickedButtonSendLoad();
	afx_msg void OnBnClickedButtonSendInfo();
	afx_msg void OnBnClickedDefaultCancel();
	afx_msg void OnBnClickedDefaultOK();
	afx_msg void OnBnClickedCheckServerip();
	afx_msg void OnBnClickedCheckChange();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);

	void OnCopyResult( int iResultType, int iRequestType, int iVersion, const CheckInfo &rReceiveInfo );
	void OnDeleteResult( int iResultType, int iRequestType, int iVersion, const CheckInfo &rReceiveInfo );
	void OnLoadResult( int iResultType, int iVersion, const CheckInfo &rReceiveInfo );
	void OnInfoResult( int iResultType, const CheckInfo &rReceiveInfo, SP2Packet &rkPacket );

protected:
	void SendDeleteMsg();

	void InActiveAllBtn();
	void ActiveAllBtn();	
};
