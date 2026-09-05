// LauncherTabCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_launcher.h"
#include "LauncherTabCtrl.h"
#include "WinMessage.h"
#include "LS_launcherDlg.h"

// CLauncherTabCtrl

IMPLEMENT_DYNAMIC(CLauncherTabCtrl, CTabCtrl)

CLauncherTabCtrl::CLauncherTabCtrl()
{
	m_tabIndex	= 0;
}

CLauncherTabCtrl::~CLauncherTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CLauncherTabCtrl, CTabCtrl)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()



// CLauncherTabCtrl 메시지 처리기입니다.


void CLauncherTabCtrl::Init()
{
	m_tabIndex	= 0;

	CServerDialog* dlg;
	int tapCount = 0;
	for(SERVER_DIALOGS::iterator it = m_dialogs.begin() ; it != m_dialogs.end() ; ++it)
	{
		dlg = it->second;
		delete dlg;

		this->DeleteItem(tapCount++);
	}
	m_dialogs.clear();
	
	for(SERVER_DIALOGS::iterator it = m_dialogs.begin() ; it != m_dialogs.end() ; ++it)
	{
		dlg = it->second;
		delete dlg;

		this->DeleteItem(tapCount++);
	}
	m_dialogs.clear();

	// 이작업을 해주어야 다시 로드(탭추가)를 할 수 있다.
	CLS_launcherDlg* dlg2 = static_cast<CLS_launcherDlg*>( AfxGetMainWnd() );
	dlg2->SetLoaded(FALSE);

	SetMinTabWidth(100);
}

void CLauncherTabCtrl::AddTab(CString& title, CString& IP, UINT port)
{
	this->InsertItem(GetItemCount(), title);
	
	// 다이얼로그 처리
	if(GetItemCount() == 1) // 최초 한번만 실행
	{
		CRect tabRect, itemRect;
		GetClientRect(&tabRect);
		GetItemRect(0, &itemRect);

		m_x			= itemRect.left;
		m_y			= itemRect.bottom+1;
		m_width		= tabRect.right - itemRect.left-1;
		m_height	= tabRect.bottom - m_y -1;
	}

	CServerDialog* dlg = new CServerDialog(this);
	dlg->Create(IDD_SERVERINFO_DIALOG, this);
	dlg->SetWindowPos(&wndTop, m_x, m_y, m_width, m_height, SWP_HIDEWINDOW);

	UINT index = GetItemCount() - 1;

	RECT rect;
	this->GetItemRect(index, &rect);
	dlg->Init(title, IP, port, rect);
	m_dialogs[index] = dlg;

	// 포커스 초기화 0으로..
	SetCurSel(index);
	SetFocus(index);
}

void CLauncherTabCtrl::DeleteTab(const UINT focus)
{
	CServerDialog* dlg = GetDialog(focus);
	if(!dlg) return;

	this->DeleteItem(focus);
	
	// 탭Item값에 맞추어서 리스트 재조정.
	TapRenewal(focus);
	
	// 서버 정보(m_serverInfos) 삭제
	DeleteServerInfo(dlg->GetTitle());
	
	delete dlg;
	dlg = NULL;
}

void CLauncherTabCtrl::DeleteServerInfo(CString name)
{
	CLS_launcherDlg* dlg = static_cast<CLS_launcherDlg*>( AfxGetMainWnd() );
	dlg->DeleteServerInfo(name);
}

void CLauncherTabCtrl::TapRenewal( UINT focus )
{
	SERVER_DIALOGS::iterator it = m_dialogs.find(focus);
	if(it != m_dialogs.end())
	{
		m_dialogs.erase(it);
	}

	// 탭Item값에 맞추어서 리스트 재조정.
	UINT count=0;
	UINT index = 0;
	RECT rect;
	CServerDialog* dlg = NULL;
	SERVER_DIALOGS dialogs;
	for(it = m_dialogs.begin() ; it != m_dialogs.end() ; ++it)
	{
		index = count++;
		dlg = dialogs[index] = it->second;
		if(dlg)
		{   // 탭 위치 재조정
			GetItemRect(index, &rect);
			dlg->SetTabRect(rect);
		}
	}
	m_dialogs.clear();
	m_dialogs = dialogs;

	// 포커스 초기화 0으로..
	SetCurSel(0);
	SetFocus(0);

	// 탭이 다 지워졌다면 초기화를 해준다.
	if(m_dialogs.empty())	Init();
}

