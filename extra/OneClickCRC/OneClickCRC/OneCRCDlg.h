#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"

#include <vector>
typedef std::vector<CString>* pVecString;

// OneCRCDlg 대화 상자입니다.
class CCRCManager;
class CFTPManager;
class OneCRCDlg : public CDialog
{
	DECLARE_DYNAMIC(OneCRCDlg)
public:
	OneCRCDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~OneCRCDlg();
	void	PrintWorkLog(CString* str);
// 대화 상자 데이터입니다.
	enum { IDD = IDD_ONECRCDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

//생성한 함수입니다
private:
	enum	OBJ_ID { LIST_BOX1 };
	void InitializationDlg(); //초기화
	char* UTF8toANSI(const char* pTxt);
	void ChangeFontString( TCHAR* pSour, char* pDest );
	CCRCManager*	m_pCRCMgr;
	CFTPManager*	m_pFTPMgr;
	pVecString		m_pvecUploadListPath;
	CString*		m_pstrDownPath;
	CString*		m_pstrIP;
	CString*		m_pstrID;
	CString*		m_pstrPW;
public:
	void SetConnectInfo(CString* strip, CString* strid, CString* strpw);
	void SetUploadListPath(pVecString vecupload)	{ m_pvecUploadListPath = vecupload; };
	void SetDownloadPath(CString* strPath)			{ m_pstrDownPath = strPath; };
	void SetCRCMgr(CCRCManager* pMgr)				{ m_pCRCMgr = pMgr; }
	void SetFTPMgr(CFTPManager* pMgr)				{ m_pFTPMgr = pMgr; }
	//CString m_strLog;
	afx_msg void OnBnClickedConnectBt();
	CListBox m_ListBox;
};
