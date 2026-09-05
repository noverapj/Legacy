#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "OXToolTipCtrl.h"

// LS_ServerInfoDlg 대화 상자입니다.

class LS_ServerInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LS_ServerInfoDlg)

public:
	LS_ServerInfoDlg(CWnd* pParent,ServerInfo_& stdata);   // 표준 생성자입니다.
	virtual ~LS_ServerInfoDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MONITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult );
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btn_apply;
	CButton m_check_serverstate;
	CEdit m_edit_maxuser;
	afx_msg void OnBnClickedButtonApply();
	LRESULT        SendPacket(WPARAM wParam,LPARAM lParam);
	std::string m_ipaddr;
	int port;
	ServerInfo_ m_svrInfo;
	CMenu m_parrentmenu;
	CMenu *m_menu;
private:
	
	
public:
	virtual BOOL OnInitDialog();
	CString m_static_title;
	CProgressCtrl m_progress;
	CStatic m_static_user;
	CString m_sendcount;
	CString m_usercount;
	CString m_strmaxuser;
	afx_msg void OnStnClickedStaticSvrinfo();
	COXToolTipCtrl m_tooltip;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnNeedText(NMHDR  *pnmh,  LRESULT*  pResult); 
	int m_maxcount;
	CString m_strmaxusercount;
	afx_msg void OnDisableCurrentserver();

	void DisableServerState();

	afx_msg void OnEnableCurrnetserver();

	void SendServerStatePacket( int a );

	void EnableServerState();

};
