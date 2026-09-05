
// SVNPosterDlg.h : 헤더 파일
//

#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "afxhtml.h"
#include "SVNParser.h"

// CSVNPosterDlg 대화 상자
class CSVNPosterDlg : public CDialogEx
{
// 생성입니다.
public:
	CSVNPosterDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SVNPOSTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:
	void MakeTemporaries();
	BOOL LoadTemporaries();
	BOOL ParseTemporaries(int& revision);
	void Print();
	void CopyToClipBoard();

	void SetRegistry(const TCHAR* option, CString& value);
	BOOL GetRegistry(const TCHAR* option, CString& value);

	void Progress(const int progress)
	{
		m_progress = progress;
		m_progressBar.SetPos(m_progress);
	}
	int GetProgress() { return m_progress; }

protected:
	CString m_fileName;
	CString m_repository;
	CSVNParser m_SVN;
	CString m_output;

	int m_progress;

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CEdit m_rivision;
	CButton m_zone_alpha;
	CButton m_zone_dev;
	CButton m_zone_qa;
	CButton m_zone_beta;
	CEdit m_log;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSVNRepository();
	CProgressCtrl m_progressBar;
	afx_msg void OnBnClickedButtonCopyToClipboard();
};
