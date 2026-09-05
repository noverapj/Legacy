// BackupDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_launcher.h"
#include "BackupDialog.h"


extern BOOL SelectFolder(HWND hWnd, CString &folder);

// CBackupDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CBackupDialog, CDialog)

CBackupDialog::CBackupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBackupDialog::IDD, pParent)
{
}

CBackupDialog::~CBackupDialog()
{
}

void CBackupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SOURCE, m_sourcePath);
	DDX_Control(pDX, IDC_STATIC_TARGET, m_targetPath);
}


BEGIN_MESSAGE_MAP(CBackupDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE, &CBackupDialog::OnBnClickedButtonSource)
	ON_BN_CLICKED(IDC_BUTTON_TARGET, &CBackupDialog::OnBnClickedButtonTarget)
	ON_BN_CLICKED(IDC_BUTTON_BACKUPOK, &CBackupDialog::OnBnClickedButtonBackupOK)
	ON_BN_CLICKED(IDC_BUTTON_BACKUPCANCEL, &CBackupDialog::OnBnClickedButtonBackupCancel)
END_MESSAGE_MAP()


// CBackupDialog 메시지 처리기입니다.

void CBackupDialog::OnBnClickedButtonSource()
{
	// 소스폴더 선택
	m_source = _T("");
	if(SelectFolder(this->GetSafeHwnd(), m_source))
	{
		m_sourcePath.SetWindowText(m_source);
	}
	else
	{
		m_sourcePath.SetWindowText(_T(""));
	}
}

void CBackupDialog::OnBnClickedButtonTarget()
{
	// 타겟폴더 선택
	m_target = _T("");
	if(SelectFolder(this->GetSafeHwnd(), m_target))
	{
		m_targetPath.SetWindowText(m_target);
	}
	else
	{
		m_targetPath.SetWindowText(_T(""));
	}
}

BOOL CBackupDialog::GetBackupPath(CString& source, CString& target)
{
	if(m_source.IsEmpty()) return FALSE;
	if(m_target.IsEmpty()) return FALSE;

	source = m_source;
	target = m_target;
	return TRUE;
}

void CBackupDialog::OnBnClickedButtonBackupOK()
{
	CDialog::EndDialog(IDOK);
}

void CBackupDialog::OnBnClickedButtonBackupCancel()
{
	CDialog::EndDialog(IDCANCEL);
}
