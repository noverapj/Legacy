
// LS_MonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LS_Monitor.h"
#include "LS_MonitorDlg.h"
#include "WinMessage.h"
#include "afxdialogex.h"
#include "Define.h"
#include "ServerTypes.h"
#include "IOSocket.h"
#include "IOServerSocket.h"
#include "Network/SP2Packet.h"
#include "Network/SP2Packet.h"
#include "Network/Protocol.h"
#include "ZoneDialog.h"
#include "LoginDialog.h"
#include "PanelDialog.h"
#include <iostream>
#include <fstream>
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CLS_MonitorDlg::CLS_MonitorDlg(const TCHAR* parameter, CWnd* pParent /*=NULL*/)
	: CDialogEx(CLS_MonitorDlg::IDD, pParent)
{
	m_parameter = parameter;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLS_MonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_ZONES, m_zoneTabs);
}

BEGIN_MESSAGE_MAP(CLS_MonitorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_DESTROY()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CLS_MonitorDlg)
	EASYSIZE(IDC_TAB_ZONES,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,ES_HCENTER)
	//EASYSIZE(IDC_RADIO1,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	//EASYSIZE(IDC_RADIO2,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	//EASYSIZE(IDC_STATUSFRAME,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDC_STATUS,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDC_MYICON2,ES_BORDER,ES_BORDER,IDC_TITLE,ES_KEEPSIZE,ES_HCENTER)
	//EASYSIZE(IDC_CONTENT,IDC_TITLE,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDC_MIDFRAME,ES_BORDER,ES_BORDER,IDC_CONTENT,ES_BORDER,0)
	//EASYSIZE(IDC_MIDRECT,ES_BORDER,ES_BORDER,IDC_CONTENT,ES_BORDER,0)
	//EASYSIZE(IDC_MYICON1,IDC_MIDRECT,IDC_MIDRECT,IDC_MIDRECT,IDC_MIDRECT,ES_HCENTER|ES_VCENTER)
END_EASYSIZE_MAP

// CLS_MonitorDlg message handlers

BOOL CLS_MonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString filePath = m_parameter;
	if(filePath.IsEmpty())
	{
		filePath = _T("config.ini");
	}

	LoadINI(filePath);
	LoadIP();
	 
	m_zoneTabs.ShowTab(0);  
	GetClientRect(m_clientRect);

	INIT_EASYSIZE;

	m_ServerSocket = new CIOServerSocket;
	m_ServerSocket->Disconnect();		

	m_ServerSocket->Start(m_strIP, m_iPort);

	iClientsocketNumber = 11;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLS_MonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLS_MonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CLS_MonitorDlg::LoadBillingFromINI(const TCHAR* file, CString section, BILLINGSERVERTABLE &tempBillingServerTable)
{
	BOOL flag = FALSE;
	
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);
	CString fullPath;
	CString defaultStr= "default";

	fullPath.Format(_T("%s\\%s"), path, file);
	CString name, billing;
	
	if(section == "")
		return FALSE;

	int i = 1;
	while(TRUE)
	{
		CString strNum;
		strNum.Format(_T("billing%d"), i);
		// name
		DWORD result= GetPrivateProfileString(
			section,		//zone
			strNum,			//billing
			defaultStr, 
			buffer, 
			_countof(buffer), 
			fullPath);

		buffer[result] = 0;
		if(defaultStr != buffer)
		{
			billing = buffer;

			i++;
			int curPos = 0;
			int count = tempBillingServerTable.size();		//count 값이 빌링 구분값

			BILLINGSERVERINFO* billingServer	= new BILLINGSERVERINFO;
			billingServer->serviceIP			= billing.Tokenize(_T(":"), curPos);	
			CString billingPort					= billing.Tokenize(_T(":"), curPos);
			billingServer->servicePort			= atoi(billingPort);		
			billingServer->billingServerState	= SERVERSTATE_OFF;
			billingServer->serverType			= SERVER_TYPE_BILLING;
			billingServer->serverKeyIndex		= count;
			
			tempBillingServerTable[count] = billingServer;
		}
		else
		{
			break;
		}
	}
	if( tempBillingServerTable.size() > 0 )
	{
		flag = TRUE;
	}
	return flag;
}

//. ini 에서 빌링서버옵션 읽을 수 있도록 추가 
void CLS_MonitorDlg::LoadINI(const TCHAR* file)
{
	BOOL billingFlag = FALSE;
	
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%s"), path, file);

	CString defaultStr= "default", section;
	CString strIP, name, main, login, billing;

	int nCode = false;

	section.Format(_T("monitor"));
// name
	DWORD result= GetPrivateProfileString(
		section,
		_T("server"), 
		defaultStr, 
		buffer, 
		_countof(buffer), 
		fullPath);
	buffer[result] = 0;
	if(defaultStr != buffer)
	{
		strIP = buffer;
	}


	int curPos = 0;
	m_strIP = strIP.Tokenize(_T(":"), curPos);
	CString mainPort = strIP.Tokenize(_T(":"), curPos);
	m_iPort = atoi(mainPort);

	int iReult = 0;
	for(int i = 1 ; ; i++)
	{
		section.Format(_T("zone%d"), i);
		billingFlag = FALSE;	//초기화

		// name
		DWORD result= GetPrivateProfileString(
			section,
			_T("name"), 
			defaultStr, 
			buffer, 
			_countof(buffer), 
			fullPath);
		buffer[result] = 0;
		if(defaultStr == buffer)
			break;

		name = buffer;
		
		// main
		result= GetPrivateProfileString(
			section,
			_T("main"), 
			defaultStr, 
			buffer, 
			_countof(buffer), 
			fullPath);
		buffer[result] = 0;

		main = buffer;		

		// login
		result= GetPrivateProfileString(
			section,
			_T("login"), 
			defaultStr, 
			buffer, 
			_countof(buffer), 
			fullPath);
		buffer[result] = 0;

		login = buffer;


		nCode = GetPrivateProfileInt(section, _T("countrycode"), 0, fullPath); // EU 국가별 동접 표시

		//빌링 추가
		BILLINGSERVERTABLE billingServerTable;
		billingFlag = LoadBillingFromINI(file, section, billingServerTable);
		
		if(billingFlag == TRUE)
		{
			int index = m_zoneTabs.GetItemCount();
			//m_zoneTabs.AddTab(name, TRUE, billingServerTable); 
			m_zoneTabs.AddTab(name, main, login, TRUE, billingServerTable, false); 
			m_zoneTabs.ShowTab(index, FALSE);
		}

		// 탭추가 
		if(billingFlag == FALSE)
		{
			int index = m_zoneTabs.GetItemCount();
			m_zoneTabs.AddTab(name, main, login, FALSE, billingServerTable, (bool)nCode);  
			m_zoneTabs.ShowTab(index, FALSE);
		}
		
	}
}

