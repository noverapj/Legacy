// GetTextDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PatchManager.h"
#include "GetTextDlg.h"
#include "afxdialogex.h"


// CGetTextDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGetTextDlg, CDialogEx)

CGetTextDlg::CGetTextDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGetTextDlg::IDD, pParent)
	, m_strString(_T("")), m_bStartSet(false)
{
}

CGetTextDlg::~CGetTextDlg()
{
}

void CGetTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FIND_EDIT1, m_strString);
	DDX_Control(pDX, IDC_FIND_EDIT1, m_strCtr);
}


BEGIN_MESSAGE_MAP(CGetTextDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGetTextDlg::OnBnClickedOk)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CGetTextDlg 메시지 처리기입니다.


void CGetTextDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	*m_pStr = m_strString;
	UpdateData(FALSE);
	CDialogEx::OnOK();
}

void CGetTextDlg::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
	if(!m_bStartSet)
	{
		m_strCtr.SetSel(0,0);
		m_bStartSet = true;
		keybd_event(VK_LEFT,0,0,0); //키보드 자동입력
	}
}