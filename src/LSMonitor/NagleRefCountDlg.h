#pragma once


// CNagleRefCountDlg 대화 상자입니다.
#include "resource.h"

class CNagleRefCountDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNagleRefCountDlg)

public:
	CNagleRefCountDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNagleRefCountDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NAGLE_REFCOUNT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	uint32 m_refCount;

	uint32 GetRefCount(){ return m_refCount; }
};
