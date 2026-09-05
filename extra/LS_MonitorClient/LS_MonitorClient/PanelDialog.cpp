// PanelDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_MonitorClient.h"
#include "PanelDialog.h"
#include "WinMessage.h"
#include "Define.h"
#include "afxdialogex.h"


// CPanelDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPanelDialog, CDialogEx)

CPanelDialog::CPanelDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPanelDialog::IDD, pParent), m_maxValue(0), m_current(0), m_serverId(0), m_serverPort(0), m_blockState(0), m_serverState(0)
{

}

CPanelDialog::~CPanelDialog()
{
}

void CPanelDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TITLE_STATIC, m_title);
}


BEGIN_MESSAGE_MAP(CPanelDialog, CDialogEx)
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CPanelDialog 메시지 처리기입니다.
BOOL CPanelDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CreatePopup();

	CRect rc;
	GetDlgItem(IDC_STATIC_MBAR)->GetWindowRect(rc);
	ScreenToClient(rc);

	m_graph.Create(WS_VISIBLE | WS_CHILD, rc, this); 
	m_graph.SetRange( 0, 100 );
	m_graph.SetPlotColor(RGB(128, 128, 128), FALSE);
	m_graph.SetBackgroundColor(RGB(128, 128, 128), TRUE);
	m_graph.InitToolTip();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPanelDialog::Startup(int serverId, CString title, CString serverIP, int serverPort, int blockState, int maxUser)
{
	m_title.SetWindowText(title);
	m_graph.SetRange(0, maxUser);
	m_graph.DrawGraph(0);

	m_serverId		= serverId;
	m_serverName	= title;
	m_serverIP		= serverIP;
	m_serverPort	= serverPort;
	m_blockState	= blockState;

	CString tooltip;
	tooltip.Format(_T("%s(%s:%d) - %d/%d"), m_serverName, m_serverIP, m_serverPort, m_current, m_maxValue);
	m_graph.SetToolTip(tooltip);

	UpdatePopup(m_blockState);
}

void CPanelDialog::UpdateBegin()
{
	m_serverState = ESOCKET::LS_REQSERVERSTATE;
}

void CPanelDialog::UpdateEnd()
{
	switch(m_serverState)
	{
	case ESOCKET::LS_DISCONNECTED :
		break;
	
	case ESOCKET::LS_RESPSERVERSTATE :
		m_blockState = 0;
		m_graph.SetPlotColor(RGB(128, 255, 128), FALSE);
		m_graph.SetBackgroundColor(RGB(0, 0, 0), TRUE);
		break;

	case ESOCKET::LS_ZOMBIESERVERSTATE :
		m_blockState = 0;
		m_graph.SetPlotColor(RGB(255, 0, 0), FALSE);
		m_graph.SetBackgroundColor(RGB(0, 0, 0), TRUE);
		break;
	
	case ESOCKET::LS_BLOCKED :
		m_blockState = 1;
		m_graph.SetPlotColor(RGB(128, 128, 128), FALSE);
		m_graph.SetBackgroundColor(RGB(0, 0, 0), TRUE);
		break;
	}
}

void CPanelDialog::Update(int currentUser, int sendCount, int serverState)
{
	m_current		= currentUser;
	m_serverState	= serverState;
	if(m_maxValue < m_current)
	{
		m_maxValue = m_current;
	}

	CString tooltip;
	tooltip.Format(_T("%s(%s:%d) - %d/%d"), m_serverName, m_serverIP, m_serverPort, m_current, m_maxValue);
	m_graph.SetToolTip(tooltip);
	m_graph.DrawGraph(currentUser);
}

void CPanelDialog::CreatePopup()
{
}

void CPanelDialog::UpdatePopup(int blockState)
{
}

void CPanelDialog::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint pos;
	GetCursorPos(&pos);

	m_menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);

	CDialogEx::OnRButtonDown(nFlags, point);
}


BOOL CPanelDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return CDialogEx::OnCommand(wParam, lParam);
}


BOOL CPanelDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(RestrictMessage(pMsg))
		return FALSE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