#define DESIRED_WINSOCK_VERSION        0x0101
#define MINIMUM_WINSOCK_VERSION        0x0001



void CLS_MonitorDlg::LoadIP()
{
	/*
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[255];
	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD(2, 0);

	if(WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		if(gethostname(name, sizeof(name)) == 0)
		{
			if((hostinfo = gethostbyname(name)) != NULL)
				m_strIP2 = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
		} 
		WSACleanup();
	}


	TCHAR hostName[512];
	gethostname(hostName, sizeof(hostName));
	hostent* host = gethostbyname(hostName);

	struct in_addr addr;
	int i = 0;
    while (host->h_addr_list[i] != 0) 
	{
        addr.s_addr = *(u_long *) host->h_addr_list[i++];
		m_strIP = inet_ntoa(addr);
		if(!m_strIP.IsEmpty())
		{
			CString title;
			title.Format(_T("LostSaga Monitor - %s"), m_strIP);
			SetWindowText(title);
			break;
		}
    }

	WSADATA wsadata;
	CString strIP; // 이 변수에 IP주소가 저장된다.
	strIP = "";

	if( !WSAStartup( DESIRED_WINSOCK_VERSION, &wsadata ) )
	{
		if( wsadata.wVersion >= MINIMUM_WINSOCK_VERSION )
		{
			HOSTENT *p_host_info;
			IN_ADDR in;
			char host_name[128]={0, };

			gethostname(host_name, 128);
			p_host_info = gethostbyname( host_name );

			if( p_host_info != NULL )
			{
				for( int i = 0; p_host_info->h_addr_list[i]; i++ )
				{
					memcpy( &in, p_host_info->h_addr_list[i], 4 );
					m_strIP3 = inet_ntoa( in );
				}
			}
		}
		WSACleanup();
	}
	*/
}


void CLS_MonitorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;
}


void CLS_MonitorDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int width = m_clientRect.Width()+16;
	pRect->right = pRect->left + width;

	EASYSIZE_MINSIZE(m_clientRect.Width()+16, m_clientRect.Height()+38, fwSide, pRect);
}


void CLS_MonitorDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//lpMMI->ptMaxSize.
	int width = m_clientRect.Width()+16;
	int spacing = lpMMI->ptMaxTrackSize.x - lpMMI->ptMaxSize.x;

	lpMMI->ptMaxSize.x = width;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


BOOL CLS_MonitorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(RestrictMessage(pMsg))
		return FALSE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CLS_MonitorDlg::UpdateTitle(const int currentUser, const int maxUser)
{
	CString title;
	title.Format(_T("LostSaga Monitor - %s(%d/%d)"), m_strIP, currentUser, maxUser);
	SetWindowText(title);
}

void CLS_MonitorDlg::OnAccept()
{
	CIOClientSocket *pkClientSocket = new CIOClientSocket;
	m_ServerSocket->Accept(*pkClientSocket);
	pkClientSocket->SetClientNumber(iClientsocketNumber);
	pkClientSocket->SetConnected(true);
	m_ClientSocketList.insert( std::pair< int, CIOClientSocket* > ( pkClientSocket->GetClientNumber(), pkClientSocket ) );
	iClientsocketNumber++;
}


void CLS_MonitorDlg::OnClose(int iClientNumber)
{
	std::map< int, CIOClientSocket* >::iterator it;
	it = m_ClientSocketList.find( iClientNumber );
	if( it == m_ClientSocketList.end() )
	{
		return;
	}

	m_ClientSocketList.erase(it);
}


CIOClientSocket* CLS_MonitorDlg::OnFindClient(int iClientNumber)
{
	std::map< int, CIOClientSocket* >::iterator it;
	it = m_ClientSocketList.find( iClientNumber );
	if( it == m_ClientSocketList.end() )
	{
		return NULL;
	}

	return it->second;
}


void CLS_MonitorDlg::OnDestroy()
{
 	VERIFY(m_zoneTabs.DeleteAllItems());

	m_ClientSocketList.clear();
}

