// ioOTPDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "ioOTPDlg.h"
#include ".\iootpdlg.h"
#include "HelpFunc.h"
#include "AutoUpgradeDlg.h"
#include "Local\ioLocalParent.h"
#include "NetWork\ioClientSocket.h"
#include "NetWork\SP2Packet.h"
#include "NetWork\Protocol.h"
#include "StringManager\ioHashString.h"

// ioOTPDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(ioOTPDlg, CDialog)
ioOTPDlg::ioOTPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ioOTPDlg::IDD, pParent)
{
	m_pClientSocket = NULL;
	m_bOTPOk      = false;
	m_iOTPTimerCnt= 0;
	m_eNetworkState = NS_START;
}

ioOTPDlg::~ioOTPDlg()
{
	if( m_pClientSocket && m_pClientSocket->IsConnected() )
		m_pClientSocket->CloseClientTCP();

	SAFEDELETE( m_pClientSocket );
}

void ioOTPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEND, m_SendBtn);
	DDX_Control(pDX, IDC_CANCEL, m_CancelBtn);
	DDX_Control(pDX, IDC_1, m_1Btn);
	DDX_Control(pDX, IDC_2, m_2Btn);
	DDX_Control(pDX, IDC_3, m_3Btn);
	DDX_Control(pDX, IDC_4, m_4Btn);
	DDX_Control(pDX, IDC_5, m_5Btn);
	DDX_Control(pDX, IDC_6, m_6Btn);
	DDX_Control(pDX, IDC_7, m_7Btn);
	DDX_Control(pDX, IDC_8, m_8Btn);
	DDX_Control(pDX, IDC_9, m_9Btn);
	DDX_Control(pDX, IDC_10, m_10Btn);
	DDX_Control(pDX, IDC_PWEDIT, m_PWEdit);
	DDX_Control(pDX, IDC_DELETE, m_DeleteBtn);
}


BEGIN_MESSAGE_MAP(ioOTPDlg, CDialog)
	ON_BN_CLICKED(IDC_SEND, OnBnClickedSend)
	ON_BN_CLICKED(IDC_CANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_1, OnBnClicked1)
	ON_BN_CLICKED(IDC_2, OnBnClicked2)
	ON_BN_CLICKED(IDC_3, OnBnClicked3)
	ON_BN_CLICKED(IDC_4, OnBnClicked4)
	ON_BN_CLICKED(IDC_5, OnBnClicked5)
	ON_BN_CLICKED(IDC_6, OnBnClicked6)
	ON_BN_CLICKED(IDC_7, OnBnClicked7)
	ON_BN_CLICKED(IDC_8, OnBnClicked8)
	ON_BN_CLICKED(IDC_9, OnBnClicked9)
	ON_BN_CLICKED(IDC_10, OnBnClicked10)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_MESSAGE (WM_CLIENTOTPSOCKET  , OnClientSocket  )
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// ioOTPDlg 메시지 처리기입니다.

void ioOTPDlg::OnBnClickedSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString sPWText;
	m_PWEdit.GetWindowText( sPWText );
	if( sPWText.IsEmpty() )
		return;

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsRightPW( (LPCTSTR) sPWText ) )
	{
		CAutoUpgradeDlg* pDlg = NULL;
		if(AfxGetApp())
			pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
		if( pDlg )
			pDlg->ShowErrorDlg( pLocal->GetLoginWrongPWMent() , 227, false ); 

		m_PWEdit.SetWindowText( "" );
		return;
	}

	if( m_eNetworkState == NS_CONNECTED )
	{
		if( !SendServer( (LPCTSTR) sPWText ) )
			return;
	}

	SetChildsActive( false );

	SetTimer( TIME_ID_OTP, 1000, NULL );
	m_iOTPTimerCnt = 0;
}

void ioOTPDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_pClientSocket )
		m_pClientSocket->CloseClientTCP();
	SAFEDELETE( m_pClientSocket );

	EndDialog(0);

	if(AfxGetApp())
	{
		CAutoUpgradeDlg* pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
		if(pDlg)
		{
			pDlg->SetWantEndDialog();
		}
	}
}

