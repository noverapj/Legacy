// LSMonitorDlg.h : 헤더 파일
//

#pragma once

#include "resource.h"
#include "NetWork/SystemInfoUDPSocket.h"
#include "afxcmn.h"
#include "SMSDlg.h"
#include "AnnounceDlg.h"
#include "ClientVersionDlg.h"
#include "ControlDlg.h"
#include "Resize/ETSLayout.h"
#include "ThreadModules/Thread.h"
#include "LoingPanel.h"

class CServerInfoDlg;

class HttpConnector : public Thread
{
protected:
	enum 
	{
		MAX_DEQUE = 100, 
	};
protected:
	CString m_sURL;
	bool    m_bClose;
	
public:
	void SetURL( CString &rsURL );

	virtual void Run();
	void Close();

protected:
	int OpenURL( IN const char *szURL, OUT char *szReturnData, IN int iReturnDataSize );

public:
	HttpConnector();
	virtual ~HttpConnector();
};

typedef std::vector<HttpConnector*> vHttpConnectorVec;
//---------------------------------------------------------------------------------

#define GAME_SERVER_NAME "GameServer"

// CLSMonitorDlg 대화 상자
class CLSMonitorDlg : public ETSLayoutDialog
{
protected:
	enum eDlgType
	{
		DT_NORMAL     = 0,
		DT_SERVER_SMS = 1,
		DT_SYSTEM_SMS = 2,
		DT_ANNOUNCE   = 3,
		DT_ANNOUNCE_VERSION = 4,
		DT_ANNOUNCE_CONTROL = 5,
		DT_ANNOUNCE_CONNECT = 6,

		DT_DEVELOPER		= 10,	// 개발자 전용
	};

protected:
	//eDlgType    m_eDlgType;
	int			m_iMaxConnect;
	vZONEINFO	m_vZoneInfo;	
	vSERVERINFO m_vloginserverinfo;
	SystemInfoUDPSocket m_SystemInfoUDPSocket;
	int         m_iWriteConnectSec;

	// window
	CSMSDlg      *m_pSMSDlg;
	CAnnounceDlg  m_AnnounceDlg;
	CClientVersionDlg m_VersionDlg;
	CControlDlg       m_ControlDlg;
	CServerInfoDlg*	m_pServerInfoDlg;

	// ctrl
	CListCtrl   m_ListCtrlServerInfo;
	CListCtrl   m_ListCtrlSystemInfo;
	CTabCtrl    m_TabCtrlZone;

	char m_szLogPrevTime[MAX_PATH];
	char m_szConnectLogPrevTime[MAX_PATH];
	char m_szMaxLogPrevTime[MAX_PATH];
	vHttpConnectorVec m_vHttpConnectorVec;

	// ListCtrl Index
	int			m_selectIndex;

protected:
	void LoadINI();
	
	void _OnTimer1Minutes();
	void _OnTimer10Minutes();

	void _OnTimerSendMsg();
	void _OnTimerUpdateNoReaciton();
	void _OnTimerCreateNewLog( CLog &rLog, const char *szFileHeadName, char *szPrevTime, int iPrevTimeSize );
	void _OnTimerWriteConnectLog();

	void SetServerDefaultList( int iZoneArray );
	void SetSystemDefaultList( int iZoneArray );
	void SetServerStateText( const SERVERINFO *pServerInfo, int iItem, OUT CString &szState );
	void SetSystemStateText( const SYSTEMINFO *pSystemInfo, int iItem, OUT CString &szState );

	void InitListServerInfo();
	void InitListSystemInfo();

	void SendUDPMsg();

	void HideChildBtns();

	void SendWebServer();

	// receive
	void OnNumConnect( UINT wndMsg, SP2Packet &rkPacket );
	void OnServerDisconnect( UINT wndMsg, SP2Packet &rkPacket );
	void OnLoadCS3File( UINT wndMsg, SP2Packet &rkPacket );
	void OnCS3FileVersion( UINT wndMsg, SP2Packet &rkPacket );
	void OnAllServerInfo( UINT wndMsg, SP2Packet &rkPacket );
	void OnServerInfoAck( UINT wndMsg, SP2Packet &rkPacket );

public:
	SERVERINFO  *GetServerInfo( ZONEINFO *pZoneInfo, SP2Packet &rkPacket );
	SystemInfoUDPSocket& GetUDPSocket() { return m_SystemInfoUDPSocket; }
	SYSTEMINFO *GetSystemInfo( IN const char* szIP , OUT bool &rbUpdateList );
	SYSTEMINFO *GetSystemInfoByURL( IN const char* szURL , OUT bool &rbUpdateList );
	CString     GetCurZoneName();
	int         GetCurZoneArray();
	int         GetSMSArray( int iZoneArray, int iListArray );
	int         GetCurSMSInfoSize( int iZoneArray );

public:
	void UpdateServerListCtrl( const SERVERINFO  *pServerInfo );
	void UpdateSystemListCtrl( const SYSTEMINFO *pSystemInfo );
	void UpdateBtnCtrl( int gradeType );
	void ProcessPacket(UINT wndMsg, SP2Packet &rkPacket );

	void OnLSResPonse( UINT wndMsg, SP2Packet & rkPacket ) ;

	void SetServerBlock(UINT wndMsg, SP2Packet & rkPacket, int type );

	void OnFillServerInfo(UINT wndMsg, SP2Packet & rkPacket );

	void OnGetServerInfo( UINT wndMsg, SP2Packet & rkPacket );



