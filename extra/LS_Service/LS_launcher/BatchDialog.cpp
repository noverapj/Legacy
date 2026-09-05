// BatchDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_launcher.h"
#include "BatchDialog.h"
#include "afxdialogex.h"


// CBatchDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CBatchDialog, CDialogEx)

CBatchDialog::CBatchDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBatchDialog::IDD, pParent)
{

}

CBatchDialog::~CBatchDialog()
{
}

void CBatchDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BATCH, m_batchList);
	DDX_Control(pDX, IDC_STATIC_BATCHRESULT, m_batchResult);
}


BEGIN_MESSAGE_MAP(CBatchDialog, CDialogEx)
END_MESSAGE_MAP()


// CBatchDialog 메시지 처리기입니다.


BOOL CBatchDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_batchList.DeleteAllItems();
	m_batchList.InitProgressColumn(1);
	m_batchList.InsertColumn(0, _T("Command"), LVCFMT_LEFT, 200);
	m_batchList.InsertColumn(1, _T("Progress"), LVCFMT_LEFT, 200);
	m_batchList.InsertColumn(2, _T("Result"), LVCFMT_LEFT, 100);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CBatchDialog::BeginProgress(const TCHAR* title, const int progress, const TCHAR* result, int& index)
{
	index = m_batchList.GetItemCount();

	CString ratio;
	ratio.Format(_T("%d"), progress);

	m_batchList.InsertItem(index, _T(""), 0);
	m_batchList.SetItemText(index, 0, title);
	m_batchList.SetItemText(index, 1, ratio);
	m_batchList.SetItemText(index, 2, result);

	TRACE(_T("insert :: [%d] - %d\n"), index, progress);
}

void CBatchDialog::DoingProgress(const int index, const int progress)
{
	TRACE(_T("set [%d] - %d\n"), index, progress);

	CString ratio;
	ratio.Format(_T("%d"), progress);

	m_batchList.SetItemText(index, 1, ratio);
}

void CBatchDialog::EndProgress(const int index, const int progress, const TCHAR* result)
{
	CString ratio;
	ratio.Format(_T("%d"), progress);

	m_batchList.SetItemText(index, 1, ratio);
	m_batchList.SetItemText(index, 2, result);
}

void CBatchDialog::UpdateBatchResult(CString result)
{
	//_tprintf(result.GetBuffer());


	m_batchResult.SetWindowText( result );
}
