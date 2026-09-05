// ioLoginDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "ioLoginDlg.h"
#include ".\iologindlg.h"
#include "ioINILoaderAU.h"
#include "HelpFunc.h"
#include "AutoUpgradeDlg.h"
#include "Local\ioLocalParent.h"
#include "NetWork\ioClientSocket.h"
#include "Local\ioLocalIndonesia.h"
#include "NetWork\SP2Packet.h"
#include "NetWork\Protocol.h"
#include "StringManager\ioHashString.h"
#include <mmsystem.h>
#include "StringManager\Safesprintf.h"
#include "StringManager\ioStringManager.h"
#include <exdisp.h>
#include <mshtmlc.h>
#include <comdef.h>
#include <mshtmhst.h>


// ioLoginDlg 대화 상자입니다.

ioLoginDlg::ioLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogSK(ioLoginDlg::IDD, pParent)
{
	m_bIDEditBlank = false;
	m_bPWEditBlank = false;
	m_bSelectIDEdit = false;
	m_bSelectPWEdit = false;
	m_pClientSocket = NULL;
	m_bLoginOk      = false;
	m_iConnectTimerCnt = 0;
	m_iTotalConnectTimerCnt = 0;
	m_eNetworkState = NS_START;
	m_iServerIPPortIdx      = 0;
}

ioLoginDlg::~ioLoginDlg()
{
	m_IDEditNormalBitmap.DeleteObject();
	m_IDEditSelectBitmap.DeleteObject();
	m_PWEditNormalBitmap.DeleteObject();
	m_PWEditSelectBitmap.DeleteObject();
	m_EditNormalBGBrush.DeleteObject();
	m_EditSelectBGBrush.DeleteObject();

	if( m_pClientSocket && m_pClientSocket->IsConnected() )
		m_pClientSocket->CloseClientTCP();

	SAFEDELETE( m_pClientSocket );

	m_vServerIPPort.clear();
}

void ioLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogSK::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGIN_X, m_XBtn);
	DDX_Control(pDX, IDC_LOGIN_IDSAVE, m_IDSaveBtn);
	DDX_Control(pDX, IDC_LOGIN_ENTER, m_LoginBtn);
	DDX_Control(pDX, IDC_LOGIN_ENTRY, m_EntryBtn);
	DDX_Control(pDX, IDC_LOGIN_FIND_ID, m_FindIDBtn);
	DDX_Control(pDX, IDC_LOGIN_EDIT_ID, m_IDEdit);
	DDX_Control(pDX, IDC_LOGIN_EDIT_PW, m_PWEdit);
	DDX_Control(pDX, IDC_LOGIN_FIND_PW, m_FindPWBtn);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
	DDX_Control(pDX, IDC_LOGIN_TEXT, m_Static);
	DDX_Control(pDX, IDC_EXPLORER_IMG, m_webBackImage);
}


BEGIN_MESSAGE_MAP(ioLoginDlg, CDialogSK)
	ON_BN_CLICKED(IDC_LOGIN_IDSAVE, OnBnClickedLoginIdsave)
	ON_BN_CLICKED(IDC_LOGIN_X, OnBnClickedLoginX)
	ON_BN_CLICKED(IDC_LOGIN_ENTER, OnBnClickedLoginEnter)
	ON_BN_CLICKED(IDC_LOGIN_ENTRY, OnBnClickedLoginEntry)
	ON_BN_CLICKED(IDC_LOGIN_FIND_ID, OnBnClickedLoginFindId)
	ON_BN_CLICKED(IDC_LOGIN_FIND_PW, OnBnClickedLoginFindPw)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_MESSAGE (WM_CLIENTSOCKET  , OnClientSocket  )
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeDomain)
END_MESSAGE_MAP()


// ioLoginDlg 메시지 처리기입니다.
void ioLoginDlg::OnBnClickedLoginX()
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

void ioLoginDlg::OnBnClickedLoginIdsave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ioLoginDlg::OnBnClickedLoginEnter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_bIDEditBlank || m_bPWEditBlank )
		return;

	CString sID;
	m_IDEdit.GetWindowText( sID );
	if( sID.IsEmpty() )
		return;

	CString sPW;
	m_PWEdit.GetWindowText( sPW );
	if( sPW.IsEmpty() )
		return;

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	if( pLocal && pLocal->IsPrivateLowerID() )
	{
		sID.MakeLower();
	}

	if( pLocal && !pLocal->IsRightID( (LPCTSTR) sID ) )
	{
		CAutoUpgradeDlg* pDlg = NULL;
		if(AfxGetApp())
			pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
		if( pDlg )
			pDlg->ShowErrorDlg( pLocal->GetLoginWrongIDMent() , 227, false ); 

		m_IDEdit.SetWindowText( "" );
		return;
	}

	if( pLocal && !pLocal->IsRightPW( (LPCTSTR) sPW ) )
	{
		CAutoUpgradeDlg* pDlg = NULL;
		if(AfxGetApp())
			pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
		if( pDlg )
			pDlg->ShowErrorDlg( pLocal->GetLoginWrongPWMent() , 227, false ); 

		m_PWEdit.SetWindowText( "" );
		return;
	}

	if( pLocal )
	{
		pLocal->SetUserType( "WMU" );
	}

	// 유럽쪽 에러 텍스트가 길어 사이즈 늘림.	JCLEE 130326
	char szErrorMsg[MAX_PATH*4] = {0, };
	if( pLocal && !pLocal->GetLoginResultPostData(szErrorMsg, sizeof(szErrorMsg), NULL, sID.GetString(), sPW.GetString()) )
	{
		CAutoUpgradeDlg* pDlg = NULL;
		if(AfxGetApp())
			pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

		pDlg->ShowErrorDlg( szErrorMsg, 100, false);
		return;
	}

	if( m_eNetworkState == NS_CONNECTED )
	{
		if( !SendServer( (LPCTSTR) sID, (LPCTSTR) sPW ) )
			return;
	}
}

void ioLoginDlg::OnBnClickedLoginEntry()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_sEntryURL.IsEmpty() )
		Help::ExcuteWeb( (LPCTSTR) m_sEntryURL );
}


void ioLoginDlg::OnBnClickedLoginFindId()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_sFindIDURL.IsEmpty() )
		Help::ExcuteWeb( (LPCTSTR) m_sFindIDURL );
}

void ioLoginDlg::OnBnClickedLoginFindPw()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsFacebookLogin() )
	{
		char rootDir[MAX_PATH]="";
		::GetCurrentDirectory(MAX_PATH,rootDir);

		char szClientName[MAX_PATH] = "";
		StringCbPrintf(szClientName, sizeof(szClientName), FACEBOOK_FILE_NAME, rootDir );

		char szCmdLine[MAX_PATH]="";
		StringCbCopy( szCmdLine, sizeof( szCmdLine ), FACEBOOK_LOGIN_CMD );

		Help::CreateProcessWithInfo( szClientName, szCmdLine );
		return;
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_sFindPWURL.IsEmpty() )
		Help::ExcuteWeb( (LPCTSTR) m_sFindPWURL );
}

BOOL ioLoginDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		OnBnClickedLoginEnter();
		return TRUE;
	}
	else if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
		return FALSE;


	return CDialogSK::PreTranslateMessage(pMsg);
}

