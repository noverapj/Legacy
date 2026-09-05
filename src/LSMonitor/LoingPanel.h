#pragma once


// CLoingPanel 대화 상자입니다.
class CLSMonitorDlg;
class LS_ServerInfoDlg;
class CScrollPanel;
class CLoingPanel : public CDialogEx
{
	DECLARE_DYNAMIC(CLoingPanel)

public:
	CLoingPanel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoingPanel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LoginPanel };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void Init();
	void EndDrag();
	UINT wmid;
 
 
public:
	afx_msg void OnBnClickedOk();
	virtual void PostNcDestroy();

private:
	

	int m_nCurHeight;
	int m_nScrollPos;
	CRect m_rect;
	CMenu m_parrentmenu;
	CMenu* m_menu;

	
public:
	void CreatePopup();
	LRESULT        SendPacket(WPARAM wParam,LPARAM lParam);

	void EnableServerState();

	void DisableServerState();

	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void EnableAllServer();

	void SendBlcokPacket( int a );

	afx_msg void DisableAllServerBlock();
	std::vector<LS_ServerInfoDlg*> m_vinfodlg;
	VSVRINFO m_vsvrInfo;
	bool     m_bInit;
	std::string ipaddr;
	int port;
	int m_blockstate;
private:
	

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
