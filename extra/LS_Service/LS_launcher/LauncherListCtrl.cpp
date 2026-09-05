// LauncherListCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_launcher.h"
#include "LauncherListCtrl.h"
#include "WinMessage.h"



// CLauncherListCtrl

IMPLEMENT_DYNAMIC(CLauncherListCtrl, CListCtrl)

CLauncherListCtrl::CLauncherListCtrl()
{

}

CLauncherListCtrl::~CLauncherListCtrl()
{
}


BEGIN_MESSAGE_MAP(CLauncherListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CLauncherListCtrl::OnNMRClick)
END_MESSAGE_MAP()



// CLauncherListCtrl 메시지 처리기입니다.



void CLauncherListCtrl::CreatePopup()
{
	// 팝업메뉴
	m_menu.CreatePopupMenu();

	CString menu;

	menu = _T("Play Service(A)");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_PLAY, (LPCTSTR)menu);

	menu = _T("Stop Service(A)");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_STOP, (LPCTSTR)menu);

	//m_menu.AppendMenu(MF_SEPARATOR);

	//menu = _T("Play Console");
	//m_menu.AppendMenu(MF_STRING, IDC_SERVICE_PLAY_CONSOLE, (LPCTSTR)menu);

	//menu = _T("Stop Console");
	//m_menu.AppendMenu(MF_STRING, IDC_SERVICE_STOP_CONSOLE, (LPCTSTR)menu);

	m_menu.AppendMenu(MF_SEPARATOR);

	menu = _T("Uninstall");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_UNINSTALL, (LPCTSTR)menu);
}

int CLauncherListCtrl::GetSelectedIndex()
{
	int index = -1;
	for(int i = 0; i < GetItemCount(); i++)
	{
		if(LVIS_SELECTED == GetItemState(i, LVIS_SELECTED))
		{
			index = i;
			break;
		}
	}
	return index;
}

void CLauncherListCtrl::Save()
{
	int index = -1;
	for(int i = 0; i < GetItemCount(); i++)
	{
		if(LVIS_SELECTED == GetItemState(i, LVIS_SELECTED))
		{
			m_currentIndex = i;
			break;
		}
	}

	m_topIndex = GetTopIndex();
}

void CLauncherListCtrl::Restore()
{
	if(0 != m_topIndex)
	{
		EnsureVisible(GetItemCount()-1, FALSE); 
		EnsureVisible(m_topIndex, FALSE);
	}

	if(-1 != m_currentIndex)
	{
		if(m_currentIndex < GetItemCount())
		{
			SetItemState(m_currentIndex, LVIS_SELECTED, LVIS_SELECTED );
		}
	}
}


void CLauncherListCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pos;
	GetCursorPos(&pos);
	 
	int index = GetSelectedIndex();
	if(index != -1)
	{
		// 마우스 위치에 팝업메뉴를 출력한다.
		m_menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);
	}

	*pResult = 0;
}

BOOL CLauncherListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int index = -1;
	for(int i = 0; i < GetItemCount(); i++)
	{
		if(LVIS_SELECTED == GetItemState(i, LVIS_SELECTED))
		{
			index = i;
			break;
		}
	}

	switch(wParam)
	{
	case IDC_SERVICE_MONITOR :
		GetParent()->SendMessage(IDC_SERVICE_MONITOR, index, 0);
		break;

	case IDC_SERVICE_PLAY :
		GetParent()->SendMessage(IDC_SERVICE_PLAY, index, 0);
		break;

	case IDC_SERVICE_STOP :
		GetParent()->SendMessage(IDC_SERVICE_STOP, index, 0);
		break;

	case IDC_SERVICE_PLAY_CONSOLE :
		GetParent()->SendMessage(IDC_SERVICE_PLAY_CONSOLE, index, 0);
		break;

	case IDC_SERVICE_STOP_CONSOLE :
		GetParent()->SendMessage(IDC_SERVICE_STOP_CONSOLE, index, 0);
		break;

	case IDC_SERVICE_UNINSTALL :
		GetParent()->SendMessage(IDC_SERVICE_UNINSTALL, index, 0);
		break;
	}

	return CListCtrl::OnCommand(wParam, lParam);
}