BOOL ioLoginDlg::OnInitDialog()
{
	CDialogSK::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// skin
	DWORD dwLoginBackID     = IDB_LOGIN;
	DWORD dwLoginEnterBtnID = IDB_LOGIN_ENTER_BTN;
	DWORD dwLoginEntryBtnID = IDB_LOGIN_ENTRY_BTN;
	DWORD dwLoginFindIDBtnID = IDB_LOGIN_FINDID_BTN;
	DWORD dwLoginFindPWBtnID = IDB_LOGIN_FINDPW_BTN;
	CString		sLoginDlgImageUrl;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	if( pLocal )
	{
		dwLoginBackID      = pLocal->GetResourceIDLoginBackBmp();
		dwLoginEnterBtnID  = pLocal->GetResourceIDLoginEnterBtnBmp();
		dwLoginEntryBtnID  = pLocal->GetResourceIDLoginEntryBtnBmp();
		dwLoginFindIDBtnID = pLocal->GetResourceIDLoginFindIDBtnBmp();
		dwLoginFindPWBtnID = pLocal->GetResourceIDLoginFindPWBtnBmp();
	}
	//
	EnableEasyMove();                    // enable moving of the dialog by clicking anywhere in the dialog
	SetBitmap( dwLoginBackID );          // set background bitmap
	SetStyle(LO_RESIZE);                 // resize dialog to the size of the bitmap
	SetTransparentColor(RGB(0, 255, 0)); // set green as the transparent color

	m_XBtn.LoadBitmap(IDB_LOGIN_X_BTN, TRUE );
	m_XBtn.MoveWindow(311, 5, 25, 26);

	m_LoginBtn.LoadBitmap(dwLoginEnterBtnID, TRUE);
	m_LoginBtn.MoveWindow(224, 263, 102, 60);

	m_EntryBtn.LoadBitmap(dwLoginEntryBtnID, TRUE);
	m_EntryBtn.MoveWindow(15, 378, 311, 29);

	m_FindIDBtn.LoadBitmap(dwLoginFindIDBtnID, TRUE);
	m_FindIDBtn.MoveWindow(15, 351, 154, 24);

	m_FindPWBtn.LoadBitmap(dwLoginFindPWBtnID, TRUE);
	m_FindPWBtn.MoveWindow(171, 351, 155, 24);
	
#ifdef XTRAP 
	m_FindPWBtn.EnableWindow(FALSE);
#else
	m_FindPWBtn.EnableWindow(TRUE);
#endif

	m_IDSaveBtn.LoadBitmap(IDB_LOGIN_IDSAVE_BTN, TRUE);
	m_IDSaveBtn.MoveWindow(192, 263, 29, 29);
	m_IDSaveBtn.SetCheckBtn( TRUE ); 

	m_IDEdit.MoveWindow(20, 265, 165, 25);
	m_PWEdit.MoveWindow(24, 302, 193, 19);

	m_EditTextColor = RGB(58,58,58);
	m_EditNoticeTextColor = RGB(124,124,124);

	m_EditNormalBGColor   = RGB(230, 230, 230);
	m_EditNormalBGBrush.CreateSolidBrush( m_EditNormalBGColor );

	m_EditSelectBGColor   = RGB(249, 249, 249);
	m_EditSelectBGBrush.CreateSolidBrush( m_EditSelectBGColor );

	if( pLocal && pLocal->IsDomain() )
	{
		pLocal->SetDomainList();
		int iMax = pLocal->GetMaxDomain();
		for (int i = 0; i < iMax; i++)
		{
			ioHashString sTitle;
			pLocal->GetDomainTitle( i, sTitle );
			m_ComboBox.AddString( sTitle.c_str() );
		}
		m_ComboBox.MoveWindow( 15, 162, 165, 75 );
		m_ComboBox.SetCurSel(0);
	}
	else
	{
		m_ComboBox.ShowWindow( SW_HIDE );
	}
	//

	// font
	m_Font.CreateFont(13,0,0,0,FW_BOLD,FALSE,FALSE, FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH,"Tahoma");
	m_IDEdit.SetFont(&m_Font,TRUE);
	m_PWEdit.SetFont(&m_Font,TRUE);

	// edit back img
	m_IDEditNormalBitmap.DeleteObject();
	m_IDEditNormalBitmap.LoadBitmap(IDB_LOGIN_EDIT_NORMAL );

	m_IDEditSelectBitmap.DeleteObject();
	m_IDEditSelectBitmap.LoadBitmap(IDB_LOGIN_EDIT_SELECT );

	m_PWEditNormalBitmap.DeleteObject();
	m_PWEditNormalBitmap.LoadBitmap(IDB_LOGIN_EDIT_PW_NORMAL );

	m_PWEditSelectBitmap.DeleteObject();
	m_PWEditSelectBitmap.LoadBitmap(IDB_LOGIN_EDIT_PW_SELECT );

	// edit 위치 수정
	CRect rRect( 4, 6, 165, 25 );
	m_IDEdit.SetRect( &rRect );	

	SetWindowText( "Lost Saga" ); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void ioLoginDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogSK::OnShowWindow(bShow, nStatus);

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
}

HBRUSH ioLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogSK::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( pWnd->GetDlgCtrlID() == IDC_LOGIN_IDSAVE )
		return ((HBRUSH)GetStockObject(NULL_BRUSH));
	else if(pWnd->GetDlgCtrlID() == IDC_LOGIN_ENTER)
		return ((HBRUSH)GetStockObject(NULL_BRUSH));
		

	// TODO:  여기서 DC의 특성을 변경합니다.
	switch(nCtlColor)
	{
	case CTLCOLOR_EDIT:   // edit 박스가 쓰기상태일때
	case CTLCOLOR_STATIC: // edit 박스가 read only 일때
		{
			if( pDC && pWnd )
			{
				if( pWnd->GetDlgCtrlID() == IDC_LOGIN_EDIT_ID ) 
				{
					if( m_bIDEditBlank )
						pDC->SetTextColor( m_EditNoticeTextColor );
					else
						pDC->SetTextColor( m_EditTextColor );

					if( m_bSelectIDEdit )
					{
						pDC->SetBkColor( m_EditSelectBGColor );
						return m_EditSelectBGBrush;
					}
					else
					{
						pDC->SetBkColor( m_EditNormalBGColor );
						return m_EditNormalBGBrush;
					}
				}
				else if( pWnd->GetDlgCtrlID() == IDC_LOGIN_EDIT_PW )
				{
					if( m_bPWEditBlank )
						pDC->SetTextColor( m_EditNoticeTextColor );
					else
						pDC->SetTextColor( m_EditTextColor );

					if( m_bSelectPWEdit )
					{
						pDC->SetBkColor( m_EditSelectBGColor );
						return m_EditSelectBGBrush;
					}
					else
					{
						pDC->SetBkColor( m_EditNormalBGColor );
						return m_EditNormalBGBrush;
					}
				}
			}
		}
		break;
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

void ioLoginDlg::OnClose() // ALT + F4 방지 위해서 빈함수로
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
}

void ioLoginDlg::LoadINI()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	char szBuf[MAX_PATH*2]="";

	ioINILoaderAU kLoader( "info\\start.ini" );
	kLoader.SetTitle( "Info" );
	kLoader.LoadString( "LoginURL", "", szBuf, sizeof( szBuf ) );
	if( strcmp( szBuf, "" ) != 0 )
	{
		if( pLocal && pLocal->IsLoginURLDelete() ) // 인도네시아에서 보안상 삭제해달라고 함
			kLoader.SaveString( "LoginURL", "" ); 
	}
	ZeroMemory( szBuf, sizeof( szBuf ) );

	if( pLocal && pLocal->IsLoginURLINI() )
	{
		kLoader.LoadString( "EntryURL", "", szBuf, sizeof( szBuf ) );
		m_sEntryURL = szBuf;
		ZeroMemory( szBuf, sizeof( szBuf ) );

		kLoader.LoadString( "FindIDURL", "", szBuf, sizeof( szBuf ) );
		m_sFindIDURL = szBuf;
		ZeroMemory( szBuf, sizeof( szBuf ) );

		kLoader.LoadString( "FindPWURL", "", szBuf, sizeof( szBuf ) );
		m_sFindPWURL = szBuf;
		ZeroMemory( szBuf, sizeof( szBuf ) );
	}
	else
	{
		if( pLocal )
		{
			m_sEntryURL  = pLocal->GetEntryURL();
			m_sFindIDURL = pLocal->GetFindIDURL();
			m_sFindPWURL = pLocal->GetFindPWURL();
//			m_sLoginDlgImage   = pLocal->GetLoginDlgImage();
		}
	}

	// size은 ini 읽어오지 않는다.
	if( pLocal )
	{
		m_IDEdit.SetLimitText( pLocal->GetMaxIDSize() );
		m_PWEdit.SetLimitText( pLocal->GetMaxPWSize() );
	}
	
	ioINILoaderAU kIDLoader("Save\\default\\setting.ini");
	kIDLoader.SetTitle("Setting");
	kIDLoader.LoadString( "LoginID", "", szBuf, sizeof( szBuf ) );
	if( strcmp( szBuf, "" ) != 0 )
	{
		m_IDEdit.SetWindowText( szBuf );
		// 글자표시 일부 PC 오동작으로 제거 
		m_PWEdit.ModifyStyle(NULL, ES_PASSWORD);
		m_PWEdit.SetPasswordChar(0); 
		//m_PWEdit.SetWindowText( PWEDIT_BLANK_TEXT );
		m_PWEdit.SetWindowText( "Password" );
		m_bIDEditBlank = false;
		m_bPWEditBlank = true;
		m_IDSaveBtn.SetChecked( TRUE );
		m_IDSaveBtn.Invalidate();
	}
	else
	{
		m_IDEdit.SetWindowText( "Username" );
//		m_IDEdit.SetWindowText( IDEDIT_BLANK_TEXT );
		m_PWEdit.ModifyStyle(NULL, ES_PASSWORD);
		m_PWEdit.SetPasswordChar(0); // 글자표시
		//m_PWEdit.SetWindowText( PWEDIT_BLANK_TEXT ); //출력할 문자열?;
		m_PWEdit.SetWindowText( "Password" );
		m_bIDEditBlank = true;
		m_bPWEditBlank = true;
		m_IDSaveBtn.SetChecked( FALSE );
	}

	if( pLocal && pLocal->IsDomain() )
	{
		int iSelect = kIDLoader.LoadInt( "Domain", 0 );
		m_ComboBox.SetCurSel( iSelect );
	}

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_EU 
		|| ioLocalManager::GetLocalType() == ioLocalManager::LCT_GERMANY
		|| ioLocalManager::GetLocalType() == ioLocalManager::LCT_FRANCE
		|| ioLocalManager::GetLocalType() == ioLocalManager::LCT_ITALIA
		|| ioLocalManager::GetLocalType() == ioLocalManager::LCT_POLAND
		|| ioLocalManager::GetLocalType() == ioLocalManager::LCT_TURKEY )
	{
		pLocal->RegisterCallBack(m_hWnd);
	}

}

void ioLoginDlg::LoadBackImage()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	CAutoUpgradeDlg* pDlg = NULL;
	pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	const char* szTemp = pDlg->GetBackGroundURL();

	GetDlgItem(IDC_LOGIN_TEXT)->ShowWindow(FALSE);
	ioLocalManager::LocalType CurrLocal= ioLocalManager::GetLocalType();
	if( CurrLocal == ioLocalManager::LCT_INDONESIA ||
		CurrLocal == ioLocalManager::LCT_SINGAPORE ||
		CurrLocal == ioLocalManager::LCT_MALAYSIA ||
		CurrLocal == ioLocalManager::LCT_US		||
		CurrLocal == ioLocalManager::LCT_BRAZIL		||
		CurrLocal == ioLocalManager::LCT_EU		||
		CurrLocal == ioLocalManager::LCT_ITALIA	||
		CurrLocal == ioLocalManager::LCT_GERMANY ||
		CurrLocal == ioLocalManager::LCT_PHILIPPINE)
	{
		m_webBackImage.Navigate(szTemp, NULL, NULL, NULL, NULL);
		m_webBackImage.MoveWindow( 2, 44, 337, 205 );
		m_webBackImage.ShowWindow( SW_SHOW);
	}
	else
	{
//		m_webBackImage.Navigate("http://www.nxcreative.co.kr/pageA.php", NULL, NULL, NULL, NULL);
		m_webBackImage.ShowWindow( SW_HIDE);		
	}
}

