#pragma once

#include "resource.h"
#include "afxcmn.h"
#include "Batch/ListCtrlEx.h"
#include "afxwin.h"

// CBatchDialog 대화 상자입니다.

class CBatchDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CBatchDialog)

public:
	CBatchDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CBatchDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_BATCH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	void BeginProgress(const TCHAR* title, const int progress, const TCHAR* result, int& index);
	void DoingProgress(const int index, const int progress);
	void EndProgress(const int index, const int progress, const TCHAR* result);

	void UpdateBatchResult(CString result);

public:
	CListCtrlEx m_batchList;

	virtual BOOL OnInitDialog();
	CStatic m_batchResult;
};
