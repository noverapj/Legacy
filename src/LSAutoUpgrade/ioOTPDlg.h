#pragma once
#include "afxwin.h"


// ioOTPDlg 대화 상자입니다.

#include "NetWork/SP2Packet.h"

#define TIME_ID_OTP      201
#define MAX_OTP_TIME_SEC 120
#define MAX_OTP_CONNECT_TIME_SEC 30

class ioClientSocket;
class ioOTPDlg : public CDialog
{
	DECLARE_DYNAMIC(ioOTPDlg)

public:
	ioOTPDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ioOTPDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OTPDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	typedef std::vector<int> IntVec;

protected:
	CButton m_SendBtn;
	CButton m_CancelBtn;
	CButton m_1Btn;
	CButton m_2Btn;
	CButton m_3Btn;
	CButton m_4Btn;
	CButton m_5Btn;
	CButton m_6Btn;
	CButton m_7Btn;
	CButton m_8Btn;
	CButton m_9Btn;
	CButton m_10Btn;
	CEdit m_PWEdit;
	CButton m_DeleteBtn;

	ioClientSocket   *m_pClientSocket;
	bool              m_bOTPOk;	
	int               m_iOTPTimerCnt;
	NetworkState      m_eNetworkState;

public:
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClicked1();
	afx_msg void OnBnClicked2();
	afx_msg void OnBnClicked3();
	afx_msg void OnBnClicked4();
	afx_msg void OnBnClicked5();
	afx_msg void OnBnClicked6();
	afx_msg void OnBnClicked7();
	afx_msg void OnBnClicked8();
	afx_msg void OnBnClicked9();
	afx_msg void OnBnClicked10();
	afx_msg void OnBnClickedDelete();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnClientSocket(WPARAM wParam, LPARAM lParam);

public:
	void ConnectSocket();
	void RecvPacket( SP2Packet &rkPacket );
	bool IsOTPOk() const { return m_bOTPOk; }
	void SetNetworkState( NetworkState eNetworkState ) { m_eNetworkState = eNetworkState; }

protected:
	void RandBtns();
	void AddPWEdit( CString &rsTextNum );
	bool SendServer( const char *szPW );

	void SetChildsActive( bool bActive );
};
