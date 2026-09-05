// WaitingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "NetWork/ioClientSocket.h"
#include "WaitingDlg.h"
#include "afxdialogex.h"


// CWaitingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWaitingDlg, CDialog)

CWaitingDlg::CWaitingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitingDlg::IDD, pParent)
{

}

CWaitingDlg::~CWaitingDlg()
{
}

void CWaitingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWaitingDlg, CDialog)
	ON_MESSAGE(WM_LOGINSOCKET, OnReceive)
END_MESSAGE_MAP()


// CWaitingDlg 메시지 처리기입니다.

LONG CWaitingDlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	LONG result = Evaluate(wParam, lParam);
	return result; 
}

HWND CWaitingDlg::GetHandle()
{
	return GetSafeHwnd();
}