BOOL ioLoginDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(HIWORD(wParam) == EN_SETFOCUS)
	{
		CEdit *pEdit=(CEdit*)GetDlgItem(LOWORD(wParam));
		if( pEdit )
		{
			if( pEdit->GetDlgCtrlID() == IDC_LOGIN_EDIT_ID )
			{
				m_bSelectIDEdit = true;

				if( m_bIDEditBlank )
				{
					pEdit->SetWindowText( "" );
					m_bIDEditBlank = false; // 글자입력시 글자색 변경을 위해서
				}
				Invalidate(FALSE);
			}
			else if( pEdit->GetDlgCtrlID() == IDC_LOGIN_EDIT_PW )
			{
				m_bSelectPWEdit = true;

				if( m_bPWEditBlank )
				{
					//m_PWEdit.ModifyStyle(NULL, ES_PASSWORD);
					pEdit->SetPasswordChar('*');
					pEdit->SetWindowText( "" );
					m_bPWEditBlank = false; // 글자입력시 글자색 변경을 위해서
				}
				Invalidate(FALSE);
			}
		}
	}
	else if( HIWORD(wParam) == EN_KILLFOCUS )
	{
		CEdit *pEdit=(CEdit*)GetDlgItem(LOWORD(wParam));
		if( pEdit )
		{
			if( pEdit->GetDlgCtrlID() == IDC_LOGIN_EDIT_ID )
			{
				m_bSelectIDEdit = false;

				CString sText;
				pEdit->GetWindowText(sText);
				if( !sText.IsEmpty() )
					m_bIDEditBlank = false;
				else
					m_bIDEditBlank = true;

				if( m_bIDEditBlank )
				{
					//	pEdit->SetWindowText( IDEDIT_BLANK_TEXT );
					pEdit->SetWindowText( "" );
				}

				Invalidate(FALSE);
			}
			else if( pEdit->GetDlgCtrlID() == IDC_LOGIN_EDIT_PW )
			{
				m_bSelectPWEdit = false;

				CString sText;
				pEdit->GetWindowText(sText);
				if( !sText.IsEmpty() )
					m_bPWEditBlank = false;
				else
					m_bPWEditBlank = true;

				if( m_bPWEditBlank )
				{
					//pEdit->ModifyStyle(NULL, ES_PASSWORD);
					//pEdit->SetPasswordChar(0); // 글자표시
					//pEdit->SetWindowText( PWEDIT_BLANK_TEXT );
					pEdit->SetWindowText( "" );
				}

				Invalidate(FALSE);
			}
		}
	}
	return CDialogSK::OnCommand(wParam, lParam);
}

void ioLoginDlg::DrawBitmap( CPaintDC *pDC, CBitmap *pBitmap, int iX, int iY, int iWidth, int iHeight )
{
	if( !pDC )
		return;

	if( !pBitmap )
		return;

	CBitmap  *pOldBitmap = NULL;
	CDC      MemDC;

	MemDC.CreateCompatibleDC(pDC);
	pOldBitmap = (CBitmap *)MemDC.SelectObject(pBitmap);
//	pDC->BitBlt( iX, iY, iWidth, iHeight, &MemDC, 0, 0, SRCCOPY);

	pDC->TransparentBlt(iX, iY, iWidth, iHeight, &MemDC, 0, 0, iWidth, iHeight, RGB(0, 255, 0) );

	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC();
}
void ioLoginDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogSK::OnPaint()을(를) 호출하지 마십시오.
	enum 
	{
		X = 15,
		Y = 263,
		WIDTH  = 175,
		HEIGHT = 29,

		X_PW      = 15,
		Y_PW      = 294,
		WIDTH_PW  = 206,
		HEIGHT_PW = 29,
	};

	if( m_bSelectIDEdit )
		DrawBitmap( &dc, &m_IDEditSelectBitmap, X, Y, WIDTH, HEIGHT ); 
	else
		DrawBitmap( &dc, &m_IDEditNormalBitmap, X, Y, WIDTH, HEIGHT ); 


	if( m_bSelectPWEdit )
		DrawBitmap( &dc, &m_PWEditSelectBitmap, X_PW, Y_PW, WIDTH_PW, HEIGHT_PW ); 
	else
		DrawBitmap( &dc, &m_PWEditNormalBitmap, X_PW, Y_PW, WIDTH_PW, HEIGHT_PW ); 
}

void ioLoginDlg::FirstConnectServer()
{
	if( !m_pClientSocket )
		m_pClientSocket  = new ioClientSocket( WM_CLIENTSOCKET );

	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	if( pDlg && pDlg->IsGameServerID() )
	{
		char szIP[MAX_PATH]="";
		int iPort = 0;
		Help::ConvertIPPortIDToStr( pDlg->GetGameServerID(), szIP, sizeof( szIP ), iPort );
		SetServerNumber( szIP );
		ConnectServer( szIP, iPort );
	}
	else
	{
		LoadServerIPPortList();

		char szIP[MAX_PATH]="";
		int  iPort = 0;
		GetNextServerIPPort( szIP, sizeof( szIP ), iPort );
		SetServerNumber( szIP );
		ConnectServer( szIP, iPort );
	}
	
	char szTemp[MAX_PATH]="";
	SafeSprintf( szTemp, sizeof( szTemp ), STR(1), m_sServerNumber.c_str() );
	m_sNoticeText = szTemp;
	SetText( (LPCSTR)m_sNoticeText );
}

bool ioLoginDlg::SendServer( const char *szID, const char *szPW )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsUseSocket() )
		return false;

	if( szID == NULL )
		return false;

	if( szPW == NULL )
		return false;

	char szRealID[MAX_PATH*2]="";
	if( pLocal && pLocal->IsDomain() )
	{
		// domain + ID : THPP.tester001
		ioHashString sToken;
		pLocal->GetDomainToken( m_ComboBox.GetCurSel(), sToken );

		if(  sToken.IsEmpty() )
			return false;

		// 로그인 아이디와 private id가 다르기 때문에 구분을 위해서 | 추가
		StringCbPrintf( szRealID, sizeof( szRealID ), "%s.%s|", sToken.c_str(), szID );
	}
	else if( pLocal && pLocal->IsAddLoginDelimiter() )
	{
		bool bAdd = true;
		int iLength = strlen( szID );
		if( iLength > 0 )
		{
			if( szID[iLength-1] == '|' ) // 이미 구분자를 넣었다.
				bAdd = false;
		}

		if( bAdd )
		{
			StringCbCopy( szRealID, sizeof( szRealID ), szID );
			StringCbCat( szRealID, sizeof( szRealID ), "|" ); // 페이스북 숫자 아이디가 실제 privateID와 중복 될 수 있으므로 |구분자을 붙여서 분리
		}
	}
	else
	{
		StringCbCopy( szRealID, sizeof( szRealID ), szID );
	}


	char szRandomKey[MAX_PATH];
	ZeroMemory( szRandomKey, sizeof( szRandomKey ) );
	Help::GetRandomKey( szRandomKey, sizeof( szRandomKey ) );

	char szUserKey[MAX_PATH]="";
	StringCbPrintf( szUserKey, sizeof( szUserKey ), "%s%s", szRealID, szRandomKey );

	char szLoginEncodePW[MAX_PATH*2];
	ZeroMemory( szLoginEncodePW, sizeof( szLoginEncodePW ) );
	StringCbCopy( szLoginEncodePW, sizeof( szLoginEncodePW ), szRandomKey );

	char szEncode[MAX_PATH*2];
	ZeroMemory( szEncode, sizeof( szEncode ) );
	Help::Encode( szPW, strlen( szPW ), szEncode, sizeof( szEncode ), szUserKey, strlen( szUserKey ) );

	StringCbCat( szLoginEncodePW, sizeof( szLoginEncodePW ), szEncode );

	if( strcmp( szLoginEncodePW, "") == 0 )
		return false;
	
	char szMyIP[MAX_PATH]="";
	Help::GetMyIP( szMyIP, sizeof( szMyIP ) );

	SP2Packet kPacket( ASTPK_OTHER_COMPANY_LOGIN_REQUEST );
	kPacket << szRealID;
	if(pLocal->GetType() == ioLocalManager::LCT_PHILIPPINE)
	{
		kPacket << szLoginEncodePW;
	}
	else
	{
		kPacket << szPW;
	}
	kPacket << szMyIP;

	if( pLocal )
	{
		pLocal->FillLoginData( kPacket );
	}

	if( m_pClientSocket )
		m_pClientSocket->SendLoop( kPacket );

	if( pLocal )
#ifndef SRC_TH
		pLocal->SetLoginData( szRealID, szLoginEncodePW );
#else
		pLocal->SetLoginData( szRealID, szPW );
#endif

	m_bLoginOk = false;
	m_eNetworkState = NS_SENT; 

	char szTemp[MAX_PATH]="";
	SafeSprintf( szTemp, sizeof( szTemp ), STR(1), m_sServerNumber.c_str() );
	m_sNoticeText = szTemp;
	SetText( (LPCSTR)m_sNoticeText );
	LOG.PrintTimeAndLog( 0, "Send Login" );
	return true;
}

