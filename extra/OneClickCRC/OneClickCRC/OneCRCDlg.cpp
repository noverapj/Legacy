// OneCRCDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OneClickCRC.h"
#include "OneCRCDlg.h"
#include "afxdialogex.h"


// OneCRCDlg 대화 상자입니다.
//윈 7스타일
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

//매니저
#include "Manager/CRC/CRCManager.h"
#include "Manager/FTP/FTPManager.h"

//글로벌 핸들
HWND	g_hWnd;


IMPLEMENT_DYNAMIC(OneCRCDlg, CDialog)

OneCRCDlg::OneCRCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OneCRCDlg::IDD, pParent)
/*	, m_strLog(_T(""))*/
{
		g_hWnd = m_hWnd;
}

OneCRCDlg::~OneCRCDlg()
{
}

void OneCRCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	InitializationDlg();
/*	DDX_Text(pDX, IDC_EDIT1, m_strLog);*/
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

//다이얼로그 초기화 함수입니다.
void OneCRCDlg::InitializationDlg()
{
}


BEGIN_MESSAGE_MAP(OneCRCDlg, CDialog)
	ON_BN_CLICKED(IDOK, &OneCRCDlg::OnBnClickedOk)
	ON_BN_CLICKED(ID_CONNECT_BT, &OneCRCDlg::OnBnClickedConnectBt)
END_MESSAGE_MAP()


// OneCRCDlg 메시지 처리기입니다.


void OneCRCDlg::OnBnClickedOk()
{
	CString strLog;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
 	m_pCRCMgr->SetDlg(this);
 	m_pCRCMgr->RunProcess();
	strLog = L"FTP전송을 시작합니다.";
	PrintWorkLog(&strLog);
	m_pFTPMgr->SetDlg(this);
	m_pFTPMgr->SetUseData(m_pvecUploadListPath, m_pstrDownPath);
	m_pFTPMgr->SetUploadList(m_pvecUploadListPath);
	m_pFTPMgr->ConnectProcess(m_pstrIP, m_pstrID, m_pstrPW);
	strLog = L"FTP전송을 끝마쳤습니다.";
	PrintWorkLog(&strLog);

	CCRCManager::GetInst()->OtherServerProcess();
	MessageBox(L"모든 작업을 끝마쳤습니다.",MB_OK);
	AfxGetMainWnd()->PostMessageW(WM_CLOSE); //종료
}

void OneCRCDlg::OnBnClickedConnectBt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pFTPMgr->SetDlg(this);
	m_pFTPMgr->SetUseData(m_pvecUploadListPath, m_pstrDownPath);
	m_pFTPMgr->ConnectProcess(m_pstrIP, m_pstrID, m_pstrPW);
}

char* OneCRCDlg::UTF8toANSI(const char* pTxt )
{
	BSTR	bstrWide;
	char*	pszAnsi;
	int		nLength;

	nLength = MultiByteToWideChar(CP_UTF8, 0, pTxt, lstrlen((LPCWSTR)pTxt)+1, NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, nLength);

	MultiByteToWideChar(CP_UTF8, 0, pTxt, lstrlen((LPCWSTR)pTxt)+1, bstrWide,nLength);
	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	pszAnsi = new char[nLength];
	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
	SysFreeString(bstrWide);
	return pszAnsi;
}

void OneCRCDlg::ChangeFontString( TCHAR* pSour, char* pDest )
{
#if defined(UNICODE) | defined(_UNICODE)
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
		pDest, strlen(pDest), pSour, strlen(pDest));
#else
	strcpy(pSour, pDest);
#endif
}

void OneCRCDlg::SetConnectInfo( CString* strip, CString* strid, CString* strpw )
{
	m_pstrIP	= strip;
	m_pstrID	= strid;
	m_pstrPW	= strpw;
}

void OneCRCDlg::PrintWorkLog( CString* str )
{
	CString strDiv;
	int iMax = 60;
	if((*str).GetLength() > iMax)
	{
		for(int i = 0 ; i < (*str).GetLength(); ++i)
		{
			strDiv += (*str)[i];
			if(iMax < i)
			{
				m_ListBox.AddString(strDiv);
				strDiv = L"";
				iMax += iMax; //60개씩 자른다.
			}
		}
		m_ListBox.AddString(strDiv);
	}
	else
		m_ListBox.AddString(*str);
	m_ListBox.RedrawWindow();
}