void CLauncherTabCtrl::SetFocus(const UINT index)
{
	if(GetTabIndex() != index)
	{
		ShowTab(GetTabIndex(), FALSE);
		ShowTab(GetCurFocus(), TRUE);
	}
	else
	{
		ShowTab(GetTabIndex(), TRUE);
	}
}
void CLauncherTabCtrl::UserFocus(const UINT index)
{
	if(GetTabIndex() != index)
	{
		ShowTab(GetTabIndex(), FALSE);
		ShowTab(index, TRUE);
	}
	else
	{
		ShowTab(GetTabIndex(), TRUE);
	}

}

void CLauncherTabCtrl::ShowTab(const UINT index, const BOOL show)
{
	CServerDialog* dlg = GetDialog(index);
	if(!dlg) return;
	
	if(show)
	{
		dlg->ShowWindow(SW_SHOW);
		dlg->SetFocus();
		SetCurSel(index);

		m_tabIndex = index;
	
		TCHAR buffer[256] = {0};
		TCITEM item;
		item.mask		= TCIF_TEXT;
		item.pszText	= buffer;
		item.cchTextMax = sizeof(buffer);
		this->GetItem(m_tabIndex, &item);

		CLS_launcherDlg* parentDlg = (CLS_launcherDlg*)(this->GetParent());
		parentDlg->SetWindowText(buffer);
	}
	else
	{
		dlg->ShowWindow(SW_HIDE);
	}
}

CServerDialog* CLauncherTabCtrl::GetDialog(const int index)
{
	SERVER_DIALOGS::iterator it = m_dialogs.find(index);
	if(it != m_dialogs.end())
	{
		return it->second;
	}
	return NULL;
}

void CLauncherTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SetFocus(GetCurFocus());
	
	CTabCtrl::OnLButtonUp(nFlags, point);
}

void CLauncherTabCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UINT choiceTabindex = GetTabFocus(point.x, point.y);
	SetCurSel(choiceTabindex);
	SetFocus(choiceTabindex);

	CLS_launcherDlg* dlg = static_cast<CLS_launcherDlg*>( AfxGetMainWnd() );
	dlg->SetTabCurFocus(choiceTabindex);		

	CPoint pos;
	GetCursorPos(&pos);
	// 마우스 위치에 팝업메뉴를 출력한다.
	m_menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);
	
	CTabCtrl::OnRButtonUp(nFlags, point);
}

void CLauncherTabCtrl::CreatePopup()
{
	// 팝업메뉴
	m_menu.CreatePopupMenu();

	CString menu;

	menu = _T("Add IP");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_ADD_IP, (LPCTSTR)menu);

	menu = _T("Delete IP");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_DELETE_IP, (LPCTSTR)menu);

	//m_menu.AppendMenu(MF_SEPARATOR);

	//menu = _T("Spread");
	//m_menu.AppendMenu(MF_STRING, IDC_SERVICE_SPREAD, (LPCTSTR)menu);
}

BOOL CLauncherTabCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
	case IDC_SERVICE_ADD_IP :
		GetParent()->SendMessage(IDC_SERVICE_ADD_IP, index, 0);
		break;

	case IDC_SERVICE_DELETE_IP :
		GetParent()->SendMessage(IDC_SERVICE_DELETE_IP, index, 0);
		break;

	case IDC_SERVICE_SPREAD :
		GetParent()->SendMessage(IDC_SERVICE_SPREAD, index, 0);
		break;
	}

	return CTabCtrl::OnCommand(wParam, lParam);
}

UINT CLauncherTabCtrl::GetTabFocus(LONG x, LONG y)
{
	CServerDialog* dlg = NULL;

	for(SERVER_DIALOGS::iterator it = m_dialogs.begin() ; it != m_dialogs.end() ; ++it)
	{
		dlg = it->second;
		if(!dlg) continue;

		if( dlg->IsMouseOverTab(x, y) )
		{
			return it->first;
		}
	}
	return -1;
}

BOOL CLauncherTabCtrl::IsAllReady()
{
	if( m_dialogs.empty() )
		return FALSE;

	for(SERVER_DIALOGS::iterator it = m_dialogs.begin() ; it != m_dialogs.end() ; ++it)
	{
		CServerDialog* dlg = it->second;
		if(!dlg) continue;

		if( !dlg->IsReady() )
		{
			return FALSE;
		}
	}
	return TRUE;
}