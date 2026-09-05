// InstallDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_launcher.h"
#include "InstallDialog.h"


// CInstallDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInstallDialog, CDialog)

CInstallDialog::CInstallDialog(const CString folder, CWnd* pParent /*=NULL*/)
	: CDialog(CInstallDialog::IDD, pParent)
{
	m_folder = folder;
}

CInstallDialog::~CInstallDialog()
{
}

void CInstallDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SELECTDEMON, m_selectedDemon);
	DDX_Control(pDX, IDC_STATIC_SELECTSCRIPT, m_selectedScript);
	DDX_Control(pDX, IDC_EDIT_SERVICENAME, m_serviceName);
}


BEGIN_MESSAGE_MAP(CInstallDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SELECTDEMON, &CInstallDialog::OnBnClickedButtonSelectDemon)
	ON_BN_CLICKED(IDC_BUTTON_SELECTSCRIPT, &CInstallDialog::OnBnClickedButtonSelectScript)
	ON_BN_CLICKED(IDC_BUTTON_INSTALLCANCEL, &CInstallDialog::OnBnClickedButtonInstallCancel)
	ON_BN_CLICKED(IDC_BUTTON_INSTALLOK, &CInstallDialog::OnBnClickedButtonInstallOK)
END_MESSAGE_MAP()


// CInstallDialog 메시지 처리기입니다.


void CInstallDialog::OnBnClickedButtonSelectDemon()
{
	TCHAR name[512] = _T("exe Files (*.exe)|*.exe|All Files (*.*)|*.*||");
     
    CFileDialog fileInfo(TRUE, _T("exe"), _T("*.exe"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, name, NULL);
    fileInfo.m_ofn.nFilterIndex = 1;
	fileInfo.m_ofn.lpstrInitialDir = m_folder;
          
    if(fileInfo.DoModal() == IDOK)
	{
		m_demon	= fileInfo.GetPathName();
		m_selectedDemon.SetWindowText(fileInfo.GetFileName());
	}
}

void CInstallDialog::OnBnClickedButtonSelectScript()
{
	//TCHAR name[512] = _T("lua script Files (*.lua)|*.lua|All Files (*.*)|*.*||");
	TCHAR name[512] = _T("ini Files (*.ini)|*.ini|All Files (*.*)|*.*||");
     
    CFileDialog fileInfo(TRUE, _T("ini"), _T("*.ini"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, name, NULL);
    fileInfo.m_ofn.nFilterIndex = 1;
	fileInfo.m_ofn.lpstrInitialDir = m_folder;
          
    if(fileInfo.DoModal() == IDOK)
	{
		m_script = fileInfo.GetPathName();
		m_selectedScript.SetWindowText(fileInfo.GetFileName());
	}
}

void CInstallDialog::OnBnClickedButtonInstallOK()
{
	m_serviceName.GetWindowText(m_service);
	CDialog::EndDialog(IDOK);
}

void CInstallDialog::OnBnClickedButtonInstallCancel()
{
	CDialog::EndDialog(IDCANCEL);
}