	void Disconnect(UINT wndMsg);
	int  CreateUDPPort( int iPort );
	bool CheckSocketUDPPortUsed( int iPort );
	void UpdateListItem( int iSMSArray , const char *szText);
	void SetSMSComboList( OUT CComboBox &rComboBox );
	void SetSystemSMSErrorToNormal( int iSMSArray );
	void SetActiveTab( bool bActive );

	void SendCopyUDPMsg( OUT CControlDlg::vCheckInfo &rvCheckInfo, CString &rszIP, int iVersion );
	void SendDeleteUDPMsg( OUT CControlDlg::vCheckInfo &rvCheckInfo, int iVersion );
	void OnCopyResult( int iResultType, int iRequestType, int iVersion, const CControlDlg::CheckInfo &rReceiveInfo );
	void OnDeleteResult( int iResultType, int iRequestType, int iVersion, const CControlDlg::CheckInfo &rReceiveInfo );
	void GetLoadCheckFileInfo( OUT CControlDlg::vCheckInfo &rvCheckInfo, IN CString &rszOnlyIP );

	void GetMyIP( OUT ioHashString &rszIP );
	void SendMonitorPacekt();
	void SendReqSvrInfoPacekt();
	afx_msg LONG SendPacket2LoginServer(WPARAM wParam,LPARAM lParam);

// 생성
public:
	CLSMonitorDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_LSMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	std::vector<CLoingPanel*> m_vLoginPanel;
		

private:
		// Get
			void GetSelectedServerInfo( ioHashString& ip, int& port );
			void GetSelectedLoginInfo( ioHashString& ip, int& port );

public:
	afx_msg void OnConnect(int wmid);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg LONG OnTCPSocket1   (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket2   (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket3   (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket4   (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket5   (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket6   (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket7   (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket8   (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket9   (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket10  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket11  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket12  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket13  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket14  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket15  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket16  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket17  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket18  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket19  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket20  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket21  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket22  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket23  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket24  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket25  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket26  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket27  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket28  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket29  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket30  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket31  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket32  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket33  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket34  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket35  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket36  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket37  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket38  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket39  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket40  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket41  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket42  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket43  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket44  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket45  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket46  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket47  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket48  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket49  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket50  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket51  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket52  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket53  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket54  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket55  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket56  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket57  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket58  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket59  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket60  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket61  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket62  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket63  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket64  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket65  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket66  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket67  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket68  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket69  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket70  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket71  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket72  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket73  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket74  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket75  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket76  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket77  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket78  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket79  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket80  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket81  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket82  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket83  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket84  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket85  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket86  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket87  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket88  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket89  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket90  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket91  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket92  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket93  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket94  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket95  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket96  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket97  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket98  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket99  (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTCPSocket100 (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnDisConnected (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnUDPSocket(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMCustomdrawListServerInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawListSysteminfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTabSelChangeZone(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAnnounce();
	afx_msg void OnBnClickedBtnSystemSms();
	afx_msg void OnBnClickedBtnServerSms();
	afx_msg void OnBnClickedButtonVersion();
	afx_msg void OnBnClickedDefaultCancel();
	afx_msg void OnBnClickedButtonControl();
	afx_msg void OnBnClickedBtnConnectLog();
	afx_msg void OnNMRClickServerInfo( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnNMClickServerInfo( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnNMDBClickServerInfo( NMHDR *pNHDR, LRESULT *pResult );
	afx_msg void OnMainServerInfoDrawReq();
		//LoginsErver
	afx_msg void OnLoginServerInfodraw();
		// ServerInfo
	afx_msg void OnGameserverInfodraw();

		// MainServer
	afx_msg void OnMainserverMainserverexit();
	afx_msg void OnMainserverQuickallexit();
	afx_msg void OnMainserverSafetyallexit();
	afx_msg void OnMainserverReloadcloseinfo();
	afx_msg void OnMainserverDbagentextend();
	afx_msg void OnMainserverGameserveroption();
	afx_msg void OnMainserverGsreloadini();
	afx_msg void OnMainserverExtraitemini();
	afx_msg void OnMainserverEventshopini();

		// GameServer
	afx_msg void OnGameserverReloadhackconstant();
	afx_msg void OnGameserverReloaduserdispersion();
	afx_msg void OnGameserverReloadprocessini();
	afx_msg void OnGameserverReloadiniclass();
	afx_msg void OnGameserverReloadinideco();
	afx_msg void OnGameserverReloadinietc();
	afx_msg void OnGameserverReloadinievent();
	afx_msg void OnGameserverReloadquestini();
	afx_msg void OnGameserverReloadmodeini();
	afx_msg void OnGameserverReloadpresentini();
	afx_msg void OnGameserverReloadfishingini();
	afx_msg void OnGameserverReloadexca();
	afx_msg void OnGameserverReloaditemcompoundini();
	afx_msg void OnGameserverReloadextraitemini();
	afx_msg void OnGameserverReloadtradeini();
	afx_msg void OnGameserverReloadlevelmatchini();
	afx_msg void OnGameserverReloaditeminitcontrol();
	afx_msg void OnGameserverReloadconfigini();
	afx_msg void OnGameserverQuickexit();
	afx_msg void OnGameserverSafetyexit();
	afx_msg void OnMainserverGssetnaglerefcount();
	afx_msg void OnMainserverGssetnagletime();
	afx_msg void OnLoginserverLoginserverinFill();
};
