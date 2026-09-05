#pragma once


// CServerInfoDlg 대화 상자입니다.

class LSMonitorDlg;

class CServerInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CServerInfoDlg)

public:
	CServerInfoDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CServerInfoDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERVER_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	
private:
	int	m_type;	// 0:default, 1:main, 2:game

public:
	CString m_szEdit;

	void	PrintMainserverTextOut( SP2Packet& rPacket );
	void	PrintGameServerTextOut( SP2Packet& rPacket );
	void    PrintLoginServerTextOut(SP2Packet& rPacket ); 

protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedButtonRefresh();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
