// InterfaceManager.h: interface for the InterfaceManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERFACEMANAGER_H__3E1BF4D0_1273_420D_A773_746F07EF1D9C__INCLUDED_)
#define AFX_INTERFACEMANAGER_H__3E1BF4D0_1273_420D_A773_746F07EF1D9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class InterfaceManager  
{
	static InterfaceManager *sg_Instance;
private:
	int             m_max_progress;
	CProgressCtrl	*m_pctrlProgress;
	CListBox	    *m_pctrlList;
	CButton         *m_pBtnCreateInfo;
	CButton         *m_pBtnUpload;
	CButton         *m_pBtnShowLog;
	CComboBox       *m_pComboBox;
	CButton         *m_pBtnDeleteList;
	CButton         *m_pBtnUpdate;
	CButton         *m_pBtnDelNoServer;
	CButton         *m_pBtnFullZip;
	CButton			*m_pBtnPatchFix;
	CButton			*m_pBtnCheckSyncFile;
	CButton			*m_pBtnMakePatch;
	CButton			*m_pBtnSycnPatch;
	CButton			*m_pBtnMakeFullzip;
	CButton			*m_pBtnMakeDeleteFList;
public:
	static InterfaceManager *GetInstance();
	static void ReleaseInstance();

public:
	void SetInterface( CProgressCtrl *pPC,CListBox *pLB,CButton *pBtnPatchInfo,CButton *pBtnUpload, CButton *pBtnShowLog, CComboBox *pCombo, CButton *pBtnDeleteList,CButton *pBtnUpdate, CButton *pBtnDelNoServer, CButton *pBtnFullZip, CButton *pBtnPatchFix, CButton* pBtnCheckSync, CButton* pBtnMakePath, CButton* pBtnSyncPath, CButton* pBtnMakeFull, CButton* pMakeDelFileList );

public:
	void SetUploadBtnEnable(BOOL bEnable);
	void SetCreateInfoBtnEnable(BOOL bEnable);
	void SetShowLogBtnEnable(BOOL bEnable);
	void SetComboBoxEnable(BOOL bEnable );
	void SetDeleteListBtnEnable( BOOL bEnable );
	void SetUpdateBtnEnable( BOOL bEnable );
	void SetDelNoServerBtnEnable( BOOL bEnable );
	void SetChangeFullZipBtnEnable( BOOL bEnable );
	void SetPatchInfoFixEnable( BOOL bEnable );
	void SetCheckFileSyncEnable( BOOL bEnable );
	void SetMakePatchEnable( BOOL bEnable );
	void SetSyncPatchEnable(BOOL bEnable );
	void SetMakeFullzipEnable( BOOL bEnable );
	void SetMakeDelFileEnalbe( BOOL bEnable );
public:
	void SetAnnounce(CString filename);
	void SetAnnounceFirstLine(CString filename);
	void SetAnnounceFirstLineAfterDel(CString filename);
	void InitAnnounce();

public:
	void SetPatchGauge(int max,int cur);

public:
	void GetCurComboBoxText(OUT CString& rString);
	
private:
	InterfaceManager();
	virtual ~InterfaceManager();
};

#endif // !defined(AFX_INTERFACEMANAGER_H__3E1BF4D0_1273_420D_A773_746F07EF1D9C__INCLUDED_)
