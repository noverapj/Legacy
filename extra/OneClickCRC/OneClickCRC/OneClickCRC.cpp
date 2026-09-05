
// OneClickCRC.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "OneClickCRC.h"
//#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//추가한 내용입니다.
#include "Manager/CRC/CRCManager.h"
#include "Manager/FTP/FTPManager.h"
#include "OneCRCDlg.h"

// COneClickCRCApp

BEGIN_MESSAGE_MAP(COneClickCRCApp, CWinAppEx)
END_MESSAGE_MAP()


// COneClickCRCApp 생성

COneClickCRCApp::COneClickCRCApp()
	: m_pCRCMgr(NULL), m_pFTPMgr(NULL)
{
	//m_bHiColorIcons = TRUE;

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("OneClickCRC.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 COneClickCRCApp 개체입니다.

COneClickCRCApp theApp;


// COneClickCRCApp 초기화

BOOL COneClickCRCApp::InitInstance()
{
	CWinAppEx::InitInstance();


//	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.	
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
 	// 적절한 내용으로 수정해야 합니다.
// 	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
// 
// 
// 	InitContextMenuManager();
// 
// 	InitKeyboardManager();
// 
// 	InitTooltipManager();
// 	CMFCToolTipInfo ttParams;
// 	ttParams.m_bVislManagerTheme = TRUE;
// 	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
// 		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 주 창을 만들기 위해 이 코드에서는 새 프레임 창 개체를
	// 만든 다음 이를 응용 프로그램의 주 창 개체로 설정합니다.
// 	CMainFrame* pFrame = new CMainFrame;
// 	if (!pFrame)
// 		return FALSE;
// 	m_pMainWnd = pFrame;
// 	// 프레임을 만들어 리소스와 함께 로드합니다.
// 	pFrame->LoadFrame(IDR_MAINFRAME,
// 		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
// 		NULL);






	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
// 	pFrame->ShowWindow(SW_SHOW);
// 	pFrame->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.

	//다이얼로그 설정 부분입니다.
	OneCRCDlg	oneDlg;
	m_pMainWnd	= &oneDlg;
	m_pCRCMgr	= CCRCManager::GetInst();
	m_pFTPMgr	= CFTPManager::GetInst();
	oneDlg.SetCRCMgr(m_pCRCMgr);
	oneDlg.SetFTPMgr(m_pFTPMgr);
	int nResponse = oneDlg.DoModal();

	return TRUE;
}

int COneClickCRCApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	m_pCRCMgr->ReleaseInst();
	m_pFTPMgr->ReleaseInst();
	return CWinAppEx::ExitInstance();
}

// COneClickCRCApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.

// COneClickCRCApp 메시지 처리기