void ioOTPDlg::OnBnClicked1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sTextNum;
	m_1Btn.GetWindowText( sTextNum );
	AddPWEdit( sTextNum );
}

void ioOTPDlg::OnBnClicked2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sTextNum;
	m_2Btn.GetWindowText( sTextNum );
	AddPWEdit( sTextNum );
}

void ioOTPDlg::OnBnClicked3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sTextNum;
	m_3Btn.GetWindowText( sTextNum );
	AddPWEdit( sTextNum );
}

void ioOTPDlg::OnBnClicked4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sTextNum;
	m_4Btn.GetWindowText( sTextNum );
	AddPWEdit( sTextNum );
}

void ioOTPDlg::OnBnClicked5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sTextNum;
	m_5Btn.GetWindowText( sTextNum );
	AddPWEdit( sTextNum );
}

void ioOTPDlg::OnBnClicked6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sTextNum;
	m_6Btn.GetWindowText( sTextNum );
	AddPWEdit( sTextNum );
}

void ioOTPDlg::OnBnClicked7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sTextNum;
	m_7Btn.GetWindowText( sTextNum );
	AddPWEdit( sTextNum );
}

void ioOTPDlg::OnBnClicked8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sTextNum;
	m_8Btn.GetWindowText( sTextNum );
	AddPWEdit( sTextNum );
}

void ioOTPDlg::OnBnClicked9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sTextNum;
	m_9Btn.GetWindowText( sTextNum );
	AddPWEdit( sTextNum );
}

void ioOTPDlg::OnBnClicked10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sTextNum;
	m_10Btn.GetWindowText( sTextNum );
	AddPWEdit( sTextNum );
}

void ioOTPDlg::OnBnClickedDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szPWText;
	m_PWEdit.GetWindowText( szPWText );
	int iLen = szPWText.GetLength();
	if( iLen < 0 )
		return;
	szPWText.Delete(iLen-1);
	m_PWEdit.SetWindowText( szPWText );
}

BOOL ioOTPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	SetWindowText( "Lost Saga" ); 
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL ioOTPDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		OnBnClickedSend();
		return TRUE;
	}
	else if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
		return FALSE;

	return CDialog::PreTranslateMessage(pMsg);
}

void ioOTPDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(nStatus != 0) return;

	//----------창 중간에 띄우기 ---------------------
	RECT winrect, workrect;
	// Find how large the desktop work area is
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrect, 0);
	int workwidth = workrect.right -  workrect.left;
	int workheight = workrect.bottom - workrect.top;

	// And how big the window is
	GetWindowRect(&winrect);
	int winwidth = winrect.right - winrect.left;
	int winheight = winrect.bottom - winrect.top;
	// Make sure it"s not bigger than the work area
	winwidth = min(winwidth, workwidth);
	winheight = min(winheight, workheight);

	// Now center it
	SetWindowPos( 
		NULL,
		workrect.left + (workwidth-winwidth) / 2,
		workrect.top + (workheight-winheight) / 2,
		winwidth, winheight, 
		SWP_SHOWWINDOW);
	//----------------------------------------------------------
	RandBtns();
}

void ioOTPDlg::OnClose() // ALT + F4 방지 위해서 빈함수로
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
}

void ioOTPDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch(nIDEvent)
	{
	case TIME_ID_OTP:
		{
			// 유저에게 로그인 중임을 나타내기 위해서 임의로 hover 발생
			//if( m_SendBtn.IsHover() )
			//	m_SendBtn.SetHover( FALSE );
			//else
			//	m_SendBtn.SetHover( TRUE );
			//m_SendBtn.Invalidate();

			m_iOTPTimerCnt++;

			if( m_eNetworkState == NS_CONNECTED )
			{
				CString sPW;
				m_PWEdit.GetWindowText( sPW );
				SendServer( (LPCTSTR) sPW );
			}

			if( m_iOTPTimerCnt > MAX_OTP_CONNECT_TIME_SEC )
			{
				if( m_pClientSocket && !m_pClientSocket->IsConnected() )
				{
					CAutoUpgradeDlg* pDlg = NULL;
					if(AfxGetApp())
						pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

					ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
					if( pDlg && pLocal )
						pDlg->ShowErrorDlg( pLocal->GetConnectFailMent() , 227, false ); 
					LOG.PrintTimeAndLog( 0, "%s - TCP Socket Failed(%d)", __FUNCTION__, WSAGetLastError() );

					m_pClientSocket->CloseClientTCP();
					m_pClientSocket->InitClientTCP( GetSafeHwnd(), m_pClientSocket->GetProcID(), m_pClientSocket->GetConnectedIP(), m_pClientSocket->GetConnectedPort() );

					m_eNetworkState = NS_START;
					KillTimer(TIME_ID_OTP);
					SetChildsActive( true );
					return;
				}
			}

			// 2분간 응답이 없으면 버튼 활성화 
			if( m_iOTPTimerCnt > MAX_OTP_TIME_SEC )
			{
				m_eNetworkState = NS_CONNECTED;
				KillTimer(TIME_ID_OTP);
				SetChildsActive( true );
				return;
			}
		}
		break;
	}
}

void ioOTPDlg::RandBtns()
{
	IntVec vNumber;
	enum { MAX_NUM = 10, };
	for (int i = 0; i < MAX_NUM ; i++)
	{
		vNumber.push_back(i);	
	}
	
	std::random_shuffle( vNumber.begin(), vNumber.end() );

	int iSize = vNumber.size();
	for (int i = 0; i < iSize ; i++)
	{
		char szTextNum[MAX_PATH]="";
		StringCbPrintf( szTextNum, sizeof( szTextNum ), "%d", vNumber[i] );
		if( i == 0 )
			m_1Btn.SetWindowText( szTextNum );
		else if( i == 1 )
			m_2Btn.SetWindowText( szTextNum );
		else if( i == 2 )
			m_3Btn.SetWindowText( szTextNum );
		else if( i == 3 )
			m_4Btn.SetWindowText( szTextNum );
		else if( i == 4 )
			m_5Btn.SetWindowText( szTextNum );
		else if( i == 5 )
			m_6Btn.SetWindowText( szTextNum );
		else if( i == 6 )
			m_7Btn.SetWindowText( szTextNum );
		else if( i == 7 )
			m_8Btn.SetWindowText( szTextNum );
		else if( i == 8 )
			m_9Btn.SetWindowText( szTextNum );
		else if( i == 9 )
			m_10Btn.SetWindowText( szTextNum );
	}
}

void ioOTPDlg::AddPWEdit( CString &rsTextNum )
{
	CString szPWText;
	m_PWEdit.GetWindowText( szPWText );

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal  )
	{
		if( szPWText.GetLength() >= pLocal->GetMaxOPTSize() )
			return;
	}

	szPWText += rsTextNum;
	m_PWEdit.SetWindowText( szPWText );
}

bool ioOTPDlg::SendServer( const char *szPW )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsUseSocket() )
		return false;

	if( szPW == NULL )
		return false;

	char szRealID[MAX_PATH]="";
	if( pLocal )
		StringCbCopy( szRealID, sizeof( szRealID ), pLocal->GetLoginID() );

	char szRealPW[MAX_PATH]="";
	if( pLocal )
		StringCbPrintf( szRealPW, sizeof( szRealPW ), "%s|%s", pLocal->GetOTPID(), szPW ); // 0은 로그인 PW 자리

	char szRandomKey[MAX_PATH];
	ZeroMemory( szRandomKey, sizeof( szRandomKey ) );
	Help::GetRandomKey( szRandomKey, sizeof( szRandomKey ) );

	char szUserKey[MAX_PATH]="";
	StringCbPrintf( szUserKey, sizeof( szUserKey ), "%s%s", szRealID, szRandomKey );

	char szOTPEncodePW[MAX_PATH];
	ZeroMemory( szOTPEncodePW, sizeof( szOTPEncodePW ) );
	StringCbCopy( szOTPEncodePW, sizeof( szOTPEncodePW ), szRandomKey );

	char szEncode[MAX_PATH];
	ZeroMemory( szEncode, sizeof( szEncode ) );
	Help::Encode( szRealPW, strlen( szRealPW ), szEncode, sizeof( szEncode ), szUserKey, strlen( szUserKey ) );

	StringCbCat( szOTPEncodePW, sizeof( szOTPEncodePW ), szEncode );

	if( strcmp( szOTPEncodePW, "") == 0 )
		return false;

	char szRealEncodePW[MAX_PATH]="0|"; // 로그인 비밀번호 자리 확보
	StringCbCat( szRealEncodePW, sizeof( szRealEncodePW ), szOTPEncodePW );
	

	SP2Packet kPacket( ASTPK_OTHER_COMPANY_OTP_REQUEST );
	kPacket << szRealID;
	kPacket << szRealEncodePW;
	if( m_pClientSocket )
		m_pClientSocket->SendLoop( kPacket );

	if( pLocal )
		pLocal->SetOTPEncodePW( szOTPEncodePW );

	m_bOTPOk = false;
	m_eNetworkState = NS_SENT; 
	LOG.PrintTimeAndLog( 0, "Send OTP" );
	return true;
}

