// InterfaceManager.cpp: implementation of the InterfaceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "InterfaceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

InterfaceManager *InterfaceManager::sg_Instance = NULL;

InterfaceManager::InterfaceManager()
{
	m_max_progress   = 0;
	m_pctrlProgress  = NULL;
	m_pctrlList		 = NULL;
	m_pBtnCreateInfo = NULL;
	m_pBtnUpload     = NULL;
	m_pBtnDelNoServer       = NULL;
	m_pBtnFullZip    = NULL;
	m_pBtnPatchFix	 = NULL;
	m_pBtnMakeFullzip		= NULL;
}

InterfaceManager::~InterfaceManager()
{

}

InterfaceManager *InterfaceManager::GetInstance()
{
	if(sg_Instance == NULL)
		sg_Instance = new InterfaceManager;
	return sg_Instance;		
}

void InterfaceManager::ReleaseInstance()
{
	if(sg_Instance)
		delete sg_Instance;
	sg_Instance = NULL;
}

void InterfaceManager::SetInterface( CProgressCtrl *pPC,CListBox *pLB,CButton *pBtnPatchInfo,CButton *pBtnUpload, CButton *pBtnShowLog, CComboBox *pCombo, CButton *pBtnDeleteList,CButton *pBtnUpdate, CButton *pBtnDelNoServer, CButton *pBtnFullZip, CButton *pBtnPatchFix, CButton* pBtnCheckSync, CButton* pBtnMakePath, CButton* pBtnSyncPath, CButton* pBtnMakeFull, CButton* pMakeDelFileList )
{
	m_pctrlProgress			= pPC;
	m_pctrlList				= pLB;
	m_pBtnCreateInfo		= pBtnPatchInfo;
	m_pBtnUpload			= pBtnUpload;
	m_pBtnShowLog			= pBtnShowLog;
	m_pComboBox             = pCombo;
	m_pBtnDeleteList        = pBtnDeleteList;
	m_pBtnUpdate            = pBtnUpdate;
	m_pBtnDelNoServer       = pBtnDelNoServer;
	m_pBtnFullZip           = pBtnFullZip;
	m_pBtnPatchFix			= pBtnPatchFix;
	m_pBtnCheckSyncFile		= pBtnCheckSync;
	m_pBtnMakePatch			= pBtnMakePath;
	m_pBtnSycnPatch			= pBtnSyncPath;
	m_pBtnMakeFullzip		= pBtnMakeFull;
	m_pBtnMakeDeleteFList	= pMakeDelFileList;
}


void InterfaceManager::SetUploadBtnEnable(BOOL bEnable)
{
	if(m_pBtnUpload)
		m_pBtnUpload->EnableWindow(bEnable);
}

void InterfaceManager::SetCreateInfoBtnEnable(BOOL bEnable)
{
	if(m_pBtnCreateInfo)
		m_pBtnCreateInfo->EnableWindow(bEnable);
}

void InterfaceManager::SetAnnounce(CString filename)
{
	if(m_pctrlList)
	{
		CString announce;
		announce.Format(" %s",filename);
		m_pctrlList->AddString(announce);
		m_pctrlList->SetSel(0,FALSE);		
	}
}

void InterfaceManager::InitAnnounce()
{
	if(m_pctrlList)
	{
		m_pctrlList->ResetContent();
		SetAnnounce("");
	}
}



void InterfaceManager::SetPatchGauge(int max,int cur)
{
	if(m_pctrlProgress)
	{
		//처음 시도면 사이즈 재설정
		if(m_max_progress != max)
			m_pctrlProgress->SetRange32(0,max);		
		m_max_progress = max;
		m_pctrlProgress->SetPos(cur);
	}
}

void InterfaceManager::SetAnnounceFirstLine( CString filename )
{
	if(m_pctrlList)
	{
		CString announce;
		announce.Format(" %s",filename);
		m_pctrlList->InsertString(0, announce);
		m_pctrlList->SetSel(0,FALSE);		
	}
}

void InterfaceManager::SetAnnounceFirstLineAfterDel( CString filename )
{
	if(m_pctrlList)
	{
		CString announce;
		announce.Format(" %s",filename);
		m_pctrlList->DeleteString(0);
		m_pctrlList->InsertString(0, announce);
		m_pctrlList->SetSel(0,FALSE);		
	}
}

void InterfaceManager::SetShowLogBtnEnable( BOOL bEnable )
{
	if(m_pBtnShowLog)
		m_pBtnShowLog->EnableWindow(bEnable);
}

void InterfaceManager::SetComboBoxEnable( BOOL bEnable )
{
	if(m_pComboBox)
		m_pComboBox->EnableWindow(bEnable);
}

void InterfaceManager::GetCurComboBoxText(OUT CString& rString )
{
	if(m_pComboBox)
	{
		int iIdx = m_pComboBox->GetCurSel();
		m_pComboBox->GetLBText(iIdx, rString);
	}
}

void InterfaceManager::SetDeleteListBtnEnable( BOOL bEnable )
{
	if(m_pBtnDeleteList)
		m_pBtnDeleteList->EnableWindow(bEnable);
}

void InterfaceManager::SetUpdateBtnEnable( BOOL bEnable )
{
	if(m_pBtnUpdate)
		m_pBtnUpdate->EnableWindow(bEnable);
}

void InterfaceManager::SetDelNoServerBtnEnable( BOOL bEnable )
{
	if( m_pBtnDelNoServer )
		m_pBtnDelNoServer->EnableWindow( bEnable );
}

void InterfaceManager::SetChangeFullZipBtnEnable( BOOL bEnable )
{
	if( m_pBtnFullZip )
	{
		m_pBtnFullZip->EnableWindow( bEnable );
		m_pBtnFullZip->ShowWindow(bEnable);
	}
}

void InterfaceManager::SetMakeFullzipEnable( BOOL bEnable )
{
	if(m_pBtnMakeFullzip)
	{
		m_pBtnMakeFullzip->EnableWindow(bEnable);
		m_pBtnMakeFullzip->ShowWindow(bEnable);
	}
}

void InterfaceManager::SetMakeDelFileEnalbe( BOOL bEnable )
{
	if(m_pBtnMakeDeleteFList)
		m_pBtnMakeDeleteFList->EnableWindow(bEnable);
}

void InterfaceManager::SetPatchInfoFixEnable( BOOL bEnable )
{
	if( m_pBtnPatchFix )
		m_pBtnPatchFix->EnableWindow( bEnable );
}

void InterfaceManager::SetCheckFileSyncEnable( BOOL bEnable )
{
	if( m_pBtnCheckSyncFile )
		m_pBtnCheckSyncFile->EnableWindow( bEnable );
}

void InterfaceManager::SetSyncPatchEnable( BOOL bEnable )
{
	if( m_pBtnSycnPatch )
	{
		m_pBtnSycnPatch->EnableWindow( bEnable );
		m_pBtnSycnPatch->ShowWindow(bEnable);
	}
}

void InterfaceManager::SetMakePatchEnable( BOOL bEnable )
{
	if( m_pBtnMakePatch )
	{
		m_pBtnMakePatch->EnableWindow( bEnable );
		m_pBtnMakePatch->ShowWindow(bEnable);
	}
}
