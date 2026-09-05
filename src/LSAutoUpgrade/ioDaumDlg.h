#pragma once
#include "afxwin.h"
#include "skin\urllinkbutton.h"


// ioDaumDlg 대화 상자입니다.

// FROM DAUM
#define RELEASE_VERSION	0.9
#define MAIN			_T("Software\\Microsoft\\Internet Explorer\\Main")
#define TAB_BROSWING	_T("Software\\Microsoft\\Internet Explorer\\TabbedBrowsing")
#define VERSION			_T("SOFTWARE\\Microsoft\\Internet Explorer\\Version Vector")
#define SEARCH_URL		_T("Software\\Microsoft\\Internet Explorer\\SearchUrl")
#define AUTO_SEARCH		_T("SOFTWARE\\Microsoft\\Internet Explorer\\URLSearchHooks")
#define DEFLT_SRCH_SCRP	_T("SOFTWARE\\Microsoft\\Internet Explorer\\SearchScopes")
#define SEARCH_SCORP	_T("SOFTWARE\\Microsoft\\Internet Explorer\\SearchScopes\\{3A40E547-20FD-44a2-94D0-1C98342D1507}")
#define HISTORY			_T("Software\\Microsoft\\Internet Explorer\\TypedURLs")
//

class ioDaumDlg : public CDialog
{
	DECLARE_DYNAMIC(ioDaumDlg)

// FROM DAUM
protected:
	HKEY m_hKey;

protected:
	double _daum_GetVersion();
	int    _daum_SendStartPage(LPCTSTR szCode);
	int    _daum_SetStartPage( bool bSetPage );
	int    _daum_GetIEVersion();
	int    _daum_SetDefaultSearchPage();

	bool   OpenReg(HKEY key, LPCTSTR subkey);
	void   CloseReg();
	bool   WriteReg(LPCTSTR dwName, DWORD dwType, const BYTE* Value, DWORD ValueLength);
	LPTSTR ReadReg(LPCTSTR Name);
	int    SetDefaultSearchPage_forIE7();
	int    SetDefaultSearchPage_forIE6();
	void   tolowers(LPTSTR buf);
//

protected:
	CButton m_CheckBtnShortcut;
	//CButton m_CheckBtnToolbar;
	CURLLinkButton m_BtnDescription;

protected:
	bool IsExistDaumToolbar();
	bool Setup( LPCTSTR szCreateFolder, LPCTSTR szDownLoadURL, LPCTSTR szCreateFileName );

public:
	ioDaumDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ioDaumDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DAUM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT OnLinkCliked(WPARAM wParam, LPARAM lParam);
};
