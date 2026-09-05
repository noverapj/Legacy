
// LS_MonitorDlg.h : header file
//

#pragma once

#include "afxcmn.h"
#include "MonitorTabCtrl.h"
#include "IOServerSocket.h"
#include "IOClientSocket.h"
#include "Network/SP2Packet.h"


// CLS_MonitorDlg dialog
class CLS_MonitorDlg : public CDialogEx
{
	DECLARE_EASYSIZE

// Construction
public:
	CLS_MonitorDlg(const TCHAR* parameter, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LS_MONITOR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	CString GetIP()	{ return m_strIP; }
	void UpdateTitle(const int currentUser, const int maxUser);
	void OnAccept();
	void OnClose(int iClientNumber);
	CIOClientSocket* OnFindClient(int iClientNumber);
	
protected:
	BOOL LoadBillingFromINI(const TCHAR* file, CString section, BILLINGSERVERTABLE &tempBillingServerTable);
	void LoadINI(const TCHAR* file);
	void LoadIP();

	void AddTab(TCHAR* name, TCHAR* IP, const int port);

protected:
	CString m_strIP;
	int m_iPort;
	CRect m_clientRect;
	CString m_parameter;
	CIOServerSocket* m_ServerSocket;

	int iClientsocketNumber;

	std::map<int, CIOClientSocket*>		m_ClientSocketList;


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

public:
	CMonitorTabCtrl m_zoneTabs;
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnDestroy();

public:
	void OnReceive(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnTodayNumConnect(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnSearchNumConnect(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnRequipNumConnect(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnRequestMainServerAdminCommand(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnRequestGameServerCommand(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnServerExit(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);

	void OnGameServerReloadINI(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnGameServerOption(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnReloadCloseInfo(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnResetEventShop(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnResetOldMissionData(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnWhiteListRequest(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);
	void OnUpdateClientVersion(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);

	void OnLSResponse(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket);

	void OnLoginServerInfo();
	void OnLoginServerInfo(CIOClientSocket* pkClientSocket);

	void OnLoginServerDisConnect(SERVERINFO* serverInfo, const ServerStates state);
	void OnMainServerDisConnect(SERVERINFO* serverInfo, const ServerStates state);
	void OnGameServerDisconnect(ioHashString serverIP, int serverPort);

	void OnMainServerAdmin(int ClientNumber, CPacket &rkPacket);
};