LONG ioLoginDlg::OnClientSocket( WPARAM wParam, LPARAM lParam )
{
	LONG result=0;
	if(m_pClientSocket)
		result = m_pClientSocket->EvaluateSocket( wParam, lParam );
	return result; 
}

void ioLoginDlg::RecvPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case ASTPK_OTHER_COMPANY_LOGIN_RESULT:
		{
			m_eNetworkState = NS_CONNECTED;
			KillTimer(TIME_ID_LOGIN);

			bool bOk = false;
			rkPacket >> bOk;
			
			CAutoUpgradeDlg* pDlg = NULL;
			if(AfxGetApp())
				pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

			if( bOk )
			{
				ioHashString sPrivateID;
				ioHashString sOTPID;
				rkPacket >> sPrivateID;
				rkPacket >> sOTPID;
				//sOTP를 토큰 sPrivateID를 어카운트ID
				m_bLoginOk = true;

				// save
				Help::CreateDefaultSettingINI();
				ioINILoaderAU kIniLoder("Save\\default\\setting.ini");
				kIniLoder.SetTitle("Setting");
				if( m_IDSaveBtn.IsChecked() )
				{
					CString sID;
					m_IDEdit.GetWindowText( sID );
					if( !sID.IsEmpty() )
						kIniLoder.SaveString( "LoginID", (LPCTSTR) sID );
				}
				else
				{
					ioINILoaderAU kIniLoder("Save\\default\\setting.ini");
					kIniLoder.SetTitle("Setting");
					kIniLoder.SaveString( "LoginID", "" );
				}
				kIniLoder.SaveInt( "Domain", m_ComboBox.GetCurSel() );
				//

				if( pDlg )
					pDlg->ResumeHttpManager();

				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( !sPrivateID.IsEmpty() )
				{
					if( !sOTPID.IsEmpty() )
					{
						if( pLocal )
							pLocal->SetOTPID( sOTPID.c_str() );
					}
				}

				
				if( pDlg && pDlg->IsGameServerID() )
				{
					string sGameServerID;
					pDlg->GetStrGameServerID( sGameServerID );
					if( pLocal )
						pLocal->SetServerID( sGameServerID.c_str() );
				}
				else
				{
					if( COMPARE( m_iServerIPPortIdx , 0, (int)m_vServerIPPort.size() ) )
					{
						if( pLocal )
							pLocal->SetServerID( m_vServerIPPort[m_iServerIPPortIdx].c_str() );
					}
				}
				
				if( m_pClientSocket )
					m_pClientSocket->CloseClientTCP();

#ifdef SRC_TH
				if( pLocal )
				{
					pLocal->SetLoginData( sPrivateID.c_str(), sOTPID.c_str() );
				}
#endif
				//pLocal

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
#ifndef SRC_PH
					sError = "Unknown Error";
#else
					sError = "You have entered wrong information.";
#endif

				SetChildsActive( true );

				if( pDlg )
					pDlg->ShowErrorDlg( sError.c_str() , 227, false ); 

				char szCheck[MAX_PATH]="";
				StringCbCopy( szCheck, sizeof( szCheck ), sError.c_str() );
				strlwr( szCheck );
				
				if( strstr( szCheck, "password" ) != NULL )
				{
					m_PWEdit.SetWindowText( "" );
				}
			}
		}
		return;
	}
}


void ioLoginDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case TIME_ID_LOGIN:
		{
			m_iConnectTimerCnt++;
			m_iTotalConnectTimerCnt++;
			SetText( (LPCSTR)m_sNoticeText );

			if( m_eNetworkState == NS_CONNECTED )
			{
				CString sID;
				CString sPW;
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal && pLocal->IsFacebookLogin() )
				{
					sID = pLocal->GetLoginID();
					sPW = pLocal->GetLoginEncodePW();
				}
				else
				{
					LOG.PrintTimeAndLog(0, "TIME_ID_LOGIN");					
					m_IDEdit.GetWindowText( sID );
					m_PWEdit.GetWindowText( sPW );
				}
				
				SendServer( (LPCTSTR) sID, (LPCTSTR) sPW );
			}

			// 1분간 응답이 없으면 다른 서버로 연결
			if( m_iConnectTimerCnt > MAX_CONNECT_TIME_SEC )
			{
				ioHashString szPreServerNumber = m_sServerNumber;

				CAutoUpgradeDlg* pDlg = NULL;
				if(AfxGetApp())
					pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

				if( pDlg && pDlg->IsGameServerID() )
				{
					char szIP[MAX_PATH]="";
					int iPort = 0;
					Help::ConvertIPPortIDToStr( pDlg->GetGameServerID(), szIP, sizeof( szIP ), iPort );
					SetServerNumber( szIP );
					ConnectServer( szIP, iPort );
				}
				else
				{
					char szIP[MAX_PATH]="";
					int  iPort = 0;
					GetNextServerIPPort( szIP, sizeof( szIP ), iPort );
					SetServerNumber( szIP );
					ConnectServer( szIP, iPort );
				}

				char szTemp[MAX_PATH]="";
				SafeSprintf( szTemp, sizeof( szTemp ), STR(1), szPreServerNumber.c_str(), m_sServerNumber.c_str() );
				m_sNoticeText = szTemp;
				SetText( (LPCSTR)m_sNoticeText );
			}
			
			// 3분간 응답이 없으면 버튼 활성화 
			if( m_iTotalConnectTimerCnt > MAX_LOGIN_TIME_SEC )
			{
				m_iTotalConnectTimerCnt = 0; // text 표시 위해서 
				ioHashString szPreServerNumber = m_sServerNumber;

				CAutoUpgradeDlg* pDlg = NULL;
				if(AfxGetApp())
					pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pDlg && pLocal )
				{
					if( m_eNetworkState == NS_START )
					{
						pDlg->ShowErrorDlg( pLocal->GetConnectFailMent() , 227, false ); 
						LOG.PrintTimeAndLog( 0, "%s - TCP Socket Failed(%d)", __FUNCTION__, WSAGetLastError() );
					}
					else
					{
						pDlg->ShowErrorDlg( pLocal->GetServerFullMent() , 228, false ); 
						LOG.PrintTimeAndLog( 0, "%s - Server Full", __FUNCTION__ );
					}
				}
				
				char szIP[MAX_PATH]="";
				int  iPort = 0;
				GetNextServerIPPort( szIP, sizeof( szIP ), iPort );
				SetServerNumber( szIP );
				ConnectServer( szIP, iPort );

				char szTemp[MAX_PATH]="";
				SafeSprintf( szTemp, sizeof( szTemp ), STR(2), szPreServerNumber.c_str(), m_sServerNumber.c_str() );
				m_sNoticeText = szTemp;
				SetText( (LPCSTR)m_sNoticeText );

				KillTimer(TIME_ID_LOGIN);
				SetChildsActive( true );
				return;
			}
		}
		break;
	}
}

void ioLoginDlg::OnCbnSelchangeDomain()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void ioLoginDlg::SetChildsActive( bool bActive )
{
	if( bActive )
	{
		m_IDSaveBtn.EnableWindow( TRUE );
		m_LoginBtn.EnableWindow( TRUE );
		m_EntryBtn.EnableWindow( TRUE );
		m_FindIDBtn.EnableWindow( TRUE );

#ifdef XTRAP 
		m_FindPWBtn.EnableWindow(FALSE);
#else
		m_FindPWBtn.EnableWindow(FALSE);
#endif

		m_IDEdit.EnableWindow( TRUE );
		m_PWEdit.EnableWindow( TRUE );
		m_XBtn.EnableWindow( TRUE );
		m_ComboBox.EnableWindow( TRUE );
		m_LoginBtn.SetHover( FALSE );
	}
	else
	{
		m_IDSaveBtn.EnableWindow( FALSE );
		m_LoginBtn.EnableWindow( FALSE );
		m_EntryBtn.EnableWindow( FALSE );
		m_FindIDBtn.EnableWindow( FALSE );
		m_FindPWBtn.EnableWindow( FALSE );	
		m_IDEdit.EnableWindow( FALSE );
		m_PWEdit.EnableWindow( FALSE );
		m_XBtn.EnableWindow( FALSE );
		m_ComboBox.EnableWindow( FALSE );
	}
	Invalidate(FALSE);
}

void ioLoginDlg::ConnectServer( const char *szServerIP, int iServerPort )
{
	if( !m_pClientSocket )
		return;
	if( !szServerIP )
		return;
	if( iServerPort == 0 )
		return;

/*
	szServerIP = "172.30.82.188";
	iServerPort = 14009;*/
	m_pClientSocket->CloseClientTCP();
	m_pClientSocket->InitClientTCP( GetSafeHwnd(), WM_CLIENTSOCKET, szServerIP, iServerPort );
	m_eNetworkState = NS_START;
	m_iConnectTimerCnt= 0;
}