void CLS_MonitorDlg::OnReceive(CIOClientSocket* pkClientSocket, SP2Packet &rkPacket)
{

	switch(rkPacket.GetPacketID())
	{
	case TOOL_SVR::MCTMS_L_CONTROL_SERVER:
		{
			OnLSResponse(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_REQUEST_TODAY_CCU:
		{
			OnTodayNumConnect(pkClientSocket, rkPacket); //기존
		}
		break;

	case TOOL_SVR::MCTMS_REQUEST_SEARCH_CCU:
		{
			OnSearchNumConnect(pkClientSocket, rkPacket); //기존
		}
		break;

	case TOOL_SVR::MCTMS_REQUEST_NUM_CONNECT:
		{
			OnRequipNumConnect(pkClientSocket, rkPacket); //기존
		}
		break;

	case TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND:
		{
			OnRequestMainServerAdminCommand(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_C_GAMESERVER_PROTOCAL:
		{
			OnRequestGameServerCommand(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_C_MAINSERVER_EXIT:
		{
			OnServerExit(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_C_MAINSERVER_QUICKALLEXIT:
		{
			OnServerExit(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_C_MAINSERVER_SAFETYALLEXIT:
		{
			OnServerExit(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_C_MAINSERVER_GAMESERVER_RELOADINI:
		{
			OnGameServerReloadINI(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_C_MAINSERVER_GAMESERVER_OPTION:
		{
			OnGameServerOption(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_C_MAINSERVER_RELOADCLOSEINFO:
		{
			OnReloadCloseInfo(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_C_MAINSERVER_RESETEVENTSHOP:
		{
			OnResetEventShop(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_C_MAINSERVER_RESET_OLDMISSIONDATA:
		{
			OnResetOldMissionData(pkClientSocket, rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_C_WHITELIST_REQUEST:
		{
			OnWhiteListRequest(pkClientSocket, rkPacket);
		}
		break;	

	case TOOL_SVR::MCTSC_C_UPDATE_CLIENT_VERSION:
		{
			OnUpdateClientVersion(pkClientSocket, rkPacket);
		}
		break;	


		/*
	
	case STPKM_ANSWER_NUM_CONNECT:
		{
			OnAnswerNumConnect(rkPacket); //기존
		}
		break;

	case STPKM_ANSWER_BILLING_CONNECT:
		{
			OnAnswerBillingConnect( rkPacket );
			
		}
		break;

	case STPKM_BILLING_USER:
		{
			OnBillingUserInfo(rkPacket);
		}
		break;

	case STPKM_BILLINGSERVER_RELOAD_INI :
		{
			OnBillingReloadINIStatus(rkPacket); 
		}
		break;


	case STPKM_SERVER_DISCONNECT:
		{
			TRACE(_T("[%s]STPKM_SERVER_DISCONNECT\r\n"), GetTitle());
			OnServerDisconnect(rkPacket);
		}
		break;

		
	case STPKM_ALL_SERVER_INFO :
		{
			OnAllServerInfo(rkPacket);
		}
		break;

	case STPKM_LOAD_CS3_FILE :
		{
			OnLoadCS3File(rkPacket);
		}
		break;

	case STPKM_CS3_FILE_VERSION :
		{
			OnCS3FileVersion(rkPacket);

		}
		break;

	case STPKM_SERVER_INFO_ACK :
		{
			OnServerInfoAck(rkPacket);
		}
		break;

	case STPKM_ADMIN_RESPONSE :
		{
			OnAdminResponse(rkPacket);
		}
		break;

	case STPKM_SERVER_INFO_COMPLETE_ACK :
		{
			break;
		}

	case STPKM_COUNTRY_CCU_COUNT :
		{
			//국가별 ccu 
			if(m_bCountryCode)	OnReceiveEUCCU(rkPacket);
		}
		break;
		//packet 보내주면 afxMessageBox
	case STPKM_LICENSE_ALERT_CHECK : 
		{
			OnReceiveLicenseCheck( rkPacket );
		}
		break;
		*/
	default :
		TRACE(_T("잘못된 패킷ID : %lu\r\n"), rkPacket.GetPacketID());
		break;
	}
}

void CLS_MonitorDlg::OnLSResponse(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket )
{
	int type;
	rkPacket >> type;

	switch(type)
	{
	case EMCONTROLTYPE::LS_GETSERVERINFO:
		{
			OnLoginServerInfo(pkClientSocket);
		}
		break;
		/*
	case EMCONTROLTYPE::LS_FILLSERVERINFO:
		{
			OnLoginServerRefresh(rkPacket);
		}
		break;
	case EMCONTROLTYPE::LS_SETSERVERBLOCK:
		{
			OnLoginServerBlock(rkPacket);
		}
		break;
	case EMCONTROLTYPE::LS_SETALLSERVERBLOCK:
		{
			//TCHAR serverIP[STR_IP_MAX];
			//int blockstate;
			//rkPacket >> serverIP;
			//rkPacket >> bloc*kstate;
			//for(int i=0; i<m_vLoginPanel.size();++i)
			//{
			//	if(strcmp(m_vLoginPanel[i]->serverIP.c_str(),serverIP) == 0)
			//	{
			//		m_vLoginPanel[i]->PostMessageA(WM_LSLOGINSERVER,type,blockstate);
			//	}
			//}

		}
		break;
	case EMCONTROLTYPE::LS_FILLINFODRAW:
		{
			OnLoginInfoAck(rkPacket);
		}
		break;
		*/
	}
}

void CLS_MonitorDlg::OnLoginServerInfo(CIOClientSocket* pkClientSocket)
{
	CString tmp;
	int waitUser = 0;
	int maxWait = 0;
	int allBlock= 0;

	int m_maxUser;
	int m_CCU;
	int m_maxPanel;
	int m_matrix;
	int m_curHeight;
	int m_scrollPos;
	int m_panelWidth, m_panelHeight;

	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		TCHAR serverIP[64] = "";
		SP2Packet kPacket(TOOL_SVR::MSTMC_L_SERVER_RESPONSE);
		int type = EMCONTROLTYPE::LS_GETSERVERINFO;

		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		SERVERINFO* pkServerInfo = pkZoneDlg->GetOtherServerInfo(SERVER_TYPE_LOGIN);
		int serverState = SERVERSTATE_OFF;
		if(NULL != pkServerInfo)
		{
			serverState = pkServerInfo->serverState;
			sprintf_s( serverIP, sizeof(serverIP), "%s", pkServerInfo->serviceIP);
		}

		kPacket << type;
		kPacket << pkZoneDlg->m_loginDlg->m_maxUser;
		kPacket << pkZoneDlg->m_loginDlg->m_maxPanel;
		kPacket << waitUser;
		kPacket << maxWait;
		kPacket << allBlock;
		kPacket << serverIP;
		kPacket << serverState;


		for(int j = 0; j < pkZoneDlg->m_loginDlg->m_maxPanel; j++)
		{
			CPanelDialog* pkPanelDlg = pkZoneDlg->m_loginDlg->m_panels[j];

			kPacket << pkPanelDlg->m_serverId;
			kPacket << (TCHAR*)(LPCTSTR)pkPanelDlg->m_serverName;
			kPacket << 0;
			kPacket << (TCHAR*)(LPCTSTR)pkPanelDlg->m_serverIP;
			kPacket << pkPanelDlg->m_serverPort;
		}
		pkClientSocket->SendPacket(kPacket);	
	}
}

void CLS_MonitorDlg::OnLoginServerInfo()
{
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);

		for(std::map<int, CIOClientSocket*>::iterator it = m_ClientSocketList.begin() ; it != m_ClientSocketList.end() ; ++it)
		{
			CIOClientSocket* pkClientSocket = it->second;
			if(!pkClientSocket)
			{
				continue;
			}
			OnLoginServerInfo(pkClientSocket);
		}
	}
}

void CLS_MonitorDlg::OnTodayNumConnect(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	WORD wLogYear, wLogMonth, wLogDay;
	rkPacket >> wLogYear >> wLogMonth >> wLogDay;

	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		SP2Packet kPacket(TOOL_SVR::MSTMC_ANSWER_TODAY_CCU);
		int type = EMCONTROLTYPE::LS_GETSERVERINFO;

		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		CString strTitle = pkZoneDlg->GetTitle();

		CString fullPath;
		fullPath.Format(_T("%s\\%04lu%02lu%02lu.txt"), strTitle, wLogYear, wLogMonth, wLogDay);
		kPacket << wLogYear << wLogMonth << wLogDay;

		ifstream ifile;
		char line[200]; // 한 줄씩 읽어서 임시로 저장할 공간
		CString strTemp; 
		CString strCount; 
		int iLastMaxCount = 0;
		int imaxValue = 0;
		int iMaxCount =0;
		int iLineCount = 0;
        
		ifile.open(fullPath);  // 파일 열기
		if (ifile.is_open())
		{
			while (ifile.getline(line, sizeof(line))) // 한 줄씩 읽어 처리를 시작한다.
			{
				cout << line << endl; // 내용 출력
				strTemp = line;
				CString strYear = strTemp.Mid(1, 4);
				int iYear = _ttoi(strYear);
				CString strMonth = strTemp.Mid(6, 2);
				int iMonth = _ttoi(strMonth);
				CString strDay = strTemp.Mid(9, 2);
				int iDay = _ttoi(strDay);
				CString strHour = strTemp.Mid(12, 2);
				int iHour = _ttoi(strHour);
				CString strMinute = strTemp.Mid(15, 2);
				int iMinute = _ttoi(strMinute);
				CString strSecond = strTemp.Mid(18, 2);
				int iSecond = _ttoi(strSecond);

				strCount = strTemp.Left(strTemp.GetLength() - (1));
				int iPoint2 = strCount.ReverseFind('[');
				strCount = strCount.Right(strCount.GetLength() - (iPoint2 + 1));
				int iCount = _ttoi(strCount);
				if(iMaxCount < iCount)
				{
					iMaxCount = iCount;
				}
				iLineCount++;
			}

			if(iLastMaxCount < iMaxCount)
			{
				if(iMaxCount / 100 == 0) imaxValue = 100;
				else if(iMaxCount / 500 == 0) imaxValue = 500;
				else if(iMaxCount / 1000 == 0) imaxValue = 1000;
				else if(iMaxCount / 2500 == 0) imaxValue = 2500;
				else if(iMaxCount / 5000 == 0) imaxValue = 5000;
				else if(iMaxCount / 10000 == 0) imaxValue = 10000;
				else if(iMaxCount / 20000 == 0) imaxValue = 20000;
				else if(iMaxCount / 50000 == 0) imaxValue = 50000;
				else if(iMaxCount / 100000 == 0) imaxValue = 100000;
				else if(iMaxCount / 500000 == 0) imaxValue = 500000;
				else if(iMaxCount / 1000000 == 0) imaxValue = 1000000;
			}

			if(iLastMaxCount != imaxValue)
			{
				iLastMaxCount = imaxValue;
			}

			kPacket << iLastMaxCount << iLineCount;
		
			ifile.clear();
			ifile.seekg(0, ios::beg);
			while (ifile.getline(line, sizeof(line))) // 한 줄씩 읽어 처리를 시작한다.
			{
				cout << line << endl; // 내용 출력
				strTemp = line;
				CString strYear = strTemp.Mid(1, 4);
				WORD wYear = _ttoi(strYear);
				CString strMonth = strTemp.Mid(6, 2);
				WORD wMonth = _ttoi(strMonth);
				CString strDay = strTemp.Mid(9, 2);
				WORD wDay = _ttoi(strDay);
				CString strHour = strTemp.Mid(12, 2);
				WORD wHour = _ttoi(strHour);
				CString strMinute = strTemp.Mid(15, 2);
				WORD wMinute = _ttoi(strMinute);
				CString strSecond = strTemp.Mid(18, 2);
				int iSecond = _ttoi(strSecond);

				strCount = strTemp.Left(strTemp.GetLength() - (1));
				int iPoint2 = strCount.ReverseFind('[');
				strCount = strCount.Right(strCount.GetLength() - (iPoint2 + 1));
				int iCount = _ttoi(strCount);

				kPacket << wHour << wMinute << iCount;
			}
		}
		else
		{
			// 파일 오픈 실패
			kPacket << 0;
		}
		pkClientSocket->SendPacket(kPacket);	
		ifile.close(); // 파일 닫기
	}
}

void CLS_MonitorDlg::OnSearchNumConnect(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	WORD wLogYear, wLogMonth, wLogDay;
	rkPacket >> wLogYear >> wLogMonth >> wLogDay;

	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		SP2Packet kPacket(TOOL_SVR::MCTMS_ANSWER_SEARCH_CCU);
		int type = EMCONTROLTYPE::LS_GETSERVERINFO;

		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		CString strTitle = pkZoneDlg->GetTitle();

		if(true == pkZoneDlg->GetBillingFlag())
		{
			continue;
		}

		CString fullPath;
		fullPath.Format(_T("%s\\%04lu%02lu%02lu.txt"), strTitle, wLogYear, wLogMonth, wLogDay);
		kPacket << wLogYear << wLogMonth << wLogDay;

		ifstream ifile;
		char line[200]; // 한 줄씩 읽어서 임시로 저장할 공간
		CString strTemp; 
		CString strCount; 
		int iLastMaxCount = 0;
		int imaxValue = 0;
		int iMaxCount =0;
		int iLineCount = 0;
        
		ifile.open(fullPath);  // 파일 열기
		if (ifile.is_open())
		{
			while (ifile.getline(line, sizeof(line))) // 한 줄씩 읽어 처리를 시작한다.
			{
				cout << line << endl; // 내용 출력
				strTemp = line;
				CString strYear = strTemp.Mid(1, 4);
				int iYear = _ttoi(strYear);
				CString strMonth = strTemp.Mid(6, 2);
				int iMonth = _ttoi(strMonth);
				CString strDay = strTemp.Mid(9, 2);
				int iDay = _ttoi(strDay);
				CString strHour = strTemp.Mid(12, 2);
				int iHour = _ttoi(strHour);
				CString strMinute = strTemp.Mid(15, 2);
				int iMinute = _ttoi(strMinute);
				CString strSecond = strTemp.Mid(18, 2);
				int iSecond = _ttoi(strSecond);

				strCount = strTemp.Left(strTemp.GetLength() - (1));
				int iPoint2 = strCount.ReverseFind('[');
				strCount = strCount.Right(strCount.GetLength() - (iPoint2 + 1));
				int iCount = _ttoi(strCount);
				if(iMaxCount < iCount)
				{
					iMaxCount = iCount;
				}
				iLineCount++;
			}

			if(iLastMaxCount < iMaxCount)
			{
				if(iMaxCount / 100 == 0) imaxValue = 100;
				else if(iMaxCount / 500 == 0) imaxValue = 500;
				else if(iMaxCount / 1000 == 0) imaxValue = 1000;
				else if(iMaxCount / 2500 == 0) imaxValue = 2500;
				else if(iMaxCount / 5000 == 0) imaxValue = 5000;
				else if(iMaxCount / 10000 == 0) imaxValue = 10000;
				else if(iMaxCount / 20000 == 0) imaxValue = 20000;
				else if(iMaxCount / 50000 == 0) imaxValue = 50000;
				else if(iMaxCount / 100000 == 0) imaxValue = 100000;
				else if(iMaxCount / 500000 == 0) imaxValue = 500000;
				else if(iMaxCount / 1000000 == 0) imaxValue = 1000000;
			}

			if(iLastMaxCount != imaxValue)
			{
				iLastMaxCount = imaxValue;
			}

			kPacket << iLastMaxCount << iLineCount;
		
			ifile.clear();
			ifile.seekg(0, ios::beg);
			while (ifile.getline(line, sizeof(line))) // 한 줄씩 읽어 처리를 시작한다.
			{
				cout << line << endl; // 내용 출력
				strTemp = line;
				CString strYear = strTemp.Mid(1, 4);
				WORD wYear = _ttoi(strYear);
				CString strMonth = strTemp.Mid(6, 2);
				WORD wMonth = _ttoi(strMonth);
				CString strDay = strTemp.Mid(9, 2);
				WORD wDay = _ttoi(strDay);
				CString strHour = strTemp.Mid(12, 2);
				WORD wHour = _ttoi(strHour);
				CString strMinute = strTemp.Mid(15, 2);
				WORD wMinute = _ttoi(strMinute);
				CString strSecond = strTemp.Mid(18, 2);
				int iSecond = _ttoi(strSecond);

				strCount = strTemp.Left(strTemp.GetLength() - (1));
				int iPoint2 = strCount.ReverseFind('[');
				strCount = strCount.Right(strCount.GetLength() - (iPoint2 + 1));
				int iCount = _ttoi(strCount);

				kPacket << wHour << wMinute << iCount;
			}
		}
		else
		{
			// 파일 오픈 실패
			kPacket << 0;
		}
		pkClientSocket->SendPacket(kPacket);	
		ifile.close(); // 파일 닫기
	}
}

void CLS_MonitorDlg::OnRequipNumConnect(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		SP2Packet kPacket(TOOL_SVR::MSTMC_ANSWER_NUM_CONNECT);
		int type = EMCONTROLTYPE::LS_GETSERVERINFO;

		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		int ServerCount = pkZoneDlg->GetGameServerCount() + 1;

		kPacket << ServerCount;
		for(int i = 0 ; i < (ServerCount-1) ; i++)
		{
			SERVERINFO* pkServerInfo = pkZoneDlg->GetGameServerInfo(i);
			if(NULL == pkServerInfo)
			{
				continue;
			}

			kPacket << (TCHAR*)(LPCTSTR)pkServerInfo->serviceName.GetString() << (TCHAR*)(LPCTSTR)pkServerInfo->serviceIP.GetString() << pkServerInfo->servicePort;
			kPacket << pkServerInfo->connect << pkServerInfo->roomCount << pkServerInfo->plazaCount << pkServerInfo->battleRoomCount;
			kPacket << pkServerInfo->pingMS << pkServerInfo->DBMS << pkServerInfo->delay << pkServerInfo->relayFlag << pkServerInfo->serverState;
		}
	
		////////////////////////////////////////////////////////////
		// 메인서버 갱신
		////////////////////////////////////////////////////////////
		SERVERINFO *serverInfo = pkZoneDlg->GetOtherServerInfo(SERVER_TYPE_MAIN);
		if(NULL == serverInfo)
		{
			return ;
		}

		ioHashString mainIP;
		int mainPort;
		kPacket << (TCHAR*)(LPCTSTR)serverInfo->serviceIP.GetString() << serverInfo->servicePort;
		kPacket << serverInfo->serverState; // 이후 데이타는 모두 0
		pkClientSocket->SendPacket(kPacket);	
	}
}

void CLS_MonitorDlg::OnRequestMainServerAdminCommand(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	// 관리자툴에서 사용하는 프로토콜
	int iType = 0;
	rkPacket >> iType;

	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		if(NULL == pkZoneDlg)
		{
			continue;
		}

		switch( iType )
		{
		case ADMINCOMMAND_MTOOL_KICK:
			{
				ioHashString szID;
				ioHashString szPwd;
				ioHashString szPublicID;

				rkPacket >> szID >> szPwd;
				rkPacket >> szPublicID;

				SP2Packet kPacket(CTPKM_MAINSERVER_ADMINCOMMAND);
				kPacket << ADMINCOMMAND_MTOOL_KICK;
				kPacket << szID << szPwd;
				kPacket << szPublicID;
				pkZoneDlg->GetSockMain()->SendPacket(kPacket);
			}
			break;

		case ADMINCOMMAND_MTOOL_ANNOUNCE:
			{
				int msgType;
				ioHashString szID;
				ioHashString szPwd;
				ioHashString ioAnnounce, ioPublicID;
				rkPacket >> szID >> szPwd;
				rkPacket >> ioAnnounce >> msgType;
				if(msgType == ANNOUNCE_TYPE_ONE)
				{
					rkPacket >> ioPublicID;
				}

				SP2Packet kPacket(CTPKM_MAINSERVER_ADMINCOMMAND);
				kPacket << ADMINCOMMAND_MTOOL_ANNOUNCE;
				kPacket << szID << szPwd;
				kPacket << ioAnnounce << msgType;
				if(msgType == ANNOUNCE_TYPE_ONE)
				{
					kPacket << ioPublicID;
				}
				pkZoneDlg->GetSockMain()->SendPacket(kPacket);
			}
			break;

		case ADMINCOMMAND_MTOOL_ITEMINSERT :
			{
				ioHashString szID;
				ioHashString szPwd;
				ioHashString ioUserId;
				int presentType, value1, value2, ment, isPublicID;
				rkPacket >> szID >> szPwd;
				rkPacket >> ioUserId >> presentType >> value1 >> value2 >> ment >> isPublicID;

				SP2Packet kPacket(CTPKM_MAINSERVER_ADMINCOMMAND);
				kPacket << ADMINCOMMAND_MTOOL_ITEMINSERT;
				kPacket << szID << szPwd;
				kPacket << ioUserId << presentType << value1 << value2 << ment << isPublicID;
				pkZoneDlg->GetSockMain()->SendPacket(kPacket);
			}
			break;

		case ADMINCOMMAND_MTOOL_EVENTINSERT:
			{
				ioHashString szID;
				ioHashString szPwd;
				int values[64];
				int valueCount;
				rkPacket >> szID >> szPwd;
				rkPacket >> valueCount;
				for(int i = 0 ; i < valueCount ; i++)
				{
					rkPacket >> values[i];
				}

				SP2Packet kPacket(CTPKM_MAINSERVER_ADMINCOMMAND);
				kPacket << ADMINCOMMAND_MTOOL_EVENTINSERT ;
				kPacket << szID << szPwd;
				kPacket << valueCount;
				for(int i = 0 ; i < valueCount ; i++)
				{
					kPacket << values[i];
				}
				pkZoneDlg->GetSockMain()->SendPacket(kPacket);
			}
			break;

		case ADMINCOMMAND_MTOOL_AUTH :
			{
				ioHashString szID;
				ioHashString szPwd;

				rkPacket >> szID >> szPwd;

				SP2Packet kPacket(CTPKM_MAINSERVER_ADMINCOMMAND);

				kPacket << iType;
				kPacket << pkClientSocket->GetClientNumber();
				kPacket << szID;
				kPacket << szPwd;

				pkZoneDlg->GetSockMain()->SendPacket(kPacket);
			}
			break;

		case ADMINCOMMAND_MTOOL_USERBLOCK:
			{
				ioHashString szID;
				ioHashString szPwd;
				ioHashString ioPublicID;
				ioHashString ioReason;
				ioHashString ioBlockNote;
				int64 limitTime;
				int	iBlockLevel;
				
				rkPacket >> szID >> szPwd >> ioPublicID >> ioReason >> ioBlockNote >> limitTime >> iBlockLevel;

				SP2Packet kPacket(CTPKM_MAINSERVER_ADMINCOMMAND);
				kPacket << iType << pkClientSocket->GetClientNumber() << szID << szPwd << ioPublicID << ioReason << ioBlockNote << limitTime << iBlockLevel;
				
				pkZoneDlg->GetSockMain()->SendPacket(kPacket);
			}
			break;

		case ADMINCOMMAND_MTOOL_USERUNBLOCK:
			{
				ioHashString szID;
				ioHashString szPwd;
				ioHashString ioPublicID;
				ioHashString ioReason;
				ioHashString ioBlockNote;

				rkPacket >> szID >> szPwd >> ioPublicID >> ioReason >> ioBlockNote;

				SP2Packet kPacket(CTPKM_MAINSERVER_ADMINCOMMAND);
				kPacket << iType << pkClientSocket->GetClientNumber() << szID << szPwd << ioPublicID << ioReason << ioBlockNote;
				
				pkZoneDlg->GetSockMain()->SendPacket(kPacket);
			}
			break;

		case ADMINCOMMAND_MTOOL_SECRETSHOP:
			{
				int iCode, iCount;
				rkPacket >> iCode >> iCount;

				SP2Packet kPacket(CTPKM_MAINSERVER_ADMINCOMMAND);
				kPacket << ADMINCOMMAND_SECRETSHOP;
				kPacket << iCode << iCount;
				pkZoneDlg->GetSockMain()->SendPacket(kPacket);
			}
			break;
		
		case ADMINCOMMAND_MTOOL_COMPENSATION:
			{
				int presentType, iCode, iValue;
				__int64 iEndDate;
				rkPacket >> presentType >> iCode >> iValue >> iEndDate;

				SP2Packet kPacket(CTPKM_MAINSERVER_ADMINCOMMAND);
				kPacket << ADMINCOMMAND_COMPENSATION;
				kPacket << presentType << iCode << iValue <<iEndDate;
				pkZoneDlg->GetSockMain()->SendPacket(kPacket);
			}
			break;

		case ADMINCOMMAND_MTOOL_PRACTICE:
			{
				ioHashString szID;
				ioHashString szPwd;
				ioHashString szPublicID;

				rkPacket >> szID >> szPwd;
				rkPacket >> szPublicID;

				SP2Packet kPacket(CTPKM_MAINSERVER_ADMINCOMMAND);
				kPacket << ADMINCOMMAND_MTOOL_PRACTICE;
				kPacket << szID << szPwd;
				kPacket << szPublicID;
				pkZoneDlg->GetSockMain()->SendPacket(kPacket);
			}
			break;
		}
	}
}


void CLS_MonitorDlg::OnRequestGameServerCommand(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	ioHashString strServerIP;
	int iServicePort;
	DWORD protocol = 0;
	ioHashString szID, szPwd;
	rkPacket >> szID >> szPwd;
	rkPacket >> strServerIP >> iServicePort >> protocol;

	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		if(NULL == pkZoneDlg)
		{
			continue;
		}
		
		SP2Packet kPacket(CTPKM_MTOOL_GAMESERVER_PROTOCAL);
		kPacket << szID << szPwd;
		kPacket << strServerIP << iServicePort << protocol;

		if(CTPKM_GAMESERVER_CHANGE_RELAYSVR_STATE == protocol)
		{
			BOOL bRelayServerState = FALSE;
			rkPacket >> bRelayServerState;	
			kPacket << bRelayServerState;
		}
		pkZoneDlg->GetSockMain()->SendPacket(kPacket);
	}
}

void CLS_MonitorDlg::OnServerExit(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	ioHashString szID, szPwd;
	rkPacket >> szID >> szPwd;
	DWORD dwType = 0;

	SP2Packet kPacket;
	if(rkPacket.GetPacketID() ==TOOL_SVR::MCTMS_C_MAINSERVER_EXIT)
	{
		SP2Packet kTempPacket(CTPKM_MTOOL_MAINSERVER_EXIT);
		kPacket = kTempPacket;
		kPacket << szID << szPwd;
	}
	else if(rkPacket.GetPacketID() ==TOOL_SVR::MCTMS_C_MAINSERVER_SAFETYALLEXIT)
	{
		SP2Packet kTempPacket(CTPKM_MTOOL_MAINSERVER_SAFETYALLEXIT);
		kPacket = kTempPacket;
		rkPacket >> dwType;
		kPacket << szID << szPwd;
		kPacket << dwType;
	}
	else if(rkPacket.GetPacketID() ==TOOL_SVR::MCTMS_C_MAINSERVER_QUICKALLEXIT)
	{
		SP2Packet kTempPacket(CTPKM_MTOOL_MAINSERVER_QUICKALLEXIT);
		kPacket = kTempPacket;
		rkPacket >> dwType;
		kPacket << szID << szPwd;
		kPacket << dwType;
	}

	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		if(NULL == pkZoneDlg)
		{
			continue;
		}
		
		pkZoneDlg->GetSockMain()->SendPacket(kPacket);
	}
}

void CLS_MonitorDlg::OnGameServerReloadINI(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	ioHashString szID, szPwd;
	rkPacket >> szID >> szPwd;

	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		if(NULL == pkZoneDlg)
		{
			continue;
		}
		
		SP2Packet kPacket( CTPKM_MTOOL_MAINSERVER_GAMESERVER_RELOADINI );
		kPacket << szID << szPwd;
		pkZoneDlg->GetSockMain()->SendPacket(kPacket);
	}
}

void CLS_MonitorDlg::OnGameServerOption(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		if(NULL == pkZoneDlg)
		{
			continue;
		}
		
		SP2Packet kPacket( CTPKM_MAINSERVER_GAMESERVER_OPTION );
		pkZoneDlg->GetSockMain()->SendPacket(kPacket);
	}
}

void CLS_MonitorDlg::OnReloadCloseInfo(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	ioHashString szID, szPwd;
	rkPacket >> szID >> szPwd;

	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		if(NULL == pkZoneDlg)
		{
			continue;
		}
		
		SP2Packet kPacket( CTPKM_MTOOL_MAINSERVER_RELOADCLOSEINFO );
		kPacket << szID << szPwd;
		pkZoneDlg->GetSockMain()->SendPacket(kPacket);
	}
}

void CLS_MonitorDlg::OnResetEventShop(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	int type;
	rkPacket >> type;

	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		if(NULL == pkZoneDlg)
		{
			continue;
		}
		
		SP2Packet kPacket( CTPKM_MAINSERVER_RESETEVENTSHOP );
		kPacket << type;
		pkZoneDlg->GetSockMain()->SendPacket(kPacket);
	}
}

void CLS_MonitorDlg::OnResetOldMissionData(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		if(NULL == pkZoneDlg)
		{
			continue;
		}
		
		SP2Packet kPacket( CTPKM_MAINSERVER_RESET_OLDMISSIONDATA );
		pkZoneDlg->GetSockMain()->SendPacket(kPacket);
	}
}

void CLS_MonitorDlg::OnWhiteListRequest(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	BOOL bWhiteList;
	rkPacket >> bWhiteList;
	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		if(NULL == pkZoneDlg)
		{
			continue;
		}
		
		SP2Packet kPacket( CTPKM_WHITELIST_REQUEST );
		kPacket << bWhiteList;
		pkZoneDlg->GetSockMain()->SendPacket(kPacket);
	}
}

void CLS_MonitorDlg::OnUpdateClientVersion(CIOClientSocket* pkClientSocket, SP2Packet& rkPacket)
{
	bool useVersion;
	int version;

	rkPacket >> useVersion >> version;

	//서버 정보 전부 전송
	int index = m_zoneTabs.GetItemCount();
	for(int i = 0; i < index; i++)
	{
		CZoneDialog* pkZoneDlg = m_zoneTabs.GetDialog(i);
		if(NULL == pkZoneDlg)
		{
			continue;
		}
		
		SP2Packet kPacket( CTPKM_UPDATE_CLIENT_VERSION );
		kPacket << pkClientSocket->GetClientNumber() << useVersion << version;
		pkZoneDlg->GetSockMain()->SendPacket(kPacket);
	}
}



void CLS_MonitorDlg::OnLoginServerDisConnect(SERVERINFO* serverInfo, const ServerStates state)
{
	SP2Packet kPacket(TOOL_SVR::MSTMC_L_SERVER_DISCONNECT);

	kPacket << serverInfo->serverType;
	kPacket << state;

	for(std::map<int, CIOClientSocket*>::iterator it = m_ClientSocketList.begin() ; it != m_ClientSocketList.end() ; ++it)
	{
		CIOClientSocket* pkClientSocket = it->second;
		if(!pkClientSocket)
		{
			continue;
		}
		pkClientSocket->SendPacket(kPacket);	
	}
}

void CLS_MonitorDlg::OnMainServerDisConnect(SERVERINFO* serverInfo, const ServerStates state)
{
	SP2Packet kPacket(TOOL_SVR::MSTMC_M_SERVER_DISCONNECT);

	kPacket << serverInfo->serverType;
	kPacket << state;

	for(std::map<int, CIOClientSocket*>::iterator it = m_ClientSocketList.begin() ; it != m_ClientSocketList.end() ; ++it)
	{
		CIOClientSocket* pkClientSocket = it->second;
		if(!pkClientSocket)
		{
			continue;
		}
		pkClientSocket->SendPacket(kPacket);	
	}
}

void CLS_MonitorDlg::OnGameServerDisconnect(ioHashString serverIP, int serverPort)
{
	SP2Packet kPacket(TOOL_SVR::MSTMC_SERVER_DISCONNECT);
	kPacket << serverIP << serverPort;

	for(std::map<int, CIOClientSocket*>::iterator it = m_ClientSocketList.begin() ; it != m_ClientSocketList.end() ; ++it)
	{
		CIOClientSocket* pkClientSocket = it->second;
		if(!pkClientSocket)
		{
			continue;
		}
		pkClientSocket->SendPacket(kPacket);	
	}
}

void CLS_MonitorDlg::OnMainServerAdmin(int ClientNumber, CPacket &rkPacket)
{
	CIOClientSocket* pkClientSocket = OnFindClient(ClientNumber);
	if(NULL == pkClientSocket)
	{
		return ;
	}

	pkClientSocket->SendPacket(rkPacket);	
}
