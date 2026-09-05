// MonitorCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_launcher.h"
#include "MonitorCtrl.h"


// CMonitorCtrl

IMPLEMENT_DYNAMIC(CMonitorCtrl, CProgressCtrl)

CMonitorCtrl::CMonitorCtrl()
{

}

CMonitorCtrl::~CMonitorCtrl()
{
}


BEGIN_MESSAGE_MAP(CMonitorCtrl, CProgressCtrl)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CMonitorCtrl 메시지 처리기입니다.

void CMonitorCtrl::InitToolTip()
{
	if (m_toolTip.m_hWnd == NULL) 
	{ 
		m_toolTip.Create(this); 
		m_toolTip.Activate(FALSE); 

		m_toolTip.SetDelayTime(TTDT_AUTOPOP, 3000);
	} 
}

void CMonitorCtrl::SetToolTip(CString text)
{
	m_toolTip.DelTool(this, 0);
	m_toolTip.AddTool(this, text, NULL, 0);
}

BOOL CMonitorCtrl::PreTranslateMessage(MSG* pMsg)
{
	m_toolTip.RelayEvent(pMsg);
	return CProgressCtrl::PreTranslateMessage(pMsg); 
}

void CMonitorCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_toolTip.Activate(TRUE); 

	CProgressCtrl::OnMouseMove(nFlags, point);
}