void ioLoginDlg::LoadServerIPPortList()
{
	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	enum 
	{ 
		MAX_LOOP                = 4, 
		MAX_CMD                 = 4096, 
		TYPE_GAMESERVER_ID_LIST = 3,
		SERVERID_LIST_SIZE      = 1000, 
	};

	// cmd
	char szCmdLine[MAX_CMD];
	ZeroMemory( szCmdLine, MAX_CMD );
	if(AfxGetApp())
		StringCbCopy( szCmdLine, MAX_CMD, AfxGetApp()->m_lpCmdLine );

	char szGameServerIDData[MAX_CMD]="";
	ZeroMemory( szGameServerIDData, sizeof( szGameServerIDData ) );

	// parsing main token
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok( szCmdLine, CMD_SECTION_TOKEN );
		else
			pPos = strtok( NULL, CMD_SECTION_TOKEN );

		if( pPos == NULL )
			break;

		if( i == TYPE_GAMESERVER_ID_LIST )
		{
			StringCbCopy( szGameServerIDData, sizeof( szGameServerIDData ), pPos );
			break;
		}
	}

	// parsing server id list
	m_vServerIPPort.clear();
	char szServerID[MAX_PATH]="";
	for (int i = 0; i < SERVERID_LIST_SIZE ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok( szGameServerIDData, CMD_EXPAND_SECTION_TOKEN );
		else
			pPos = strtok( NULL, CMD_EXPAND_SECTION_TOKEN );

		if( pPos == NULL )
			break;

		ZeroMemory( szServerID, sizeof( szServerID ) );
		StringCbCopy( szServerID, sizeof( szServerID ), pPos  );
		
		if( strcmp( szServerID, "" ) == 0 )
			continue;
		m_vServerIPPort.push_back( ioHashString( szServerID ) );
	}

	m_iServerIPPortIdx = (rand()+(DWORD)GetCurrentProcessId()) % m_vServerIPPort.size();
}

void ioLoginDlg::GetNextServerIPPort( OUT char *szServerIP, IN int iServerIPSize, OUT int &riServerPort )
{
	// get idx
	m_iServerIPPortIdx++;
	int iSize = m_vServerIPPort.size();
	if( !COMPARE( m_iServerIPPortIdx , 0, iSize ) )
		m_iServerIPPortIdx = 0;

	// str -> ip, port
	__int64 iIPAndPort = _atoi64( m_vServerIPPort[m_iServerIPPortIdx].c_str() );
	
	unsigned char szIPAndPort[MAX_PATH];
	memset( szIPAndPort, 0, MAX_PATH );
	memcpy( szIPAndPort, &iIPAndPort, 8 );

	ZeroMemory( szServerIP, iServerIPSize );
	StringCbPrintf( szServerIP, iServerIPSize, "%d.%d.%d.%d", szIPAndPort[0], szIPAndPort[1], szIPAndPort[2], szIPAndPort[3]);
	
	riServerPort = 0;
	memcpy(&riServerPort, &szIPAndPort[4], 4 );
}

void ioLoginDlg::SetServerNumber( const char *szServerIP )
{
	char szServerNumber[MAX_PATH]="";
	int iServerNumberCnt = 0;
	int iDotCnt = 0;
	int iSize = strlen( szServerIP );
	for (int i = 0; i < iSize ; i++)
	{
		if( szServerIP[i] == '.' )	
		{
			iDotCnt++;
			continue;
		}

		if( iDotCnt == 3 && COMPARE( iServerNumberCnt, 0, MAX_PATH ) )
		{
			szServerNumber[iServerNumberCnt] = szServerIP[i];
			iServerNumberCnt++;
		}
	}

	m_sServerNumber = szServerNumber;
}

void ioLoginDlg::SetText( const char *pText )
{
	CString szTimeCntText;
	szTimeCntText.Format("%s[%d/%d]", pText, m_iTotalConnectTimerCnt, MAX_LOGIN_TIME_SEC );
	SetDlgItemText(IDC_LOGIN_TEXT, szTimeCntText);
	m_Static.Invalidate();
}

void ioLoginDlg::SetTextNetworkConnected( NetworkState eNetworkState )
{
	if( eNetworkState != NS_CONNECTED )
		return;
	char szTemp[MAX_PATH]="";
	SafeSprintf( szTemp, sizeof( szTemp ), STR(1), m_sServerNumber.c_str() );
	m_sNoticeText = szTemp;
	SetText( (LPCSTR)m_sNoticeText );
}

void ioLoginDlg::SetLoginTimer()
{
	SetChildsActive( false );
	SetTimer( TIME_ID_LOGIN, 1000, NULL );
	m_iConnectTimerCnt = 0;
	m_iTotalConnectTimerCnt = 0;
}


BEGIN_EVENTSINK_MAP(ioLoginDlg, CDialogSK)
	ON_EVENT(ioLoginDlg, IDC_EXPLORER_IMG, 259, DocumentCompleteExplorer2, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

void ioLoginDlg::DocumentCompleteExplorer2(LPDISPATCH pDisp, VARIANT* URL)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	HRESULT hr = S_OK; 
	CComPtr<IWebBrowser2> pWB = NULL; 

	hr = pDisp->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&pWB)); 

	if( !pWB )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IDispatch> pDocDisp; 
	hr = pWB->get_Document(&pDocDisp); 

	if( !pDocDisp )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IHTMLDocument2> pDoc; 
	hr = pDocDisp->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void **>(&pDoc)); 

	if( !pDoc )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IHTMLElement> pElement; 
	hr = pDoc->get_body(&pElement); 

	if( !pElement )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IHTMLStyle > pStyle;
	hr = pElement->get_style( &pStyle );

	if( !pStyle )
		return;
	if( FAILED(hr) )
		return;

	pStyle->put_border( CComBSTR(L"none") );

	CComPtr<IHTMLBodyElement> pBodyElement; 
	hr = pElement->QueryInterface(IID_IHTMLBodyElement, (void**)&pBodyElement); 

	if( !pBodyElement )
		return;
	if( FAILED(hr) )
		return;

	pBodyElement->put_scroll(CComBSTR(L"no")); 
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

ioExplorerDlg::ioExplorerDlg(CWnd* pParent /*=NULL*/)
	: CDialogSK(ioExplorerDlg::IDD, pParent)
{
	m_pClientSocket = NULL;
	m_iConnectTimerCnt = 0;
	m_iTotalConnectTimerCnt = 0;
	m_eNetworkState = NS_START;
	m_iServerIPPortIdx      = 0;
	m_iMigration = MIG_NO_DONE;
}

ioExplorerDlg::~ioExplorerDlg()
{
	if( m_pClientSocket && m_pClientSocket->IsConnected() )
		m_pClientSocket->CloseClientTCP();

	SAFEDELETE( m_pClientSocket );

	m_vServerIPPort.clear();
}

void ioExplorerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogSK::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GARENA_COMBO, m_ComboBox);
	DDX_Control(pDX, IDC_GA_EXPLORER, m_webBackImage);
	DDX_Control(pDX, IDC_BTN_OK, m_ConfirmBtn);
	DDX_Control(pDX, IDC_EXPLORER_X, m_XBtn);
}


BEGIN_MESSAGE_MAP(ioExplorerDlg, CDialogSK)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_MESSAGE (WM_MIGRATIONSOCKET  , OnClientSocket  )
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_GARENA_COMBO, OnCbnSelchangeDomain)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_OK, &ioExplorerDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED( IDC_EXPLORER_X, OnExpClickedExitBtn )
END_MESSAGE_MAP()



void ioExplorerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogSK::OnShowWindow(bShow, nStatus);

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
}


LONG ioExplorerDlg::OnClientSocket( WPARAM wParam, LPARAM lParam )
{
	LONG result=0;
	if(m_pClientSocket)
		result = m_pClientSocket->EvaluateSocket( wParam, lParam );
	return result; 
}


void ioExplorerDlg::OnCbnSelchangeDomain()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelect = m_ComboBox.GetCurSel();

	m_stDomain[SITE_ASIASOFT].bSelect = false;
	m_stDomain[SITE_PLAYPARK].bSelect = false;
	m_stDomain[SITE_TCG].bSelect = false;

	m_stDomain[nSelect].bSelect = true;
	m_stDomain[nSelect].szID = "";
	m_webBackImage.Navigate(m_stDomain[nSelect].szHome.GetString(), NULL, NULL, NULL, NULL);
	m_webBackImage.put_Silent(true);
	m_ConfirmBtn.EnableWindow( FALSE );
	m_vecStrID.clear();
}

