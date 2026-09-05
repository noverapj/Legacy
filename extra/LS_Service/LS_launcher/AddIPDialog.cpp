// AddIPDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_launcher.h"
#include "AddIPDialog.h"
#include "WinMessage.h"

// CAddIPDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAddIPDialog, CDialog)

CAddIPDialog::CAddIPDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAddIPDialog::IDD, pParent)
	, m_tapName(_T(""))
	, m_port(0)
	
	, m_ip(_T(""))
{

}

CAddIPDialog::~CAddIPDialog()
{
}

void CAddIPDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TAP_NAME_EDIT, m_tapName);
	DDX_Text(pDX, IDC_PORT_EDIT, m_port);

	DDX_Text(pDX, IDC_ADDIP_EDIT, m_ip);
}


BEGIN_MESSAGE_MAP(CAddIPDialog, CDialog)
	ON_BN_CLICKED(IDC_ADD_IP_OK_BUTTON, &CAddIPDialog::OnBnClickedAddIpOkButton)
	ON_BN_CLICKED(IDC_ADD_IP_CANCEL_BUTTON, &CAddIPDialog::OnBnClickedAddIpCancelButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAddIPDialog 메시지 처리기입니다.

void CAddIPDialog::OnBnClickedAddIpOkButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	OnOK();
}

void CAddIPDialog::OnBnClickedAddIpCancelButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}


void CAddIPDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case ECOMMAND::WATCHDOG:
		{

		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}
