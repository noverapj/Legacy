// NagleRefCountDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "NagleRefCountDlg.h"
#include "afxdialogex.h"


// CNagleRefCountDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNagleRefCountDlg, CDialogEx)

CNagleRefCountDlg::CNagleRefCountDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNagleRefCountDlg::IDD, pParent)
	, m_refCount(0)
{

}

CNagleRefCountDlg::~CNagleRefCountDlg()
{
}

void CNagleRefCountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAGLECOUNT, m_refCount);
}


BEGIN_MESSAGE_MAP(CNagleRefCountDlg, CDialogEx)
END_MESSAGE_MAP()


// CNagleRefCountDlg 메시지 처리기입니다.