BOOL ioExplorerDlg::OnInitDialog()
{
	CDialogSK::OnInitDialog();

	// skin
	EnableEasyMove(); 
	SetBitmap( IDB_MIGRATION_BACK ); //세팅용 주석 //Migration 관련 사이즈 조정 기존 사이즈 
	SetStyle(LO_RESIZE);                 // resize dialog to the size of the bitmap
	SetTransparentColor(RGB(0, 255, 0)); // set green as the transparent color
	SetWindowText("LOSTSAGA_MIGRATION");

	m_XBtn.EnableWindow( TRUE );
	m_XBtn.LoadBitmap(IDB_XBTN, TRUE );
	m_XBtn.MoveWindow(810, 11, 23, 23);

	m_ConfirmBtn.EnableWindow( FALSE );
	m_ConfirmBtn.LoadBitmap(IDB_CONFIRMBTN, TRUE );
	m_ConfirmBtn.MoveWindow(670,55,155,24);

	CString szDomain[3][2] = {
		"https://secure3.asiasoft.co.th/apmember/Default.aspx",
		"https://secure3.asiasoft.co.th/apmember/DefaultManage.aspx",
		"http://game.playpark.com/th",
/*		"http://www.playpark.com/th/",*/
		"https://my.playpark.com/",
		"http://profile.playcybergames.com/Profile/Login.aspx",
		"http://profile.playcybergames.com/index.aspx",
	};

	for(int i = 0; i < MAX_SITE; i++)
	{
		m_stDomain[i].bSelect = false;
		m_stDomain[i].bSuccess = false;
		m_stDomain[i].szHome = szDomain[i][0];
		m_stDomain[i].szRet = szDomain[i][1];
		m_stDomain[i].szID = "";
	}

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	if( pLocal && pLocal->IsDomain() )
	{
		m_vecStrID.clear();

		int iMax = pLocal->GetMaxDomain();
		for (int i = 0; i < iMax; i++)
		{
			ioHashString sTitle;
			pLocal->GetDomainTitle( i, sTitle );
			m_ComboBox.AddString( sTitle.c_str() );
		}
		m_ComboBox.MoveWindow(20,60,165,75);
		m_ComboBox.SetCurSel(1);
		
		m_stDomain[SITE_PLAYPARK].bSelect = true;
		m_webBackImage.Navigate(m_stDomain[SITE_PLAYPARK].szHome.GetString(), NULL, NULL, NULL, NULL);
		m_webBackImage.MoveWindow( 20, 90,804,572 );
	}

	if(AfxGetApp())
	{
		CAutoUpgradeDlg* pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
		if(pDlg)
		{
			pDlg->SetStartButton(false);
#ifdef MIGRATION
			if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
				pDlg->SetMigrationButton(false);
#endif
		}
	}
	FirstConnectServer();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void ioExplorerDlg::OnExpClickedExitBtn()
{
	OnExit();
}

void ioExplorerDlg::OnExit()
{
	if(AfxGetApp())
	{
		CAutoUpgradeDlg* pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
		if(pDlg)
		{
			pDlg->SetMigrationButton(true);
			if( m_iMigration == MIG_SUCCESS )
			{
				pDlg->SetMigrationButton(false);
				pDlg->SetStartButton(true);
			}
			else
				MessageBox("Migration work did not fully complete","ERROR", MB_OK);
		}
	}
	
	EndDialog(0);
	
}


void ioExplorerDlg::OnClose()
{
	return; //alt + f4 막음
}


void ioExplorerDlg::OnBnClickedBtnOk()
{
	m_ConfirmBtn.EnableWindow( FALSE );
	
	if(m_stDomain[m_ComboBox.GetCurSel()].bSelect == true && m_stDomain[m_ComboBox.GetCurSel()].bSuccess == true)
	{
		if( m_eNetworkState == NS_CONNECTED )
			SendServer();

		SetLoginTimer();
	}
	else
	{
		MessageBox("Please check login process","Warning",MB_OK);
	}
	m_ConfirmBtn.EnableWindow( TRUE );
}

void ioExplorerDlg::FirstConnectServer()
{
	if( !m_pClientSocket )
		m_pClientSocket  = new ioClientSocket( WM_MIGRATIONSOCKET );

	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	if( pDlg && pDlg->IsGameServerID() )
	{
		char szIP[MAX_PATH]="";
		int iPort = 0;
		Help::ConvertIPPortIDToStr( pDlg->GetGameServerID(), szIP, sizeof( szIP ), iPort );
		SetServerNumber( szIP );
		ConnectServer( szIP, iPort );
	}
	else
	{
		LoadServerIPPortList();

		char szIP[MAX_PATH]="";
		int  iPort = 0;
		GetNextServerIPPort( szIP, sizeof( szIP ), iPort );
		SetServerNumber( szIP );
		ConnectServer( szIP, iPort );
	}
}

bool ioExplorerDlg::SendServer()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsUseSocket() )
		return false;

	int nSite = m_ComboBox.GetCurSel();

	char szRealID[MAX_PATH*2]="";
	if( pLocal && pLocal->IsDomain() )
	{
		// domain + ID : THPP.tester001
		ioHashString sToken;
		pLocal->GetDomainToken( nSite, sToken );

		if(  sToken.IsEmpty() )
			return false;
		if( ! m_vecStrID.empty())
		{
			vector<CString>::iterator iter = m_vecStrID.end();
			--iter;
			CString str = *iter;
			StringCbPrintf( szRealID, sizeof( szRealID ), "%s.%s", sToken.c_str(), str );
			LOG.PrintTimeAndLog( 0, "%s szRealID:%s.%s" , __FUNCTION__, sToken.c_str(), str );
		}
		else
			return false;
	}

	char szMyIP[MAX_PATH]="";
	Help::GetMyIP( szMyIP, sizeof( szMyIP ) );
	
	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	SP2Packet kPacket( ASTPK_OTHER_COMPANY_LOGIN_REQUEST );
	kPacket << pDlg->m_szOGPKey;
	kPacket << szRealID;
//	kPacket << szMyIP;
#ifdef SRC_SEA
	char szExternalIP[MAX_PATH]="";
	Help::GetExternalIP(szExternalIP, sizeof( szExternalIP ));
	kPacket << szExternalIP;
#endif
	if( m_pClientSocket )
		m_pClientSocket->SendLoop( kPacket );

	m_bLoginOk = false;
	m_eNetworkState = NS_SENT; 
	LOG.PrintTimeAndLog( 0, "Send Token and Real ID" );
	return true;
}
/****************************************************************************
	Migration Error Code 
	-1 : 입력 된 계정이 아시아소프트DB에 없음.
	-2 : 유저 인덱스 가져오는 부분 시스템 에러
	-3 : 이미 마이그레이션 한 가레나UID
	-4 : 이미 마이그레이션 한 아시아소프트ID
	-5 : userMemberDB insert 실패
	-6 : userLoginDB insert 실패
	-7 : userAbilityDB insert 실패
	-8 : userAwardDB insert 실패
	-9 : userCharacterDB insert 실패
	-10 : userClassDB insert 실패
	-11 : userEventDB insert 실패
	-12 : userFishDB insert 실패
	-13 : userGameDB insert 실패
	-14 : userHqDB insert 실패
	-15 : userItemDB insert 실패
	-16 : userItemEtcDB insert 실패
	-17 : userItemExtraDB insert 실패
	-18 : userItemMedalDB insert 실패
	-19 : userItemMedalExtendDB insert 실패
	-20 : userPiFeceDB insert 실패
	-21 : userQuestCompleteDB insert 실패
	-22 : userQuestDB insert 실패
	-23 : userRankingDB_award insert 실패
	-24 : userRecordBattleDB insert 실패
	-25 : userRelativeLevelDB insert 실패
	-26 : TBL_migration_mapping_log insert 실패
	-27 : userMoneyDB insert 실패
****************************************************************************/

void ioExplorerDlg::RecvPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case ASTPK_OTHER_COMPANY_LOGIN_RESULT:
		{
			m_eNetworkState = NS_CONNECTED;
			KillTimer(TIME_ID_LOGIN);

			bool bOk = false;
			rkPacket >> bOk;

			CAutoUpgradeDlg* pDlg = NULL;
			if(AfxGetApp())
				pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

			if( bOk )
			{
				ioHashString sPrivateID;
				ioHashString sOTPID;
				rkPacket >> sPrivateID;
				rkPacket >> sOTPID;
				rkPacket >> m_iMigration; //결과

				if( m_iMigration != MIG_SUCCESS ) //error 출력하자
				{
					if( !m_vecStrID.empty() )
					{
						m_ConfirmBtn.EnableWindow(FALSE);
						m_vecStrID.pop_back();
						if( SendServer() )
							return;
					}
					if( m_iMigration == MIG_NO_DBDATA ) //db에 없음
						MessageBox( "The account is not in the Asia-soft database.","DATABASE ERROR", MB_OK );
					else if( m_iMigration == MIG_USR_INDEX_ERR ) //유저 인덱스 오류
						MessageBox( "User Index Error.","SYSTEM ERROR", MB_OK );
					else if( m_iMigration == MIG_GARENA_ALREADY_DONE  || m_iMigration == MIG_ASIA_ALREADY_DONE ) //이미 완료
						MessageBox( "Migration working is already done.","MIGRATION DONE", MB_OK );
					else //0~4번이 아닌 경우
						MessageBox( "Unknown system error.\nPlease try after a while.","SYSTEM ERROR", MB_OK );

					LOG.PrintTimeAndLog( 0, "%s - Migration System Error(%d)", __FUNCTION__, m_iMigration );
					m_ConfirmBtn.EnableWindow(TRUE); //다시 클릭할 수 있게
					return;
				}
				else
				{
					m_vecStrID.clear();
				}

				m_bLoginOk = true;

				if( m_pClientSocket )
					m_pClientSocket->CloseClientTCP();

// 				if(AfxGetApp()) //마이그레이션 알파전용
// 				{
// 					CAutoUpgradeDlg* pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
// 					if(pDlg)
// 					{
// 						pDlg->SetStartButton(true);
// 						MSG msg;
// 						msg.message = WM_KEYDOWN; 
// 						msg.wParam = VK_SPACE;
// 						pDlg->PreTranslateMessage( &msg ); //migration 성공시 자동 실행
// 					}
// 				}
				MessageBox( "Thank you. :)","MIGRATION SUCCESS", MB_OK );
				LOG.PrintTimeAndLog( 0, "%s - MIGRATION SUCCESS(%d)", __FUNCTION__, m_iMigration );

				OnExit();
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
	
				MessageBox( "Sorry. Account information is empty.","SYSTEM ERROR", MB_OK );
				LOG.PrintTimeAndLog( 0, "%s - Migration System Error(%s)", __FUNCTION__, sError.c_str() );
			}
		}
		return;
	}
}


void ioExplorerDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case TIME_ID_LOGIN:
		{
			m_iConnectTimerCnt++;
			m_iTotalConnectTimerCnt++;

			if( m_eNetworkState == NS_CONNECTED )
			{
				SendServer();
			}

			// 1분간 응답이 없으면 다른 서버로 연결
// 			if( m_iConnectTimerCnt > MAX_CONNECT_TIME_SEC )
// 			{
// 				ioHashString szPreServerNumber = m_sServerNumber;
// 
// 				CAutoUpgradeDlg* pDlg = NULL;
// 				if(AfxGetApp())
// 					pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
// 
// 				if( pDlg && pDlg->IsGameServerID() )
// 				{
// 					char szIP[MAX_PATH]="";
// 					int iPort = 0;
// 					Help::ConvertIPPortIDToStr( pDlg->GetGameServerID(), szIP, sizeof( szIP ), iPort );
// 					SetServerNumber( szIP );
// 					ConnectServer( szIP, iPort );
// 				}
// 				else
// 				{
// 					char szIP[MAX_PATH]="";
// 					int  iPort = 0;
// 					GetNextServerIPPort( szIP, sizeof( szIP ), iPort );
// 					SetServerNumber( szIP );
// 					ConnectServer( szIP, iPort );
// 				}
// 			}

			if( m_iTotalConnectTimerCnt > (MAX_CONNECT_TIME_SEC * 2) )  //3분 -> 2분으로 변경
			{
				m_iTotalConnectTimerCnt = 0; // text 표시 위해서 
				ioHashString szPreServerNumber = m_sServerNumber;

				CAutoUpgradeDlg* pDlg = NULL;
				if(AfxGetApp())
					pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pDlg && pLocal )
				{
					if( m_eNetworkState == NS_START )
					{
						pDlg->ShowErrorDlg( pLocal->GetConnectFailMent() , 227, false ); 
						LOG.PrintTimeAndLog( 0, "%s - TCP Socket Failed(%d)", __FUNCTION__, WSAGetLastError() );
					}
					else
					{
						pDlg->ShowErrorDlg( pLocal->GetServerFullMent() , 228, false ); 
						LOG.PrintTimeAndLog( 0, "%s - Server Full", __FUNCTION__ );
					}
				}

				char szIP[MAX_PATH]="";
				int  iPort = 0;
				GetNextServerIPPort( szIP, sizeof( szIP ), iPort );
				SetServerNumber( szIP );
				ConnectServer( szIP, iPort );
				KillTimer(TIME_ID_LOGIN);
				OnExit();
				return;
			}
		}
		break;
	}
}


void ioExplorerDlg::ConnectServer( const char *szServerIP, int iServerPort )
{
	if( !m_pClientSocket )
		return;
#ifdef MIGRATION
	//태국 전용 마이그레이션 서버
	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND)
	{
 		//m_pClientSocket->InitClientTCP( GetSafeHwnd(), WM_MIGRATIONSOCKET, "172.20.20.59", 14008 );// 알파용
 		m_pClientSocket->InitClientTCP( GetSafeHwnd(), WM_MIGRATIONSOCKET, "111.223.43.1", 14009 ); //베타용
	}
#else
	if( !szServerIP )
		return;
	if( iServerPort == 0 )
		return;
	m_pClientSocket->CloseClientTCP();
	m_pClientSocket->InitClientTCP( GetSafeHwnd(), WM_MIGRATIONSOCKET, szServerIP, iServerPort );
#endif
	m_eNetworkState = NS_START;
	m_iConnectTimerCnt= 0;
}

void ioExplorerDlg::LoadServerIPPortList()
{
	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	enum 
	{ 
		MAX_LOOP                = 4, 
		MAX_CMD                 = 4096, 
		TYPE_GAMESERVER_ID_LIST = 3,
		SERVERID_LIST_SIZE      = 1000, 
	};

	// cmd
	char szCmdLine[MAX_CMD];
	ZeroMemory( szCmdLine, MAX_CMD );
	if(AfxGetApp())
		StringCbCopy( szCmdLine, MAX_CMD, AfxGetApp()->m_lpCmdLine );

	char szGameServerIDData[MAX_CMD]="";
	ZeroMemory( szGameServerIDData, sizeof( szGameServerIDData ) );

	// parsing main token
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok( szCmdLine, CMD_SECTION_TOKEN );
		else
			pPos = strtok( NULL, CMD_SECTION_TOKEN );

		if( pPos == NULL )
			break;

		if( i == TYPE_GAMESERVER_ID_LIST )
		{
			StringCbCopy( szGameServerIDData, sizeof( szGameServerIDData ), pPos );
			break;
		}
	}

	// parsing server id list
	m_vServerIPPort.clear();
	char szServerID[MAX_PATH]="";
	for (int i = 0; i < SERVERID_LIST_SIZE ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok( szGameServerIDData, CMD_EXPAND_SECTION_TOKEN );
		else
			pPos = strtok( NULL, CMD_EXPAND_SECTION_TOKEN );

		if( pPos == NULL )
			break;

		ZeroMemory( szServerID, sizeof( szServerID ) );
		StringCbCopy( szServerID, sizeof( szServerID ), pPos  );

		if( strcmp( szServerID, "" ) == 0 )
			continue;
		m_vServerIPPort.push_back( ioHashString( szServerID ) );
	}

	m_iServerIPPortIdx = (rand()+(DWORD)GetCurrentProcessId()) % m_vServerIPPort.size();
}

void ioExplorerDlg::GetNextServerIPPort( OUT char *szServerIP, IN int iServerIPSize, OUT int &riServerPort )
{
	// get idx
	m_iServerIPPortIdx++;
	int iSize = m_vServerIPPort.size();
	if( !COMPARE( m_iServerIPPortIdx , 0, iSize ) )
		m_iServerIPPortIdx = 0;

	// str -> ip, port
	__int64 iIPAndPort = _atoi64( m_vServerIPPort[m_iServerIPPortIdx].c_str() );

	unsigned char szIPAndPort[MAX_PATH];
	memset( szIPAndPort, 0, MAX_PATH );
	memcpy( szIPAndPort, &iIPAndPort, 8 );

	ZeroMemory( szServerIP, iServerIPSize );
	StringCbPrintf( szServerIP, iServerIPSize, "%d.%d.%d.%d", szIPAndPort[0], szIPAndPort[1], szIPAndPort[2], szIPAndPort[3]);

	riServerPort = 0;
	memcpy(&riServerPort, &szIPAndPort[4], 4 );
}

void ioExplorerDlg::SetServerNumber( const char *szServerIP )
{
	char szServerNumber[MAX_PATH]="";
	int iServerNumberCnt = 0;
	int iDotCnt = 0;
	int iSize = strlen( szServerIP );
	for (int i = 0; i < iSize ; i++)
	{
		if( szServerIP[i] == '.' )	
		{
			iDotCnt++;
			continue;
		}

		if( iDotCnt == 3 && COMPARE( iServerNumberCnt, 0, MAX_PATH ) )
		{
			szServerNumber[iServerNumberCnt] = szServerIP[i];
			iServerNumberCnt++;
		}
	}

	m_sServerNumber = szServerNumber;
}


void ioExplorerDlg::SetLoginTimer()
{
	SetTimer( TIME_ID_LOGIN, 1000, NULL );
	m_iConnectTimerCnt = 0;
	m_iTotalConnectTimerCnt = 0;
}



BEGIN_EVENTSINK_MAP(ioExplorerDlg, CDialogSK)
	ON_EVENT(ioExplorerDlg, IDC_GA_EXPLORER, 259, DocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(ioExplorerDlg, IDC_GA_EXPLORER, 250, BeforeNavigate2Explorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

void ioExplorerDlg::DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	int nRet = m_ComboBox.GetCurSel();

	switch (nRet)
	{
	case SITE_ASIASOFT:
	case SITE_PLAYPARK:
	case SITE_TCG:
		{
			VariantChangeType(URL, URL, 0, VT_BSTR);
			CString szURL = URL->bstrVal;  // To CString
			CString szTmp = szURL.Left(m_stDomain[nRet].szRet.GetLength());

			if( m_stDomain[nRet].szRet.CompareNoCase(szTmp) == 0)
			{
				m_stDomain[SITE_ASIASOFT].bSuccess = false;
				m_stDomain[SITE_PLAYPARK].bSuccess = false;
				m_stDomain[SITE_TCG].bSuccess = false;
				m_stDomain[nRet].bSuccess = true;
				m_ConfirmBtn.EnableWindow( TRUE );
			}
		}
		break;
	}
	

	HRESULT hr = S_OK; 
	CComPtr<IWebBrowser2> pWB = NULL; 

	hr = pDisp->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&pWB)); 

	if( !pWB )
		return;
	if( FAILED(hr) )
		return;

	CString szURL = URL->bstrVal;  // To CString
	CString szTmp = "https://secure3.asiasoft.co.th/slide/playpark/center/index.html";

	if( szURL.CompareNoCase(szTmp) == 0)
		m_webBackImage.put_Silent(false);

	CComPtr<IDispatch> pDocDisp; 
	hr = pWB->get_Document(&pDocDisp); 

	if( !pDocDisp )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IHTMLDocument2> pDoc; 
	hr = pDocDisp->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void **>(&pDoc)); 

	if( !pDoc )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IHTMLElement> pElement; 
	hr = pDoc->get_body(&pElement); 

	if( !pElement )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IHTMLStyle > pStyle;
	hr = pElement->get_style( &pStyle );

	if( !pStyle )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IHTMLBodyElement> pBodyElement; 
	hr = pElement->QueryInterface(IID_IHTMLBodyElement, (void**)&pBodyElement); 

	if( !pBodyElement )
		return;
	if( FAILED(hr) )
		return;
}


