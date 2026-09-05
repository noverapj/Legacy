#pragma once
#include "resource.h"
#include "afxwin.h"

// CEditDlg 대화 상자입니다.

#define _KEYDOWN(k)	((GetAsyncKeyState(k) & 0x8000) ? TRUE : FALSE)
#define _KEYUP(k)	((GetAsyncKeyState(k) & 0x8000) ? FALSE : TRUE)

struct KeyWord
{
	CString StrKey;
	int		iIndex;
};

class CEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditDlg)
	enum	FUNC_KEY { FK_INSERT, FK_BACK, FK_SHIFT, FK_FIND_NEXT, FK_COPY, FK_CUT,FK_PASTE, FK_FIND , FK_ALL_SELECT ,FK_END };
	struct RollBack
	{
		int		iPos;
		CString strInput;
		CString strBackup;
	};
public:
	CEditDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEditDlg();
	CString*	GetEditString() { return &m_strTxtData; }
	void		SetLoadFilePath(const CString& strPath);
	ULONGLONG	GetSize() {return m_uFileSize;}
	int GetTest(CString str){return m_strTxtData.Find(str);}
// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDITDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


private:
	void	InsertText();
	void	SetChangeText(char cTxt, int iStart, int iEnd);
	void	Funckey(int ikey);
	CString	m_strPath;
	BOOL	m_bFuncKey[FK_END];
	char	m_cNowPushedKey;
	CString	m_strCopy;
	int		m_iStartSel, m_iEndSel;
	BOOL	m_bFirstSet;
	ULONGLONG	m_uFileSize;
	KeyWord	m_KeyWord;
public:
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString				m_strTxtData;
	CEdit				m_EditCtr;
	vector<RollBack>	vecRollstr;
	CRITICAL_SECTION	m_cs;

	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
