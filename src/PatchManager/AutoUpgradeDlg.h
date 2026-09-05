// AutoUpgradeDlg.h : header file
//

#if !defined(AFX_AUTOUPGRADEDLG_H__0FBFC81C_40E3_4498_B087_A9EF3536B687__INCLUDED_)
#define AFX_AUTOUPGRADEDLG_H__0FBFC81C_40E3_4498_B087_A9EF3536B687__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "PatchManager.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeDlg dialog

#define ADMIN_CHECK_VALUE 56851245

class CAutoUpgradeDlg : public CDialog
{
//------------------------------------------------------
protected:
	CButton	        m_btnUpload;
	CButton	        m_btnCreateInfo;
	CButton         m_btnShowLog;
	CProgressCtrl	m_ctrlProgress;
	CListBox     	m_ctrlList;
	CComboBox       m_ComboBox;
	CBrush          m_brh;
	CButton         m_btnDeleteList;
	CButton         m_btnUpdate;
	CButton         m_btnFullZip;
	CButton         m_btnNoServer;
	CButton			m_btnPatchFix;
	CButton			m_btnCheckSync;
	CButton			m_btnMakeFull;
	CButton			m_btnMakeDelFile;
protected:
	CPatchManager  *m_pPatchManager;
	char            m_szRootDir[MAX_PATH];	
	ioINILoaderAU   m_INILoader;
	DWORD           m_dwDisplaySec;
	CString         m_szDisplayTitle;
	COLORREF        m_bgColor;
	static WNDPROC  m_OldProc;
	bool            m_bAdmin;
protected:
	void Run(CPatchManager::RunState eRunState );

	void GetPath(OUT char *szPath, int size, const char*szTitle );
	bool LoadPatchFolderINI();
	void GetFolderName( OUT CString &rszRoot , const char *szINIKeyName, const char *szTitle);
	bool LoadPatchConnectINI();
	void LoadIniForZone();
	void LoadRGBAndName( int iCurSel );
	bool IsDirectory( const string & path );
	bool MakePatchToDirectory( const string & fullPath ); 
	static LRESULT CALLBACK ListProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

public:
	DWORD GetDisplaySec() const;
	void  SetDisplaySec(DWORD dwDisplaySec);
	void SetDisplayTitle( const CString &rszDisplayTitle);
	char *GetRootDir();
	void  SaveUploadedDate( const CString &rszDate );
	bool GetExportBtnEnableState() { return m_bExportBtn; };
	bool GetVersionEnableState()   { return m_bVersionUp; };
	bool GetZipfileState()		   { return m_bZipFile;	  };
	bool GetServerFileState()	   { return m_bServerFile; };
	int  GetPacDivieSize()		   { return m_iPacDivide_Size; };
//------------------------------------------------------
public:
	// Construction
	CAutoUpgradeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoUpgradeDlg)
	enum { IDD = IDD_AUTOUPGRADE_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoUpgradeDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CAutoUpgradeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnBnClickedUpload();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCreateInfo();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedShowLog();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedDeleteFileList();
	afx_msg void OnBnClickedBtnUpdate();
	afx_msg void OnBnClickedBtnFullZip();
	afx_msg void OnBnClickedNoServerDelBtn();
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();

	afx_msg void OnBnClickedSelectFolder();
	afx_msg void OnBnClickedSelectFile();
	std::vector<S_PATCHINFO>& GetPatchFileInfoList() { return m_vecPatchFileInfo; };
private:
	enum { CLIENT_FILE = 1, SERVER_FILE, TOOL_FILE };
	enum { SERVER_CLEAN = 0, GAMESVR_RUNNING, MAINSVR_RUNNING, DBAGENT_RUNNING, BILLINGSVR_RUNNING, LOGINSVR_RUNNING };
	bool CheckServerName();
	void LoadFullZipInfo();
	void LoadInPatchInfo();
	void SetPatchFileBtn(bool bState );
	void SelectFileList();
	void LoadRightFileName();
	bool IsRightfile(CString& strName);
	bool GetSelectFolerPath( OUT CString& strPath );
	void SearchDirectory( IN CString& strPath , IN bool bGetFilePath = false );
	bool FindPatchDirectory( IN CString& strPath );
	bool FindPatchDirectory(IN int iType, IN CString  strPatchPath, IN CString strFilePath, OUT CString& DestPath );
	bool FindDirectory( IN CString& strPath, IN CString strKey, OUT CString& strFindPath );
	bool CopyPatchFile( IN CString& strPath );
	bool FindDirectoryInFile( IN int iFileType, IN CString strPath ,IN CString strFileName, IN CString strFilePatch, OUT CString& strDestPath );
	bool FindPatchFilePath( IN CString strFileName, IN CString strFilePatch,  OUT CString& strDestPatch );
	bool IsZip( IN const CString& strFilePath );
	bool ExtractZip( const string zipFile, const string dstPath );
	int CheckServerFile();
	bool CheckRunningProcess( IN CString szProcessName );
	typedef std::vector<CString> VecString;
	VecString m_vecStrFileType;
	VecString m_vecRightFile;
	bool	  m_bWrongFile;
	bool	  m_bFindTargetFolder;
	VSPATCHINFO m_vecPatchFileInfo;
	CString		m_strClientPath;
	CString		m_strServerPath;
	CString		m_strDefaultPath;
	CString		m_strPatchInfoSavePath;
	CString		m_strVersionRoot;
	int			m_iPacDivide_Size;
	bool		m_bExportBtn;
	bool		m_bVersionUp;
	bool		m_bZipFile;
	bool		m_bServerFile;
//	int			m_iPatchType;
public:
	CButton m_btnSelFolder;
	CButton m_btnSelFile;
	CButton m_btnSyncPatch;
	CButton m_btnMakePatch;
	afx_msg void OnBnClickedBtnPPac();
	afx_msg void OnBnClickedBtnSPac();
	afx_msg void OnBnClickedMakeFullzipBtn();
	afx_msg void OnBnClickedButton7();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOUPGRADEDLG_H__0FBFC81C_40E3_4498_B087_A9EF3536B687__INCLUDED_)