void ioExplorerDlg::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int nRet = m_ComboBox.GetCurSel();

	switch (nRet)
	{
	case SITE_ASIASOFT:
		GetID_AsiaSoft( pDisp, m_stDomain[nRet].szID );
		break;

	case SITE_PLAYPARK:
		GetID_PlayPark( pDisp, m_stDomain[nRet].szID );
		break;

	case SITE_TCG:
		GetID_PlayCyberGames( pDisp, m_stDomain[nRet].szID );
		break;
	}
}


void ioExplorerDlg::GetID_AsiaSoft(LPDISPATCH pDisp, CString &szID)
{
	// Asia soft 공식 홈페이지 ID
	// https://secure3.asiasoft.co.th/apmember/Default.aspx
	HRESULT hr = S_OK; 
	CComPtr<IWebBrowser2> pWB = NULL; 

	hr = pDisp->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&pWB)); 

	if( FAILED(hr) || !pWB )
		return;

	CComPtr<IDispatch> spIDispatchDoc2 = NULL;
	hr = pWB->get_Document( &spIDispatchDoc2 );

	if( FAILED(hr) || !spIDispatchDoc2 )
		return;

	CComPtr<IHTMLDocument2> spIHTMLDocument2_1 = NULL;
	hr = spIDispatchDoc2.QueryInterface<IHTMLDocument2>( &spIHTMLDocument2_1 );

	if( FAILED(hr) || !spIHTMLDocument2_1 )
		return;

	CComPtr<IHTMLElement> spIHTMLElement = NULL;
	BSTR bstrID = L"pgMainAPManage";
	hr = spIHTMLDocument2_1->get_activeElement( &spIHTMLElement );

	if( FAILED(hr) || !spIHTMLElement )
		return;

	CComPtr<IDispatch> pDispatchDoc2 = NULL;
	hr = spIHTMLElement->get_document( &pDispatchDoc2 );

	if( FAILED(hr) || !pDispatchDoc2 )
		return;

	CComPtr<IHTMLDocument2> spIHTMLDocument2_2 = NULL;
	hr = pDispatchDoc2.QueryInterface<IHTMLDocument2>( &spIHTMLDocument2_2 );

	if( FAILED(hr) || !spIHTMLDocument2_2 )
		return;

	CComPtr<IHTMLElementCollection> spIHTMLElementCollection = NULL;
	hr = spIHTMLDocument2_2->get_forms( &spIHTMLElementCollection );

	if( FAILED(hr) || !spIHTMLElementCollection )
		return;

	CComPtr<IDispatch> pElemDisp = NULL;
	CComPtr<IDispatch> pItem = NULL;
	hr = spIHTMLElementCollection->item( CComVariant(0), CComVariant(0), &pElemDisp );

	if( FAILED(hr) || !pElemDisp )
		return;

	CComPtr<IHTMLFormElement> spIHTMLFormElement = NULL;
	hr = pElemDisp->QueryInterface( IID_IHTMLFormElement, (void**)&spIHTMLFormElement );

	if( FAILED(hr) || !spIHTMLFormElement )
		return;

	CComPtr<IHTMLInputElement> spIHTMLInputElement = NULL;
	hr = spIHTMLFormElement->item( CComVariant(0), CComVariant(0), &pItem );

	if( FAILED(hr) || !pItem )
		return;

	pItem->QueryInterface( IID_IHTMLInputElement, (void**)&spIHTMLInputElement );

	if( FAILED(hr) || !spIHTMLInputElement )
		return;

	BSTR bstr;

	spIHTMLInputElement->get_value( &bstr );
	szID = (LPCWSTR)bstr;

	if( szID.GetLength() < MAX_ID_LENGTH && szID.GetLength() != 0 )
	{
		m_vecStrID.push_back(szID);
		LOG.PrintTimeAndLog( 0, "%s szID:%s" , __FUNCTION__, szID );
	}
}


void ioExplorerDlg::GetID_PlayPark(LPDISPATCH pDisp, CString &szID)
{
	// PlayPark ID
	// https://authen.playpark.net/authen_gateway/inpp/login_default.aspx
	HRESULT hr = S_OK; 
	CComPtr<IWebBrowser2> pWB = NULL; 

	hr = pDisp->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&pWB)); 

	if( FAILED(hr) || !pWB )
		return;

	CComPtr<IDispatch> spIDispatchDoc = NULL;
	hr = pWB->get_Document( &spIDispatchDoc );

	if( FAILED(hr) || !spIDispatchDoc )
		return;

	CComPtr<IHTMLDocument2> spIHTMLDocument2 = NULL;
	hr = spIDispatchDoc.QueryInterface<IHTMLDocument2>( &spIHTMLDocument2 );

	if( FAILED(hr) || !spIHTMLDocument2 )
		return;

	CComPtr<IHTMLElementCollection> spIHTMLElementCollection = NULL;
	hr = spIHTMLDocument2->get_forms( &spIHTMLElementCollection );

	if( FAILED(hr) || !spIHTMLElementCollection )
		return;

	CComPtr<IDispatch> pElemDisp = NULL;
	CComPtr<IDispatch> pItem = NULL;
	hr = spIHTMLElementCollection->item( CComVariant(0), CComVariant(0), &pElemDisp );

	if( FAILED(hr) || !pElemDisp )
		return;

	CComPtr<IHTMLFormElement> spIHTMLFormElement = NULL;
	hr = pElemDisp->QueryInterface( IID_IHTMLFormElement, (void**)&spIHTMLFormElement );

	if( FAILED(hr) || !spIHTMLFormElement )
		return;

	CComPtr<IHTMLInputElement> spIHTMLInputElement = NULL;
	hr = spIHTMLFormElement->item( CComVariant(4), CComVariant(4), &pItem );

	if( FAILED(hr) || !pItem )
		return;

	pItem->QueryInterface( IID_IHTMLInputElement, (void**)&spIHTMLInputElement );

	if( FAILED(hr) || !spIHTMLInputElement )
		return;

	BSTR bstr;
	spIHTMLInputElement->get_value( &bstr );
	szID = (LPCWSTR)bstr;
	if( szID.GetLength() < MAX_ID_LENGTH )
	{
		LOG.PrintTimeAndLog( 0, "%s szID:%s" , __FUNCTION__, szID );
		m_vecStrID.push_back(szID);
	}
	//if( szID.GetLength() > MAX_ID_LENGTH )
	{
		pItem = NULL;
		spIHTMLFormElement = NULL;
		spIHTMLInputElement = NULL;
		hr = pElemDisp->QueryInterface( IID_IHTMLFormElement, (void**)&spIHTMLFormElement );
		hr = spIHTMLFormElement->item( CComVariant(5), CComVariant(5), &pItem );

		if( FAILED(hr) || !pItem )
			return;

		pItem->QueryInterface( IID_IHTMLInputElement, (void**)&spIHTMLInputElement );

		if( FAILED(hr) || !spIHTMLInputElement )
			return;

		BSTR bstr;
		spIHTMLInputElement->get_value( &bstr );
		szID = (LPCWSTR)bstr;

		if( szID.GetLength() < MAX_ID_LENGTH && szID.GetLength() != 0 )
		{
			m_vecStrID.push_back(szID);
			LOG.PrintTimeAndLog( 0, "%s szID:%s" , __FUNCTION__, szID );
		}
	}
}



void ioExplorerDlg::GetID_PlayCyberGames(LPDISPATCH pDisp, CString &szID)
{
	// Play Cyber Games (TCG) ID
	// http://profile.playcybergames.com/Profile/Login.aspx
	HRESULT hr = S_OK; 
	CComPtr<IWebBrowser2> pWB = NULL; 

	hr = pDisp->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&pWB)); 

	if( FAILED(hr) || !pWB )
		return;

	CComPtr<IDispatch> spIDispatchDoc = NULL;
	hr = pWB->get_Document( &spIDispatchDoc );

	if( FAILED(hr) || !spIDispatchDoc )
		return;

	CComPtr<IHTMLDocument2> spIHTMLDocument2 = NULL;
	hr = spIDispatchDoc.QueryInterface<IHTMLDocument2>( &spIHTMLDocument2 );

	if( FAILED(hr) || !spIHTMLDocument2 )
		return;

	CComPtr<IHTMLElementCollection> spIHTMLElementCollection = NULL;
	hr = spIHTMLDocument2->get_forms( &spIHTMLElementCollection );

	if( FAILED(hr) || !spIHTMLElementCollection )
		return;

	CComPtr<IDispatch> pElemDisp = NULL;
	CComPtr<IDispatch> pItem = NULL;
	hr = spIHTMLElementCollection->item( CComVariant(0), CComVariant(0), &pElemDisp );

	if( FAILED(hr) || !pElemDisp )
		return;

	CComPtr<IHTMLFormElement> spIHTMLFormElement = NULL;
	hr = pElemDisp->QueryInterface( IID_IHTMLFormElement, (void**)&spIHTMLFormElement );

	if( FAILED(hr) || !spIHTMLFormElement )
		return;

	CComPtr<IHTMLInputElement> spIHTMLInputElement = NULL;
	hr = spIHTMLFormElement->item( CComVariant(4), CComVariant(4), &pItem );

	if( FAILED(hr) || !pItem )
		return;

	pItem->QueryInterface( IID_IHTMLInputElement, (void**)&spIHTMLInputElement );

	if( FAILED(hr) || !spIHTMLInputElement )
		return;

	BSTR bstr;
	spIHTMLInputElement->get_value( &bstr );
	szID = (LPCWSTR)bstr;

	if( szID.GetLength() < MAX_ID_LENGTH && szID.GetLength() != 0 )
	{
		m_vecStrID.push_back(szID);
		LOG.PrintTimeAndLog( 0, "%s szID:%s" , __FUNCTION__, szID );
	}
}