LONG ioOTPDlg::OnClientSocket( WPARAM wParam, LPARAM lParam )
{
	LONG result=0;
	if(m_pClientSocket)
		result = m_pClientSocket->EvaluateSocket( wParam, lParam );
	return result; 
}

void ioOTPDlg::ConnectSocket()
{
	if( m_pClientSocket )
		return;

	m_pClientSocket = Help::CreateSocket( WM_CLIENTOTPSOCKET, GetSafeHwnd() );
}

void ioOTPDlg::RecvPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case ASTPK_OTHER_COMPANY_OTP_RESULT:
		{
			m_eNetworkState = NS_CONNECTED;
			KillTimer(TIME_ID_OTP);

			bool bOk = false;
			rkPacket >> bOk;

			CAutoUpgradeDlg* pDlg = NULL;
			if(AfxGetApp())
				pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

			if( bOk )
			{
				m_bOTPOk = true;

				if( pDlg )
					pDlg->ResumeHttpManager();

				if( m_pClientSocket )
					m_pClientSocket->CloseClientTCP();

				EndDialog(0);
			}
			else
			{
				bool bError = false;
				ioHashString sError;
				rkPacket >> bError;
				if( bError )
					rkPacket >> sError;
				else
					sError = "Unknown Error";

				SetChildsActive( true );

				if( pDlg )
					pDlg->ShowErrorDlg( sError.c_str() , 227, false ); 

				m_PWEdit.SetWindowText( "" );
			}
		}
		return;
	}
}

void ioOTPDlg::SetChildsActive( bool bActive )
{
	if( bActive )
	{
		m_SendBtn.EnableWindow( TRUE );
		m_CancelBtn.EnableWindow( TRUE );
		m_1Btn.EnableWindow( TRUE );
		m_2Btn.EnableWindow( TRUE );
		m_3Btn.EnableWindow( TRUE );	
		m_4Btn.EnableWindow( TRUE );
		m_5Btn.EnableWindow( TRUE );
		m_6Btn.EnableWindow( TRUE );
		m_7Btn.EnableWindow( TRUE );
		m_8Btn.EnableWindow( TRUE );
		m_9Btn.EnableWindow( TRUE );
		m_10Btn.EnableWindow( TRUE );
		m_DeleteBtn.EnableWindow( TRUE );
	}
	else
	{
		m_SendBtn.EnableWindow( FALSE );
		m_CancelBtn.EnableWindow( FALSE );
		m_1Btn.EnableWindow( FALSE );
		m_2Btn.EnableWindow( FALSE );
		m_3Btn.EnableWindow( FALSE );	
		m_4Btn.EnableWindow( FALSE );
		m_5Btn.EnableWindow( FALSE );
		m_6Btn.EnableWindow( FALSE );
		m_7Btn.EnableWindow( FALSE );
		m_8Btn.EnableWindow( FALSE );
		m_9Btn.EnableWindow( FALSE );
		m_10Btn.EnableWindow( FALSE );
		m_DeleteBtn.EnableWindow( FALSE );
	}
}